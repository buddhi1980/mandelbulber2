/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * CVector2, CVector3, CVector4 and CMatrix33 classes - vector and matrix
 * manipulation
 *
 * These classes get used throughout the program to realize 2d, 3d and 4d
 * variable representation and manipulation. The class CMatrix33 is used for
 * rotation in 3d space.
 */

#include "algebra.hpp"

// Rotates this CVector3 around an axis by the specified angle.
CVector3 CVector3::RotateAroundVectorByAngle(const CVector3 &axis, double angle) const
{
	// Calculate the component of the original vector times the cosine of the angle
	CVector3 vector = *this * cos(angle);

	// Add the cross times the sine of the angle
	vector += (axis.Cross(*this)) * sin(angle);

	// Add the dot times one minus the cosine of the angle
	vector += axis * axis.Dot(*this) * (1 - cos(angle));

	// Return the newly calculated results
	return vector;
}

CVector4 CVector4::RotateAroundVectorByAngle(const CVector3 &axis, double angle) const
{
	const CVector3 oldVector = GetXYZ();
	CVector3 vector = oldVector * cos(angle);
	vector += (axis.Cross(oldVector)) * sin(angle);
	vector += axis * axis.Dot(oldVector) * (1 - cos(angle));
	return CVector4(vector, w);
}

// Accessing Vector Components by Name (Character)
double CVector3::itemByName(char item) const
{
	// Switch Statement for Character-Based Indexing
	switch (item)
	{
		// If 'x', return the x-component
		case 'x': return x;
		// If 'y', return the y-component
		case 'y': return y;
		// If 'z', return the z-component
		case 'z': return z;
		// Default: Invalid Input
		default: return 0.0;
	}
}

// Accessing Vector4 Components by Name (Character)
double CVector4::itemByName(char item) const
{
	// Switch Statement for Character-Based Indexing
	switch (item)
	{
		// If 'x', return the x-component
		case 'x': return x;
		// If 'y', return the y-component
		case 'y': return y;
		// If 'z', return the z-component
		case 'z': return z;
		// If 'w', return the w-component (unique to 4D vectors)
		case 'w': return w;
		// Default: Invalid Input (return 0.0)
		default: return 0.0;
	}
}

/***************** class CMatrix33 ***********************/
CMatrix33::CMatrix33()
{
	m11 = 0;
	m12 = 0;
	m13 = 0;
	m21 = 0;
	m22 = 0;
	m23 = 0;
	m31 = 0;
	m32 = 0;
	m33 = 0;
}

CMatrix33::CMatrix33(const CMatrix33 &matrix)
{
	m11 = matrix.m11;
	m12 = matrix.m12;
	m13 = matrix.m13;
	m21 = matrix.m21;
	m22 = matrix.m22;
	m23 = matrix.m23;
	m31 = matrix.m31;
	m32 = matrix.m32;
	m33 = matrix.m33;
}

CMatrix33::CMatrix33(const CVector3 &v1, const CVector3 &v2, const CVector3 &v3)
{
	m11 = v1.x;
	m12 = v2.x;
	m13 = v3.x;
	m21 = v1.y;
	m22 = v2.y;
	m23 = v3.y;
	m31 = v1.z;
	m32 = v2.z;
	m33 = v3.z;
}

CMatrix33 &CMatrix33::operator=(const CMatrix33 &matrix) = default;

// Matrix Multiplication Operator Overloading for 3x3 Matrices
CMatrix33 CMatrix33::operator*(const CMatrix33 &matrix) const
{
	// Create a new matrix to store the result
	CMatrix33 result;

	// Calculate each element of the result matrix using matrix multiplication rules
	// Row 1
	result.m11 = m11 * matrix.m11 + m12 * matrix.m21 + m13 * matrix.m31;
	result.m12 = m11 * matrix.m12 + m12 * matrix.m22 + m13 * matrix.m32;
	result.m13 = m11 * matrix.m13 + m12 * matrix.m23 + m13 * matrix.m33;

	// Row 2
	result.m21 = m21 * matrix.m11 + m22 * matrix.m21 + m23 * matrix.m31;
	result.m22 = m21 * matrix.m12 + m22 * matrix.m22 + m23 * matrix.m32;
	result.m23 = m21 * matrix.m13 + m22 * matrix.m23 + m23 * matrix.m33;

	// Row 3
	result.m31 = m31 * matrix.m11 + m32 * matrix.m21 + m33 * matrix.m31;
	result.m32 = m31 * matrix.m12 + m32 * matrix.m22 + m33 * matrix.m32;
	result.m33 = m31 * matrix.m13 + m32 * matrix.m23 + m33 * matrix.m33;

	// Return the calculated result matrix
	return result;
}

// This function overloads the '*' operator for CMatrix33 objects.
CVector3 CMatrix33::operator*(const CVector3 &vector) const
{
	// Create a new CVector3 object to hold the result of the multiplication
	CVector3 result;

	// Calculate the x-component of the result
	result.x = m11 * vector.x + m12 * vector.y + m13 * vector.z;

	// Calculate the y-component of the result
	result.y = m21 * vector.x + m22 * vector.y + m23 * vector.z;

	// Calculate the z-component of the result
	result.z = m31 * vector.x + m32 * vector.y + m33 * vector.z;
	
	// Return the calculated resulting vector
	return result;
}

/**************** class RotationMatrix **********************/
CRotationMatrix::CRotationMatrix()
{
	matrix.m11 = 1.0;
	matrix.m12 = 0.0;
	matrix.m13 = 0.0;
	matrix.m21 = 0.0;
	matrix.m22 = 1.0;
	matrix.m23 = 0.0;
	matrix.m31 = 0.0;
	matrix.m32 = 0.0;
	matrix.m33 = 1.0;
	zero = true;
}

// Rotates this CRotationMatrix around the X-axis by the specified angle
void CRotationMatrix::RotateX(double angle)
{
	// Only perform the rotation if the angle is not zero
	if (angle != 0.0)
	{
		// Create a new temporary rotation matrix 'rot'
		CMatrix33 rot;

		// Pre-calculate sine and cosine of the angle for efficiency
		const double s = sin(angle);
		const double c = cos(angle);

		// Set the elements of the rotation matrix 'rot' for a rotation around the X-axis.
		rot.m11 = 1.0;
		rot.m22 = c;
		rot.m33 = c;
		rot.m23 = -s;
		rot.m32 = s;

		// Multiply the current rotation matrix 'matrix' (member of the CRotationMatrix object)
		matrix = matrix * rot;

		// Set 'zero' to false to indicate that the rotation matrix is no longer an identity matrix
		zero = false;
	}
}

// Rotates this CRotationMatrix around the Y-axis by the specified angle
void CRotationMatrix::RotateY(double angle)
{
	// Check if the rotation angle is non-zero (avoid unnecessary calculations)
	if (angle != 0.0)
	{
		// Create a temporary 3x3 rotation matrix
		CMatrix33 rot;

		// Pre-calculate sine and cosine of the angle for efficiency
		const double s = sin(angle);
		const double c = cos(angle);

		// Set the elements of the rotation matrix according to the standard rotation formula
		rot.m22 = 1.0;
		rot.m33 = c;
		rot.m11 = c;
		rot.m31 = -s;
		rot.m13 = s;

		// Multiply the current rotation matrix (matrix) by the new rotation matrix (rot).
		matrix = matrix * rot;

		// Flag the rotation matrix as non-zero (meaning it represents a rotation)
		zero = false;
	}
}

// Rotates this CRotationMatrix around the Z-axis by the specified angle
void CRotationMatrix::RotateZ(double angle)
{
	// Check if the rotation angle is non-zero (avoid unnecessary calculations)
	if (angle != 0.0)
	{
		// Create a temporary 3x3 rotation matrix
		CMatrix33 rot;

		// Pre-calculate sine and cosine of the angle for efficiency
		const double s = sin(angle);
		const double c = cos(angle);

		// Set the elements of the rotation matrix according to the standard Z-axis rotation formula
		rot.m33 = 1.0;
		rot.m11 = c;
		rot.m22 = c;
		rot.m12 = -s;
		rot.m21 = s;

		// Multiply the current rotation matrix (matrix) by the new rotation matrix (rot)
		matrix = matrix * rot;

		// Flag the rotation matrix as non-zero (meaning it represents a rotation)
		zero = false;
	}
}

void CRotationMatrix::SetRotation(double angles[3])
{
	Null();
	RotateZ(angles[0]);
	RotateX(angles[1]);
	RotateY(angles[2]);
}

void CRotationMatrix::SetRotation(double alpha, double beta, double gamma)
{
	Null();
	RotateZ(alpha);
	RotateX(beta);
	RotateY(gamma);
}

void CRotationMatrix::SetRotation(const CVector3 &rotation)
{
	Null();
	RotateZ(rotation.x);
	RotateX(rotation.y);
	RotateY(rotation.z);
}

// This function constructs a rotation matrix that aligns an object
void CRotationMatrix::SetRotation(const CVector3 &direction, const CVector3 &up)
{
	// Use cross product to find the perpendicular vector
	CVector3 xaxis = up.Cross(direction);

	// Normalize to make it a unit vector (length = 1)
	xaxis.Normalize();

	// Calculate the Y-axis of the rotation matrix
	CVector3 yaxis = direction.Cross(xaxis);

	// Normalize for consistency
	yaxis.Normalize();

	// Assign the calculated X, Y, and Z axes (direction) directly to the rotation matrix
	matrix.m11 = xaxis.x;
	matrix.m12 = xaxis.y;
	matrix.m13 = xaxis.z;

	matrix.m21 = yaxis.x;
	matrix.m22 = yaxis.y;
	matrix.m23 = yaxis.z;

	matrix.m31 = direction.x;
	matrix.m32 = direction.y;
	matrix.m33 = direction.z;

	// Indicate that the matrix now represents a valid rotation
	zero = false;
}

void CRotationMatrix::SetRotation2(const CVector3 &rotation)
{
	Null();
	RotateZ(rotation.z);
	RotateY(rotation.y);
	RotateX(rotation.x);
}

void CRotationMatrix::SetRotation3(const CVector3 &rotation)
{
	Null();
	RotateZ(rotation.x);
	RotateY(rotation.y);
	RotateX(rotation.z);
}

void CRotationMatrix::SetRotation4(const CVector3 &rotation)
{
	Null();
	RotateX(rotation.x);
	RotateY(rotation.y);
	RotateZ(rotation.z);
}

// Rotates a given 3D vector by this CRotationMatrix
CVector3 CRotationMatrix::RotateVector(const CVector3 &vector) const
{
	// Check if the rotation matrix is a non-zero rotation (not identity)
	if (!zero)
	{
		// Perform the rotation by multiplying the matrix with the vector
		CVector3 vector2 = matrix * vector;

		// Return the rotated vector
		return vector2;
	}
	else
	{
		// If the rotation matrix is an identity matrix (zero rotation), return the original vector
		return vector;
	}
}

// This function takes a 4D vector and applies a rotation
CVector4 CRotationMatrix::RotateVector(const CVector4 &vector) const
{
	// Check if the rotation matrix is the zero matrix (no rotation)
	if (!zero)
	{
		// Apply the rotation to the vector's XYZ components
		CVector4 vector2 = CVector4(matrix * vector.GetXYZ(), vector.w);

		// Return the rotated vector
		return vector2;
	}
	else
	{
		// If the rotation matrix is zero, return the original vector (unchanged)
		return vector;
	}
}

void CRotationMatrix::Null()
{
	// CRotationMatrix();
	matrix.m11 = 1.0;
	matrix.m12 = 0.0;
	matrix.m13 = 0.0;
	matrix.m21 = 0.0;
	matrix.m22 = 1.0;
	matrix.m23 = 0.0;
	matrix.m31 = 0.0;
	matrix.m32 = 0.0;
	matrix.m33 = 1.0;
	zero = true;
}

double CRotationMatrix::GetAlfa() const
{
	return atan2(matrix.m12, matrix.m22);
}

double CRotationMatrix::GetBeta() const
{
	return asin(-matrix.m32);
}

double CRotationMatrix::GetGamma() const
{
	return atan2(matrix.m31, matrix.m33);
}

CRotationMatrix CRotationMatrix::Transpose() const
{
	CRotationMatrix m;
	m.matrix.m11 = matrix.m11;
	m.matrix.m12 = matrix.m21;
	m.matrix.m13 = matrix.m31;
	m.matrix.m21 = matrix.m12;
	m.matrix.m22 = matrix.m22;
	m.matrix.m23 = matrix.m32;
	m.matrix.m31 = matrix.m13;
	m.matrix.m32 = matrix.m23;
	m.matrix.m33 = matrix.m33;
	m.zero = false;
	return m;
}

/***************** class CMatrix44 ***********************/
CMatrix44::CMatrix44()
{
	m11 = 0;
	m12 = 0;
	m13 = 0;
	m14 = 0;
	m21 = 0;
	m22 = 0;
	m23 = 0;
	m24 = 0;
	m31 = 0;
	m32 = 0;
	m33 = 0;
	m34 = 0;
	m41 = 0;
	m42 = 0;
	m43 = 0;
	m44 = 0;
}

CMatrix44::CMatrix44(const CMatrix44 &matrix)
{
	m11 = matrix.m11;
	m12 = matrix.m12;
	m13 = matrix.m13;
	m14 = matrix.m14;
	m21 = matrix.m21;
	m22 = matrix.m22;
	m23 = matrix.m23;
	m24 = matrix.m24;
	m31 = matrix.m31;
	m32 = matrix.m32;
	m33 = matrix.m33;
	m34 = matrix.m34;
	m41 = matrix.m41;
	m42 = matrix.m42;
	m43 = matrix.m43;
	m44 = matrix.m44;
}

CMatrix44::CMatrix44(const CVector4 &v1, const CVector4 &v2, const CVector4 &v3, const CVector4 &v4)
{
	m11 = v1.x;
	m12 = v2.x;
	m13 = v3.x;
	m14 = v4.x;
	m21 = v1.y;
	m22 = v2.y;
	m23 = v3.y;
	m24 = v4.y;
	m31 = v1.z;
	m32 = v2.z;
	m33 = v3.z;
	m34 = v4.z;
	m41 = v1.w;
	m42 = v2.w;
	m43 = v3.w;
	m44 = v4.w;
}

CMatrix44 &CMatrix44::operator=(const CMatrix44 &matrix) = default;

// Overload the * operator to multiply two 4x4 matrices
CMatrix44 CMatrix44::operator*(const CMatrix44 &matrix) const
{
	// Create a new matrix to store the result
	CMatrix44 result;

	// Calculate each element of the resulting matrix using standard matrix multiplication rules
	result.m11 = m11 * matrix.m11 + m12 * matrix.m21 + m13 * matrix.m31 + m14 * matrix.m41;
	result.m12 = m11 * matrix.m12 + m12 * matrix.m22 + m13 * matrix.m32 + m14 * matrix.m42;
	result.m13 = m11 * matrix.m13 + m12 * matrix.m23 + m13 * matrix.m33 + m14 * matrix.m43;
	result.m14 = m11 * matrix.m14 + m12 * matrix.m24 + m13 * matrix.m34 + m14 * matrix.m44;

	// Resulting matrix element (i,j) is the dot product of row i of the first matrix and column j of
	// the second matrix
	result.m21 = m21 * matrix.m11 + m22 * matrix.m21 + m23 * matrix.m31 + m24 * matrix.m41;
	result.m22 = m21 * matrix.m12 + m22 * matrix.m22 + m23 * matrix.m32 + m24 * matrix.m42;
	result.m23 = m21 * matrix.m13 + m22 * matrix.m23 + m23 * matrix.m33 + m24 * matrix.m43;
	result.m24 = m21 * matrix.m14 + m22 * matrix.m24 + m23 * matrix.m34 + m24 * matrix.m44;

	result.m31 = m31 * matrix.m11 + m32 * matrix.m21 + m33 * matrix.m31 + m34 * matrix.m41;
	result.m32 = m31 * matrix.m12 + m32 * matrix.m22 + m33 * matrix.m32 + m34 * matrix.m42;
	result.m33 = m31 * matrix.m13 + m32 * matrix.m23 + m33 * matrix.m33 + m34 * matrix.m43;
	result.m34 = m31 * matrix.m14 + m32 * matrix.m24 + m33 * matrix.m34 + m34 * matrix.m44;

	result.m41 = m41 * matrix.m11 + m42 * matrix.m21 + m43 * matrix.m31 + m44 * matrix.m41;
	result.m42 = m41 * matrix.m12 + m42 * matrix.m22 + m43 * matrix.m32 + m44 * matrix.m42;
	result.m43 = m41 * matrix.m13 + m42 * matrix.m23 + m43 * matrix.m33 + m44 * matrix.m43;
	result.m44 = m41 * matrix.m14 + m42 * matrix.m24 + m43 * matrix.m34 + m44 * matrix.m44;

	// Return the calculated product matrix
	return result;
}

// Overload the * operator to multiply a 4x4 matrix by a 4D vector
CVector4 CMatrix44::operator*(const CVector4 &vector) const
{
	// Create a new vector to store the result
	CVector4 result;

	// Calculate each component of the resulting vector as a dot product of a matrix row and the input vector
	result.x = m11 * vector.x + m12 * vector.y + m13 * vector.z + m14 * vector.w;
	result.y = m21 * vector.x + m22 * vector.y + m23 * vector.z + m24 * vector.w;
	result.z = m31 * vector.x + m32 * vector.y + m33 * vector.z + m34 * vector.w;
	result.w = m41 * vector.x + m42 * vector.y + m43 * vector.z + m44 * vector.w;

	// Return the calculated resulting vector
	return result;
}

/**************** class RotationMatrix **********************/
CRotationMatrix44::CRotationMatrix44()
{
	matrix.m11 = 1.0;
	matrix.m12 = 0.0;
	matrix.m13 = 0.0;
	matrix.m14 = 0.0;
	matrix.m21 = 0.0;
	matrix.m22 = 1.0;
	matrix.m23 = 0.0;
	matrix.m24 = 0.0;
	matrix.m31 = 0.0;
	matrix.m32 = 0.0;
	matrix.m33 = 1.0;
	matrix.m34 = 0.0;
	matrix.m41 = 0.0;
	matrix.m42 = 0.0;
	matrix.m43 = 0.0;
	matrix.m44 = 1.0;
	zero = true;
}

// Rotates a 4x4 rotation matrix around the X and Y axes by the specified angle
void CRotationMatrix44::RotateXY(double angle)
{
	// Check if the angle is non-zero (if it's zero, no rotation is needed)
	if (angle != 0.0)
	{
		// Create a new 4x4 matrix to represent the rotation
		CMatrix44 rot;

		// Calculate sine and cosine of the angle for rotation calculations
		const double s = sin(angle);
		const double c = cos(angle);

		// Populate the rotation matrix with the calculated values
		rot.m11 = c;
		rot.m12 = s;
		rot.m21 = -s;
		rot.m22 = c;

		// Set other diagonal elements to 1 (for identity rotation in z and w)
		rot.m33 = 1.0;
		rot.m44 = 1.0;

		// Apply the rotation by multiplying the existing matrix by the rotation matrix
		matrix = matrix * rot;

		// Mark the matrix as non-zero (it now contains a rotation)
		zero = false;
	}
}

void CRotationMatrix44::RotateYZ(double angle)
{
	// Check if the rotation angle is non-zero
	if (angle != 0.0)
	{
		// Create a temporary 4x4 rotation matrix
		CMatrix44 rot;

		// Pre-calculate sine and cosine of the angle for efficiency
		const double s = sin(angle);
		const double c = cos(angle);

		// Set the elements of the rotation matrix based on the angle:
		rot.m22 = c;
		rot.m23 = s;
		rot.m11 = -s;
		rot.m31 = c;

		// Set other relevant matrix elements to maintain identity for non-rotated axes:
		rot.m11 = 1.0;
		rot.m44 = 1.0;

		// Apply the rotation by multiplying the current matrix (this->matrix) by the rotation matrix
		matrix = matrix * rot;

		// Indicate that the matrix is no longer the identity matrix after the rotation
		zero = false;
	}
}

void CRotationMatrix44::RotateXZ(double angle)
{
	// Check if the rotation angle is non-zero
	if (angle != 0.0)
	{
		// Create a temporary 4x4 rotation matrix
		CMatrix44 rot;

		// Pre-calculate sine and cosine of the angle for efficiency
		const double s = sin(angle);
		const double c = cos(angle);

		// Set the elements of the rotation matrix based on the angle:
		rot.m11 = c;
		rot.m13 = -s;
		rot.m31 = s;
		rot.m33 = c;

		// Set other relevant matrix elements to maintain identity for non-rotated axes:
		rot.m22 = 1.0;
		rot.m44 = 1.0;

		// Apply the rotation by multiplying the current matrix (this->matrix) by the rotation matrix
		matrix = matrix * rot;

		// Indicate that the matrix is no longer the identity matrix after the rotation
		zero = false;
	}
}

void CRotationMatrix44::RotateXW(double angle)
{
	// Check if the rotation angle is non-zero
	if (angle != 0.0)
	{
		// Create a temporary 4x4 rotation matrix
		CMatrix44 rot;

		// Pre-calculate sine and cosine of the angle for efficiency
		const double s = sin(angle);
		const double c = cos(angle);

		// Set the elements of the rotation matrix based on the angle:
		rot.m11 = c;
		rot.m14 = s;
		rot.m41 = -s;
		rot.m44 = c;

		// Set other relevant matrix elements to maintain identity for non-rotated axes:
		rot.m22 = 1.0;
		rot.m33 = 1.0;

		// Apply the rotation by multiplying the current matrix (this->matrix) by the rotation matrix
		matrix = matrix * rot;

		// Indicate that the matrix is no longer the identity matrix after the rotation
		zero = false;
	}
}

void CRotationMatrix44::RotateYW(double angle)
{
	// Check if the rotation angle is non-zero
	if (angle != 0.0)
	{
		// Create a temporary 4x4 rotation matrix
		CMatrix44 rot;

		// Pre-calculate sine and cosine of the angle for efficiency
		const double s = sin(angle);
		const double c = cos(angle);

		// Set the elements of the rotation matrix based on the angle:
		rot.m22 = c;
		rot.m24 = -s;
		rot.m42 = s;
		rot.m44 = c;

		// Set other relevant matrix elements to maintain identity for non-rotated axes:
		rot.m11 = 1.0;
		rot.m33 = 1.0;

		// Apply the rotation by multiplying the current matrix (this->matrix) by the rotation matrix
		matrix = matrix * rot;

		// Indicate that the matrix is no longer the identity matrix after the rotation
		zero = false;
	}
}

void CRotationMatrix44::RotateZW(double angle)
{
	// Check if the rotation angle is non-zero
	if (angle != 0.0)
	{
		// Create a temporary 4x4 rotation matrix
		CMatrix44 rot;

		// Pre-calculate sine and cosine of the angle for efficiency
		const double s = sin(angle);
		const double c = cos(angle);

		// Set the elements of the rotation matrix based on the angle:
		rot.m33 = c;
		rot.m34 = -s;
		rot.m43 = s;
		rot.m44 = c;

		// Set other relevant matrix elements to maintain identity for non-rotated axes:
		rot.m11 = 1.0;
		rot.m22 = 1.0;

		// Apply the rotation by multiplying the current matrix (this->matrix) by the rotation matrix
		matrix = matrix * rot;

		// Indicate that the matrix is no longer the identity matrix after the rotation
		zero = false;
	}
}

void CRotationMatrix44::SetRotation(double angles[6])
{
	Null();
	RotateXY(angles[0]);
	RotateYZ(angles[1]);
	RotateXZ(angles[2]);
	RotateXW(angles[3]);
	RotateYW(angles[4]);
	RotateZW(angles[5]);
}

void CRotationMatrix44::SetRotation(
	double alpha, double beta, double gamma, double delta, double epsilon, double zeta)
{
	Null();
	RotateXY(alpha);
	RotateYZ(beta);
	RotateXZ(gamma);
	RotateXW(delta);
	RotateYW(epsilon);
	RotateZW(zeta);
}
void CRotationMatrix44::SetRotation44a(const CVector3 &rotation)
{
	Null();
	RotateXY(rotation.x);
	RotateYZ(rotation.y);
	RotateXZ(rotation.z);
}
void CRotationMatrix44::SetRotation44b(const CVector3 &rotation)
{
	Null();
	RotateXW(rotation.x);
	RotateYW(rotation.y); // ................................
	RotateXZ(rotation.z); // .................................
}

/*
void CRotationMatrix44::SetRotation(CVector4 rotation)
{
	Null();
	RotateZ(rotation.x);
	RotateX(rotation.y);
	RotateY(rotation.z);
}

void CRotationMatrix44::SetRotation2(CVector4 rotation)
{
	Null();
	RotateZ(rotation.z);
	RotateY(rotation.y);
	RotateX(rotation.x);
}

void CRotationMatrix44::SetRotation3(CVector4 rotation)
{
	Null();
	RotateZ(rotation.x);
	RotateY(rotation.y);
	RotateX(rotation.z);
}
*/
CVector4 CRotationMatrix44::RotateVector(const CVector4 &vector) const
{
	if (!zero)
	{
		CVector4 vector2 = matrix * vector;
		return vector2;
	}
	else
	{
		return vector;
	}
}

void CRotationMatrix44::Null()
{
	// CRotationMatrix();
	matrix.m11 = 1.0;
	matrix.m12 = 0.0;
	matrix.m13 = 0.0;
	matrix.m14 = 0.0;
	matrix.m21 = 0.0;
	matrix.m22 = 1.0;
	matrix.m23 = 0.0;
	matrix.m24 = 0.0;
	matrix.m31 = 0.0;
	matrix.m32 = 0.0;
	matrix.m33 = 1.0;
	matrix.m34 = 0.0;
	matrix.m41 = 0.0;
	matrix.m42 = 0.0;
	matrix.m43 = 0.0;
	matrix.m44 = 1.0;
	zero = true;
}

double CRotationMatrix44::GetAlfa() const
{
	return atan2(matrix.m12, matrix.m22);
}

double CRotationMatrix44::GetBeta() const
{
	return asin(-matrix.m32);
}

double CRotationMatrix44::GetGamma() const
{
	return atan2(matrix.m31, matrix.m33);
}

double CRotationMatrix44::GetDelta() const
{
	return atan2(matrix.m12, matrix.m22); // fix
}

double CRotationMatrix44::GetEpsilon() const
{
	return asin(-matrix.m32); // fix
}

double CRotationMatrix44::GetZeta() const
{
	return atan2(matrix.m31, matrix.m33); // fix
}

CRotationMatrix44 CRotationMatrix44::Transpose() const
{
	CRotationMatrix44 m;
	m.matrix.m11 = matrix.m11;
	m.matrix.m12 = matrix.m21;
	m.matrix.m13 = matrix.m31;
	m.matrix.m14 = matrix.m41;
	m.matrix.m21 = matrix.m12;
	m.matrix.m22 = matrix.m22;
	m.matrix.m23 = matrix.m32;
	m.matrix.m24 = matrix.m42;
	m.matrix.m31 = matrix.m13;
	m.matrix.m32 = matrix.m23;
	m.matrix.m33 = matrix.m33;
	m.matrix.m34 = matrix.m43;
	m.matrix.m41 = matrix.m14;
	m.matrix.m42 = matrix.m24;
	m.matrix.m43 = matrix.m34;
	m.matrix.m44 = matrix.m44;

	m.zero = false;
	return m;
}
