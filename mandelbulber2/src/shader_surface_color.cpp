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
 * cRenderWorker::SurfaceColour method - calculates color of fractal surface
 */
#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::SurfaceColour(const sShaderInputData &input) const
{
	sRGBAfloat out;

	switch (data->objectData[input.objectId].objectType)
	{
		case fractal::objFractal:
		{
			sRGB colour(256, 256, 256);
			if (input.material->useColorsFromPalette)
			{
				int formulaIndex = input.objectId;

				CVector3 tempPoint = input.point;

				if (!params->booleanOperatorsEnabled)
					formulaIndex = -1;
				else
				{
					tempPoint = (tempPoint - params->formulaPosition[formulaIndex])
												.mod(params->formulaRepeat[formulaIndex]);
					tempPoint = params->mRotFormulaRotation[formulaIndex].RotateVector(tempPoint);
					tempPoint *= params->formulaScale[formulaIndex];
				}

				sFractalIn fractIn(
					tempPoint, 0, params->N * 10, params->common, formulaIndex, input.material);
				sFractalOut fractOut;
				Compute<fractal::calcModeColouring>(*fractal, fractIn, &fractOut);
				int nrCol = floor(fractOut.colorIndex);
				nrCol = abs(nrCol) % (248 * 256);

				int color_number;

				color_number =
					int(nrCol * input.material->coloring_speed + 256 * input.material->paletteOffset) % 65536;

				colour = input.material->palette.IndexToColour(color_number);
			}
			else
			{
				colour.R = input.material->color.R / 256.0;
				colour.G = input.material->color.G / 256.0;
				colour.B = input.material->color.B / 256.0;
			}

			out.R = colour.R / 256.0f;
			out.G = colour.G / 256.0f;
			out.B = colour.B / 256.0f;
			break;
		}

		case fractal::objPlane:
		case fractal::objWater:
		case fractal::objSphere:
		case fractal::objBox:
		case fractal::objRectangle:
		case fractal::objCircle:
		case fractal::objCone:
		case fractal::objTorus:
		case fractal::objCylinder:
		{
			out.R = input.material->color.R / 65536.0f;
			out.G = input.material->color.G / 65536.0f;
			out.B = input.material->color.B / 65536.0f;
			break;
		}
		case fractal::objNone:
		{
			out = sRGBAfloat();
			break;
		}
	};

	return out;
}
