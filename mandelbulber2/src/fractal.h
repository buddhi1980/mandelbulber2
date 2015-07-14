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
	CVector3 colorFactor;
	double colorFactorR;
	double colorFactorSp1;
	double colorFactorSp2;
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

struct sFractalMandelbulb5
{
    bool boxFold1Enabled;
    int boxFold1StartIterations;
    int boxFold1StopIterations;
    double boxFold1FoldingLimit;
    double boxFold1FoldingValue;
    double boxFold1Weight;

    bool mainRotation1Enabled;
    int mainRotation1StartIterations;
    int mainRotation1StopIterations;
    CVector3 mainRotation1;
    CRotationMatrix mainRot1;
    double mainRotation1Weight;


    bool fabsAddConstant1Enabled;
    bool fabsAddConstant1Enabledx;
    bool fabsAddConstant1Enabledy;
    bool fabsAddConstant1Enabledz;
    int fabsAddConstant1StartIterations;
    int fabsAddConstant1StopIterations;
    CVector3 fabsAddConstant1A;
    CVector3 fabsAddConstant1B;
    CVector3 fabsAddConstant1C;
    double fabsAddConstant1Weight;

    bool constantMultiplier1Enabled;
    int constantMultiplier1StartIterations;
    int constantMultiplier1StopIterations;
    CVector3 constantMultiplier1Vect;
    double constantMultiplier1Weight;

    bool fabsSubConstant1Enabled;
    bool fabsSubConstant1Enabledx;
    bool fabsSubConstant1Enabledy;
    bool fabsSubConstant1Enabledz;
    int fabsSubConstant1StartIterations;
    int fabsSubConstant1StopIterations;
    CVector3 fabsSubConstant1A;
    CVector3 fabsSubConstant1B;
    double fabsSubConstant1Weight;

    bool fabsFormulaZAB1Enabled;
    bool fabsFormulaZAB1Enabledx;
    bool fabsFormulaZAB1Enabledy;
    bool fabsFormulaZAB1Enabledz;
    int fabsFormulaZAB1StartIterations;
    int fabsFormulaZAB1StopIterations;
    CVector3 fabsFormulaZAB1A;
    CVector3 fabsFormulaZAB1B;
    CVector3 fabsFormulaZAB1C;
    double fabsFormulaZAB1Weight;

    bool fabsFormulaAB1Enabled;
    bool fabsFormulaAB1Enabledx;
    bool fabsFormulaAB1Enabledy;
    bool fabsFormulaAB1Enabledz;
    int fabsFormulaAB1StartIterations;
    int fabsFormulaAB1StopIterations;
    CVector3 fabsFormulaAB1A;
    CVector3 fabsFormulaAB1B;
    double fabsFormulaAB1Weight;

    bool fabsFormulaABCD1Enabled;
    bool fabsFormulaABCD1Enabledx;
    bool fabsFormulaABCD1Enabledy;
    bool fabsFormulaABCD1Enabledz;
    int fabsFormulaABCD1StartIterations;
    int fabsFormulaABCD1StopIterations;
    CVector3 fabsFormulaABCD1A;
    CVector3 fabsFormulaABCD1B;
    CVector3 fabsFormulaABCD1C;
    CVector3 fabsFormulaABCD1D;
    double fabsFormulaABCD1Weight;

    bool additionConstant1Enabled;
    int additionConstant1StartIterations;
    int additionConstant1StopIterations;
    CVector3 additionConstant1;
    double additionConstant1Weight;

    bool mainRotation2Enabled;
    int mainRotation2StartIterations;
    int mainRotation2StopIterations;
    CVector3 mainRotation2;
    CRotationMatrix mainRot2;
    double mainRotation2Weight;

    bool mainFormula1Enabled;
    int mainFormula1StartIterations;
    int mainFormula1StopIterations;
    double power;
    double alphaAngleOffset;
    double betaAngleOffset;
    double mainFormula1Weight;

    bool mainRotation3Enabled;
    int mainRotation3StartIterations;
    int mainRotation3StopIterations;
    CVector3 mainRotation3;
    CRotationMatrix mainRot3;
    double mainRotation3Weight;

    bool constantMultiplier2Enabled;
    int constantMultiplier2StartIterations;
    int constantMultiplier2StopIterations;
    CVector3 constantMultiplier2Vect;
    double constantMultiplier2Weight;

    bool additionConstant2Enabled;
    int additionConstant2StartIterations;
    int additionConstant2StopIterations;
    CVector3 additionConstant2;
    double additionConstant2Weight;

    bool fabsFormulaAB2Enabled;
    bool fabsFormulaAB2Enabledx;
    bool fabsFormulaAB2Enabledy;
    bool fabsFormulaAB2Enabledz;
    int fabsFormulaAB2StartIterations;
    int fabsFormulaAB2StopIterations;
    CVector3 fabsFormulaAB2A;
    CVector3 fabsFormulaAB2B;
    double fabsFormulaAB2Weight;

    bool boxFold2Enabled;
    int boxFold2StartIterations;
    int boxFold2StopIterations;
    double boxFold2FoldingLimit;
    double boxFold2FoldingValue;
    double boxFold2Weight;



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
	sFractalMandelbulb5 mandelbulb5;

#ifdef CLSUPPORT
	double customParameters[15];
	double deltaDEStep;
	char customOCLFormulaName[100];
	fractal::enumOCLDEMode customOCLFormulaDEMode;
#endif


};

#endif /* FRACTAL_H_ */
