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
void DefineFractalList(QList<cAbstractFractal *> *fractals)
{
	//====================== FRACTAL LIST - START ==================
	fractals->append(new cFractalNone());
#ifdef USE_OPENCL
	fractals->append(new cFractalCustom());
#endif
	//------------------------ ANALYTIC DE FORMULAS ----------------------
	// ----- Logarithmic -------
	fractals->append(new cFractalAexion());
	fractals->append(new cFractalAexion4dV2());

	// Benesi formulas
	fractals->append(new cFractalBenesiPineTree());
	fractals->append(new cFractalBenesi());
	fractals->append(new cFractalBenesiPwr2Mandelbulb());
	fractals->append(new cFractalBenesiT1PineTree());

	// boxBulb formulas
	fractals->append(new cFractalBoxFoldBulbPow2V2());
	fractals->append(new cFractalBoxFoldBulbPow2V3());
	fractals->append(new cFractalBoxFoldBulbMenger());
	fractals->append(new cFractalBoxFoldQuat());
	fractals->append(new cFractalBoxFold4dBulbPow2());
	fractals->append(new cFractalBoxFoldBulbV4());

	fractals->append(new cFractalBristorbrot());
	fractals->append(new cFractalBristorbrot2());
	fractals->append(new cFractalBristorbrot4d());
	fractals->append(new cFractalBuffalo());

	// Nested sin() formulas
	fractals->append(new cFractalCoastalbrot());

	// hypercomlex
	fractals->append(new cFractalHypercomplex());
	fractals->append(new cFractalHypercomplexV2());
	fractals->append(new cFractalIqBulb());
	fractals->append(new cFractalLkmitch());
	fractals->append(new cFractalMakin3d2());

	// Mandelbulb formulas
	fractals->append(new cFractalMandelbar());
	fractals->append(new cFractalMandelbarV2());
	fractals->append(new cFractalMandelbarV3());

	fractals->append(new cFractalMandelbulb());
	fractals->append(new cFractalMandelbulb2());
	fractals->append(new cFractalMandelbulb3());
	fractals->append(new cFractalMandelbulb4());
	fractals->append(new cFractalMandelbulbAbs());
	fractals->append(new cFractalMandelbulbAbsPower2());
	fractals->append(new cFractalMandelbulbAtan2Power2());
	fractals->append(new cFractalMandelbulbBermarte());
	fractals->append(new cFractalMandelbulbEye());
	fractals->append(new cFractalMandelbulbIqPow8());
	fractals->append(new cFractalMandelbulbIqV2());
	fractals->append(new cFractalMandelbulbJuliabulb());
	fractals->append(new cFractalMandelbulbKali());
	fractals->append(new cFractalMandelbulbKaliMulti());
	fractals->append(new cFractalMandelbulbKosalos());
	fractals->append(new cFractalMandelbulbKosalosV2());
	fractals->append(new cFractalMandelbulbLambda());
	fractals->append(new cFractalModulusMandelbulb());
	fractals->append(new cFractalMandelbulbMulti());
	fractals->append(new cFractalMandelbulbMulti2());
	fractals->append(new cFractalMandelbulbPlusZ());
	fractals->append(new cFractalMandelbulbPower1234());
	fractals->append(new cFractalMandelbulbPower2());
	fractals->append(new cFractalMandelbulbPow2V1());
	fractals->append(new cFractalMandelbulbPow2V2());
	fractals->append(new cFractalMandelbulbPow2V3());
	fractals->append(new cFractalMandelbulbPupuku());
	fractals->append(new cFractalMandelbulbQuadrat());
	fractals->append(new cFractalMandelbulbQuat());
	fractals->append(new cFractalMandelbulbSinCos());
	fractals->append(new cFractalMandelbulbSinCosV2());
	fractals->append(new cFractalMandelbulbSinCosV3());
	fractals->append(new cFractalMandelbulbSinCosV4());
	fractals->append(new cFractalChebyshevBulb());
	fractals->append(new cFractalMandelbulbVaryPowerV1());
	fractals->append(new cFractalMandelcup());
	fractals->append(new cFractalMandelnest());
	fractals->append(new cFractalMandelnestFull());
	fractals->append(new cFractalMandelnestV2());
	fractals->append(new cFractalMandelnest4d());
	fractals->append(new cFractalMandeltorus());
	fractals->append(new cFractalMandeltorusV2());

	// Msltoe formulas
	fractals->append(new cFractalMsltoeSym2Mod());
	fractals->append(new cFractalMsltoeSym3Mod());
	fractals->append(new cFractalEiffieMsltoe());
	fractals->append(new cFractalMsltoeSym3Mod2());
	fractals->append(new cFractalMsltoeSym3Mod3());
	fractals->append(new cFractalMsltoeSym3Mod4());
	fractals->append(new cFractalMsltoeSym3Mod5());
	fractals->append(new cFractalMsltoeSym3Mod6());
	fractals->append(new cFractalMsltoeSym4Mod());
	fractals->append(new cFractalMsltoeSym4Mod1());
	fractals->append(new cFractalMsltoeToroidal());
	fractals->append(new cFractalMsltoeToroidalV2());
	fractals->append(new cFractalMsltoeToroidalMulti());




	// Quaternion formulas
	fractals->append(new cFractalQuaternion());
	fractals->append(new cFractalQuaternion3d());
	fractals->append(new cFractalQuaternion4d());
	fractals->append(new cFractalQuaternionCubic4d());

	// Quick Dudley formulas
	fractals->append(new cFractalQuickDudley());
	fractals->append(new cFractalQuickDudleyMod());

	// Riemann formulas
	fractals->append(new cFractalRiemannSphereHoboldPow4());
	fractals->append(new cFractalRiemannSphereHoboldPow8());
	fractals->append(new cFractalRiemannSphereHoboldMulti());
	fractals->append(new cFractalRiemannSphereMsltoeV1());
	fractals->append(new cFractalRiemannSphereMsltoeV2());

	fractals->append(new cFractalXenodreambuie());
	fractals->append(new cFractalXenodreambuieV2());
	fractals->append(new cFractalXenodreambuieV3());

	// ----- Linear -------

	// Abox formulas
	fractals->append(new cFractalAboxMod1());
	fractals->append(new cFractalAboxMb3d1());
	fractals->append(new cFractalAboxMod2());
	fractals->append(new cFractalAboxMod11());
	fractals->append(new cFractalAboxMod12());
	fractals->append(new cFractalAboxMod13());
	fractals->append(new cFractalAboxMod14());
	fractals->append(new cFractalAboxMod15());
	fractals->append(new cFractalAboxModKali());
	fractals->append(new cFractalAboxModKaliEiffie());
	fractals->append(new cFractalAboxModKaliV2());
	fractals->append(new cFractalAboxModKaliV3());
	fractals->append(new cFractalAboxDonut4d());
	fractals->append(new cFractalAboxDonut4dV2());
	fractals->append(new cFractalAboxKlein());
	fractals->append(new cFractalAboxSphere4d());
	fractals->append(new cFractalAboxSmooth());
	fractals->append(new cFractalAboxSurfBox());
	fractals->append(new cFractalAboxTetra());
	fractals->append(new cFractalAboxTetra4d());
	fractals->append(new cFractalAboxVSIcen1());
	fractals->append(new cFractalAbox4d());

	// Amazing Surf formulas
	fractals->append(new cFractalAmazingIfs());
	fractals->append(new cFractalAmazingSurf());
	fractals->append(new cFractalAmazingSurfMod1());
	fractals->append(new cFractalAmazingSurfMod2());
	fractals->append(new cFractalAmazingSurfMod3());
	fractals->append(new cFractalAmazingSurfMod4());
	fractals->append(new cFractalAmazingSurfKlein());
	fractals->append(new cFractalAmazingSurfKleinV2());
	fractals->append(new cFractalAmazingSurfM3d());
	fractals->append(new cFractalAmazingSurfMulti());

	fractals->append(new cFractalBenesiMagTransforms());

	// Collatz formulas
	fractals->append(new cFractalCollatz());
	fractals->append(new cFractalCollatzMod());

	// Folds and IFS
	fractals->append(new cFractalFoldBoxMod1());
	fractals->append(new cFractalGeneralizedFoldBox());
	fractals->append(new cFractalIfsGen());
	fractals->append(new cFractalIfsXY());
	fractals->append(new cFractalKaleidoscopicIfs());
	fractals->append(new cFractalKalisets1());
	fractals->append(new cFractalKochIfs());

	fractals->append(new cFractalMandalayBoxV1());
	fractals->append(new cFractalMandalayBoxV2());
	fractals->append(new cFractalMandalayKIFS());

	// Mandelbox formulas
	fractals->append(new cFractalMandelbox());
	fractals->append(new cFractalMandelboxFast());
	fractals->append(new cFractalMandelboxMenger());
	fractals->append(new cFractalDualPowerMandelbox());
	fractals->append(new cFractalMandelboxSmooth());
	fractals->append(new cFractalMandelboxVariable());
	fractals->append(new cFractalMandelboxVaryScale4d());

	// Menger formulas
	fractals->append(new cFractalMengerSponge());
	fractals->append(new cFractalMengerChebyshev());
	fractals->append(new cFractalMengerCrossKIFS());
	fractals->append(new cFractalMengerCrossMod1());
	fractals->append(new cFractalMengerMod1());
	fractals->append(new cFractalModulusMengerSponge());
	fractals->append(new cFractalMengerMiddleMod());
	fractals->append(new cFractalMengerOcto());
	fractals->append(new cFractalMengerPrismShape());
	fractals->append(new cFractalMengerPrismShape2());
	fractals->append(new cFractalMengerPolyFold());
	fractals->append(new cFractalMengerPwr2Poly());
	fractals->append(new cFractalMengerPyramid());
	fractals->append(new cFractalMengerSmooth());
	fractals->append(new cFractalMengerSmoothMod1());
	fractals->append(new cFractalMengerSmoothChebyshev());
	fractals->append(new cFractalMengerV4());
	fractals->append(new cFractalMengerV5());
	fractals->append(new cFractalMenger3());
	fractals->append(new cFractalMenger3M3d());
	fractals->append(new cFractalMenger4d());
	fractals->append(new cFractalMenger4dMod1());
	fractals->append(new cFractalMenger4dMod2());

	fractals->append(new cFractalMixPinski4d());
	fractals->append(new cFractalSierpinski3d());
	fractals->append(new cFractalSierpinski3dV2());
	fractals->append(new cFractalSierpinski3dV3());
	fractals->append(new cFractalSierpinski3dV4());
	fractals->append(new cFractalSierpinski4d());
	fractals->append(new cFractalSpheretreeV4());
	fractals->append(new cFractalVicsek());

	// ----- JosLeys-Kleinian -----
	fractals->append(new cFractalJosKleinian());
	fractals->append(new cFractalJosKleinianV2());

	// ----- Pseudo Kleinian ----
	fractals->append(new cFractalPseudoKleinian());
	fractals->append(new cFractalPseudoKleinianMod1());
	fractals->append(new cFractalPseudoKleinianMod2());
	fractals->append(new cFractalPseudoKleinianMod4());
	fractals->append(new cFractalPseudoKleinianMod5());


	// ----- custom DE - dIFS formulas------
	fractals->append(new cFractalDIFSAmazingIfs());
	fractals->append(new cFractalDIFSBoxV1());
	fractals->append(new cFractalDIFSBoxDiagonalV1());
	fractals->append(new cFractalDIFSBoxDiagonalV3());
	fractals->append(new cFractalDIFSCylinder());
	fractals->append(new cFractalDIFSEllipsoid());
	fractals->append(new cFractalDIFSGreekIfs());
	fractals->append(new cFractalDIFSHextgrid2());
	fractals->append(new cFractalDIFSMenger());
	fractals->append(new cFractalDIFSMsltoeDonut());
	fractals->append(new cFractalDIFSPrism());
	fractals->append(new cFractalDIFSSphere());
	fractals->append(new cFractalDIFSTorus());
	fractals->append(new cFractalDIFSMultiV1());

	// ---- custom DE - dIFS transforms----
	fractals->append(new cFractalTransfDIFSAmazingIfs());
	fractals->append(new cFractalTransfDIFSBox());
	fractals->append(new cFractalTransfDIFSBoxV2());
	fractals->append(new cFractalTransfDIFSBoxV3());
	fractals->append(new cFractalTransfDIFSBoxFrame());
	fractals->append(new cFractalTransfDIFSCayley2());
	fractals->append(new cFractalTransfDIFSChessboard());
	fractals->append(new cFractalTransfDIFSClipCustom());
	fractals->append(new cFractalTransfDIFSClipPlane());
	fractals->append(new cFractalTransfDIFSCylinder());
	fractals->append(new cFractalTransfDIFSCylinderV2());
	fractals->append(new cFractalTransfDIFSDiamond());
	fractals->append(new cFractalTransfDIFSEllipsoid());
	fractals->append(new cFractalTransfDIFSGearV1());
	fractals->append(new cFractalTransfDIFSGrid());
	fractals->append(new cFractalTransfDIFSGridV2());
	fractals->append(new cFractalTransfDIFSGridV3());
	fractals->append(new cFractalTransfDIFSHeart());
	fractals->append(new cFractalTransfDIFSHelix());
	fractals->append(new cFractalTransfDIFSHelixV2());
	fractals->append(new cFractalTransfDIFSHelixMenger());
	fractals->append(new cFractalTransfDIFSHextgrid2());
	fractals->append(new cFractalTransfDIFSHexprism());
	fractals->append(new cFractalTransfDIFSHexprismV2());
	fractals->append(new cFractalTransfDIFSOctahedron());
	fractals->append(new cFractalTransfDIFSOctahedronV2());
	fractals->append(new cFractalTransfDIFSPiriform());
	fractals->append(new cFractalTransfDIFSPolyhedra());
	fractals->append(new cFractalTransfDIFSPolyhedraV2());
	fractals->append(new cFractalTransfDIFSPrism());
	fractals->append(new cFractalTransfDIFSPrismV2());
	fractals->append(new cFractalTransfDIFSSphere());
	fractals->append(new cFractalTransfDIFSSphereGrid());
	fractals->append(new cFractalTransfDIFSSphereGridV2());
	fractals->append(new cFractalTransfDIFSSphereGridV3());
	fractals->append(new cFractalTransfDIFSSpring());
	fractals->append(new cFractalTransfDIFSSupershape());
	fractals->append(new cFractalTransfDIFSSupershapeV2());
	fractals->append(new cFractalTransfDIFSTorus());
	fractals->append(new cFractalTransfDIFSTorusV2());
	fractals->append(new cFractalTransfDIFSTorusV3());
	fractals->append(new cFractalTransfDIFSTorusV4());
	fractals->append(new cFractalTransfDIFSTube());
	fractals->append(new cFractalTransfDIFSTorusGrid());
	fractals->append(new cFractalTransfDIFSTorusMenger());
	fractals->append(new cFractalTransfDIFSTorusTwist());
	fractals->append(new cFractalTransfDIFSTriGrid());

	fractals->append(new cFractalTransfDIFSHybridColor());

	// ---- custom DE - non-dIFS Formulas----
	fractals->append(new cFractalFoldCutCube());
	fractals->append(new cFractalJosKleinianV3());
	fractals->append(new cFractalJosKleinianV4());
	fractals->append(new cFractalKoch());
	fractals->append(new cFractalKochV2());
	fractals->append(new cFractalKochV3());
	fractals->append(new cFractalKochV4());
	fractals->append(new cFractalKochV5());
	fractals->append(new cFractalKnotV1());
	fractals->append(new cFractalKnotV2());
	fractals->append(new cFractalMengerV2());
	fractals->append(new cFractalMengerV3());
	fractals->append(new cFractalMengerV6());
    fractals->append(new cFractalMengerV7());
	fractals->append(new cFractalOctahedron());
	fractals->append(new cFractalPseudoKleinianStdDE());
	fractals->append(new cFractalPseudoKleinianMod3());
	fractals->append(new cFractalPseudoKleinianMod6());
	fractals->append(new cFractalPseudoKleinian4d());
	fractals->append(new cFractalPseudoKleinianTrig());
	fractals->append(new cFractalSphereCluster());
	fractals->append(new cFractalSphereClusterV2());
	fractals->append(new cFractalSphereClusterV3());
	fractals->append(new cFractalSpheretree());
	fractals->append(new cFractalSpheretreeV2());
    fractals->append(new cFractalSpheretreeV5());
	fractals->append(new cFractalTransfDEControls());
	fractals->append(new cFractalTransfDELinearCube());

	//------------------------ DELTA DE FORMULAS ------------------------

	// ----- Logarithmic -----
	fractals->append(new cFractalAexionOctopusMod());

	fractals->append(new cFractalIdes());
	fractals->append(new cFractalIdes2());

	fractals->append(new cFractalMandelbulbTails());
	fractals->append(new cFractalMandelbulbTailsV2());
	fractals->append(new cFractalNewtonPow3());
	fractals->append(new cFractalRiemannBulbMsltoeMod2());
	fractals->append(new cFractalRiemannSphereMsltoe());
	fractals->append(new cFractalRiemannSphereMsltoeM3d());

	fractals->append(new cFractalScatorPower2Real());
	fractals->append(new cFractalScatorPower2Imaginary());

	// ----- Linear -----
	fractals->append(new cFractalBoxFoldBulbPow2());
	fractals->append(new cFractalMsltoeDonut());


	//-------------------------- Transforms -----------------------------------

	// Abs Transforms
	fractals->append(new cFractalTransfAbsAddConditional());
	fractals->append(new cFractalTransfAbsAddConditional2());
	fractals->append(new cFractalTransfAbsAddConstant());
	fractals->append(new cFractalTransfAbsAddConstantV2());
	fractals->append(new cFractalTransfAbsAddConstant4d());
	fractals->append(new cFractalTransfAbsAddTgladFold());
	fractals->append(new cFractalTransfAbsAddTgladFold4d());
	fractals->append(new cFractalTransfAbsAddMulti());
	fractals->append(new cFractalTransfAbsAddMulti4d());
	fractals->append(new cFractalTransfNegAbsAddConstant());
	fractals->append(new cFractalTransfAbsAddConditional4d());
	fractals->append(new cFractalTransfAbsRecFoldXY());
	fractals->append(new cFractalTransfAbsSym3());

	// Add Transforms
	fractals->append(new cFractalTransfAddConstant());
	fractals->append(new cFractalTransfAddConstant4d());
	fractals->append(new cFractalTransfAddConstantMod1());
	fractals->append(new cFractalTransfAddConstantMod2());
	fractals->append(new cFractalTransfAddConstantMod3());
	fractals->append(new cFractalTransfAddConstantRotV1());
	fractals->append(new cFractalTransfAddConstantVaryV1());
	fractals->append(new cFractalTransfAddNorm());
	fractals->append(new cFractalTransfAddExp2Z());
	fractals->append(new cFractalTransfAddScaleRotate());
	fractals->append(new cFractalTransfAddSphericalInvert());

	// Add Cpixel Transforms
	fractals->append(new cFractalTransfAddCpixel());
	fractals->append(new cFractalTransfAddCpixel4d());
	fractals->append(new cFractalTransfAddCpixelAxisSwap());
	fractals->append(new cFractalTransfAddCpixelRotated());
	fractals->append(new cFractalTransfAddCpixelCxCyAxisSwap());
	fractals->append(new cFractalTransfAddCpixelInvert());
	fractals->append(new cFractalTransfAddCpixelScator());
	fractals->append(new cFractalTransfAddCpixelSinOrCos());
	fractals->append(new cFractalTransfAddCpixelSphereFold());
	fractals->append(new cFractalTransfAddCpixelSymmetrical());
	fractals->append(new cFractalTransfAddCpixelTile());
	fractals->append(new cFractalTransfAddCpixelVaryV1());

	// Benesi Transforms
	fractals->append(new cFractalTransfBenesiT1());
	fractals->append(new cFractalTransfBenesiT1Mod());
	fractals->append(new cFractalTransfBenesiT2());
	fractals->append(new cFractalTransfBenesiT3());
	fractals->append(new cFractalTransfBenesiT4());
	fractals->append(new cFractalTransfBenesiT5b());
	fractals->append(new cFractalTransfBenesiCubeSphere());
	fractals->append(new cFractalTransfBenesiSphereCube());
	fractals->append(new cFractalTransfBenesiMagForward());
	fractals->append(new cFractalTransfBenesiMagBackward());

	fractals->append(new cFractalTransfBlockify());
	fractals->append(new cFractalTransfBlockifyV2());

	// Box Fold Transforms
	fractals->append(new cFractalTransfBoxFold());
	fractals->append(new cFractalTransfBoxFold4d());
	fractals->append(new cFractalTransfBoxFold4dTglad());
	fractals->append(new cFractalTransfBoxFold4dInfy());
	fractals->append(new cFractalTransfBoxFoldVaryV1());
	fractals->append(new cFractalTransfBoxFoldXYZ());

	fractals->append(new cFractalTransfBoxOffset());
	fractals->append(new cFractalTransfBoxTiling4d());
	fractals->append(new cFractalTransfBoxTilingV2());
	fractals->append(new cFractalTransfBoxTilingV3());
	fractals->append(new cFractalTransfBoxWrap4d());
	fractals->append(new cFractalTransfCayley2V1());
	fractals->append(new cFractalTransfClamp4d());
	fractals->append(new cFractalTransfDiagonalFold());
	fractals->append(new cFractalTransfDotFold());
	fractals->append(new cFractalTransfFoldingTetra3d());
	fractals->append(new cFractalTransfGnarl());
	fractals->append(new cFractalTransfIterationWeight());
	fractals->append(new cFractalTransfIterationWeight4d());
	fractals->append(new cFractalTransfInitial4d());
	fractals->append(new cFractalTransfInvCylindrical());
	fractals->append(new cFractalTransfJuliabox());
	fractals->append(new cFractalTransfJuliaboxV2());
	fractals->append(new cFractalTransfLinCombineCXYZ());
	fractals->append(new cFractalTransfLowResMode());
	fractals->append(new cFractalTransfMandalayFoldV1());
	fractals->append(new cFractalTransfMandalayFoldV2());
	fractals->append(new cFractalTransfMandalayFold4d());
	fractals->append(new cFractalTransfMengerFold());
	fractals->append(new cFractalTransfMengerFoldV2());
	fractals->append(new cFractalTransfMultipleAngle());
	fractals->append(new cFractalTransfOctoFold());
	fractals->append(new cFractalTransfOffsetSCurve4d());
	fractals->append(new cFractalTransfParabFold());
	fractals->append(new cFractalTransfPlatonicSolid());
	fractals->append(new cFractalTransfPolyFoldAtan());
	fractals->append(new cFractalTransfPolyFoldAtan2());
	fractals->append(new cFractalTransfPolyFoldAtan2Iter());
	fractals->append(new cFractalTransfPolyFoldSymXY());
	fractals->append(new cFractalTransfPolyFoldSymMulti());
	fractals->append(new cFractalTransfPolyXYFoldV1());
	fractals->append(new cFractalTransfPwr2Polynomial());
	fractals->append(new cFractalTransfQuadraticFold4d());
	fractals->append(new cFractalTransfQuaternionFold());

	// R Power Transforms
	fractals->append(new cFractalTransfRpow3());
	fractals->append(new cFractalTransfRPower());
	fractals->append(new cFractalTransfReciprocal3());
	fractals->append(new cFractalTransfReciprocal4d());

	// Rotation Transforms
	fractals->append(new cFractalTransfRotation());
	fractals->append(new cFractalTransfRotationM3d());
	fractals->append(new cFractalTransfRotation4d());
	fractals->append(new cFractalTransfRotateAboutVec3());
	fractals->append(new cFractalTransfRotationChebyshev());
	fractals->append(new cFractalTransfRotationIterControls());
	fractals->append(new cFractalTransfRotationVaryV1());
	fractals->append(new cFractalTransfRotationFoldingPlane());
	fractals->append(new cFractalTransfRotationFolding());

	// Scale Transforms
	fractals->append(new cFractalTransfScale());
	fractals->append(new cFractalTransfScaleOffset());
	fractals->append(new cFractalTransfScaleOffsetV2());
	fractals->append(new cFractalTransfScaleVaryV212());
	fractals->append(new cFractalTransfScaleVaryMulti());
	fractals->append(new cFractalTransfScaleVaryV1());
	fractals->append(new cFractalTransfScaleVaryVCL());
	fractals->append(new cFractalTransfScale3d());
	fractals->append(new cFractalTransfScale4d());

	// Trig Transforms
	fractals->append(new cFractalTransfSinAdd());
	fractals->append(new cFractalTransfSinOrCos());
	fractals->append(new cFractalTransfSinAndCos());
	fractals->append(new cFractalTransfSinAndCosMax());
	fractals->append(new cFractalTransfSincos());
	fractals->append(new cFractalTransfSincosV2());
	fractals->append(new cFractalTransfSincosHelix());
	fractals->append(new cFractalTransfSinTan());
	fractals->append(new cFractalTransfSinYM3d());

	fractals->append(new cFractalTransfSmooth());
	fractals->append(new cFractalTransfSmoothV2());

	// Spherical Transforms
	fractals->append(new cFractalTransfSphericalCoordInvs());
	fractals->append(new cFractalTransfSphericalFold());
	fractals->append(new cFractalTransfSphericalFold4d());
	fractals->append(new cFractalTransfSphericalFold4dV2());
	fractals->append(new cFractalTransfSphericalFoldAbox());
	fractals->append(new cFractalTransfSphericalFoldCHS());
	fractals->append(new cFractalTransfSphericalFoldCuboid());
	fractals->append(new cFractalTransfSphericalFoldParab());
	fractals->append(new cFractalTransfSphericalFoldPnorm());
	fractals->append(new cFractalTransfSphericalFoldSmooth());
	fractals->append(new cFractalTransfSphericalFoldVaryV1());
	fractals->append(new cFractalTransfSphericalFoldVaryVCL());
	fractals->append(new cFractalTransfSphericalFoldV1());
	fractals->append(new cFractalTransfSphericalFoldV2());
	fractals->append(new cFractalTransfSphericalFoldV3());
	fractals->append(new cFractalTransfSphericalFoldXYZBias());
	fractals->append(new cFractalTransfSphericalInv());
	fractals->append(new cFractalTransfSphericalInvV2());
	fractals->append(new cFractalTransfSphericalInvV3());
	fractals->append(new cFractalTransfSphericalInvV4());
	fractals->append(new cFractalTransfSphericalInvPnorm());
	fractals->append(new cFractalTransfSphericalInvC());
	fractals->append(new cFractalTransfSphericalPwrFold());
	fractals->append(new cFractalTransfSphericalOffset());
	fractals->append(new cFractalTransfSphericalOffsetVCL());


	fractals->append(new cFractalTransfStepXY());

	// Surf Transforms
	fractals->append(new cFractalTransfSurfBoxFold());
	fractals->append(new cFractalTransfSurfBoxFoldV2());
	fractals->append(new cFractalTransfSurfBoxFoldV24d());
	fractals->append(new cFractalTransfSurfFoldMulti());
	fractals->append(new cFractalTransfZvectorAxisSwap());


	// ------------------ hybrid color trial -----------------------
	fractals->append(new cFractalTransfHybridColor());
	fractals->append(new cFractalTransfHybridColor2());

	// ---------- experimental do not use -----------------------
	fractals->append(new cFractalTesting());
	fractals->append(new cFractalTestingLog());
	fractals->append(new cFractalTesting4d());
	fractals->append(new cFractalTestingTransform());
	fractals->append(new cFractalTestingTransform2());


	fractals->append(new cFractalEiffieCar());
	fractals->append(new cFractalMsltoeToroidalV3());
	fractals->append(new cFractalMandelbulbPow2V4());

	fractals->append(new cFractalPseudoKleinianMod7());


	fractals->append(new cFractalPseudoKleinianTrigV2());
	fractals->append(new cFractalPseudoKleinianTrigV3());

	fractals->append(new cFractalTinkerTowers());

	fractals->append(new cFractalTransfSupershape());
	fractals->append(new cFractalScatorPower2StdR());
	fractals->append(new cFractalScatorPower2());
	fractals->append(new cFractalScatorTest());



	//====================== FRACTAL LIST - END ==================

	for (cAbstractFractal *fractal : newFractalList)
	{
		fractal->CheckForErrors();
	}
}
