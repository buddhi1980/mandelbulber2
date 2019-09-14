/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Fast kernel for rendering opencl with missing effects
 */

// defined to force recompilation of kernels on NVidia cards with new releases
#define MANDELBULBER_VERSION 2.20-dev002

int GetInteger(int byte, __global char *array)
{
	__global int *intPointer = (__global int *)&array[byte];
	return *intPointer;
}

//------------------ MAIN RENDER FUNCTION --------------------
kernel void fractal3D(__global sClPixel *out, __global char *inBuff,
	__constant sClInConstants *consts, image2d_t image2dBackground, int initRandomSeed)
{
	// get actual pixel
	const int imageX = get_global_id(0);
	const int imageY = get_global_id(1);
	const int cl_offsetX = get_global_offset(0);
	const int cl_offsetY = get_global_offset(1);
	const int buffIndex = (imageX - cl_offsetX) + (imageY - cl_offsetY) * get_global_size(0);

	// randomizing random seed
	int randomSeed = imageX + imageY * consts->params.imageWidth + initRandomSeed;
	for (int i = 0; i < 3; i++)
	{
		randomSeed = RandomInt(&randomSeed);
	}

	float2 screenPoint = (float2){convert_float(imageX), convert_float(imageY)};
	float width = convert_float(consts->params.imageWidth);
	float height = convert_float(consts->params.imageHeight);
	float resolution = 1.0f / height;

	//-------- decode data file ----------------
	// main offset for materials
	int materialsMainOffset = GetInteger(0, inBuff);
	int AOVectorsMainOffset = GetInteger(1 * sizeof(int), inBuff);
	int lightsMainOffset = GetInteger(2 * sizeof(int), inBuff);
	int primitivesMainOffset = GetInteger(3 * sizeof(int), inBuff);

	//--- main material

#ifdef USE_SURFACE_GRADIENT
	int paletteSurfaceOffset;
	int paletteSurfaceLength;
#endif
#ifdef USE_SPECULAR_GRADIENT
	int paletteSpecularOffset;
	int paletteSpecularLength;
#endif
#ifdef USE_DIFFUSE_GRADIENT
	int paletteDiffuseOffset;
	int paletteDiffuseLength;
#endif
#ifdef USE_LUMINOSITY_GRADIENT
	int paletteLuminosityOffset;
	int paletteLuminosityLength;
#endif
#ifdef USE_ROUGHNESS_GRADIENT
	int paletteRoughnessOffset;
	int paletteRoughnessLength;
#endif
#ifdef USE_REFLECTANCE_GRADIENT
	int paletteReflectanceOffset;
	int paletteReflectanceLength;
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
	int paletteTransparencyOffset;
	int paletteTransparencyLength;
#endif

	// number of materials
	int numberOfMaterials = GetInteger(materialsMainOffset, inBuff);

	// materials 1 offset:
	const int materialAddressOffset = materialsMainOffset + 2 * sizeof(int);
	int material0Offset = GetInteger(materialAddressOffset, inBuff);

	// material header
	int materialClOffset = GetInteger(material0Offset, inBuff);
	int paletteItemsOffset = GetInteger(material0Offset + sizeof(int), inBuff);

#ifdef USE_SURFACE_GRADIENT
	paletteSurfaceOffset = GetInteger(material0Offset + sizeof(int) * 2, inBuff);
	paletteSurfaceLength = GetInteger(material0Offset + sizeof(int) * 3, inBuff);
#endif
#ifdef USE_SPECULAR_GRADIENT
	paletteSpecularOffset = GetInteger(material0Offset + sizeof(int) * 4, inBuff);
	paletteSpecularLength = GetInteger(material0Offset + sizeof(int) * 5, inBuff);
#endif
#ifdef USE_DIFFUSE_GRADIENT
	paletteDiffuseOffset = GetInteger(material0Offset + sizeof(int) * 6, inBuff);
	paletteDiffuseLength = GetInteger(material0Offset + sizeof(int) * 7, inBuff);
#endif
#ifdef USE_LUMINOSITY_GRADIENT
	paletteLuminosityOffset = GetInteger(material0Offset + sizeof(int) * 8, inBuff);
	paletteLuminosityLength = GetInteger(material0Offset + sizeof(int) * 9, inBuff);
#endif
#ifdef USE_ROUGHNESS_GRADIENT
	paletteRoughnessOffset = GetInteger(material0Offset + sizeof(int) * 10, inBuff);
	paletteRoughnessLength = GetInteger(material0Offset + sizeof(int) * 11, inBuff);
#endif
#ifdef USE_REFLECTANCE_GRADIENT
	paletteReflectanceOffset = GetInteger(material0Offset + sizeof(int) * 12, inBuff);
	paletteReflectanceLength = GetInteger(material0Offset + sizeof(int) * 13, inBuff);
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
	paletteTransparencyOffset = GetInteger(material0Offset + sizeof(int) * 14, inBuff);
	paletteTransparencyLength = GetInteger(material0Offset + sizeof(int) * 15, inBuff);
#endif

	// material data
	__global sMaterialCl *material = (__global sMaterialCl *)&inBuff[materialClOffset];

	// palette data
	__global float4 *palette = (__global float4 *)&inBuff[paletteItemsOffset];

	//--- AO vectors

	// AO vectors count
	int AOVectorsCount = GetInteger(AOVectorsMainOffset, inBuff);
	int AOVectorsOffset = GetInteger(AOVectorsMainOffset + 1 * sizeof(int), inBuff);

	__global sVectorsAroundCl *__attribute__((aligned(16))) AOVectors =
		(__global sVectorsAroundCl *)&inBuff[AOVectorsOffset];

	//--- Lights

	// AO vectors count
	int numberOfLights = GetInteger(lightsMainOffset, inBuff);
	int lightssOffset = GetInteger(lightsMainOffset + 1 * sizeof(int), inBuff);

	__global sLightCl *__attribute__((aligned(16))) lights =
		(__global sLightCl *)&inBuff[lightssOffset];

	//--- Primitives

	// primitives count
	int numberOfPrimitives = GetInteger(primitivesMainOffset, inBuff);
	int primitivesGlobalPositionOffset = GetInteger(primitivesMainOffset + 1 * sizeof(int), inBuff);
	int primitivesOffset = GetInteger(primitivesMainOffset + 2 * sizeof(int), inBuff);

	// global position of primitives
	__global sPrimitiveGlobalPositionCl *primitivesGlobalPosition =
		(__global sPrimitiveGlobalPositionCl *)&inBuff[primitivesGlobalPositionOffset];

	// data for primitives
	__global sPrimitiveCl *__attribute__((aligned(16))) primitives =
		(__global sPrimitiveCl *)&inBuff[primitivesOffset];

	//--------- end of data file ----------------------------------

	sClPixel pixel;

#ifdef STEREO_REYCYAN
	float3 pixelLeftColor = 0.0f;
	float3 pixelRightColor = 0.0f;
	for (int eye = 0; eye < 2; eye++)
	{
#endif

		// auxiliary vectors
		const float3 one = (float3){1.0f, 0.0f, 0.0f};
		const float3 ones = 1.0f;

		// main rotation matrix
		matrix33 rot;
		rot.m1 = (float3){1.0f, 0.0f, 0.0f};
		rot.m2 = (float3){0.0f, 1.0f, 0.0f};
		rot.m3 = (float3){0.0f, 0.0f, 1.0f};

		rot = RotateZ(rot, consts->params.viewAngle.x);
		rot = RotateX(rot, consts->params.viewAngle.y);
		rot = RotateY(rot, consts->params.viewAngle.z);

		// main light vector
		float lightAlpha = consts->params.mainLightAlpha / 180.0f * M_PI_F;
		float lightBeta = consts->params.mainLightBeta / 180.0f * M_PI_F;
		float3 lightVector = (float3){cos(lightAlpha - 0.5f * M_PI_F) * cos(lightBeta),
			sin(lightAlpha - 0.5f * M_PI_F) * cos(lightBeta), sin(lightBeta)};

		if (consts->params.mainLightPositionAsRelative)
			lightVector = Matrix33MulFloat3(rot, lightVector);

		// sweet spot rotation
		rot = RotateZ(rot, -consts->params.sweetSpotHAngle);
		rot = RotateX(rot, consts->params.sweetSpotVAngle);

		matrix33 rotInv = TransposeMatrix(rot);

		// starting point for ray-marching
		float3 start = consts->params.camera;

// view vector
#ifdef PERSP_EQUIRECTANGULAR
		float aspectRatio = 2.0f;
#else
	float aspectRatio = width / height;
#endif

#ifdef STEREOSCOPIC
#ifndef STEREO_REYCYAN
#ifndef PERSP_EQUIRECTANGULAR
		aspectRatio = StereoModifyAspectRatio(aspectRatio);
#endif
#endif
#endif

		float2 normalizedScreenPoint;
		normalizedScreenPoint.x = (screenPoint.x / width - 0.5f);
		normalizedScreenPoint.y = -(screenPoint.y / height - 0.5f);
		if (consts->params.legacyCoordinateSystem) normalizedScreenPoint.y *= -1.0f;

#ifdef STEREOSCOPIC
#ifndef STEREO_REYCYAN
		int eye = StereoWhichEye(normalizedScreenPoint);
		normalizedScreenPoint = StereoModifyImagePoint(normalizedScreenPoint);
#endif
#endif

		normalizedScreenPoint.x *= aspectRatio;

#ifdef MONTE_CARLO_ANTI_ALIASING
		normalizedScreenPoint.x +=
			(Random(1000.0f, &randomSeed) / 1000.0f - 0.5f) / width * aspectRatio;
		normalizedScreenPoint.y += (Random(1000.0f, &randomSeed) / 1000.0f - 0.5f) / height;
#endif

		float3 viewVectorNotRotated = CalculateViewVector(normalizedScreenPoint, consts->params.fov);
		float3 viewVector = Matrix33MulFloat3(rot, viewVectorNotRotated);

#ifdef MONTE_CARLO_DOF
		MonteCarloDOF(&start, &viewVector, consts, rot, &randomSeed);
#endif

#ifdef STEREOSCOPIC
#ifndef PERSP_FISH_EYE_CUT

#ifdef PERSP_EQUIRECTANGULAR // reduce of stereo effect on poles
		float stereoIntensity = 1.0f - pow(normalizedScreenPoint.y * 2.0f, 10.0f);
#else
		float stereoIntensity = 1.0f;
#endif

		start = StereoCalcEyePosition(start, viewVector, consts->params.topVector,
			consts->params.stereoEyeDistance * stereoIntensity, eye, consts->params.stereoSwapEyes);

		StereoViewVectorCorrection(consts->params.stereoInfiniteCorrection * stereoIntensity, &rot,
			&rotInv, eye, consts->params.stereoSwapEyes, &viewVector);
#else	// PERSP_FISH_EYE_CUT
		float3 eyePosition = 0.0f;
		float3 sideVector = normalize(cross(viewVector, consts->params.topVector));
		float3 rightVector =
			normalize(cross(consts->params.target - consts->params.camera, consts->params.topVector));
		float eyeDistance = consts->params.stereoEyeDistance;
		if (consts->params.stereoSwapEyes) eyeDistance *= -1.0f;

		if (eye == 0)
		{
			eyePosition = start + 0.5f * (rightVector * eyeDistance + sideVector * eyeDistance);
		}
		else
		{
			eyePosition = start - 0.5f * (rightVector * eyeDistance + sideVector * eyeDistance);
		}
		start = eyePosition;
#endif // PERSP_FISH_EYE_CUT
#endif // STEREOSCOPIC

#ifdef PERSP_FISH_EYE_CUT
		bool hemisphereCut = false;
		if (length(normalizedScreenPoint) > 0.5f / consts->params.fov) hemisphereCut = true;
#endif

		float4 color4 = 0.0f;
		float opacityOut = 0.0f;
		float3 surfaceColor = 0.0f;
		float3 iridescence = 0.0f;

		bool found = false;
		int count = 0;

		float3 point;
		float scan, distThresh, distance;

		scan = 1e-10f;

#ifdef PERSP_FISH_EYE_CUT
		if (!hemisphereCut)
		{
#endif // PERSP_FISH_EYE_CUT

			sClCalcParams calcParam;
			calcParam.N = consts->params.N;
			calcParam.normalCalculationMode = false;
			calcParam.iterThreshMode = consts->params.iterThreshMode;
			distThresh = 1e-6f;

			sRenderData renderData;
			renderData.lightVector = lightVector;
			renderData.viewVectorNotRotated = viewVectorNotRotated;
			renderData.material = material;
			renderData.palette = palette;
			renderData.AOVectors = AOVectors;
			renderData.lights = lights;
			renderData.numberOfLights = numberOfLights;

#ifdef USE_SURFACE_GRADIENT
			renderData.paletteSurfaceOffset = paletteSurfaceOffset;
			renderData.paletteSurfaceLength = paletteSurfaceLength;
#endif
#ifdef USE_SPECULAR_GRADIENT
			renderData.paletteSpecularOffset = paletteSpecularOffset;
			renderData.paletteSpecularLength = paletteSpecularLength;
#endif
#ifdef USE_DIFFUSE_GRADIENT
			renderData.paletteDiffuseOffset = paletteDiffuseOffset;
			renderData.paletteDiffuseLength = paletteDiffuseLength;
#endif
#ifdef USE_LUMINOSITY_GRADIENT
			renderData.paletteLuminosityOffset = paletteLuminosityOffset;
			renderData.paletteLuminosityLength = paletteLuminosityLength;
#endif
#ifdef USE_ROUGHNESS_GRADIENT
			renderData.paletteRoughnessOffset = paletteRoughnessOffset;
			renderData.paletteRoughnessLength = paletteRoughnessLength;
#endif
#ifdef USE_REFLECTANCE_GRADIENT
			renderData.paletteReflectanceOffset = paletteReflectanceOffset;
			renderData.paletteReflectanceLength = paletteReflectanceLength;
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
			renderData.paletteTransparencyOffset = paletteTransparencyOffset;
			renderData.paletteTransparencyLength = paletteTransparencyLength;
#endif
			renderData.AOVectorsCount = AOVectorsCount;
			renderData.reflectionsMax = 0;
			renderData.primitives = primitives;
			renderData.numberOfPrimitives = numberOfPrimitives;
			renderData.primitivesGlobalPosition = primitivesGlobalPosition;
			renderData.mRot = rot;
			renderData.mRotInv = rotInv;

			formulaOut outF;
			float step = 0.0f;

			float searchAccuracy = 0.001f * consts->params.detailLevel;
			float searchLimit = 1.0f - searchAccuracy;

			// ray-marching
			for (count = 0; count < MAX_RAYMARCHING && scan < consts->params.viewDistanceMax; count++)
			{
				point = start + viewVector * scan;
				distThresh = CalcDistThresh(point, consts);
				calcParam.distThresh = distThresh;
				calcParam.detailSize = distThresh;
				outF = CalculateDistance(consts, point, &calcParam, &renderData);
				distance = outF.distance;

				if (distance < distThresh)
				{
					found = true;
					break;
				}

#ifdef INTERIOR_MODE
				step = (distance - 0.8f * distThresh) * consts->params.DEFactor;
#else
		step = (distance - 0.5f * distThresh) * consts->params.DEFactor;
#endif

				step *= (1.0f - Random(1000, &randomSeed) / 10000.0f);

#ifdef ADVANCED_QUALITY
				step = clamp(step, consts->params.absMinMarchingStep, consts->params.absMaxMarchingStep);

				if (distThresh > consts->params.absMinMarchingStep)
					step = clamp(step, consts->params.relMinMarchingStep * distThresh,
						consts->params.relMaxMarchingStep * distThresh);
#endif

				scan += step / length(viewVector);
			}

			point = start + viewVector * scan;

			// final binary searching
			if (found)
			{
				step *= 0.5f;
				for (int i = 0; i < 30; i++)
				{
					if (distance < distThresh && distance > distThresh * searchLimit)
					{
						break;
					}
					else
					{
						if (distance > distThresh)
						{
							scan += step;
							point = start + viewVector * scan;
						}
						else if (distance < distThresh * searchLimit)
						{
							scan -= step;
							point = start + viewVector * scan;
						}
					}
					outF = CalculateDistance(consts, point, &calcParam, &renderData);
					distance = outF.distance;
					step *= 0.5f;
				}
			}

			// shaders
			float3 color = 0.0f;
			float alpha = 0.0f;

			distThresh = CalcDistThresh(point, consts);

			sShaderInputDataCl shaderInputData;
			shaderInputData.point = point;
			shaderInputData.viewVector = viewVector;
			shaderInputData.viewVectorNotRotated = viewVectorNotRotated;
			shaderInputData.lightVect = lightVector;
			shaderInputData.distThresh = distThresh;
			shaderInputData.lastDist = distance;
			shaderInputData.delta = calcParam.detailSize;
			shaderInputData.depth = scan;
			shaderInputData.invertMode = false;
			shaderInputData.material = material;
			shaderInputData.palette = palette;
#ifdef USE_SURFACE_GRADIENT
			shaderInputData.paletteSurfaceOffset = renderData.paletteSurfaceOffset;
			shaderInputData.paletteSurfaceLength = renderData.paletteSurfaceLength;
#endif
#ifdef USE_SPECULAR_GRADIENT
			shaderInputData.paletteSpecularOffset = renderData.paletteSpecularOffset;
			shaderInputData.paletteSpecularLength = renderData.paletteSpecularLength;
#endif
#ifdef USE_DIFFUSE_GRADIENT
			shaderInputData.paletteDiffuseOffset = renderData.paletteDiffuseOffset;
			shaderInputData.paletteDiffuseLength = renderData.paletteDiffuseLength;
#endif
#ifdef USE_LUMINOSITY_GRADIENT
			shaderInputData.paletteLuminosityOffset = renderData.paletteLuminosityOffset;
			shaderInputData.paletteLuminosityLength = renderData.paletteLuminosityLength;
#endif
#ifdef USE_ROUGHNESS_GRADIENT
			shaderInputData.paletteRoughnessOffset = renderData.paletteRoughnessOffset;
			shaderInputData.paletteRoughnessLength = renderData.paletteRoughnessLength;
#endif
#ifdef USE_REFLECTANCE_GRADIENT
			shaderInputData.paletteReflectanceOffset = renderData.paletteReflectanceOffset;
			shaderInputData.paletteReflectanceLength = renderData.paletteReflectanceLength;
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
			shaderInputData.paletteTransparencyOffset = renderData.paletteTransparencyOffset;
			shaderInputData.paletteTransparencyLength = renderData.paletteTransparencyLength;
#endif
			shaderInputData.stepCount = count;
			shaderInputData.randomSeed = randomSeed;

			float3 normal =
				NormalVector(consts, &renderData, point, distance, distThresh, false, &calcParam);
			shaderInputData.normal = normal;

			float3 specular = 0.0f;

			sClGradientsCollection gradients;

			if (found)
			{
				color = ObjectShader(consts, &renderData, &shaderInputData, &calcParam, &surfaceColor,
					&specular, &iridescence, &gradients);
				alpha = 1.0f;
			}
			else
			{
				color = BackgroundShader(consts, &renderData, image2dBackground, &shaderInputData);
				scan = 1e20f;
				alpha = 0.0f;
			}

#ifdef GLOW
			// glow init
			float glow = count * consts->params.glowIntensity / 512.0f * consts->params.DEFactor;
			float glowN = 1.0f - glow;
			if (glowN < 0.0f) glowN = 0.0f;

			float3 glowColor;

			glowColor = (glowN * consts->params.glowColor1 + consts->params.glowColor2 * glow);

			glow *= 0.7f;
			float glowOpacity = 1.0f * glow;
			if (glowOpacity > 1.0f) glowOpacity = 1.0f;
			color = glow * glowColor + (1.0f - glowOpacity) * color;
			alpha += glowOpacity;
#endif // GLOW

			color4 = (float4){color.s0, color.s1, color.s2, alpha};

#ifdef PERSP_FISH_EYE_CUT
		}
#endif // PERSP_FISH_EYE_CUT

#ifdef STEREO_REYCYAN
		if (eye == 0)
		{
			pixelLeftColor.s0 = color4.s0;
			pixelLeftColor.s1 = color4.s1;
			pixelLeftColor.s2 = color4.s2;
		}
		else
		{
			pixelRightColor.s0 = color4.s0;
			pixelRightColor.s1 = color4.s1;
			pixelRightColor.s2 = color4.s2;

			pixel.R = pixelRightColor.s0;
			pixel.G = pixelLeftColor.s1;
			pixel.B = pixelLeftColor.s2;
			pixel.zBuffer = scan;
			pixel.colR = surfaceColor.s0 * 256.0f;
			pixel.colG = surfaceColor.s1 * 256.0f;
			pixel.colB = surfaceColor.s2 * 256.0f;
			pixel.opacity = opacityOut * 65535;
			pixel.alpha = color4.s3 * 65535;

			out[buffIndex] = pixel;
		}
	} // next exe

#else	// no STEREO_REYCYAN

	pixel.R = color4.s0;
	pixel.G = color4.s1;
	pixel.B = color4.s2;
	pixel.zBuffer = scan;
	pixel.colR = surfaceColor.s0 * 256.0f;
	pixel.colG = surfaceColor.s1 * 256.0f;
	pixel.colB = surfaceColor.s2 * 256.0f;
	pixel.opacity = opacityOut * 65535;
	pixel.alpha = color4.s3 * 65535;

	out[buffIndex] = pixel;
#endif // STEREO_REYCYAN
}
