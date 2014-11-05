/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * CVector2, CVector3 and CMatrix33 classes - vector and matrix manimulation
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef ALGEBRA_H_
#define ALGEBRA_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <QString>

/************************* vector 3D **********************/
class CVector3
{
public:
	inline CVector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	inline CVector3(double x_init, double y_init, double z_init)
	{
		x = x_init;
		y = y_init;
		z = z_init;
	}
	inline CVector3(double alpha, double beta)
	{
		x = cos(beta) * cos(alpha);
		y = cos(beta) * sin(alpha);
		z = sin(beta);
	}
	inline CVector3(const CVector3 &vector)
	{
		x = vector.x;
		y = vector.y;
		z = vector.z;
	}
	inline CVector3(const double vector[3])
	{
		x = vector[0];
		y = vector[1];
		z = vector[2];
	}
	inline CVector3 operator+(const CVector3 &vector) const
	{
		return CVector3(x + vector.x, y + vector.y, z + vector.z);
	}
	inline CVector3 operator-(const CVector3 &vector) const
	{
		return CVector3(x - vector.x, y - vector.y, z - vector.z);
	}
	inline CVector3 operator%(const CVector3 &vector) const
	{
		return CVector3((vector.x > 0.0 ? fmod(x, vector.x) : x), (vector.y > 0.0 ? fmod(y, vector.y) : y), (vector.z > 0.0 ? fmod(z, vector.z) : z));
	}
	inline CVector3 mod(const CVector3 &vector) const
	{
		if (vector.Length() == 0.0) return *this;
		return (*this % vector + vector) % vector - vector * 0.5;
	}
	inline CVector3 operator*(const double &scalar) const
	{
		return CVector3(x * scalar, y * scalar, z * scalar);
	}
	inline CVector3 operator/(const double &scalar) const
	{
		return CVector3(x / scalar, y / scalar, z / scalar);
	}
	inline CVector3& operator=(const CVector3 &vector)
	{
		x = vector.x;
		y = vector.y;
		z = vector.z;
		return *this;
	}
	inline CVector3& operator+=(const CVector3 &vector)
	{
		x += vector.x;
		y += vector.y;
		z += vector.z;
		return *this;
	}
	inline CVector3& operator-=(const CVector3 &vector)
	{
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		return *this;
	}
	inline CVector3& operator*=(const double &scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	inline CVector3& operator/=(const double &scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
	inline double Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}
	inline double Dot(const CVector3& vector) const
	{
		return x * vector.x + y * vector.y + z * vector.z;
	}
	inline CVector3 Cross(const CVector3& v)
	{
		CVector3 c;
		c.x =  y*v.z - z*v.y;
	  c.y = -x*v.z + z*v.x;
	  c.z =  x*v.y - y*v.x;
	  return c;
	}
	inline double Normalize() //returns normalization factor
	{
		double norm = 1.0 / Length();
		x = x * norm;
		y = y * norm;
		z = z * norm;
		return norm;
	}
	inline double GetAlpha() const
	{
		return atan2(y, x);
	}
	inline double GetBeta() const
	{
		return atan2(z, sqrt(x * x + y * y));
	}
	CVector3 RotateAroundVectorByAngle(CVector3 axis, double angle);
	QString Debug() const
	{
		return QString("[") + QString::number(x) + QString(", ") + QString::number(y) + QString(", ") + QString::number(z)+ QString("]");
	}

	double itemByName(char item);

	double x;
	double y;
	double z;
};

template <typename T>
class CVector2
{
public:
	inline CVector2() : x(), y() {};
	inline CVector2(T x_init, T y_init) : x(x_init), y(y_init) {};
	inline CVector2(const CVector2<T> &vector)
	{
		x = vector.x;
		y = vector.y;
	}
	inline CVector2 operator+(const CVector2 &vector) const
	{
		return CVector2(x + vector.x, y + vector.y);
	}
	inline CVector2 operator-(const CVector2 &vector) const
	{
		return CVector2(x - vector.x, y - vector.y);
	}
	inline CVector2 operator*(const double &scalar) const
	{
		return CVector2(x * scalar, y * scalar);
	}
	inline CVector2 operator/(const double &scalar) const
	{
		return CVector2(x / scalar, y / scalar);
	}
	inline CVector2& operator=(const CVector2 &vector)
	{
		x = vector.x;
		y = vector.y;
		return *this;
	}
	inline CVector2& operator+=(const CVector2 &vector)
	{
		x += vector.x;
		y += vector.y;
		return *this;
	}
	inline CVector2& operator-=(const CVector2 &vector)
	{
		x -= vector.x;
		y -= vector.y;
		return *this;
	}
	inline CVector2& operator*=(const double &scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}
	inline CVector2& operator/=(const double &scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}
	inline double Length() const
	{
		return sqrt(x * x + y * y);
	}
	inline double Dot(const CVector2& vector) const
	{
		return x * vector.x + y * vector.y;
	}
	inline double Normalize() //returns normalization factor
	{
		double norm = 1.0 / Length();
		x = x * norm;
		y = y * norm;
		return norm;
	}
	QString Debug()
	{
		return QString("[") + QString::number(x) + QString(", ") + QString::number(y) + QString("]");
	}

	T x;
	T y;
};

/************************* matrix 3x3 (fast) *****************/
class CMatrix33
{
public:
	CMatrix33();
	CMatrix33(const CMatrix33 &matrix);
	CMatrix33 operator*(const CMatrix33 &matrix) const;
	CVector3 operator*(const CVector3 &vector) const;
	CMatrix33& operator=(const CMatrix33&);
	double m11;
	double m12;
	double m13;
	double m21;
	double m22;
	double m23;
	double m31;
	double m32;
	double m33;
};

/************************* rotation matrix *******************/
class CRotationMatrix
{
public:
	CRotationMatrix();
	void RotateX(double angle);
	void RotateY(double angle);
	void RotateZ(double angle);
	void Null();
	CVector3 RotateVector(const CVector3& vector) const;
	double GetAlfa() const;
	double GetBeta() const;
	double GetGamma() const;
	void SetRotation(double angles[3]);
	void SetRotation(double alpha, double beta, double gamma);
	void SetRotation(CVector3 rotation);
	void SetRotation2(CVector3 rotation);
	void SetRotation3(CVector3 rotation);
	CRotationMatrix Transpose(void) const;
	CMatrix33 GetMatrix() {return matrix;}

private:
	CMatrix33 matrix;
	bool zero;
};

#endif /* ALGEBRA_H_ */
