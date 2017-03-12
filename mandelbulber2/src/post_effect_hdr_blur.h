/*
 * post_effect_hdr_blur.h
 *
 *  Created on: 10 mar 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_POST_EFFECT_HDR_BLUR_H_
#define MANDELBULBER2_SRC_POST_EFFECT_HDR_BLUR_H_

#include "color_structures.hpp"

// forward declarations
class cImage;

class cPostEffectHdrBlur : public QObject
{
	Q_OBJECT

public:
	cPostEffectHdrBlur(cImage *_image);
	~cPostEffectHdrBlur();
	void SetParameters(double _radius, double _intensity);

	void Render(bool *stopRequest);

	cImage *image;
	sRGBFloat *tempImage;
	double radius;
	double intensity;

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
};

#endif /* MANDELBULBER2_SRC_POST_EFFECT_HDR_BLUR_H_ */
