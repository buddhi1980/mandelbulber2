/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2021 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * TODO: description
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

// cLight::~cLight() = default;

// this static list will be use to optimize usage of material parameters
const QStringList cLight::paramsList = {"is_defined", "enabled", "cast_shadows", "penetrating",
	"relative_position", "volumetric", "cone_angle", "cone_soft_angle", "intensity", "visibility",
	"volumetric_visibility", "size", "soft_shadow_cone", "contour_sharpness", "position", "rotation",
	"alpha", "beta", "color", "type", "decayFunction", "file_texture", "repeat_texture",
	"projection_horizonal_angle", "projection_vertical_angle"};

void cLight::setParameters(int _id, const std::shared_ptr<cParameterContainer> lightParam,
	bool loadTextures, bool quiet, bool useNetRender)
{
	id = _id;

	int frameNo = lightParam->Get<int>("frame_no");

	enabled = lightParam->Get<bool>(Name("enabled", id));
	type = enumLightType(lightParam->Get<int>(Name("type", id)));
	castShadows = lightParam->Get<bool>(Name("cast_shadows", id));
	penetrating = lightParam->Get<bool>(Name("penetrating", id));
	relativePosition = lightParam->Get<bool>(Name("relative_position", id));
	repeatTexture = lightParam->Get<bool>(Name("repeat_texture", id));
	volumetric = lightParam->Get<bool>(Name("volumetric", id));

	double allLightsIntensity = lightParam->Get<double>("all_lights_intensity");
	double allLightsVisibility = lightParam->Get<double>("all_lights_visibility");
	double allLightsSize = lightParam->Get<double>("all_lights_size");

	coneAngle = lightParam->Get<double>(Name("cone_angle", id)) / 180.0 * M_PI;
	coneSoftAngle = lightParam->Get<double>(Name("cone_soft_angle", id)) / 180.0 * M_PI;
	intensity = lightParam->Get<double>(Name("intensity", id)) * allLightsIntensity;
	visibility = lightParam->Get<double>(Name("visibility", id)) * allLightsVisibility;
	volumetricVisibility = lightParam->Get<double>(Name("volumetric_visibility", id));
	size = lightParam->Get<double>(Name("size", id)) * allLightsSize;
	softShadowCone = lightParam->Get<double>(Name("soft_shadow_cone", id)) / 180.0 * M_PI;
	contourSharpness = lightParam->Get<double>(Name("contour_sharpness", id));

	rotation = lightParam->Get<CVector3>(Name("rotation", id)) / 180.8 * M_PI;
	if (type == lightDirectional)
	{
		rotation *= CVector3(1.0, -1.0, 1.0);
	}
	else
	{
		rotation *= CVector3(-1.0, 1.0, 1.0);
	}

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

		lightTopVector = cameraTarget.GetTopVector();
		lightTopVector =
			lightTopVector.RotateAroundVectorByAngle(cameraTarget.GetForwardVector(), rotation.z);
		lightTopVector =
			lightTopVector.RotateAroundVectorByAngle(cameraTarget.GetRightVector(), rotation.y);
		lightTopVector =
			lightTopVector.RotateAroundVectorByAngle(cameraTarget.GetTopVector(), rotation.x);

		lightRightVector = lightDirection.Cross(lightTopVector);
	}
	else
	{
		position = lightParam->Get<CVector3>(Name("position", id));
		lightDirection = rotMatrix.RotateVector(CVector3(0.0, -1.0, 0.0));
		lightTopVector = rotMatrix.RotateVector(CVector3(0.0, 0.0, 1.0));
		lightRightVector = rotMatrix.RotateVector(CVector3(-1.0, 0.0, 0.0));
	}

	color = toRGBFloat(lightParam->Get<sRGB>(Name("color", id)));

	decayFunction = enumLightDecayFunction(lightParam->Get<int>(Name("decayFunction", id)));

	coneRatio = cos(coneAngle);
	coneSoftRatio = cos(coneSoftAngle + coneAngle);
	projectionHorizontalRatio =
		tan(lightParam->Get<double>(Name("projection_horizonal_angle", id)) / 360.0 * M_PI) * 2.0;
	projectionVerticalRatio =
		tan(lightParam->Get<double>(Name("projection_vertical_angle", id)) / 360.0 * M_PI) * 2.0;

	if (loadTextures)
	{
		if (type == lightProjection)
		{
			colorTexture = cTexture(lightParam->Get<QString>(Name("file_texture", id)),
				cTexture::doNotUseMipmaps, frameNo, quiet, useNetRender);
		}
	}
}

float cLight::CalculateCone(const CVector3 &lightVector, sRGBFloat &outColor) const
{
	outColor = {1.0, 1.0, 1.0};
	float intens = 1.0;

	if (type == lightConical)
	{
		double axiality = lightVector.Dot(lightDirection);

		if (axiality > coneRatio)
		{
			intens = 1.0;
		}
		else if (axiality > coneSoftRatio)
		{
			intens = (axiality - coneSoftRatio) / (coneRatio - coneSoftRatio);
		}
		else
		{
			intens = 0.0;
		}
	}
	else if (type == lightProjection)
	{
		double axiality = lightVector.Dot(lightDirection);
		if (axiality > 0.0 && projectionHorizontalRatio > 0.0 && projectionVerticalRatio > 0.0)
		{
			double texX = lightRightVector.Dot(lightVector) / projectionHorizontalRatio / axiality + 0.5;
			double texY = lightTopVector.Dot(lightVector) / projectionVerticalRatio / axiality + 0.5;

			if (repeatTexture || (texX > 0.0 && texX < 1.0 && texY > 0.0 && texY < 1.0))
			{
				sRGBFloat pixel = colorTexture.Pixel(CVector2<float>(texX, texY), 0.0);
				outColor = pixel;
				intens = 1.0;
			}
			else
			{
				outColor = {0.0, 0.0, 0.0};
				intens = 0.0;
			}
		}
		else
		{
			outColor = {0.0, 0.0, 0.0};
			intens = 0.0;
		}
	}
	return intens;
}

CVector3 cLight::CalculateLightVector(const CVector3 &point, double delta, double resolution,
	double viewDistanceMax, double &outDistance) const
{
	CVector3 lightVector;
	if (type == cLight::lightDirectional)
	{
		lightVector = lightDirection;
		if (penetrating)
		{
			outDistance = delta / resolution;
		}
		else
		{
			outDistance = viewDistanceMax;
		}
	}
	else
	{
		CVector3 d = position - point;
		lightVector = d;
		lightVector.Normalize();
		outDistance = d.Length();
	}

	return lightVector;
}
