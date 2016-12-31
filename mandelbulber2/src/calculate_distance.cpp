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
 * CalculateDistanceSimple() function calculates approximate distance to
 * the fractal surface using fractal computation functions.
 *
 * CalculateDistance() function calculates resultant distance to all
 * objects on scene including boolean operators.
 */

#include "calculate_distance.hpp"

#include "compute_fractal.hpp"
#include "displacement_map.hpp"
#include "fractal_enums.h"
#include "fractal.h"
#include "fractparams.hpp"
#include "nine_fractals.hpp"
#include "render_data.hpp"
#include "texture_mapping.hpp"
#include "global_data.hpp"

using namespace std;

double CalculateDistance(const cParamRender &params, const cNineFractals &fractals,
	const sDistanceIn &in, sDistanceOut *out, sRenderData *data)
{
	double distance;
	out->objectId = 0;
	out->totalIters = 0;

	double limitBoxDist = 0.0;

	if (params.limitsEnabled)
	{
		double distance_a = max(in.point.x - params.limitMax.x, -(in.point.x - params.limitMin.x));
		double distance_b = max(in.point.y - params.limitMax.y, -(in.point.y - params.limitMin.y));
		double distance_c = max(in.point.z - params.limitMax.z, -(in.point.z - params.limitMin.z));
		limitBoxDist = max(max(distance_a, distance_b), distance_c);

		if (limitBoxDist > in.detailSize)
		{
			out->maxiter = false;
			out->distance = limitBoxDist;
			out->objectId = 0;
			out->maxiter = false;
			out->iters = 0;
			return limitBoxDist;
		}
	}

	if (params.booleanOperatorsEnabled)
	{
		sDistanceIn inTemp = in;
		CVector3 point = inTemp.point;

		point = point.mod(params.formulaRepeat[0]) - params.formulaPosition[0];
		point = params.mRotFormulaRotation[0].RotateVector(point);
		point *= params.formulaScale[0];
		inTemp.point = point;

		distance = CalculateDistanceSimple(params, fractals, inTemp, out, 0) / params.formulaScale[0];

		distance = DisplacementMap(distance, in.point, 0, data);

		for (int i = 0; i < NUMBER_OF_FRACTALS - 1; i++)
		{
			if (fractals.GetFractal(i + 1)->formula != fractal::none)
			{
				sDistanceOut outTemp = *out;

				point = in.point;
				point = point.mod(params.formulaRepeat[i + 1]) - params.formulaPosition[i + 1];
				point = params.mRotFormulaRotation[i + 1].RotateVector(point);
				point *= params.formulaScale[i + 1];
				inTemp.point = point;

				double distTemp = CalculateDistanceSimple(params, fractals, inTemp, &outTemp, i + 1)
													/ params.formulaScale[i + 1];

				distTemp = DisplacementMap(distTemp, in.point, i + 1, data);

				params::enumBooleanOperator boolOperator = params.booleanOperator[i];

				switch (boolOperator)
				{
					case params::booleanOperatorOR:
						if (distTemp < distance)
						{
							outTemp.objectId = 1 + i;
							*out = outTemp;
						}
						distance = min(distTemp, distance);
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
						double limit = 1.5;
						if (distance < in.detailSize) // if inside 1st
						{
							if (distTemp < in.detailSize * limit) // if inside 2nd
							{
								if (in.normalCalculationMode)
								{
									distance = in.detailSize * limit - distTemp;
								}
								else
								{
									distance = in.detailSize * limit;
								}
							}
							else // if outside of 2nd
							{
								if (in.detailSize * limit - distTemp > distance)
								{
									outTemp.objectId = 1 + i;
									*out = outTemp;
								}

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
		distance = CalculateDistanceSimple(params, fractals, in, out, -1);
		distance = DisplacementMap(distance, in.point, 0, data);
	}

	distance =
		min(distance, params.primitives.TotalDistance(in.point, distance, &out->objectId, data));

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

	out->distance = distance;

	return distance;
}

double CalculateDistanceSimple(const cParamRender &params, const cNineFractals &fractals,
	const sDistanceIn &in, sDistanceOut *out, int forcedFormulaIndex)
{
	double distance = 0;

	int N = in.normalCalculationMode ? params.N * 5 : params.N;

	sFractalIn fractIn(in.point, params.minN, N, params.common, forcedFormulaIndex);
	sFractalOut fractOut;
	fractOut.colorIndex = 0;

	if (fractals.GetDEType(forcedFormulaIndex) == fractal::analyticDEType)
	{
		Compute<fractal::calcModeNormal>(fractals, fractIn, &fractOut);
		distance = fractOut.distance;
		// qDebug() << "computed distance" << distance;
		out->maxiter = fractOut.maxiter;
		out->iters = fractOut.iters;
		out->colorIndex = fractOut.colorIndex;
		out->totalIters += fractOut.iters;

		// if (distance < 1e-20) distance = 1e-20;

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
	}
	else
	{
		double deltaDE = 1e-10;

		Compute<fractal::calcModeDeltaDE1>(fractals, fractIn, &fractOut);
		double r = fractOut.z.Length();
		bool maxiter = out->maxiter = fractOut.maxiter;
		out->iters = fractOut.iters;
		out->colorIndex = fractOut.colorIndex;
		out->totalIters += fractOut.iters;

		fractIn.maxN = fractOut.iters; // for other directions must be the same number of iterations

		fractIn.point = in.point + CVector3(deltaDE, 0.0, 0.0);
		Compute<fractal::calcModeDeltaDE1>(fractals, fractIn, &fractOut);
		double r2 = fractOut.z.Length();
		double dr1 = fabs(r2 - r) / deltaDE;
		out->totalIters += fractOut.iters;

		fractIn.point = in.point + CVector3(0.0, deltaDE, 0.0);
		Compute<fractal::calcModeDeltaDE1>(fractals, fractIn, &fractOut);
		r2 = fractOut.z.Length();
		double dr2 = fabs(r2 - r) / deltaDE;
		out->totalIters += fractOut.iters;

		fractIn.point = in.point + CVector3(0.0, 0.0, deltaDE);
		Compute<fractal::calcModeDeltaDE1>(fractals, fractIn, &fractOut);
		r2 = fractOut.z.Length();
		double dr3 = fabs(r2 - r) / deltaDE;
		out->totalIters += fractOut.iters;

		double dr = sqrt(dr1 * dr1 + dr2 * dr2 + dr3 * dr3);

		if (dr > 0)
		{
			// DE functions for deltaDE
			if (fractals.GetDEFunctionType(forcedFormulaIndex) == fractal::linearDEFunction)
				distance = 0.5 * r / dr;
			else if (fractals.GetDEFunctionType(forcedFormulaIndex) == fractal::logarithmicDEFunction)
				distance = 0.5 * r * log(r) / dr;
			else if (fractals.GetDEFunctionType(forcedFormulaIndex) == fractal::pseudoKleinianDEFunction)
			{
				CVector3 z = fractOut.z;
				double rxy = sqrt(z.x * z.x + z.y * z.y);
				distance = max(rxy - 0.92784, fabs(rxy * z.z) / r) / (dr);
			}
		}
		else
		{
			distance = r;
		}

		// if (distance < 1e-20) distance = 1e-20;

		if (maxiter)
		{
			distance = 0;
		}

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
	}

	return distance;
}

double CalculateDistanceMinPlane(const cParamRender &params, const cNineFractals &fractals,
	const CVector3 planePoint, const CVector3 direction, const CVector3 orthDdirection,
	bool *stopRequest)
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
			double angle = (1.0 * i / transVectorAngles) * 2.0 * M_PI;
			CVector3 transversalVect = orthDdirection;
			transversalVect = transversalVect.RotateAroundVectorByAngle(rotationAxis, angle);
			transversalVect.Normalize();
			CVector3 pointNext = point + direction * distStep;
			if (i > 0) pointNext += transversalVect * distStep / 2.0;
			sDistanceIn in(pointNext, 0, false);
			sDistanceOut out;
			double dist = CalculateDistance(params, fractals, in, &out);
			double newDistStep = dist * detail * 0.5;
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
			qDebug() << "surface not found!";
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
