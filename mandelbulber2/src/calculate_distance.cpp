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
#include "render_worker.hpp"

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
			out->objectId = 0; //TODO to be checked if it can be zero (or -1)
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

		if(data && data->materials[data->objectData[0].materialId].useBumpmapTexture)
		{
			CVector2<double> textureCoordinates;
			textureCoordinates = CVector2<double>(in.point.x*1.0, in.point.y*1.0);
			sRGBfloat bump3 = data->materials[1].bumpmapTexture.Pixel(textureCoordinates);
			double bump = bump3.R;
			distance -= bump * 0.025;
			if(distance < 0.0) distance = 0.0;
		}

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
						if (distance < in.detailSize) //if inside 1st
						{
							if (distTemp < in.detailSize * limit) //if inside 2nd
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
							else //if outside of 2nd
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
						else //if outside of 1st
						{
							//
						}
						break;
					}
					default:
						break;
				}
			}
		}
	}
	else
	{
		distance = CalculateDistanceSimple(params, fractals, in, out, -1);
		distance = DisplacementMap(distance, in.point, 0, data);
	}

	distance = min(distance, params.primitives.TotalDistance(in.point, distance, &out->objectId, data));


	//****************************************************

	if (params.limitsEnabled)
	{
		if (limitBoxDist < in.detailSize)
		{
			distance = max(distance, limitBoxDist);
		}
	}

	if(distance == distance/0.0) //check if not a number
	{
		distance = 0.0;
	}

	out->distance = distance;

	return distance;
}

double CalculateDistanceSimple(const cParamRender &params, const cNineFractals &fractals,
		const sDistanceIn &in, sDistanceOut *out, int forcedFormulaIndex)
{
	double distance;

	int N = in.normalCalculationMode ? params.N * 5 : params.N;

	sFractalIn fractIn(in.point, params.minN, N, params.common, forcedFormulaIndex);
	sFractalOut fractOut;
	fractOut.colorIndex = 0;

	if (true) //TODO !params.primitives.plane.onlyPlane
	{
		if (fractals.GetDEType(forcedFormulaIndex) == fractal::analyticDEType)
		{
			Compute<fractal::calcModeNormal>(fractals, fractIn, &fractOut);
			distance = fractOut.distance;
			//qDebug() << "computed distance" << distance;
			out->maxiter = fractOut.maxiter;
			out->iters = fractOut.iters;
			out->colorIndex = fractOut.colorIndex;
			out->totalIters += fractOut.iters;

			//if (distance < 1e-20) distance = 1e-20;

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

			if (params.iterThreshMode && !in.normalCalculationMode && !fractOut.maxiter)
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

			fractIn.maxN = fractOut.iters; //for other directions must be the same number of iterations

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
				if (fractals.GetDEFunctionType(forcedFormulaIndex) == fractal::linearDEFunction)
					distance = 0.5 * r / dr;
				else
					distance = 0.5 * r * log(r) / dr;
			}
			else
			{
				distance = r;
			}

			//if (distance < 1e-20) distance = 1e-20;

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

			if (params.iterThreshMode && !in.normalCalculationMode && !maxiter)
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

double DisplacementMap(double oldDistance, CVector3 point, int objectId, sRenderData *data)
{
	double distance = oldDistance;
	if(data)
	{
		const cMaterial *mat = &data->materials[data->objectData[objectId].materialId];
		if(mat->useBumpmapTexture)
		{
			CVector2<double> textureCoordinates;
			textureCoordinates = cRenderWorker::TextureMapping(point, data->objectData[objectId], mat) + CVector2<double>(0.5, 0.5);
			sRGBfloat bump3 = mat->bumpmapTexture.Pixel(textureCoordinates);
			double bump = bump3.R;
			distance -= bump * mat->bumpmapTextureHeight;
			if(distance < 0.0) distance = 0.0;
		}
	}
	return distance;
}

