/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

sRGBAFloat cRenderWorker::SurfaceColour(
	CVector3 point, const sShaderInputData &input, sGradientsCollection *gradients) const
{
	sRGBAFloat out;

	switch (data->objectData[input.objectId].objectType)
	{
		case fractal::objFractal:
		{
			sRGBFloat colour(1.0, 1.0, 1.0);
			if (input.material->useColorsFromPalette)
			{
				int formulaIndex = input.objectId;

				CVector3 tempPoint = point;

				if (!params->booleanOperatorsEnabled)
					formulaIndex = -1;
				else
				{
					tempPoint = tempPoint - params->formulaPosition[formulaIndex];
					tempPoint = params->mRotFormulaRotation[formulaIndex].RotateVector(tempPoint);
					tempPoint = tempPoint.repeatMod(params->formulaRepeat[formulaIndex]);
					tempPoint *= params->formulaScale[formulaIndex];
				}

				sFractalIn fractIn(
					tempPoint, 0, -1, 4, 0, &params->common, formulaIndex, false, input.material);
				sFractalOut fractOut;
				Compute<fractal::calcModeColouring>(*fractal, fractIn, &fractOut);
				double nrCol = fmod(fabs(fractOut.colorIndex), 248.0 * 256.0); // kept for compatibility

				double colorPosition = fmod(
					nrCol / 256.0 / 10.0 * input.material->coloring_speed + input.material->paletteOffset,
					1.0);

				if (input.material->surfaceGradientEnable)
				{
					colour = input.material->gradientSurface.GetColorFloat(colorPosition, false);
					// TODO - smooth mode for gradient
					gradients->surface = colour;
				}
				else
				{
					colour.R = input.material->color.R;
					colour.G = input.material->color.G;
					colour.B = input.material->color.B;
				}

				if (input.material->specularGradientEnable)
				{
					gradients->specular =
						input.material->gradientSpecular.GetColorFloat(colorPosition, false);
				}

				if (input.material->diffuseGradientEnable)
				{
					gradients->diffuse = input.material->gradientDiffuse.GetColorFloat(colorPosition, false);
				}

				if (input.material->luminosityGradientEnable)
				{
					gradients->luminosity =
						input.material->gradientLuminosity.GetColorFloat(colorPosition, false);
				}

				if (input.material->roughnessGradientEnable)
				{
					gradients->roughness =
						input.material->gradientRoughness.GetColorFloat(colorPosition, false);
				}

				if (input.material->reflectanceGradientEnable)
				{
					gradients->reflectance =
						input.material->gradientReflectance.GetColorFloat(colorPosition, false);
				}

				if (input.material->transparencyGradientEnable)
				{
					gradients->trasparency =
						input.material->gradientTransparency.GetColorFloat(colorPosition, false);
				}
			}
			else
			{
				colour.R = input.material->color.R;
				colour.G = input.material->color.G;
				colour.B = input.material->color.B;
			}

			out.R = colour.R;
			out.G = colour.G;
			out.B = colour.B;
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
		case fractal::objPrism:
		{
			out.R = input.material->color.R;
			out.G = input.material->color.G;
			out.B = input.material->color.B;
			break;
		}
		case fractal::objNone:
		{
			out = sRGBAFloat();
			break;
		}
	};

	return out;
}
