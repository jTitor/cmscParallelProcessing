#include "Vec3.h"
#include "MathHelpers.h"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <sstream>

using namespace Graphics;

void Vec3::init(ScalarT pX, ScalarT pY, ScalarT pZ)
{
	this->X() = pX;
	this->Y() = pY;
	this->Z() = pZ;
}

#pragma region Static Methods
//Returns the zero vector.
Vec3 Vec3::Zero()
{
	return Vec3(0, 0, 0);
}

Vec3 Vec3::One()
{
	return Vec3(1, 1, 1);
}

Vec3 Vec3::AllMax()
{
	return Vec3(GRAPHICS_SCALAR_MAX, GRAPHICS_SCALAR_MAX, GRAPHICS_SCALAR_MAX);
}

Vec3 Vec3::AllMin()
{
	return Vec3(GRAPHICS_SCALAR_MIN, GRAPHICS_SCALAR_MIN, GRAPHICS_SCALAR_MIN);
}

ScalarT Vec3::SqrLen(const Vec3& l, const Vec3& r)
{
	Vec3 temp = r - l;
	return temp.SqrMag();
}

ScalarT Vec3::Len(const Vec3& l, const Vec3& r)
{
	return sqrtf(SqrLen(l, r));
}

ScalarT Vec3::Dot(const Vec3& l, const Vec3& r)
{
	Vec3 temp = l;
	return temp.Dot(r);
}

Vec3 Vec3::Cross(const Vec3& l, const Vec3& r)
{
	Vec3 temp = l;
	return temp.Cross(r);
}

//Returns a normalized version of the given vector,
//but does NOT modify the vector.
Vec3 Vec3::GetNormalized(const Vec3& v)
{
	Vec3 result = v;
	result.Normalize();
	return result;
}

Vec3 Vec3::Reciprocal(const Vec3& v)
{
	Vec3 result = Vec3();
	result.X() = 1.0f / v.GetX();
	result.Y() = 1.0f / v.GetY();
	result.Z() = 1.0f / v.GetZ();

	return result;
}

Vec3 Vec3::ComponentWiseMultiply(const Vec3& a, const Vec3& b)
{
	Vec3 result = a;
	result.X() *= b.GetX();
	result.Y() *= b.GetY();
	result.Z() *= b.GetZ();

	return result;
}

Vec3 Vec3::ComponentWiseDivide(const Vec3& a, const Vec3& b)
{
	return Vec3::ComponentWiseMultiply(a, Vec3::Reciprocal(b));
}

Vec3 Vec3::ComponentWiseMax(const Vec3& clampee, const Vec3& clamper)
{
	Vec3 result = clampee;
	for (int i = 0; i < VEC3_NUM_COMPONENTS; ++i)
	{
		result.AtIndex(i) = fmaxf(result.AtIndex(i), clamper.AtIndex(i));
	}

	return result;
}

Vec3 Vec3::ComponentWiseMin(const Vec3& clampee, const Vec3& clamper)
{
	Vec3 result = clampee;
	for (int i = 0; i < VEC3_NUM_COMPONENTS; ++i)
	{
		result.AtIndex(i) = fminf(result.AtIndex(i), clamper.AtIndex(i));
	}

	return result;
}

Vec3 Vec3::ComponentWiseClamp(const Vec3& clampee, const Vec3& clamperMin, const Vec3& clamperMax)
{
	Vec3 result = clampee;
	for (int i = 0; i < VEC3_NUM_COMPONENTS; ++i)
	{
		result.AtIndex(i) = fmaxf(fminf(result.AtIndex(i), clamperMax.AtIndex(i)), clamperMin.AtIndex(i));
	}

	return result;
}

Vec3 Vec3::ComponentWiseAbs(const Vec3& v)
{
	Vec3 result = v;
	for (int i = 0; i < VEC3_NUM_COMPONENTS; ++i)
	{
		result.AtIndex(i) = fabsf(result.AtIndex(i));
	}

	return result;
}
#pragma endregion

Vec3::Vec3()
{
	init(0, 0, 0);
}

Vec3::Vec3(ScalarT pX, ScalarT pY, ScalarT pZ)
{
	init(pX, pY, pZ);
}

Vec3::Vec3(const Vec3& copy)
{
	init(copy.GetX(), copy.GetY(), copy.GetZ());
}

ScalarT& Vec3::X()
{
	return const_cast<ScalarT&>(GetX());
}

ScalarT& Vec3::Y()
{
	return const_cast<ScalarT&>(GetY());
}

ScalarT& Vec3::Z()
{
	return const_cast<ScalarT&>(GetZ());
}

const ScalarT& Vec3::GetX() const
{
	return AtIndex(0);
}

const ScalarT& Vec3::GetY() const
{
	return AtIndex(1);
}

const ScalarT& Vec3::GetZ() const
{
	return AtIndex(2);
}

ScalarT& Vec3::AtIndex(unsigned int idx)
{
	return components[idx];
}

const ScalarT& Vec3::AtIndex(unsigned int idx) const
{
	return components[idx];
}


//Operations.
string Vec3::ToString() const
{
	ostringstream strStream;
	strStream << "<" << this->GetX() << ", " << this->GetY() << ", " << this->GetZ() << ">";
	return strStream.str();
}

const ScalarT* Vec3::ToArray() const
{
	return components;
}

//Gets the squared magnitude of a given vector.
ScalarT Vec3::SqrMag() const
{
	return this->Dot(*this);
}

//Gets the magnitude of a given vector.
ScalarT Vec3::Mag() const
{
	return sqrtf(this->SqrMag());
}

//Gets the dot product of two vectors.
ScalarT Vec3::Dot(const Vec3& other) const
{
	ScalarT result = 0;
	for (int i = 0; i < VEC3_NUM_COMPONENTS; ++i)
	{
		result += this->AtIndex(i) * other.AtIndex(i);
	}
	return result;
}

//Gets the cross product of two vectors.
Vec3 Vec3::Cross(const Vec3& other) const
{
	Vec3 result = Zero();

	result.X() = this->GetY()*other.GetZ() - this->GetZ()*other.GetY();
	result.Y() = this->GetZ()*other.GetX() - this->GetX()*other.GetZ();
	result.Z() = this->GetX()*other.GetY() - this->GetY()*other.GetX();

	return result;
}

//Normalizes the given vector.
void Vec3::Normalize()
{
	//The zero vector normalizes to the zero vector.
	ScalarT mag = this->Mag();
	if (mag <= 0)
	{
		return;
	}
	(*this) /= mag;
}

Vec3& Vec3::operator=(const Vec3& other)
{
	if (this != &other)
	{
		init(other.GetX(), other.GetY(), other.GetZ());
	}
	return *this;
}

//Vector arithmetic.
Vec3 operator-(const Vec3& v)
{
	Vec3 result = Vec3();
	for (int i = 0; i < VEC3_NUM_COMPONENTS; ++i)
	{
		result.AtIndex(i) = -v.AtIndex(i);
	}
	return result;
}


Vec3& operator+=(Vec3& l, const Vec3& r)
{
	for (int i = 0; i < VEC3_NUM_COMPONENTS; ++i)
	{
		l.AtIndex(i) += r.AtIndex(i);
	}
	return l;
}

Vec3& operator-=(Vec3& l, const Vec3& r)
{
	return operator+=(l, -r);
}

Vec3& operator*=(Vec3& l, ScalarT r)
{
	for (int i = 0; i < VEC3_NUM_COMPONENTS; ++i)
	{
		l.AtIndex(i) *= r;
	}
	return l;
}

Vec3& operator/=(Vec3& l, ScalarT r)
{
	return operator*=(l, 1.0f / r);
}

Vec3& operator*=(ScalarT l, Vec3& r)
{
	return operator*=(r, l);
}

Vec3 operator+(const Vec3& l, const Vec3& r)
{
	Vec3 result = l;
	result += r;

	return result;
}

Vec3 operator-(const Vec3& l, const Vec3& r)
{
	return operator+(l, -r);
}

Vec3 operator*(const Vec3& l, ScalarT r)
{
	Vec3 result = l;
	result *= r;
	return result;
}

Vec3 operator/(const Vec3& l, ScalarT r)
{
	return operator*(l, 1.0f / r);
}

Vec3 operator*(ScalarT l, const Vec3& r)
{
	return operator*(r, l);
}

bool operator==(const Vec3& l, const Vec3& r)
{
	for (int i = 0; i < VEC3_NUM_COMPONENTS; ++i)
	{
		if (!GRAPHICS_SCALAR_EQUALITY(l.AtIndex(i), r.AtIndex(i)))
		{
			return false;
		}
	}
	return true;
}

bool operator!=(const Vec3& l, const Vec3& r)
{
	return !(l == r);
}

bool Graphics::TestVec3()
{
	//All operations better work properly.
	return false;
}