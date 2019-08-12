/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * sFractal struct - container for fractal formula parameters
 */

#ifndef MANDELBULBER2_SRC_FRACTAL_H_
#define MANDELBULBER2_SRC_FRACTAL_H_

#include "algebra.hpp"
#include "fractal_list.hpp"

#ifndef M_PI_180
#define M_PI_180 0.01745329251994329576923690768489
#endif

// forward declarations
class cParameterContainer;

const int IFS_VECTOR_COUNT = 9;
const int HYBRID_COUNT = 5;
const int MANDELBOX_FOLDS = 2;

namespace fractal
{
enum enumOCLDEMode
{
	ocl_DECalculated = 0,
	ocl_deltaDE = 1,
	ocl_noDE = 2
};
}

enum enumGeneralizedFoldBoxType
{
	generalizedFoldBoxType_foldTet = 0,
	generalizedFoldBoxType_foldCube = 1,
	generalizedFoldBoxType_foldOct = 2,
	generalizedFoldBoxType_foldDodeca = 3,
	generalizedFoldBoxType_foldOctCube = 4,
	generalizedFoldBoxType_foldIcosa = 5,
	generalizedFoldBoxType_foldBox6 = 6,
	generalizedFoldBoxType_foldBox5 = 7
};
struct sExtendedAux
{
	int i;

	CVector4 c;
	CVector4 const_c;
	CVector4 old_z;
	CVector4 sum_z;
	double pos_neg;
	double cw;

	double r;
	double DE;
	double dist;
	double pseudoKleinianDE;
	double linearDE;

	double actualScale;
	double actualScaleA;

	double color;
	double colorHybrid;

	double temp1000;
	double addDist;
};

struct sFoldColor
{
	CVector3 factor;

	CVector4 factor4D;
	double factorR;
	double factorSp1;
	double factorSp2;
	double scaleA0;
	double scaleB0;
	double scaleD0;
	double scaleF0;
	double scaleA1;
	double scaleB1;
	double scaleC1;

	int intAx0;
	int intAy0;
	int intAz0;

	bool auxColorEnabled;
	bool auxColorEnabledFalse;

	bool distanceEnabledFalse;
};

struct sFractalGeneralizedFoldBox
{
	enumGeneralizedFoldBoxType type;
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
	bool absX;
	bool absY;
	bool absZ;
	bool enabled[IFS_VECTOR_COUNT];
	bool mengerSpongeMode;
	bool rotationEnabled;
	bool edgeEnabled;
	CRotationMatrix mainRot;
	CRotationMatrix rot[IFS_VECTOR_COUNT];
	CVector4 direction[IFS_VECTOR_COUNT];
	CVector3 edge;
	CVector4 offset;
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
	CVector4 offset;
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
	double foldFactor;
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
enum enumMulti_acosOrAsin
{
	multi_acosOrAsin_acos,
	multi_acosOrAsin_asin
};

enum enumMulti_atanOrAtan2
{
	multi_atanOrAtan2_atan,
	multi_atanOrAtan2_atan2
};

enum enumMulti_OrderOfXYZ
{
	multi_OrderOfXYZ_xyz,
	multi_OrderOfXYZ_xzy,
	multi_OrderOfXYZ_yxz,
	multi_OrderOfXYZ_yzx,
	multi_OrderOfXYZ_zxy,
	multi_OrderOfXYZ_zyx
};
struct sFractalMandelbulbMulti
{
	enumMulti_acosOrAsin acosOrAsin;
	enumMulti_acosOrAsin acosOrAsinA;
	enumMulti_atanOrAtan2 atanOrAtan2;
	enumMulti_atanOrAtan2 atanOrAtan2A;

	enumMulti_OrderOfXYZ orderOfXYZ;
	enumMulti_OrderOfXYZ orderOfXYZ2;
	enumMulti_OrderOfXYZ orderOfXYZC;
};

// sinTan2Trig
enum enumMulti_asinOrAcos
{
	multi_asinOrAcos_asin,
	multi_asinOrAcos_acos
};

enum enumMulti_atan2OrAtan
{
	multi_atan2OrAtan_atan2,
	multi_atan2OrAtan_atan
};

enum enumMulti_OrderOfZYX
{
	multi_OrderOfZYX_zyx,
	multi_OrderOfZYX_zxy,
	multi_OrderOfZYX_yzx,
	multi_OrderOfZYX_yxz,
	multi_OrderOfZYX_xzy,
	multi_OrderOfZYX_xyz
};
struct sFractalSinTan2Trig
{
	enumMulti_asinOrAcos asinOrAcos;
	enumMulti_atan2OrAtan atan2OrAtan;
	enumMulti_OrderOfZYX orderOfZYX;
};

// surf fold box
enum enumMulti_orderOfFolds
{
	multi_orderOfFolds_type1,
	multi_orderOfFolds_type2,
	multi_orderOfFolds_type3,
	multi_orderOfFolds_type4,
	multi_orderOfFolds_type5
};
struct sFractalSurfFolds
{
	enumMulti_orderOfFolds orderOfFolds1;
	enumMulti_orderOfFolds orderOfFolds2;
	enumMulti_orderOfFolds orderOfFolds3;
	enumMulti_orderOfFolds orderOfFolds4;
	enumMulti_orderOfFolds orderOfFolds5;
};

// asurf mod2
enum enumMulti_orderOf3Folds
{
	multi_orderOf3Folds_type1,
	multi_orderOf3Folds_type2,
	multi_orderOf3Folds_type3
};
struct sFractalASurf3Folds
{
	enumMulti_orderOf3Folds orderOf3Folds1;
	enumMulti_orderOf3Folds orderOf3Folds2;
	enumMulti_orderOf3Folds orderOf3Folds3;
};

// benesi mag transforms
enum enumMulti_orderOfTransf
{
	multi_orderOfTransf_typeT1,
	multi_orderOfTransf_typeT1Mod,
	multi_orderOfTransf_typeT2,
	multi_orderOfTransf_typeT3,
	multi_orderOfTransf_typeT4,
	multi_orderOfTransf_typeT5b,
};
struct sFractalMagTransforms
{
	enumMulti_orderOfTransf orderOfTransf1;
	enumMulti_orderOfTransf orderOfTransf2;
	enumMulti_orderOfTransf orderOfTransf3;
	enumMulti_orderOfTransf orderOfTransf4;
	enumMulti_orderOfTransf orderOfTransf5;
};

// combo4
enum enumMulti_combo4
{
	multi_combo4_type1,
	multi_combo4_type2,
	multi_combo4_type3,
	multi_combo4_type4,
};
struct sFractalCombo4
{
	enumMulti_combo4 combo4;
};

// combo5
enum enumMulti_combo5
{
	multi_combo5_type1,
	multi_combo5_type2,
	multi_combo5_type3,
	multi_combo5_type4,
	multi_combo5_type5,
};
struct sFractalCombo5
{
	enumMulti_combo5 combo5;
};

// combo6
enum enumMulti_combo6
{
	multi_combo6_type1,
	multi_combo6_type2,
	multi_combo6_type3,
	multi_combo6_type4,
	multi_combo6_type5,
	multi_combo6_type6,
};
struct sFractalCombo6
{
	enumMulti_combo6 combo6;
};

// basic combo
enum enumCombo
{
	combo_mode0,
	combo_mode1,
	combo_mode2,
	combo_mode3,
	combo_mode4,
	combo_mode5,
	combo_mode6,
	combo_mode7,
};
struct sFractalCombo
{
	enumCombo modeA;
	//		combo modeB;
	//		combo modeC;
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
	CVector4 offset1A111;
	CVector4 offset1B111;
	CVector4 offset2A111;
	CVector4 offset2B111;
	CVector4 offset3A111;
	CVector4 offset3B111;
	CVector4 offset1A222;
	CVector4 offset1B222;
	double scale1Z1;
};

// for curvilinear
struct sFractalCpara
{
	bool enabledLinear;
	bool enabledCurves;
	bool enabledParabFalse;
	bool enabledParaAddP0;
	double para00;
	double paraA0;
	double paraB0;
	double paraC0;
	double parabOffset0;
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
	bool enabled;
	bool enabledFalse;
	// bool enabledAuxR2False; // only used once. Remove
	double scale1;
	double tweak005;
	double offset0;
	double offset1;
	double offset2;
	// double factor2;
	// double scaleLin; // out of date name, only 4 uses. Remove
	// double offsetLin; // out of date name, only 4 uses. Remove
};

// common parameters for transforming formulas
struct sFractalTransformCommon
{
	double angle0;
	double alphaAngleOffset;
	double betaAngleOffset;
	double foldingValue;
	double foldingLimit;
	double offset;
	double offset0;
	double offsetA0;
	double offsetB0;
	double offsetC0;
	double offsetD0;
	double offset0005;
	double offset05;
	double offset1;
	double offsetA1;
	double offset105;
	double offset2;
	double offset4;
	double multiplication;
	double minR0;
	double minR05;
	double minR06;
	double minR2p25;
	double maxR2d1;
	double maxMinR2factor;
	double scale;
	double scale0;
	double scale025;
	double scale05;
	double scale08;
	double scale1;
	double scaleA1;
	double scaleB1;
	double scaleC1;
	double scaleD1;
	double scaleE1;
	double scaleF1;
	double scaleG1;
	double scaleA2;
	double scale015;
	double scale2;
	double scale3;
	double scaleA3;
	double scaleB3;
	double scale4;
	double scale8;

	double scaleMain2;
	double scaleVary0;

	double pwr05;
	double pwr4;
	double pwr8;
	double pwr8a;
	double sqtR;
	double mboxFactor1;

	int startIterations;
	int startIterations250;
	int stopIterations;
	int stopIterations15;
	int startIterationsA;
	int stopIterationsA;
	int startIterationsB;
	int stopIterationsB;
	int startIterationsC;
	int stopIterationsC;
	int stopIterationsC1;
	int startIterationsD;
	int stopIterationsD;
	int stopIterationsD1;
	int startIterationsE;
	int stopIterationsE;
	int startIterationsF;
	int stopIterationsF;
	int startIterationsG;
	int stopIterationsG;
	int startIterationsH;
	int stopIterationsH;
	int startIterationsI;
	int stopIterationsI;
	int startIterationsJ;
	int stopIterationsJ;
	int startIterationsK;
	int stopIterationsK;
	int startIterationsM;
	int stopIterationsM;
	int startIterationsO;
	int stopIterationsO;
	int startIterationsP;
	int stopIterationsP1;
	int startIterationsR;
	int stopIterationsR;
	int startIterationsRV;
	int stopIterationsRV;
	int startIterationsS;
	int stopIterationsS;
	int startIterationsT;
	int stopIterationsT;
	int stopIterationsT1;
	int startIterationsTM;
	int stopIterationsTM1;
	int startIterationsX;
	int stopIterationsX;
	int startIterationsY;
	int stopIterationsY;
	int startIterationsZ;
	int stopIterationsZ;

	int stopIterations1;

	int intA;
	int intB;
	int int1;
	int int2;
	int int3;
	int int6;
	int int8X;
	int int8Y;
	int int8Z;

	CVector4 additionConstant0555;
	CVector4 additionConstant0777;
	CVector4 additionConstant000;
	CVector4 additionConstantA000;
	CVector4 additionConstantP000;
	CVector4 additionConstant111;
	CVector4 additionConstantA111;
	CVector4 additionConstant222;
	CVector4 additionConstantNeg100;
	CVector4 constantMultiplier000;
	CVector4 constantMultiplier001;
	CVector4 constantMultiplier010;
	CVector4 constantMultiplier100;
	CVector4 constantMultiplierA100;
	CVector4 constantMultiplier111;
	CVector4 constantMultiplierA111;
	CVector4 constantMultiplierB111;
	CVector4 constantMultiplierC111;
	CVector4 constantMultiplier121;
	CVector4 constantMultiplier122;
	CVector4 constantMultiplier221;
	CVector4 constantMultiplier222;
	CVector4 constantMultiplier441;
	CVector4 juliaC;
	CVector4 offset000;
	CVector4 offsetA000;
	CVector4 offsetF000;
	CVector4 offset001;
	CVector4 offset002;
	CVector4 offset010;
	CVector4 offset100;
	CVector4 offset1105;
	CVector4 offset111;
	CVector4 offsetA111;
	CVector4 offsetB111;
	CVector4 offsetC111;
	CVector4 offset200;
	CVector4 offsetA200;
	CVector4 offset222;
	CVector4 offsetA222;
	CVector4 power025;
	CVector4 power8;
	CVector4 vec111;

	CVector3 rotation; // vec3s
	CVector3 rotation2;
	CVector3 rotationVary;
	CVector3 rotation44a; //.........................
	CVector3 rotation44b; //..........................

	CVector4 scaleP222;
	CVector4 scale3D000;
	CVector4 scale3D111;
	CVector4 scale3D222;
	CVector4 scale3Da222;
	CVector4 scale3Db222;
	CVector4 scale3Dc222;
	CVector4 scale3Dd222;
	CVector4 scale3D333;
	CVector4 scale3D444;

	CVector4 additionConstant0000;
	CVector4 offset0000;
	CVector4 offsetA0000;
	CVector4 offset1111;
	CVector4 offsetA1111;
	CVector4 offsetB1111;
	CVector4 offsetNeg1111;
	CVector4 offset2222;
	CVector4 additionConstant111d5;
	CVector4 constantMultiplier1220;
	CVector4 scale0000;
	CVector4 scale1111;

	CRotationMatrix rotationMatrix;
	CRotationMatrix rotationMatrix2;
	CRotationMatrix rotationMatrixVary;
	CRotationMatrix44 rotationMatrix44; //....................

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
	bool functionEnabledw;
	bool functionEnabledxFalse;
	bool functionEnabledyFalse;
	bool functionEnabledzFalse;
	bool functionEnabledwFalse;
	bool functionEnabledAx;
	bool functionEnabledAy;
	bool functionEnabledAz;
	bool functionEnabledAw;
	bool functionEnabledAxFalse;
	bool functionEnabledAyFalse;
	bool functionEnabledAzFalse;
	bool functionEnabledAwFalse;
	bool functionEnabledBx;
	bool functionEnabledBy;
	bool functionEnabledBz;
	bool functionEnabledBxFalse;
	bool functionEnabledByFalse;
	bool functionEnabledBzFalse;
	bool functionEnabledCx;
	bool functionEnabledCy;
	bool functionEnabledCz;
	bool functionEnabledCxFalse;
	bool functionEnabledCyFalse;
	bool functionEnabledCzFalse;
	bool functionEnabledDFalse;
	bool functionEnabledEFalse;
	bool functionEnabledFFalse;
	bool functionEnabledJFalse;
	bool functionEnabledKFalse;
	bool functionEnabledM;
	bool functionEnabledMFalse;
	bool functionEnabledPFalse;
	bool functionEnabledRFalse;
	bool functionEnabledSFalse;
	bool functionEnabledSwFalse;
	bool functionEnabledXFalse;
	bool juliaMode;
	bool rotationEnabled;
	bool rotation2EnabledFalse;
	bool sphereInversionEnabledFalse;
	bool spheresEnabled;

	// bool functionEnabledTempFalse;
};

struct sFractal
{
	sFractal(const cParameterContainer *par);
	void RecalculateFractalParams();

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
	sFractalCombo combo;
	sFractalASurf3Folds aSurf3Folds;
	sFractalCombo4 combo4;
	sFractalCombo5 combo5;
	sFractalCombo6 combo6;

#ifdef USE_OPENCL
//	double customParameters[15];
//	double deltaDEStep;
//	char customOCLFormulaName[100];
//	fractal::enumOCLDEMode customOCLFormulaDEMode;
#endif
};

#endif /* MANDELBULBER2_SRC_FRACTAL_H_ */
