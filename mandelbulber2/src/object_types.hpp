/*
 * object_types.hpp
 *
 *  Created on: 3 kwi 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OBJECT_TYPES_HPP_
#define MANDELBULBER2_SRC_OBJECT_TYPES_HPP_

namespace fractal
{
enum enumObjectType
{
	objNone = -1,
	objFractal = 0,
	objPlane = 1,
	objWater = 2,
	objSphere = 3,
	objBox = 4,
	objRectangle = 5,
	objCircle = 6,
	objCone = 7,
	objCylinder = 8,
	objTorus = 9
};
}

#endif /* MANDELBULBER2_SRC_OBJECT_TYPES_HPP_ */
