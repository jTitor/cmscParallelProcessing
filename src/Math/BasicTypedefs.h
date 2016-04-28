#pragma once
#include <climits>
#include <cfloat>
#include "MathHelpers.h"

#if defined(DISCRETE_SCALARS)
typedef ptrdiff_t ScalarT;
#define GRAPHICS_SCALAR_MIN PTRDIFF_MIN
#define GRAPHICS_SCALAR_MAX PTRDIFF_MAX
#define GRAPHICS_SCALAR_EQUALITY(a, b) a == b
#else
typedef float ScalarT;
#define GRAPHICS_SCALAR_MIN FLT_MIN
#define GRAPHICS_SCALAR_MAX FLT_MAX
#define GRAPHICS_SCALAR_EQUALITY(a, b) NearlyEqual(a,b)
#endif