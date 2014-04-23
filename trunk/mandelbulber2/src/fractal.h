/*********************************************************
 /                   MANDELBULBER
 / definition of structures for fractal parameters
 /
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 ********************************************************/

#ifndef FRACTAL_H_
#define FRACTAL_H_

#include <vector>
#include <stddef.h>
#include <string>
#include "algebra.hpp"

const int IFS_VECTOR_COUNT = 9;
const int HYBRID_COUNT = 5;
const int MANDELBOX_FOLDS = 2;

namespace fractal
{
enum enumFractalFormula
{
	none = 0,
	trig_DE = 1,
	trig_optim = 2,
	fast_trig = 3,
	hypercomplex = 4,
	quaternion = 5,
	minus_fast_trig = 6,
	menger_sponge = 7,
	tglad = 8,
	kaleidoscopic = 10,
	xenodreambuie = 11,
	hybrid = 12,
	mandelbulb2 = 13,
	mandelbulb3 = 14,
	mandelbulb4 = 15,
	foldingIntPow2 = 16,
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
	ocl_custom = 38
};

enum enumCalculationMode
{
	normal = 0, colouring = 1, fake_AO = 2, deltaDE1 = 3, deltaDE2 = 4, orbitTrap = 5
};

enum enumGeneralizedFoldBoxType
{
	foldTet = 0, foldCube = 1, foldOct = 2, foldDodeca = 3, foldOctCube = 4, foldIcosa = 5, foldBox6 = 6, foldBox5 = 7
};

enum enumObjectType
{
	objFractal = 0, objPlane = 1, objWater = 2, objSphere = 3, objSphereInv = 4, objBox = 5, objBoxInv = 6
};

enum enumOCLDEMode
{
	calculated = 0, deltaDE = 1, noDE = 2
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
	double rotationGamma;
	double rotationAlfa;
	double rotationBeta;
	double scale;
	double distance[IFS_VECTOR_COUNT];
	double alfa[IFS_VECTOR_COUNT];
	double beta[IFS_VECTOR_COUNT];
	double gamma[IFS_VECTOR_COUNT];
	double intensity[IFS_VECTOR_COUNT];
	CVector3 offset;
	CVector3 direction[IFS_VECTOR_COUNT];
	CVector3 edge;
	bool absX, absY, absZ;
	bool foldingMode; // Kaleidoscopic IFS folding mode
	bool enabled[IFS_VECTOR_COUNT];
	bool mengerSpongeMode;
	int foldingCount;
	CRotationMatrix mainRot;
	CRotationMatrix rot[IFS_VECTOR_COUNT];
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
	double rotationMain[3];
	double rotation[MANDELBOX_FOLDS][3][3];
	double colorFactorX;
	double colorFactorY;
	double colorFactorZ;
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
	sFractalMandelboxVary4D vary4D;
	bool rotationsEnabled;
	CRotationMatrix mainRot;
	CRotationMatrix rot[MANDELBOX_FOLDS][3];
	CRotationMatrix rotinv[MANDELBOX_FOLDS][3];
};


struct sFractal
{
	double FoldingIntPowZfactor;
	double FoldingIntPowFoldFactor;
	double foldingLimit; //paramters of TGlad's folding
	double foldingValue;
	double foldingSphericalMin;
	double foldingSphericalFixed;
	double power;		 //power of fractal formula
	double cadd;
	double hybridPower[HYBRID_COUNT];
#ifdef CLSUPPORT
	double customParameters[15];
	double deltaDEStep;
#endif



	  // maximum number of iterations


	fractal::enumFractalFormula formula;

	int hybridIters[HYBRID_COUNT];
	fractal::enumFractalFormula hybridFormula[HYBRID_COUNT];

	std::vector<fractal::enumFractalFormula> formulaSequence;
	std::vector<double> hybridPowerSequence;
	char customOCLFormulaName[100];
	fractal::enumOCLDEMode customOCLFormulaDEMode;

	sFractalIFS IFS;
	sFractalMandelbox mandelbox;
	sFractalGeneralizedFoldBox genFoldBox;

	int frameNo;

	int itersOut;
	fractal::enumObjectType objectOut;


};

template <int Mode> double Compute(CVector3 z, const sFractal &par, int *iter_count = NULL);
double CalculateDistance(CVector3 point, sFractal &par, bool *max_iter = NULL);

#endif /* FRACTAL_H_ */
