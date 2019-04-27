/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
//------------ Volumetric shader ----------------
float4 VolumetricShader(__constant sClInConstants *consts, sRenderData *renderData,
	sShaderInputDataCl *input, sClCalcParams *calcParam, float4 oldPixel, float *opacityOut)
{
	float4 out4 = oldPixel;
	float3 output = oldPixel.xyz;
	float totalOpacity = 0.0f;

	// visible lights init
	int numberOfLights = renderData->numberOfLights;
	if (numberOfLights < 4) numberOfLights = 4;

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

		float step = (distance - 0.5f * input2.distThresh) * consts->params.DEFactor
								 * consts->params.volumetricLightDEFactor;

		step *= (1.0f - Random(1000, &input->randomSeed) / 10000.0f);

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

#ifdef VISIBLE_AUX_LIGHTS
		//------------------ visible light
		{

			for (int i = 0; i < numberOfLights; ++i)
			{
				__global sLightCl *light = &renderData->lights[i];
				if (light->enabled && light->intensity > 0.0f)
				{
					float lastMiniSteps = -1.0f;
					float miniStep = 0.0f;

					for (float miniSteps = 0.0f; miniSteps < step; miniSteps += miniStep)
					{
						float3 lightDistVect = point - input->viewVector * miniSteps - light->position;
						float lightDist = fast_length(lightDistVect);
						float lightSize = native_sqrt(light->intensity) * consts->params.auxLightVisibilitySize;

						float distToLightSurface = lightDist - lightSize;
						distToLightSurface = max(distToLightSurface, 0.0f);

						miniStep = 0.1f * (distToLightSurface + 0.1f * distToLightSurface);
						miniStep = clamp(miniStep, step * 0.01f, step - miniSteps);
						miniStep = max(miniStep, 1e-6f);

						float r2 = native_divide(lightDist, lightSize);
						float bellFunction = native_divide(1.0f, (1.0f + native_powr(r2, 4.0f)));
						float lightDensity =
							native_divide(miniStep * bellFunction * consts->params.auxLightVisibility, lightSize);

						output += lightDensity * light->colour;
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
			output += light * consts->params.fakeLightsColor;
			out4.s3 += fakeLight * step * consts->params.fakeLightsVisibility;
		}
#endif // FAKE_LIGHTS

#ifdef VOLUMETRIC_LIGHTS
		for (int i = 0; i < 5; i++)
		{
			if (i == 0 && consts->params.volumetricLightEnabled[0] && consts->params.mainLightEnable)
			{
				float3 shadowOutputTemp = MainShadow(consts, renderData, &input2, calcParam);
				output += shadowOutputTemp.s0 * step * consts->params.volumetricLightIntensity[0]
									* consts->params.mainLightColour;
				out4.s3 += (shadowOutputTemp.s0 + shadowOutputTemp.s1 + shadowOutputTemp.s2) / 3.0f * step
									 * consts->params.volumetricLightIntensity[0];
			}
#ifdef AUX_LIGHTS
			if (i > 0)
			{
				__global sLightCl *light = &renderData->lights[i - 1];
				if (light->enabled && consts->params.volumetricLightEnabled[i])
				{
					float3 lightVectorTemp = light->position - point;
					float distanceLight = length(lightVectorTemp);
					float distanceLight2 = distanceLight * distanceLight;
					lightVectorTemp = normalize(lightVectorTemp);
					float lightShadow = AuxShadow(consts, renderData, &input2, distanceLight, lightVectorTemp,
						calcParam, light->intensity);

					output += lightShadow * light->colour * consts->params.volumetricLightIntensity[i] * step
										/ distanceLight2;
					out4.s3 +=
						lightShadow * consts->params.volumetricLightIntensity[i] * step / distanceLight2;
				}
			}
#endif // AUX_LIGHTS
		}
#endif // VOLUMETRIC_LIGHTS

//----------------------- basic fog
#ifdef BASIC_FOG
		{
			float fogDensity = step / consts->params.fogVisibility;
			if (fogDensity > 1.0f) fogDensity = 1.0f;

			output = fogDensity * consts->params.fogColor + (1.0f - fogDensity) * output;

			totalOpacity = fogDensity + (1.0f - fogDensity) * totalOpacity;
			out4.s3 = fogDensity + (1.0f - fogDensity) * out4.s3;
		}
#endif // BASIC_FOG

//-------------------- volumetric fog
#ifdef VOLUMETRIC_FOG
		{
			float distanceShifted = fabs(distance - consts->params.volFogDistanceFromSurface)
															+ 0.1f * consts->params.volFogDistanceFromSurface;

			float densityTemp =
				step * consts->params.volFogDistanceFactor
				/ (distanceShifted * distanceShifted
						+ consts->params.volFogDistanceFactor * consts->params.volFogDistanceFactor);

			float k = distanceShifted / consts->params.volFogColour1Distance;
			if (k > 1.0f) k = 1.0f;
			float kn = 1.0f - k;
			float3 fogTemp;
			fogTemp = consts->params.volFogColour1 * kn + consts->params.volFogColour2 * k;

			float k2 = distanceShifted / consts->params.volFogColour2Distance * k;
			if (k2 > 1) k2 = 1.0f;
			kn = 1.0f - k2;
			fogTemp = fogTemp * kn + consts->params.volFogColour3 * k2;

			float fogDensity = 0.3f * consts->params.volFogDensity * densityTemp
												 / (1.0f + consts->params.volFogDensity * densityTemp);
			if (fogDensity > 1.0f) fogDensity = 1.0f;

			output = fogDensity * fogTemp + (1.0f - fogDensity) * output;

			totalOpacity = fogDensity + (1.0f - fogDensity) * totalOpacity;
			out4.s3 = fogDensity + (1.0f - fogDensity) * out4.s3;
		}
#endif // VOLUMETRIC_FOG

// ---------- iter fog
#ifdef ITER_FOG
		{
			int L = outF.iters;
			float opacity = IterOpacity(step, L, consts->params.N, consts->params.iterFogOpacityTrim,
				consts->params.iterFogOpacityTrimHigh, consts->params.iterFogOpacity);

			float3 newColour = 0.0f;

			if (opacity > 0.0f)
			{
				// fog colour
				float iterFactor1 =
					(L - consts->params.iterFogOpacityTrim)
					/ (consts->params.iterFogColor1Maxiter - consts->params.iterFogOpacityTrim);
				float k = iterFactor1;
				if (k > 1.0f) k = 1.0f;
				if (k < 0.0f) k = 0.0f;
				float kn = 1.0f - k;
				float3 fogCol = consts->params.iterFogColour1 * kn + consts->params.iterFogColour2 * k;

				float iterFactor2 =
					(L - consts->params.iterFogColor1Maxiter)
					/ (consts->params.iterFogColor2Maxiter - consts->params.iterFogColor1Maxiter);
				float k2 = iterFactor2;
				if (k2 < 0.0f) k2 = 0.0f;
				if (k2 > 1.0f) k2 = 1.0f;
				kn = 1.0f - k2;
				fogCol = fogCol * kn + consts->params.iterFogColour3 * k2;
				//----

				for (int i = 0; i < 5; i++)
				{
					if (i == 0)
					{
						if (consts->params.mainLightEnable && consts->params.mainLightIntensity > 0.0f)
						{
							float3 shadowOutputTemp = 1.0f;
							if (consts->params.iterFogShadows)
							{
								shadowOutputTemp = MainShadow(consts, renderData, &input2, calcParam);
							}
							else
							{
								shadowOutputTemp *= consts->params.iterFogBrightnessBoost;
							}
							newColour += shadowOutputTemp * consts->params.mainLightColour
													 * consts->params.mainLightIntensity;
						}
					}

#ifdef AUX_LIGHTS
					if (i > 0)
					{
						__global sLightCl *light = &renderData->lights[i - 1];
						if (light->enabled)
						{
							float3 lightVectorTemp = light->position - point;
							float distanceLight = length(lightVectorTemp);
							float distanceLight2 = distanceLight * distanceLight;
							lightVectorTemp = normalize(lightVectorTemp);
							float lightShadow = 1.0f;
							if (consts->params.iterFogShadows)
							{
								calcParam->distThresh = input2.distThresh;
								calcParam->detailSize = input2.distThresh;
								lightShadow = AuxShadow(consts, renderData, &input2, distanceLight, lightVectorTemp,
									calcParam, light->intensity);
							}
							float intensity = light->intensity * consts->params.iterFogBrightnessBoost;
							newColour += lightShadow * light->colour / distanceLight2 * intensity;
						}
					}
#endif // AUX_LIGHTS
				}

#ifdef AO_MODE_MULTIPLE_RAYS
				float3 AO = AmbientOcclusion(consts, renderData, &input2, calcParam);
				newColour += AO * consts->params.ambientOcclusion;
#endif // AO_MODE_MULTIPLE_RAYS

				if (opacity > 1.0f) opacity = 1.0f;

				output = output * (1.0f - opacity) + newColour * opacity * fogCol;
				totalOpacity = opacity + (1.0f - opacity) * totalOpacity;
				out4.s3 = opacity + (1.0f - opacity) * out4.s3;
			}
		}
#endif // ITER FOG

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