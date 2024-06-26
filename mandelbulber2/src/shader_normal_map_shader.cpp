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
 * cRenderWorker::NormalMapShader method - modifies normal vector based on normal map texture
 */
#include <algorithm>

#include "render_data.hpp"
#include "render_worker.hpp"
#include "texture_mapping.hpp"

using std::max;

CVector3 cRenderWorker::NormalMapShader(const sShaderInputData &input) const
{
	cObjectData objectData = data->objectData[input.objectId];
	CVector3 texX, texY;
	double texturePixelSize;
	CVector2<float> texPoint =
		TextureMapping(input.point, input.normal, objectData, input.material, &texX, &texY)
		+ CVector2<float>(0.5, 0.5);

	// mipmapping - calculation of texture pixel size
	float delta = CalcDelta(input.point);
	float deltaTexX =
		(TextureMapping(input.point + texX * delta, input.normal, objectData, input.material)
			+ CVector2<float>(0.5, 0.5) - texPoint)
			.Length();
	float deltaTexY =
		(TextureMapping(input.point + texY * delta, input.normal, objectData, input.material)
			+ CVector2<float>(0.5, 0.5) - texPoint)
			.Length();
	deltaTexX = fabs(deltaTexX) / fabs(input.viewVector.Dot(input.normal));
	deltaTexY = fabs(deltaTexY) / fabs(input.viewVector.Dot(input.normal));
	texturePixelSize = 1.0 / max(deltaTexX, deltaTexY);

	CVector3 n = input.normal;
	// tangent vectors:
	CVector3 t = n.Cross(texX);
	if (t.Length() < 1e-30) return input.normal;
	t.Normalize();
	CVector3 b = n.Cross(texY);
	if (b.Length() < 1e-30) return input.normal;
	b.Normalize();
	CMatrix33 tbn(b, t, n);

	CVector3 tex;

	if (input.material->normalMapTextureFromBumpmap)
	{
		tex = input.material->normalMapTexture.NormalMapFromBumpMap(
			texPoint, input.material->normalMapTextureHeight, texturePixelSize);
	}
	else
	{
		tex =
			input.material->normalMapTexture.NormalMap(texPoint, input.material->normalMapTextureHeight,
				input.material->normalMapTextureInvertGreen, texturePixelSize);
	}

	CVector3 result = tbn * tex;
	result.Normalize();
	return result;
}
