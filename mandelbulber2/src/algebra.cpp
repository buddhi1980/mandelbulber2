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

CVector3 CVector3::RotateAroundVectorByAngle(const CVector3 &axis, double angle) const
{
	CVector3 vector = *this * cos(angle);
	vector += (axis.Cross(*this)) * sin(angle);
	vector += axis * axis.Dot(*this) * (1 - cos(angle));
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

double CVector3::itemByName(char item) const
{
	switch (item)
	{
		case 'x': return x;
		case 'y': return y;
		case 'z': return z;
		default: return 0.0;
	}
}

double CVector4::itemByName(char item) const
{
	switch (item)
	{
		case 'x': return x;
		case 'y': return y;
		case 'z': return z;
		case 'w': return w;
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

CMatrix33 CMatrix33::operator*(const CMatrix33 &matrix) const
{
	CMatrix33 result;
	result.m11 = m11 * matrix.m11 + m12 * matrix.m21 + m13 * matrix.m31;
	result.m12 = m11 * matrix.m12 + m12 * matrix.m22 + m13 * matrix.m32;
	result.m13 = m11 * matrix.m13 + m12 * matrix.m23 + m13 * matrix.m33;
	result.m21 = m21 * matrix.m11 + m22 * matrix.m21 + m23 * matrix.m31;
	result.m22 = m21 * matrix.m12 + m22 * matrix.m22 + m23 * matrix.m32;
	result.m23 = m21 * matrix.m13 + m22 * matrix.m23 + m23 * matrix.m33;
	result.m31 = m31 * matrix.m11 + m32 * matrix.m21 + m33 * matrix.m31;
	result.m32 = m31 * matrix.m12 + m32 * matrix.m22 + m33 * matrix.m32;
	result.m33 = m31 * matrix.m13 + m32 * matrix.m23 + m33 * matrix.m33;
	return result;
}

CVector3 CMatrix33::operator*(const CVector3 &vector) const
{
	CVector3 result;
	result.x = m11 * vector.x + m12 * vector.y + m13 * vector.z;
	result.y = m21 * vector.x + m22 * vector.y + m23 * vector.z;
	result.z = m31 * vector.x + m32 * vector.y + m33 * vector.z;
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

void CRotationMatrix::RotateX(double angle)
{
	if (angle != 0.0)
	{
		CMatrix33 rot;
		const double s = sin(angle);
		const double c = cos(angle);
		rot.m11 = 1.0;
		rot.m22 = c;
		rot.m33 = c;
		rot.m23 = -s;
		rot.m32 = s;
		matrix = matrix * rot;
		zero = false;
	}
}

void CRotationMatrix::RotateY(double angle)
{
	if (angle != 0.0)
	{
		CMatrix33 rot;
		const double s = sin(angle);
		const double c = cos(angle);
		rot.m22 = 1.0;
		rot.m33 = c;
		rot.m11 = c;
		rot.m31 = -s;
		rot.m13 = s;
		matrix = matrix * rot;
		zero = false;
	}
}

void CRotationMatrix::RotateZ(double angle)
{
	if (angle != 0.0)
	{
		CMatrix33 rot;
		const double s = sin(angle);
		const double c = cos(angle);
		rot.m33 = 1.0;
		rot.m11 = c;
		rot.m22 = c;
		rot.m12 = -s;
		rot.m21 = s;
		matrix = matrix * rot;
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

CVector3 CRotationMatrix::RotateVector(const CVector3 &vector) const
{
	if (!zero)
	{
		CVector3 vector2 = matrix * vector;
		return vector2;
	}
	else
	{
		return vector;
	}
}

CVector4 CRotationMatrix::RotateVector(const CVector4 &vector) const
{
	if (!zero)
	{
		CVector4 vector2 = CVector4(matrix * vector.GetXYZ(), vector.w);
		return vector2;
	}
	else
	{
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

CMatrix44 CMatrix44::operator*(const CMatrix44 &matrix) const
{
	CMatrix44 result;
	result.m11 = m11 * matrix.m11 + m12 * matrix.m21 + m13 * matrix.m31 + m14 * matrix.m41;
	result.m12 = m11 * matrix.m12 + m12 * matrix.m22 + m13 * matrix.m32 + m14 * matrix.m42;
	result.m13 = m11 * matrix.m13 + m12 * matrix.m23 + m13 * matrix.m33 + m14 * matrix.m43;
	result.m14 = m11 * matrix.m14 + m12 * matrix.m24 + m13 * matrix.m34 + m14 * matrix.m44;

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

	return result;
}

CVector4 CMatrix44::operator*(const CVector4 &vector) const
{
	CVector4 result;
	result.x = m11 * vector.x + m12 * vector.y + m13 * vector.z + m14 * vector.w;
	result.y = m21 * vector.x + m22 * vector.y + m23 * vector.z + m24 * vector.w;
	result.z = m31 * vector.x + m32 * vector.y + m33 * vector.z + m34 * vector.w;
	result.w = m41 * vector.x + m42 * vector.y + m43 * vector.z + m44 * vector.w;
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

void CRotationMatrix44::RotateXY(double angle)
{
	if (angle != 0.0)
	{
		CMatrix44 rot;
		const double s = sin(angle);
		const double c = cos(angle);
		rot.m11 = c;
		rot.m12 = s;
		rot.m21 = -s;
		rot.m22 = c;
		rot.m33 = 1.0;
		rot.m44 = 1.0;
		matrix = matrix * rot;
		zero = false;
	}
}

void CRotationMatrix44::RotateYZ(double angle)
{
	if (angle != 0.0)
	{
		CMatrix44 rot;
		const double s = sin(angle);
		const double c = cos(angle);
		rot.m22 = c;
		rot.m23 = s;
		rot.m11 = -s;
		rot.m31 = c;
		rot.m11 = 1.0;
		rot.m44 = 1.0;
		matrix = matrix * rot;
		zero = false;
	}
}

void CRotationMatrix44::RotateXZ(double angle)
{
	if (angle != 0.0)
	{
		CMatrix44 rot;
		const double s = sin(angle);
		const double c = cos(angle);
		rot.m11 = c;
		rot.m13 = -s;
		rot.m31 = s;
		rot.m33 = c;
		rot.m22 = 1.0;
		rot.m44 = 1.0;
		matrix = matrix * rot;
		zero = false;
	}
}

void CRotationMatrix44::RotateXW(double angle)
{
	if (angle != 0.0)
	{
		CMatrix44 rot;
		const double s = sin(angle);
		const double c = cos(angle);
		rot.m11 = c;
		rot.m14 = s;
		rot.m41 = -s;
		rot.m44 = c;
		rot.m22 = 1.0;
		rot.m33 = 1.0;
		matrix = matrix * rot;
		zero = false;
	}
}

void CRotationMatrix44::RotateYW(double angle)
{
	if (angle != 0.0)
	{
		CMatrix44 rot;
		const double s = sin(angle);
		const double c = cos(angle);
		rot.m22 = c;
		rot.m24 = -s;
		rot.m42 = s;
		rot.m44 = c;
		rot.m11 = 1.0;
		rot.m33 = 1.0;
		matrix = matrix * rot;
		zero = false;
	}
}

void CRotationMatrix44::RotateZW(double angle)
{
	if (angle != 0.0)
	{
		CMatrix44 rot;
		const double s = sin(angle);
		const double c = cos(angle);
		rot.m33 = c;
		rot.m34 = -s;
		rot.m43 = s;
		rot.m44 = c;
		rot.m11 = 1.0;
		rot.m22 = 1.0;
		matrix = matrix * rot;
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
