/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * functions for calculation single fractal iterations
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Graeme McLaren
 */

#ifndef FRACTAL_FORMULAS_HPP_
#define FRACTAL_FORMULAS_HPP_

#include "four_fractals.hpp"
#include "commonparams.hpp"

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


void MandelbulbIteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux);
void Mandelbulb2Iteration(CVector3 &z, sMandelbulbAux &aux);
void Mandelbulb3Iteration(CVector3 &z, sMandelbulbAux &aux);
void Mandelbulb4Iteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux);
void MandelbulbPower2Iteration(CVector3 &z);
void XenodreambuieIteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux);
void MandelboxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux);
void SmoothMandelboxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux);
void BoxFoldBulbPow2Iteration(CVector3 &z, const cFractal *fractal);
void MengerSpongeIteration(CVector3 &z, sIFSAux &aux);
void KaleidoscopicIFSIteration(CVector3 &z, const cFractal *fractal, sIFSAux &aux);
void MandelboxVaryScale4DIteration(CVector3 &z, double &w, const cFractal *fractal, sMandelboxAux &aux);
void AexionIteration(CVector3 &z, double &w, const cFractal *fractal, sAexionAux &aux);
void HypercomplexIteration(CVector3 &z, double &w);
void QuaternionIteration(CVector3 &z, double &w);
void BenesiIteration(CVector3 &z, CVector3 &c);
void BristorbrotIteration(CVector3 &z);
void IdesIteration(CVector3 &z);
void Ides2Iteration(CVector3 &z, CVector3 &c);
void BuffaloIteration(CVector3 &z, const cFractal *fractal);
void QuickDudleyIteration(CVector3 &z);
void LkmitchIteration(CVector3 &z);
void Makin3D2Iteration(CVector3 &z);
void MsltoeSym2(CVector3 &z, const cFractal *fractal);
void PlatonicSolidIteration(CVector3 &z, const cFractal *fractal);
void GeneralizedFoldBoxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux);

void BoxFolding(CVector3 &z, const sFractalFoldings *foldings, double &foldColor);
void SphericalFolding(CVector3 &z, const sFractalFoldings *foldings, double &foldColor, double &foldDE, double r);
  //----------Transforms----------------
void additionConstantTransform3D(const sTransformAdditionConstant &additionConstant, CVector3 &z, int i);
void additionConstantTransform4D(const sTransformAdditionConstant &additionConstant, CVector4 &z, int i);
void boxConstantMultiplierTransform3D(const sTransformBoxConstantMultiplier &boxConstantMultiplier, CVector3 &z,CVector3 &c, int i);
void boxFoldOriginalTransform3D(const sTransformBoxFoldOriginal &boxFoldOriginal, CVector3 &z, int i);
void boxFoldTransform3D(const sTransformBoxFold &boxFold, CVector3 &z, int i);
void boxFoldTransform4D(const sTransformBoxFold &boxFold, CVector4 &z, int i);
void boxOffsetTransform3D(const sTransformBoxOffset &boxOffset, CVector3 &z, int i);
void constantMultiplierOriginalTransform3D(const sTransformConstantMultiplierOriginal &constantMultiplierOriginal, CVector3 &z, CVector3 &c, int i);
void constantMultiplierOriginalTransform4D(const sTransformConstantMultiplierOriginal &constantMultiplierOriginal, CVector4 &z, const CVector4 &c, int i);
void constantMultiplierTransform3D(const sTransformConstantMultiplier &constantMultiplier, CVector3 &z, CVector3 &c, int i);
void constantMultiplierTransform4D(const sTransformConstantMultiplier &constantMultiplier, CVector4 &z, const CVector4 &c, int i);
void fabsAddConstantTransform3D(const sTransformFabsAddConstant &fabsAddConstant, CVector3 &z, int i);
void fabsAddConstantTransform4D(const sTransformFabsAddConstant &fabsAddConstant, CVector4 &z, int i);
void fabsFormulaABTransform3D(const sTransformFabsFormulaAB &fabsFormulaAB, CVector3 &z, int i);
void fabsFormulaABCDTransform3D(const sTransformFabsFormulaABCD &fabsFormulaABCD, CVector3 &z, int i);
void fabsFormulaABCDTransform4D(const sTransformFabsFormulaABCD &fabsFormulaABCD, CVector4 &z, int i);
void fabsFormulaZABTransform3D(const sTransformFabsFormulaZAB &fabsFormulaZAB, CVector3 &z, int i);
void fabsSubConstantTransform3D(const sTransformFabsSubConstant &fabsSubConstant, CVector3 &z, int i);
void iterationWeightTransform3D(const sTransformIterationWeight &iterationWeight, CVector3 &z, int i);
void iterationWeightTransform4D(const sTransformIterationWeight &iterationWeight, CVector4 &z, int i);
void mandelbulbTransform3D(const sTransformMandelbulb &mandelbulb, CVector3 &z, int i, sMandelbulbAux &aux);
void mandelbulbOriginalTransform3D(const sTransformMandelbulbOriginal &mandelbulbOriginal, CVector3 &z, int i, sMandelbulbAux &aux);
void mainRotationTransform3D(const sTransformMainRotation &mainRotation, CVector3 &z, int i);
void mainRotationTransform4D(const sTransformMainRotation &mainRotation, CVector4 &z, int i);
void mengerSpongeOriginalTransform3D(const sTransformMengerSpongeOriginal &mengerSpongeOriginal, CVector3 &z, int i, sIFSAux &aux);
void mengerSpongeTransform3D(const sTransformMengerSponge &mengerSponge, CVector3 &z, int i, sIFSAux &aux);
void quaternionOriginalTransform4D(const sTransformQuaternionOriginal &quaternionOriginal, CVector4 &z, int i);
void quaternionTransform4D(const sTransformQuaternion &quaternion, CVector4 &z, int i);
void scaleOriginalTransform3D(const sTransformScaleOriginal &scaleOriginal, CVector3 &z, int i, sIFSAux &aux);
void scaleTransform3D(const sTransformScale &scale, CVector3 &z, int i, sIFSAux &aux);
void scaleTransform4D(const sTransformScale &scale, CVector4 &z, int i, sIFSAux &aux);
void sphericalFoldOriginal3D(const sTransformSphericalFoldOriginal &sphericalFoldOriginal, CVector3 &z, int i, sIFSAux &aux);
void sphericalFold3D(const sTransformSphericalFold &sphericalFold, CVector3 &z, int i, sIFSAux &aux);
void sphericalFold4D(const sTransformSphericalFold &sphericalFold, CVector4 &z, int i);
void sphericalOffset3D(const sTransformSphericalOffset &sphericalOffset, CVector3 &z, int i, sIFSAux &aux);

// hmmm sometime we want Mbox aux

void Mandelbulb5Iteration(CVector3 &z, CVector3 &c, int &i, const cFractal *fractal, sMandelbulbAux &aux);
void Mandelbox103Iteration(CVector3 &z, CVector3 &c, int &i, const cFractal *fractal,  sIFSAux &aux);
void Quaternion104Iteration(CVector4 &z, const CVector4 &c, int &i, const cFractal *fractal,sIFSAux &aux);
void MengerSponge105Iteration(CVector3 &z, CVector3 &c, int &i, const cFractal *fractal, sIFSAux &aux);

#endif /* FRACTAL_FORMULAS_HPP_ */
