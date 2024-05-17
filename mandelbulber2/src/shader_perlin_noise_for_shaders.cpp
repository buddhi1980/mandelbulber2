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
 * cRenderWorker::PerlinNoiseForShaders method - calculates of perlin noise for shaders
 *
 */

#include "shader_perlin_noise_for_shaders.hpp"
#include "common_math.h"
#include "render_worker.hpp"
#include "material.h"
#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "perlin_noise_octaves.h"
#include "render_data.hpp"

void cRenderWorker::PerlinNoiseForShaders(sShaderInputData *shaderInputData) const
{
	if (shaderInputData->material->perlinNoiseEnable)
	{
		CVector3 pointModified;
		if (shaderInputData->material->textureFractalize)
		{
			sFractalIn fractIn(
				shaderInputData->point, 0, -1, 1, 0, &params->common, -1, false, shaderInputData->material);
			sFractalOut fractOut;
			Compute<fractal::calcModeCubeOrbitTrap>(*fractal, fractIn, &fractOut);
			pointModified = fractOut.z;
		}
		else
		{
			pointModified = shaderInputData->point;
		}
		float perlin = data->perlinNoise->normalizedOctaveNoise3D_0_1(
			pointModified.x / shaderInputData->material->perlinNoisePeriod.x,
			pointModified.y / shaderInputData->material->perlinNoisePeriod.y,
			pointModified.z / shaderInputData->material->perlinNoisePeriod.z, 0.0, 0.0, 0.0,
			shaderInputData->material->perlinNoiseIterations);
		perlin += shaderInputData->material->perlinNoiseValueOffset;
		if (shaderInputData->material->perlinNoiseAbs) perlin = fabs(perlin - 0.5) * 2.0;

		perlin = clamp(perlin, 0.0f, 1.0f);
		shaderInputData->perlinNoise = perlin;
	}
	else
	{
		shaderInputData->perlinNoise = 0.0f;
	}
}

double PerlinNoiseDisplacement(
	double distance, const CVector3 &point, sRenderData *data, int objectId)
{
	if (data)
	{
		const cMaterial *mat = &data->materials[data->objectData[objectId].materialId];
		if (mat->perlinNoiseEnable && mat->perlinNoiseDisplacementEnable)
		{
			double perlin = data->perlinNoise->normalizedOctaveNoise3D_0_1(
				point.x / mat->perlinNoisePeriod.x, point.y / mat->perlinNoisePeriod.y,
				point.z / mat->perlinNoisePeriod.z, 0.0, 0.0, 0.0, mat->perlinNoiseIterations);
			if (mat->perlinNoiseAbs) perlin = fabs(perlin - 0.5) * 2.0;

			perlin += mat->perlinNoiseValueOffset;
			distance -= perlin * mat->perlinNoiseDisplacementIntensity;
		}
	}
	return distance;
}
