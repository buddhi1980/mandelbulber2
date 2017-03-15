/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cRenderWorker::ObjectShader method - calculates surface and volumetric shaders
 */

#include <algorithm>
#include "ao_modes.h"
#include "calculate_distance.hpp"
#include "common_math.h"
#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"
#include "texture_mapping.hpp"

using std::max;

sRGBAfloat cRenderWorker::ObjectShader(
	const sShaderInputData &_input, sRGBAfloat *surfaceColour, sRGBAfloat *specularOut) const
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

	// calculate specular highlight
	sRGBAfloat specular;
	if (params->mainLightEnable)
	{
		specular = MainSpecular(input);
		specular.R *= mat->specular;
		specular.G *= mat->specular;
		specular.B *= mat->specular;
	}

	// calculate surface colour
	sRGBAfloat colour = SurfaceColour(input);
	float texColInt = mat->colorTextureIntensity;
	float texColIntN = 1.0f - mat->colorTextureIntensity;
	colour.R *= input.texColor.R * texColInt + texColIntN;
	colour.G *= input.texColor.G * texColInt + texColIntN;
	colour.B *= input.texColor.B * texColInt + texColIntN;
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
	auxLights = AuxLightsShader(input, &auxLightsSpecular);

	// fake orbit trap lights
	sRGBAfloat fakeLights(0.0, 0.0, 0.0, 0.0);
	sRGBAfloat fakeLightsSpecular(0.0, 0.0, 0.0, 0.0);
	if (params->fakeLightsEnabled)
	{
		fakeLights = FakeLights(input, &fakeLightsSpecular);
	}

	// luminosity
	sRGBAfloat luminosity;
	luminosity.R = input.texLuminosity.R * mat->luminosityTextureIntensity
								 + mat->luminosity * mat->luminosityColor.R / 65536.0f;
	luminosity.G = input.texLuminosity.G * mat->luminosityTextureIntensity
								 + mat->luminosity * mat->luminosityColor.G / 65536.0f;
	luminosity.B = input.texLuminosity.B * mat->luminosityTextureIntensity
								 + mat->luminosity * mat->luminosityColor.B / 65536.0f;

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

	specularOut->R = auxLightsSpecular.R + fakeLightsSpecular.R + mainLight.R * specular.R * shadow.R;
	specularOut->G = auxLightsSpecular.G + fakeLightsSpecular.G + mainLight.G * specular.G * shadow.G;
	specularOut->B = auxLightsSpecular.B + fakeLightsSpecular.B + mainLight.B * specular.B * shadow.B;
	specularOut->A = output.A;

	return output;
}

sRGBAfloat cRenderWorker::BackgroundShader(const sShaderInputData &input) const
{
	sRGBAfloat pixel2;

	if (params->texturedBackground)
	{
		switch (params->texturedBackgroundMapType)
		{
			case params::mapDoubleHemisphere:
			{
				CVector3 rotatedViewVector = params->mRotBackgroundRotation.RotateVector(input.viewVector);
				double alphaTexture = rotatedViewVector.GetAlpha();
				double betaTexture = rotatedViewVector.GetBeta();
				int texWidth = data->textures.backgroundTexture.Width() / 2;
				int texHeight = data->textures.backgroundTexture.Height();
				int offset = 0;

				if (betaTexture < 0)
				{
					betaTexture = -betaTexture;
					alphaTexture = M_PI - alphaTexture;
					offset = texWidth;
				}
				double texX = 0.5 * texWidth
											+ cos(alphaTexture) * (1.0 - betaTexture / (0.5 * M_PI)) * texWidth * 0.5
											+ offset;
				double texY = 0.5 * texHeight
											+ sin(alphaTexture) * (1.0 - betaTexture / (0.5 * M_PI)) * texHeight * 0.5;
				sRGBFloat pixel = data->textures.backgroundTexture.Pixel(texX, texY);
				pixel2.R = pixel.R;
				pixel2.G = pixel.G;
				pixel2.B = pixel.B;
				break;
			}
			case params::mapEquirectangular:
			{
				CVector3 rotatedViewVector = params->mRotBackgroundRotation.RotateVector(input.viewVector);
				double alphaTexture = fmod(-rotatedViewVector.GetAlpha() + 3.5 * M_PI, 2 * M_PI);
				double betaTexture = -rotatedViewVector.GetBeta();
				if (betaTexture > 0.5 * M_PI) betaTexture = 0.5 * M_PI - betaTexture;
				if (betaTexture < -0.5 * M_PI) betaTexture = -0.5 * M_PI + betaTexture;

				CVector2<double> tex(
					alphaTexture / (2.0 * M_PI) / params->backgroundHScale + params->backgroundTextureOffsetX,
					(betaTexture / M_PI + 0.5) / params->backgroundVScale + params->backgroundTextureOffsetY);

				sRGBFloat pixel = data->textures.backgroundTexture.Pixel(tex);
				pixel2.R = pixel.R;
				pixel2.G = pixel.G;
				pixel2.B = pixel.B;
				break;
			}
			case params::mapFlat:
			{
				CVector3 vect = mRotInv.RotateVector(input.viewVector);
				vect = params->mRotBackgroundRotation.RotateVector(vect);
				double texX, texY;
				if (fabs(vect.y) > 1e-20)
				{
					texX = vect.x / vect.y / params->fov * params->imageHeight / params->imageWidth;
					texY = -vect.z / vect.y / params->fov;
				}
				else
				{
					texX = vect.x > 0.0 ? 1.0 : -1.0;
					texY = vect.z > 0.0 ? -1.0 : 1.0;
				}

				texX = (texX / params->backgroundHScale) + 0.5 + params->backgroundTextureOffsetX;
				texY = (texY / params->backgroundVScale) + 0.5 + params->backgroundTextureOffsetY;

				sRGBFloat pixel = data->textures.backgroundTexture.Pixel(CVector2<double>(texX, texY));
				pixel2.R = pixel.R;
				pixel2.G = pixel.G;
				pixel2.B = pixel.B;
				break;
			}
		}
		pixel2.R *= params->background_brightness;
		pixel2.G *= params->background_brightness;
		pixel2.B *= params->background_brightness;
	}
	else
	{
		CVector3 vector(0.0, 0.0, 1.0);
		vector.Normalize();
		CVector3 viewVectorNorm = input.viewVector;
		viewVectorNorm.Normalize();
		double grad = viewVectorNorm.Dot(vector) + 1.0;
		sRGB16 pixel;
		if (grad < 1)
		{
			double gradN = 1.0 - grad;
			pixel.R = quint16(params->background_color3.R * gradN + params->background_color2.R * grad);
			pixel.G = quint16(params->background_color3.G * gradN + params->background_color2.G * grad);
			pixel.B = quint16(params->background_color3.B * gradN + params->background_color2.B * grad);
		}
		else
		{
			grad = grad - 1;
			double gradN = 1.0 - grad;
			pixel.R = quint16(params->background_color2.R * gradN + params->background_color1.R * grad);
			pixel.G = quint16(params->background_color2.G * gradN + params->background_color1.G * grad);
			pixel.B = quint16(params->background_color2.B * gradN + params->background_color1.B * grad);
		}

		pixel2.R = pixel.R / 65536.0f;
		pixel2.G = pixel.G / 65536.0f;
		pixel2.B = pixel.B / 65536.0f;
		pixel2.A = 0.0;
	}

	CVector3 viewVectorNorm = input.viewVector;
	viewVectorNorm.Normalize();
	double light =
		(viewVectorNorm.Dot(input.lightVect) - 1.0) * 360.0 / params->mainLightVisibilitySize;
	light = 1.0 / (1.0 + pow(light, 6.0)) * params->mainLightVisibility * params->mainLightIntensity;
	pixel2.R += light * params->mainLightColour.R / 65536.0;
	pixel2.G += light * params->mainLightColour.G / 65536.0;
	pixel2.B += light * params->mainLightColour.B / 65536.0;

	return pixel2;
}

sRGBAfloat cRenderWorker::VolumetricShader(
	const sShaderInputData &input, sRGBAfloat oldPixel, sRGBAfloat *opacityOut) const
{
	sRGBAfloat output;
	float totalOpacity = 0.0;

	output.R = oldPixel.R;
	output.G = oldPixel.G;
	output.B = oldPixel.B;
	output.A = oldPixel.A;

	// volumetric fog init
	double colourThresh = params->volFogColour1Distance;
	double colourThresh2 = params->volFogColour2Distance;
	double fogReduce = params->volFogDistanceFactor;
	float fogIntensity = params->volFogDensity;

	// visible lights init
	int numberOfLights = data->lights.GetNumberOfLights();
	if (numberOfLights < 4) numberOfLights = 4;

	// glow init
	float glow = input.stepCount * params->glowIntensity / 512.0f * float(params->DEFactor);
	float glowN = 1.0f - glow;
	if (glowN < 0.0f) glowN = 0.0f;
	float glowR = (params->glowColor1.R * glowN + params->glowColor2.R * glow) / 65536.0f;
	float glowG = (params->glowColor1.G * glowN + params->glowColor2.G * glow) / 65536.0f;
	float glowB = (params->glowColor1.B * glowN + params->glowColor2.B * glow) / 65536.0f;

	double totalStep = 0.0;

	// qDebug() << "Start volumetric shader &&&&&&&&&&&&&&&&&&&&";

	sShaderInputData input2 = input;
	for (int index = input.stepCount - 1; index > 0; index--)
	{
		double step = input.stepBuff[index].step;
		double distance = input.stepBuff[index].distance;
		CVector3 point = input.stepBuff[index].point;
		totalStep += step;

		input2.point = point;
		input2.distThresh = input.stepBuff[index].distThresh;

		// qDebug() << "i" << index << "dist" << distance << "iters" << input.stepBuff[index].iters <<
		// "distThresh" << input2.distThresh << "step" << step << "point" << point.Debug();

		if (totalStep < CalcDelta(point))
		{
			continue;
		}
		step = totalStep;
		totalStep = 0.0;

		//------------------- glow
		if (params->glowEnabled && input.stepCount > 0)
		{
			float glowOpacity = glow / input.stepCount;
			if (glowOpacity > 1.0f) glowOpacity = 1.0f;
			output.R = glowOpacity * glowR + (1.0f - glowOpacity) * output.R;
			output.G = glowOpacity * glowG + (1.0f - glowOpacity) * output.G;
			output.B = glowOpacity * glowB + (1.0f - glowOpacity) * output.B;
			output.A += glowOpacity;
		}
		// qDebug() << "step" << step;
		//------------------ visible light
		if (data->lights.IsAnyLightEnabled() && params->auxLightVisibility > 0)
		{
			double miniStep;
			double lastMiniSteps = -1.0;

			for (double miniSteps = 0.0; miniSteps < step; miniSteps += miniStep)
			{
				double lowestLightSize = 1e10;
				double lowestLightDist = 1e10;
				for (int i = 0; i < numberOfLights; ++i)
				{
					const sLight *light = data->lights.GetLight(i);
					if (light->enabled)
					{
						CVector3 lightDistVect = point - input.viewVector * miniSteps - light->position;
						double lightDist = lightDistVect.Length();
						double lightSize = sqrt(light->intensity) * params->auxLightVisibilitySize;
						double distToLightSurface = lightDist - lightSize;
						if (distToLightSurface < 0.0) distToLightSurface = 0.0;
						if (distToLightSurface <= lowestLightDist)
						{
							if (lightSize < lowestLightSize)
							{
								lowestLightSize = lightSize;
							}
							lowestLightDist = distToLightSurface;
						}
					}
				}

				miniStep = 0.1 * (lowestLightDist + 0.1 * lowestLightSize);
				if (miniStep > step - miniSteps) miniStep = step - miniSteps;
				if (miniStep < step * 0.001) miniStep = step * 0.001;
				// qDebug() << "lowDist:" << lowestLightDist << "lowSize" << lowestLightSize << "miniStep"
				// << miniStep;

				for (int i = 0; i < numberOfLights; ++i)
				{
					const sLight *light = data->lights.GetLight(i);
					if (light->enabled && light->intensity > 0)
					{
						CVector3 lightDistVect = point - input.viewVector * miniSteps - light->position;
						double lightDist = lightDistVect.Length();
						double lightSize = sqrt(light->intensity) * params->auxLightVisibilitySize;
						double r2 = lightDist / lightSize;
						double bellFunction = 1.0 / (1.0 + pow(r2, 4.0));
						double lightDensity = miniStep * bellFunction * params->auxLightVisibility / lightSize;

						output.R += lightDensity * light->colour.R / 65536.0;
						output.G += lightDensity * light->colour.G / 65536.0;
						output.B += lightDensity * light->colour.B / 65536.0;
						output.A += lightDensity;
					}
				}
				if (miniSteps == lastMiniSteps)
				{
					// qWarning() << "Dead computation\n"
					//		<< "\pointN:" << (point - input.viewVector * miniSteps).Debug();
					break;
				}
				lastMiniSteps = miniSteps;
			}
		}

		// fake lights (orbit trap)
		if (params->fakeLightsEnabled)
		{
			sFractalIn fractIn(point, params->minN, params->N, params->common, -1);
			sFractalOut fractOut;
			Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
			double r = fractOut.orbitTrapR;
			r = sqrt(1.0 / (r + 1.0e-30));
			double fakeLight = 1.0 / (pow(r, 10.0 / params->fakeLightsVisibilitySize)
																	 * pow(10.0, 10.0 / params->fakeLightsVisibilitySize)
																 + 1e-100);
			output.R += fakeLight * step * params->fakeLightsVisibility;
			output.G += fakeLight * step * params->fakeLightsVisibility;
			output.B += fakeLight * step * params->fakeLightsVisibility;
			output.A += fakeLight * step * params->fakeLightsVisibility;
		}

		//---------------------- volumetric lights with shadows in fog

		for (int i = 0; i < 5; i++)
		{
			if (i == 0 && params->volumetricLightEnabled[0])
			{
				sRGBAfloat shadowOutputTemp = MainShadow(input2);
				output.R += shadowOutputTemp.R * step * params->volumetricLightIntensity[0]
										* params->mainLightColour.R / 65536.0;
				output.G += shadowOutputTemp.G * step * params->volumetricLightIntensity[0]
										* params->mainLightColour.G / 65536.0;
				output.B += shadowOutputTemp.B * step * params->volumetricLightIntensity[0]
										* params->mainLightColour.B / 65536.0;
				output.A += (shadowOutputTemp.R + shadowOutputTemp.G + shadowOutputTemp.B) / 3.0 * step
										* params->volumetricLightIntensity[0];
			}
			if (data->lights.IsAnyLightEnabled() && i > 0)
			{
				const sLight *light = data->lights.GetLight(i - 1);
				if (light->enabled && params->volumetricLightEnabled[i])
				{
					CVector3 lightVectorTemp = light->position - point;
					double distanceLight = lightVectorTemp.Length();
					double distanceLight2 = distanceLight * distanceLight;
					lightVectorTemp.Normalize();
					double lightShadow = AuxShadow(input2, distanceLight, lightVectorTemp);
					output.R += lightShadow * light->colour.R / 65536.0 * params->volumetricLightIntensity[i]
											* step / distanceLight2;
					output.G += lightShadow * light->colour.G / 65536.0 * params->volumetricLightIntensity[i]
											* step / distanceLight2;
					output.B += lightShadow * light->colour.B / 65536.0 * params->volumetricLightIntensity[i]
											* step / distanceLight2;
					output.A += lightShadow * params->volumetricLightIntensity[i] * step / distanceLight2;
				}
			}
		}

		//----------------------- basic fog
		if (params->fogEnabled)
		{
			float fogDensity = step / params->fogVisibility;
			if (fogDensity > 1.0f) fogDensity = 1.0f;
			output.R = fogDensity * params->fogColor.R / 65536.0f + (1.0f - fogDensity) * output.R;
			output.G = fogDensity * params->fogColor.G / 65536.0f + (1.0f - fogDensity) * output.G;
			output.B = fogDensity * params->fogColor.B / 65536.0f + (1.0f - fogDensity) * output.B;
			totalOpacity = fogDensity + (1.0f - fogDensity) * totalOpacity;
			output.A = fogDensity + (1.0f - fogDensity) * output.A;
		}

		//-------------------- volumetric fog
		if (fogIntensity > 0.0f && params->volFogEnabled)
		{
			float densityTemp = step * fogReduce / (distance * distance + fogReduce * fogReduce);

			float k = distance / colourThresh;
			if (k > 1) k = 1.0f;
			float kn = 1.0f - k;
			float fogTempR = params->volFogColour1.R * kn + params->volFogColour2.R * k;
			float fogTempG = params->volFogColour1.G * kn + params->volFogColour2.G * k;
			float fogTempB = params->volFogColour1.B * kn + params->volFogColour2.B * k;

			float k2 = distance / colourThresh2 * k;
			if (k2 > 1) k2 = 1.0;
			kn = 1.0f - k2;
			fogTempR = fogTempR * kn + params->volFogColour3.R * k2;
			fogTempG = fogTempG * kn + params->volFogColour3.G * k2;
			fogTempB = fogTempB * kn + params->volFogColour3.B * k2;

			float fogDensity = 0.3f * fogIntensity * densityTemp / (1.0f + fogIntensity * densityTemp);
			if (fogDensity > 1) fogDensity = 1.0;

			output.R = fogDensity * fogTempR / 65536.0f + (1.0f - fogDensity) * output.R;
			output.G = fogDensity * fogTempG / 65536.0f + (1.0f - fogDensity) * output.G;
			output.B = fogDensity * fogTempB / 65536.0f + (1.0f - fogDensity) * output.B;
			// qDebug() << "densityTemp " << densityTemp << "k" << k << "k2" << k2 << "fogTempR" <<
			// fogTempR << "fogDensity" << fogDensity << "output.R" << output.R;

			totalOpacity = fogDensity + (1.0f - fogDensity) * totalOpacity;
			output.A = fogDensity + (1.0f - fogDensity) * output.A;
		}

		// iter fog
		if (params->iterFogEnabled)
		{
			int L = input.stepBuff[index].iters;
			float opacity =
				IterOpacity(step, L, params->N, params->iterFogOpacityTrim, params->iterFogOpacity);

			sRGBAfloat newColour(0.0, 0.0, 0.0, 0.0);
			if (opacity > 0)
			{
				// fog colour
				float iterFactor1 = (L - params->iterFogOpacityTrim)
														/ (params->iterFogColor1Maxiter - params->iterFogOpacityTrim);
				float k = iterFactor1;
				if (k > 1.0f) k = 1.0f;
				if (k < 0.0f) k = 0.0f;
				float kn = 1.0f - k;
				float fogColR = params->iterFogColour1.R * kn + params->iterFogColour2.R * k;
				float fogColG = params->iterFogColour1.G * kn + params->iterFogColour2.G * k;
				float fogColB = params->iterFogColour1.B * kn + params->iterFogColour2.B * k;

				float iterFactor2 = (L - params->iterFogColor1Maxiter)
														/ (params->iterFogColor2Maxiter - params->iterFogColor1Maxiter);
				float k2 = iterFactor2;
				if (k2 < 0.0f) k2 = 0.0;
				if (k2 > 1.0f) k2 = 1.0;
				kn = 1.0f - k2;
				fogColR = fogColR * kn + params->iterFogColour3.R * k2;
				fogColG = fogColG * kn + params->iterFogColour3.G * k2;
				fogColB = fogColB * kn + params->iterFogColour3.B * k2;
				//----

				for (int i = 0; i < 5; i++)
				{
					if (i == 0)
					{
						if (params->mainLightEnable && params->mainLightIntensity > 0.0f)
						{
							sRGBAfloat shadowOutputTemp = MainShadow(input2);
							newColour.R += shadowOutputTemp.R * params->mainLightColour.R / 65536.0f
														 * params->mainLightIntensity;
							newColour.G += shadowOutputTemp.G * params->mainLightColour.G / 65536.0f
														 * params->mainLightIntensity;
							newColour.B += shadowOutputTemp.B * params->mainLightColour.B / 65536.0f
														 * params->mainLightIntensity;
						}
					}

					if (i > 0)
					{
						const sLight *light = data->lights.GetLight(i - 1);
						if (light->enabled)
						{
							CVector3 lightVectorTemp = light->position - point;
							double distanceLight = lightVectorTemp.Length();
							double distanceLight2 = distanceLight * distanceLight;
							lightVectorTemp.Normalize();
							double lightShadow = AuxShadow(input2, distanceLight, lightVectorTemp);
							double intensity = light->intensity * 100.0;
							newColour.R += lightShadow * light->colour.R / 65536.0 / distanceLight2 * intensity;
							newColour.G += lightShadow * light->colour.G / 65536.0 / distanceLight2 * intensity;
							newColour.B += lightShadow * light->colour.B / 65536.0 / distanceLight2 * intensity;
						}
					}
				}

				if (params->ambientOcclusionEnabled
						&& params->ambientOcclusionMode == params::AOModeMultipleRays)
				{
					sRGBAfloat AO = AmbientOcclusion(input2);
					newColour.R += AO.R * params->ambientOcclusion;
					newColour.G += AO.G * params->ambientOcclusion;
					newColour.B += AO.B * params->ambientOcclusion;
				}

				if (opacity > 1.0f) opacity = 1.0f;

				output.R = output.R * (1.0f - opacity) + newColour.R * opacity * fogColR / 65536.0f;
				output.G = output.G * (1.0f - opacity) + newColour.G * opacity * fogColG / 65536.0f;
				output.B = output.B * (1.0f - opacity) + newColour.B * opacity * fogColB / 65536.0f;
				totalOpacity = opacity + (1.0f - opacity) * totalOpacity;
				output.A = opacity + (1.0f - opacity) * output.A;
			}
		}

		if (totalOpacity > 1.0f) totalOpacity = 1.0f;
		if (output.A > 1.0f) output.A = 1.0f;
		(*opacityOut).R = totalOpacity;
		(*opacityOut).G = totalOpacity;
		(*opacityOut).B = totalOpacity;

	} // next stepCount

	return output;
}

sRGBAfloat cRenderWorker::MainShadow(const sShaderInputData &input) const
{
	sRGBAfloat shadow(1.0, 1.0, 1.0, 1.0);

	// starting point
	CVector3 point2;

	double factor = input.delta / params->resolution;
	if (!params->penetratingLights) factor = params->viewDistanceMax;
	double dist;

	double DEFactor = params->DEFactor;
	if (params->iterFogEnabled || params->volumetricLightEnabled[0]) DEFactor = 1.0;

	// double start = input.delta;
	double start = input.distThresh;
	if (params->interiorMode) start = input.distThresh * DEFactor;

	double opacity;
	double shadowTemp = 1.0;

	double softRange = tan(params->shadowConeAngle / 180.0 * M_PI);
	double maxSoft = 0.0;

	const bool bSoft = !params->iterFogEnabled && !params->limitsEnabled
										 && !params->common.iterThreshMode && softRange > 0.0;

	for (double i = start; i < factor; i += dist * DEFactor)
	{
		point2 = input.point + input.lightVect * i;

		double dist_thresh;
		if (params->iterFogEnabled || params->volumetricLightEnabled[0])
		{
			dist_thresh = CalcDistThresh(point2);
		}
		else
			dist_thresh = input.distThresh;

		sDistanceOut distanceOut;
		sDistanceIn distanceIn(point2, dist_thresh, false);
		dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		if (bSoft)
		{
			double angle = (dist - dist_thresh) / i;
			if (angle < 0) angle = 0;
			if (dist < dist_thresh) angle = 0;
			double softShadow = 1.0 - angle / softRange;
			if (params->penetratingLights) softShadow *= (factor - i) / factor;
			if (softShadow < 0) softShadow = 0;
			if (softShadow > maxSoft) maxSoft = softShadow;
		}

		if (params->iterFogEnabled)
		{
			opacity = IterOpacity(dist * DEFactor, distanceOut.iters, params->N,
				params->iterFogOpacityTrim, params->iterFogOpacity);
		}
		else
		{
			opacity = 0.0;
		}
		shadowTemp -= opacity * (factor - i) / factor;

		if (dist < dist_thresh || shadowTemp < 0.0)
		{
			shadowTemp -= (factor - i) / factor;
			if (!params->penetratingLights) shadowTemp = 0.0;
			if (shadowTemp < 0.0) shadowTemp = 0.0;
			break;
		}
	}
	if (!bSoft)
	{
		shadow.R = shadowTemp;
		shadow.G = shadowTemp;
		shadow.B = shadowTemp;
	}
	else
	{
		shadow.R = 1.0 - maxSoft;
		shadow.G = 1.0 - maxSoft;
		shadow.B = 1.0 - maxSoft;
	}
	return shadow;
}

sRGBAfloat cRenderWorker::FastAmbientOcclusion(const sShaderInputData &input) const
{
	// reference Iñigo Quilez –iq/rgba:
	// http://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf
	double delta = input.distThresh;
	float aoTemp = 0;
	double quality = params->ambientOcclusionQuality;
	double lastDist = 1e20;
	for (int i = 1; i < quality * quality; i++)
	{
		double scan = i * i * delta;
		CVector3 pointTemp = input.point + input.normal * scan;

		sDistanceOut distanceOut;
		sDistanceIn distanceIn(pointTemp, input.distThresh, false);
		double dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);
		if (dist > lastDist * 2) dist = lastDist * 2.0;
		lastDist = dist;
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;
		aoTemp +=
			1.0 / pow(2.0, i) * (scan - params->ambientOcclusionFastTune * dist) / input.distThresh;
	}
	float ao = 1.0f - 0.2f * aoTemp;
	if (ao < 0) ao = 0;
	sRGBAfloat output(ao, ao, ao, 1.0f);
	return output;
}

sRGBAfloat cRenderWorker::AmbientOcclusion(const sShaderInputData &input) const
{
	sRGBAfloat AO(0, 0, 0, 1.0);

	double start_dist = input.delta;
	double end_dist = input.delta / params->resolution;
	double intense;

	for (int i = 0; i < AOVectorsCount; i++)
	{
		sVectorsAround v = AOVectorsAround[i];

		double dist;

		double opacity;
		double shadowTemp = 1.0;

		for (double r = start_dist; r < end_dist; r += dist * 2.0)
		{
			CVector3 point2 = input.point + v.v * r;

			sDistanceOut distanceOut;
			sDistanceIn distanceIn(point2, input.distThresh, false);
			dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);
			data->statistics.totalNumberOfIterations += distanceOut.totalIters;

			if (params->iterFogEnabled)
			{
				opacity = IterOpacity(dist * 2.0, distanceOut.iters, params->N, params->iterFogOpacityTrim,
					params->iterFogOpacity);
			}
			else
			{
				opacity = 0.0;
			}
			shadowTemp -= opacity * (end_dist - r) / end_dist;

			float dist_thresh;
			if (params->iterFogEnabled || params->volumetricLightEnabled[0])
			{
				dist_thresh = CalcDistThresh(point2);
			}
			else
				dist_thresh = input.distThresh;

			if (dist < dist_thresh || distanceOut.maxiter || shadowTemp < 0.0)
			{
				shadowTemp -= (end_dist - r) / end_dist;
				if (shadowTemp < 0.0) shadowTemp = 0.0;
				break;
			}
		}

		intense = shadowTemp;

		AO.R += intense * v.R;
		AO.G += intense * v.G;
		AO.B += intense * v.B;
	}
	AO.R /= AOVectorsCount * 65536.0;
	AO.G /= AOVectorsCount * 65536.0;
	AO.B /= AOVectorsCount * 65536.0;

	return AO;
}

CVector3 cRenderWorker::CalculateNormals(const sShaderInputData &input) const
{
	CVector3 normal(0.0, 0.0, 0.0);
	// calculating normal vector based on distance estimation (gradient of distance function)
	if (!params->slowShading)
	{
		double delta = input.delta * params->smoothness;
		if (params->interiorMode) delta = input.distThresh * 0.2 * params->smoothness;

		double sx1, sx2, sy1, sy2, sz1, sz2;
		sDistanceOut distanceOut;

		CVector3 deltaX(delta, 0.0, 0.0);
		sDistanceIn distanceIn1(input.point + deltaX, input.distThresh, true);
		sx1 = CalculateDistance(*params, *fractal, distanceIn1, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;
		sDistanceIn distanceIn2(input.point - deltaX, input.distThresh, true);
		sx2 = CalculateDistance(*params, *fractal, distanceIn2, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		CVector3 deltaY(0.0, delta, 0.0);
		sDistanceIn distanceIn3(input.point + deltaY, input.distThresh, true);
		sy1 = CalculateDistance(*params, *fractal, distanceIn3, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;
		sDistanceIn distanceIn4(input.point - deltaY, input.distThresh, true);
		sy2 = CalculateDistance(*params, *fractal, distanceIn4, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		CVector3 deltaZ(0.0, 0.0, delta);
		sDistanceIn distanceIn5(input.point + deltaZ, input.distThresh, true);
		sz1 = CalculateDistance(*params, *fractal, distanceIn5, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;
		sDistanceIn distanceIn6(input.point - deltaZ, input.distThresh, true);
		sz2 = CalculateDistance(*params, *fractal, distanceIn6, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		normal.x = sx1 - sx2;
		normal.y = sy1 - sy2;
		normal.z = sz1 - sz2;
	}

	// calculating normal vector based on average value of binary central difference
	else
	{
		CVector3 point2;
		CVector3 point3;
		double delta = input.delta * params->smoothness * 0.5;
		if (params->interiorMode) delta = input.distThresh * 0.2 * params->smoothness;

		sDistanceOut distanceOut;
		for (point2.x = -1.0; point2.x <= 1.0; point2.x += 0.2) //+0.2
		{
			for (point2.y = -1.0; point2.y <= 1.0; point2.y += 0.2)
			{
				for (point2.z = -1.0; point2.z <= 1.0; point2.z += 0.2)
				{
					point3 = input.point + point2 * delta;

					sDistanceIn distanceIn(point3, input.distThresh, true);
					double dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);
					data->statistics.totalNumberOfIterations += distanceOut.totalIters;
					normal += point2 * dist;
				}
			}
		}
	}

	if (qFuzzyIsNull(normal.x) && qFuzzyIsNull(normal.y) && qFuzzyIsNull(normal.z))
	{
		normal = CVector3(1.0, 0.0, 0.0);
	}
	else
	{
		normal.Normalize();
	}

	if (normal.IsNotANumber())
	{
		normal = CVector3(1.0, 0.0, 0.0);
	}

	if (input.invertMode) normal *= -1.0;

	// qDebug() << input.point.Debug() << normal.Debug();

	return normal;
}

sRGBAfloat cRenderWorker::MainShading(const sShaderInputData &input)
{
	sRGBAfloat shading;
	double shade = input.normal.Dot(input.lightVect);
	if (shade < 0) shade = 0;
	shading.R = shade;
	shading.G = shade;
	shading.B = shade;
	return shading;
}

sRGBAfloat cRenderWorker::MainSpecular(const sShaderInputData &input) const
{
	sRGBAfloat specular;
	CVector3 half = input.lightVect - input.viewVector;
	half.Normalize();
	float shade2 = input.normal.Dot(half);
	if (shade2 < 0.0f) shade2 = 0.0f;
	float diffuse =
		10.0f * (1.1f
							- input.material->diffusionTextureIntensity
									* (input.texDiffuse.R + input.texDiffuse.G + input.texDiffuse.B) / 3.0f);
	shade2 = pow(shade2, 30.0f / input.material->specularWidth / diffuse) / diffuse;
	if (shade2 > 15.0f) shade2 = 15.0f;
	specular.R =
		shade2 * input.material->specularColor.R / 65536.0f * (input.texDiffuse.R * 0.5f + 0.5f);
	specular.G =
		shade2 * input.material->specularColor.G / 65536.0f * (input.texDiffuse.G * 0.5f + 0.5f);
	specular.B =
		shade2 * input.material->specularColor.B / 65536.0f * (input.texDiffuse.B * 0.5f + 0.5f);
	return specular;
}

sRGBAfloat cRenderWorker::EnvMapping(const sShaderInputData &input) const
{
	sRGBAfloat envReflect;
	CVector3 reflect;
	double dot = -input.viewVector.Dot(input.normal);
	reflect = input.normal * 2.0 * dot + input.viewVector;

	double alphaTexture = -reflect.GetAlpha() + M_PI;
	double betaTexture = -reflect.GetBeta();
	double texWidth = data->textures.envmapTexture.Width();
	double texHeight = data->textures.envmapTexture.Height();

	if (betaTexture > 0.5 * M_PI) betaTexture = 0.5 * M_PI - betaTexture;

	if (betaTexture < -0.5 * M_PI) betaTexture = -0.5 * M_PI + betaTexture;

	double dtx = alphaTexture / (2.0 * M_PI) * texWidth + texWidth * 8.25;
	double dty = (betaTexture / M_PI + 0.5) * texHeight + texHeight * 8.0;
	dtx = fmod(dtx, texWidth);
	dty = fmod(dty, texHeight);
	if (dtx < 0) dtx = 0;
	if (dty < 0) dty = 0;

	float reflectance = 1.0f;
	if (input.material->fresnelReflectance)
	{
		double n1 = 1.0;
		double n2 = input.material->transparencyIndexOfRefraction;
		reflectance = Reflectance(input.normal, input.viewVector, n1, n2);
		if (reflectance < 0.0f) reflectance = 0.0f;
		if (reflectance > 1.0f) reflectance = 1.0f;
	}

	envReflect.R = data->textures.envmapTexture.Pixel(dtx, dty).R * reflectance;
	envReflect.G = data->textures.envmapTexture.Pixel(dtx, dty).G * reflectance;
	envReflect.B = data->textures.envmapTexture.Pixel(dtx, dty).B * reflectance;
	return envReflect;
}

sRGBAfloat cRenderWorker::SurfaceColour(const sShaderInputData &input) const
{
#ifdef FAST_MAND
	double zx = point.x;
	double zy = point.y;
	int N = params->N * 10.0;
	double p = 2.0;
	double smooth = 0.0;
	int L = 0;
	double r = 0.0;
	int nrKol = 253 * 256;
	for (L = 0; L < N; L++)
	{
		double temp = zx * zx - zy * zy + point.x;
		zy = 2.0 * zx * zy + point.y;
		zx = temp;
		r = zx * zx + zy * zy;
		if (r > 1e20)
		{
			smooth = (L - log(log(sqrt(r)) / log(N)) / log(p));
			nrKol = smooth * 50.0;
			nrKol = abs(nrKol) % (248 * 256);
			break;
		}
	}

	N_counter += L + 1;
	Loop_counter++;

	if (L / 10 < 64)
		histogram[L / 10]++;
	else
		histogram[63]++;
	return nrKol;

#else
	sRGBAfloat out;

	switch (data->objectData[input.objectId].objectType)
	{
		case fractal::objFractal:
		{
			sRGB colour(256, 256, 256);
			if (input.material->useColorsFromPalette)
			{
				int formulaIndex = input.objectId;

				CVector3 tempPoint = input.point;

				if (!params->booleanOperatorsEnabled)
					formulaIndex = -1;
				else
				{
					tempPoint = tempPoint.mod(params->formulaRepeat[formulaIndex])
											- params->formulaPosition[formulaIndex];
					tempPoint = params->mRotFormulaRotation[formulaIndex].RotateVector(tempPoint);
					tempPoint *= params->formulaScale[formulaIndex];
				}

				sFractalIn fractIn(tempPoint, 0, params->N * 10, params->common, formulaIndex,
					input.material->fractalColoring);
				sFractalOut fractOut;
				Compute<fractal::calcModeColouring>(*fractal, fractIn, &fractOut);
				int nrCol = floor(fractOut.colorIndex);
				nrCol = abs(nrCol) % (248 * 256);

				int color_number;

				color_number =
					int(nrCol * input.material->coloring_speed + 256 * input.material->paletteOffset) % 65536;

				colour = input.material->palette.IndexToColour(color_number);
			}
			else
			{
				colour.R = input.material->color.R / 256.0;
				colour.G = input.material->color.G / 256.0;
				colour.B = input.material->color.B / 256.0;
			}

			out.R = colour.R / 256.0f;
			out.G = colour.G / 256.0f;
			out.B = colour.B / 256.0f;
			break;
		}

		case fractal::objPlane:
		case fractal::objWater:
		case fractal::objSphere:
		case fractal::objBox:
		case fractal::objRectangle:
		case fractal::objCircle:
		case fractal::objCone:
		case fractal::objTorus:
		case fractal::objCylinder:
		{
			out.R = input.material->color.R / 65536.0f;
			out.G = input.material->color.G / 65536.0f;
			out.B = input.material->color.B / 65536.0f;
			break;
		}
		case fractal::objNone:
		{
			out = sRGBAfloat();
			break;
		}
	};

	return out;
#endif
}

sRGBAfloat cRenderWorker::LightShading(
	const sShaderInputData &input, const sLight *light, int number, sRGBAfloat *outSpecular) const
{
	sRGBAfloat shading;

	CVector3 d = light->position - input.point;

	float distance = d.Length();

	// angle of incidence
	CVector3 lightVector = d;
	lightVector.Normalize();

	// intensity of lights is divided by 6 because of backward compatibility. There was an error
	// where number of light was always 24
	float intensity = 100.0f * light->intensity / (distance * distance) / number / 6;
	float shade = input.normal.Dot(lightVector);
	if (shade < 0) shade = 0;
	shade = 1.0f - input.material->shading + shade * input.material->shading;

	shade = shade * intensity;
	if (shade > 500.0f) shade = 500.0f;

	// specular
	CVector3 half = lightVector - input.viewVector;
	half.Normalize();
	float shade2 = input.normal.Dot(half);
	if (shade2 < 0.0f) shade2 = 0.0f;

	float diffuse =
		10.0f * (1.1f
							- input.material->diffusionTextureIntensity
									* (input.texDiffuse.R + input.texDiffuse.G + input.texDiffuse.B) / 3.0f);

	shade2 = pow(shade2, 30.0f / input.material->specularWidth / diffuse) / diffuse;
	shade2 *= intensity * input.material->specular;
	if (shade2 > 15.0f) shade2 = 15.0f;

	// calculate shadow
	if ((shade > 0.01f || shade2 > 0.01f) && params->shadow)
	{
		double auxShadow = AuxShadow(input, distance, lightVector);
		shade *= auxShadow;
		shade2 *= auxShadow;
	}
	else
	{
		if (params->shadow)
		{
			shade = 0;
			shade2 = 0;
		}
	}

	shading.R = shade * light->colour.R / 65536.0f;
	shading.G = shade * light->colour.G / 65536.0f;
	shading.B = shade * light->colour.B / 65536.0f;

	outSpecular->R = shade2 * light->colour.R / 65536.0f;
	outSpecular->G = shade2 * light->colour.G / 65536.0f;
	outSpecular->B = shade2 * light->colour.B / 65536.0f;

	return shading;
}

sRGBAfloat cRenderWorker::AuxLightsShader(
	const sShaderInputData &input, sRGBAfloat *specularOut) const
{

	int numberOfLights = data->lights.GetNumberOfLights();
	if (numberOfLights < 4) numberOfLights = 4;
	sRGBAfloat shadeAuxSum;
	sRGBAfloat specularAuxSum;
	for (int i = 0; i < numberOfLights; i++)
	{
		const sLight *light = data->lights.GetLight(i);
		if (i < params->auxLightNumber || light->enabled)
		{
			sRGBAfloat specularAuxOutTemp;
			sRGBAfloat shadeAux = LightShading(input, light, numberOfLights, &specularAuxOutTemp);
			shadeAuxSum.R += shadeAux.R;
			shadeAuxSum.G += shadeAux.G;
			shadeAuxSum.B += shadeAux.B;
			specularAuxSum.R += specularAuxOutTemp.R;
			specularAuxSum.G += specularAuxOutTemp.G;
			specularAuxSum.B += specularAuxOutTemp.B;
		}
	}
	*specularOut = specularAuxSum;
	return shadeAuxSum;
}

double cRenderWorker::AuxShadow(
	const sShaderInputData &input, double distance, CVector3 lightVector) const
{
	// double step = input.delta;
	double dist;
	double light;

	double opacity;
	double shadowTemp = 1.0;

	double DE_factor = params->DEFactor;
	if (params->iterFogEnabled || params->volumetricLightAnyEnabled) DE_factor = 1.0;

	for (double i = input.delta; i < distance; i += dist * DE_factor)
	{
		CVector3 point2 = input.point + lightVector * i;

		sDistanceOut distanceOut;
		sDistanceIn distanceIn(point2, input.distThresh, false);
		dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		if (params->iterFogEnabled)
		{
			opacity = IterOpacity(dist * DE_factor, distanceOut.iters, params->N,
				params->iterFogOpacityTrim, params->iterFogOpacity);
		}
		else
		{
			opacity = 0.0;
		}
		shadowTemp -= opacity * (distance - i) / distance;

		float dist_thresh;
		if (params->iterFogEnabled || params->volumetricLightAnyEnabled)
		{
			dist_thresh = CalcDistThresh(point2);
		}
		else
			dist_thresh = input.distThresh;

		if (dist < dist_thresh || shadowTemp < 0.0)
		{
			if (params->penetratingLights)
			{
				shadowTemp -= (distance - i) / distance;
				if (shadowTemp < 0.0) shadowTemp = 0.0;
			}
			else
			{
				shadowTemp = 0.0;
			}
			break;
		}
	}
	light = shadowTemp;
	return light;
}

double cRenderWorker::IterOpacity(
	const double step, double iters, double maxN, double trim, double opacitySp)
{
	double opacity = (iters - trim) / maxN;
	if (opacity < 0.0) opacity = 0.0;
	opacity *= opacity;
	opacity *= step * opacitySp;
	if (opacity > 1.0) opacity = 1.0;
	return opacity;
}

// will be done later
sRGBAfloat cRenderWorker::FakeLights(const sShaderInputData &input, sRGBAfloat *fakeSpec) const
{
	sRGBAfloat fakeLights;

	double delta = input.distThresh * params->smoothness;

	sFractalIn fractIn(input.point, params->minN, params->N, params->common, -1);
	sFractalOut fractOut;
	Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
	double rr = fractOut.orbitTrapR;

	double fakeLight = params->fakeLightsIntensity / rr;
	double r = 1.0 / (rr + 1e-30);

	CVector3 deltaX(delta, 0.0, 0.0);
	CVector3 deltaY(0.0, delta, 0.0);
	CVector3 deltaZ(0.0, 0.0, delta);

	fractIn.point = input.point + deltaX;
	Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
	double rx = 1.0 / (fractOut.orbitTrapR + 1e-30);

	fractIn.point = input.point + deltaY;
	Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
	double ry = 1.0 / (fractOut.orbitTrapR + 1e-30);

	fractIn.point = input.point + deltaZ;
	Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
	double rz = 1.0 / (fractOut.orbitTrapR + 1e-30);

	CVector3 fakeLightNormal;
	fakeLightNormal.x = r - rx;
	fakeLightNormal.y = r - ry;
	fakeLightNormal.z = r - rz;

	if (qFuzzyIsNull(fakeLightNormal.x) && qFuzzyIsNull(fakeLightNormal.y)
			&& qFuzzyIsNull(fakeLightNormal.z))
	{
		fakeLightNormal.x = 0.0;
	}
	else
	{
		fakeLightNormal.Normalize();
	}
	float fakeLight2 = fakeLight * input.normal.Dot(fakeLightNormal);
	if (fakeLight2 < 0) fakeLight2 = 0;

	fakeLights.R = fakeLight2;
	fakeLights.G = fakeLight2;
	fakeLights.B = fakeLight2;

	CVector3 half = fakeLightNormal - input.viewVector;
	half.Normalize();
	float fakeSpecular = input.normal.Dot(half);
	if (fakeSpecular < 0.0f) fakeSpecular = 0.0f;
	float diffuse =
		10.0f * (1.1f
							- input.material->diffusionTextureIntensity
									* (input.texDiffuse.R + input.texDiffuse.G + input.texDiffuse.B) / 3.0f);
	fakeSpecular = pow(fakeSpecular, 30.0f / input.material->specularWidth / diffuse) / diffuse;
	if (fakeSpecular > 15.0f) fakeSpecular = 15.0f;
	fakeSpec->R = fakeSpecular;
	fakeSpec->G = fakeSpecular;
	fakeSpec->B = fakeSpecular;

	*fakeSpec = sRGBAfloat();
	return fakeLights;
}

sRGBFloat cRenderWorker::TextureShader(
	const sShaderInputData &input, texture::enumTextureSelection texSelect, cMaterial *mat) const
{
	cObjectData objectData = data->objectData[input.objectId];
	double texturePixelSize;
	CVector3 textureVectorX, textureVectorY;
	CVector2<double> texPoint =
		TextureMapping(input.point, input.normal, objectData, mat, &textureVectorX, &textureVectorY)
		+ CVector2<double>(0.5, 0.5);

	// mipmapping - calculation of texture pixel size
	double delta = CalcDelta(input.point);
	double deltaTexX =
		(TextureMapping(input.point + textureVectorX * delta, input.normal, objectData, mat)
			+ CVector2<double>(0.5, 0.5) - texPoint)
			.Length();
	double deltaTexY =
		(TextureMapping(input.point + textureVectorY * delta, input.normal, objectData, mat)
			+ CVector2<double>(0.5, 0.5) - texPoint)
			.Length();

	if (deltaTexX > 0.5) deltaTexX = 1.0 - deltaTexX;
	if (deltaTexY > 0.5) deltaTexY = 1.0 - deltaTexY;

	deltaTexX = fabs(deltaTexX) / fabs(input.viewVector.Dot(input.normal));
	deltaTexY = fabs(deltaTexY) / fabs(input.viewVector.Dot(input.normal));

	texturePixelSize = 1.0 / max(deltaTexX, deltaTexY);

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
	}

	return sRGBFloat(tex.R, tex.G, tex.B);
}

CVector3 cRenderWorker::NormalMapShader(const sShaderInputData &input) const
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

	CVector3 n = input.normal;
	// tangent vectors:
	CVector3 t = n.Cross(texX);
	t.Normalize();
	CVector3 b = n.Cross(texY);
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
		tex = input.material->normalMapTexture.NormalMap(
			texPoint, input.material->normalMapTextureHeight, texturePixelSize);
	}

	CVector3 result = tbn * tex;
	result.Normalize();
	return result;
}
