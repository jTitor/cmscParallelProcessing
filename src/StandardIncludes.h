#pragma once
#include "Primitives/Color.h"
#include <cassert>

typedef float EnergyT;
typedef ptrdiff_t signedSize_t;

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#define CUDA_DEVICE_ONLY_MEMBER __device__
#define CUDA_GLOBAL_MEMBER __global__
#else
#define CUDA_CALLABLE_MEMBER
#define CUDA_DEVICE_ONLY_MEMBER
#define CUDA_GLOBAL_MEMBER
#endif