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
 * calculation of texture
 */

#ifdef USE_TEXTURES
#if defined(USE_COLOR_TEXTURE) || defined(USE_DIFFUSION_TEXTURE)         \
	|| defined(USE_LUMINOSITY_TEXTURE) || defined(USE_REFLECTANCE_TEXTURE) \
	|| defined(USE_TRANSPARENCY_TEXTURE)
float3 TextureShader(__constant sClInConstants *consts, sClCalcParams *calcParams,
	sShaderInputDataCl *input, sRenderData *renderData, __global sObjectDataCl *objectData,
	int textureIndex, float3 substituteColor)
{
	float3 texOut = substituteColor;

	if (textureIndex >= 0)
	{
		float3 textureVectorX = 0.0f;
		float3 textureVectorY = 0.0f;

		float3 pointFractalized = input->point;

#ifdef FRACTALIZE_TEXTURE
		if (input->material->textureFractalize)
		{
			formulaOut outF;

			outF = Fractal(consts, input->point, calcParams, calcModeCubeOrbitTrap, input->material, -1);
			pointFractalized = outF.z.xyz;
		}
#endif

		float2 texturePoint = TextureMapping(pointFractalized, input->normal, objectData,
			input->material, &textureVectorX, &textureVectorY);

		texturePoint += (float2){0.5f, 0.5f};

		int2 textureSize = renderData->textureSizes[textureIndex];
		__global uchar4 *texture = renderData->textures[textureIndex];

		texOut =
			BicubicInterpolation(texturePoint.x, texturePoint.y, texture, textureSize.x, textureSize.y);
	}

	return texOut;
}

#endif
#endif
