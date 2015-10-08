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
	calcModeNormal = 0, calcModeColouring = 1, calcModeFake_AO = 2, calcModeDeltaDE1 = 3, calcModeDeltaDE2 = 4, calcModeOrbitTrap = 5
};

enum enumGeneralizedFoldBoxType
{
	foldTet = 0, foldCube = 1, foldOct = 2, foldDodeca = 3, foldOctCube = 4, foldIcosa = 5, foldBox6 = 6, foldBox5 = 7
};

enum enumOCLDEMode
{
	ocl_DEcalculated = 0, ocl_deltaDE = 1, ocl_noDE = 2
};
}

struct sMandelbulbAux
{
	double r_dz;
	double r;
};

struct sMandelboxAux
{
	double mboxDE;
	double mboxColor;
	double actualScale;
};

struct sIFSAux
{
	double ifsDE;
};

struct sAexionAux
{
	CVector3 c;
	double cw;
	int iterNo;
};

struct sExtendedAux
{
	double r_dz;
	double r;
	double DE;
	double color;
	double actualScale;
};

struct sFoldColor
{
	CVector3 factor;
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

struct sMsltoeSym2
{
	double y_multiplier;
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
  double mandelbulbPower;
  double mandelbulbAlphaAngleOffset;
  double mandelbulbBetaAngleOffset;
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
  CVector4  quaternionOriginalFactorConstant;
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
  double mandelbulbPower;
  double mandelbulbAlphaAngleOffset;
  double mandelbulbBetaAngleOffset;
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
  sTransformConstantMultiplier  constantMultiplier1;
  sTransformConstantMultiplier  constantMultiplier2;
  sTransformFabsAddConstant fabsAddConstant1;
  sTransformFabsFormulaAB fabsFormulaAB1;
  sTransformFabsFormulaAB fabsFormulaAB2;
  sTransformFabsFormulaABCD fabsFormulaABCD1;
  sTransformFabsFormulaABCD fabsFormulaABCD2;
  sTransformFabsFormulaZAB fabsFormulaZAB1;
  sTransformFabsSubConstant fabsSubConstant1;
  sTransformIterationWeight iterationWeight1;
  sTransformMandelbulbOriginal  mandelbulbOriginal1;
  sTransformMandelbulb  mandelbulb1;
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

  //TODO here will be more transforms
};


//----------------------------------------------------------
struct sFractalPlatonicSolid
{
	double frequency;
	double amplitude;
	double rhoMul;
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
	sMsltoeSym2 msltoeSym2;
  sFractalTransform transform;
	sFractalPlatonicSolid platonicSolid;

#ifdef CLSUPPORT
	double customParameters[15];
	double deltaDEStep;
	char customOCLFormulaName[100];
	fractal::enumOCLDEMode customOCLFormulaDEMode;
#endif


};

#endif /* FRACTAL_H_ */
