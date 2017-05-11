/*
 * opencl_typedefs.h
 *
 *  Created on: 10 maj 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_OPENCL_OPENCL_TYPEDEFS_H_
#define MANDELBULBER2_OPENCL_OPENCL_TYPEDEFS_H_

typedef float4 cl_float4;
typedef float3 cl_float3;
typedef float cl_float;
typedef int3 cl_int3;
typedef int cl_int;
typedef unsigned int cl_uint;
typedef unsigned short cl_ushort;
typedef unsigned char cl_uchar;
typedef struct {
	cl_float3 m1;
	cl_float3 m2;
	cl_float3 m3;
} matrix33;
#endif /* MANDELBULBER2_OPENCL_OPENCL_TYPEDEFS_H_ */
