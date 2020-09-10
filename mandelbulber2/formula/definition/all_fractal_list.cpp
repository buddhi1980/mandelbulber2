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
 * list of fractal formulas
 */

#include "all_fractal_list.hpp"

#include "legacy_fractal_transforms.hpp"

QList<cAbstractFractal *> newFractalList;

using namespace fractal;
void DefineFractalList(QList<cAbstractFractal *> *fractalList)
{
	//====================== FRACTAL LIST - START ==================
	fractalList->append(new cFractalNone());
#ifdef USE_OPENCL
	fractalList->append(new cFractalCustom());
#endif
	//------------------------ ANALYTIC DE FORMULAS ----------------------
	// ----- Logarithmic -------
	fractalList->append(new cFractalAexion());
	fractalList->append(new cFractalAexion4dV2());

	// Benesi formulas
	fractalList->append(new cFractalBenesiPineTree());
	fractalList->append(new cFractalBenesi());
	fractalList->append(new cFractalBenesiPwr2Mandelbulb());
	fractalList->append(new cFractalBenesiT1PineTree());

	// boxBulb formulas
	fractalList->append(new cFractalBoxFoldBulbPow2V2());
	fractalList->append(new cFractalBoxFoldBulbPow2V3());
	fractalList->append(new cFractalBoxFoldBulbMenger());
	fractalList->append(new cFractalBoxFoldQuat());
	fractalList->append(new cFractalBoxFold4dBulbPow2());
	fractalList->append(new cFractalBristorbrot());
	fractalList->append(new cFractalBristorbrot2());
	fractalList->append(new cFractalBristorbrot4d());
	fractalList->append(new cFractalBuffalo());

	// Nested sin() formulas
	fractalList->append(new cFractalCoastalbrot());

	// hypercomlex
	fractalList->append(new cFractalHypercomplex());
	fractalList->append(new cFractalIqBulb());
	fractalList->append(new cFractalLkmitch());
	fractalList->append(new cFractalMakin3d2());

	// Mandelbulb formulas
	fractalList->append(new cFractalMandelbulb());
	fractalList->append(new cFractalMandelbulb2());
	fractalList->append(new cFractalMandelbulb3());
	fractalList->append(new cFractalMandelbulb4());
	fractalList->append(new cFractalMandelbulbAbsPower2());
	fractalList->append(new cFractalMandelbulbAtan2Power2());
	fractalList->append(new cFractalMandelbulbBermarte());
	fractalList->append(new cFractalMandelbulbEye());
	fractalList->append(new cFractalMandelbulbJuliabulb());
	fractalList->append(new cFractalMandelbulbKali());
	fractalList->append(new cFractalMandelbulbKaliMulti());
	fractalList->append(new cFractalMandelbulbKosalos());
	fractalList->append(new cFractalMandelbulbKosalosV2());
	fractalList->append(new cFractalMandelbulbLambda());
	fractalList->append(new cFractalModulusMandelbulb());
	fractalList->append(new cFractalMandelbulbMulti());
	fractalList->append(new cFractalMandelbulbMulti2());
	fractalList->append(new cFractalMandelbulbPower1234());
	fractalList->append(new cFractalMandelbulbPower2());
	fractalList->append(new cFractalMandelbulbPow2V2());
	fractalList->append(new cFractalMandelbulbQuat());
	fractalList->append(new cFractalMandelbulbVaryPowerV1());
	fractalList->append(new cFractalMandelcup());
	fractalList->append(new cFractalMandeltorus());
	fractalList->append(new cFractalMandeltorusV2());

	// Msltoe formulas
	fractalList->append(new cFractalMsltoeSym2Mod());
	fractalList->append(new cFractalMsltoeSym3Mod());
	fractalList->append(new cFractalEiffieMsltoe());
	fractalList->append(new cFractalMsltoeSym3Mod2());
	fractalList->append(new cFractalMsltoeSym3Mod3());
	fractalList->append(new cFractalMsltoeSym4Mod());
	fractalList->append(new cFractalMsltoeToroidal());
	fractalList->append(new cFractalMsltoeToroidalMulti());

	// Quaternion formulas
	fractalList->append(new cFractalQuaternion());
	fractalList->append(new cFractalQuaternion3d());
	fractalList->append(new cFractalQuaternion4d());
	fractalList->append(new cFractalQuaternionCubic4d());

	// Quick Dudley formulas
	fractalList->append(new cFractalQuickDudley());
	fractalList->append(new cFractalQuickDudleyMod());

	// Riemann formulas
	fractalList->append(new cFractalRiemannSphereHoboldPow4());
	fractalList->append(new cFractalRiemannSphereHoboldPow8());
	fractalList->append(new cFractalRiemannSphereHoboldMulti());
	fractalList->append(new cFractalRiemannSphereMsltoeV1());
	fractalList->append(new cFractalRiemannSphereMsltoeV2());
	fractalList->append(new cFractalScatorPower2StdR());
	fractalList->append(new cFractalXenodreambuie());

	// ----- Linear -------

	// Abox formulas
	fractalList->append(new cFractalAboxMod1());
	fractalList->append(new cFractalAboxMod2());
	fractalList->append(new cFractalAboxMod11());
	fractalList->append(new cFractalAboxMod12());
	fractalList->append(new cFractalAboxMod13());
	fractalList->append(new cFractalAboxMod14());
	fractalList->append(new cFractalAboxModKali());
	fractalList->append(new cFractalAboxModKaliEiffie());
	fractalList->append(new cFractalAboxModKaliV2());
	fractalList->append(new cFractalAboxModKaliV3());
	fractalList->append(new cFractalAboxSphere4d());
	fractalList->append(new cFractalAboxSurfBox());
	fractalList->append(new cFractalAboxTetra4d());
	fractalList->append(new cFractalAboxVSIcen1());
	fractalList->append(new cFractalAbox4d());

	// Amazing Surf formulas
	fractalList->append(new cFractalAmazingSurf());
	fractalList->append(new cFractalAmazingSurfMod1());
	fractalList->append(new cFractalAmazingSurfMod2());
	fractalList->append(new cFractalAmazingSurfMod3());
	fractalList->append(new cFractalAmazingSurfMod4());
	fractalList->append(new cFractalAmazingSurfMulti());
	fractalList->append(new cFractalBenesiMagTransforms());

	// Collatz formulas
	fractalList->append(new cFractalCollatz());
	fractalList->append(new cFractalCollatzMod());

	// Folds and IFS
	fractalList->append(new cFractalFoldBoxMod1());
	fractalList->append(new cFractalGeneralizedFoldBox());
	fractalList->append(new cFractalIfsXY());
	fractalList->append(new cFractalKaleidoscopicIfs());
	fractalList->append(new cFractalKalisets1());
	fractalList->append(new cFractalMandalayBoxV1());
	fractalList->append(new cFractalMandalayBoxV2());
	fractalList->append(new cFractalMandalayKIFS());

	// Mandelbox formulas
	fractalList->append(new cFractalMandelbox());
	fractalList->append(new cFractalMandelboxFast());
	fractalList->append(new cFractalMandelboxMenger());
	fractalList->append(new cFractalMandelboxSmooth());
	fractalList->append(new cFractalMandelboxVariable());
	fractalList->append(new cFractalMandelboxVaryScale4d());

	// Menger formulas
	fractalList->append(new cFractalMengerSponge());
	fractalList->append(new cFractalMengerChebyshev());
	fractalList->append(new cFractalMengerCrossKIFS());
	fractalList->append(new cFractalMengerCrossMod1());
	fractalList->append(new cFractalMengerMod1());
	fractalList->append(new cFractalModulusMengerSponge());
	fractalList->append(new cFractalMengerMiddleMod());
	fractalList->append(new cFractalMengerOcto());
	fractalList->append(new cFractalMengerPrismShape());
	fractalList->append(new cFractalMengerPrismShape2());
	fractalList->append(new cFractalMengerPolyFold());
	fractalList->append(new cFractalMengerPwr2Poly());
	fractalList->append(new cFractalMengerSmooth());
	fractalList->append(new cFractalMengerSmoothMod1());
	fractalList->append(new cFractalMenger3());
	fractalList->append(new cFractalMenger4d());
	fractalList->append(new cFractalMenger4dMod1());
	fractalList->append(new cFractalMenger4dMod2());
	fractalList->append(new cFractalMixPinski4d());
	fractalList->append(new cFractalSierpinski3d());
	fractalList->append(new cFractalSierpinski3dV2());
	fractalList->append(new cFractalSierpinski3dV3());
	fractalList->append(new cFractalSierpinski3dV4());
	fractalList->append(new cFractalSierpinski4d());
	fractalList->append(new cFractalVicsek());

	// ----- JosLeys-Kleinian -----
	fractalList->append(new cFractalJosKleinian());
	fractalList->append(new cFractalJosKleinianV2());

	// ----- Pseudo Kleinian ----
	fractalList->append(new cFractalPseudoKleinian());
	fractalList->append(new cFractalPseudoKleinianMod1());
	fractalList->append(new cFractalPseudoKleinianMod2());

	// ----- custom DE - dIFS formulas------
	fractalList->append(new cFractalDIFSBoxV1());
	fractalList->append(new cFractalDIFSBoxDiagonalV1());
	fractalList->append(new cFractalDIFSBoxDiagonalV3());
	fractalList->append(new cFractalDIFSCylinder());
	fractalList->append(new cFractalDIFSEllipsoid());
	fractalList->append(new cFractalDIFSHextgrid2());
	fractalList->append(new cFractalDIFSMenger());
	fractalList->append(new cFractalDIFSMsltoeDonut());
	fractalList->append(new cFractalDIFSPrism());
	fractalList->append(new cFractalDIFSSphere());
	fractalList->append(new cFractalDIFSTorus());
	fractalList->append(new cFractalDIFSMultiV1());

	// ---- custom DE - dIFS transforms----
	fractalList->append(new cFractalTransfDIFSBox());
	fractalList->append(new cFractalTransfDIFSBoxV2());
	fractalList->append(new cFractalTransfDIFSBoxV3());
	fractalList->append(new cFractalTransfDIFSCylinder());
	fractalList->append(new cFractalTransfDIFSCylinderV2());
	fractalList->append(new cFractalTransfDIFSEllipsoid());
	fractalList->append(new cFractalTransfDIFSGrid());
	fractalList->append(new cFractalTransfDIFSGridV2());
	fractalList->append(new cFractalTransfDIFSHextgrid2());
	fractalList->append(new cFractalTransfDIFSHexprism());
	fractalList->append(new cFractalTransfDIFSPolyhedra());
	fractalList->append(new cFractalTransfDIFSPolyhedraV2());
	fractalList->append(new cFractalTransfDIFSPrism());
	fractalList->append(new cFractalTransfDIFSPrismV2());
	fractalList->append(new cFractalTransfDIFSSphere());
	fractalList->append(new cFractalTransfDIFSTorus());
	fractalList->append(new cFractalTransfDIFSTorusV2());
	fractalList->append(new cFractalTransfDIFSTorusV3());
	fractalList->append(new cFractalTransfDIFSTorusGrid());
	fractalList->append(new cFractalTransfDIFSHybridColor());

	// ---- custom DE - non-dIFS Formulas----
	fractalList->append(new cFractalFoldCutCube());
	fractalList->append(new cFractalKoch());
	fractalList->append(new cFractalKochV2());
	fractalList->append(new cFractalKochV3());
	fractalList->append(new cFractalKnotV1());
	fractalList->append(new cFractalKnotV2());
	fractalList->append(new cFractalMengerV2());
	fractalList->append(new cFractalMengerV3());
	fractalList->append(new cFractalOctahedron());
	fractalList->append(new cFractalPseudoKleinianStdDE());
	fractalList->append(new cFractalPseudoKleinianMod3());
	fractalList->append(new cFractalScatorPower2());
	fractalList->append(new cFractalSpheretree());
	fractalList->append(new cFractalSpheretreeV2());

	//------------------------ DELTA DE FORMULAS ------------------------

	// ----- Logarithmic -----

	// Aexion formulas
	fractalList->append(new cFractalAexionOctopusMod());

	// Ides formulas
	fractalList->append(new cFractalIdes());
	fractalList->append(new cFractalIdes2());

	// Riemann formulas
	fractalList->append(new cFractalRiemannBulbMsltoeMod2());
	fractalList->append(new cFractalRiemannSphereMsltoe());
	fractalList->append(new cFractalRiemannSphereMsltoeM3d());

	// Scator Algebra formulas
	fractalList->append(new cFractalScatorPower2Real());
	fractalList->append(new cFractalScatorPower2Imaginary());

	// ----- Linear -----
	fractalList->append(new cFractalBoxFoldBulbPow2());

	// Msltoe formulas
	fractalList->append(new cFractalMsltoeDonut());

	//-------------------------- Transforms -----------------------------------

	// Abs Transforms
	fractalList->append(new cFractalTransfAbsAddConditional());
	fractalList->append(new cFractalTransfAbsAddConditional2());
	fractalList->append(new cFractalTransfAbsAddConstant());
	fractalList->append(new cFractalTransfAbsAddConstant4d());
	fractalList->append(new cFractalTransfAbsAddTgladFold());
	fractalList->append(new cFractalTransfAbsAddTgladFold4d());
	fractalList->append(new cFractalTransfAbsAddMulti());
	fractalList->append(new cFractalTransfAbsAddMulti4d());
	fractalList->append(new cFractalTransfNegAbsAddConstant());
	fractalList->append(new cFractalTransfAbsAddConditional4d());
	fractalList->append(new cFractalTransfAbsRecFoldXY());

	// Add Transforms
	fractalList->append(new cFractalTransfAddConstant());
	fractalList->append(new cFractalTransfAddConstant4d());
	fractalList->append(new cFractalTransfAddConstantMod1());
	fractalList->append(new cFractalTransfAddConstantMod2());
	fractalList->append(new cFractalTransfAddConstantRotV1());
	fractalList->append(new cFractalTransfAddConstantVaryV1());
	fractalList->append(new cFractalTransfAddNorm());
	fractalList->append(new cFractalTransfAddExp2Z());

	// Add Cpixel Transforms
	fractalList->append(new cFractalTransfAddCpixel());
	fractalList->append(new cFractalTransfAddCpixel4d());
	fractalList->append(new cFractalTransfAddCpixelAxisSwap());
	fractalList->append(new cFractalTransfAddCpixelRotated());
	fractalList->append(new cFractalTransfAddCpixelCxCyAxisSwap());
	fractalList->append(new cFractalTransfAddCpixelScator());
	fractalList->append(new cFractalTransfAddCpixelSinOrCos());
	fractalList->append(new cFractalTransfAddCpixelSymmetrical());
	fractalList->append(new cFractalTransfAddCpixelVaryV1());

	// Benesi Transforms
	fractalList->append(new cFractalTransfBenesiT1());
	fractalList->append(new cFractalTransfBenesiT1Mod());
	fractalList->append(new cFractalTransfBenesiT2());
	fractalList->append(new cFractalTransfBenesiT3());
	fractalList->append(new cFractalTransfBenesiT4());
	fractalList->append(new cFractalTransfBenesiT5b());
	fractalList->append(new cFractalTransfBenesiCubeSphere());
	fractalList->append(new cFractalTransfBenesiSphereCube());
	fractalList->append(new cFractalTransfBenesiMagForward());
	fractalList->append(new cFractalTransfBenesiMagBackward());

	// Box Fold Transforms
	fractalList->append(new cFractalTransfBoxFold());
	fractalList->append(new cFractalTransfBoxFold4d());
	fractalList->append(new cFractalTransfBoxFold4dTglad());
	fractalList->append(new cFractalTransfBoxFoldVaryV1());
	fractalList->append(new cFractalTransfBoxFoldXYZ());
	fractalList->append(new cFractalTransfBoxOffset());
	fractalList->append(new cFractalTransfBoxTiling4d());
	fractalList->append(new cFractalTransfBoxTilingV2());
	fractalList->append(new cFractalTransfBoxWrap4d());
	fractalList->append(new cFractalTransfClamp4d());
	fractalList->append(new cFractalTransfDiagonalFold());
	fractalList->append(new cFractalTransfFoldingTetra3d());
	fractalList->append(new cFractalTransfGnarl());
	fractalList->append(new cFractalTransfIterationWeight());
	fractalList->append(new cFractalTransfIterationWeight4d());
	fractalList->append(new cFractalTransfInitial4d());
	fractalList->append(new cFractalTransfInvCylindrical());
	fractalList->append(new cFractalTransfLinCombineCXYZ());
	fractalList->append(new cFractalTransfMandalayFoldV1());
	fractalList->append(new cFractalTransfMandalayFoldV2());
	fractalList->append(new cFractalTransfMandalayFold4d());
	fractalList->append(new cFractalTransfMengerFold());
	fractalList->append(new cFractalTransfMultipleAngle());
	fractalList->append(new cFractalTransfOctoFold());
	fractalList->append(new cFractalTransfOffsetSCurve4d());
	fractalList->append(new cFractalTransfParabFold());
	fractalList->append(new cFractalTransfPlatonicSolid());
	fractalList->append(new cFractalTransfPolyFoldAtan());
	fractalList->append(new cFractalTransfPolyFoldAtan2());
	fractalList->append(new cFractalTransfPolyFoldSymXY());
	fractalList->append(new cFractalTransfPolyFoldSymMulti());
	fractalList->append(new cFractalTransfPwr2Polynomial());
	fractalList->append(new cFractalTransfQuaternionFold());

	// R Power Transforms
	fractalList->append(new cFractalTransfRpow3());
	fractalList->append(new cFractalTransfRPower());
	fractalList->append(new cFractalTransfReciprocal3());
	fractalList->append(new cFractalTransfReciprocal4d());

	// Rotation Transforms
	fractalList->append(new cFractalTransfRotation());
	fractalList->append(new cFractalTransfRotation4d());
	fractalList->append(new cFractalTransfRotateAboutVec3());
	fractalList->append(new cFractalTransfRotationChebyshev());
	fractalList->append(new cFractalTransfRotationIterControls());
	fractalList->append(new cFractalTransfRotationVaryV1());
	fractalList->append(new cFractalTransfRotationFoldingPlane());
	fractalList->append(new cFractalTransfRotationFolding());

	// Scale Transforms
	fractalList->append(new cFractalTransfScale());
	fractalList->append(new cFractalTransfScaleOffset());
	fractalList->append(new cFractalTransfScaleOffsetV2());
	fractalList->append(new cFractalTransfScaleVaryV212());
	fractalList->append(new cFractalTransfScaleVaryMulti());
	fractalList->append(new cFractalTransfScaleVaryV1());
	fractalList->append(new cFractalTransfScaleVaryVCL());
	fractalList->append(new cFractalTransfScale3d());
	fractalList->append(new cFractalTransfScale4d());

	// Sin Cos Transforms
	fractalList->append(new cFractalTransfSinAdd());
	fractalList->append(new cFractalTransfSinOrCos());
	fractalList->append(new cFractalTransfSinAndCos());
	fractalList->append(new cFractalTransfSinAndCosMax());

	fractalList->append(new cFractalTransfSmooth());
	fractalList->append(new cFractalTransfSmoothV2());

	// Spherical Transforms
	fractalList->append(new cFractalTransfSphericalFold());
	fractalList->append(new cFractalTransfSphericalFold4d());
	fractalList->append(new cFractalTransfSphericalFold4dV2());
	fractalList->append(new cFractalTransfSphericalFoldAbox());
	fractalList->append(new cFractalTransfSphericalFoldCHS());
	fractalList->append(new cFractalTransfSphericalFoldCuboid());
	fractalList->append(new cFractalTransfSphericalFoldParab());
	fractalList->append(new cFractalTransfSphericalFoldVaryV1());
	fractalList->append(new cFractalTransfSphericalFoldVaryVCL());
	fractalList->append(new cFractalTransfSphericalFoldV1());
	fractalList->append(new cFractalTransfSphericalFoldV2());
	fractalList->append(new cFractalTransfSphericalFoldXYZBias());
	fractalList->append(new cFractalTransfSphericalInv());
	fractalList->append(new cFractalTransfSphericalInvV2());
	fractalList->append(new cFractalTransfSphericalInvC());
	fractalList->append(new cFractalTransfSphericalPwrFold());
	fractalList->append(new cFractalTransfSphericalOffset());
	fractalList->append(new cFractalTransfSphericalOffsetVCL());

	fractalList->append(new cFractalTransfStepXY());

	// Surf Transforms
	fractalList->append(new cFractalTransfSurfBoxFold());
	fractalList->append(new cFractalTransfSurfBoxFoldV2());
	fractalList->append(new cFractalTransfSurfBoxFoldV24d());
	fractalList->append(new cFractalTransfSurfFoldMulti());
	fractalList->append(new cFractalTransfZvectorAxisSwap());
	fractalList->append(new cFractalTransfDEControls());
	fractalList->append(new cFractalTransfDELinearCube());

	// ------------------ hybrid color trial -----------------------
	fractalList->append(new cFractalTransfHybridColor());
	fractalList->append(new cFractalTransfHybridColor2());

	// ------------------ experimental  -----------------------
	fractalList->append(new cFractalTesting());
	fractalList->append(new cFractalTestingLog());
	fractalList->append(new cFractalTesting4d());
	fractalList->append(new cFractalTransfBlockify());
	fractalList->append(new cFractalTestingTransform());
	fractalList->append(new cFractalTestingTransform2());
	//====================== FRACTAL LIST - END ==================

	for (cAbstractFractal *fractal : newFractalList)
	{
		fractal->CheckForErrors();
	}
}
