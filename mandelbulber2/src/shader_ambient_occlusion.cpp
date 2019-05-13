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
 * cRenderWorker::AmbientOcclusion method - calculates ambient occlusion effect based on multiple
 * rays and light map texture
 */

#include "calculate_distance.hpp"
#include "fractparams.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::AmbientOcclusion(const sShaderInputData &input) const
{
	sRGBAfloat AO(0, 0, 0, 1.0);

	double start_dist = input.delta;
	double end_dist = input.delta / params->resolution;
	double intense;

	int start = 0;
	int end = AOVectorsCount - 1;

	if (params->DOFEnabled && params->DOFMonteCarlo)
	{
		int randomSample = Random(AOVectorsCount - 1);
		start = randomSample;
		end = randomSample;
	}

	for (int i = start; i <= end; i++)
	{
		sVectorsAround v = AOVectorsAround[i];

		double dist;

		double opacity;
		double shadowTemp = 1.0;

		for (double r = start_dist; r < end_dist; r += dist * 2.0)
		{
			CVector3 point2 = input.point + v.v * r;

			sDistanceOut distanceOut;
			sDistanceIn distanceIn(point2, input.distThresh, false);
			dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);
			data->statistics.totalNumberOfIterations += distanceOut.totalIters;

			if (params->iterFogEnabled)
			{
				opacity = IterOpacity(dist * 2.0, distanceOut.iters, params->N, params->iterFogOpacityTrim,
					params->iterFogOpacityTrimHigh, params->iterFogOpacity);
			}
			else
			{
				opacity = 0.0;
			}
			shadowTemp -= opacity * (end_dist - r) / end_dist;

			float dist_thresh;
			if (params->iterFogEnabled || params->volumetricLightEnabled[0])
			{
				dist_thresh = CalcDistThresh(point2);
			}
			else
				dist_thresh = input.distThresh;

			if (dist < dist_thresh || shadowTemp < 0.0)
			{
				shadowTemp -= (end_dist - r) / end_dist;
				if (shadowTemp < 0.0) shadowTemp = 0.0;
				break;
			}
		}

		intense = shadowTemp;

		AO.R += intense * v.R;
		AO.G += intense * v.G;
		AO.B += intense * v.B;
	}

	if (params->DOFEnabled && params->DOFMonteCarlo)
	{
		AO.R /= 65536.0;
		AO.G /= 65536.0;
		AO.B /= 65536.0;
	}
	else
	{
		AO.R /= AOVectorsCount * 65536.0;
		AO.G /= AOVectorsCount * 65536.0;
		AO.B /= AOVectorsCount * 65536.0;
	}

	return AO;
}
