/*
 * fractal_sequence_cl.h
 *
 *  Created on: 16 maj 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_CL_H_
#define MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_CL_H_

#define OPENCL_FRACTAL_SEQUENCE_LENGTH 250

#ifndef OPENCL_KERNEL_CODE
#include "../src/fractal_enums.h"
#endif

typedef enum { analyticDEType, deltaDEType } enumDETypeCl;

typedef enum {
	linearDEFunction = 1,
	logarithmicDEFunction = 2,
} enumDEFunctionTypeCl;

typedef struct
{
	cl_int isHybrid;
	cl_int hybridSequence[OPENCL_FRACTAL_SEQUENCE_LENGTH];
	cl_float formulaWeight[NUMBER_OF_FRACTALS];
	enumDEFunctionTypeCl DEFunctionType[NUMBER_OF_FRACTALS];
	enumDETypeCl DEType[NUMBER_OF_FRACTALS];
	cl_int counts[NUMBER_OF_FRACTALS];
	cl_int formulaStartIteration[NUMBER_OF_FRACTALS];
	cl_int formulaStopIteration[NUMBER_OF_FRACTALS];
	cl_int addCConstant[NUMBER_OF_FRACTALS];
	cl_int checkForBailout[NUMBER_OF_FRACTALS];
	cl_float bailout[NUMBER_OF_FRACTALS];
	cl_int juliaEnabled[NUMBER_OF_FRACTALS];
	cl_float3 juliaConstant[NUMBER_OF_FRACTALS];
	cl_float3 constantMultiplier[NUMBER_OF_FRACTALS];
	cl_float initialWAxis[NUMBER_OF_FRACTALS];
	cl_int useAdditionalBailoutCond[NUMBER_OF_FRACTALS];
} sClFractalSequence;

#endif /* MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_CL_H_ */
