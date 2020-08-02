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

double cRenderWorker::CloudOpacity(CVector3 point) const
{
	CVector3 point2 = point;

	double opacity = perlinNoise->normalizedOctaveNoise3D_0_1(point2.x / params->cloudsPeriod,
		point2.y / params->cloudsPeriod, point2.z / params->cloudsPeriod, params->cloudsIterations);

	double h =
		clamp(2.0 - abs(3.0 / params->cloudsHeight * (point2.z - params->cloudsCenter.z)), 0.0, 1.0);

	opacity = clamp(opacity - 1.0 + params->cloudsDensity * 2.0, 0.0, 1.0) * params->cloudsOpacity;

	opacity = opacity * h;
	return opacity;
}
