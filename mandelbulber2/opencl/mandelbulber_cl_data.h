//test version of cl_data

typedef struct
{
	cl_float3 m1;
	cl_float3 m2;
	cl_float3 m3;
} matrix33;

typedef struct
{
	cl_int width;
	cl_int height;
	cl_int N;
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
} sClCalcParams;

