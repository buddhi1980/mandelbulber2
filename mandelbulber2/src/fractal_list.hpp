/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * list of fractal formulas
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

#ifndef FRACTAL_LIST_HPP_
#define FRACTAL_LIST_HPP_

#include <QtCore>

namespace fractal
{
enum enumFractalFormula
{
	none = 0,
	fast_mandelbulb = 1,
	mandelbulb = 2,
	fast_mandelbulb_power2 = 3,
	hypercomplex = 4,
	quaternion = 5,
	minus_fast_trig = 6,
	menger_sponge = 7,
	mandelbox = 8,
	kaleidoscopicIFS = 10,
	xenodreambuie = 11,
	hybrid = 12,
	mandelbulb2 = 13,
	mandelbulb3 = 14,
	mandelbulb4 = 15,
	boxFoldBulbPow2 = 16,
	smoothMandelbox = 17,
	mandelboxVaryScale4D = 18,
	aexion = 19,
	benesi = 20,
	bristorbrot = 21,
	invertX = 22,
	invertY = 23,
	invertZ = 24,
	invertR = 25,
	sphericalFold = 26,
	powXYZ = 27,
	scaleX = 28,
	scaleY = 29,
	scaleZ = 30,
	offsetX = 31,
	offsetY = 32,
	offsetZ = 33,
	angleMultiplyX = 34,
	angleMultiplyY = 35,
	angleMultiplyZ = 36,
	generalizedFoldBox = 37,
	ocl_custom = 38,
	ides = 39,
	ides2 = 40,
	buffalo = 41,
	quickdudley = 48,
	lkmitch = 49,
	makin3d2 = 50,
	msltoesym2 = 51,
	mandelbulb5 = 52,
	mandelbox103 = 53,
	quaternion104 = 54,
	mengerSponge105 = 55,
	platonicSolid = 56,
	mandelbulb6Beta = 57,
	benesiTransforms = 58,
  fabsBoxMod = 59,
  aboxModKali = 60,
  mengerMod = 61,
  quaternion3D = 62,
  aexionOctopus = 63,
  aboxMod1 = 64,
  mandelbulbMulti = 65,
  benesiPineTree = 66,
  benesiT1PineTree = 67,

	transfAdditionConstant = 1001,
	transfRotation = 1002,
	transfScale = 1003,
  transfScale3D = 1004,
  transfBoxOffset = 1005,
  transfSphericalOffset = 1006,
  transfAddCpixel = 1007,
  transfAddCpixelAxisSwap = 1008,
  transfBenesiT1 = 1009,
  transfBenesiT1Mod = 1010,
  transfBenesiT2 = 1011,
  transfBenesiT3 = 1012,
  transfBenesiT4 = 1013,
  transfBenesiT5b = 1014,
  transfBenesiMagForward = 1015,
  transfBenesiMagBackward = 1016,
  transfBenesiCubeSphere = 1017,
  transfBenesiSphereCube = 1018
};

enum enumDEType
{
	analyticDEType, deltaDEType
};

enum enumDEFunctionType
{
	logarithmicDEFunction = 0,
	linearDEFunction = 1,
	preferedDEfunction = 2,
	withoutDEFunction = 3,
};

}

struct sFractalDescription
{
	QString nameInComboBox;
	QString internalNane;
	fractal::enumFractalFormula internalID;
	fractal::enumDEType DEType;
	fractal::enumDEFunctionType DEFunctionType;

	sFractalDescription(QString _nameInComboBox, QString _internalNane,
			fractal::enumFractalFormula _internalID, fractal::enumDEType _DEType,
			fractal::enumDEFunctionType _DEFunctionType) :
					nameInComboBox(_nameInComboBox),
					internalNane(_internalNane),
					internalID(_internalID),
					DEType(_DEType),
					DEFunctionType(_DEFunctionType)
	{
	}
};

extern QList<sFractalDescription> fractalList;

void DefineFractalList(QList<sFractalDescription> *fractalList);

#endif /* FRACTAL_LIST_HPP_ */
