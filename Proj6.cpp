//Entry point for the renderer.
#include "Proj6.h"
#include "StandardIncludes.h"
#include "SystemModules/TestMacros.h"
#include "SystemModules/Processor.h"
#include "Libraries/Graphics435.h"
#include <stdio.h>
#include <assert.h>

using namespace Graphics;

int main(int numArgs, char** args)
{
	char* inPath;
	char* outPath;
	size_t newWidth;
	size_t newHeight;
	//Sanity check parameters.
	//Expecting format ./Proj6 (input path) (output path) (output_width) (output_height).
	if (numArgs < 1 + 4)
	{
		printf("Syntax: ./Proj6 (input path) (output path) (output width) (output height)\n");
		return 0;
	}
	inPath = args[1];
	outPath = args[2];
	newWidth = atoi(args[3]);
	newHeight = atoi(args[4]);
	//Also check that dimensions are sane.
	if (newWidth < 1 || newHeight < 1)
	{
		printf("main(): Invalid dimensions, aborting!\n");
		return 0;
	}

	//Load the image.
	LABColorBuffer* image = LoadImageBuffer(inPath);

	printf("main(): Resizing from %d x %d to %d x %d\n", image->Width(), image->Height(), newWidth, newHeight);
	ptrdiff_t colsToRemove = image->Width() - newWidth;
	ptrdiff_t rowsToRemove = image->Height() - newHeight;
	//Check that the output dimensions are smaller than input.
	if (rowsToRemove < 0 || colsToRemove < 0)
	{
		printf("main(): New dimensions are larger than original dimensions, aborting!\n");
		return 0;
	}
	if (rowsToRemove == 0 && colsToRemove == 0)
	{
		printf("main(): New dimensions are same as original dimensions, aborting!\n");
		return 0;
	}	
	
	//Do our work...
	Processor* processor = new Processor(*image);
	processor->ProcessImage(rowsToRemove, colsToRemove);

	WriteImageBuffer(*image, outPath);

	//Release resources.
	delete processor;
	return 0;
}