/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRandom class - crossplatform deterministic random generator based on GSL library
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 */

#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

class cRandom
{

public:
	cRandom();
	~cRandom();
	void Initialize(int seed);
	int Random(int max);
	double Random(double min, double max, double resolution);
private:
	gsl_rng *gBaseRand;
};

#endif /* RANDOM_HPP_ */
