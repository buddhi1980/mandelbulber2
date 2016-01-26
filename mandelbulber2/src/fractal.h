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
	bool foldingMode; // Kaleidoscopic IFS folding mode
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
	int foldingCount;
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





//----------------------Structure Transform---------------------------------
struct sTransformControl
{
	bool enabled;
	bool weightEnabled;
	bool oldWeightEnabled;
	int startIterations;
	int stopIterations;
	double weight;
	double oldWeight;
};

struct sTransformAdditionConstant
{
	sTransformControl control;
	CVector3 additionConstant;
	// for 4D
	CVector4 additionConstant4D;

};
struct sTransformBoxConstantMultiplier
{
	sTransformControl control;
	bool boxConstantMultiplierFabsEnabledX;
	bool boxConstantMultiplierFabsEnabledY;
	bool boxConstantMultiplierFabsEnabledZ;
	CVector3 boxConstantMultiplier;
};
struct sTransformBoxFoldOriginal
{
	sTransformControl control;
	sFoldColor color;
	double foldingLimit;
	double foldingValue;
};
struct sTransformBoxFold
{
	sTransformControl control;
	sFoldColor color;
	double foldingLimit;
	double foldingValue;
};
struct sTransformBoxOffset
{
	sTransformControl control;
	sFoldColor color;
	CVector3 boxOffset;
};
struct sTransformConstantMultiplierOriginal
{
	sTransformControl control;
	CVector3 constantMultiplierVect;
	//extra for 4D
	CVector4 constantMultiplierVect4D;
};
struct sTransformConstantMultiplier
{
	sTransformControl control;
	CVector3 constantMultiplierVect;
	//extra for 4D
	CVector4 constantMultiplierVect4D;
};
struct sTransformFabsAddConstant
{
	sTransformControl control;
	bool fabsAddConstantEnabledx;
	bool fabsAddConstantEnabledy;
	bool fabsAddConstantEnabledz;
	bool fabsAddConstantEnabledw;
	CVector3 fabsAddConstantA;
	CVector3 fabsAddConstantB;
	CVector3 fabsAddConstantC;
	// for 4D
	CVector4 fabsAddConstant4DA;
	CVector4 fabsAddConstant4DB;
	CVector4 fabsAddConstant4DC;
};
struct sTransformFabsFormulaAB
{
	sTransformControl control;
	bool fabsFormulaABEnabledx;
	bool fabsFormulaABEnabledy;
	bool fabsFormulaABEnabledz;
	bool fabsFormulaABEnabledw;
	CVector3 fabsFormulaABConstantA;
	CVector3 fabsFormulaABConstantB;
};
struct sTransformFabsFormulaABCD
{
	sTransformControl control;
	bool fabsFormulaABCDEnabledAx;
	bool fabsFormulaABCDEnabledAy;
	bool fabsFormulaABCDEnabledAz;
	bool fabsFormulaABCDEnabledBx;
	bool fabsFormulaABCDEnabledBy;
	bool fabsFormulaABCDEnabledBz;
	CVector3 fabsFormulaABCDA;
	CVector3 fabsFormulaABCDB;
	CVector3 fabsFormulaABCDC;
	CVector3 fabsFormulaABCDD;
	// for 4D
	CVector4 fabsFormulaABCD4DA;
	CVector4 fabsFormulaABCD4DB;
	CVector4 fabsFormulaABCD4DC;
	CVector4 fabsFormulaABCD4DD;
	bool fabsFormulaABCDEnabledAw;
	bool fabsFormulaABCDEnabledBw;
};
struct sTransformFabsFormulaZAB
{
	sTransformControl control;
	bool fabsFormulaZABEnabledx;
	bool fabsFormulaZABEnabledy;
	bool fabsFormulaZABEnabledz;
	CVector3 fabsFormulaZABConstantA;
	CVector3 fabsFormulaZABConstantB;
	CVector3 fabsFormulaZABConstantC;
};
struct sTransformFabsSubConstant
{
	sTransformControl control;
	bool fabsSubConstantEnabledx;
	bool fabsSubConstantEnabledy;
	bool fabsSubConstantEnabledz;
	bool fabsSubConstantEnabledw;
	CVector3 fabsSubConstantA;
	CVector3 fabsSubConstantB;
};

struct sTransformIterationWeight
{
	sTransformControl control;
	int iterationWeightIterationA;
	int iterationWeightIterationB;
	double iterationWeightConstantZ;
	double iterationWeightConstantA;
	double iterationWeightConstantB;
};
struct sTransformMandelbulbOriginal
{
	sTransformControl control;
	double power;
	double alphaAngleOffset;
	double betaAngleOffset;
	double gammaAngleOffset;
};
struct sTransformMandelbulb
{
	sTransformControl control;
	double power;
	double alphaAngleOffset;
	double betaAngleOffset;
};

struct sTransformMainRotation
{
	sTransformControl control;
	CVector3 mainRotation;
	CRotationMatrix mainRot;
};

struct sTransformMengerSpongeOriginal
{
	sTransformControl control;
	double mengerSpongeOriginalConstantZ;
	CVector3 mengerSpongeOriginalFactorConstantVect;
};
struct sTransformMengerSponge
{
	sTransformControl control;
	double mengerSpongeConstantZ;
	CVector3 mengerSpongeFactorConstantVect;
};
struct sTransformQuaternionOriginal
{
	sTransformControl control;
	CVector4 quaternionOriginalFactorConstant;
};
struct sTransformQuaternion
{
	sTransformControl control;
	CVector4 quaternionFactorConstant;
};
struct sTransformScaleOriginal
{
	sTransformControl control;
	double scaleOriginal;
};
struct sTransformScale
{
	sTransformControl control;
	double scale;
};
struct sTransformSphericalFoldOriginal
{
	sTransformControl control;
	sFoldColor color;
	double radMin;
	double radFixed;
	double fR2;
	double mR2;
	double mboxFactor1;
};

struct sTransformSphericalFold
{
	sTransformControl control;
	sFoldColor color;
	double radMin;
	double radFixed;
	double fR2;
	double mR2;
	double mboxFactor1;
};

struct sTransformSphericalOffset
{
	sTransformControl control;
	double offsetRadius;
	double scale;
};

struct sTransformVariableConstantMultiplier
{
	sTransformControl control;
	CVector3 constantMultiplier;
	CVector3 variableConstant;
	int variableStartIterations;
	int variableStopIterations;
};
struct sTransformVariableMandelbulbPower
{
	sTransformControl control;
	double power;
	double alphaAngleOffset;
	double betaAngleOffset;
	double variableConstant;
	int variableStartIterations;
	int variableStopIterations;
};
struct sTransformVariableScale
{
	sTransformControl control;
	CVector3 scale;
	CVector3 variableConstant;
	int variableStartIterations;
	int variableStopIterations;
};

struct sTransformColoringParameters
{
	sTransformControl control;
	sFoldColor color;
};

struct sTransformColorTrial
{
	sTransformControl control;
	sFoldColor color;
	bool axisBiasEnabled;
	bool orbitTrapsEnabled;
	bool transformSamplingEnabled;
	double minimumRWeight;
	double colorConstant1;
	double colorConstant2;
	double colorConstant3;
	//double  sampleConstant1;
	//double  sampleConstant2;
	//double  sampleConstant3;
	//double  sampleConstant4;
	//double  sampleConstant5;
	//double  sampleConstant6;
	double mainOrbitTrapWeight;
	double orbitTrap1Weight;
	double orbitTrap2Weight;
	CVector3 orbitTrap1;
	CVector3 orbitTrap2;
};

struct sTransformMandelbulbMulti
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
	sTransformControl control;
	multi_acosOrasin acosOrasin;
	multi_atanOratan2 atanOratan2;
	multi_OrderOfxyz orderOfxyz;
	CVector3 mainRotation;
	CRotationMatrix mainRot;
	double power;
	double alphaAngleOffset;
	double betaAngleOffset;
};

struct sTransformBenesiMagForwardTransformOne
{
	sTransformControl control;
};

struct sTransformBenesiMagBackTransformOne
{
	sTransformControl control;
};

struct sTransformBenesiMagTransformOne
{
	sTransformControl control;
	CVector3 offset;
	CVector3 scale;
};

struct sTransformBenesiMagTransformOneEnabled
{
	sTransformControl control;
	CVector3 offset;
	CVector3 scale;
};

struct sTransformBenesiMagTransformOnePlusMinus  // nine spot
{
	sTransformControl control;
	CVector3 offset;
	CVector3 scale;
};

struct sTransformBenesiMagTransformTwo
{
	sTransformControl control;
	CVector3 offset;
	CVector3 scale;
};

struct sTransformBenesiMagTransformThree
{
	sTransformControl control;
	CVector3 offset;
	CVector3 scale;
};

struct sTransformBenesiMagTransformFour
{
	sTransformControl control;
	CVector3 offset;
	CVector3 scale;
};

struct sTransformBenesiMagTransformFiveB
{
	sTransformControl control;
	CVector3 offset;
	CVector3 scale;
	CVector3 powOne;
	CVector3 powTwo;
};

struct sTransformBenesiFastPwr2PineTree
{
	sTransformControl control;
	CVector3 constantMultiplierVect;
};
struct sTransformBenesiFastPwr2PineTreeEnabled
{
	sTransformControl control;
	CVector3 constantMultiplierVect;
};

struct sTransformBoxFoldMulti
{
	sTransformControl control;
	sFoldColor color;
	CVector3 foldingLimit;
	CVector3 foldingValue;
};

struct sTransformCubeSphere
{
	sTransformControl control;
};

struct sTransformSphereCube
{
	sTransformControl control;
};

struct sTransformFabsBoxMod
{
  sTransformControl control;
  CVector3 constantF1;
  CVector3 constantF2;
  CVector3 constantF3;
  bool constantF1Enabledx;
  bool constantF1Enabledy;
  bool constantF1Enabledz;
  bool constantF2Enabledx;
  bool constantF2Enabledy;
  bool constantF2Enabledz;

};

struct sFractalTransform
{
	sTransformAdditionConstant additionConstant1;
	sTransformAdditionConstant additionConstant2;
	sTransformBoxConstantMultiplier boxConstantMultiplier1;
	sTransformBoxConstantMultiplier boxConstantMultiplier2;
	sTransformBoxFoldOriginal boxFoldOriginal1;
	sTransformBoxFold boxFold1;
	sTransformBoxFold boxFold2;
	sTransformBoxOffset boxOffset1;
	sTransformBoxOffset boxOffset2;
	sTransformConstantMultiplierOriginal constantMultiplierOriginal1;
	sTransformConstantMultiplier constantMultiplier1;
	sTransformConstantMultiplier constantMultiplier2;
	sTransformFabsAddConstant fabsAddConstant1;
	sTransformFabsFormulaAB fabsFormulaAB1;
	sTransformFabsFormulaAB fabsFormulaAB2;
	sTransformFabsFormulaABCD fabsFormulaABCD1;
	sTransformFabsFormulaABCD fabsFormulaABCD2;
	sTransformFabsFormulaZAB fabsFormulaZAB1;
	sTransformFabsSubConstant fabsSubConstant1;
	sTransformIterationWeight iterationWeight1;
	sTransformMandelbulbOriginal mandelbulbOriginal1;
	sTransformMandelbulb mandelbulb1;
	sTransformMainRotation mainRotation1;
	sTransformMainRotation mainRotation2;
	sTransformMainRotation mainRotation3;
	sTransformMainRotation mainRotation4;
	sTransformMengerSpongeOriginal mengerSpongeOriginal1;
	sTransformMengerSponge mengerSponge1;
	sTransformMengerSponge mengerSponge2;
	sTransformQuaternionOriginal quaternionOriginal1;
	sTransformQuaternion quaternion1;
	sTransformScaleOriginal scaleOriginal1;
	sTransformScale scale1;
	sTransformScale scale2;
	sTransformSphericalFoldOriginal sphericalFoldOriginal1;
	sTransformSphericalFold sphericalFold1;
	sTransformSphericalFold sphericalFold2;
	sTransformSphericalOffset sphericalOffset1;
	sTransformSphericalOffset sphericalOffset2;
	sTransformVariableConstantMultiplier variableConstantMultiplier1;
	sTransformVariableMandelbulbPower variableMandelbulbPower1;
	sTransformVariableScale variableScale1;
	sTransformVariableScale variableScale2;
	sTransformColorTrial colorTrial1;
	sTransformColoringParameters coloringParameters1;
	sTransformMandelbulbMulti mandelbulbMulti1;
	sTransformMandelbulbMulti mandelbulbMulti2;

	sTransformBenesiMagForwardTransformOne benesiMagForwardTransformOne1;
	sTransformBenesiMagForwardTransformOne benesiMagForwardTransformOne2;
	sTransformBenesiMagBackTransformOne benesiMagBackTransformOne1;
	sTransformBenesiMagBackTransformOne benesiMagBackTransformOne2;

	sTransformBenesiMagTransformOneEnabled benesiMagTransformOneEnabled1;
	sTransformBenesiMagTransformOne benesiMagTransformOne1;
	sTransformBenesiMagTransformOne benesiMagTransformOne2;
	sTransformBenesiMagTransformOne benesiMagTransformOne3;
	sTransformBenesiMagTransformOnePlusMinus benesiMagTransformOnePlusMinus1;
	sTransformBenesiMagTransformOnePlusMinus benesiMagTransformOnePlusMinus2;
	sTransformBenesiMagTransformTwo benesiMagTransformTwo1;
	sTransformBenesiMagTransformTwo benesiMagTransformTwo2;
	sTransformBenesiMagTransformThree benesiMagTransformThree1;
	sTransformBenesiMagTransformThree benesiMagTransformThree2;
	sTransformBenesiMagTransformFour benesiMagTransformFour1;
	sTransformBenesiMagTransformFour benesiMagTransformFour2;
	sTransformBenesiMagTransformFiveB benesiMagTransformFiveB1;
	sTransformBenesiMagTransformFiveB benesiMagTransformFiveB2;

	sTransformBenesiFastPwr2PineTreeEnabled benesiFastPwr2PineTreeEnabled1;
	sTransformBenesiFastPwr2PineTree benesiFastPwr2PineTree1;
	sTransformBenesiFastPwr2PineTree benesiFastPwr2PineTree2;
	sTransformBenesiFastPwr2PineTree benesiFastPwr2PineTree3;

	sTransformBoxFoldMulti boxFoldMulti1;
	sTransformBoxFoldMulti boxFoldMulti2;

	sTransformCubeSphere cubeSphere1;
	sTransformCubeSphere cubeSphere2;
	sTransformSphereCube sphereCube1;
	sTransformSphereCube sphereCube2;

  sTransformFabsBoxMod fabsBoxMod;


	//TODO here will be more transforms
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
  double multiplication;
  double minR0;
  double minR05;
  double minR06;
  double scale;
  double scale0;
  double scale1;
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
  CVector3 additionConstant111;
  CVector3 additionConstantA111;
  CVector3 additionConstant222;
  CVector3 additionConstantNeg100;
  CVector3 constantMultiplier000;
  CVector3 constantMultiplier001;
  CVector3 constantMultiplier010;
  CVector3 constantMultiplier100;
  CVector3 constantMultiplier111;
  CVector3 constantMultiplier122;
  CVector3 constantMultiplier221;
  CVector3 constantMultiplier441;
  CVector3 juliaC;
  CVector3 offset000;
  CVector3 offset200;
  CVector3 power025;
  CVector3 power8;
	CVector3 rotation;
  CRotationMatrix rotationMatrix;
  CVector3 scale3D111;
  CVector3 scale3D222;
  CVector3 scale3D333;
  CVector3 scale3D444;

  CVector4 additionConstant0000;
  CVector4 constantMultiplier1220;

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
	sFractalTransform transform;
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
