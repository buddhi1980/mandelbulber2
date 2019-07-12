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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Robert Pancoast (RobertPancoast77@gmail.com)
 *
 * CVector2, CVector3, CVector4 and CMatrix33 classes - vector and matrix
 * manipulation
 *
 * These classes get used throughout the program to realize 2d, 3d and 4d
 * variable representation and manipulation. The class CMatrix33 is used for
 * rotation in 3d space.
 */

#ifndef MANDELBULBER2_SRC_ALGEBRA_HPP_
#define MANDELBULBER2_SRC_ALGEBRA_HPP_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <gsl/gsl_sys.h>

#include <QString>

#include "include_header_wrapper.hpp"

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
	inline explicit CVector3(const double vector[3])
	{
		if (vector)
		{
			x = vector[0];
			y = vector[1];
			z = vector[2];
		}
		else
		{
			qFatal("Assigned nullptr to CVector3 as initialization!");
		}
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
		return CVector3((vector.x > 0.0 ? fmod(x, vector.x) : x),
			(vector.y > 0.0 ? fmod(y, vector.y) : y), (vector.z > 0.0 ? fmod(z, vector.z) : z));
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
	inline CVector3 &operator=(const CVector3 &vector) = default;
	inline CVector3 &operator+=(const CVector3 &vector)
	{
		x += vector.x;
		y += vector.y;
		z += vector.z;
		return *this;
	}
	inline CVector3 &operator-=(const CVector3 &vector)
	{
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		return *this;
	}
	inline CVector3 &operator*=(const double &scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	inline CVector3 &operator*=(const CVector3 &vector)
	{
		x *= vector.x;
		y *= vector.y;
		z *= vector.z;
		return *this;
	}
	inline CVector3 &operator/=(const double &scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
	inline CVector3 &operator/=(const CVector3 &vector)
	{
		x /= vector.x;
		y /= vector.y;
		z /= vector.z;
		return *this;
	}
	inline bool operator==(const CVector3 &vector) const
	{
		return x == vector.x && y == vector.y && z == vector.z;
	}
	inline double Length() const { return sqrt(x * x + y * y + z * z); }
	inline double LengthPow(double p) const
	{
		return pow(pow(x, p) + pow(y, p) + pow(z, p), 1.0 / p);
	}
	inline double Dot(const CVector3 &vector) const
	{
		return x * vector.x + y * vector.y + z * vector.z;
	}
	inline CVector3 Cross(const CVector3 &v) const
	{
		CVector3 c;
		c.x = y * v.z - z * v.y;
		c.y = -x * v.z + z * v.x;
		c.z = x * v.y - y * v.x;
		return c;
	}
	inline CVector3 Abs() const
	{
		CVector3 c;
		c.x = fabs(x);
		c.y = fabs(y);
		c.z = fabs(z);
		return c;
	}
	inline double Normalize() // returns normalization factor
	{
		const double norm = 1.0 / Length();
		x = x * norm;
		y = y * norm;
		z = z * norm;
		return norm;
	}
	inline double GetAlpha() const { return atan2(y, x); }
	inline double GetBeta() const { return atan2(z, sqrt(x * x + y * y)); }
	inline bool IsNotANumber() const
	{
		// Check for NaN
		// single "&" is intended: since "most of times" gsl_finite is true,
		// always evaluating xyz at once allows cpu to perform faster
		// than evaluating each dimension sequentially.
		return !(gsl_finite(x) & gsl_finite(y) & gsl_finite(z));
	}
	CVector3 RotateAroundVectorByAngle(const CVector3 &axis, double angle) const;
	QString Debug() const
	{
		return QString("[%1, %2, %3]").arg(QString::number(x), QString::number(y), QString::number(z));
	}

	double itemByName(char item) const;

	double x;
	double y;
	double z;
};

inline CVector3 operator*(double scalar, const CVector3 &vector)
{
	return CVector3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

inline CVector3 operator/(double scalar, const CVector3 &vector)
{
	return CVector3(scalar / vector.x, scalar / vector.y, scalar / vector.z);
}
inline CVector3 fabs(CVector3 v)
{
	v.x = fabs(v.x);
	v.y = fabs(v.y);
	v.z = fabs(v.z);
	return v;
}

/******************* Structured vector 3D ****************/
class StructuredCVector3
{
public:
	inline StructuredCVector3()
	{
		for (int n = 0; n < 8; n++)
		{
			x[n] = 0.0;
			y[n] = 0.0;
			z[n] = 0.0;
		}
	}

	inline explicit StructuredCVector3(CVector3 vectors[])
	{
		for (int n = 0; n < 8; n++)
		{
			x[n] = vectors[n].x;
			y[n] = vectors[n].y;
			z[n] = vectors[n].z;
		}
	}

	double x[8]{};
	double y[8]{};
	double z[8]{};
};

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
	inline CVector4(const CVector3 &vectXYZ, double _w)
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
	inline explicit CVector4(const double v[4])
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
	inline CVector4 operator*(const double &s) const { return CVector4(x * s, y * s, z * s, w * s); }
	inline CVector4 operator*(const CVector4 &v) const
	{
		return CVector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}
	inline CVector4 operator*(const CVector3 &v) const
	{
		return CVector4(x * v.x, y * v.y, z * v.z, w);
	}

	inline CVector4 operator/(const double &s) const { return CVector4(x / s, y / s, z / s, w / s); }
	inline CVector4 &operator=(const CVector4 &v) = default;
	inline CVector4 &operator+=(const CVector4 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	inline CVector4 &operator-=(const CVector4 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	inline CVector4 &operator*=(const double &s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}
	inline CVector4 &operator*=(const CVector4 &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}
	inline CVector4 &operator*=(const CVector3 &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= 1.0;
		return *this;
	}
	inline CVector4 &operator/=(const double &s)
	{
		x /= s;
		y /= s;
		z /= s;
		w /= s;
		return *this;
	}
	inline bool operator==(const CVector4 &v) const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}
	inline double Length() const { return sqrt(x * x + y * y + z * z + w * w); }
	inline double Dot(const CVector4 &v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
	inline CVector4 Abs() const
	{
		CVector4 c;
		c.x = fabs(x);
		c.y = fabs(y);
		c.z = fabs(z);
		c.w = fabs(w);
		return c;
	}
	inline void GetXYZWInto(CVector3 &v, double &wOutput) const
	{
		v.x = x;
		v.y = y;
		v.z = z;
		wOutput = w;
	}
	inline CVector3 GetXYZ() const { return CVector3(x, y, z); }

	inline double Normalize()
	{
		const double norm = 1.0 / Length();
		x = x * norm;
		y = y * norm;
		z = z * norm;
		w = w * norm;
		return norm;
	}
	inline CVector4 operator%(const CVector4 &vector) const
	{
		return CVector4((vector.x > 0.0 ? fmod(x, vector.x) : x),
			(vector.y > 0.0 ? fmod(y, vector.y) : y), (vector.z > 0.0 ? fmod(z, vector.z) : z),
			(vector.w > 0.0 ? fmod(w, vector.w) : w));
	}
	inline CVector4 mod(const CVector4 &v) const
	{
		if (v.Length() == 0.0) return *this;
		return (((*this - v * 0.5) % v) + v) % v - v * 0.5;
	}

	inline bool IsNotANumber() const
	{
		// Check for NaN
		// single "&" is intended: since "most of times" gsl_finite is true,
		// always evaluating xyz at once allows cpu to perform faster
		// than evaluating each dimension sequentially.
		return !(gsl_finite(x) & gsl_finite(y) & gsl_finite(z) & gsl_finite(w));
	}

	CVector4 RotateAroundVectorByAngle(const CVector3 &axis, double angle) const;

	QString Debug() const
	{
		return QString("[%1, %2, %3, %4]")
			.arg(QString::number(x), QString::number(y), QString::number(z), QString::number(w));
	}

	double itemByName(char item) const;

	double x, y, z, w;
};

inline CVector4 operator*(double scalar, const CVector4 &vector)
{
	return CVector4(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar);
}

inline CVector4 operator/(double scalar, const CVector4 &vector)
{
	return CVector4(scalar / vector.x, scalar / vector.y, scalar / vector.z, scalar / vector.w);
}

inline CVector4 fabs(CVector4 v)
{
	v.x = fabs(v.x);
	v.y = fabs(v.y);
	v.z = fabs(v.z);
	v.w = fabs(v.w);
	return v;
}
/************************* vector 2D **********************/

template <typename T>
class CVector2
{
public:
	inline CVector2() : x(), y(){};
	inline CVector2(T x_init, T y_init) : x(x_init), y(y_init){};
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
	inline CVector2 operator*(const double &scalar) const { return CVector2(x * scalar, y * scalar); }
	inline CVector2 operator/(const double &scalar) const { return CVector2(x / scalar, y / scalar); }
	inline CVector2 &operator=(const CVector2 &vector)
	{
		x = vector.x;
		y = vector.y;
		return *this;
	}
	inline CVector2 &operator+=(const CVector2 &vector)
	{
		x += vector.x;
		y += vector.y;
		return *this;
	}
	inline CVector2 &operator-=(const CVector2 &vector)
	{
		x -= vector.x;
		y -= vector.y;
		return *this;
	}
	inline CVector2 &operator*=(const double &scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}
	inline CVector2 &operator/=(const double &scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}
	inline double Length() const { return sqrt(x * x + y * y); }
	inline double LengthPow(double p) const { return pow(pow(x, p) + pow(y, p), 1.0 / p); }
	inline CVector2 Deadband() const
	{
		CVector2 c;
		c.x = x;
		c.y = y;
		if (fabs(x) < 0.1) c.x = 0;
		if (fabs(y) < 0.1) c.y = 0;
		return c;
	}
	inline double Dot(const CVector2 &vector) const { return x * vector.x + y * vector.y; }
	inline CVector2 Abs()
	{
		CVector2 c;
		c.x = fabs(x);
		c.y = fabs(y);
		return c;
	}
	inline CVector2 operator%(const CVector2 &vector) const
	{
		return CVector2(
			(vector.x > 0.0 ? fmod(x, vector.x) : x), (vector.y > 0.0 ? fmod(y, vector.y) : y));
	}
	inline CVector2 mod(const CVector2 &vector) const
	{
		if (vector.Length() == 0.0) return *this;
		return (((*this - vector * 0.5) % vector) + vector) % vector - vector * 0.5;
	}
	inline double Normalize() // returns normalization factor
	{
		const double norm = 1.0 / Length();
		x = x * norm;
		y = y * norm;
		return norm;
	}
	QString Debug() { return QString("[%1, %2]").arg(QString::number(x), QString::number(y)); }

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
	return CVector2<T>(scalar / vector.x, scalar / vector.y);
}

template <typename T>
inline CVector2<T> fabs(CVector2<T> v)
{
	v.x = fabs(v.x);
	v.y = fabs(v.y);
	return v;
}

/************************* matrix 3x3 (fast) *****************/
class CMatrix33
{
public:
	CMatrix33();
	CMatrix33(const CMatrix33 &matrix);
	CMatrix33(const CVector3 &v1, const CVector3 &v2, const CVector3 &v3);
	CMatrix33 operator*(const CMatrix33 &matrix) const;
	CVector3 operator*(const CVector3 &vector) const;
	CMatrix33 &operator=(const CMatrix33 &);
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
	CVector3 RotateVector(const CVector3 &vector) const;
	CVector4 RotateVector(const CVector4 &vector) const;
	double GetAlfa() const;
	double GetBeta() const;
	double GetGamma() const;
	void SetRotation(double angles[3]);
	void SetRotation(double alpha, double beta, double gamma);
	void SetRotation(const CVector3 &rotation);
	void SetRotation2(const CVector3 &rotation);
	void SetRotation3(const CVector3 &rotation);
	CRotationMatrix Transpose() const;
	CMatrix33 GetMatrix() const { return matrix; }

private:
	CMatrix33 matrix;
	bool zero;
};

/************************* matrix 4x4 (fast) *****************/
class CMatrix44
{
public:
	CMatrix44();
	CMatrix44(const CMatrix44 &matrix);
	CMatrix44(const CVector4 &v1, const CVector4 &v2, const CVector4 &v3, const CVector4 &v4);
	CMatrix44 operator*(const CMatrix44 &matrix) const;
	CVector4 operator*(const CVector4 &vector) const;
	CMatrix44 &operator=(const CMatrix44 &);
	double m11;
	double m12;
	double m13;
	double m14;
	double m21;
	double m22;
	double m23;
	double m24;
	double m31;
	double m32;
	double m33;
	double m34;
	double m41;
	double m42;
	double m43;
	double m44;
};
/************************* rotation matrix 44 *******************/
class CRotationMatrix44
{
public:
	CRotationMatrix44();
	void RotateXY(double angle);
	void RotateYZ(double angle);
	void RotateXZ(double angle);
	void RotateXW(double angle);
	void RotateYW(double angle);
	void RotateZW(double angle);
	void Null();
	CVector4 RotateVector(const CVector4 &vector) const; // ....................??
	double GetAlfa() const;
	double GetBeta() const;
	double GetGamma() const;
	double GetDelta() const;
	double GetEpsilon() const;
	double GetZeta() const;
	void SetRotation(double angles[6]);
	void SetRotation(
		double alpha, double beta, double gamma, double delta, double epsilon, double zeta);
	// void SetRotation(CVector4 rotation);
	// void SetRotation2(CVector4 rotation);
	// void SetRotation3(CVector4 rotation);

	void SetRotation44a(const CVector3 &rotation);
	void SetRotation44b(const CVector3 &rotation);

	CRotationMatrix44 Transpose() const;
	CMatrix44 GetMatrix() const { return matrix; }

private:
	CMatrix44 matrix;
	bool zero;
};

#endif /* MANDELBULBER2_SRC_ALGEBRA_HPP_ */
