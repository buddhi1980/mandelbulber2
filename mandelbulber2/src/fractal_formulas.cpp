/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * functions for calculation single fractal iterations
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Graeme McLaren
 */

#include "fractal_formulas.hpp"
#include "fractal.h"
#include "common_math.h"

#define SQRT_1_3 0.57735026918962576450914878050196
#define SQRT_1_2 0.70710678118654752440084436210485
#define SQRT_2_3 0.81649658092772603273242802490196
#define SQRT_3_2 1.22474487139158904909864203735295

#ifndef M_PI_8
#define M_PI_8  0.39269908169872415480783042290994
#endif

using namespace fractal;

/**
 * Classic Mandelbulb fractal.
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void MandelbulbIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (aux.r < 1e-21) aux.r = 1e-21;
	double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	double th = th0 * fractal->bulb.power;
	double ph = ph0 * fractal->bulb.power;
	double cth = cos(th);
	aux.r_dz = rp * aux.r_dz * fractal->bulb.power + 1.0;
	rp *= aux.r;
	z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;
}

/**
 * Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * @reference http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 */
void MandelboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->mandelbox.rotationsEnabled)
	{
		bool lockout = false;
		z = fractal->mandelbox.rot[0][0].RotateVector(z);
		if (z.x > fractal->mandelbox.foldingLimit)
		{
			z.x = fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
			lockout = true;
		}
		z = fractal->mandelbox.rotinv[0][0].RotateVector(z);

		z = fractal->mandelbox.rot[1][0].RotateVector(z);
		if (!lockout && z.x < -fractal->mandelbox.foldingLimit)
		{
			z.x = -fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		z = fractal->mandelbox.rotinv[1][0].RotateVector(z);

		lockout = false;
		z = fractal->mandelbox.rot[0][1].RotateVector(z);
		if (z.y > fractal->mandelbox.foldingLimit)
		{
			z.y = fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
			lockout = true;
		}
		z = fractal->mandelbox.rotinv[0][1].RotateVector(z);

		z = fractal->mandelbox.rot[1][1].RotateVector(z);
		if (!lockout && z.y < -fractal->mandelbox.foldingLimit)
		{
			z.y = -fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		z = fractal->mandelbox.rotinv[1][1].RotateVector(z);

		lockout = false;
		z = fractal->mandelbox.rot[0][2].RotateVector(z);
		if (z.z > fractal->mandelbox.foldingLimit)
		{
			z.z = fractal->mandelbox.foldingValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
			lockout = true;
		}
		z = fractal->mandelbox.rotinv[0][2].RotateVector(z);

		z = fractal->mandelbox.rot[1][2].RotateVector(z);
		if (!lockout && z.z < -fractal->mandelbox.foldingLimit)
		{
			z.z = -fractal->mandelbox.foldingValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
		z = fractal->mandelbox.rotinv[1][2].RotateVector(z);
	}
	else
	{
		if (z.x > fractal->mandelbox.foldingLimit)
		{
			z.x = fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		else if (z.x < -fractal->mandelbox.foldingLimit)
		{
			z.x = -fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (z.y > fractal->mandelbox.foldingLimit)
		{
			z.y = fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		else if (z.y < -fractal->mandelbox.foldingLimit)
		{
			z.y = -fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		if (z.z > fractal->mandelbox.foldingLimit)
		{
			z.z = fractal->mandelbox.foldingValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
		else if (z.z < -fractal->mandelbox.foldingLimit)
		{
			z.z = -fractal->mandelbox.foldingValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}

	double r2 = z.Dot(z) + 1e-60;

	z += fractal->mandelbox.offset;

	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}

	z -= fractal->mandelbox.offset;

	if (fractal->mandelbox.mainRotationEnabled)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	z = z * fractal->mandelbox.scale;
	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
}

/**
 * Fractal formula created by Buddhi
 */
void Mandelbulb2Iteration(CVector3 &z, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;

	double temp, tempR;
	tempR = sqrt(z.x * z.x + z.y * z.y + 1e-061);
	z *= (1.0 / tempR);
	temp = z.x * z.x - z.y * z.y;
	z.y = 2.0 * z.x * z.y;
	z.x = temp;
	z *= tempR;

	tempR = sqrt(z.y * z.y + z.z * z.z + 1e-061);
	z *= (1.0 / tempR);
	temp = z.y * z.y - z.z * z.z;
	z.z = 2.0 * z.y * z.z;
	z.y = temp;
	z *= tempR;

	tempR = sqrt(z.x * z.x + z.z * z.z + 1e-061);
	z *= (1.0 / tempR);
	temp = z.x * z.x - z.z * z.z;
	z.z = 2.0 * z.x * z.z;
	z.x = temp;
	z *= tempR;

	z = z * aux.r;
}

/**
 * Fractal formula created by Buddhi
 */
void Mandelbulb3Iteration(CVector3 &z, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;

	double temp, tempR;

	double sign = 1.0;
	double sign2 = 1.0;

	if (z.x < 0)
		sign2 = -1.0;
	tempR = sqrt(z.x * z.x + z.y * z.y + 1e-061);
	z *= (1.0 / tempR);
	temp = z.x * z.x - z.y * z.y;
	z.y = 2.0 * z.x * z.y;
	z.x = temp;
	z *= tempR;

	if (z.x < 0)
		sign = -1.0;
	tempR = sqrt(z.x * z.x + z.z * z.z + 1e-061);
	z *= (1.0 / tempR);
	temp = z.x * z.x - z.z * z.z;
	z.z = 2.0 * z.x * z.z * sign2;
	z.x = temp * sign;
	z *= tempR;

	z = z * aux.r;
}

/**
 * Fractal formula created by Buddhi
 */
void Mandelbulb4Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double rp = pow(aux.r, fractal->bulb.power - 1);
	aux.r_dz = rp * aux.r_dz * fractal->bulb.power + 1.0;

	double angZ = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double angY = atan2(z.z, z.x) + fractal->bulb.betaAngleOffset;
	double angX = atan2(z.z, z.y) + fractal->bulb.gammaAngleOffset;

	CRotationMatrix rotM;
	rotM.RotateX(angX * (fractal->bulb.power - 1));
	rotM.RotateY(angY * (fractal->bulb.power - 1));
	rotM.RotateZ(angZ * (fractal->bulb.power - 1));

	z = rotM.RotateVector(z) * rp;
}

/**
 * Classic Mandelbulb Power 2 fractal
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void MandelbulbPower2Iteration(CVector3 &z, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	double x2 = z.x * z.x + 1e-60;
	double y2 = z.y * z.y + 1e-60;
	double z2 = z.z * z.z;
	double temp = 1.0 - z2 / (x2 + y2);
	double newx = (x2 - y2) * temp;
	double newy = 2.0 * z.x * z.y * temp;
	double newz = -2.0 * z.z * sqrt(x2 + y2);
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

/**
 * Xenodreambuie
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/273/
 */
void XenodreambuieIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double rp = pow(aux.r, fractal->bulb.power - 1);
	aux.r_dz = rp * aux.r_dz * fractal->bulb.power + 1.0;
	rp *= aux.r;

	double th = atan2(z.y, z.x) + fractal->bulb.betaAngleOffset;
	double ph = acos(z.z / aux.r) + fractal->bulb.alphaAngleOffset;
	if (ph > 0.5 * M_PI)
	{
		ph = M_PI - ph;
	}
	else if (ph < -0.5 * M_PI)
	{
		ph = -M_PI - ph;
	}

	z.x = rp * cos(th * fractal->bulb.power) * sin(ph * fractal->bulb.power);
	z.y = rp * sin(th * fractal->bulb.power) * sin(ph * fractal->bulb.power);
	z.z = rp * cos(ph * fractal->bulb.power);
}

/**
 * Menger Sponge formula created by Knighty
 * @reference http://www.fractalforums.com/ifs-iterated-function-systems/kaleidoscopic-(escape-time-ifs)/
 */
void MengerSpongeIteration(CVector3 &z, sExtendedAux &aux)
{
	double temp;
	z.x = fabs(z.x);
	z.y = fabs(z.y);
	z.z = fabs(z.z);
	if (z.x - z.y < 0)
	{
		temp = z.y;
		z.y = z.x;
		z.x = temp;
	}
	if (z.x - z.z < 0)
	{
		temp = z.z;
		z.z = z.x;
		z.x = temp;
	}
	if (z.y - z.z < 0)
	{
		temp = z.z;
		z.z = z.y;
		z.y = temp;
	}

	z *= 3.0;

	z.x -= 2.0;
	z.y -= 2.0;
	if (z.z > 1.0)
		z.z -= 2.0;

	aux.DE *= 3.0;
}

/**
 * Smooth Mandelbox created by Buddhi
 * @reference http://www.fractalforums.com/3d-fractal-generation/mandelbox-with-'smooth'-conditions/
 */
void SmoothMandelboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double sm = fractal->mandelbox.sharpness;

	double zk1 = SmoothConditionAGreaterB(z.x, fractal->mandelbox.foldingLimit, sm);
	double zk2 = SmoothConditionALessB(z.x, -fractal->mandelbox.foldingLimit, sm);
	z.x = z.x * (1.0 - zk1) + (fractal->mandelbox.foldingValue - z.x) * zk1;
	z.x = z.x * (1.0 - zk2) + (-fractal->mandelbox.foldingValue - z.x) * zk2;
	aux.color += (zk1 + zk2) * fractal->mandelbox.color.factor.x;

	double zk3 = SmoothConditionAGreaterB(z.y, fractal->mandelbox.foldingLimit, sm);
	double zk4 = SmoothConditionALessB(z.y, -fractal->mandelbox.foldingLimit, sm);
	z.y = z.y * (1.0 - zk3) + (fractal->mandelbox.foldingValue - z.y) * zk3;
	z.y = z.y * (1.0 - zk4) + (-fractal->mandelbox.foldingValue - z.y) * zk4;
	aux.color += (zk3 + zk4) * fractal->mandelbox.color.factor.y;

	double zk5 = SmoothConditionAGreaterB(z.z, fractal->mandelbox.foldingLimit, sm);
	double zk6 = SmoothConditionALessB(z.z, -fractal->mandelbox.foldingLimit, sm);
	z.z = z.z * (1.0 - zk5) + (fractal->mandelbox.foldingValue - z.z) * zk5;
	z.z = z.z * (1.0 - zk6) + (-fractal->mandelbox.foldingValue - z.z) * zk6;
	aux.color += (zk5 + zk6) * fractal->mandelbox.color.factor.z;

	double r = z.Length();
	double r2 = r * r;
	double tglad_factor2 = fractal->mandelbox.fR2 / r2;
	double rk1 = SmoothConditionALessB(r2, fractal->mandelbox.mR2, sm);
	double rk2 = SmoothConditionALessB(r2, fractal->mandelbox.fR2, sm);
	double rk21 = (1.0 - rk1) * rk2;

	z = z * (1.0 - rk1) + z * (fractal->mandelbox.mboxFactor1 * rk1);
	z = z * (1.0 - rk21) + z * (tglad_factor2 * rk21);
	aux.DE = aux.DE * (1.0 - rk1) + aux.DE * (fractal->mandelbox.mboxFactor1 * rk1);
	aux.DE = aux.DE * (1.0 - rk21) + aux.DE * (tglad_factor2 * rk21);
	aux.color += rk1 * fractal->mandelbox.color.factorSp1;
	aux.color += rk21 * fractal->mandelbox.color.factorSp2;

	z = fractal->mandelbox.mainRot.RotateVector(z);
	z = z * fractal->mandelbox.scale;

	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
}

/**
 * Hybrid of Mandelbox and Mandelbulb power 2 with scaling of z axis
 */
void BoxFoldBulbPow2Iteration(CVector3 &z, const cFractal *fractal)
{
	if (z.x > fractal->foldingIntPow.foldfactor)
		z.x = fractal->foldingIntPow.foldfactor * 2.0 - z.x;
	else if (z.x < -fractal->foldingIntPow.foldfactor)
		z.x = -fractal->foldingIntPow.foldfactor * 2.0 - z.x;

	if (z.y > fractal->foldingIntPow.foldfactor)
		z.y = fractal->foldingIntPow.foldfactor * 2.0 - z.y;
	else if (z.y < -fractal->foldingIntPow.foldfactor)
		z.y = -fractal->foldingIntPow.foldfactor * 2.0 - z.y;

	if (z.z > fractal->foldingIntPow.foldfactor)
		z.z = fractal->foldingIntPow.foldfactor * 2.0 - z.z;
	else if (z.z < -fractal->foldingIntPow.foldfactor)
		z.z = -fractal->foldingIntPow.foldfactor * 2.0 - z.z;

	double r = z.Length();

	double fR2_2 = 1.0;
	double mR2_2 = 0.25;
	double r2_2 = r * r;
	double tglad_factor1_2 = fR2_2 / mR2_2;

	if (r2_2 < mR2_2)
	{
		z = z * tglad_factor1_2;
	}
	else if (r2_2 < fR2_2)
	{
		double tglad_factor2_2 = fR2_2 / r2_2;
		z = z * tglad_factor2_2;
	}

	z = z * 2.0;
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double temp = 1.0 - z2 / (x2 + y2);
	CVector3 zTemp;
	zTemp.x = (x2 - y2) * temp;
	zTemp.y = 2.0 * z.x * z.y * temp;
	zTemp.z = -2.0 * z.z * sqrt(x2 + y2);
	z = zTemp;
	z.z *= fractal->foldingIntPow.zFactor;

	//INFO remark: changed sequence of operation. adding of C constant was before multiplying by z-factor
}

/**
 * Kaleidoscopic Iterated Function Systems (IFS)
 * @reference http://www.fractalforums.com/ifs-iterated-function-systems/kaleidoscopic-(escape-time-ifs)/
 */
void KaleidoscopicIFSIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->IFS.absX)
		z.x = fabs(z.x);
	if (fractal->IFS.absY)
		z.y = fabs(z.y);
	if (fractal->IFS.absZ)
		z.z = fabs(z.z);

	for (int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		if (fractal->IFS.enabled[i])
		{
			z = fractal->IFS.rot[i].RotateVector(z);
			double length = z.Dot(fractal->IFS.direction[i]);

			if (length < fractal->IFS.distance[i])
			{
				z -= fractal->IFS.direction[i]
						* (2.0 * (length - fractal->IFS.distance[i]) * fractal->IFS.intensity[i]);
			}
		}
	}

	if(fractal->IFS.rotationEnabled)
		z = fractal->IFS.mainRot.RotateVector(z - fractal->IFS.offset) + fractal->IFS.offset;

	if (fractal->IFS.edge.x > 0)
		z.x = fractal->IFS.edge.x - fabs(fractal->IFS.edge.x - z.x);
	if (fractal->IFS.edge.y > 0)
		z.y = fractal->IFS.edge.y - fabs(fractal->IFS.edge.y - z.y);
	if (fractal->IFS.edge.z > 0)
		z.z = fractal->IFS.edge.z - fabs(fractal->IFS.edge.z - z.z);

	z *= fractal->IFS.scale;
	if (fractal->IFS.mengerSpongeMode)
	{
		z.x -= fractal->IFS.offset.x * (fractal->IFS.scale - 1.0);
		z.y -= fractal->IFS.offset.y * (fractal->IFS.scale - 1.0);
		if (z.z > 0.5 * fractal->IFS.offset.z * (fractal->IFS.scale - 1.0))
			z.z -= fractal->IFS.offset.z * (fractal->IFS.scale - 1.0);
	}
	else
	{
		z -= fractal->IFS.offset * (fractal->IFS.scale - 1.0);
	}

	aux.DE *= fractal->IFS.scale;
}

/**
 * Aexion's Quadray Sets from FractalForums
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/quadray-sets/msg31458/#msg31458
 */
void AexionIteration(CVector3 &z, double &w, int i, const cFractal *fractal, sExtendedAux &aux)
{
	if (i == 0)
	{
		double cx = fabs(aux.c.x + aux.c.y + aux.c.z) + fractal->aexion.cadd;
		double cy = fabs(-aux.c.x - aux.c.y + aux.c.z) + fractal->aexion.cadd;
		double cz = fabs(-aux.c.x + aux.c.y - aux.c.z) + fractal->aexion.cadd;
		double cw = fabs(aux.c.x - aux.c.y - aux.c.z) + fractal->aexion.cadd;
		aux.c.x = cx;
		aux.c.y = cy;
		aux.c.z = cz;
		aux.cw = cw;
		double tempx = fabs(z.x + z.y + z.z) + fractal->aexion.cadd;
		double tempy = fabs(-z.x - z.y + z.z) + fractal->aexion.cadd;
		double tempz = fabs(-z.x + z.y - z.z) + fractal->aexion.cadd;
		double tempw = fabs(z.x - z.y - z.z) + fractal->aexion.cadd;
		z.x = tempx;
		z.y = tempy;
		z.z = tempz;
		w = tempw;
	}
	double tempx = z.x * z.x - z.y * z.y + 2.0 * w * z.z + aux.c.x;
	double tempy = z.y * z.y - z.x * z.x + 2.0 * w * z.z + aux.c.y;
	double tempz = z.z * z.z - w * w + 2.0 * z.x * z.y + aux.c.z;
	double tempw = w * w - z.z * z.z + 2.0 * z.x * z.y + aux.cw;
	z.x = tempx;
	z.y = tempy;
	z.z = tempz;
	w = tempw;
}

/**
 * 3D Mandelbrot formula invented by David Makin
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/msg7235/#msg7235
 */
void HypercomplexIteration(CVector3 &z, double &w, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	CVector3 newz(z.x * z.x - z.y * z.y - z.z * z.z - w * w,
								2.0 * z.x * z.y - 2.0 * w * z.z,
								2.0 * z.x * z.z - 2.0 * z.y * w);
	double neww = 2.0 * z.x * w - 2.0 * z.y * z.z;
	z = newz;
	w = neww;
}

/**
 * Quaternion fractal
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void QuaternionIteration(CVector3 &z, double &w, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	CVector3 newz(
				z.x * z.x - z.y * z.y - z.z * z.z - w * w,
				2.0 * z.x * z.y,
				2.0 * z.x * z.z);
	double neww = 2.0 * z.x * w;
	z = newz;
	w = neww;
}

/**
 * Formula invented by Benesi
 * @reference http://www.fractalforums.com/index.php?action=profile;u=1170
 */
void BenesiIteration(CVector3 &z, CVector3 &c, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	double r1 = z.y * z.y + z.z * z.z + 1e-60;
	double newx = 0;
	if (c.x < 0 || z.x < sqrt(r1))
	{
		newx = z.x * z.x - r1;
	}
	else
	{
		newx = -z.x * z.x + r1;
	}
	r1 = -1.0 / sqrt(r1) * 2.0 * fabs(z.x);
	double newy = r1 * (z.y * z.y - z.z * z.z);
	double newz = r1 * 2.0 * z.y * z.z;

	z.x = newx;
	z.y = newy;
	z.z = newz;
}

/**
 * Bristorbrot formula
 */
void BristorbrotIteration(CVector3 &z, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	double newx = z.x * z.x - z.y * z.y - z.z * z.z;
	double newy = z.y * (2.0 * z.x - z.z);
	double newz = z.z * (2.0 * z.x + z.y);
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

/**
 * From M3D. A formula made by Trafassel, the original Ide's Formula thread
 * @reference http://www.fractalforums.com/videos/formula-21-%28julia-set-interpretation%29/
 */
void IdesIteration(CVector3 &z, const cFractal *fractal)
{
	if (fabs(z.x) < 2.5) z.x = z.x * .9;
	if (fabs(z.z) < 2.5) z.z = z.z * .9;

	CVector3 z2 = z * z;
	CVector3 newZ;
	newZ.x = fractal->transformCommon.constantMultiplier121.x * z2.x
			- fractal->transformCommon.additionConstant0555.x * (z2.y + z2.z);
	newZ.y = fractal->transformCommon.constantMultiplier121.y * z.x * z.y * z.z;
	newZ.z = fractal->transformCommon.constantMultiplier121.z * z2.z
			- fractal->transformCommon.additionConstant0555.z * (z2.x + z2.y);
	z = newZ;
}

/**
 * From M3D. A formula made by Trafassel, the original Ide's Formula thread
 * @reference http://www.fractalforums.com/videos/formula-21-%28julia-set-interpretation%29/
 */
void Ides2Iteration(CVector3 &z, const cFractal *fractal)
{
	CVector3 z2 = z * z;
	CVector3 newZ;
	newZ.x = fractal->transformCommon.constantMultiplier121.x * z2.x
			- fractal->transformCommon.additionConstant0555.x * (z2.y + z2.z);
	newZ.y = fractal->transformCommon.constantMultiplier121.y * z.x * z.y * z.z;
	newZ.z = fractal->transformCommon.constantMultiplier121.z * z2.z
			- fractal->transformCommon.additionConstant0555.z * (z2.x + z2.y);
	z = newZ + z;
}

/**
 * Buffalo fractal
 */
void BuffaloIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;

	if (fractal->buffalo.preabsx)
		z.x = fabs(z.x);
	if (fractal->buffalo.preabsy)
		z.y = fabs(z.y);
	if (fractal->buffalo.preabsz)
		z.z = fabs(z.z);

	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double temp = 1.0 - z2 / (x2 + y2); // may need exception catching
	double newx = (x2 - y2) * temp;
	double newy = 2.0 * z.x * z.y * temp;
	double newz;

	if (fractal->buffalo.posz)
		newz = 2.0 * z.z * sqrt(x2 + y2);
	else
		newz = -2.0 * z.z * sqrt(x2 + y2);

	if (fractal->buffalo.absx)
		z.x = fabs(newx);
	else
		z.x = newx;

	if (fractal->buffalo.absy)
		z.y = fabs(newy);
	else
		z.y = newy;

	if (fractal->buffalo.absz)
		z.z = fabs(newz);
	else
		z.z = newz;
}

/**
 * From M3D
 * @reference http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/
 */
void QuickDudleyIteration(CVector3 &z)
{
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double newx = x2 - 2 * z.y * z.z;
	double newy = z2 + 2 * z.x * z.y;
	double newz = y2 + 2 * z.x * z.z;
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

/**
 * From M3D
 * @reference http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/
 */
void QuickDudleyModIteration(CVector3 &z, const cFractal *fractal)
{
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double newx = x2 * fractal->transformCommon.constantMultiplier111.x
			- fractal->transformCommon.constantMultiplier222.x * z.y * z.z;
	double newy = z2 * fractal->transformCommon.constantMultiplier111.y
			+ fractal->transformCommon.constantMultiplier222.y * z.x * z.y;
	double newz = y2 * fractal->transformCommon.constantMultiplier111.z
			+ fractal->transformCommon.constantMultiplier222.z * z.z * z.x;
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

/**
 * LkmitchIteration
 * @reference http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/
 */
void LkmitchIteration(CVector3 &z)
{
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double newx = x2 - 2 * z.y * z.z;
	double newy = z2 + 2 * z.x * z.y;
	double newz = y2 - 2 * z.x * z.z;
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

/**
 * Makin3D-2 found through the another shot at the holy grail topic at ff
 * @reference http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/
 */
void Makin3D2Iteration(CVector3 &z)
{
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double newx = x2 + 2 * z.y * z.z;
	double newy = -y2 - 2 * z.x * z.z;
	double newz = -z2 + 2 * z.x * z.y;
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

// NEW FORMULAS-----------------------------------------------------------------

/**
 * ABoxMod1, a formula from Mandelbulb3D.
 * Inspired from a 2D formula proposed by Kali at Fractal Forums
 * @reference http://www.fractalforums.com/new-theories-and-research/kaliset-plus-boxfold-nice-new-2d-fractal/msg33670/#new
 */
void AboxMod1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale = aux.actualScale
			+ fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	z.x = fractal->mandelbox.foldingValue
			- fabs(fabs(z.x + fractal->transformCommon.additionConstant000.x) - fractal->mandelbox.foldingValue)
			- fabs(fractal->transformCommon.additionConstant000.x);
	z.y = fractal->mandelbox.foldingValue
			- fabs(fabs(z.y + fractal->transformCommon.additionConstant000.y) - fractal->mandelbox.foldingValue)
			- fabs(fractal->transformCommon.additionConstant000.y);
	z.z = fractal->mandelbox.foldingValue
			- fabs(fabs(z.z + fractal->transformCommon.additionConstant000.z) - fractal->mandelbox.foldingValue)
			- fabs(fractal->transformCommon.additionConstant000.z);

	double rr = (z.x * z.x + z.y * z.y + z.z * z.z);
	if (rr < 1e-21) rr = 1e-21;
	double m;
	double sqrtMinR = sqrt(fractal->transformCommon.minR0);
	if (sqrtMinR < 1e-21 && sqrtMinR > -1e-21) sqrtMinR = (sqrtMinR > 0) ? 1e-21 : -1e-21;
	if (rr < sqrtMinR)
	{
		m = aux.actualScale / sqrtMinR;
	}
	else
	{
		if (rr < 1)
		{
			m = aux.actualScale / rr;
		}
		else
		{
			m = aux.actualScale;
		}
	}
	z *= m;
	aux.DE = aux.DE * fabs(m) + 1.0;
}

/**
 * ABoxMod2, Based on a formula from Mandelbulb3D.  Inspired from a 2D formula proposed by Kali at Fractal Forums
 * @reference http://www.fractalforums.com/new-theories-and-research/kaliset-plus-boxfold-nice-new-2d-fractal/msg33670/#new
 */
void AboxMod2Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
			- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
			- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
			- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z; // should be default 1.5
	double tem = fabs(z.z) - fractal->transformCommon.offset05;
	double rr;
	if (tem > 0.0)
	{
		rr = (z.x * z.x + z.y * z.y + z.z * z.z); //on top & bottom of cyl
	}
	else
	{
		rr = (z.x * z.x + z.y * z.y);// on cyl body
	}
	if (rr < 1e-21) rr = 1e-21;
	double m;
	double sqrtMinR = sqrt(fractal->transformCommon.minR05);
	if (sqrtMinR < 1e-21 && sqrtMinR > -1e-21) sqrtMinR = (sqrtMinR > 0) ? 1e-21 : -1e-21;
	if (rr < sqrtMinR)
	{
		m = fractal->mandelbox.scale / sqrtMinR;
	}
	else
	{
		if (rr < 1)
		{
			m = fractal->mandelbox.scale / rr;
		}
		else
		{
			m = fractal->mandelbox.scale;
		}
	}
	z *= m;
	aux.DE = aux.DE * fabs(m) + 1.0;
}

/**
 * ABoxModKali, a formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/
 */
void AboxModKaliIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	z = fractal->transformCommon.additionConstant0555 - fabs(z);
	double rr = z.x * z.x + z.y * z.y + z.z * z.z;
	if(rr < 1e-21) rr = 1e-21;
	double MinR = fractal->transformCommon.minR06;
	if (MinR > -1e-21 && MinR < 1e-21) MinR = (MinR > 0) ? 1e-21 : -1e-21;
	double m;
	if (rr < (MinR)) m = fractal->transformCommon.scale015/(MinR);
	else
	{
		if (rr < 1) m =  fractal->transformCommon.scale015/rr;
		else m = fractal->transformCommon.scale015;
	}
	z = z * m ;
	aux.DE = aux.DE * fabs(m) + 1.0;
}
/**
 * ABoxModKaliEiffie, a formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/
 */
void AboxModKaliEiffieIteration(CVector3 &z, CVector3 &c, int i, const cFractal *fractal, sExtendedAux &aux)
{
  CVector3 z1 = z;
  z1.x = fabs(z.x + fractal->transformCommon.additionConstant111.x) // swapping x and z
                  - fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
  z1.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
                  - fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
  z1.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
                  - fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
  z = z1;
  if (fractal->transformCommon.functionEnabled)
  {
    z = CVector3(z.z, z.y, z.x); // swap
  }
  if(z.y > z.x) z = CVector3(z.y, z.x, z.z); // conditional

  double rr = z.x * z.x + z.y * z.y + z.z * z.z;
  if(rr < 1e-21) rr = 1e-21;
  double MinR = fractal->transformCommon.minR05;
  if (MinR < -1e-21 && MinR < 1e-21) MinR = (MinR > 0) ? 1e-21 : -1e-21;
  double m;
  if (rr < (MinR)) m = fractal->transformCommon.scale015/(MinR);
  else
  {
    if (rr < 1) m =  fractal->transformCommon.scale015/rr;
    else m = fractal->transformCommon.scale015;
  }
  z = z * m;

  z += fractal->transformCommon.additionConstant000;

  if (fractal->transformCommon.addCpixelEnabled)
  {
    switch (fractal->mandelbulbMulti.orderOfxyz)
    {
    case sFractalMandelbulbMulti::xyz:
    default:
      c = CVector3(c.x, c.y, c.z);
      break;
    case sFractalMandelbulbMulti::xzy:
      c = CVector3(c.x, c.z, c.y);
      break;
    case sFractalMandelbulbMulti::yxz:
      c = CVector3(c.y, c.x, c.z);
      break;
    case sFractalMandelbulbMulti::yzx:
      c = CVector3(c.y, c.z, c.x);
      break;
    case sFractalMandelbulbMulti::zxy:
      c = CVector3(c.z, c.x, c.y);
      break;
    case sFractalMandelbulbMulti::zyx:
      c = CVector3(c.z, c.y, c.x);
      break;
    }
    z += c * fractal->transformCommon.constantMultiplier111;
  }
  if (fractal->transformCommon.rotationEnabled && i >= fractal->transformCommon.startIterations
      && i < fractal->transformCommon.stopIterations)
  {
    z = fractal->transformCommon.rotationMatrix.RotateVector(z);
  }
  aux.DE = aux.DE * fabs(m) + 1.0;
}
/**
 * ABoxVS_icen1, a formula from Mandelbulb3D.  Inspired from a 2D formula proposed by Kali at Fractal Forums
 * @reference http://www.fractalforums.com/new-theories-and-research/kaliset-plus-boxfold-nice-new-2d-fractal/msg33670/#new
 */
void AboxVSIcen1Iteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale = aux.actualScale
			+ fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
			- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
			- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
			- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;

	if (fractal->transformCommon.juliaMode)
	{
		z += c * fractal->transformCommon.constantMultiplier111;
	}
	else
	{
		z += fractal->transformCommon.juliaC;
	}

	double r2 = z.Dot(z);
	if (r2 < 1e-21 && r2 > -1e-21)
		r2 = (r2 > 0) ? 1e-21 : -1e-21;
	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
	z *= aux.actualScale;

	aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;

	if (fractal->transformCommon.juliaMode)
	{
		z += fractal->transformCommon.juliaC - c * fractal->transformCommon.constantMultiplier111;
	}
	else
	{
		z += c * fractal->transformCommon.constantMultiplier111 + fractal->transformCommon.juliaC;
	}
}

/**
 * aexion octopus. Based on Aex_Octo from Mandelbulber3D
 * @reference http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void AexionOctopusModIteration(CVector3 &z, CVector3 &c, const cFractal *fractal)
{
	CVector3 tempN;
	tempN.x = z.x * z.z * fractal->transformCommon.scale3D111.x;
	tempN.y = (z.x * z.x - z.z * z.z) * fractal->transformCommon.scale3D111.y;
	tempN.z = z.y;

	if (fractal->transformCommon.functionEnabledAx)
		tempN.x = fabs(tempN.x);

	if (fractal->transformCommon.functionEnabledAy)
		tempN.y = fabs(tempN.y);

	if (fractal->transformCommon.functionEnabledAz)
		tempN.z = fabs(tempN.z);

	z = tempN;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx)
			tempFAB.x = fabs(tempFAB.x);

		if (fractal->transformCommon.functionEnabledy)
			tempFAB.y = fabs(tempFAB.y);

		if (fractal->transformCommon.functionEnabledz)
			tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		if (z.x > 0)
			z.x += tempFAB.x;
		else
			z.x -= tempFAB.x;
		if (z.y > 0)
			z.y += tempFAB.y;
		else
			z.y -= tempFAB.y;
		if (z.z > 0)
			z.z += tempFAB.z;
		else
			z.z -= tempFAB.z;
	}
}

/**
 * amazing surf from Mandelbulber3D. Formula proposed by Kali, with features added by Darkbeam
 * @reference ????
 */
void AmazingSurfIteration(CVector3 &z,  const cFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale = aux.actualScale
			+ fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
			- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
			- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	// no z fold

	double rr;
	if (fractal->transformCommon.functionEnabledFalse) // force cylinder fold
	{
		rr = (z.x * z.x + z.y * z.y); // cylinder fold  ;
	}
	else
	{
		rr = (z.x * z.x + z.y * z.y + z.z * z.z);
	}
	if (rr < 1e-21)
		rr = 1e-21;

	double m;
	double sqrtMinR = sqrt(fractal->transformCommon.minR05);
	if (sqrtMinR < 1e-21 && sqrtMinR > -1e-21)
		sqrtMinR = (sqrtMinR > 0) ? 1e-21 : -1e-21;
	if (rr < sqrtMinR)
		m = aux.actualScale / sqrtMinR;
	else
	{
		if (rr < 1)
			m = aux.actualScale / rr;
		else
			m = aux.actualScale;
	}
	z *= m * fractal->transformCommon.scale1 + 1.0 * (1.0 - fractal->transformCommon.scale1);

	aux.DE = aux.DE * fabs(m) + 1.0;

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

/**
 * Based on Amazing Surf Mod 1 from Mandelbulber3D, a formula proposed by Kali, with features added by Darkbeam
 * @reference ????
 */
void AmazingSurfMod1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale = aux.actualScale
			+ fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	if (fractal->transformCommon.functionEnabledAx)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
				- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
				- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	}

	//z = fold - fabs( fabs(z) - fold)
	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		z.x = fractal->transformCommon.additionConstant111.x
				- fabs(fabs(z.x) - fractal->transformCommon.additionConstant111.x);
		z.y = fractal->transformCommon.additionConstant111.y
				- fabs(fabs(z.y) - fractal->transformCommon.additionConstant111.y);
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
	}

	// if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
	if (fractal->transformCommon.functionEnabledxFalse)
	{
		if (z.x > fractal->transformCommon.additionConstant111.x)
		{
			z.x = fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		else if (z.x < -fractal->transformCommon.additionConstant111.x)
		{
			z.x = -fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (z.y > fractal->transformCommon.additionConstant111.y)
		{
			z.y = fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		else if (z.y < -fractal->transformCommon.additionConstant111.y)
		{
			z.y = -fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
	}

	//z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
	if (fractal->transformCommon.functionEnabledyFalse)
	{
		z.x = fractal->transformCommon.offset2
				- fabs(fabs(z.x + fractal->transformCommon.additionConstant111.x)
							 - fractal->transformCommon.offset2)
				- fractal->transformCommon.additionConstant111.x;
		z.y = fractal->transformCommon.offset2
				- fabs(fabs(z.y + fractal->transformCommon.additionConstant111.y)
							 - fractal->transformCommon.offset2)
				- fractal->transformCommon.additionConstant111.y;
	}

	z += fractal->transformCommon.additionConstant000;

	double rr;
	if (fractal->transformCommon.functionEnabledFalse) // force cylinder fold
	{
		rr = (z.x * z.x + z.y * z.y); // cylinder fold  ;
	}
	else
	{
		rr = (z.x * z.x + z.y * z.y + z.z * z.z);
	}
	if (rr < 1e-21)
		rr = 1e-21;

	double m;
	double sqrtMinR = sqrt(fractal->transformCommon.minR05);
	if (sqrtMinR < 1e-21 && sqrtMinR > -1e-21)
		sqrtMinR = (sqrtMinR > 0) ? 1e-21 : -1e-21;
	if (rr < sqrtMinR)
	{
		m = aux.actualScale / sqrtMinR;
	}
	else
	{
		if (rr < 1)
		{
			m = aux.actualScale / rr;
		}
		else
		{
			m = aux.actualScale;
		}
	}

	z *= m * fractal->transformCommon.scale1 + 1.0 * (1.0 - fractal->transformCommon.scale1);

	aux.DE = aux.DE * fabs(m) + 1.0;
	z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

/**
 * benesiFastPwr2PineTree  3D
 * @reference http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void BenesiPineTreeIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 temp = z;
	aux.r = z.Length();
	z *= z;
	double t = 2 * temp.x;
	if (z.y + z.z > 0.0)
		t = t / sqrt(z.y + z.z);
	else
		t = 1.0;

	z.x = (z.x - z.y - z.z) + c.x * fractal->transformCommon.constantMultiplier100.x;
	z.z = (t * (z.y - z.z)) + c.y * fractal->transformCommon.constantMultiplier100.y;
	z.y = (2 * t * temp.y * temp.z) + c.z * fractal->transformCommon.constantMultiplier100.z;
	aux.r_dz = aux.r * aux.r_dz * 2.0 + 1.0;
}

/**
 * benesiT1PineTree 3D
 * @reference http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void BenesiT1PineTreeIteration(CVector3 &z, CVector3 &c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.benesiT1Enabled && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3((tempXZ - z.y) * SQRT_1_2,
								 (tempXZ + z.y) * SQRT_1_2,
								 z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		if (tempL < 1e-21) tempL =  1e-21;
		double avgScale = z.Length()/tempL;

		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		if (fractal->transformCommon.rotationEnabled)
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3,
								 (z.y - z.x) * SQRT_1_2,
								 z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
		z = z - fractal->transformCommon.offset200;
	}
	CVector3 temp = z;
	aux.r = z.Length();
	z *= z;
	double t = 2 * temp.x;
	if (z.y + z.z > 0.0)
		t = t / sqrt(z.y + z.z);
	else
		t = 1.0;
	if (fractal->transformCommon.addCpixelEnabled)
	{
		z.x = (z.x - z.y - z.z) + c.x * fractal->transformCommon.constantMultiplier100.x;
		z.z = (t * (z.y - z.z)) + c.y * fractal->transformCommon.constantMultiplier100.y;  // Cy Cx swap
		z.y = (2 * t * temp.y * temp.z) + c.z * fractal->transformCommon.constantMultiplier100.z;
	}
	if (fractal->transformCommon.juliaMode)
	{
		z.x += fractal->transformCommon.juliaC.x * fractal->transformCommon.constantMultiplier100.x;
		z.z += fractal->transformCommon.juliaC.y * fractal->transformCommon.constantMultiplier100.y;
		z.y += fractal->transformCommon.juliaC.z * fractal->transformCommon.constantMultiplier100.z;
	}
	aux.r_dz = aux.r * aux.r_dz * 2.0 + 1.0;
}

/**
 * Modified Mandelbox (ABox) formula
 */
void FoldBoxMod1Iteration(CVector3 &z, int &i, const cFractal *fractal, sExtendedAux &aux)
{
	if (i >= fractal->transformCommon.startIterations && i < fractal->transformCommon.stopIterations)
	{
		CVector3 tempA, tempB;

		if (fractal->transformCommon.functionEnabledx)
			tempA.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
		if (fractal->transformCommon.functionEnabledAx)
			tempB.x = fabs(z.x - fractal->transformCommon.additionConstantA111.x);
		z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale3D111.x);

		if (fractal->transformCommon.functionEnabledy)
			tempA.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
		if (fractal->transformCommon.functionEnabledAy)
			tempB.y = fabs(z.y - fractal->transformCommon.additionConstantA111.y);
		z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale3D111.y);

		if (fractal->transformCommon.functionEnabledz)
			tempA.z = fabs(z.z + fractal->transformCommon.additionConstant111.z);
		if (fractal->transformCommon.functionEnabledAz)
			tempB.z = fabs(z.z - fractal->transformCommon.additionConstantA111.z);

		z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale3D111.z);
	}

	if (fractal->transformCommon.functionEnabledFalse
			&& i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA) // box fold
	{
		if (z.x > fractal->mandelbox.foldingLimit)
		{
			z.x = fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		else if (z.x < -fractal->mandelbox.foldingLimit)
		{
			z.x = -fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (z.y > fractal->mandelbox.foldingLimit)
		{
			z.y = fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		else if (z.y < -fractal->mandelbox.foldingLimit)
		{
			z.y = -fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
		double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
		if (z.z > zLimit)
		{
			z.z = zValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
		else if (z.z < -zLimit)
		{
			z.z = -zValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}

	if (i >= fractal->transformCommon.startIterationsB
			&& i < fractal->transformCommon.stopIterationsB) // spherical fold
	{
		double r2 = z.Dot(z);
		if (r2 < 1e-21 && r2 > -1e-21)
			r2 = (r2 > 0) ? 1e-21 : -1e-21;
		if (r2 < fractal->mandelbox.mR2)
		{
			z *= fractal->mandelbox.mboxFactor1;
			aux.DE *= fractal->mandelbox.mboxFactor1;
			aux.color += fractal->mandelbox.color.factorSp1;
		}
		else if (r2 < fractal->mandelbox.fR2)
		{
			double tglad_factor2 = fractal->mandelbox.fR2 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}

	z *= fractal->mandelbox.scale;
	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
	aux.r_dz *= fabs(fractal->mandelbox.scale);

	if (fractal->mandelbox.mainRotationEnabled && i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC)
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}
}

/**
 * IQ-Bulb from Mandelbulb 3D and Inigo Quilez
 * @reference http://iquilezles.org/www/articles/mandelbulb/mandelbulb.htm
 */
void IQbulbIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	// extract polar coordinates
	double wr = aux.r;
	if (wr < 1e-21)
		wr = 1e-21;
	double wo = acos(z.y / wr);
	double wi = atan2(z.x, z.z);

	// scale and rotate the point
	wr = pow(wr, fractal->transformCommon.pwr8 - 1.0);
	aux.r_dz = wr * aux.r_dz * fractal->transformCommon.pwr8 + 1.0;
	wr *= aux.r;
	wo *= fractal->transformCommon.pwr8;
	wi *= fractal->transformCommon.pwr8a;

	// convert back to cartesian coordinates
	z.x = sin(wo) * sin(wi);
	z.y = cos(wo);
	z.z = sin(wo) * cos(wi);

	z *= wr;  // then add Cpixel constant
}

/**
 * Based on Kalisets1 and KaliDucks, from Mandelbulb 3D, and refer Formula proposed by Kali, with features added by Darkbeam.
 *
 * <b>M3D notes:</b>
 * Try out julias and low R_bailout values of 2 down to 1!
 * You might have to cutoff at z=0 or so, to see something.
 * @reference http://www.fractalforums.com/new-theories-and-research/very-simple-formula-for-fractal-patterns
 */
void Kalisets1Iteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux)
{
	z = fabs(z);
	double sqs = (z.x * z.x + z.y * z.y + z.z * z.z + 1e-21); // sph inv
	double m;
	double minR = fractal->transformCommon.minR0;  //  KaliDucks

	if (sqs < minR)
		m = 1 / sqrt(minR);
	else
		m = fractal->transformCommon.scale / sqs; //kalisets

	z = z * m;
	aux.DE = aux.DE * fabs(m) + 1.0;

	if (fractal->transformCommon.addCpixelEnabledFalse)
		z += c * fractal->transformCommon.constantMultiplier111;

	if (fractal->transformCommon.juliaMode)
		z += fractal->transformCommon.juliaC;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

/**
 * mandelbulbMulti 3D
 */
void MandelbulbMultiIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r = z.Length();

	double th0 = fractal->bulb.betaAngleOffset;
	double ph0 = fractal->bulb.alphaAngleOffset;
	double v1, v2, v3;

	switch (fractal->mandelbulbMulti.orderOfxyz)
	{
	case sFractalMandelbulbMulti::xyz:
	default:
		v1 = z.x;
		v2 = z.y;
		v3 = z.z;
		break;
	case sFractalMandelbulbMulti::xzy:
		v1 = z.x;
		v2 = z.z;
		v3 = z.y;
		break;
	case sFractalMandelbulbMulti::yxz:
		v1 = z.y;
		v2 = z.x;
		v3 = z.z;
		break;
	case sFractalMandelbulbMulti::yzx:
		v1 = z.y;
		v2 = z.z;
		v3 = z.x;
		break;
	case sFractalMandelbulbMulti::zxy:
		v1 = z.z;
		v2 = z.x;
		v3 = z.y;
		break;
	case sFractalMandelbulbMulti::zyx:
		v1 = z.z;
		v2 = z.y;
		v3 = z.x;
		break;
	}
	if (aux.r < 1e-21)
		aux.r = 1e-21;
	if (v3 < 1e-21 && v3 > -1e-21)
		v3 = (v3 > 0) ? 1e-21 : -1e-21;

	if (fractal->mandelbulbMulti.acosOrasin == sFractalMandelbulbMulti::acos)
		th0 += acos(v1 / aux.r);
	else
		th0 += asin(v1 / aux.r);

	if (fractal->mandelbulbMulti.atanOratan2 == sFractalMandelbulbMulti::atan)
		ph0 += atan(v2 / v3);
	else
		ph0 += atan2(v2, v3);

	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	double th = th0 * fractal->bulb.power;
	double ph = ph0 * fractal->bulb.power;
	double cth = cos(th);
	aux.r_dz = rp * aux.r_dz * fractal->bulb.power + 1.0;
	rp *= aux.r;
	z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;
}

/**
 * mandelbulb vary scaleV1
 */
void MandelbulbVaryPowerV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double tempVC = fractal->bulb.power;   // constant to be varied

	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250 != 0))
	{
		tempVC =
				(tempVC
				 + fractal->transformCommon.offset0 * (i - fractal->transformCommon.startIterations250)
				 / (fractal->transformCommon.stopIterations
						- fractal->transformCommon.startIterations250));
	}
	if (i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset0);
	}

	aux.r = z.Length();
	if (aux.r < 1e-21)
		aux.r = 1e-21;
	double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, tempVC - 1.0);
	double th = th0 * tempVC;
	double ph = ph0 * tempVC;
	double cth = cos(th);
	aux.r_dz = rp * aux.r_dz * tempVC + 1.0;
	rp *= aux.r;
	z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;
}

/**
 * Menger Sponge formula modified by Mclarekin
 */
void MengerMod1Iteration( CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double tempMS;
	z = fabs(z);
	if (z.x - z.y < 0)
	{
		tempMS = z.y;
		z.y = z.x;
		z.x = tempMS;
	}
	if (z.x - z.z < 0)
	{
		tempMS = z.z;
		z.z = z.x;
		z.x = tempMS;
	}
	if (z.y - z.z < 0)
	{
		tempMS = z.z;
		z.z = z.y;
		z.y = tempMS;
	}
	z *= fractal->transformCommon.scale3;
	z.x -= 2.0 * fractal->transformCommon.constantMultiplier111.x;
	z.y -= 2.0 * fractal->transformCommon.constantMultiplier111.y;
	if (z.z > 1)
		z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;

	aux.DE *= fractal->transformCommon.scale3;

	if (fractal->transformCommon.rotationEnabled && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector3 zA = z * 0;
		CVector3 zB = z * 0;
		if (i == fractal->transformCommon.intA)
		{
			zA = z;
		}
		if (i == fractal->transformCommon.intB)
		{
			zB = z;
		}
		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offset)
				+ (zB * fractal->transformCommon.offset0);
	}
}

/**
 * Msltoe Donut formula
 * @reference http://www.fractalforums.com/new-theories-and-research/low-hanging-dessert-an-escape-time-donut-fractal/msg90171/#msg90171
 */
void MsltoeDonutIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double rad2 = fractal->donut.ringThickness;
	double nSect = 2 * M_PI / fractal->donut.number;
	double fact = fractal->donut.factor;

	double R = sqrt(z.x * z.x + z.y * z.y);
	double R2 = fractal->donut.ringRadius - R;
	double t = R2 * R2 + z.z * z.z - rad2 * rad2;

	double theta = atan2(z.y, z.x);
	double theta2 = nSect * round(theta / nSect);

	if (t > 0.03)
	{
		double c1 = cos(theta2);
		double s1 = sin(theta2);

		double x1 = c1 * z.x + s1 * z.y;
		double y1 = -s1 * z.x + c1 * z.y;
		double z1 = z.z;

		x1 = x1 - fractal->donut.ringRadius;

		z.x = fact * x1;
		z.y = fact * z1;
		z.z = fact * y1;

	}
	else
	{
		z /= t;
	}
	aux.color += theta2;
}

/**
 * MsltoeSym2Mod based on the formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/15/
 */
void MsltoeSym2ModIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	CVector3 temp = z;

	if (fabs(z.y) < fabs(z.z)) // then swap
	{
		z.y = temp.z;  // making z.y furthest away from axis
		z.z = temp.y;
	}
	if (z.y > z.z) // then change sign of z.x and z.z
	{
		z.x = -z.x;
	}

	CVector3 z2 = z * z;  // squares
	double v3 = (z2.x + z2.y + z2.z); // sum of squares
	if (v3 < 1e-21 && v3 > -1e-21)
		v3 = (v3 > 0) ? 1e-21 : -1e-21;
	double zr = 1.0 - z2.z / v3;
	temp.x = (z2.x - z2.y) * zr;
	temp.y = 2.0 * z.x * z.y * zr * fractal->transformCommon.scale; // scaling temp.y
	temp.z = 2.0 * z.z * sqrt(z2.x + z2.y);
	z = temp + fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx)
			tempFAB.x = fabs(tempFAB.x);

		if (fractal->transformCommon.functionEnabledy)
			tempFAB.y = fabs(tempFAB.y);

		if (fractal->transformCommon.functionEnabledz)
			tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		if (z.x > 0)
			z.x += tempFAB.x;
		else
			z.x -= tempFAB.x;
		if (z.y > 0)
			z.y += tempFAB.y;
		else
			z.y -= tempFAB.y;
		if (z.z > 0)
			z.z += tempFAB.z;
		else
			z.z -= tempFAB.z;
	}

	double lengthTempZ = -z.Length();
	if (lengthTempZ > -1e-21)
		lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale1);
}

/**
 * MsltoeSym2Mod based on the formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/15/
 */
void MsltoeSym3ModIteration(CVector3 &z,CVector3 &c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	CVector3 temp = z;
	if (fabs(z.y) < fabs(z.z)) // then swap
	{
		z.y = temp.z;  // making z.y furthest away from axis
		z.z = temp.y;
	}
	if (z.y > z.z) // then change sign of z.x and z.z
	{
		z.x = -z.x;
		z.z = -z.z;
	}
	CVector3 z2 = z * z ;  // squares
	double v3 = (z2.x + z2.y + z2.z) + fractal->transformCommon.scale0 * fractal->transformCommon.scale0 * z2.y  * z2.z; ; // sum of squares
	if (v3 < 1e-21 && v3 > -1e-21) v3 = (v3 > 0) ? 1e-21 : -1e-21;
	double zr = 1.0 - z2.z / v3;
	temp.x = (z2.x - z2.y) * zr;
	temp.y = 2.0 * z.x * z.y * zr * fractal->transformCommon.scale;// scaling temp.y
	temp.z = 2.0 * z.z * sqrt(z2.x + z2.y);

	z = temp +  fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx)
		{
			tempFAB.x = fabs(tempFAB.x);
		}
		if (fractal->transformCommon.functionEnabledy)
		{
			tempFAB.y = fabs(tempFAB.y);
		}
		if (fractal->transformCommon.functionEnabledz)
		{
			tempFAB.z = fabs(tempFAB.z);
		}
		tempFAB *= fractal->transformCommon.constantMultiplier000;
		if (z.x > 0) z.x += tempFAB.x;
		else z.x -= tempFAB.x;
		if (z.y > 0) z.y += tempFAB.y;
		else z.y -= tempFAB.y;
		if (z.z > 0) z.z += tempFAB.z;
		else z.z -= tempFAB.z;
	}
	double lengthTempZ = -z.Length();
	if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale1);

	if (fractal->transformCommon.functionEnabledFalse // quaternion fold
			&& i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA)
	{
		aux.r_dz = aux.r_dz * 2.0 * z.Length();
		z = CVector3(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z);
		if (fractal->transformCommon.functionEnabledAxFalse)
		{
			CVector3 temp = z;
			double tempL = temp.Length();
			z *= CVector3(1.0, 2.0, 2.0); // mult. scale (1,2,2)
			if (tempL < 1e-21) tempL = 1e-21;
			double avgScale = z.Length()/tempL;
			//aux.r_dz *= avgScale * fractal->transformCommon.scaleA1;
			double tempAux = aux.r_dz * avgScale;
			aux.r_dz = aux.r_dz + (tempAux - aux.r_dz) * fractal->transformCommon.scaleA1;
		}
		else
		{
			z *= CVector3(1.0, 2.0, 2.0); // mult. scale (1,2,2)
		}
	}
}

/**
 * MsltoeJuliaBulb Eiffie. Refer post by Eiffie    Reply #69 on: January 27, 2015
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/60/
 */
void EiffieMsltoeIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux)
{
	double psi = fabs(fmod(atan2(z.z, z.y) + M_PI + M_PI_8,  M_PI_4) - M_PI_8);
	double lengthYZ = sqrt(z.y * z.y + z.z * z.z);

	z.y = cos(psi) * lengthYZ;
	z.z = sin(psi) * lengthYZ;
	aux.r_dz = aux.r_dz * 2.0 * aux.r;

	CVector3 z2 = z * z;
	double rr = z2.x + z2.y + z2.z + 1e-60;
	double m = 1.0 - z2.z / rr;
	CVector3 newz;
	newz.x = (z2.x - z2.y) * m;
	newz.y = 2.0 * z.x * z.y * m * fractal->transformCommon.scale;// scaling y;;
	newz.z = 2.0 * z.z * sqrt(z2.x + z2.y);
	z = newz + fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx)
		{
			tempFAB.x = fabs(tempFAB.x);
		}
		if (fractal->transformCommon.functionEnabledy)
		{
			tempFAB.y = fabs(tempFAB.y);
		}
		if (fractal->transformCommon.functionEnabledz)
		{
			tempFAB.z = fabs(tempFAB.z);
		}
		tempFAB *= fractal->transformCommon.constantMultiplier000;
		if (z.x > 0) z.x += tempFAB.x;
		else z.x -= tempFAB.x;
		if (z.y > 0) z.y += tempFAB.y;
		else z.y -= tempFAB.y;
		if (z.z > 0) z.z += tempFAB.z;
		else z.z -= tempFAB.z;
	}
	double lengthTempZ = -z.Length();
	if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale1);

}

/**
 * Msltoe_Julia_Bulb_Mod2
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/msg14198/#msg14198
 */
void MsltoeSym3Mod2Iteration(CVector3 &z,CVector3 &c, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;

	double theta;
	double phi;
	CVector3 z2 = z * z;
	double r = z2.x + z2.y + z2.z;
	if (r < 1e-21)
		r = 1e-21;
	double r1 = sqrt(r + fractal->transformCommon.scale0 * z2.y * z2.z);
	if (r1 < 1e-21)
		r1 = 1e-21;
	if (z2.z < z2.y)
	{
		theta = 2 * atan2(z.y, z.x);
		phi = 2 * asin(z.z / r1);
		z.x = r * cos(theta) * cos(phi);
		z.y = r * sin(theta) * cos(phi);
		z.z = -r * sin(phi);
	}
	else
	{
		theta = 2 * atan2(z.z, z.x);
		phi = 2 * asin(z.y / r1);
		z.x = r * cos(theta) * cos(phi);
		z.y = -r * sin(phi);
		z.z = r * sin(theta) * cos(phi);
	}
	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx)
			tempFAB.x = fabs(tempFAB.x);

		if (fractal->transformCommon.functionEnabledy)
			tempFAB.y = fabs(tempFAB.y);

		if (fractal->transformCommon.functionEnabledz)
			tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		if (z.x > 0)
			z.x += tempFAB.x;
		else
			z.x -= tempFAB.x;
		if (z.y > 0)
			z.y += tempFAB.y;
		else
			z.y -= tempFAB.y;
		if (z.z > 0)
			z.z += tempFAB.z;
		else
			z.z -= tempFAB.z;
	}
	double lengthTempZ = -z.Length();
	if (lengthTempZ > -1e-21)
		lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale1);
}

/**
 * Msltoe_Julia_Bulb_Mod3
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/msg14320/#msg14320
 */
void MsltoeSym3Mod3Iteration(CVector3 &z,CVector3 &c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	CVector3 z1 = z;
	double psi = atan2(z.z, z.y) + M_PI * 2.0;
	double psi2 = 0;
	while (psi > M_PI_8)
	{
		psi -= M_PI_4;
		psi2 -= M_PI_4;  // M_PI_4 = pi/4
	}
	double cs = cos(psi2);
	double sn = sin(psi2);
	z1.y = z.y * cs - z.z * sn;
	z1.z = z.y * sn + z.z * cs;
	z.y = z1.y;
	z.z = z1.z;
	CVector3 zs = z * z;
	double zs2 = zs.x + zs.y;
	if (zs2 < 1e-21)
		zs2 = 1e-21;
	double zs3 = (zs2 + zs.z)
			+ fractal->transformCommon.scale0 * fractal->transformCommon.scale0 * zs.y * zs.z;
	double zsd = (1 - zs.z / zs3);

	z1.x = (zs.x - zs.y) * zsd;
	z1.y = (2 * z.x * z.y) * zsd * fractal->transformCommon.scale;  // scaling y;
	z1.z = 2 * z.z * sqrt(zs2);
	z.x = z1.x;
	z.y = z1.y * cs + z1.z * sn;
	z.z = -z1.y * sn + z1.z * cs;
	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse) // symmetrical addCpixel
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx)
			tempFAB.x = fabs(tempFAB.x);

		if (fractal->transformCommon.functionEnabledy)
			tempFAB.y = fabs(tempFAB.y);

		if (fractal->transformCommon.functionEnabledz)
			tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		if (z.x > 0)
			z.x += tempFAB.x;
		else
			z.x -= tempFAB.x;
		if (z.y > 0)
			z.y += tempFAB.y;
		else
			z.y -= tempFAB.y;
		if (z.z > 0)
			z.z += tempFAB.z;
		else
			z.z -= tempFAB.z;
	}
	double lengthTempZ = -z.Length(); // spherical offset
	if (lengthTempZ > -1e-21)
		lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale1);

	if (fractal->transformCommon.functionEnabledFalse // quaternion fold
			&& i >= fractal->transformCommon.startIterationsA && i < fractal->transformCommon.stopIterationsA)
	{
		aux.r_dz = aux.r_dz * 2.0 * z.Length();
		z = CVector3(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z);
		if (fractal->transformCommon.functionEnabledAxFalse)
		{
			CVector3 temp = z;
			double tempL = temp.Length();
			z *= CVector3(1.0, 2.0, 2.0); // mult. scale (1,2,2)
			if (tempL < 1e-21)
				tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			aux.r_dz *= avgScale;
		}
		else
		{
			z *= CVector3(1.0, 2.0, 2.0); // mult. scale (1,2,2)
		}
	}
}

/**
 * MsltoeSym4Mod  Based on the formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/15/
 */
void MsltoeSym4ModIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	CVector3 temp = z;
	double tempL = temp.Length();
	if (tempL < 1e-21)
		tempL = 1e-21;
	z *= fractal->transformCommon.scale3D111;

	aux.r_dz *= fabs(z.Length() / tempL);

	double swap = z.x;
	if (fabs(z.x) < fabs(z.z))
	{
		z.x = z.z;
		z.z = swap;
	}
	if (fabs(z.x) < fabs(z.y))
	{
		swap = z.x;
		z.x = z.y;
		z.y = swap;
	}
	if (fabs(z.y) < fabs(z.z))
	{
		swap = z.y;
		z.y = z.z;
		z.z = swap;
	}

	if (z.x * z.z < 0)
		z.z = -z.z;
	if (z.x * z.y < 0)
		z.y = -z.y;

	temp.x = z.x * z.x - z.y * z.y - z.z * z.z;
	temp.y = 2 * z.x * z.y;
	temp.z = 2 * z.x * z.z;

	z = temp + fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx)
			tempFAB.x = fabs(tempFAB.x);

		if (fractal->transformCommon.functionEnabledy)
			tempFAB.y = fabs(tempFAB.y);

		if (fractal->transformCommon.functionEnabledz)
			tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		if (z.x > 0)
			z.x += tempFAB.x;
		else
			z.x -= tempFAB.x;
		if (z.y > 0)
			z.y += tempFAB.y;
		else
			z.y -= tempFAB.y;
		if (z.z > 0)
			z.z += tempFAB.z;
		else
			z.z -= tempFAB.z;
	}
	double lengthTempZ = -z.Length();
	if (lengthTempZ > -1e-21)
		lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale1);
}

/**
 * MsltoeToroidal
 * @reference http://www.fractalforums.com/theory/toroidal-coordinates/msg9428/
 */
void MsltoeToroidalIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse)// pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.r_dz *= z.Length() / aux.r;
		aux.DE = aux.DE * z.Length() / aux.r + 1.0;
	}
	// Torioidal bulb
	double r1 = fractal->transformCommon.minR05;// default 0.5
	double theta = atan2(z.y, z.x);
	double x1 = r1 * cos(theta);
	double y1 = r1 * sin(theta);
	double r = (z.x -  x1) * (z.x -  x1) + ( z.y - y1) *  (z.y - y1) + z.z * z.z + 1e-061;

	aux.r = r;
	double rp = pow(aux.r, fractal->transformCommon.pwr4);// default 4.0

	double phi = asin( z.z / sqrt( aux.r ));

	phi = fractal->transformCommon.pwr8 * phi; // default 8
	theta = fractal->bulb.power * theta;// default 9 gives 8 symmetry
	// convert back to cartesian coordinates
	z.x= ( r1 + rp * cos(phi)) * cos(theta);
	z.y = ( r1 + rp * cos(phi)) * sin(theta);
	z.z = -rp * sin(phi);

	if (fractal->transformCommon.functionEnabledAxFalse)// spherical offset
	{
		double lengthTempZ = -z.Length();
		if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1 + fractal->transformCommon.offset / lengthTempZ;
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
		aux.r_dz *= fabs(fractal->transformCommon.scale);
	}
	// then add Cpixel constant vector
}

/**
 * RiemannSphereMsltoe
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/riemann-fractals/msg33500/#msg33500
 */
void RiemannSphereMsltoeIteration(CVector3 &z, const cFractal *fractal)
{
	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	double r = z.Length();
	if (r < 1e-21) r = 1e-21;
	z *= fractal->transformCommon.scale / r;

	double w = 1.0 - z.z;
	if (w > -1e-21 && w < 1e-21) w = (w > 0) ? 1e-21 : -1e-21;
	w = 1.0 / w;

	double s = z.x * w;
	double t = z.y * w;

	w = 1.0 + s * s + t * t;

	s = fabs(sin(M_PI * s));
	t = fabs(sin(M_PI * t));

	r *= r;
	if (r < 1e-21)
		r = 1e-21;

	if (w > 36)
		w = 36;   // problem with pow()
	r = -0.25 + pow(r, w);

	w = r / (1.0 + s * s + t * t);
	z.x = 2 * s;
	z.y = 2 * t;
	z.z = -1.0 + s * s + t * t;
	z *= w;

	z += fractal->transformCommon.additionConstant000;


}

/**
 * RiemannSphereMsltoe Variation1
 * @reference http://www.fractalforums.com/new-theories-and-research/revisiting-the-riemann-sphere-%28again%29/
 */
void RiemannSphereMsltoeV1Iteration(CVector3 &z, const cFractal *fractal)
{
	double r = z.Length();
	if (r < 1e-21) r = 1e-21;
	z *= fractal->transformCommon.scale / r;
	double w = 1.0 - z.z;
	if (w > -1e-21 && w < 1e-21)
		w = (w > 0) ? 1e-21 : -1e-21;
	w = 1.0 / w;
	CVector3 t3;
	t3.x = z.x * w;
	t3.y = z.y * w;

	t3.z = (r - 1.5) * (1.0 + t3.x * t3.x + t3.y * t3.y);

	if (fractal->transformCommon.rotationEnabled)
		t3 = fractal->transformCommon.rotationMatrix.RotateVector(t3);

	t3.x = t3.x - floor(t3.x + 0.5);
	t3.y = t3.y - floor(t3.y + 0.5);

	z = t3 * fractal->transformCommon.constantMultiplier441;

	z += fractal->transformCommon.additionConstant000;
}

/**
 * RiemannBulbMsltoe Mod2
 * @reference http://www.fractalforums.com/new-theories-and-research/another-way-to-make-my-riemann-sphere-'bulb'-using-a-conformal-transformation/
 */
void RiemannBulbMsltoeMod2Iteration(CVector3 &z, const cFractal *fractal)
{
	double rad2 = fractal->transformCommon.minR05; // radius default = 0.5
	double r2 = z.x * z.x + z.y * z.y + z.z * z.z;// r2 or point radius squared
	if (r2 < rad2 * rad2)
	{
		if (fractal->transformCommon.functionEnabled)
			z *= rad2 * ((r2 * 0.1) + 0.4) * 1.18 * fractal->transformCommon.scaleA1 /r2; // smooth inside
		else
		{
			z *= fractal->transformCommon.constantMultiplier111;
		}
	} // if internal smooth function is not enabled, then z = z * scale, default vect3(1,1,1)
	else
	{
		z *= fractal->transformCommon.constantMultiplier222; //1st scale variable, default vect3 (1.7, 1.7, 1.7),
		double shift = fractal->transformCommon.offset1;
		double r1 = z.x * z.x + (z.y-shift)*(z.y-shift) + z.z * z.z  + 1e-061;// r1 = length^2,
		z.x = z.x / r1; // inversions by length^2
		z.y = (z.y - shift) / r1;
		z.z = z.z / r1;
		z *= fractal->transformCommon.scale3;// 2nd scale variable , default = double (3.0)
		z.y = z.y + fractal->transformCommon.offset105;// y offset variable, default = double (1.9);
		if (fractal->transformCommon.functionEnabledx)
		{
			z.x = z.x - round(z.x); // periodic cubic tiling,
			z.z = z.z - round(z.z);
		}
		if (fractal->transformCommon.functionEnabledxFalse)
		{
			z.x = fabs(sin(M_PI * z.x * fractal->transformCommon.scale1));
			z.z = fabs(sin(M_PI * z.z * fractal->transformCommon.scale1));
		}
	}
}

/**
 * Quaternion3DE - Quaternion fractal with extended controls
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void Quaternion3DIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	z = CVector3(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z);

	double tempL = z.Length();
	z *= fractal->transformCommon.constantMultiplier122; // mult. scale (1,2,2)
	if (tempL < 1e-21) tempL = 1e-21;
	double avgScale = CVector3(z.x, z.y / 2.0, z.z / 2.0).Length() / tempL;
	double tempAux = aux.r_dz * avgScale;
	aux.r_dz = aux.r_dz + (tempAux - aux.r_dz) * fractal->transformCommon.scaleA1;

	if (fractal->transformCommon.rotationEnabled) // rotation tweak
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	z += fractal->transformCommon.additionConstant000; // addition of constant (0,0,0)
}

/**
 * GeneralizedFoldBoxIteration - Quaternion fractal with extended controls
 * @reference http://www.fractalforums.com/new-theories-and-research/generalized-box-fold/
 */
void GeneralizedFoldBoxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	int i;
	const CVector3 *Nv;
	int sides;

	Nv = fractal->genFoldBox.Nv_tet;
	sides = fractal->genFoldBox.sides_tet;

	if (fractal->genFoldBox.type == foldCube)
	{
		Nv = fractal->genFoldBox.Nv_cube;
		sides = fractal->genFoldBox.sides_cube;
	}
	else if (fractal->genFoldBox.type == foldOct)
	{
		Nv = fractal->genFoldBox.Nv_oct;
		sides = fractal->genFoldBox.sides_oct;
	}
	else if (fractal->genFoldBox.type == foldDodeca)
	{
		Nv = fractal->genFoldBox.Nv_dodeca;
		sides = fractal->genFoldBox.sides_dodeca;
	}
	else if (fractal->genFoldBox.type == foldOctCube)
	{
		Nv = fractal->genFoldBox.Nv_oct_cube;
		sides = fractal->genFoldBox.sides_oct_cube;
	}
	else if (fractal->genFoldBox.type == foldIcosa)
	{
		Nv = fractal->genFoldBox.Nv_icosa;
		sides = fractal->genFoldBox.sides_icosa;
	}
	else if (fractal->genFoldBox.type == foldBox6)
	{
		Nv = fractal->genFoldBox.Nv_box6;
		sides = fractal->genFoldBox.sides_box6;
	}
	else if (fractal->genFoldBox.type == foldBox5)
	{
		Nv = fractal->genFoldBox.Nv_box5;
		sides = fractal->genFoldBox.sides_box5;
	}

	double melt = fractal->mandelbox.melt;
	double solid = fractal->mandelbox.solid;

	// Find the closest cutting plane if any that cuts the line between the origin and z.
	// Line is parameterized as X = Y + L*a;
	// Cutting plane is X.Dot(Nv) = Solid.
	// (Y + L*a).Dot(Nv) = solid.
	// a = (solid - Y.Dot(Nv))/L.Dot(Nv) = b/c
	CVector3 L = z;
	double a = 1;
	CVector3 Y; // Y is the origin in this case.
	int side = -1;
	double b, c;

	for (i = 0; i < sides; i++)
	{
		b = solid;
		c = L.Dot(Nv[i]);
		// A bit subtle here. a_r must be positive and I want to avoid divide by zero.
		if ((c > 0) && ((a * c) > b))
		{
			side = i;
			a = b / c;
		}
	}

	// If z is above the foldingValue we may have to fold. Else early out.
	if (side != -1)
	{ // mirror check
		int side_m = side;
		CVector3 Nv_m = Nv[side_m];
		CVector3 X_m = z - Nv_m * (z.Dot(Nv_m) - solid);

		// Find any plane (Nv_r) closest to X_m that cuts the line between Nv_m and X_m.
		// Nv_m cross Nv_r will define a possible rotation axis.
		// a = (solid - Y.Dot(Nv)/L.Dot(Nv) = b/c.
		L = X_m - Nv_m;
		Y = Nv_m;
		a = 1;
		side = -1;

		for (i = 0; i < sides; i++)
		{
			if (i != side_m)
			{
				b = solid - Y.Dot(Nv[i]);
				c = L.Dot(Nv[i]);
				// A bit subtle here. a_r must be positive and I want to avoid divide by zero.
				if ((c > 0) && ((a * c) > b))
				{
					side = i;
					a = b / c;
				}
			}
		}

		// Was a cutting plane found?
		if (side != -1)
		{ // rotation check
			CVector3 Xmr_intersect = Y + L * a;
			int side_r = side;
			CVector3 Nv_r = Nv[side_r];
			// The axis of rotation is define by the cross product of Nv_m and Nv_r and
			// the intersection of the line between Nv_m and Nv_r and  Xmr_intersect.
			CVector3 L_r = Nv_m.Cross(Nv_r);
			// The closest point between z and the line of rotation can be found by minimizing
			// the square of the distance (D) between z and the line
			// X = Xmr_intersect + L_r * a_rmin.
			// Setting dD/da_rmin equal to zero and solving for a_rmin.
			double a_rmin = (z.Dot(L_r) - Xmr_intersect.Dot(L_r)) / (L_r.Dot(L_r));

			// force a_rmin to be positive. I think I made an even number of sign errors here.
			if (a_rmin < 0)
			{
				a_rmin = -a_rmin;
				L_r = L_r * (-1);
			}
			CVector3 X_r = Xmr_intersect + L_r * a_rmin;

			// Find any plane (Nv_i) closest to Xmr_intersect that cuts the line between
			// Xmr_intersect and X_r. This will define a possible inversion point.
			// a = (solid - Y.Dot(Nv)/L.Dot(Nv) = b/c.
			L = X_r - Xmr_intersect;
			Y = Xmr_intersect;
			a = 1;
			side = -1;

			for (i = 0; i < sides; i++)
			{
				if ((i != side_m) && (i != side_r))
				{
					b = solid - Y.Dot(Nv[i]);
					c = L.Dot(Nv[i]);
					// A bit subtile here. a must be positive and I want to avoid divide by zero.
					if ((c > 0) && ((a * c) > b))
					{
						side = i;
						a = b / c;
					}
				}
			}

			if (side != -1)
			{ // inversion check
				// Only inversion point possible but still need to check for melt.

				CVector3 X_i = Y + L * a;
				CVector3 z2X = X_i - z;
				// Is z above the melt layer.
				if (z2X.Dot(z2X) > (melt * melt))
				{
					double z2X_mag = z2X.Length();
					z = z + z2X * (2 * (z2X_mag - melt) / (z2X_mag + .00000001));
					aux.color += fractal->mandelbox.color.factor.z;
				}
			}
			else
			{
				// Only rotation line possible but still need to check for melt.
				// Is z above the melt layer.
				CVector3 z2X = X_r - z;
				if (z2X.Dot(z2X) > (melt * melt))
				{
					double z2X_mag = z2X.Length();
					z = z + z2X * (2 * (z2X_mag - melt) / (z2X_mag + .00000001));
					aux.color += fractal->mandelbox.color.factor.y;
				}
			}

		}
		else
		{
			// Only mirror plane possible but still need to check for melt.
			CVector3 z2X = X_m - z;
			if (z2X.Dot(z2X) > (melt * melt))
			{
				double z2X_mag = z2X.Length();
				z = z + z2X * (2 * (z2X_mag - melt) / (z2X_mag + .00000001));
				aux.color += fractal->mandelbox.color.factor.x;
			}
		}
	} // outside solid

	double r = z.Length();
	double r2 = r * r;

	z += fractal->mandelbox.offset;

	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}

	z -= fractal->mandelbox.offset;

	if (fractal->mandelbox.mainRotationEnabled)
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	z = z * fractal->mandelbox.scale;
	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
}

void BoxFolding(CVector3 &z, const sFractalFoldings *foldings, sExtendedAux &aux)
{
	if (z.x > foldings->boxLimit)
	{
		z.x = foldings->boxValue - z.x;
		aux.color *= 0.9;
	}
	else if (z.x < -foldings->boxLimit)
	{
		z.x = -foldings->boxValue - z.x;
		aux.color *= 0.9;
	}
	if (z.y > foldings->boxLimit)
	{
		z.y = foldings->boxValue - z.y;
		aux.color *= 0.9;
	}
	else if (z.y < -foldings->boxLimit)
	{
		z.y = -foldings->boxValue - z.y;
		aux.color *= 0.9;
	}
	if (z.z > foldings->boxLimit)
	{
		z.z = foldings->boxValue - z.z;
		aux.color *= 0.9;
	}
	else if (z.z < -foldings->boxLimit)
	{
		z.z = -foldings->boxValue - z.z;
		aux.color *= 0.9;
	}
}

void SphericalFolding(CVector3 &z, const sFractalFoldings *foldings, sExtendedAux &aux)
{
	double fR2_2 = foldings->sphericalOuter * foldings->sphericalOuter;
	double mR2_2 = foldings->sphericalInner * foldings->sphericalInner;
	double r2_2 = aux.r * aux.r;
	double fold_factor1_2 = fR2_2 / mR2_2;

	if (r2_2 < mR2_2)
	{
		z = z * fold_factor1_2;
		aux.DE *= fold_factor1_2;
		aux.r_dz*= sqrt(fold_factor1_2);
		aux.color *= 0.9;
	}
	else if (r2_2 < fR2_2)
	{
		double fold_factor2_2 = fR2_2 / r2_2;
		z = z * fold_factor2_2;
		aux.DE *= fold_factor2_2;
		aux.r_dz*= sqrt(fold_factor1_2);
		aux.color *= 0.9;
	}
}

/**
 * quadratic iteration in imaginary scator algebra
 * @reference http://www.fractalforums.com/new-theories-and-research/ix-possibly-the-holy-grail-fractal-%28in-fff-lore%29
 *            http://luz.izt.uam.mx/index.html/?q=node/95&language=en
 * @author Manuel Fernandez-Guasti
 */
void FastImagscaPower2Iteration(CVector3 &z, const cFractal *fractal)
{
  double x2 = z.x * z.x + 1e-061;
  double y2 = z.y * z.y;
  double z2 = z.z * z.z;

  double newx = x2 - y2 - z2 + (y2 * z2) / x2;
	double newy = 2.0 * z.x * z.y * (1 - z2 / x2);
	double newz = 2.0 * z.x * z.z * (1 - y2 / x2);

	z.x = newx;
	z.y = newy;
	z.z = newz;
}


// NEW TRANSFORM FORMULAS-----------------------------------------------------------------

/**
 * Adds c constant to z vector
 */
void TransformAdditionConstantIteration(CVector3 &z, const cFractal *fractal)
{
	z += fractal->transformCommon.additionConstant000;
	z += fractal->transformCommon.additionConstantA000;
}

/**
 * Adds c constant to z vector. C addition constant varies based on iteration parameters.
 */
void TransformAdditionConstantVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal)
{
	CVector3 tempVC = fractal->transformCommon.additionConstant000;   // constant to be varied
	if (i < fractal->transformCommon.startIterations250)
	{
		;
	}
	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations
					- fractal->transformCommon.startIterations250 != 0))
	{
		tempVC = (tempVC
							+ fractal->transformCommon.offset000
							* (i - fractal->transformCommon.startIterations250)
							/ (fractal->transformCommon.stopIterations
								 - fractal->transformCommon.startIterations250));
	}
	if (i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset000);
	}
	z += tempVC;
}

/**
 * Adds Cpixel constant to z vector
 */
void TransformAddCpixelIteration(CVector3 &z, CVector3 &c, const cFractal *fractal)
{
	z += c * fractal->transformCommon.constantMultiplier111;
}

/**
 * Adds Cpixel constant to z vector, swapping the Cpixel vector x and y axes
 */
void TransformAddCpixelCxCyAxisSwapIteration(CVector3 &z, CVector3 &c, const cFractal *fractal)
{
	if (fractal->transformCommon.functionEnabled)
		c = CVector3(c.y, c.x, c.z);

	z += c * fractal->transformCommon.constantMultiplier111;
}

/**
 * Adds Cpixel constant to z vector. Possible to swap Cpixel vector axes.
 */
void TransformAddCpixelAxisSwapIteration(CVector3 &z, CVector3 &c, const cFractal *fractal)
{
	if (fractal->transformCommon.addCpixelEnabled)
	{
		switch (fractal->mandelbulbMulti.orderOfxyz)
		{
		case sFractalMandelbulbMulti::xyz:
		default:
			c = CVector3(c.x, c.y, c.z);
			break;
		case sFractalMandelbulbMulti::xzy:
			c = CVector3(c.x, c.z, c.y);
			break;
		case sFractalMandelbulbMulti::yxz:
			c = CVector3(c.y, c.x, c.z);
			break;
		case sFractalMandelbulbMulti::yzx:
			c = CVector3(c.y, c.z, c.x);
			break;
		case sFractalMandelbulbMulti::zxy:
			c = CVector3(c.z, c.x, c.y);
			break;
		case sFractalMandelbulbMulti::zyx:
			c = CVector3(c.z, c.y, c.x);
			break;
		}
		z += c * fractal->transformCommon.constantMultiplier111;
	}
}

/**
 * Adds Cpixel constant to z vector, with symmetry
 */
void TransformAddCpixelPosNegIteration(CVector3 &z, CVector3 &c, const cFractal *fractal)
{
	if (fractal->transformCommon.addCpixelEnabled)
	{
		CVector3 tempFAB = c;

		if (fractal->transformCommon.functionEnabledx)
			tempFAB.x = fabs(tempFAB.x);

		if (fractal->transformCommon.functionEnabledy)

			tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz)
			tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier111;

		if (fractal->transformCommon.functionEnabledFalse)
			tempFAB *= -1.0;

		if (z.x > 0)
			z.x += tempFAB.x;
		else
			z.x -= tempFAB.x;
		if (z.y > 0)
			z.y += tempFAB.y;
		else
			z.y -= tempFAB.y;
		if (z.z > 0)
			z.z += tempFAB.z;
		else
			z.z -= tempFAB.z;
	}
}

/**
 * Adds Cpixel constant to z, Cpixel scaled  based on variable iteration parameters.
 */
void TransformAddCpixelVaryV1Iteration(CVector3 &z, CVector3 &c, int i, const cFractal *fractal)
{
	CVector3 tempVC = fractal->transformCommon.constantMultiplier111;   // constant to be varied
	if (i < fractal->transformCommon.startIterations250)
	{
		;
	}
	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations
					- fractal->transformCommon.startIterations250 != 0))
	{
		tempVC = (tempVC
							+ fractal->transformCommon.offset000
							* (i - fractal->transformCommon.startIterations250)
							/ (fractal->transformCommon.stopIterations
								 - fractal->transformCommon.startIterations250));
	}
	if (i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset000);
	}
	z += c * tempVC;
}

/**
 * benesiT1  3D
 * @reference http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT1Iteration(CVector3 &z,  const cFractal *fractal, sExtendedAux &aux)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector3((tempXZ - z.y) * SQRT_1_2,
							 (tempXZ + z.y) * SQRT_1_2,
							 z.x * SQRT_1_3 + z.z * SQRT_2_3);

	CVector3 temp = z;
	double tempL = temp.Length();
	z = fabs(z) * fractal->transformCommon.scale3D222;
	if (tempL < 1e-21) tempL = 1e-21;
	double avgScale = z.Length() / tempL;
	aux.r_dz *= avgScale;
	aux.DE = aux.DE * avgScale + 1.0;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	tempXZ = (z.y + z.x) * SQRT_1_2;

	z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3,
							 (z.y - z.x) * SQRT_1_2,
							 z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
	z = z - fractal->transformCommon.offset200;
}

/**
 * benesiT1Mod  3D based on benesiT1
 * @reference http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT1ModIteration(CVector3 &z,  const cFractal *fractal, sExtendedAux &aux)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector3((tempXZ - z.y) * SQRT_1_2,
							 (tempXZ + z.y) * SQRT_1_2,
							 z.x * SQRT_1_3 + z.z * SQRT_2_3);

	CVector3 temp = z;
	double tempL = temp.Length();
	z = fabs(z) * fractal->transformCommon.scale3D333;
	if (tempL < 1e-21) tempL = 1e-21;
	double avgScale = z.Length() / tempL;
	aux.r_dz *= avgScale;
	aux.DE = aux.DE * avgScale + 1.0;

	z = (fabs(z + fractal->transformCommon.additionConstant111)
			 - fabs(z - fractal->transformCommon.additionConstant111) - z);

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	tempXZ = (z.y + z.x) * SQRT_1_2;

	z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3,
							 (z.y - z.x) * SQRT_1_2,
							 z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * benesi T2
 * @reference http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT2Iteration(CVector3 &z,  const cFractal *fractal, sExtendedAux &aux)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector3((tempXZ - z.y) * SQRT_1_2,
							 (tempXZ + z.y) * SQRT_1_2,
							 z.x * SQRT_1_3 + z.z * SQRT_2_3);

	CVector3 tempV2 = z;
	tempV2.x = sqrt(z.y * z.y + z.z * z.z);
	tempV2.y = sqrt(z.x * z.x + z.z * z.z);  // switching, squared, sqrt
	tempV2.z = sqrt(z.x * z.x + z.y * z.y);

	z = fabs(tempV2 - fractal->transformCommon.additionConstant111);

	CVector3 temp = z;
	double tempL = temp.Length();
	z = fabs(z) * fractal->transformCommon.scale3D444;
	if (tempL < 1e-21) tempL = 1e-21;
	double avgScale = z.Length() / tempL;
	aux.r_dz *= avgScale;
	aux.DE = aux.DE * avgScale + 1.0;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	tempXZ = (z.y + z.x) * SQRT_1_2;

	z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3,
							 (z.y - z.x) * SQRT_1_2,
							 z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * benesi T3
 * @reference http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT3Iteration(CVector3 &z,  const cFractal *fractal)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector3((tempXZ - z.y) * SQRT_1_2,
							 (tempXZ + z.y) * SQRT_1_2,
							 z.x * SQRT_1_3 + z.z * SQRT_2_3);

	CVector3 tempV2 = z;
	tempV2.x = (z.y + z.z);
	tempV2.y = (z.x + z.z);   // switching
	tempV2.z = (z.x + z.y);
	z = (fabs(tempV2 - fractal->transformCommon.additionConstant222))
			* fractal->transformCommon.scale3D222;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	tempXZ = (z.y + z.x) * SQRT_1_2;

	z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3,
							 (z.y - z.x) * SQRT_1_2,
							 z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * benesi T4
 * @reference http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT4Iteration(CVector3 &z,  const cFractal *fractal)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector3((tempXZ - z.y) * SQRT_1_2,
							 (tempXZ + z.y) * SQRT_1_2,
							 z.x * SQRT_1_3 + z.z * SQRT_2_3);

	CVector3 tempV2 = z;
	tempV2.x = (z.y * z.y + z.z * z.z);
	tempV2.y = (z.x * z.x + z.z * z.z);  // switching, squared,
	tempV2.z = (z.x * z.x + z.y * z.y);
	z = (fabs(tempV2 - fractal->transformCommon.additionConstant111))
			* fractal->transformCommon.scale3D222;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

/**
 * benesi T5
 * @reference http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT5bIteration(CVector3 &z,  const cFractal *fractal)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector3((tempXZ - z.y) * SQRT_1_2,
							 (tempXZ + z.y) * SQRT_1_2,
							 z.x * SQRT_1_3 + z.z * SQRT_2_3);

	if (z.x > -1e-21 && z.x < 1e-21)
		z.x = (z.x > 0) ? 1e-21 : -1e-21;
	if (z.y > -1e-21 && z.y < 1e-21)
		z.y = (z.y > 0) ? 1e-21 : -1e-21;
	if (z.z > -1e-21 && z.z < 1e-21)
		z.z = (z.z > 0) ? 1e-21 : -1e-21;
	CVector3 tempV2 = z;
	tempV2.x = fabs(pow(pow(z.y, fractal->transformCommon.power8.x)
											+ pow(z.z, fractal->transformCommon.power8.x),
											fractal->transformCommon.power025.x));
	tempV2.y = fabs(pow(pow(z.x, fractal->transformCommon.power8.y)
											+ pow(z.z, fractal->transformCommon.power8.y),
											fractal->transformCommon.power025.y));
	tempV2.z = fabs(pow(pow(z.x, fractal->transformCommon.power8.z)
											+ pow(z.y, fractal->transformCommon.power8.z),
											fractal->transformCommon.power025.z));
	z = (fabs(tempV2 - fractal->transformCommon.additionConstant111))
			* fractal->transformCommon.scale3D222;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	tempXZ = (z.y + z.x) * SQRT_1_2;

	z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3,
							 (z.y - z.x) * SQRT_1_2,
							 z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * benesi mag forward
 */
void TransformBenesiMagForwardIteration(CVector3 &z)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector3((tempXZ - z.y) * SQRT_1_2,
							 (tempXZ + z.y) * SQRT_1_2,
							 z.x * SQRT_1_3 + z.z * SQRT_2_3);
}

/**
 * benesi mag backward
 */
void TransformBenesiMagBackwardIteration(CVector3 &z)
{
	double tempXZ = (z.y + z.x) * SQRT_1_2;
	z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3,
							 (z.y - z.x) * SQRT_1_2,
							 z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * Benesi Cube to sphere transform
 * Warps a cube to a sphere; transform made by M.Benesi, optimized by Luca.
 * @reference http://www.fractalforums.com/mathematics/circle2square/
 */
void TransformBenesiCubeSphereIteration(CVector3 &z)
{
	z *= z;       // so all now positive

	if (z.x == 0.0)
		z.x = 1e-21;
	if (z.z == 0.0)
		z.z = 1e-21;

	double rCyz = z.y / z.z;

	double rCxyz = (z.y + z.z) / z.x;

	if (rCxyz == -1.0)
		z.z = 1.0 + 1e-21;
	if (rCyz < 1.0)
		rCyz = sqrt(rCyz + 1.0);
	else
		rCyz = sqrt(1.0 / rCyz + 1.0);

	if (rCxyz < 1.0)
		rCxyz = sqrt(rCxyz + 1.0);
	else
		rCxyz = sqrt(1.0 / rCxyz + 1.0);

	z.y *= rCyz;
	z.z *= rCyz;

	z *= rCxyz / SQRT_3_2;
}

/**
 * Benesi Sphere to Cube transform
 * Warps a sphere to a cube; transform made by M.Benesi, optimized by
 * Luca.  Scavenged and edited from code optimized by Luca.
 * @reference http://www.fractalforums.com/mathematics/circle2square/
 */
void TransformBenesiSphereCubeIteration(CVector3 &z)
{
	z *= z;
	if (z.z == 0.0) z.z = 1e-21;
	double rCyz = z.y / z.z;
	if (rCyz < 1.0) rCyz = 1.0 / sqrt(rCyz + 1.0);
	else rCyz = 1.0 / sqrt(1.0 / rCyz + 1.0);

	z.y *= rCyz;
	z.z *= rCyz;

	if (z.x == 0.0) z.x = 1e-21;
	double rCxyz = (z.y * z.y + z.z * z.z) / z.x;

	if (rCxyz < 1.0) rCxyz = 1.0 / sqrt(rCxyz + 1.0);
	else rCxyz = 1.0 / sqrt(1.0 / rCxyz + 1.0);

	z *= rCxyz * SQRT_3_2;
}

/**
 * Box Fold
 */
void TransformBoxFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (z.x > fractal->mandelbox.foldingLimit)
	{
		z.x = fractal->mandelbox.foldingValue - z.x;
		aux.color += fractal->mandelbox.color.factor.x;
	}
	else if (z.x < -fractal->mandelbox.foldingLimit)
	{
		z.x = -fractal->mandelbox.foldingValue - z.x;
		aux.color += fractal->mandelbox.color.factor.x;
	}
	if (z.y > fractal->mandelbox.foldingLimit)
	{
		z.y = fractal->mandelbox.foldingValue - z.y;
		aux.color += fractal->mandelbox.color.factor.y;
	}
	else if (z.y < -fractal->mandelbox.foldingLimit)
	{
		z.y = -fractal->mandelbox.foldingValue - z.y;
		aux.color += fractal->mandelbox.color.factor.y;
	}
	if (z.z > fractal->mandelbox.foldingLimit)
	{
		z.z = fractal->mandelbox.foldingValue - z.z;
		aux.color += fractal->mandelbox.color.factor.z;
	}
	else if (z.z < -fractal->mandelbox.foldingLimit)
	{
		z.z = -fractal->mandelbox.foldingValue - z.z;
		aux.color += fractal->mandelbox.color.factor.z;
	}
}

/**
 * Box Fold XYZ, set different folding parameters for each axis
 */
void TransformBoxFoldXYZIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (z.x > fractal->transformCommon.additionConstant111.x)
	{
		z.x = fractal->transformCommon.additionConstant222.x - z.x;
		aux.color += fractal->mandelbox.color.factor.x;
	}
	else if (z.x < -fractal->transformCommon.additionConstant111.x)
	{
		z.x = -fractal->transformCommon.additionConstant222 .x - z.x;
		aux.color += fractal->mandelbox.color.factor.x;
	}
	if (z.y > fractal->transformCommon.additionConstant111.y)
	{
		z.y = fractal->transformCommon.additionConstant222.y - z.y;
		aux.color += fractal->mandelbox.color.factor.y;
	}
	else if (z.y < -fractal->transformCommon.additionConstant111.y)
	{
		z.y = -fractal->transformCommon.additionConstant222.y - z.y;
		aux.color += fractal->mandelbox.color.factor.y;
	}
	if (z.z > fractal->transformCommon.additionConstant111.z)
	{
		z.z = fractal->transformCommon.additionConstant222.z - z.z;
		aux.color += fractal->mandelbox.color.factor.z;
	}
	else if (z.z < -fractal->transformCommon.additionConstant111.z)
	{
		z.z = -fractal->transformCommon.additionConstant222 .z - z.z;
		aux.color += fractal->mandelbox.color.factor.z;
	}
}

/**
 * Box Offset, set different offset for each axis, added symmetrically about the origin
 */
void TransformBoxOffsetIteration(CVector3 &z, const cFractal *fractal)
{
	if (z.x > 0)
		z.x = z.x + fractal->transformCommon.additionConstant000.x;
	else
		z.x = z.x - fractal->transformCommon.additionConstant000.x;

	if (z.y > 0)
		z.y = z.y + fractal->transformCommon.additionConstant000.y;
	else
		z.y = z.y - fractal->transformCommon.additionConstant000.y;

	if (z.z > 0)
		z.z = z.z + fractal->transformCommon.additionConstant000.z;
	else
		z.z = z.z - fractal->transformCommon.additionConstant000.z;
}

/**
 * fabs add  constant,  z = fabs( z + constant)
 */
void TransformFabsAddConstantIteration(CVector3 &z, const cFractal *fractal)
{
	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledx)
		z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledy)
		z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledz)
		z.z = fabs(z.z);
}

/**
 * fabs. Add fabs constantV2,  z = fabs( z + constant) - fabs( z - constant) - z:
 */
void TransformFabsAddConstantV2Iteration(CVector3 &z, const cFractal *fractal)
{
	z = fabs(z + fractal->transformCommon.additionConstant000)
			- fabs(z - fractal->transformCommon.additionConstant000) - z;
}

/**
 * fabs. Negative fabs constant,  z = - fabs( z + constant)
 */
void TransformNegFabsAddConstantIteration(CVector3 &z, const cFractal *fractal)
{
	z += fractal->transformCommon.additionConstant000;
	if (fractal->transformCommon.functionEnabledx)
		z.x = -fabs(z.x);

	if (fractal->transformCommon.functionEnabledy)
		z.y = -fabs(z.y);

	if (fractal->transformCommon.functionEnabledz)
		z.z = -fabs(z.z);
}

/**
 * fabs Multi - Multiple parameters for fabs functions
 */
void TransformFabsAddMultiIteration(CVector3 &z, const cFractal *fractal)
{
	CVector3 tempA, tempB;

	if (fractal->transformCommon.functionEnabledAx)
		tempA.x = fabs(z.x + fractal->transformCommon.additionConstant000.x);

	if (fractal->transformCommon.functionEnabledx)
		tempB.x = fabs(z.x - fractal->transformCommon.offset000.x);

	z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale3D111.x);

	if (fractal->transformCommon.functionEnabledAy)
		tempA.y = fabs(z.y + fractal->transformCommon.additionConstant000.y);

	if (fractal->transformCommon.functionEnabledy)
		tempB.y = fabs(z.y - fractal->transformCommon.offset000.y);

	z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale3D111.y);

	if (fractal->transformCommon.functionEnabledAz)
		tempA.z = fabs(z.z + fractal->transformCommon.additionConstant000.z);

	if (fractal->transformCommon.functionEnabledz)
		tempB.z = fabs(z.z - fractal->transformCommon.offset000.z);

	z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale3D111.z);
}

/**
 * iteration weight. Influence fractal based on the weight of
 * Z values after different iterations
 */
void TransformIterationWeightIteration(CVector3 &z, int i, const cFractal *fractal)
{
	CVector3 zA, zB;

	if (i - 1 == fractal->transformCommon.intA )
	{
		zA = z;
	}
	if (i == fractal->transformCommon.intB)
	{
		zB = z;
	}
	z = (z * fractal->transformCommon.scale)
			+ (zA * fractal->transformCommon.offset)
			+ (zB * fractal->transformCommon.offset0);
}

/**
 * Linear Combine transform from Mandelbulb3D.
 * Can create multiple combination for the addition of Cpixel
 */
void TransformLinCombineCxyz(CVector3 &c, const cFractal *fractal)
{
	CVector3 temp = c;
	CVector3 mulX = fractal->transformCommon.constantMultiplier100;
	CVector3 mulY = fractal->transformCommon.constantMultiplier010;
	CVector3 mulZ = fractal->transformCommon.constantMultiplier001;
	c.x = mulX.x * temp.x + mulX.y * temp.y + mulX.z * temp.z;
	c.y = mulY.x * temp.x + mulY.y * temp.y + mulY.z * temp.z;
	c.z = mulZ.x * temp.x + mulZ.y * temp.y + mulZ.z * temp.z;
}

/**
 * multiple angle
 */
void TransformMultipleAngle(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double th0 = asin(z.z / aux.r);
	double ph0 = atan2(z.y, z.x);
	double th = th0 * fractal->transformCommon.multiplication;
	double ph = ph0 * fractal->transformCommon.multiplication;
	double cth = cos(th);
	z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * aux.r;
}

/**
 * platonic solid
 * @reference http://www.fractalforums.com/3d-fractal-generation/platonic-dimensions/msg36528/#msg36528
 */
void TransformPlatonicSolidIteration(CVector3 &z, const cFractal *fractal)
{
	double rho = sqrt(z.Length()); // the radius
	double theta = cos(fractal->platonicSolid.frequency * z.x)
			* sin(fractal->platonicSolid.frequency * z.y)
			+ cos(fractal->platonicSolid.frequency * z.y) * sin(fractal->platonicSolid.frequency * z.z)
			+ cos(fractal->platonicSolid.frequency * z.z) * sin(fractal->platonicSolid.frequency * z.x);
	double r = theta * fractal->platonicSolid.amplitude + rho * fractal->platonicSolid.rhoMul;
	z *= r;
}

/**
 * z(n+1) = z(n) * abs(z(n)) ^ p-1
 */
void TransformPowerR(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	aux.r_dz = rp * aux.r_dz * fractal->bulb.power + 1.0;
	z *= rp;
	aux.DE *= rp;
}

/**
 * rotation
 */
void TransformRotationIteration(CVector3 &z, const cFractal *fractal)
{
	z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

/**
 * rotation variation v1. Rotation angles vary based on iteration parameters.
 */
void TransformRotationVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal)
{
	CVector3 tempVC = fractal->transformCommon.rotation;   // constant to be varied

	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250 != 0))
	{
		tempVC =
				(tempVC
				 + fractal->transformCommon.offset000 * (i - fractal->transformCommon.startIterations250)
				 / (fractal->transformCommon.stopIterations
						- fractal->transformCommon.startIterations250));
	}
	if (i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset000);
	}

	tempVC *= (M_PI / 180.0);

	z = z.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), tempVC.x);
	z = z.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), tempVC.y);
	z = z.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), tempVC.z);
}

/**
 * scale
 */
void TransformScaleIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale);
}

/**
 * scale variation v1. Scale varies based on iteration parameters.
 */
void TransformScaleVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double tempVC = fractal->transformCommon.scale;   // constant to be varied

	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250 != 0))
	{
		tempVC = (tempVC
							+ fractal->transformCommon.offset0 * (i - fractal->transformCommon.startIterations250)
							/ (fractal->transformCommon.stopIterations
								 - fractal->transformCommon.startIterations250));
	}
	if (i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset0);
	}
	z *= tempVC;
	aux.DE = aux.DE * fabs(tempVC) + 1.0; //prepared for future analytic DE for hybrids
	aux.r_dz *= fabs(tempVC);
}

/**
 * scale 3d. Scale each axis individually
 */
void TransformScale3DIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	z *= fractal->transformCommon.scale3D111;
	aux.r_dz *= z.Length() / aux.r;
	aux.DE = aux.DE * z.Length() / aux.r + 1.0;
}

/**
 * inverted sphere - A transform from Mandelbulb3D.
 */
void TransformSphereInvCIteration(CVector3 &z, CVector3 &c, const cFractal *fractal)
{
	c *= fractal->transformCommon.constantMultiplier111;
	double rSqrL = c.x * c.x + c.y * c.y + c.z * c.z;
	if (rSqrL < 1e-21) rSqrL = 1e-21;
	rSqrL = 1/rSqrL;
	c *= rSqrL;

	rSqrL = z.x * z.x + z.y * z.y + z.z * z.z;
	if (rSqrL < 1e-21) rSqrL = 1e-21;
	rSqrL = 1/rSqrL;
	z *= rSqrL;
}

/**
 * spherical fold
 */
void TransformSphericalFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double r2 = z.Dot(z);
	if (r2 < 1e-21) r2 = 1e-21;
	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
}

/**
 * spherical fold with offset. Offset radially.
 */
void TransformSphericalOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double lengthTempZ = -z.Length();
	if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale);
}

/**
 * spherical pwr fold
 */
void TransformSphericalPwrFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (z.x > -1e-21 && z.x < 1e-21) z.x = (z.x > 0) ? 1e-21 : -1e-21;
	if (z.y > -1e-21 && z.y < 1e-21) z.y = (z.y > 0) ? 1e-21 : -1e-21;
	if (z.z > -1e-21 && z.z < 1e-21) z.z = (z.z > 0) ? 1e-21 : -1e-21;
	double r2 = pow(pow(z.x, fractal->transformCommon.pwr4) + pow(z.y, fractal->transformCommon.pwr4)
									+ pow(z.z, fractal->transformCommon.pwr4),
									fractal->transformCommon.pwr05);

	if (r2 < 1e-21 && r2 > -1e-21)
		r2 = (r2 > 0) ? 1e-21 : -1e-21;
	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
}

/**
 * z vector - axis swap
 */
void TransformZvectorAxisSwapIteration(CVector3 &z, const cFractal *fractal)
{
	switch (fractal->mandelbulbMulti.orderOfxyz)
	{
	case sFractalMandelbulbMulti::xyz:
	default:
		z = CVector3(z.x, z.y, z.z);
		break;
	case sFractalMandelbulbMulti::xzy:
		z = CVector3(z.x, z.z, z.y);
		break;
	case sFractalMandelbulbMulti::yxz:
		z = CVector3(z.y, z.x, z.z);
		break;
	case sFractalMandelbulbMulti::yzx:
		z = CVector3(z.y, z.z, z.x);
		break;
	case sFractalMandelbulbMulti::zxy:
		z = CVector3(z.z, z.x, z.y);
		break;
	case sFractalMandelbulbMulti::zyx:
		z = CVector3(z.z, z.y, z.x);
		break;
	}
}

//-------------------------------- 4D ----------------------------------------------

/**
 * Quaternion4D
 */
void Quaternion4DIteration(CVector4 &z4D, int i, const cFractal *fractal)
{
	double w0 = 0.0;
	if (i < 1)
		w0 = fractal->transformCommon.offset0;
	z4D.w += w0;

	z4D = CVector4(z4D.x * z4D.x - z4D.y * z4D.y - z4D.z * z4D.z - z4D.w * z4D.w,
								 z4D.x * z4D.y,
								 z4D.x * z4D.z,
								 z4D.w);
	z4D *= fractal->transformCommon.constantMultiplier1220;
	z4D += fractal->transformCommon.additionConstant0000;
}

/**
 * Formula based on Mandelbox (ABox). Extended to 4 dimensions and with variable scale parameter.
 */
void MandelboxVaryScale4DIteration(CVector4 &z4D, int i,  const cFractal *fractal,
																	 sExtendedAux &aux)
{
	double w0 = 0.0;
	if (i < 1.0)
		w0 = fractal->transformCommon.offset0;
	z4D.w += w0;

	aux.actualScale = aux.actualScale
			+ fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);
	CVector4 oldz = z4D;
	z4D.x = fabs(z4D.x + fractal->mandelboxVary4D.fold)
			- fabs(z4D.x - fractal->mandelboxVary4D.fold) - z4D.x;
	z4D.y = fabs(z4D.y + fractal->mandelboxVary4D.fold)
			- fabs(z4D.y - fractal->mandelboxVary4D.fold) - z4D.y;
	z4D.z = fabs(z4D.z + fractal->mandelboxVary4D.fold)
			- fabs(z4D.z - fractal->mandelboxVary4D.fold) - z4D.z;
	z4D.w = fabs(z4D.w + fractal->mandelboxVary4D.fold)
			- fabs(z4D.w - fractal->mandelboxVary4D.fold) - z4D.w;

	if (z4D.x != oldz.x)
		aux.color += fractal->mandelbox.color.factor4D.x;
	if (z4D.y != oldz.y)
		aux.color += fractal->mandelbox.color.factor4D.y;
	if (z4D.z != oldz.z)
		aux.color += fractal->mandelbox.color.factor4D.z;
	if (z4D.w != oldz.w)
		aux.color += fractal->mandelbox.color.factor4D.w;

	double rr = pow(z4D.x * z4D.x + z4D.y * z4D.y + z4D.z * z4D.z + z4D.w * z4D.w,
									fractal->mandelboxVary4D.rPower);
	double m = aux.actualScale;
	if (rr < fractal->mandelboxVary4D.minR * fractal->mandelboxVary4D.minR)
	{
		m = aux.actualScale / (fractal->mandelboxVary4D.minR * fractal->mandelboxVary4D.minR);
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < 1.0)
	{
		m = aux.actualScale / rr;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
	z4D *= m;
	z4D.w += fractal->mandelboxVary4D.wadd;
	aux.DE = aux.DE * fabs(m) + 1.0;
}

/**
 * Adds c constant to z vector 4D
 */
void TransformAdditionConstant4DIteration(CVector4 &z4D, const cFractal *fractal)
{
	z4D += fractal->transformCommon.additionConstant0000;
}

/**
 * box fold 4D
 */
void TransformBoxFold4DIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux)
{
	if (z4D.x > fractal->mandelbox.foldingLimit)
	{
		z4D.x = fractal->mandelbox.foldingValue - z4D.x;
		aux.color += fractal->mandelbox.color.factor4D.x;
	}
	else if (z4D.x < -fractal->mandelbox.foldingLimit)
	{
		z4D.x = -fractal->mandelbox.foldingValue - z4D.x;
		aux.color += fractal->mandelbox.color.factor4D.x;
	}
	if (z4D.y > fractal->mandelbox.foldingLimit)
	{
		z4D.y = fractal->mandelbox.foldingValue - z4D.y;
		aux.color += fractal->mandelbox.color.factor4D.y;
	}
	else if (z4D.y < -fractal->mandelbox.foldingLimit)
	{
		z4D.y = -fractal->mandelbox.foldingValue - z4D.y;
		aux.color += fractal->mandelbox.color.factor4D.y;
	}
	if (z4D.z > fractal->mandelbox.foldingLimit)
	{
		z4D.z = fractal->mandelbox.foldingValue - z4D.z;
		aux.color += fractal->mandelbox.color.factor4D.z;
	}
	else if (z4D.z < -fractal->mandelbox.foldingLimit)
	{
		z4D.z = -fractal->mandelbox.foldingValue - z4D.z;
		aux.color += fractal->mandelbox.color.factor4D.z;
	}
	if (z4D.z > fractal->mandelbox.foldingLimit)
	{
		z4D.z = fractal->mandelbox.foldingValue - z4D.w;
		aux.color += fractal->mandelbox.color.factor4D.w;
	}
	else if (z4D.z < -fractal->mandelbox.foldingLimit)
	{
		z4D.z = -fractal->mandelbox.foldingValue - z4D.w;
		aux.color += fractal->mandelbox.color.factor4D.w;
	}
}

/**
 * fabs add  constant,  z = fabs( z + constant)
 */
void TransformFabsAddConstant4DIteration(CVector4 &z4D, const cFractal *fractal)
{
	z4D += fractal->transformCommon.additionConstant0000;

	if (fractal->transformCommon.functionEnabledx)
		z4D.x = fabs(z4D.x);

	if (fractal->transformCommon.functionEnabledy)
		z4D.y = fabs(z4D.y);

	if (fractal->transformCommon.functionEnabledz)
		z4D.z = fabs(z4D.z);

	if (fractal->transformCommon.functionEnabled)
		z4D.w = fabs(z4D.w);
}

/**
 * fabs.  Add fabs constantV2,  z = fabs( z + constant) - fabs( z - constant) - z:
 */
void TransformFabsAddConstantV24DIteration(CVector4 &z4D, const cFractal *fractal)
{
	z4D = fabs(z4D + fractal->transformCommon.additionConstant0000)
			- fabs(z4D - fractal->transformCommon.additionConstant0000) - z4D;
}

/**
 * iteration weight 4D
 */
void TransformIterationWeight4DIteration(CVector4 &z4D, int i, const cFractal *fractal)
{
	CVector4 zA, zB;

	if (i - 1 == fractal->transformCommon.intA)
		zA = z4D;

	if (i == fractal->transformCommon.intB)
		zB = z4D;

	z4D = (z4D * fractal->transformCommon.scale) + (zA * fractal->transformCommon.offset)
			+ (zB * fractal->transformCommon.offset0);
}

/**
 * scale 4D
 */
void TransformScale4DIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux)
{
	z4D *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale);
}

/**
 * spherical fold 4D
 */
void TransformSphericalFold4DIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux)
{
	double r2 = z4D.Dot(z4D);
	if (r2 < 1e-21 && r2 > -1e-21) r2 = (r2 > 0) ? 1e-21 : -1e-21;

	if (r2 < fractal->mandelbox.mR2)
	{
		z4D *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z4D *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
}

/**
 * TransformSurfFoldMultiIteration
 */
void TransformSurfFoldMultiIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
  if (fractal->transformCommon.functionEnabledAx)
  {
    z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
        - fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
    z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
        - fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
  }

  //z = fold - fabs( fabs(z) - fold)
  if (fractal->transformCommon.functionEnabledAyFalse)
  {
    z.x = fractal->transformCommon.additionConstant111.x
        - fabs(fabs(z.x) - fractal->transformCommon.additionConstant111.x);
    z.y = fractal->transformCommon.additionConstant111.y
        - fabs(fabs(z.y) - fractal->transformCommon.additionConstant111.y);
  }

  if (fractal->transformCommon.functionEnabledAzFalse)
  {
    z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
    z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
  }

  // if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
  if (fractal->transformCommon.functionEnabledxFalse)
  {
    if (z.x > fractal->transformCommon.additionConstant111.x)
    {
      z.x = fractal->mandelbox.foldingValue - z.x;
      aux.color += fractal->mandelbox.color.factor.x;
    }
    else if (z.x < -fractal->transformCommon.additionConstant111.x)
    {
      z.x = -fractal->mandelbox.foldingValue - z.x;
      aux.color += fractal->mandelbox.color.factor.x;
    }
    if (z.y > fractal->transformCommon.additionConstant111.y)
    {
      z.y = fractal->mandelbox.foldingValue - z.y;
      aux.color += fractal->mandelbox.color.factor.y;
    }
    else if (z.y < -fractal->transformCommon.additionConstant111.y)
    {
      z.y = -fractal->mandelbox.foldingValue - z.y;
      aux.color += fractal->mandelbox.color.factor.y;
    }
  }

  //z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
  if (fractal->transformCommon.functionEnabledyFalse)
  {
    z.x = fractal->transformCommon.offset2
        - fabs(fabs(z.x + fractal->transformCommon.additionConstant111.x)
        - fractal->transformCommon.offset2)
        - fractal->transformCommon.additionConstant111.x;
    z.y = fractal->transformCommon.offset2
        - fabs(fabs(z.y + fractal->transformCommon.additionConstant111.y)
        - fractal->transformCommon.offset2)
        - fractal->transformCommon.additionConstant111.y;
  }
}

