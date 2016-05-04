/*
 * texture_mapping.hpp
 *
 *  Created on: 2 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_TEXTURE_MAPPING_HPP_
#define MANDELBULBER2_SRC_TEXTURE_MAPPING_HPP_

#include "object_data.hpp"
#include "material.h"

CVector2<double> TextureMapping(CVector3 inPoint, CVector3 normalVector,
		const cObjectData &objectData, const cMaterial *material, CVector3 *textureVectorX = NULL,
		CVector3 *textureVectorY = NULL);



#endif /* MANDELBULBER2_SRC_TEXTURE_MAPPING_HPP_ */
