/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2019 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * cRenderWorker::RoughnessTexture method - gets roughness value form texture
 */
#include <algorithm>

#include "render_data.hpp"
#include "render_worker.hpp"
#include "texture_mapping.hpp"

using std::max;

float cRenderWorker::RoughnessTexture(const sShaderInputData &input) const
{
	cObjectData objectData = data->objectData[input.objectId];
	CVector3 texX, texY;
	double texturePixelSize;
	CVector2<double> texPoint =
		TextureMapping(input.point, input.normal, objectData, input.material, &texX, &texY)
		+ CVector2<double>(0.5, 0.5);

	// mipmapping - calculation of texture pixel size
	double delta = CalcDelta(input.point);
	double deltaTexX =
		(TextureMapping(input.point + texX * delta, input.normal, objectData, input.material)
			+ CVector2<double>(0.5, 0.5) - texPoint)
			.Length();
	double deltaTexY =
		(TextureMapping(input.point + texY * delta, input.normal, objectData, input.material)
			+ CVector2<double>(0.5, 0.5) - texPoint)
			.Length();
	deltaTexX = fabs(deltaTexX) / fabs(input.viewVector.Dot(input.normal));
	deltaTexY = fabs(deltaTexY) / fabs(input.viewVector.Dot(input.normal));
	texturePixelSize = 1.0 / max(deltaTexX, deltaTexY);

	sRGBFloat tex = input.material->roughnessTexture.Pixel(texPoint, texturePixelSize);
	return (tex.R + tex.G + tex.B) / 3.0f;
}
