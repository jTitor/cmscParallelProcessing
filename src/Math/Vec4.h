#pragma once
#include "BasicTypedefs.h"
#include <string>
#include "Vec3.h"
#define VEC4_NUM_COMPONENTS 4
using namespace std;
namespace Graphics
{
	//Represents a 3D vector.
	class Vec4
	{
	private:
		void init(ScalarT pX, ScalarT pY, ScalarT pZ, ScalarT pW);
		ScalarT components[VEC4_NUM_COMPONENTS];
	public:
		//Returns the zero vector.
		static Vec4 Zero();

		static ScalarT SqrLen(const Vec4& l, const Vec4& r);
		static ScalarT Len(const Vec4& l, const Vec4& r);
		static ScalarT Dot(const Vec4& l, const Vec4& r);
		//Returns a normalized version of the given vector,
		//but does NOT modify the vector.
		static Vec4 GetNormalized(const Vec4& v);

		Vec4();
		Vec4(const Vec4& copy);
		Vec4(const Vec3& v, ScalarT pW);
		Vec4(ScalarT pX, ScalarT pY, ScalarT pZ, ScalarT pW);

		ScalarT& X();
		ScalarT& Y();
		ScalarT& Z();
		ScalarT& W();
		const ScalarT& GetX() const;
		const ScalarT& GetY() const;
		const ScalarT& GetZ() const;
		const ScalarT& GetW() const;
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
		ScalarT Dot(const Vec4& other) const;
		//Normalizes the given vector.
		void Normalize();

		Vec4& operator=(const Vec4& other);
	};
}

//Vector arithmetic.
Graphics::Vec4 operator-(const Graphics::Vec4& v);

Graphics::Vec4& operator+=(Graphics::Vec4& l, const Graphics::Vec4& r);
Graphics::Vec4& operator-=(Graphics::Vec4& l, const Graphics::Vec4& r);
Graphics::Vec4& operator*=(Graphics::Vec4& l, ScalarT r);
Graphics::Vec4& operator/=(Graphics::Vec4& l, ScalarT r);

Graphics::Vec4 operator+(const Graphics::Vec4& l, const Graphics::Vec4& r);
Graphics::Vec4 operator-(const Graphics::Vec4& l, const Graphics::Vec4& r);
Graphics::Vec4 operator*(const Graphics::Vec4& l, ScalarT r);
Graphics::Vec4 operator/(const Graphics::Vec4& l, ScalarT r);

bool operator==(const Graphics::Vec4& l, const Graphics::Vec4& r);
bool operator!=(const Graphics::Vec4& l, const Graphics::Vec4& r);