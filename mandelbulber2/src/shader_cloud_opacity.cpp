/*
 * shader_cloud_opacity.cpp
 *
 *  Created on: 31 lip 2020
 *      Author: krzysztof
 */

#include "render_worker.hpp"
#include "perlin_noise_octaves.h"
#include "common_math.h"
#include "fractparams.hpp"

double cRenderWorker::CloudOpacity(CVector3 point, double distance) const
{
	double h = 1.0; // height factor
	CVector3 point2;

	if (params->cloudsPlainShape)
	{
		point2 = params->mRotCloudsRotation.RotateVector(point - params->cloudsCenter);
		h = clamp(2.0 - abs(3.0 / params->cloudsHeight * (point2.z)), 0.0, 1.0);
	}
	else
	{
		h = 1.0;
		point2 = point;
	}

	if (params->cloudsDistanceMode)
	{
		h *= clamp(
			2.0 - abs(3.0 / params->cloudsDistanceLayer * (distance - params->cloudsDistance)), 0.0, 1.0);
	}

	if (h > 0)
	{
		double opacity = perlinNoise->normalizedOctaveNoise3D_0_1(point2.x / params->cloudsPeriod,
			point2.y / params->cloudsPeriod, point2.z / params->cloudsPeriod, params->cloudsIterations);
		opacity = clamp(opacity - 1.0 + params->cloudsDensity * 2.0, 0.0, 1.0) * params->cloudsOpacity;

		opacity = opacity * h;
		return opacity;
	}
	else
	{
		return 0.0;
	}
}
