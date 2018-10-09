/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * cRenderWorker::EnvMapping method - calculates enviroment mapping effect
 */
#include "common_math.h"
#include "render_data.hpp"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::EnvMapping(const sShaderInputData &input) const
{
	sRGBAfloat envReflect;
	CVector3 reflect;
	double dot = -input.viewVector.Dot(input.normal);
	reflect = input.normal * 2.0 * dot + input.viewVector;

	double alphaTexture = -reflect.GetAlpha() + M_PI;
	double betaTexture = -reflect.GetBeta();
	double texWidth = data->textures.envmapTexture.Width();
	double texHeight = data->textures.envmapTexture.Height();

	if (betaTexture > 0.5 * M_PI) betaTexture = 0.5 * M_PI - betaTexture;

	if (betaTexture < -0.5 * M_PI) betaTexture = -0.5 * M_PI + betaTexture;

	double dtx = alphaTexture / (2.0 * M_PI) * texWidth + texWidth * 8.25;
	double dty = (betaTexture / M_PI + 0.5) * texHeight + texHeight * 8.0;
	dtx = fmod(dtx, texWidth);
	dty = fmod(dty, texHeight);
	if (dtx < 0) dtx = 0;
	if (dty < 0) dty = 0;

	float reflectance = 1.0f;
	if (input.material->fresnelReflectance)
	{
		double n1 = 1.0;
		double n2 = input.material->transparencyIndexOfRefraction;
		reflectance = Reflectance(input.normal, input.viewVector, n1, n2);
		if (reflectance < 0.0f) reflectance = 0.0f;
		if (reflectance > 1.0f) reflectance = 1.0f;
	}

	envReflect.R = data->textures.envmapTexture.Pixel(dtx, dty).R * reflectance;
	envReflect.G = data->textures.envmapTexture.Pixel(dtx, dty).G * reflectance;
	envReflect.B = data->textures.envmapTexture.Pixel(dtx, dty).B * reflectance;
	return envReflect;
}
