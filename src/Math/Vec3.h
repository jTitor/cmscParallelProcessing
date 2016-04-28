#pragma once
#include "BasicTypedefs.h"
#include <string>
#define VEC3_NUM_COMPONENTS 3
using namespace std;
namespace Graphics
{
	//Represents a 3D vector.
	class Vec3
	{
	private:
		void init(ScalarT pX, ScalarT pY, ScalarT pZ);
		ScalarT components[VEC3_NUM_COMPONENTS];
	public:
#pragma region Static Members
		//Returns the zero vector.
		static Vec3 Zero();
		static Vec3 One();
		//Returns a vector with all components set to the maximum possible ScalarTing point value.
		static Vec3 AllMax();
		//Returns a vector with all components set to the minimum possible ScalarTing point value.
		static Vec3 AllMin();

		static ScalarT SqrLen(const Vec3& l, const Vec3& r);
		static ScalarT Len(const Vec3& l, const Vec3& r);
		static ScalarT Dot(const Vec3& l, const Vec3& r);
		static Vec3 Cross(const Vec3& l, const Vec3& r);
		//Returns a normalized version of the given vector,
		//but does NOT modify the vector.
		static Vec3 GetNormalized(const Vec3& v);

		/**
		Gets the vector's componentwise reciprocal <1/x, 1/y, 1/z>.
		*/
		static Vec3 Reciprocal(const Vec3& v);
		static Vec3 ComponentWiseMultiply(const Vec3& a, const Vec3& b);
		static Vec3 ComponentWiseDivide(const Vec3& a, const Vec3& b);

		static Vec3 ComponentWiseMax(const Vec3& clampee, const Vec3& clamper);
		static Vec3 ComponentWiseMin(const Vec3& clampee, const Vec3& clamper);
		static Vec3 ComponentWiseClamp(const Vec3& clampee, const Vec3& clamperMin, const Vec3& clamperMax);
		static Vec3 ComponentWiseAbs(const Vec3& v);
#pragma endregion

		Vec3();
		Vec3(const Vec3& copy);
		Vec3(ScalarT pX, ScalarT pY, ScalarT pZ);

		ScalarT& X();
		ScalarT& Y();
		ScalarT& Z();
		const ScalarT& GetX() const;
		const ScalarT& GetY() const;
		const ScalarT& GetZ() const;
		ScalarT& AtIndex(unsigned int idx);
		const ScalarT& AtIndex(unsigned int idx) const;

		//Operations.
		string ToString() const;
		const ScalarT* ToArray() const;
		//Gets the squared magnitude of a given vector.
		ScalarT SqrMag() const;
		//Gets the magnitude of a given vector.
		ScalarT Mag() const;
		//Gets the dot product of two vectors.
		ScalarT Dot(const Vec3& other) const;
		//Gets the cross product of two vectors.
		Vec3 Cross(const Vec3& other) const;
		//Normalizes the given vector.
		void Normalize();

		Vec3& operator=(const Vec3& other);
	};

	//Unit tests for Vec3 class.
	bool TestVec3();
}

//Vector arithmetic.
Graphics::Vec3 operator-(const Graphics::Vec3& v);

Graphics::Vec3& operator+=(Graphics::Vec3& l, const Graphics::Vec3& r);
Graphics::Vec3& operator-=(Graphics::Vec3& l, const Graphics::Vec3& r);
Graphics::Vec3& operator*=(Graphics::Vec3& l, ScalarT r);
Graphics::Vec3& operator/=(Graphics::Vec3& l, ScalarT r);
Graphics::Vec3& operator*=(ScalarT l, Graphics::Vec3& r);

Graphics::Vec3 operator+(const Graphics::Vec3& l, const Graphics::Vec3& r);
Graphics::Vec3 operator-(const Graphics::Vec3& l, const Graphics::Vec3& r);
Graphics::Vec3 operator*(const Graphics::Vec3& l, ScalarT r);
Graphics::Vec3 operator/(const Graphics::Vec3& l, ScalarT r);
Graphics::Vec3 operator*(ScalarT l, const Graphics::Vec3& r);

bool operator==(const Graphics::Vec3& l, const Graphics::Vec3& r);
bool operator!=(const Graphics::Vec3& l, const Graphics::Vec3& r);