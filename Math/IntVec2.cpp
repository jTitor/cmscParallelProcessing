#include "IntVec2.h"
#include "MathHelpers.h"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <sstream>

using namespace Graphics;
static IntVec2 zero = IntVec2();

void IntVec2::init(ptrdiff_t pX, ptrdiff_t pY)
{
	this->X() = pX;
	this->Y() = pY;
}

//Returns the zero vector.
IntVec2 IntVec2::Zero()
{
	return zero;
}

ptrdiff_t IntVec2::SqrLen(const IntVec2& l, const IntVec2& r)
{
	IntVec2 temp = r - l;
	return temp.SqrMag();
}

ptrdiff_t IntVec2::Len(const IntVec2& l, const IntVec2& r)
{
	return sqrtf(SqrLen(l, r));
}

ptrdiff_t IntVec2::Dot(const IntVec2& l, const IntVec2& r)
{
	IntVec2 temp = l;
	return temp.Dot(r);
}

//Returns a normalized version of the given vector,
//but does NOT modify the vector.
IntVec2 IntVec2::GetNormalized(const IntVec2& v)
{
	IntVec2 result = v;
	result.Normalize();
	return result;
}

IntVec2::IntVec2()
{
	init(0, 0);
}

IntVec2::IntVec2(ptrdiff_t pX, ptrdiff_t pY)
{
	init(pX, pY);
}

IntVec2::IntVec2(const IntVec2& copy)
{
	init(copy.GetX(), copy.GetY());
}

ptrdiff_t& IntVec2::X()
{
	return const_cast<ptrdiff_t&>(GetX());
}

ptrdiff_t& IntVec2::Y()
{
	return const_cast<ptrdiff_t&>(GetY());
}

const ptrdiff_t& IntVec2::GetX() const
{
	return AtIndex(0);
}

const ptrdiff_t& IntVec2::GetY() const
{
	return AtIndex(1);
}

ptrdiff_t& IntVec2::AtIndex(unsigned int idx)
{
	return components[idx];
}

const ptrdiff_t& IntVec2::AtIndex(unsigned int idx) const
{
	return components[idx];
}


//Operations.
string IntVec2::ToString() const
{
	ostringstream strStream;
	strStream << "<" << this->GetX() << ", " << this->GetY() << ">";
	return strStream.str();
}

const ptrdiff_t* IntVec2::ToArray() const
{
	return components;
}

//Gets the squared magnitude of a given vector.
ptrdiff_t IntVec2::SqrMag() const
{
	return this->Dot(*this);
}

//Gets the magnitude of a given vector.
ptrdiff_t IntVec2::Mag() const
{
	return sqrtf(this->SqrMag());
}

//Gets the dot product of two vectors.
ptrdiff_t IntVec2::Dot(const IntVec2& other) const
{
	ptrdiff_t result = 0;
	for (int i = 0; i < VEC2_NUM_COMPONENTS; ++i)
	{
		result += this->AtIndex(i) * other.AtIndex(i);
	}
	return result;
}

//Normalizes the given vector.
void IntVec2::Normalize()
{
	(*this) /= this->Mag();
}

IntVec2& IntVec2::operator=(const IntVec2& other)
{
	if (this != &other)
	{
		init(other.GetX(), other.GetY());
	}
	return *this;
}

//Vector arithmetic.
IntVec2 operator-(const IntVec2& v)
{
	IntVec2 result = IntVec2();
	for (int i = 0; i < VEC2_NUM_COMPONENTS; ++i)
	{
		result.AtIndex(i) = -v.AtIndex(i);
	}
	return result;
}


IntVec2& operator+=(IntVec2& l, const IntVec2& r)
{
	for (int i = 0; i < VEC2_NUM_COMPONENTS; ++i)
	{
		l.AtIndex(i) += r.AtIndex(i);
	}
	return l;
}

IntVec2& operator-=(IntVec2& l, const IntVec2& r)
{
	return operator+=(l, -r);
}

IntVec2& operator*=(IntVec2& l, ptrdiff_t r)
{
	for (int i = 0; i < VEC2_NUM_COMPONENTS; ++i)
	{
		l.AtIndex(i) *= r;
	}
	return l;
}

IntVec2& operator*=(ptrdiff_t r,IntVec2& l)
{
	return operator*=(l, r);
}

IntVec2& operator/=(IntVec2& l, ptrdiff_t r)
{
	return operator*=(l, 1.0f / r);
}

IntVec2& operator/=(ptrdiff_t r, IntVec2& l)
{
	return operator/=(l, r);
}

IntVec2 operator+(const IntVec2& l, const IntVec2& r)
{
	IntVec2 result = l;
	result += r;

	return result;
}

IntVec2 operator-(const IntVec2& l, const IntVec2& r)
{
	return operator+(l, -r);
}

IntVec2 operator*(const IntVec2& l, ptrdiff_t r)
{
	IntVec2 result = l;
	result *= r;
	return result;
}

IntVec2 operator/(const IntVec2& l, ptrdiff_t r)
{
	return operator*(l, 1.0f / r);
}

bool operator==(const IntVec2& l, const IntVec2& r)
{
	for (int i = 0; i < VEC2_NUM_COMPONENTS; ++i)
	{
		if (l.AtIndex(i) != r.AtIndex(i))
		{
			return false;
		}
	}
	return true;
}

bool operator!=(const IntVec2& l, const IntVec2& r)
{
	return !(l == r);
}