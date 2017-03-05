/*
 * trace_behind.cpp
 *
 *  Created on: 5 mar 2017
 *      Author: krzysztof
 */

#include "calculate_distance.hpp"
#include "fractal_container.hpp"
#include "fractparams.hpp"
#include "nine_fractals.hpp"
#include "parameters.hpp"

double traceBehindFractal(cParameterContainer *params, cFractalContainer *fractals, double maxDist,
	CVector3 viewVector, double startingDepth, double resolution)
{
	cParamRender *paramRender = new cParamRender(params);
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

		sDistanceIn in(point, 0, false);
		sDistanceOut out;
		double distance = CalculateDistance(*paramRender, *nineFractals, in, &out);

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
	}
	return distanceBehind;
}
