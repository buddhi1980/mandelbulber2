/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cFractal class - container for fractal formula parameters
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

#ifndef FRACTAL_H_
#define FRACTAL_H_

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

enum enumFractalColoringAlgorithm
{
	fractalColoringStandard = 0,
	fractalColoringZDotPoint = 1,
	fractalColoringSphere = 2,
	fractalColoringCross = 3,
	fractalColoringLine = 4
};

enum enumOCLDEMode
{
	ocl_DEcalculated = 0, ocl_deltaDE = 1, ocl_noDE = 2
};
}

//struct sMandelbulbAux
//{
//	double r_dz;
//	double r;
//};

//struct sMandelboxAux
//{
//	double mboxDE;
//	double mboxColor;
//	double actualScale;
//};

//struct sIFSAux
//{
//	double ifsDE;
//};

//struct sAexionAux
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
	CVector3 c;
};

struct sFoldColor
{
	CVector3 factor;
        CVector4 factor4D;
	double factorR;
	double factorSp1;
	double factorSp2;
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

//mandelbulb multi
struct sFractalMandelbulbMulti
{
  enum multi_acosOrasin
  {
    acos, asin
  };
  enum multi_atanOratan2
  {
    atan, atan2
  };
  enum multi_OrderOfxyz
  {
    xyz, xzy, yxz, yzx, zxy, zyx
  };
  multi_acosOrasin acosOrasin;
  multi_atanOratan2 atanOratan2;
  multi_OrderOfxyz orderOfxyz;
};

//common parameters for transforming formulas
struct sFractalTransformCommon
{

  double offset;
  double offset0;
  double offset05;
  double offset1;
  double offset105;
  double offset2;
  double multiplication;
  double minR0;
  double minR05;
  double minR06;
  double scale;
  double scale0;
  double scale1;
  double scaleA1;
  double scale015;
  double scale3;
  double pwr05;
  double pwr4;
  double pwr8;
  double pwr8a;

  int startIterations;
  int startIterations250;
  int stopIterations;
  int startIterationsA;
  int stopIterationsA;
  int startIterationsB;
  int stopIterationsB;
  int startIterationsC;
  int stopIterationsC;
  int intA;
  int intB;

  CVector3 additionConstant0555;
  CVector3 additionConstant000;
  CVector3 additionConstantA000;
  CVector3 additionConstant111;
  CVector3 additionConstantA111;
  CVector3 additionConstant222;
  CVector3 additionConstantNeg100;
  CVector3 constantMultiplier000;
  CVector3 constantMultiplier001;
  CVector3 constantMultiplier010;
  CVector3 constantMultiplier100;
  CVector3 constantMultiplier111;
  CVector3 constantMultiplier121;
  CVector3 constantMultiplier122;
  CVector3 constantMultiplier221;
  CVector3 constantMultiplier222;
  CVector3 constantMultiplier441;
  CVector3 juliaC;
  CVector3 offset000;
  CVector3 offset200;
  CVector3 power025;
  CVector3 power8;
  CVector3 rotation;
  CVector3 scale3D111;
  CVector3 scale3D222;
  CVector3 scale3D333;
  CVector3 scale3D444;

  CVector4 additionConstant0000;
  CVector4 constantMultiplier1220;

  CRotationMatrix rotationMatrix;
  CRotationMatrix tempRotmatrix;

  bool addCpixelEnabled;
  bool addCpixelEnabledFalse;
  bool benesiT1Enabled;
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
        sFractalMandelbulbMulti mandelbulbMulti;
        sFractalDonut donut;

#ifdef CLSUPPORT
	double customParameters[15];
	double deltaDEStep;
	char customOCLFormulaName[100];
	fractal::enumOCLDEMode customOCLFormulaDEMode;
#endif

};

#endif /* FRACTAL_H_ */
