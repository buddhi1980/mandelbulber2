/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include "write_log.hpp"

using namespace fractal;
using namespace std;

QString cPrimitives::PrimitiveNames(enumObjectType primitiveType)
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

enumObjectType cPrimitives::PrimitiveNameToEnum(const QString &primitiveType)
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

cPrimitives::cPrimitives(
	const std::shared_ptr<cParameterContainer> par, QVector<cObjectData> *objectData)
{
	WriteLog("cPrimitives::cPrimitives(const std::shared_ptr<cParameterContainer> par) started", 3);
	isAnyPrimitive = false;

	QList<QString> listOfParameters = par->GetListOfParameters();
	QList<sPrimitiveItem> listOfPrimitives = GetListOfPrimitives(par);

	// bubble sort by calculation order
	for (int i = listOfPrimitives.size() - 1; i > 0; i--)
	{
		for (int j = 0; j < listOfPrimitives.size() - 1; j++)
		{
			int order1 = par->Get<int>(listOfPrimitives.at(j).fullName + "_calculation_order");
			int order2 = par->Get<int>(listOfPrimitives.at(j + 1).fullName + "_calculation_order");

			if (order1 > order2)
			{
#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
				listOfPrimitives.swap(j, j + 1);
#else
				listOfPrimitives.swapItemsAt(j, j + 1);
#endif
			}
		}
	}

	for (auto item : listOfPrimitives)
	{
		using namespace fractal;
		sPrimitiveBasic *primitive;

		switch (item.type)
		{
			case objPlane:
			{
				primitive = new sPrimitivePlane(item.fullName, par);
				break;
			}
			case objBox:
			{
				primitive = new sPrimitiveBox(item.fullName, par);
				break;
			}
			case objSphere:
			{
				primitive = new sPrimitiveSphere(item.fullName, par);
				break;
			}
			case objWater:
			{
				primitive = new sPrimitiveWater(item.fullName, par);
				break;
			}
			case objCone:
			{
				primitive = new sPrimitiveCone(item.fullName, par);
				break;
			}
			case objCylinder:
			{
				primitive = new sPrimitiveCylinder(item.fullName, par);
				break;
			}
			case objTorus:
			{
				primitive = new sPrimitiveTorus(item.fullName, par);
				break;
			}
			case objCircle:
			{
				primitive = new sPrimitiveCircle(item.fullName, par);
				break;
			}
			case objRectangle:
			{
				primitive = new sPrimitiveRectangle(item.fullName, par);
				break;
			}
			default:
			{
				qCritical() << "cannot handle " << PrimitiveNames(item.type)
										<< " in cPrimitives::cPrimitives()";
				return;
			}
		}

		primitive->objectType = item.type;

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

	WriteLog("cPrimitives::cPrimitives(const std::shared_ptr<cParameterContainer> par) finished", 3);
}

cPrimitives::~cPrimitives()
{
	qDeleteAll(allPrimitives);
}

double cPrimitives::TotalDistance(CVector3 point, double fractalDistance, double detailSize,
	bool normalCalculationMode, int *closestObjectId, sRenderData *data) const
{
	using namespace fractal;
	int closestObject = *closestObjectId;
	double distance = fractalDistance;

	if (allPrimitives.count() > 0)
	{
		CVector3 point2 = point - allPrimitivesPosition;
		point2 = mRotAllPrimitivesRotation.RotateVector(point2);

		for (auto primitive : allPrimitives)
		{
			if (primitive->enable)
			{
				sPrimitiveWater *water = dynamic_cast<sPrimitiveWater *>(primitive);
				double distTemp;
				if (water)
				{
					distTemp = water->PrimitiveDistanceWater(point2, distance);
				}
				else
				{
					distTemp = primitive->PrimitiveDistance(point2);
				}
				distTemp = DisplacementMap(distTemp, point2, primitive->objectId, data);

				switch (primitive->booleanOperator)
				{
					case primBooleanOperatorOR:
					{
						if (distTemp < distance)
						{
							closestObject = primitive->objectId;
						}
						distance = min(distance, distTemp);
						// distance = smoothMin(distance, distTemp, 0.1);
						break;
					}
					case primBooleanOperatorAND:
					{
						if (distTemp > distance)
						{
							closestObject = primitive->objectId;
						}
						distance = max(distance, distTemp);
						break;
					}
					case primBooleanOperatorSUB:
					{
						const double limit = 1.5;
						if (distance < detailSize) // if inside 1st
						{
							if (distTemp < detailSize * limit * 1.5)
							{
								closestObject = primitive->objectId;
							}

							if (distTemp < detailSize * limit) // if inside 2nd
							{
								if (normalCalculationMode)
								{
									distance = max(detailSize * limit - distTemp, distance);
								}
								else
								{
									distance = detailSize * limit;
								}
							}
							else // if outside of 2nd
							{
								distance = max(detailSize * limit - distTemp, distance);
								if (distance < 0) distance = 0;
							}
						}
						break;
					}
					case primBooleanOperatorRevSUB:
					{
						int closestObjectTemp = closestObject;
						closestObject = primitive->objectId;
						const double limit = 1.5;
						if (distTemp < detailSize) // if inside 2nd
						{
							if (distance < detailSize * limit * 1.5)
							{
								closestObject = closestObjectTemp;
							}

							if (distance < detailSize * limit) // if inside 1st
							{
								if (normalCalculationMode)
								{
									distance = max(detailSize * limit - distance, distTemp);
								}
								else
								{
									distance = detailSize * limit;
								}
							}
							else // if outside of 1st
							{
								distTemp = max(detailSize * limit - distance, distTemp);
								distance = distTemp;
								if (distance < 0) distance = 0;
							}
						}
						else
						{
							distance = distTemp;
						}
						break;
					}
				} // switch
			}
		}

	} // if is any primitive

	*closestObjectId = closestObject;

	return distance;
}

QList<sPrimitiveItem> cPrimitives::GetListOfPrimitives(
	const std::shared_ptr<cParameterContainer> par)
{
	QList<sPrimitiveItem> listOfPrimitives;

	QList<QString> listOfParameters = par->GetListOfParameters();
	for (auto &parameterName : listOfParameters)
	{
		if (parameterName.left(parameterName.indexOf('_')) == "primitive")
		{
			QStringList split = parameterName.split('_');
			QString primitiveName = split.at(0) + "_" + split.at(1) + "_" + split.at(2);
			QString objectTypeString = split.at(1);
			int index = split.at(2).toInt();

			bool found = false;
			for (const auto &listOfPrimitive : listOfPrimitives)
			{
				if (listOfPrimitive.fullName == primitiveName)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				fractal::enumObjectType objectType = PrimitiveNameToEnum(objectTypeString);
				sPrimitiveItem newItem(objectType, index, primitiveName, objectTypeString);
				listOfPrimitives.append(newItem);
			}
		}
	}
	return listOfPrimitives;
}

int cPrimitives::NewPrimitiveIndex(
	const QString &primitiveType, const QList<sPrimitiveItem> &listOfPrimitives)
{
	QString primitiveName = QString("primitive_") + primitiveType;
	fractal::enumObjectType objectType = PrimitiveNameToEnum(primitiveType);

	int newId = 0;

	// look for the lowest free id
	bool occupied = true;

	while (occupied)
	{
		newId++;
		occupied = false;
		for (const auto &primitiveItem : listOfPrimitives)
		{
			if (objectType == primitiveItem.type && newId == primitiveItem.id) occupied = true;
		}
	}

	return newId;
}
