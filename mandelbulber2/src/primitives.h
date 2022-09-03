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
 * containers with primitive objects
 */

#ifndef MANDELBULBER2_SRC_PRIMITIVES_H_
#define MANDELBULBER2_SRC_PRIMITIVES_H_

#include <memory>
#include <utility>
#include <vector>

#include <QtCore>
#include <QString>
#include "color_structures.hpp"
#include "primitive.hpp"
#include "object_data.hpp"
#include "object_types.hpp"

// forward declarations
class cParameterContainer;
struct sRenderData;

class cPrimitives
{
	// some of functions for primitives were taken from
	// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm

public:
	cPrimitives(
		const std::shared_ptr<cParameterContainer> par, QVector<cObjectData> *objectData = nullptr);
	~cPrimitives();
	double TotalDistance(CVector3 point, double fractalDistance, double detailSize,
		bool normalCalculationMode, int *closestObjectId, sRenderData *data) const;
	const std::shared_ptr<sPrimitiveBasic> GetPrimitive(const int index) const
	{
		return allPrimitives[index];
	}
	int GetNumberOfPrimivives() const { return allPrimitives.size(); }
	static QList<sPrimitiveItem> GetListOfPrimitives(const std::shared_ptr<cParameterContainer> par);
	static int NewPrimitiveIndex(
		const QString &primitiveType, const QList<sPrimitiveItem> &listOfPrimitives);
	static QString PrimitiveNames(fractal::enumObjectType primitiveType);
	static fractal::enumObjectType PrimitiveNameToEnum(const QString &primitiveType);

	CVector3 allPrimitivesPosition;
	CVector3 allPrimitivesRotation;
	CRotationMatrix mRotAllPrimitivesRotation;

private:
	std::vector<std::shared_ptr<sPrimitiveBasic>> allPrimitives;

	static double Plane(CVector3 point, CVector3 position, CVector3 normal)
	{
		return (normal.Dot(point - position));
	}
	bool isAnyPrimitive;
};

#endif /* MANDELBULBER2_SRC_PRIMITIVES_H_ */
