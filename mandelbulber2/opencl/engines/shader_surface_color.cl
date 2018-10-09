/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

float3 IndexToColour(int index, sShaderInputDataCl *input)
{
	float3 color = (float3){1.0f, 1.0f, 1.0f};

	if (index < 0)
	{
		color = input->palette[input->paletteSize - 1].xyz;
	}
	else
	{
		int col = (index / 256) % (input->paletteSize);
		int colPlus1 = (col + 1) % (input->paletteSize);

		float4 color1 = input->palette[col];
		float4 color2 = input->palette[colPlus1];
		float4 deltaC = (color2 - color1) / 256.0f;

		float delta = index % 256;
		color = (color1 + (deltaC * delta)).xyz;
	}
	return color;
}

float3 SurfaceColor(__constant sClInConstants *consts, sRenderData *renderData,
	sShaderInputDataCl *input, sClCalcParams *calcParams)
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

	float3 pointTemp = input->point;
	float3 color = (float3){1.0f, 1.0f, 1.0f};

#if (defined(BOOLEAN_OPERATORS) || defined(USE_PRIMITIVES)) && defined(FULL_ENGINE)
	enumObjectTypeCl objectType = renderData->objectsData[input->objectId].objectType;
	switch (objectType)
	{
		case objFractal: {
#endif // defined(BOOLEAN_OPERATORS) || defined(USE_PRIMITIVES)

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
				int nCol = floor(fout.colorIndex);
				nCol = abs(nCol) % (248 * 256);
				int color_number =
					(int)(nCol * input->material->coloring_speed + 256 * input->material->paletteOffset)
					% 65536;

				color = IndexToColour(color_number, input);
			}
			else
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
