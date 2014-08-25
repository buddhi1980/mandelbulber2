/*
 * dof.hpp
 *
 *  Created on: Aug 25, 2014
 *      Author: krzysztof
 */

#ifndef DOF_HPP_
#define DOF_HPP_

#include "cimage.hpp"

template <class TYPE>
struct sSortZ
{
	TYPE z;
	int i;
};

void PostRendering_DOF(cImage *image, double deep, double neutral);
template <class T> void QuickSortZBuffer(sSortZ<T> *dane, int l, int p);

#endif /* DOF_HPP_ */
