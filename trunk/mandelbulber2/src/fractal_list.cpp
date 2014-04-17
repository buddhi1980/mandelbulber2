/*
 * fractal_list.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: krzysztof
 */

#include "fractal_list.hpp"

QList<sFractalDescription> fractalList;

void DefineFractalList(QList<sFractalDescription> *fractalList)
{
	//list of fractal formulas (name visible in UI, internal name, internal ID)
	fractalList->append(sFractalDescription("None", "", 0));
	fractalList->append(sFractalDescription("Mandelbulb", "mandelbulb", 1));
	fractalList->append(sFractalDescription("Mandelbulb Daniel White's", "mandelbulb2", 2));
	fractalList->append(sFractalDescription("Mandelbox", "mandelbox", 3));
}

