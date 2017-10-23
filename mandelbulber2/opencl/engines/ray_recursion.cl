/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * contains the RayMarching function and the ray iteration function
 * used in the full_engine_cl
 */

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
	float3 point;
	float lastDist;
	float depth;
	float distThresh;
	int objectId;
	bool found;
	int count;
} sRayMarchingOut;

typedef enum {
	rayBranchReflection = 0,
	rayBranchRefraction = 1,
	rayBranchDone = 2,
} enumRayBranch;

typedef struct
{
	sRayMarchingIn rayMarchingIn;
	float4 resultShader;
	float3 objectColour;
	enumRayBranch rayBranch;
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
	sRayRecursionIn in;
	sRayRecursionOut out;
#ifdef USE_REFLECTANCE
	float4 reflectShader;
#endif
#ifdef USE_REFRACTION
	float4 transparentShader;
#endif
	enumRayBranch rayBranch;
	bool goDeeper;
} sRayStack;

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

#if defined(USE_REFRACTION) || defined(USE_REFLECTANCE)
float3 ReflectionVector(const float3 normal, const float3 incident)
{
	return incident - normal * dot(incident, normal) * 2.0f;
}
#endif

#ifdef USE_REFRACTION
float3 RefractVector(const float3 normal, const float3 incident, float n1, float n2)
{
	const float n = n1 / n2;
	const float cosI = -dot(normal, incident);
	const float sinT2 = n * n * (1.0f - cosI * cosI);
	if (sinT2 > 1.0f) return (float3){0.0f, 0.0f, 0.0f}; // total internal reflection
	const float cosT = sqrt(1.0f - sinT2);
	return incident * n + normal * (n * cosI - cosT);
}
#endif

#if defined(USE_REFRACTION) || defined(USE_REFLECTANCE)
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
#endif

void RayMarching(
	sRayMarchingIn in, sRayMarchingOut *out, __constant sClInConstants *consts, int *randomSeed)
{
	bool found = false;
	int count;

	float3 point;
	float scan, distance;

	scan = in.minScan;

	sClCalcParams calcParam;
	calcParam.N = consts->params.N;
	calcParam.normalCalculationMode = false;
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

#ifdef USE_REFRACTION
		if (in.invertMode)
		{
			distance = distThresh * 1.99 - distance;
			if (distance < 0.0) distance = 0.0;
		}
#endif

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

			//#ifdef USE_REFRACTION
			if (in.invertMode)
			{
				distance = distThresh * 1.99 - distance;
				if (distance < 0.0) distance = 0.0;
			}
			//#endif

			step *= 0.5f;
		}
	}

	out->found = found;
	out->lastDist = distance;
	out->depth = scan;
	out->distThresh = distThresh;
	out->point = point;
	out->count = count;
}

sRayRecursionOut RayRecursion(
	sRayRecursionIn in, sRenderData *renderData, __constant sClInConstants *consts, int *randomSeed)
{
	int rayIndex = 0; // level of recursion

	sRayStack rayStack[REFLECTIONS_MAX + 2];

	rayStack[rayIndex].in = in;
	rayStack[rayIndex].rayBranch = rayBranchReflection;
	rayStack[rayIndex].goDeeper = true;

	for (int i = 0; i < renderData->reflectionsMax + 1; i++)
	{
		rayStack[i].rayBranch = rayBranchReflection;

#ifdef USE_REFLECTANCE
		rayStack[i].reflectShader = 0.0f;
#endif
#ifdef USE_REFRACTION
		rayStack[i].transparentShader = 0.0f;
#endif
		rayStack[i].in.objectColour = 0.0f;
		rayStack[i].in.resultShader = 0.0f;
	}

	do
	{
		if (rayStack[rayIndex].goDeeper)
		{

			sRayMarchingOut rayMarchingOut;
			RayMarching(rayStack[rayIndex].in.rayMarchingIn, &rayMarchingOut, consts, randomSeed);
			float3 point = rayMarchingOut.point;

			bool found = rayMarchingOut.found;
			float depth = rayMarchingOut.depth;

			float distThresh = CalcDistThresh(point, consts);

			sClCalcParams calcParam;
			calcParam.N = consts->params.N;
			calcParam.normalCalculationMode = false;

			sShaderInputDataCl shaderInputData;
			shaderInputData.distThresh = distThresh;
			shaderInputData.delta = distThresh;
			shaderInputData.lightVect = renderData->lightVector;
			shaderInputData.point = point;
			shaderInputData.viewVector = rayStack[rayIndex].in.rayMarchingIn.direction;
			shaderInputData.viewVectorNotRotated = renderData->viewVectorNotRotated;
			shaderInputData.lastDist = rayMarchingOut.lastDist;
			shaderInputData.depth = rayMarchingOut.depth;
			shaderInputData.invertMode = rayStack[rayIndex].in.calcInside;
			shaderInputData.material = renderData->material;
			shaderInputData.palette = renderData->palette;
			shaderInputData.paletteSize = renderData->paletteSize;
			shaderInputData.AOVectors = renderData->AOVectors;
			shaderInputData.AOVectorsCount = renderData->AOVectorsCount;
			shaderInputData.lights = renderData->lights;
			shaderInputData.numberOfLights = renderData->numberOfLights;
			shaderInputData.stepCount = rayMarchingOut.count;
			shaderInputData.randomSeed = *randomSeed;

			float reflect = shaderInputData.material->reflectance;
			float transparent = shaderInputData.material->transparencyOfSurface;

			rayStack[rayIndex].out.rayMarchingOut = rayMarchingOut;

			// if found any object
			if (found)
			{
				sClCalcParams calcParam;
				calcParam.N = consts->params.N;
				calcParam.normalCalculationMode = false;
				calcParam.distThresh = shaderInputData.distThresh;
				calcParam.detailSize = shaderInputData.delta;

				float3 normal = NormalVector(consts, shaderInputData.point, shaderInputData.lastDist,
					shaderInputData.distThresh, shaderInputData.invertMode, &calcParam);
				shaderInputData.normal = normal;

				rayStack[rayIndex].out.normal = normal;

#if defined(USE_REFRACTION) || defined(USE_REFLECTANCE)
				// prepare refraction values
				float n1, n2;
				if (rayStack[rayIndex].in.calcInside) // if trace is inside the object
				{
					n1 =
						shaderInputData.material->transparencyIndexOfRefraction; // reverse refractive indices
					n2 = 1.0f;
				}
				else
				{
					n1 = 1.0f;
					n2 = shaderInputData.material->transparencyIndexOfRefraction;
				}

				if (rayIndex < renderData->reflectionsMax)
				{

					enumRayBranch rayBranch = rayStack[rayIndex].rayBranch;
					if (rayStack[rayIndex].rayBranch == rayBranchReflection)
					{
						// qDebug() << "Reflection" << rayIndex;
						rayStack[rayIndex].rayBranch = rayBranchRefraction;

#ifdef USE_REFLECTANCE
						// calculate reflection
						if (reflect > 0.0f)
						{
							rayIndex++; // increase recursion level

							sRayRecursionIn recursionIn;
							sRayMarchingIn rayMarchingIn;

							// calculate new direction of reflection
							float3 newDirection =
								ReflectionVector(normal, rayStack[rayIndex - 1].in.rayMarchingIn.direction);
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
							recursionIn.resultShader = rayStack[rayIndex - 1].in.resultShader;
							recursionIn.objectColour = rayStack[rayIndex - 1].in.objectColour;
							recursionIn.rayBranch = rayBranchReflection;

							// recursion for reflection
							rayStack[rayIndex].in = recursionIn;
							rayStack[rayIndex].goDeeper = true;
							rayStack[rayIndex].rayBranch = rayBranchReflection;

							continue;
						}
#endif
					}

					if (rayStack[rayIndex].rayBranch == rayBranchRefraction)
					{
						rayStack[rayIndex].rayBranch = rayBranchDone;

#ifdef USE_REFRACTION
						// calculate refraction (transparency)
						if (transparent > 0.0f)
						{
							rayIndex++; // increase recursion level

							sRayRecursionIn recursionIn;
							sRayMarchingIn rayMarchingIn;

							// calculate direction of refracted light
							float3 newDirection =
								RefractVector(normal, rayStack[rayIndex - 1].in.rayMarchingIn.direction, n1, n2);

							// move starting point a little
							float3 newPoint = point
																+ rayStack[rayIndex - 1].in.rayMarchingIn.direction
																		* shaderInputData.distThresh * 1.0f;

							// if is total internal reflection the use reflection instead of refraction
							bool internalReflection = false;
							if (length(newDirection) == 0.0f)
							{
								newDirection =
									ReflectionVector(normal, rayStack[rayIndex - 1].in.rayMarchingIn.direction);
								newPoint = point
													 + rayStack[rayIndex - 1].in.rayMarchingIn.direction
															 * shaderInputData.distThresh * 1.0f;
								internalReflection = true;
							}

							// preparation for new recursion
							rayMarchingIn.binaryEnable = true;
							rayMarchingIn.direction = newDirection;
							rayMarchingIn.maxScan = consts->params.viewDistanceMax;
							rayMarchingIn.minScan = 0.0f;
							rayMarchingIn.start = newPoint;
							rayMarchingIn.invertMode =
								!rayStack[rayIndex - 1].in.calcInside || internalReflection;
							recursionIn.rayMarchingIn = rayMarchingIn;
							recursionIn.calcInside = !rayStack[rayIndex - 1].in.calcInside || internalReflection;
							recursionIn.resultShader = rayStack[rayIndex - 1].in.resultShader;
							recursionIn.objectColour = rayStack[rayIndex - 1].in.objectColour;
							recursionIn.rayBranch = rayBranchRefraction;

							// recursion for refraction
							rayStack[rayIndex].in = recursionIn;
							rayStack[rayIndex].goDeeper = true;
							rayStack[rayIndex].rayBranch = rayBranchReflection;

							continue;
						}
#endif
					}

					if (rayStack[rayIndex].rayBranch == rayBranchDone)
					{
						rayStack[rayIndex].goDeeper = false;
					}
				} // reflectionsMax

				else
				{
					rayStack[rayIndex].goDeeper = false;
				}
#else // defined(USE_REFRACTION) || defined(USE_REFLECTANCE)
				rayStack[rayIndex].goDeeper = false;
#endif
			} // found
			else
			{
				rayStack[rayIndex].goDeeper = false;
			}
		} // goDeeper

		if (!rayStack[rayIndex].goDeeper)
		{
			sRayRecursionOut recursionOut;

			recursionOut = rayStack[rayIndex].out;

			sRayMarchingOut rayMarchingOut = recursionOut.rayMarchingOut;

			float3 point = rayMarchingOut.point;

#ifdef USE_REFLECTANCE
			float4 reflectShader = rayStack[rayIndex].reflectShader;
#endif
#ifdef USE_REFRACTION
			float4 transparentShader = rayStack[rayIndex].transparentShader;
#endif

			float distThresh = CalcDistThresh(point, consts);

			sShaderInputDataCl shaderInputData;
			shaderInputData.distThresh = distThresh;
			shaderInputData.delta = distThresh;
			shaderInputData.lightVect = renderData->lightVector;
			shaderInputData.point = point;
			shaderInputData.viewVector = rayStack[rayIndex].in.rayMarchingIn.direction;
			shaderInputData.viewVectorNotRotated = renderData->viewVectorNotRotated;
			shaderInputData.lastDist = rayMarchingOut.lastDist;
			shaderInputData.depth = rayMarchingOut.depth;
			shaderInputData.invertMode = rayStack[rayIndex].in.calcInside;
			shaderInputData.material = renderData->material;
			shaderInputData.palette = renderData->palette;
			shaderInputData.paletteSize = renderData->paletteSize;
			shaderInputData.AOVectors = renderData->AOVectors;
			shaderInputData.AOVectorsCount = renderData->AOVectorsCount;
			shaderInputData.lights = renderData->lights;
			shaderInputData.numberOfLights = renderData->numberOfLights;
			shaderInputData.stepCount = rayMarchingOut.count;
			shaderInputData.randomSeed = *randomSeed;

			sClCalcParams calcParam;
			calcParam.N = consts->params.N;
			calcParam.normalCalculationMode = false;
			calcParam.distThresh = shaderInputData.distThresh;
			calcParam.detailSize = shaderInputData.distThresh;
			calcParam.randomSeed = *randomSeed;

#ifdef USE_REFLECTANCE
			float reflect = shaderInputData.material->reflectance;
#endif
#ifdef USE_REFRACTION
			float transparent = shaderInputData.material->transparencyOfSurface;
#endif

			float4 resultShader = rayStack[rayIndex].in.resultShader;
			float3 objectColour = rayStack[rayIndex].in.objectColour;

#ifdef USE_REFRACTION
			float4 transparentColor = (float4){shaderInputData.material->transparencyInteriorColor.s0,
				shaderInputData.material->transparencyInteriorColor.s1,
				shaderInputData.material->transparencyInteriorColor.s2, 0.0};
			resultShader = transparentColor;
#endif

			float3 objectShader;
			float3 backgroundShader;
			float3 volumetricShader;
			float3 specular;

			shaderInputData.normal = recursionOut.normal;

			if (rayMarchingOut.found)
			{
				specular = 0.0f;
				objectShader = ObjectShader(consts, &shaderInputData, &calcParam, &objectColour, &specular);

// calculate reflectance according to Fresnel equations

#if defined(USE_REFRACTION) || defined(USE_REFLECTANCE)
				// prepare refraction values
				float n1, n2;
				if (rayStack[rayIndex].in.calcInside) // if trace is inside the object
				{
					n1 =
						shaderInputData.material->transparencyIndexOfRefraction; // reverse refractive indices
					n2 = 1.0f;
				}
				else
				{
					n1 = 1.0f;
					n2 = shaderInputData.material->transparencyIndexOfRefraction;
				}

				float reflectance = 1.0f;
				float reflectanceN = 1.0f; // TODO to check if 1.0 is correct value

				if (shaderInputData.material->fresnelReflectance)
				{
					reflectance = Reflectance(
						shaderInputData.normal, rayStack[rayIndex].in.rayMarchingIn.direction, n1, n2);
					if (reflectance < 0.0f) reflectance = 0.0f;
					if (reflectance > 1.0f) reflectance = 1.0f;
					reflectanceN = 1.0f - reflectance;
				}

				if (rayIndex == renderData->reflectionsMax)
				{
					reflectance = 0.0f;
					reflectanceN = 1.0f;
				}
#endif

				// combine all results
				resultShader.xyz = (objectShader);

#if defined(USE_REFRACTION) || defined(USE_REFLECTANCE)
				if (renderData->reflectionsMax > 0)
				{
#ifdef USE_REFRACTION
					resultShader = transparentShader * transparent * reflectanceN
												 + (1.0f - transparent * reflectanceN) * resultShader;
#endif

#ifdef USE_REFLECTANCE
					resultShader =
						reflectShader * reflect * reflectance + (1.0f - reflect * reflectance) * resultShader;
#endif
				}
#endif
				resultShader = max(resultShader, 0.0f);
				resultShader.w = 1.0f;
			}
			else
			{
				backgroundShader = BackgroundShader(consts, &shaderInputData);
				resultShader.xyz = backgroundShader;
				resultShader.w = 0.0f;
				rayMarchingOut.depth = 1e20f;
				// vn = mRot.RotateVector(CVector3(0.0, -1.0, 0.0));
			}

			float opacityOut = 0.0f;

#ifdef USE_REFRACTION
			float step = rayMarchingOut.depth / shaderInputData.stepCount;
			if (rayStack[rayIndex]
						.in.calcInside) // if the object interior is traced, then the absorption of light has
														// to
														// be
														// calculated
			{
				for (int index = shaderInputData.stepCount - 1; index > 0; index--)
				{

					float opacity = (-1.0 + 1.0 / shaderInputData.material->transparencyOfInterior) * step;
					if (opacity > 1.0) opacity = 1.0;

					resultShader = opacity * transparentColor + (1.0 - opacity) * resultShader;
				}
			}
			else
#endif
			{
				resultShader =
					VolumetricShader(consts, &shaderInputData, &calcParam, resultShader, &opacityOut);
			}

			recursionOut.point = point;
			recursionOut.rayMarchingOut = rayMarchingOut;
			recursionOut.objectColour = objectColour;
			recursionOut.resultShader = resultShader;
			recursionOut.found = rayMarchingOut.found;
			recursionOut.fogOpacity = opacityOut;
			recursionOut.normal = shaderInputData.normal;

			rayStack[rayIndex].out = recursionOut;

#if defined(USE_REFRACTION) || defined(USE_REFLECTANCE)
			if (rayIndex > 0)
			{
				if (rayStack[rayIndex].in.rayBranch == rayBranchReflection)
				{
#ifdef USE_REFLECTANCE
					rayStack[rayIndex - 1].reflectShader = resultShader;
#endif
				}
				else
				{
#ifdef USE_REFRACTION
					rayStack[rayIndex - 1].transparentShader = resultShader;
#endif
				}
				rayStack[rayIndex - 1].in.resultShader = resultShader;
				rayStack[rayIndex - 1].in.objectColour = objectColour;
			}
#endif

			rayIndex--;
		}
		// prepare final result

	} while (rayIndex >= 0);

	sRayRecursionOut out = rayStack[0].out;

	return out;
}
