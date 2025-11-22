/*
 * object_node_type.h
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OBJECT_NODE_TYPE_H_
#define MANDELBULBER2_SRC_OBJECT_NODE_TYPE_H_

enum class enumNodeType
{
	fractal = 1,
	primitive = 2,
	hybrid = 10,
	booleanAdd = 11,
	booleanMul = 12,
	booleanSub = 13,
};

#endif /* MANDELBULBER2_SRC_OBJECT_NODE_TYPE_H_ */
