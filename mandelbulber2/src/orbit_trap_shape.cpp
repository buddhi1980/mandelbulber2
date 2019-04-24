/*
 * orbit_trap_shape.cpp
 *
 *  Created on: 21 kwi 2019
 *      Author: krzysztof
 */

#include "orbit_trap_shape.hpp"

using std::max;
using std::min;

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
		case params::fakeLightsShapeSquare:
		{
			delta = par.mRotFakeLightsRotation.RotateVector(delta);

			double dy = max(fabs(fabs(delta.y) - par.fakeLightsOrbitTrapSize),
				fabs(delta.z) - par.fakeLightsOrbitTrapSize);
			double dz = max(fabs(fabs(delta.z) - par.fakeLightsOrbitTrapSize),
				fabs(delta.y) - par.fakeLightsOrbitTrapSize);
			double deltaRect = min(dy, dz);
			dist = sqrt(deltaRect * deltaRect + delta.x * delta.x);
			break;
		}
		case params::fakeLightsShapeSphere:
		{
			delta = par.mRotFakeLightsRotation.RotateVector(delta);

			double lengthYZ = sqrt(delta.y * delta.y + delta.z * delta.z) - par.fakeLightsOrbitTrapSize;
			double distYZ = sqrt(lengthYZ * lengthYZ + delta.x * delta.x);

			double lengthXZ = sqrt(delta.x * delta.x + delta.z * delta.z) - par.fakeLightsOrbitTrapSize;
			double distXZ = sqrt(lengthXZ * lengthXZ + delta.y * delta.y);

			double lengthXY = sqrt(delta.x * delta.x + delta.y * delta.y) - par.fakeLightsOrbitTrapSize;
			double distXY = sqrt(lengthXY * lengthXY + delta.z * delta.z);

			dist = min(min(distYZ, distXZ), distXY);
			break;
		}
		case params::fakeLightsShapeCube:
		{
			delta = par.mRotFakeLightsRotation.RotateVector(delta);

			double size = par.fakeLightsOrbitTrapSize;

			double dy_x = max(fabs(fabs(delta.y) - size), fabs(delta.z) - size);
			double dz_x = max(fabs(fabs(delta.z) - size), fabs(delta.y) - size);
			double dx_z = max(fabs(fabs(delta.y) - size), fabs(delta.x) - size);
			double deltaRectYZ = min(dy_x, dz_x);
			double distYZ = CVector2<double>(deltaRectYZ, fabs(fabs(delta.x) - size)).Length();
			double distXZ = CVector2<double>(dx_z, fabs(fabs(z.z) - size)).Length();

			dist = min(distYZ, distXZ);
			break;
		}
	}

	return dist / par.fakeLightsThickness;
}
