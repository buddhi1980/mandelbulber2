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

#ifndef MANDELBULBER2_SRC_PRIMITIVES_H_
#define MANDELBULBER2_SRC_PRIMITIVES_H_

#include "QtCore"
#include "algebra.hpp"
#include "color_structures.hpp"
#include "object_data.hpp"
#include "object_types.hpp"

// forward declarations
class cParameterContainer;
struct sRenderData;

struct sPrimitiveItem
{
	sPrimitiveItem(fractal::enumObjectType _type, int _id, QString _name)
			: type(_type), id(_id), name(_name)
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
	virtual ~sPrimitiveBasic() {}
	virtual double PrimitiveDistance(CVector3 _point) const = 0;
};

struct sPrimitivePlane : sPrimitiveBasic
{
	bool empty;
	double PrimitiveDistance(CVector3 _point) const override;
};

struct sPrimitiveBox : sPrimitiveBasic
{
	bool empty;
	double rounding;
	CVector3 repeat;
	double PrimitiveDistance(CVector3 _point) const override;
};

struct sPrimitiveSphere : sPrimitiveBasic
{
	bool empty;
	double radius;
	CVector3 repeat;
	double PrimitiveDistance(CVector3 _point) const override;
};

struct sPrimitiveWater : sPrimitiveBasic
{
	bool empty;
	bool waveFromObjectsEnable;
	double relativeAmplitude;
	double animSpeed;
	double length;
	double waveFromObjectsRelativeAmplitude;
	int iterations;
	int animFrame;
	double PrimitiveDistance(CVector3 _point) const override;
	double PrimitiveDistanceWater(CVector3 _point, double distanceFromAnother) const;
};

struct sPrimitiveCone : sPrimitiveBasic
{
	bool empty;
	bool caps;
	double radius;
	double height;
	CVector2<double> wallNormal;
	CVector3 repeat;
	double PrimitiveDistance(CVector3 _point) const override;
};

struct sPrimitiveCylinder : sPrimitiveBasic
{
	bool empty;
	bool caps;
	double radius;
	double height;
	CVector3 repeat;
	double PrimitiveDistance(CVector3 _point) const override;
};

struct sPrimitiveTorus : sPrimitiveBasic
{
	bool empty;
	double radius;
	double radius_lpow;
	double tube_radius;
	double tube_radius_lpow;
	CVector3 repeat;
	double PrimitiveDistance(CVector3 _point) const override;
};

struct sPrimitiveCircle : sPrimitiveBasic
{
	double radius;
	double PrimitiveDistance(CVector3 _point) const override;
};

struct sPrimitiveRectangle : sPrimitiveBasic
{
	double height;
	double width;
	double PrimitiveDistance(CVector3 _point) const override;
};

QString PrimitiveNames(fractal::enumObjectType primitiveType);

fractal::enumObjectType PrimitiveNameToEnum(const QString &primitiveType);

class cPrimitives
{
	// some of functions for primitives were taken from
	// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm

public:
	cPrimitives(const cParameterContainer *par, QVector<cObjectData> *objectData = nullptr);
	~cPrimitives();
	double TotalDistance(
		CVector3 point, double fractalDistance, int *closestObjectId, sRenderData *data) const;

private:
	QList<sPrimitiveBasic *> allPrimitives;

	static double Plane(CVector3 point, CVector3 position, CVector3 normal)
	{
		return (normal.Dot(point - position));
	}

	CVector3 allPrimitivesPosition;
	CVector3 allPrimitivesRotation;
	CRotationMatrix mRotAllPrimitivesRotation;
	bool isAnyPrimitive;
};

#endif /* MANDELBULBER2_SRC_PRIMITIVES_H_ */
