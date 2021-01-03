/*
 * light.cpp
 *
 *  Created on: 22 gru 2020
 *      Author: krzysztof
 */

#include "light.h"
#include "parameters.hpp"

cLight::cLight()
{
	// nothing to construct
}

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
	id = _id;

	int frameNo = lightParam->Get<int>("frame_no");

	enabled = lightParam->Get<bool>(Name("enabled", id));
	castShadows = lightParam->Get<bool>(Name("cast_shadows", id));
	penetrating = lightParam->Get<bool>(Name("penetrating", id));
	relativePosition = lightParam->Get<bool>(Name("relative_position", id));
	volumetric = lightParam->Get<bool>(Name("volumetric", id));

	coneAngle = lightParam->Get<double>(Name("cone_angle", id)) * 180.0 / M_PI;
	coneSoftAngle = lightParam->Get<double>(Name("cone_soft_angle", id)) * 180.0 / M_PI;
	intensity = lightParam->Get<double>(Name("intensity", id));
	visibility = lightParam->Get<double>(Name("visibility", id));
	volumetricVisibility = lightParam->Get<double>(Name("volumetric_visibility", id));
	size = lightParam->Get<double>(Name("size", id));
	softShadowCone = lightParam->Get<double>(Name("soft_shadow_cone", id)) * 180.0 / M_PI;

	position = lightParam->Get<CVector3>(Name("position", id));
	rotation = lightParam->Get<CVector3>(Name("rotation", id));

	color = toRGBFloat(lightParam->Get<sRGB>(Name("color", id)));

	type = enumLightType(lightParam->Get<int>(Name("type", id)));
	decayFunction = enumLightDecayFunction(lightParam->Get<int>(Name("decayFunction", id)));

	rotMatrix.SetRotation(rotation);

	// TODO rest of parameters
}
