/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * list of fractal formulas
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "fractal_list.hpp"

QList<sFractalDescription> fractalList;

using namespace fractal;
void DefineFractalList(QList<sFractalDescription> *fractalList)
{
	//list of fractal formulas (name visible in UI, internal name, internal ID)
	fractalList->append(sFractalDescription("None", "", none, deltaDE));
	fractalList->append(sFractalDescription("Mandelbulb", "mandelbulb", mandelbulb, analitycDE));
	fractalList->append(sFractalDescription("Mandelbulb 2", "mandelbulb2", mandelbulb2, deltaDE));
	fractalList->append(sFractalDescription("Mandelbulb 3", "mandelbulb3", mandelbulb3, deltaDE));
	fractalList->append(sFractalDescription("Mandelbulb 4", "mandelbulb4", mandelbulb4, deltaDE));
	fractalList->append(sFractalDescription("Mandelbulb Classic Power 2", "mandelbulb_power_2", fast_mandelbulb_power2, deltaDE));
	fractalList->append(sFractalDescription("Xenodreambuie's formula", "xenodreambuie", xenodreambuie, deltaDE));
	fractalList->append(sFractalDescription("Mandelbox", "mandelbox", mandelbox, analitycDE));
	fractalList->append(sFractalDescription("Smooth Mandelbox", "smooth_mandelbox", smoothMandelbox, analitycDE));
	fractalList->append(sFractalDescription("Mandelbox Vary Scale 4D", "mandelbox_vary_scale_4d", mandelboxVaryScale4D, analitycDE));
	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2", "boxfold_bulbpow2", boxFoldBulbPow2, deltaDE));
	// TODO Generazlized fold box formula
	//fractalList->append(sFractalDescription("generalized Fold Box", "generalozed_fold_box", generalizedFoldBox, analitycDE));
	fractalList->append(sFractalDescription("Menger sponge", "menger_sponge", menger_sponge, analitycDE));
	fractalList->append(sFractalDescription("Kaleidoscopic IFS", "ifs", kaleidoscopicIFS, analitycDE));
	fractalList->append(sFractalDescription("Hypercomplex", "hypercomplex", hypercomplex, deltaDE));
	fractalList->append(sFractalDescription("Quaternion", "quaternion", quaternion, deltaDE));
	fractalList->append(sFractalDescription("Aexion", "aexion", aexion, deltaDE));
	fractalList->append(sFractalDescription("Benesi", "benesi", benesi, deltaDE));
	fractalList->append(sFractalDescription("Bristorbrot", "bristorbrot", bristorbrot, deltaDE));

	fractalList->append(sFractalDescription("Ides Formula", "ides", ides, deltaDE));
	fractalList->append(sFractalDescription("Ides Formula 2", "ides2", ides2, deltaDE));
	fractalList->append(sFractalDescription("Buffalo 1", "buffalo1", buffalo1, deltaDE));
	fractalList->append(sFractalDescription("Buffalo 2", "buffalo2", buffalo2, deltaDE));
	fractalList->append(sFractalDescription("Buffalo 3", "buffalo3", buffalo3, deltaDE));
	fractalList->append(sFractalDescription("Buffalo 4", "buffalo4", buffalo4, deltaDE));
	fractalList->append(sFractalDescription("Buffalo 5", "buffalo5", buffalo5, deltaDE));
	fractalList->append(sFractalDescription("Buffalo 6", "buffalo6", buffalo6, deltaDE));
	fractalList->append(sFractalDescription("Buffalo 7", "buffalo7", buffalo7, deltaDE));
	fractalList->append(sFractalDescription("Quick-Dudley", "quickdudley", quickdudley, deltaDE));
	fractalList->append(sFractalDescription("Lkmitch", "lkmitch", lkmitch, deltaDE));
	fractalList->append(sFractalDescription("Makin3D-2", "makin3d2", makin3d2, deltaDE));
}
