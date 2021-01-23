/*
 * light.cpp
 *
 *  Created on: 22 gru 2020
 *      Author: krzysztof
 */

#include "light.h"

#include "camera_target.hpp"
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
QStringList cLight::paramsList = {"enabled", "cast_shadows", "penetrating", "relative_position",
	"volumetric", "cone_angle", "cone_soft_angle", "intensity", "visibility", "volumetric_visibility",
	"size", "soft_shadow_cone", "position", "rotation", "color", "type", "decayFunction"};

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

	coneAngle = lightParam->Get<double>(Name("cone_angle", id)) / 180.0 * M_PI;
	coneSoftAngle = lightParam->Get<double>(Name("cone_soft_angle", id)) / 180.0 * M_PI;
	intensity = lightParam->Get<double>(Name("intensity", id));
	visibility = lightParam->Get<double>(Name("visibility", id));
	volumetricVisibility = lightParam->Get<double>(Name("volumetric_visibility", id));
	size = lightParam->Get<double>(Name("size", id));
	softShadowCone = lightParam->Get<double>(Name("soft_shadow_cone", id)) / 180.0 * M_PI;

	rotation =
		lightParam->Get<CVector3>(Name("rotation", id)) / 180.8 * M_PI * CVector3(-1.0, 1.0, 1.0);
	rotMatrix.SetRotation(rotation);

	if (relativePosition)
	{
		CVector3 camera = lightParam->Get<CVector3>("camera");
		CVector3 target = lightParam->Get<CVector3>("target");
		CVector3 top = lightParam->Get<CVector3>("camera_top");
		cCameraTarget cameraTarget(camera, target, top);

		// qDebug() << camera.Debug() << target.Debug() << top.Debug();

		CVector3 deltaPosition = lightParam->Get<CVector3>(Name("position", id));
		CVector3 deltaPositionRotated = cameraTarget.GetForwardVector() * deltaPosition.z
																		+ cameraTarget.GetTopVector() * deltaPosition.y
																		+ cameraTarget.GetRightVector() * deltaPosition.x;
		position = camera + deltaPositionRotated;
		lightDirection = (-1.0) * cameraTarget.GetForwardVector();
		lightDirection =
			lightDirection.RotateAroundVectorByAngle(cameraTarget.GetForwardVector(), rotation.z);
		lightDirection =
			lightDirection.RotateAroundVectorByAngle(cameraTarget.GetRightVector(), rotation.y);
		lightDirection =
			lightDirection.RotateAroundVectorByAngle(cameraTarget.GetTopVector(), rotation.x);
	}
	else
	{
		position = lightParam->Get<CVector3>(Name("position", id));
		lightDirection = rotMatrix.RotateVector(CVector3(0.0, -1.0, 0.0));
	}

	color = toRGBFloat(lightParam->Get<sRGB>(Name("color", id)));

	type = enumLightType(lightParam->Get<int>(Name("type", id)));
	decayFunction = enumLightDecayFunction(lightParam->Get<int>(Name("decayFunction", id)));

	coneRatio = cos(coneAngle);
	coneSoftRatio = cos(coneSoftAngle + coneAngle);

	// TODO rest of parameters
}

double cLight::CalculateCone(const CVector3 &lightVector) const
{
	double intensity = 1.0;

	if (type == lightConical)
	{
		double axiality = lightVector.Dot(lightDirection);

		if (axiality > coneRatio)
		{
			intensity = 1.0;
		}
		else if (axiality > coneSoftRatio)
		{
			intensity = (axiality - coneSoftRatio) / (coneRatio - coneSoftRatio);
		}
		else
		{
			intensity = 0.0f;
		}
	}
	return intensity;
}
