/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * CVector2, CVector3 and CMatrix33 classes - vector and matrix manipulation
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
		return (((*this - vector * 0.5) % vector) + vector) % vector - vector * 0.5;
	}
	inline CVector3 operator*(const double &scalar) const
	{
		return CVector3(x * scalar, y * scalar, z * scalar);
	}
	inline CVector3 operator*(const CVector3 &vector) const
	{
		return CVector3(x * vector.x, y * vector.y, z * vector.z);
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
	inline CVector3& operator*=(const CVector3 &vector)
	{
		x *= vector.x;
		y *= vector.y;
		z *= vector.z;
		return *this;
	}
	inline CVector3& operator/=(const double &scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
	inline bool operator==(const CVector3 &vector)
	{
		return x == vector.x && y == vector.y && z == vector.z;
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

inline CVector3 operator*(double scalar, CVector3 vector)
{
	return CVector3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

inline CVector3 operator/(double scalar, CVector3 vector)
{
	return CVector3(vector.x / scalar, vector.y / scalar, vector.z / scalar);
}

/************************* vector 4D **********************/
class CVector4
{
public:
	inline CVector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	inline CVector4(double _x, double _y, double _z, double _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	inline CVector4(CVector3 vectXYZ, double _w)
	{
		x = vectXYZ.x;
		y = vectXYZ.y;
		z = vectXYZ.z;
		w = _w;
	}
	inline CVector4(const CVector4 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}
	inline CVector4(const double v[4])
	{
		x = v[0];
		y = v[1];
		z = v[2];
		w = v[3];
	}
	inline CVector4 operator+(const CVector4 &v) const
	{
		return CVector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	inline CVector4 operator-(const CVector4 &v) const
	{
		return CVector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}
	inline CVector4 operator*(const double &s) const
	{
		return CVector4(x * s, y * s, z * s, w * s);
	}
	inline CVector4 operator*(const CVector4 &v) const
	{
		return CVector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}
	inline CVector4 operator/(const double &s) const
	{
		return CVector4(x / s, y / s, z / s, w / s);
	}
	inline CVector4& operator=(const CVector4 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}
	inline CVector4& operator+=(const CVector4 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	inline CVector4& operator-=(const CVector4 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	inline CVector4& operator*=(const double &s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}
	inline CVector4& operator*=(const CVector4 &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}
	inline CVector4& operator/=(const double &s)
	{
		x /= s;
		y /= s;
		z /= s;
		w /= s;
		return *this;
	}
	inline bool operator==(const CVector4 &v)
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}
	inline double Length() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}
	inline double Dot(const CVector4& v) const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}
	inline CVector3 GetXYZ()
	{
		return CVector3(x, y, z);
	}

	inline double Normalize() {
		double norm = 1.0 / Length();
		x = x * norm; y = y * norm; z = z * norm; w = w * norm;
		return norm;
	}
	inline CVector4 operator%(const CVector4 &vector) const
	{
		return CVector4(
			(vector.x > 0.0 ? fmod(x, vector.x) : x),
			(vector.y > 0.0 ? fmod(y, vector.y) : y),
			(vector.z > 0.0 ? fmod(z, vector.z) : z),
			(vector.w > 0.0 ? fmod(w, vector.w) : w)
		);
	}
	inline CVector4 mod(const CVector4 &v) const
	{
		if (v.Length() == 0.0) return *this;
		return (((*this - v * 0.5) % v) + v) % v - v * 0.5;
	}
	QString Debug() const
	{
		return QString("[")
			+ QString::number(x) + QString(", ")
			+ QString::number(y) + QString(", ")
			+ QString::number(z) + QString(", ")
			+ QString::number(w) + QString(" ]");
	}

	double x, y, z, w;
};

inline CVector4 operator*(double scalar, CVector4 vector)
{
	return CVector4(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar);
}

inline CVector4 operator/(double scalar, CVector4 vector)
{
	return CVector4(vector.x / scalar, vector.y / scalar, vector.z / scalar, vector.w / scalar);
}

/************************* vector 2D **********************/

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

template <typename T>
inline CVector2<T> operator*(T scalar, CVector2<T> vector)
{
	return CVector2<T>(vector.x * scalar, vector.y * scalar);
}

template <typename T>
inline CVector2<T> operator/(T scalar, CVector2<T> vector)
{
	return CVector2<T>(vector.x / scalar, vector.y / scalar);
}


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
