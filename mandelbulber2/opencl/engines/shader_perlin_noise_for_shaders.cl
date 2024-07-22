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
 * calculation of perlin noise for shaders
 *
 */

#ifdef USE_PERLIN_NOISE
void PerlinNoiseForShaders(__constant sClInConstants *consts, sClCalcParams *calcParam,
	sShaderInputDataCl *shaderInputData, sRenderData *renderData, float3 point)
{
	if (shaderInputData->material->perlinNoiseEnable)
	{
		float3 pointModified = point;

#ifdef FRACTALIZE_TEXTURE
		if (shaderInputData->material->textureFractalize)
		{
			formulaOut outF;
			outF = Fractal(
				consts, pointModified, calcParam, calcModeCubeOrbitTrap, shaderInputData->material, -1);
			pointModified = outF.z.xyz;
		}
#endif

		pointModified =
			Matrix33MulFloat3(shaderInputData->material->rotMatrixPerlinNoise, pointModified);

		float perlin = NormalizedOctavePerlinNoise3D_0_1(
			pointModified.x / shaderInputData->material->perlinNoisePeriod.x,
			pointModified.y / shaderInputData->material->perlinNoisePeriod.y,
			pointModified.z / shaderInputData->material->perlinNoisePeriod.z,
			shaderInputData->material->perlinNoisePositionOffset,
			shaderInputData->material->perlinNoiseIterations, renderData->perlinNoiseSeeds);

		perlin += shaderInputData->material->perlinNoiseValueOffset;

		if (shaderInputData->material->perlinNoiseAbs) perlin = fabs(perlin - 0.5f) * 2.0f;
		perlin = clamp(perlin, 0.0f, 1.0f);
		shaderInputData->perlinNoise = perlin;
	}
	else
	{
		shaderInputData->perlinNoise = 0.0f;
	}
}

float3 PerlinNoiseForReflectance(sShaderInputDataCl *shaderInputData, float3 reflectance)
{
	float perlin = (shaderInputData->material->perlinNoiseReflectanceInvert)
									 ? 1.0f - shaderInputData->perlinNoise
									 : shaderInputData->perlinNoise;
	float3 reflectancePerlin = 1.0f;

#ifdef USE_REFLECTANCE_GRADIENT
	if (shaderInputData->material->reflectanceGradientEnable)
	{
		float colorPosition = fmod(
			perlin * shaderInputData->material->coloring_speed + shaderInputData->material->paletteOffset,
			1.0f);

		float3 gradientColor =
			GetColorFromGradient(colorPosition, false, shaderInputData->paletteReflectanceLength,
				shaderInputData->palette + shaderInputData->paletteReflectanceOffset);

		reflectancePerlin = gradientColor;
	}
	else
#endif // USE_REFLECTANCE_GRADIENT
	{
		float perlinCol = perlin;
		reflectancePerlin = perlinCol;
	}
	float perlinRefInt = shaderInputData->material->perlinNoiseReflectanceIntensity;
	float perlinRefIntN = 1.0f - shaderInputData->material->perlinNoiseReflectanceIntensity;
	reflectance *= reflectancePerlin * perlinRefInt + perlinRefIntN;

	return reflectance;
}

float3 PerlinNoiseForTransparency(
	sShaderInputDataCl *shaderInputData, float3 transparency, bool volumeMode)
{
	float perlin = (shaderInputData->material->perlinNoiseTransparencyColorInvert)
									 ? 1.0f - shaderInputData->perlinNoise
									 : shaderInputData->perlinNoise;
	float3 transparencyPerlin = 1.0f;

#ifdef USE_TRANSPARENCY_GRADIENT
	if (shaderInputData->material->transparencyGradientEnable)
	{
		float colorPosition = fmod(
			perlin * shaderInputData->material->coloring_speed + shaderInputData->material->paletteOffset,
			1.0f);

		float3 gradientColor =
			GetColorFromGradient(colorPosition, false, shaderInputData->paletteTransparencyLength,
				shaderInputData->palette + shaderInputData->paletteTransparencyOffset);

		transparencyPerlin = gradientColor;
	}
	else
#endif // USE_TRANSPARENCY_GRADIENT
	{
		float perlinCol = perlin;
		transparencyPerlin = perlinCol;
	}

	float intensity = (volumeMode)
											? shaderInputData->material->perlinNoiseTransparencyColorIntensityVol
											: shaderInputData->material->perlinNoiseTransparencyColorIntensity;

	float perlinRefInt = intensity;
	float perlinRefIntN = 1.0f - intensity;
	transparency *= transparencyPerlin * perlinRefInt + perlinRefIntN;

	return transparency;
}

float PerlinNoiseDisplacement(float distance, float3 point, sRenderData *renderData, int objectId)
{
	__global sObjectDataCl *objectData = &renderData->objectsData[objectId];
	__global sMaterialCl *mat = renderData->materials[objectData->materialId];

	if (mat->perlinNoiseEnable && mat->perlinNoiseDisplacementEnable)
	{
		float3 pointModified = Matrix33MulFloat3(mat->rotMatrixPerlinNoise, point);

		float perlin = NormalizedOctavePerlinNoise3D_0_1(pointModified.x / mat->perlinNoisePeriod.x,
			pointModified.y / mat->perlinNoisePeriod.y, pointModified.z / mat->perlinNoisePeriod.z,
			mat->perlinNoisePositionOffset, mat->perlinNoiseIterations, renderData->perlinNoiseSeeds);

		perlin += mat->perlinNoiseValueOffset;

		if (mat->perlinNoiseAbs) perlin = fabs(perlin - 0.5f) * 2.0f;
		if (mat->perlinNoiseDisplacementInvert) perlin = 1.0 - perlin;
		perlin = clamp(perlin, 0.0f, 1.0f);

		distance -= perlin * mat->perlinNoiseDisplacementIntensity;
	}
	return distance;
}

#endif // USE_PERLIN_NOISE
