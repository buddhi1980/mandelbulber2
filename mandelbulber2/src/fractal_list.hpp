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
#include "system.hpp"

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
	msltoesym2Mod = 51,
	//mandelbulb5 = 52,
	//mandelbox103 = 53,
	//quaternion104 = 54,
	//mengerSponge105 = 55,
	platonicSolid = 56,
	//mandelbulb6Beta = 57,
	//benesiTransforms = 58,
	foldBoxMod1 = 59,
	aboxModKali = 60,
	mengerMod1 = 61,
	quaternion3D = 62,
	aexionOctopusMod = 63,
	aboxMod1 = 64,
	mandelbulbMulti = 65,
	benesiPineTree = 66,
	benesiT1PineTree = 67,
	msltoesym3Mod = 68,
	msltoesym4Mod = 69,
	eiffieMsltoe = 70,
	riemannSphereMsltoeV1 = 71,
	riemannSphereMsltoe = 72,
	amazingSurf = 73,
	kalisets1 = 74,
	aboxMod2 = 75,
	aboxVSIcen1 = 76,
	amazingSurfMod1 = 77,
	mandelbulbVaryPowerV1 = 78,
	quaternion4D = 79,
	msltoesym3Mod2 = 80,
	msltoesym3Mod3 = 81,
	msltoeDonut = 82,
	iqBulb = 83,
	quickDudleyMod = 84,
	riemannBulbMsltoeMod2 = 85,
	msltoeToroidal = 86,
	aboxModKaliEiffie = 87,
	collatz = 88,
        collatzMod = 89,
        mengerPwr2Poly = 90,



	// 152 to 172 reserved for scator
	fastImagscaPower2 = 152,


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
	transfBenesiSphereCube = 1018,
	transfAddCpixelPosNeg = 1019,
	transfBoxFold = 1020,
	transfSphericalFold = 1021,
	transfZvectorAxisSwap = 1022,
	transfFabsAddConstant = 1023,
	transfNegFabsAddConstant = 1024,
	transfFabsAddConstantV2 = 1025,
	transfMultipleAngle = 1026,
	transfRPower = 1027,
	transfIterationWeight = 1028,
	transfFabsAddMulti = 1029,
	transfSphericalPwrFold = 1030,
	transfAddCpixelVaryV1 = 1031,
	transfScaleVaryV1 = 1032,
	transfAdditionConstantVaryV1 = 1033,
	transfBoxFoldXYZ = 1034,
	transfAddCpixelCxCyAxisSwap = 1035,
	transfRotationVaryV1 = 1036,
	transfLinCombineCxyz = 1037,
	transfSphereInvC = 1038,
	transfAdditionConstant4D = 1039,
	transfBoxFold4D = 1040,
	transfIterationWeight4D = 1041,
	transfSphericalFold4D = 1042,
	transfScale4D = 1043,
	transfFabsAddConstant4D = 1044,
	transfFabsAddConstantV24D = 1045,
        transfSurfFoldMulti = 1046,
        transfPwr2Polynomial = 1047
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

enum enumCPixelAddition
{
	cpixelEnabledByDefault = 0,
	cpixelDisabledByDefault = 1,
	cpixelAlreadyHas = 2
};

}

struct sFractalDescription
{
	QString nameInComboBox;
	QString internalName;
	fractal::enumFractalFormula internalID;
	fractal::enumDEType DEType;
	fractal::enumDEFunctionType DEFunctionType;
	fractal::enumCPixelAddition cpixelAddition;
	double defaultBailout;

	sFractalDescription(QString _nameInComboBox, QString _internalName,
			fractal::enumFractalFormula _internalID, fractal::enumDEType _DEType,
			fractal::enumDEFunctionType _DEFunctionType, fractal::enumCPixelAddition _cpixelAddition,
			double _defaultBailout) :
					nameInComboBox(_nameInComboBox),
					internalName(_internalName),
					internalID(_internalID),
					DEType(_DEType),
					DEFunctionType(_DEFunctionType),
					cpixelAddition(_cpixelAddition),
					defaultBailout(_defaultBailout)
	{
	}
	QString getIconName(){
		if(internalID == fractal::none)
		{
			return QString(":system/icons/list-remove.svg");
		}
		return systemData.sharedDir + "qt_data" + QDir::separator()
				+ "formula_and_transform_images" + QDir::separator() + internalName + ".png";
	}
};

extern QList<sFractalDescription> fractalList;

void DefineFractalList(QList<sFractalDescription> *fractalList);

#endif /* FRACTAL_LIST_HPP_ */
