#ifndef MANDELBULBER2_OPENCL_ALGEBRA_HPP_
#define MANDELBULBER2_OPENCL_ALGEBRA_HPP_

#ifndef OPENCL_KERNEL_CODE
#include <QString>
#include "../src/algebra.hpp"
#include "../src/color_structures.hpp"
#endif

typedef struct {
	cl_float3 m1;
	cl_float3 m2;
	cl_float3 m3;
} matrix33;

#ifndef OPENCL_KERNEL_CODE
inline matrix33 toClMatrix33(CRotationMatrix source)
{
	CMatrix33 matrix = source.GetMatrix();
	matrix33 m;
	m.m1 = {cl_float(matrix.m11), cl_float(matrix.m12), cl_float(matrix.m13)};
	m.m2 = {cl_float(matrix.m21), cl_float(matrix.m22), cl_float(matrix.m23)};
	m.m3 = {cl_float(matrix.m31), cl_float(matrix.m32), cl_float(matrix.m33)};
	return m;
}
inline cl_float3 toClFloat3(CVector3 v)
{
	cl_float3 retval = {cl_float(v.x), cl_float(v.y), cl_float(v.z)};
	return retval;
}
inline cl_int3 toClInt3(sRGB c)
{
	cl_int3 retval = {cl_int(c.R), cl_int(c.G), cl_int(c.B)};
	return retval;
}
inline cl_float4 toClFloat4(CVector4 v)
{
	cl_float4 retval = {cl_float(v.x), cl_float(v.y), cl_float(v.z), cl_float(v.w)};
	return retval;
}
#endif

#ifdef OPENCL_KERNEL_CODE
inline float3 Matrix33MulFloat3(matrix33 matrix, float3 vect)
{
	float3 out;
	out.x = dot(vect, matrix.m1);
	out.y = dot(vect, matrix.m2);
	out.z = dot(vect, matrix.m3);
	return out;
}

matrix33 Matrix33MulMatrix33(matrix33 m1, matrix33 m2)
{
	matrix33 out;
	out.m1.x = m1.m1.x * m2.m1.x + m1.m1.y * m2.m2.x + m1.m1.z * m2.m3.x;
	out.m1.y = m1.m1.x * m2.m1.y + m1.m1.y * m2.m2.y + m1.m1.z * m2.m3.y;
	out.m1.z = m1.m1.x * m2.m1.z + m1.m1.y * m2.m2.z + m1.m1.z * m2.m3.z;
	out.m2.x = m1.m2.x * m2.m1.x + m1.m2.y * m2.m2.x + m1.m2.z * m2.m3.x;
	out.m2.y = m1.m2.x * m2.m1.y + m1.m2.y * m2.m2.y + m1.m2.z * m2.m3.y;
	out.m2.z = m1.m2.x * m2.m1.z + m1.m2.y * m2.m2.z + m1.m2.z * m2.m3.z;
	out.m3.x = m1.m3.x * m2.m1.x + m1.m3.y * m2.m2.x + m1.m3.z * m2.m3.x;
	out.m3.y = m1.m3.x * m2.m1.y + m1.m3.y * m2.m2.y + m1.m3.z * m2.m3.y;
	out.m3.z = m1.m3.x * m2.m1.z + m1.m3.y * m2.m2.z + m1.m3.z * m2.m3.z;
	return out;
}

matrix33 RotateX(matrix33 m, float angle)
{
	matrix33 out, rot;
	float s = sin(angle);
	float c = cos(angle);
	rot.m1 = (float3){1.0f, 0.0f, 0.0f};
	rot.m2 = (float3){0.0f, c, -s};
	rot.m3 = (float3){0.0f, s, c};
	out = Matrix33MulMatrix33(m, rot);
	return out;
}

matrix33 RotateY(matrix33 m, float angle)
{
	matrix33 out, rot;
	float s = sin(angle);
	float c = cos(angle);
	rot.m1 = (float3){c, 0.0f, s};
	rot.m2 = (float3){0.0f, 1.0f, 0.0f};
	rot.m3 = (float3){-s, 0.0f, c};
	out = Matrix33MulMatrix33(m, rot);
	return out;
}

matrix33 RotateZ(matrix33 m, float angle)
{
	matrix33 out, rot;
	float s = sin(angle);
	float c = cos(angle);
	rot.m1 = (float3){c, -s, 0.0f};
	rot.m2 = (float3){s, c, 0.0f};
	rot.m3 = (float3){0.0f, 0.0f, 1.0f};
	out = Matrix33MulMatrix33(m, rot);
	return out;
}

float3 RotateAroundVectorByAngle(float3 origin, float3 axis, float angle)
{
	float3 vector = origin * cos(angle);
	vector += cross(axis, origin) * sin(angle);
	vector += axis * dot(axis, origin) * (1.0f - cos(angle));
	return vector;
}
#endif

#endif //MANDELBULBER2_OPENCL_ALGEBRA_HPP_
