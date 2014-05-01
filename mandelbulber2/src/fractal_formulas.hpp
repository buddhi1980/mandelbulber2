/*
 * fractal_formulas.hpp
 *
 *  Created on: Apr 27, 2014
 *      Author: krzysztof
 */

#ifndef FRACTAL_FORMULAS_HPP_
#define FRACTAL_FORMULAS_HPP_

#include "four_fractals.hpp"

struct sMandelbulbAux
{
	double r_dz;
	double r;
};

void MandelbulbIteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux);

#endif /* FRACTAL_FORMULAS_HPP_ */
