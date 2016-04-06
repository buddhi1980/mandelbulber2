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

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "algebra.hpp"
#include "color_structures.hpp"
#include "QtCore"
#include "parameters.hpp"
#include "object_data.hpp"
#include "object_types.hpp"

struct sPrimitiveItem
{
	sPrimitiveItem(fractal::enumObjectType _type, int _id, QString _name) :
			type(_type), id(_id), name(_name)
	{
	}

	fractal::enumObjectType type;
	int id;
	QString name;
};

struct sPrimitiveBasic : cObjectData
{
	bool enable;
	int objectId;
};

struct sPrimitivePlane: sPrimitiveBasic
{
	bool empty;
};

struct sPrimitiveBox: sPrimitiveBasic
{
	bool empty;
	double rounding;
	CVector3 repeat;
};

struct sPrimitiveSphere: sPrimitiveBasic
{
	bool empty;
	double radius;
	CVector3 repeat;
};

struct sPrimitiveWater: sPrimitiveBasic
{
	bool empty;
	double amplitude;
	double animSpeed;
	double length;
	int iterations;
	int animFrame;
};

struct sPrimitiveCone: sPrimitiveBasic
{
	bool empty;
	bool caps;
	double radius;
	double height;
	CVector2<double> wallNormal;
	CVector3 repeat;
};

struct sPrimitiveCylinder: sPrimitiveBasic
{
	bool empty;
	bool caps;
	double radius;
	double height;
	CVector3 repeat;
};

struct sPrimitiveTorus: sPrimitiveBasic
{
	bool empty;
	double radius;
	double tube_radius;
	CVector3 repeat;
};

struct sPrimitiveCircle: sPrimitiveBasic
{
	double radius;
};

struct sPrimitiveRectangle: sPrimitiveBasic
{
	double height;
	double width;
};

QString PrimitiveNames(fractal::enumObjectType primitiveType);

fractal::enumObjectType PrimitiveNameToEnum(const QString &primitiveType);

class cPrimitives
{
	//some of functions for primitives were taken from http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm

public:
	cPrimitives(const cParameterContainer *par, QVector<cObjectData> *obejctData = NULL);
	double TotalDistance(CVector3 point, double fractalDistance,
			int *closestObjectId) const;

private:
	QVector<sPrimitiveBox> boxes;
	QVector<sPrimitiveCircle> circles;
	QVector<sPrimitiveCone> cones;
	QVector<sPrimitiveCylinder> cylinders;
	QVector<sPrimitivePlane> planes;
	QVector<sPrimitiveRectangle> rectangles;
	QVector<sPrimitiveSphere> spheres;
	QVector<sPrimitiveWater> waters;
	QVector<sPrimitiveTorus> toruses;

	double PrimitiveBox(CVector3 point, const sPrimitiveBox &box) const;
	double PrimitivePlane(CVector3 point, const sPrimitivePlane &plane) const;
	double PrimitiveSphere(CVector3 point, const sPrimitiveSphere &sphere) const;
	double PrimitiveRectangle(CVector3 point, const sPrimitiveRectangle &rectangle) const;
	double PrimitiveCylinder(CVector3 point, const sPrimitiveCylinder &cylinder) const;
	double PrimitiveCircle(CVector3 point, const sPrimitiveCircle &circle) const;
	double PrimitiveCone(CVector3 point, const sPrimitiveCone &cone) const;
	double PrimitiveWater(CVector3 point, const sPrimitiveWater &water) const;
	double PrimitiveTorus(CVector3 point, const sPrimitiveTorus &torus) const;

	inline double Plane(CVector3 point, CVector3 position, CVector3 normal) const
	{
		return (normal.Dot(point - position));
	}

	CVector3 allPrimitivesPosition;
	CVector3 allPrimitivesRotation;
	CRotationMatrix mRotAllPrimitivesRotation;
	bool isAnyPrimitive;
};

#endif /* PRIMITIVES_HPP_ */
