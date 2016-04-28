#pragma once
#include "BasicTypedefs.h"
#include <string>
#include "IntVec2.h"
#define VEC2_NUM_COMPONENTS 2
using namespace std;
namespace Graphics
{
	//Represents a 3D vector.
	class IntVec2
	{
	private:
		void init(ptrdiff_t pX, ptrdiff_t pY);
		ptrdiff_t components[VEC2_NUM_COMPONENTS];
	public:
		//Returns the zero vector.
		static IntVec2 Zero();

		static ptrdiff_t SqrLen(const IntVec2& l, const IntVec2& r);
		static ptrdiff_t Len(const IntVec2& l, const IntVec2& r);
		static ptrdiff_t Dot(const IntVec2& l, const IntVec2& r);
		//Returns a normalized version of the given vector,
		//but does NOT modify the vector.
		static IntVec2 GetNormalized(const IntVec2& v);

		IntVec2();
		IntVec2(const IntVec2& copy);
		IntVec2(ptrdiff_t pX, ptrdiff_t p);

		ptrdiff_t& X();
		ptrdiff_t& Y();
		const ptrdiff_t& GetX() const;
		const ptrdiff_t& GetY() const;
		ptrdiff_t& AtIndex(unsigned int idx);
		const ptrdiff_t& AtIndex(unsigned int idx) const;

		//Operations.
		string ToString() const;
		const ptrdiff_t* ToArray() const;
		//Gets the squared magnitude of a given vector.
		ptrdiff_t SqrMag() const;
		//Gets the magnitude of a given vector.
		ptrdiff_t Mag() const;
		//Gets the dot product of two vectors.
		ptrdiff_t Dot(const IntVec2& other) const;
		//Normalizes the given vector.
		void Normalize();

		IntVec2& operator=(const IntVec2& other);
	};
}

//Vector arithmetic.
Graphics::IntVec2 operator-(const Graphics::IntVec2& v);

Graphics::IntVec2& operator+=(Graphics::IntVec2& l, const Graphics::IntVec2& r);
Graphics::IntVec2& operator-=(Graphics::IntVec2& l, const Graphics::IntVec2& r);
Graphics::IntVec2& operator*=(Graphics::IntVec2& l, ptrdiff_t r);
Graphics::IntVec2& operator*=(ptrdiff_t r, Graphics::IntVec2& l);
Graphics::IntVec2& operator/=(Graphics::IntVec2& l, ptrdiff_t r);
Graphics::IntVec2& operator/=(ptrdiff_t r, Graphics::IntVec2& l);

Graphics::IntVec2 operator+(const Graphics::IntVec2& l, const Graphics::IntVec2& r);
Graphics::IntVec2 operator-(const Graphics::IntVec2& l, const Graphics::IntVec2& r);
Graphics::IntVec2 operator*(const Graphics::IntVec2& l, ptrdiff_t r);
Graphics::IntVec2 operator/(const Graphics::IntVec2& l, ptrdiff_t r);

bool operator==(const Graphics::IntVec2& l, const Graphics::IntVec2& r);
bool operator!=(const Graphics::IntVec2& l, const Graphics::IntVec2& r);