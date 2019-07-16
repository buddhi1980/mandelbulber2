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
#define MANDELBULBER_VERSION 2.19

int GetInteger(int byte, __global char *array)
{
	__global int *intPointer = (__global int *)&array[byte];
	return *intPointer;
}

//------------------ MAIN RENDER FUNCTION --------------------
kernel void fractal3D(__global sClPixel *out, __global char *inBuff,
	__constant sClInConstants *consts, int initRandomSeed)
{
	// get actual pixel
	// get actual pixel
	const int imageX = get_global_id(0);
	const int imageY = get_global_id(1);
	const int cl_offsetX = get_global_offset(0);
	const int cl_offsetY = get_global_offset(1);
	const int buffIndex = (imageX - cl_offsetX) + (imageY - cl_offsetY) * get_global_size(0);

	float2 screenPoint = (float2){convert_float(imageX), convert_float(imageY)};
	float width = convert_float(consts->params.imageWidth);
	float height = convert_float(consts->params.imageHeight);
	float resolution = 1.0f / height;

	//-------- decode data file ----------------
	int primitivesMainOffset = GetInteger(3 * sizeof(int), inBuff);

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

		sRenderData renderData;
		renderData.lightVector = 0;
		renderData.viewVectorNotRotated = 0;
		renderData.material = 0;
		renderData.palette = 0;
		renderData.AOVectors = 0;
		renderData.lights = 0;
		renderData.numberOfLights = 0;
		renderData.AOVectorsCount = 0;
		renderData.reflectionsMax = 0;
		renderData.primitives = primitives;
		renderData.numberOfPrimitives = numberOfPrimitives;
		renderData.primitivesGlobalPosition = primitivesGlobalPosition;

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

		float lightAlpha = consts->params.mainLightAlpha / 180.0f * M_PI_F;
		float lightBeta = consts->params.mainLightBeta / 180.0f * M_PI_F;
		float3 lightVector = (float3){cos(lightAlpha - 0.5f * M_PI_F) * cos(lightBeta),
			sin(lightAlpha - 0.5f * M_PI_F) * cos(lightBeta), sin(lightBeta)};
		lightVector = Matrix33MulFloat3(rot, lightVector);

		rot = RotateZ(rot, -consts->params.sweetSpotHAngle);
		rot = RotateX(rot, consts->params.sweetSpotVAngle);

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

		float3 viewVectorNotRotated = CalculateViewVector(normalizedScreenPoint, consts->params.fov);
		float3 viewVector = Matrix33MulFloat3(rot, viewVectorNotRotated);

#ifdef STEREOSCOPIC
#ifndef PERSP_FISH_EYE_CUT

#ifdef PERSP_EQUIRECTANGULAR // reduce of stereo effect on poles
		float stereoIntensity = 1.0f - pow(normalizedScreenPoint.y * 2.0f, 10.0f);
#else
		float stereoIntensity = 1.0f;
#endif

		start = StereoCalcEyePosition(start, viewVector, consts->params.topVector,
			consts->params.stereoEyeDistance * stereoIntensity, eye, consts->params.stereoSwapEyes);

		matrix33 rotInv = TransposeMatrix(rot);
		StereoViewVectorCorrection(consts->params.stereoInfiniteCorrection * stereoIntensity, &rot,
			&rotInv, eye, consts->params.stereoSwapEyes, &viewVector);
#else // PERSP_FISH_EYE_CUT
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
#endif
#endif // STEREOSCOPIC

		bool found = false;
		int count;

		float3 point;
		float scan, distThresh, distance;

		scan = 1e-10f;

		sClCalcParams calcParam;
		calcParam.N = consts->params.N;
		calcParam.normalCalculationMode = false;
		calcParam.iterThreshMode = consts->params.iterThreshMode;
		distThresh = 1e-6f;

		formulaOut outF;
		float step = 0.0f;

		// ray-marching
		for (count = 0; count < MAX_RAYMARCHING && scan < consts->params.viewDistanceMax; count++)
		{
			point = start + viewVector * scan;
			distThresh = CalcDistThresh(point, consts);
			calcParam.distThresh = distThresh;
			calcParam.detailSize = distThresh;
			outF = CalculateDistance(consts, point, &calcParam, &renderData);
			distance = outF.distance;

			if (distance < distThresh * 0.95f)
			{
				found = true;
				break;
			}

			step = (distance - 0.5f * distThresh) * consts->params.DEFactor;

#ifdef ADVANCED_QUALITY
			step = clamp(step, consts->params.absMinMarchingStep, consts->params.absMaxMarchingStep);

			if (distThresh > consts->params.absMinMarchingStep)
				step = clamp(step, consts->params.relMinMarchingStep * distThresh,
					consts->params.relMaxMarchingStep * distThresh);
#endif

			scan += step / length(viewVector);
		}

		// final binary searching
		if (found)
		{
			step *= 0.5f;
			for (int i = 0; i < 5; i++)
			{
				if (distance < distThresh && distance > distThresh * 0.95f)
				{
					break;
				}
				else
				{
					if (distance > distThresh)
					{
						point += viewVector * step;
					}
					else if (distance < distThresh * 0.95f)
					{
						point -= viewVector * step;
					}
				}
				outF = CalculateDistance(consts, point, &calcParam, &renderData);
				distance = outF.distance;
				step *= 0.5f;
			}
		}
		else
		{
			scan = 1e20f;
		}

		float3 colour = 0.7f;
		float alpha = 0.0f;
		float3 surfaceColour = 1.0f;
		if (found)
		{
			distThresh = CalcDistThresh(point, consts);

			float3 normal =
				NormalVector(consts, &renderData, point, distance, distThresh, false, &calcParam);

			float shade = dot(lightVector, normal);
			if (shade < 0.0f) shade = 0.0f;

			float3 halfVector = lightVector - viewVector;
			halfVector = fast_normalize(halfVector);
			float specular = dot(normal, halfVector);
			if (specular < 0.0f) specular = 0.0f;
			specular = pown(specular, 30.0f);
			if (specular > 15.0f) specular = 15.0f;

			colour = colour * (shade + specular);
			alpha = 1.0f;
		}
		else
		{
#ifdef STEREOSCOPIC
			float grid = 0.5f
									 + 0.5f * sin(10.0f * sin(atan2(viewVector.y, viewVector.x) * 10.0f))
											 * sin(10.0f * sin(atan2(viewVector.z, length(viewVector.xy)) * 10.0f));
			grid *= grid;
			colour = grid;
#else	// not STEREOSCOPIC
		colour = (float3){0.0f, 0.0f, screenPoint.y / height};
		alpha = 0.0f;
#endif // STEREOSCOPIC
		}

		float glow = count / 500.0 * consts->params.DEFactor;

#ifdef STEREO_REYCYAN
		if (eye == 0)
		{
			pixelLeftColor.s0 = colour.s2 + glow;
			pixelLeftColor.s1 = colour.s2 + glow;
			pixelLeftColor.s2 = colour.s2;
		}
		else
		{
			pixelRightColor.s0 = colour.s2 + glow;
			pixelRightColor.s1 = colour.s2 + glow;
			pixelRightColor.s2 = colour.s2 + glow;

			sClPixel pixel;
			pixel.R = pixelRightColor.s0;
			pixel.G = pixelLeftColor.s1;
			pixel.B = pixelLeftColor.s2;
			pixel.zBuffer = scan;
			pixel.colR = 128;
			pixel.colG = 128;
			pixel.colB = 128;
			pixel.opacity = 0;
			pixel.alpha = alpha * 65535;

			out[buffIndex] = pixel;
		}
	} // next exe

#else	// no STEREO_REYCYAN

	pixel.R = colour.s0 + glow;
	pixel.G = colour.s1 * glow * 10.0f;
	pixel.B = colour.s2;
	pixel.zBuffer = scan;
	pixel.colR = 128;
	pixel.colG = 128;
	pixel.colB = 128;
	pixel.opacity = 0;
	pixel.alpha = alpha * 65535;

	out[buffIndex] = pixel;
#endif // STEREO_REYCYAN
}
