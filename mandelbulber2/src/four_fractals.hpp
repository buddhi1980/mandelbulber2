/*
 * four_fractals.hpp
 *
 *  Created on: Apr 26, 2014
 *      Author: krzysztof
 */

#ifndef FOUR_FRACTALS_HPP_
#define FOUR_FRACTALS_HPP_

#include "fractal.h"
#include "parameters.hpp"
#include "fractal_list.hpp"
#include <QtCore>

class cFourFractals
{
public:
	cFourFractals();
	~cFourFractals();
	cFourFractals(const parameters::container *fractalPar, const parameters::container *generalPar);
	cFractal* GetFractal(int index) const {return fourFractals[index];}
	cFractal **fourFractals;
	fractal::enumDEType DEType;
	int GetSequence(int i) const;
	bool IsHybrid() const {return isHybrid;}

private:
	QVector<int> hybridSequence;
	void CreateSequence(const parameters::container *par, const parameters::container *generalPar);
	int maxN;
	bool isHybrid;
};


#endif /* FOUR_FRACTALS_HPP_ */
