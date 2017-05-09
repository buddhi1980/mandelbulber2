// test version of cl_data

typedef struct
{
	cl_float3 m1;
	cl_float3 m2;
	cl_float3 m3;
} matrix33;

typedef struct
{
	cl_int imageWidth;
	cl_int imageHeight;
	cl_int N;
	cl_float fov;
	cl_float DEFactor;
	cl_float3 camera;
	cl_float3 target;
	cl_float3 viewAngle;
} sClParams;

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
	sClParams params;
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
