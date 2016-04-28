#pragma once
#include "../StandardIncludes.h"

namespace Graphics
{
	enum SeamTracebackDirection
	{
		SEAM_TB_END,	//Reached the end of the image.
		SEAM_TB_LDOWN,
		SEAM_TB_LEFT,
		SEAM_TB_LUP,
		SEAM_TB_COUNT
	};

	class SeamTracebackElem
	{
	public:
		SeamTracebackDirection TracebackDirection;
		EnergyT SeamCost;

		SeamTracebackElem();
	};
}