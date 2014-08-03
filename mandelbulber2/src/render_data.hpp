/*
 * render_data.hpp
 *
 *  Created on: Aug 3, 2014
 *      Author: krzysztof
 */

#ifndef RENDER_DATA_HPP_
#define RENDER_DATA_HPP_

#include "lights.hpp"
#include "color_palette.hpp"
#include "region.hpp"

struct sRenderData
{
	int rendererID;
	int numberOfThreads;
	cRegion<int> screenRegion;
	cRegion<double> imageRegion;
	sTextures textures;
	cColorPalette palette;
	cLights lights;
};

#endif /* RENDER_DATA_HPP_ */
