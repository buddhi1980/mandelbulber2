/*
 * shader_cloud_opacity.cpp
 *
 *  Created on: 31 lip 2020
 *      Author: krzysztof
 */

#include "render_worker.hpp"
#include "perlin_noise_octaves.h"
#include "common_math.h"

double cRenderWorker::CloudOpacity(CVector3 point, int iterations) const
{
	double opacity = perlinNoise->normalizedOctaveNoise3D_0_1(point.x, point.y, point.z, iterations);
	double h = clamp(0.7 - abs((point.z-1.0) * 1.0), 0.0, 1.0);
	double density = 100.0;
	opacity = clamp(h * (opacity * 1.0 - 0.5), 0.0, 1.0) * density;
	return opacity;
}
