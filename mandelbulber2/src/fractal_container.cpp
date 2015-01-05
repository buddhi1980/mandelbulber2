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

#include "fractal_container.hpp"

cParameterContainer &cFractalContainer::operator[](int index)
{
	if (index >= 0 && index < NUMBER_OF_FRACTALS)
	{
		return fractals[index];
	}
	else
	{
		qCritical() << "cParameterContainer &cFractalContainer::operator[](int index): wrong index:" << index;
		return fractals[0];
	}
}

const cParameterContainer &cFractalContainer::operator[](int index) const
{
	if (index >= 0 && index < NUMBER_OF_FRACTALS)
	{
		return fractals[index];
	}
	else
	{
		qCritical() << "const cParameterContainer &cFractalContainer::operator[](int index) const: wrong index:" << index;
		return fractals[0];
	}
}

cParameterContainer &cFractalContainer::at(int index)
{
	if (index >= 0 && index < NUMBER_OF_FRACTALS)
	{
		return fractals[index];
	}
	else
	{
		qCritical() << "cParameterContainer &cFractalContainer::at(int index): wrong index:" << index;
		return fractals[0];
	}
}

const cParameterContainer &cFractalContainer::at(int index) const
{
	if (index >= 0 && index < NUMBER_OF_FRACTALS)
	{
		return fractals[index];
	}
	else
	{
		qCritical() << "const cParameterContainer &cFractalContainer::at(int index) const: wrong index:" << index;
		return fractals[0];
	}
}

