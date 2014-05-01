/*
 * calculate_distance.cpp
 *
 *  Created on: Apr 26, 2014
 *      Author: krzysztof
 */

#include "calculate_distance.hpp"
#include <algorithm>

using namespace std;

double CalculateDistance(const cParamRender &params, const cFourFractals &four, const sDistanceIn &in, sDistanceOut *out)
{
	int L;
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

	sFractalIn fractIn(in.point, params.juliaC, params.minN, params.N);
	sFractalOut fractOut;

	if(!params.primitives.plane.onlyPlane)
	{
		if (four.DEType == fractal::analitycDE)
		{
			Compute<fractal::normal>(four, fractIn, &fractOut);
			distance = fractOut.distance;
			out->maxiter = fractOut.maxiter;
			out->iters = fractOut.iters;
			out->colorIndex = fractOut.colorIndex;

			if (distance < 0) distance = 0;

			if (L < params.minN && distance < in.detailSize) distance = in.detailSize;

			if (params.interiorMode && !params.normalCalculationMode)
			{
				if (distance < 0.5 * in.detailSize || fractOut.maxiter)
				{
					distance = in.detailSize;
					out->maxiter = false;
				}
			}
			else if(params.interiorMode && params.normalCalculationMode)
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

			if (L < params.minN && distance < in.detailSize) distance = in.detailSize;

			if (params.interiorMode && !params.normalCalculationMode)
			{
				if (distance < 0.5 * in.detailSize || maxiter)
				{
					distance = in.detailSize;
					out->maxiter = false;
				}
			}
			else if(params.interiorMode && params.normalCalculationMode)
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

