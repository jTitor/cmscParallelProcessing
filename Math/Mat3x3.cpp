#include "Mat3x3.h"
#include <strstream>

using namespace Graphics;

void init(	Mat3x3& m,
			ScalarT p00, ScalarT p01, ScalarT p02,
			ScalarT p10, ScalarT p11, ScalarT p12,
			ScalarT p20, ScalarT p21, ScalarT p22)
{
	m(0, 0) = p00;
	m(0, 1) = p01;
	m(0, 2) = p02;
	m(1, 0) = p10;
	m(1, 1) = p11;
	m(1, 2) = p12;
	m(2, 0) = p20;
	m(2, 1) = p21;
	m(2, 2) = p22;
}

Mat3x3 Mat3x3::Zero()
{
	return Mat3x3();
}

Mat3x3 Mat3x3::Identity()
{
	return Mat3x3(	1, 0, 0,
					0, 1, 0,
					0, 0, 1);
}

Mat3x3::Mat3x3()
{
	init(	*this,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0);
}

Mat3x3::Mat3x3(	ScalarT p00, ScalarT p01, ScalarT p02,
				ScalarT p10, ScalarT p11, ScalarT p12,
				ScalarT p20, ScalarT p21, ScalarT p22)
{
	init(	*this,
			p00, p01, p02,
			p10, p11, p12,
			p20, p21, p22);
}

void writeRow(ostrstream& strStream, Mat3x3& m, unsigned int y)
{
	strStream << "[" << m(y, 0);
	for (int i = 1; i < MAT3X3_WIDTH; ++i)
	{
		strStream << ", " << m(y, i);
	}
	strStream << "]\n";
}

string Mat3x3::ToString()
{
	ostrstream strStream;
	writeRow(strStream, *this, 0);
	for (int i = 1; i < MAT3X3_HEIGHT; ++i)
	{
		strStream << "\n";
		writeRow(strStream, *this, i);
	}
	return strStream.str();
}

const ScalarT* Mat3x3::ToArray() const
{
	return cells;
}

#define MAT3X3_GET_CELL_EXPR(y, x) this->cells[y * MAT3X3_WIDTH + x]
#define MAT3X3_GET_ARRAY_EXPR(idx) this->cells[idx]

ScalarT& Mat3x3::operator()(unsigned int y, unsigned int x)
{
	return MAT3X3_GET_CELL_EXPR(y, x);
}

ScalarT& Mat3x3::operator[](unsigned int idx)
{
	return MAT3X3_GET_ARRAY_EXPR(idx);
}

ScalarT Mat3x3::AtCoordinate(unsigned int y, unsigned int x) const
{
	return MAT3X3_GET_CELL_EXPR(y, x);
}

ScalarT Mat3x3::AtIndex(unsigned int idx) const
{
	return MAT3X3_GET_ARRAY_EXPR(idx);
}

ScalarT Mat3x3::Determinant() const
{
	//Ugly, but it's 3 AM and I don't want to make a loop for this
	return	this->AtCoordinate(0, 0)*(this->AtCoordinate(1, 1) * this->AtCoordinate(2, 2) - this->AtCoordinate(1, 2) * this->AtCoordinate(2, 1)) -
			this->AtCoordinate(0, 1)*(this->AtCoordinate(1, 0) * this->AtCoordinate(2, 2) - this->AtCoordinate(1, 2) * this->AtCoordinate(2, 0)) +
			this->AtCoordinate(0, 2)*(this->AtCoordinate(1, 0) * this->AtCoordinate(2, 1) - this->AtCoordinate(1, 1) * this->AtCoordinate(2, 0));
}

Graphics::Mat3x3 operator-(const Graphics::Mat3x3& v)
{
	Mat3x3 result = v;
	for (int i = 0; i < MAT3X3_NUM_CELLS; ++i)
	{
		result[i] = -result[i];
	}
	return result;
}


Graphics::Mat3x3& operator+=(Graphics::Mat3x3& l, const Graphics::Mat3x3& r)
{
	for (int i = 0; i < MAT3X3_NUM_CELLS; ++i)
	{
		l[i] += r.AtIndex(i);
	}
	
	return l;
}

Graphics::Mat3x3& operator-=(Graphics::Mat3x3& l, const Graphics::Mat3x3& r)
{
	return operator+=(l, -r);
}

Graphics::Mat3x3& operator*=(Graphics::Mat3x3& l, const Graphics::Mat3x3& r)
{
	l = operator*(l, r);
	return l;
}

Graphics::Mat3x3& operator*=(Graphics::Mat3x3& l, ScalarT r)
{
	for (int i = 0; i < MAT3X3_NUM_CELLS; ++i)
	{
		l[i] *= r;
	}

	return l;
}

Graphics::Mat3x3& operator/=(Graphics::Mat3x3& l, ScalarT r)
{
	return operator*=(l, 1.0f / r);
}


Graphics::Mat3x3 operator+(const Graphics::Mat3x3& l, const Graphics::Mat3x3& r)
{
	Mat3x3 result = l;
	result += r;
	return result;
}

Graphics::Mat3x3 operator-(const Graphics::Mat3x3& l, const Graphics::Mat3x3& r)
{
	return operator+(l, -r);
}

Graphics::Mat3x3 operator*(const Graphics::Mat3x3& l, const Graphics::Mat3x3& r)
{
	Mat3x3 result = l;
	//Iterate by row and column.
	for (int i = 0; i < MAT3X3_HEIGHT; ++i)
	{
		for (int j = 0; j < MAT3X3_WIDTH; ++j)
		{
			ScalarT sum = 0;
			for (int k = 0; k < MAT3X3_WIDTH; ++k)
			{
				sum += l.AtCoordinate(i, k) * r.AtCoordinate(k, j);
			}
			//This cell effectively equals the dot product
			//of L's row matrix and R's column matrix.
			result(i, j) = sum;
		}
	}
	return result;
}

Graphics::Mat3x3 operator*(const Graphics::Mat3x3& l, ScalarT r)
{
	Mat3x3 result = l;
	result *= r;
	return result;
}

Graphics::Mat3x3 operator/(const Graphics::Mat3x3& l, ScalarT r)
{
	return operator*(l, 1.0f / r);
}

bool operator==(const Graphics::Mat3x3& l, const Graphics::Mat3x3& r)
{
	for (int i = 0; i < MAT3X3_NUM_CELLS; ++i)
	{
		if (!GRAPHICS_SCALAR_EQUALITY(l.AtIndex(i), r.AtIndex(i)))
		{
			return false;
		}
	}
	return true;
}

bool operator!=(const Graphics::Mat3x3& l, const Graphics::Mat3x3& r)
{
	return !operator==(l, r);
}
