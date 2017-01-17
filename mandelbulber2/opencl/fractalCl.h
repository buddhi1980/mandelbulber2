/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

// forward declarations
class cParameterContainer;

const cl_int IFS_VECTOR_COUNT = 9;
const cl_int HYBRID_COUNT = 5;
const cl_int MANDELBOX_FOLDS = 2;

namespace fractal
{
typedef enum {
	foldTet = 0,
	foldCube = 1,
	foldOct = 2,
	foldDodeca = 3,
	foldOctCube = 4,
	foldIcosa = 5,
	foldBox6 = 6,
	foldBox5 = 7
} eClenumGeneralizedFoldBoxType;

typedef enum { ocl_DEcalculated = 0, ocl_deltaDE = 1, ocl_noDE = 2 } eClenumOCLDEMode;
}

// struct sMandelbulbAux
//{
//	cl_float r_dz;
//	cl_float r;
//};

// struct sMandelboxAux
//{
//	cl_float mboxDE;
//	cl_float mboxColor;
//	cl_float actualScale;
//};

// struct sIFSAux
//{
//	cl_float ifsDE;
//};

// struct sAexionAux
//{
//	cl_float3 c;
//	cl_float cw;
//	cl_int iterNo;
//};

typedef struct
{
	cl_float r_dz;
	cl_float r;
	cl_float DE;
	cl_float color;
	cl_float actualScale;
	// cl_float newR;
	// cl_float orbitTraps;
	// cl_float axisBias;
	cl_float pseudoKleinianDE;
	cl_float cw;
	cl_float foldFactor;
	cl_float minRFactor;
	cl_float scaleFactor;
	cl_float3 c;
} sClsExtendedAux;

typedef struct
{
	cl_float3 factor;
	cl_float4 factor4D;
	cl_float factorR;
	cl_float factorSp1;
	cl_float factorSp2;
	cl_float colorMin;
	cl_float compFold0;
	cl_float compFold;
	cl_float compMinR;
	cl_float compScale;
} sClsFoldColor;

typedef struct
{
	fractal::enumGeneralizedFoldBoxType type;
	cl_float3 Nv_tet[4];
	cl_float3 Nv_cube[6];
	cl_float3 Nv_oct[8];
	cl_float3 Nv_oct_cube[14];
	cl_float3 Nv_dodeca[12];
	cl_float3 Nv_icosa[20];
	cl_float3 Nv_box6[8];
	cl_float3 Nv_box5[7];
	cl_int sides_tet;
	cl_int sides_cube;
	cl_int sides_oct;
	cl_int sides_oct_cube;
	cl_int sides_dodeca;
	cl_int sides_icosa;
	cl_int sides_box6;
	cl_int sides_box5;
} sClsFractalGeneralizedFoldBox;

typedef struct
{
	cl_int absX, absY, absZ;
	cl_int enabled[IFS_VECTOR_COUNT];
	cl_int mengerSpongeMode;
	cl_int rotationEnabled;
	cl_int edgeEnabled;
	matrix33 mainRot;
	matrix33 rot[IFS_VECTOR_COUNT];
	cl_float3 direction[IFS_VECTOR_COUNT];
	cl_float3 edge;
	cl_float3 offset;
	cl_float3 rotations[IFS_VECTOR_COUNT];
	cl_float distance[IFS_VECTOR_COUNT];
	cl_float intensity[IFS_VECTOR_COUNT];
	cl_float3 rotation;
	cl_float scale;
} sClsFractalIFS;

typedef struct
{
	cl_float fold;
	cl_float minR;
	cl_float scaleVary;
	cl_float wadd;
	cl_float rPower;
} sClsFractalMandelboxVary4D;

typedef struct
{
	cl_float3 rotationMain;
	cl_float3 rotation[MANDELBOX_FOLDS][3];
	sFoldColor color;
	cl_float scale;
	cl_float foldingLimit;
	cl_float foldingValue;
	cl_float foldingSphericalMin;
	cl_float foldingSphericalFixed;
	cl_float sharpness;
	cl_float solid;
	cl_float melt;
	cl_float3 offset;
	cl_int rotationsEnabled;
	cl_int mainRotationEnabled;
	matrix33 mainRot;
	matrix33 rot[MANDELBOX_FOLDS][3];
	matrix33 rotinv[MANDELBOX_FOLDS][3];

	cl_float fR2;
	cl_float mR2;
	cl_float mboxFactor1;
} sClsFractalMandelbox;

typedef struct
{
	cl_float zFactor;
	cl_float foldfactor;
} sClsFractalBoxFoldBulbPow2;

typedef struct
{
	cl_float power;
	cl_float alphaAngleOffset;
	cl_float betaAngleOffset;
	cl_float gammaAngleOffset;
} sClsFractalMandelbulb;

typedef struct
{
	cl_float cadd;
} sClsFractalAexion;

typedef struct
{
	cl_int preabsx;
	cl_int preabsy;
	cl_int preabsz;
	cl_int absx;
	cl_int absy;
	cl_int absz;
	cl_int posz;
} sClsFractalBuffalo;

typedef struct
{
	cl_float ringRadius;
	cl_float ringThickness;
	cl_float factor;
	cl_float number;
} sClsFractalDonut;

//----------------------------------------------------------
typedef struct
{
	cl_float frequency;
	cl_float amplitude;
	cl_float rhoMul;
} sClsFractalPlatonicSolid;

// mandelbulb multi
typedef struct
{
	typedef enum { acos, asin } eClmulti_acosOrasin;

	typedef enum { atan, atan2 } eClmulti_atanOratan2;

	typedef enum { xyz, xzy, yxz, yzx, zxy, zyx } eClmulti_OrderOfxyz;

	multi_acosOrasin acosOrasin;
	multi_acosOrasin acosOrasinA;
	multi_atanOratan2 atanOratan2;
	multi_atanOratan2 atanOratan2A;

	multi_OrderOfxyz orderOfxyz;
	multi_OrderOfxyz orderOfxyz2;
	multi_OrderOfxyz orderOfxyzC;
} sClsFractalMandelbulbMulti;

// sinTan2Trig
typedef struct
{
	typedef enum { asin, acos } eClmulti_asinOracos;

	typedef enum { atan2, atan } eClmulti_atan2Oratan;

	typedef enum { zyx, zxy, yzx, yxz, xzy, xyz } eClmulti_OrderOfzyx;

	multi_asinOracos asinOracos;
	multi_atan2Oratan atan2Oratan;
	multi_OrderOfzyx orderOfzyx;
} sClsFractalSinTan2Trig;

// surf fold box
typedef struct
{
	typedef enum { type1, type2, type3, type4, type5 } eClmulti_orderOfFolds;
	multi_orderOfFolds orderOfFolds1;
	multi_orderOfFolds orderOfFolds2;
	multi_orderOfFolds orderOfFolds3;
	multi_orderOfFolds orderOfFolds4;
	multi_orderOfFolds orderOfFolds5;
} sClsFractalSurfFolds;

// benesi mag transfroms
typedef struct
{
	typedef enum {
		typeT1,
		typeT1Mod,
		typeT2,
		typeT3,
		typeT4,
		typeT5b,
	} eClmulti_orderOfTransf;
	multi_orderOfTransf orderOfTransf1;
	multi_orderOfTransf orderOfTransf2;
	multi_orderOfTransf orderOfTransf3;
	multi_orderOfTransf orderOfTransf4;
	multi_orderOfTransf orderOfTransf5;
} sClsFractalMagTransforms;

// basic combo
typedef struct
{
	typedef enum {
		mode0,
		mode1,
		mode2,
		mode3,
		mode4,
		mode5,
		mode6,
		mode7,
	} eClcombo;
	combo modeA;
	//		combo modeB;
	//		combo modeC;
} sClsFractalCombo;

// for surfbox types
typedef struct
{
	cl_int enabledX1;
	cl_int enabledY1;
	cl_int enabledZ1;
	cl_int enabledX2False;
	cl_int enabledY2False;
	cl_int enabledZ2False;
	cl_int enabledX3False;
	cl_int enabledY3False;
	cl_int enabledZ3False;
	cl_int enabledX4False;
	cl_int enabledY4False;
	cl_int enabledZ4False;
	cl_int enabledX5False;
	cl_int enabledY5False;
	cl_int enabledZ5False;
	cl_float3 offset1A111;
	cl_float3 offset1B111;
	cl_float3 offset2A111;
	cl_float3 offset2B111;
	cl_float3 offset3A111;
	cl_float3 offset3B111;
	cl_float3 offset1A222;
	cl_float3 offset1B222;
	cl_float scale1Z1;
} sClsFractalSurfBox;

// for curvilinear
typedef struct
{
	cl_int enabledLinear;
	cl_int enabledCurves;
	cl_int enabledParabFalse;
	cl_int enabledParaAddP0;
	cl_float para00;
	cl_float paraA0;
	cl_float paraB0;
	cl_float paraC0;
	cl_float parabOffset0;
	cl_float para0;
	cl_float paraA;
	cl_float paraB;
	cl_float paraC;
	cl_float parabOffset;
	cl_float parabSlope;
	cl_float parabScale;
	cl_int iterA;
	cl_int iterB;
	cl_int iterC;
} sClsFractalCpara;

typedef struct
{
	cl_int enabledFalse;
	cl_int enabledAuxR2False;
	cl_float scale1;
	cl_float tweak005;
	cl_float offset0;
	cl_float offset1;
	cl_float offset2;
	cl_float scaleLin;
	cl_float offsetLin;
} sClsFractalAnalyticDE;

// common parameters for transforming formulas
typedef struct
{
	cl_float alphaAngleOffset;
	cl_float betaAngleOffset;
	cl_float foldingValue;
	cl_float foldingLimit;
	cl_float offset;
	cl_float offset0;
	cl_float offsetA0;
	cl_float offsetB0;
	cl_float offsetC0;
	cl_float offset0005;
	cl_float offset05;
	cl_float offset1;
	cl_float offset105;
	cl_float offset2;
	cl_float multiplication;
	cl_float minRneg1;
	cl_float minR0;
	cl_float minR05;
	cl_float minR06;
	cl_float minR2p25;
	cl_float maxR2d1;
	cl_float maxMinR2factor;
	cl_float scale;
	cl_float scale0;
	cl_float scale025;
	cl_float scale05;
	cl_float scale08;
	cl_float scale1;
	cl_float scaleA1;
	cl_float scaleB1;
	cl_float scaleC1;
	cl_float scaleD1;
	cl_float scaleE1;
	cl_float scaleF1;
	cl_float scaleG1;
	cl_float scaleA2;
	cl_float scale015;
	cl_float scale2;
	cl_float scale3;
	cl_float scaleA3;
	cl_float scaleB3;
	cl_float scale4;
	cl_float scale8;
	cl_float pwr05;
	cl_float pwr4;
	cl_float pwr8;
	cl_float pwr8a;
	cl_float sqtR;
	cl_float mboxFactor1;

	cl_int startIterations;
	cl_int startIterations250;
	cl_int stopIterations;
	cl_int startIterationsA;
	cl_int stopIterationsA;
	cl_int startIterationsB;
	cl_int stopIterationsB;
	cl_int startIterationsC;
	cl_int stopIterationsC;
	cl_int startIterationsD;
	cl_int stopIterationsD;
	cl_int startIterationsE;
	cl_int stopIterationsE;
	cl_int startIterationsF;
	cl_int stopIterationsF;
	cl_int startIterationsP;
	cl_int stopIterationsP1;
	cl_int startIterationsR;
	cl_int stopIterationsR;
	cl_int startIterationsS;
	cl_int stopIterationsS;
	cl_int startIterationsT;
	cl_int stopIterationsT;
	cl_int startIterationsM;
	cl_int stopIterationsM;
	cl_int stopIterations1;
	cl_int stopIterationsT1;
	cl_int stopIterationsTM1;
	cl_int intA;
	cl_int intB;
	cl_int int1;
	cl_int int8X;
	cl_int int8Y;
	cl_int int8Z;

	cl_float3 additionConstant0555;
	cl_float3 additionConstant0777;
	cl_float3 additionConstant000;
	cl_float3 additionConstantA000;
	cl_float3 additionConstantP000;
	cl_float3 additionConstant111;
	cl_float3 additionConstantA111;
	cl_float3 additionConstant222;
	cl_float3 additionConstantNeg100;
	cl_float3 constantMultiplier000;
	cl_float3 constantMultiplier001;
	cl_float3 constantMultiplier010;
	cl_float3 constantMultiplier100;
	cl_float3 constantMultiplierA100;
	cl_float3 constantMultiplier111;
	cl_float3 constantMultiplierA111;
	cl_float3 constantMultiplierB111;
	cl_float3 constantMultiplierC111;
	cl_float3 constantMultiplier121;
	cl_float3 constantMultiplier122;
	cl_float3 constantMultiplier221;
	cl_float3 constantMultiplier222;
	cl_float3 constantMultiplier441;
	cl_float3 juliaC;
	cl_float3 offset000;
	cl_float3 offsetA000;
	cl_float3 offsetF000;
	cl_float3 offset100;
	cl_float3 offset1105;
	cl_float3 offset111;
	cl_float3 offsetA111;
	cl_float3 offsetB111;
	cl_float3 offsetC111;
	cl_float3 offset200;
	cl_float3 offsetA200;
	cl_float3 offset222;

	cl_float3 power025;
	cl_float3 power8;
	cl_float3 rotation;
	cl_float3 rotation2;
	cl_float3 rotation44a; //.........................
	cl_float3 rotation44b; //..........................
	cl_float3 scale3D111;
	cl_float3 scale3D222;
	cl_float3 scale3Da222;
	cl_float3 scale3Db222;
	cl_float3 scale3Dc222;
	cl_float3 scale3Dd222;
	cl_float3 scale3D333;
	cl_float3 scale3D444;

	cl_float4 additionConstant0000;
	cl_float4 offset0000;
	cl_float4 offset1111;
	cl_float4 offsetA1111;
	cl_float4 additionConstant111d5;
	cl_float4 constantMultiplier1220;

	matrix33 rotationMatrix;
	matrix33 rotationMatrix2;
	CRotationMatrix44 rotationMatrix44; //....................
	matrix33 tempRotmatrix;

	cl_int addCpixelEnabled;
	cl_int addCpixelEnabledFalse;
	cl_int alternateEnabledFalse;
	cl_int benesiT1Enabled;
	cl_int benesiT1EnabledFalse;
	cl_int benesiT1MEnabledFalse;
	cl_int functionEnabled;
	cl_int functionEnabledFalse;
	cl_int functionEnabledx;
	cl_int functionEnabledy;
	cl_int functionEnabledz;
	cl_int functionEnabledw;
	cl_int functionEnabledxFalse;
	cl_int functionEnabledyFalse;
	cl_int functionEnabledzFalse;
	cl_int functionEnabledAx;
	cl_int functionEnabledAy;
	cl_int functionEnabledAz;
	cl_int functionEnabledAw;
	cl_int functionEnabledAxFalse;
	cl_int functionEnabledAyFalse;
	cl_int functionEnabledAzFalse;
	cl_int functionEnabledAwFalse;
	cl_int functionEnabledBx;
	cl_int functionEnabledBy;
	cl_int functionEnabledBz;
	cl_int functionEnabledBxFalse;
	cl_int functionEnabledByFalse;
	cl_int functionEnabledBzFalse;
	cl_int functionEnabledCx;
	cl_int functionEnabledCy;
	cl_int functionEnabledCz;
	cl_int functionEnabledCxFalse;
	cl_int functionEnabledCyFalse;
	cl_int functionEnabledCzFalse;
	cl_int functionEnabledFFalse;
	cl_int functionEnabledKFalse;
	cl_int functionEnabledM;
	cl_int functionEnabledMFalse;
	cl_int functionEnabledPFalse;
	cl_int functionEnabledRFalse;
	cl_int functionEnabledSFalse;
	cl_int functionEnabledSwFalse;
	cl_int functionEnabledXFalse;
	cl_int juliaMode;
	cl_int rotationEnabled;
} sClsFractalTransformCommon;

class cFractal
{
public:
	cFractal(const cParameterContainer *par);
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

#ifdef CLSUPPORT
	cl_float customParameters[15];
	cl_float deltaDEStep;
	char customOCLFormulaName[100];
	fractal::enumOCLDEMode customOCLFormulaDEMode;
#endif
};

#endif /* MANDELBULBER2_SRC_FRACTAL_H_ */
