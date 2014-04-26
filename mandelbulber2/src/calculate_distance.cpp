/*
 * calculate_distance.cpp
 *
 *  Created on: Apr 26, 2014
 *      Author: krzysztof
 */


//******************* Calculate distance *******************8

/*
double CalculateDistance(CVector3 point, sFractal &params, bool *max_iter)
{
	int L;
	double distance;
	params.objectOut = fractal::objFractal;

	double limitBoxDist = 0.0;

	if (params.limits_enabled)
	{
		double distance_a = MAX(point.x - params.amax, -(point.x - params.amin));
		double distance_b = MAX(point.y - params.bmax, -(point.y - params.bmin));
		double distance_c = MAX(point.z - params.cmax, -(point.z - params.cmin));
		limitBoxDist = dMax(distance_a, distance_b, distance_c);

		if(limitBoxDist > params.detailSize)
		{
			if (max_iter != NULL)
					*max_iter = false;
			return limitBoxDist;
		}
	}

	if(!params.primitives.onlyPlane)
	{
		if (params.analitycDE)
		{
			distance = Compute<fractal::normal>(point, params, &L);
			if (max_iter != NULL)
			{
				if (L == (int)params.N) *max_iter = true;
				else *max_iter = false;
			}
			params.itersOut = L;
			if (distance < 0) distance = 0;

			if (L < params.minN && distance < params.detailSize) distance = params.detailSize;

			if (params.interiorMode && !params.normalCalculationMode)
			{
				if (distance < 0.5 * params.detailSize || L == (int)params.N)
				{
					distance = params.detailSize;
					if (max_iter != NULL) *max_iter = false;
				}
			}
			else if(params.interiorMode && params.normalCalculationMode)
			{
				if (distance < 0.9 * params.detailSize)
				{
					distance = params.detailSize - distance;
					if (max_iter != NULL) *max_iter = false;
				}
			}

			if (params.iterThresh && L < (int)params.N)
			{
				if(distance < params.detailSize)
				{
					distance = params.detailSize * 1.01;
				}
			}
		}
		else
		{
			double deltaDE = 1e-10;

			double r = Compute<fractal::deltaDE1>(point, params, &L);
			int retval = L;
			params.itersOut = L;

			point.x += deltaDE;
			point.y += 0;
			point.z += 0;
			double r2 = Compute<fractal::deltaDE2>(point, params, &L);
			double dr1 = fabs(r2 - r) / deltaDE;

			point.x -= deltaDE;
			point.y += deltaDE;
			point.z += 0;
			r2 = Compute<fractal::deltaDE2>(point, params, &L);
			double dr2 = fabs(r2 - r) / deltaDE;

			point.x += 0;
			point.y -= deltaDE;
			point.z += deltaDE;
			r2 = Compute<fractal::deltaDE2>(point, params, &L);
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

			if (retval == (int)params.N)
			{
				if (max_iter != NULL) *max_iter = true;
				distance = 0;
			}
			else if (max_iter != NULL) *max_iter = false;

			if (L < params.minN && distance < params.detailSize) distance = params.detailSize;

			if (params.interiorMode && !params.normalCalculationMode)
			{
				if (distance < 0.5 * params.detailSize || L == (int)params.N)
				{
					distance = params.detailSize;
					if (max_iter != NULL) *max_iter = false;
				}
			}
			else if(params.interiorMode && params.normalCalculationMode)
			{
				if (distance < 0.9 * params.detailSize)
				{
					distance = params.detailSize - distance;
					if (max_iter != NULL) *max_iter = false;
				}
			}

			if (params.iterThresh && retval < (int)params.N)
			{
				if(distance < params.detailSize)
				{
					distance = params.detailSize * 1.01;
				}
			}

		}
	}
	else
	{
		distance = 10.0;
		if (max_iter != NULL) *max_iter = false;
	}

	//plane
	if (params.primitives.planeEnable)
	{
		double planeDistance = PrimitivePlane(point, params.primitives.planeCentre, params.primitives.planeNormal);
		if(!params.primitives.onlyPlane && planeDistance < distance) 	params.objectOut = fractal::objPlane;
		distance = (planeDistance < distance) ? planeDistance : distance;

	}

	//box
	if (params.primitives.boxEnable)
	{
		double boxDistance = PrimitiveBox(point, params.primitives.boxCentre, params.primitives.boxSize);
		if(boxDistance < distance) 	params.objectOut = fractal::objBox;
		distance = (boxDistance < distance) ? boxDistance : distance;
	}

	//inverted box
	if (params.primitives.invertedBoxEnable)
	{
		double boxDistance = PrimitiveInvertedBox(point, params.primitives.invertedBoxCentre, params.primitives.invertedBoxSize);
		if(boxDistance < distance) 	params.objectOut = fractal::objBoxInv;
		distance = (boxDistance < distance) ? boxDistance : distance;
	}

	//sphere
	if (params.primitives.sphereEnable)
	{
		double sphereDistance = PrimitiveSphere(point, params.primitives.sphereCentre, params.primitives.sphereRadius);
		if(sphereDistance < distance) 	params.objectOut = fractal::objSphere;
		distance = (sphereDistance < distance) ? sphereDistance : distance;
	}

	//invertedSphere
	if (params.primitives.invertedSphereEnable)
	{
		double sphereDistance = PrimitiveInvertedSphere(point, params.primitives.invertedSphereCentre, params.primitives.invertedSphereRadius);
		if(sphereDistance < distance) 	params.objectOut = fractal::objSphereInv;
		distance = (sphereDistance < distance) ? sphereDistance : distance;
	}

	//water
	if (params.primitives.waterEnable)
	{
		double waterDistance = PrimitiveWater(point, params.primitives.waterHeight, params.primitives.waterAmplitude,
				params.primitives.waterLength, params.primitives.waterRotation, params.primitives.waterIterations, params.primitives.waterAnimSpeed, params.frameNo);
		if(waterDistance < distance) 	params.objectOut = fractal::objWater;
		distance = (waterDistance < distance) ? waterDistance : distance;
	}


	if (params.limits_enabled)
	{
		if (limitBoxDist < params.detailSize)
		{
			distance = MAX(distance, limitBoxDist);
		}
	}

	return distance;
}

*/
