/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cLights class - container for light source definitions
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

#ifndef LIGHTS_HPP_
#define LIGHTS_HPP_

#include "parameters.hpp"
#include "fractal_container.hpp"

class cLights
{
public:
	struct sLight
	{
		CVector3 position;
		sRGB colour;
		double intensity;
		bool enabled;
		sLight() : position(), colour(), intensity(), enabled() {};
	};

	cLights();
	cLights(const cLights& _lights) {Copy(_lights);}
	cLights(const cParameterContainer *_params, const cFractalContainer *_fractal);
	void Set(const cParameterContainer *_params, const cFractalContainer *_fractal);
	~cLights();
	sLight GetLight(int index) const;
	int GetNumberOfLights(void) const {return numberOfLights;}

private:
	void Copy(const cLights&);

	sLight *lights;
	int numberOfLights;
	bool lightsReady;
};



#endif /* LIGHTS_HPP_ */
