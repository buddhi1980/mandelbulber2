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
 * cRenderWorker::VolumetricShader method - calculates volumetric shaders
 */

#include <cmath>

#include "algebra.hpp"
#include "ao_modes.h"
#include "calculate_distance.hpp"
#include "calculation_mode.h"
#include "color_structures.hpp"
#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "lights.hpp"
#include "nine_fractals.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::VolumetricShader(
	const sShaderInputData &input, sRGBAfloat oldPixel, sRGBAfloat *opacityOut) const
{
	sRGBAfloat output;
	float totalOpacity = 0.0;

	output.R = oldPixel.R;
	output.G = oldPixel.G;
	output.B = oldPixel.B;
	output.A = oldPixel.A;

	// volumetric fog init
	double colourThresh = params->volFogColour1Distance;
	double colourThresh2 = params->volFogColour2Distance;
	double fogReduce = params->volFogDistanceFactor;
	float fogIntensity = params->volFogDensity;

	// visible lights init
	int numberOfLights = data->lights.GetNumberOfLights();
	if (numberOfLights < 4) numberOfLights = 4;

	// glow init
	float glow = input.stepCount * params->glowIntensity / 512.0f * float(params->DEFactor);
	float glowN = 1.0f - glow;
	if (glowN < 0.0f) glowN = 0.0f;
	float glowR = (params->glowColor1.R * glowN + params->glowColor2.R * glow);
	float glowG = (params->glowColor1.G * glowN + params->glowColor2.G * glow);
	float glowB = (params->glowColor1.B * glowN + params->glowColor2.B * glow);

	double totalStep = 0.0;

	// qDebug() << "Start volumetric shader &&&&&&&&&&&&&&&&&&&&";

	int numberOfSteps;
	bool recalcStepsMode;
	double scan = 0.0f;
	double lastCloudDistance = params->cloudsPeriod;
	bool end = false;

	if (!qFuzzyCompare(params->volumetricLightDEFactor, 1.0) || params->cloudsEnable)
	{
		numberOfSteps = MAX_RAYMARCHING;
		recalcStepsMode = true;
		scan = CalcDistThresh(input.point);
	}
	else
	{
		numberOfSteps = input.stepCount - 1;
		recalcStepsMode = false;
	}

	sShaderInputData input2 = input;
	for (int index = numberOfSteps; index > 0; index--)
	{
		double step;
		double distance;
		CVector3 point;
		int iterations = 0;

		// distance, points and steps are recalculated when custom DE step is usew=d
		if (recalcStepsMode)
		{
			point = input.point - input.viewVector * scan;

			input2.point = point;
			input2.distThresh = CalcDistThresh(point);
			input2.delta = CalcDelta(point);

			sDistanceOut distanceOut;
			sDistanceIn distanceIn(point, input2.distThresh, false);
			distance = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);
			iterations = distanceOut.iters;

			step = (min(distance, lastCloudDistance) - 0.5 * input2.distThresh) * params->DEFactor
						 * params->volumetricLightDEFactor;

			step *= (1.0 - Random(1000) / 10000.0);

			if (params->advancedQuality)
			{
				step = clamp(step, params->absMinMarchingStep, params->absMaxMarchingStep);

				if (input2.distThresh > params->absMinMarchingStep)
					step = clamp(step, params->relMinMarchingStep * input2.distThresh,
						params->relMaxMarchingStep * input2.distThresh);
			}

			step = max(step, input2.distThresh);

			end = false;
			if (step > input.depth - scan)
			{
				step = input.depth - scan;
				end = true;
			}
			scan += step;

			// qDebug() << point.Debug() << distance << step << input.depth - scan << lastCloudDistance;
		}
		// distance, points and steps are taken from arrays
		else
		{
			step = input.stepBuff[index].step;
			distance = input.stepBuff[index].distance;
			point = input.stepBuff[index].point;
			iterations = input.stepBuff[index].iters;

			totalStep += step;

			input2.point = point;
			input2.distThresh = input.stepBuff[index].distThresh;
			input2.delta = CalcDelta(point);

			// qDebug() << "i" << index << "dist" << distance << "iters" << input.stepBuff[index].iters <<
			// "distThresh" << input2.distThresh << "step" << step << "point" << point.Debug();

			if (totalStep < 1e-10 * CalcDistThresh(point)) // if two steps are the same
			{
				continue;
			}
			step = totalStep;
			totalStep = 0.0;
		}

		//------------------- glow
		if (params->glowEnabled && input.stepCount > 0)
		{
			float glowOpacity = glow / input.stepCount * params->volumetricLightDEFactor;
			if (glowOpacity > 1.0f) glowOpacity = 1.0f;
			output.R = glowOpacity * glowR + (1.0f - glowOpacity) * output.R;
			output.G = glowOpacity * glowG + (1.0f - glowOpacity) * output.G;
			output.B = glowOpacity * glowB + (1.0f - glowOpacity) * output.B;
			output.A += glowOpacity;
		}
		// qDebug() << "step" << step;

		//---------------------- volumetric lights with shadows in fog

		float iterFogOpacity = 0.0f;
		sRGBFloat iterFogCol;

		if (params->iterFogEnabled)
		{
			iterFogOpacity = IterOpacity(step, iterations, params->N, params->iterFogOpacityTrim,
				params->iterFogOpacityTrimHigh, params->iterFogOpacity);

			if (iterFogOpacity > 0.0)
			{
				// fog colour
				float iterFactor1 = (iterations - params->iterFogOpacityTrim)
														/ (params->iterFogColor1Maxiter - params->iterFogOpacityTrim);
				float k = iterFactor1;
				if (k > 1.0f) k = 1.0f;
				if (k < 0.0f) k = 0.0f;
				float kn = 1.0f - k;
				iterFogCol.R = params->iterFogColour1.R * kn + params->iterFogColour2.R * k;
				iterFogCol.G = params->iterFogColour1.G * kn + params->iterFogColour2.G * k;
				iterFogCol.B = params->iterFogColour1.B * kn + params->iterFogColour2.B * k;

				float iterFactor2 = (iterations - params->iterFogColor1Maxiter)
														/ (params->iterFogColor2Maxiter - params->iterFogColor1Maxiter);
				float k2 = iterFactor2;
				if (k2 < 0.0f) k2 = 0.0;
				if (k2 > 1.0f) k2 = 1.0;
				kn = 1.0f - k2;
				iterFogCol.R = iterFogCol.R * kn + params->iterFogColour3.R * k2;
				iterFogCol.G = iterFogCol.G * kn + params->iterFogColour3.G * k2;
				iterFogCol.B = iterFogCol.B * kn + params->iterFogColour3.B * k2;
			}
			//----
		}

		sRGBAfloat totalLights(0.0, 0.0, 0.0, 0.0);

		for (int i = 0; i < data->lights.GetNumberOfLights(); i++)
		{
			if (data->lights.IsAnyLightEnabled())
			{
				const cLight *light = data->lights.GetLight(i);
				if (light->enabled)
				{
					bool shadowNeeded = false;
					bool lightNeeded = false;

					if (light->volumetric)
					{
						shadowNeeded = true;
						lightNeeded = true;
					}

					if (params->iterFogEnabled && iterFogOpacity > 0.0)
					{
						lightNeeded = true;
						if (params->iterFogShadows) shadowNeeded = true;
					}

					if (lightNeeded)
					{
						double distanceLight = 0.0;
						CVector3 lightVectorTemp = light->CalculateLightVector(
							point, input2.delta, params->resolution, params->viewDistanceMax, distanceLight);

						float lightIntensity;
						if (light->type == cLight::lightDirectional)
							lightIntensity = light->intensity;
						else
							lightIntensity = light->intensity / light->Decay(distanceLight)
															 * params->iterFogBrightnessBoost * 4.0;

						sRGBFloat textureColor(0.0, 0.0, 0.0);
						lightIntensity *= light->CalculateCone(lightVectorTemp, textureColor);

						sRGBAfloat lightShadow(0.0, 0.0, 0.0, 0.0);
						if (shadowNeeded)
						{
							if (lightIntensity > 1e-3)
								lightShadow = AuxShadow(input2, light, distanceLight, lightVectorTemp);
							else
								lightShadow = sRGBAfloat();
						}

						sRGBFloat calculatedLight(0.0, 0.0, 0.0);
						calculatedLight.R = lightShadow.R * light->color.R * lightIntensity * textureColor.R;
						calculatedLight.G = lightShadow.G * light->color.G * lightIntensity * textureColor.G;
						calculatedLight.B = lightShadow.B * light->color.B * lightIntensity * textureColor.B;

						totalLights.R += calculatedLight.R;
						totalLights.G += calculatedLight.G;
						totalLights.B += calculatedLight.B;

						if (light->volumetric)
						{
							output.R += calculatedLight.R * light->volumetricVisibility * float(step);
							output.G += calculatedLight.G * light->volumetricVisibility * float(step);
							output.B += calculatedLight.B * light->volumetricVisibility * float(step);
							output.A +=
								lightShadow.A * float(step) * lightIntensity * light->volumetricVisibility;
						}
					} // if light needed
				}		// if light enabled
			}			// if any light enabled
		}				// next light

		if (params->iterFogEnabled && iterFogOpacity > 0.0)
		{
			if (params->ambientOcclusionEnabled
					&& params->ambientOcclusionMode == params::AOModeMultipleRays)
			{
				sRGBAfloat AO = AmbientOcclusion(input2);
				totalLights.R += AO.R * params->ambientOcclusion;
				totalLights.G += AO.G * params->ambientOcclusion;
				totalLights.B += AO.B * params->ambientOcclusion;
			}

			if (iterFogOpacity > 1.0f) iterFogOpacity = 1.0f;

			output.R = output.R * (1.0f - iterFogOpacity) + totalLights.R * iterFogOpacity * iterFogCol.R;
			output.G = output.G * (1.0f - iterFogOpacity) + totalLights.G * iterFogOpacity * iterFogCol.G;
			output.B = output.B * (1.0f - iterFogOpacity) + totalLights.B * iterFogOpacity * iterFogCol.B;

			totalOpacity = iterFogOpacity + (1.0f - iterFogOpacity) * totalOpacity;
			output.A = iterFogOpacity + (1.0f - iterFogOpacity) * output.A;
		}

		//----------------------- basic fog
		if (params->fogEnabled)
		{
			float fogDensity = step / params->fogVisibility;
			if (fogDensity > 1.0f) fogDensity = 1.0f;
			output.R = fogDensity * params->fogColor.R + (1.0f - fogDensity) * output.R;
			output.G = fogDensity * params->fogColor.G + (1.0f - fogDensity) * output.G;
			output.B = fogDensity * params->fogColor.B + (1.0f - fogDensity) * output.B;
			totalOpacity = fogDensity + (1.0f - fogDensity) * totalOpacity;
			output.A = fogDensity + (1.0f - fogDensity) * output.A;
		}

		//-------------------- volumetric fog
		if (fogIntensity > 0.0f && params->volFogEnabled)
		{
			double distanceShifted = fabs(distance - params->volFogDistanceFromSurface)
															 + 0.1 * params->volFogDistanceFromSurface;
			float densityTemp =
				step * fogReduce / (distanceShifted * distanceShifted + fogReduce * fogReduce);

			float k = distanceShifted / colourThresh;
			if (k > 1) k = 1.0f;
			float kn = 1.0f - k;
			float fogTempR = params->volFogColour1.R * kn + params->volFogColour2.R * k;
			float fogTempG = params->volFogColour1.G * kn + params->volFogColour2.G * k;
			float fogTempB = params->volFogColour1.B * kn + params->volFogColour2.B * k;

			float k2 = distanceShifted / colourThresh2 * k;
			if (k2 > 1) k2 = 1.0;
			kn = 1.0f - k2;
			fogTempR = fogTempR * kn + params->volFogColour3.R * k2;
			fogTempG = fogTempG * kn + params->volFogColour3.G * k2;
			fogTempB = fogTempB * kn + params->volFogColour3.B * k2;

			float fogDensity = 0.3f * fogIntensity * densityTemp / (1.0f + fogIntensity * densityTemp);
			if (fogDensity > 1) fogDensity = 1.0;

			output.R = fogDensity * fogTempR + (1.0f - fogDensity) * output.R;
			output.G = fogDensity * fogTempG + (1.0f - fogDensity) * output.G;
			output.B = fogDensity * fogTempB + (1.0f - fogDensity) * output.B;

			totalOpacity = fogDensity + (1.0f - fogDensity) * totalOpacity;
			output.A = fogDensity + (1.0f - fogDensity) * output.A;
		}

		//-------------- perlin noise clouds
		double cloudsOpacity = 0.0;
		if (params->cloudsEnable)
		{
			double distanceToClouds = 0.0;
			double cloud = CloudOpacity(point, distance, input2.delta, &distanceToClouds);

			CVector3 deltaCloud;
			if (!params->cloudsCastShadows)
			{
				double delta = params->cloudsPeriod / pow(2.0, params->cloudsIterations) * 5.0f;
				double distanceToCloudsDummy = 0.0;
				deltaCloud.x = CloudOpacity(point + CVector3(delta, 0.0, 0.0), distance, input2.delta,
												 &distanceToCloudsDummy)
											 - cloud;
				deltaCloud.y = CloudOpacity(point + CVector3(0.0, delta, 0.0), distance, input2.delta,
												 &distanceToCloudsDummy)
											 - cloud;
				deltaCloud.z = CloudOpacity(point + CVector3(0.0, 0.0, delta), distance, input2.delta,
												 &distanceToCloudsDummy)
											 - cloud;

				if (deltaCloud.Length() > 0.0)
				{
					deltaCloud.Normalize();
				}
			}

			double opacity = cloud * step;
			// qDebug() << cloud;

			lastCloudDistance = distanceToClouds;

			sRGBAfloat newColour(0.0, 0.0, 0.0, 0.0);
			sRGBAfloat shadowOutputTemp(1.0, 1.0, 1.0, 1.0);

			double ambient = params->cloudsAmbientLight;
			double nAmbient = 1.0 - params->cloudsAmbientLight;

			if (opacity > 0)
			{
				for (int l = 0; l < data->lights.GetNumberOfLights(); l++)
				{
					const cLight *light = data->lights.GetLight(l);
					if (light->enabled)
					{
						double distanceLight = 0.0;
						CVector3 lightVectorTemp = light->CalculateLightVector(
							point, input2.delta, params->resolution, params->viewDistanceMax, distanceLight);

						float intensity;
						if (light->type == cLight::lightDirectional)
							intensity = light->intensity;
						else
							intensity = 100.0f * light->intensity / light->Decay(distanceLight) / 6.0;

						sRGBFloat textureColor;
						intensity *= light->CalculateCone(lightVectorTemp, textureColor);

						sRGBAfloat lightShadow(1.0, 1.0, 1.0, 1.0);

						if (params->cloudsCastShadows && light->castShadows && intensity > 1e-3)
						{
							lightShadow = AuxShadow(input2, light, distanceLight, lightVectorTemp);
						}
						else
						{
							if (deltaCloud.Length() > 0)
							{
								double shade = clamp(-lightVectorTemp.Dot(deltaCloud), 0.0, 1.0);
								lightShadow.R *= shade;
								lightShadow.G *= shade;
								lightShadow.B *= shade;
							}
						}

						lightShadow.R = lightShadow.R * nAmbient + ambient;
						lightShadow.G = lightShadow.G * nAmbient + ambient;
						lightShadow.B = lightShadow.B * nAmbient + ambient;

						newColour.R += lightShadow.R * light->color.R * intensity * textureColor.R;
						newColour.G += lightShadow.G * light->color.G * intensity * textureColor.G;
						newColour.B += lightShadow.B * light->color.B * intensity * textureColor.B;
					}
				}
			}

			if (opacity > 1.0f) opacity = 1.0f;

			output.R = output.R * (1.0f - opacity) + newColour.R * opacity * params->cloudsColor.R;
			output.G = output.G * (1.0f - opacity) + newColour.G * opacity * params->cloudsColor.G;
			output.B = output.B * (1.0f - opacity) + newColour.B * opacity * params->cloudsColor.B;
			totalOpacity = opacity + (1.0f - opacity) * totalOpacity;
			output.A = opacity + (1.0f - opacity) * output.A;

			cloudsOpacity = cloud;
		}

		//		// iter fog
		//		if (params->iterFogEnabled)
		//		{
		//			float opacity = IterOpacity(step, iterations, params->N, params->iterFogOpacityTrim,
		//				params->iterFogOpacityTrimHigh, params->iterFogOpacity);
		//
		//			sRGBAfloat newColour(0.0, 0.0, 0.0, 0.0);
		//			if (opacity > 0)
		//			{
		//				// fog colour
		//				float iterFactor1 = (iterations - params->iterFogOpacityTrim)
		//														/ (params->iterFogColor1Maxiter - params->iterFogOpacityTrim);
		//				float k = iterFactor1;
		//				if (k > 1.0f) k = 1.0f;
		//				if (k < 0.0f) k = 0.0f;
		//				float kn = 1.0f - k;
		//				float fogColR = params->iterFogColour1.R * kn + params->iterFogColour2.R * k;
		//				float fogColG = params->iterFogColour1.G * kn + params->iterFogColour2.G * k;
		//				float fogColB = params->iterFogColour1.B * kn + params->iterFogColour2.B * k;
		//
		//				float iterFactor2 = (iterations - params->iterFogColor1Maxiter)
		//														/ (params->iterFogColor2Maxiter - params->iterFogColor1Maxiter);
		//				float k2 = iterFactor2;
		//				if (k2 < 0.0f) k2 = 0.0;
		//				if (k2 > 1.0f) k2 = 1.0;
		//				kn = 1.0f - k2;
		//				fogColR = fogColR * kn + params->iterFogColour3.R * k2;
		//				fogColG = fogColG * kn + params->iterFogColour3.G * k2;
		//				fogColB = fogColB * kn + params->iterFogColour3.B * k2;
		//				//----
		//
		//				for (int i = 0; i < data->lights.GetNumberOfLights(); i++)
		//				{
		//					const cLight *light = data->lights.GetLight(i);
		//					if (light->enabled)
		//					{
		//						double distanceLight = 0.0;
		//						CVector3 lightVectorTemp = light->CalculateLightVector(
		//							point, input2.delta, params->resolution, params->viewDistanceMax,
		// distanceLight);
		//
		//						float intensity;
		//						if (light->type == cLight::lightDirectional)
		//							intensity = light->intensity;
		//						else
		//							intensity = light->intensity / light->Decay(distanceLight)
		//													* params->iterFogBrightnessBoost * 4.0;
		//
		//						sRGBFloat textureColor;
		//						intensity *= light->CalculateCone(lightVectorTemp, textureColor);
		//
		//						sRGBAfloat lightShadow(1.0, 1.0, 1.0, 1.0);
		//						if (params->iterFogShadows && intensity > 1e-3)
		//						{
		//							lightShadow = AuxShadow(input2, light, distanceLight, lightVectorTemp);
		//						}
		//						newColour.R += lightShadow.R * light->color.R * intensity * textureColor.R;
		//						newColour.G += lightShadow.G * light->color.G * intensity * textureColor.G;
		//						newColour.B += lightShadow.B * light->color.B * intensity * textureColor.B;
		//					}
		//				}
		//
		//				if (params->ambientOcclusionEnabled
		//						&& params->ambientOcclusionMode == params::AOModeMultipleRays)
		//				{
		//					sRGBAfloat AO = AmbientOcclusion(input2);
		//					newColour.R += AO.R * params->ambientOcclusion;
		//					newColour.G += AO.G * params->ambientOcclusion;
		//					newColour.B += AO.B * params->ambientOcclusion;
		//				}
		//
		//				if (opacity > 1.0f) opacity = 1.0f;
		//
		//				output.R = output.R * (1.0f - opacity) + newColour.R * opacity * fogColR;
		//				output.G = output.G * (1.0f - opacity) + newColour.G * opacity * fogColG;
		//				output.B = output.B * (1.0f - opacity) + newColour.B * opacity * fogColB;
		//				totalOpacity = opacity + (1.0f - opacity) * totalOpacity;
		//				output.A = opacity + (1.0f - opacity) * output.A;
		//			}
		//		}

		//------------------ visible light
		if (data->lights.IsAnyLightEnabled())
		{
			for (int i = 0; i < data->lights.GetNumberOfLights(); ++i)
			{
				const cLight *light = data->lights.GetLight(i);
				if (light->enabled && light->intensity > 0.0 && light->visibility > 0.0
						&& light->type != cLight::lightDirectional)
				{
					double lastMiniSteps = -1.0;
					double miniStep;

					CVector3 lightPosition = light->CalculateBeam(light->position, light->target);

					for (double miniSteps = 0.0; miniSteps < step; miniSteps += miniStep)
					{
						CVector3 lightDistVect = point - input.viewVector * miniSteps - lightPosition;
						double lightDist = lightDistVect.Length();
						double lightSize = sqrt(light->intensity) * light->size;

						double distToLightSurface = lightDist - lightSize;
						if (distToLightSurface < 0.0) distToLightSurface = 0.0;

						miniStep = 0.1 * (distToLightSurface + 0.1 * distToLightSurface);
						if (miniStep > step - miniSteps) miniStep = step - miniSteps;
						if (miniStep < step * 0.001) miniStep = step * 0.001;

						double r2 = lightDist / lightSize;

						double bellFunction;
						if (light->type == cLight::lightConical)
						{
							bellFunction = 1.0 / (0.01 + pow(r2, double((light->decayFunction + 1) * 2)));
						}
						else
						{
							bellFunction = 1.0 / (1.0 + pow(r2, double((light->decayFunction + 1) * 2)));
						}

						CVector3 lightDirection = lightDistVect;
						lightDirection.Normalize();
						sRGBFloat textureColor;
						bellFunction *= light->CalculateCone((-1.0) * lightDirection, textureColor);

						float lightDensity = miniStep * bellFunction * light->visibility / lightSize;

						lightDensity *= 1.0f + params->cloudsLightsBoost * cloudsOpacity;

						output.R += lightDensity * light->color.R * textureColor.R;
						output.G += lightDensity * light->color.G * textureColor.G;
						output.B += lightDensity * light->color.B * textureColor.B;
						output.A += lightDensity;

						if (miniSteps == lastMiniSteps)
						{
							// qWarning() << "Dead computation\n"
							//		<< "\pointN:" << (point - input.viewVector * miniSteps).Debug();
							break;
						}
						lastMiniSteps = miniSteps;
					}
				}
			}
		}

		// fake lights (orbit trap)
		if (params->fakeLightsEnabled)
		{
			sFractalIn fractIn(point, params->minN, params->N, &params->common, -1, false);
			sFractalOut fractOut;
			Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
			float r = fractOut.orbitTrapR;
			r = sqrtf(1.0f / (r + 1.0e-20f));
			float fakeLight = 1.0f
												/ (powf(r, 10.0f / params->fakeLightsVisibilitySize)
														 * powf(10.0f, 10.0f / params->fakeLightsVisibilitySize)
													 + 0.1f);

			fakeLight *= 1.0f + params->cloudsLightsBoost * cloudsOpacity;

			output.R +=
				fakeLight * float(step) * params->fakeLightsVisibility * params->fakeLightsColor.R;
			output.G +=
				fakeLight * float(step) * params->fakeLightsVisibility * params->fakeLightsColor.G;
			output.B +=
				fakeLight * float(step) * params->fakeLightsVisibility * params->fakeLightsColor.B;
			output.A += fakeLight * float(step) * params->fakeLightsVisibility;
		}

		if (totalOpacity > 1.0f) totalOpacity = 1.0f;
		if (output.A > 1.0f) output.A = 1.0f;
		(*opacityOut).R = totalOpacity;
		(*opacityOut).G = totalOpacity;
		(*opacityOut).B = totalOpacity;

		if (end) break;

	} // next stepCount

	return output;
}
