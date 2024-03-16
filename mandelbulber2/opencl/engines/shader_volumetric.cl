/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * calculation of all volumetric shaders
 */

#ifdef FULL_ENGINE

#ifdef RAYLEIGH_SCATTERING
void RayleighScattering(__constant sClInConstants *consts, float3 lightVectorTemp,
	sShaderInputDataCl *input, float3 *raleighScatteringRGB, float3 *mieScatteringRGB)
{
	if (consts->params.rayleighScatteringBlue > 0.0f)
	{
		float raleighScattering = (1.0f + pow(dot(lightVectorTemp, input->viewVector), 2.0f))
															* consts->params.rayleighScatteringBlue;
		raleighScatteringRGB->s0 = 1.0f;
		raleighScatteringRGB->s1 = (1.0f + 0.2f * raleighScattering);
		raleighScatteringRGB->s2 = (1.0f + 2.0f * raleighScattering);
	}
	if (consts->params.rayleighScatteringRed > 0.0f)
	{
		float mieScatteringR = pow(dot(lightVectorTemp, input->viewVector) * 0.5f + 0.5f, 15.0f)
													 * consts->params.rayleighScatteringRed;
		float mieScatteringG = pow(dot(lightVectorTemp, input->viewVector) * 0.5f + 0.5f, 10.0f)
													 * consts->params.rayleighScatteringRed;
		mieScatteringRGB->s0 = (1.0f + 5.0f * mieScatteringR);
		mieScatteringRGB->s1 = (1.0f + mieScatteringG);
		mieScatteringRGB->s2 = 1.0f;
	}
}
#endif // RAYLEIGH_SCATTERING

//------------ Volumetric shader ----------------
float4 VolumetricShader(__constant sClInConstants *consts, sRenderData *renderData,
	sShaderInputDataCl *input, sClCalcParams *calcParam, image2d_t image2dBackground, float4 oldPixel,
	float *opacityOut)
{
	float4 out4 = oldPixel;
	float3 output = oldPixel.xyz;
	float totalOpacity = 0.0f;

	// visible lights init
	int numberOfLights = renderData->numberOfLights;

#ifdef GLOW
	// glow init
	float glow = input->stepCount * consts->params.glowIntensity / 512.0f * consts->params.DEFactor;
	float glowN = 1.0f - glow;
	if (glowN < 0.0f) glowN = 0.0f;

	float3 glowColor;

	glowColor = (glowN * consts->params.glowColor1 + consts->params.glowColor2 * glow);

#endif // GLOW

#ifdef SIMPLE_GLOW // only simple glow, no another shaders
#ifdef GLOW
	glow *= 0.7f;
	float glowOpacity = 1.0f * glow;
	if (glowOpacity > 1.0f) glowOpacity = 1.0f;
	output = glow * glowColor + (1.0f - glowOpacity) * output;
	out4.s3 += glowOpacity;
#endif // GLOW

#else // not SIMPLE_GLOW
	float totalStep = 0.0f;
	float scan = CalcDistThresh(input->point, consts);
	float lastCloudDistance = consts->params.cloudsPeriod;

	sShaderInputDataCl input2 = *input;

	for (int i = 0; i < MAX_RAYMARCHING; i++)
	{
		float3 point = input->point - input->viewVector * scan;

		input2.point = point;
		input2.distThresh = CalcDistThresh(point, consts);
		input2.delta = CalcDelta(point, consts);

		calcParam->distThresh = input2.distThresh;
		calcParam->detailSize = input2.distThresh;

		formulaOut outF;
		outF = CalculateDistance(consts, point, calcParam, renderData);
		float distance = outF.distance;

		float step = (min(distance, lastCloudDistance) - 0.5f * input2.distThresh)
								 * consts->params.DEFactor * consts->params.volumetricLightDEFactor;

#ifdef MONTE_CARLO
		step *= (1.0f - Random(1000, &input->randomSeed) / 4000.0f);
#else
		step *= (1.0f - Random(1000, &input->randomSeed) / 10000.0f);
#endif

#ifdef ADVANCED_QUALITY
		step = clamp(step, consts->params.absMinMarchingStep, consts->params.absMaxMarchingStep);

		if (input2.distThresh > consts->params.absMinMarchingStep)
			step = clamp(step, consts->params.relMinMarchingStep * input2.distThresh,
				consts->params.relMaxMarchingStep * input2.distThresh);
#endif

		step = max(step, input2.distThresh);

		bool end = false;
		if (step > input->depth - scan)
		{
			step = input->depth - scan;
			end = true;
		}
		scan += step;

//------------------- glow
#ifdef GLOW
		{
			if (input->stepCount > 0)
			{
				float glowOpacity = glow / input->stepCount * consts->params.volumetricLightDEFactor;
				if (glowOpacity > 1.0f) glowOpacity = 1.0f;

				output = glowOpacity * glowColor + (1.0f - glowOpacity) * output;
				out4.s3 += glowOpacity;
			}
		}
#endif // GLOW

#ifdef BASIC_FOG
		float basicFogOpacity = 0.0f;
		{
			basicFogOpacity = step / consts->params.fogVisibility;

#if (defined(USE_PRIMITIVES) && defined(BASIC_FOG_SHAPE_FROM_PRIMITIVE))
			if (renderData->primitivesGlobalData->primitiveIndexForBasicFog >= 0)
			{
				int closestId = -1;

				if (TotalDistanceToPrimitives(consts, renderData, point, distance, input2.delta, false,
							&closestId, renderData->primitivesGlobalData->primitiveIndexForBasicFog)
						> input2.delta)
					basicFogOpacity = 0.0f;
			}
#endif // USE_PRIMITIVES
		}
#endif // BASIC_FOG

#ifdef ITER_FOG
		float iterFogOpacity = 0.0f;
		float3 iterFogCol = 0.0f;

		{
			int L = outF.iters;
			iterFogOpacity = IterOpacity(step, L, consts->params.N, consts->params.iterFogOpacityTrim,
				consts->params.iterFogOpacityTrimHigh, consts->params.iterFogOpacity);

#if (defined(USE_PRIMITIVES) && defined(ITER_FOG_SHAPE_FROM_PRIMITIVE))
			if (iterFogOpacity > 0.0f && renderData->primitivesGlobalData->primitiveIndexForIterFog >= 0)
			{
				int closestId = -1;

				if (TotalDistanceToPrimitives(consts, renderData, point, distance, input2.delta, false,
							&closestId, renderData->primitivesGlobalData->primitiveIndexForIterFog)
						> input2.delta)
					iterFogOpacity = 0.0f;
			}
#endif // USE_PRIMITIVES

			if (iterFogOpacity > 0.0f)
			{
				// fog colour
				float iterFactor1 =
					(L - consts->params.iterFogOpacityTrim)
					/ (consts->params.iterFogColor1Maxiter - consts->params.iterFogOpacityTrim);
				float k = iterFactor1;
				if (k > 1.0f) k = 1.0f;
				if (k < 0.0f) k = 0.0f;
				float kn = 1.0f - k;
				iterFogCol = consts->params.iterFogColour1 * kn + consts->params.iterFogColour2 * k;

				float iterFactor2 =
					(L - consts->params.iterFogColor1Maxiter)
					/ (consts->params.iterFogColor2Maxiter - consts->params.iterFogColor1Maxiter);
				float k2 = iterFactor2;
				if (k2 < 0.0f) k2 = 0.0f;
				if (k2 > 1.0f) k2 = 1.0f;
				kn = 1.0f - k2;
				iterFogCol = iterFogCol * kn + consts->params.iterFogColour3 * k2;
				//----
			}
		}
#endif // ITER_FOG

		//--------- clouds --------
#ifdef CLOUDS
		float cloudsOpacity = 0.0f;
		float cloudDensity = 0.0;
		float3 deltaCloud = 0.0f;

		{
			// perlin noise clouds
			float distanceToClouds = 0.0f;
			bool calculateClouds = true;

#if (defined(USE_PRIMITIVES) && defined(CLOUDS_SHAPE_FROM_PRIMITIVE))
			if (renderData->primitivesGlobalData->primitiveIndexForClouds >= 0)
			{
				int closestId = -1;

				if (TotalDistanceToPrimitives(consts, renderData, point, distance, input2.delta, false,
							&closestId, renderData->primitivesGlobalData->primitiveIndexForClouds)
						> input2.delta)
				{
					cloudDensity = 0.0f;
					calculateClouds = false;
				}
			}
#endif // USE_PRIMITIVES

			if (calculateClouds)
			{
				cloudDensity = CloudOpacity(
					consts, renderData->perlinNoiseSeeds, point, distance, input2.delta, &distanceToClouds);
			}
			else
			{
				distanceToClouds = distance;
			}

#ifndef CLOUDSSHADOWS
			if (cloudDensity > 0.0f)
			{
				float delta =
					consts->params.cloudsPeriod / pown(2.0f, consts->params.cloudsIterations) * 5.0f;
				float distanceToCloudsDummy = 0.0f;

				deltaCloud.x =
					CloudOpacity(consts, renderData->perlinNoiseSeeds, point + (float3){delta, 0.0f, 0.0f},
						distance, input2.delta, &distanceToCloudsDummy)
					- cloudDensity;

				deltaCloud.y =
					CloudOpacity(consts, renderData->perlinNoiseSeeds, point + (float3){0.0f, delta, 0.0f},
						distance, input2.delta, &distanceToCloudsDummy)
					- cloudDensity;

				deltaCloud.z =
					CloudOpacity(consts, renderData->perlinNoiseSeeds, point + (float3){0.0f, 0.0f, delta},
						distance, input2.delta, &distanceToCloudsDummy)
					- cloudDensity;

				if (length(deltaCloud) > 0.0f)
				{
					deltaCloud = normalize(deltaCloud);
				}
			}
#endif // CLOUDSSHADOWS

			cloudsOpacity = cloudDensity * step;

			lastCloudDistance = distanceToClouds;
		}
#endif // CLOUDS

//-------------------- volumetric fog
#ifdef VOLUMETRIC_FOG
		float distFogOpacity = 0.0f;
		float3 distFogColor = 0.0f;

		{
			float distanceShifted;
			distFogOpacity = DistanceFogOpacity(step, distance, consts->params.volFogDistanceFromSurface,
				consts->params.volFogDistanceFactor, consts->params.volFogDensity, &distanceShifted);

#if (defined(USE_PRIMITIVES) && defined(DIST_FOG_SHAPE_FROM_PRIMITIVE))
			if (distFogOpacity > 0.0f && renderData->primitivesGlobalData->primitiveIndexForDistFog >= 0)
			{
				int closestId = -1;

				if (TotalDistanceToPrimitives(consts, renderData, point, distance, input2.delta, false,
							&closestId, renderData->primitivesGlobalData->primitiveIndexForDistFog)
						> input2.delta)
					distFogOpacity = 0.0f;
			}
#endif // USE_PRIMITIVES

			float k = distanceShifted / consts->params.volFogColour1Distance;
			if (k > 1.0f) k = 1.0f;
			float kn = 1.0f - k;
			float3 fogTemp;
			fogTemp = consts->params.volFogColour1 * kn + consts->params.volFogColour2 * k;

			float k2 = distanceShifted / consts->params.volFogColour2Distance * k;
			if (k2 > 1.0f) k2 = 1.0f;
			kn = 1.0f - k2;
			distFogColor = fogTemp * kn + consts->params.volFogColour3 * k2;
		}
#endif // VOLUMETRIC_FOG

		float3 totalLightsWithShadows = 0.0f;
		float3 totalLights = 0.0f;
		float3 totalLightsClouds = 0.0f;

// loop for proceessing all lights and volumetric effects
#ifdef AUX_LIGHTS
#if (defined(VOLUMETRIC_LIGHTS) || defined(ITER_FOG) || defined(DIST_FOG_SHADOWS) \
		 || defined(CLOUDS) || defined(BASIC_FOG))
		for (int i = 0; i < numberOfLights; i++)
		{
			__global sLightCl *light = &renderData->lights[i];

			if (light->enabled)
			{
				bool shadowNeeded = false;
				bool lightNeeded = false;

#ifdef VOLUMETRIC_LIGHTS
				if (light->volumetric)
				{
					shadowNeeded = true;
					lightNeeded = true;
				}
#endif

#ifdef ITER_FOG
				if (iterFogOpacity > 0.0f)
				{
					lightNeeded = true;
					if (consts->params.iterFogShadows) shadowNeeded = true;
				}
#endif

#ifdef CLOUDS
				if (cloudsOpacity > 0.0)
				{
					lightNeeded = true;
					if (consts->params.cloudsCastShadows) shadowNeeded = true;
				}
#endif

#ifdef VOLUMETRIC_FOG
				if (consts->params.distanceFogShadows && distFogOpacity > 0.0f)
				{
					lightNeeded = true;
					shadowNeeded = true;
				}
#endif

#ifdef BASIC_FOG
				if (consts->params.fogCastShadows && basicFogOpacity > 0.0f)
				{
					lightNeeded = true;
					shadowNeeded = true;
				}
#endif

				if (!light->castShadows) shadowNeeded = false;

				if (lightNeeded)
				{
					float distanceLight = 0.0f;

					float3 lightVectorTemp =
						CalculateLightVector(light, point, input2.delta, consts->params.resolution,
							consts->params.viewDistanceMax, &distanceLight, &input->randomSeed);

					float lightIntensity = 0.0f;
					if (light->type == lightDirectional)
						lightIntensity = light->intensity;
					else
						lightIntensity =
							light->intensity / LightDecay(distanceLight, light->decayFunction) * 4.0f;

					float3 textureColor;
					lightIntensity *= CalculateLightCone(light, renderData, lightVectorTemp, &textureColor);

					float3 lightShadow = 1.0f;

#ifdef SHADOWS
					if (shadowNeeded)
					{
						if (lightIntensity > 1e-3)
							lightShadow = AuxShadow(consts, renderData, &input2, light, distanceLight,
								lightVectorTemp, calcParam, light->intensity);
						else
							lightShadow = 0.0f;
					}
#endif

					float3 raleighScatteringRGB = 1.0f;
					float3 mieScatteringRGB = 1.0f;

#ifdef RAYLEIGH_SCATTERING
					RayleighScattering(
						consts, lightVectorTemp, input, &raleighScatteringRGB, &mieScatteringRGB);
#endif

					float3 calculatedLight =
						light->color * lightIntensity * textureColor * raleighScatteringRGB * mieScatteringRGB;
					totalLightsWithShadows += calculatedLight * lightShadow;
					totalLights += calculatedLight;

#ifdef CLOUDS
					float shadeClouds = clamp(-dot(lightVectorTemp, deltaCloud), 0.0f, 1.0f);
					totalLightsClouds += calculatedLight * shadeClouds;
#endif

					if (light->volumetric)
					{
						output += calculatedLight * light->volumetricVisibility * lightShadow * step;
						out4.s3 += lightShadow.s0 * step * lightIntensity * light->volumetricVisibility;
					}
				} // if light needed
			}		// if light enabled
		}

#endif // VOLUMETRIC_LIGHTS
#endif // AUX_LIGHTS

#if !defined(MC_GI_VOLUMETRIC) && defined(MC_GI_FOG_ILLUMINATION) \
	&& defined(MONTE_CARLO_DOF_GLOBAL_ILLUMINATION)
		{
			float3 gi =
				GlobalIlumination(consts, renderData, &input2, calcParam, image2dBackground, 1.0f, true);
			totalLights += gi;
			totalLightsWithShadows += gi;
		}
#endif

		float3 AO = 0.0f;
		bool aoNeeded = false;
#ifdef ITER_FOG
		if (iterFogOpacity > 0.0f) aoNeeded = true;
#endif
			// #ifdef VOLUMETRIC_FOG
			//		if (distFogOpacity > 0.0f) aoNeeded = true;
			// #endif

#ifdef AO_MODE_MULTIPLE_RAYS
		if (aoNeeded)
		{
			AO =
				AmbientOcclusion(consts, renderData, &input2, calcParam) * consts->params.ambientOcclusion;
		}
#endif // AO_MODE_MULTIPLE_RAYS

#ifdef ITER_FOG
		if (iterFogOpacity > 0.0f)
		{
			float3 light = (consts->params.iterFogShadows) ? totalLightsWithShadows : totalLights;

			if (iterFogOpacity > 1.0f) iterFogOpacity = 1.0f;

			output = output * (1.0f - iterFogOpacity)
							 + (light * consts->params.iterFogBrightnessBoost + AO) * iterFogOpacity * iterFogCol;

			totalOpacity = iterFogOpacity + (1.0f - iterFogOpacity) * totalOpacity;
			out4.s3 = iterFogOpacity + (1.0f - iterFogOpacity) * out4.s3;
		}

#endif // ITER_FOG

#ifdef CLOUDS
		if (cloudsOpacity > 0.0f)
		{
			float3 light =
				(consts->params.cloudsCastShadows) ? totalLightsWithShadows : totalLightsClouds;

			float ambient = consts->params.cloudsAmbientLight;
			float nAmbient = 1.0f - consts->params.cloudsAmbientLight;

			light = ambient * totalLights + nAmbient * light;

			if (cloudsOpacity > 1.0f) cloudsOpacity = 1.0f;

			output =
				output * (1.0f - cloudsOpacity) + (light + AO) * cloudsOpacity * consts->params.cloudsColor;

			totalOpacity = cloudsOpacity + (1.0f - cloudsOpacity) * totalOpacity;
			out4.s3 = cloudsOpacity + (1.0f - cloudsOpacity) * out4.s3;
		}
#endif // CLOUDS

#ifdef VOLUMETRIC_FOG
		if (distFogOpacity > 0.0f)
		{
			float3 light = (consts->params.distanceFogShadows) ? totalLightsWithShadows : 1.0f;

			if (distFogOpacity > 1.0f) distFogOpacity = 1.0f;

			output = distFogOpacity * distFogColor * (light + AO) + (1.0f - distFogOpacity) * output;

			totalOpacity = distFogOpacity + (1.0f - distFogOpacity) * totalOpacity;
			out4.s3 = distFogOpacity + (1.0f - distFogOpacity) * out4.s3;
		}
#endif

//----------------------- basic fog
#ifdef BASIC_FOG
		if (basicFogOpacity > 0)
		{
			float3 light = (consts->params.fogCastShadows) ? totalLightsWithShadows : 1.0f;

			if (basicFogOpacity > 1.0f) basicFogOpacity = 1.0f;

			output = basicFogOpacity * consts->params.fogColor * (light + AO)
							 + (1.0f - basicFogOpacity) * output;

			totalOpacity = basicFogOpacity + (1.0f - basicFogOpacity) * totalOpacity;
			out4.s3 = basicFogOpacity + (1.0f - basicFogOpacity) * out4.s3;
		}
#endif // BASIC_FOG

#ifdef VISIBLE_AUX_LIGHTS
		//------------------ visible light
		{
			for (int i = 0; i < numberOfLights; ++i)
			{
				__global sLightCl *light = &renderData->lights[i];
				if (light->enabled && light->intensity > 0.0f && light->visibility > 0.0f
						&& light->type != lightDirectional)
				{
					float lastMiniSteps = -1.0f;
					float miniStep = 0.0f;

					float3 lightPosition =
						CalculateBeam(light, light->position, light->target, &input->randomSeed);

					for (float miniSteps = 0.0f; miniSteps < step; miniSteps += miniStep)
					{
						float3 lightDistVect = point - input->viewVector * miniSteps - lightPosition;
						float lightDist = fast_length(lightDistVect);
						float lightSize = native_sqrt(light->intensity) * light->size;

						float distToLightSurface = lightDist - lightSize;
						distToLightSurface = max(distToLightSurface, 0.0f);

						miniStep = 0.1f * (distToLightSurface + 0.1f * distToLightSurface);
						miniStep = clamp(miniStep, step * 0.01f, step - miniSteps);
						miniStep = max(miniStep, 1e-6f);

						float r2 = lightDist / lightSize;
						float bellFunction;
						if (light->type == lightConical)
						{
							bellFunction = 1.0f / (0.01f + pown(r2, ((int)light->decayFunction + 1) * 2));
						}
						else
						{
							bellFunction = 1.0f / (1.0f + pown(r2, ((int)light->decayFunction + 1) * 2));
						}

						float3 lightDirection = normalize(lightDistVect);

						float3 textureColor;
						bellFunction *=
							CalculateLightCone(light, renderData, (-1.0f) * lightDirection, &textureColor);

						float lightDensity = miniStep * bellFunction * light->visibility / lightSize;

#ifdef CLOUDS
						lightDensity *= 1.0f + consts->params.cloudsLightsBoost * cloudsOpacity;
#endif

						output += lightDensity * light->color * textureColor;
						out4.s3 += lightDensity;

						if (miniSteps == lastMiniSteps)
						{
							// Dead computation
							break;
						}
						lastMiniSteps = miniSteps;
					}
				}
			}
		}
#endif

#ifdef FAKE_LIGHTS
		// fake lights (orbit trap)
		{
			formulaOut outF;
			outF = Fractal(consts, input2.point, calcParam, calcModeOrbitTrap, NULL, -1);
			float r = outF.orbitTrapR;
			r = sqrt(1.0f / (r + 1.0e-20f));
			float fakeLight = 1.0f
												/ (pow(r, 10.0f / consts->params.fakeLightsVisibilitySize)
														 * pow(10.0f, 10.0f / consts->params.fakeLightsVisibilitySize)
													 + 0.1f);
			float3 light = fakeLight * step * consts->params.fakeLightsVisibility;
#ifdef CLOUDS
			light *= 1.0f + consts->params.cloudsLightsBoost * cloudDensity;
#endif

			output += light * consts->params.fakeLightsColor;
			out4.s3 += fakeLight * step * consts->params.fakeLightsVisibility;
		}
#endif // FAKE_LIGHTS

		if (totalOpacity > 1.0f) totalOpacity = 1.0f;
		if (out4.s3 > 1.0f) out4.s3 = 1.0f; // alpha channel

		*opacityOut = totalOpacity;

		if (end) break;
	}
#endif // not SIMPLE_GLOW

	out4.xyz = output;
	return out4;
}
#endif // FULL_ENGINE