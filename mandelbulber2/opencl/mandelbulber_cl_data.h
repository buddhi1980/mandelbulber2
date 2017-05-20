#ifndef OPENCL_KERNEL_CODE
#include "../opencl/fractparams_cl.hpp"
#include "../opencl/fractal_cl.h"
#include "../opencl/fractal_sequence_cl.h"
#endif

#ifndef MANDELBULBER_OPENCL_DATA
#define MANDELBULBER_OPENCL_DATA

typedef struct
{
	cl_float R;
	cl_float G;
	cl_float B;
	cl_float zBuffer;
	cl_ushort opacity;
	cl_ushort alpha;
	cl_uchar colR;
	cl_uchar colG;
	cl_uchar colB;
} sClPixel;

typedef struct
{
	sParamRenderCl params;
	sFractalCl fractal[NUMBER_OF_FRACTALS]; // temporary for testing
	sClFractalSequence sequence;
} sClInConstants;

typedef struct
{
	cl_int dummy[256];
} sClInBuff;

typedef struct
{
	cl_uint N;
	cl_int randomSeed;
	cl_float3 orbitTrap;
	cl_float distThresh;
	cl_float normalCalculationMode;
} sClCalcParams;

#endif // MANDELBULBER_OPENCL_DATA
