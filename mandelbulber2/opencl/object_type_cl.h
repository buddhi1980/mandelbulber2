/*
 * object_type_cl.h
 *
 *  Created on: 3 kwi 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_OPENCL_OBJECT_TYPE_CL_H_
#define MANDELBULBER2_OPENCL_OBJECT_TYPE_CL_H_

typedef enum {
	clObjNone = -1,
	clObjFractal = 0,
	clObjPlane = 1,
	clObjWater = 2,
	clObjSphere = 3,
	clObjBox = 4,
	clObjRectangle = 5,
	clObjCircle = 6,
	clObjCone = 7,
	clObjCylinder = 8,
	clObjTorus = 9
} enumObjectTypeCl;

#endif /* MANDELBULBER2_OPENCL_OBJECT_TYPE_CL_H_ */
