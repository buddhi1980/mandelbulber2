/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

// formula constants
#define SQRT_1_3 0.57735026918962576450914878050196
#define SQRT_1_2 0.70710678118654752440084436210485
#define SQRT_2_3 0.81649658092772603273242802490196
#define SQRT_3_2 1.22474487139158904909864203735295
#define SQRT_3_4 0.86602540378443864676372317075294
#define SQRT_3_4d2 0.43301270189221932338186158537647
#define SQRT_3 1.73205080756887729352744634150587

#define FRAC_1_3 0.33333333333333333333333333333333

#ifndef M_PI_180
#define M_PI_180 0.01745329251994329576923690768489
#endif

#ifndef M_PI_8
#define M_PI_8 0.39269908169872415480783042290994
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830961566084581988
#endif

#ifndef M_PI_2x
#define M_PI_2x 6.283185307179586476925286766559
#endif

// end of formula constants

// forward declarations
struct sFractal;
struct sExtendedAux;
struct sFractalFoldings;

void BoxFolding(CVector4 &z, const sFractalFoldings *foldings, sExtendedAux &aux);
void SphericalFolding(CVector4 &z, const sFractalFoldings *foldings, sExtendedAux &aux);

void AboxMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AboxMod2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AboxMod11Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AboxMod12Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AboxMod13Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AboxModKaliIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AboxModKaliEiffieIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AboxVSIcen1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AexionIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AexionOctopusModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AmazingSurfIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AmazingSurfMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AmazingSurfMod2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void AmazingSurfMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BenesiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BenesiPineTreeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BenesiT1PineTreeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BenesiMagTransformsIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BenesiPwr2MandelbulbIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BoxFoldBulbPow2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BoxFoldBulbPow2V2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BoxFoldBulbPow2V3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BoxFoldQuatIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BristorbrotIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void BuffaloIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void CoastalbrotIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void CollatzIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void CollatzModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void CrossMengerMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void EiffieMsltoeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void FoldBoxMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void ImaginaryScatorPower2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void GeneralizedFoldBoxIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void HypercomplexIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Ides2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void IdesIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void IqBulbIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void JosKleinianIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void KaleidoscopicIfsIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Kalisets1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void LkmitchIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Makin3d2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

void MandelboxIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

void MandelboxMengerIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Mandelbulb2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Mandelbulb3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Mandelbulb4Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

void MandelbulbIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

void ModulusMengerSpongeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void ModulusMandelbulbIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

void MandelbulbBermarteIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MandelbulbJuliabulbIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MandelbulbKaliIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MandelbulbKaliMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MandelbulbMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MandelbulbMulti2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MandelbulbPower2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MandelbulbQuatIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MandelbulbVaryPowerV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerSpongeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerCrossMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerCrossKIFSIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerMiddleModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerOctoIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerPrismShapeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerPrismShape2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerPwr2PolyIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerSmoothIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MengerSmoothMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MsltoeDonutIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MsltoeSym2ModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MsltoeSym3ModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MsltoeSym3Mod2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MsltoeSym3Mod3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MsltoeSym4ModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MsltoeToroidalIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MsltoeToroidalMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void PseudoKleinianStdDEIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void PseudoKleinianIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void PseudoKleinianMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void PseudoKleinianMod2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Quaternion3dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void QuaternionIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void QuaternionCubic4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void QuickDudleyIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void QuickDudleyModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void RiemannSphereMsltoeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void RiemannSphereMsltoeV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void RiemannBulbMsltoeMod2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Sierpinski3dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MandelboxSmoothIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void XenodreambuieIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void CollatzIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void CollatzModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

// transforming formulas
void TransfAddCpixelAxisSwapIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAddCpixelCxCyAxisSwapIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAddCpixelIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAddCpixelRotatedIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAddCpixelSymmetricalIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAddCpixelVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAddConstantIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAddConstantMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAddConstantVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAddExp2ZIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBenesiCubeSphereIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBenesiMagBackwardIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBenesiMagForwardIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBenesiSphereCubeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBenesiT1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBenesiT1ModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBenesiT2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBenesiT3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBenesiT4Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBenesiT5bIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBoxFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBoxFoldVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBoxFoldXYZIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBoxOffsetIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAbsAddConstantIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAbsAddTgladFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAbsAddConditionalIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfNegAbsAddConstantIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAbsAddMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfFoldingTetra3dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfIterationWeightIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

void TransfInvCylindricalIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfLinCombineCXYZIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfMengerFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfMultipleAngleIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfOctoFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfParabFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfPlatonicSolidIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfRPowerIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfPwr2PolynomialIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfQuaternionFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfReciprocal3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfRpow3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfRotationIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfRotationFoldingPlaneIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfRotationVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfRotationFoldingIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfScaleIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfScaleVaryV212Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfScaleVaryMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfScaleVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfScaleVaryVCLIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfScale3dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalInvCIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalInvIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldAboxIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldCHSIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldCuboidIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldParabIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldVaryVCLIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalFoldXYZBiasIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalPwrFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalOffsetIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalOffsetVCLIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSurfBoxFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSurfFoldMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfZvectorAxisSwapIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

// 4D
void Abox4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Bristorbrot4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MandelboxVaryScale4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void MixPinski4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Menger4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Menger4dMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Sierpinski4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void Quaternion4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

void TransfAddConstant4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBoxFold4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfBoxFold4dTgladIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAbsAddConstant4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAbsAddTgladFold4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfAbsAddConditional4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfIterationWeight4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfReciprocal4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfRotation4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfScale4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfSphericalFold4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

void TransfHybridColorIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);
void TransfHybridColor2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

void TestingIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

void Testing4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux);

#endif /* MANDELBULBER2_SRC_FRACTAL_FORMULAS_HPP_ */
