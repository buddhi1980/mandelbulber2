/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

typedef enum
{
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

void RayMarching(sRayMarchingIn in, sRayMarchingOut *out, __constant sClInConstants *consts,
	sRenderData *renderData, int *randomSeed)
{
	bool found = false;
	int count;

	float3 point;
	float scan, distance;

	scan = in.minScan;

	sClCalcParams calcParam;
	calcParam.N = consts->params.N;
	calcParam.normalCalculationMode = false;
	calcParam.iterThreshMode = consts->params.iterThreshMode;
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
		outF = CalculateDistance(consts, point, &calcParam, renderData);
		distance = outF.distance;
		out->objectId = outF.objectId;

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

#ifdef ADVANCED_QUALITY
		step = clamp(step, consts->params.absMinMarchingStep, consts->params.absMaxMarchingStep);

		if (distThresh > consts->params.absMinMarchingStep)
			step = clamp(step, consts->params.relMinMarchingStep * distThresh,
				consts->params.relMaxMarchingStep * distThresh);
#endif

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
			outF = CalculateDistance(consts, point, &calcParam, renderData);
			distance = outF.distance;
			out->objectId = outF.objectId;

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

sRayRecursionOut RayRecursion(sRayRecursionIn in, sRenderData *renderData,
	__constant sClInConstants *consts, image2d_t image2dBackground, int *randomSeed)
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
			RayMarching(
				rayStack[rayIndex].in.rayMarchingIn, &rayMarchingOut, consts, renderData, randomSeed);
			float3 point = rayMarchingOut.point;

			bool found = rayMarchingOut.found;
			float depth = rayMarchingOut.depth;

			float distThresh = CalcDistThresh(point, consts);

			sClCalcParams calcParam;
			calcParam.N = consts->params.N;
			calcParam.normalCalculationMode = false;
			calcParam.iterThreshMode = consts->params.iterThreshMode;

			sShaderInputDataCl shaderInputData;
			shaderInputData.distThresh = distThresh;
			shaderInputData.delta = CalcDelta(point, consts);
			shaderInputData.lightVect = renderData->lightVector;
			shaderInputData.point = point;
			shaderInputData.viewVector = rayStack[rayIndex].in.rayMarchingIn.direction;
			shaderInputData.viewVectorNotRotated = renderData->viewVectorNotRotated;
			shaderInputData.lastDist = rayMarchingOut.lastDist;
			shaderInputData.depth = rayMarchingOut.depth;
			shaderInputData.invertMode = rayStack[rayIndex].in.calcInside;
#if (defined(BOOLEAN_OPERATORS) || defined(USE_PRIMITIVES))
			shaderInputData.objectId = rayMarchingOut.objectId;
#else
			shaderInputData.objectId = 0;
#endif
			__global sObjectDataCl *objectData = &renderData->objectsData[shaderInputData.objectId];
			shaderInputData.material = renderData->materials[objectData->materialId];
			shaderInputData.palette = renderData->palettes[objectData->materialId];
#ifdef USE_SURFACE_GRADIENT
			shaderInputData.paletteSurfaceOffset =
				renderData->paletteSurfaceOffsets[objectData->materialId];
			shaderInputData.paletteSurfaceLength =
				renderData->paletteSurfaceLengths[objectData->materialId];
#endif
#ifdef USE_SPECULAR_GRADIENT
			shaderInputData.paletteSpecularOffset =
				renderData->paletteSpecularOffsets[objectData->materialId];
			shaderInputData.paletteSpecularLength =
				renderData->paletteSpecularLengths[objectData->materialId];
#endif
#ifdef USE_DIFFUSE_GRADIENT
			shaderInputData.paletteDiffuseOffset =
				renderData->paletteDiffuseOffsets[objectData->materialId];
			shaderInputData.paletteDiffuseLength =
				renderData->paletteDiffuseLengths[objectData->materialId];
#endif
#ifdef USE_LUMINOSITY_GRADIENT
			shaderInputData.paletteLuminosityOffset =
				renderData->paletteLuminosityOffsets[objectData->materialId];
			shaderInputData.paletteLuminosityLength =
				renderData->paletteLuminosityLengths[objectData->materialId];
#endif
#ifdef USE_ROUGHNESS_GRADIENT
			shaderInputData.paletteRoughnessOffset =
				renderData->paletteRoughnessOffsets[objectData->materialId];
			shaderInputData.paletteRoughnessLength =
				renderData->paletteRoughnessLengths[objectData->materialId];
#endif
#ifdef USE_REFLECTANCE_GRADIENT
			shaderInputData.paletteReflectanceOffset =
				renderData->paletteReflectanceOffsets[objectData->materialId];
			shaderInputData.paletteReflectanceLength =
				renderData->paletteReflectanceLengths[objectData->materialId];
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
			shaderInputData.paletteTransparencyOffset =
				renderData->paletteTransparencyOffsets[objectData->materialId];
			shaderInputData.paletteTransparencyLength =
				renderData->paletteTransparencyLengths[objectData->materialId];
#endif

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
				calcParam.iterThreshMode = consts->params.iterThreshMode;
				calcParam.distThresh = shaderInputData.distThresh;
				calcParam.detailSize = shaderInputData.delta;

				float3 normal =
					NormalVector(consts, renderData, shaderInputData.point, shaderInputData.lastDist,
						shaderInputData.distThresh, shaderInputData.invertMode, &calcParam);
				shaderInputData.normal = normal;

#ifdef USE_ROUGH_SURFACE
				float roughnessGradient = 1.0f;

#ifdef USE_ROUGHNESS_GRADIENT
				if (shaderInputData.material->useColorsFromPalette
						&& shaderInputData.material->roughnessGradientEnable)
				{
					sClGradientsCollection gradients;
					SurfaceColor(consts, renderData, &shaderInputData, &calcParam, &gradients);
					roughnessGradient = gradients.roughness.s0;
				}
#endif // USE_ROUGHNESS_GRADIENT

				float roughnesTexture = 1.0f;
#ifdef USE_TEXTURES
#ifdef USE_ROUGHNESS_TEXTURE
				roughnesTexture = RoughnessTexture(&shaderInputData, renderData, objectData,
					shaderInputData.material->roughnessTextureIndex);
#endif // USE_ROUGHNESS_TEXTURE
#endif // USE_TEXTURES

				if (shaderInputData.material->roughSurface)
				{
					float roughness = shaderInputData.material->surfaceRoughness;
					// every axis is calculated twice because of simple Random() function (increase
					// randomness)
					float totalRoughness = roughnesTexture * roughnessGradient * roughness;
					normal.x += totalRoughness * (Random(20000, randomSeed) / 10000.0f - 1.0f);
					normal.x += totalRoughness * (Random(20000, randomSeed) / 10000.0f - 1.0f);
					normal.y += totalRoughness * (Random(20000, randomSeed) / 10000.0f - 1.0f);
					normal.y += totalRoughness * (Random(20000, randomSeed) / 10000.0f - 1.0f);
					normal.z += totalRoughness * (Random(20000, randomSeed) / 10000.0f - 1.0f);
					normal.z += totalRoughness * (Random(20000, randomSeed) / 10000.0f - 1.0f);
					shaderInputData.normal = normal = normalize(normal);
				}
#endif // USE_ROUGH_SURFACE

#ifdef USE_TEXTURES
#ifdef USE_NORMAL_MAP_TEXTURE
				normal = NormalMapShader(&shaderInputData, renderData, objectData,
					shaderInputData.material->normalMapTextureIndex);
				shaderInputData.normal = normal;
#endif
#endif

				rayStack[rayIndex].out.normal = normal;

#if defined(USE_REFRACTION) || defined(USE_REFLECTANCE)
				// prepare refraction values
				float n1, n2;

#ifdef CHROMATIC_ABERRATION
				float aberrationStrength = consts->params.DOFMonteCarloCADispersionGain * 0.01f;
				float hueEffect = 1.0f - aberrationStrength + aberrationStrength * renderData->hue / 180.0f;
#else
				float hueEffect = 1.0f;
#endif

				if (rayStack[rayIndex].in.calcInside) // if trace is inside the object
				{
					n1 = shaderInputData.material->transparencyIndexOfRefraction
							 / hueEffect; // reverse refractive indices
					n2 = 1.0f;
				}
				else
				{
					n1 = 1.0f;
					n2 = shaderInputData.material->transparencyIndexOfRefraction / hueEffect;
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
			shaderInputData.delta = CalcDelta(point, consts);
			shaderInputData.lightVect = renderData->lightVector;
			shaderInputData.point = point;
			shaderInputData.viewVector = rayStack[rayIndex].in.rayMarchingIn.direction;
			shaderInputData.viewVectorNotRotated = renderData->viewVectorNotRotated;
			shaderInputData.lastDist = rayMarchingOut.lastDist;
			shaderInputData.depth = rayMarchingOut.depth;
			shaderInputData.invertMode = rayStack[rayIndex].in.calcInside;
#if (defined(BOOLEAN_OPERATORS) || defined(USE_PRIMITIVES))
			shaderInputData.objectId = rayMarchingOut.objectId;
#else
			shaderInputData.objectId = 0;
#endif
			__global sObjectDataCl *objectData = &renderData->objectsData[shaderInputData.objectId];
			shaderInputData.material = renderData->materials[objectData->materialId];
			shaderInputData.palette = renderData->palettes[objectData->materialId];
#ifdef USE_SURFACE_GRADIENT
			shaderInputData.paletteSurfaceOffset =
				renderData->paletteSurfaceOffsets[objectData->materialId];
			shaderInputData.paletteSurfaceLength =
				renderData->paletteSurfaceLengths[objectData->materialId];
#endif
#ifdef USE_SPECULAR_GRADIENT
			shaderInputData.paletteSpecularOffset =
				renderData->paletteSpecularOffsets[objectData->materialId];
			shaderInputData.paletteSpecularLength =
				renderData->paletteSpecularLengths[objectData->materialId];
#endif
#ifdef USE_DIFFUSE_GRADIENT
			shaderInputData.paletteDiffuseOffset =
				renderData->paletteDiffuseOffsets[objectData->materialId];
			shaderInputData.paletteDiffuseLength =
				renderData->paletteDiffuseLengths[objectData->materialId];
#endif
#ifdef USE_LUMINOSITY_GRADIENT
			shaderInputData.paletteLuminosityOffset =
				renderData->paletteLuminosityOffsets[objectData->materialId];
			shaderInputData.paletteLuminosityLength =
				renderData->paletteLuminosityLengths[objectData->materialId];
#endif
#ifdef USE_ROUGHNESS_GRADIENT
			shaderInputData.paletteRoughnessOffset =
				renderData->paletteRoughnessOffsets[objectData->materialId];
			shaderInputData.paletteRoughnessLength =
				renderData->paletteRoughnessLengths[objectData->materialId];
#endif
#ifdef USE_REFLECTANCE_GRADIENT
			shaderInputData.paletteReflectanceOffset =
				renderData->paletteReflectanceOffsets[objectData->materialId];
			shaderInputData.paletteReflectanceLength =
				renderData->paletteReflectanceLengths[objectData->materialId];
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
			shaderInputData.paletteTransparencyOffset =
				renderData->paletteTransparencyOffsets[objectData->materialId];
			shaderInputData.paletteTransparencyLength =
				renderData->paletteTransparencyLengths[objectData->materialId];
#endif

			shaderInputData.stepCount = rayMarchingOut.count;
			shaderInputData.randomSeed = *randomSeed;

			sClCalcParams calcParam;
			calcParam.N = consts->params.N;
			calcParam.normalCalculationMode = false;
			calcParam.iterThreshMode = consts->params.iterThreshMode;
			calcParam.distThresh = shaderInputData.distThresh;
			calcParam.detailSize = shaderInputData.delta;
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
			float3 iridescence;

			shaderInputData.normal = recursionOut.normal;

			if (rayMarchingOut.found)
			{

// color texture
#ifdef USE_TEXTURES
#ifdef USE_COLOR_TEXTURE
				shaderInputData.texColor = TextureShader(consts, &calcParam, &shaderInputData, renderData,
					objectData, shaderInputData.material->colorTextureIndex, 1.0f);
#endif

#ifdef USE_DIFFUSION_TEXTURE
				shaderInputData.texDiffuse = TextureShader(consts, &calcParam, &shaderInputData, renderData,
					objectData, shaderInputData.material->diffusionTextureIndex, 1.0f);
#endif

#ifdef USE_LUMINOSITY_TEXTURE
				shaderInputData.texLuminosity = TextureShader(consts, &calcParam, &shaderInputData,
					renderData, objectData, shaderInputData.material->luminosityTextureIndex, 0.0f);
#endif

#ifdef USE_REFLECTANCE_TEXTURE
				shaderInputData.texReflectance = TextureShader(consts, &calcParam, &shaderInputData,
					renderData, objectData, shaderInputData.material->reflectanceTextureIndex, 1.0f);
#endif

#ifdef USE_TRANSPARENCY_TEXTURE
				shaderInputData.texTransparency = TextureShader(consts, &calcParam, &shaderInputData,
					renderData, objectData, shaderInputData.material->transparencyTextureIndex, 1.0f);
#endif
#endif

				sClGradientsCollection gradients;

				specular = 0.0f;
				objectShader = ObjectShader(consts, renderData, &shaderInputData, &calcParam, &objectColour,
					&specular, &iridescence, &gradients);

#ifdef MONTE_CARLO_DOF_GLOBAL_ILLUMINATION
				float3 globalIllumination = GlobalIlumination(
					consts, renderData, &shaderInputData, &calcParam, image2dBackground, objectColour);
				objectShader += globalIllumination;
#endif // MONTE_CARLO_DOF_GLOBAL_ILLUMINATION

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

#ifdef USE_REFRACTION
#ifdef USE_TRANSPARENCY_GRADIENT
				if (shaderInputData.material->useColorsFromPalette
						&& shaderInputData.material->transparencyGradientEnable)
					transparentShader *= (float4){
						gradients.transparency.s0, gradients.transparency.s1, gradients.transparency.s2, 1.0f};
				else
#endif // USE_TRANSPARENCY_GRADIENT
					transparentShader *= (float4){shaderInputData.material->transparencyColor.s0,
						shaderInputData.material->transparencyColor.s1,
						shaderInputData.material->transparencyColor.s2, 1.0f};

#ifdef USE_TRANSPARENCY_TEXTURE
				if (shaderInputData.material->useTransparencyTexture)
				{
					float texTransInt = shaderInputData.material->transparencyTextureIntensity;
					float texTransIntN = 1.0f - shaderInputData.material->transparencyTextureIntensity;
					float3 texTransparencyIntens =
						shaderInputData.texTransparency * texTransInt + texTransIntN;
					transparentShader *= (float4){texTransparencyIntens.s0, texTransparencyIntens.s1,
						texTransparencyIntens.s2, transparentShader.s3};
				}
#endif // USE_TRANSPARENCY_TEXTURE
#endif // USE_REFRACTION

#if defined(USE_REFRACTION) || defined(USE_REFLECTANCE)
				if (renderData->reflectionsMax > 0)
				{
#if defined(USE_DIFFUSION_TEXTURE) || defined(USE_DIFFUSE_GRADIENT)
					float3 reflectDiffused = reflect;

#ifdef USE_DIFFUSION_TEXTURE
					float diffusionIntensity = shaderInputData.material->diffusionTextureIntensity;
					float diffusionIntensityN = 1.0f - diffusionIntensity;

					reflectDiffused = reflect * shaderInputData.texDiffuse * diffusionIntensity
														+ reflect * diffusionIntensityN;
#endif // USE_DIFFUSION_TEXTURE

#ifdef USE_DIFFUSE_GRADIENT
					if (shaderInputData.material->useColorsFromPalette
							&& shaderInputData.material->diffuseGradientEnable)
					{
						reflectDiffused *= gradients.diffuse;
					}
#endif // USE_DIFFUSE_GRADIENT

#else // not USE_DIFFUSION_TEXTURE or USE_DIFFUSE_GRADIENT
#ifdef USE_REFLECTANCE
					float3 reflectDiffused = reflect;
#else
					float3 reflectDiffused = 0.0f;
#endif // USE_REFLECTANCE
#endif // USE_DIFFUSION_TEXTURE

					reflectDiffused *= iridescence;

#ifdef USE_REFLECTANCE_GRADIENT
					if (shaderInputData.material->useColorsFromPalette
							&& shaderInputData.material->reflectanceGradientEnable)
						reflectDiffused *= gradients.reflectance;
					else
#endif // USE_REFLECTANCE_GRADIENT
						reflectDiffused *= shaderInputData.material->reflectionsColor;

#ifdef USE_REFRACTION
					resultShader = transparentShader * transparent * reflectanceN
												 + (1.0f - transparent * reflectanceN) * resultShader;
#endif // USE_REFRACTION

#ifdef USE_REFLECTANCE
#ifdef USE_REFLECTANCE_TEXTURE
					if (shaderInputData.material->useReflectanceTexture)
					{
						float texReflInt = shaderInputData.material->reflectanceTextureIntensity;
						float texReflIntN = 1.0f - shaderInputData.material->reflectanceTextureIntensity;
						reflectDiffused *= shaderInputData.texReflectance * texReflInt + texReflIntN;
					}
#endif // USE_REFLECTANCE_TEXTURE

					float reflectDiffusedAvg =
						(reflectDiffused.s0 + reflectDiffused.s1 + reflectDiffused.s2) / 3.0f;

					float4 reflectDiffused4 =
						(float4){reflectDiffused.s0, reflectDiffused.s1, reflectDiffused.s2, 1.0f};
					resultShader = reflectShader * reflectDiffused4 * reflectance
												 + (1.0f - reflectDiffusedAvg * reflectance) * resultShader;
#endif // USE_REFLECTANCE
				}
#endif // USE_REFRACTION || USE_REFLECTANCE
				resultShader = max(resultShader, 0.0f);
				resultShader.w = 1.0f;
			}
			else
			{
				backgroundShader =
					BackgroundShader(consts, renderData, image2dBackground, &shaderInputData);
				resultShader.xyz = backgroundShader;
				resultShader.w = 0.0f;
				rayMarchingOut.depth = 1e20f;
				// vn = mRot.RotateVector(CVector3(0.0, -1.0, 0.0));
			}

			float opacityOut = 0.0f;

#ifdef USE_REFRACTION
			float step = rayMarchingOut.depth / shaderInputData.stepCount;
			if (rayStack[rayIndex].in.calcInside) // if the object interior is traced, then the absorption
																						// of light has to be calculated
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
				resultShader = VolumetricShader(
					consts, renderData, &shaderInputData, &calcParam, resultShader, &opacityOut);
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
