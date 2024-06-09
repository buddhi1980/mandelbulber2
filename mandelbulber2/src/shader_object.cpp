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
 * cRenderWorker::ObjectShader method - calculates surface
 */

#include "fractparams.hpp"
#include "material.h"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::ObjectShader(const sShaderInputData &_input, sRGBAfloat *surfaceColour,
	sRGBAfloat *specularOut, sRGBFloat *iridescenceOut, sRGBAfloat *outShadow,
	sRGBFloat *luminosityEmissiveOut, sGradientsCollection *gradients) const
{
	sRGBAfloat output;
	float alpha = 1.0f;

	// normal vector
	CVector3 vn = _input.normal;
	sShaderInputData input = _input;
	cMaterial *mat = input.material;
	input.normal = vn;

	// fill light
	sRGBAfloat fillLight;
	fillLight.R = params->fillLightColor.R;
	fillLight.G = params->fillLightColor.G;
	fillLight.B = params->fillLightColor.B;

	if (params->allPrimitivesInvisibleAlpha)
	{
		if (input.objectId >= NUMBER_OF_FRACTALS)
		{
			//	alpha = 1.0f - (shadow.R + shadow.G + shadow.B) / 3.0f;
		}
	}

	gradients->specular = sRGBFloat(1.0, 1.0, 1.0);

	sRGBAfloat colour(1.0, 1.0, 1.0, 1.0);

	if (mat->perlinNoiseEnable && mat->perlinNoiseColorEnable)
	{
		float perlinColInt = mat->perlinNoiseColorIntensity;
		float perlinIntN = 1.0f - mat->perlinNoiseColorIntensity;
		float perlin = (mat->perlinNoiseColorInvert) ? 1.0f - input.perlinNoise : input.perlinNoise;

		if (input.material->surfaceGradientEnable)
		{
			float colorPosition =
				fmod(perlin * input.material->coloring_speed + input.material->paletteOffset, 1.0f);
			sRGBFloat gradientColor = input.material->gradientSurface.GetColorFloat(colorPosition, false);
			colour.R *= gradientColor.R * perlinColInt + perlinIntN;
			colour.G *= gradientColor.G * perlinColInt + perlinIntN;
			colour.B *= gradientColor.B * perlinColInt + perlinIntN;
		}
		else
		{
			float perlinCol = perlin * perlinColInt + perlinIntN;
			colour.R *= perlinCol;
			colour.G *= perlinCol;
			colour.B *= perlinCol;
		}
	}
	else
	{
		// calculate surface colour
		colour = SurfaceColour(input.point, input, gradients);
		float texColInt = mat->colorTextureIntensity;
		float texColIntN = 1.0f - mat->colorTextureIntensity;

		colour.R *= input.texColor.R * texColInt + texColIntN;
		colour.G *= input.texColor.G * texColInt + texColIntN;
		colour.B *= input.texColor.B * texColInt + texColIntN;
	}

	*surfaceColour = colour;

	// ambient occlusion
	sRGBAfloat ambient(0.0, 0.0, 0.0, 0.0);
	if (params->ambientOcclusionEnabled)
	{
		// fast mode
		if (params->ambientOcclusionMode == params::AOModeFast)
		{
			ambient = FastAmbientOcclusion(input);
		}
		else if (params->ambientOcclusionMode == params::AOModeMultipleRays)
		{
			ambient = AmbientOcclusion(input);
		}
	}
	sRGBAfloat ambient2;
	ambient2.R = ambient.R * params->ambientOcclusion * params->ambientOcclusionColor.R;
	ambient2.G = ambient.G * params->ambientOcclusion * params->ambientOcclusionColor.G;
	ambient2.B = ambient.B * params->ambientOcclusion * params->ambientOcclusionColor.B;

	// environment mapping
	sRGBAfloat envMapping(0.0, 0.0, 0.0, 0.0);
	if (params->envMappingEnable)
	{
		envMapping = EnvMapping(input);
	}
	envMapping.R *= mat->reflectance * input.texDiffuse.R;
	envMapping.G *= mat->reflectance * input.texDiffuse.G;
	envMapping.B *= mat->reflectance * input.texDiffuse.B;

	// additional lights
	sRGBAfloat auxLights;
	sRGBAfloat auxLightsSpecular;
	auxLights = AuxLightsShader(input, colour, gradients, &auxLightsSpecular, outShadow);

	// fake orbit trap lights
	sRGBAfloat fakeLights(0.0, 0.0, 0.0, 0.0);
	sRGBAfloat fakeLightsSpecular(0.0, 0.0, 0.0, 0.0);
	if (params->fakeLightsEnabled)
	{
		fakeLights = FakeLights(input, colour, &fakeLightsSpecular);
	}

	// luminosity
	sRGBAfloat luminosity;
	if (mat->useColorsFromPalette && mat->luminosityGradientEnable)
	{
		luminosity.R = input.texLuminosity.R * mat->luminosityTextureIntensity
									 + mat->luminosity * gradients->luminosity.R;
		luminosity.G = input.texLuminosity.G * mat->luminosityTextureIntensity
									 + mat->luminosity * gradients->luminosity.G;
		luminosity.B = input.texLuminosity.B * mat->luminosityTextureIntensity
									 + mat->luminosity * gradients->luminosity.B;
	}
	else
	{
		luminosity.R = input.texLuminosity.R * mat->luminosityTextureIntensity
									 + mat->luminosity * mat->luminosityColor.R;
		luminosity.G = input.texLuminosity.G * mat->luminosityTextureIntensity
									 + mat->luminosity * mat->luminosityColor.G;
		luminosity.B = input.texLuminosity.B * mat->luminosityTextureIntensity
									 + mat->luminosity * mat->luminosityColor.B;
	}

	if (mat->perlinNoiseEnable && mat->perlinNoiseLuminosityEnable)
	{
		float perlin =
			(mat->perlinNoiseLuminosityInvert) ? 1.0f - input.perlinNoise : input.perlinNoise;
		float perlinLumInt = mat->perlinNoiseLuminosityIntensity;

		if (input.material->luminosityGradientEnable)
		{

			float colorPosition =
				fmod(perlin * input.material->coloring_speed + input.material->paletteOffset, 1.0f);
			sRGBFloat gradientColor =
				input.material->gradientLuminosity.GetColorFloat(colorPosition, false);
			luminosity.R += gradientColor.R * perlinLumInt;
			luminosity.G += gradientColor.G * perlinLumInt;
			luminosity.B += gradientColor.B * perlinLumInt;
		}
		else
		{
			float perlinCol = perlin * perlinLumInt;
			luminosity.R *= perlinCol;
			luminosity.G *= perlinCol;
			luminosity.B *= perlinCol;
		}
	}

	luminosityEmissiveOut->R = luminosity.R * mat->luminosityEmissive;
	luminosityEmissiveOut->G = luminosity.G * mat->luminosityEmissive;
	luminosityEmissiveOut->B = luminosity.B * mat->luminosityEmissive;

	sRGBFloat iridescence(1.0, 1.0, 1.0);
	if (input.material->iridescenceEnabled)
	{
		iridescence = IridescenceShader(input);
	}
	*iridescenceOut = iridescence;

	// total shader
	output.R = envMapping.R + (fillLight.R + ambient2.R) * colour.R;
	output.G = envMapping.G + (fillLight.G + ambient2.G) * colour.G;
	output.B = envMapping.B + (fillLight.B + ambient2.B) * colour.B;

	output.R += (auxLights.R + fakeLights.R) * colour.R;
	output.G += (auxLights.G + fakeLights.G) * colour.G;
	output.B += (auxLights.B + fakeLights.B) * colour.B;

	output.R += luminosity.R;
	output.G += luminosity.G;
	output.B += luminosity.B;

	output.A = alpha;

	specularOut->R = (auxLightsSpecular.R + fakeLightsSpecular.R) * iridescence.R;
	specularOut->G = (auxLightsSpecular.G + fakeLightsSpecular.G) * iridescence.G;
	specularOut->B = (auxLightsSpecular.B + fakeLightsSpecular.B) * iridescence.B;
	specularOut->A = output.A;

	return output;
}
