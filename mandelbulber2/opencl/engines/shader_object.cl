/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
	float3 *iridescenceOut, float3 *outShadow, float3 *outLuminosityEmissive, float *alphaOut,
	sClGradientsCollection *gradients)
{
	float3 color = 0.7f;

	float3 shade = 0.0f;
	float3 specular = 0.0f;
	float3 shadow = 1.0f;
	*alphaOut = 1.0f;

	float3 fillLight = consts->params.fillLightColor;

	float3 surfaceColor = 1.0f;

#if defined(USE_PERLIN_NOISE) && defined(USE_PERLIN_NOISE_COLOR)
	if (input->material->perlinNoiseEnable && input->material->perlinNoiseColorEnable)
	{
		float perlinColInt = input->material->perlinNoiseColorIntensity;
		float perlinIntN = 1.0f - input->material->perlinNoiseColorIntensity;
		float perlin =
			(input->material->perlinNoiseColorInvert) ? 1.0f - input->perlinNoise : input->perlinNoise;

#ifdef USE_SURFACE_GRADIENT
		if (input->material->surfaceGradientEnable)
		{
			float colorPosition =
				fmod(perlin * input->material->coloring_speed + input->material->paletteOffset, 1.0f);

			float3 gradientColor = GetColorFromGradient(colorPosition, false, input->paletteSurfaceLength,
				input->palette + input->paletteSurfaceOffset);

			surfaceColor *= gradientColor * perlinColInt + perlinIntN;
		}
		else
#endif // USE_SURFACE_GRADIENT
		{
			float perlinCol = perlin * perlinColInt + perlinIntN;
			surfaceColor *= perlinCol;
		}
	}
	else
#endif // USE_PERLIN_NOISE
	{
		surfaceColor = SurfaceColor(consts, renderData, input, calcParam, gradients);

#ifdef USE_TEXTURES
#ifdef USE_COLOR_TEXTURE
		float texColInt = input->material->colorTextureIntensity;
		float texColIntN = 1.0f - texColInt;
		surfaceColor *= input->texColor * texColInt + texColIntN;
#endif
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
		AO *= consts->params.ambientOcclusion * consts->params.ambientOcclusionColor;
	}

	float3 envMapping = 0.0f;
#ifdef USE_ENV_MAPPING
	envMapping = EnvMappingShader(consts, renderData, input);
	envMapping *= input->material->reflectance;
#ifdef USE_TEXTURES
#ifdef USE_DIFFUSION_TEXTURE
	envMapping *= input->texDiffuse;
#endif
#endif

#endif

	float3 auxLights = 0.0f;
	float3 auxSpecular = 0.0f;

#ifdef AUX_LIGHTS
	auxLights = AuxLightsShader(
		consts, renderData, input, calcParam, surfaceColor, gradients, &auxSpecular, outShadow);
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

	float3 totalSpecular = (fakeLightsSpecular + auxSpecular) * iridescence;

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

#if defined(USE_PERLIN_NOISE) && defined(USE_PERLIN_NOISE_LUMINOSITY)
	if (input->material->perlinNoiseEnable && input->material->perlinNoiseLuminosityEnable)
	{
		float perlinLumInt = input->material->perlinNoiseLuminosityIntensity;
		float perlin = (input->material->perlinNoiseLuminosityInvert) ? 1.0f - input->perlinNoise
																																	: input->perlinNoise;
#ifdef USE_LUMINOSITY_GRADIENT
		if (input->material->luminosityGradientEnable)
		{
			float colorPosition =
				fmod(perlin * input->material->coloring_speed + input->material->paletteOffset, 1.0f);

			float3 gradientColor = GetColorFromGradient(colorPosition, false,
				input->paletteLuminosityLength, input->palette + input->paletteLuminosityOffset);

			luminosity += gradientColor * perlinLumInt;
		}
		else
#endif // USE_LUMINOSITY_GRADIENT
		{
			float perlinCol = perlin * perlinLumInt;
			luminosity *= perlinCol;
		}
	}
#endif // USE_PERLIN_NOISE

#ifdef USE_TEXTURES
#ifdef USE_LUMINOSITY_TEXTURE
	luminosity += input->texLuminosity * input->material->luminosityTextureIntensity;
#endif
#endif

	*outLuminosityEmissive = luminosity * input->material->luminosityEmissive;

	color = surfaceColor * (fillLight + auxLights + fakeLights + AO) + envMapping + totalSpecular
					+ luminosity;
	*outSpecular = totalSpecular;

	*outSurfaceColor = surfaceColor;
	return color;
}
