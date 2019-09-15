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
 * cRenderWorker::SpecularHighlight method - calculates specular highlight
 */
#include "common_math.h"
#include "material.h"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::SpecularHighlight(const sShaderInputData &input, CVector3 lightVector,
	float specularWidth, float roughness, sRGBFloat diffuseGradient) const
{
	sRGBAfloat specular;
	CVector3 half = lightVector - input.viewVector;
	half.Normalize();
	float shade2 = input.normal.Dot(half);
	if (shade2 < 0.0f) shade2 = 0.0f;

	float diffuse = 1.0;

	if (input.material->useDiffusionTexture)
	{
		diffuse *= 10.0f
							 * (1.1f
									 - input.material->diffusionTextureIntensity
											 * (input.texDiffuse.R + input.texDiffuse.G + input.texDiffuse.B) / 3.0f);
	}

	if (input.material->useColorsFromPalette && input.material->diffuseGradientEnable)
	{
		diffuse *= 10.0f * (1.1f - (diffuseGradient.R + diffuseGradient.G + diffuseGradient.B) / 3.0f);
	}

	shade2 = pow(shade2, 30.0f / specularWidth / diffuse) / diffuse;

	if (roughness > 0.0f)
	{
		shade2 *= (1.0 + Random(1000) / 1000.0f * roughness);
	}
	if (shade2 > 15.0f) shade2 = 15.0f;
	specular.R =
		shade2 * input.material->specularColor.R * (input.texDiffuse.R * 0.5f + 0.5f);
	specular.G =
		shade2 * input.material->specularColor.G * (input.texDiffuse.G * 0.5f + 0.5f);
	specular.B =
		shade2 * input.material->specularColor.B * (input.texDiffuse.B * 0.5f + 0.5f);
	return specular;
}
