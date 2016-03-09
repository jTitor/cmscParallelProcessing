#pragma once
#include "../StandardIncludes.h"
#include "../Math/IntVec2.h"

namespace Graphics
{
#define IMGBUFFER_NUM_PIXELS(x, y) x*y
#define IMGBUFFER_XY_TO_IDX(x, y, width) y*width + x

	template<typename ElementT>
	class ImgBuffer
	{
	private:
		ElementT* pixels;
		const IntVec2 maxDimensions;
		IntVec2 origin;
		IntVec2 end;//dimensions;
		inline IntVec2 getDimensions() const
		{
			return end - origin;
		}
		inline IntVec2 getAnchoredMaxDimensions() const
		{
			return maxDimensions - origin;
		}
	public:
		/**
		Gets the width of the image buffer.
		*/
		size_t Width() const
		{
			return getDimensions().GetX();
		}

		bool SetWidth(size_t newWidth)
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
		size_t Height() const
		{
			return getDimensions().GetY();
		}

		bool SetHeight(size_t newHeight)
		{
			//Abort if this is larger than the maximum end.
			if (newHeight < 1 || newHeight > getAnchoredMaxDimensions().GetY())
			{
				return false;
			}
			end.Y() = origin.GetY() + newHeight;
			return true;
		}

		const IntVec2& Origin() const
		{
			return origin;
		}

		bool SetOrigin(const IntVec2& newOrigin)
		{
			if (newOrigin.GetX() < 0 || newOrigin.GetX() >= end.GetX() ||
				newOrigin.GetY() < 0 || newOrigin.GetY() >= end.GetY())
			{
				return false;
			}
			origin = newOrigin;
			return true;
		}

		const IntVec2& End() const
		{
			return end;
		}

		bool SetEnd(const IntVec2& newEnd)
		{
			if (newEnd.GetX() <= origin.GetX() || newEnd.GetX() > maxDimensions.GetX() ||
				newEnd.GetY() <= origin.GetY() || newEnd.GetY() > maxDimensions.GetY())
			{
				return false;
			}
			end = newEnd;
			return true;
		}

		bool SetDimensions(const IntVec2& newDimensions)
		{
			if (newDimensions.GetX() < 0 || newDimensions.GetX() > getAnchoredMaxDimensions().GetX() ||
				newDimensions.GetY() < 0 || newDimensions.GetY() > getAnchoredMaxDimensions().GetY())
			{
				return false;
			}
			end = origin + newDimensions;
			true;
		}

		bool IsInBounds(ptrdiff_t x, ptrdiff_t y) const
		{
			
			return (x >= 0 && x < getDimensions().GetX()) &&
				(y >= 0 && y < getDimensions().GetY());
		}
		bool IsInBounds(const IntVec2& pos) const
		{
			return IsInBounds(pos.GetX(), pos.GetY());
		}

		/**
		Gets a constant reference to the pixel at (x, y).
		*/
		const ElementT* PixelAt(ptrdiff_t x, ptrdiff_t y) const
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
		const ElementT* PixelAt(const IntVec2& pos) const
		{
			return PixelAt(pos.GetX(), pos.GetY());
		}

		/**
		Gets a writeable reference to the pixel at (x, y).
		*/
		ElementT* WritablePixelAt(ptrdiff_t x, ptrdiff_t y)
		{
			return const_cast<ElementT*>(PixelAt(x, y));
		}
		ElementT* WritablePixelAt(const IntVec2& pos)
		{
			return WritablePixelAt(pos.GetX(), pos.GetY());
		}

		void SetPixelAt(ptrdiff_t x, ptrdiff_t y, const ElementT& value)
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
		void SetPixelAt(const IntVec2& pos, const ElementT& value)
		{
			SetPixelAt(pos.GetX(), pos.GetY(), value);
		}

		/**
		Sets all pixels in the buffer to the given value.
		*/
		void Clear(const ElementT& value)
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
		void CopyToBuffer(const ElementT* srcPixels)
		{
			//For each pixel:
			for (size_t i = 0; i < IMGBUFFER_NUM_PIXELS(maxDimensions.GetX(), maxDimensions.GetY()); ++i)
			{
				//Copy over the given value.
				pixels[i] = srcPixels[i];
			}
		}

		ImgBuffer<ElementT>(size_t pWidth, size_t pHeight) : maxDimensions(pWidth, pHeight)
		{
			assert(pWidth > 0 && pHeight > 0 && "ImgBuffer::ImgBuffer(): Trying to construct a buffer with 0 area!");
			origin = IntVec2::Zero();
			end = maxDimensions;
			SetWidth(maxDimensions.GetX());
			SetHeight(maxDimensions.GetY());
			//Allocate our buffer.
			//ElementT better have a default constructor...
			pixels = new ElementT[IMGBUFFER_NUM_PIXELS(maxDimensions.GetX(), maxDimensions.GetY())];
			//Zero out the buffer.
			//Should be unnecessary since array new should call default constructor,
			//but I'm paranoid.
			memset(pixels, 0, IMGBUFFER_NUM_PIXELS(maxDimensions.GetX(), maxDimensions.GetY()) * sizeof(ElementT));
		}

		~ImgBuffer<ElementT>()
		{
			//Get rid of our buffer.
			delete[] pixels;
		}
	};
}