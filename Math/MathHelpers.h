#pragma once

namespace Graphics
{
	/**
	Finds the sign of a signed number.
	Returns: 1 if the number is positive, 0 if the number is 0, and -1 if the number is negative.
	*/
	template<typename T>
	T Sign(T val)
	{
		if (val > 0)
		{
			return 1;
		}
		else if (val < 0)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	bool NearlyEqual(float a, float b);
}