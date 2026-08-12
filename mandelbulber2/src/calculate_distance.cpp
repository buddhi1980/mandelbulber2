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
 * CalculateDistanceSimple() function calculates approximate distance to
 * the fractal surface using fractal computation functions.
 *
 * CalculateDistance() function calculates resultant distance to all
 * objects on scene including boolean operators.
 */

#include "calculate_distance.hpp"

#include <QVector>

#include "compute_fractal.hpp"
#include "common_math.h"
#include "displacement_map.hpp"
#include "fractal.h"
#include "fractal_enums.h"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "object_node_type.h"
#include "perlin_noise_octaves.h"
#include "render_data.hpp"
#include "shader_perlin_noise_for_shaders.hpp"
#include "texture_mapping.hpp"
#include "write_log.hpp"
#include "primitive.hpp"

using namespace std;

double CalculateDistance(const sParamRender &params, const cHybridFractalSequences &fractals,
	const sDistanceIn &in, sDistanceOut *out, sRenderData *data)
{
	return CalculateDistanceFromObjectsTree(params, fractals, in, out, data);
}

// Stack frame for traversing the object tree hierarchy.
// The tree uses a preorder traversal where boolean group nodes push a new
// stack level and leaf nodes (fractals/primitives) are merged into the
// current top level using the appropriate boolean operator.
// 'cumulativeDistance' holds the running distance result for the current
// boolean group scope (union, intersection, or subtraction).
// 'closestObjectId' / 'closestObjectSequence' track which fractal leaf
// is closest, needed later for coloring, displacement, and normal calc.
struct ObjectTreeStackFrame
{
	int level;
	int closestObjectId;
	int closestObjectSequence;
	double cumulativeDistance;
	double detailSize;
	enumNodeType nodeType;
	CVector3 transformedPoint;
	bool hasTransformedPoint;
	bool smoothCombineEnable = false;
	double smoothCombineDistance = 0.0;
};

// Merge a child node's distance result into its parent boolean group.
// Uses a stack-based approach: each boolean group node pushes a new
// frame with identity distance (inf for union, -inf for intersection),
// then leaf nodes are merged into the current frame.
//
// Distance combination rules:
// - booleanMul (intersection): take the MAX of distances (inside both = closer to exit)
// - booleanSub (subtraction): standard set difference with smooth transition
//   when child is within 1.5x detailSize of the surface
// - booleanAdd (union): take the MIN of distances (closest surface wins),
//   optionally with smooth union blending if smooth_de_combine_enable is set
//
// The closestObjectId/transformedPoint are propagated so that downstream
// code (coloring, displacement, normal calculation) knows which fractal
// or primitive was the closest hit.
static void mergeChildIntoParent(const ObjectTreeStackFrame &child, ObjectTreeStackFrame *parent,
	const sRenderData *data, double detailSize)
{
	const double childDistance = child.cumulativeDistance;

	switch (parent->nodeType)
	{
		case enumNodeType::booleanMul:
		{
			// Intersection: take the maximum distance (the object we are INSIDE the most).
			// For intersection, the signed distance is the max of component distances.
			if (childDistance > parent->cumulativeDistance)
			{
				parent->cumulativeDistance = childDistance;
				parent->closestObjectId = child.closestObjectId;
				parent->closestObjectSequence = child.closestObjectSequence;
				parent->transformedPoint = child.transformedPoint;
				parent->hasTransformedPoint = child.hasTransformedPoint;
			}
			break;
		}
		case enumNodeType::booleanSub:
		{
			// Subtraction (A - B): first child initializes the result,
			// subsequent children subtract their contribution.
			// Uses a smooth transition zone: when childDistance < 1.5 * detailSize,
			// apply smooth subtraction; otherwise fall back to max(childDist, parentDist).
			if (parent->cumulativeDistance >= 1e19)
			{
				// First child in subtraction group initializes the result.
				parent->cumulativeDistance = childDistance;
				parent->closestObjectId = child.closestObjectId;
				parent->closestObjectSequence = child.closestObjectSequence;
				parent->transformedPoint = child.transformedPoint;
				parent->hasTransformedPoint = child.hasTransformedPoint;
			}
			else if (parent->detailSize > 0 && childDistance < parent->detailSize)
			{
				// Child is close enough to the parent surface to apply smooth subtraction.
				const double limit = 1.5;
				const double limitDist = parent->detailSize * limit;

				if (childDistance < limitDist)
				{
					// Within the smooth transition zone: clamp to limitDist.
					parent->cumulativeDistance = limitDist;
					parent->closestObjectId = child.closestObjectId;
					parent->closestObjectSequence = child.closestObjectSequence;
					parent->transformedPoint = child.transformedPoint;
					parent->hasTransformedPoint = child.hasTransformedPoint;
				}
				else
				{
					// Beyond transition zone: smooth subtraction using max(limitDist - childDist,
					// parentDist).
					const double adjustedDistance =
						max(limitDist - childDistance, parent->cumulativeDistance);
					parent->cumulativeDistance = (adjustedDistance < 0) ? 0 : adjustedDistance;
					parent->closestObjectId = child.closestObjectId;
					parent->closestObjectSequence = child.closestObjectSequence;
					parent->transformedPoint = child.transformedPoint;
					parent->hasTransformedPoint = child.hasTransformedPoint;
				}
			}
			break;
		}
		case enumNodeType::booleanAdd:
		default:
		{
			// Union (A + B): take the minimum distance (closest surface).
			// If smooth_de_combine_enable is set, blend the two distances using
			// opSmoothUnion (based on iquilezles' smooth minimum/union functions)
			// with smooth_de_combine_distance controlling the blend radius.
			const bool smoothEnabled = child.smoothCombineEnable;
			const double smoothDistance = child.smoothCombineDistance;

			if (smoothEnabled && parent->cumulativeDistance < 1e19)
			{
				const double parentDistanceBefore = parent->cumulativeDistance;
				parent->cumulativeDistance =
					opSmoothUnion(childDistance, parent->cumulativeDistance, smoothDistance);
				if (childDistance < parentDistanceBefore)
				{
					parent->closestObjectId = child.closestObjectId;
					parent->closestObjectSequence = child.closestObjectSequence;
					parent->transformedPoint = child.transformedPoint;
					parent->hasTransformedPoint = child.hasTransformedPoint;
				}
			}
			else if (childDistance < parent->cumulativeDistance)
			{
				// Standard union: take the closer surface.
				parent->cumulativeDistance = childDistance;
				parent->closestObjectId = child.closestObjectId;
				parent->closestObjectSequence = child.closestObjectSequence;
				parent->transformedPoint = child.transformedPoint;
				parent->hasTransformedPoint = child.hasTransformedPoint;
			}
			break;
		}
	}
}

// Traverse the objects tree (a hierarchical scene graph) and compute the
// resultant signed distance to all objects using SDF boolean operations.
//
// The objects tree is stored as a flat list of nodes in preorder traversal
// order. Each node has a 'level' indicating its depth. Boolean group nodes
// (booleanAdd, booleanMul, booleanSub) push a new stack frame, and leaf
// nodes (fractal, primitive) are merged into the current frame.
//
// Transform pipeline for each leaf node:
//   1. world -> local: apply node.worldToLocalMatrix (precomputed from
//      the node's position/rotation/scale hierarchy)
//   2. repeat in LOCAL space: pointLocal.repeatMod(repeat / absScale)
//      The repeat is scaled by absScale so that the tiling period is
//      consistent in world space regardless of the object's scale.
//   3. fractal computation uses the transformed point directly
//   4. distance is scaled back by absNodeScale to restore world-space units
//
// This order (transform -> repeat -> fractal -> scale back) ensures that
// repeat tiling works correctly on rotated/scaled objects.
double CalculateDistanceFromObjectsTree(const sParamRender &params,
	const cHybridFractalSequences &fractals, const sDistanceIn &in, sDistanceOut *out,
	sRenderData *data)
{
	// limit to 10 levels of tree
	ObjectTreeStackFrame stack[10];

	out->objectId = 0;
	out->totalIters = 0;
	out->maxiter = false;
	out->seqIndex = 0;
	out->transformedPoint = in.point;
	out->hasTransformedPoint = false;

	if (data)
	{
		const auto &nodes = data->nodesDataForRendering;
		const auto &primitives = params.primitives.allPrimitives;
		const int nodeCount = nodes.size();
		int totalIters = 0;

		stack[0].cumulativeDistance = 1e20;
		stack[0].level = 0;
		stack[0].closestObjectId = -1;
		stack[0].closestObjectSequence = -1;
		stack[0].detailSize = in.detailSize;
		stack[0].nodeType = enumNodeType::booleanAdd;
		stack[0].transformedPoint = in.point;
		stack[0].hasTransformedPoint = false;
		stack[0].smoothCombineEnable = false;
		stack[0].smoothCombineDistance = 0.0;

		// Early-out: if the point is outside the user-defined limit box
		// by more than detailSize, return the box distance directly.
		// This mirrors the OpenCL LIMITS_ENABLED path.
		// boxDist components are negative when inside the box, positive when outside.
		if (params.limitsEnabled)
		{
			CVector3 boxDist;
			boxDist.x = max(in.point.x - params.limitMax.x, -(in.point.x - params.limitMin.x));
			boxDist.y = max(in.point.y - params.limitMax.y, -(in.point.y - params.limitMin.y));
			boxDist.z = max(in.point.z - params.limitMax.z, -(in.point.z - params.limitMin.z));
			double limitBoxDist = std::max({boxDist.x, boxDist.y, boxDist.z});
			if (limitBoxDist > in.detailSize)
			{
				out->maxiter = false;
				out->distance = limitBoxDist;
				out->totalIters = 0;
				return limitBoxDist;
			}
		}

		int stackLevel = 0;
		int numberOfFractalsToSkip = 0;

		for (int i = 0; i < nodeCount; ++i)
		{
			const auto &node = nodes[i];
			if (!node.enabled)
			{
				continue;
			}
			double distance = 1e20;
			int objectId = -1;
			int sequenceIndex = -1;
			int leafIters = 0;

			// Apply combined inverse transform (world -> local space)
			// using the pre-calculated 4×4 homogeneous matrix.
			// worldToLocalMatrix encodes: translate(-position) * rotate(-rotation)
			// TransformPoint uses the upper 3×3 rotation + translation (w=1).
			CVector3 pointLocal = node.worldToLocalMatrix.TransformPoint(in.point);

			// Apply repeat (tiling) in LOCAL space.
			// repeat is divided by absScale so the tiling period is in world-space
			// units, making the tiling visually consistent regardless of object scale.
			const double absNodeScale = node.absScale;
			CVector3 repeatForLocalSpace = node.repeat / absNodeScale;
			CVector3 pointWithRepeat = (repeatForLocalSpace.Length() > 0.0)
																	 ? pointLocal.repeatMod(repeatForLocalSpace)
																	 : pointLocal;

			CVector3 pointTransformed = pointWithRepeat;
			const double detailMult = node.detailLevelMultiplier;
			sDistanceIn nodeIn = in;

			// Adjust detailSize for the node's scale and detail multiplier.
			// detailSize controls the raymarching step size and bailout thresholds.
			// When the object is scaled, the effective detail size in local space
			// must be divided by the scale factor to maintain correct step sizing.
			if (absNodeScale > 0.0 && detailMult > 0.0)
			{
				nodeIn.detailSize = in.detailSize / absNodeScale / detailMult;
			}
			else if (absNodeScale > 0.0)
			{
				nodeIn.detailSize = in.detailSize / absNodeScale;
			}
			else if (detailMult > 0.0)
			{
				nodeIn.detailSize = in.detailSize / detailMult;
			}
			else
			{
				nodeIn.detailSize = in.detailSize;
			}

			// Handle tree traversal: when the current node's level is less than
			// the stack depth, pop stack frames (close boolean group scopes).
			if (node.level < stackLevel)
			{
				// pop stack levels
				while (stackLevel > node.level)
				{
					ObjectTreeStackFrame child = stack[stackLevel];
					stackLevel--;
					mergeChildIntoParent(child, &stack[stackLevel], data, stack[stackLevel].detailSize);
				}
			}

			switch (node.type)
			{
				case enumNodeType::fractal:
				{
					if (numberOfFractalsToSkip == 0)
					{
						int seqIndex = node.hybridSequenceIndex;
						const cHybridFractalSequences::sSequence *seq =
							data->hybridFractalSequences.GetSequence(seqIndex);
						if (!seq)
						{
							continue;
						}
						sDistanceOut nodeOut;
						distance = CalculateDistanceSimple(
							params, nodeIn, pointTransformed, &nodeOut, node.internalObjectId, seq);
						// Scale distance back to world space (distance was computed in local space).
						distance *= absNodeScale;
						objectId = node.internalObjectId;
						sequenceIndex = seqIndex;
						leafIters = nodeOut.iters;
					}
					else
					{
						// skipping fractal because it is part of hybrid sequence
						numberOfFractalsToSkip--;
						continue;
					}
					// Apply displacement maps (bump maps) and Perlin noise to the distance.
					// These modify the raw distance to add surface detail without changing
					// the underlying geometry. Both operate in world space (use in.point).
					if (objectId >= 0)
					{
						distance = DisplacementMap(distance, in.point, objectId, data);
						distance = PerlinNoiseDisplacement(distance, in.point, data, objectId);
					}
					break;
				}
				case enumNodeType::primitive:
				{
					int primIdx = node.primitiveIdx;
					if (primIdx >= 0 && primIdx < (int)primitives.size() && primitives[primIdx])
					{
						{
							// sPrimitiveWater has a special distance function that combines
							// the primitive's base distance with another object's distance
							// to create wave-like interactions.
							sPrimitiveWater *water = dynamic_cast<sPrimitiveWater *>(primitives[primIdx].get());
							if (water)
							{
								distance = water->PrimitiveDistanceWater(pointTransformed, distance);
							}
							else
							{
								distance = primitives[primIdx]->PrimitiveDistance(pointTransformed);
							}
							// Scale distance back to world space.
							distance *= absNodeScale;
						}
						objectId = node.internalObjectId;
					}
					// Apply displacement maps (bump maps) and Perlin noise to the distance.
					if (objectId >= 0)
					{
						distance = DisplacementMap(distance, in.point, objectId, data);
						distance = PerlinNoiseDisplacement(distance, in.point, data, objectId);
					}
					break;
				}
				case enumNodeType::hybrid:
				{
					// Hybrid nodes represent a sequence of fractal formulas applied
					// in a specific order. The sequence contains multiple fractal
					// formulas that are iterated in a loop.
					int seqIndex = node.hybridSequenceIndex;
					const cHybridFractalSequences::sSequence *seq =
						data->hybridFractalSequences.GetSequence(seqIndex);
					if (!seq)
					{
						continue;
					}
					if (seq->fractData.empty())
					{
						continue;
					}
					sDistanceOut nodeOut;
					distance = CalculateDistanceSimple(params, nodeIn, pointTransformed, &nodeOut, -1, seq);
					distance *= absNodeScale;
					// Mark how many subsequent fractal nodes should be skipped
					// (they are already included in this hybrid sequence).
					numberOfFractalsToSkip = seq->numberOfFractalsInTheSequence;
					sequenceIndex = seqIndex;
					objectId = seq->internalObjectId;
					leafIters = nodeOut.iters;
					if (objectId >= 0)
					{
						distance = DisplacementMap(distance, in.point, objectId, data);
						distance = PerlinNoiseDisplacement(distance, in.point, data, objectId);
					}
					break;
				}
				case enumNodeType::booleanAdd:
				case enumNodeType::booleanMul:
				case enumNodeType::booleanSub:
				{
					// Boolean group node: push a new stack frame and initialize
					// the cumulative distance to the identity for the operation.
					// Union (add): identity = +inf (any distance will be smaller)
					// Intersection (mul): identity = -inf (any distance will be larger)
					// Subtraction: identity = +inf (same as union for first operand)
					stackLevel++;
					stack[stackLevel].cumulativeDistance =
						(node.type == enumNodeType::booleanMul) ? -1e20 : 1e20;
					stack[stackLevel].level = node.level;
					stack[stackLevel].nodeType = node.type;
					stack[stackLevel].closestObjectId = -1;
					stack[stackLevel].closestObjectSequence = -1;
					stack[stackLevel].detailSize = nodeIn.detailSize;
					stack[stackLevel].transformedPoint = in.point;
					stack[stackLevel].hasTransformedPoint = false;
					stack[stackLevel].smoothCombineEnable = node.smooth_de_combine_enable;
					stack[stackLevel].smoothCombineDistance = node.smooth_de_combine_distance;
					continue;
				}

				default: break;
			}

			// Wrap the leaf result as a stack frame and merge it into the
			// current boolean group scope.
			ObjectTreeStackFrame leaf;
			leaf.cumulativeDistance = distance;
			leaf.closestObjectId = objectId;
			leaf.closestObjectSequence = sequenceIndex;
			leaf.detailSize = nodeIn.detailSize;
			leaf.transformedPoint = pointTransformed;
			leaf.hasTransformedPoint = (objectId >= 0);
			leaf.smoothCombineEnable = node.smooth_de_combine_enable;
			leaf.smoothCombineDistance = node.smooth_de_combine_distance;
			mergeChildIntoParent(leaf, &stack[stackLevel], data, stack[stackLevel].detailSize);
			totalIters += leafIters;
		}

		// final node summation
		if (stackLevel > 0)
		{
			// pop stack levels
			while (stackLevel > 0)
			{
				ObjectTreeStackFrame child = stack[stackLevel];
				stackLevel--;
				mergeChildIntoParent(child, &stack[stackLevel], data, stack[stackLevel].detailSize);
			}
		}

		out->distance = stack[0].cumulativeDistance;
		out->objectId = stack[0].closestObjectId;
		out->seqIndex = stack[0].closestObjectSequence;
		out->transformedPoint = stack[0].transformedPoint;
		out->hasTransformedPoint = stack[0].hasTransformedPoint;
		out->iters = totalIters;
		out->detailLevelMultiplier = 1.0;
		if (stack[0].closestObjectId >= 0
				&& stack[0].closestObjectId < static_cast<int>(data->objectData.size()))
		{
			out->detailLevelMultiplier = data->objectData[stack[0].closestObjectId].detailLevelMultiplier;
		}
		return out->distance;
	}
	return 0;
}

// Compute the distance to a single fractal (or hybrid fractal sequence)
// at a given point using either analytic distance estimation or the
// numerical delta-DE method.
//
// The fractal computation is done in local space (point is already
// transformed by the caller). The returned distance is in local space
// and must be scaled by absNodeScale by the caller.
//
// DE (distance estimation) modes:
// - analyticDEType: the fractal formula computes an exact analytical DE.
//   This is faster and more accurate but only available for certain formulas.
// - deltaDEType: numerically estimate DE by perturbing the point along
//   each axis and measuring the change in |z| (orbit radius). The DE
//   value is then combined with the orbit radius using a DE function
//   (linear, logarithmic, pseudo-Kleinian, etc.).
double CalculateDistanceSimple(const sParamRender &params, const sDistanceIn &in,
	const CVector3 &point, sDistanceOut *out, int forcedFormulaIndex,
	const cHybridFractalSequences::sSequence *sequence)
{
	double distance = 0;

	// When calculating normal vectors, we need extra iterations for accuracy.
	// iterThreshMode amplifies this further to ensure proper bailout thresholds.
	const int maxiterMultiplier = (in.normalCalculationMode && params.common.iterThreshMode) ? 5 : 1;

	// sFractalIn carries all parameters needed by the fractal computation.
	// forcedFormulaIndex overrides which fractal formula to use (-1 = use sequence default).
	sFractalIn fractIn(point, params.minN, -1, maxiterMultiplier, 0, &params.common,
		forcedFormulaIndex, in.normalCalculationMode);
	sFractalOut fractOut;
	fractOut.colorIndex = 0;

	if (sequence->DEType == fractal::analyticDEType)
	{
		// Analytic DE: the fractal formula computes the distance directly.
		Compute<fractal::calcModeNormal>(sequence, fractIn, &fractOut);
		distance = fractOut.distance;
		// qDebug() << "computed distance" << distance;
		out->maxiter = fractOut.maxiter;
		out->iters = fractOut.iters;
		out->colorIndex = fractOut.colorIndex;
		out->totalIters += fractOut.iters;

		// don't use maxiter when limits are disabled and iterThresh mode is not used
		if (!params.common.iterThreshMode) out->maxiter = false;

		// never use maxiter if normal vectors are calculated
		if (in.normalCalculationMode) out->maxiter = false;

		if (out->maxiter) distance = 0.0;

		if (fractOut.iters < params.minN && distance < in.detailSize) distance = in.detailSize;

		// Interior mode: detect when the point is inside the fractal.
		// Without normal calc: if distance is very small (close to surface) or maxiter hit,
		// the point is considered inside → return detailSize as a safe upper bound.
		// With normal calc: use a wider threshold (0.9 * detailSize) and return
		// (detailSize - distance) to get a positive interior distance value.
		if (params.interiorMode && !in.normalCalculationMode)
		{
			if (distance < 0.5 * in.detailSize || fractOut.maxiter)
			{
				distance = in.detailSize;
				out->maxiter = false;
			}
		}
		else if (params.interiorMode && in.normalCalculationMode)
		{
			if (distance < 0.9 * in.detailSize)
			{
				distance = in.detailSize - distance;
				out->maxiter = false;
			}
		}

		// iterThreshMode: push the distance slightly beyond detailSize to force
		// the raymarcher to take a step. This prevents the renderer from getting
		// stuck on points that are technically "close" but not actually on the surface.
		if (params.common.iterThreshMode && !in.normalCalculationMode && !fractOut.maxiter)
		{
			if (distance < in.detailSize)
			{
				distance = in.detailSize * 1.01;
			}
		}

		if (distance < 0.0) distance = 0.0;
		if (distance > 10.0) distance = 10.0;
	}
	else
	{
		// Delta-DE mode: compute DE numerically by perturbing the point
		// along each axis and measuring the change in orbit radius |z|.
		// deltaDE is the perturbation step size, scaled by either the point's
		// distance from origin or detailSize (depending on advancedQuality).
		double deltaDE;
		if (params.advancedQuality)
		{
			deltaDE = max(fractIn.point.Length() * 1e-14, params.deltaDERelativeDelta * in.detailSize);
		}
		else
		{
			deltaDE = max(fractIn.point.Length() * 1e-14, 1e-5 * in.detailSize);
		}

		// First compute the fractal at the original point to get the orbit radius r = |z|.
		Compute<fractal::calcModeDeltaDE1>(sequence, fractIn, &fractOut);
		const double r = fractOut.z.Length();
		CVector3 zFromIters = fractOut.z;
		out->maxiter = fractOut.maxiter;
		bool maxiter = fractOut.maxiter;
		out->iters = fractOut.iters;
		out->colorIndex = fractOut.colorIndex;
		out->totalIters += fractOut.iters;

		// don't use maxiter when limits are disabled and iterThresh mode is not used
		if (!params.limitsEnabled)
		{
			if (!params.common.iterThreshMode) maxiter = false;
		}
		else
		{
			// never use maxiter if normal vectors are calculated
			if (in.normalCalculationMode) maxiter = false;
		}

		// For subsequent perturbations, use the same iteration count as the first pass.
		// This ensures the comparison is fair (same number of iterations for each direction).
		fractIn.forcedMaxiter =
			fractOut.iters; // for other directions must be the same number of iterations

		// Perturb along X axis and measure change in |z|.
		fractIn.point = point + CVector3(deltaDE, 0.0, 0.0);
		Compute<fractal::calcModeDeltaDE1>(sequence, fractIn, &fractOut);
		double r2 = fractOut.z.Length();
		const double dr1 = fabs(r2 - r) / deltaDE;
		out->totalIters += fractOut.iters;

		// Perturb along Y axis.
		fractIn.point = point + CVector3(0.0, deltaDE, 0.0);
		Compute<fractal::calcModeDeltaDE1>(sequence, fractIn, &fractOut);
		r2 = fractOut.z.Length();
		const double dr2 = fabs(r2 - r) / deltaDE;
		out->totalIters += fractOut.iters;

		// Perturb along Z axis.
		fractIn.point = point + CVector3(0.0, 0.0, deltaDE);
		Compute<fractal::calcModeDeltaDE1>(sequence, fractIn, &fractOut);
		r2 = fractOut.z.Length();
		const double dr3 = fabs(r2 - r) / deltaDE;
		out->totalIters += fractOut.iters;

		// Combine the three partial derivatives into the total derivative magnitude.
		// This is the gradient of |z|: |∇|z|| = sqrt(dr1² + dr2² + dr3²).
		const double dr = sqrt(dr1 * dr1 + dr2 * dr2 + dr3 * dr3);

		if (dr > 0)
		{
			// Apply the appropriate DE function based on the fractal type.
			// The DE function converts (r, dr) into a distance estimate.
			// Different fractal families require different DE formulas:
			// - linear: standard DE for power-based fractals (distance = 0.5 * r / dr)
			// - logarithmic: for log-based fractals (distance = 0.5 * r * log(r) / dr)
			// - pseudoKleinian: for pseudo-Kleinian group fractals
			// - josKleinian: for Jos Kleinian group fractals
			// - maxAxis: for max-axis DE method
			if (sequence->DEFunctionType == fractal::linearDEFunction)
				distance = 0.5 * r / dr;
			else if (sequence->DEFunctionType == fractal::logarithmicDEFunction)
				distance = 0.5 * r * log(r) / dr;
			else if (sequence->DEFunctionType == fractal::pseudoKleinianDEFunction)
			{
				const CVector3 z = fractOut.z;
				const double rxy = sqrt(z.x * z.x + z.y * z.y);
				distance = max(rxy - 0.92784, fabs(rxy * z.z) / r) / (dr);
			}
			else if (sequence->DEFunctionType == fractal::josKleinianDEFunction)
			{
				const CVector3 z = fractOut.z;
				const double rxy = sqrt(z.x * z.x + z.z * z.z);
				distance = (fabs(rxy * z.y) / r) / (dr);
				maxiter = false;
			}
			else if (sequence->DEFunctionType == fractal::customDEFunction)
			{
				distance = r; // FIXME: Can we calculate dIFS in deltaDE mode ???
			}
			else if (sequence->DEFunctionType == fractal::maxAxisDEFunction)
			{
				CVector3 absZ = fabs(zFromIters);
				double maxZ = dMax(absZ.x, absZ.y, absZ.z);
				double maxDr = dMax(fabs(dr1), fabs(dr2), fabs(dr3));
				distance = 0.5 * maxZ / maxDr;
			}
		}
		else
		{
			// If the derivative is zero (e.g., at the origin or a fixed point),
			// fall back to using the orbit radius as the distance.
			distance = r;
		}

		// if (distance < 1e-20) distance = 1e-20;

		// don't use maxiter when limits are disabled and iterThresh mode is not used
		if (!params.common.iterThreshMode) out->maxiter = false;

		// never use maxiter if normal vectors are calculated
		if (in.normalCalculationMode) out->maxiter = false;

		if (out->maxiter) distance = 0.0;

		if (fractOut.iters < params.minN && distance < in.detailSize) distance = in.detailSize;

		// Interior mode: same logic as analytic DE path.
		if (params.interiorMode && !in.normalCalculationMode)
		{
			if (distance < 0.5 * in.detailSize || maxiter)
			{
				distance = in.detailSize;
				out->maxiter = false;
			}
		}
		else if (params.interiorMode && in.normalCalculationMode)
		{
			if (distance < 0.9 * in.detailSize)
			{
				distance = in.detailSize - distance;
				out->maxiter = false;
			}
		}

		if (params.common.iterThreshMode && !in.normalCalculationMode && !maxiter)
		{
			if (distance < in.detailSize)
			{
				distance = in.detailSize * 1.01;
			}
		}

		if (distance < 0.0) distance = 0.0;
		if (distance > 10.0) distance = 10.0;
	}

	return distance;
}

// Find the minimum distance from a plane to the fractal surface by iteratively
// searching along the plane's normal direction.
//
// The plane is defined by a point on the plane (planePoint) and its normal
// (direction). The search starts at planePoint and moves along the normal,
// sampling points in a circular pattern perpendicular to the normal to find
// the point closest to the fractal surface.
//
// This is used by the UI "move to fractal" feature to snap the camera/limits
// to the nearest point on the fractal surface.
//
// Returns the signed distance along the normal direction from planePoint to
// the closest point on the fractal surface. Returns 0 if the surface is not
// found or the search diverges.
double CalculateDistanceMinPlane(std::shared_ptr<const sParamRender> params,
	std::shared_ptr<const cHybridFractalSequences> fractals, const CVector3 planePoint,
	const CVector3 direction, const CVector3 orthDirection, bool *stopRequest)
{
	// the plane is defined by the 'planePoint' and the orthogogonal 'direction'
	// the method will return the minimum distance from the plane to the fractal
	double distStep = 0.0;
	CVector3 point = planePoint;
	const double detail = 0.5;
	const int transVectorAngles = 5;

	// Use the current point as the rotation axis for sampling circles
	// perpendicular to the search direction.
	CVector3 rotationAxis = planePoint;
	rotationAxis.Normalize();

	while (distStep == 0 || distStep > 0.00001)
	{
		CVector3 pointNextBest(0, 0, 0);
		double newDistStepMin = 0;
		// Sample points in a circle perpendicular to the search direction.
		// Each sample is offset by distStep/2 along the transversal direction
		// to explore the neighborhood around the current search ray.
		for (int i = 0; i <= transVectorAngles; i++)
		{
			const double angle = (double(i) / transVectorAngles) * 2.0 * M_PI;
			CVector3 transversalVect = orthDirection;
			transversalVect = transversalVect.RotateAroundVectorByAngle(rotationAxis, angle);
			transversalVect.Normalize();
			CVector3 pointNext = point + direction * distStep;
			if (i > 0) pointNext += transversalVect * distStep / 2.0;
			const sDistanceIn in(pointNext, 0, false);
			sDistanceOut out;
			const double dist = CalculateDistance(*params.get(), *fractals.get(), in, &out);
			// The next step size is proportional to the current distance estimate.
			// Smaller distance → smaller step (converging toward the surface).
			const double newDistStep = dist * detail * 0.5;
			if (newDistStep < newDistStepMin || newDistStepMin == 0)
			{
				pointNextBest = pointNext;
				newDistStepMin = newDistStep;
			}
		}
		if (newDistStepMin > 1000) newDistStepMin = 1000;
		if (distStep != 0 && newDistStepMin > distStep) break;
		distStep = newDistStepMin;
		point = pointNextBest;
		// qDebug() << "pointNextBest" << pointNextBest.Debug();
		// Guard against divergence: if the search point moves too far from origin,
		// the fractal surface is not in this direction.
		if (point.Length() > 1000000)
		{
			WriteLog("CalculateDistanceMinPlane(): surface not found!", 1);
			return 0;
		}
		gApplication->processEvents();
		if (*stopRequest)
		{
			return 0;
		}
	}
	// Return the signed projection of the final point onto the search direction.
	return CVector3(point - planePoint).Dot(direction);
}
