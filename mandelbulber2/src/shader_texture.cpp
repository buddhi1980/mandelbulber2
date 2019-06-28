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
 * cRenderWorker::TextureShader method - calculates texture
 */
#include <algorithm>

#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"
#include "texture_mapping.hpp"

using std::max;

sRGBFloat cRenderWorker::TextureShader(
	const sShaderInputData &input, texture::enumTextureSelection texSelect, cMaterial *mat) const
{
	cObjectData objectData = data->objectData[input.objectId];
	double texturePixelSize = 0.0;
	CVector3 textureVectorX, textureVectorY;

	CVector3 pointFractalized;

	if (mat->textureFractalize)
	{
		sFractalIn fractIn(input.point, 0, params->N, params->common, -1, false, input.material);
		sFractalOut fractOut;
		Compute<fractal::calcModeCubeOrbitTrap>(*fractal, fractIn, &fractOut);
		pointFractalized = fractOut.z;
	}
	else
	{
		pointFractalized = input.point;
	}

	CVector2<double> texPoint = TextureMapping(pointFractalized, input.normal, objectData, mat,
																&textureVectorX, &textureVectorY)
															+ CVector2<double>(0.5, 0.5);

	if (!mat->textureFractalize)
	{
		// mipmapping - calculation of texture pixel size
		double delta = CalcDelta(input.point);
		double deltaTexX =
			(TextureMapping(pointFractalized + textureVectorX * delta, input.normal, objectData, mat)
				+ CVector2<double>(0.5, 0.5) - texPoint)
				.Length();
		double deltaTexY =
			(TextureMapping(pointFractalized + textureVectorY * delta, input.normal, objectData, mat)
				+ CVector2<double>(0.5, 0.5) - texPoint)
				.Length();

		if (deltaTexX > 0.5) deltaTexX = 1.0 - deltaTexX;
		if (deltaTexY > 0.5) deltaTexY = 1.0 - deltaTexY;

		deltaTexX = fabs(deltaTexX) / fabs(input.viewVector.Dot(input.normal));
		deltaTexY = fabs(deltaTexY) / fabs(input.viewVector.Dot(input.normal));

		texturePixelSize = 1.0 / max(deltaTexX, deltaTexY);
	}

	sRGBFloat tex;
	switch (texSelect)
	{
		case texture::texColor:
		{
			tex = input.material->colorTexture.Pixel(texPoint, texturePixelSize);
			break;
		}
		case texture::texDiffuse:
		{
			tex = input.material->diffusionTexture.Pixel(texPoint, texturePixelSize);
			break;
		}
		case texture::texLuminosity:
		{
			tex = input.material->luminosityTexture.Pixel(texPoint, texturePixelSize);
			break;
		}
		case texture::texDisplacement:
		{
			tex = input.material->displacementTexture.Pixel(texPoint, texturePixelSize);
			break;
		}
		case texture::texReflectance:
		{
			tex = input.material->reflectanceTexture.Pixel(texPoint, texturePixelSize);
			break;
		}
		case texture::texTransparency:
		{
			tex = input.material->transparencyTexture.Pixel(texPoint, texturePixelSize);
			break;
		}
	}

	return sRGBFloat(tex.R, tex.G, tex.B);
}
