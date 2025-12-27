/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cRenderWorker::AuxShadow method - calculates shadows for auxiliary light sources
 */
#include "calculate_distance.hpp"
#include "fractparams.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"

sRGBAFloat cRenderWorker::AuxShadow(
	sShaderInputData &input, const cLight *light, double distance, CVector3 lightVector) const
{
	sRGBAFloat lightShaded(1.0, 1.0, 1.0, 1.0);
	double totalOpacity = 0.0;
	double shadowTemp = 1.0;

	bool cloudMode = params->cloudsEnable;

	double DEFactor = params->DEFactor;
	if (params->iterFogEnabled || light->volumetric || params->distanceFogShadows) DEFactor = 1.0;
	if (cloudMode) DEFactor = params->DEFactor * params->volumetricLightDEFactor;

	// remark: for aux light was: i = input.delta;
	double start = input.distThresh;
	if (params->interiorMode) start = input.distThresh * DEFactor;

	double softRange;
	if (params->monteCarloSoftShadows && light->type != cLight::lightDirectional)
	{
		double lightSize = sqrt(light->intensity) * light->size;
		softRange = lightSize / distance;
	}
	else
	{
		softRange = tan(light->softShadowCone);
	}

	bool goThrough = input.material->subsurfaceScattering;
	sShaderInputData input2;
	if (goThrough) input2 = input;

	double maxSoft = 0.0;

	const bool bSoft = !goThrough && !cloudMode && !params->iterFogEnabled
										 && !params->distanceFogShadows && !params->fogCastShadows
										 && !params->common.iterThreshMode && !params->interiorMode && softRange > 0.0
										 && !(params->monteCarloSoftShadows && params->DOFMonteCarlo);

	if (params->DOFMonteCarlo && params->monteCarloSoftShadows)
	{
		CVector3 randomVector;
		randomVector.x = Random(10000) / 5000.0 - 1.0;
		randomVector.y = Random(10000) / 5000.0 - 1.0;
		randomVector.z = Random(10000) / 5000.0 - 1.0;
		double randomSphereRadius = pow(Random(10000) / 10000.0, 1.0 / 3.0);
		CVector3 randomSphere = randomVector * (softRange * randomSphereRadius / randomVector.Length());
		lightVector += randomSphere;
		lightVector.Normalize();
	}

	double lastDistanceToClouds = 1e6f;
	int count = 0;
	double step = input.distThresh;

	for (double i = start; i < distance; i += step)
	{
		CVector3 point2 = input.point + lightVector * i;

		float dist_thresh;
		if (params->iterFogEnabled || light->volumetric || params->distanceFogShadows || cloudMode
				|| goThrough)
		{
			dist_thresh = CalcDistThresh(point2);
		}
		else
			dist_thresh = input.distThresh;

		sDistanceOut distanceOut;
		sDistanceIn distanceIn(point2, input.distThresh, false);
		double dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		cObjectData &objectData = data->objectData[distanceOut.objectId];
		cMaterial *material = &data->materials[objectData.materialId];
		goThrough = material->subsurfaceScattering;

		bool limitsReached = false;
		if (params->limitsEnabled)
		{
			if (point2.x < params->limitMin.x || point2.x > params->limitMax.x
					|| point2.y < params->limitMin.y || point2.y > params->limitMax.y
					|| point2.z < params->limitMin.z || point2.z > params->limitMax.z)
			{
				limitsReached = true;
			}
		}

		if (bSoft && !limitsReached)
		{
			double angle = (dist - dist_thresh) / i;
			if (angle < 0) angle = 0;
			if (dist < dist_thresh) angle = 0;
			double softShadow = 1.0 - angle / softRange;
			if (light->penetrating) softShadow *= (distance - i) / distance;
			if (softShadow < 0) softShadow = 0;
			if (softShadow > maxSoft) maxSoft = softShadow;
		}

		if (params->iterFogEnabled)
		{
			double opacity = IterOpacity(step, distanceOut.iters, params->N, params->iterFogOpacityTrim,
				params->iterFogOpacityTrimHigh, params->iterFogOpacity);
			if (opacity > 0.0 && params->primitives.primitiveIndexForIterFog >= 0)
			{
				int closestId = -1;
				if (params->primitives.TotalDistance(point2, dist, dist_thresh, false, &closestId, data,
							params->primitives.primitiveIndexForIterFog)
						> dist_thresh)
					opacity = 0.0f;
			}
			opacity *= (distance - i) / distance;
			opacity = qMin(opacity, 1.0);
			totalOpacity = opacity + (1.0 - opacity) * totalOpacity;
		}

		if (params->volFogEnabled && params->distanceFogShadows)
		{
			double distanceShifted;
			double opacity = DistanceFogOpacity(step, dist, params->volFogDistanceFromSurface,
				params->volFogDistanceFactor, params->volFogDensity, distanceShifted);
			if (opacity > 0.0 && params->primitives.primitiveIndexForDistFog >= 0)
			{
				int closestId = -1;
				if (params->primitives.TotalDistance(point2, dist, dist_thresh, false, &closestId, data,
							params->primitives.primitiveIndexForDistFog)
						> dist_thresh)
					opacity = 0.0f;
			}
			opacity *= (distance - i) / distance;
			opacity = qMin(opacity, 1.0);
			totalOpacity = opacity + (1.0 - opacity) * totalOpacity;
		}

		if (cloudMode)
		{
			double distanceToClouds = 0.0;
			bool calculateClouds = true;
			double opacity = 0.0;

			if (params->primitives.primitiveIndexForClouds >= 0)
			{
				int closestId = -1;
				if (params->primitives.TotalDistance(point2, dist, dist_thresh, false, &closestId, data,
							params->primitives.primitiveIndexForClouds)
						> dist_thresh)
				{
					opacity = 0.0f;
					calculateClouds = false;
				}
			}

			if (calculateClouds)
			{
				opacity = CloudOpacity(point2, dist, dist_thresh, &distanceToClouds) * step;
			}
			else
			{
				distanceToClouds = dist;
			}

			lastDistanceToClouds = distanceToClouds;
			opacity *= (distance - i) / distance;
			opacity = qMin(opacity, 1.0);
			totalOpacity = opacity + (1.0 - opacity) * totalOpacity;
		}

		if (params->fogEnabled && params->fogCastShadows)
		{
			double opacity = step / params->fogVisibility;
			if (params->primitives.primitiveIndexForBasicFog >= 0)
			{
				int closestId = -1;
				if (params->primitives.TotalDistance(point2, dist, dist_thresh, false, &closestId, data,
							params->primitives.primitiveIndexForBasicFog)
						> dist_thresh)
					opacity = 0.0f;
			}
			opacity *= (distance - i) / distance;
			opacity = qMin(opacity, 1.0);
			totalOpacity = opacity + (1.0 - opacity) * totalOpacity;
		}

		if (goThrough && dist < dist_thresh)
		{
			double opacityCollected = 1.0;
			if (material->insideColoringEnable && material->diffuseGradientEnable)
			{
				sGradientsCollection gradients;
				input2.objectId = distanceOut.objectId;
				input2.point = point2;
				SurfaceColour(point2, input2, &gradients);
				opacityCollected = gradients.diffuse.R;
			}

			float texOpacity = 0.0;
			if (input.material->useTransparencyAlphaTexture)
			{
				if (input.material->transparencyAlphaTexture.IsLoaded())
				{
					sRGBFloat tex = TextureShader(input2, texture::texTransparencyAlpha, input.material);
					texOpacity =
						(1.0f - tex.R) * input.material->transparencyAlphaTextureIntensityVol + 1e-6f;
					opacityCollected = opacityCollected * (1.0 - texOpacity) + texOpacity;
				}
			}

			if (input.material->perlinNoiseEnable && input.material->perlinNoiseTransparencyAlphaEnable)
			{
				PerlinNoiseForShaders(&input, point2);
				float alpha = (input.material->perlinNoiseTransparencyColorInvert)
												? 1.0f - input2.perlinNoise
												: input2.perlinNoise;
				alpha = clamp(alpha * input.material->perlinNoiseTransparencyAlphaIntensityVol, 0.0f, 1.0f);
				alpha = 1.0f - alpha;
				opacityCollected = opacityCollected * (1.0f - alpha) + alpha + 1e-6f;
			}

			double opacity =
				(-1.0f + 1.0f / (opacityCollected * input.material->transparencyOfInterior)) * step;
			opacity *= (distance - i) / distance;
			opacity = qMin(opacity, 1.0);
			totalOpacity = opacity + (1.0 - opacity) * totalOpacity;
			dist = CalcDelta(point2);
		}

		shadowTemp = 1.0 - totalOpacity;

		if ((!goThrough && (dist < dist_thresh || shadowTemp < 0.0))
				|| (goThrough && shadowTemp < 0.0001))
		{
			if (light->penetrating)
			{
				shadowTemp -= (distance - i) / distance;
				if (shadowTemp < 0.0) shadowTemp = 0.0;
			}
			else
			{
				shadowTemp = 0.0;
			}
			break;
		}

		step = std::min(dist, lastDistanceToClouds) * DEFactor;
		step = std::max(step, 1e-15);

		count++;
		if (count > params->maxRaymarchingSteps) break;
	}

	if (!bSoft)
	{
		lightShaded.R = shadowTemp;
		lightShaded.G = shadowTemp;
		lightShaded.B = shadowTemp;
	}
	else
	{
		lightShaded.R = 1.0f - maxSoft;
		lightShaded.G = 1.0f - maxSoft;
		lightShaded.B = 1.0f - maxSoft;
	}
	return lightShaded;
}
