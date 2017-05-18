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
 * Compute - function fractal computation
 */

#include "compute_fractal.hpp"
#include "common_math.h"
#include "fractal.h"
#include "fractal_formulas.hpp"
#include "nine_fractals.hpp"

// temporary functions for performance profiling
/*
 long int perf = 0;
 int perfCount = 0;
 inline unsigned long int rdtsc()
 {
 timespec ts;
 clock_gettime(CLOCK_REALTIME, &ts);
 return (unsigned long int)ts.tv_sec * 1000000000LL + (unsigned long int)ts.tv_nsec;
 }
 */

using namespace fractal;

template <fractal::enumCalculationMode Mode>
void Compute(const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out)
{
	// QTextStream outStream(stdout);
	// clock_t tim;
	// tim = rdtsc();

	// repeat, move and rotate
	CVector3 point2 = in.point.mod(in.common.repeat) - in.common.fractalPosition;
	point2 = in.common.mRotFractalRotation.RotateVector(point2);

	CVector4 z = CVector4(point2, 0.0);
	double r = z.Length();
	double minimumR = 100.0;

	if (in.forcedFormulaIndex >= 0)
	{
		z.w = fractals.GetInitialWAxis(in.forcedFormulaIndex);
	}
	else
	{
		z.w = fractals.GetInitialWAxis(0);
	}

	double orbitTrapTotal = 0.0;

	enumFractalFormula formula = fractal::none;

	if (in.common.iterThreshMode)
		out->maxiter = true;
	else
		out->maxiter = false;

	int fractalIndex = 0;
	if (in.forcedFormulaIndex >= 0) fractalIndex = in.forcedFormulaIndex;

	const sFractal *defaultFractal = fractals.GetFractal(fractalIndex);

	sExtendedAux extendedAux;

	extendedAux.r_dz = 1.0;
	extendedAux.r = r;
	extendedAux.color = 1.0;
	extendedAux.actualScale = fractals.GetFractal(fractalIndex)->mandelbox.scale;
	extendedAux.DE = 1.0;
	extendedAux.c = z;
	extendedAux.const_c = z;
	extendedAux.cw = 0;
	extendedAux.foldFactor = 0.0;
	extendedAux.minRFactor = 0.0;
	extendedAux.scaleFactor = 0.0;
	// extendedAux.newR = 1e+20;
	// extendedAux.axisBias = 1e+20;
	// extendedAux.orbitTraps = 1e+20;
	extendedAux.pseudoKleinianDE = 1.0;
	// extendedAux.pseudoKleinianZZ = 1.0;

	// main iteration loop
	int i;
	int sequence;

	CVector4 lastGoodZ;
	CVector4 lastZ;

	for (i = 0; i < in.maxN; i++)
	{
		lastGoodZ = lastZ;
		lastZ = z;

		// hybrid fractal sequence
		if (in.forcedFormulaIndex >= 0)
		{
			sequence = in.forcedFormulaIndex;
		}
		else
		{
			sequence = fractals.GetSequence(i);
		}

		// foldings
		if (in.common.foldings.boxEnable)
		{
			BoxFolding(z, &in.common.foldings, extendedAux);
			r = z.Length();
		}

		if (in.common.foldings.sphericalEnable)
		{
			extendedAux.r = r;
			SphericalFolding(z, &in.common.foldings, extendedAux);
			r = z.Length();
		}

		const sFractal *fractal = fractals.GetFractal(sequence);
		formula = fractal->formula;

		// temporary vector for weight function
		CVector4 tempZ = z;

		extendedAux.r = r;
		extendedAux.i = i;

		if (!fractals.IsHybrid() || fractals.GetWeight(sequence) > 0.0)
		{
			// calls for fractal formulas
			switch (formula)
			{

				case mandelbulb:
				{
					MandelbulbIteration(z, fractal, extendedAux);
					break;
				}
				case mandelbulb2:
				{
					Mandelbulb2Iteration(z, fractal, extendedAux);
					break;
				}
				case mandelbulb3:
				{
					Mandelbulb3Iteration(z, fractal, extendedAux);
					break;
				}
				case mandelbulb4:
				{
					Mandelbulb4Iteration(z, fractal, extendedAux);
					break;
				}
				case mandelbulbPower2:
				{
					MandelbulbPower2Iteration(z, fractal, extendedAux);
					break;
				}
				case xenodreambuie:
				{
					XenodreambuieIteration(z, fractal, extendedAux);
					break;
				}
				case mandelbox:
				{
					MandelboxIteration(z, fractal, extendedAux);
					break;
				}
				case mandelboxSmooth:
				{
					MandelboxSmoothIteration(z, fractal, extendedAux);
					break;
				}
				case boxFoldBulbPow2:
				{
					BoxFoldBulbPow2Iteration(z, fractal, extendedAux);
					break;
				}
				case mengerSponge:
				{
					MengerSpongeIteration(z, fractal, extendedAux);
					break;
				}
				case kaleidoscopicIFS:
				{
					KaleidoscopicIFSIteration(z, fractal, extendedAux);
					break;
				}
				case aexion:
				{
					AexionIteration(z, fractal, extendedAux);
					break;
				}
				case hypercomplex:
				{
					HypercomplexIteration(z, fractal, extendedAux);
					break;
				}
				case quaternion:
				{
					QuaternionIteration(z, fractal, extendedAux);
					break;
				}
				case benesi:
				{
					BenesiIteration(z, fractal, extendedAux);
					break;
				}
				case bristorbrot:
				{
					BristorbrotIteration(z, fractal, extendedAux);
					break;
				}
				case ides:
				{
					IdesIteration(z, fractal, extendedAux);
					break;
				}
				case ides2:
				{
					Ides2Iteration(z, fractal, extendedAux);
					break;
				}
				case buffalo:
				{
					BuffaloIteration(z, fractal, extendedAux);
					break;
				}
				case quickDudley:
				{
					QuickDudleyIteration(z, fractal, extendedAux);
					break;
				}
				case quickDudleyMod:
				{
					QuickDudleyModIteration(z, fractal, extendedAux);
					break;
				}
				case lkmitch:
				{
					LkmitchIteration(z, fractal, extendedAux);
					break;
				}
				case makin3d2:
				{
					Makin3d2Iteration(z, fractal, extendedAux);
					break;
				}
				case msltoeDonut:
				{
					MsltoeDonutIteration(z, fractal, extendedAux);
					break;
				}
				case msltoeSym2Mod:
				{
					MsltoeSym2ModIteration(z, fractal, extendedAux);
					break;
				}
				case msltoeSym3Mod:
				{
					MsltoeSym3ModIteration(z, fractal, extendedAux);
					break;
				}
				case msltoeSym3Mod2:
				{
					MsltoeSym3Mod2Iteration(z, fractal, extendedAux);
					break;
				}
				case msltoeSym3Mod3:
				{
					MsltoeSym3Mod3Iteration(z, fractal, extendedAux);
					break;
				}
				case msltoeSym4Mod:
				{
					MsltoeSym4ModIteration(z, fractal, extendedAux);
					break;
				}
				case msltoeToroidal:
				{
					MsltoeToroidalIteration(z, fractal, extendedAux);
					break;
				}
				case msltoeToroidalMulti:
				{
					MsltoeToroidalMultiIteration(z, fractal, extendedAux);
					break;
				}
				case generalizedFoldBox:
				{
					GeneralizedFoldBoxIteration(z, fractal, extendedAux);
					break;
				}
				case aboxMod1:
				{
					AboxMod1Iteration(z, fractal, extendedAux);
					break;
				}
				case aboxMod2:
				{
					AboxMod2Iteration(z, fractal, extendedAux);
					break;
				}
				case aboxMod11:
				{
					AboxMod11Iteration(z, fractal, extendedAux);
					break;
				}
				case aboxModKali:
				{
					AboxModKaliIteration(z, fractal, extendedAux);
					break;
				}
				case aboxModKaliEiffie:
				{
					AboxModKaliEiffieIteration(z, fractal, extendedAux);
					break;
				}
				case aboxVSIcen1:
				{
					AboxVSIcen1Iteration(z, fractal, extendedAux);
					break;
				}
				case aexionOctopusMod:
				{
					AexionOctopusModIteration(z, fractal, extendedAux);
					break;
				}
				case amazingSurf:
				{
					AmazingSurfIteration(z, fractal, extendedAux);
					break;
				}
				case amazingSurfMod1:
				{
					AmazingSurfMod1Iteration(z, fractal, extendedAux);
					break;
				}
				case amazingSurfMulti:
				{
					AmazingSurfMultiIteration(z, fractal, extendedAux);
					break;
				}
				case benesiPineTree:
				{
					BenesiPineTreeIteration(z, fractal, extendedAux);
					break;
				}
				case benesiT1PineTree:
				{
					BenesiT1PineTreeIteration(z, fractal, extendedAux);
					break;
				}
				case benesiMagTransforms:
				{
					BenesiMagTransformsIteration(z, fractal, extendedAux);
					break;
				}
				case benesiPwr2Mandelbulb:
				{
					BenesiPwr2MandelbulbIteration(z, fractal, extendedAux);
					break;
				}
				case collatz:
				{
					CollatzIteration(z, fractal, extendedAux);
					break;
				}
				case collatzMod:
				{
					CollatzModIteration(z, fractal, extendedAux);
					break;
				}

				case eiffieMsltoe:
				{
					EiffieMsltoeIteration(z, fractal, extendedAux);
					break;
				}
				case foldBoxMod1:
				{
					FoldBoxMod1Iteration(z, fractal, extendedAux);
					break;
				}
				case iqBulb:
				{
					IqBulbIteration(z, fractal, extendedAux);
					break;
				}
				case kalisets1:
				{
					Kalisets1Iteration(z, fractal, extendedAux);
					break;
				}
				case mandelboxMenger:
				{
					MandelboxMengerIteration(z, fractal, extendedAux);
					break;
				}
				case mandelbulbBermarte:
				{
					MandelbulbBermarteIteration(z, fractal, extendedAux);
					break;
				}
				case mandelbulbKali:
				{
					MandelbulbKaliIteration(z, fractal, extendedAux);
					break;
				}
				case mandelbulbKaliMulti:
				{
					MandelbulbKaliMultiIteration(z, fractal, extendedAux);
					break;
				}
				case mandelbulbMulti:
				{
					MandelbulbMultiIteration(z, fractal, extendedAux);
					break;
				}
				case mandelbulbMulti2:
				{
					MandelbulbMulti2Iteration(z, fractal, extendedAux);
					break;
				}
				case mandelbulbVaryPowerV1:
				{
					MandelbulbVaryPowerV1Iteration(z, fractal, extendedAux);
					break;
				}
				case mengerCrossKIFS:
				{
					MengerCrossKIFSIteration(z, fractal, extendedAux);
					break;
				}
				case mengerCrossMod1:
				{
					MengerCrossMod1Iteration(z, fractal, extendedAux);
					break;
				}
				case mengerMod1:
				{
					MengerMod1Iteration(z, fractal, extendedAux);
					break;
				}
				case mengerMiddleMod:
				{
					MengerMiddleModIteration(z, fractal, extendedAux);
					break;
				}
				case mengerOcto:
				{
					MengerOctoIteration(z, fractal, extendedAux);
					break;
				}
				case mengerPrismShape:
				{
					MengerPrismShapeIteration(z, fractal, extendedAux);
					break;
				}
				case mengerPrismShape2:
				{
					MengerPrismShape2Iteration(z, fractal, extendedAux);
					break;
				}
				case mengerPwr2Poly:
				{
					MengerPwr2PolyIteration(z, fractal, extendedAux);
					break;
				}
				case mengerSmooth:
				{
					MengerSmoothIteration(z, fractal, extendedAux);
					break;
				}
				case mengerSmoothMod1:
				{
					MengerSmoothMod1Iteration(z, fractal, extendedAux);
					break;
				}
				case pseudoKleinianStdDE:
				{
					PseudoKleinianStdDEIteration(z, fractal, extendedAux);
					break;
				}
				case pseudoKleinian:
				{
					PseudoKleinianIteration(z, fractal, extendedAux);
					break;
				}
				case pseudoKleinianMod1:
				{
					PseudoKleinianMod1Iteration(z, fractal, extendedAux);
					break;
				}
				case pseudoKleinianMod2:
				{
					PseudoKleinianMod2Iteration(z, fractal, extendedAux);
					break;
				}
				case quaternion3d:
				{
					Quaternion3dIteration(z, fractal, extendedAux);
					break;
				}
				case riemannSphereMsltoe:
				{
					RiemannSphereMsltoeIteration(z, fractal, extendedAux);
					break;
				}
				case riemannSphereMsltoeV1:
				{
					RiemannSphereMsltoeV1Iteration(z, fractal, extendedAux);
					break;
				}
				case riemannBulbMsltoeMod2:
				{
					RiemannBulbMsltoeMod2Iteration(z, fractal, extendedAux);
					break;
				}
				case sierpinski3d:
				{
					Sierpinski3dIteration(z, fractal, extendedAux);
					break;
				}
				case imaginaryScatorPower2:
				{
					ImaginaryScatorPower2Iteration(z, fractal, extendedAux);
					break;
				}

				// transforms  ------------------------------------------------------------------
				case transfAddConstant:
				{
					TransfAddConstantIteration(z, fractal, extendedAux);
					break;
				}
				case transfAddConstantVaryV1:
				{
					TransfAddConstantVaryV1Iteration(z, fractal, extendedAux);
					break;
				}
				case transfAddCpixel:
				{
					TransfAddCpixelIteration(z, fractal, extendedAux);
					break;
				}
				case transfAddCpixelAxisSwap:
				{
					TransfAddCpixelAxisSwapIteration(z, fractal, extendedAux);
					break;
				}
				case transfAddCpixelCxCyAxisSwap:
				{
					TransfAddCpixelCxCyAxisSwapIteration(z, fractal, extendedAux);
					break;
				}
				case transfAddCpixelSymmetrical:
				{
					TransfAddCpixelSymmetricalIteration(z, fractal, extendedAux);
					break;
				}
				case transfAddCpixelVaryV1:
				{
					TransfAddCpixelVaryV1Iteration(z, fractal, extendedAux);
					break;
				}
				case transfAddExp2Z:
				{
					TransfAddExp2ZIteration(z, fractal, extendedAux);
					break;
				}

				case transfBenesiT1:
				{
					TransfBenesiT1Iteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiT1Mod:
				{
					TransfBenesiT1ModIteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiT2:
				{
					TransfBenesiT2Iteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiT3:
				{
					TransfBenesiT3Iteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiT4:
				{
					TransfBenesiT4Iteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiT5b:
				{
					TransfBenesiT5bIteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiMagForward:
				{
					TransfBenesiMagForwardIteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiMagBackward:
				{
					TransfBenesiMagBackwardIteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiCubeSphere:
				{
					TransfBenesiCubeSphereIteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiSphereCube:
				{
					TransfBenesiSphereCubeIteration(z, fractal, extendedAux);
					break;
				}
				case transfBoxFold:
				{
					TransfBoxFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfBoxFoldVaryV1:
				{
					TransfBoxFoldVaryV1Iteration(z, fractal, extendedAux);
					break;
				}
				case transfBoxFoldXYZ:
				{
					TransfBoxFoldXYZIteration(z, fractal, extendedAux);
					break;
				}
				case transfBoxOffset:
				{
					TransfBoxOffsetIteration(z, fractal, extendedAux);
					break;
				}
				case transfFabsAddConstant:
				{
					TransfFabsAddConstantIteration(z, fractal, extendedAux);
					break;
				}
				case transfFabsAddTgladFold:
				{
					TransfFabsAddTgladFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfFabsAddConditional:
				{
					TransfFabsAddConditionalIteration(z, fractal, extendedAux);
					break;
				}
				case transfFabsAddMulti:
				{
					TransfFabsAddMultiIteration(z, fractal, extendedAux);
					break;
				}
				case transfFoldingTetra3d:
				{
					TransfFoldingTetra3dIteration(z, fractal, extendedAux);
					break;
				}
				case transfIterationWeight:
				{
					TransfIterationWeightIteration(z, fractal, extendedAux);
					break;
				}
				case transfInvCylindrical:
				{
					TransfInvCylindricalIteration(z, fractal, extendedAux);
					break;
				}
				case transfLinCombineCXYZ:
				{
					TransfLinCombineCXYZIteration(z, fractal, extendedAux);
					break;
				}
				case transfMultipleAngle:
				{
					TransfMultipleAngleIteration(z, fractal, extendedAux);
					break;
				}
				case transfNegFabsAddConstant:
				{
					TransfNegFabsAddConstantIteration(z, fractal, extendedAux);
					break;
				}
				case transfOctoFold:
				{
					TransfOctoFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfPwr2Polynomial:
				{
					TransfPwr2PolynomialIteration(z, fractal, extendedAux);
					break;
				}
				case transfRotation:
				{
					TransfRotationIteration(z, fractal, extendedAux);
					break;
				}
				case transfRotationVaryV1:
				{
					TransfRotationVaryV1Iteration(z, fractal, extendedAux);
					break;
				}
				case transfRotationFolding:
				{
					TransfRotationFoldingIteration(z, fractal, extendedAux);
					break;
				}
				case transfRpow3:
				{
					TransfRpow3Iteration(z, fractal, extendedAux);
					break;
				}
				case transfScale:
				{
					TransfScaleIteration(z, fractal, extendedAux);
					break;
				}
				case transfScaleVaryAbox:
				{
					TransfScaleVaryAboxIteration(z, fractal, extendedAux);
					break;
				}
				case transfScaleVaryVCL:
				{
					TransfScaleVaryVCLIteration(z, fractal, extendedAux);
					break;
				}
				case transfScaleVaryV1:
				{
					TransfScaleVaryV1Iteration(z, fractal, extendedAux);
					break;
				}
				case transfScale3d:
				{
					TransfScale3dIteration(z, fractal, extendedAux);
					break;
				}
				case transfPlatonicSolid:
				{
					TransfPlatonicSolidIteration(z, fractal, extendedAux);
					break;
				}
				case transfRPower:
				{
					TransfRPowerIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalInvC:
				{
					TransfSphericalInvCIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalInv:
				{
					TransfSphericalInvIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalOffset:
				{
					TransfSphericalOffsetIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalOffsetVCL:
				{
					TransfSphericalOffsetVCLIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalFold:
				{
					TransfSphericalFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalFoldAbox:
				{
					TransfSphericalFoldAboxIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalFoldCHS:
				{
					TransfSphericalFoldCHSIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalFoldParab:
				{
					TransfSphericalFoldParabIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalFoldVaryV1:
				{
					TransfSphericalFoldVaryV1Iteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalFoldVaryVCL:
				{
					TransfSphericalFoldVaryVCLIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalPwrFold:
				{
					TransfSphericalPwrFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfSurfBoxFold:
				{
					TransfSurfBoxFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfSurfFoldMulti:
				{
					TransfSurfFoldMultiIteration(z, fractal, extendedAux);
					break;
				}
				case transfParabFold:
				{
					TransfParabFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfZvectorAxisSwap:
				{
					TransfZvectorAxisSwapIteration(z, fractal, extendedAux);
					break;
				}
				case transfRotationFoldingPlane:
				{
					TransfRotationFoldingPlaneIteration(z, fractal, extendedAux);
					break;
				}
				case transfQuaternionFold:
				{
					TransfQuaternionFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfMengerFold:
				{
					TransfMengerFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfReciprocal3:
				{
					TransfReciprocal3Iteration(z, fractal, extendedAux);
					break;
				}

				// 4D  ---------------------------------------------------------------------------
				case abox4d:
				{
					Abox4dIteration(z, fractal, extendedAux);
					break;
				}

				case quaternion4d:
				{
					Quaternion4dIteration(z, fractal, extendedAux);
					break;
				}
				case mandelboxVaryScale4d:
				{
					MandelboxVaryScale4dIteration(z, fractal, extendedAux);
					break;
				}
				case bristorbrot4d:
				{
					Bristorbrot4dIteration(z, fractal, extendedAux);
					break;
				}
				case menger4d:
				{
					Menger4dIteration(z, fractal, extendedAux);
					break;
				}
				case menger4dMod1:
				{
					Menger4dMod1Iteration(z, fractal, extendedAux);
					break;
				}

				case mixPinski4d:
				{
					MixPinski4dIteration(z, fractal, extendedAux);
					break;
				}
				case sierpinski4d:
				{
					Sierpinski4dIteration(z, fractal, extendedAux);
					break;
				}
				case transfAddConstant4d:
				{
					TransfAddConstant4dIteration(z, fractal, extendedAux);
					break;
				}
				case transfBoxFold4d:
				{
					TransfBoxFold4dIteration(z, fractal, extendedAux);
					break;
				}
				case transfFabsAddConstant4d:
				{
					TransfFabsAddConstant4dIteration(z, fractal, extendedAux);
					break;
				}
				case transfFabsAddTgladFold4d:
				{
					TransfFabsAddTgladFold4dIteration(z, fractal, extendedAux);
					break;
				}
				case transfFabsAddConditional4d:
				{
					TransfFabsAddConditional4dIteration(z, fractal, extendedAux);
					break;
				}
				case transfIterationWeight4d:
				{
					TransfIterationWeight4dIteration(z, fractal, extendedAux);
					break;
				}
				case transfReciprocal4d:
				{
					TransfReciprocal4dIteration(z, fractal, extendedAux);
					break;
				}
				case transfRotation4d:
				{
					TransfRotation4dIteration(z, fractal, extendedAux);
					break;
				}
				case transfScale4d:
				{
					TransfScale4dIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalFold4d:
				{
					TransfSphericalFold4dIteration(z, fractal, extendedAux);
					break;
				}
				default:
					double high = fractals.GetBailout(sequence) * 10.0;
					z = CVector4(high, high, high, high);
					break;
			}
		}

		// addition of constant
		if (fractals.IsAddCConstant(sequence))
		{
			switch (formula)
			{
				case aboxMod1:
				case amazingSurf:
					// case amazingSurfMod1:
					{
						if (fractals.IsJuliaEnabled(sequence))
						{
							CVector3 juliaC =
								fractals.GetJuliaConstant(sequence) * fractals.GetConstantMultiplier(sequence);
							z += CVector4(juliaC.y, juliaC.x, juliaC.z, 0.0);
						}
						else
						{
							z +=
								CVector4(extendedAux.const_c.y, extendedAux.const_c.x, extendedAux.const_c.z, 0.0)
								* fractals.GetConstantMultiplier(sequence);
						}
						break;
					}

				default:
				{
					if (fractals.IsJuliaEnabled(sequence))
					{
						z += CVector4(
							fractals.GetJuliaConstant(sequence) * fractals.GetConstantMultiplier(sequence), 0.0);
					}
					else
					{
						z += extendedAux.const_c * fractals.GetConstantMultiplier(sequence);
					}
					break;
				}
			}
		}

		if (fractals.IsHybrid())
		{
			z = SmoothCVector(tempZ, z, fractals.GetWeight(sequence));
		}

		// r calculation
		// r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z + w * w);
		switch (fractal->formula)
		{
			case imaginaryScatorPower2:
			{
				CVector4 z2 = z * z;
				r = sqrt(z2.x + z2.y + z2.z) + (z2.y * z2.z) / (z2.x);
				break;
			}
			// scator magnitudes
			// magnitude in imaginary scator algebra

			case pseudoKleinianStdDE:
				// case pseudoKleinian:
				// case pseudoKleinianMod1:
				// case pseudoKleinianMod2:
				{

					// r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z * extendedAux.pseudoKleinianZZ);
					r = sqrt(z.x * z.x + z.y * z.y);
					break;
				}

			default:
			{
				r = z.Length();
				break;
			}
		}

		if (z.IsNotANumber())
		{
			z = lastZ;
			r = z.Length();
			out->maxiter = true;
			break;
		}

		// escape conditions
		if (fractals.IsCheckForBailout(sequence))
		{
			if (Mode == calcModeNormal)
			{
				if (r > fractals.GetBailout(sequence))
				{
					out->maxiter = false;
					break;
				}

				if (fractals.UseAdditionalBailoutCond(sequence))
				{
					if ((z - lastZ).Length() / r < 0.1 / fractals.GetBailout(sequence))
					{
						out->maxiter = false;
						break;
					}
				}
			}
			else if (Mode == calcModeDeltaDE1)
			{
				if (r > fractals.GetBailout(sequence))
				{
					out->maxiter = false;
					break;
				}

				if (fractals.UseAdditionalBailoutCond(sequence))
				{
					if ((z - lastZ).Length() / r < 0.1 / fractals.GetBailout(sequence))
					{
						out->maxiter = false;
						break;
					}
				}
			}
			else if (Mode == calcModeDeltaDE2)
			{
				if (i == in.maxN) break;
			}
			else if (Mode == calcModeColouring)
			{
				double len = 0.0;
				switch (in.fractalColoring.coloringAlgorithm)
				{
					case sFractalColoring::fractalColoringStandard:
					{
						len = r;
						break;
					}
					case sFractalColoring::fractalColoringZDotPoint:
					{
						len = fabs(z.Dot(CVector4(in.point, 0.0)));
						break;
					}
					case sFractalColoring::fractalColoringSphere:
					{
						len = fabs((z - CVector4(in.point, 0.0)).Length() - in.fractalColoring.sphereRadius);
						break;
					}
					case sFractalColoring::fractalColoringCross:
					{
						len = dMin(fabs(z.x), fabs(z.y), fabs(z.z));
						break;
					}
					case sFractalColoring::fractalColoringLine:
					{
						len = fabs(z.Dot(CVector4(in.fractalColoring.lineDirection, 0.0)));
						break;
					}
					case sFractalColoring::fractalColoringNone:
					{
						len = r;
						break;
					}
				}
				if (fractal->formula != mandelbox
						|| in.fractalColoring.coloringAlgorithm != sFractalColoring::fractalColoringStandard)
				{
					if (len < minimumR) minimumR = len;
				}
				if (r > 1e15 || (z - lastZ).Length() / r < 1e-15) break;
			}
			else if (Mode == calcModeOrbitTrap)
			{
				CVector4 delta = z - CVector4(in.common.fakeLightsOrbitTrap, 0.0);
				double distance = delta.Length();
				if (i >= in.common.fakeLightsMinIter && i <= in.common.fakeLightsMaxIter)
					orbitTrapTotal += (1.0 / (distance * distance));
				if (distance > 1000)
				{
					out->orbitTrapR = orbitTrapTotal;
					break;
				}
			}
		}

		if (z.IsNotANumber()) // detection of dead computation
		{
			// if(Mode != calcModeColouring)
			// qWarning() << "Dead computation\n"
			//		<< "iteration: " << i << "Formula:" << formula << "Sequence:" << sequence
			//		<< "\nPoint:" << in.point.Debug()
			//		<< "\nLast good z:" << lastGoodZ.Debug()
			//		<< "\nPrevious z:" << lastZ.Debug();
			z = lastGoodZ;
			break;
		}
	}

	double r3D = z.GetXYZ().Length();

	// final calculations
	if (Mode == calcModeNormal)
	{
		if (fractals.IsHybrid())
		{
			if (extendedAux.r_dz > 0)
			{
				if (fractals.GetDEFunctionType(0) == fractal::linearDEFunction)
				{
					out->distance = (r3D - in.common.linearDEOffset) / fabs(extendedAux.DE);
				}
				else if (fractals.GetDEFunctionType(0) == fractal::logarithmicDEFunction)
				{
					out->distance = 0.5 * r3D * log(r3D) / extendedAux.r_dz;
				}
				else if (fractals.GetDEFunctionType(0) == fractal::pseudoKleinianDEFunction)
				{
					double rxy = sqrt(z.x * z.x + z.y * z.y);
					out->distance =
						max(rxy - extendedAux.pseudoKleinianDE, fabs(rxy * z.z) / r3D) / (extendedAux.DE);
				}
			}
			else
			{
				out->distance = r;
			}
		}
		else
		{
			switch (formula)
			{
				case benesi:
				case benesiPineTree:
				case benesiT1PineTree:
				case benesiPwr2Mandelbulb:
				case bristorbrot:
				case bristorbrot4d:
				case buffalo:
				case eiffieMsltoe:
				case mandelbulbPower2:
				case hypercomplex:
				case iqBulb:
				case mandelbulb:
				case mandelbulb2:
				case mandelbulb3:
				case mandelbulb4:
				case mandelbulbBermarte:
				case mandelbulbKali:
				case mandelbulbKaliMulti:
				case mandelbulbMulti:
				case mandelbulbMulti2:
				case mandelbulbVaryPowerV1:
				case msltoeSym2Mod:
				case msltoeSym3Mod:
				case msltoeSym3Mod2:
				case msltoeSym3Mod3:
				case msltoeSym4Mod:
				case msltoeToroidal:			// TODO fix??
				case msltoeToroidalMulti: // TODO fix??
				case quaternion:
				case transfQuaternionFold: // hmmm, this issue again
				case quaternion3d:
				case xenodreambuie:
				{
					if (extendedAux.r_dz > 0)
						out->distance = 0.5 * r3D * log(r3D) / extendedAux.r_dz;
					else
						out->distance = r3D;
					break;
				}
				case mandelbox:
				case mandelboxMenger:
				case mandelboxSmooth:
				case mandelboxVaryScale4d:
				case generalizedFoldBox:
				case foldBoxMod1:
				case aboxModKali:
				case aboxModKaliEiffie:
				case aboxMod1:
				case aboxMod2:
				case aboxMod11:
				case amazingSurf:
				case amazingSurfMod1:
				case amazingSurfMulti:
				case kalisets1:
				case aboxVSIcen1:
				case pseudoKleinianStdDE:
				case abox4d:
				{
					if (extendedAux.DE > 0)
						out->distance = r3D / fabs(extendedAux.DE);
					else
						out->distance = r3D;
					break;
				}
				case kaleidoscopicIFS:
				case mengerSponge:
				case mengerCrossKIFS:
				case mengerCrossMod1:
				case mengerPrismShape:
				case mengerPrismShape2:
				case collatz:
				case collatzMod:
				case mengerMod1:
				case mengerMiddleMod:
				case transfMengerFold: // hmmm, this issue again
				case mengerPwr2Poly:
				case mixPinski4d:
				case sierpinski4d:
				case sierpinski3d:
				case menger4d:
				case menger4dMod1:
				case mengerSmooth:
				case mengerSmoothMod1:
				case mengerOcto:
				{
					if (extendedAux.DE > 0)
						out->distance = (r3D - 2.0) / (extendedAux.DE);
					else
						out->distance = r3D;
					break;
				}
				case pseudoKleinian:
				case pseudoKleinianMod1:
				case pseudoKleinianMod2:
				{
					if (extendedAux.DE > 0)
					{
						double rxy = sqrt(z.x * z.x + z.y * z.y);
						out->distance = max(rxy - extendedAux.pseudoKleinianDE, fabs(rxy * z.z) / r3D)
														/ (extendedAux.DE); // 0.92784 extendedAux.pseudoKleinianDE
					}
					else
						out->distance = r3D;
					break;
				}

				default: out->distance = -1.0; break;
			}
		}
	}
	// color calculation
	else if (Mode == calcModeColouring)
	{
		double mboxDE;
		mboxDE = extendedAux.DE;
		double r2 = r / fabs(mboxDE);
		if (r2 > 20) r2 = 20;

		if (fractals.IsHybrid())
		{
			if (minimumR > 100) minimumR = 100;

			double mboxColor;

			mboxColor = extendedAux.color;

			if (mboxColor > 1000) mboxColor = 1000;

			out->colorIndex = minimumR * 1000.0 + mboxColor * 100 + r2 * 5000.0;
			/*out->colorIndex =

					extendedAux.color * 100.0 * extendedAux.foldFactor	 // folds part

					+ r * defaultFractal->mandelbox.color.factorR / 1e13 // abs z part

					+ 1.0 * r2 * 5000.0 // for backwards compatibility

					+ extendedAux.scaleFactor * r * i / 1e15						 // scale part conditional on i & r
					+ ((in.fractalColoring.coloringAlgorithm != sFractalColoring::fractalColoringStandard)
								? minimumR * extendedAux.minRFactor * 1000.0
								: 0.0);*/
		}
		else
		{
			switch (formula)
			{
				case mandelbox:
				case mandelboxSmooth:
				case mandelboxVaryScale4d:
				case generalizedFoldBox:

				case foldBoxMod1:
					out->colorIndex =
						extendedAux.color * 100.0														 // folds part
						+ r * defaultFractal->mandelbox.color.factorR / 1e13 // abs z part
						+ ((in.fractalColoring.coloringAlgorithm != sFractalColoring::fractalColoringStandard)
									? minimumR * 1000.0
									: 0.0);
					break;

				case mengerMod1:
				case aboxModKali:
				case aboxMod1:
				case mengerSponge:
				case collatz:
				case collatzMod:
				case kaleidoscopicIFS:
				case mengerPwr2Poly:
				case mengerMiddleMod: out->colorIndex = minimumR * 1000.0; break;

				case amazingSurf: out->colorIndex = minimumR * 200.0; break;

				case amazingSurfMulti:
				case mandelboxMenger:
				case amazingSurfMod1:
				case aboxModKaliEiffie:
				case abox4d:
				case aboxMod11:
					out->colorIndex =
						extendedAux.color * 100.0 * extendedAux.foldFactor	 // folds part
						+ r * defaultFractal->mandelbox.color.factorR / 1e13 // abs z part
						+ extendedAux.scaleFactor * r2 * 5000.0							 // for backwards compatibility
						//+ extendedAux.scaleFactor * r * i / 1e15						 // scale part conditional on i &
						// r
						+ ((in.fractalColoring.coloringAlgorithm != sFractalColoring::fractalColoringStandard)
									? minimumR * extendedAux.minRFactor * 1000.0
									: 0.0);
					break;

				case msltoeDonut: out->colorIndex = extendedAux.color * 2000.0 / i; break;

				default: out->colorIndex = minimumR * 5000.0; break;
			}
		}
	}
	else
	{
		out->distance = 0.0;
	}

	out->iters = i + 1;
	out->z = z.GetXYZ(); // CVector3( z.x, z.y, w);// z;
	// tim = rdtsc() - tim; perf+= tim; perfCount++; outStream << (double)perf/perfCount - 560.0 <<
	// endl;
	//------------- 3249 ns for all calculation  ----------------
}

template void Compute<calcModeNormal>(
	const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeDeltaDE1>(
	const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeDeltaDE2>(
	const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeColouring>(
	const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeOrbitTrap>(
	const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out);
