

/*
 * test_engine.c
 *
 *  Created on: 5 maj 2017
 *      Author: krzysztof
 */

typedef float3 cl_float3;
typedef float cl_float;
typedef int cl_int;
typedef unsigned int cl_uint;
typedef unsigned short cl_ushort;
typedef unsigned char cl_uchar;

#include INCLUDE_PATH_CL_DATA

#define MAX_RAYMARCHING 500

typedef struct
{
	float3 z;
	float iters;
	float distance;
	float colourIndex;
} formulaOut;

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

formulaOut Fractal(__constant sClInConstants *consts, float3 point, sClCalcParams *calcParam)
{
	// begin
	float dist = 0.0f;
	int N = calcParam->N;
	float3 z = point;
	float3 c = point;
	int i;
	formulaOut out;
	float colourMin = 1e8f;

	// formula init
	sClsExtendedAux aux;
	aux.r = length(z);
	aux.r_dz = 1.0f;

	sCLFractal fractal;
	fractal.bulb.alphaAngleOffset = 0;
	fractal.bulb.betaAngleOffset = 0;
	fractal.bulb.power = 7;

	// loop
	for (i = 0; i < N; i++)
	{
		MandelbulbIteration(&z, &fractal, &aux);

		z += c;
		aux.r = length(z);

		if (aux.r < colourMin) colourMin = aux.r;

		if (aux.r > 4.0f || any(isinf(z)))
		{
			dist = 0.5f * aux.r * native_log(aux.r) / (aux.r_dz);
			out.colourIndex = colourMin * 5000.0f;
			break;
		}
	}

	// end
	if (dist < 0.0f) dist = 0.0f;
	out.distance = dist;
	out.iters = i;
	out.z = z;

	return out;
}

formulaOut CalculateDistance(
	__constant sClInConstants *consts, float3 point, sClCalcParams *calcParam)
{
	formulaOut out;
	out.z = 0.0f;
	out.iters = 0;
	out.distance = 0.0f;
	out.colourIndex = 0.0f;

	out = Fractal(consts, point, calcParam);

	if (out.iters == calcParam->N)
	{
		out.distance = 0.0f;
	}
	else
	{
		if (isinf(out.distance)) out.distance = 0.0f;
		if (out.distance < 0.0f) out.distance = 0.0f;
		if (out.distance > 10.0f) out.distance = 10.0f;
	}

	return out;
}

//------------------ MAIN RENDER FUNCTION --------------------
kernel void fractal3D(__global sClPixel *out, __global sClInBuff *inBuff,
	__constant sClInConstants *consts, int Gcl_offset)
{
	int cl_offset = Gcl_offset;

	// get actual pixel
	const int id = get_global_id(0) + cl_offset;
	const int imageX = id % consts->params.imageWidth;
	const int imageYtemp = id / consts->params.imageWidth;
	const int buffIndex = (id - cl_offset);

	const int imageY = clamp(imageYtemp, 0, consts->params.imageHeight - 1);

	float2 screenPoint = (float2){convert_float(imageX), convert_float(imageY)};
	float width = convert_float(consts->params.imageWidth);
	float height = convert_float(consts->params.imageHeight);
	float resolution = 1.0f / width;

	// axiliary vectors
	const float3 one = (float3){1.0f, 0.0f, 0.0f};
	const float3 ones = 1.0f;

	// main rotation matrix
	matrix33 rot;
	rot.m1 = (float3){1.0f, 0.0f, 0.0f};
	rot.m2 = (float3){0.0f, 1.0f, 0.0f};
	rot.m3 = (float3){0.0f, 0.0f, 1.0f};

	rot = RotateZ(rot, consts->params.viewAngle.x);
	rot = RotateX(rot, consts->params.viewAngle.y);
	rot = RotateY(rot, consts->params.viewAngle.z);

	// starting point for ray-marching
	float3 start = consts->params.camera;

	// view vector
	float aspectRatio = width / height;
	float x2, z2;
	x2 = (screenPoint.x / width - 0.5f) * aspectRatio;
	z2 = -(screenPoint.y / height - 0.5f);
	float3 viewVector = (float3){x2 * consts->params.fov, 1.0f, z2 * consts->params.fov};
	viewVector = Matrix33MulFloat3(rot, viewVector);

	bool found = false;
	int count;

	float3 point;
	float scan, distThresh, distance;

	scan = 1e-10f;

	sClCalcParams calcParam;
	calcParam.N = 9.0f; // hardcoded for testing
		distThresh = 1e-6f;

	formulaOut outF;
	float step = 0.0f;

	for (count = 0; count < MAX_RAYMARCHING && scan < 50.0f; count++)
	{
		point = start + viewVector * scan;
		calcParam.distThresh = distThresh;
		outF = CalculateDistance(consts, point, &calcParam);
		distance = outF.distance;
		distThresh = scan * resolution * consts->params.fov;

		if (distance < distThresh)
		{
			found = true;
			break;
		}

		step = (distance - 0.5f * distThresh) * consts->params.DEFactor;
		scan += step;
	}

	sClPixel pixel;

	pixel.R = count / 25.0f;
	pixel.G = count / 250.0f;
	pixel.B = count / 250.0f;
	pixel.zBuffer = scan;
	pixel.colR = 0.0f;
	pixel.colG = 0.0f;
	pixel.colB = 0.0f;
	pixel.opacity = 0;
	pixel.alpha = 65535;

	out[buffIndex] = pixel;
}
