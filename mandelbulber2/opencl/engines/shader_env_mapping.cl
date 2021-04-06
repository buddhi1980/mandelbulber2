/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * calculate environment mapping effecr
 */

#ifdef USE_ENV_MAPPING
float3 EnvMappingShader(
	__constant sClInConstants *consts, sRenderData *renderData, sShaderInputDataCl *input)
{
	float3 envReflect;

	float dotp = -dot(input->viewVector, input->normal);
	float3 reflect = input->normal * 2.0f * dotp + input->viewVector;

	float alphaTexture = -GetAlpha(reflect) + M_PI_F;
	float betaTexture = -GetBeta(reflect);
	int texWidth = renderData->textureSizes[0].x;
	int texHeight = renderData->textureSizes[0].y;

	if (betaTexture > 0.5f * M_PI_F) betaTexture = 0.5f * M_PI_F - betaTexture;

	if (betaTexture < -0.5f * M_PI_F) betaTexture = -0.5f * M_PI_F + betaTexture;

	float dtx = alphaTexture / (2.0f * M_PI_F) * texWidth + texWidth * 8.25f;
	float dty = (betaTexture / M_PI_F + 0.5f) * texHeight + texHeight * 8.0f;
	dtx = fmod(dtx, texWidth);
	dty = fmod(dty, texHeight);
	if (dtx < 0.0f) dtx = 0.0f;
	if (dty < 0.0f) dty = 0.0f;

	float reflectance = 1.0f;
	if (input->material->fresnelReflectance)
	{
		float n1 = 1.0f;
		float n2 = input->material->transparencyIndexOfRefraction;
		reflectance = Reflectance(input->normal, input->viewVector, n1, n2);
		if (reflectance < 0.0f) reflectance = 0.0f;
		if (reflectance > 1.0f) reflectance = 1.0f;
	}

	__global uchar4 *texture = renderData->textures[0];

	envReflect = BicubicInterpolation(dtx / texWidth, dty / texHeight, texture, texWidth, texHeight)
							 * reflectance;

	return envReflect;
}
#endif