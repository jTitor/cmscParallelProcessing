#pragma once
#include "../Time/Clock.h"

namespace Graphics {
	
	enum ProfileCode {
		PC_CALC_ALL_ENERGY,
		PC_CALC_SEAM_COSTS,
		PC_FIND_MIN_COST_SEAM,
		PC_REMOVE_SEAM,
		PC_RECALC_SEAM_ENERGY,
		PC_BOUNDS_ADJUST,
		PC_COUNT
	};
	
	//Profiler. Assumes all categories are same level - none are subcategories of other categories.
	class Profiler {
	private:
		Seconds* profileTotalTimes;
		Seconds* profileLastStartTimes;
	public:
		Profiler();
		~Profiler();
		void StartProfile(ProfileCode category);
		void EndProfile(ProfileCode category);
		/**
		Prints all profile data as a nice table.
		*/
		void PrintAll();
		/**
		Prints all profile data to a file as CSV data.
		*/
		void PrintAllCSV(FILE* output);
	};
}