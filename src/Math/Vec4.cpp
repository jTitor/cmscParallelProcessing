#include "Vec4.h"
#include "MathHelpers.h"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <sstream>

using namespace Graphics;
static Vec4 zero = Vec4();

void Vec4::init(ScalarT pX, ScalarT pY, ScalarT pZ, ScalarT pW)
{
	this->X() = pX;
	this->Y() = pY;
	this->Z() = pZ;
	this->W() = pW;
}

//Returns the zero vector.
Vec4 Vec4::Zero()
{
	return zero;
}

ScalarT Vec4::SqrLen(const Vec4& l, const Vec4& r)
{
	Vec4 temp = r - l;
	return temp.SqrMag();
}

ScalarT Vec4::Len(const Vec4& l, const Vec4& r)
{
	return sqrtf(SqrLen(l, r));
}

ScalarT Vec4::Dot(const Vec4& l, const Vec4& r)
{
	Vec4 temp = l;
	return temp.Dot(r);
}

//Returns a normalized version of the given vector,
//but does NOT modify the vector.
Vec4 Vec4::GetNormalized(const Vec4& v)
{
	Vec4 result = v;
	result.Normalize();
	return result;
}

Vec4::Vec4()
{
	init(0, 0, 0, 0);
}

Vec4::Vec4(ScalarT pX, ScalarT pY, ScalarT pZ, ScalarT pW)
{
	init(pX, pY, pZ, pW);
}

Vec4::Vec4(const Vec3& v, ScalarT pW)
{
	init(v.GetX(), v.GetY(), v.GetZ(), pW);
}

Vec4::Vec4(const Vec4& copy)
{
	init(copy.GetX(), copy.GetY(), copy.GetZ(), copy.GetW());
}

ScalarT& Vec4::X()
{
	return const_cast<ScalarT&>(GetX());
}

ScalarT& Vec4::Y()
{
	return const_cast<ScalarT&>(GetY());
}

ScalarT& Vec4::Z()
{
	return const_cast<ScalarT&>(GetZ());
}

ScalarT& Vec4::W()
{
	return const_cast<ScalarT&>(GetW());
}

const ScalarT& Vec4::GetX() const
{
	return AtIndex(0);
}

const ScalarT& Vec4::GetY() const
{
	return AtIndex(1);
}

const ScalarT& Vec4::GetZ() const
{
	return AtIndex(2);
}

const ScalarT& Vec4::GetW() const
{
	return AtIndex(3);
}

ScalarT& Vec4::AtIndex(unsigned int idx)
{
	return components[idx];
}

const ScalarT& Vec4::AtIndex(unsigned int idx) const
{
	return components[idx];
}


//Operations.
string Vec4::ToString() const
{
	ostringstream strStream;
	strStream << "<" << this->GetX() << ", " << this->GetY() << ", " << this->GetZ() << "," << this->GetW() << ">";
	return strStream.str();
}

const ScalarT* Vec4::ToArray() const
{
	return components;
}

//Gets the squared magnitude of a given vector.
ScalarT Vec4::SqrMag() const
{
	return this->Dot(*this);
}

//Gets the magnitude of a given vector.
ScalarT Vec4::Mag() const
{
	return sqrtf(this->SqrMag());
}

//Gets the dot product of two vectors.
ScalarT Vec4::Dot(const Vec4& other) const
{
	ScalarT result = 0;
	for (int i = 0; i < VEC4_NUM_COMPONENTS; ++i)
	{
		result += this->AtIndex(i) * other.AtIndex(i);
	}
	return result;
}

//Normalizes the given vector.
void Vec4::Normalize()
{
	(*this) /= this->Mag();
}

Vec4& Vec4::operator=(const Vec4& other)
{
	if (this != &other)
	{
		init(other.GetX(), other.GetY(), other.GetZ(), other.GetW());
	}
	return *this;
}

//Vector arithmetic.
Vec4 operator-(const Vec4& v)
{
	Vec4 result = Vec4();
	for (int i = 0; i < VEC4_NUM_COMPONENTS; ++i)
	{
		result.AtIndex(i) = -v.AtIndex(i);
	}
	return result;
}


Vec4& operator+=(Vec4& l, const Vec4& r)
{
	for (int i = 0; i < VEC4_NUM_COMPONENTS; ++i)
	{
		l.AtIndex(i) += r.AtIndex(i);
	}
	return l;
}

Vec4& operator-=(Vec4& l, const Vec4& r)
{
	return operator+=(l, -r);
}

Vec4& operator*=(Vec4& l, ScalarT r)
{
	for (int i = 0; i < VEC4_NUM_COMPONENTS; ++i)
	{
		l.AtIndex(i) *= r;
	}
	return l;
}

Vec4& operator/=(Vec4& l, ScalarT r)
{
	return operator*=(l, 1.0f / r);
}


Vec4 operator+(const Vec4& l, const Vec4& r)
{
	Vec4 result = l;
	result += r;

	return result;
}

Vec4 operator-(const Vec4& l, const Vec4& r)
{
	return operator+(l, -r);
}

Vec4 operator*(const Vec4& l, ScalarT r)
{
	Vec4 result = l;
	result *= r;
	return result;
}

Vec4 operator/(const Vec4& l, ScalarT r)
{
	return operator*(l, 1.0f / r);
}

bool operator==(const Vec4& l, const Vec4& r)
{
	for (int i = 0; i < VEC4_NUM_COMPONENTS; ++i)
	{
		if (!GRAPHICS_SCALAR_EQUALITY(l.AtIndex(i), r.AtIndex(i)))
		{
			return false;
		}
	}
	return true;
}

bool operator!=(const Vec4& l, const Vec4& r)
{
	return !(l == r);
}