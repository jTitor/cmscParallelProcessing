#pragma once
#include "../Math/Vec3.h"

namespace Graphics
{
	/**
	Represents an axis-aligned bounding box.
	*/
	class AABB
	{
	private:
		void init(const Vec3& pCenter, const Vec3& pHalfExtents);

		//The center of the box.
		Vec3 center;
		//The distance from the center to
		//the given axis' edge on the box.
		Vec3 halfExtents;
	public:
		static AABB FromMinMaxBounds(const Vec3& min, const Vec3& max);

		AABB();
		AABB(const Vec3& pCenter, const Vec3& pHalfExtents);

		/**
		Gets the center of the AABB.
		This can be used to modify the AABB's center.
		*/
		Vec3& Center();
		/**
		Gets a constant reference to the center of the AABB.
		*/
		const Vec3& GetCenter() const;
		/**
		Gets the dimensions of the AABB divided by 2.
		This can be used to modify the AABB's dimensions.
		*/
		Vec3& HalfExtents();
		/**
		Gets a constant reference to the dimensions of the AABB divided by 2.
		This can be used to modify the AABB's dimensions.
		*/
		const Vec3& GetHalfExtents() const;
		Vec3 Min() const;
		Vec3 Max() const;
		/**
		Constrains a vector to the AABB.
		*/
		Vec3 ConstrainVec3(const Vec3& v) const;
		Vec3 GetViolatingPlanes(const Vec3& v) const;
		bool Contains(const Vec3& v) const;
	};
}