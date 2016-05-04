/*
 * image_adjustments.h
 *
 *  Created on: 2 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_IMAGE_ADJUSTMENTS_H_
#define MANDELBULBER2_SRC_IMAGE_ADJUSTMENTS_H_

struct sImageAdjustments
{
	sImageAdjustments() :
			brightness(1.0), contrast(1.0), imageGamma(1.0), hdrEnabled(false)
	{
	}

	double brightness;
	double contrast;
	double imageGamma;
	bool hdrEnabled;
};

#endif /* MANDELBULBER2_SRC_IMAGE_ADJUSTMENTS_H_ */
