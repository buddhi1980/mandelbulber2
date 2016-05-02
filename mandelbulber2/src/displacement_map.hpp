/*
 * displacement_map.hpp
 *
 *  Created on: 2 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_DISPLACEMENT_MAP_HPP_
#define MANDELBULBER2_SRC_DISPLACEMENT_MAP_HPP_

#include "algebra.hpp"
#include "render_data.hpp"

double DisplacementMap(double oldDistance, CVector3 point, int objectId, sRenderData *data);

#endif /* MANDELBULBER2_SRC_DISPLACEMENT_MAP_HPP_ */
