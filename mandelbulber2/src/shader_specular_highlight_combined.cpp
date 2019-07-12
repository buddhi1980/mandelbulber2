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
 * cRenderWorker::SpecularHighlightCombined method - calculates combined specular highlight for
 * plastic and metalic surfaces
 */

#include "material.h"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::SpecularHighlightCombined(const sShaderInputData &input,
	CVector3 lightVector, sRGBAfloat surfaceColor, sRGBFloat diffuseGradient) const
{
	sRGBAfloat specular;
	sRGBAfloat specularPlastic;
	if (input.material->specularPlasticEnable)
	{
		specularPlastic =
			SpecularHighlight(input, lightVector, input.material->specularWidth, 0.0f, diffuseGradient);
		specularPlastic.R *= input.material->specular;
		specularPlastic.G *= input.material->specular;
		specularPlastic.B *= input.material->specular;
	}

	sRGBAfloat specularMetallic;
	if (input.material->metallic)
	{
		specularMetallic = SpecularHighlight(input, lightVector, input.material->specularMetallicWidth,
			input.material->specularMetallicRoughness, diffuseGradient);
		specularMetallic.R *= input.material->specularMetallic * surfaceColor.R;
		specularMetallic.G *= input.material->specularMetallic * surfaceColor.G;
		specularMetallic.B *= input.material->specularMetallic * surfaceColor.B;
	}

	specular.R = specularPlastic.R + specularMetallic.R;
	specular.G = specularPlastic.G + specularMetallic.G;
	specular.B = specularPlastic.B + specularMetallic.B;

	return specular;
}
