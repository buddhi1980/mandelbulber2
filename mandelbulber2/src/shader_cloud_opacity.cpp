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
#include <algorithm>

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
