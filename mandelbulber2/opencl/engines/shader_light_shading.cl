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
 * calculation of auxiliary light sources
 */

#ifdef AUX_LIGHTS

float3 CalculateLightVector(__global sLightCl *light, float3 point, float delta, float resolution,
	float viewDistanceMax, float *outDistance)
{
	float3 lightVector;
	if (light->type == lightDirectional)
	{
		lightVector = light->lightDirection;
		if (light->penetrating)
		{
			*outDistance = delta / resolution;
		}
		else
		{
			*outDistance = viewDistanceMax;
		}
	}
	else
	{
		float3 d = light->position - point;
		lightVector = normalize(d);
		*outDistance = length(d);
	}
	return lightVector;
}

float LightDecay(float dist, enumLightDecayFunctionCl decayFunction)
{
	return pow(dist, (float)((int)decayFunction + 1));
}

float CalculateLightCone(
	__global sLightCl *light, sRenderData *renderData, float3 lightVector, float3 *outColor)
{
	float3 color = 1.0f;
	float intensity = 1.0f;

	if (light->type == lightConical)
	{
		float axiality = dot(lightVector, light->lightDirection);

		if (axiality > light->coneRatio)
		{
			intensity = 1.0f;
		}
		else if (axiality > light->coneSoftRatio)
		{
			intensity = (axiality - light->coneSoftRatio) / (light->coneRatio - light->coneSoftRatio);
		}
		else
		{
			intensity = 0.0f;
		}
	}
	else if (light->type == lightProjection)
	{
#ifdef USE_LIGHT_TEXTURE
		if (light->colorTextureIndex >= 0)
		{
			float axiality = dot(lightVector, light->lightDirection);
			if (axiality > 0.0f && light->projectionHorizontalRatio > 0.0f
					&& light->projectionVerticalRatio > 0.0f)
			{
				float texX =
					dot(light->lightRightVector, lightVector) / light->projectionHorizontalRatio / axiality
					+ 0.5f;
				float texY =
					dot(light->lightTopVector, lightVector) / light->projectionVerticalRatio / axiality
					+ 0.5f;

				if (light->repeatTexture || (texX > 0.0f && texX < 1.0f && texY > 0.0f && texY < 1.0f))
				{
					float2 texturePoint = (float2){texX, texY};

					int2 textureSize = renderData->textureSizes[light->colorTextureIndex];
					__global uchar4 *texture = renderData->textures[light->colorTextureIndex];

					float3 texOut = BicubicInterpolation(
						texturePoint.x, texturePoint.y, texture, textureSize.x, textureSize.y);

					color = texOut;
					intensity = 1.0f;
				}
				else
				{
					color = 0.0f;
					intensity = 0.0f;
				}
			}
			else
			{
				color = 0.0f;
				intensity = 0.0f;
			}
		}
#endif // USE_LIGHT_TEXTURE
	}

	*outColor = color;
	return intensity;
}

float3 LightShading(__constant sClInConstants *consts, sRenderData *renderData,
	sShaderInputDataCl *input, sClCalcParams *calcParam, float3 surfaceColor,
	__global sLightCl *light, sClGradientsCollection *gradients, float3 *outSpecular)
{
	float3 shading = 0.0f;

	float dist = 0.0f;

	float3 lightVector = CalculateLightVector(light, input->point, input->delta,
		consts->params.resolution, consts->params.viewDistanceMax, &dist);

	float intensity = 0.0f;
	if (light->type == lightDirectional)
	{
		intensity = light->intensity;
	}
	else
	{
		intensity = 100.0f * light->intensity / LightDecay(dist, light->decayFunction) / 6.0f;
	}

	float3 textureColor;
	intensity *= CalculateLightCone(light, renderData, lightVector, &textureColor);

	float shade = dot(input->normal, lightVector);
	if (shade < 0.0f) shade = 0.0f;
	shade = 1.0f - input->material->shading + shade * input->material->shading;

	shade = shade * intensity;
	if (shade > 500.0f) shade = 500.0f;

	// specular
	float3 specular =
		SpecularHighlightCombined(input, calcParam, lightVector, surfaceColor, gradients) * intensity;

#ifdef USE_SPECULAR_GRADIENT
	if (input->material->useColorsFromPalette && input->material->specularGradientEnable)
	{
		specular *= gradients->specular;
	}
#endif

	float specularMax = max(max(specular.s0, specular.s1), specular.s2);

	// calculate shadow

	float3 auxShadow = 1.0f;
#ifdef SHADOWS
	if (light->castShadows)
	{
		if (shade > 0.001f || specularMax > 0.001f)
		{
			auxShadow =
				AuxShadow(consts, renderData, input, light, dist, lightVector, calcParam, light->intensity);
			specular *= auxShadow;
		}
		else
		{
			auxShadow = 0.0f;
			specular = 0.0f;
		}
	}
#endif // SHADOWS

	shading = shade * light->color * auxShadow * textureColor;
	*outSpecular = specular * light->color * textureColor;

	return shading;
}
#endif // AUX_LIGHTS
