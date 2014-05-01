/*
 * fractal_list.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: krzysztof
 */

#include "fractal_list.hpp"

QList<sFractalDescription> fractalList;

using namespace fractal;
void DefineFractalList(QList<sFractalDescription> *fractalList)
{
	//list of fractal formulas (name visible in UI, internal name, internal ID)
	fractalList->append(sFractalDescription("None", "", none, fractal::deltaDE));
	fractalList->append(sFractalDescription("Mandelbulb", "mandelbulb", mandelbulb, fractal::analitycDE));
	fractalList->append(sFractalDescription("Mandelbulb Daniel White's", "mandelbulb2", fast_mandelbulb, fractal::analitycDE));
	fractalList->append(sFractalDescription("Mandelbox", "mandelbox", mandelbox, fractal::analitycDE));
}

