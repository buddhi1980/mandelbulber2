/**
 * Mandelbulber v2, a 3D fractal generator
 * Copyright (C) 2017-22 Mandelbulber Team
 *
 * OpenCL enum types for fractal DE and coloring
 * Derived from src/nine_fractal.*
 */

#ifndef MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_CL_H_
#define MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_CL_H_

typedef enum
{
	analyticDEType,
	deltaDEType
} enumDETypeCl;

typedef enum
{
	linearDEFunction = 1,
	logarithmicDEFunction = 2,
	pseudoKleinianDEFunction = 3,
	josKleinianDEFunction = 4,
	customDEFunction = 5,
	maxAxisDEFunction = 6,
} enumDEFunctionTypeCl;

typedef enum
{
	clAnalyticFunctionUndefined = -1,
	clAnalyticFunctionNone = 0,
	clAnalyticFunctionLinear = 1,
	clAnalyticFunctionLogarithmic = 2,
	clAnalyticFunctionIFS = 3,
	clAnalyticFunctionPseudoKleinian = 4,
	clAnalyticFunctionJosKleinian = 5,
	clAnalyticFunctionCustomDE = 6,
	clAnalyticFunctionMaxAxis = 7,
} enumDEAnalyticFunctionCl;

typedef enum
{
	clColoringFunctionUndefined = -1,
	clColoringFunctionDefault = 0,
	clColoringFunctionABox = 1,
	clColoringFunctionIFS = 2,
	clColoringFunctionAmazingSurf = 3,
	clColoringFunctionABox2 = 4,
	clColoringFunctionDonut = 5,
} enumColoringFunctionCl;

#endif /* MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_CL_H_ */
