#include "Clock.h"
#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <time.h>
#elif defined(__APPLE__)

#endif

const double milisecondsInSeconds = 1000.0;
const double microsecondsInSeconds = 1000.0 * milisecondsInSeconds;
const double nanosecondsInSeconds = 1000.0 * microsecondsInSeconds;

using namespace Graphics;

Seconds Clock::GetCurrentTimestamp()
{
	//platform dependent!
	Seconds result = 0;
#if defined(_WIN32)
	LARGE_INTEGER frequency = { 0 };
	LARGE_INTEGER timestamp = { 0 };
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&timestamp);
	double castSeconds = (double)(timestamp.QuadPart) / frequency.QuadPart;
	result = (Seconds)castSeconds;
#elif defined(__linux__)
	struct timespec time;
	//Get the timestamp.
	clock_gettime(CLOCK_MONOTONIC, &time);
	//We aren't running before 1970, so we're good!
	//For now, we don't care about resolution.
	//Just sum up the seconds and nanoseconds.
	double seconds = (double)time.tv_sec;
	double nanosecondsAsSeconds = ((double)time.tv_nsec) / nanosecondsInSeconds;
	seconds += nanosecondsInSeconds;
	result = (Seconds)seconds;
#elif defined(__APPLE__)
	//Get absolute time.
	uint64_t time = mach_absolute_time();
	//Convert this to nanoseconds...
	Nanoseconds nanoTime = AbsoluteToNanoseconds(*(AbsoluteTime*)time);
	time = *(uint64_t*)&nanoTime;
	//Now convert that to a double for the moment.
	double castTime = (double)time;
	//Convert this to seconds.
	castTime /= nanosecondsInSeconds;
	result = (Seconds)castTime;
#else
	static_assert(false, "Unrecognized platform; implement clockGetCurrentTimestamp for this platform!");
#endif
	return result;
}

Clock::Clock()
{
	startTime = GetCurrentTimestamp();
	currentTime = startTime;
	previousTime = currentTime;
}

Seconds Clock::ElapsedTime() const
{
	return currentTime - previousTime;
}

Seconds Clock::TotalTime() const
{
	return currentTime - startTime;
}

void Clock::Tick()
{
	previousTime = currentTime;
	currentTime = GetCurrentTimestamp();
}