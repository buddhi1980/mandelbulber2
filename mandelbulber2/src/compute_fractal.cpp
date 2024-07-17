/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include "material.h"
#include "nine_fractals.hpp"
#include "orbit_trap_shape.hpp"

#include "formula/definition/legacy_fractal_transforms.hpp"

using namespace fractal;

template <fractal::enumCalculationMode Mode>
void Compute(const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out)
{
	cAbstractFractal *fractalFormulaFunction;

	// repeat, move and rotate
	CVector3 pointTransformed = in.point - in.common->fractalPosition;
	pointTransformed = in.common->mRotFractalRotation.RotateVector(pointTransformed);
	pointTransformed = pointTransformed.repeatMod(in.common->repeat);

	CVector4 z = CVector4(pointTransformed, 0.0);

	double colorMin = 1000.0;

	if (in.forcedFormulaIndex >= 0)
	{
		z.w = fractals.GetInitialWAxis(in.forcedFormulaIndex);
	}
	else
	{
		z.w = fractals.GetInitialWAxis(0);
	}

	// double r = z.Length();

	double initialWAxisColor = z.w;

	double orbitTrapTotal = 0.0;
	out->orbitTrapR = 0.0;

	enumFractalFormula formula = fractal::none;

	out->maxiter = true;

	int fractalIndex = 0;
	if (in.forcedFormulaIndex >= 0) fractalIndex = in.forcedFormulaIndex;

	const sFractal *defaultFractal = fractals.GetFractal(fractalIndex);

	sExtendedAux aux;

	aux.c = z;			 // variable c
	aux.const_c = z; // constant c
	aux.old_z = z;	 // used in hybrid color2
	aux.pos_neg = 1.0;
	aux.r = z.Length();					// r
	aux.DE = 1.0;								// partially calculated distance (derivative)  in fractal formulas
	aux.DE0 = 0.0;							// used in difs formulas
	aux.dist = 1000.0;					// used in difs formulas
	aux.pseudoKleinianDE = 1.0; // used to calculate DE for pseudo kleinian

	aux.actualScale = fractals.GetFractal(fractalIndex)->mandelbox.scale; // used for vary scale
	aux.actualScaleA = 0.0;																								// used for vary scale
	aux.color = 1.0;			 // used to calculate color from most of formulas
	aux.colorHybrid = 0.0; // used for hybrid color
	aux.temp1000 = 1000.0; // used for hybrid color 2 (initial value 1000)

	// main iteration loop
	int i;
	int sequence = 0;

	CVector4 lastGoodZ;
	CVector4 lastZ;
	CVector4 lastLastZ;

	int fakeLightsMinIter = in.common->fakeLightsMinIter;
	int fakeLightsMaxIter = in.common->fakeLightsMaxIter;

	if (Mode == calcModeOrbitTrap)
	{
		if (in.orbitTrapIndex == 0)
		{
			if (in.common->fakeLightsColor2Enabled) fakeLightsMaxIter = fakeLightsMinIter;
		}
		else if (in.orbitTrapIndex == 1)
		{
			fakeLightsMinIter = in.common->fakeLightsMinIter + 1;
			fakeLightsMaxIter = max(fakeLightsMinIter, in.common->fakeLightsMaxIter);
			if (in.common->fakeLightsColor3Enabled) fakeLightsMaxIter = fakeLightsMinIter;
		}
		else if (in.orbitTrapIndex == 2)
		{
			fakeLightsMinIter = in.common->fakeLightsMinIter + 2;
			fakeLightsMaxIter = max(fakeLightsMinIter, in.common->fakeLightsMaxIter);
		}
	}

	int maxN;

	if (in.forcedFormulaIndex >= 0)
	{
		maxN = fractals.GetFormulaMaxiter(in.forcedFormulaIndex) * in.maxiterMultiplier;
	}
	else
	{
		maxN = fractals.GetFormulaMaxiter(0) * in.maxiterMultiplier;
	}
	if (in.forcedMaxiter >= 0) maxN = in.forcedMaxiter;

	// main iteration loop
	for (i = 0; i < maxN; i++)
	{
		lastGoodZ = lastZ;
		lastLastZ = lastZ;

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
		if (in.common->foldings.boxEnable)
		{
			BoxFolding(z, &in.common->foldings, aux);
			aux.r = z.Length();
		}

		if (in.common->foldings.sphericalEnable)
		{
			SphericalFolding(z, &in.common->foldings, aux);
			aux.r = z.Length();
		}

		const sFractal *fractal = fractals.GetFractal(sequence);
		formula = fractal->formula;

		// temporary vector for weight function
		CVector4 tempZ = z;
		double tempAuxDE = aux.DE;
		double tempAuxColor = aux.color;

		aux.i = i;

		fractalFormulaFunction = fractals.GetFractalFormulaFunction(sequence);

		if (!fractals.IsHybrid() || fractals.GetWeight(sequence) > 0.0)
		{
			// -------------- call for fractal formulas by function pointers ---------------
			if (fractalFormulaFunction && formula != none)
			{
				fractalFormulaFunction->FormulaCode(z, fractal, aux);
			}
			else
			{
				double high = fractals.GetBailout(sequence) * 10.0;
				z = CVector4(high, high, high, high);
				out->distance = 10.0;
				out->iters = 1;
				out->z = z.GetXYZ();
				return;
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
							z += CVector4(aux.const_c.y, aux.const_c.x, aux.const_c.z, 0.0)
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
						z += aux.const_c * fractals.GetConstantMultiplier(sequence);
					}
					break;
				}
			}
		}

		if (fractals.IsHybrid())
		{
			double k = fractals.GetWeight(sequence);
			if (k < 1.0)
			{
				z = SmoothCVector(tempZ, z, k);
				double kn = 1.0 - k;
				aux.DE = aux.DE * k + tempAuxDE * kn;
				aux.color = aux.color * k + tempAuxColor * kn;
			}
		}

		// r calculation
		// r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z + w * w);

		aux.r = z.Length();

		if (z.IsNotANumber())
		{
			z = lastZ;
			aux.r = z.Length();
			out->maxiter = true;
			break;
		}

		// escape conditions
		if (fractals.IsCheckForBailout(sequence))
		{
			if (Mode == calcModeNormal || Mode == calcModeDeltaDE1)
			{
				if (aux.r > fractals.GetBailout(sequence))
				{
					out->maxiter = false;
					break;
				}

				if (fractals.UseAdditionalBailoutCond(sequence))
				{
					out->maxiter = false; // maxiter flag has to be always disabled for pseudo klienian
					if ((z - lastZ).Length() / aux.r < 0.1 / fractals.GetBailout(sequence))
					{
						break;
					}
					if ((z - lastLastZ).Length() / aux.r < 0.1 / fractals.GetBailout(sequence))
					{
						break;
					}
				}
			}
			else if (Mode == calcModeDeltaDE2)
			{
				if (i == maxN) break;
			}
			else if (Mode == calcModeColouring)
			{
				CVector4 colorZ = z;
				if (!in.material->fractalColoring.color4dEnabledFalse) colorZ.w = 0.0;
				double len = 0.0;
				switch (in.material->fractalColoring.coloringAlgorithm)
				{
					case fractalColoring_Standard:
					{
						len = colorZ.Length();
						break;
					}
					case fractalColoring_ZDotPoint:
					{
						len = fabs(colorZ.Dot(CVector4(pointTransformed, initialWAxisColor)));
						break;
					}
					case fractalColoring_Sphere:
					{
						len = fabs((colorZ - CVector4(pointTransformed, initialWAxisColor)).Length()
											 - in.material->fractalColoring.sphereRadius);
						break;
					}
					case fractalColoring_Cross:
					{

						len = dMin(fabs(colorZ.x), fabs(colorZ.y), fabs(colorZ.z));
						if (in.material->fractalColoring.color4dEnabledFalse) len = min(len, fabs(colorZ.w));
						break;
					}
					case fractalColoring_Line:
					{

						len = fabs(colorZ.Dot(in.material->fractalColoring.lineDirection));
						break;
					}
					case fractalColoring_None:
					{
						len = aux.r;
						break;
					}
				}
				if (!in.material->fractalColoring.colorPreV215False)
				{ // updated code in V2.15
					if (fractal->formula != mandelbox)
					{
						if (len < colorMin) colorMin = len;
						if (aux.r > fractals.GetBailout(sequence)) break;

						if (fractals.UseAdditionalBailoutCond(sequence) && (z - lastZ).Length() / aux.r < 1e-15)
							break;
					}
					else // for Mandelbox. Note in Normal Mode (abox_color) colorMin = 0, else has a value
					{
						if (in.material->fractalColoring.coloringAlgorithm == fractalColoring_Standard)
						{
							if (aux.r > 1e15 || (z - lastZ).Length() / aux.r < 1e-15) break;
						}
						else
						{
							if (len < colorMin) colorMin = len;
							if (aux.r > fractals.GetBailout(sequence) || (z - lastZ).Length() / aux.r < 1e-15)
								break;
						}
					}
				}
				else // pre-v2.15 mode
				{
					if (fractal->formula != mandelbox
							|| in.material->fractalColoring.coloringAlgorithm != fractalColoring_Standard)
					{
						if (len < colorMin) colorMin = len;
						if (aux.r > 1e15 || (z - lastZ).Length() / aux.r < 1e-15)
							break; // old, is updated v2.15
					}
					else // for mandbox and fractalColoring_Standard
					{
						if (aux.r > 1e15 || (z - lastZ).Length() / aux.r < 1e-15) break;
					}
				}
			}

			else if (Mode == calcModeOrbitTrap)
			{
				double distance = (in.common->fakeLightsRelativeCenter)
														? OrbitTrapShapeDistance(z - aux.const_c, in.common)
														: OrbitTrapShapeDistance(z, in.common);

				if (i >= fakeLightsMinIter && i <= fakeLightsMaxIter)
					orbitTrapTotal += (1.0 / (distance * distance));
				if (distance > fractals.GetBailout(sequence))
				{
					out->orbitTrapR = orbitTrapTotal;
					break;
				}
			}
			else if (Mode == calcModeCubeOrbitTrap)
			{
				if (i >= in.material->textureFractalizeStartIteration)
				{
					double size = in.material->textureFractalizeCubeSize;
					CVector3 zz = z.GetXYZ() - pointTransformed;
					if (zz.x > -size && zz.x < size && zz.y > -size && zz.y < size && zz.z > -size
							&& zz.z < size)
					{
						out->colorIndex = (fabs(z.x - size) + fabs(z.y - size) + fabs(z.z - size)) * 100.0;
						out->iters = i + 1;
						out->z = z.GetXYZ();
						return;
					}
				}
				if (aux.r > in.material->textureFractalizeCubeSize * 100.0)
				{
					out->colorIndex = 0.0;
					out->iters = i + 1;
					out->z = z.GetXYZ() / z.Length();
					return;
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
	if (Mode == calcModeNormal) // analytic
	{
		if (aux.DE > 0.0)
		{
			if (fractals.IsHybrid())
			{
				if (fractals.GetDEFunctionType(0) == fractal::linearDEFunction)
				{
					out->distance = (aux.r - in.common->linearDEOffset) / aux.DE;
				}
				else if (fractals.GetDEFunctionType(0) == fractal::logarithmicDEFunction)
				{
					// out->distance = 0.5 * r * log(r) / aux.DE;
					if (aux.r > 1.0)
						out->distance = 0.5 * aux.r * log(aux.r) / aux.DE;
					else
						out->distance = 0.0;
				}
				else if (fractals.GetDEFunctionType(0) == fractal::pseudoKleinianDEFunction)
				{
					double rxy = sqrt(z.x * z.x + z.y * z.y);

					out->distance = max(rxy - aux.pseudoKleinianDE, fabs(rxy * z.z) / aux.r) / aux.DE;
				}
				else if (fractals.GetDEFunctionType(0) == fractal::josKleinianDEFunction)
				{
					if (fractals.GetFractal(0)->transformCommon.spheresEnabled)
						z.y = min(z.y, fractals.GetFractal(0)->transformCommon.foldingValue - z.y);

					out->distance = min(z.y, fractals.GetFractal(0)->analyticDE.tweak005)
													/ max(aux.DE, fractals.GetFractal(0)->analyticDE.offset1);
				}
				else if (fractals.GetDEFunctionType(0) == fractal::customDEFunction)
				{
					out->distance = aux.dist;
				}
				else if (fractals.GetDEFunctionType(0) == fractal::maxAxisDEFunction)
				{
					CVector4 absZ = fabs(z);
					double rd = max(absZ.x, max(absZ.y, absZ.z));
					out->distance = rd / aux.DE;
				}
			}
			else
			{
				switch (fractals.GetDEAnalyticFunction(sequence))
				{
					case analyticFunctionLogarithmic:
					{
						if (aux.r > 1.0)
							out->distance = 0.5 * aux.r * log(aux.r) / aux.DE;
						else
							out->distance = 0.0;
						break;
					}
					case analyticFunctionLinear:
					{
						out->distance = aux.r / aux.DE;
						break;
					}
					case analyticFunctionIFS:
					{
						out->distance = (aux.r - 2.0) / aux.DE;
						break;
					}
					case analyticFunctionPseudoKleinian:
					{
						double rxy = sqrt(z.x * z.x + z.y * z.y); // * z.w * z.w)
						out->distance = max(rxy - aux.pseudoKleinianDE, fabs(rxy * z.z) / aux.r) / aux.DE;
						break;
					}
					case analyticFunctionJosKleinian:
					{
						if (fractals.GetFractal(sequence)->transformCommon.spheresEnabled)
							z.y = min(z.y, fractals.GetFractal(sequence)->transformCommon.foldingValue - z.y);

						out->distance = min(z.y, fractals.GetFractal(sequence)->analyticDE.tweak005)
														/ max(aux.DE, fractals.GetFractal(sequence)->analyticDE.offset1);
						break;
					}
					case analyticFunctionCustomDE:
					{
						out->distance = aux.dist;
						break;
					}
					case analyticFunctionMaxAxis:
					{
						CVector4 absZ = fabs(z);
						double rd = max(absZ.x, max(absZ.y, absZ.z));
						out->distance = rd / aux.DE;
						break;
					}

					case analyticFunctionNone: out->distance = -1.0; break;
					case analyticFunctionUndefined: out->distance = aux.r; break;
				}
			}
		}
		else
			out->distance = aux.r;
	}

	// color calculation
	else if (Mode == calcModeColouring)
	{
		enumColoringFunction coloringFunction = fractals.GetColoringFunction(sequence);
		out->colorIndex = CalculateColorIndex(fractals.IsHybrid(), aux.r, z, colorMin, aux,
			in.material->fractalColoring, coloringFunction, defaultFractal);
	}
	else
	{
		out->distance = 0.0;

		// needed for JosKleinian fractal to calculate spheres in deltaDE mode
		if (fractals.GetDEFunctionType(0) == fractal::josKleinianDEFunction)
		{
			if (fractals.GetFractal(sequence)->transformCommon.spheresEnabled)
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
template void Compute<calcModeCubeOrbitTrap>(
	const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out);
