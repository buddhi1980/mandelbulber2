/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * distance calculation function for opencl
 */

#ifndef MANDELBULBER2_OPENCL_ENGINES_CALCULATE_DISTANCE_CL_
#define MANDELBULBER2_OPENCL_ENGINES_CALCULATE_DISTANCE_CL_

// calculation of distance where ray-marching stops
float CalcDistThresh(float3 point, __constant sClInConstants *consts)
{
	float distThresh;
	if (consts->params.common.iterThreshMode)
	{
		distThresh =
			length(consts->params.camera - point) * consts->params.resolution * consts->params.fov;
	}
	else
	{
		if (consts->params.constantDEThreshold)
			distThresh = consts->params.DEThresh;
		else
			distThresh = length(consts->params.camera - point) * consts->params.resolution
									 * consts->params.fov / consts->params.detailLevel;
	}

#if defined(PERSP_EQUIRECTANGULAR)
	distThresh *= 0.5f;
#endif

#ifdef ADVANCED_QUALITY
	distThresh = clamp(distThresh, consts->params.detailSizeMin, consts->params.detailSizeMax);
#else
	distThresh = max(distThresh, 1e-6f);
#endif

	return distThresh;
}

// calculation of "voxel" size
float CalcDelta(float3 point, __constant sClInConstants *consts)
{
	float delta;
	delta = length(consts->params.camera - point) * consts->params.resolution * consts->params.fov;

#if defined(PERSP_EQUIRECTANGULAR)
	delta *= 0.5f;
#endif

	delta = max(delta, 1e-6f);
	return delta;
}

formulaOut CalculateDistanceSimple(__constant sClInConstants *consts, float3 point,
	sClCalcParams *calcParam, sRenderData *renderData, int hybridSequenceIndex)
{
	formulaOut out;
	out.z = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	out.iters = 0;
	out.distance = 0.0f;
	out.colorIndex = 0.0f;
	out.orbitTrapR = 0.0f;
	out.maxiter = false;
	out.objectId = 0;

	int seqIdx = (hybridSequenceIndex >= 0) ? hybridSequenceIndex : 0;
	__global sHybridSequenceCl *seq = &renderData->hybridSequences[seqIdx];

#ifdef ANALYTIC_DE
#ifdef BOOLEAN_OPERATORS
	if (seq->DEType == analyticDEType)
#endif
	{
		out = Fractal(consts, point, calcParam, calcModeNormal, NULL, -1,
			renderData, hybridSequenceIndex);
		bool maxiter = out.maxiter;

		// don't use maxiter when limits are disabled and iterThresh mode is not used
		if (!consts->params.iterThreshMode) maxiter = false;

		// never use maxiter if normal vectors are calculated
		if (calcParam->normalCalculationMode) maxiter = false;

		if (maxiter) out.distance = 0.0f;

		if (out.iters < consts->params.minN && out.distance < calcParam->detailSize)
			out.distance = calcParam->detailSize;

#ifdef INTERIOR_MODE
		if (!calcParam->normalCalculationMode)
		{
			if (out.distance < 0.5f * calcParam->detailSize || maxiter)
			{
				out.distance = calcParam->detailSize;
				out.maxiter = false;
			}
		}
		else
		{
			if (out.distance < 0.9f * calcParam->detailSize)
			{
				out.distance = calcParam->detailSize - out.distance;
				out.maxiter = false;
			}
		}
#endif

		if (consts->params.common.iterThreshMode && !calcParam->normalCalculationMode && !maxiter)
		{
			if (out.distance < calcParam->detailSize)
			{
				out.distance = calcParam->detailSize * 1.01f;
			}
		}

		if (isinf(out.distance)) out.distance = 0.0f;
		if (out.distance < 0.0f) out.distance = 0.0f;
		if (out.distance > 5.0f) out.distance = 5.0f;
	}
#endif // ANALYTIC_DE

#ifdef DELTA_DE
#ifdef BOOLEAN_OPERATORS
	if (seq->DEType == deltaDEType)
#endif
	{

#ifdef ADVANCED_QUALITY
		float delta =
			max(length(point) * 1.0e-6f, calcParam->detailSize * consts->params.deltaDERelativeDelta);
#else
		float delta = max(length(point) * 1.0e-6f, calcParam->detailSize * 0.1f);
#endif
		float3 dr = 0.0f;

		out = Fractal(consts, point, calcParam, calcModeDeltaDE1, NULL, -1,
			renderData, hybridSequenceIndex);
		calcParam->deltaDEMaxN = out.iters - 1;
		float r = length(out.z);
		float4 zFromIters = out.z;

		bool maxiter = out.maxiter;

// don't use maxiter when limits are disabled and iterThresh mode is not used
#ifndef LIMITS_ENABLED
		if (!consts->params.iterThreshMode) maxiter = false;
#else
		// never use maxiter if normal vectors are calculated
		if (calcParam->normalCalculationMode) maxiter = false;
#endif

		float3 deltas[6];
		deltas[0] = (float3){delta, 0.0f, 0.0f};
		deltas[1] = (float3){-delta, 0.0f, 0.0f};
		deltas[2] = (float3){0.0f, delta, 0.0f};
		deltas[3] = (float3){0.0f, -delta, 0.0f};
		deltas[4] = (float3){0.0f, 0.0f, delta};
		deltas[5] = (float3){0.0f, 0.0f, -delta};

		float rDelta[6];
		for (int i = 0; i < 6; i++)
		{
			rDelta[i] = length(
				Fractal(consts, point + deltas[i], calcParam, calcModeDeltaDE2, NULL, -1,
					renderData, hybridSequenceIndex)
					.z);
		}
		dr.x = min(fabs(rDelta[0] - r), fabs(rDelta[1] - r)) / delta;
		dr.y = min(fabs(rDelta[2] - r), fabs(rDelta[3] - r)) / delta;
		dr.z = min(fabs(rDelta[4] - r), fabs(rDelta[5] - r)) / delta;

		float d = length(dr);

		if (isinf(r) || isinf(d) || d == 0.0f)
		{
			out.distance = 0.0f;
		}
		else
		{
#ifdef DELTA_LINEAR_DE
			out.distance = 0.5f * r / d;
#elif DELTA_LOG_DE
			out.distance = 0.5f * r * native_log(r) / d;
#elif DELTA_PSEUDO_KLEINIAN_DE
			float rxy = native_sqrt(out.z.x * out.z.x + out.z.y * out.z.y);
			out.distance = max(rxy - 0.92784f, fabs(rxy * out.z.z) / r) / d;
#elif DELTA_JOS_KLEINIAN_DE
			float4 z = out.z;
			float rxy = native_sqrt(z.x * z.x + z.z * z.z);
			out.distance = (fabs(rxy * z.y) / r) / d;
			out.maxiter = false;
#elif DELTA_MAXAXIS_DE
			float4 absZ = fabs(zFromIters);
			float maxZ = max(max(absZ.x, absZ.y), absZ.z);
			float maxDr = max(max(fabs(dr.x), fabs(dr.y)), fabs(dr.z));
			out.distance = 0.5f * maxZ / maxDr;
#endif
		}

		// don't use maxiter when limits are disabled and iterThresh mode is not used
		if (!consts->params.iterThreshMode) maxiter = false;

		// never use maxiter if normal vectors are calculated
		if (calcParam->normalCalculationMode) maxiter = false;

		if (maxiter) out.distance = 0.0f;

		if (out.iters < consts->params.minN && out.distance < calcParam->detailSize)
			out.distance = calcParam->detailSize;

#ifdef INTERIOR_MODE
		if (!calcParam->normalCalculationMode)
		{
			if (out.distance < 0.5f * calcParam->detailSize || maxiter)
			{
				out.distance = calcParam->detailSize;
				out.maxiter = false;
			}
		}
		else
		{
			if (out.distance < 0.9f * calcParam->detailSize)
			{
				out.distance = calcParam->detailSize - out.distance;
				out.maxiter = false;
			}
		}
#endif

		if (consts->params.common.iterThreshMode && !calcParam->normalCalculationMode && !maxiter)
		{
			if (out.distance < calcParam->detailSize)
			{
				out.distance = calcParam->detailSize * 1.01f;
			}
		}

		if (out.distance < 0.0f) out.distance = 0.0f;
		if (out.distance > 5.0f) out.distance = 5.0f;
	}
#endif // DELTA_DE

	return out;
}

//------------------------- Tree-based distance calculation -------------------

typedef struct
{
	int level;
	int closestObjectId;
	int closestObjectSequence;
	float cumulativeDistance;
	enumNodeTypeCl nodeType;
	float3 transformedPoint;
	bool hasTransformedPoint;
} ObjectTreeStackFrameCl;

void mergeChildIntoParentCl(
	const ObjectTreeStackFrameCl *child, ObjectTreeStackFrameCl *parent,
	__global sObjectDataCl *objectsData, int numberOfObjects)
{
	const float childDistance = child->cumulativeDistance;

	switch (parent->nodeType)
	{
		case nodeTypeBooleanMul:
		{
			if (childDistance > parent->cumulativeDistance)
			{
				parent->cumulativeDistance = childDistance;
				parent->closestObjectId = child->closestObjectId;
				parent->closestObjectSequence = child->closestObjectSequence;
				parent->transformedPoint = child->transformedPoint;
				parent->hasTransformedPoint = child->hasTransformedPoint;
			}
			break;
		}
		case nodeTypeBooleanSub:
		{
			if (parent->cumulativeDistance >= 1e19f)
			{
				parent->cumulativeDistance = childDistance;
				parent->closestObjectId = child->closestObjectId;
				parent->closestObjectSequence = child->closestObjectSequence;
				parent->transformedPoint = child->transformedPoint;
				parent->hasTransformedPoint = child->hasTransformedPoint;
			}
			else
			{
				parent->cumulativeDistance = max(parent->cumulativeDistance, -childDistance);
				if (childDistance < parent->cumulativeDistance)
				{
					parent->closestObjectId = child->closestObjectId;
					parent->closestObjectSequence = child->closestObjectSequence;
					parent->transformedPoint = child->transformedPoint;
					parent->hasTransformedPoint = child->hasTransformedPoint;
				}
			}
			break;
		}
		case nodeTypeBooleanAdd:
		default:
		{
			const int childObjectId = child->closestObjectId;
			bool smoothEnabled = false;
			float smoothDistance = 0.0f;
			if (childObjectId >= 0 && childObjectId < numberOfObjects)
			{
				smoothEnabled = objectsData[childObjectId].smoothDeCombineEnable;
				smoothDistance = objectsData[childObjectId].smoothDeCombineDistance;
			}

			if (smoothEnabled && parent->cumulativeDistance < 1e19f)
			{
				const float parentDistanceBefore = parent->cumulativeDistance;
				parent->cumulativeDistance =
					opSmoothUnion(childDistance, parent->cumulativeDistance, smoothDistance);
				if (childDistance < parentDistanceBefore)
				{
					parent->closestObjectId = child->closestObjectId;
					parent->closestObjectSequence = child->closestObjectSequence;
					parent->transformedPoint = child->transformedPoint;
					parent->hasTransformedPoint = child->hasTransformedPoint;
				}
			}
			else if (childDistance < parent->cumulativeDistance)
			{
				parent->cumulativeDistance = childDistance;
				parent->closestObjectId = child->closestObjectId;
				parent->closestObjectSequence = child->closestObjectSequence;
				parent->transformedPoint = child->transformedPoint;
				parent->hasTransformedPoint = child->hasTransformedPoint;
			}
			break;
		}
	}
}

formulaOut CalculateDistance(__constant sClInConstants *consts, float3 point,
	sClCalcParams *calcParam, sRenderData *renderData)
{
	formulaOut out;
	out.z = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	out.iters = 0;
	out.distance = 0.0f;
	out.colorIndex = 0.0f;
	out.orbitTrapR = 0.0f;
	out.maxiter = false;
	out.objectId = 0;

	float limitBoxDist = 0.0f;

#ifdef LIMITS_ENABLED
	float3 boxDistance = max(point - consts->params.limitMax, -(point - consts->params.limitMin));
	limitBoxDist = max(max(boxDistance.x, boxDistance.y), boxDistance.z);

	if (limitBoxDist > calcParam->detailSize)
	{
		out.maxiter = false;
		out.distance = limitBoxDist;
		out.iters = 0;
		return out;
	}
#endif

	// Tree-based distance calculation
	#define MAX_TREE_LEVELS 10
	ObjectTreeStackFrameCl stack[MAX_TREE_LEVELS];

	__global sNodeDataForRenderingCl *nodesData = renderData->nodesData;
	const int nodeCount = renderData->numberOfNodes;
	const int numberOfObjects = renderData->numberOfObjects;

	stack[0].cumulativeDistance = 1e20f;
	stack[0].level = 0;
	stack[0].closestObjectId = -1;
	stack[0].closestObjectSequence = -1;
	stack[0].nodeType = nodeTypeBooleanAdd;
	stack[0].transformedPoint = point;
	stack[0].hasTransformedPoint = false;

	int stackLevel = 0;
	int numberOfFractalsToSkip = 0;

	for (int i = 0; i < nodeCount; ++i)
	{
		__global sNodeDataForRenderingCl *node = &nodesData[i];
		float distance = 1e20f;
		int objectId = -1;
		int sequenceIndex = -1;

		// Apply repeat modulation first (in world space)
		float3 pointWithRepeat = modRepeat(point, node->repeat);

		// Apply combined inverse transform (translate + rotate + scale)
		float3 pointTransformed = Matrix44TransformPoint(node->inverseTransformMatrix, pointWithRepeat);

		const float absNodeScale = node->absScale;
		float savedDetailSize = calcParam->detailSize;
		calcParam->detailSize = (absNodeScale > 0.0f) ? calcParam->detailSize / absNodeScale : calcParam->detailSize;

		if (node->level < stackLevel)
		{
			// pop stack levels
			while (stackLevel > node->level)
			{
				ObjectTreeStackFrameCl child = stack[stackLevel];
				stackLevel--;
				mergeChildIntoParentCl(&child, &stack[stackLevel],
					renderData->objectsData, numberOfObjects);
			}
		}

		switch (node->type)
		{
			case nodeTypeFractal:
			{
				if (numberOfFractalsToSkip == 0)
				{
					formulaOut nodeOut = CalculateDistanceSimple(consts, pointTransformed,
						calcParam, renderData, node->hybridSequenceIndex);
					distance = nodeOut.distance * absNodeScale;
					objectId = node->internalObjectId;
					sequenceIndex = node->hybridSequenceIndex;
					out.iters = nodeOut.iters;
					out.colorIndex = nodeOut.colorIndex;
					out.maxiter = nodeOut.maxiter;
					out.z = nodeOut.z;
				}
				else
				{
					numberOfFractalsToSkip--;
					calcParam->detailSize = savedDetailSize;
					continue;
				}
				break;
			}
			case nodeTypePrimitive:
			{
#ifdef USE_PRIMITIVES
				int primIdx = node->primitiveIdx;
				if (primIdx >= 0 && primIdx < renderData->numberOfPrimitives)
				{
					__global sPrimitiveCl *primitive = &renderData->primitives[primIdx];
					distance = PrimitiveDistanceByType(primitive, pointTransformed, 1e20f);
					distance *= absNodeScale;
					objectId = node->internalObjectId;
				}
#endif
				break;
			}
			case nodeTypeHybrid:
			{
				formulaOut nodeOut = CalculateDistanceSimple(consts, pointTransformed,
					calcParam, renderData, node->hybridSequenceIndex);
				distance = nodeOut.distance * absNodeScale;
				objectId = node->internalObjectId;
				sequenceIndex = node->hybridSequenceIndex;
				out.iters = nodeOut.iters;
				out.colorIndex = nodeOut.colorIndex;
				out.maxiter = nodeOut.maxiter;
				out.z = nodeOut.z;
				// skip next fractals because they are part of this hybrid sequence
				// numberOfFractalsToSkip is set based on sequence data
				// For now hybrid sequences are handled via the Fractal() function
				break;
			}
			case nodeTypeBooleanAdd:
			case nodeTypeBooleanMul:
			case nodeTypeBooleanSub:
			{
				stackLevel++;
				if (stackLevel < MAX_TREE_LEVELS)
				{
					stack[stackLevel].cumulativeDistance =
						(node->type == nodeTypeBooleanMul) ? -1e20f : 1e20f;
					stack[stackLevel].level = stackLevel;
					stack[stackLevel].nodeType = node->type;
					stack[stackLevel].closestObjectId = -1;
					stack[stackLevel].closestObjectSequence = -1;
					stack[stackLevel].transformedPoint = point;
					stack[stackLevel].hasTransformedPoint = false;
				}
				calcParam->detailSize = savedDetailSize;
				continue;
			}
			default: break;
		}

		calcParam->detailSize = savedDetailSize;

		ObjectTreeStackFrameCl leaf;
		leaf.cumulativeDistance = distance;
		leaf.closestObjectId = objectId;
		leaf.closestObjectSequence = sequenceIndex;
		leaf.transformedPoint = pointTransformed;
		leaf.hasTransformedPoint = (objectId >= 0);
		mergeChildIntoParentCl(&leaf, &stack[stackLevel],
			renderData->objectsData, numberOfObjects);
	}

	// final node summation - pop remaining stack levels
	while (stackLevel > 0)
	{
		ObjectTreeStackFrameCl child = stack[stackLevel];
		stackLevel--;
		mergeChildIntoParentCl(&child, &stack[stackLevel],
			renderData->objectsData, numberOfObjects);
	}

	out.distance = stack[0].cumulativeDistance;
	out.objectId = stack[0].closestObjectId;

#ifdef LIMITS_ENABLED
	if (limitBoxDist < calcParam->detailSize)
	{
		out.distance = max(out.distance, limitBoxDist);
	}
#endif

	float distFromCamera = length(point - consts->params.camera);
	float distanceLimitMin = consts->params.viewDistanceMin - distFromCamera;
	out.distance = max(out.distance, distanceLimitMin);

	if (distanceLimitMin > calcParam->detailSize)
	{
		out.maxiter = false;
		out.iters = 0;
	}

	return out;
}

#endif // MANDELBULBER2_OPENCL_ENGINES_CALCULATE_DISTANCE_CL_
