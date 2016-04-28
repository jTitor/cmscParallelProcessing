#pragma once
#include "../StandardIncludes.h"
#include "BufferSpecializations.h"
#include "../Profiling/Profiler.h"

namespace Graphics
{
	class OMPProcessor
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

		size_t numThreads;
		LABColorBuffer& image;
		EnergyBuffer* energy;
		SeamTracebackBuffer* seamTraceback;
		RemoveMode removeMode;
		Profiler& profiler;

		void calcScharrAtPixel(signedSize_t x, signedSize_t y);
		//void calcEnergyAtPixel(signedSize_t x, signedSize_t y);
		void calcAllEnergy();
		/**
		Recalculates energy around a removed seam.
		*/
		void recalcSeamEnergy(size_t seamIdx, bool transpose);
		void calcSeamCosts(bool transpose);
		size_t findMinCostSeam(bool transpose);
		SeamRemoveDirection removeSeam(size_t seamIdx, bool transpose);
		void highlightSeam(LABColorBuffer& seamBuffer, size_t seamIdx, bool transpose);
	public:
		OMPProcessor(LABColorBuffer& pImage, Profiler& pProfiler, size_t pNumThreads);
		~OMPProcessor();

		/**
		Performs seam cutting on an image.
		*/
		void ProcessImage(size_t numRowsToRemove, size_t numColsToRemove);
		void TestProcessImage();
	};
}