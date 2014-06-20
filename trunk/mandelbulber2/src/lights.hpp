/*
 * lights.hpp
 *
 *  Created on: May 18, 2014
 *      Author: krzysztof
 */

#ifndef LIGHTS_HPP_
#define LIGHTS_HPP_

#include "parameters.hpp"

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
	cLights(const cParameterContainer *_params, const cParameterContainer *_fractal);
	void Set(const cParameterContainer *_params, const cParameterContainer *_fractal);
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
