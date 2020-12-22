/*
 * light.cpp
 *
 *  Created on: 22 gru 2020
 *      Author: krzysztof
 */

#include "light.h"
#include "parameters.hpp"

cLight::cLight(int _id, const std::shared_ptr<cParameterContainer> lightParam, bool loadTextures,
	bool quiet, bool useNetRender)
{
	setParameters(_id, lightParam, loadTextures, quiet, useNetRender);
}

cLight::~cLight() = default;

// this static list will be use to optimize usage of material parameters
QStringList cLight::paramsList = {"light_color", "TODO list of parameters....."};

void cLight::setParameters(int _id, const std::shared_ptr<cParameterContainer> lightParam,
	bool loadTextures, bool quiet, bool useNetRender)
{
	int frameNo = lightParam->Get<int>("frame_no");

	castShadows = lightParam->Get<bool>(Name("cast_shadows", id));

	// TODO rest of parameters
}
