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
 * cRenderWorker::BackgroundShader method - calculates background
 */

#include "fractparams.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"

sRGBAFloat cRenderWorker::BackgroundShader(const sShaderInputData &input) const
{
	sRGBAFloat pixel2;

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

				CVector2<float> tex(
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

				sRGBFloat pixel = data->textures.backgroundTexture.Pixel(CVector2<float>(texX, texY));
				pixel2.R = pixel.R;
				pixel2.G = pixel.G;
				pixel2.B = pixel.B;
				break;
			}
		}
		pixel2.R *= params->background_brightness;
		pixel2.G *= params->background_brightness;
		pixel2.B *= params->background_brightness;
		pixel2.R = powf(pixel2.R, 1.0f / params->background_gamma);
		pixel2.G = powf(pixel2.G, 1.0f / params->background_gamma);
		pixel2.B = powf(pixel2.B, 1.0f / params->background_gamma);
		pixel2.A = 1.0;
	}
	else
	{
		sRGBFloat pixel;
		if (params->background3ColorsEnable)
		{
			CVector3 vector(0.0, 0.0, 1.0);
			vector.Normalize();
			CVector3 viewVectorNorm = input.viewVector;
			viewVectorNorm.Normalize();
			float grad = viewVectorNorm.Dot(vector) + 1.0f;
			if (grad < 1.0f)
			{
				float gradN = 1.0f - grad;
				pixel.R = params->background_color3.R * gradN + params->background_color2.R * grad;
				pixel.G = params->background_color3.G * gradN + params->background_color2.G * grad;
				pixel.B = params->background_color3.B * gradN + params->background_color2.B * grad;
			}
			else
			{
				grad = grad - 1.0f;
				float gradN = 1.0f - grad;
				pixel.R = params->background_color2.R * gradN + params->background_color1.R * grad;
				pixel.G = params->background_color2.G * gradN + params->background_color1.G * grad;
				pixel.B = params->background_color2.B * gradN + params->background_color1.B * grad;
			}

			pixel.R *= params->background_brightness;
			pixel.G *= params->background_brightness;
			pixel.B *= params->background_brightness;
		}
		else
		{
			pixel.R = params->background_color1.R * params->background_brightness;
			pixel.G = params->background_color1.G * params->background_brightness;
			pixel.B = params->background_color1.B * params->background_brightness;
		}

		pixel2.R = powf(pixel.R, 1.0f / params->background_gamma);
		pixel2.G = powf(pixel.G, 1.0f / params->background_gamma);
		pixel2.B = powf(pixel.B, 1.0f / params->background_gamma);
		pixel2.A = 0.0;
	}

	CVector3 viewVectorNorm = input.viewVector;
	viewVectorNorm.Normalize();

	//	if (params->mainLightEnable)
	//	{
	//		double intensity =
	//			-(viewVectorNorm.Dot(input.lightVect) - 1.0) * 360.0 / params->mainLightVisibilitySize;
	//		intensity = 1.0 / (1.0 + pow(intensity, 6.0 * params->mainLightContourSharpness))
	//								* params->mainLightVisibility * params->mainLightIntensity;
	//		pixel2.R += intensity * params->mainLightColour.R;
	//		pixel2.G += intensity * params->mainLightColour.G;
	//		pixel2.B += intensity * params->mainLightColour.B;
	//	}

	for (int i = 0; i < data->lights.GetNumberOfLights(); i++)
	{
		const cLight *light = data->lights.GetLight(i);
		if (light->enabled && light->type == cLight::lightDirectional)
		{
			double intensity = -(viewVectorNorm.Dot(light->lightDirection) - 1.0) * 360.0 / light->size;
			intensity = 1.0 / (1.0 + pow(intensity, 6.0 * light->contourSharpness)) * light->visibility
									* light->intensity;
			pixel2.R += intensity * light->color.R;
			pixel2.G += intensity * light->color.G;
			pixel2.B += intensity * light->color.B;
		}
	}

	return pixel2;
}
