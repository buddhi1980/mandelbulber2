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
 * calculation of surface shader
 */

float3 ObjectShader(__constant sClInConstants *consts, sRenderData *renderData,
	sShaderInputDataCl *input, sClCalcParams *calcParam, float3 *outSurfaceColor, float3 *outSpecular,
	float3 *iridescenceOut, sClGradientsCollection *gradients)
{
	float3 color = 0.7f;
	float3 mainLight = consts->params.mainLightColour * consts->params.mainLightIntensity;

	float3 shade = 0.0f;
	float3 specular = 0.0f;
	float3 shadow = 1.0f;

	if (consts->params.mainLightEnable)
	{
		shade = MainShading(input);
		shade = consts->params.mainLightIntensity
						* ((float3)1.0f - input->material->shading + input->material->shading * shade);

#ifdef SHADOWS
		if (consts->params.shadow)
		{
			shadow = MainShadow(consts, renderData, input, calcParam);
		}
#endif
	}

	float3 surfaceColor = SurfaceColor(consts, renderData, input, calcParam, gradients);

#ifdef USE_TEXTURES
#ifdef USE_COLOR_TEXTURE
	float texColInt = input->material->colorTextureIntensity;
	float texColIntN = 1.0f - texColInt;
	surfaceColor *= input->texColor * texColInt + texColIntN;
#endif
#endif

	if (consts->params.mainLightEnable)
	{
		specular =
			SpecularHighlightCombined(input, calcParam, input->lightVect, surfaceColor, gradients);

#ifdef USE_SPECULAR_GRADIENT
		if (input->material->useColorsFromPalette && input->material->specularGradientEnable)
		{
			specular *= gradients->specular;
		}
#endif
	}

	float3 AO = 0.0f;
	if (consts->params.ambientOcclusionEnabled)
	{
#ifdef AO_MODE_FAST
		AO = FastAmbientOcclusion(consts, renderData, input, calcParam);
#endif
#ifdef AO_MODE_MULTIPLE_RAYS
		AO = AmbientOcclusion(consts, renderData, input, calcParam);
#endif
		AO *= consts->params.ambientOcclusion;
	}

	float3 auxLights = 0.0f;
	float3 auxSpecular = 0.0f;

#ifdef AUX_LIGHTS
	auxLights =
		AuxLightsShader(consts, renderData, input, calcParam, surfaceColor, gradients, &auxSpecular);
#endif

	float3 fakeLights = 0.0f;
	float3 fakeLightsSpecular = 0.0f;
#ifdef FAKE_LIGHTS
	fakeLights =
		FakeLightsShader(consts, input, calcParam, surfaceColor, gradients, &fakeLightsSpecular);
#endif

	float3 iridescence = 1.0f;
#ifdef USE_IRIDESCENCE
	if (input->material->iridescenceEnabled)
	{
		iridescence = IridescenceShader(consts, renderData, input, calcParam);
	}
#endif
	*iridescenceOut = iridescence;

	float3 totalSpecular =
		(mainLight * shadow * specular + fakeLightsSpecular + auxSpecular) * iridescence;

	float3 luminosity;
#ifdef USE_LUMINOSITY_GRADIENT
	if (input->material->useColorsFromPalette && input->material->luminosityGradientEnable)
	{
		luminosity = input->material->luminosity * gradients->luminosity;
	}
	else
#endif
	{
		luminosity = input->material->luminosity * input->material->luminosityColor;
	}

#ifdef USE_TEXTURES
#ifdef USE_LUMINOSITY_TEXTURE
	luminosity += input->texLuminosity * input->material->luminosityTextureIntensity;
#endif
#endif

	color = surfaceColor * (mainLight * shadow * shade + auxLights + fakeLights + AO) + totalSpecular
					+ luminosity;
	*outSpecular = totalSpecular;

	*outSurfaceColor = surfaceColor;
	return color;
}
