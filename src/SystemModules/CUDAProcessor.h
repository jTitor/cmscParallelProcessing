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

		CUDA_DEVICE_ONLY_MEMBER void calcScharrAtPixel(signedSize_t x, signedSize_t y);
		//void calcEnergyAtPixel(signedSize_t x, signedSize_t y);
		CUDA_GLOBAL_MEMBER void calcAllEnergy();
		/**
		Recalculates energy around a removed seam.
		*/
		CUDA_DEVICE_ONLY_MEMBER void recalcSeamEnergy(size_t seamIdx, bool transpose);
		CUDA_GLOBAL_MEMBER void resetSeamStart(bool transpose);
		CUDA_GLOBAL_MEMBER void calcSeamCosts(bool transpose);
		CUDA_DEVICE_ONLY_MEMBER size_t findMinCostSeam(bool transpose);
		CUDA_DEVICE_ONLY_MEMBER SeamRemoveDirection removeSeam(size_t seamIdx, bool transpose);
		CUDA_GLOBAL_MEMBER void updateBounds(size_t oppositeRowsLeft, SeamRemoveDirection seamRemoveDirection);
		void doProcessImage(size_t numRowsToRemove, size_t numColsToRemove, size_t numCores);
	public:
		CUDAProcessor(LABColorBuffer& pImage, Profiler& pProfiler);
		~CUDAProcessor();

		/**
		Performs seam cutting on an image.
		*/
		void ProcessImage(size_t numRowsToRemove, size_t numColsToRemove);
	};
}