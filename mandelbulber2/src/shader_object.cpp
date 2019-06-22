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
 * cRenderWorker::ObjectShader method - calculates surface
 */

#include "fractparams.hpp"
#include "material.h"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::ObjectShader(const sShaderInputData &_input, sRGBAfloat *surfaceColour,
	sRGBAfloat *specularOut, sRGBFloat *iridescenceOut, sGradientsCollection *gradients) const
{
	sRGBAfloat output;

	// normal vector
	CVector3 vn = _input.normal;
	sShaderInputData input = _input;
	cMaterial *mat = input.material;
	input.normal = vn;

	// main light
	sRGBAfloat mainLight;
	mainLight.R = params->mainLightIntensity * params->mainLightColour.R / 65536.0f;
	mainLight.G = params->mainLightIntensity * params->mainLightColour.G / 65536.0f;
	mainLight.B = params->mainLightIntensity * params->mainLightColour.B / 65536.0f;

	// calculate shading based on angle of incidence
	sRGBAfloat shade;
	if (params->mainLightEnable)
	{
		shade = MainShading(input);
		shade.R = params->mainLightIntensity * (1.0f - mat->shading + mat->shading * shade.R);
		shade.G = params->mainLightIntensity * (1.0f - mat->shading + mat->shading * shade.G);
		shade.B = params->mainLightIntensity * (1.0f - mat->shading + mat->shading * shade.B);
	}

	// calculate shadow
	sRGBAfloat shadow(1.0, 1.0, 1.0, 1.0);
	if (params->shadow && params->mainLightEnable) shadow = MainShadow(input);

	gradients->specular = sRGBFloat(1.0, 1.0, 1.0);

	// calculate surface colour
	sRGBAfloat colour = SurfaceColour(input, gradients);
	float texColInt = mat->colorTextureIntensity;
	float texColIntN = 1.0f - mat->colorTextureIntensity;
	colour.R *= input.texColor.R * texColInt + texColIntN;
	colour.G *= input.texColor.G * texColInt + texColIntN;
	colour.B *= input.texColor.B * texColInt + texColIntN;
	*surfaceColour = colour;

	// calculate specular highlight
	sRGBAfloat specular;
	if (params->mainLightEnable)
	{
		specular = SpecularHighlightCombined(input, input.lightVect, colour, gradients->diffuse);
		if (mat->useColorsFromPalette && mat->specularGradientEnable)
		{
			specular.R *= gradients->specular.R;
			specular.G *= gradients->specular.G;
			specular.B *= gradients->specular.B;
		}
	}

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
	ambient2.R = ambient.R * params->ambientOcclusion;
	ambient2.G = ambient.G * params->ambientOcclusion;
	ambient2.B = ambient.B * params->ambientOcclusion;

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
	auxLights = AuxLightsShader(input, colour, gradients, &auxLightsSpecular);

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
									 + mat->luminosity * mat->luminosityColor.R / 65536.0f;
		luminosity.G = input.texLuminosity.G * mat->luminosityTextureIntensity
									 + mat->luminosity * mat->luminosityColor.G / 65536.0f;
		luminosity.B = input.texLuminosity.B * mat->luminosityTextureIntensity
									 + mat->luminosity * mat->luminosityColor.B / 65536.0f;
	}

	sRGBFloat iridescence(1.0, 1.0, 1.0);
	if (input.material->iridescenceEnabled)
	{
		iridescence = IridescenceShader(input);
	}
	*iridescenceOut = iridescence;

	// total shader
	output.R = envMapping.R + (ambient2.R + mainLight.R * shade.R * shadow.R) * colour.R;
	output.G = envMapping.G + (ambient2.G + mainLight.G * shade.G * shadow.G) * colour.G;
	output.B = envMapping.B + (ambient2.B + mainLight.B * shade.B * shadow.B) * colour.B;

	output.R += (auxLights.R + fakeLights.R) * colour.R;
	output.G += (auxLights.G + fakeLights.G) * colour.G;
	output.B += (auxLights.B + fakeLights.B) * colour.B;

	output.R += luminosity.R;
	output.G += luminosity.G;
	output.B += luminosity.B;

	output.A = 1.0;

	specularOut->R =
		(auxLightsSpecular.R + fakeLightsSpecular.R + mainLight.R * specular.R * shadow.R)
		* iridescence.R;
	specularOut->G =
		(auxLightsSpecular.G + fakeLightsSpecular.G + mainLight.G * specular.G * shadow.G)
		* iridescence.G;
	specularOut->B =
		(auxLightsSpecular.B + fakeLightsSpecular.B + mainLight.B * specular.B * shadow.B)
		* iridescence.B;
	specularOut->A = output.A;

	return output;
}
