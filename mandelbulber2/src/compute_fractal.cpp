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

	CVector3 z = point2;
	double r = z.Length();
	CVector3 c = z;
	double minimumR = 100.0;

	double w;
	if (in.forcedFormulaIndex >= 0)
	{
		w = fractals.GetInitialWAxis(in.forcedFormulaIndex);
	}
	else
	{
		w = fractals.GetInitialWAxis(0);
	}

	double orbitTrapTotal = 0.0;

	enumFractalFormula formula = fractal::none;

	if (in.common.iterThreshMode)
		out->maxiter = true;
	else
		out->maxiter = false;

	int fractalIndex = 0;
	if (in.forcedFormulaIndex >= 0) fractalIndex = in.forcedFormulaIndex;

	const cFractal *defaultFractal = fractals.GetFractal(fractalIndex);

	sExtendedAux extendedAux;

	extendedAux.r_dz = 1.0;
	extendedAux.r = r;
	extendedAux.color = 1.0;
	extendedAux.actualScale = fractals.GetFractal(fractalIndex)->mandelbox.scale;
	extendedAux.DE = 1.0;
	extendedAux.c = c;
	extendedAux.cw = 0;
	extendedAux.foldFactor = 0.0;
	extendedAux.minRFactor = 0.0;
	extendedAux.scaleFactor = 0.0;
	// extendedAux.newR = 1e+20;
	// extendedAux.axisBias = 1e+20;
	// extendedAux.orbitTraps = 1e+20;
	// extendedAux.transformSampling = 1e+20;

	// main iteration loop
	int i;
	int sequence;

	CVector3 lastGoodZ;
	CVector3 lastZ;

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

		const cFractal *fractal = fractals.GetFractal(sequence);
		formula = fractal->formula;

		// temporary vector for weight function
		CVector3 tempZ = z;

		extendedAux.r = r;

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
					Mandelbulb2Iteration(z, extendedAux);
					break;
				}
				case mandelbulb3:
				{
					Mandelbulb3Iteration(z, extendedAux);
					break;
				}
				case mandelbulb4:
				{
					Mandelbulb4Iteration(z, fractal, extendedAux);
					break;
				}
				case fast_mandelbulb_power2:
				{
					MandelbulbPower2Iteration(z, extendedAux);
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
				case smoothMandelbox:
				{
					SmoothMandelboxIteration(z, fractal, extendedAux);
					break;
				}
				case boxFoldBulbPow2:
				{
					BoxFoldBulbPow2Iteration(z, fractal);
					break;
				}
				case menger_sponge:
				{
					MengerSpongeIteration(z, extendedAux);
					break;
				}
				case kaleidoscopicIFS:
				{
					KaleidoscopicIFSIteration(z, fractal, extendedAux);
					break;
				}
				case aexion:
				{
					AexionIteration(z, w, i, fractal, extendedAux);
					break;
				}
				case hypercomplex:
				{
					HypercomplexIteration(z, w, extendedAux);
					break;
				}
				case quaternion:
				{
					QuaternionIteration(z, w, extendedAux);
					break;
				}
				case benesi:
				{
					BenesiIteration(z, c, extendedAux);
					break;
				}
				case bristorbrot:
				{
					BristorbrotIteration(z, extendedAux);
					break;
				}
				case ides:
				{
					IdesIteration(z, fractal);
					break;
				}
				case ides2:
				{
					Ides2Iteration(z, fractal);
					break;
				}
				case buffalo:
				{
					BuffaloIteration(z, fractal, extendedAux);
					break;
				}
				case quickdudley:
				{
					QuickDudleyIteration(z);
					break;
				}
				case quickDudleyMod:
				{
					QuickDudleyModIteration(z, fractal);
					break;
				}
				case lkmitch:
				{
					LkmitchIteration(z);
					break;
				}
				case makin3d2:
				{
					Makin3D2Iteration(z);
					break;
				}
				case msltoeDonut:
				{
					MsltoeDonutIteration(z, fractal, extendedAux);
					break;
				}
				case msltoesym2Mod:
				{
					MsltoeSym2ModIteration(z, c, fractal, extendedAux);
					break;
				}
				case msltoesym3Mod:
				{
					MsltoeSym3ModIteration(z, c, i, fractal, extendedAux);
					break;
				}
				case msltoesym3Mod2:
				{
					MsltoeSym3Mod2Iteration(z, c, fractal, extendedAux);
					break;
				}
				case msltoesym3Mod3:
				{
					MsltoeSym3Mod3Iteration(z, c, i, fractal, extendedAux);
					break;
				}
				case msltoesym4Mod:
				{
					MsltoeSym4ModIteration(z, c, fractal, extendedAux);
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
				case aboxModKali:
				{
					AboxModKaliIteration(z, fractal, extendedAux);
					break;
				}
				case aboxModKaliEiffie:
				{
					AboxModKaliEiffieIteration(z, c, i, fractal, extendedAux);
					break;
				}
				case aboxVSIcen1:
				{
					AboxVSIcen1Iteration(z, c, fractal, extendedAux);
					break;
				}
				case aexionOctopusMod:
				{
					AexionOctopusModIteration(z, c, fractal);
					break;
				}
				case amazingSurf:
				{
					AmazingSurfIteration(z, c, fractal, extendedAux);
					break;
				}
				case amazingSurfMod1:
				{
					AmazingSurfMod1Iteration(z, fractal, extendedAux);
					break;
				}
				case amazingSurfMulti:
				{
					AmazingSurfMultiIteration(z, c, i, fractal, extendedAux);
					break;
				}
				case benesiPineTree:
				{
					BenesiPineTreeIteration(z, c, fractal, extendedAux);
					break;
				}
				case benesiT1PineTree:
				{
					BenesiT1PineTreeIteration(z, c, i, fractal, extendedAux);
					break;
				}
				case benesiMagTransforms:
				{
					BenesiMagTransformsIteration(z, c, i, fractal, extendedAux);
					break;
				}
				case benesiPwr2s:
				{
					BenesiPwr2sIteration(z, c, i, fractal, extendedAux);
					break;
				}
				case collatz:
				{
					CollatzIteration(z, extendedAux);
					break;
				}
				case collatzMod:
				{
					CollatzModIteration(z, c, fractal, extendedAux);
					break;
				}

				case eiffieMsltoe:
				{
					EiffieMsltoeIteration(z, c, fractal, extendedAux);
					break;
				}
				case foldBoxMod1:
				{
					FoldBoxMod1Iteration(z, i, fractal, extendedAux);
					break;
				}
				case iqBulb:
				{
					IQbulbIteration(z, fractal, extendedAux);
					break;
				}
				case kalisets1:
				{
					Kalisets1Iteration(z, c, fractal, extendedAux);
					break;
				}
				case mandelboxMenger:
				{
					MandelboxMengerIteration(z, c, i, fractal, extendedAux);
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
					MandelbulbKaliMultiIteration(z, c, fractal, extendedAux);
					break;
				}
				case mandelbulbMulti:
				{
					MandelbulbMultiIteration(z, c, fractal, extendedAux);
					break;
				}
				case mandelbulbVaryPowerV1:
				{
					MandelbulbVaryPowerV1Iteration(z, i, fractal, extendedAux);
					break;
				}
				case mengerCrossKIFS:
				{
					MengerCrossKIFSIteration(z, i, fractal, extendedAux);
					break;
				}
				case mengerCrossMod1:
				{
					MengerCrossMod1Iteration(z, i, fractal, extendedAux);
					break;
				}
				case mengerMod1:
				{
					MengerMod1Iteration(z, i, fractal, extendedAux);
					break;
				}
				case mengerMiddleMod:
				{
					MengerMiddleModIteration(z, c, i, fractal, extendedAux);
					break;
				}
				case mengerOcto:
				{
					MengerOctoIteration(z, i, fractal, extendedAux);
					break;
				}
				case mengerPrismShape:
				{
					MengerPrismShapeIteration(z, i, fractal, extendedAux);
					break;
				}
				case mengerPrismShape2:
				{
					z = CVector3{-z.z, z.x, z.y};
					MengerPrismShape2Iteration(z, i, fractal, extendedAux);
					break;
				}
				case mengerPwr2Poly:
				{
					MengerPwr2PolyIteration(z, c, i, fractal, extendedAux);
					break;
				}
				case mengerSmooth:
				{
					MengerSmoothIteration(z, i, fractal, extendedAux);
					break;
				}
				case mengerSmoothMod1:
				{
					MengerSmoothMod1Iteration(z, i, fractal, extendedAux);
					break;
				}
				case pseudoKleinian1:
				{
					PseudoKleinian1Iteration(z, i, fractal, extendedAux);
					break;
				}
				case pseudoKleinian2:
				{
					PseudoKleinian2Iteration(z, i, fractal, extendedAux);
					break;
				}
				case pseudoKleinian3:
				{
					PseudoKleinian3Iteration(z, i, fractal, extendedAux);
					break;
				}
				case quaternion3D:
				{
					Quaternion3DIteration(z, fractal, extendedAux);
					break;
				}
				case riemannSphereMsltoe:
				{
					RiemannSphereMsltoeIteration(z, fractal);
					break;
				}
				case riemannSphereMsltoeV1:
				{
					RiemannSphereMsltoeV1Iteration(z, fractal);
					break;
				}
				case riemannBulbMsltoeMod2:
				{
					RiemannBulbMsltoeMod2Iteration(z, fractal);
					break;
				}
				case sierpinski3D:
				{
					Sierpinski3DIteration(z, i, fractal, extendedAux);
					break;
				}
				case fastImagscaPower2:
				{
					FastImagscaPower2Iteration(z);
					break;
				}

					// transforms  ------------------------------------------------------------------
					//				case transfAdditionConstant:
					{
						TransformAdditionConstantIteration(z, fractal);
						break;
					}
				case transfAdditionConstantVaryV1:
				{
					TransformAdditionConstantVaryV1Iteration(z, i, fractal);
					break;
				}
				case transfAddCpixel:
				{
					TransformAddCpixelIteration(z, c, fractal);
					break;
				}
				case transfAddCpixelAxisSwap:
				{
					TransformAddCpixelAxisSwapIteration(z, c, fractal, extendedAux);
					break;
				}
				case transfAddCpixelCxCyAxisSwap:
				{
					TransformAddCpixelCxCyAxisSwapIteration(z, c, fractal, extendedAux);
					break;
				}
				case transfAddCpixelPosNeg:
				{
					TransformAddCpixelPosNegIteration(z, c, fractal);
					break;
				}
				case transfAddCpixelVaryV1:
				{
					TransformAddCpixelVaryV1Iteration(z, c, i, fractal);
					break;
				}
				case transfAddExp2Z:
				{
					TransformAddExp2ZIteration(z, fractal, extendedAux);
					break;
				}

				case transfBenesiT1:
				{
					TransformBenesiT1Iteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiT1Mod:
				{
					TransformBenesiT1ModIteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiT2:
				{
					TransformBenesiT2Iteration(z, fractal, extendedAux);
					break;
				}
				case transfBenesiT3:
				{
					TransformBenesiT3Iteration(z, fractal);
					break;
				}
				case transfBenesiT4:
				{
					TransformBenesiT4Iteration(z, fractal);
					break;
				}
				case transfBenesiT5b:
				{
					TransformBenesiT5bIteration(z, fractal);
					break;
				}
				case transfBenesiMagForward:
				{
					TransformBenesiMagForwardIteration(z);
					break;
				}
				case transfBenesiMagBackward:
				{
					TransformBenesiMagBackwardIteration(z);
					break;
				}
				case transfBenesiCubeSphere:
				{
					TransformBenesiCubeSphereIteration(z);
					break;
				}
				case transfBenesiSphereCube:
				{
					TransformBenesiSphereCubeIteration(z);
					break;
				}
				case transfBoxFold:
				{
					TransformBoxFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfBoxFoldVaryV1:
				{
					TransformBoxFoldVaryV1Iteration(z, i, fractal, extendedAux);
					break;
				}
				case transfBoxFoldXYZ:
				{
					TransformBoxFoldXYZIteration(z, fractal, extendedAux);
					break;
				}
				case transfBoxOffset:
				{
					TransformBoxOffsetIteration(z, fractal, extendedAux);
					break;
				}
				case transfFabsAddConstant:
				{
					TransformFabsAddConstantIteration(z, fractal);
					break;
				}
				case transfFabsAddConstantV2:
				{
					TransformFabsAddConstantV2Iteration(z, fractal);
					break;
				}
				case transfFabsAddConditional:
				{
					TransformFabsAddConditionalIteration(z, fractal, extendedAux);
					break;
				}
				case transfFabsAddMulti:
				{
					TransformFabsAddMultiIteration(z, fractal);
					break;
				}
				case transfFoldingTetra3D:
				{
					TransformFoldingTetra3DIteration(z, fractal);
					break;
				}
				case transfIterationWeight:
				{
					TransformIterationWeightIteration(z, i, fractal, extendedAux);
					break;
				}
				case transfInvCylindrical:
				{
					TransformInvCylindricalIteration(z, fractal, extendedAux);
					break;
				}
				case transfLinCombineCxyz:
				{
					TransformLinCombineCxyz(z, c, fractal, extendedAux);
					break;
				}
				case transfMultipleAngle:
				{
					TransformMultipleAngle(z, fractal, extendedAux);
					break;
				}
				case transfNegFabsAddConstant:
				{
					TransformNegFabsAddConstantIteration(z, fractal);
					break;
				}
				case transfOctoFold:
				{
					TransformOctoFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfPwr2Polynomial:
				{
					TransformPwr2PolynomialIteration(z, fractal, extendedAux);
					break;
				}
				case transfRotation:
				{
					TransformRotationIteration(z, fractal);
					break;
				}
				case transfRotationVaryV1:
				{
					TransformRotationVaryV1Iteration(z, i, fractal);
					break;
				}
				case transfRotatedFolding:
				{
					TransformRotatedFoldingIteration(z, fractal);
					break;
				}
				case transfRpow3:
				{
					TransformRpow3Iteration(z, fractal, extendedAux);
					break;
				}
				case transfScale:
				{
					TransformScaleIteration(z, fractal, extendedAux);
					break;
				}
				case transfScaleVaryVCL:
				{
					TransformScaleVaryVCLIteration(z, i, fractal, extendedAux);
					break;
				}
				case transfScaleVaryV1:
				{
					TransformScaleVaryV1Iteration(z, i, fractal, extendedAux);
					break;
				}
				case transfScale3D:
				{
					TransformScale3DIteration(z, fractal, extendedAux);
					break;
				}
				case platonicSolid:
				{
					TransformPlatonicSolidIteration(z, fractal);
					break;
				}
				case transfRPower:
				{
					TransformPowerR(z, fractal, extendedAux);
					break;
				}
				case transfSphereInvC:
				{
					TransformSphereInvCIteration(z, c, fractal);
					break;
				}
				case transfSphereInv:
				{
					TransformSphereInvIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalOffset:
				{
					TransformSphericalOffsetIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalOffsetVCL:
				{
					TransformSphericalOffsetVCLIteration(z, i, fractal, extendedAux);
					break;
				}
				case transfSphericalFold:
				{
					TransformSphericalFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalFoldAbox:
				{
					TransformSphericalFoldAboxIteration(z, fractal, extendedAux);
					break;
				}
				case transfSphericalFoldVaryV1:
				{
					TransformSphericalFoldVaryV1Iteration(z, i, fractal, extendedAux);
					break;
				}
				case transfSpherFoldVaryVCL:
				{
					TransformSpherFoldVaryVCLIteration(z, i, fractal, extendedAux);
					break;
				}
				case transfSphericalPwrFold:
				{
					TransformSphericalPwrFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfSurfBoxFold:
				{
					TransformSurfBoxFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfSurfFoldMulti:
				{
					TransformSurfFoldMultiIteration(z, fractal, extendedAux);
					break;
				}
				case transfZvectorAxisSwap:
				{
					TransformZvectorAxisSwapIteration(z, i, fractal);
					break;
				}
				case transfRotationFoldingPlane:
				{
					TransformRotationFoldingPlane(z, fractal, extendedAux);
					break;
				}
				case transfQuaternionFold:
				{
					TransformQuaternionFoldIteration(z, c, fractal, extendedAux);
					break;
				}
				case transfMengerFold:
				{
					TransformMengerFoldIteration(z, fractal, extendedAux);
					break;
				}
				case transfReciprocal3:
				{
					TransformReciprocal3Iteration(z, fractal, extendedAux);
					break;
				}

				// 4D  ---------------------------------------------------------------------------
				case quaternion4D:
				{
					CVector4 z4D(z, w);
					Quaternion4DIteration(z4D, fractal);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case mandelboxVaryScale4D:
				{
					CVector4 z4D(z, w);
					MandelboxVaryScale4DIteration(z4D, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case bristorbrot4D:
				{
					CVector4 z4D(z, w);
					Bristorbrot4DIteration(z4D, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case menger4D:
				{
					CVector4 z4D(z, w);
					Menger4DIteration(z4D, i, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case menger4Dmod1:
				{
					CVector4 z4D(z, w);
					Menger4Dmod1Iteration(z4D, i, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}

				case mixPinski4D:
				{
					CVector4 z4D(z, w);
					MixPinski4DIteration(z4D, i, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case sierpinski4D:
				{
					CVector4 z4D(z, w);
					Sierpinski4DIteration(z4D, i, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case transfAdditionConstant4D:
				{
					CVector4 z4D(z, w);
					TransformAdditionConstant4DIteration(z4D, fractal);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case transfBoxFold4D:
				{
					CVector4 z4D(z, w);
					TransformBoxFold4DIteration(z4D, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case transfFabsAddConstant4D:
				{
					CVector4 z4D(z, w);
					TransformFabsAddConstant4DIteration(z4D, fractal);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case transfFabsAddConstantV24D:
				{
					CVector4 z4D(z, w);
					TransformFabsAddConstantV24DIteration(z4D, fractal);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case transfFabsAddConditional4D:
				{
					CVector4 z4D(z, w);
					TransformFabsAddConditional4DIteration(z4D, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case transfIterationWeight4D:
				{
					CVector4 z4D(z, w);
					TransformIterationWeight4DIteration(z4D, i, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case transfReciprocal4D:
				{
					CVector4 z4D(z, w);
					TransformReciprocal4DIteration(z4D, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case transfScale4D:
				{
					CVector4 z4D(z, w);
					TransformScale4DIteration(z4D, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case transfSphericalFold4D:
				{
					CVector4 z4D(z, w);
					TransformSphericalFold4DIteration(z4D, fractal, extendedAux);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}

				default:
					double high = fractals.GetBailout(sequence) * 10.0;
					z = CVector3(high, high, high);
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
							z += CVector3(juliaC.y, juliaC.x, juliaC.z);
						}
						else
						{
							z += CVector3(c.y, c.x, c.z) * fractals.GetConstantMultiplier(sequence);
						}
						break;
					}

				default:
				{
					if (fractals.IsJuliaEnabled(sequence))
					{
						z += fractals.GetJuliaConstant(sequence) * fractals.GetConstantMultiplier(sequence);
					}
					else
					{
						z += c * fractals.GetConstantMultiplier(sequence);
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
			case fastImagscaPower2:
			{
				CVector3 z2 = z * z;
				r = sqrt(z2.x + z2.y + z2.z) + (z2.y * z2.z) / (z2.x);
				break;
			}
			// scator magnitudes
			// magnitude in imaginary scator algebra

			case pseudoKleinian1:
			{
				r = sqrt(z.x * z.x + z.y * z.y);
				break;
			}

			default:
			{
				r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z + w * w);
				break;
			}
		}

		if (z.IsNotANumber())
		{
			z = lastZ;
			r = z.Length();
			w = 0.0;
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
						len = fabs(z.Dot(in.point));
						break;
					}
					case sFractalColoring::fractalColoringSphere:
					{
						len = fabs((z - in.point).Length() - in.fractalColoring.sphereRadius);
						break;
					}
					case sFractalColoring::fractalColoringCross:
					{
						len = dMin(fabs(z.x), fabs(z.y), fabs(z.z));
						break;
					}
					case sFractalColoring::fractalColoringLine:
					{
						len = fabs(z.Dot(in.fractalColoring.lineDirection));
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
				CVector3 delta = z - in.common.fakeLightsOrbitTrap;
				double distance = delta.Length();
				if (i >= in.common.fakeLightsMinIter && i <= in.common.fakeLightsMaxIter)
					orbitTrapTotal += (1.0f / (distance * distance));
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

	// final calculations
	if (Mode == calcModeNormal)
	{
		if (fractals.IsHybrid())
		{
			if (extendedAux.r_dz > 0)
			{
				if (fractals.GetDEFunctionType(0) == fractal::linearDEFunction)
				{
					out->distance = r / fabs(extendedAux.DE);
				}
				else if (fractals.GetDEFunctionType(0) == fractal::logarithmicDEFunction)
				{
					out->distance = 0.5 * r * log(r) / extendedAux.r_dz;
				}
				else if (fractals.GetDEFunctionType(0) == fractal::pseudoKleinianDEFunction)
				{
					double rxy = sqrt(z.x * z.x + z.y * z.y);
					out->distance = max(rxy - 0.92784, fabs(rxy * z.z) / r) / (extendedAux.DE);
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
				case benesiPwr2s:
				case bristorbrot:
				case bristorbrot4D:
				case buffalo:
				case eiffieMsltoe:
				case fast_mandelbulb_power2:
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
				case mandelbulbVaryPowerV1:
				case msltoesym2Mod:
				case msltoesym3Mod:
				case msltoesym3Mod2:
				case msltoesym3Mod3:
				case msltoesym4Mod:
				case msltoeToroidal:			// TODO fix??
				case msltoeToroidalMulti: // TODO fix??
				case quaternion:
				case transfQuaternionFold: // hmmm, this issue again
				case quaternion3D:
				case xenodreambuie:
				{
					if (extendedAux.r_dz > 0)
						out->distance = 0.5 * r * log(r) / extendedAux.r_dz;
					else
						out->distance = r;
					break;
				}
				case mandelbox:
				case mandelboxMenger:
				case smoothMandelbox:
				case mandelboxVaryScale4D:
				case generalizedFoldBox:
				case foldBoxMod1:
				case aboxModKali:
				case aboxModKaliEiffie:
				case aboxMod1:
				case aboxMod2:
				case amazingSurf:
				case amazingSurfMod1:
				case amazingSurfMulti:
				case kalisets1:
				case aboxVSIcen1:
				case pseudoKleinian1:
				case mixPinski4D:
					// case mengerSmooth:
					// case mengerSmoothMod1:
					{
						if (extendedAux.DE > 0)
							out->distance = r / fabs(extendedAux.DE);
						else
							out->distance = r;
						break;
					}
				case kaleidoscopicIFS:
				case menger_sponge:
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
				// case mixPinski4D:
				case sierpinski4D:
				case sierpinski3D:
				case menger4D:
				case menger4Dmod1:
				case mengerSmooth:
				case mengerSmoothMod1:
				case mengerOcto:
				{
					if (extendedAux.DE > 0)
						out->distance = (r - 2.0) / (extendedAux.DE);
					else
						out->distance = r;
					break;
				}
				case pseudoKleinian2:
				case pseudoKleinian3:
				{
					if (extendedAux.DE > 0)
					{
						double rxy = sqrt(z.x * z.x + z.y * z.y);
						out->distance = max(rxy - 0.92784, fabs(rxy * z.z) / r) / (extendedAux.DE);
					}
					else
						out->distance = r;
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

					+ 1.0 * r2 * 5000.0 // for backwards compatability

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
				case smoothMandelbox:
				case mandelboxVaryScale4D:
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
				case menger_sponge:
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
					out->colorIndex =
						extendedAux.color * 100.0 * extendedAux.foldFactor	 // folds part
						+ r * defaultFractal->mandelbox.color.factorR / 1e13 // abs z part
						+ extendedAux.scaleFactor * r2 * 5000.0							 // for backwards compatability
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
	out->z = z; // CVector3( z.x, z.y, w);// z;
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
