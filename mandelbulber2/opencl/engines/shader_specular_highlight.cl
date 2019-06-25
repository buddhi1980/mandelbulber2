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
 * calculation of specular highlights
 */

float3 SpecularHighlight(sShaderInputDataCl *input, sClCalcParams *calcParam, float3 lightVector,
	float specularWidth, float roughness, sClGradientsCollection *gradients)
{
	float3 halfVector = lightVector - input->viewVector;
	halfVector = fast_normalize(halfVector);
	float specular = dot(input->normal, halfVector);
	if (specular < 0.0f) specular = 0.0f;

	float diffuse = 1.0f;

#ifdef USE_ROUGHNESS_GRADIENT
	if (input->material->useColorsFromPalette && input->material->roughnessGradientEnable)
	{
		roughness *= gradients->roughness.s0;
	}
#endif

#if defined(USE_TEXTURES) && defined(USE_DIFFUSION_TEXTURE)
	if (input->material->useDiffusionTexture)
	{
		diffuse *=
			10.0f
			* (1.1f
					- input->material->diffusionTextureIntensity
							* (input->texDiffuse.s0 + input->texDiffuse.s1 + input->texDiffuse.s2) / 3.0f);
	}
#endif

#ifdef USE_DIFFUSE_GRADIENT
	if (input->material->useColorsFromPalette && input->material->diffuseGradientEnable)
	{
		diffuse *=
			10.0f
			* (1.1f - (gradients->diffuse.s0 + gradients->diffuse.s1 + gradients->diffuse.s2) / 3.0f);
	}
#endif

	specular = pow(specular, 30.0f / specularWidth / diffuse) / diffuse;

	if (roughness > 0.0f)
	{
		specular *= (1.0f + Random(1000, &calcParam->randomSeed) / 1000.0f * roughness);
	}
	if (specular > 15.0f) specular = 15.0f;
	float3 out = specular;
	out *= input->material->specularColor;
	return out;
}
