/*
 * orbit_trap_shape.cpp
 *
 *  Created on: 21 kwi 2019
 *      Author: krzysztof
 */

#include "orbit_trap_shape.hpp"

double OrbitTrapShapeDistance(CVector4 z4, const sCommonParams &par)
{
	double dist = 0.0;
	CVector3 z = z4.GetXYZ();
	CVector3 delta = z - par.fakeLightsOrbitTrap;

	switch (par.fakeLightsOrbitTrapShape)
	{
		case params::fakeLightsShapePoint:
		{
			dist = delta.Length();
			break;
		}
		case params::fakeLightsShapeLine:
		{
			delta = par.mRotFakeLightsRotation.RotateVector(delta);
			dist = sqrt(delta.y * delta.y + delta.z * delta.z);
			break;
		}
		case params::fakeLightsShapeCircle:
		{
			delta = par.mRotFakeLightsRotation.RotateVector(delta);
			double lengthYZ = sqrt(delta.y * delta.y + delta.z * delta.z) - par.fakeLightsOrbitTrapSize;
			dist = sqrt(lengthYZ * lengthYZ + delta.x * delta.x);
			break;
		}
	}

	return dist;
}
