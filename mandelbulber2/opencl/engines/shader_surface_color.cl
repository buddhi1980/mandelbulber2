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
 * surface color calculation
 */

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

float3 SurfaceColor(__constant sClInConstants *consts, sRenderData *renderData,
	sShaderInputDataCl *input, sClCalcParams *calcParams, sClGradientsCollection *gradients)
{
	float3 out;
	calcParams->distThresh = input->distThresh;
	calcParams->detailSize = input->delta;

	formulaOut fout;
	fout.z = 0.0f;
	fout.iters = 0;
	fout.distance = 0.0f;
	fout.colorIndex = 0.0f;
	fout.maxiter = false;

#ifdef USE_SURFACE_GRADIENT
	gradients->surface = 1.0f;
#endif
#ifdef USE_SPECULAR_GRADIENT
	gradients->specular = 1.0f;
#endif
#ifdef USE_DIFFUSE_GRADIENT
	gradients->diffuse = 1.0f;
#endif
#ifdef USE_LUMINOSITY_GRADIENT
	gradients->luminosity = 0.0f;
#endif
#ifdef USE_ROUGHNESS_GRADIENT
	gradients->roughness = 1.0f;
#endif
#ifdef USE_REFLECTANCE_GRADIENT
	gradients->reflectance = 1.0f;
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
	gradients->transparency = 1.0f;
#endif

	float3 pointTemp = input->point;
	float3 color = (float3){1.0f, 1.0f, 1.0f};

#if (defined(BOOLEAN_OPERATORS) || defined(USE_PRIMITIVES)) && defined(FULL_ENGINE)
	enumObjectTypeCl objectType = renderData->objectsData[input->objectId].objectType;
	switch (objectType)
	{
		case objFractal: {
#endif // defined(BOOLEAN_OPERATORS) || defined(USE_PRIMITIVES)

#ifdef USE_FRACTAL_COLORING
			if (input->material->useColorsFromPalette)
			{
#ifdef BOOLEAN_OPERATORS
				int formulaIndex = input->objectId;

				pointTemp = modRepeat(pointTemp - consts->params.formulaPosition[formulaIndex],
					consts->params.formulaRepeat[formulaIndex]);
				pointTemp = Matrix33MulFloat3(consts->params.mRotFormulaRotation[formulaIndex], pointTemp);
				pointTemp *= consts->params.formulaScale[formulaIndex];

#else
				int formulaIndex = -1;
#endif
				fout =
					Fractal(consts, pointTemp, calcParams, calcModeColouring, input->material, formulaIndex);
				float nCol = fmod(fabs(fout.colorIndex), 248.0f * 256.0f);

				float colorPosition = fmod(
					nCol / 256.0f / 10.0f * input->material->coloring_speed + input->material->paletteOffset,
					1.0f);

#ifdef USE_SURFACE_GRADIENT
				if (input->material->surfaceGradientEnable)
				{
					color = GetColorFromGradient(colorPosition, false, input->paletteSurfaceLength,
						input->palette + input->paletteSurfaceOffset);
					gradients->surface = color;
				}
				else
#endif
				{
					color = input->material->color;
				}

#ifdef USE_SPECULAR_GRADIENT
				if (input->material->specularGradientEnable)
				{
					gradients->specular = GetColorFromGradient(colorPosition, false,
						input->paletteSpecularLength, input->palette + input->paletteSpecularOffset);
				}
#endif
#ifdef USE_DIFFUSE_GRADIENT
				if (input->material->diffuseGradientEnable)
				{
					gradients->diffuse = GetColorFromGradient(colorPosition, false,
						input->paletteDiffuseLength, input->palette + input->paletteDiffuseOffset);
				}
#endif
#ifdef USE_LUMINOSITY_GRADIENT
				if (input->material->luminosityGradientEnable)
				{
					gradients->luminosity = GetColorFromGradient(colorPosition, false,
						input->paletteLuminosityLength, input->palette + input->paletteLuminosityOffset);
				}
#endif
#ifdef USE_ROUGHNESS_GRADIENT
				if (input->material->roughnessGradientEnable)
				{
					gradients->roughness = GetColorFromGradient(colorPosition, false,
						input->paletteRoughnessLength, input->palette + input->paletteRoughnessOffset);
				}
#endif
#ifdef USE_REFLECTANCE_GRADIENT
				if (input->material->reflectanceGradientEnable)
				{
					gradients->reflectance = GetColorFromGradient(colorPosition, false,
						input->paletteReflectanceLength, input->palette + input->paletteReflectanceOffset);
				}
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
				if (input->material->transparencyGradientEnable)
				{
					gradients->transparency = GetColorFromGradient(colorPosition, false,
						input->paletteTransparencyLength, input->palette + input->paletteTransparencyOffset);
				}
#endif
			}
			else
#endif // USE_FRACTAL_COLORING
			{
				color = input->material->color;
			}
#if (defined(BOOLEAN_OPERATORS) || defined(USE_PRIMITIVES)) && defined(FULL_ENGINE)
			break;
		}
		case objPlane:
		case objWater:
		case objSphere:
		case objBox:
		case objRectangle:
		case objCircle:
		case objCone:
		case objTorus:
		case objCylinder:
		{
			color = input->material->color;
			break;
		}
		case objNone: { color = 0.0f;
		}
	}
#endif

	out = (float3){color.x, color.y, color.z};

	return out;
}
