/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * CalculateDistance function - calculates approximate distance to fractal surface
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "calculate_distance.hpp"
#include <algorithm>
#include "common_math.h"

using namespace std;

double CalculateDistance(const cParamRender &params, const cFourFractals &four, const sDistanceIn &in, sDistanceOut *out)
{
	double distance;
	out->object = fractal::objFractal;

	double limitBoxDist = 0.0;

	if (params.limitsEnabled)
	{
		double distance_a = max(in.point.x - params.limitMax.x, -(in.point.x - params.limitMin.x));
		double distance_b = max(in.point.y - params.limitMax.y, -(in.point.y - params.limitMin.y));
		double distance_c = max(in.point.z - params.limitMax.z, -(in.point.z - params.limitMin.z));
		limitBoxDist = max(max(distance_a, distance_b), distance_c);

		if(limitBoxDist > in.detailSize)
		{
			out->maxiter = false;
			out->distance = limitBoxDist;
			out->object = fractal::objNone;
			out->maxiter = false;
			out->iters = 0;
			return limitBoxDist;
		}
	}

	if(params.booleanOperatorsEnabled)
	{
		//TODO coloring source depending on nearest fractal

		distance = CalculateDistanceSimple(params, four, in, out, 0);
		for(int i = 0; i < 3; i++)
		{
			if(four.GetFractal(i + 1)->formula != fractal::none)
			{
				double distTemp = CalculateDistanceSimple(params, four, in, out, i + 1);

				params::enumBooleanOperator boolOperator = params.booleanOperator[i];

				switch (boolOperator)
				{
					case params::booleanOperatorOR:
						distance = min(distTemp, distance);
						break;
					case params::booleanOperatorAND:
						distance = max(distTemp, distance);
						break;

					default:
						break;
				}
			}
		}
	}
	else
	{
		distance = CalculateDistanceSimple(params, four, in, out, -1);
	}

	distance = min(distance, params.primitives.TotalDistance(in.point, distance, &out->object, &out->objectColor, &out->objectReflect));

	if (params.limitsEnabled)
	{
		if (limitBoxDist < in.detailSize)
		{
			distance = max(distance, limitBoxDist);
		}
	}

	out->distance = distance;

	return distance;
}

double CalculateDistanceSimple(const cParamRender &params, const cFourFractals &four, const sDistanceIn &in, sDistanceOut *out, int forcedFormulaIndex)
{
	double distance;

	int N = in.normalCalculationMode ? params.N * 5 : params.N;

	sFractalIn fractIn(in.point, params.minN, N, params.common, forcedFormulaIndex);
	sFractalOut fractOut;

	if (true) //TODO !params.primitives.plane.onlyPlane
	{

		//TODO separate DE type for each fractal
		if (four.DEType == fractal::analitycDE)
		{

			Compute<fractal::normal>(four, fractIn, &fractOut);
			distance = fractOut.distance;
			out->maxiter = fractOut.maxiter;
			out->iters = fractOut.iters;
			out->colorIndex = fractOut.colorIndex;

			if (out->maxiter) distance = 0.0;

			if (distance < 0) distance = 0;

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
					;
				}
			}

			if (params.iterThreshMode && !fractOut.maxiter)
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

			Compute<fractal::deltaDE1>(four, fractIn, &fractOut);
			double r = fractOut.z.Length();
			bool maxiter = out->maxiter = fractOut.maxiter;
			out->iters = fractOut.iters;
			out->colorIndex = fractOut.colorIndex;

			fractIn.maxN = fractOut.iters; //for other directions must be the same number of iterations

			fractIn.point = in.point + CVector3(deltaDE, 0.0, 0.0);
			Compute<fractal::deltaDE1>(four, fractIn, &fractOut);
			double r2 = fractOut.z.Length();
			double dr1 = fabs(r2 - r) / deltaDE;

			fractIn.point = in.point + CVector3(0.0, deltaDE, 0.0);
			Compute<fractal::deltaDE1>(four, fractIn, &fractOut);
			r2 = fractOut.z.Length();
			double dr2 = fabs(r2 - r) / deltaDE;

			fractIn.point = in.point + CVector3(0.0, 0.0, deltaDE);
			Compute<fractal::deltaDE1>(four, fractIn, &fractOut);
			r2 = fractOut.z.Length();
			double dr3 = fabs(r2 - r) / deltaDE;

			double dr = sqrt(dr1 * dr1 + dr2 * dr2 + dr3 * dr3);

			if (params.linearDEmode)
			{
				distance = 0.5 * r / dr;
			}
			else
			{
				distance = 0.5 * r * log(r) / dr;
			}

			if (distance < 0) distance = 0;

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

			if (params.iterThreshMode && !maxiter)
			{
				if (distance < in.detailSize)
				{
					distance = in.detailSize * 1.01;
				}
			}

		}
	}
	else
	{
		distance = 10.0;
		out->maxiter = false;
	}

	return distance;
}

