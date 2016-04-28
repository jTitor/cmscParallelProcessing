#include "BufferSpecializations.h"

using namespace Graphics;
using namespace cimg_library;

void doWrite(CImg<double>& outBuffer, const char* outPath)
{
	//Might as well switch on the output extension.
	if (strstr(outPath, "jpg"))
	{
		outBuffer.save_jpeg(outPath);
	}
	if (strstr(outPath, "png"))
	{
		outBuffer.save_png(outPath);
	}
	//Default to .bmp.
	else
	{
		outBuffer.save_bmp(outPath);
	}
}

void copyEnergyToGrayscaleCImg(CImg<double>& outImage, EnergyBuffer& inImage, EnergyT maxEnergy)
{
	for (unsigned int i = 0; i < inImage.Width(); i++)
	{
		for (unsigned int j = 0; j < inImage.Height(); j++)
		{
			auto pixel = inImage.PixelAt(i, j);
			//Set each pixel!
			outImage(i, j, 0) = (*pixel / maxEnergy) * 255;
		}
	}
}

void copyLABCImgToImage(LABColorBuffer& outImageBuffer, CImg<double>& labIn)
{
	for (unsigned int i = 0; i < outImageBuffer.Width(); i++)
	{
		for (unsigned int j = 0; j < outImageBuffer.Height(); j++)
		{
			auto pixel = outImageBuffer.WritablePixelAt(i, j);
			//Set each pixel!
			for (int k = 0; k < VEC3_NUM_COMPONENTS; ++k)
			{
				pixel->AtIndex(k) = labIn(i, j, k);
			}
		}
	}
}

void copyImageToLABCImg(CImg<double>& labOut, LABColorBuffer& imageBuffer)
{
	for (unsigned int i = 0; i < imageBuffer.Width(); i++)
	{
		for (unsigned int j = 0; j < imageBuffer.Height(); j++)
		{
			auto pixel = imageBuffer.PixelAt(i, j);
			//Set each pixel!
			for (int k = 0; k < VEC3_NUM_COMPONENTS; ++k)
			{
				labOut(i, j, k) = pixel->AtIndex(k);
			}
		}
	}
}

void copyTracebackToRGBCImg(CImg<double>& rgbOut, SeamTracebackBuffer& tracebackBuffer, EnergyT maxCost)
{
	for (unsigned int i = 0; i < tracebackBuffer.Width(); i++)
	{
		for (unsigned int j = 0; j < tracebackBuffer.Height(); j++)
		{
			auto pixel = tracebackBuffer.PixelAt(i, j);
			float x = 1;
			float y = 0;
			switch (pixel->TracebackDirection)
			{
			case SEAM_TB_LDOWN:
				y = 0;
				break;
			case SEAM_TB_LEFT:
				y = 0.5;
				break;
			case SEAM_TB_LUP:
				y = 1;
				break;
			}
			Vec3 color = Vec3(x, y, 0) * 255 * (pixel->SeamCost / maxCost);
			//Set each pixel!
			for (int k = 0; k < VEC3_NUM_COMPONENTS; ++k)
			{
				rgbOut(i, j, k) = color.AtIndex(k);
			}
		}
	}
}

void Graphics::WriteEnergyBuffer(EnergyBuffer& energyBuffer, const char* outPath)
{
	//Energies are unnormalized;
	//you may need to normalize it first.
	//Get max energy to normalize by.
	EnergyT maxEnergy = FLT_MIN;
	for (unsigned int i = 0; i < energyBuffer.Width(); i++)
	{
		for (unsigned int j = 0; j < energyBuffer.Height(); j++)
		{
			auto pixel = energyBuffer.PixelAt(i, j);
			if (*pixel > maxEnergy)
			{
				maxEnergy = *pixel;
			}
		}
	}
#ifdef _DEBUG
	printf("Graphics::WriteEnergyBuffer(): Writing file %s with max energy: %f\n", outPath, maxEnergy);
#endif

	//Open our buffer.
	//It only needs grayscale output.
	CImg<double> grayscaleOut = CImg<double>(energyBuffer.Width(), energyBuffer.Height(), 1, 1);
	copyEnergyToGrayscaleCImg(grayscaleOut, energyBuffer, maxEnergy);
	//Write buffer out to disk.
	doWrite(grayscaleOut, outPath);
}

void Graphics::WriteImageBuffer(LABColorBuffer& imageBuffer, const char* outPath)
{
	//Open our buffer.
	CImg<double> labOut = CImg<double>(imageBuffer.Width(), imageBuffer.Height(), 1, 3);
	copyImageToLABCImg(labOut, imageBuffer);
	CImg<double> rgbOut = labOut.LabtoRGB();

#ifdef _DEBUG
	printf("Graphics::WriteImageBuffer(): Writing file %s\n", outPath);
#endif

	//Write buffer out to disk.
	doWrite(rgbOut, outPath);
}

void Graphics::WriteTracebackBuffer(SeamTracebackBuffer& tracebackBuffer, const char* outPath)
{
	EnergyT maxCost = FLT_MIN;
	for (unsigned int i = 0; i < tracebackBuffer.Width(); i++)
	{
		auto pixel = tracebackBuffer.PixelAt(i, tracebackBuffer.Height() - 1);
		if (pixel->SeamCost > maxCost)
		{
			maxCost = pixel->SeamCost;
		}
	}
	for (unsigned int i = 0; i < tracebackBuffer.Height(); i++)
	{
		auto pixel = tracebackBuffer.PixelAt(tracebackBuffer.Width() - 1, i);
		if (pixel->SeamCost > maxCost)
		{
			maxCost = pixel->SeamCost;
		}
	}
#ifdef _DEBUG
	printf("Graphics::WriteTracebackBuffer(): Writing file %s with max cost: %f\n", outPath, maxCost);
#endif

	CImg<double> rgbOut = CImg<double>(tracebackBuffer.Width(), tracebackBuffer.Height(), 1, 3);
	copyTracebackToRGBCImg(rgbOut, tracebackBuffer, maxCost);
	//Write buffer out to disk.
	doWrite(rgbOut, outPath);
}

LABColorBuffer* Graphics::LoadImageBuffer(const char* inPath)
{
	//Remember to convert from RGB!
	CImg<double> inputRGB = CImg<double>(inPath);
	CImg<double> inputLAB = inputRGB.RGBtoLab();

	LABColorBuffer* image = new LABColorBuffer(inputLAB.width(), inputLAB.height());
	if (!image)
	{
		assert(false && "Graphics::LoadImageBuffer(): Out of memory for image buffer!");
		return image;
	}
	copyLABCImgToImage(*image, inputLAB);
	return image;
}