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
 * calculation of MC Global Illumination effect
 */

#if defined(MONTE_CARLO_DOF_GLOBAL_ILLUMINATION) && defined(FULL_ENGINE)
float3 GlobalIlumination(__constant sClInConstants *consts, sRenderData *renderData,
	sShaderInputDataCl *input, sClCalcParams *calcParam, image2d_t image2dBackground,
	float3 objectColor)
{
	float3 out = 0.0f;
	sShaderInputDataCl inputCopy = *input;
	float3 objectColorTemp = objectColor;
	float totalOpacity = 0.0f;

	bool finished = false;
	for (int rayDepth = 0; rayDepth < consts->params.reflectionsMax; rayDepth++)
	{
		float3 reflectedDirection = inputCopy.normal;
		float randomX = (Random(20000, &calcParam->randomSeed) - 10000) / 10000.0f;
		float randomY = (Random(20000, &calcParam->randomSeed) - 10000) / 10000.0f;
		float randomZ = (Random(20000, &calcParam->randomSeed) - 10000) / 10000.0f;
		float3 randomVector = (float3){randomX * 1.2f, randomY * 1.2f, randomZ * 1.2f};

		float3 randomizedDirection = normalize(reflectedDirection + randomVector);
		inputCopy.viewVector = randomizedDirection;

		float dist = 0.0f;
		bool found = false;
		int objectId = 0;

		float4 resultShader = 0.0f;
		float3 newColor = objectColor;

		for (float scan = inputCopy.distThresh; scan < consts->params.viewDistanceMax;
				 scan += dist * consts->params.DEFactor)
		{
			float3 point = inputCopy.point + scan * randomizedDirection;

			float distThresh = CalcDistThresh(point, consts);
			inputCopy.distThresh = distThresh;

			calcParam->distThresh = distThresh;
			formulaOut outF;
			outF = CalculateDistance(consts, point, calcParam, renderData);
			dist = outF.distance;
			objectId = outF.objectId;

			if (dist < distThresh)
			{
				if (scan < distThresh * 2.0f)
				{
					return out;
				}
				inputCopy.point = point;

				float3 normal = NormalVector(consts, renderData, inputCopy.point, inputCopy.lastDist,
					inputCopy.distThresh, inputCopy.invertMode, calcParam);
				inputCopy.normal = normal;
				inputCopy.objectId = objectId;
				inputCopy.depth = scan;

				found = true;
				break;
			}
		}

		if (found)
		{
			float3 objectColor;
			float3 specular;
			float3 iridescence;

			__global sObjectDataCl *objectData = &renderData->objectsData[inputCopy.objectId];
			inputCopy.material = renderData->materials[objectData->materialId];
			inputCopy.palette = renderData->palettes[objectData->materialId];

#ifdef USE_SURFACE_GRADIENT
			inputCopy.paletteSurfaceOffset = renderData->paletteSurfaceOffsets[objectData->materialId];
			inputCopy.paletteSurfaceLength = renderData->paletteSurfaceLengths[objectData->materialId];
#endif
#ifdef USE_SPECULAR_GRADIENT
			inputCopy.paletteSpecularOffset = renderData->paletteSpecularOffsets[objectData->materialId];
			inputCopy.paletteSpecularLength = renderData->paletteSpecularLengths[objectData->materialId];
#endif
#ifdef USE_DIFFUSE_GRADIENT
			inputCopy.paletteDiffuseOffset = renderData->paletteDiffuseOffsets[objectData->materialId];
			inputCopy.paletteDiffuseLength = renderData->paletteDiffuseLengths[objectData->materialId];
#endif
#ifdef USE_LUMINOSITY_GRADIENT
			inputCopy.paletteLuminosityOffset =
				renderData->paletteLuminosityOffsets[objectData->materialId];
			inputCopy.paletteLuminosityLength =
				renderData->paletteLuminosityLengths[objectData->materialId];
#endif
#ifdef USE_ROUGHNESS_GRADIENT
			inputCopy.paletteRoughnessOffset =
				renderData->paletteRoughnessOffsets[objectData->materialId];
			inputCopy.paletteRoughnessLength =
				renderData->paletteRoughnessLengths[objectData->materialId];
#endif
#ifdef USE_REFLECTANCE_GRADIENT
			inputCopy.paletteReflectanceOffset =
				renderData->paletteReflectanceOffsets[objectData->materialId];
			inputCopy.paletteReflectanceLength =
				renderData->paletteReflectanceLengths[objectData->materialId];
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
			inputCopy.paletteTransparencyOffset =
				renderData->paletteTransparencyOffsets[objectData->materialId];
			inputCopy.paletteTransparencyLength =
				renderData->paletteTransparencyLengths[objectData->materialId];
#endif

#if USE_TEXTURES
#ifdef USE_COLOR_TEXTURE
			inputCopy.texColor = TextureShader(consts, calcParam, &inputCopy, renderData, objectData,
				inputCopy.material->colorTextureIndex, 1.0f);
#endif

#ifdef USE_DIFFUSION_TEXTURE
			inputCopy.texDiffuse = TextureShader(consts, calcParam, &inputCopy, renderData, objectData,
				inputCopy.material->diffusionTextureIndex, 1.0f);
#endif

#ifdef USE_LUMINOSITY_TEXTURE
			inputCopy.texLuminosity = TextureShader(consts, calcParam, &inputCopy, renderData, objectData,
				inputCopy.material->luminosityTextureIndex, 0.0f);
#endif
#endif

			sClGradientsCollection gradients;

			float3 objectShader = ObjectShader(consts, renderData, &inputCopy, calcParam, &objectColor,
				&specular, &iridescence, &gradients);

			newColor = objectColor;
			resultShader.xyz = objectShader;
		}
		else
		{
			inputCopy.point = inputCopy.point + consts->params.viewDistanceMax * randomizedDirection;
			inputCopy.depth = consts->params.viewDistanceMax;

			float3 backgroundShader = BackgroundShader(consts, renderData, image2dBackground, &inputCopy);
			resultShader.xyz = backgroundShader;
			finished = true;
		}

		float opacityOut = 0.0f;

#ifdef MC_GI_VOLUMETRIC
		resultShader =
			VolumetricShader(consts, renderData, &inputCopy, calcParam, resultShader, &opacityOut);
#endif

		float influence = clamp(1.0f - totalOpacity, 0.0f, 1.0f);
		out += resultShader.xyz * objectColorTemp * influence;

		totalOpacity += opacityOut;
		objectColorTemp = newColor;

		if (finished || totalOpacity > 1.0) break;
	}
	return out;
}
#endif // MONTE_CARLO_DOF_GLOBAL_ILLUMINATION
