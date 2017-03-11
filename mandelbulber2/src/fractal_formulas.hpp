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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Graeme McLaren,
 *  Sebastian Jennen (jenzebas@gmail.com)
 *
 * functions for calculation single fractal iterations
 */

#ifndef MANDELBULBER2_SRC_FRACTAL_FORMULAS_HPP_
#define MANDELBULBER2_SRC_FRACTAL_FORMULAS_HPP_

#include "algebra.hpp"

// forward declarations
class cFractal;
struct sExtendedAux;
struct sFractalFoldings;

void AboxMod1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AboxMod2Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AboxModKaliIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AboxModKaliEiffieIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void AboxVSIcen1Iteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void AexionIteration(CVector3 &z, double &w, int i, const cFractal *fractal, sExtendedAux &aux);
void AexionOctopusModIteration(CVector3 &z, CVector3 c, const cFractal *fractal);
void AmazingSurfIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void AmazingSurfMod1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void AmazingSurfMultiIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void BenesiIteration(CVector3 &z, CVector3 c, sExtendedAux &aux);
void BenesiPineTreeIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void BenesiT1PineTreeIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void BenesiMagTransformsIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void BenesiPwr2MandelbulbIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void BoxFoldBulbPow2Iteration(CVector3 &z, const cFractal *fractal);
void BristorbrotIteration(CVector3 &z, sExtendedAux &aux);
void BuffaloIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void CollatzIteration(CVector3 &z, sExtendedAux &aux);
void CollatzModIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void CrossMengerMod1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void EiffieMsltoeIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void FoldBoxMod1Iteration(CVector3 &z, int &i, const cFractal *fractal, sExtendedAux &aux);
void ImaginaryScatorPower2Iteration(CVector3 &z);
void GeneralizedFoldBoxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void HypercomplexIteration(CVector3 &z, double &w, sExtendedAux &aux);
void Ides2Iteration(CVector3 &z, const cFractal *fractal);
void IdesIteration(CVector3 &z, const cFractal *fractal);
void IqBulbIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void KaleidoscopicIFSIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void Kalisets1Iteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void LkmitchIteration(CVector3 &z);
void Makin3d2Iteration(CVector3 &z);
void MandelboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelboxMengerIteration(
	CVector3 &z, CVector3 c, int &i, const cFractal *fractal, sExtendedAux &aux);
void Mandelbulb2Iteration(CVector3 &z, sExtendedAux &aux);
void Mandelbulb3Iteration(CVector3 &z, sExtendedAux &aux);
void Mandelbulb4Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbBermarteIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbKaliIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbKaliMultiIteration(
	CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbMultiIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbPower2Iteration(CVector3 &z, sExtendedAux &aux);
void MandelbulbVaryPowerV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerSpongeIteration(CVector3 &z, sExtendedAux &aux);
void MengerCrossMod1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerCrossKIFSIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerMod1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerMiddleModIteration(
	CVector3 &z, CVector3 &c, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerOctoIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerPrismShapeIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerPrismShape2Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerPwr2PolyIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerSmoothIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MengerSmoothMod1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MsltoeDonutIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym2ModIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym3ModIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym3Mod2Iteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym3Mod3Iteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux);
void MsltoeSym4ModIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void MsltoeToroidalIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void MsltoeToroidalMultiIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void PseudoKleinian1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void PseudoKleinian2Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void PseudoKleinian3Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void PseudoKleinianMod2Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void Quaternion3dIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void QuaternionIteration(CVector3 &z, double &w, sExtendedAux &aux);
void QuickDudleyIteration(CVector3 &z);
void QuickDudleyModIteration(CVector3 &z, const cFractal *fractal);
void RiemannSphereMsltoeIteration(CVector3 &z, const cFractal *fractal);
void RiemannSphereMsltoeV1Iteration(CVector3 &z, const cFractal *fractal);
void RiemannBulbMsltoeMod2Iteration(CVector3 &z, const cFractal *fractal);
void Sierpinski3dIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void MandelboxSmoothIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void XenodreambuieIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void CollatzIteration(CVector3 &z, sExtendedAux &aux);
void CollatzModIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);

void BoxFolding(CVector3 &z, const sFractalFoldings *foldings, sExtendedAux &aux);
void SphericalFolding(CVector3 &z, const sFractalFoldings *foldings, sExtendedAux &aux);

// transforming formulas
void TransfAddCpixelAxisSwapIteration(
	CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void TransfAddCpixelCxCyAxisSwapIteration(
	CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void TransfAddCpixelIteration(CVector3 &z, CVector3 c, const cFractal *fractal);
void TransfAddCpixelSymmetricalIteration(CVector3 &z, CVector3 c, const cFractal *fractal);
void TransfAddCpixelVaryV1Iteration(CVector3 &z, CVector3 c, int i, const cFractal *fractal);
void TransfAddConstantIteration(CVector3 &z, const cFractal *fractal);
void TransfAddConstantVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal);
void TransfAddExp2ZIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfBenesiCubeSphereIteration(CVector3 &z);
void TransfBenesiMagBackwardIteration(CVector3 &z);
void TransfBenesiMagForwardIteration(CVector3 &z);
void TransfBenesiSphereCubeIteration(CVector3 &z);
void TransfBenesiT1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfBenesiT1ModIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfBenesiT2Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfBenesiT3Iteration(CVector3 &z, const cFractal *fractal);
void TransfBenesiT4Iteration(CVector3 &z, const cFractal *fractal);
void TransfBenesiT5bIteration(CVector3 &z, const cFractal *fractal);
void TransfBoxFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfBoxFoldVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void TransfBoxFoldXYZIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfBoxOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfFabsAddConstantIteration(CVector3 &z, const cFractal *fractal);
void TransfFabsAddTgladFoldIteration(CVector3 &z, int i, const cFractal *fractal);
void TransfFabsAddConditionalIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfNegFabsAddConstantIteration(CVector3 &z, const cFractal *fractal);
void TransfFabsAddMultiIteration(CVector3 &z, const cFractal *fractal);
void TransfFoldingTetra3dIteration(CVector3 &z, const cFractal *fractal);
void TransfIterationWeightIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);

void TransfInvCylindricalIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfLinCombineCXYZIteration(
	CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void TransfMengerFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfMultipleAngleIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfOctoFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfPlatonicSolidIteration(CVector3 &z, const cFractal *fractal);
void TransfRPowerIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfPwr2PolynomialIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfQuaternionFoldIteration(
	CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux);
void TransfReciprocal3Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfRpow3Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfRotationIteration(CVector3 &z, const cFractal *fractal);
void TransfRotationFoldingPlaneIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfRotationVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal);
void TransfRotationFoldingIteration(CVector3 &z, const cFractal *fractal);
void TransfScaleIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfScaleVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void TransfScaleVaryVCLIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void TransfScale3dIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfSphericalInvCIteration(CVector3 &z, CVector3 c, const cFractal *fractal);
void TransfSphericalInvIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldAboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldVaryV1Iteration(
	CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldVaryVCLIteration(
	CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void TransfSphericalPwrFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfSphericalOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfSphericalOffsetVCLIteration(
	CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux);
void TransfSurfBoxFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfSurfFoldMultiIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux);
void TransfZvectorAxisSwapIteration(CVector3 &z, int i, const cFractal *fractal);

// 4D
void Abox4dIteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux);
void Bristorbrot4dIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux);
void MandelboxVaryScale4dIteration(
	CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux);
void MixPinski4dIteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux);
void Menger4dIteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux);
void Menger4dMod1Iteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux);
void Sierpinski4dIteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux);
void Quaternion4dIteration(CVector4 &z4D, int i, const cFractal *fractal);

void TransfAddConstant4dIteration(CVector4 &z4D, const cFractal *fractal);
void TransfBoxFold4dIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux);
void TransfFabsAddConstant4dIteration(CVector4 &z4D, const cFractal *fractal);
void TransfFabsAddTgladFold4dIteration(CVector4 &z4D, const cFractal *fractal);
void TransfFabsAddConditional4dIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux);
void TransfIterationWeight4dIteration(
	CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux);
void TransfReciprocal4dIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux);
void TransfRotation4dIteration(CVector4 &z4D, const cFractal *fractal);
void TransfScale4dIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux);
void TransfSphericalFold4dIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux);

#endif /* MANDELBULBER2_SRC_FRACTAL_FORMULAS_HPP_ */
