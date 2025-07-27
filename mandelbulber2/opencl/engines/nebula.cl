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
	__global char *inBuff, int randomInt)
{
	const int index = get_global_id(0);
	int imageSize = consts->params.imageWidth * consts->params.imageHeight;

	int randomSeed = index + randomInt;

	for (int i = 0; i < 10; i++)
	{
		int dummy = Random(1000000, &randomSeed);
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

	float4 point;

#ifdef LIMITS_ENABLED
	point.x = Random(2147483647, &randomSeed) / 2147483647.0f;
	point.y = Random(2147483647, &randomSeed) / 2147483647.0f;
	point.z = Random(2147483647, &randomSeed) / 2147483647.0f;

	float3 limitMax = consts->params.limitMax;
	float3 limitMin = consts->params.limitMin;

	// scale point to limits
	point.xyz = point.xyz * (limitMax - limitMin) + limitMin;
#else
	point.x = (Random(2147483647, &randomSeed) / 1073741823.0f - 1.0f) * 2.0f;
	point.y = (Random(2147483647, &randomSeed) / 1073741823.0f - 1.0f) * 2.0f;
	point.z = (Random(2147483647, &randomSeed) / 1073741823.0f - 1.0f) * 2.0f;

	float3 limitMax = 2.0f;
	float3 limitMin = -2.0f;
#endif

#ifdef NEBULA_GRID_DOMAIN_ENABLED
	int axisSelection = Random(3, &randomSeed);

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

	point.w = consts->sequence.initialWAxis[0];

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
	aux.actualScale = consts->fractal[fractalIndex].mandelbox.scale;
	aux.actualScaleA = 0.0f;
	aux.color = 1.0f;
	aux.colorHybrid = 0.0f;
	aux.temp1000 = 1000.0f;

	int sequence = 0;
	__constant sFractalCl *fractal;
	__constant sFractalCl *defaultFractal = &consts->fractal[fractalIndex];

	float4 zHistory[MAX_ITERATIONS];

	// loop
	for (i = 0; i < MAX_ITERATIONS; i++)
	{

#if defined(IS_HYBRID)
		sequence = consts->sequence.hybridSequence[min(i, 249)];
#else
		sequence = 0;
#endif

		fractal = &consts->fractal[sequence];

		aux.i = i;

		zHistory[i] = z;

#ifdef ITERATION_WEIGHT
		if (consts->sequence.formulaWeight[sequence] > 0)
		{
#endif

#if defined(IS_HYBRID)
			switch (sequence)
			{
				case 0: z = FORMULA_ITER_0(z, fractal, &aux); break;
				case 1: z = FORMULA_ITER_1(z, fractal, &aux); break;
				case 2: z = FORMULA_ITER_2(z, fractal, &aux); break;
				case 3: z = FORMULA_ITER_3(z, fractal, &aux); break;
				case 4: z = FORMULA_ITER_4(z, fractal, &aux); break;
				case 5: z = FORMULA_ITER_5(z, fractal, &aux); break;
				case 6: z = FORMULA_ITER_6(z, fractal, &aux); break;
				case 7: z = FORMULA_ITER_7(z, fractal, &aux); break;
				case 8: z = FORMULA_ITER_8(z, fractal, &aux); break;
			}
#else	 // not HYBRID
		z = FORMULA_ITER_0(z, fractal, &aux);
#endif // defined(IS_HYBRID)

#ifdef ITERATION_WEIGHT
		}
#endif

		if (aux.r < 0.0f) // if was run DummyIteration
		{
			break;
		}

		if (consts->sequence.addCConstant[sequence])
		{
			switch (fractal->formula)
			{
				case 64: // aboxMod1
				case 73: // amazingSurf
				{
					if (consts->sequence.juliaEnabled[sequence])
					{
						float4 juliaC = consts->sequence.juliaConstant[sequence]
														* consts->sequence.constantMultiplier[sequence];
						z += (float4){juliaC.y, juliaC.x, juliaC.z, juliaC.w};
					}
					else
					{
						z += (float4){aux.const_c.y, aux.const_c.x, aux.const_c.z, aux.const_c.w}
								 * consts->sequence.constantMultiplier[sequence];
					}
					break;
				}

				default:
				{
					if (consts->sequence.juliaEnabled[sequence])
					{
						z += consts->sequence.juliaConstant[sequence]
								 * consts->sequence.constantMultiplier[sequence];
					}
					else
					{
						z += aux.const_c * consts->sequence.constantMultiplier[sequence];
					}
				}
			}
		}

#ifdef ITERATION_WEIGHT
		if (consts->sequence.isHybrid)
		{
			float k = consts->sequence.formulaWeight[sequence];
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
		if (consts->sequence.checkForBailout[sequence])
		{
			if (aux.r > consts->sequence.bailout[sequence])
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
					outPixel.s3 = old.s3 + 1.0f;
					inOutImage[screenIndex] = outPixel;
				}
			}
		}
	}
}
