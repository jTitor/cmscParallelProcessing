#pragma once
#include "BasicTypedefs.h"
#include <string>

#define MAT3X3_WIDTH 3
#define MAT3X3_HEIGHT 3
#define MAT3X3_NUM_CELLS MAT3X3_WIDTH * MAT3X3_HEIGHT

using namespace std;
namespace Graphics
{
	//Represents a 3x3 matrix.
	//Only really used for Mat4x4's inversion.
	class Mat3x3
	{
	private:
		ScalarT cells[MAT3X3_NUM_CELLS];
	public:
		static Mat3x3 Zero();
		static Mat3x3 Identity();

		Mat3x3();
		Mat3x3(	ScalarT p00, ScalarT p01, ScalarT p02,
				ScalarT p10, ScalarT p11, ScalarT p12,
				ScalarT p20, ScalarT p21, ScalarT p22);

		string ToString();
		const ScalarT* ToArray() const;

		/**
		Cell access.
		Remarks:
			This is 0-indexed.
			No error checking is done; if x or y are under 0 or over 2,
			this will cause an invalid access.
		*/
		ScalarT& operator()(unsigned int y, unsigned int x);
		/**
		Array-like access.
		Remarks:
			This is 0-indexed.
			No error checking is done; if idx is over 8,
			this will cause an invalid access.
		*/
		ScalarT& operator[](unsigned int idx);
		/**
		Constant cell access.
		Remarks:
		This is 0-indexed.
		No error checking is done; if x or y are under 0 or over 2,
		this will cause an invalid access.
		*/
		ScalarT AtCoordinate(unsigned int y, unsigned int x) const;
		/**
		Constant array-like access.
		Remarks:
		This is 0-indexed.
		No error checking is done; if idx is over 8,
		this will cause an invalid access.
		*/
		ScalarT AtIndex(unsigned int idx) const;
		/**
		Gets the determinant of the matrix.
		*/
		ScalarT Determinant() const;
		/**
		Gets the inverse of the given matrix.
		There is no non-static version, since the matrix may not be invertible.
		*/
		//static Mat3x3 GetInverted(const Mat3x3& m);
	};
}

Graphics::Mat3x3 operator-(const Graphics::Mat3x3& v);

Graphics::Mat3x3& operator+=(Graphics::Mat3x3& l, const Graphics::Mat3x3& r);
Graphics::Mat3x3& operator-=(Graphics::Mat3x3& l, const Graphics::Mat3x3& r);
Graphics::Mat3x3& operator*=(Graphics::Mat3x3& l, const Graphics::Mat3x3& r);
Graphics::Mat3x3& operator*=(Graphics::Mat3x3& l, ScalarT r);
Graphics::Mat3x3& operator/=(Graphics::Mat3x3& l, ScalarT r);

Graphics::Mat3x3 operator+(const Graphics::Mat3x3& l, const Graphics::Mat3x3& r);
Graphics::Mat3x3 operator-(const Graphics::Mat3x3& l, const Graphics::Mat3x3& r);
Graphics::Mat3x3 operator*(const Graphics::Mat3x3& l, const Graphics::Mat3x3& r);
Graphics::Mat3x3 operator*(const Graphics::Mat3x3& l, ScalarT r);
Graphics::Mat3x3 operator/(const Graphics::Mat3x3& l, ScalarT r);

bool operator==(const Graphics::Mat3x3& l, const Graphics::Mat3x3& r);
bool operator!=(const Graphics::Mat3x3& l, const Graphics::Mat3x3& r);