/*
 * image_scale.hpp
 *
 *  Created on: 2 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_IMAGE_SCALE_HPP_
#define MANDELBULBER2_SRC_IMAGE_SCALE_HPP_

#include "cimage.hpp"

double ImageScaleComboSelection2Double(int index);
double CalcMainImageScale(double scale, int previewWidth, int previewHeight, cImage *image);

#endif /* MANDELBULBER2_SRC_IMAGE_SCALE_HPP_ */
