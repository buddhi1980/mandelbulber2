/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include "orbit_trap_shape.hpp"

#include "formula/definition/legacy_fractal_transforms.hpp"
#include "hybrid_fractal_sequences.h"

using namespace fractal;

// Core fractal iteration function, instantiated as a template for different calculation modes.
// Each mode specialization serves a distinct purpose in the rendering pipeline:
//
// calcModeNormal:      Main distance estimation pass. Computes z = f(z) iteration with
//                       analytic DE. Used by CalculateDistanceSimple() for the primary DE.
// calcModeDeltaDE1:    First pass of numerical DE — computes orbit radius r = |z| at the
//                       original point. Needed before perturbation passes.
// calcModeDeltaDE2:    Perturbation passes (X, Y, Z axes) — computes |z'| at offset points
//                       to estimate the derivative dr. Each pass reuses the iteration count
//                       from the first pass (forcedMaxiter).
// calcModeColouring:   Coloring pass — iterates to compute colorIndex via orbit traps or
//                       coloring algorithms. Bailout based on color metric, not distance.
// calcModeOrbitTrap:    Fake lights pass — accumulates 1/distance² along the orbit for
//                        procedural lighting. Iteration range controlled by
//                        fakeLightsMinIter/MaxIter.
// calcModeCubeOrbitTrap: Fractalize texture pass — checks if z enters a bounding cube to
//                         extract texture coordinates from the orbit.
//
// The function handles hybrid fractal sequences (multiple formulas blended per-iteration),
// Julia mode, foldings, bailout, and DE function selection.
template <fractal::enumCalculationMode Mode>
void Compute(const cHybridFractalSequences::sSequence *seq, const sFractalIn &in, sFractalOut *out)
{
	cAbstractFractal *fractalFormulaFunction;

	// z is the main variable, initialized to the input point as a 4D vector.
	// The 4th component (z.w) is used for hypercomplex/4D fractals (e.g., Mandelbox).
	CVector4 z = CVector4(in.point, 0.0);

	double colorMin = 1000.0;

	// Initialize the 4th component for 4D fractals (e.g., Mandelbox uses w-axis).
	z.w = seq->initialWAxis;

	// double r = z.Length();

	// Store the initial w-axis value for orbit trap calculations that reference
	// the starting point in 4D space.
	double initialWAxisColor = z.w;

	double orbitTrapTotal = 0.0;
	out->orbitTrapR = 0.0;

	enumFractalFormula formula = fractal::none;

	// maxiter starts true; set to false when the orbit escapes (bailout).
	out->maxiter = true;

	// Get the first fractal formula from the hybrid sequence.
	// defaultFractal provides fallback parameters (e.g., mandelbox.scale for vary scale).
	int fractalIndex = seq->seqence[0];
	const sFractal *defaultFractal = &seq->fractData[fractalIndex].fractalParameters;

	// sExtendedAux carries per-iteration state between fractal formula calls.
	// Each field has a specific purpose used by the formula implementations:
	sExtendedAux aux;

	// aux.c: current value of the constant term c in z²+c / z'+c formulas
	// aux.const_c: original constant (copy of initial z), used for Julia set offset
	// aux.old_z: previous z value, used in hybrid color2 calculations
	aux.c = z;			 // variable c
	aux.const_c = z; // constant c
	aux.old_z = z;	 // used in hybrid color2
	aux.pos_neg = 1.0;
	// aux.r: orbit radius |z|, recomputed each iteration
	// aux.DE: accumulated derivative (distance estimation), initialized to 1.0 (identity)
	// aux.DE0: secondary DE, used in difs (IFS) formulas
	// aux.dist: custom distance, used in difs formulas
	// aux.pseudoKleinianDE: special DE accumulator for pseudo-Kleinian fractals
	aux.r = z.Length();					// r
	aux.DE = 1.0;								// partially calculated distance (derivative)  in fractal formulas
	aux.DE0 = 0.0;							// used in difs formulas
	aux.dist = 1000.0;					// used in difs formulas
	aux.pseudoKleinianDE = 1.0; // used to calculate DE for pseudo kleinian

	// aux.actualScale / actualScaleA: current scale factor for Mandelbox vary-scale mode
	// aux.color: accumulated color value from formulas
	// aux.colorHybrid: hybrid blending color accumulator
	// aux.temp1000: temporary value (initial 1000) used in hybrid color2
	aux.actualScale = defaultFractal->mandelbox.scale; // used for vary scale
	aux.actualScaleA = 0.0;														 // used for vary scale
	aux.color = 1.0;			 // used to calculate color from most of formulas
	aux.colorHybrid = 0.0; // used for hybrid color
	aux.temp1000 = 1000.0; // used for hybrid color 2 (initial value 1000)

	// main iteration loop
	int i;
	int sequence = 0;

	// Track previous z values for convergence-based bailout (detecting when
	// successive iterations change very little, indicating a fixed point).
	CVector4 lastGoodZ;
	CVector4 lastZ;
	CVector4 lastLastZ;

	// Fake lights orbit trap iteration range, adjusted per orbit trap pass index.
	// Each orbit trap pass (0, 1, 2) computes a different fake light color,
	// so the iteration ranges are offset to sample different parts of the orbit.
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

	// Maximum iteration count: base formula iterations × multiplier (5x for normal calc mode).
	// Can be overridden by forcedMaxiter from a previous iteration pass (used in delta-DE
	// passes to ensure the same number of iterations for all perturbation directions).
	int maxN = seq->formulaMaxiter * in.maxiterMultiplier;

	if (in.forcedMaxiter >= 0) maxN = in.forcedMaxiter;

	// main iteration loop
	for (i = 0; i < maxN; i++)
	{
		lastGoodZ = lastZ;
		lastLastZ = lastZ;

		lastZ = z;

		// Get the fractal formula index for this iteration from the hybrid sequence.
		// GetSequence() wraps around if i exceeds the sequence length.
		sequence = seq->GetSequence(i);

		const cHybridFractalSequences::sFractalData &fractData = seq->fractData[sequence];

		// Apply global foldings (box folding and spherical folding) before the fractal formula.
		// These are shared foldings from sCommonParams that apply to all formulas in the sequence.
		// BoxFolding mirrors coordinates when they exceed boxLimit (TGlad-style folding).
		// SphericalFolding mirrors on spherical shells between sphericalInner and sphericalOuter.
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

		const sFractal *fractal = &fractData.fractalParameters;
		formula = fractal->formula;

		// Save state before the formula call for hybrid blending.
		// When formulaWeight < 1.0, the result is smoothly interpolated between
		// the pre-formula state (tempZ) and post-formula state (z).
		CVector4 tempZ = z;
		double tempAuxDE = aux.DE;
		double tempAuxColor = aux.color;

		aux.i = i;

		fractalFormulaFunction = fractData.fractalFormulaObject;

		if (fractData.formulaWeight > 0.0)
		{
			// -------------- call for fractal formulas by function pointers ---------------
			// Each formula in a hybrid sequence is a cAbstractFractal-derived object
			// that implements FormulaCode(z, fractal, aux). The formula modifies z in-place
			// and updates aux fields (DE, color, etc.) as needed.
			if (fractalFormulaFunction && formula != none)
			{
				fractalFormulaFunction->FormulaCode(z, fractal, aux);
			}
			else
			{
				// Fallback: if the formula object is missing, set z to a large value
				// to trigger immediate bailout in the next iteration check.
				double high = fractData.bailout * 10.0;
				z = CVector4(high, high, high, high);
				out->distance = 10.0;
				out->iters = 1;
				out->z = z.GetXYZ();
				return;
			}
			// -----------------------------------------------------------------------------
		}

		// Julia mode: add the Julia constant to z after the formula iteration.
		// In standard mode, the pixel coordinate (stored in aux.const_c) serves as c.
		// In Julia mode, a fixed juliaConstant is used instead.
		// Special handling for aboxMod1/amazingSurf: swap x and y components.
		if (fractData.addCConstant)
		{
			switch (formula)
			{
				case aboxMod1:
				case amazingSurf:
					// case amazingSurfMod1:
					{
						if (seq->juliaEnabled)
						{
							CVector3 juliaC = seq->juliaConstant * seq->constantMultiplier;
							// Swap x and y for these special formulas.
							z += CVector4(juliaC.y, juliaC.x, juliaC.z, 0.0);
						}
						else
						{
							z += CVector4(aux.const_c.y, aux.const_c.x, aux.const_c.z, 0.0)
									 * seq->constantMultiplier;
						}
						break;
					}

				default:
				{
					if (seq->juliaEnabled)
					{
						z += CVector4(seq->juliaConstant * seq->constantMultiplier, 0.0);
					}
					else
					{
						z += aux.const_c * seq->constantMultiplier;
					}
					break;
				}
			}
		}

		// Hybrid blending: when formulaWeight < 1.0, smoothly interpolate between
		// the pre-formula state and post-formula state using the weight k.
		// This creates smooth transitions between formulas in a hybrid sequence.
		// The DE and color values are also blended to maintain consistency.
		if (seq->isHybrid)
		{
			double k = fractData.formulaWeight;
			if (k < 1.0)
			{
				z = SmoothCVector(tempZ, z, k);
				double kn = 1.0 - k;
				aux.DE = aux.DE * k + tempAuxDE * kn;
				aux.color = aux.color * k + tempAuxColor * kn;
			}
		}

		aux.r = z.Length();

		// NaN detection: if the orbit produces NaN (e.g., from sqrt of negative),
		// revert to the last valid z value and mark as maxiter (bailout).
		if (z.IsNotANumber())
		{
			z = lastZ;
			aux.r = z.Length();
			out->maxiter = true;
			break;
		}

		// escape conditions
		if (fractData.checkForBailout)
		{
			if (Mode == calcModeNormal || Mode == calcModeDeltaDE1)
			{
				// Standard bailout: orbit escaped when |z| exceeds the fractal-specific threshold.
				if (aux.r > fractData.bailout)
				{
					out->maxiter = false;
					break;
				}

				// Additional convergence bailout: if successive iterations change very little
				// relative to the orbit radius, the orbit has converged to a fixed point or
				// cycle. This is used for pseudo-Kleinian interior detection.
				if (fractData.useAdditionalBailoutCond)
				{
					out->maxiter = false; // maxiter flag has to be always disabled for pseudo klienian
					if ((z - lastZ).Length() / aux.r < 0.1 / fractData.bailout)
					{
						break;
					}
					if ((z - lastLastZ).Length() / aux.r < 0.1 / fractData.bailout)
					{
						break;
					}
				}
			}
			else if (Mode == calcModeDeltaDE2)
			{
				// DeltaDE2 pass: only run to the end (no early bailout) to get the
				// full perturbed orbit for derivative calculation.
				if (i == maxN) break;
			}
			else if (Mode == calcModeColouring)
			{
				// Coloring mode: compute a color metric and use it for bailout.
				// Different coloring algorithms measure different aspects of the orbit.
				CVector4 colorZ = z;
				if (!in.material->fractalColoring.color4dEnabledFalse) colorZ.w = 0.0;
				double len = 0.0;
				switch (in.material->fractalColoring.coloringAlgorithm)
				{
					case fractalColoring_Standard:
					{
						// Standard: use |z| as the color metric (same as normal bailout).
						len = colorZ.Length();
						break;
					}
					case fractalColoring_ZDotPoint:
					{
						// Z·P: dot product of z with the starting point (initialWAxisColor in w).
						// Creates coloring based on the angle between z and the starting point.
						len = fabs(colorZ.Dot(CVector4(in.point, initialWAxisColor)));
						break;
					}
					case fractalColoring_Sphere:
					{
						// Sphere: distance from |z - P| to a sphere radius.
						// Creates ring-like coloring patterns.
						len = fabs((colorZ - CVector4(in.point, initialWAxisColor)).Length()
											 - in.material->fractalColoring.sphereRadius);
						break;
					}
					case fractalColoring_Cross:
					{
						// Cross: minimum absolute value of any component of z.
						// Creates cross-shaped coloring patterns.
						len = dMin(fabs(colorZ.x), fabs(colorZ.y), fabs(colorZ.z));
						if (in.material->fractalColoring.color4dEnabledFalse) len = min(len, fabs(colorZ.w));
						break;
					}
					case fractalColoring_Line:
					{
						// Line: absolute dot product with a line direction.
						// Creates line-like coloring patterns.
						len = fabs(colorZ.Dot(in.material->fractalColoring.lineDirection));
						break;
					}
					case fractalColoring_None:
					{
						// No coloring: use |z| directly (same as Standard but without min tracking).
						len = aux.r;
						break;
					}
				}
				// Post-v2.15 bailout logic: track the minimum color metric value
				// and bailout when |z| exceeds the bailout threshold.
				if (!in.material->fractalColoring.colorPreV215False)
				{ // updated code in V2.15
					if (fractal->formula != mandelbox)
					{
						if (len < colorMin) colorMin = len;
						if (aux.r > fractData.bailout) break;

						if (fractData.useAdditionalBailoutCond && (z - lastZ).Length() / aux.r < 1e-15) break;
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
							if (aux.r > fractData.bailout || (z - lastZ).Length() / aux.r < 1e-15) break;
						}
					}
				}
				// Pre-v2.15 bailout logic: older, more aggressive bailout.
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
				// Fake lights orbit trap: compute the distance from the orbit point z to
				// a procedural light shape (point, line, circle, sphere, cube).
				// If fakeLightsRelativeCenter is set, the distance is measured from (z - c)
				// instead of z, making the light position relative to the Julia constant.
				// Accumulate 1/distance² for iterations within the fake lights range to
				// compute procedural lighting intensity.
				double distance = (in.common->fakeLightsRelativeCenter)
														? OrbitTrapShapeDistance(z - aux.const_c, in.common)
														: OrbitTrapShapeDistance(z, in.common);

				if (i >= fakeLightsMinIter && i <= fakeLightsMaxIter)
					orbitTrapTotal += (1.0 / (distance * distance));
				if (distance > fractData.bailout)
				{
					out->orbitTrapR = orbitTrapTotal;
					break;
				}
			}
			else if (Mode == calcModeCubeOrbitTrap)
			{
				// Fractalize texture: when z enters a bounding cube around the starting point,
				// extract texture coordinates from the orbit position. This is used for
				// fractal texture mapping (FractalizeTexture in displacement_map.cpp).
				if (i >= in.material->textureFractalizeStartIteration)
				{
					double size = in.material->textureFractalizeCubeSize;
					CVector3 zz = z.GetXYZ() - in.point;
					if (zz.x > -size && zz.x < size && zz.y > -size && zz.y < size && zz.z > -size
							&& zz.z < size)
					{
						out->colorIndex = (fabs(z.x - size) + fabs(z.y - size) + fabs(z.z - size)) * 100.0;
						out->iters = i + 1;
						out->z = z.GetXYZ();
						return;
					}
				}
				// If the orbit escapes the cube region, return with colorIndex = 0 (no fractalize).
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
		// Compute the final distance estimate from the accumulated derivative (aux.DE)
		// and orbit radius (aux.r) using the appropriate DE function.
		// For hybrid sequences, DEFunctionType is set from the sequence config.
		// For non-hybrid, DEAnalyticFunction is set from the individual fractal config.
		if (aux.DE > 0.0)
		{
			if (seq->isHybrid)
			{
				// Hybrid sequence DE functions:
				if (seq->DEFunctionType == fractal::linearDEFunction)
				{
					// Linear DE: distance = (r - offset) / DE
					// The linearDEOffset shifts the distance estimate (used for certain fractal types).
					out->distance = (aux.r - in.common->linearDEOffset) / aux.DE;
				}
				else if (seq->DEFunctionType == fractal::logarithmicDEFunction)
				{
					// Logarithmic DE: distance = 0.5 * r * log(r) / DE
					// Required for fractals where the derivative grows exponentially.
					// out->distance = 0.5 * r * log(r) / aux.DE;
					if (aux.r > 1.0)
						out->distance = 0.5 * aux.r * log(aux.r) / aux.DE;
					else
						out->distance = 0.0;
				}
				else if (seq->DEFunctionType == fractal::pseudoKleinianDEFunction)
				{
					// Pseudo-Kleinian DE: combines radial and z-component terms.
					// rxy = sqrt(x² + y²) is the distance from the z-axis.
					// The DE uses max(rxy - pseudoKleinianDE, |rxy * z| / r) to handle
					// the inversion symmetry of Kleinian group fractals.
					double rxy = sqrt(z.x * z.x + z.y * z.y);

					out->distance = max(rxy - aux.pseudoKleinianDE, fabs(rxy * z.z) / aux.r) / aux.DE;
				}
				else if (seq->DEFunctionType == fractal::josKleinianDEFunction)
				{
					// Jos Kleinian DE: uses foldingValue for sphere folding.
					// FIXME: it should not refer to fractal with 0 index but to the current sequence, but it
					// is needed for some formulas to work in deltaDE mode. It needs to be fixed in a better
					// way.
					if (seq->fractData[0].fractalParameters.transformCommon.spheresEnabled)
						z.y = min(z.y, seq->fractData[0].fractalParameters.transformCommon.foldingValue - z.y);

					out->distance = min(z.y, seq->fractData[0].fractalParameters.analyticDE.tweak005)
													/ max(aux.DE, seq->fractData[0].fractalParameters.analyticDE.offset1);
				}
				else if (seq->DEFunctionType == fractal::customDEFunction)
				{
					// Custom DE: use the distance computed by the formula itself (aux.dist).
					out->distance = aux.dist;
				}
				else if (seq->DEFunctionType == fractal::maxAxisDEFunction)
				{
					// Max-axis DE: distance = max(|x|, |y|, |z|) / DE.
					// Used for fractals where the max component dominates the growth.
					CVector4 absZ = fabs(z);
					double rd = max(absZ.x, max(absZ.y, absZ.z));
					out->distance = rd / aux.DE;
				}
			}
			else
			{
				// Non-hybrid: use the fractal's own DEAnalyticFunction setting.
				switch (seq->DEAnalyticFunction)
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
						// Standard linear DE: distance = r / DE = |z| / |dz/dn|
						// This is the most common DE formula (e.g., Mandelbulb).
						out->distance = aux.r / aux.DE;
						break;
					}
					case analyticFunctionIFS:
					{
						// IFS DE: distance = (r - 2) / DE.
						// The offset of 2 accounts for the IFS contraction factor.
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
						if (seq->fractData[sequence].fractalParameters.transformCommon.spheresEnabled)
							z.y = min(
								z.y, seq->fractData[sequence].fractalParameters.transformCommon.foldingValue - z.y);

						out->distance =
							min(z.y, seq->fractData[sequence].fractalParameters.analyticDE.tweak005)
							/ max(aux.DE, seq->fractData[sequence].fractalParameters.analyticDE.offset1);
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
		// Compute the final color index using the accumulated orbit data.
		// CalculateColorIndex handles all coloring algorithms (orbit trap, z-depth,
		// sphere, cross, line, etc.) and hybrid blending.
		enumColoringFunction coloringFunction = seq->coloringFunction;
		out->colorIndex = CalculateColorIndex(seq->isHybrid, aux.r, z, colorMin, aux,
			in.material->fractalColoring, coloringFunction, defaultFractal);
	}
	else
	{
		// DeltaDE passes and orbit trap passes don't compute a meaningful distance.
		out->distance = 0.0;

		// needed for JosKleinian fractal to calculate spheres in deltaDE mode
		// FIXME: it should not refer to fractal with 0 index but to the current sequence, but it
		// is needed for some formulas to work in deltaDE mode. It needs to be fixed in a better
		// way.
		if (seq->DEFunctionType == fractal::josKleinianDEFunction)
		{
			if (seq->fractData[sequence].fractalParameters.transformCommon.spheresEnabled)
				z.y =
					min(z.y, seq->fractData[sequence].fractalParameters.transformCommon.foldingValue - z.y);
		}
	}

	// Store the final iteration count (1-indexed) and the final orbit position.
	out->iters = i + 1;
	out->z = z.GetXYZ();
}

template void Compute<calcModeNormal>(
	const cHybridFractalSequences::sSequence *sequence, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeDeltaDE1>(
	const cHybridFractalSequences::sSequence *sequence, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeDeltaDE2>(
	const cHybridFractalSequences::sSequence *sequence, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeColouring>(
	const cHybridFractalSequences::sSequence *sequence, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeOrbitTrap>(
	const cHybridFractalSequences::sSequence *sequence, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeCubeOrbitTrap>(
	const cHybridFractalSequences::sSequence *sequence, const sFractalIn &in, sFractalOut *out);
