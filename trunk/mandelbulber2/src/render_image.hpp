/*
 * render_image.hpp
 *
 *  Created on: May 3, 2014
 *      Author: krzysztof
 */

#ifndef RENDER_IMAGE_HPP_
#define RENDER_IMAGE_HPP_

#include "cimage.hpp"
#include "fractparams.hpp"
#include "four_fractals.hpp"
#include "render_data.hpp"

class cRenderer
{
public:
	cRenderer(const cParamRender *_params, const cFourFractals *_fractal, const sRenderData *_renderData, cImage *_image);

	bool RenderImage();

private:
	const cParamRender *params;
	const cFourFractals *fractal;
	const sRenderData *data;
	cImage *image;

};




#endif /* RENDER_IMAGE_HPP_ */
