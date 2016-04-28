#pragma once
#include "../StandardIncludes.h"
#include "BufferSpecializations.h"
#include "../Profiling/Profiler.h"

namespace Graphics
{
	class CUDAProcessor
	{
	private:
		enum RemoveMode
		{
			REMOVE_ROWS = 0,
			REMOVE_COLS = 1,
			REMOVE_MODE_COUNT
		};

		enum SeamRemoveDirection
		{
			REMOVE_DIRECTION_UP,
			REMOVE_DIRECTION_DOWN,
			REMOVE_DIRECTION_COUNT,
		};

		size_t imageWidth;
		size_t imageHeight;
		size_t imageNumPixels;
		CUDALABColorBuffer image;
		CUDAEnergyBuffer energy;
		CUDASeamTracebackBuffer seamTraceback;
		RemoveMode removeMode;
		Profiler& profiler;

		void calcScharrAtPixel(signedSize_t x, signedSize_t y);
		//void calcEnergyAtPixel(signedSize_t x, signedSize_t y);
		void calcAllEnergy();
		/**
		Recalculates energy around a removed seam.
		*/
		void recalcSeamEnergy(size_t seamIdx, bool transpose);
		void resetSeamStart(bool transpose);
		void calcSeamCosts(bool transpose);
		size_t findMinCostSeam(bool transpose);
		SeamRemoveDirection removeSeam(size_t seamIdx, bool transpose);
		void highlightSeam(LABColorBuffer& seamBuffer, size_t seamIdx, bool transpose);
		void updateBounds(size_t oppositeRowsLeft, SeamRemoveDirection seamRemoveDirection);
		void doProcessImage(size_t numRowsToRemove, size_t numColsToRemove, size_t numCores);
	public:
		CUDAProcessor(LABColorBuffer& pImage, Profiler& pProfiler);
		~CUDAProcessor();

		/**
		Performs seam cutting on an image.
		*/
		void ProcessImage(size_t numRowsToRemove, size_t numColsToRemove);
		void TestProcessImage();
	};
}