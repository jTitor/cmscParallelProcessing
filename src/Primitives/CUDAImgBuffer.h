#pragma once
#include "../StandardIncludes.h"
#include "../Math/IntVec2.h"
#include "ImgBuffer.h"
#include <cstring>
#include <cuda_runtime.h>

using namespace std;

namespace Graphics
{
	template<typename ElementT>
	class CUDAImgBuffer
	{
	private:
		ElementT* pixels;
		const IntVec2 maxDimensions;
		IntVec2 origin;
		IntVec2 end;
		CUDA_DEVICE_ONLY_MEMBER inline IntVec2 getDimensions() const
		{
			return end - origin;
		}
		CUDA_DEVICE_ONLY_MEMBER inline IntVec2 getAnchoredMaxDimensions() const
		{
			return maxDimensions - origin;
		}
	public:
		CUDA_DEVICE_ONLY_MEMBER ElementT* Buffer() {
			return pixels;
		}

		/**
		Gets the width of the image buffer.
		*/
		CUDA_DEVICE_ONLY_MEMBER size_t Width() const
		{
			return getDimensions().GetX();
		}

		CUDA_DEVICE_ONLY_MEMBER bool SetWidth(size_t newWidth)
		{
			//Abort if this is larger than the maximum end.
			if (newWidth < 1 || newWidth > getAnchoredMaxDimensions().GetX())
			{
				return false;
			}

			end.X() = origin.GetX() + newWidth;
			return true;
		}

		/**
		Gets the height of the image buffer.
		*/
		CUDA_DEVICE_ONLY_MEMBER size_t Height() const
		{
			return getDimensions().GetY();
		}

		CUDA_DEVICE_ONLY_MEMBER bool SetHeight(size_t newHeight)
		{
			//Abort if this is larger than the maximum end.
			if (newHeight < 1 || newHeight > getAnchoredMaxDimensions().GetY())
			{
				return false;
			}
			end.Y() = origin.GetY() + newHeight;
			return true;
		}

		CUDA_DEVICE_ONLY_MEMBER const IntVec2& Origin() const
		{
			return origin;
		}

		CUDA_DEVICE_ONLY_MEMBER bool SetOrigin(const IntVec2& newOrigin)
		{
			if (newOrigin.GetX() < 0 || newOrigin.GetX() >= end.GetX() ||
				newOrigin.GetY() < 0 || newOrigin.GetY() >= end.GetY())
			{
				return false;
			}
			origin = newOrigin;
			return true;
		}

		CUDA_DEVICE_ONLY_MEMBER const IntVec2& End() const
		{
			return end;
		}

		CUDA_DEVICE_ONLY_MEMBER bool SetEnd(const IntVec2& newEnd)
		{
			if (newEnd.GetX() <= origin.GetX() || newEnd.GetX() > maxDimensions.GetX() ||
				newEnd.GetY() <= origin.GetY() || newEnd.GetY() > maxDimensions.GetY())
			{
				return false;
			}
			end = newEnd;
			return true;
		}

		CUDA_DEVICE_ONLY_MEMBER bool SetDimensions(const IntVec2& newDimensions)
		{
			if (newDimensions.GetX() < 0 || newDimensions.GetX() > getAnchoredMaxDimensions().GetX() ||
				newDimensions.GetY() < 0 || newDimensions.GetY() > getAnchoredMaxDimensions().GetY())
			{
				return false;
			}
			end = origin + newDimensions;
			true;
		}

		CUDA_DEVICE_ONLY_MEMBER bool IsInBounds(ptrdiff_t x, ptrdiff_t y) const
		{
			
			return (x >= 0 && x < getDimensions().GetX()) &&
				(y >= 0 && y < getDimensions().GetY());
		}
		CUDA_DEVICE_ONLY_MEMBER bool IsInBounds(const IntVec2& pos) const
		{
			return IsInBounds(pos.GetX(), pos.GetY());
		}

		/**
		Gets a constant reference to the pixel at (x, y).
		*/
		CUDA_DEVICE_ONLY_MEMBER const ElementT* PixelAt(ptrdiff_t x, ptrdiff_t y) const
		{
			if (!IsInBounds(x,y))
			{
				//Crash angrily.
				assert(false && "ImgBuffer::PixelAt(): Coordinates out of bounds!");
				//Return 0th element since we can't return a stack value as reference.
				return NULL;
			}
			x += origin.GetX();
			y += origin.GetY();
			return &pixels[IMGBUFFER_XY_TO_IDX(x, y, maxDimensions.GetX())];
		}
		CUDA_DEVICE_ONLY_MEMBER const ElementT* PixelAt(const IntVec2& pos) const
		{
			return PixelAt(pos.GetX(), pos.GetY());
		}

		/**
		Gets a writeable reference to the pixel at (x, y).
		*/
		CUDA_DEVICE_ONLY_MEMBER ElementT* WritablePixelAt(ptrdiff_t x, ptrdiff_t y)
		{
			return const_cast<ElementT*>(PixelAt(x, y));
		}
		CUDA_DEVICE_ONLY_MEMBER ElementT* WritablePixelAt(const IntVec2& pos)
		{
			return WritablePixelAt(pos.GetX(), pos.GetY());
		}

		CUDA_DEVICE_ONLY_MEMBER void SetPixelAt(ptrdiff_t x, ptrdiff_t y, const ElementT& value)
		{
			if (!IsInBounds(x, y))
			{
				//Crash angrily.
				assert(false && "ImgBuffer::SetPixelAt(): Coordinates out of bounds!");
				//Return 0th element since we can't return a stack value as reference.
				return;
			}
			x += origin.GetX();
			y += origin.GetY();
			pixels[IMGBUFFER_XY_TO_IDX(x, y, maxDimensions.GetX())] = value;
		}
		CUDA_DEVICE_ONLY_MEMBER void SetPixelAt(const IntVec2& pos, const ElementT& value)
		{
			SetPixelAt(pos.GetX(), pos.GetY(), value);
		}

		/**
		Sets all pixels in the buffer to the given value.
		*/
		CUDA_DEVICE_ONLY_MEMBER void Clear(const ElementT& value)
		{
			//For each pixel:
			for (size_t i = 0; i < IMGBUFFER_NUM_PIXELS(maxDimensions.GetX(), maxDimensions.GetY()); ++i)
			{
				//Copy over the given value.
				pixels[i] = value;
			}
		}

		/**
		Copies the given pixels to this buffer.
		The source buffer MUST have (width)*(height) pixels or this will crash.
		*/
		CUDA_DEVICE_ONLY_MEMBER void CopyToBuffer(const ElementT* srcPixels)
		{
			//For each pixel:
			for (size_t i = 0; i < IMGBUFFER_NUM_PIXELS(maxDimensions.GetX(), maxDimensions.GetY()); ++i)
			{
				//Copy over the given value.
				pixels[i] = srcPixels[i];
			}
		}

		void CopyFromHostBuffer(ImgBuffer<ElementT> hostBuffer) {
			size_t numPixels = IMGBUFFER_NUM_PIXELS(maxDimensions.GetX(), maxDimensions.GetY());
			if (cudaMemcpy(pixels, hostBuffer.Buffer(), sizeof(ElementT)*numPixels, cudaMemcpyHostToDevice) != cudaSuccess) {
				//Something bad happened!
				assert(false && "Copy from host buffer to device buffer failed!");
			}
		}
		
		void CopyToHostBuffer(ImgBuffer<ElementT> hostBuffer) {
			size_t numPixels = IMGBUFFER_NUM_PIXELS(maxDimensions.GetX(), maxDimensions.GetY());
			if (cudaMemcpy(hostBuffer.Buffer(), pixels sizeof(ElementT)*numPixels, cudaMemcpyDeviceToHost) != cudaSuccess) {
				//Something bad happened!
				assert(false && "Copy from device buffer to host buffer failed!");
			}
		}

		CUDAImgBuffer<ElementT>(size_t pWidth, size_t pHeight) : maxDimensions(pWidth, pHeight)
		{
			assert(pWidth > 0 && pHeight > 0 && "ImgBuffer::ImgBuffer(): Trying to construct a buffer with 0 area!");
			origin = IntVec2::Zero();
			end = maxDimensions;
			SetWidth(maxDimensions.GetX());
			SetHeight(maxDimensions.GetY());
			//Allocate our buffer.
			//ElementT better have a default constructor...
			void* pixelMem;
			if (cudaMalloc((void**)&pixelMem, sizeof(ElementT)*IMGBUFFER_NUM_PIXELS(maxDimensions.GetX(), maxDimensions.GetY())) != cudaSuccess) {
				//Error ocurred.
				assert(false && "Failed to allocate device-side image buffer!");
			}
			pixels = new (pixelMem) ElementT[IMGBUFFER_NUM_PIXELS(maxDimensions.GetX(), maxDimensions.GetY())];
			//Zero out the buffer.
			//Should be unnecessary since array new should call default constructor,
			//but I'm paranoid.
			cudaMemset(pixels, 0, IMGBUFFER_NUM_PIXELS(maxDimensions.GetX(), maxDimensions.GetY()) * sizeof(ElementT));
		}

		~CUDAImgBuffer<ElementT>()
		{
			//Get rid of our buffer.
			cudaFree(pixels);
		}
	};
}
