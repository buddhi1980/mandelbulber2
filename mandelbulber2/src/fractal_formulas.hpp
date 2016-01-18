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

#include "commonparams.hpp"
#include "nine_fractals.hpp"


void AboxMod1Iteration(CVector3 &z,const cFractal *fractal, sExtendedAux &aux);
void AboxMod2Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AboxModKaliIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AboxVSIcen1Iteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux);
void AexionIteration(CVector3 &z, double &w, int i, const cFractal *fractal, sExtendedAux &aux);
void AexionOctopusIteration(CVector3 &z, const cFractal *fractal);
void AmazingSurfIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AmazingSurfMod1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void BenesiIteration(CVector3 &z, CVector3 &c, sExtendedAux &aux);
void BenesiPineTreeIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux);
void BenesiT1PineTreeIteration(CVector3 &z, CVector3 &c, int i, const cFractal *fractal, sExtendedAux &aux);
void BenesiTransformsIteration(CVector3 &z, CVector3 &c, double minimumR, int &i, const cFractal *fractal, sExtendedAux &aux);
void BoxFoldBulbPow2Iteration(CVector3 &z, const cFractal *fractal);
void BristorbrotIteration(CVector3 &z, sExtendedAux &aux);
void BuffaloIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void EiffieMsltoeIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void FoldBoxMod1Iteration(CVector3 &z, int &i, const cFractal *fractal, sExtendedAux &aux);
void GeneralizedFoldBoxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void HypercomplexIteration(CVector3 &z, double &w, sExtendedAux &aux);
void Ides2Iteration(CVector3 &z, CVector3 &c);
void IdesIteration(CVector3 &z);
void KaleidoscopicIFSIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void Kalisets1Iteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux);
void LkmitchIteration(CVector3 &z);
void Makin3D2Iteration(CVector3 &z);
void Mandelbox103Iteration(CVector3 &z, CVector3 &c, double minimumR, int &i, const cFractal *fractal, sExtendedAux &aux);
void MandelboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelboxVaryScale4DIteration(CVector3 &z, double &w, const cFractal *fractal, sExtendedAux &aux);
void Mandelbulb2Iteration(CVector3 &z, sExtendedAux &aux);
void Mandelbulb3Iteration(CVector3 &z, sExtendedAux &aux);
void Mandelbulb4Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void Mandelbulb5Iteration(CVector3 &z, CVector3 &c, double minimumR, int &i, const cFractal *fractal, sExtendedAux &aux);
void Mandelbulb6BetaIteration(CVector3 &z, CVector3 &c, double minimumR, int &i, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbMultiIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbPower2Iteration(CVector3 &z, sExtendedAux &aux);
void MandelbulbVaryPowerV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerMod1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerSponge105Iteration(CVector3 &z, CVector3 &c, double minimumR, int &i, const cFractal *fractal, sExtendedAux &aux);
void MengerSpongeIteration(CVector3 &z, sExtendedAux &aux);
void MsltoeSym2Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym3ModIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym4ModIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux);
void Quaternion104Iteration(CVector4 &z, const CVector4 &c, int &i, const cFractal *fractal, sExtendedAux &aux);
void Quaternion3DIteration(CVector3 &z, const cFractal *fractal);
void QuaternionIteration(CVector3 &z, double &w, sExtendedAux &aux);
void QuickDudleyIteration(CVector3 &z);
void RiemannSphereMsltoeIteration(CVector3 &z, const cFractal *fractal);
void RiemannSphereMsltoeV1Iteration(CVector3 &z, const cFractal *fractal);
void SmoothMandelboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void XenodreambuieIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);

void BoxFolding(CVector3 &z, const sFractalFoldings *foldings, sExtendedAux &aux);
void SphericalFolding(CVector3 &z, const sFractalFoldings *foldings, sExtendedAux &aux);

//transforming formulas
void TransformAddCpixelAxisSwapIteration(CVector3 &z, CVector3 &c, const cFractal *fractal);
void TransformAddCpixelCxCyAxisSwapIteration(CVector3 &z, CVector3 &c, const cFractal *fractal);
void TransformAddCpixelIteration(CVector3 &z, CVector3 &c, const cFractal *fractal);
void TransformAddCpixelPosNegIteration(CVector3 &z, CVector3 &c, const cFractal *fractal);
void TransformAddCpixelVaryV1Iteration(CVector3 &z, CVector3 &c, int i, const cFractal *fractal);
void TransformAdditionConstantIteration(CVector3 &z, const cFractal *fractal);
void TransformAdditionConstantVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal);
void TransformBenesiCubeSphereIteration(CVector3 &z);
void TransformBenesiMagBackwardIteration(CVector3 &z);
void TransformBenesiMagForwardIteration(CVector3 &z);
void TransformBenesiSphereCubeIteration(CVector3 &z);
void TransformBenesiT1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformBenesiT1ModIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformBenesiT2Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformBenesiT3Iteration(CVector3 &z, const cFractal *fractal);
void TransformBenesiT4Iteration(CVector3 &z, const cFractal *fractal);
void TransformBenesiT5bIteration(CVector3 &z, const cFractal *fractal);
void TransformBoxFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformBoxFoldXYZIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformBoxOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformMultipleAngle(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformFabsAddConstantIteration(CVector3 &z, const cFractal *fractal);
void TransformFabsAddConstantV2Iteration(CVector3 &z, const cFractal *fractal);
void TransformNegFabsAddConstantIteration(CVector3 &z, const cFractal *fractal);
void TransformFabsAddMultiIteration(CVector3 &z, const cFractal *fractal);
void TransformIterationWeightIteration(CVector3 &z, int i, const cFractal *fractal);
void TransformPlatonicSolidIteration(CVector3 &z, const cFractal *fractal);
void TransformPowerR(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformRotationIteration(CVector3 &z, const cFractal *fractal);
void TransformRotationVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal);
void TransformScaleIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformScaleVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void TransformScale3DIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformSphericalFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformSphericalPwrFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformSphericalOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);

void TransformZvectorAxisSwapIteration(CVector3 &z, const cFractal *fractal);



#endif /* FRACTAL_FORMULAS_HPP_ */
