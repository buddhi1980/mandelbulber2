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
 * Nebula dractal renderig kernel
 */

#define MANDELBULBER_VERSION 2.34

float4 DummyIteration(float4 z, __global sFractalCl *fractal, sExtendedAuxCl *aux)
{
	aux->r = -1.0f; // signal for main loop to break;
	return 0.0f;
}

int GetInteger(int byte, __global char *array)
{
	__global int *intPointer = (__global int *)&array[byte];
	return *intPointer;
}

float3 GradientInterpolate(
	int paletteIndex, float pos, bool smooth, int gradientSize, __global float4 *palette)
{
	float3 color = 0.0f;
	// if last element then just copy color value (no interpolation)
	if (paletteIndex == gradientSize - 1)
	{
		color = palette[paletteIndex - 1].xyz;
	}
	else
	{
		// interpolation
		float3 color1 = palette[paletteIndex].xyz;
		float pos1 = palette[paletteIndex].w;
		float3 color2 = palette[paletteIndex + 1].xyz;
		float pos2 = palette[paletteIndex + 1].w;

		// relative delta
		if (pos2 - pos1 > 0.0f)
		{
			float delta = (pos - pos1) / (pos2 - pos1);

			if (smooth) delta = 0.5f * (1.0f - cos(delta * M_PI_F));

			float nDelta = 1.0f - delta;
			color.s0 = color1.s0 * nDelta + color2.s0 * delta;
			color.s1 = color1.s1 * nDelta + color2.s1 * delta;
			color.s2 = color1.s2 * nDelta + color2.s2 * delta;
		}
		else
		{
			color = color1;
		}
	}
	return color;
}

int GradientIterator(
	int paletteIndex, float colorPosition, int gradientSize, __global float4 *palette)
{
	int newIndex = paletteIndex;
	while (newIndex < gradientSize - 1 && colorPosition > palette[newIndex + 1].s3)
	{
		newIndex++;
	}
	return newIndex;
}

float3 GetColorFromGradient(float position, bool smooth, int gradientSize, __global float4 *palette)
{
	int paletteIndex = GradientIterator(0, position, gradientSize, palette);
	return GradientInterpolate(paletteIndex, position, smooth, gradientSize, palette);
}

//------------------ MAIN RENDER FUNCTION --------------------
kernel void Nebula(__global float4 *inOutImage, __constant sClInConstants *consts,
	__global char *inBuff, int4 randomInt4)
{
	const ulong index = get_global_id(0);
	const ulong groupId = get_group_id(0);
	const ulong workGroupSize = get_local_size(0);
	const ulong offset = get_global_offset(0);
	ulong gripIdGlobal = groupId + offset / workGroupSize;

	// calculation is done by blocks of 16x16x16 to reduce variety of calculations within one
	// workgroup
	const uint blockGridSize = max(consts->params.imageHeight / 64, 1);
	const uint numberOfBlocks = blockGridSize * blockGridSize * blockGridSize;

	uint blockId = (uint)(gripIdGlobal % numberOfBlocks);
	uint blockX = (blockId % blockGridSize);
	uint blockY = (uint)(blockId / blockGridSize) % blockGridSize;
	uint blockZ = (uint)(blockId / (blockGridSize * blockGridSize)) % blockGridSize;

	float blockSize = 1.0f / blockGridSize;
	float blockOffsetX = (float)blockX * blockSize;
	float blockOffsetY = (float)blockY * blockSize;
	float blockOffsetZ = (float)blockZ * blockSize;

	int imageSize = consts->params.imageWidth * consts->params.imageHeight;

	// preparation of random seeds
	ulong randomSeedX = randomInt4.x + index;
	ulong randomSeedY = randomInt4.y + index;
	ulong randomSeedZ = randomInt4.z + index;
	ulong randomSeedW = randomInt4.w + index;

	//-------- random seed randomization ----------------
	for (int i = 0; i < 10; i++)
	{
		uint dummyx = RandomL(1298117, &randomSeedX);
		uint dummyy = RandomL(855229, &randomSeedY);
		uint dummyz = RandomL(473167, &randomSeedZ);
		uint dummyw = RandomL(672131, &randomSeedW);

		randomSeedX += dummyx;
		randomSeedY += dummyy;
		randomSeedZ += dummyz;
		randomSeedW += dummyw;
	}

	//-------- decode main data file ----------------
	int gradientsOffset = GetInteger(0, inBuff);
	int paletteItemsOffset = GetInteger(gradientsOffset + sizeof(int) * 0, inBuff);

	int paletteOffsetXAxis = GetInteger(gradientsOffset + sizeof(int) * 1, inBuff);
	int paletteLengthXAxis = GetInteger(gradientsOffset + sizeof(int) * 2, inBuff);

	int paletteOffsetYAxis = GetInteger(gradientsOffset + sizeof(int) * 3, inBuff);
	int paletteLengthYAxis = GetInteger(gradientsOffset + sizeof(int) * 4, inBuff);

	int paletteOffsetZAxis = GetInteger(gradientsOffset + sizeof(int) * 5, inBuff);
	int paletteLengthZAxis = GetInteger(gradientsOffset + sizeof(int) * 6, inBuff);

	int paletteOffsetIterations = GetInteger(gradientsOffset + sizeof(int) * 7, inBuff);
	int paletteLengthIterations = GetInteger(gradientsOffset + sizeof(int) * 8, inBuff);

	__global float4 *gradients = (__global float4 *)&inBuff[paletteItemsOffset];

	//--- Nebula sequences (from inBuff using header offsets) ---
	int nebulaSequencesHeaderOffset = GetInteger(1 * sizeof(int), inBuff);
	int numberOfNebulaSequences = GetInteger(nebulaSequencesHeaderOffset, inBuff);
	int nebulaSequencesArrayOffset =
		GetInteger(nebulaSequencesHeaderOffset + 1 * sizeof(int), inBuff);
	__global sNebulaSequenceCl *nebulaSequences = 0;
	if (numberOfNebulaSequences > 0)
	{
		nebulaSequences = (__global sNebulaSequenceCl *)&inBuff[nebulaSequencesArrayOffset];
	}

	//--- Hybrid Sequences (only first sequence, index 0) ---
	int hybridSequencesMainOffset = GetInteger(6 * sizeof(int), inBuff);
	int numberOfHybridSequences = GetInteger(hybridSequencesMainOffset, inBuff);
	int hybridSequencesArrayOffset = GetInteger(hybridSequencesMainOffset + 1 * sizeof(int), inBuff);
	__global sHybridSequenceCl *hybridSequences =
		(__global sHybridSequenceCl *)&inBuff[hybridSequencesArrayOffset];

	//--- Fractals ---
	int fractalsMainOffset = GetInteger(7 * sizeof(int), inBuff);
	int numberOfFractals = GetInteger(fractalsMainOffset, inBuff);
	int fractalsArrayOffset = GetInteger(fractalsMainOffset + 1 * sizeof(int), inBuff);
	__global sFractalCl *fractals = (__global sFractalCl *)&inBuff[fractalsArrayOffset];

	float4 point;

#ifdef LIMITS_ENABLED
	point.x = RandomL(2147483647, &randomSeedX) / 2147483647.0f * blockSize + blockOffsetX;
	point.y = RandomL(2147483647, &randomSeedY) / 2147483647.0f * blockSize + blockOffsetY;
	point.z = RandomL(2147483647, &randomSeedZ) / 2147483647.0f * blockSize + blockOffsetZ;

	float3 limitMax = consts->params.limitMax;
	float3 limitMin = consts->params.limitMin;

	// scale point to limits
	point.xyz = point.xyz * (limitMax - limitMin) + limitMin;
#else
	point.x =
		(RandomL(2147483647, &randomSeedX) / 2147483647.0f * blockSize + blockOffsetX - 0.5f) * 4.0f;
	point.y =
		(RandomL(2147483647, &randomSeedY) / 2147483647.0f * blockSize + blockOffsetY - 0.5f) * 4.0f;
	point.z =
		(RandomL(2147483647, &randomSeedZ) / 2147483647.0f * blockSize + blockOffsetZ - 0.5f) * 4.0f;

	float3 limitMax = 2.0f;
	float3 limitMin = -2.0f;
#endif

#ifdef NEBULA_GRID_DOMAIN_ENABLED
	int axisSelection = RandomL(3, &randomSeedW);

	switch (axisSelection)
	{
		case 0:
		{
			ulong pointX = point.x / consts->params.nebulaXGridSize;
			point.x = (float)pointX * consts->params.nebulaXGridSize;

			ulong pointY = point.y / consts->params.nebulaYGridSize;
			point.y = (float)pointY * consts->params.nebulaYGridSize;
			break;
		}
		case 1:
		{
			ulong pointX = point.x / consts->params.nebulaXGridSize;
			point.x = (float)pointX * consts->params.nebulaXGridSize;

			ulong pointZ = point.z / consts->params.nebulaZGridSize;
			point.z = (float)pointZ * consts->params.nebulaZGridSize;
			break;
		}
		case 2:
		{
			ulong pointY = point.y / consts->params.nebulaYGridSize;
			point.y = (float)pointY * consts->params.nebulaYGridSize;

			ulong pointZ = point.z / consts->params.nebulaZGridSize;
			point.z = (float)pointZ * consts->params.nebulaZGridSize;
			break;
		}
	}

#endif

	// repeat, move and rotate
	float3 pointTransformed = point.xyz - consts->params.common.fractalPosition;
	pointTransformed = Matrix33MulFloat3(consts->params.common.mRotFractalRotation, pointTransformed);
	pointTransformed = modRepeat(pointTransformed, consts->params.common.repeat);

	point.w = (numberOfHybridSequences > 0) ? hybridSequences[0].initialWAxis : 0.0f;

	float4 z = (float4){pointTransformed.x, pointTransformed.y, pointTransformed.z, point.w};
	float4 c = z;
	int i;

	int fractalIndex = 0;

	// formula init
	sExtendedAuxCl aux;

	aux.c = c;
	aux.const_c = c;
	aux.old_z = z;
	aux.pos_neg = 1.0f;
	aux.r = length(z);
	aux.DE = 1.0f;
	aux.DE0 = 0.0;
	aux.dist = 1000.0f;
	aux.pseudoKleinianDE = 1.0f;
	aux.actualScale = fractals[fractalIndex].mandelbox.scale;
	aux.actualScaleA = 0.0f;
	aux.color = 1.0f;
	aux.colorHybrid = 0.0f;
	aux.temp1000 = 1000.0f;

	int sequence = 0;
	__global sFractalCl *fractal;
	__global sFractalCl *defaultFractal = &fractals[fractalIndex];

#ifdef IS_HYBRID
	int fractIdx = 0;
#else
	// Hoisted constants - never change during single-fractal nebula rendering
	int nebulaAddCConstant = nebulaSequences[0].addCConstant;
	int nebulaJuliaEnabled = nebulaSequences[0].juliaEnabled;
	float3 nebulaJuliaConstant = (float3){nebulaSequences[0].juliaConstant.x,
		nebulaSequences[0].juliaConstant.y, nebulaSequences[0].juliaConstant.z};
	float3 nebulaConstantMultiplier = (float3){nebulaSequences[0].constantMultiplier.x,
		nebulaSequences[0].constantMultiplier.y, nebulaSequences[0].constantMultiplier.z};
	int nebulaCheckForBailout = nebulaSequences[0].checkForBailout;
	float nebulaBailout = nebulaSequences[0].bailout;
#endif

	float4 zHistory[MAX_ITERATIONS];

	// loop
	for (i = 0; i < MAX_ITERATIONS; i++)
	{
#ifdef IS_HYBRID
		int seqIdx = 0;
		fractIdx =
			GetInteger(hybridSequences[seqIdx].sequenceArrayOffset + min(i, 249) * sizeof(int), inBuff);
		fractal = &fractals[hybridSequences[seqIdx].formulaBaseIndex + fractIdx];
#else
		fractal = &fractals[fractalIndex];
#endif

		aux.i = i;

		zHistory[i] = z;

#ifdef ITERATION_WEIGHT
		// temporary copies for iteration weighting (mirrors compute_fractal.cl)
		float4 tempZ = z;
		float tempAuxDE = aux.DE;
		float tempAuxColor = aux.color;
#endif

#ifdef ITERATION_WEIGHT
		// Read formulaWeight from fractData
		float formulaWeight = 0.0f;
#ifdef IS_HYBRID
		if (numberOfHybridSequences > 0)
		{
			int fdArrayOffset = hybridSequences[seqIdx].fractDataArrayOffset;
			__global sHybridFractalDataCl *fractData =
				(__global sHybridFractalDataCl *)&inBuff[fdArrayOffset];
			formulaWeight = fractData[fractIdx].formulaWeight;
		}
#endif

		if (nebulaSequences && nebulaSequences[0].formulaWeight > 0)
		{
#endif

			// PLACEHOLDER_FOR_FORMULA_ITER

#ifdef ITERATION_WEIGHT
		}
#endif

		if (aux.r < 0.0f) // if was run DummyIteration
		{
			break;
		}

		// Read addCConstant and julia data from fractData/hybridSequences
		int addCConstant = 0;
		int juliaEnabled = 0;
		float3 juliaConstant = (float3)0.0f;
		float3 constantMultiplier = (float3)1.0f;

#ifdef IS_HYBRID
		if (numberOfHybridSequences > 0)
		{
			int fdArrayOffset = hybridSequences[seqIdx].fractDataArrayOffset;
			__global sHybridFractalDataCl *fractData =
				(__global sHybridFractalDataCl *)&inBuff[fdArrayOffset];
			addCConstant = fractData[fractIdx].addCConstant;

			juliaEnabled = hybridSequences[seqIdx].juliaEnabled;
			juliaConstant = hybridSequences[seqIdx].juliaConstant;
			constantMultiplier = hybridSequences[seqIdx].constantMultiplier;
		}
		else if (nebulaSequences)
		{
			addCConstant = nebulaSequences[0].addCConstant;
			juliaEnabled = nebulaSequences[0].juliaEnabled;
			juliaConstant = (float3){nebulaSequences[0].juliaConstant.x,
				nebulaSequences[0].juliaConstant.y, nebulaSequences[0].juliaConstant.z};
			constantMultiplier = (float3){nebulaSequences[0].constantMultiplier.x,
				nebulaSequences[0].constantMultiplier.y, nebulaSequences[0].constantMultiplier.z};
		}
#else
		addCConstant = nebulaAddCConstant;
		juliaEnabled = nebulaJuliaEnabled;
		juliaConstant = nebulaJuliaConstant;
		constantMultiplier = nebulaConstantMultiplier;
#endif

		if (addCConstant)
		{
			switch (fractal->formula)
			{
				case 64: // aboxMod1
				case 73: // amazingSurf
				{
					if (juliaEnabled)
					{
						float4 juliaC =
							(float4){juliaConstant.y, juliaConstant.x, juliaConstant.z, 0.0f}
							* (float4){constantMultiplier.x, constantMultiplier.y, constantMultiplier.z, 0.0f};
						z += juliaC;
					}
					else
					{
						z += (float4){aux.const_c.y, aux.const_c.x, aux.const_c.z, aux.const_c.w}
								 * (float4){constantMultiplier.x, constantMultiplier.y, constantMultiplier.z, 0.0f};
					}
					break;
				}

				default:
				{
					if (juliaEnabled)
					{
						z += (float4){juliaConstant.x, juliaConstant.y, juliaConstant.z, 0.0f}
								 * (float4){constantMultiplier.x, constantMultiplier.y, constantMultiplier.z, 0.0f};
					}
					else
					{
						z += aux.const_c * constantMultiplier.x;
					}
				}
			}
		}

#ifdef ITERATION_WEIGHT
		if (formulaWeight > 0)
		{
			float k = formulaWeight;
			if (k < 1.0f)
			{
				z = SmoothCVector(tempZ, z, k);
				float kn = 1.0f - k;
				aux.DE = aux.DE * k + tempAuxDE * kn;
				aux.color = aux.color * k + tempAuxColor * kn;
			}
		}
#endif

		// calculate r

		aux.r = length(z);

		// escape conditions
		int checkForBailout = 0;
		float bailout = 0.0f;

#ifdef IS_HYBRID
		if (numberOfHybridSequences > 0)
		{
			int fdArrayOffset = hybridSequences[seqIdx].fractDataArrayOffset;
			__global sHybridFractalDataCl *fractData =
				(__global sHybridFractalDataCl *)&inBuff[fdArrayOffset];
			checkForBailout = fractData[fractIdx].checkForBailout;
			bailout = fractData[fractIdx].bailout;
		}
		else if (nebulaSequences)
		{
			checkForBailout = nebulaSequences[0].checkForBailout;
			bailout = nebulaSequences[0].bailout;
		}
#else
		checkForBailout = nebulaCheckForBailout;
		bailout = nebulaBailout;
#endif

		if (checkForBailout)
		{
			if (aux.r > bailout)
			{
				break;
			}
		}
	} // next i;

	if ((aux.i < MAX_ITERATIONS - 1 && consts->params.nebulaOuterEnabled)
			|| (aux.i == MAX_ITERATIONS - 1 && consts->params.nebulaInnerEnabled))
	{
		float3 camera = consts->params.camera;
		float3 target = consts->params.target;
		float3 top = consts->params.topVector;
		float fov = consts->params.fov;

		float3 forward = normalize(target - camera);
		float3 right = normalize(cross(forward, top));

		matrix33 rotationMatrix;
		rotationMatrix.m1.x = right.x;
		rotationMatrix.m1.y = right.y;
		rotationMatrix.m1.z = right.z;
		rotationMatrix.m2.x = top.x;
		rotationMatrix.m2.y = top.y;
		rotationMatrix.m2.z = top.z;
		rotationMatrix.m3.x = forward.x;
		rotationMatrix.m3.y = forward.y;
		rotationMatrix.m3.z = forward.z;

		int width = consts->params.imageWidth;
		int height = consts->params.imageHeight;
		float aspectRatio = (float)width / height;

		for (int i = consts->params.nebulaMinIteration; i < aux.i; i++)
		{
			z = zHistory[i];

			float distFromClipPlane1 = dot(z.xyz - camera, forward);

			if (distFromClipPlane1 > 0.0f)
			{
				float3 viewVector = z.xyz - camera;
				viewVector = Matrix33MulFloat3(rotationMatrix, viewVector);
				float zDepth = length(viewVector);

				viewVector.x /= viewVector.z;
				viewVector.y /= viewVector.z;

				float2 pointPersp;
				pointPersp.x = viewVector.x / fov;
				pointPersp.y = viewVector.y / fov;

				float2 screenPoint =
					(float2){(pointPersp.x / aspectRatio + 0.5f) * width, (-pointPersp.y + 0.5f) * height};

				int2 screenPointInt = (int2){(int)screenPoint.x, (int)screenPoint.y};

				if (screenPointInt.x >= 0 && screenPointInt.x < width && screenPointInt.y >= 0
						&& screenPointInt.y < height)
				{
					// write to output image
					int screenIndex = (int)(screenPointInt.x + screenPointInt.y * width);

#ifdef NEBULA_X_AXIS_COLORS
					float colorPosX =
						(limitMax.x != limitMin.x)
							? clamp((point.x - limitMin.x) / (limitMax.x - limitMin.x), 0.0f, 1.0f)
							: 0.5f;
					float3 gradientColorX = GetColorFromGradient(
						colorPosX, false, paletteLengthXAxis, gradients + paletteOffsetXAxis);
#else
#ifdef NEBULA_COLOR_MIXING_LIGHTEN
					float3 gradientColorX = (float3){0.0f, 0.0f, 0.0f};
#else
					float3 gradientColorX = (float3){1.0f, 1.0f, 1.0f};
#endif
#endif // NEBULA_X_AXIS_COLORS

#ifdef NEBULA_Y_AXIS_COLORS
					float colorPosY =
						(limitMax.y != limitMin.y)
							? clamp((point.y - limitMin.y) / (limitMax.y - limitMin.y), 0.0f, 1.0f)
							: 0.5f;
					float3 gradientColorY = GetColorFromGradient(
						colorPosY, false, paletteLengthYAxis, gradients + paletteOffsetYAxis);
#else
#ifdef NEBULA_COLOR_MIXING_LIGHTEN
					float3 gradientColorY = (float3){0.0f, 0.0f, 0.0f};
#else
					float3 gradientColorY = (float3){1.0f, 1.0f, 1.0f};
#endif
#endif // NEBULA_Y_AXIS_COLORS

#ifdef NEBULA_Z_AXIS_COLORS
					float colorPosZ =
						(limitMax.z != limitMin.z)
							? clamp((point.z - limitMin.z) / (limitMax.z - limitMin.z), 0.0f, 1.0f)
							: 0.5f;
					float3 gradientColorZ = GetColorFromGradient(
						colorPosZ, false, paletteLengthZAxis, gradients + paletteOffsetZAxis);
#else
#ifdef NEBULA_COLOR_MIXING_LIGHTEN
					float3 gradientColorZ = (float3){0.0f, 0.0f, 0.0f};
#else
					float3 gradientColorZ = (float3){1.0f, 1.0f, 1.0f};
#endif
#endif // NEBULA_Z_AXIS_COLORS

#ifdef NEBULA_ITERATIONS_COLORS
					float colorIterations = (float)(i - consts->params.nebulaMinIteration)
																	/ (float)(MAX_ITERATIONS - consts->params.nebulaMinIteration);
					float3 gradientColorIterations = GetColorFromGradient(
						colorIterations, false, paletteLengthIterations, gradients + paletteOffsetIterations);
#else
#ifdef NEBULA_COLOR_MIXING_LIGHTEN
					float3 gradientColorIterations = (float3){0.0f, 0.0f, 0.0f};
#else
					float3 gradientColorIterations = (float3){1.0f, 1.0f, 1.0f};
#endif
#endif // NEBULA_ITERATIONS_COLORS

					float3 color = 0.0f;

#ifdef NEBULA_COLOR_MIXING_LIGHTEN
					color = gradientColorX + gradientColorY + gradientColorZ + gradientColorIterations;
#endif
#ifdef NEBULA_COLOR_MIXING_DARKEN
					color = gradientColorX * gradientColorY * gradientColorZ * gradientColorIterations;
#endif
#ifdef NEBULA_COLOR_MIXING_DARKEN_BY_BRIGHTNESS
					{
						float luminanceX =
							0.3333f * (gradientColorX.s0 + gradientColorX.s1 + gradientColorX.s2);
						float luminanceY =
							0.3333f * (gradientColorY.s0 + gradientColorY.s1 + gradientColorY.s2);
						float luminanceZ =
							0.3333f * (gradientColorZ.s0 + gradientColorZ.s1 + gradientColorZ.s2);
						float luminanceIterations = 0.3333f
																				* (gradientColorIterations.s0 + gradientColorIterations.s1
																					 + gradientColorIterations.s2);
						float luminance = luminanceX * luminanceY * luminanceZ * luminanceIterations;
						color = luminance
										* (gradientColorX + gradientColorY + gradientColorZ + gradientColorIterations);
					}
#endif

					float4 old = inOutImage[screenIndex];
					float4 outPixel;
					outPixel.s0 = old.s0 + color.s0;
					outPixel.s1 = old.s1 + color.s1;
					outPixel.s2 = old.s2 + color.s2;
					outPixel.s3 = old.s3 + (zDepth - old.s3) * 0.01f; // depth averaging;
					inOutImage[screenIndex] = outPixel;
				}
			}
		}
	}
}
