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

using namespace fractal;

template <fractal::enumCalculationMode Mode>
void Compute(const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out)
{
	fractalFormulaFcn fractalFormulaFunction;

	// repeat, move and rotate
	CVector3 point2 = in.point.mod(in.common.repeat) - in.common.fractalPosition;
	point2 = in.common.mRotFractalRotation.RotateVector(point2);

	CVector4 z = CVector4(point2, 0.0);
	double r = z.Length();

	// trial
	double minimumR = in.fractalColoring.initialMiniumuR;

	double len = 0.0; // Temp: declared here for access outside orbit traps code

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

	extendedAux.c = z;
	extendedAux.const_c = z;
	extendedAux.old_z = z;
	extendedAux.sum_z = z;
	extendedAux.pos_neg = 1.0;
	extendedAux.cw = 0;

	extendedAux.r_dz = 1.0;
	extendedAux.r = r;
	extendedAux.DE = 1.0;
	extendedAux.pseudoKleinianDE = 1.0;

	extendedAux.actualScale = fractals.GetFractal(fractalIndex)->mandelbox.scale;
	extendedAux.actualScaleA = 0.0;

	extendedAux.color = 1.0;
	extendedAux.colorHybrid = 0.0;

	//these are temp, and most might be removed later
	// once some decisions have been made
	extendedAux.minRFactor = 0.0; // orbit trap weight
	extendedAux.foldFactor = 0.0; // aux color weight
	extendedAux.radiusFactor = 0.0; // radius weight
	extendedAux.scaleFactor = 0.0; // DE weight
	//extendedAux.oldHybridFactor = 0.0; // old hybid weight
	//extendedAux.temp1Factor = 0.0;

	extendedAux.temp100 = 100.0;
	extendedAux.addDist = 0.0;

	// main iteration loop
	int i;
	int sequence = 0;

	CVector4 lastGoodZ;
	CVector4 lastZ;
	CVector4 lastLastZ;

	// main iteration loop
	for (i = 0; i < in.maxN; i++)
	{
		lastGoodZ = lastZ;
		lastLastZ = lastZ;
		;
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

		fractalFormulaFunction = fractals.GetFractalFormulaFunction(sequence);

		if (!fractals.IsHybrid() || fractals.GetWeight(sequence) > 0.0)
		{
			// -------------- call for fractal formulas by function pointers ---------------
			if (fractalFormulaFunction)
			{
				fractalFormulaFunction(z, fractal, extendedAux);
			}
			else
			{
				double high = fractals.GetBailout(sequence) * 10.0;
				z = CVector4(high, high, high, high);
				break;
			}
			// -----------------------------------------------------------------------------
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
			if (Mode == calcModeNormal || Mode == calcModeDeltaDE1)
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
					if ((z - lastLastZ).Length() / r < 0.1 / fractals.GetBailout(sequence))
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
				//double len = 0.0;
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
					out->distance = (r - in.common.linearDEOffset) / fabs(extendedAux.DE);
				}
				else if (fractals.GetDEFunctionType(0) == fractal::logarithmicDEFunction)
				{
					out->distance = 0.5 * r * log(r) / extendedAux.r_dz;
				}
				else if (fractals.GetDEFunctionType(0) == fractal::pseudoKleinianDEFunction)
				{
					double rxy = sqrt(z.x * z.x + z.y * z.y);
					out->distance =
						max(rxy - extendedAux.pseudoKleinianDE, fabs(rxy * z.z) / r) / (extendedAux.DE);
				}
				else if (fractals.GetDEFunctionType(0) == fractal::josKleinianDEFunction)
				{
					if (fractals.GetFractal(0)->transformCommon.functionEnabled)
						z.y = min(z.y, fractals.GetFractal(0)->transformCommon.foldingValue - z.y);

					out->distance =
						min(z.y, fractals.GetFractal(0)->analyticDE.tweak005)
						/ max(extendedAux.pseudoKleinianDE, fractals.GetFractal(0)->analyticDE.offset1);
				}
			}
			else
			{
				out->distance = r;
			}
		}
		else
		{
			switch (fractals.GetDEAnalyticFunction(sequence))
			{
				case analyticFunctionLogarithmic:
				{
					if (extendedAux.r_dz > 0)
						out->distance = 0.5 * r * log(r) / extendedAux.r_dz;
					else
						out->distance = r;
					break;
				}
				case analyticFunctionLinear:
				{
					if (extendedAux.DE > 0)
						out->distance = r / fabs(extendedAux.DE);
					else
						out->distance = r;
					break;
				}
				case analyticFunctionIFS:
				{
					if (extendedAux.DE > 0)
						out->distance = (r - 2.0) / fabs(extendedAux.DE);
					else
						out->distance = r;
					break;
				}
				case analyticFunctionPseudoKleinian:
				{
					if (extendedAux.DE > 0)
					{
						double rxy = sqrt(z.x * z.x + z.y * z.y);
						out->distance =
							max(rxy - extendedAux.pseudoKleinianDE, fabs(rxy * z.z) / r) / (extendedAux.DE);
					}
					else
						out->distance = r;

					break;
				}
				case analyticFunctionJosKleinian:
				{
					if (fractals.GetFractal(sequence)->transformCommon.functionEnabled)
						z.y = min(z.y, fractals.GetFractal(sequence)->transformCommon.foldingValue - z.y);

					out->distance =
						min(z.y, fractals.GetFractal(sequence)->analyticDE.tweak005)
						/ max(extendedAux.pseudoKleinianDE, fractals.GetFractal(sequence)->analyticDE.offset1);
					break;
				}

				case analyticFunctionNone: out->distance = -1.0; break;
			}
		}
	}

	// color calculation    TODO review limits
	else if (Mode == calcModeColouring)
	{
		double minR1000 = minimumR * 1000.0; // limited at 100,000 hybrid mode
		double minR5000 = minimumR * 5000.0; // DEFAULT
		double auxColorValue100 = extendedAux.color * 100.0; // limited at 100,000,
		double radDE5000 = 0.0;
		double rad1000 = 0.0;

		if (in.fractalColoring.radFalse)
		{
			rad1000 = r;
		if (in.fractalColoring.radSquaredFalse) rad1000 = r * r;
			rad1000 *= 1000.0 * in.fractalColoring.radWeight;
		}

		if (in.fractalColoring.radDivDeFalse)
		{
			radDE5000 = r;
		if (in.fractalColoring.radSquaredFalse)  radDE5000 = r * r;
			radDE5000 *= 5000.0 * in.fractalColoring.radDivDeWeight
				/ fabs(extendedAux.DE); ; // r /DE // was named r2
		}

		double addValue = 0.0;

		// example of a basic input
		double xyzValue = 0.0;
		if (in.fractalColoring.xyzBiasEnabledFalse)
		{
			CVector3 xyzAxis = 0;
			if (in.fractalColoring.cModeEnabledFalse)
			{
				xyzAxis = fabs(CVector3( extendedAux.c.x, extendedAux.c.y, extendedAux.c.z))
				* in.fractalColoring.xyz000;
			}
			else
			{
				xyzAxis = fabs(CVector3( z.x, z.y, z.z)) * in.fractalColoring.xyz000;
			}
			xyzValue = (xyzAxis.x + xyzAxis.y + xyzAxis.z ) * in.fractalColoring.xyzIterScale;
		}

		addValue += xyzValue; // addValue accumulates outputs

		// HYBRID MODE
		if (fractals.IsHybrid())
		{

			//*new hybrid*
			double inputGeneral = 0.0;
			if (in.fractalColoring.extraColorEnabledFalse)
			{
				inputGeneral =
				minR5000 * in.fractalColoring.orbitTrapWeight // orbit trap only
				+ auxColorValue100 * in.fractalColoring.auxColorWeight// aux.color
				+ rad1000
				+ radDE5000
				+ addValue // all extra inputs
				+ extendedAux.colorHybrid;// transf_hybrid_color inputs


				// global palette controls
				inputGeneral += in.fractalColoring.iterAddScale * (extendedAux.i * extendedAux.i);

					//+ r * extendedAux.radiusFactor / 1e13//  radius // this may be replaced
				if (extendedAux.i >= in.fractalColoring.iStartValue)
				{
					int iUse = in.fractalColoring.iStartValue - extendedAux.i;
					if (in.fractalColoring.iSquaredEnabledFalse) iUse *= iUse;

					if (in.fractalColoring.iInvertEnabledFalse)
					{
						if (in.fractalColoring.iSquaredEnabledFalse)
							inputGeneral *= (1.0 + 1.0/(iUse + 1.0)/in.fractalColoring.iterScale);
						else
							inputGeneral *= (1.0 + 1.0/(extendedAux.i + 1.0)/in.fractalColoring.iterScale);
					}
					else
					{
						inputGeneral *= (iUse * in.fractalColoring.iterScale) + 1.0;
					}
				}
				// "pseudo" palette controls
				if (in.fractalColoring.cosEnabledFalse)
				{ // trig palette
					double trig = 128 * -in.fractalColoring.cosAdd
												* (cos(inputGeneral * 2.0 * M_PI / in.fractalColoring.cosPeriod) - 1.0);
					inputGeneral += trig;
				}
				double minCV = in.fractalColoring.minColorValue;
				double maxCV = in.fractalColoring.maxColorValue;
				if (inputGeneral < minCV) inputGeneral = minCV;
				if (inputGeneral > maxCV) inputGeneral = maxCV;

				out->colorIndex = inputGeneral;
			}
			else
			{
				//*old hybrid*
				minR1000 = min( minR1000, 1e5);
				//if (minR1000 > 100000.0) minR1000 = 100000.0; // limit is only in old hybrid mode?
				if (auxColorValue100 > 1e5) auxColorValue100 = 1e5; // limit
				radDE5000 = 5000.0 * r / fabs(extendedAux.DE); // was named r2
				if (radDE5000 > 1e5) radDE5000 = 1e5;
				double oldHybridValue = (minR1000 + auxColorValue100 + radDE5000); // old hybrid code
				out->colorIndex = oldHybridValue;
			}
		}
				// NORMAL MODE
		else
		{
			switch (fractals.GetColoringFunction(sequence))
			{
				case coloringFunctionABox:
					out->colorIndex =
						 auxColorValue100 // aux.color
						+ r * defaultFractal->mandelbox.color.factorR / 1e13 // radius scale
						+ ((in.fractalColoring.coloringAlgorithm != sFractalColoring::fractalColoringStandard)
									? minR1000
									: 0.0);
					break;

				case coloringFunctionIFS: out->colorIndex = minR1000;
					break;

				case coloringFunctionAmazingSurf: out->colorIndex = minR1000 * 0.2;
					break;

				case coloringFunctionABox2:
					radDE5000 = 5000.0 * r / fabs(extendedAux.DE); // was named r2
					out->colorIndex =
						 minR1000 * extendedAux.minRFactor // orbit trap DEFAULT
						+  auxColorValue100 * extendedAux.foldFactor // aux.color
						+ r * extendedAux.radiusFactor  / 1e13 // radius// this may be replaced
						+ radDE5000 * extendedAux.scaleFactor // r/DE  for backwards compatibility
						+ addValue; // all extra inputs

						/*+ ((in.fractalColoring.coloringAlgorithm != sFractalColoring::fractalColoringStandard)
									? minimumR * extendedAux.minRFactor * 1000.0
									: 0.0);*/ // temp removed
					break;

				case coloringFunctionDonut: out->colorIndex = auxColorValue100 * 20.0 / extendedAux.i;
					break;

				case coloringFunctionDefault: out->colorIndex = minR5000;
					break;

				case coloringFunctionGeneral:
					double inputGeneral = 0.0;
//					if (minR5000 > 1e5) minR5000 = 1e5; // limit is only in hybrid mode?
					if (in.fractalColoring.extraColorEnabledFalse)
					{
						inputGeneral =
						minR5000 * in.fractalColoring.orbitTrapWeight // orbit trap only
						+ auxColorValue100 * in.fractalColoring.auxColorWeight// aux.color
						+ rad1000 // radius
						+ radDE5000 // r /DE
						+ addValue; // all extra inputs

						//Iter ADD,  this allows the input to be influenced by iteration number
						if (in.fractalColoring.iterAddScaleFalse && extendedAux.i > in.fractalColoring.iStartValue)
						{
							int iUse = extendedAux.i - in.fractalColoring.iStartValue;
							if (in.fractalColoring.iSquaredEnabledFalse) iUse *= iUse;
							if (in.fractalColoring.iInvertEnabledFalse) iUse = 1.0 / iUse;
							inputGeneral += in.fractalColoring.iterAddScale * iUse;
						}

						//Iter SCALE,
						if (in.fractalColoring.iterScaleFalse && extendedAux.i >= in.fractalColoring.iStartValue)
						{
							int iUse = extendedAux.i - in.fractalColoring.iStartValue;
							if (in.fractalColoring.iSquaredEnabledFalse) iUse *= iUse;

							if (in.fractalColoring.iInvertEnabledFalse)
							{
								if (in.fractalColoring.iSquaredEnabledFalse)
									inputGeneral *= (1.0 + 1.0/(iUse + 1.0)/in.fractalColoring.iterScale);
								else
									inputGeneral *= (1.0 + 1.0/(extendedAux.i + 1.0)/in.fractalColoring.iterScale);
							}
							else
							{
								inputGeneral *= (iUse * in.fractalColoring.iterScale) + 1.0;
							}
						}
						if (in.fractalColoring.cosEnabledFalse)
						{ // trig
							double trig = 128 * -in.fractalColoring.cosAdd
														* (cos(inputGeneral * 2.0 * M_PI / in.fractalColoring.cosPeriod) - 1.0);
							inputGeneral += trig;
						}
						double minCV = in.fractalColoring.minColorValue;
						double maxCV = in.fractalColoring.maxColorValue;
						if (inputGeneral < minCV) inputGeneral = minCV;
						if (inputGeneral > maxCV) inputGeneral = maxCV;

						out->colorIndex = inputGeneral;
					}
					else
					{
						out->colorIndex = minR5000 ;
					}
					break;
			}
		}
	}
	else
	{
		out->distance = 0.0;

		// needed for JosKleinian fractal to calculate spheres in deltaDE mode
		if (fractals.GetDEFunctionType(0) == fractal::josKleinianDEFunction)
		{
			if (fractals.GetFractal(sequence)->transformCommon.functionEnabled)
				z.y = min(z.y, fractals.GetFractal(sequence)->transformCommon.foldingValue - z.y);
		}
	}

	out->iters = i + 1;
	out->z = z.GetXYZ();
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
