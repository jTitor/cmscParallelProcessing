#pragma once
#include "../Primitives/ImgBuffer.h"
#include "../Primitives/CUDAImgBuffer.h"
#include "SeamTracebackElem.h"
#include "../Libraries/Graphics435.h"

namespace Graphics
{
	typedef ImgBuffer<Vec3> LABColorBuffer;
	typedef ImgBuffer<EnergyT> EnergyBuffer;
	typedef ImgBuffer<SeamTracebackElem> SeamTracebackBuffer;

	void WriteEnergyBuffer(EnergyBuffer& energyBuffer, const char* outPath);
	void WriteImageBuffer(LABColorBuffer& imageBuffer, const char* outPath);
	void WriteTracebackBuffer(SeamTracebackBuffer& tracebackBuffer, const char* outPath);

	LABColorBuffer* LoadImageBuffer(const char* inPath);

	typedef CUDAImgBuffer<Vec3> CUDALABColorBuffer;
	typedef CUDAImgBuffer<EnergyT> CUDAEnergyBuffer;
	typedef CUDAImgBuffer<SeamTracebackElem> CUDASeamTracebackBuffer;
}