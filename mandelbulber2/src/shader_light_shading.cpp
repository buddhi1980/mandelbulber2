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
 * cRenderWorker::LightShading method - calculates auxiliary light sources
 */
#include "common_math.h"
#include "fractparams.hpp"
#include "lights.hpp"
#include "material.h"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::LightShading(const sShaderInputData &input, sRGBAfloat surfaceColor,
	const cLight *light, sGradientsCollection *gradients, sRGBAfloat *outSpecular) const
{
	sRGBAfloat shading;

	double distance = 0.0;
	CVector3 lightVector = light->CalculateLightVector(
		input.point, input.delta, params->resolution, params->viewDistanceMax, distance);

	// intensity of lights is divided by 6 because of backward compatibility. There was an error
	// where number of light was always 24
	// float intensity = 100.0f * light->intensity / light->Decay(distance) / number / 6;
	float intensity;
	if (light->type == cLight::lightDirectional)
	{
		intensity = light->intensity;
	}
	else
	{
		intensity = 100.0f * light->intensity / light->Decay(distance) / 6.0f;
	}

	sRGBFloat textureColor;
	intensity *= light->CalculateCone(lightVector, textureColor);

	float shade = input.normal.Dot(lightVector);
	if (shade < 0) shade = 0;
	shade = 1.0f - input.material->shading + shade * input.material->shading;

	shade = shade * intensity;
	if (shade > 500.0f) shade = 500.0f;

	// specular
	sRGBAfloat specular =
		SpecularHighlightCombined(input, lightVector, surfaceColor, gradients->diffuse);
	specular.R *= intensity;
	specular.G *= intensity;
	specular.B *= intensity;

	if (input.material->useColorsFromPalette && input.material->specularGradientEnable)
	{
		specular.R *= gradients->specular.R / 256.0;
		specular.G *= gradients->specular.G / 256.0;
		specular.B *= gradients->specular.B / 256.0;
	}

	float specularMax = dMax(specular.R, specular.G, specular.B);

	// calculate shadow
	sRGBAfloat auxShadow(1.0, 1.0, 1.0, 1.0);
	if (light->castShadows)
	{
		if (shade > 0.001f || specularMax > 0.001f)
		{
			auxShadow = AuxShadow(input, light, distance, lightVector);
			specular.R *= auxShadow.R;
			specular.G *= auxShadow.G;
			specular.B *= auxShadow.B;
		}
		else
		{
			auxShadow = sRGBAfloat();
			specular = sRGBAfloat();
		}
	}

	shading.R = shade * light->color.R * auxShadow.R * textureColor.R;
	shading.G = shade * light->color.G * auxShadow.G * textureColor.G;
	shading.B = shade * light->color.B * auxShadow.B * textureColor.B;

	outSpecular->R = specular.R * light->color.R * textureColor.R;
	outSpecular->G = specular.G * light->color.G * textureColor.G;
	outSpecular->B = specular.B * light->color.B * textureColor.B;

	return shading;
}
