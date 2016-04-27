#pragma once
#define cimg_display 0
//Set to 1 to enable support 
//for the specified format.
//Windows is acting crazy,
//so these are both disabled by default and only BMPs can be loaded.
#undef cimg_use_jpeg
#undef cimg_use_png

#include "graphics435/slIO.H"
#include "graphics435/slVector.H"
#include "graphics435/CImg.h"