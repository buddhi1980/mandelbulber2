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
 * cRenderWorker::GlobalIlumination method - calculates global illumination using Monte Carlo
 * algorithm
 */
#include "calculate_distance.hpp"
#include "common_math.h"
#include "fractparams.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"

sRGBFloat cRenderWorker::GlobalIlumination(
	const sShaderInputData &input, sRGBAfloat objectColor) const
{
	sRGBFloat out;
	sShaderInputData inputCopy = input;
	sRGBAfloat objectColorTemp = objectColor;

	sStep *stepBuff = new sStep[maxRaymarchingSteps + 2];
	inputCopy.stepBuff = stepBuff;
	inputCopy.stepCount = 0;

	sRGBAfloat resultShader;
	sRGBAfloat newColor = objectColor;
	double totalOpacity = 0.0;

	bool finished = false;
	for (int rayDepth = 0; rayDepth < params->reflectionsMax; rayDepth++)
	{
		CVector3 reflectedDirection = inputCopy.normal;
		double randomX = (Random(20000) - 10000) / 10000.0;
		double randomY = (Random(20000) - 10000) / 10000.0;
		double randomZ = (Random(20000) - 10000) / 10000.0;
		CVector3 randomVector(randomX * 1.2, randomY * 1.2, randomZ * 1.2);
		CVector3 randomizedDirection = reflectedDirection + randomVector;
		randomizedDirection.Normalize();
		inputCopy.viewVector = randomizedDirection;

		double dist = 0.0f;
		bool found = false;
		int objectId = 0;
		inputCopy.stepCount = 0;
		int stepCount = 0;

		for (double scan = inputCopy.distThresh; scan < params->viewDistanceMax;
				 scan += dist * params->DEFactor)
		{
			if (stepCount >= maxRaymarchingSteps) break;
			sStep step;
			step.step = dist * params->DEFactor;

			CVector3 point = inputCopy.point + scan * randomizedDirection;

			double distThresh = CalcDistThresh(point);
			inputCopy.distThresh = distThresh;
			inputCopy.delta = distThresh;

			sDistanceOut distanceOut;
			sDistanceIn distanceIn(point, distThresh, false);
			dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);
			objectId = distanceOut.objectId;

			step.distThresh = distThresh;
			step.distance = dist;
			step.iters = distanceOut.iters;
			step.point = point;
			stepBuff[stepCount] = step;

			if (dist < distThresh)
			{
				if (scan < distThresh * 2.0)
				{
					delete[] stepBuff;
					return out;
				}
				inputCopy.point = point;
				CVector3 vn = CalculateNormals(inputCopy);
				inputCopy.normal = vn;
				inputCopy.objectId = objectId;

				found = true;
				break;
			}
			stepCount++;
		}

		inputCopy.stepCount = stepCount;

		if (found)
		{
			sRGBAfloat objectColor;
			sRGBAfloat specular;
			sRGBFloat iridescence;

			cObjectData objectData = data->objectData[inputCopy.objectId];
			inputCopy.material = &data->materials[objectData.materialId];

			// letting colors from textures (before normal map shader)
			if (inputCopy.material->colorTexture.IsLoaded())
			{
				inputCopy.texColor = TextureShader(inputCopy, texture::texColor, inputCopy.material);
			}
			else
				inputCopy.texColor = sRGBFloat(1.0, 1.0, 1.0);

			if (inputCopy.material->luminosityTexture.IsLoaded())
				inputCopy.texLuminosity =
					TextureShader(inputCopy, texture::texLuminosity, inputCopy.material);
			else
				inputCopy.texLuminosity = sRGBFloat(0.0, 0.0, 0.0);

			if (inputCopy.material->diffusionTexture.IsLoaded())
				inputCopy.texDiffuse = TextureShader(inputCopy, texture::texDiffuse, inputCopy.material);
			else
				inputCopy.texDiffuse = sRGBFloat(1.0, 1.0, 1.0);

			sGradientsCollection gradients;
			sRGBAfloat objectShader =
				ObjectShader(inputCopy, &objectColor, &specular, &iridescence, &gradients);

			newColor = objectColor;
			resultShader.R = objectShader.R + specular.R;
			resultShader.G = objectShader.G + specular.G;
			resultShader.B = objectShader.B + specular.B;

			out.R += (objectShader.R + specular.R) * objectColorTemp.R;
			out.G += (objectShader.G + specular.G) * objectColorTemp.G;
			out.B += (objectShader.B + specular.B) * objectColorTemp.B;
		}
		else
		{
			sRGBAfloat backgroundShader = BackgroundShader(inputCopy);
			resultShader = backgroundShader;
			finished = true;
		}

		sRGBAfloat opacity;
		if (params->monteCarloGIVolumetric)
		{
			resultShader = VolumetricShader(inputCopy, resultShader, &opacity);
		}
		double influence = qBound(0.0, 1.0 - totalOpacity, 1.0);

		out.R += resultShader.R * objectColorTemp.R * influence;
		out.G += resultShader.G * objectColorTemp.G * influence;
		out.B += resultShader.B * objectColorTemp.B * influence;

		totalOpacity += opacity.R;
		objectColorTemp = newColor;

		if (finished || totalOpacity > 1.0) break;
	}

	delete[] stepBuff;
	return out;
}
