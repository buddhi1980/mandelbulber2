#include "../opencl/fractparams_cl.hpp"

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

// temporary struct for mandelbulb
typedef struct
{
	cl_float power;
	cl_float alphaAngleOffset;
	cl_float betaAngleOffset;
	cl_float gammaAngleOffset;
} sClFractalMandelbulb;

// temporary struct for fractal
typedef struct
{
	sClFractalMandelbulb bulb;
} sClFractal;

typedef struct
{
	cl_float r;
	cl_float r_dz;
} sClsExtendedAux;

typedef struct
{
	sParamRenderCl params;
	sClFractal fractal[9]; // temporary for testing
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
