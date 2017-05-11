

/*
 * test_engine.c
 *
 *  Created on: 5 maj 2017
 *      Author: krzysztof
 */

#define MAX_RAYMARCHING 500

typedef struct
{
	float3 z;
	float iters;
	float distance;
	float colourIndex;
} formulaOut;

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
	float r_dz = 1.0f;
	sClsExtendedAux aux;
	aux.r = length(z);
	aux.r_dz = 1.0f;

	__constant sClFractal *fractal = &consts->fractal[0];

	// loop
	for (i = 0; i < N; i++)
	{
		MandelbulbIteration(&z, fractal, &aux);
		// Mandelbulb4Iteration(&z, fractal, &aux);
		// Mandelbulb3Iteration(&z, &aux);

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

float3 NormalVector(__constant sClInConstants *consts, float3 point, float mainDistance,
	float distThresh, sClCalcParams *calcParam)
{
	float delta = distThresh;
	float s1 = CalculateDistance(consts, point + (float3){delta, 0.0f, 0.0f}, calcParam).distance;
	float s2 = CalculateDistance(consts, point + (float3){0.0f, delta, 0.0f}, calcParam).distance;
	float s3 = CalculateDistance(consts, point + (float3){0.0f, 0.0f, delta}, calcParam).distance;
	float3 normal = (float3){s1 - mainDistance, s2 - mainDistance, s3 - mainDistance};
	normal = normalize(normal);
	return normal;
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
	calcParam.N = consts->params.N;
	distThresh = 1e-6f;

	formulaOut outF;
	float step = 0.0f;

	// ray-marching
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

	// final binary searching
	if (found)
	{
		step *= 0.5f;
		for (int i = 0; i < 5; i++)
		{
			if (distance < distThresh && distance > distThresh * 0.95f)
			{
				break;
			}
			else
			{
				if (distance > distThresh)
				{
					point += viewVector * step;
				}
				else if (distance < distThresh * 0.95f)
				{
					point -= viewVector * step;
				}
			}
			outF = CalculateDistance(consts, point, &calcParam);
			distance = outF.distance;
			step *= 0.5f;
		}
	}

	float3 colour = 0.7f;
	float3 surfaceColour = 1.0f;
	if (found)
	{
		float3 normal = NormalVector(consts, point, distance, distThresh, &calcParam);

		float3 lightVector = (float3){
			cos(consts->params.mainLightAlpha - 0.5f * M_PI_F) * cos(-consts->params.mainLightBeta),
			sin(consts->params.mainLightAlpha - 0.5f * M_PI_F) * cos(-consts->params.mainLightBeta),
			sin(-consts->params.mainLightBeta)};
		lightVector = Matrix33MulFloat3(rot, lightVector);
		float shade = dot(lightVector, normal);
		if (shade < 0.0f) shade = 0.0f;

		float3 halfVector = lightVector - viewVector;
		halfVector = fast_normalize(halfVector);
		float specular = dot(normal, halfVector);
		if (specular < 0.0f) specular = 0.0f;
		specular = pown(specular, 30.0f);
		if (specular > 15.0f) specular = 15.0f;

		colour = colour * (shade + specular);
	}
	else
	{
		colour = 0.0f;
	}

	sClPixel pixel;

	float glow = count / 100.0 * consts->params.DEFactor;

	pixel.R = colour.s0 + glow;
	pixel.G = colour.s1 * glow * 10.0f;
	pixel.B = colour.s2;
	pixel.zBuffer = scan;
	pixel.colR = 0.0f;
	pixel.colG = 0.0f;
	pixel.colB = 0.0f;
	pixel.opacity = 0;
	pixel.alpha = 65535;

	out[buffIndex] = pixel;
}
