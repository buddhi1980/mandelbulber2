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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Graeme McLaren
 *
 * functions for calculation single fractal iterations
 */

#ifndef MANDELBULBER2_SRC_FRACTAL_FORMULAS_HPP_
#define MANDELBULBER2_SRC_FRACTAL_FORMULAS_HPP_

#include "commonparams.hpp"
#include "fractal.h"

void AboxMod1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AboxMod2Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AboxModKaliIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AboxModKaliEiffieIteration(CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void AboxVSIcen1Iteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void AexionIteration(CVector3 &z, double &w, int i, const cFractal *fractal, sExtendedAux &aux);
void AexionOctopusModIteration(CVector3 &z, CVector3 c, const cFractal *fractal);
void AmazingSurfIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void AmazingSurfMod1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AmazingSurfMultiIteration(CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void BenesiIteration(CVector3 &z, CVector3 c, sExtendedAux &aux);
void BenesiPineTreeIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void BenesiT1PineTreeIteration(CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void BenesiMagTransformsIteration(CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void BoxFoldBulbPow2Iteration(CVector3 &z, const cFractal *fractal);
void BristorbrotIteration(CVector3 &z, sExtendedAux &aux);
void Bristorbrot4DIteration(CVector3 &z, double &w, int i, const cFractal *fractal, sExtendedAux &aux);
void BuffaloIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void EiffieMsltoeIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void FoldBoxMod1Iteration(CVector3 &z, int &i, const cFractal *fractal, sExtendedAux &aux);
void FastImagscaPower2Iteration(CVector3 &z);
void GeneralizedFoldBoxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void HypercomplexIteration(CVector3 &z, double &w, sExtendedAux &aux);
void Ides2Iteration(CVector3 &z, const cFractal *fractal);
void IdesIteration(CVector3 &z, const cFractal *fractal);
void IQbulbIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void KaleidoscopicIFSIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void Kalisets1Iteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void LkmitchIteration(CVector3 &z);
void Makin3D2Iteration(CVector3 &z);
void MandelboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelboxMengerIteration(CVector3 &z, CVector3 c, int &i, const cFractal *fractal, sExtendedAux &aux);
void MandelboxVaryScale4DIteration(
	CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux);
void Mandelbulb2Iteration(CVector3 &z, sExtendedAux &aux);
void Mandelbulb3Iteration(CVector3 &z, sExtendedAux &aux);
void Mandelbulb4Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbBermarteIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbKaliIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbKaliMultiIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbMultiIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbPower2Iteration(CVector3 &z, sExtendedAux &aux);
void MandelbulbVaryPowerV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerMod1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerMiddleModIteration(
	CVector3 &z, CVector3 &c, int i, const cFractal *fractal, sExtendedAux &aux);

void MengerSpongeIteration(CVector3 &z, sExtendedAux &aux);
void MengerPrismShapeIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerPwr2PolyIteration(CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void MsltoeDonutIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym2ModIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym3ModIteration(CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym3Mod2Iteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym3Mod3Iteration(CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym4ModIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void MsltoeToroidalIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MsltoeToroidalMultiIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void PseudoKleinian1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void Quaternion3DIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void Quaternion4DIteration(CVector4 &z, int i, const cFractal *fractal);
void QuaternionIteration(CVector3 &z, double &w, sExtendedAux &aux);
void QuickDudleyIteration(CVector3 &z);
void QuickDudleyModIteration(CVector3 &z, const cFractal *fractal);
void RiemannSphereMsltoeIteration(CVector3 &z, const cFractal *fractal);
void RiemannSphereMsltoeV1Iteration(CVector3 &z, const cFractal *fractal);
void RiemannBulbMsltoeMod2Iteration(CVector3 &z, const cFractal *fractal);
void SmoothMandelboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void XenodreambuieIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void CollatzIteration(CVector3 &z, sExtendedAux &aux);
void CollatzModIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void CrossMengerIteration(
	CVector3 &z, CVector3 &c, int i, const cFractal *fractal, sExtendedAux &aux);

void BoxFolding(CVector3 &z, const sFractalFoldings *foldings, sExtendedAux &aux);
void SphericalFolding(CVector3 &z, const sFractalFoldings *foldings, sExtendedAux &aux);

// transforming formulas
void TransformAddCpixelAxisSwapIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux );
void TransformAddCpixelCxCyAxisSwapIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void TransformAddCpixelIteration(CVector3 &z, CVector3 c, const cFractal *fractal);
void TransformAddCpixelPosNegIteration(CVector3 &z, CVector3 c, const cFractal *fractal);
void TransformAddCpixelVaryV1Iteration(CVector3 &z, CVector3 c, int i, const cFractal *fractal);
void TransformAdditionConstantIteration(CVector3 &z, const cFractal *fractal);
void TransformAdditionConstant4DIteration(CVector4 &z4D, const cFractal *fractal);
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
void TransformBoxFold4DIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux);
void TransformBoxFoldXYZIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformBoxOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformLinCombineCxyz(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void TransformMengerFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformMultipleAngle(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformFabsAddConstantIteration(CVector3 &z, const cFractal *fractal);
void TransformFabsAddConstantV2Iteration(CVector3 &z, const cFractal *fractal);
void TransformFabsAddConstant4DIteration(CVector4 &z4D, const cFractal *fractal);
void TransformFabsAddConstantV24DIteration(CVector4 &z4D, const cFractal *fractal);
void TransformNegFabsAddConstantIteration(CVector3 &z, const cFractal *fractal);
void TransformFabsAddMultiIteration(CVector3 &z, const cFractal *fractal);
void TransformFoldingTetra3DIteration(CVector3 &z, const cFractal *fractal);
void TransformIterationWeightIteration(CVector3 &z, int i, const cFractal *fractal);
void TransformIterationWeight4DIteration(CVector4 &z4D, int i, const cFractal *fractal);
void TransformInvCylindricalIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);

void TransformPlatonicSolidIteration(CVector3 &z, const cFractal *fractal);
void TransformPowerR(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformPwr2PolynomialIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformRpow3Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);

void TransformQuaternionFoldIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void TransformRotationIteration(CVector3 &z, const cFractal *fractal);
void TransformRotationFoldingPlane(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformRotationVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal);
void TransformScaleIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformScale4DIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux);
void TransformScaleVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void TransformScale3DIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformSphereInvCIteration(CVector3 &z, CVector3 c, const cFractal *fractal);
void TransformSphereInvIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformSphericalFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformSphericalFoldAboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformSphericalFoldVaryV1Iteration(
	CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void TransformSphericalFold4DIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux);
void TransformSphericalPwrFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformSphericalOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformSurfFoldMultiIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransformZvectorAxisSwapIteration(CVector3 &z, const cFractal *fractal);

#endif /* MANDELBULBER2_SRC_FRACTAL_FORMULAS_HPP_ */
