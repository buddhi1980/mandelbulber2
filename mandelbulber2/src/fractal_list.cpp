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
	fractalList->append(sFractalDescription("Mandelbulb 2", "mandelbulb2", mandelbulb2, fractal::deltaDE));
	fractalList->append(sFractalDescription("Mandelbulb 3", "mandelbulb3", mandelbulb3, fractal::deltaDE));
	fractalList->append(sFractalDescription("Mandelbulb 4", "mandelbulb4", mandelbulb4, fractal::deltaDE));
	fractalList->append(sFractalDescription("Mandelbulb Classic Power 2", "mandelbulb_power_2", fast_mandelbulb_power2, fractal::deltaDE));
	fractalList->append(sFractalDescription("Xenodreambuie's formula", "xenodreambuie", xenodreambuie, fractal::deltaDE));
	fractalList->append(sFractalDescription("Mandelbox", "mandelbox", mandelbox, fractal::analitycDE));
	fractalList->append(sFractalDescription("Smooth Mandelbox", "smooth_mandelbox", smoothMandelbox, fractal::analitycDE));
	fractalList->append(sFractalDescription("Mandelbox Vary Scale 4D", "mandelbox_vary_scale_4d", mandelboxVaryScale4D, fractal::analitycDE));
	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2", "boxfold_bulbpow2", boxFoldBulbPow2, fractal::deltaDE));
	// TODO Generazlized fold box formula
	//fractalList->append(sFractalDescription("generalized Fold Box", "generalozed_fold_box", generalizedFoldBox, fractal::analitycDE));
	fractalList->append(sFractalDescription("Menger sponge", "menger_sponge", menger_sponge, fractal::analitycDE));
	fractalList->append(sFractalDescription("Kaleidoscopic IFS", "ifs", kaleidoscopicIFS, fractal::analitycDE));
}

