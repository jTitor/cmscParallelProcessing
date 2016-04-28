#include "CUDAProcessor.h"
#include "../Math/IntVec2.h"
#include "../Profiling/Profiler.h"
#include <omp.h>
#include <new>
#include <cstdlib>
#include <cstdio>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

using namespace std;
using namespace Graphics;

__device__ static int wrap(signedSize_t x, signedSize_t mod)
{
	if (x >= 0)
	{
		return x % mod;
	}
	//Otherwise, take modulus of x and add that to mod to get our result.
	return mod + (x % mod);
}

__device__ static void followSeam(IntVec2* outVec, SeamTracebackDirection direction, bool transpose)
{
	IntVec2 seamUp = IntVec2(!transpose ? 0 : -1, !transpose ? -1 : 0);
	IntVec2 seamLeft = IntVec2(!transpose ? -1 : 0, !transpose ? 0 : -1);
	switch (direction)
	{
	case SEAM_TB_LDOWN:
		*outVec += -seamUp + seamLeft;
		break;
	case SEAM_TB_LEFT:
		*outVec += seamLeft;
		break;
	case SEAM_TB_LUP:
		*outVec += seamUp + seamLeft;
		break;
	default:
		assert(false && "CUDAProcessor::removeSeam(): Seam data is corrupt!");
		abort();
		break;
	}
}

__device__ void CUDAProcessor::calcScharrAtPixel(signedSize_t x, signedSize_t y)
{
	//Unclear - can we filter in place?

	//Whatever the case, get our current pixel.
	auto pixel = energy.WritablePixelAt(x, y);
	//Now get the neighboring pixels.
	//Remember to wrap around if the pixel would be out of bounds.
	const int kNumPixels = 8;
	//Pixels are in order: up-left, up, up-right, right, down-right, down, down-left, left.
	//The filters are in the same order.
	Vec3 neighbors[] = {
	*image.PixelAt(wrap(x - 1, image.Width()),
		wrap(y - 1, image.Height())),
	*image.PixelAt(x,
		wrap(y - 1, image.Height())),
	*image.PixelAt(wrap(x + 1, image.Width()),
		wrap(y - 1, image.Height())),
	*image.PixelAt(wrap(x + 1, image.Width()),
		y),
	*image.PixelAt(wrap(x + 1, image.Width()),
		wrap(y + 1,
		image.Height())),
	*image.PixelAt(x,
		wrap(y + 1, image.Height())),
	*image.PixelAt(wrap(x - 1, image.Width()),
		wrap(y + 1, image.Height())),
	*image.PixelAt(wrap(x - 1, image.Width()),
		y)
	};
	const EnergyT ScharrX[] = {3, 0, -3, -10, -3, 0, 3, 10};
	const EnergyT ScharrY[] = {3, 10, 3, 0, -3, -10, -3, 0};

	//Now our horizontal gradient is the x filter applied to the neighbors;
	Vec3 horizGradientSquared = Vec3::Zero();
	for (int i = 0; i < kNumPixels; ++i)
	{
		horizGradientSquared += neighbors[i] * ScharrX[i];
	}
	//vertical is the y filter to the neighbors.
	Vec3 vertGradientSquared = Vec3::Zero();
	for (int i = 0; i < kNumPixels; ++i)
	{
		vertGradientSquared += neighbors[i] * ScharrY[i];
	}

	//Once again, take square root to it.
	*pixel = sqrtf(horizGradientSquared.SqrMag() + vertGradientSquared.SqrMag());
}

__global__ void CUDAProcessor::calcAllEnergy()
{
	//If this is out of bounds, abort.
	size_t pixelIdx = (blockIdx.x * blockDim.x) + threadIdx.x;
	if (pixelIdx >= imageNumPixels) {
		return;
	}

	//Get the energy at this block's pixel.
	int y = pixelIdx / imageWidth;
	int x = pixelIdx % imageWidth;
	calcScharrAtPixel(x, y);
}

__device__ void CUDAProcessor::recalcSeamEnergy(size_t seamIdx, bool transpose)
{
	//TODO: parallelize this.

	//This is before the seam data is recalculated, so we still have the seam path.
	//The image has had the seam removed, so we have the right intensities.

	//Figure out constants given transpose mode:
	//For moving along seam.
	IntVec2 seamUp = IntVec2(!transpose ? 0 : -1, !transpose ? -1 : 0);
	IntVec2 seamLeft = IntVec2(!transpose ? -1 : 0, !transpose ? 0 : -1);

	//Turn the index into actual X/Y coordinates.
	IntVec2 seamPos = IntVec2(!transpose ? image.Width() - 1 : 0,
		!transpose ? 0 : image.Height() - 1);
	seamPos += (-seamUp)*seamIdx;

	const SeamTracebackElem* currSeamElem = seamTraceback.PixelAt(seamPos);
	//For each pixel P in the seam:
	while (currSeamElem->TracebackDirection != SEAM_TB_END)
	{
		//recalc energy(xS, yS) - this was the lower/right side of the seam.
		//recalc energy of neighboring pixel (xS, yS-1) - this was the upper/left side of the seam.
		IntVec2 neighbor = seamPos + seamUp;
		//Neighbor may be out of bounds, wrap it.
		//neighbor.X() = wrap(neighbor.X(), energy.Width());
		//neighbor.Y() = wrap(neighbor.Y(), energy.Height());

		calcScharrAtPixel(seamPos.X(), seamPos.Y());
		if (energy.IsInBounds(neighbor))
		{
			calcScharrAtPixel(neighbor.X(), neighbor.Y());
		}
		//Get the next pixel on the seam.
		followSeam(&seamPos, currSeamElem->TracebackDirection, transpose);

		if (!energy.IsInBounds(seamPos)) {
			break;
		}

		currSeamElem = seamTraceback.PixelAt(seamPos);
	}
}

__global__ void CUDAProcessor::calcSeamCosts(bool transpose)
{
	IntVec2 pixelPos = IntVec2::Zero();

	//Figure out constants given transpose mode:
	//For moving along seam.
	const IntVec2 seamUp = IntVec2(!transpose ? 0 : -1, !transpose ? -1 : 0);
	const IntVec2 seamRight = IntVec2(!transpose ? 1 : 0, !transpose ? 0 : 1);

	const size_t bottomEdge = !transpose ? seamTraceback.Height() : seamTraceback.Width();
	const size_t rightEdge = !transpose ? seamTraceback.Width() : seamTraceback.Height();

	size_t pixelIdx = (blockIdx.x * blockDim.x) + threadIdx.x;

	//Quit if this pixel is out of bounds.
	if (pixelIdx >= bottomEdge) {
		return;
	}

	//Solve our pixel!
	pixelPos = (-seamUp)*pixelIdx;
	auto pixel = seamTraceback.WritablePixelAt(pixelPos);
	pixel->SeamCost = *energy.PixelAt(pixelPos);
	pixel->TracebackDirection = SEAM_TB_END;
}

__global__ void CUDAProcessor::calcSeamCosts(bool transpose)
{
	IntVec2 pixelPos = IntVec2::Zero();

	//Figure out constants given transpose mode:
	//For moving along seam.
	const IntVec2 seamUp = IntVec2(!transpose ? 0 : -1, !transpose ? -1 : 0);
	const IntVec2 seamRight = IntVec2(!transpose ? 1 : 0, !transpose ? 0 : 1);

	const size_t bottomEdge = !transpose ? seamTraceback.Height() : seamTraceback.Width();
	const size_t rightEdge = !transpose ? seamTraceback.Width() : seamTraceback.Height();

	size_t pixelIdx = (blockIdx.x * blockDim.x) + threadIdx.x;

	//Quit if this pixel is out of bounds.
	if (pixelIdx >= bottomEdge) {
		return;
	}

	//Now solve the pixels ahead of them.
	//For each column (can't parallelize this):
	for (size_t i = 1; i < rightEdge; ++i)
	{
		//Solve our element of the row.
		pixelPos = seamRight*i - seamUp*pixelIdx;
		auto pixel = seamTraceback.WritablePixelAt(pixelPos);

		//The left seam cost will always be valid.
		IntVec2 leftPos = pixelPos - seamRight;
		IntVec2 upLeftPos = leftPos + seamUp;
		IntVec2 downLeftPos = leftPos - seamUp;
		EnergyT leftCost = seamTraceback.PixelAt(leftPos)->SeamCost;
		EnergyT upLeftCost = seamTraceback.IsInBounds(upLeftPos) ?
							seamTraceback.PixelAt(upLeftPos)->SeamCost : FLT_MAX;
		EnergyT downLeftCost = seamTraceback.IsInBounds(downLeftPos) ?
			seamTraceback.PixelAt(downLeftPos)->SeamCost : FLT_MAX;

		EnergyT minCost = fminf(fminf(leftCost, upLeftCost), downLeftCost);
			
		//Now set our pixel's cost.
		pixel->SeamCost = *energy.PixelAt(pixelPos) + minCost;
		//Figure out our pixel's traceback direction.
		if (minCost == leftCost)
		{
			pixel->TracebackDirection = SEAM_TB_LEFT;
		}
		else if (minCost == upLeftCost)
		{
			pixel->TracebackDirection = SEAM_TB_LUP;
		}
		else if (minCost == downLeftCost)
		{
			pixel->TracebackDirection = SEAM_TB_LDOWN;
		}
	}
}

__device__ size_t CUDAProcessor::findMinCostSeam(bool transpose)
{
	IntVec2 currPos = IntVec2(!transpose ? image.Width() - 1 : 0,
		!transpose ? 0 : image.Height() - 1);
	//Scan right side of image.
	IntVec2 incr = IntVec2(!transpose ? 0 : 1,
		!transpose ? 1 : 0);
	const size_t scanLen = !transpose ? image.Height() : image.Width();
	size_t minIdx = 0;
	EnergyT minSeamCost = FLT_MAX;

	//TODO: Is this parallelizable under the server's OpenMP version?
	//For each pixel at the edge:
	for (size_t i = 0; i < scanLen; ++i)
	{
		auto currPixel = seamTraceback.PixelAt(currPos + incr*i);
		//Is this cost lower than the last known cost?
		if (currPixel->SeamCost < minSeamCost)
		{
			//If so, update last observed minimum.
			minSeamCost = currPixel->SeamCost;
			minIdx = i;
		}
	}

	//Return the seam.
	return minIdx;
}

__device__ CUDAProcessor::SeamRemoveDirection CUDAProcessor::removeSeam(size_t seamIdx, bool transpose)
{
	SeamRemoveDirection removeDirection = REMOVE_DIRECTION_DOWN;
	//Figure out constants given transpose mode:
	//For getting seam start.
	//For moving along seam.
	IntVec2 seamUp = IntVec2(!transpose ? 0 : -1, !transpose ? -1 : 0);
	IntVec2 seamLeft = IntVec2(!transpose ? -1 : 0, !transpose ? 0 : -1);

	//Each pixel in the seam gets removed.
	//Turn the index into actual X/Y coordinates.
	IntVec2 seamPos = IntVec2(!transpose ? image.Width() - 1 : 0,
		!transpose ? 0 : image.Height() - 1);
	seamPos += (-seamUp)*seamIdx;

	const SeamTracebackElem* currSeamElem = seamTraceback.PixelAt(seamPos);
	//Which way should we remove?
	size_t midpoint = (transpose ? image.Height() : image.Width()) / 2;
	removeDirection = seamIdx < midpoint ? REMOVE_DIRECTION_UP : REMOVE_DIRECTION_DOWN;
	ptrdiff_t directionFactor = removeDirection == REMOVE_DIRECTION_UP ? 1 : -1;

	//TODO: parallelize this.
	//In order to do this, we need the position of the pixel on the seam
	//WITHOUT following the seam. This means storing an extra buffer and removing the traceback code.
	//May not be worth doing.

	//For each pixel P in the seam:
	while (currSeamElem->TracebackDirection != SEAM_TB_END)
	{
		//For each pixel P' below P: (or to the right, if this is a vertical seam)
		IntVec2 pixelToCopyPos = seamPos;
		IntVec2 nextPixelPos = pixelToCopyPos + seamUp*directionFactor;
		while (image.IsInBounds(pixelToCopyPos) && image.IsInBounds(nextPixelPos))
		{
			//P.value = P'.value
			image.SetPixelAt(pixelToCopyPos, *image.PixelAt(nextPixelPos));
			//Copy the energy too!
			energy.SetPixelAt(pixelToCopyPos, *energy.PixelAt(pixelToCopyPos));

			//Move to the next pixel.
			pixelToCopyPos = nextPixelPos;
			nextPixelPos = pixelToCopyPos + seamUp*directionFactor;
		}

		//Get the next pixel on the seam.
		followSeam(&seamPos, currSeamElem->TracebackDirection, transpose);

		//ugh: early out if we somehow went out of bounds.
		if (!image.IsInBounds(seamPos)) {
			break;
		}

		currSeamElem = seamTraceback.PixelAt(seamPos);
	}

	return removeDirection;
}

__global__ void CUDAProcessor::updateBounds(size_t oppositeRowsLeft, SeamRemoveDirection seamRemoveDirection) {
	IntVec2 removeVec = IntVec2(0, 0);
	if (removeMode == REMOVE_ROWS)
	{
		removeVec = IntVec2(0, 1);
	}
	else
	{
		removeVec = IntVec2(1, 0);
	}
	if (seamRemoveDirection == REMOVE_DIRECTION_DOWN)
	{
		image.SetEnd(image.End() - removeVec);
		energy.SetEnd(energy.End() - removeVec);
		seamTraceback.SetEnd(seamTraceback.End() - removeVec);
	}
	else
	{
		image.SetOrigin(image.Origin() + removeVec);
		energy.SetOrigin(energy.Origin() + removeVec);
		seamTraceback.SetOrigin(seamTraceback.Origin() + removeVec);
	}

	//Check the other count - do we need to transpose?
	RemoveMode oppositeMode = removeMode == REMOVE_ROWS ? REMOVE_COLS : REMOVE_ROWS;//(RemoveMode)((removeMode + 1) % REMOVE_MODE_COUNT);
	if (oppositeRowsLeft > 0)
	{
		//If so, transpose at this point.
		removeMode = oppositeMode;
	}
}

void CUDAProcessor::doProcessImage(size_t numRowsToRemove, size_t numColsToRemove, size_t numCores)
{
	//Note how many threads we'll be using.
	std::printf("CUDAProcessor::ProcessImage(): Using %d cores\n", numCores);

	//Calculate the initial energy gradient of the image.
	profiler.StartProfile(ProfileCode::PC_CALC_ALL_ENERGY);
	//X*Y operation; split evenly among SMs.
	size_t numThreads = imageNumPixels / numCores;
	assert(numThreads*numCores == imageNumPixels && "Cores don't evenly split image pixels!");
	calcAllEnergy<<<numCores, numThreads>>>(numThreads);
	profiler.EndProfile(ProfileCode::PC_CALC_ALL_ENERGY);

#if defined(_DEBUG)
	{
		//Write the energy to output.
		EnergyBuffer tempEnergy(imageWidth, imageHeight);
		energy.CopyToHostBuffer(tempEnergy);
		WriteEnergyBuffer(tempEnergy, "energyBuffer.bmp");
	}
#endif

	//How many rows to remove?
	//How many columns to remove?
	size_t rowsColsToRemove[2] = { numRowsToRemove, numColsToRemove };

	//Start by removing rows.
	RemoveMode removeMode = numRowsToRemove > 0 ? REMOVE_ROWS : REMOVE_COLS;

	std::printf("CUDAProcessor::ProcessImage(): Removing %d rows and %d columns\n", rowsColsToRemove[REMOVE_ROWS], rowsColsToRemove[REMOVE_COLS]);
	//While you have too many rows/columns:
	while (rowsColsToRemove[REMOVE_ROWS] > 0 || rowsColsToRemove[REMOVE_COLS] > 0)
	{
		bool transpose = removeMode != REMOVE_ROWS;

		//Find the cost of each seam in the image.
		profiler.StartProfile(ProfileCode::PC_CALC_SEAM_COSTS);
		//Y operation.
		size_t numThreads = !transpose ? imageHeight : imageWidth;
		numThreads /= numCores;
		resetSeamStart<<<numCores, numThreads>>>(transpose, numThreads);
		calcSeamCosts<<<numCores, numThreads>>>(transpose, numThreads);
		profiler.EndProfile(ProfileCode::PC_CALC_SEAM_COSTS);

		//Remove minimum cost seam.
		profiler.StartProfile(ProfileCode::PC_FIND_MIN_COST_SEAM);
		TODO;
		//Unclear if parallelizable, but is a Y operation.
		size_t targetSeamIdx = findMinCostSeam(transpose);
		profiler.EndProfile(ProfileCode::PC_FIND_MIN_COST_SEAM);
		profiler.StartProfile(ProfileCode::PC_REMOVE_SEAM);
		//Another part that may not be parallelizable. X operation.
		TODO;
		auto seamRemoveDirection = removeSeam(targetSeamIdx, transpose);
		profiler.EndProfile(ProfileCode::PC_FIND_MIN_COST_SEAM);

		//Now the image has been modified; recalculate the energy near the removed seam.
		profiler.StartProfile(ProfileCode::PC_RECALC_SEAM_ENERGY);
		//Hasn't been parallelized. Unknown dimensions.
		recalcSeamEnergy(targetSeamIdx, transpose);
		profiler.EndProfile(ProfileCode::PC_RECALC_SEAM_ENERGY);
		//Seam traceback doesn't have to be updated since
		//calcSeamCosts() must evaluate the entire image.

		//Update buffer dimensions.
		profiler.StartProfile(ProfileCode::PC_BOUNDS_ADJUST);
		rowsColsToRemove[removeMode] -= 1;
		updateBounds<<<1, 1>>>(rowsColsToRemove[oppositeMode], seamRemoveDirection);
		profiler.EndProfile(ProfileCode::PC_BOUNDS_ADJUST);
	}
}

CUDAProcessor::CUDAProcessor(LABColorBuffer& pImage, Profiler& pProfiler) : image(pImage.Width(), pImage.Height()), energy(pImage.Width(), pImage.Height()), seamTraceback(pImage.Width(), pImage.Height()), profiler(pProfiler)
{
	imageWidth = pImage.Width();
	imageHeight = pImage.Height();
	imageNumPixels = imageWidth*imageHeight;
	//Copy source image to device.
	image.CopyFromHostBuffer(pImage);
	removeMode = REMOVE_ROWS;
}

CUDAProcessor::~CUDAProcessor()
{
}

void CUDAProcessor::ProcessImage(size_t numRowsToRemove, size_t numColsToRemove)
{
	//Do the actual work.
	//Should probably lookup how many blocks
	//the device has available first.
	doProcessImage(numRowsToRemove, numColsToRemove, numCores);
}