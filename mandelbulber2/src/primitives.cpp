/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * definitions of primitive objects
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


#include "primitives.h"
#include <math.h>
#include "system.hpp"
#include "common_math.h"

using namespace fractal;

QString PrimitiveNames(enumObjectType primitiveType)
{
	QString name;
	switch (primitiveType) {
		case objPlane:
			name = "plane";
			break;
		case objWater:
			name = "water";
			break;
		case objSphere:
			name = "sphere";
			break;
		case objBox:
			name = "box";
			break;
		case objRectangle:
			name = "rectangle";
			break;
		case objCircle:
			name = "circle";
			break;
		case objCone:
			name = "cone";
			break;
		case objCylinder:
			name = "cylinder";
			break;
		case objTorus:
			name = "torus";
			break;

		default:
			break;
	}
	return name;
}

enumObjectType PrimitiveNameToEnum(const QString &primitiveType)
{
	enumObjectType type = objNone;
	if(primitiveType == QString("plane"))
		type = objPlane;
	else if(primitiveType == QString("water"))
		type = objWater;
	else if(primitiveType == QString("sphere"))
		type = objSphere;
	else if(primitiveType == QString("box"))
		type = objBox;
	else if(primitiveType == QString("rectangle"))
		type = objRectangle;
	else if(primitiveType == QString("circle"))
		type = objCircle;
	else if(primitiveType == QString("cone"))
		type = objCone;
	else if(primitiveType == QString("cylinder"))
		type = objCylinder;
	else if(primitiveType == QString("torus"))
		type = objTorus;
	else
		qCritical() << "Wrong primitive name: " << primitiveType;

	return type;
}

cPrimitives::cPrimitives(const cParameterContainer *par)
{
	WriteLog("cPrimitives::cPrimitives(const cParameterContainer *par) started");

	isAnyPrimitive = false;

	QList<QString> listOfParameters = par->GetListOfParameters();
	QList<sPrimitiveItem> listOfPrimitives;

	//generating fresh list of primitives based of parameter list
	for(int i = 0; i < listOfParameters.size(); i++)
	{
		QString parameterName = listOfParameters.at(i);
		if(parameterName.left(9) == "primitive")
		{
			isAnyPrimitive = true;

			QString primitiveName = parameterName.left(parameterName.lastIndexOf('_'));
			QStringList split = parameterName.split('_');
			QString typeName = split.at(1);
			fractal::enumObjectType type = PrimitiveNameToEnum(typeName);
			int index = split.at(2).toInt();

			//check if item is already on the list
			bool found = false;
			for(int l = 0; l < listOfPrimitives.size(); l++)
			{
				if(listOfPrimitives.at(l).id == index && listOfPrimitives.at(l).type == type)
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				listOfPrimitives.append(sPrimitiveItem(type, index, primitiveName));
			}
		}
	}

	for(int i = 0; i < listOfPrimitives.size(); i++)
	{
		sPrimitiveItem item = listOfPrimitives.at(i);

		using namespace fractal;
		switch(item.type)
		{
			case objPlane:
			{
				sPrimitivePlane object;
				object.enable = par->Get<bool>(item.name + "_enabled");
				object.empty = par->Get<bool>(item.name + "_empty");
				object.position = par->Get<CVector3>(item.name + "_position");
				object.rotation = par->Get<CVector3>(item.name + "_rotation");
				object.reflect = par->Get<double>(item.name + "_reflection");
				object.color = par->Get<sRGB>(item.name + "_color");
				object.rotationMatrix.SetRotation2(object.rotation * M_PI / 180.0);
				planes.append(object);
				break;
			}
			case objBox:
			{
				sPrimitiveBox object;
				object.enable = par->Get<bool>(item.name + "_enabled");
				object.empty = par->Get<bool>(item.name + "_empty");
				object.position = par->Get<CVector3>(item.name + "_position");
				object.size = par->Get<CVector3>(item.name + "_size");
				object.rounding = par->Get<double>(item.name + "_rounding");
				object.rotation = par->Get<CVector3>(item.name + "_rotation");
				object.reflect = par->Get<double>(item.name + "_reflection");
				object.color = par->Get<sRGB>(item.name + "_color");
				object.repeat = par->Get<CVector3>(item.name + "_repeat");
				object.rotationMatrix.SetRotation2(object.rotation * M_PI / 180.0);
				boxes.append(object);
				break;
			}
			case objSphere:
			{
				sPrimitiveSphere object;
				object.enable = par->Get<bool>(item.name + "_enabled");
				object.empty = par->Get<bool>(item.name + "_empty");
				object.position = par->Get<CVector3>(item.name + "_position");
				object.radius = par->Get<double>(item.name + "_radius");
				object.rotation = par->Get<CVector3>(item.name + "_rotation");
				object.reflect = par->Get<double>(item.name + "_reflection");
				object.color = par->Get<sRGB>(item.name + "_color");
				object.repeat = par->Get<CVector3>(item.name + "_repeat");
				object.rotationMatrix.SetRotation2(object.rotation * M_PI / 180.0);
				spheres.append(object);
				break;
			}
			case objWater:
			{
				sPrimitiveWater object;
				object.enable = par->Get<bool>(item.name + "_enabled");
				object.empty = par->Get<bool>(item.name + "_empty");
				object.position = par->Get<CVector3>(item.name + "_position");
				object.rotation = par->Get<CVector3>(item.name + "_rotation");
				object.reflect = par->Get<double>(item.name + "_reflection");
				object.color = par->Get<sRGB>(item.name + "_color");
				object.amplitude = par->Get<double>(item.name + "_amplitude");
				object.length = par->Get<double>(item.name + "_length");
				object.animSpeed = par->Get<double>(item.name + "_anim_speed");
				object.iterations = par->Get<int>(item.name + "_iterations");
				object.animFrame = par->Get<int>("frame_no");
				object.rotationMatrix.SetRotation2(object.rotation * M_PI / 180.0);
				waters.append(object);
				break;
			}
			case objCone:
			{
				sPrimitiveCone object;
				object.enable = par->Get<bool>(item.name + "_enabled");
				object.caps = par->Get<bool>(item.name + "_caps");
				object.empty = par->Get<bool>(item.name + "_empty");
				object.position = par->Get<CVector3>(item.name + "_position");
				object.radius = par->Get<double>(item.name + "_radius");
				object.height = par->Get<double>(item.name + "_height");
				object.rotation = par->Get<CVector3>(item.name + "_rotation");
				object.reflect = par->Get<double>(item.name + "_reflection");
				object.color = par->Get<sRGB>(item.name + "_color");
				object.repeat = par->Get<CVector3>(item.name + "_repeat");
				object.wallNormal = CVector2<double>(1.0, object.radius/object.height);
				object.wallNormal.Normalize();
				object.rotationMatrix.SetRotation2(object.rotation * M_PI / 180.0);
				cones.append(object);
				break;
			}
			case objCylinder:
			{
				sPrimitiveCylinder object;
				object.enable = par->Get<bool>(item.name + "_enabled");
				object.caps = par->Get<bool>(item.name + "_caps");
				object.empty = par->Get<bool>(item.name + "_empty");
				object.position = par->Get<CVector3>(item.name + "_position");
				object.radius = par->Get<double>(item.name + "_radius");
				object.height = par->Get<double>(item.name + "_height");
				object.rotation = par->Get<CVector3>(item.name + "_rotation");
				object.reflect = par->Get<double>(item.name + "_reflection");
				object.color = par->Get<sRGB>(item.name + "_color");
				object.repeat = par->Get<CVector3>(item.name + "_repeat");
				object.rotationMatrix.SetRotation2(object.rotation * M_PI / 180.0);
				cylinders.append(object);
				break;
			}
			case objCircle:
			{
				sPrimitiveCircle object;
				object.enable = par->Get<bool>(item.name + "_enabled");
				object.position = par->Get<CVector3>(item.name + "_position");
				object.radius = par->Get<double>(item.name + "_radius");
				object.rotation = par->Get<CVector3>(item.name + "_rotation");
				object.reflect = par->Get<double>(item.name + "_reflection");
				object.color = par->Get<sRGB>(item.name + "_color");
				object.rotationMatrix.SetRotation2(object.rotation * M_PI / 180.0);
				circles.append(object);
				break;
			}
			case objRectangle:
			{
				sPrimitiveRectangle object;
				object.enable = par->Get<bool>(item.name + "_enabled");
				object.position = par->Get<CVector3>(item.name + "_position");
				object.height = par->Get<double>(item.name + "_height");
				object.width = par->Get<double>(item.name + "_width");
				object.rotation = par->Get<CVector3>(item.name + "_rotation");
				object.reflect = par->Get<double>(item.name + "_reflection");
				object.color = par->Get<sRGB>(item.name + "_color");
				object.rotationMatrix.SetRotation2(object.rotation * M_PI / 180.0);
				rectangles.append(object);
				break;
			}
			case objTorus:
			{
				sPrimitiveTorus object;
				object.enable = par->Get<bool>(item.name + "_enabled");
				object.empty = par->Get<bool>(item.name + "_empty");
				object.position = par->Get<CVector3>(item.name + "_position");
				object.radius = par->Get<double>(item.name + "_radius");
				object.tube_radius = par->Get<double>(item.name + "_tube_radius");
				object.rotation = par->Get<CVector3>(item.name + "_rotation");
				object.reflect = par->Get<double>(item.name + "_reflection");
				object.color = par->Get<sRGB>(item.name + "_color");
				object.repeat = par->Get<CVector3>(item.name + "_repeat");
				object.rotationMatrix.SetRotation2(object.rotation * M_PI / 180.0);
				toruses.append(object);
				break;
			}
			default:
				break;
		}
	}

	allPrimitivesPosition = par->Get<CVector3>("all_primitives_position");
	allPrimitivesRotation = par->Get<CVector3>("all_primitives_rotation");
	mRotAllPrimitivesRotation.SetRotation2(allPrimitivesRotation / 180.0 * M_PI);

	WriteLog("cPrimitives::cPrimitives(const cParameterContainer *par) finished");
}

double cPrimitives::PrimitivePlane(CVector3 _point, const sPrimitivePlane &plane) const
{
	CVector3 relativePoint = _point - plane.position;
	CVector3 point = plane.rotationMatrix.RotateVector(relativePoint);
	double dist = point.z;
	return plane.empty ? fabs(dist) : dist;
}

double cPrimitives::PrimitiveBox(CVector3 _point, const sPrimitiveBox &box) const
{
	CVector3 point = (_point - box.position);
	point = box.rotationMatrix.RotateVector(point);
	point = point.mod(box.repeat);
	if(box.empty)
	{
		double boxDist = -1e10;
		boxDist = max(fabs(point.x) - box.size.x * 0.5, boxDist);
		boxDist = max(fabs(point.y) - box.size.y * 0.5, boxDist);
		boxDist = max(fabs(point.z) - box.size.z * 0.5, boxDist);
		return fabs(boxDist);
	}
	else
	{
		CVector3 boxTemp;
		boxTemp.x = max(fabs(point.x) - box.size.x * 0.5, 0.0);
		boxTemp.y = max(fabs(point.y) - box.size.y * 0.5, 0.0);
		boxTemp.z = max(fabs(point.z) - box.size.z * 0.5, 0.0);
		return boxTemp.Length() - box.rounding;
	}
}

double cPrimitives::PrimitiveSphere(CVector3 _point, const sPrimitiveSphere &sphere) const
{
	//TODO add rotation for repeat operator
	double dist = (_point - sphere.position).mod(sphere.repeat ).Length() - sphere.radius;
	return sphere.empty ? fabs(dist) : dist;
}

double cPrimitives::PrimitiveRectangle(CVector3 _point, const sPrimitiveRectangle &rectangle) const
{
	CVector3 relativePoint = _point - rectangle.position;
	CVector3 point = rectangle.rotationMatrix.RotateVector(relativePoint);
	CVector3 boxTemp;
	boxTemp.x = max(fabs(point.x) - rectangle.height * 0.5, 0.0);
	boxTemp.y = max(fabs(point.y) - rectangle.width * 0.5, 0.0);
	boxTemp.z = fabs(point.z);
	return boxTemp.Length();
}

double cPrimitives::PrimitiveCylinder(CVector3 _point, const sPrimitiveCylinder &cylinder) const
{
	CVector3 point = (_point - cylinder.position);
	point = cylinder.rotationMatrix.RotateVector(point);
	point = point.mod(cylinder.repeat);

	CVector2<double> cylTemp(point.x, point.y);
	double dist = cylTemp.Length() - cylinder.radius;
	if(!cylinder.caps) dist = fabs(dist);
	dist = max(fabs(point.z) - cylinder.height*0.5, dist);
	return cylinder.empty ? fabs(dist) : dist;
}

double cPrimitives::PrimitiveCircle(CVector3 _point, const sPrimitiveCircle &circle) const
{
	CVector3 relativePoint = _point - circle.position;
	CVector3 point = circle.rotationMatrix.RotateVector(relativePoint);
	CVector2<double> cylTemp(point.x, point.y);
	double distTemp = cylTemp.Length() - circle.radius;
	distTemp = max(fabs(point.z), distTemp);
	return distTemp;
}

double cPrimitives::PrimitiveCone(CVector3 _point, const sPrimitiveCone &cone) const
{
	CVector3 point = (_point - cone.position);
	point = cone.rotationMatrix.RotateVector(point);
	point = point.mod(cone.repeat);

	point.z -= cone.height;
	float q = sqrt(point.x * point.x + point.y * point.y);
  CVector2<double> vect(q, point.z);
  double dist = cone.wallNormal.Dot(vect);
	if(!cone.caps) dist = fabs(dist);
  dist = max(-point.z - cone.height, dist);
  return cone.empty ? fabs(dist) : dist;
}

double cPrimitives::PrimitiveWater(CVector3 _point, const sPrimitiveWater &water) const
{
	//TODO to use rendering technique from here: //https://www.shadertoy.com/view/Ms2SD1

	CVector3 relativePoint = _point - water.position;
	CVector3 point = water.rotationMatrix.RotateVector(relativePoint);

	double planeDistance = point.z;
	if(planeDistance < water.amplitude * 10.0)
	{
		double phase = water.animSpeed * water.animFrame * 0.1;
		double k=0.23;
		double waveXtemp = point.x;
		double waveYtemp = point.y;
		double waveX = 0;
		double waveY = 0;
		double p = 1.0;
		double p2 = 0.05;
		for(int i=1; i<=water.iterations; i++)
		{
			float p3 = p * p2;
			double shift = phase / (i/3.0 + 1.0);
			waveXtemp = sin(i + 0.4*(waveX)*p3 + sin(k* point.y / water.length*p3) + point.x/water.length*p3 + shift)/p;
			waveYtemp = cos(i + 0.4*(waveY)*p3 + sin(point.x / water.length*p3) + k*point.y/water.length*p3 + shift*0.23)/p;
			waveX+=waveXtemp;
			waveY+=waveYtemp;
			p2 = p2 + (1.0 - p2) * 0.7;
			p *= 1.872;
		}

		planeDistance += (waveX + waveY) * water.amplitude;
	}
	return water.empty ? fabs(planeDistance) : planeDistance;
}

double cPrimitives::PrimitiveTorus(CVector3 _point, const sPrimitiveTorus &torus) const
{
	CVector3 point = (_point - torus.position);
	point = torus.rotationMatrix.RotateVector(point);
	point = point.mod(torus.repeat);

	double d1 = sqrt(point.x * point.x + point.y * point.y) - torus.radius;
	double dist = sqrt(d1 * d1 + point.z * point.z) - torus.tube_radius;
	return torus.empty ? fabs(dist) : dist;
}

double cPrimitives::TotalDistance(CVector3 point, double fractalDistance, fractal::enumObjectType *closestObjectType, sRGB *objectColor, double *objectReflect) const
{
	using namespace fractal;
	enumObjectType closestObject = objFractal;
	sRGB color = sRGB(0, 0, 0);
	double reflect = 0.0;
	double distance = fractalDistance;

	if (isAnyPrimitive)
	{

		CVector3 point2 = point - allPrimitivesPosition;
		point2 = mRotAllPrimitivesRotation.RotateVector(point2);

		for (int i = 0; i < planes.size(); i++)
		{
			const sPrimitivePlane &plane = planes.at(i);
			if (plane.enable)
			{
				double distTemp = PrimitivePlane(point, plane);
				if (distTemp < distance)
				{
					closestObject = objPlane;
					color = plane.color;
					reflect = plane.reflect;
				}
				distance = min(distance, distTemp);
			}
		}

		for (int i = 0; i < boxes.size(); i++)
		{
			const sPrimitiveBox &box = boxes.at(i);
			if (box.enable)
			{
				double distTemp = PrimitiveBox(point2, box);
				if (distTemp < distance)
				{
					closestObject = objPlane;
					color = box.color;
					reflect = box.reflect;
				}
				distance = min(distance, distTemp);
			}
		}

		for (int i = 0; i < rectangles.size(); i++)
   	{
			const sPrimitiveRectangle &rectangle = rectangles.at(i);
			if (rectangle.enable)
			{
				double distTemp = PrimitiveRectangle(point2, rectangle);
				if (distTemp < distance)
				{
					closestObject = objPlane;
					color = rectangle.color;
					reflect = rectangle.reflect;
				}
				distance = min(distance, distTemp);
			}
		}

		for (int i = 0; i < spheres.size(); i++)
		{
			const sPrimitiveSphere &sphere = spheres.at(i);
			if (sphere.enable)
			{
				double distTemp = PrimitiveSphere(point2, sphere);
				if (distTemp < distance)
				{
					closestObject = objPlane;
					color = sphere.color;
					reflect = sphere.reflect;
				}
				distance = min(distance, distTemp);
			}
		}

		for (int i = 0; i < cylinders.size(); i++)
		{
			const sPrimitiveCylinder &cylinder = cylinders.at(i);
			if (cylinder.enable)
			{
				double distTemp = PrimitiveCylinder(point2, cylinder);
				if (distTemp < distance)
				{
					closestObject = objPlane;
					color = cylinder.color;
					reflect = cylinder.reflect;
				}
				distance = min(distance, distTemp);
			}
		}

		for (int i = 0; i < circles.size(); i++)
		{
			const sPrimitiveCircle &circle = circles.at(i);
			if (circle.enable)
			{
				double distTemp = PrimitiveCircle(point2, circle);
				if (distTemp < distance)
				{
					closestObject = objPlane;
					color = circle.color;
					reflect = circle.reflect;
				}
				distance = min(distance, distTemp);
			}
		}

		for (int i = 0; i < cones.size(); i++)
		{
			const sPrimitiveCone &cone = cones.at(i);
			if (cone.enable)
			{
				double distTemp = PrimitiveCone(point2, cone);
				if (distTemp < distance)
				{
					closestObject = objPlane;
					color = cone.color;
					reflect = cone.reflect;
				}
				distance = min(distance, distTemp);
			}
		}

		for (int i = 0; i < waters.size(); i++)
		{
			const sPrimitiveWater &water = waters.at(i);
			if (water.enable)
			{
				double distTemp = PrimitiveWater(point, water);
				if (distTemp < distance)
				{
					closestObject = objPlane;
					color = water.color;
					reflect = water.reflect;
				}
				distance = min(distance, distTemp);
			}
		}

		for (int i = 0; i < toruses.size(); i++)
		{
			const sPrimitiveTorus &torus = toruses.at(i);
			if (torus.enable)
			{
				double distTemp = PrimitiveTorus(point2, torus);
				if (distTemp < distance)
				{
					closestObject = objPlane;
					color = torus.color;
					reflect = torus.reflect;
				}
				distance = min(distance, distTemp);
			}
		}
	}//if is any primitive

	*closestObjectType = closestObject;
	*objectColor = color;
	*objectReflect = reflect;
	return distance;
}
