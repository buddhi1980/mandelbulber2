/**
 * Mandelbulber v2, a 3D fractal generator
 * Copyright (C) 2025 Mandelbulber Team
 *
 * OpenCL structures for nebula sequence data
 * derived from opencl/hybrid_sequence_cl.h
 */

#ifndef MANDELBULBER2_OPENCL_NEBULA_SEQUENCE_CL_H_
#define MANDELBULBER2_OPENCL_NEBULA_SEQUENCE_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "../src/fractal_enums.h"
#include "fractal_sequence_cl.h"
#endif

// OpenCL version of nebula sequence data (per-sequence scalar fields)
// Each field is a single value, not an array — indexed by sequence position
// in the dynamic array.
typedef struct
{
	// scalar fields from sClFractalSequence
	cl_float formulaWeight;
	enumDEFunctionTypeCl DEFunctionType;
	enumDETypeCl DEType;
	cl_int counts;
	cl_int formulaStartIteration;
	cl_int formulaStopIteration;
	cl_int addCConstant;
	cl_int checkForBailout;
	cl_float bailout;
	cl_int juliaEnabled;
	cl_float4 juliaConstant;
	cl_float4 constantMultiplier;
	cl_float initialWAxis;
	cl_int useAdditionalBailoutCond;
	cl_int formulaMaxiter;
	enumDEAnalyticFunctionCl DEAnalyticFunction;
	enumColoringFunctionCl coloringFunction;
} sNebulaSequenceCl;

#endif /* MANDELBULBER2_OPENCL_NEBULA_SEQUENCE_CL_H_ */
