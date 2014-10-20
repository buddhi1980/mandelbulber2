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

	int N = in.normalCalculationMode ? params.N * 5 : params.N;
	sFractalIn fractIn(in.point, params.minN, N, params.common);
	sFractalOut fractOut;

	if(true) //TODO !params.primitives.plane.onlyPlane
	{
		if (four.DEType == fractal::analitycDE)
		{

			Compute<fractal::normal>(four, fractIn, &fractOut);
			distance = fractOut.distance;
			out->maxiter = fractOut.maxiter;
			out->iters = fractOut.iters;
			out->colorIndex = fractOut.colorIndex;

			//---------------- 3587.7 ns for Compute -----------
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
			else if(params.interiorMode && in.normalCalculationMode)
			{
				if (distance < 0.9 * in.detailSize)
				{
					distance = in.detailSize - distance;
					out->maxiter = false;;
				}
			}

			if (params.iterThreshMode && !fractOut.maxiter)
			{
				if(distance < in.detailSize)
				{
					distance = in.detailSize * 1.01;
				}
			}
			// ---------- 75 ns for rest of calculation -------------
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
			else if(params.interiorMode && in.normalCalculationMode)
			{
				if (distance < 0.9 * in.detailSize)
				{
					distance = in.detailSize - distance;
					out->maxiter = false;
				}
			}

			if (params.iterThreshMode && !maxiter)
			{
				if(distance < in.detailSize)
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

	/*
	//plane
	if (params.primitives.plane.enable)
	{
		double planeDistance = PrimitivePlane(in.point, params.primitives.plane.centre, params.primitives.plane.normal);
		if(!params.primitives.plane.onlyPlane && planeDistance < distance) 	out->object = fractal::objPlane;
		distance = (planeDistance < distance) ? planeDistance : distance;

	}

	//box
	if (params.primitives.box.enable)
	{
		double boxDistance = PrimitiveBox(in.point, params.primitives.box.centre, params.primitives.box.size);
		if(boxDistance < distance) 	out->object = fractal::objBox;
		distance = (boxDistance < distance) ? boxDistance : distance;
	}

	//inverted box
	if (params.primitives.invertedBox.enable)
	{
		double boxDistance = PrimitiveInvertedBox(in.point, params.primitives.invertedBox.centre, params.primitives.invertedBox.size);
		if(boxDistance < distance) 	out->object = fractal::objBoxInv;
		distance = (boxDistance < distance) ? boxDistance : distance;
	}

	//sphere
	if (params.primitives.sphere.enable)
	{
		double sphereDistance = PrimitiveSphere(in.point, params.primitives.sphere.centre, params.primitives.sphere.radius);
		if(sphereDistance < distance) 	out->object = fractal::objSphere;
		distance = (sphereDistance < distance) ? sphereDistance : distance;
	}

	//invertedSphere
	if (params.primitives.invertedSphere.enable)
	{
		double sphereDistance = PrimitiveInvertedSphere(in.point, params.primitives.invertedSphere.centre, params.primitives.invertedSphere.radius);
		if(sphereDistance < distance) out->object = fractal::objSphereInv;
		distance = (sphereDistance < distance) ? sphereDistance : distance;
	}

	//water
	if (params.primitives.water.enable)
	{
		double waterDistance = PrimitiveWater(in.point, params.primitives.water.level, params.primitives.water.amplitude,
				params.primitives.water.length, params.primitives.water.rotation, params.primitives.water.iterations, params.primitives.water.animSpeed, params.frameNo);
		if(waterDistance < distance) 	out->object = fractal::objWater;
		distance = (waterDistance < distance) ? waterDistance : distance;
	}
	*/

	distance = min(distance, params.primitives.TotalDistance(in.point, distance, &out->object, &out->objectColor, &out->objectReflect));

	if (params.limitsEnabled)
	{
		if (limitBoxDist < in.detailSize)
		{
			distance = max(distance, limitBoxDist);
		}
	}

	out->distance = distance;

	//-------------- 3936.54 ns for distance calculation -------------
	return distance;
}

