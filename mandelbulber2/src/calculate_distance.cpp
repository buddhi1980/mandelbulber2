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
#include "displacement_map.hpp"
#include "fractal.h"
#include "fractal_enums.h"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "nine_fractals.hpp"
#include "object_node_type.h"
#include "perlin_noise_octaves.h"
#include "render_data.hpp"
#include "shader_perlin_noise_for_shaders.hpp"
#include "texture_mapping.hpp"
#include "write_log.hpp"

using namespace std;

double CalculateDistance(const sParamRender &params, const cNineFractals &fractals,
	const sDistanceIn &in, sDistanceOut *out, sRenderData *data)
{
	if (params.objectsTreeEnable)
	{
		return CalculateDistanceFromObjectsTree(params, fractals, in, out, data);
	}

	double distance;
	out->objectId = 0;
	out->totalIters = 0;

	double limitBoxDist = 0.0;
	if (params.limitsEnabled)
	{
		const double distance_a =
			max(in.point.x - params.limitMax.x, -(in.point.x - params.limitMin.x));
		const double distance_b =
			max(in.point.y - params.limitMax.y, -(in.point.y - params.limitMin.y));
		const double distance_c =
			max(in.point.z - params.limitMax.z, -(in.point.z - params.limitMin.z));
		limitBoxDist = max(max(distance_a, distance_b), distance_c);

		if (limitBoxDist > in.detailSize)
		{
			out->maxiter = false;
			out->distance = limitBoxDist;
			out->objectId = 0;
			out->iters = 0;
			return limitBoxDist;
		}
	}

	if (params.booleanOperatorsEnabled)
	{
		sDistanceIn inTemp = in;
		CVector3 point = inTemp.point;

		point = point - params.formulaPosition[0];
		point = params.mRotFormulaRotation[0].RotateVector(point);
		point = point.repeatMod(params.formulaRepeat[0]);
		point *= params.formulaScale[0];
		inTemp.point = point;

		distance =
			CalculateDistanceSimple(params, fractals, inTemp, out, 0, nullptr) / params.formulaScale[0];

		CVector3 pointFractalized = inTemp.point;
		double reduceDisplacement = 1.0;
		pointFractalized = FractalizeTexture(
			inTemp.point, data, params, out->objectId, out->seqIndex, &reduceDisplacement);

		distance = DisplacementMap(distance, pointFractalized, 0, data, reduceDisplacement);

		for (int i = 0; i < NUMBER_OF_FRACTALS - 1; i++)
		{
			if (fractals.GetFractal(i + 1)->formula != fractal::none)
			{
				sDistanceOut outTemp = *out;

				point = in.point - params.formulaPosition[i + 1];
				point = params.mRotFormulaRotation[i + 1].RotateVector(point);
				point = point.repeatMod(params.formulaRepeat[i + 1]);
				point *= params.formulaScale[i + 1];
				inTemp.point = point;

				double distTemp =
					CalculateDistanceSimple(params, fractals, inTemp, &outTemp, i + 1, nullptr)
					/ params.formulaScale[i + 1];

				CVector3 pointFractalized = inTemp.point;
				double reduceDisplacement = 1.0;
				pointFractalized =
					FractalizeTexture(inTemp.point, data, params, i + 1, out->seqIndex, &reduceDisplacement);

				distTemp = DisplacementMap(distTemp, pointFractalized, i + 1, data);
				distance = PerlinNoiseDisplacement(distance, pointFractalized, data, i + 1);

				const params::enumBooleanOperator boolOperator = params.booleanOperator[i];

				switch (boolOperator)
				{
					case params::booleanOperatorOR:
						if (distTemp < distance)
						{
							outTemp.objectId = 1 + i;
							*out = outTemp;
						}

						if (params.smoothDeCombineEnable[i + 1])
						{
							distance = opSmoothUnion(distTemp, distance, params.smoothDeCombineDistance[i + 1]);
						}
						else
						{
							distance = min(distTemp, distance);
						}

						break;
					case params::booleanOperatorAND:
						if (distTemp > distance)
						{
							outTemp.objectId = 1 + i;
							*out = outTemp;
						}
						distance = max(distTemp, distance);
						break;
					case params::booleanOperatorSUB:
					{
						const double limit = 1.5;
						if (distance < in.detailSize) // if inside 1st
						{
							if (distTemp < in.detailSize * limit * 1.5)
							{
								outTemp.objectId = 1 + i;
								*out = outTemp;
							}

							if (distTemp < in.detailSize * limit) // if inside 2nd
							{
								if (in.normalCalculationMode)
								{
									distance = max(in.detailSize * limit - distTemp, distance);
								}
								else
								{
									distance = in.detailSize * limit;
								}
							}
							else // if outside of 2nd
							{
								distance = max(in.detailSize * limit - distTemp, distance);
								if (distance < 0) distance = 0;
							}
						}
						else // if outside of 1st
						{
							//
						}
						break;
					}
					default: break;
				}
			}
		}
	}
	else
	{
		distance = CalculateDistanceSimple(params, fractals, in, out, -1, nullptr);

		CVector3 pointFractalized = in.point;
		double reduceDisplacement = 1.0;

		pointFractalized = FractalizeTexture(in.point, data, params, -1, 0, &reduceDisplacement);

		distance = DisplacementMap(distance, pointFractalized, 0, data, reduceDisplacement);
		distance = PerlinNoiseDisplacement(distance, in.point, data, 0);
	}

	distance = params.primitives.TotalDistance(
		in.point, distance, in.detailSize, in.normalCalculationMode, &out->objectId, data, -1);

	//****************************************************

	if (params.limitsEnabled)
	{
		if (limitBoxDist < in.detailSize)
		{
			distance = max(distance, limitBoxDist);
		}
	}

	if (CheckNAN(distance)) // check if not a number
	{
		distance = 0.0;
	}

	const double distFromCamera = (in.point - params.camera).Length();
	const double distanceLimitMin = params.viewDistanceMin - distFromCamera;
	if (distanceLimitMin > in.detailSize)
	{
		out->maxiter = false;
		out->objectId = 0;
		out->iters = 0;
	}

	distance = max(distance, distanceLimitMin);

	out->distance = distance;

	return distance;
}

// In mandelbulber2/src/calculate_distance.cpp

double CalculateDistanceFromObjectsTree(const sParamRender &params, const cNineFractals &fractals,
	const sDistanceIn &in, sDistanceOut *out, sRenderData *data)
{
	struct StackFrame
	{
		int nodeIdx;
		int level;
		int closestObjectId;
		int closestObjectSequence;
		double cumulativeDistance;
		enumNodeType nodeType;
	};

	// limit to 10 levels of tree
	StackFrame stack[10];

	out->objectId = 0;
	out->totalIters = 0;
	out->maxiter = false;
	out->seqIndex = 0;

	if (data)
	{
		const auto &nodes = data->nodesDataForRendering;
		const auto &primitives = params.primitives.allPrimitives;
		const int nodeCount = nodes.size();

		stack[0].cumulativeDistance = 1e20;
		stack[0].nodeIdx = -1;
		stack[0].level = 0;
		stack[0].closestObjectId = -1;
		stack[0].closestObjectSequence = -1;
		stack[0].nodeType = enumNodeType::booleanAdd;

		int stackLevel = 0;
		int numberOfFractalsToSkip = 0;

		for (int i = 0; i < nodeCount; ++i)
		{
			const auto &node = nodes[i];
			double distance = 1e20;

			if (node.level < stackLevel)
			{
				// pop stack levels
				while (stackLevel > node.level)
				{
					// combine distances
					double childDistance = stack[stackLevel].cumulativeDistance;
					int closestObjectIdOfChild = stack[stackLevel].closestObjectId;
					int closestObjectSequenceOfChild = stack[stackLevel].closestObjectSequence;

					stackLevel--;

					// update cumulative distance of the previous level
					if (stack[stackLevel].nodeType == enumNodeType::booleanAdd)
					{
						if (childDistance < stack[stackLevel].cumulativeDistance)
						{
							stack[stackLevel].cumulativeDistance = childDistance;
							stack[stackLevel].closestObjectId = closestObjectIdOfChild;
							stack[stackLevel].closestObjectSequence = closestObjectSequenceOfChild;
						}
					}
				}
			}

			switch (node.type)
			{
				case enumNodeType::fractal:
				{
					if (numberOfFractalsToSkip == 0)
					{
						sDistanceOut nodeOut;
						distance = CalculateDistanceSimple(
							params, fractals, in, &nodeOut, node.internalObjectId, nullptr);
					}
					else
					{
						// skipping fractal because it is part of hybrid sequence
						numberOfFractalsToSkip--;
					}
					break;
				}
				case enumNodeType::primitive:
				{
					int primIdx = node.primitiveIdx;
					if (primIdx >= 0 && primIdx < (int)primitives.size() && primitives[primIdx])
						distance = primitives[primIdx]->PrimitiveDistance(in.point);
					break;
				}
				case enumNodeType::hybrid:
				{
					// hybrid fractal sequence
					int seqIndex = node.hybridSequenceIndex;
					sDistanceOut nodeOut;
					distance = CalculateDistanceSimple(
						params, fractals, in, &nodeOut, -1, data->hybridFractalSequences.GetSequence(seqIndex));
					// skip next fractals because they are part of this hybrid sequence
					numberOfFractalsToSkip =
						data->hybridFractalSequences.GetSequence(seqIndex)->numberOfFractalsInTheSequence;
					stack[stackLevel].closestObjectSequence = seqIndex;
					stack[stackLevel].closestObjectId = node.userObjectId;
					break;
				}
				case enumNodeType::booleanAdd:
				{
					stackLevel++;
					stack[stackLevel].cumulativeDistance = 1e20;
					stack[stackLevel].nodeIdx = i;
					stack[stackLevel].level = stackLevel;
					stack[stackLevel].nodeType = enumNodeType::booleanAdd;
					continue;
				}

				default: break;
			}

			if (stack[stackLevel].nodeType == enumNodeType::booleanAdd)
			{
				if (distance < stack[stackLevel].cumulativeDistance)
				{
					stack[stackLevel].cumulativeDistance = distance;
					stack[stackLevel].closestObjectId = node.internalObjectId;
				}
			}
		}

		// final node summation
		if (stackLevel > 0)
		{
			// pop stack levels
			while (stackLevel > 0)
			{
				// combine distances
				double childDistance = stack[stackLevel].cumulativeDistance;
				int closestObjectIdOfChild = stack[stackLevel].closestObjectId;
				int closestObjectSequenceOfChild = stack[stackLevel].closestObjectSequence;

				stackLevel--;

				// update cumulative distance of the previous level
				if (stack[stackLevel].nodeType == enumNodeType::booleanAdd)
				{
					if (childDistance < stack[stackLevel].cumulativeDistance)
					{
						stack[stackLevel].cumulativeDistance = childDistance;
						stack[stackLevel].closestObjectId = closestObjectIdOfChild;
						stack[stackLevel].closestObjectSequence = closestObjectSequenceOfChild;
					}
				}
			}
		}

		out->distance = stack[0].cumulativeDistance;
		out->objectId = stack[0].closestObjectId;
		out->seqIndex = stack[0].closestObjectSequence;
		return out->distance;
	}
	return 0;
}

double CalculateDistanceSimple(const sParamRender &params, const cNineFractals &fractals,
	const sDistanceIn &in, sDistanceOut *out, int forcedFormulaIndex,
	const cHybridFractalSequences::sSequence *sequence)
{
	double distance = 0;

	const int maxiterMultiplier = (in.normalCalculationMode && params.common.iterThreshMode) ? 5 : 1;

	sFractalIn fractIn(in.point, params.minN, -1, maxiterMultiplier, 0, &params.common,
		forcedFormulaIndex, in.normalCalculationMode);
	sFractalOut fractOut;
	fractOut.colorIndex = 0;

	if (fractals.GetDEType(forcedFormulaIndex) == fractal::analyticDEType)
	{
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
		double deltaDE;
		if (params.advancedQuality)
		{
			deltaDE = max(fractIn.point.Length() * 1e-14, params.deltaDERelativeDelta * in.detailSize);
		}
		else
		{
			deltaDE = max(fractIn.point.Length() * 1e-14, 1e-5 * in.detailSize);
		}

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

		fractIn.forcedMaxiter =
			fractOut.iters; // for other directions must be the same number of iterations

		fractIn.point = in.point + CVector3(deltaDE, 0.0, 0.0);
		Compute<fractal::calcModeDeltaDE1>(sequence, fractIn, &fractOut);
		double r2 = fractOut.z.Length();
		const double dr1 = fabs(r2 - r) / deltaDE;
		out->totalIters += fractOut.iters;

		fractIn.point = in.point + CVector3(0.0, deltaDE, 0.0);
		Compute<fractal::calcModeDeltaDE1>(sequence, fractIn, &fractOut);
		r2 = fractOut.z.Length();
		const double dr2 = fabs(r2 - r) / deltaDE;
		out->totalIters += fractOut.iters;

		fractIn.point = in.point + CVector3(0.0, 0.0, deltaDE);
		Compute<fractal::calcModeDeltaDE1>(sequence, fractIn, &fractOut);
		r2 = fractOut.z.Length();
		const double dr3 = fabs(r2 - r) / deltaDE;
		out->totalIters += fractOut.iters;

		const double dr = sqrt(dr1 * dr1 + dr2 * dr2 + dr3 * dr3);

		if (dr > 0)
		{
			// DE functions for deltaDE
			if (fractals.GetDEFunctionType(forcedFormulaIndex) == fractal::linearDEFunction)
				distance = 0.5 * r / dr;
			else if (fractals.GetDEFunctionType(forcedFormulaIndex) == fractal::logarithmicDEFunction)
				distance = 0.5 * r * log(r) / dr;
			else if (fractals.GetDEFunctionType(forcedFormulaIndex) == fractal::pseudoKleinianDEFunction)
			{
				const CVector3 z = fractOut.z;
				const double rxy = sqrt(z.x * z.x + z.y * z.y);
				distance = max(rxy - 0.92784, fabs(rxy * z.z) / r) / (dr);
			}
			else if (fractals.GetDEFunctionType(forcedFormulaIndex) == fractal::josKleinianDEFunction)
			{
				const CVector3 z = fractOut.z;
				const double rxy = sqrt(z.x * z.x + z.z * z.z);
				distance = (fabs(rxy * z.y) / r) / (dr);
				maxiter = false;
			}
			else if (fractals.GetDEFunctionType(forcedFormulaIndex) == fractal::customDEFunction)
			{
				distance = r; // FIXME: Can we calculate dIFS in deltaDE mode ???
			}
			else if (fractals.GetDEFunctionType(forcedFormulaIndex) == fractal::maxAxisDEFunction)
			{
				CVector3 absZ = fabs(zFromIters);
				double maxZ = dMax(absZ.x, absZ.y, absZ.z);
				double maxDr = dMax(fabs(dr1), fabs(dr2), fabs(dr3));
				distance = 0.5 * maxZ / maxDr;
			}
		}
		else
		{
			distance = r;
		}

		// if (distance < 1e-20) distance = 1e-20;

		// don't use maxiter when limits are disabled and iterThresh mode is not used
		if (!params.common.iterThreshMode) out->maxiter = false;

		// never use maxiter if normal vectors are calculated
		if (in.normalCalculationMode) out->maxiter = false;

		if (out->maxiter) distance = 0.0;

		if (fractOut.iters < params.minN && distance < in.detailSize) distance = in.detailSize;

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

double CalculateDistanceMinPlane(std::shared_ptr<const sParamRender> params,
	std::shared_ptr<const cNineFractals> fractals, const CVector3 planePoint,
	const CVector3 direction, const CVector3 orthDirection, bool *stopRequest)
{
	// the plane is defined by the 'planePoint' and the orthogogonal 'direction'
	// the method will return the minimum distance from the plane to the fractal
	double distStep = 0.0;
	CVector3 point = planePoint;
	const double detail = 0.5;
	const int transVectorAngles = 5;

	CVector3 rotationAxis = planePoint;
	rotationAxis.Normalize();

	while (distStep == 0 || distStep > 0.00001)
	{
		CVector3 pointNextBest(0, 0, 0);
		double newDistStepMin = 0;
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
	return CVector3(point - planePoint).Dot(direction);
}
