/*
 * shader_perlin_noise_for_shaders.hpp
 *
 *  Created on: 17 maj 2024
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_SHADER_PERLIN_NOISE_FOR_SHADERS_HPP_
#define MANDELBULBER2_SRC_SHADER_PERLIN_NOISE_FOR_SHADERS_HPP_

#include "algebra.hpp"

struct sRenderData;

double PerlinNoiseDisplacement(
	double distance, const CVector3 &point, sRenderData *data, int objectId);

#endif /* MANDELBULBER2_SRC_SHADER_PERLIN_NOISE_FOR_SHADERS_HPP_ */
