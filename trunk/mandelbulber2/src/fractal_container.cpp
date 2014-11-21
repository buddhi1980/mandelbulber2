/*
 * fractal_container.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: krzysztof
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

