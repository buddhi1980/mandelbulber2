/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
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
 * cFractal class - container for fractal formula parameters
 */

#ifndef MANDELBULBER2_SRC_FRACTAL_H_
#define MANDELBULBER2_SRC_FRACTAL_H_

#include "algebra.hpp"
#include "fractal_list.hpp"
#include "parameters.hpp"

const int IFS_VECTOR_COUNT = 9;
const int HYBRID_COUNT = 5;
const int MANDELBOX_FOLDS = 2;

namespace fractal
{
enum enumCalculationMode
{
	calcModeNormal = 0,
	calcModeColouring = 1,
	calcModeFake_AO = 2,
	calcModeDeltaDE1 = 3,
	calcModeDeltaDE2 = 4,
	calcModeOrbitTrap = 5
};

enum enumGeneralizedFoldBoxType
{
	foldTet = 0,
	foldCube = 1,
	foldOct = 2,
	foldDodeca = 3,
	foldOctCube = 4,
	foldIcosa = 5,
	foldBox6 = 6,
	foldBox5 = 7
};

enum enumDEMethod
{
	preferedDEMethod = 0,
	forceDeltaDEMethod = 1,
};

enum enumOCLDEMode
{
	ocl_DEcalculated = 0,
	ocl_deltaDE = 1,
	ocl_noDE = 2
};
}

// struct sMandelbulbAux
//{
//	double r_dz;
//	double r;
//};

// struct sMandelboxAux
//{
//	double mboxDE;
//	double mboxColor;
//	double actualScale;
//};

// struct sIFSAux
//{
//	double ifsDE;
//};

// struct sAexionAux
//{
//	CVector3 c;
//	double cw;
//	int iterNo;
//};

struct sExtendedAux
{
	double r_dz;
	double r;
	double DE;
	double color;
	double actualScale;
	double newR;
	double orbitTraps;
	double axisBias;
	double transformSampling;
	double cw;
	double foldFactor;
	double minRFactor;
	double scaleFactor;
	CVector3 c;
};

struct sFoldColor
{
	CVector3 factor;
	CVector4 factor4D;
	double factorR;
	double factorSp1;
	double factorSp2;
	double colorMin;
	double compFold0;
	double compFold;
	double compMinR;
	double compScale;
};

struct sFractalGeneralizedFoldBox
{
	fractal::enumGeneralizedFoldBoxType type;
	CVector3 Nv_tet[4];
	CVector3 Nv_cube[6];
	CVector3 Nv_oct[8];
	CVector3 Nv_oct_cube[14];
	CVector3 Nv_dodeca[12];
	CVector3 Nv_icosa[20];
	CVector3 Nv_box6[8];
	CVector3 Nv_box5[7];
	int sides_tet;
	int sides_cube;
	int sides_oct;
	int sides_oct_cube;
	int sides_dodeca;
	int sides_icosa;
	int sides_box6;
	int sides_box5;
};

struct sFractalIFS
{
	bool absX, absY, absZ;
	bool enabled[IFS_VECTOR_COUNT];
	bool mengerSpongeMode;
	bool rotationEnabled;
	bool edgeEnabled;
	CRotationMatrix mainRot;
	CRotationMatrix rot[IFS_VECTOR_COUNT];
	CVector3 direction[IFS_VECTOR_COUNT];
	CVector3 edge;
	CVector3 offset;
	CVector3 rotations[IFS_VECTOR_COUNT];
	double distance[IFS_VECTOR_COUNT];
	double intensity[IFS_VECTOR_COUNT];
	CVector3 rotation;
	double scale;
};

struct sFractalMandelboxVary4D
{
	double fold;
	double minR;
	double scaleVary;
	double wadd;
	double rPower;
};

struct sFractalMandelbox
{
	CVector3 rotationMain;
	CVector3 rotation[MANDELBOX_FOLDS][3];
	sFoldColor color;
	double scale;
	double foldingLimit;
	double foldingValue;
	double foldingSphericalMin;
	double foldingSphericalFixed;
	double sharpness;
	double solid;
	double melt;
	CVector3 offset;
	bool rotationsEnabled;
	bool mainRotationEnabled;
	CRotationMatrix mainRot;
	CRotationMatrix rot[MANDELBOX_FOLDS][3];
	CRotationMatrix rotinv[MANDELBOX_FOLDS][3];

	double fR2;
	double mR2;
	double mboxFactor1;
};

struct sFractalBoxFoldBulbPow2
{
	double zFactor;
	double foldfactor;
};

struct sFractalMandelbulb
{
	double power;
	double alphaAngleOffset;
	double betaAngleOffset;
	double gammaAngleOffset;
};

struct sFractalAexion
{
	double cadd;
};

struct sFractalBuffalo
{
	bool preabsx;
	bool preabsy;
	bool preabsz;
	bool absx;
	bool absy;
	bool absz;
	bool posz;
};

struct sFractalDonut
{
	double ringRadius;
	double ringThickness;
	double factor;
	double number;
};

//----------------------------------------------------------
struct sFractalPlatonicSolid
{
	double frequency;
	double amplitude;
	double rhoMul;
};

// mandelbulb multi
struct sFractalMandelbulbMulti
{
	enum multi_acosOrasin
	{
		acos,
		asin
	};

	enum multi_atanOratan2
	{
		atan,
		atan2
	};

	enum multi_OrderOfxyz
	{
		xyz,
		xzy,
		yxz,
		yzx,
		zxy,
		zyx
	};

	multi_acosOrasin acosOrasin;
	multi_acosOrasin acosOrasinA;
	multi_atanOratan2 atanOratan2;
	multi_atanOratan2 atanOratan2A;

	multi_OrderOfxyz orderOfxyz;
	multi_OrderOfxyz orderOfxyz2;
	multi_OrderOfxyz orderOfxyzC;
};

// sinTan2Trig
struct sFractalSinTan2Trig
{
	enum multi_asinOracos
	{
		asin,
		acos
	};

	enum multi_atan2Oratan
	{
		atan2,
		atan
	};

	enum multi_OrderOfzyx
	{
		zyx,
		zxy,
		yzx,
		yxz,
		xzy,
		xyz
	};

	multi_asinOracos asinOracos;
	multi_atan2Oratan atan2Oratan;
	multi_OrderOfzyx orderOfzyx;
};

// surf fold box
struct sFractalSurfFolds
{
	enum multi_orderOfFolds
	{
		type1,
		type2,
		type3,
		type4,
		type5
	};
	multi_orderOfFolds orderOfFolds1;
	multi_orderOfFolds orderOfFolds2;
	multi_orderOfFolds orderOfFolds3;
	multi_orderOfFolds orderOfFolds4;
	multi_orderOfFolds orderOfFolds5;
};

// benesi mag transfroms
struct sFractalMagTransforms
{
	enum multi_orderOfTransf
	{
		typeT1,
		typeT1Mod,
		typeT2,
		typeT3,
		typeT4,
		typeT5b,
	};
	multi_orderOfTransf orderOfTransf1;
	multi_orderOfTransf orderOfTransf2;
	multi_orderOfTransf orderOfTransf3;
	multi_orderOfTransf orderOfTransf4;
	multi_orderOfTransf orderOfTransf5;
};

// for surfbox types
struct sFractalSurfBox
{
	bool enabledX1;
	bool enabledY1;
	bool enabledZ1;
	bool enabledX2False;
	bool enabledY2False;
	bool enabledZ2False;
	bool enabledX3False;
	bool enabledY3False;
	bool enabledZ3False;
	bool enabledX4False;
	bool enabledY4False;
	bool enabledZ4False;
	bool enabledX5False;
	bool enabledY5False;
	bool enabledZ5False;
	CVector3 offset1A111;
	CVector3 offset1B111;
	CVector3 offset2A111;
	CVector3 offset2B111;
	CVector3 offset3A111;
	CVector3 offset3B111;
	CVector3 offset1A222;
	CVector3 offset1B222;
	double scale1Z1;
};

// for curvilinear
struct sFractalCpara
{
	bool enabledLinear;
	bool enabledCurves;
	bool enabledParabFalse;
	bool enabledParaAddP0;
	double para0;
	double paraA;
	double paraB;
	double paraC;
	double parabOffset;
	double parabSlope;
	double parabScale;
	int iterA;
	int iterB;
	int iterC;
};

struct sFractalAnalyticDE
{
	bool enabledFalse;
	bool enabledAuxR2False;
	double scale1;
	double tweak005;
	double offset1;
	double offset2;
	double scaleLin;
	double offsetLin;
};

// common parameters for transforming formulas
struct sFractalTransformCommon
{
	double alphaAngleOffset;
	double betaAngleOffset;
	double foldingValue;
	double foldingLimit;
	double offset;
	double offset0;
	double offset05;
	double offset1;
	double offset105;
	double offset2;
	double multiplication;
	double minRneg1;
	double minR0;
	double minR05;
	double minR06;
	double scale;
	double scale0;
	double scale025;
	double scale1;
	double scaleA1;
	double scaleB1;
	double scaleC1;
	double scaleD1;
	double scaleE1;
	double scaleA2;
	double scale015;
	double scale2;
	double scale3;
	double scaleA3;
	double scale4;
	double scale8;
	double pwr05;
	double pwr4;
	double pwr8;
	double pwr8a;
	double sqtR;
	double mboxFactor1;

	int startIterations;
	int startIterations250;
	int stopIterations;
	int startIterationsA;
	int stopIterationsA;
	int startIterationsB;
	int stopIterationsB;
	int startIterationsC;
	int stopIterationsC;
	int startIterationsD;
	int stopIterationsD;
	int startIterationsE;
	int stopIterationsE;
	int startIterationsF;
	int stopIterationsF;
	int startIterationsP;
	int stopIterationsP1;
	int startIterationsR;
	int stopIterationsR;
	int startIterationsS;
	int stopIterationsS;
	int startIterationsT;
	int stopIterationsT;
	int startIterationsM;
	int stopIterationsM;
	int stopIterations1;
	int stopIterationsT1;
	int stopIterationsTM1;
	int intA;
	int intB;
	int int1;
	int int8X;
	int int8Y;
	int int8Z;

	CVector3 additionConstant0555;
	CVector3 additionConstant0777;
	CVector3 additionConstant000;
	CVector3 additionConstantA000;
	CVector3 additionConstantP000;
	CVector3 additionConstant111;
	CVector3 additionConstantA111;
	CVector3 additionConstant222;
	CVector3 additionConstantNeg100;
	CVector3 constantMultiplier000;
	CVector3 constantMultiplier001;
	CVector3 constantMultiplier010;
	CVector3 constantMultiplier100;
	CVector3 constantMultiplierA100;
	CVector3 constantMultiplier111;
	CVector3 constantMultiplierA111;
	CVector3 constantMultiplierB111;
	CVector3 constantMultiplierC111;
	CVector3 constantMultiplier121;
	CVector3 constantMultiplier122;
	CVector3 constantMultiplier221;
	CVector3 constantMultiplier222;
	CVector3 constantMultiplier441;
	CVector3 juliaC;
	CVector3 offset000;
	CVector3 offsetA000;
	CVector3 offsetF000;
	CVector3 offset100;
	CVector3 offset111;
	CVector3 offsetA111;
	CVector3 offsetB111;
	CVector3 offsetC111;
	CVector3 offset200;
	CVector3 offsetA200;
	CVector3 offset222;

	CVector3 power025;
	CVector3 power8;
	CVector3 rotation;
	CVector3 rotation2;
	CVector3 scale3D111;
	CVector3 scale3D222;
	CVector3 scale3Da222;
	CVector3 scale3Db222;
	CVector3 scale3Dc222;
	CVector3 scale3Dd222;
	CVector3 scale3D333;
	CVector3 scale3D444;

	CVector4 additionConstant0000;
	CVector4 constantMultiplier1220;

	CRotationMatrix rotationMatrix;
	CRotationMatrix rotationMatrix2;
	CRotationMatrix tempRotmatrix;

	bool addCpixelEnabled;
	bool addCpixelEnabledFalse;
	bool alternateEnabledFalse;
	bool benesiT1Enabled;
	bool benesiT1EnabledFalse;
	bool benesiT1MEnabledFalse;
	bool functionEnabled;
	bool functionEnabledFalse;
	bool functionEnabledx;
	bool functionEnabledy;
	bool functionEnabledz;
	bool functionEnabledxFalse;
	bool functionEnabledyFalse;
	bool functionEnabledzFalse;
	bool functionEnabledAx;
	bool functionEnabledAy;
	bool functionEnabledAz;
	bool functionEnabledAxFalse;
	bool functionEnabledAyFalse;
	bool functionEnabledAzFalse;
	bool functionEnabledBxFalse;
	bool functionEnabledByFalse;
	bool functionEnabledBzFalse;
	bool functionEnabledCxFalse;
	bool functionEnabledCyFalse;
	bool functionEnabledCzFalse;
	bool functionEnabledFFalse;
	bool functionEnabledKFalse;
	bool functionEnabledM;
	bool functionEnabledMFalse;
	bool functionEnabledPFalse;
	bool functionEnabledRFalse;
	bool functionEnabledXFalse;
	bool juliaMode;
	bool rotationEnabled;
};

class cFractal
{
public:
	cFractal(const cParameterContainer *par);
	void RecalculateFractalParams(void);

	fractal::enumFractalFormula formula;
	sFractalMandelbulb bulb;
	sFractalIFS IFS;
	sFractalMandelbox mandelbox;
	sFractalGeneralizedFoldBox genFoldBox;
	sFractalBoxFoldBulbPow2 foldingIntPow;
	sFractalMandelboxVary4D mandelboxVary4D;
	sFractalAexion aexion;
	sFractalBuffalo buffalo;
	sFractalPlatonicSolid platonicSolid;
	sFractalTransformCommon transformCommon;
	sFractalAnalyticDE analyticDE;
	sFractalMandelbulbMulti mandelbulbMulti;
	sFractalSinTan2Trig sinTan2Trig;
	sFractalSurfBox surfBox;
	sFractalSurfFolds surfFolds;
	sFractalDonut donut;
	sFoldColor foldColor;
	sFractalMagTransforms magTransf;
	sFractalCpara Cpara;

#ifdef CLSUPPORT
	double customParameters[15];
	double deltaDEStep;
	char customOCLFormulaName[100];
	fractal::enumOCLDEMode customOCLFormulaDEMode;
#endif
};

#endif /* MANDELBULBER2_SRC_FRACTAL_H_ */
