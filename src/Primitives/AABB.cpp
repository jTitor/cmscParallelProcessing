#include "AABB.h"

using namespace Graphics;

void AABB::init(const Vec3& pCenter, const Vec3& pHalfExtents)
{
	center = pCenter;
	halfExtents = pHalfExtents;
}

AABB::AABB()
{
	init(Vec3::Zero(), Vec3::Zero());
}

AABB::AABB(const Vec3& pCenter, const Vec3& pHalfExtents)
{
	init(pCenter, pHalfExtents);
}

AABB AABB::FromMinMaxBounds(const Vec3& min, const Vec3& max)
{
	Vec3 halfDims = (max - min) / 2.0f;
	Vec3 center = min + halfDims;
	return AABB(center, halfDims);
}

/**
Gets the center of the AABB.
This can be used to modify the AABB's center.
*/
Vec3& AABB::Center()
{
	return center;
}

/**
Gets a constant reference to the center of the AABB.
*/
const Vec3& AABB::GetCenter() const
{
	return center;
}

/**
Gets the dimensions of the AABB divided by 2.
This can be used to modify the AABB's dimensions.
*/
Vec3& AABB::HalfExtents()
{
	return halfExtents;
}

/**
Gets a constant reference to the dimensions of the AABB divided by 2.
This can be used to modify the AABB's dimensions.
*/
const Vec3& AABB::GetHalfExtents() const
{
	return halfExtents;
}

Vec3 AABB::Min() const
{
	return center - halfExtents;
}

Vec3 AABB::Max() const
{
	return center + halfExtents;
}

Vec3 AABB::ConstrainVec3(const Vec3& v) const
{
	return Vec3::ComponentWiseMin(Vec3::ComponentWiseMax(v, Min()), Max());
}

Vec3 AABB::GetViolatingPlanes(const Vec3& v) const
{
	Vec3 min = Min();
	Vec3 max = Max();
	float x = 0;
	if (v.GetX() < min.X())
	{
		x = -1;
	}
	else if (v.GetX() > max.X())
	{
		x = 1;
	}
	float y = 0;
	if (v.GetY() < min.Y())
	{
		y = -1;
	}
	else if (v.GetY() > max.Y())
	{
		y = 1;
	}
	float z = 0;
	if (v.GetZ() < min.Z())
	{
		z = -1;
	}
	else if (v.GetZ() > max.Z())
	{
		z = 1;
	}
	return Vec3(x, y, z);
}

bool AABB::Contains(const Vec3& v) const
{
	Vec3 min = Min();
	Vec3 max = Max();
	return (v.GetX() >= min.X() && v.GetX() <= max.X()) &&
		(v.GetY() >= min.Y() && v.GetY() <= max.Y()) &&
		(v.GetZ() >= min.Z() && v.GetZ() <= max.Z());
}