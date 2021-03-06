#include "Profiler.h"
#include <cassert>
#include <cstdio>

using namespace std;
using namespace Graphics;

static char* profileNames[] = {
	"Calc. All Energy",
	"Calc. Seam Costs",
	"Find Min. Cost Seam",
	"Remove Seam",
	"Recalc Image Energy",
	"Adjust Boundaries"
	};

Profiler::Profiler()
{
	//Initialize profile buffers.
	profileTotalTimes = new Seconds[PC_COUNT];
	profileLastStartTimes = new Seconds[PC_COUNT];
	
	for(int i = 0; i < PC_COUNT; ++i) {
		profileTotalTimes[i] = 0;
		profileLastStartTimes[i] = -1;
	}
}

Profiler::~Profiler()
{
	//Free profile buffers.
	delete[] profileTotalTimes;
	delete[] profileLastStartTimes;
}

void Profiler::StartProfile(ProfileCode category)
{
	assert(category < PC_COUNT);
	profileLastStartTimes[category] = Clock::GetCurrentTimestamp();
}

void Profiler::EndProfile(ProfileCode category)
{
	//Category should be in bounds and already had StartProfile called on it.
	assert(category < PC_COUNT && profileLastStartTimes[category] >= 0);
	//Get the elapsed time.
	auto currTime = Clock::GetCurrentTimestamp();
	//Add that to the category total.
	profileTotalTimes[category] += currTime - profileLastStartTimes[category];
}

/**
Prints all profile data as a nice table.
*/
void Profiler::PrintAll()
{
	//Print the header.
	printf("%s\n", "Category\t| Total Time (s)\t| Percentage of Execution");
	printf("%s\n", "-----------------------------------------------------");
	
	//Calculate the total execution time.
	Seconds totalTime = 0;
	for(int i = 0; i < PC_COUNT; ++i) {
		totalTime += profileTotalTimes[i];
	}
	
	//Now display the data!
	for(int i = 0; i < PC_COUNT; ++i) {
		Seconds profileTime = profileTotalTimes[i];
		printf("%s\t| %f\t| %f\n", profileNames[i], profileTime, profileTime / totalTime);
	}
	printf("%s\t| %f\t| %f\n", "Total", totalTime, totalTime / totalTime);
}

/**
Prints all profile data to a file as CSV data.
*/
void Profiler::PrintAllCSV(FILE* output)
{
	//Print the header.
	fprintf(output, "%s\n", "Category,Total Time (s),Percentage of Execution");
	
	//Calculate the total execution time.
	Seconds totalTime = 0;
	for(int i = 0; i < PC_COUNT; ++i) {
		totalTime += profileTotalTimes[i];
	}
	
	//Now display the data!
	for(int i = 0; i < PC_COUNT; ++i) {
		Seconds profileTime = profileTotalTimes[i];
		fprintf(output, "%s,%e,%e\n", profileNames[i], profileTime, profileTime / totalTime);
	}
	fprintf(output, "%s,%e,%e\n", "Total", totalTime, totalTime / totalTime);
	fprintf(output, "\n");
}
