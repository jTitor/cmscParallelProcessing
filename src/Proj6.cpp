//Entry point for the renderer.
#include "Proj6.h"
#include "StandardIncludes.h"
#include "SystemModules/TestMacros.h"
#include "SystemModules/OMPProcessor.h"
#include "Profiling/Profiler.h"
#include "Libraries/Graphics435.h"
#include <stdio.h>
#include <assert.h>
#include <omp.h>

using namespace Graphics;

struct ProgContext {
	char* InPath;
	char* OutPath;
	size_t NewWidth;
	size_t NewHeight;
	ptrdiff_t RowsToRemove;
	ptrdiff_t ColsToRemove;
	int NumThreads;
	LABColorBuffer* Image;
	Profiler* Profiler;
};

bool loadParameters(ProgContext& ctx, int numArgs, char** args) {
	//Set defaults.
	ctx.NumThreads = omp_get_num_procs();

	//Sanity check parameters.
	//Expecting format ./Proj6 (input path) (output path) (output_width) (output_height).
	if (numArgs < 1 + 4)
	{
		printf("%s\n", "Syntax: ./Proj6 (input path) (output path) (output width) (output height) [number of threads]");
		return false;
	}
	ctx.InPath = args[1];
	ctx.OutPath = args[2];
	ctx.NewWidth = atoi(args[3]);
	ctx.NewHeight = atoi(args[4]);
	//Also check that dimensions are sane.
	if (ctx.NewWidth < 1 || ctx.NewHeight < 1)
	{
		printf("main(): Invalid dimensions, aborting!\n");
		return false;
	}

	//Do we have a thread count parameter?
	if (numArgs >= 1 + 5) {
		//Load that up too.
		int numThreads = atoi(args[5]);
		if (numThreads >= 1 && numThreads < omp_get_max_threads()) {
			ctx.NumThreads = numThreads;
		}
	}

	return true;
}

bool validateNewDimensions(ProgContext& ctx) {
	printf("main(): Resizing from %d x %d to %d x %d\n", ctx.Image->Width(), ctx.Image->Height(), ctx.NewWidth, ctx.NewHeight);
	//Check that the output dimensions are smaller than input.
	if (ctx.RowsToRemove < 0 || ctx.ColsToRemove < 0)
	{
		printf("%s\n", "main(): New dimensions are larger than original dimensions, aborting!");
		return false;
	}
	if (ctx.RowsToRemove == 0 && ctx.ColsToRemove == 0)
	{
		printf("%s\n", "main(): New dimensions are same as original dimensions, aborting!");
		return false;
	}

	return true;
}

void processImage(ProgContext& ctx) {
	OMPProcessor* processor = new OMPProcessor(*ctx.Image, *ctx.Profiler, ctx.NumThreads);
	processor->ProcessImage(ctx.RowsToRemove, ctx.ColsToRemove);

	WriteImageBuffer(*ctx.Image, ctx.OutPath);

	delete processor;
}

void displayResults(ProgContext& ctx) {
	printf("%s\n", "Resize complete, displaying profile data:");
	ctx.Profiler->PrintAll();
	//Also save them out to CSV.
	const char* profilePath = "profileData.csv";
	printf("Saving profile data to %s...\n", profilePath);
	FILE* profileFile = fopen(profilePath, "a+");
	if (!profileFile) {
		printf("Couldn't open %s! Didn't save profile data!", profilePath);
	}
	else {
		ctx.Profiler->PrintAllCSV(profileFile);
	}
}

int main(int numArgs, char** args)
{
	ProgContext ctx = { 0 };
	if (!loadParameters(ctx, numArgs, args)) {
		return 1;
	}
	
	//Load the image.
	ctx.Image = LoadImageBuffer(ctx.InPath);
	ctx.RowsToRemove = ctx.Image->Width() - ctx.NewWidth;
	ctx.ColsToRemove = ctx.Image->Height() - ctx.NewHeight;
	if (!validateNewDimensions(ctx)) {
		return 0;
	}
	
	
	//Do our work...
	ctx.Profiler = new Profiler();
	processImage(ctx);
	

	//Display performance results.
	displayResults(ctx);

	//Release resources.
	delete ctx.Profiler;
	return 0;
}