#include "Processor.h"
#include "../Math/IntVec2.h"
#include "../Profiling/Profiler.h"

using namespace Graphics;

int wrap(signedSize_t x, signedSize_t mod)
{
	if (x >= 0)
	{
		return x % mod;
	}
	//Otherwise, take modulus of x and add that to mod to get our result.
	return mod + (x % mod);
}

void followSeam(IntVec2* outVec, SeamTracebackDirection direction, bool transpose)
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
		assert(false && "Processor::removeSeam(): Seam data is corrupt!");
		abort();
		break;
	}
}

void Processor::calcScharrAtPixel(signedSize_t x, signedSize_t y)
{
	//Unclear - can we filter in place?

	//Whatever the case, get our current pixel.
	auto pixel = energy->WritablePixelAt(x, y);
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

void Processor::calcAllEnergy()
{
	//Get energy on all pixels :
	for (size_t x = 0; x < energy->Width(); ++x)
	{
		for (size_t y = 0; y < energy->Height(); ++y)
		{
			//Calculate horizontal and vertical gradient.
			// (Optional)smooth the gradients.
			// Run energy function given gradients.
			//calcEnergyAtPixel(x, y);
			calcScharrAtPixel(x, y);
		}
	}
}

void Processor::recalcSeamEnergy(size_t seamIdx, bool transpose)
{
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

	const SeamTracebackElem* currSeamElem = seamTraceback->PixelAt(seamPos);
	//For each pixel P in the seam:
	while (currSeamElem->TracebackDirection != SEAM_TB_END)
	{
		//recalc energy(xS, yS) - this was the lower/right side of the seam.
		//recalc energy of neighboring pixel (xS, yS-1) - this was the upper/left side of the seam.
		IntVec2 neighbor = seamPos + seamUp;
		//Neighbor may be out of bounds, wrap it.
		//neighbor.X() = wrap(neighbor.X(), energy->Width());
		//neighbor.Y() = wrap(neighbor.Y(), energy->Height());

		calcScharrAtPixel(seamPos.X(), seamPos.Y());
		if (energy->IsInBounds(neighbor))
		{
			calcScharrAtPixel(neighbor.X(), neighbor.Y());
		}
		//Get the next pixel on the seam.
		followSeam(&seamPos, currSeamElem->TracebackDirection, transpose);
		currSeamElem = seamTraceback->PixelAt(seamPos);
	}
}

void Processor::calcSeamCosts(bool transpose)
{
	IntVec2 pixelPos = IntVec2::Zero();

	//Figure out constants given transpose mode:
	//For moving along seam.
	const IntVec2 seamUp = IntVec2(!transpose ? 0 : -1, !transpose ? -1 : 0);
	const IntVec2 seamRight = IntVec2(!transpose ? 1 : 0, !transpose ? 0 : 1);

	const size_t bottomEdge = !transpose ? seamTraceback->Height() : seamTraceback->Width();
	const size_t rightEdge = !transpose ? seamTraceback->Width() : seamTraceback->Height();

	//Solve pixels at left edge first.
	//They'll just have energy.
	for (size_t i = 0; i < bottomEdge; ++i)
	{
		pixelPos = (-seamUp)*i;
		auto pixel = seamTraceback->WritablePixelAt(pixelPos);
		pixel->SeamCost = *energy->PixelAt(pixelPos);
		pixel->TracebackDirection = SEAM_TB_END;
	}

	//Now solve the pixels ahead of them.
	for (size_t i = 1; i < rightEdge; ++i)
	{
		for (size_t j = 0; j < bottomEdge; ++j)
		{
			pixelPos = seamRight*i - seamUp*j;
			auto pixel = seamTraceback->WritablePixelAt(pixelPos);

			//The left seam cost will always be valid.
			IntVec2 leftPos = pixelPos - seamRight;
			IntVec2 upLeftPos = leftPos + seamUp;
			IntVec2 downLeftPos = leftPos - seamUp;
			EnergyT leftCost = seamTraceback->PixelAt(leftPos)->SeamCost;
			EnergyT upLeftCost = seamTraceback->IsInBounds(upLeftPos) ?
								seamTraceback->PixelAt(upLeftPos)->SeamCost : FLT_MAX;
			EnergyT downLeftCost = seamTraceback->IsInBounds(downLeftPos) ?
				seamTraceback->PixelAt(downLeftPos)->SeamCost : FLT_MAX;

			EnergyT minCost = fminf(fminf(leftCost, upLeftCost), downLeftCost);
			
			//Now set our pixel's cost.
			pixel->SeamCost = *energy->PixelAt(pixelPos) + minCost;
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
}

size_t Processor::findMinCostSeam(bool transpose)
{
	IntVec2 currPos = IntVec2(!transpose ? image.Width() - 1 : 0,
		!transpose ? 0 : image.Height() - 1);
	//Scan right side of image.
	IntVec2 incr = IntVec2(!transpose ? 0 : 1,
		!transpose ? 1 : 0);
	const size_t scanLen = !transpose ? image.Height() : image.Width();
	size_t minIdx = 0;
	EnergyT minSeamCost = FLT_MAX;

	//For each pixel at the edge:
	for (size_t i = 0; i < scanLen; ++i)
	{
		auto currPixel = seamTraceback->PixelAt(currPos + incr*i);
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

Processor::SeamRemoveDirection Processor::removeSeam(size_t seamIdx, bool transpose)
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

	const SeamTracebackElem* currSeamElem = seamTraceback->PixelAt(seamPos);
	//Which way should we remove?
	size_t midpoint = (transpose ? image.Height() : image.Width()) / 2;
	removeDirection = seamIdx < midpoint ? REMOVE_DIRECTION_UP : REMOVE_DIRECTION_DOWN;
	ptrdiff_t directionFactor = removeDirection == REMOVE_DIRECTION_UP ? 1 : -1;

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
			energy->SetPixelAt(pixelToCopyPos, *energy->PixelAt(pixelToCopyPos));

			//Move to the next pixel.
			pixelToCopyPos = nextPixelPos;
			nextPixelPos = pixelToCopyPos + seamUp*directionFactor;
		}

		//Get the next pixel on the seam.
		followSeam(&seamPos, currSeamElem->TracebackDirection, transpose);
		currSeamElem = seamTraceback->PixelAt(seamPos);
	}

	return removeDirection;
}

void Processor::highlightSeam(LABColorBuffer& buffer, size_t seamIdx, bool transpose)
{
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

	const SeamTracebackElem* currSeamElem = seamTraceback->PixelAt(seamPos);
	bool dropDrawn = false;
	int i = 0;
	//For each pixel P in the seam:
	while (currSeamElem->TracebackDirection != SEAM_TB_END)
	{

		//Highlight the seam pixel in the highlight buffer.
		buffer.SetPixelAt(seamPos, Vec3(50, 100, 100));
		//Draw all pixels below the seam.
		IntVec2 pixelToCopyPos = seamPos;
		IntVec2 nextPixelPos = pixelToCopyPos - seamUp;
		while (image.IsInBounds(pixelToCopyPos) && image.IsInBounds(nextPixelPos))
		{
			//P.value = P'.value
			*buffer.WritablePixelAt(pixelToCopyPos) = Vec3(50 + 50*((float)i / (float)image.Width()), -100, 100);
				
			//Move to the next pixel.
			pixelToCopyPos -= seamUp;
			nextPixelPos -= seamUp;
		}

		//Get the next pixel on the seam.
		followSeam(&seamPos, currSeamElem->TracebackDirection, transpose);
		currSeamElem = seamTraceback->PixelAt(seamPos);
		++i;
	}
}

Processor::Processor(LABColorBuffer& pImage, Profiler& pProfiler) : image(pImage), profiler(pProfiler)
{
	//Derive other buffers from the given image buffer.
	energy = new EnergyBuffer(pImage.Width(), pImage.Height());
	seamTraceback = new SeamTracebackBuffer(pImage.Width(), pImage.Height());
	removeMode = REMOVE_ROWS;
}

Processor::~Processor()
{
	delete seamTraceback;
	delete energy;
}

//Highlights a row and column seam.
void Processor::TestProcessImage()
{
	printf("Processor::TestProcessImage(): Testing seam finding\n");
	WriteImageBuffer(image, "unmodified.bmp");
	calcAllEnergy();
	WriteEnergyBuffer(*energy, "energyBufferStart.bmp");
	//Highlight a row, then highlight a column.
	size_t rowsColsToRemove[2] = { 1, 1 };
	LABColorBuffer seamBuffer = LABColorBuffer(image.Width(), image.Height());

	RemoveMode removeMode = rowsColsToRemove[REMOVE_ROWS] > 0 ? REMOVE_ROWS : REMOVE_COLS;

	//While you have too many rows/columns:
	while (rowsColsToRemove[REMOVE_ROWS] > 0 || rowsColsToRemove[REMOVE_COLS] > 0)
	{
		bool transpose = removeMode != REMOVE_ROWS;

		calcSeamCosts(transpose);
		//Write out the calculated seams.
		WriteTracebackBuffer(*seamTraceback, "seamTraceback.bmp");

		//Remove minimum cost seam.
		size_t targetSeamIdx = findMinCostSeam(transpose);
		highlightSeam(seamBuffer, targetSeamIdx, transpose);
		removeSeam(targetSeamIdx, transpose);

		//Now the image has been modified; recalculate the energy.
		recalcSeamEnergy(targetSeamIdx, transpose);
		//Seam traceback doesn't have to be updated since
		//calcSeamCosts() must evaluate the entire image.

		//Update buffer dimensions.
		rowsColsToRemove[removeMode] -= 1;
		if (removeMode == REMOVE_ROWS)
		{
#if defined(_DEBUG)
			printf("Processor::ProcessImage(): Removed a row, %d rows and %d columns remain\n", rowsColsToRemove[REMOVE_ROWS], rowsColsToRemove[REMOVE_COLS]);
#endif
			image.SetHeight(image.Height() - 1);
			energy->SetHeight(energy->Height() - 1);
			seamTraceback->SetHeight(seamTraceback->Height() - 1);
			seamBuffer.SetHeight(seamBuffer.Height() - 1);
		}
		else
		{
#if defined(_DEBUG)
			printf("Processor::ProcessImage(): Removed a column, %d rows and %d columns remain\n", rowsColsToRemove[REMOVE_ROWS], rowsColsToRemove[REMOVE_COLS]);
#endif
			image.SetWidth(image.Width() - 1);
			energy->SetWidth(energy->Width() - 1);
			seamTraceback->SetWidth(seamTraceback->Width() - 1);
			seamBuffer.SetWidth(seamBuffer.Width() - 1);
		}

		//Check the other count - do we need to transpose?
		RemoveMode oppositeMode = (RemoveMode)((removeMode + 1) % REMOVE_MODE_COUNT);
		if (rowsColsToRemove[oppositeMode] > 0)
		{
			//If so, transpose at this point.
			removeMode = oppositeMode;
		}
	}
	//Write out the seam findings.
	WriteImageBuffer(seamBuffer, "markedSeams.bmp");

	//Print out the image.
	WriteImageBuffer(image, "imageTwoSeamsRemoved.bmp");
}

void Processor::ProcessImage(size_t numRowsToRemove, size_t numColsToRemove)
{
	//Calculate the initial energy gradient of the image.
	profiler.StartProfile(ProfileCode::PC_CALC_ALL_ENERGY);
	calcAllEnergy();
	profiler.EndProfile(ProfileCode::PC_CALC_ALL_ENERGY);

#if defined(_DEBUG)
	//Write the energy to output.
	WriteEnergyBuffer(*energy, "energyBuffer.bmp");
#endif

	//How many rows to remove?
	//How many columns to remove?
	size_t rowsColsToRemove[2] = {numRowsToRemove, numColsToRemove};

	//Start by removing rows.
	RemoveMode removeMode = numRowsToRemove > 0 ? REMOVE_ROWS : REMOVE_COLS;

	printf("Processor::ProcessImage(): Removing %d rows and %d columns\n", rowsColsToRemove[REMOVE_ROWS], rowsColsToRemove[REMOVE_COLS]);
	//While you have too many rows/columns:
	while (rowsColsToRemove[REMOVE_ROWS] > 0 || rowsColsToRemove[REMOVE_COLS] > 0)
	{
		bool transpose = removeMode != REMOVE_ROWS;

		//Find the cost of each seam in the image.
		profiler.StartProfile(ProfileCode::PC_CALC_SEAM_COSTS);
		calcSeamCosts(transpose);
		profiler.EndProfile(ProfileCode::PC_CALC_SEAM_COSTS);
		
		//Remove minimum cost seam.
		profiler.StartProfile(ProfileCode::PC_FIND_MIN_COST_SEAM);
		size_t targetSeamIdx = findMinCostSeam(transpose);
		profiler.EndProfile(ProfileCode::PC_FIND_MIN_COST_SEAM);
		profiler.StartProfile(ProfileCode::PC_REMOVE_SEAM);
		auto seamRemoveDirection = removeSeam(targetSeamIdx, transpose);
		profiler.EndProfile(ProfileCode::PC_FIND_MIN_COST_SEAM);

		//Now the image has been modified; recalculate the energy near the removed seam.
		profiler.StartProfile(ProfileCode::PC_RECALC_SEAM_ENERGY);
		recalcSeamEnergy(targetSeamIdx, transpose);
		profiler.EndProfile(ProfileCode::PC_RECALC_SEAM_ENERGY);
		//Seam traceback doesn't have to be updated since
		//calcSeamCosts() must evaluate the entire image.

		//Update buffer dimensions.
		profiler.StartProfile(ProfileCode::PC_BOUNDS_ADJUST);
		rowsColsToRemove[removeMode] -= 1;
		IntVec2 removeVec = IntVec2(0, 0);
		if (removeMode == REMOVE_ROWS)
		{
#if defined(_DEBUG)
			//printf("Processor::ProcessImage(): Removed a row, %d rows and %d columns remain\n", rowsColsToRemove[REMOVE_ROWS], rowsColsToRemove[REMOVE_COLS]);
#endif
			removeVec = IntVec2(0, 1);
		}
		else
		{
#if defined(_DEBUG)
			//printf("Processor::ProcessImage(): Removed a column, %d rows and %d columns remain\n", rowsColsToRemove[REMOVE_ROWS], rowsColsToRemove[REMOVE_COLS]);
#endif
			removeVec = IntVec2(1, 0);
		}
		if (seamRemoveDirection == REMOVE_DIRECTION_DOWN)
		{
			image.SetEnd(image.End() - removeVec);
			energy->SetEnd(energy->End() - removeVec);
			seamTraceback->SetEnd(seamTraceback->End() - removeVec);
		}
		else
		{
			image.SetOrigin(image.Origin() + removeVec);
			energy->SetOrigin(energy->Origin() + removeVec);
			seamTraceback->SetOrigin(seamTraceback->Origin() + removeVec);
		}

		//Check the other count - do we need to transpose?
		RemoveMode oppositeMode = removeMode == REMOVE_ROWS ? REMOVE_COLS : REMOVE_ROWS;//(RemoveMode)((removeMode + 1) % REMOVE_MODE_COUNT);
		if (rowsColsToRemove[oppositeMode] > 0)
		{
			//If so, transpose at this point.
			removeMode = oppositeMode;
		}
		profiler.EndProfile(ProfileCode::PC_BOUNDS_ADJUST);
	}
}