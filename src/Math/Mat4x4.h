#pragma once
#include "Vec4.h"
#include <string>
#define MAT4X4_WIDTH 4
#define MAT4X4_HEIGHT 4
#define MAT4X4_NUM_CELLS MAT4X4_WIDTH * MAT4X4_HEIGHT

using namespace std;
namespace Graphics
{
	//Represents a 4x4 matrix.
	class Mat4x4
	{
	private:
		ScalarT cells[MAT4X4_NUM_CELLS];
	public:
		static Mat4x4 Zero();
		static Mat4x4 Identity();

		Mat4x4();
		Mat4x4(	ScalarT p00, ScalarT p01, ScalarT p02, ScalarT p03,
				ScalarT p10, ScalarT p11, ScalarT p12, ScalarT p13,
				ScalarT p20, ScalarT p21, ScalarT p22, ScalarT p23,
				ScalarT p30, ScalarT p31, ScalarT p32, ScalarT p33);

		string ToString();
		const ScalarT* ToArray() const;

		/**
		Cell access.
		Remarks:
			This is 0-indexed.
			No error checking is done; if x or y are under 0 or over 3,
			this will cause an invalid access.
		*/
		ScalarT& operator()(unsigned int y, unsigned int x);
		
		/**
		Array-like access.
		Remarks:
			This is 0-indexed.
			No error checking is done; if idx is over 15,
			this will cause an invalid access.
		*/
		ScalarT& operator[](unsigned int idx);
		
		/**
		Constant cell access.
		Remarks:
		This is 0-indexed.
		No error checking is done; if x or y are under 0 or over 3,
		this will cause an invalid access.
		*/
		ScalarT AtCoordinate(unsigned int y, unsigned int x) const;

		/**
		Constant array-like access.
		Remarks:
		This is 0-indexed.
		No error checking is done; if idx is over 15,
		this will cause an invalid access.
		*/
		ScalarT AtIndex(unsigned int idx) const;

		/**
		Gets the determinant of the matrix.
		*/
		ScalarT Determinant() const;

		/**
		Transposes the matrix.
		*/
		void Transpose();

		/**
		Gets the transpose of the given matrix.
		*/
		static Mat4x4 GetTransposed(const Mat4x4& m);
	};
}

Graphics::Mat4x4 operator-(const Graphics::Mat4x4& v);

Graphics::Mat4x4& operator+=(Graphics::Mat4x4& l, const Graphics::Mat4x4& r);
Graphics::Mat4x4& operator-=(Graphics::Mat4x4& l, const Graphics::Mat4x4& r);
Graphics::Mat4x4& operator*=(Graphics::Mat4x4& l, const Graphics::Mat4x4& r);
Graphics::Mat4x4& operator*=(Graphics::Mat4x4& l, ScalarT r);
Graphics::Mat4x4& operator/=(Graphics::Mat4x4& l, ScalarT r);

Graphics::Mat4x4 operator+(const Graphics::Mat4x4& l, const Graphics::Mat4x4& r);
Graphics::Mat4x4 operator-(const Graphics::Mat4x4& l, const Graphics::Mat4x4& r);
Graphics::Mat4x4 operator*(const Graphics::Mat4x4& l, const Graphics::Mat4x4& r);
Graphics::Mat4x4 operator*(const Graphics::Mat4x4& l, ScalarT r);
Graphics::Vec4 operator*(const Graphics::Mat4x4& l, const Graphics::Vec4& r);
Graphics::Mat4x4 operator/(const Graphics::Mat4x4& l, ScalarT r);

bool operator==(const Graphics::Mat4x4& l, const Graphics::Mat4x4& r);
bool operator!=(const Graphics::Mat4x4& l, const Graphics::Mat4x4& r);