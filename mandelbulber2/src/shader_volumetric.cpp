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

void cRenderWorker::RayleighScattering(const CVector3 &lightVectorTemp,
	const sShaderInputData &input, sRGBFloat &raleighScatteringRGB, sRGBFloat &mieScatteringRGB) const
{
	if (params->rayleighScatteringBlue > 0.0f)
	{
		float raleighScattering =
			(1.0 + pow(lightVectorTemp.Dot(input.viewVector), 2.0)) * params->rayleighScatteringBlue;
		raleighScatteringRGB.R = 1.0;
		raleighScatteringRGB.G = (1.0 + 0.2 * raleighScattering);
		raleighScatteringRGB.B = (1.0 + 2.0 * raleighScattering);
	}
	if (params->rayleighScatteringRed > 0.0f)
	{
		float mieScatteringR =
			pow(lightVectorTemp.Dot(input.viewVector) * 0.5 + 0.5, 15.0) * params->rayleighScatteringRed;
		float mieScatteringG =
			pow(lightVectorTemp.Dot(input.viewVector) * 0.5 + 0.5, 10.0) * params->rayleighScatteringRed;
		mieScatteringRGB.R = (1.0 + 5.0 * mieScatteringR);
		mieScatteringRGB.G = (1.0 + mieScatteringG);
		mieScatteringRGB.B = 1.0;
	}
}

sRGBAfloat cRenderWorker::VolumetricShader(
	const sShaderInputData &input, sRGBAfloat oldPixel, sRGBAfloat *opacityOut) const
{
	sRGBAfloat output;
	float totalOpacity = 0.0;

	output.R = oldPixel.R;
	output.G = oldPixel.G;
	output.B = oldPixel.B;
	output.A = oldPixel.A;

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

		float basicFogOpacity = 0.0f;
		if (params->fogEnabled)
		{
			basicFogOpacity = step / params->fogVisibility;

			if (params->primitives.primitiveIndexForBasicFog >= 0)
			{
				int closestId = -1;
				if (params->primitives.TotalDistance(point, distance, input2.delta, false, &closestId, data,
							params->primitives.primitiveIndexForBasicFog)
						> input2.delta)
					basicFogOpacity = 0;
			}
		}

		if (params->iterFogEnabled)
		{
			iterFogOpacity = IterOpacity(step, iterations, params->N, params->iterFogOpacityTrim,
				params->iterFogOpacityTrimHigh, params->iterFogOpacity);

			if (iterFogOpacity > 0.0 && params->primitives.primitiveIndexForIterFog >= 0)
			{
				int closestId = -1;
				if (params->primitives.TotalDistance(point, distance, input2.delta, false, &closestId, data,
							params->primitives.primitiveIndexForIterFog)
						> input2.delta)
					iterFogOpacity = 0;
			}

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

		double cloudsOpacity = 0.0;
		double cloudDensity = 0.0;
		CVector3 deltaCloud;

		if (params->cloudsEnable)
		{
			double distanceToClouds = 0.0;
			bool calculateClouds = true;

			if (params->primitives.primitiveIndexForClouds >= 0)
			{
				int closestId = -1;
				if (params->primitives.TotalDistance(point, distance, input2.delta, false, &closestId, data,
							params->primitives.primitiveIndexForClouds)
						> input2.delta)
				{
					cloudDensity = 0;
					calculateClouds = false;
				}
			}

			if (calculateClouds)
			{
				cloudDensity = CloudOpacity(point, distance, input2.delta, &distanceToClouds);
			}
			else
			{
				distanceToClouds = distance;
			}

			if (cloudDensity > 0.0 && !params->cloudsCastShadows)
			{
				double delta = params->cloudsPeriod / pow(2.0, params->cloudsIterations) * 5.0f;
				double distanceToCloudsDummy = 0.0;
				deltaCloud.x = CloudOpacity(point + CVector3(delta, 0.0, 0.0), distance, input2.delta,
												 &distanceToCloudsDummy)
											 - cloudDensity;
				deltaCloud.y = CloudOpacity(point + CVector3(0.0, delta, 0.0), distance, input2.delta,
												 &distanceToCloudsDummy)
											 - cloudDensity;
				deltaCloud.z = CloudOpacity(point + CVector3(0.0, 0.0, delta), distance, input2.delta,
												 &distanceToCloudsDummy)
											 - cloudDensity;

				if (deltaCloud.Length() > 0.0)
				{
					deltaCloud.Normalize();
				}
			}

			cloudsOpacity = cloudDensity * step;
			// qDebug() << cloud;

			lastCloudDistance = distanceToClouds;
		}

		double distFogOpacity = 0.0;
		sRGBFloat distFogColor(0.0, 0.0, 0.0);

		//-------------------- volumetric fog
		if (params->volFogDensity > 0.0f && params->volFogEnabled)
		{
			double colourThresh = params->volFogColour1Distance;
			double colourThresh2 = params->volFogColour2Distance;

			double distanceShifted;
			distFogOpacity = DistanceFogOpacity(step, distance, params->volFogDistanceFromSurface,
				params->volFogDistanceFactor, params->volFogDensity, distanceShifted);

			if (distFogOpacity > 0.0 && params->primitives.primitiveIndexForDistFog >= 0)
			{
				int closestId = -1;
				if (params->primitives.TotalDistance(point, distance, input2.delta, false, &closestId, data,
							params->primitives.primitiveIndexForDistFog)
						> input2.delta)
					distFogOpacity = 0;
			}

			float k = distanceShifted / colourThresh;
			if (k > 1) k = 1.0f;
			float kn = 1.0f - k;
			float fogTempR = params->volFogColour1.R * kn + params->volFogColour2.R * k;
			float fogTempG = params->volFogColour1.G * kn + params->volFogColour2.G * k;
			float fogTempB = params->volFogColour1.B * kn + params->volFogColour2.B * k;

			float k2 = distanceShifted / colourThresh2 * k;
			if (k2 > 1) k2 = 1.0;
			kn = 1.0f - k2;
			distFogColor.R = fogTempR * kn + params->volFogColour3.R * k2;
			distFogColor.G = fogTempG * kn + params->volFogColour3.G * k2;
			distFogColor.B = fogTempB * kn + params->volFogColour3.B * k2;
		}

		sRGBAfloat totalLightsWithShadows(0.0, 0.0, 0.0, 0.0);
		sRGBAfloat totalLights(0.0, 0.0, 0.0, 0.0);
		sRGBAfloat totalLightsClouds(0.0, 0.0, 0.0, 0.0);

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

					if (params->cloudsEnable && cloudsOpacity > 0.0)
					{
						lightNeeded = true;
						if (params->cloudsCastShadows) shadowNeeded = true;
					}

					if (params->distanceFogShadows && distFogOpacity > 0.0)
					{
						lightNeeded = true;
						shadowNeeded = true;
					}

					if (params->fogCastShadows && basicFogOpacity > 0.0)
					{
						lightNeeded = true;
						shadowNeeded = true;
					}

					if (!light->castShadows) shadowNeeded = false;

					if (lightNeeded)
					{
						double distanceLight = 0.0;
						CVector3 lightVectorTemp = light->CalculateLightVector(
							point, input2.delta, params->resolution, params->viewDistanceMax, distanceLight);

						float lightIntensity;
						if (light->type == cLight::lightDirectional)
							lightIntensity = light->intensity;
						else
							lightIntensity = light->intensity / light->Decay(distanceLight) * 4.0;

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

						sRGBFloat raleighScatteringRGB(1.0, 1.0, 1.0);
						sRGBFloat mieScatteringRGB(1.0, 1.0, 1.0);

						RayleighScattering(lightVectorTemp, input, raleighScatteringRGB, mieScatteringRGB);

						sRGBFloat calculatedLight(0.0, 0.0, 0.0);
						calculatedLight.R = light->color.R * lightIntensity * textureColor.R
																* raleighScatteringRGB.R * mieScatteringRGB.R;
						calculatedLight.G = light->color.G * lightIntensity * textureColor.G
																* raleighScatteringRGB.G * mieScatteringRGB.G;
						calculatedLight.B = light->color.B * lightIntensity * textureColor.B
																* raleighScatteringRGB.B * mieScatteringRGB.B;

						totalLightsWithShadows.R += calculatedLight.R * lightShadow.R;
						totalLightsWithShadows.G += calculatedLight.G * lightShadow.G;
						totalLightsWithShadows.B += calculatedLight.B * lightShadow.B;

						totalLights.R += calculatedLight.R;
						totalLights.G += calculatedLight.G;
						totalLights.B += calculatedLight.B;

						double shadeClouds = clamp(-lightVectorTemp.Dot(deltaCloud), 0.0, 1.0);
						totalLightsClouds.R += calculatedLight.R * shadeClouds;
						totalLightsClouds.G += calculatedLight.G * shadeClouds;
						totalLightsClouds.B += calculatedLight.B * shadeClouds;

						if (light->volumetric)
						{
							output.R +=
								calculatedLight.R * light->volumetricVisibility * lightShadow.R * float(step);
							output.G +=
								calculatedLight.G * light->volumetricVisibility * lightShadow.G * float(step);
							output.B +=
								calculatedLight.B * light->volumetricVisibility * lightShadow.B * float(step);
							output.A +=
								lightShadow.A * float(step) * lightIntensity * light->volumetricVisibility;
						}
					} // if light needed
				}		// if light enabled
			}			// if any light enabled
		}				// next light

		if (params->DOFMonteCarloGlobalIllumination && params->monteCarloGIOfVolumetric)
		{
			sRGBFloat gi = GlobalIlumination(input2, sRGBAfloat(1.0, 1.0, 1.0, 0.0), true);
			totalLights.R += gi.R;
			totalLights.G += gi.G;
			totalLights.B += gi.B;
			totalLightsWithShadows.R += gi.R;
			totalLightsWithShadows.G += gi.G;
			totalLightsWithShadows.B += gi.B;
		}

		sRGBAfloat AO(0.0, 0.0, 0.0, 0.0);

		if (params->ambientOcclusionEnabled
				&& params->ambientOcclusionMode == params::AOModeMultipleRays)
		{
			if ((params->iterFogEnabled && iterFogOpacity > 0.0)
				/*|| (params->volFogEnabled && distFogOpacity > 0.0)*/)
			{
				AO = AmbientOcclusion(input2);
				AO.R *= params->ambientOcclusion;
				AO.G *= params->ambientOcclusion;
				AO.B *= params->ambientOcclusion;
			}
		}

		if (params->iterFogEnabled && iterFogOpacity > 0.0)
		{
			sRGBAfloat light = (params->iterFogShadows) ? totalLightsWithShadows : totalLights;

			if (iterFogOpacity > 1.0f) iterFogOpacity = 1.0f;

			output.R =
				output.R * (1.0f - iterFogOpacity)
				+ (light.R * params->iterFogBrightnessBoost + AO.R) * iterFogOpacity * iterFogCol.R;

			output.G =
				output.G * (1.0f - iterFogOpacity)
				+ (light.G * params->iterFogBrightnessBoost + AO.G) * iterFogOpacity * iterFogCol.G;

			output.B =
				output.B * (1.0f - iterFogOpacity)
				+ (light.B * params->iterFogBrightnessBoost + AO.B) * iterFogOpacity * iterFogCol.B;

			totalOpacity = iterFogOpacity + (1.0f - iterFogOpacity) * totalOpacity;
			output.A = iterFogOpacity + (1.0f - iterFogOpacity) * output.A;
		}

		if (params->cloudsEnable && cloudsOpacity > 0.0)
		{
			sRGBAfloat light = (params->cloudsCastShadows) ? totalLightsWithShadows : totalLightsClouds;

			double ambient = params->cloudsAmbientLight;
			double nAmbient = 1.0 - params->cloudsAmbientLight;

			light.R = ambient * totalLights.R + nAmbient * light.R;
			light.G = ambient * totalLights.G + nAmbient * light.G;
			light.B = ambient * totalLights.B + nAmbient * light.B;

			if (cloudsOpacity > 1.0f) cloudsOpacity = 1.0f;

			output.R = output.R * (1.0f - cloudsOpacity)
								 + (light.R + AO.R) * cloudsOpacity * params->cloudsColor.R;
			output.G = output.G * (1.0f - cloudsOpacity)
								 + (light.G + AO.G) * cloudsOpacity * params->cloudsColor.G;
			output.B = output.B * (1.0f - cloudsOpacity)
								 + (light.B + AO.B) * cloudsOpacity * params->cloudsColor.B;
			totalOpacity = cloudsOpacity + (1.0f - cloudsOpacity) * totalOpacity;
			output.A = cloudsOpacity + (1.0f - cloudsOpacity) * output.A;
		}

		if (params->volFogEnabled && distFogOpacity > 0.0)
		{
			sRGBAfloat light =
				(params->distanceFogShadows) ? totalLightsWithShadows : sRGBFloat(1.0, 1.0, 1.0);

			if (distFogOpacity > 1.0) distFogOpacity = 1.0;

			output.R =
				distFogOpacity * distFogColor.R * (light.R + AO.R) + (1.0f - distFogOpacity) * output.R;
			output.G =
				distFogOpacity * distFogColor.G * (light.G + AO.G) + (1.0f - distFogOpacity) * output.G;
			output.B =
				distFogOpacity * distFogColor.B * (light.B + AO.B) + (1.0f - distFogOpacity) * output.B;

			totalOpacity = distFogOpacity + (1.0f - distFogOpacity) * totalOpacity;
			output.A = distFogOpacity + (1.0f - distFogOpacity) * output.A;
		}

		//----------------------- basic fog
		if (params->fogEnabled && basicFogOpacity > 0.0)
		{

			sRGBAfloat light =
				(params->fogCastShadows) ? totalLightsWithShadows : sRGBFloat(1.0, 1.0, 1.0);
			if (basicFogOpacity > 1.0f) basicFogOpacity = 1.0f;
			output.R = basicFogOpacity * params->fogColor.R * (light.R + AO.R)
								 + (1.0f - basicFogOpacity) * output.R;
			output.G = basicFogOpacity * params->fogColor.G * (light.G + AO.G)
								 + (1.0f - basicFogOpacity) * output.G;
			output.B = basicFogOpacity * params->fogColor.B * (light.B + AO.B)
								 + (1.0f - basicFogOpacity) * output.B;
			totalOpacity = basicFogOpacity + (1.0f - basicFogOpacity) * totalOpacity;
			output.A = basicFogOpacity + (1.0f - basicFogOpacity) * output.A;
		}

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

						lightDensity *= 1.0f + params->cloudsLightsBoost * cloudDensity;

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
			int fakeLightMaxLoop = 1;
			if (params->common.fakeLightsColor2Enabled) fakeLightMaxLoop = 2;
			if (params->common.fakeLightsColor3Enabled) fakeLightMaxLoop = 3;

			for (int fakeLightLoop = 0; fakeLightLoop < fakeLightMaxLoop; fakeLightLoop++)
			{
				sFractalIn fractIn(point, params->minN, -1, 1, fakeLightLoop, &params->common, -1, false);
				sFractalOut fractOut;
				Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
				float r = fractOut.orbitTrapR;
				r = sqrtf(1.0f / (r + 1.0e-20f));
				float fakeLight = 1.0f
													/ (powf(r, 10.0f / params->fakeLightsVisibilitySize)
															 * powf(10.0f, 10.0f / params->fakeLightsVisibilitySize)
														 + 0.1f);

				fakeLight *= 1.0f + params->cloudsLightsBoost * cloudDensity;

				sRGBFloat color;
				switch (fakeLightLoop)
				{
					case 0: color = params->fakeLightsColor; break;
					case 1: color = params->fakeLightsColor2; break;
					case 2: color = params->fakeLightsColor3; break;
					default: color = params->fakeLightsColor; break;
				}

				output.R += fakeLight * float(step) * params->fakeLightsVisibility * color.R;
				output.G += fakeLight * float(step) * params->fakeLightsVisibility * color.G;
				output.B += fakeLight * float(step) * params->fakeLightsVisibility * color.B;
				output.A += fakeLight * float(step) * params->fakeLightsVisibility;
			}
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
