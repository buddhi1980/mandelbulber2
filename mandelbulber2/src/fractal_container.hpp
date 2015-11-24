/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cFractalContainer - class to store fractal parameters
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

#ifndef SRC_FRACTAL_CONTAINER_HPP_
#define SRC_FRACTAL_CONTAINER_HPP_

#define NUMBER_OF_FRACTALS 4

#include "parameters.hpp"

class cFractalContainer
{
public:
	cParameterContainer &operator[](int index);
	const cParameterContainer &operator[](int index) const;
	cParameterContainer &at(int index);
	const cParameterContainer &at(int index) const;

private:
	cParameterContainer fractals[NUMBER_OF_FRACTALS];
};

#endif /* SRC_FRACTAL_CONTAINER_HPP_ */
