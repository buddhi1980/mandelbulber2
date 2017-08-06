typedef struct
{
	float3 start;
	float3 direction;
	float minScan;
	float maxScan;
	bool binaryEnable;
	bool invertMode;
} sRayMarchingIn;

typedef struct
{
	float lastDist;
	float depth;
	float distThresh;
	int objectId;
	bool found;
	int count;
} sRayMarchingOut;

typedef struct
{
	sRayMarchingIn rayMarchingIn;
	float4 resultShader;
	float3 objectColour;
	bool calcInside;
} sRayRecursionIn;

typedef struct
{
	sRayMarchingOut rayMarchingOut;
	float3 point;
	float4 resultShader;
	float3 objectColour;
	float3 normal;
	float fogOpacity;
	bool found;
} sRayRecursionOut;

typedef struct
{
	int rayIndex;
} sRayRecursionInOut;

typedef struct
{
	float3 lightVector;
	float3 viewVectorNotRotated;
	__global sMaterialCl *material;
	__global float4 *palette;
	__global sVectorsAroundCl *AOVectors;
	__global sLightCl *lights;
	int reflectionsMax;
	int paletteSize;
	int numberOfLights;
	int AOVectorsCount;

} sRenderData;

float3 ReflectionVector(const float3 normal, const float3 incident)
{
	return incident - normal * dot(incident, normal) * 2.0f;
}

float3 RefractVector(const float3 normal, const float3 incident, float n1, float n2)
{
	const float n = n1 / n2;
	const float cosI = -dot(normal, incident);
	const float sinT2 = n * n * (1.0f - cosI * cosI);
	if (sinT2 > 1.0f) return (float3){0.0f, 0.0f, 0.0f}; // total internal reflection
	const float cosT = sqrt(1.0f - sinT2);
	return incident * n + normal * (n * cosI - cosT);
}

float Reflectance(const float3 normal, const float3 incident, float n1, float n2)
{
	const float n = n1 / n2;
	const float cosI = -dot(normal, incident);
	const float sinT2 = n * n * (1.0f - cosI * cosI);
	if (sinT2 > 1.0f) return 1.0f; // total internal reflection
	const float cosT = sqrt(1.0f - sinT2);
	const float r0rth = (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
	const float rPar = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);
	return (r0rth * r0rth + rPar * rPar) / 2.0f;
}

float3 RayMarching(
	sRayMarchingIn in, sRayMarchingOut *out, __constant sClInConstants *consts, int *randomSeed)
{
	bool found = false;
	int count;

	float3 point;
	float scan, distance;

	scan = in.minScan;

	sClCalcParams calcParam;
	calcParam.N = consts->params.N;
	float distThresh = 1e-6f;

	formulaOut outF;
	float step = 0.0f;

	float searchAccuracy = 0.001f * consts->params.detailLevel;
	float searchLimit = 1.0f - searchAccuracy;

	// ray-marching
	for (count = 0; count < MAX_RAYMARCHING && scan < in.maxScan; count++)
	{
		point = in.start + in.direction * scan;
		distThresh = CalcDistThresh(point, consts);
		calcParam.distThresh = distThresh;
		calcParam.detailSize = distThresh;
		outF = CalculateDistance(consts, point, &calcParam);
		distance = outF.distance;

		if (distance < distThresh)
		{
			found = true;
			break;
		}

#ifdef INTERIOR_MODE
		step = (distance - 0.8f * distThresh) * consts->params.DEFactor;
#else
		step = (distance - 0.5f * distThresh) * consts->params.DEFactor;
#endif

		step *= (1.0f - Random(1000, randomSeed) / 10000.0f);

		scan += step / length(in.direction);
	}

	point = in.start + in.direction * scan;

	// final binary searching
	if (found)
	{
		step *= 0.5f;
		for (int i = 0; i < 30; i++)
		{
			if (distance < distThresh && distance > distThresh * searchLimit)
			{
				break;
			}
			else
			{
				if (distance > distThresh)
				{
					scan += step;
					point = in.start + in.direction * scan;
				}
				else if (distance < distThresh * searchLimit)
				{
					scan -= step;
					point = in.start + in.direction * scan;
				}
			}
			outF = CalculateDistance(consts, point, &calcParam);
			distance = outF.distance;
			step *= 0.5f;
		}
	}

	out->found = found;
	out->lastDist = distance;
	out->depth = scan;
	out->distThresh = distThresh;
	out->count = count;

	return point;
}

sRayRecursionOut RayRecursion(sRayRecursionIn in, sRayRecursionInOut *inOut,
	sRenderData *renderData, __constant sClInConstants *consts, int *randomSeed)
{
	sRayMarchingOut rayMarchingOut;
	float3 point = RayMarching(in.rayMarchingIn, &rayMarchingOut, consts, randomSeed);
	bool found = rayMarchingOut.found;
	float depth = rayMarchingOut.depth;

	float4 resultShader = in.resultShader;
	float3 objectColour = in.objectColour;

	float3 objectShader;
	float3 backgroundShader;
	float4 volumetricShader;
	float3 specular;

	float distThresh = CalcDistThresh(point, consts);

	sClCalcParams calcParam;
	calcParam.N = consts->params.N;

	sShaderInputDataCl shaderInputData;
	shaderInputData.distThresh = distThresh;
	shaderInputData.delta = CalcDelta(point, consts);
	shaderInputData.lightVect = renderData->lightVector;
	shaderInputData.point = point;
	shaderInputData.viewVector = in.rayMarchingIn.direction;
	shaderInputData.viewVectorNotRotated = renderData->viewVectorNotRotated;
	shaderInputData.lastDist = rayMarchingOut.lastDist;
	shaderInputData.depth = rayMarchingOut.depth;
	shaderInputData.invertMode = in.calcInside;
	shaderInputData.material = renderData->material;
	shaderInputData.palette = renderData->palette;
	shaderInputData.paletteSize = renderData->paletteSize;
	shaderInputData.AOVectors = renderData->AOVectors;
	shaderInputData.AOVectorsCount = renderData->AOVectorsCount;
	shaderInputData.lights = renderData->lights;
	shaderInputData.numberOfLights = renderData->numberOfLights;
	shaderInputData.stepCount = rayMarchingOut.count;
	shaderInputData.randomSeed = *randomSeed;

	float4 reflectShader = in.resultShader;
	float reflect = shaderInputData.material->reflectance;

	float4 transparentShader = in.resultShader;
	float transparent = shaderInputData.material->transparencyOfSurface;
	float4 transparentColor = (float4){shaderInputData.material->transparencyInteriorColor.s0,
		shaderInputData.material->transparencyInteriorColor.s1,
		shaderInputData.material->transparencyInteriorColor.s2, 0.0};
	resultShader = transparentColor;

	float3 vn;

	if (found)
	{
		sClCalcParams calcParam;
		calcParam.N = consts->params.N;
		calcParam.distThresh = shaderInputData.distThresh;
		calcParam.detailSize = shaderInputData.delta;

		float3 normal = NormalVector(consts, shaderInputData.point, shaderInputData.lastDist,
			shaderInputData.distThresh, &calcParam);
		shaderInputData.normal = normal;

		// prepare refraction values
		float n1, n2;
		if (in.calcInside) // if trace is inside the object
		{
			n1 = shaderInputData.material->transparencyIndexOfRefraction; // reverse refractive indices
			n2 = 1.0f;
		}
		else
		{
			n1 = 1.0f;
			n2 = shaderInputData.material->transparencyIndexOfRefraction;
		}

		if (inOut->rayIndex < renderData->reflectionsMax)
		{
			// calculate refraction (transparency)
			if (transparent > 0.0f)
			{
				sRayRecursionIn recursionIn;
				sRayMarchingIn rayMarchingIn;

				// calculate direction of refracted light
				float3 newDirection = RefractVector(vn, in.rayMarchingIn.direction, n1, n2);

				// move starting point a little
				float3 newPoint = point + in.rayMarchingIn.direction * shaderInputData.distThresh * 1.0f;

				// if is total internal reflection the use reflection instead of refraction
				bool internalReflection = false;
				if (length(newDirection) == 0.0f)
				{
					newDirection = ReflectionVector(vn, in.rayMarchingIn.direction);
					newPoint = point + in.rayMarchingIn.direction * shaderInputData.distThresh * 1.0f;
					internalReflection = true;
				}

				// preparation for new recursion
				rayMarchingIn.binaryEnable = true;
				rayMarchingIn.direction = newDirection;
				rayMarchingIn.maxScan = consts->params.viewDistanceMax;
				rayMarchingIn.minScan = 0.0f;
				rayMarchingIn.start = newPoint;
				rayMarchingIn.invertMode = !in.calcInside || internalReflection;
				recursionIn.rayMarchingIn = rayMarchingIn;
				recursionIn.calcInside = !in.calcInside || internalReflection;
				recursionIn.resultShader = resultShader;
				recursionIn.objectColour = objectColour;

				// setup buffers for ray data
				inOut->rayIndex++; // increase recursion index

				// recursion for refraction
				sRayRecursionOut recursionOutTransparent =
					RayRecursion(recursionIn, inOut, renderData, consts, randomSeed);
				transparentShader = recursionOutTransparent.resultShader;
			}

			// calculate reflection
			if (reflect > 0.0f)
			{
				sRayRecursionIn recursionIn;
				sRayMarchingIn rayMarchingIn;

				// calculate new direction of reflection
				float3 newDirection = ReflectionVector(vn, in.rayMarchingIn.direction);
				float3 newPoint = point + newDirection * shaderInputData.distThresh;

				// prepare for new recursion
				rayMarchingIn.binaryEnable = true;
				rayMarchingIn.direction = newDirection;
				rayMarchingIn.maxScan = consts->params.viewDistanceMax;
				rayMarchingIn.minScan = 0.0f;
				rayMarchingIn.start = newPoint;
				rayMarchingIn.invertMode = false;
				recursionIn.rayMarchingIn = rayMarchingIn;
				recursionIn.calcInside = false;
				recursionIn.resultShader = resultShader;
				recursionIn.objectColour = objectColour;

				// setup buffers for ray data
				inOut->rayIndex++; // increase recursion index

				// recursion for reflection
				sRayRecursionOut recursionOutReflect =
					RayRecursion(recursionIn, inOut, renderData, consts, randomSeed);
				reflectShader = recursionOutReflect.resultShader;
			}

			if (transparent > 0.0f) inOut->rayIndex--; // decrease recursion index
			if (reflect > 0.0f) inOut->rayIndex--;		 // decrease recursion index
		}

		shaderInputData.normal = vn;

		float3 specular = 0.0f;
		objectShader = ObjectShader(consts, &shaderInputData, &calcParam, &objectColour, &specular);

		// calculate reflectance according to Fresnel equations
		float reflectance = 1.0f;
		float reflectanceN = 1.0f;

		if (shaderInputData.material->fresnelReflectance)
		{
			reflectance = Reflectance(vn, in.rayMarchingIn.direction, n1, n2);
			if (reflectance < 0.0f) reflectance = 0.0f;
			if (reflectance > 1.0f) reflectance = 1.0f;
			reflectanceN = 1.0f - reflectance;
		}

		if (inOut->rayIndex == renderData->reflectionsMax - 1) reflectance = 0.0;

		// combine all results
		resultShader.xyz = (objectShader + specular);

		if (renderData->reflectionsMax > 0)
		{
			resultShader = transparentShader * transparent * reflectanceN
										 + (1.0 - transparent * reflectanceN) * resultShader;
			resultShader =
				reflectShader * reflect * reflectance + (1.0 - reflect * reflectance) * resultShader;
		}
		resultShader = max(resultShader, 0.0f);
	}
	else
	{
		backgroundShader = BackgroundShader(consts, &shaderInputData);
		resultShader.xyz = backgroundShader;
		shaderInputData.depth = 1e20f;
		// vn = mRot.RotateVector(CVector3(0.0, -1.0, 0.0));
	}

	float opacityOut;

	if (in.calcInside) // if the object interior is traced, then the absorption of light has to be
										 // calculated
	{
		for (int index = shaderInputData.stepCount - 1; index > 0; index--)
		{
			//			double step = shaderInputData.stepBuff[index].step;
			//
			//			double opacity = (-1.0 + 1.0 / shaderInputData.material->transparencyOfInterior) *
			// step;
			//			if (opacity > 1.0) opacity = 1.0;
			//
			//			resultShader.R = opacity * transparentColor.R + (1.0 - opacity) * resultShader.R;
			//			resultShader.G = opacity * transparentColor.G + (1.0 - opacity) * resultShader.G;
			//			resultShader.B = opacity * transparentColor.B + (1.0 - opacity) * resultShader.B;
		}
	}
	else
	{
		float4 color4 = (float4){resultShader.s0, resultShader.s1, resultShader.s2, 0.0f};
		resultShader = VolumetricShader(consts, &shaderInputData, &calcParam, color4, &opacityOut);
	}

	// prepare final result
	sRayRecursionOut out;
	out.point = point;
	out.rayMarchingOut = rayMarchingOut;
	out.objectColour = objectColour;
	out.resultShader = resultShader;
	out.found = (shaderInputData.depth == 1e20f) ? false : true;
	out.fogOpacity = opacityOut;
	out.normal = vn;

	return out;
}
