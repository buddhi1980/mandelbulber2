/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-22 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * definitions of primitive objects
 */

#include <cmath>

#include "primitive.hpp"
#include "common_math.h"

using std::max;

sPrimitiveBasic::sPrimitiveBasic(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
{
	position = par->Get<CVector3>(fullName + "_position");
	materialId = par->Get<int>(fullName + "_material_id");
	SetRotation(par->Get<CVector3>(fullName + "_rotation"));
	enable = par->Get<bool>(fullName + "_enabled");
	booleanOperator = enumPrimitiveBooleanOperator(par->Get<int>(fullName + "_boolean_operator"));
	repeat = par->Get<CVector3>(fullName + "_repeat");
	smoothDeCombineEnable = par->Get<bool>(fullName + "_smooth_de_combine_enable");
	smoothDeCombineDistance = par->Get<double>(fullName + "_smooth_de_combine_distance");
}
std::vector<sPrimitiveBasic::sPrimitiveWireLine> sPrimitiveBasic::wireFrameShape = {};

void sPrimitiveBasic::InitPrimitiveWireframeShapes()
{
	sPrimitivePlane::InitPrimitiveWireframeShape();
	sPrimitiveBox::InitPrimitiveWireframeShape();
	sPrimitiveSphere::InitPrimitiveWireframeShape();
	sPrimitiveWater::InitPrimitiveWireframeShape();
	sPrimitiveCone::InitPrimitiveWireframeShape();
	sPrimitiveCylinder::InitPrimitiveWireframeShape();
	sPrimitiveTorus::InitPrimitiveWireframeShape();
	sPrimitiveCircle::InitPrimitiveWireframeShape();
	sPrimitiveRectangle::InitPrimitiveWireframeShape();
	sPrimitivePrism::InitPrimitiveWireframeShape();
}

sPrimitivePlane::sPrimitivePlane(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
		: sPrimitiveBasic(fullName, par)
{
	empty = par->Get<bool>(fullName + "_empty");
	size = CVector3(1.0, 1.0, 1.0);
}
sPrimitiveBasic::tWireframeShape sPrimitivePlane::wireFrameShape = {};

void sPrimitivePlane::InitPrimitiveWireframeShape()
{
	double meshSize = 1.0;

	for (int i = 0; i < wireframeSegments; i++)
	{
		double delta = (i - wireframeSegments / 2) * meshSize / (wireframeSegments / 2.0);
		wireFrameShape.push_back({{-meshSize, delta, 0.0}, {meshSize, delta, 0.0}});
		wireFrameShape.push_back({{-meshSize, -delta, 0.0}, {meshSize, -delta, 0.0}});
		wireFrameShape.push_back({{delta, -meshSize, 0.0}, {delta, meshSize, 0.0}});
		wireFrameShape.push_back({{-delta, -meshSize, 0.0}, {-delta, meshSize, 0.0}});
	}
}

sPrimitiveBox::sPrimitiveBox(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
		: sPrimitiveBasic(fullName, par)
{
	empty = par->Get<bool>(fullName + "_empty");
	rounding = par->Get<double>(fullName + "_rounding");
	repeat = par->Get<CVector3>(fullName + "_repeat");
	size = par->Get<CVector3>(fullName + "_size");
	limitsEnable = par->Get<bool>(fullName + "_limits_enable");
	limitsMax = par->Get<CVector3>(fullName + "_limits_max");
	limitsMin = par->Get<CVector3>(fullName + "_limits_min");
}
sPrimitiveBasic::tWireframeShape sPrimitiveBox::wireFrameShape = {};

void sPrimitiveBox::InitPrimitiveWireframeShape()
{
	wireFrameShape = {
		{{-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}},
		{{-0.5, -0.5, -0.5}, {-0.5, 0.5, -0.5}},
		{{0.5, 0.5, -0.5}, {0.5, -0.5, -0.5}},
		{{0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5}},

		{{-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}},
		{{-0.5, -0.5, 0.5}, {-0.5, 0.5, 0.5}},
		{{0.5, 0.5, 0.5}, {0.5, -0.5, 0.5}},
		{{0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5}},

		{{-0.5, -0.5, -0.5}, {-0.5, -0.5, 0.5}},
		{{-0.5, 0.5, -0.5}, {-0.5, 0.5, 0.5}},
		{{0.5, -0.5, -0.5}, {0.5, -0.5, 0.5}},
		{{0.5, 0.5, -0.5}, {0.5, 0.5, 0.5}},

		{{0.1, 0.0, 0.0}, {-0.1, 0.0, 0.0}},
		{{0.0, 0.1, 0.0}, {0.0, -0.1, 0.0}},
		{{0.0, 0.0, 0.1}, {0.0, 0.0, -0.1}},
	};
}

sPrimitiveSphere::sPrimitiveSphere(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
		: sPrimitiveBasic(fullName, par)
{
	empty = par->Get<bool>(fullName + "_empty");
	radius = par->Get<double>(fullName + "_radius");
	repeat = par->Get<CVector3>(fullName + "_repeat");
	size = CVector3(radius * 2.0, radius * 2.0, radius * 2.0);
	limitsEnable = par->Get<bool>(fullName + "_limits_enable");
	limitsMax = par->Get<CVector3>(fullName + "_limits_max");
	limitsMin = par->Get<CVector3>(fullName + "_limits_min");
}

sPrimitiveBasic::tWireframeShape sPrimitiveSphere::wireFrameShape = {};

void sPrimitiveSphere::InitPrimitiveWireframeShape()
{
	double r = 0.5;
	double angleStep = 2.0 * M_PI / wireframeSegments;
	for (double alpha = -M_PI * 0.5; alpha < M_PI * 0.5; alpha += angleStep)
	{
		for (double beta = 0.0; beta < 2.0 * M_PI; beta += angleStep)
		{
			double z1 = sin(alpha) * r;
			double r1 = cos(alpha) * r;
			double x1 = r1 * cos(beta);
			double y1 = r1 * sin(beta);
			double x2 = r1 * cos(beta + angleStep);
			double y2 = r1 * sin(beta + angleStep);
			wireFrameShape.push_back({{x1, y1, z1}, {x2, y2, z1}});
			double z3 = sin(alpha + angleStep) * r;
			double r3 = cos(alpha + angleStep) * r;
			double x3 = r3 * cos(beta);
			double y3 = r3 * sin(beta);
			wireFrameShape.push_back({{x1, y1, z1}, {x3, y3, z3}});
		}
	}

	wireFrameShape.push_back({{0.1, 0.0, 0.0}, {-0.1, 0.0, 0.0}});
	wireFrameShape.push_back({{0.0, 0.1, 0.0}, {0.0, -0.1, 0.0}});
	wireFrameShape.push_back({{0.0, 0.0, 0.1}, {0.0, 0.0, -0.1}});
}

sPrimitiveWater::sPrimitiveWater(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
		: sPrimitiveBasic(fullName, par)
{
	empty = par->Get<bool>(fullName + "_empty");
	relativeAmplitude = par->Get<double>(fullName + "_relative_amplitude");
	length = par->Get<double>(fullName + "_length");
	animSpeed = par->Get<double>(fullName + "_anim_speed");
	animProgressionSpeed = par->Get<double>(fullName + "_anim_progression_speed");
	iterations = par->Get<int>(fullName + "_iterations");
	waveFromObjectsEnable = par->Get<bool>(fullName + "_wave_from_objects_enable");
	waveFromObjectsRelativeAmplitude =
		par->Get<double>(fullName + "_wave_from_objects_relative_amplitude");
	animFrame = par->Get<int>("frame_no");
	size = CVector3(length * 10.0, length * 10.0, length * relativeAmplitude);
	limitsEnable = par->Get<bool>(fullName + "_limits_enable");
	limitsMax = par->Get<CVector3>(fullName + "_limits_max");
	limitsMin = par->Get<CVector3>(fullName + "_limits_min");
}

sPrimitiveBasic::tWireframeShape sPrimitiveWater::wireFrameShape = {};

void sPrimitiveWater::InitPrimitiveWireframeShape()
{
	double meshSize = 1.0;

	for (int mx = 0; mx < wireframeSegments; mx++)
	{
		double deltaX1 = (mx - wireframeSegments / 2) * meshSize / (wireframeSegments / 2.0);
		double deltaX2 = (mx + 1 - wireframeSegments / 2) * meshSize / (wireframeSegments / 2.0);

		for (int my = 0; my < wireframeSegments; my++)
		{
			double deltaY1 = (my - wireframeSegments / 2) * meshSize / (wireframeSegments / 2.0);
			double deltaY2 = (my + 1 - wireframeSegments / 2) * meshSize / (wireframeSegments / 2.0);
			{
				auto wave = [](double x, double y) { return sin(x * 23.0) + cos(y * 4.3); };

				double z1 = wave(deltaX1, deltaY1);
				double z2 = wave(deltaX2, deltaY1);
				double z3 = wave(deltaX1, deltaY2);

				wireFrameShape.push_back({{deltaX1, deltaY1, z1}, {deltaX2, deltaY1, z2}});
				wireFrameShape.push_back({{deltaX1, deltaY1, z1}, {deltaX1, deltaY2, z3}});
			}
		}
	}
}

sPrimitiveCone::sPrimitiveCone(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
		: sPrimitiveBasic(fullName, par)
{
	caps = par->Get<bool>(fullName + "_caps");
	empty = par->Get<bool>(fullName + "_empty");
	radius = par->Get<double>(fullName + "_radius");
	height = par->Get<double>(fullName + "_height");
	repeat = par->Get<CVector3>(fullName + "_repeat");
	wallNormal = CVector2<double>(1.0, radius / height);
	wallNormal.Normalize();
	size = CVector3(radius * 2.0, radius * 2.0, height);
	limitsEnable = par->Get<bool>(fullName + "_limits_enable");
	limitsMax = par->Get<CVector3>(fullName + "_limits_max");
	limitsMin = par->Get<CVector3>(fullName + "_limits_min");
}

sPrimitiveBasic::tWireframeShape sPrimitiveCone::wireFrameShape = {};

void sPrimitiveCone::InitPrimitiveWireframeShape()
{
	double r = 0.5;
	double angleStep = 2.0 * M_PI / wireframeSegments;

	for (double alpha = 0.0; alpha < 2.0 * M_PI; alpha += angleStep)
	{
		double z1 = 0.0;
		double z2 = 1.0;
		double x1 = cos(alpha) * r;
		double y1 = sin(alpha) * r;
		double x2 = cos(alpha + angleStep) * r;
		double y2 = sin(alpha + angleStep) * r;
		wireFrameShape.push_back({{x1, y1, z1}, {0.0, 0.0, z2}});
		wireFrameShape.push_back({{x1, y1, z1}, {x2, y2, z1}});
		wireFrameShape.push_back({{x1, y1, z1}, {0.0, 0.0, z1}});
	}
}

sPrimitiveCylinder::sPrimitiveCylinder(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
		: sPrimitiveBasic(fullName, par)
{
	caps = par->Get<bool>(fullName + "_caps");
	empty = par->Get<bool>(fullName + "_empty");
	radius = par->Get<double>(fullName + "_radius");
	height = par->Get<double>(fullName + "_height");
	repeat = par->Get<CVector3>(fullName + "_repeat");
	size = CVector3(radius * 2.0, radius * 2.0, height);
	limitsEnable = par->Get<bool>(fullName + "_limits_enable");
	limitsMax = par->Get<CVector3>(fullName + "_limits_max");
	limitsMin = par->Get<CVector3>(fullName + "_limits_min");
}

sPrimitiveBasic::tWireframeShape sPrimitiveCylinder::wireFrameShape = {};

void sPrimitiveCylinder::InitPrimitiveWireframeShape()
{
	double r = 0.5;
	double angleStep = 2.0 * M_PI / wireframeSegments;

	for (double alpha = 0.0; alpha < 2.0 * M_PI; alpha += angleStep)
	{
		double z1 = -0.5;
		double z2 = 0.5;
		double x1 = cos(alpha) * r;
		double y1 = sin(alpha) * r;
		double x2 = cos(alpha + angleStep) * r;
		double y2 = sin(alpha + angleStep) * r;
		wireFrameShape.push_back({{x1, y1, z1}, {x1, y1, z2}});
		wireFrameShape.push_back({{x1, y1, z1}, {x2, y2, z1}});
		wireFrameShape.push_back({{x1, y1, z2}, {x2, y2, z2}});
		wireFrameShape.push_back({{x1, y1, z1}, {0.0, 0.0, z1}});
		wireFrameShape.push_back({{x1, y1, z2}, {0.0, 0.0, z2}});
	}

	wireFrameShape.push_back({{0.1, 0.0, 0.0}, {-0.1, 0.0, 0.0}});
	wireFrameShape.push_back({{0.0, 0.1, 0.0}, {0.0, -0.1, 0.0}});
	wireFrameShape.push_back({{0.0, 0.0, 0.1}, {0.0, 0.0, -0.1}});
}

sPrimitiveTorus::sPrimitiveTorus(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
		: sPrimitiveBasic(fullName, par)
{
	empty = par->Get<bool>(fullName + "_empty");
	radius = par->Get<double>(fullName + "_radius");
	radiusLPow = par->Get<double>(fullName + "_radius_lpow");
	tubeRadius = par->Get<double>(fullName + "_tube_radius");
	tubeRadiusLPow = par->Get<double>(fullName + "_tube_radius_lpow");
	repeat = par->Get<CVector3>(fullName + "_repeat");
	size = CVector3((radius + tubeRadius) * 2.0, (radius + tubeRadius) * 2.0, tubeRadius);
	limitsEnable = par->Get<bool>(fullName + "_limits_enable");
	limitsMax = par->Get<CVector3>(fullName + "_limits_max");
	limitsMin = par->Get<CVector3>(fullName + "_limits_min");
}

sPrimitiveBasic::tWireframeShape sPrimitiveTorus::wireFrameShape = {};

void sPrimitiveTorus::InitPrimitiveWireframeShape()
{
	// just cross in the center
	wireFrameShape.push_back({{0.1, 0.0, 0.0}, {-0.1, 0.0, 0.0}});
	wireFrameShape.push_back({{0.0, 0.1, 0.0}, {0.0, -0.1, 0.0}});
	wireFrameShape.push_back({{0.0, 0.0, 0.1}, {0.0, 0.0, -0.1}});
}

sPrimitiveRectangle::sPrimitiveRectangle(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
		: sPrimitiveBasic(fullName, par)
{
	height = par->Get<double>(fullName + "_height");
	width = par->Get<double>(fullName + "_width");
	size = CVector3(width, height, 1.0);
}

sPrimitiveBasic::tWireframeShape sPrimitiveRectangle::wireFrameShape = {};

void sPrimitiveRectangle::InitPrimitiveWireframeShape()
{
	wireFrameShape = {
		{{-0.5, -0.5, 0.0}, {0.5, -0.5, 0.0}},
		{{0.5, -0.5, 0.0}, {0.5, 0.5, 0.0}},
		{{0.5, 0.5, 0.0}, {-0.5, 0.5, 0.0}},
		{{-0.5, 0.5, 0.0}, {-0.5, -0.5, 0.0}},

		{{0.1, 0.0, 0.0}, {-0.1, 0.0, 0.0}},
		{{0.0, 0.1, 0.0}, {0.0, -0.1, 0.0}},
		{{0.0, 0.0, 0.1}, {0.0, 0.0, -0.1}},
	};
}

sPrimitiveCircle::sPrimitiveCircle(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
		: sPrimitiveBasic(fullName, par)
{
	radius = par->Get<double>(fullName + "_radius");
	size = CVector3(radius * 2.0, radius * 2.0, 1.0);
}

sPrimitiveBasic::tWireframeShape sPrimitiveCircle::wireFrameShape = {};

void sPrimitiveCircle::InitPrimitiveWireframeShape()
{
	double r = 0.5;
	double angleStep = 2.0 * M_PI / wireframeSegments;

	for (double alpha = 0.0; alpha < 2.0 * M_PI; alpha += angleStep)
	{
		double z = 0.0;
		double x1 = cos(alpha) * r;
		double y1 = sin(alpha) * r;
		double x2 = cos(alpha + angleStep) * r;
		double y2 = sin(alpha + angleStep) * r;
		wireFrameShape.push_back({{x1, y1, z}, {x2, y2, z}});
		wireFrameShape.push_back({{x1, y1, z}, {0.0, 0.0, z}});
	}
}

sPrimitivePrism::sPrimitivePrism(
	const QString &fullName, const std::shared_ptr<cParameterContainer> par)
		: sPrimitiveBasic(fullName, par)
{
	empty = par->Get<bool>(fullName + "_empty");
	height = par->Get<double>(fullName + "_height") / 2.0;
	prismAngle = par->Get<double>(fullName + "_prism_angle") * M_PI / 180.0 / 2.0;
	triangleHeight = par->Get<double>(fullName + "_trangle_height") * sin(prismAngle);
	repeat = par->Get<CVector3>(fullName + "_repeat");
	normals = CVector3(sin(prismAngle), cos(prismAngle), sin(prismAngle));
	size =
		CVector3(tan(prismAngle / 2.0) * triangleHeight * 4.0 / normals.x, triangleHeight / normals.x,
			height * 2.0); // FIXME correct size
}
sPrimitiveBasic::tWireframeShape sPrimitivePrism::wireFrameShape = {};

void sPrimitivePrism::InitPrimitiveWireframeShape()
{
	wireFrameShape = {
		{{0.0, 1.0, -0.5}, {0.0, 1.0, 0.5}},
		{{-0.5, 0.0, -0.5}, {-0.5, 0.0, 0.5}},
		{{0.5, 0.0, -0.5}, {0.5, 0.0, 0.5}},

		{{-0.5, 0.0, -0.5}, {0.0, 1.0, -0.5}},
		{{0.5, 0.0, -0.5}, {0.0, 1.0, -0.5}},
		{{-0.5, 0.0, -0.5}, {0.5, 0.0, -0.5}},

		{{-0.5, 0.0, 0.5}, {0.0, 1.0, 0.5}},
		{{0.5, 0.0, 0.5}, {0.0, 1.0, 0.5}},
		{{-0.5, 0.0, 0.5}, {0.5, 0.0, 0.5}},

		{{0.1, 0.0, 0.0}, {-0.1, 0.0, 0.0}},
		{{0.0, 0.1, 0.0}, {0.0, -0.1, 0.0}},
		{{0.0, 0.0, 0.1}, {0.0, 0.0, -0.1}},
	};
}

double sPrimitivePlane::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	double dist = point.z;
	return empty ? fabs(dist) : dist;
}

double sPrimitiveBox::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	point = point.repeatMod(repeat);

	double boxDist = -1e10;

	if (empty)
	{
		boxDist = max(fabs(point.x) - size.x * 0.5, boxDist);
		boxDist = max(fabs(point.y) - size.y * 0.5, boxDist);
		boxDist = max(fabs(point.z) - size.z * 0.5, boxDist);
		boxDist = fabs(boxDist);
	}
	else
	{
		CVector3 boxTemp;
		boxTemp.x = max(fabs(point.x) - size.x * 0.5, 0.0);
		boxTemp.y = max(fabs(point.y) - size.y * 0.5, 0.0);
		boxTemp.z = max(fabs(point.z) - size.z * 0.5, 0.0);
		boxDist = boxTemp.Length() - rounding;
	}

	if (limitsEnable)
	{
		CVector3 distanceAxial = max(point - limitsMax, limitsMin - point);
		double limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		boxDist = max(boxDist, limitBoxDist);
	}
	return boxDist;
}

double sPrimitiveSphere::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	point = point.repeatMod(repeat);
	double dist = point.Length() - radius;
	dist = empty ? fabs(dist) : dist;
	if (limitsEnable)
	{
		CVector3 distanceAxial = max(point - limitsMax, limitsMin - point);
		double limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		dist = max(dist, limitBoxDist);
	}
	return dist;
}

double sPrimitiveRectangle::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	CVector3 boxTemp;
	boxTemp.x = max(fabs(point.x) - width * 0.5, 0.0);
	boxTemp.y = max(fabs(point.y) - height * 0.5, 0.0);
	boxTemp.z = fabs(point.z);
	return boxTemp.Length();
}

double sPrimitiveCylinder::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	point = point.repeatMod(repeat);

	CVector2<double> cylTemp(point.x, point.y);
	double dist = cylTemp.Length() - radius;
	if (!caps) dist = fabs(dist);
	dist = max(fabs(point.z) - height * 0.5, dist);
	dist = empty ? fabs(dist) : dist;
	if (limitsEnable)
	{
		CVector3 distanceAxial = max(point - limitsMax, limitsMin - point);
		double limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		dist = max(dist, limitBoxDist);
	}
	return dist;
}

double sPrimitiveCircle::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	CVector2<double> circleTemp(point.x, point.y);
	double distTemp = circleTemp.Length() - radius;
	distTemp = max(fabs(point.z), distTemp);
	return distTemp;
}

double sPrimitiveCone::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	point = point.repeatMod(repeat);

	point.z -= height;
	double q = sqrt(point.x * point.x + point.y * point.y);
	CVector2<double> vect(q, point.z);
	double dist = wallNormal.Dot(vect);
	if (!caps) dist = fabs(dist);
	dist = max(-point.z - height, dist);
	dist = empty ? fabs(dist) : dist;
	if (limitsEnable)
	{
		CVector3 distanceAxial = max(point - limitsMax, limitsMin - point);
		double limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		dist = max(dist, limitBoxDist);
	}
	return dist;
}

double sPrimitiveWater::PrimitiveDistance(CVector3 _point) const
{
	Q_UNUSED(_point);
	return 0.0;
}

double sPrimitiveWater::PrimitiveDistanceWater(CVector3 _point, double distanceFromAnother) const
{
	// TODO to use rendering technique from here: //https://www.shadertoy.com/view/Ms2SD1

	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);

	if (waveFromObjectsEnable)
	{
		point.x +=
			length * 20.0 * exp(-distanceFromAnother / length / 5.0) * waveFromObjectsRelativeAmplitude;
	}

	double amplitude = length * relativeAmplitude;
	double objectWave = 0.0;

	double planeDistance = point.z;
	if (planeDistance < amplitude * 10.0)
	{
		double phase = -animSpeed * animFrame * 0.1;

		if (waveFromObjectsEnable)
		{
			objectWave = sin(distanceFromAnother / length * 5.0 - phase * 2.0)
									 * exp(-distanceFromAnother / length / 5.0) * waveFromObjectsRelativeAmplitude;
		}

		double k = 0.23;
		double waveXTemp;
		double waveYTemp;
		double waveX = objectWave;
		double waveY = -objectWave;
		double p = 1.0;
		double p2 = 0.05;

		point.x += phase * 0.05
							 * (animProgressionSpeed * 3.0
									- 3.0); // 3.0 to keep compatibility and to have 0.0 stopping moving

		for (int i = 1; i <= iterations; i++)
		{
			float p3 = p * p2;
			float shift = phase / (i / 2.697 + 1.0);
			waveXTemp = sin(i + 0.4 * (waveX)*p3 + sin(k * point.y / length * p3 + shift * 0.134 * p3)
											+ point.x / length * p3 + shift * p3)
									/ p;
			waveYTemp = cos(i + 0.4 * (waveY)*p3 + sin(point.x / length * p3 + shift * 0.0179 * p3)
											+ k * point.y / length * p3 + shift * 0.023 * p3)
									/ p;
			waveX += waveXTemp;
			waveY += waveYTemp;
			p2 = p2 + (1.0 - p2 + 0.5 * sin(shift * 0.0323)) * 0.7;
			p *= 1.872;
		}

		planeDistance += (waveX + waveY) * amplitude;
	}

	planeDistance = empty ? fabs(planeDistance) : planeDistance;
	if (limitsEnable)
	{
		CVector3 distanceAxial = max(point - limitsMax, limitsMin - point);
		double limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		planeDistance = max(planeDistance, limitBoxDist);
	}
	return planeDistance;
}

double sPrimitiveTorus::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	point = point.repeatMod(repeat);

	double d1 = CVector2<double>(point.x, point.y).LengthPow(pow(2, radiusLPow)) - radius;
	double dist = CVector2<double>(d1, point.z).LengthPow(pow(2, tubeRadiusLPow)) - tubeRadius;

	dist = empty ? fabs(dist) : dist;
	if (limitsEnable)
	{
		CVector3 distanceAxial = max(point - limitsMax, limitsMin - point);
		double limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		dist = max(dist, limitBoxDist);
	}
	return dist;
}

double sPrimitivePrism::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	point = point.repeatMod(repeat);

	CVector3 q = fabs(point);

	double dist = max(q.z - height,
		max(q.x * normals.y + point.y * normals.z, -point.y + triangleHeight) - triangleHeight);

	return empty ? fabs(dist) : dist;
}
