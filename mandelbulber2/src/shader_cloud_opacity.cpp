/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#include <algorithm>

#include "common_math.h"
#include "fractparams.hpp"
#include "perlin_noise_octaves.h"
#include "render_worker.hpp"

double cRenderWorker::CloudOpacity(
	CVector3 point, double distance, double detailSize, double *distanceOut) const
{
	double h = 1.0; // height factor
	CVector3 point2;
	double distToGeometry = 0.0;
	double opacityOut = 0.0;

	if (params->cloudsPlaneShape)
	{
		point2 = params->mRotCloudsRotation.RotateVector(point - params->cloudsCenter);
		h = clamp(2.0 - fabs(3.0 / params->cloudsHeight * (point2.z)), 0.0, 1.0);

		distToGeometry =
			std::max(fabs(point2.z) - params->cloudsHeight * 2.0, 0.0) * params->cloudsDEApproaching;
	}
	else
	{
		h = 1.0;
		point2 = point;
	}

	if (params->cloudsDistanceMode)
	{
		h *= clamp(2.0 - fabs(3.0 / params->cloudsDistanceLayer * (distance - params->cloudsDistance)),
			0.0, 1.0);

		distToGeometry = std::max(distToGeometry,
											 fabs(distance - params->cloudsDistance) - params->cloudsDistanceLayer * 2.0)
										 * params->cloudsDEApproaching;
	}

	double distToCloud = distToGeometry;
	if (h > 0)
	{
		double opacity = perlinNoise->normalizedOctaveNoise3D_0_1(point2.x / params->cloudsPeriod,
			point2.y / params->cloudsPeriod, point2.z / params->cloudsPeriod, params->cloudsIterations);

		distToCloud = fabs(1.0 - opacity - params->cloudsDensity) * 0.2 * params->cloudsPeriod
									* params->cloudsDEMultiplier;

		opacity = clamp(opacity - 1.0 + params->cloudsDensity * 2.0, 0.0, 1.0) * params->cloudsOpacity;

		opacityOut = opacity * h;
	}

	*distanceOut = std::max(std::max(distToGeometry, distToCloud),
		std::max(params->cloudsPeriod * 0.02 / params->cloudsDetailAccuracy,
			detailSize * 5.0f / params->cloudsDetailAccuracy));

	return opacityOut;
}
