#include "Random.h"
#include <cstdlib>
#include <time.h>

using namespace Graphics;
using namespace std;

//Returns a float in the range [-1, 1].
float posNegFloat()
{
	return (((float)rand()) / ((float)RAND_MAX) - 0.5f) * 2.0f;
}

void Graphics::Random::RandomInit()
{
	srand(time(NULL));
}

Vec3 Graphics::Random::RandomNormalizedVec3()
{
	return Vec3::GetNormalized(Vec3(posNegFloat(), posNegFloat(), posNegFloat()));
}

Vec3 Graphics::Random::RandomInSphere(const Vec3& sphereOrigin, float sphereRadius)
{
	return sphereOrigin + RandomNormalizedVec3()*sphereRadius;
}

Vec3 Graphics::Random::RandomInAABB(const AABB& aabb)
{
	return aabb.GetCenter() + Vec3::ComponentWiseMultiply(aabb.GetHalfExtents(), RandomNormalizedVec3());
}