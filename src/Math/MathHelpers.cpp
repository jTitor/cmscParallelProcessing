#include "MathHelpers.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>

using namespace Graphics;

bool Graphics::NearlyEqual(float a, float b)
{
	//Testing difference against absolute epsilon can't handle error between very small values;
	//see http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
	//Instead, combine a quick epsilon test for differences very close to zero,
	//and a int reinterpretation (see http://randomascii.wordpress.com/2012/01/23/stupid-float-tricks-2/)
	//to find significant differences in all other situations.
	//Note that this relies on properties of IEEE floats.
	float aVal = a;
	float bVal = b;
	float absDiff = fabsf(aVal - bVal);
	if (absDiff <= FLT_EPSILON)
	{
		return true;
	}

	//If signs are different and the difference is past epsilon,
	//we can be pretty sure this isn't -0 and +0, so assume they're different.
	//Int reintepretation won't work when signs differ, as the sign bit is the MOST significant bit
	//and so the two values will appear to be billions of units apart.
	if (Sign(aVal) != Sign(bVal))
	{
		return false;
	}

	//Otherwise, there's a fairly large difference between floats.
	//Reinterpret the floats as ints and get the integer difference to find how large that difference is
	//Specifically, the absolute value of the integer difference = 
	//1 + (number of representable floats between values)
	//since IEEE floats have mantissa as least significant bits,
	//and the exponent is the next significant bits past the mantissa.
	int diff = *(reinterpret_cast<int*>(&aVal)) - *(reinterpret_cast<int*>(&bVal));

	//The numbers are approximately equal if diff is less than the maximum tolerance value.
	const int MAX_DIFF = 1;
	return abs(diff) < MAX_DIFF;
}