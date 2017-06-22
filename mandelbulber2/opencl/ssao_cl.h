/*
 * ssao_cl.h
 *
 *  Created on: 22 cze 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_OPENCL_SSAO_CL_H_
#define MANDELBULBER2_OPENCL_SSAO_CL_H_

typedef struct
{
	cl_int width;
	cl_int height;
	cl_int quality;
	cl_float fov;
} sParamsSSAO;

#endif /* MANDELBULBER2_OPENCL_SSAO_CL_H_ */
