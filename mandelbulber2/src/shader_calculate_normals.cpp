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
 * cRenderWorker::CalculateNormals method - calculates surface normal vectors
 */
#include "calculate_distance.hpp"
#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"

CVector3 cRenderWorker::CalculateNormals(const sShaderInputData &input) const
{
	CVector3 normal(0.0, 0.0, 0.0);
	// calculating normal vector based on distance estimation (gradient of distance function)
	if (!params->slowShading)
	{
		double delta = input.distThresh * params->smoothness;
		if (params->interiorMode) delta = input.distThresh * 0.2 * params->smoothness;

		double sx1, sx2, sy1, sy2, sz1, sz2;
		sDistanceOut distanceOut;

		CVector3 deltaX(delta, 0.0, 0.0);
		sDistanceIn distanceIn1(input.point + deltaX, input.distThresh, true);
		sx1 = CalculateDistance(*params, *fractal, distanceIn1, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;
		sDistanceIn distanceIn2(input.point - deltaX, input.distThresh, true);
		sx2 = CalculateDistance(*params, *fractal, distanceIn2, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		CVector3 deltaY(0.0, delta, 0.0);
		sDistanceIn distanceIn3(input.point + deltaY, input.distThresh, true);
		sy1 = CalculateDistance(*params, *fractal, distanceIn3, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;
		sDistanceIn distanceIn4(input.point - deltaY, input.distThresh, true);
		sy2 = CalculateDistance(*params, *fractal, distanceIn4, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		CVector3 deltaZ(0.0, 0.0, delta);
		sDistanceIn distanceIn5(input.point + deltaZ, input.distThresh, true);
		sz1 = CalculateDistance(*params, *fractal, distanceIn5, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;
		sDistanceIn distanceIn6(input.point - deltaZ, input.distThresh, true);
		sz2 = CalculateDistance(*params, *fractal, distanceIn6, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		normal.x = sx1 - sx2;
		normal.y = sy1 - sy2;
		normal.z = sz1 - sz2;
	}

	// calculating normal vector based on average value of binary central difference
	else
	{
		CVector3 point2;
		CVector3 point3;
		double delta = input.delta * params->smoothness;
		if (params->interiorMode) delta = input.distThresh * 0.2 * params->smoothness;

		for (point2.x = -1.0; point2.x <= 1.0; point2.x += 0.2) //+0.2
		{
			for (point2.y = -1.0; point2.y <= 1.0; point2.y += 0.2)
			{
				for (point2.z = -1.0; point2.z <= 1.0; point2.z += 0.2)
				{
					point3 = input.point + point2 * delta;

					sFractalIn fractIn(point3, params->minN, params->N, params->common, -1, false);
					sFractalOut fractOut;
					fractOut.colorIndex = 0;

					Compute<fractal::calcModeNormal>(*fractal, fractIn, &fractOut);
					double pseudoDistance = 1 + params->N - fractOut.iters;
					data->statistics.totalNumberOfIterations += fractOut.iters;
					normal += point2 * pseudoDistance;
				}
			}
		}
	}

	if (qFuzzyIsNull(normal.x) && qFuzzyIsNull(normal.y) && qFuzzyIsNull(normal.z))
	{
		normal = CVector3(1.0, 0.0, 0.0);
	}
	else
	{
		normal.Normalize();
	}

	if (normal.IsNotANumber())
	{
		normal = CVector3(1.0, 0.0, 0.0);
	}

	if (input.invertMode) normal *= -1.0;

	// qDebug() << input.point.Debug() << normal.Debug();

	return normal;
}
