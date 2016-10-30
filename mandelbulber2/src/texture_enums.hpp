/*
 * texture_enums.hpp
 *
 *  Created on: 30 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_TEXTURE_ENUMS_HPP_
#define MANDELBULBER2_SRC_TEXTURE_ENUMS_HPP_

namespace texture
{
enum enumTextureMapping
{
	mappingSpherical = 0,
	mappingCubic = 1,
	mappingCylindrical = 2,
	mappingPlanar = 3
};

enum enumTextureSelection
{
	texColor = 0,
	texDiffuse = 1,
	texLuminosity = 2,
	texDisplacement = 3,
};
}

#endif /* MANDELBULBER2_SRC_TEXTURE_ENUMS_HPP_ */
