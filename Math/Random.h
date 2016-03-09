#pragma once
#include "Vec3.h"
#include "../Primitives/AABB.h"

namespace Graphics
{
	namespace Random
	{
		void RandomInit();
		Vec3 RandomNormalizedVec3();
		Vec3 RandomInSphere(const Vec3& sphereOrigin, float sphereRadius);
		Vec3 RandomInAABB(const AABB& aabb);
	}
}