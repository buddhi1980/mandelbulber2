/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * traceBehindFractal finds the distance in viewVector direction with
 * minimum length 'startingDepth' and maximum length 'maxDist'
 * which does not collide with the fractal.
 */

#include "calculate_distance.hpp"
#include "fractal_container.hpp"
#include "fractparams.hpp"
#include "nine_fractals.hpp"
#include "parameters.hpp"

double traceBehindFractal(cParameterContainer *params, cFractalContainer *fractals, double maxDist,
	CVector3 viewVector, double startingDepth, double resolution, double distanceLimit)
{
	sParamRender *paramRender = new sParamRender(params);
	cNineFractals *nineFractals = new cNineFractals(fractals, params);
	paramRender->resolution = resolution;
	double totalDistanceBehind = 0.0;
	double distanceBehind = 0.0;
	CVector3 point = paramRender->camera + viewVector * startingDepth;

	while (totalDistanceBehind < maxDist)
	{
		double distThresh;
		if (paramRender->constantDEThreshold)
			distThresh = paramRender->DEThresh;
		else
			distThresh = (paramRender->camera - point).Length() * paramRender->resolution
									 * paramRender->fov / paramRender->detailLevel;
		if (paramRender->perspectiveType == params::perspEquirectangular
				|| paramRender->perspectiveType == params::perspFishEye
				|| paramRender->perspectiveType == params::perspFishEyeCut)
			distThresh *= M_PI;

		const sDistanceIn in(point, 0, false);
		sDistanceOut out;
		const double distance = CalculateDistance(*paramRender, *nineFractals, in, &out);

		double step = distance;
		if (step < distThresh)
		{
			step = distThresh;
		}
		else
		{
			totalDistanceBehind += step;
			if (totalDistanceBehind > maxDist)
			{
				step -= (totalDistanceBehind - maxDist);
			}
		}
		distanceBehind += step;
		point = paramRender->camera + viewVector * (startingDepth + distanceBehind);
		// qDebug() << distanceBehind << totalDistanceBehind << distThresh << step << point.Debug();

		if ((paramRender->camera - point).Length() > distanceLimit) break;
	}

	delete paramRender;
	delete nineFractals;

	return distanceBehind;
}
