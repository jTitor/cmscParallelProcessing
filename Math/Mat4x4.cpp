#include "Mat4x4.h"
#include "Mat3x3.h"
#include "Vec4.h"
#include <strstream>
#include <math.h>

using namespace Graphics;

void init(Mat4x4& m, ScalarT p00, ScalarT p01, ScalarT p02, ScalarT p03,
	ScalarT p10, ScalarT p11, ScalarT p12, ScalarT p13,
	ScalarT p20, ScalarT p21, ScalarT p22, ScalarT p23,
	ScalarT p30, ScalarT p31, ScalarT p32, ScalarT p33)
{
	m(0, 0) = p00;
	m(0, 1) = p01;
	m(0, 2) = p02;
	m(0, 3) = p03;
	m(1, 0) = p10;
	m(1, 1) = p11;
	m(1, 2) = p12;
	m(1, 3) = p13;
	m(2, 0) = p20;
	m(2, 1) = p21;
	m(2, 2) = p22;
	m(2, 3) = p23;
	m(3, 0) = p30;
	m(3, 1) = p31;
	m(3, 2) = p32;
	m(3, 3) = p33;
}

Mat4x4 Mat4x4::Zero()
{
	return Mat4x4();
}

Mat4x4 Mat4x4::Identity()
{
	return Mat4x4(	1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);
}

Mat4x4::Mat4x4()
{
	init(	*this,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0);
}

Mat4x4::Mat4x4(ScalarT p00, ScalarT p01, ScalarT p02, ScalarT p03,
	ScalarT p10, ScalarT p11, ScalarT p12, ScalarT p13,
	ScalarT p20, ScalarT p21, ScalarT p22, ScalarT p23,
	ScalarT p30, ScalarT p31, ScalarT p32, ScalarT p33)
{
	init(	*this,
			p00, p01, p02, p03,
			p10, p11, p12, p13,
			p20, p21, p22, p23,
			p30, p31, p32, p33);
}

void writeRow(ostrstream& strStream, Mat4x4& m, unsigned int y)
{
	strStream << "[" << m(y, 0);
	for (int i = 1; i < MAT4X4_WIDTH; ++i)
	{
		strStream << ", " << m(y, i);
	}
	strStream << "]\n";
}

string Mat4x4::ToString()
{
	ostrstream strStream;
	writeRow(strStream, *this, 0);
	for (int i = 1; i < MAT4X4_HEIGHT; ++i)
	{
		strStream << "\n";
		writeRow(strStream, *this, i);
	}
	return strStream.str();
}

const ScalarT* Mat4x4::ToArray() const
{
	return cells;
}

#define MAT4X4_GET_CELL_EXPR(y, x) this->cells[y * MAT4X4_WIDTH + x]
#define MAT4X4_GET_ARRAY_EXPR(idx) this->cells[idx]

ScalarT& Mat4x4::operator()(unsigned int y, unsigned int x)
{
	return MAT4X4_GET_CELL_EXPR(y, x);
}

ScalarT& Mat4x4::operator[](unsigned int idx)
{
	return MAT4X4_GET_ARRAY_EXPR(idx);
}

ScalarT Mat4x4::AtCoordinate(unsigned int y, unsigned int x) const
{
	return MAT4X4_GET_CELL_EXPR(y, x);
}

ScalarT Mat4x4::AtIndex(unsigned int idx) const
{
	return MAT4X4_GET_ARRAY_EXPR(idx);
}

/**
Gets a 3x3 matrix consisting of all cells besides those along the given row or column.
*/
Mat3x3 GetSubmatrix(const Mat4x4& m, int row, int col)
{
	int nextRow = 0;
	int nextColumn = 0;
	Mat3x3 result = Mat3x3::Zero();

	//we will loop through each element of our 4x4;
	//if the current element is valid (neither on the same row as [row] or column as [col])
	//then set Mat3x3(nextRow, nextColumn) and increment nextColumn
	//then increment nextRow
	//the last step might not be necessary
	for (int i = 0; i < MAT4X4_NUM_CELLS; i++)
	{
		//if this row is valid...
		if (i != row)
		{
			//reset nextColumn
			nextColumn = 0;
			for (int j = 0; j < MAT4X4_NUM_CELLS; j++)
			{
				if (j != col)
				{
					result(nextRow, nextColumn) = m.AtCoordinate(i, j);
					nextColumn++;
				}
			}
			//move nextRow down a row
			nextRow++;
		}
	}
	return result;
}

/**
Gets the adjoint matrix: The transpose of the matrix's cofactor.
*/
Mat4x4 Adjoint(const Mat4x4& m)
{
	//Another huge pain!
	//Build a new 4x4 matrix from cofactors.
	//C(i, j) = (-1)^(i+j)*det(submatrix(i,j))
	Mat4x4 result = Mat4x4::Zero();
	for (int i = 0; i < MAT4X4_NUM_CELLS; i++)
	{
		for (int j = 0; j < MAT4X4_NUM_CELLS; j++)
		{
			result(i, j) = pow(-1.0f, i + j) * GetSubmatrix(m, i, j).Determinant();
		}
	}
	result.Transpose();
	return result;
}

ScalarT Mat4x4::Determinant() const
{
	//naming format derived from this matrix:
	//[	a	b	c	d	]
	//[	e	f	g	h	]
	//[	i	j	k	l	]
	//[	m	n	o	p	]
	//Painful Way:
	//calculate subdeterminants
	//
	//so klop =	|	k	l	|
	//			|	o	p	|

	//Easy, But Maybe Slow Way:
	//Take submatrices at a, b, c, and d, then take their determinants
	ScalarT result = 0.0f;
	for (int i = 0; i < MAT4X4_NUM_CELLS; i++)
	{
		result += pow(-1.0f, i) * this->AtCoordinate(0, i) * GetSubmatrix(*this, 0, i).Determinant();
	}
	return result;
}

void Mat4x4::Transpose()
{
	*this = Mat4x4::GetTransposed(*this);
}

Mat4x4 GetInverted(const Mat4x4& m)
{
	//Cramer's rule: inverse = adjoint / determinant.
	ScalarT det = m.Determinant();

	//If matrix can't be inverted, return a zero matrix.
	if (det == 0.0f)
	{
		return Mat4x4::Zero();
	}

	//Otherwise, calculate the inverse.
	return Adjoint(m) / det;
}

Mat4x4 Mat4x4::GetTransposed(const Mat4x4& m)
{
	Mat4x4 result = Mat4x4();
	for (int i = 0; i < MAT4X4_NUM_CELLS; ++i)
	{
		int x = i % MAT4X4_WIDTH;
		int y = i / MAT4X4_WIDTH;

		result(y, x) = m.AtCoordinate(x, y);
	}
	return result;
}

Graphics::Mat4x4 operator-(const Graphics::Mat4x4& v)
{
	Mat4x4 result = v;
	for (int i = 0; i < MAT4X4_NUM_CELLS; ++i)
	{
		result[i] = -result[i];
	}
	return result;
}


Graphics::Mat4x4& operator+=(Graphics::Mat4x4& l, const Graphics::Mat4x4& r)
{
	for (int i = 0; i < MAT4X4_NUM_CELLS; ++i)
	{
		l[i] += r.AtIndex(i);
	}
	
	return l;
}

Graphics::Mat4x4& operator-=(Graphics::Mat4x4& l, const Graphics::Mat4x4& r)
{
	return operator+=(l, -r);
}

Graphics::Mat4x4& operator*=(Graphics::Mat4x4& l, const Graphics::Mat4x4& r)
{
	l = operator*(l, r);
	return l;
}

Graphics::Mat4x4& operator*=(Graphics::Mat4x4& l, ScalarT r)
{
	for (int i = 0; i < MAT4X4_NUM_CELLS; ++i)
	{
		l[i] *= r;
	}

	return l;
}

Graphics::Mat4x4& operator/=(Graphics::Mat4x4& l, ScalarT r)
{
	return operator*=(l, 1.0f / r);
}


Graphics::Mat4x4 operator+(const Graphics::Mat4x4& l, const Graphics::Mat4x4& r)
{
	Mat4x4 result = l;
	result += r;
	return result;
}

Graphics::Mat4x4 operator-(const Graphics::Mat4x4& l, const Graphics::Mat4x4& r)
{
	return operator+(l, -r);
}

Graphics::Mat4x4 operator*(const Graphics::Mat4x4& l, const Graphics::Mat4x4& r)
{
	Mat4x4 result = l;
	//Iterate by row and column.
	for (int i = 0; i < MAT4X4_HEIGHT; ++i)
	{
		for (int j = 0; j < MAT4X4_WIDTH; ++j)
		{
			ScalarT sum = 0;
			for (int k = 0; k < MAT4X4_WIDTH; ++k)
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

Graphics::Vec4 operator*(const Graphics::Mat4x4& l, const Graphics::Vec4& r)
{
	Vec4 result = Vec4();
	for (int i = 0; i < VEC4_NUM_COMPONENTS; ++i)
	{
		ScalarT component = 0;
		for (int j = 0; j < MAT4X4_WIDTH; ++j)
		{
			component += r.AtIndex(j) * l.AtCoordinate(j, i);
		}
		result.AtIndex(i) = component;
	}
	return result;
}

Graphics::Mat4x4 operator*(const Graphics::Mat4x4& l, ScalarT r)
{
	Mat4x4 result = l;
	result *= r;
	return result;
}

Graphics::Mat4x4 operator/(const Graphics::Mat4x4& l, ScalarT r)
{
	return operator*(l, 1.0f / r);
}

bool operator==(const Graphics::Mat4x4& l, const Graphics::Mat4x4& r)
{
	for (int i = 0; i < MAT4X4_NUM_CELLS; ++i)
	{
		if (!GRAPHICS_SCALAR_EQUALITY(l.AtIndex(i), r.AtIndex(i)))
		{
			return false;
		}
	}
	return true;
}

bool operator!=(const Graphics::Mat4x4& l, const Graphics::Mat4x4& r)
{
	return !operator==(l, r);
}
