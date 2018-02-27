/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "primitives.h"

#include <QtAlgorithms>

#include "displacement_map.hpp"
#include "parameters.hpp"
#include "system.hpp"

using namespace fractal;

QString PrimitiveNames(enumObjectType primitiveType)
{
	switch (primitiveType)
	{
		case objPlane: return "plane";
		case objWater: return "water";
		case objSphere: return "sphere";
		case objBox: return "box";
		case objRectangle: return "rectangle";
		case objCircle: return "circle";
		case objCone: return "cone";
		case objCylinder: return "cylinder";
		case objTorus: return "torus";
		default: return "";
	}
}

enumObjectType PrimitiveNameToEnum(const QString &primitiveType)
{
	enumObjectType type = objNone;
	if (primitiveType == QString("plane"))
		type = objPlane;
	else if (primitiveType == QString("water"))
		type = objWater;
	else if (primitiveType == QString("sphere"))
		type = objSphere;
	else if (primitiveType == QString("box"))
		type = objBox;
	else if (primitiveType == QString("rectangle"))
		type = objRectangle;
	else if (primitiveType == QString("circle"))
		type = objCircle;
	else if (primitiveType == QString("cone"))
		type = objCone;
	else if (primitiveType == QString("cylinder"))
		type = objCylinder;
	else if (primitiveType == QString("torus"))
		type = objTorus;
	else
		qCritical() << "Wrong primitive name: " << primitiveType;

	return type;
}

cPrimitives::cPrimitives(const cParameterContainer *par, QVector<cObjectData> *objectData)
{
	WriteLog("cPrimitives::cPrimitives(const cParameterContainer *par) started", 2);
	isAnyPrimitive = false;

	QList<QString> listOfParameters = par->GetListOfParameters();
	QList<sPrimitiveItem> listOfPrimitives;

	// generating fresh list of primitives based of parameter list
	for (int i = 0; i < listOfParameters.size(); i++)
	{
		QString parameterName = listOfParameters.at(i);
		if (parameterName.left(9) == "primitive")
		{
			isAnyPrimitive = true;

			QStringList split = parameterName.split('_');
			QString primitiveName = split.at(0) + "_" + split.at(1) + "_" + split.at(2);
			QString typeName = split.at(1);
			fractal::enumObjectType type = PrimitiveNameToEnum(typeName);
			int index = split.at(2).toInt();

			// check if item is already on the list
			bool found = false;
			for (int l = 0; l < listOfPrimitives.size(); l++)
			{
				if (listOfPrimitives.at(l).id == index && listOfPrimitives.at(l).type == type)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				listOfPrimitives.append(sPrimitiveItem(type, index, primitiveName));
			}
		}
	}

	for (int i = 0; i < listOfPrimitives.size(); i++)
	{
		sPrimitiveItem item = listOfPrimitives.at(i);

		using namespace fractal;
		sPrimitiveBasic *primitive;

		switch (item.type)
		{
			case objPlane:
			{
				primitive = new sPrimitivePlane;
				sPrimitivePlane *obj = static_cast<sPrimitivePlane *>(primitive);
				obj->empty = par->Get<bool>(item.name + "_empty");
				obj->size = CVector3(1.0, 1.0, 1.0);
				break;
			}
			case objBox:
			{
				primitive = new sPrimitiveBox;
				sPrimitiveBox *obj = static_cast<sPrimitiveBox *>(primitive);
				obj->empty = par->Get<bool>(item.name + "_empty");
				obj->rounding = par->Get<double>(item.name + "_rounding");
				obj->repeat = par->Get<CVector3>(item.name + "_repeat");
				obj->size = par->Get<CVector3>(item.name + "_size");
				break;
			}
			case objSphere:
			{
				primitive = new sPrimitiveSphere;
				sPrimitiveSphere *obj = static_cast<sPrimitiveSphere *>(primitive);
				obj->empty = par->Get<bool>(item.name + "_empty");
				obj->radius = par->Get<double>(item.name + "_radius");
				obj->repeat = par->Get<CVector3>(item.name + "_repeat");
				obj->size = CVector3(obj->radius * 2.0, obj->radius * 2.0, obj->radius * 2.0);
				break;
			}
			case objWater:
			{
				primitive = new sPrimitiveWater;
				sPrimitiveWater *obj = static_cast<sPrimitiveWater *>(primitive);
				obj->empty = par->Get<bool>(item.name + "_empty");
				obj->relativeAmplitude = par->Get<double>(item.name + "_relative_amplitude");
				obj->length = par->Get<double>(item.name + "_length");
				obj->animSpeed = par->Get<double>(item.name + "_anim_speed");
				obj->iterations = par->Get<int>(item.name + "_iterations");
				obj->waveFromObjectsEnable = par->Get<bool>(item.name + "_wave_from_objects_enable");
				obj->waveFromObjectsRelativeAmplitude =
					par->Get<double>(item.name + "_wave_from_objects_relative_amplitude");
				obj->animFrame = par->Get<int>("frame_no");
				obj->size = CVector3(1.0, 1.0, 1.0);
				break;
			}
			case objCone:
			{
				primitive = new sPrimitiveCone;
				sPrimitiveCone *obj = static_cast<sPrimitiveCone *>(primitive);
				obj->caps = par->Get<bool>(item.name + "_caps");
				obj->empty = par->Get<bool>(item.name + "_empty");
				obj->radius = par->Get<double>(item.name + "_radius");
				obj->height = par->Get<double>(item.name + "_height");
				obj->repeat = par->Get<CVector3>(item.name + "_repeat");
				obj->wallNormal = CVector2<double>(1.0, obj->radius / obj->height);
				obj->wallNormal.Normalize();
				obj->size = CVector3(obj->radius * 2.0, obj->radius * 2.0, obj->height);
				break;
			}
			case objCylinder:
			{
				primitive = new sPrimitiveCylinder;
				sPrimitiveCylinder *obj = static_cast<sPrimitiveCylinder *>(primitive);
				obj->caps = par->Get<bool>(item.name + "_caps");
				obj->empty = par->Get<bool>(item.name + "_empty");
				obj->radius = par->Get<double>(item.name + "_radius");
				obj->height = par->Get<double>(item.name + "_height");
				obj->repeat = par->Get<CVector3>(item.name + "_repeat");
				obj->size = CVector3(obj->radius * 2.0, obj->radius * 2.0, obj->height);
				break;
			}
			case objTorus:
			{
				primitive = new sPrimitiveTorus;
				sPrimitiveTorus *obj = static_cast<sPrimitiveTorus *>(primitive);
				obj->empty = par->Get<bool>(item.name + "_empty");
				obj->radius = par->Get<double>(item.name + "_radius");
				obj->radius_lpow = par->Get<double>(item.name + "_radius_lpow");
				obj->tube_radius = par->Get<double>(item.name + "_tube_radius");
				obj->tube_radius_lpow = par->Get<double>(item.name + "_tube_radius_lpow");
				obj->repeat = par->Get<CVector3>(item.name + "_repeat");
				obj->size = CVector3((obj->radius + obj->tube_radius) * 2.0,
					(obj->radius + obj->tube_radius) * 2.0, obj->tube_radius);
				break;
			}
			case objCircle:
			{
				primitive = new sPrimitiveCircle;
				sPrimitiveCircle *obj = static_cast<sPrimitiveCircle *>(primitive);
				obj->radius = par->Get<double>(item.name + "_radius");
				obj->size = CVector3(obj->radius * 2.0, obj->radius * 2.0, 1.0);
				break;
			}
			case objRectangle:
			{
				primitive = new sPrimitiveRectangle;
				sPrimitiveRectangle *obj = static_cast<sPrimitiveRectangle *>(primitive);
				obj->height = par->Get<double>(item.name + "_height");
				obj->width = par->Get<double>(item.name + "_width");
				obj->size = CVector3(obj->width, obj->height, 1.0);
				break;
			}
			default:
			{
				qCritical() << "cannot handle " << PrimitiveNames(item.type)
										<< " in cPrimitives::cPrimitives()";
				return;
			}
		}

		// set parameters, which all primitives have in common
		primitive->position = par->Get<CVector3>(item.name + "_position");
		primitive->materialId = par->Get<int>(item.name + "_material_id");
		primitive->objectType = item.type;
		primitive->SetRotation(par->Get<CVector3>(item.name + "_rotation"));
		primitive->enable = par->Get<bool>(item.name + "_enabled");
		// primitive->reflect = par->Get<double>(item.name + "_reflection");
		// primitive->color = par->Get<sRGB>(item.name + "_color");

		if (objectData)
		{
			objectData->append(*primitive);
			primitive->objectId = objectData->size() - 1;
		}
		allPrimitives.append(primitive);
	}

	allPrimitivesPosition = par->Get<CVector3>("all_primitives_position");
	allPrimitivesRotation = par->Get<CVector3>("all_primitives_rotation");
	mRotAllPrimitivesRotation.SetRotation2(allPrimitivesRotation / 180.0 * M_PI);

	WriteLog("cPrimitives::cPrimitives(const cParameterContainer *par) finished", 2);
}

cPrimitives::~cPrimitives()
{
	qDeleteAll(allPrimitives);
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
	point = point.mod(repeat);

	if (empty)
	{
		double boxDist = -1e10;
		boxDist = max(fabs(point.x) - size.x * 0.5, boxDist);
		boxDist = max(fabs(point.y) - size.y * 0.5, boxDist);
		boxDist = max(fabs(point.z) - size.z * 0.5, boxDist);
		return fabs(boxDist);
	}
	else
	{
		CVector3 boxTemp;
		boxTemp.x = max(fabs(point.x) - size.x * 0.5, 0.0);
		boxTemp.y = max(fabs(point.y) - size.y * 0.5, 0.0);
		boxTemp.z = max(fabs(point.z) - size.z * 0.5, 0.0);
		return boxTemp.Length() - rounding;
	}
}

double sPrimitiveSphere::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	point = point.mod(repeat);
	double dist = point.Length() - radius;
	return empty ? fabs(dist) : dist;
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
	point = point.mod(repeat);

	CVector2<double> cylTemp(point.x, point.y);
	double dist = cylTemp.Length() - radius;
	if (!caps) dist = fabs(dist);
	dist = max(fabs(point.z) - height * 0.5, dist);
	return empty ? fabs(dist) : dist;
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
	point = point.mod(repeat);

	point.z -= height;
	float q = sqrt(point.x * point.x + point.y * point.y);
	CVector2<double> vect(q, point.z);
	double dist = wallNormal.Dot(vect);
	if (!caps) dist = fabs(dist);
	dist = max(-point.z - height, dist);
	return empty ? fabs(dist) : dist;
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
		double phase = animSpeed * animFrame * 0.1;

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
		for (int i = 1; i <= iterations; i++)
		{
			float p3 = p * p2;
			double shift = phase / (i / 3.0 + 1.0);
			waveXTemp =
				sin(i + 0.4 * (waveX)*p3 + sin(k * point.y / length * p3) + point.x / length * p3 + shift)
				/ p;
			waveYTemp = cos(i + 0.4 * (waveY)*p3 + sin(point.x / length * p3) + k * point.y / length * p3
											+ shift * 0.23)
									/ p;
			waveX += waveXTemp;
			waveY += waveYTemp;
			p2 = p2 + (1.0 - p2) * 0.7;
			p *= 1.872;
		}

		planeDistance += (waveX + waveY) * amplitude;
	}
	return empty ? fabs(planeDistance) : planeDistance;
}

double sPrimitiveTorus::PrimitiveDistance(CVector3 _point) const
{
	CVector3 point = _point - position;
	point = rotationMatrix.RotateVector(point);
	point = point.mod(repeat);

	double d1 = CVector2<double>(point.x, point.y).LengthPow(pow(2, radius_lpow)) - radius;
	double dist = CVector2<double>(d1, point.z).LengthPow(pow(2, tube_radius_lpow)) - tube_radius;
	return empty ? fabs(dist) : dist;
}

double cPrimitives::TotalDistance(
	CVector3 point, double fractalDistance, int *closestObjectId, sRenderData *data) const
{
	using namespace fractal;
	int closestObject = *closestObjectId;
	double distance = fractalDistance;

	if (isAnyPrimitive)
	{
		CVector3 point2 = point - allPrimitivesPosition;
		point2 = mRotAllPrimitivesRotation.RotateVector(point2);

		for (int i = 0; i < allPrimitives.size(); i++)
		{
			sPrimitiveBasic *primitive = allPrimitives[i];
			if (primitive->enable)
			{
				sPrimitiveWater *water = dynamic_cast<sPrimitiveWater *>(primitive);
				double distTemp;
				if (water)
				{
					distTemp = water->PrimitiveDistanceWater(point, distance);
				}
				else
				{
					distTemp = primitive->PrimitiveDistance(point);
				}
				distTemp = DisplacementMap(distTemp, point, primitive->objectId, data);
				if (distTemp < distance)
				{
					closestObject = primitive->objectId;
					// color = plane.color;
					// reflect = plane.reflect;
				}
				distance = min(distance, distTemp);
			}
		}

	} // if is any primitive

	*closestObjectId = closestObject;

	return distance;
}
