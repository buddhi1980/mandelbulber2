/*
 * commonparams.hpp
 *
 *  Created on: Sep 28, 2014
 *      Author: krzysztof
 */

#ifndef COMMONPARAMS_HPP_
#define COMMONPARAMS_HPP_

#include "algebra.hpp"

struct sCommonParams
{
	CVector3 fakeLightsOrbitTrap;
	CVector3 juliaC;

	int fakeLightsMaxIter;
	int fakeLightsMinIter;

	bool juliaMode;
};



#endif /* COMMONPARAMS_HPP_ */
