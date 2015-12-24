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
#include "fractal_transforms.hpp"
#include "common_math.h"

#define SQRT_1_3 0.57735026918962576450914878050196
#define SQRT_1_2 0.70710678118654752440084436210485
#define SQRT_2_3 0.81649658092772603273242802490196
#define SQRT_3_2 1.22474487139158904909864203735295

using namespace fractal;

void MandelbulbIteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux)
{
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

void MandelboxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux)
{
	if (fractal->mandelbox.rotationsEnabled)
	{
		bool lockout = false;
		z = fractal->mandelbox.rot[0][0].RotateVector(z);
		if (z.x > fractal->mandelbox.foldingLimit)
		{
			z.x = fractal->mandelbox.foldingValue - z.x;
			aux.mboxColor += fractal->mandelbox.color.factor.x;
			lockout = true;
		}
		z = fractal->mandelbox.rotinv[0][0].RotateVector(z);

		z = fractal->mandelbox.rot[1][0].RotateVector(z);
		if (!lockout && z.x < -fractal->mandelbox.foldingLimit)
		{
			z.x = -fractal->mandelbox.foldingValue - z.x;
			aux.mboxColor += fractal->mandelbox.color.factor.x;
		}
		z = fractal->mandelbox.rotinv[1][0].RotateVector(z);

		lockout = false;
		z = fractal->mandelbox.rot[0][1].RotateVector(z);
		if (z.y > fractal->mandelbox.foldingLimit)
		{
			z.y = fractal->mandelbox.foldingValue - z.y;
			aux.mboxColor += fractal->mandelbox.color.factor.y;
			lockout = true;
		}
		z = fractal->mandelbox.rotinv[0][1].RotateVector(z);

		z = fractal->mandelbox.rot[1][1].RotateVector(z);
		if (!lockout && z.y < -fractal->mandelbox.foldingLimit)
		{
			z.y = -fractal->mandelbox.foldingValue - z.y;
			aux.mboxColor += fractal->mandelbox.color.factor.y;
		}
		z = fractal->mandelbox.rotinv[1][1].RotateVector(z);

		lockout = false;
		z = fractal->mandelbox.rot[0][2].RotateVector(z);
		if (z.z > fractal->mandelbox.foldingLimit)
		{
			z.z = fractal->mandelbox.foldingValue - z.z;
			aux.mboxColor += fractal->mandelbox.color.factor.z;
			lockout = true;
		}
		z = fractal->mandelbox.rotinv[0][2].RotateVector(z);

		z = fractal->mandelbox.rot[1][2].RotateVector(z);
		if (!lockout && z.z < -fractal->mandelbox.foldingLimit)
		{
			z.z = -fractal->mandelbox.foldingValue - z.z;
			aux.mboxColor += fractal->mandelbox.color.factor.z;
		}
		z = fractal->mandelbox.rotinv[1][2].RotateVector(z);
	}
	else
	{
		if (z.x > fractal->mandelbox.foldingLimit)
		{
			z.x = fractal->mandelbox.foldingValue - z.x;
			aux.mboxColor += fractal->mandelbox.color.factor.x;
		}
		else if (z.x < -fractal->mandelbox.foldingLimit)
		{
			z.x = -fractal->mandelbox.foldingValue - z.x;
			aux.mboxColor += fractal->mandelbox.color.factor.x;
		}
		if (z.y > fractal->mandelbox.foldingLimit)
		{
			z.y = fractal->mandelbox.foldingValue - z.y;
			aux.mboxColor += fractal->mandelbox.color.factor.y;
		}
		else if (z.y < -fractal->mandelbox.foldingLimit)
		{
			z.y = -fractal->mandelbox.foldingValue - z.y;
			aux.mboxColor += fractal->mandelbox.color.factor.y;
		}
		if (z.z > fractal->mandelbox.foldingLimit)
		{
			z.z = fractal->mandelbox.foldingValue - z.z;
			aux.mboxColor += fractal->mandelbox.color.factor.z;
		}
		else if (z.z < -fractal->mandelbox.foldingLimit)
		{
			z.z = -fractal->mandelbox.foldingValue - z.z;
			aux.mboxColor += fractal->mandelbox.color.factor.z;
		}
	}

	double r2 = z.Dot(z);

	z += fractal->mandelbox.offset;

	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.mboxDE *= fractal->mandelbox.mboxFactor1;
		aux.mboxColor += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.mboxDE *= tglad_factor2;
		aux.mboxColor += fractal->mandelbox.color.factorSp2;
	}

	z -= fractal->mandelbox.offset;

	if (fractal->mandelbox.mainRotationEnabled)
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	z = z * fractal->mandelbox.scale;
	aux.mboxDE = aux.mboxDE * fabs(fractal->mandelbox.scale) + 1.0;

}

void Mandelbulb2Iteration(CVector3 &z, sMandelbulbAux &aux)
{
	double temp, tempR;
	tempR = sqrt(z.x * z.x + z.y * z.y);
	z *= (1.0 / tempR);
	temp = z.x * z.x - z.y * z.y;
	z.y = 2.0 * z.x * z.y;
	z.x = temp;
	z *= tempR;

	tempR = sqrt(z.y * z.y + z.z * z.z);
	z *= (1.0 / tempR);
	temp = z.y * z.y - z.z * z.z;
	z.z = 2.0 * z.y * z.z;
	z.y = temp;
	z *= tempR;

	tempR = sqrt(z.x * z.x + z.z * z.z);
	z *= (1.0 / tempR);
	temp = z.x * z.x - z.z * z.z;
	z.z = 2.0 * z.x * z.z;
	z.x = temp;
	z *= tempR;

	z = z * aux.r;
}

void Mandelbulb3Iteration(CVector3 &z, sMandelbulbAux &aux)
{
	double temp, tempR;

	double sign = 1.0;
	double sign2 = 1.0;

	if (z.x < 0) sign2 = -1.0;
	tempR = sqrt(z.x * z.x + z.y * z.y);
	z *= (1.0 / tempR);
	temp = z.x * z.x - z.y * z.y;
	z.y = 2.0 * z.x * z.y;
	z.x = temp;
	z *= tempR;

	if (z.x < 0) sign = -1.0;
	tempR = sqrt(z.x * z.x + z.z * z.z);
	z *= (1.0 / tempR);
	temp = z.x * z.x - z.z * z.z;
	z.z = 2.0 * z.x * z.z * sign2;
	z.x = temp * sign;
	z *= tempR;

	z = z * aux.r;
}

void Mandelbulb4Iteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux)
{
	double rp = pow(aux.r, fractal->bulb.power - 1);

	double angZ = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double angY = atan2(z.z, z.x) + fractal->bulb.betaAngleOffset;
	double angX = atan2(z.z, z.y) + fractal->bulb.gammaAngleOffset;

	CRotationMatrix rotM;
	rotM.RotateX(angX * (fractal->bulb.power - 1));
	rotM.RotateY(angY * (fractal->bulb.power - 1));
	rotM.RotateZ(angZ * (fractal->bulb.power - 1));

	z = rotM.RotateVector(z) * rp;
}

void MandelbulbPower2Iteration(CVector3 &z)
{
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double temp = 1.0 - z2 / (x2 + y2);
	double newx = (x2 - y2) * temp;
	double newy = 2.0 * z.x * z.y * temp;
	double newz = -2.0 * z.z * sqrt(x2 + y2);
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

void XenodreambuieIteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux)
{
	double rp = pow(aux.r, fractal->bulb.power);
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

void MengerSpongeIteration(CVector3 &z, sIFSAux &aux)
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
	if (z.z > 1.0) z.z -= 2.0;

	aux.ifsDE *= 3.0;

}

void SmoothMandelboxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux)
{
	double sm = fractal->mandelbox.sharpness;

	double zk1 = SmoothConditionAGreaterB(z.x, fractal->mandelbox.foldingLimit, sm);
	double zk2 = SmoothConditionALessB(z.x, -fractal->mandelbox.foldingLimit, sm);
	z.x = z.x * (1.0 - zk1) + (fractal->mandelbox.foldingValue - z.x) * zk1;
	z.x = z.x * (1.0 - zk2) + (-fractal->mandelbox.foldingValue - z.x) * zk2;
	aux.mboxColor += (zk1 + zk2) * fractal->mandelbox.color.factor.x;

	double zk3 = SmoothConditionAGreaterB(z.y, fractal->mandelbox.foldingLimit, sm);
	double zk4 = SmoothConditionALessB(z.y, -fractal->mandelbox.foldingLimit, sm);
	z.y = z.y * (1.0 - zk3) + (fractal->mandelbox.foldingValue - z.y) * zk3;
	z.y = z.y * (1.0 - zk4) + (-fractal->mandelbox.foldingValue - z.y) * zk4;
	aux.mboxColor += (zk3 + zk4) * fractal->mandelbox.color.factor.y;

	double zk5 = SmoothConditionAGreaterB(z.z, fractal->mandelbox.foldingLimit, sm);
	double zk6 = SmoothConditionALessB(z.z, -fractal->mandelbox.foldingLimit, sm);
	z.z = z.z * (1.0 - zk5) + (fractal->mandelbox.foldingValue - z.z) * zk5;
	z.z = z.z * (1.0 - zk6) + (-fractal->mandelbox.foldingValue - z.z) * zk6;
	aux.mboxColor += (zk5 + zk6) * fractal->mandelbox.color.factor.z;

	double r = z.Length();
	double r2 = r * r;
	double tglad_factor2 = fractal->mandelbox.fR2 / r2;
	double rk1 = SmoothConditionALessB(r2, fractal->mandelbox.mR2, sm);
	double rk2 = SmoothConditionALessB(r2, fractal->mandelbox.fR2, sm);
	double rk21 = (1.0 - rk1) * rk2;

	z = z * (1.0 - rk1) + z * (fractal->mandelbox.mboxFactor1 * rk1);
	z = z * (1.0 - rk21) + z * (tglad_factor2 * rk21);
	aux.mboxDE = aux.mboxDE * (1.0 - rk1) + aux.mboxDE * (fractal->mandelbox.mboxFactor1 * rk1);
	aux.mboxDE = aux.mboxDE * (1.0 - rk21) + aux.mboxDE * (tglad_factor2 * rk21);
	aux.mboxColor += rk1 * fractal->mandelbox.color.factorSp1;
	aux.mboxColor += rk21 * fractal->mandelbox.color.factorSp2;

	z = fractal->mandelbox.mainRot.RotateVector(z);
	z = z * fractal->mandelbox.scale;

	aux.mboxDE = aux.mboxDE * fabs(fractal->mandelbox.scale) + 1.0;
}

void BoxFoldBulbPow2Iteration(CVector3 &z, const cFractal *fractal)
{
	if (z.x > fractal->foldingIntPow.foldfactor) z.x = fractal->foldingIntPow.foldfactor * 2.0 - z.x;
	else if (z.x < -fractal->foldingIntPow.foldfactor) z.x = -fractal->foldingIntPow.foldfactor * 2.0
			- z.x;

	if (z.y > fractal->foldingIntPow.foldfactor) z.y = fractal->foldingIntPow.foldfactor * 2.0 - z.y;
	else if (z.y < -fractal->foldingIntPow.foldfactor) z.y = -fractal->foldingIntPow.foldfactor * 2.0
			- z.y;

	if (z.z > fractal->foldingIntPow.foldfactor) z.z = fractal->foldingIntPow.foldfactor * 2.0 - z.z;
	else if (z.z < -fractal->foldingIntPow.foldfactor) z.z = -fractal->foldingIntPow.foldfactor * 2.0
			- z.z;

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

void KaleidoscopicIFSIteration(CVector3 &z, const cFractal *fractal, sIFSAux &aux)
{
	if (fractal->IFS.absX) z.x = fabs(z.x);
	if (fractal->IFS.absY) z.y = fabs(z.y);
	if (fractal->IFS.absZ) z.z = fabs(z.z);

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
	z = fractal->IFS.mainRot.RotateVector(z - fractal->IFS.offset) + fractal->IFS.offset;

	if (fractal->IFS.edge.x > 0) z.x = fractal->IFS.edge.x - fabs(fractal->IFS.edge.x - z.x);
	if (fractal->IFS.edge.y > 0) z.y = fractal->IFS.edge.y - fabs(fractal->IFS.edge.y - z.y);
	if (fractal->IFS.edge.z > 0) z.z = fractal->IFS.edge.z - fabs(fractal->IFS.edge.z - z.z);

	z *= fractal->IFS.scale;
	if (fractal->IFS.mengerSpongeMode)
	{
		z.x -= fractal->IFS.offset.x * (fractal->IFS.scale - 1.0);
		z.y -= fractal->IFS.offset.y * (fractal->IFS.scale - 1.0);
		if (z.z > 0.5 * fractal->IFS.offset.z * (fractal->IFS.scale - 1.0)) z.z -= fractal->IFS.offset.z
				* (fractal->IFS.scale - 1.0);
	}
	else
	{
		z -= fractal->IFS.offset * (fractal->IFS.scale - 1.0);
	}

	aux.ifsDE *= fractal->IFS.scale;
}

void MandelboxVaryScale4DIteration(CVector3 &z, double &w, const cFractal *fractal,
		sMandelboxAux &aux)
{
	aux.actualScale = aux.actualScale
			+ fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);
	CVector3 oldz = z;
	z.x = fabs(z.x + fractal->mandelboxVary4D.fold) - fabs(z.x - fractal->mandelboxVary4D.fold) - z.x;
	z.y = fabs(z.y + fractal->mandelboxVary4D.fold) - fabs(z.y - fractal->mandelboxVary4D.fold) - z.y;
	z.z = fabs(z.z + fractal->mandelboxVary4D.fold) - fabs(z.z - fractal->mandelboxVary4D.fold) - z.z;
	w = fabs(w + fractal->mandelboxVary4D.fold) - fabs(w - fractal->mandelboxVary4D.fold) - w;
	if (z.x != oldz.x) aux.mboxColor += fractal->mandelbox.color.factor.x;
	if (z.y != oldz.y) aux.mboxColor += fractal->mandelbox.color.factor.y;
	if (z.z != oldz.z) aux.mboxColor += fractal->mandelbox.color.factor.z;
	double rr = pow(z.x * z.x + z.y * z.y + z.z * z.z + w * w, fractal->mandelboxVary4D.rPower);
	double m = aux.actualScale;
	if (rr < fractal->mandelboxVary4D.minR * fractal->mandelboxVary4D.minR)
	{
		m = aux.actualScale / (fractal->mandelboxVary4D.minR * fractal->mandelboxVary4D.minR);
		aux.mboxColor += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < 1.0)
	{
		m = aux.actualScale / rr;
		aux.mboxColor += fractal->mandelbox.color.factorSp2;
	}
	z = z * m;
	w = w * m + fractal->mandelboxVary4D.wadd;
	aux.mboxDE = aux.mboxDE * fabs(m) + 1.0;
}

void AexionIteration(CVector3 &z, double &w, const cFractal *fractal, sAexionAux &aux)
{
	if (aux.iterNo == 0)
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

void HypercomplexIteration(CVector3 &z, double &w)
{
	CVector3 newz(z.x * z.x - z.y * z.y - z.z * z.z - w * w,
								2.0 * z.x * z.y - 2.0 * w * z.z,
								2.0 * z.x * z.z - 2.0 * z.y * w);
	double neww = 2.0 * z.x * w - 2.0 * z.y * z.z;
	z = newz;
	w = neww;
}

void QuaternionIteration(CVector3 &z, double &w)
{
	CVector3 newz(z.x * z.x - z.y * z.y - z.z * z.z - w * w, 2.0 * z.x * z.y, 2.0 * z.x * z.z);
	double neww = 2.0 * z.x * w;
	z = newz;
	w = neww;
}

void BenesiIteration(CVector3 &z, CVector3 &c)
{
	double r1 = z.y * z.y + z.z * z.z;
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

void BristorbrotIteration(CVector3 &z)
{
	double newx = z.x * z.x - z.y * z.y - z.z * z.z;
	double newy = z.y * (2.0 * z.x - z.z);
	double newz = z.z * (2.0 * z.x + z.y);
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

void IdesIteration(CVector3 &z)
{
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double newx = x2 - 0.5 * (y2 + z2) + z.x;
	double newy = 2.0 * z.x * z.y * z.z + z.y;
	double newz = z2 - 0.5 * (x2 + y2) + z.z;
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

void Ides2Iteration(CVector3 &z, CVector3 &c)
{
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double newx = x2 - 0.5 * (y2 + z2) + c.x;
	double newy = 2.0 * z.x * z.y * z.z + c.y;
	double newz = z2 - 0.5 * (x2 + y2) + c.z;
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

void BuffaloIteration(CVector3 &z, const cFractal *fractal)
{
	if (fractal->buffalo.preabsx)
	{
		z.x = fabs(z.x);
	}
	if (fractal->buffalo.preabsy)
	{
		z.y = fabs(z.y);
	}
	if (fractal->buffalo.preabsz)
	{
		z.z = fabs(z.z);
	}
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double temp = 1.0 - z2 / (x2 + y2);
	double newx = (x2 - y2) * temp;
	double newy = 2.0 * z.x * z.y * temp;
	double newz;
	if (fractal->buffalo.posz)
	{
		newz = 2.0 * z.z * sqrt(x2 + y2);
	}
	else
	{
		newz = -2.0 * z.z * sqrt(x2 + y2);
	}
	if (fractal->buffalo.absx)
	{
		z.x = fabs(newx);
	}
	else
	{
		z.x = newx;
	}
	if (fractal->buffalo.absy)
	{
		z.y = fabs(newy);
	}
	else
	{
		z.y = newy;
	}
	if (fractal->buffalo.absz)
	{
		z.z = fabs(newz);
	}
	else
	{
		z.z = newz;
	}
}

/* http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/ */
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

/* http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/ */
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

/* Makin3D-2 found through the another shot at the holy grail topic at ff */
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

/* MsltoeSym2 from mbulb3d, also somewhere on fractalforums */
void MsltoeSym2(CVector3 &z, const cFractal *fractal)
{
	if (fabs(z.y) < fabs(z.z))
	{
		double newy = z.z;
		double newz = z.y;
		z.y = newy;
		z.z = newz;
	}
	if (z.y > z.z)
	{
		z.x = -z.x;
	}
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double zr = 1 - z.z * z.z / (x2 + y2 + z2);
	double newx = (x2 - y2) * zr;
	double newy = 2 * z.x * z.y * zr * fractal->msltoeSym2.y_multiplier;
	double newz = 2 * z.z * sqrt(x2 + z2);
	z.x = newx;
	z.y = newy;
	z.z = newz;
}




//------------AexionOctopus  --------------------------------

//http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/

void AexionOctopusIteration(CVector3 &z, const cFractal *fractal)
{
  CVector3 tempN;
  tempN.x = -( 1.25 * z.x * z.z - 0.3075);
  tempN.y = -(z.x * z.x - z.z * z.z - 0.3);
  tempN.z = z.y;
  if (fractal->aexionOctopus.enabledFabsx)
  {
    tempN.x = fabs(tempN.x);
  }
  if ( fractal->aexionOctopus.enabledFabsy)
  {
    tempN.y = fabs(tempN.y);
  }
  if ( fractal->aexionOctopus.enabledFabsz)
  {
    tempN.z = fabs(tempN.z);
  }
  z = tempN;
}






//------------MANDELBULB EXTENDED--------------------------------
void Mandelbulb5Iteration(CVector3 &z, CVector3 &c, double minimumR, int &i,
		const cFractal *fractal, sExtendedAux &aux)
{
	//boxFold1
	boxFoldTransform3D(fractal->transform.boxFold1, z, i, aux);

	//mainRotation1
	mainRotationTransform3D(fractal->transform.mainRotation1, z, i);

	// z =(fabs( z + const1A.) * const1.B) + z * constC.;
	fabsAddConstantTransform3D(fractal->transform.fabsAddConstant1, z, i);

	//MAIN FORMULA ORIGINAL1
	// mandelbulb 3D 1
	mandelbulbOriginalTransform3D(fractal->transform.mandelbulbOriginal1, z, i, aux);

	// z = z + ( c * const.); 1
	constantMultiplierOriginalTransform3D(fractal->transform.constantMultiplierOriginal1, z, c, i);

	//  -fabs( z - constA.) * const.B; 1
	fabsSubConstantTransform3D(fractal->transform.fabsSubConstant1, z, i);

	// z = fabs( z + const.A ) + ( z * const.B ) + const.C; 1
	fabsFormulaZABTransform3D(fractal->transform.fabsFormulaZAB1, z, i);

	// z = z + const; 1
	additionConstantTransform3D(fractal->transform.additionConstant1, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) - z; 1
	fabsFormulaABTransform3D(fractal->transform.fabsFormulaAB1, z, i);

	//mainRotation; 2
	mainRotationTransform3D(fractal->transform.mainRotation2, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 2
	fabsFormulaABCDTransform3D(fractal->transform.fabsFormulaABCD2, z, i);

	// mandelbulb 3D 1
	mandelbulbTransform3D(fractal->transform.mandelbulb1, z, i, aux);

	//mainRotation; 3
	mainRotationTransform3D(fractal->transform.mainRotation3, z, i);

	// z = z + c * const; 1
	constantMultiplierTransform3D(fractal->transform.constantMultiplier1, z, c, i);

	// z = z + const; 2
	additionConstantTransform3D(fractal->transform.additionConstant2, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) - z; 2
	fabsFormulaABTransform3D(fractal->transform.fabsFormulaAB2, z, i);

	//boxFold; 2
	boxFoldTransform3D(fractal->transform.boxFold2, z, i, aux);

	//coloring parameters 1
	coloringParametersTransform3D(fractal->transform.coloringParameters1, z, minimumR, i, aux);

	//color trial 1
	colorTrialTransform3D(fractal->transform.colorTrial1, z, i, aux);
}

// -------------- MANDELBOX EXTENDED----------------------------
void Mandelbox103Iteration(CVector3 &z, CVector3 &c, double minimumR, int &i,
		const cFractal *fractal, sExtendedAux &aux)
{
	//boxFold1 Original (enabled); 1
	boxFoldOriginalTransform3D(fractal->transform.boxFoldOriginal1, z, i, aux);

	// sphericalFold1 Original (enabled); 1
	sphericalFoldOriginalTransform3D(fractal->transform.sphericalFoldOriginal1, z, i, aux);

	//scale Original (enabled); 1
	scaleOriginalTransform3D(fractal->transform.scaleOriginal1, z, i, aux);

	//mainRotation1
	mainRotationTransform3D(fractal->transform.mainRotation1, z, i);

	//(fabs( z + const1A.) * const1.B) + z * constC.;
	fabsAddConstantTransform3D(fractal->transform.fabsAddConstant1, z, i);

	// z = z + ( c * const.); Original (enabled); 1
	constantMultiplierOriginalTransform3D(fractal->transform.constantMultiplierOriginal1, z, c, i);

	// z = z + const; 1
	additionConstantTransform3D(fractal->transform.additionConstant1, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 1
	fabsFormulaABCDTransform3D(fractal->transform.fabsFormulaABCD1, z, i);

	//MAIN FORMULA
	//boxFold; 1
	boxFoldTransform3D(fractal->transform.boxFold1, z, i, aux);

	// sphericalFold1
	sphericalFoldTransform3D(fractal->transform.sphericalFold1, z, i, aux);

	//scale; 1
	scaleTransform3D(fractal->transform.scale1, z, i, aux);

	//mainRotation2
	mainRotationTransform3D(fractal->transform.mainRotation2, z, i);

	// z = z + c * const; 1
	constantMultiplierTransform3D(fractal->transform.constantMultiplier1, z, c, i);

	// z = z + const; 2
	additionConstantTransform3D(fractal->transform.additionConstant2, z, i);

	//mainRotation; 3
	mainRotationTransform3D(fractal->transform.mainRotation3, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 2
	fabsFormulaABCDTransform3D(fractal->transform.fabsFormulaABCD2, z, i);

	// Iteration weight z  =  (  z * const.Z) + (  zA * Const.A) + ( zB * Const.B);1
	iterationWeightTransform3D(fractal->transform.iterationWeight1, z, i);

	//coloring parameters 1
	coloringParametersTransform3D(fractal->transform.coloringParameters1, z, minimumR, i, aux);

	//color trial 1
	colorTrialTransform3D(fractal->transform.colorTrial1, z, i, aux);
}

// -------------- QUATERNION EXTENDED----------------------------
void Quaternion104Iteration(CVector4 &z, const CVector4 &c, int &i, const cFractal *fractal,
		sExtendedAux &aux)
{
	//boxFold1
	boxFoldTransform4D(fractal->transform.boxFold1, z, i, aux);

	// sphericalFold1
	sphericalFoldTransform4D(fractal->transform.sphericalFold1, z, i, aux);

	//scale; 1
	scaleTransform4D(fractal->transform.scale1, z, i, aux);

	//mainRotation1
	mainRotationTransform4D(fractal->transform.mainRotation1, z, i);

	//(fabs( z + const1A.) * const1.B) + z * constC.;
	fabsAddConstantTransform4D(fractal->transform.fabsAddConstant1, z, i);

	//quaternionORIGINAL1 Enabled
	quaternionOriginalTransform4D(fractal->transform.quaternionOriginal1, z, i);

	// z = z + ( c * const.); 1
	constantMultiplierOriginalTransform4D(fractal->transform.constantMultiplierOriginal1, z, c, i);

	// z = z + const; 1
	additionConstantTransform4D(fractal->transform.additionConstant1, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 1
	fabsFormulaABCDTransform4D(fractal->transform.fabsFormulaABCD1, z, i);

	//mainRotation2
	mainRotationTransform4D(fractal->transform.mainRotation2, z, i);

	//MAIN FORMULA 2
	//quaternion1
	quaternionTransform4D(fractal->transform.quaternion1, z, i);

	//boxFold; 2
	boxFoldTransform4D(fractal->transform.boxFold2, z, i, aux);

	// sphericalFold2
	sphericalFoldTransform4D(fractal->transform.sphericalFold2, z, i, aux);

	//scale; 2
	scaleTransform4D(fractal->transform.scale2, z, i, aux);

	//mainRotation; 3
	mainRotationTransform4D(fractal->transform.mainRotation3, z, i);

	// z = z + ( c * const.); 1
	constantMultiplierTransform4D(fractal->transform.constantMultiplier1, z, c, i);

	// z = z + const; 2
	additionConstantTransform4D(fractal->transform.additionConstant2, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 2
	fabsFormulaABCDTransform4D(fractal->transform.fabsFormulaABCD2, z, i);

	// Iteration weight z  =  (  z * const.Z) + (  zA * Const.A) + ( zB * Const.B);
	iterationWeightTransform4D(fractal->transform.iterationWeight1, z, i);

}

// -------------- MENGER SPONGE EXTENDED----------------------------
void MengerSponge105Iteration(CVector3 &z, CVector3 &c, double minimumR, int &i,
		const cFractal *fractal, sExtendedAux &aux)
{
	// Benesi MagTransformOne); 1
	benesiMagTransformOneTransform3D(fractal->transform.benesiMagTransformOne1, z, i, aux);

	//mengerSpongeORIGINAL1 Enabled
	mengerSpongeOriginalTransform3D(fractal->transform.mengerSpongeOriginal1, z, i, aux);

	//boxOffset1
	boxOffsetTransform3D(fractal->transform.boxOffset1, z, i, aux);

	//boxFoldMulti1
	boxFoldMultiTransform3D(fractal->transform.boxFoldMulti1, z, i, aux);

	//boxFold1
	boxFoldTransform3D(fractal->transform.boxFold1, z, i, aux);

	// sphericalOffset1
	sphericalOffsetTransform3D(fractal->transform.sphericalOffset1, z, i, aux);

	// sphericalFold1
	sphericalFoldTransform3D(fractal->transform.sphericalFold1, z, i, aux);

	//mainRotation1
	mainRotationTransform3D(fractal->transform.mainRotation1, z, i);

	//scale; 1
	scaleTransform3D(fractal->transform.scale1, z, i, aux);

	//(fabs( z + const1A.) * const1.B) + z * constC.;
	fabsAddConstantTransform3D(fractal->transform.fabsAddConstant1, z, i);

	//boxConstantMultiplier with enable fabs c , if (z > 0)  z = z +  (fabs(c) * const. );  else  z = z - fabs(c) * const. ); 1
	boxConstantMultiplierTransform3D(fractal->transform.boxConstantMultiplier1, z, c, i);

	//mainRotation2
	mainRotationTransform3D(fractal->transform.mainRotation2, z, i);

	// z = z + ( c * const.); 1
	constantMultiplierTransform3D(fractal->transform.constantMultiplier1, z, c, i);

	// Benesi MagTransformOne); 2
	benesiMagTransformOneTransform3D(fractal->transform.benesiMagTransformOne2, z, i, aux);

	// z = z + const; 1
	additionConstantTransform3D(fractal->transform.additionConstant1, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 1
	fabsFormulaABCDTransform3D(fractal->transform.fabsFormulaABCD1, z, i);

	//MENGER SPONGE 1
	mengerSpongeTransform3D(fractal->transform.mengerSponge1, z, i, aux);

	//boxOffset 2
	boxOffsetTransform3D(fractal->transform.boxOffset2, z, i, aux);

	//boxFold2
	boxFoldTransform3D(fractal->transform.boxFold2, z, i, aux);

	// sphericalOffset2
	sphericalOffsetTransform3D(fractal->transform.sphericalOffset2, z, i, aux);

	// sphericalFold2
	sphericalFoldTransform3D(fractal->transform.sphericalFold2, z, i, aux);

	//mainRotation; 3
	mainRotationTransform3D(fractal->transform.mainRotation3, z, i);

	//scale; 2
	scaleTransform3D(fractal->transform.scale2, z, i, aux);

	//boxConstantMultiplier with enable fabs 2
	boxConstantMultiplierTransform3D(fractal->transform.boxConstantMultiplier2, z, c, i);

	// z = z + c * const; 2
	constantMultiplierTransform3D(fractal->transform.constantMultiplier2, z, c, i);

	// z = z + const; 2
	additionConstantTransform3D(fractal->transform.additionConstant2, z, i);

	//MENGER SPONGE 2
	mengerSpongeTransform3D(fractal->transform.mengerSponge2, z, i, aux);

	//mainRotation; 4
	mainRotationTransform3D(fractal->transform.mainRotation4, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 2
	fabsFormulaABCDTransform3D(fractal->transform.fabsFormulaABCD2, z, i);

	// Iteration weight z  =  (  z * const.Z) + (  zA * Const.A) + ( zB * Const.B);1
	iterationWeightTransform3D(fractal->transform.iterationWeight1, z, i);

	//coloring parameters 1
	coloringParametersTransform3D(fractal->transform.coloringParameters1, z, minimumR, i, aux);

	//color trial 1
	//colorTrialTransform3D(fractal->transform.colorTrial1, z, sample0, sample1, sample2, sample3, sample4, sample5, sample6, i, aux);
	colorTrialTransform3D(fractal->transform.colorTrial1, z, i, aux);
}

//------------MANDELBULB6Beta  Trial Transforms--------------------------------
void Mandelbulb6BetaIteration(CVector3 &z, CVector3 &c, double minimumR, int &i,
		const cFractal *fractal, sExtendedAux &aux)
{
	// Benesi MagTransformOne); 1
	benesiMagTransformOneTransform3D(fractal->transform.benesiMagTransformOne1, z, i, aux);

	//boxOffset1
	boxOffsetTransform3D(fractal->transform.boxOffset1, z, i, aux);

	//boxFold1
	boxFoldTransform3D(fractal->transform.boxFold1, z, i, aux);

	//mainRotation1
	mainRotationTransform3D(fractal->transform.mainRotation1, z, i);

	// sphericalOffset1
	sphericalOffsetTransform3D(fractal->transform.sphericalOffset1, z, i, aux);

	// sphericalFold1
	sphericalFoldTransform3D(fractal->transform.sphericalFold1, z, i, aux);

	//scale; 1
	//scaleTransform3D(fractal->transform.scale1, z, i, aux);

	//variableScale; 1
	variableScaleTransform3D(fractal->transform.variableScale1, z, i, aux);

	// z =(fabs( z + const1A.) * const1.B) + z * constC.;
	fabsAddConstantTransform3D(fractal->transform.fabsAddConstant1, z, i);

	// Benesi FastPwr2PineTree); 1
	benesiFastPwr2PineTreeTransform3D(fractal->transform.benesiFastPwr2PineTree1, z, c, i, aux);

  // Benesi MagTransformTwo); 1
  benesiMagTransformTwoTransform3D(fractal->transform.benesiMagTransformTwo1, z, i, aux);

	//MAIN FORMULA ORIGINAL1
	// mandelbulb 3D 1
	mandelbulbOriginalTransform3D(fractal->transform.mandelbulbOriginal1, z, i, aux);

	// mandelbulb multi 3D 1
	mandelbulbMultiTransform3D(fractal->transform.mandelbulbMulti1, z, i, aux);

	// z = z + ( c * const.); 1
	constantMultiplierOriginalTransform3D(fractal->transform.constantMultiplierOriginal1, z, c, i);

	//  -fabs( z - constA.) * const.B;
	fabsSubConstantTransform3D(fractal->transform.fabsSubConstant1, z, i);

	// z = fabs( z + const.A ) + ( z * const.B ) + const.C; 1
	fabsFormulaZABTransform3D(fractal->transform.fabsFormulaZAB1, z, i);

	// z = z + const; 1
	additionConstantTransform3D(fractal->transform.additionConstant1, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) - z; 1
	fabsFormulaABTransform3D(fractal->transform.fabsFormulaAB1, z, i);

	//mainRotation; 2
	mainRotationTransform3D(fractal->transform.mainRotation2, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 2
	fabsFormulaABCDTransform3D(fractal->transform.fabsFormulaABCD2, z, i);

	// Benesi MagTransformOne); 2
	benesiMagTransformOneTransform3D(fractal->transform.benesiMagTransformOne2, z, i, aux);

	// mandelbulb variable power 3D 1
	variableMandelbulbPowerTransform3D(fractal->transform.variableMandelbulbPower1, z, i, aux);

	// mandelbulb multi 3D 2
	mandelbulbMultiTransform3D(fractal->transform.mandelbulbMulti2, z, i, aux);

	//mainRotation; 3
	mainRotationTransform3D(fractal->transform.mainRotation3, z, i);

	//variableConstantMultiplier 1
	variableConstantMultiplierTransform3D(fractal->transform.variableConstantMultiplier1, z, c, i);

	//boxConstantMultiplier with enable fabs 2
	boxConstantMultiplierTransform3D(fractal->transform.boxConstantMultiplier2, z, c, i);

	// z = z + const; 2
	additionConstantTransform3D(fractal->transform.additionConstant2, z, i);

	//mainRotation; 4
	mainRotationTransform3D(fractal->transform.mainRotation4, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) - z; 2
	fabsFormulaABTransform3D(fractal->transform.fabsFormulaAB2, z, i);

	//boxFold; 2
	boxFoldTransform3D(fractal->transform.boxFold2, z, i, aux);

	// sphericalOffset2
	sphericalOffsetTransform3D(fractal->transform.sphericalOffset2, z, i, aux);

	//coloring parameters 1
	coloringParametersTransform3D(fractal->transform.coloringParameters1, z, minimumR, i, aux);

	//color trial 1
	colorTrialTransform3D(fractal->transform.colorTrial1, z, i, aux);
}

//------------Benesi Transforms --------------------------------
void BenesiTransformsIteration(CVector3 &z, CVector3 &c, double minimumR, int &i,
		const cFractal *fractal, sExtendedAux &aux)
{
	// Benesi MagForwardTransformOne); 1
	benesiMagForwardTransformOneTransform3D(fractal->transform.benesiMagForwardTransformOne1, z, i);

	// Benesi MagTransformOneEnabled); 1
	benesiMagTransformOneEnabledTransform3D(fractal->transform.benesiMagTransformOneEnabled1,
																					z,
																					i,
																					aux);

	// Benesi FastPwr2PineTreeEnabled); 1
	benesiFastPwr2PineTreeEnabledTransform3D(fractal->transform.benesiFastPwr2PineTreeEnabled1,
																					 z,
																					 c,
																					 i,
																					 aux);

	//MENGER SPONGE 1
	mengerSpongeTransform3D(fractal->transform.mengerSponge1, z, i, aux);

	//boxOffset1
	boxOffsetTransform3D(fractal->transform.boxOffset1, z, i, aux);

	//boxFold1
	boxFoldTransform3D(fractal->transform.boxFold1, z, i, aux);

	// sphericalOffset1
	sphericalOffsetTransform3D(fractal->transform.sphericalOffset1, z, i, aux);

	// sphericalFold1
	sphericalFoldTransform3D(fractal->transform.sphericalFold1, z, i, aux);

	// z =(fabs( z + const1A.) * const1.B) + z * constC.;
	fabsAddConstantTransform3D(fractal->transform.fabsAddConstant1, z, i);

	//mainRotation1
	mainRotationTransform3D(fractal->transform.mainRotation1, z, i);

	//variableScale; 1
	variableScaleTransform3D(fractal->transform.variableScale1, z, i, aux);

	// z = z + const; 1
	additionConstantTransform3D(fractal->transform.additionConstant1, z, i);

	//z = z + c * const; 1
	constantMultiplierTransform3D(fractal->transform.constantMultiplier1, z, c, i);

	// Benesi MagBackTransformOne); 1
	benesiMagBackTransformOneTransform3D(fractal->transform.benesiMagBackTransformOne1, z, i);

	// Benesi MagTransformOne); 1
	benesiMagTransformOneTransform3D(fractal->transform.benesiMagTransformOne1, z, i, aux);

	//Benesi Mag TransformOnePlusMinus); 1
	benesiMagTransformOnePlusMinusTransform3D(fractal->transform.benesiMagTransformOnePlusMinus1,
																						z,
																						i,
																						aux);

	// Benesi SphereCube; 1
	sphereCubeTransform3D(fractal->transform.sphereCube1, z, i);

	// Benesi FastPwr2PineTree); 1
	benesiFastPwr2PineTreeTransform3D(fractal->transform.benesiFastPwr2PineTree1, z, c, i, aux);

	// Benesi MagTransformTwo); 1
	benesiMagTransformTwoTransform3D(fractal->transform.benesiMagTransformTwo1, z, i, aux);

	// Benesi MagTransformThree); 1
	benesiMagTransformThreeTransform3D(fractal->transform.benesiMagTransformThree1, z, i);

	// Benesi FastPwr2PineTree); 2
	benesiFastPwr2PineTreeTransform3D(fractal->transform.benesiFastPwr2PineTree2, z, c, i, aux);

	// Benesi MagTransformFour); 1
	benesiMagTransformFourTransform3D(fractal->transform.benesiMagTransformFour1, z, i);

	// Benesi MagTransformFiveB); 1
	benesiMagTransformFiveBTransform3D(fractal->transform.benesiMagTransformFiveB1, z, i);

	// mandelbulb multi 3D 1
	mandelbulbMultiTransform3D(fractal->transform.mandelbulbMulti1, z, i, aux);

	// z = z + c * const; 2
	constantMultiplierTransform3D(fractal->transform.constantMultiplier2, z, c, i);

	// Benesi cubeSphere; 1
	cubeSphereTransform3D(fractal->transform.cubeSphere1, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) - z; 1
	fabsFormulaABTransform3D(fractal->transform.fabsFormulaAB1, z, i);

	//mainRotation; 2
	mainRotationTransform3D(fractal->transform.mainRotation2, z, i);

	//  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 2
	fabsFormulaABCDTransform3D(fractal->transform.fabsFormulaABCD2, z, i);

	// Benesi FastPwr2PineTree); 3
	benesiFastPwr2PineTreeTransform3D(fractal->transform.benesiFastPwr2PineTree3, z, c, i, aux);

	// Benesi MagTransformOne); 2
	benesiMagTransformOneTransform3D(fractal->transform.benesiMagTransformOne2, z, i, aux);

	// mandelbulb variable power 3D 1
	variableMandelbulbPowerTransform3D(fractal->transform.variableMandelbulbPower1, z, i, aux);

	// mandelbulb multi 3D 2
	mandelbulbMultiTransform3D(fractal->transform.mandelbulbMulti2, z, i, aux);

	//mainRotation; 3
	mainRotationTransform3D(fractal->transform.mainRotation3, z, i);

	//variableScale; 2
	variableScaleTransform3D(fractal->transform.variableScale2, z, i, aux);

	//variableConstantMultiplier 1
	variableConstantMultiplierTransform3D(fractal->transform.variableConstantMultiplier1, z, c, i);

	//boxConstantMultiplier with enable fabs 2
	boxConstantMultiplierTransform3D(fractal->transform.boxConstantMultiplier2, z, c, i);

	// z = z + const; 2
	additionConstantTransform3D(fractal->transform.additionConstant2, z, i);

	//mainRotation; 4
	mainRotationTransform3D(fractal->transform.mainRotation4, z, i);

	//boxFold; 2
	boxFoldTransform3D(fractal->transform.boxFold2, z, i, aux);

	// sphericalOffset2
	sphericalOffsetTransform3D(fractal->transform.sphericalOffset2, z, i, aux);

	//coloring parameters 1
	coloringParametersTransform3D(fractal->transform.coloringParameters1, z, minimumR, i, aux);

	//color trial 1
	colorTrialTransform3D(fractal->transform.colorTrial1, z, i, aux);
}

//------------FabsBoxMod  --------------------------------
void FabsBoxModIteration(CVector3 &z, CVector3 &c, int &i, const cFractal *fractal,
		sExtendedAux &aux)
{
	//fabsBoxMod
	fabsBoxModTransform3D(fractal->transform.fabsBoxMod, z, i);

	//boxFold1
	boxFoldTransform3D(fractal->transform.boxFold1, z, i, aux);

	// sphericalFold1 Original (enabled); 1
	sphericalFoldOriginalTransform3D(fractal->transform.sphericalFoldOriginal1, z, i, aux);

	//scale Original (enabled); 1
	scaleOriginalTransform3D(fractal->transform.scaleOriginal1, z, i, aux);

	//mainRotation1
	mainRotationTransform3D(fractal->transform.mainRotation1, z, i);

	// z = z + ( c * const.); Original (enabled); 1
	constantMultiplierOriginalTransform3D(fractal->transform.constantMultiplierOriginal1, z, c, i);
}

//benesiFastPwr2PineTree  3D
//http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/

void BenesiPineTreeIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux)
{
  CVector3 temp = z;
  aux.r = z.Length();
  z *= z;
  double t = 2 * temp.x;
  if (z.y + z.z > 0.0) t = t / sqrt(z.y + z.z);
  else t = 1.0;
  c *= fractal->transformCommon.constantMultiplier100;
  z.x = (z.x - z.y - z.z) + c.x;
  z.z = (t * (z.y - z.z)) + c.y;
  z.y = (2 * t * temp.y * temp.z) + c.z;
  aux.r_dz = aux.r * aux.r_dz * 2.0 + 1.0;
}

//benesiT1PineTree  3D
//http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/

void BenesiT1PineTreeIteration(CVector3 &z, CVector3 &c, int i, const cFractal *fractal, sExtendedAux &aux)
{

  if (fractal->transformCommon.benesiT1Enabled
      && i >= fractal->transformCommon.startIterations
      && i < fractal->transformCommon.stopIterations)
  {
    //CVector3 temp = z;
    double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
    z = CVector3((tempXZ - z.y) * SQRT_1_2,
                 (tempXZ + z.y) * SQRT_1_2,
                 z.x * SQRT_1_3 + z.z * SQRT_2_3);
    z = fabs(z) * fractal->transformCommon.scale3D222;
    if (fractal->transformCommon.rotationEnabled)
    {
      z = fractal->transformCommon.rotationMatrix.RotateVector(z);
    }

    double avgScale = (fabs(fractal->transformCommon.scale3D222.x)
        + fabs(fractal->transformCommon.scale3D222.y) + fabs(fractal->transformCommon.scale3D222.z))
        / 3;  // cheap approximation

    aux.r_dz *= avgScale;
    aux.DE = aux.DE * avgScale + 1.0;
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
  if (z.y + z.z > 0.0) t = t / sqrt(z.y + z.z);
  else t = 1.0;
  if (fractal->transformCommon.addCpixelEnabled)
  {
    c *= fractal->transformCommon.constantMultiplier100;
    z.x = (z.x - z.y - z.z) + c.x;
    z.z = (t * (z.y - z.z)) + c.y;  // Cy Cx swap
    z.y = (2 * t * temp.y * temp.z) + c.z;
  }
  if (fractal->transformCommon.juliaMode)
    {
      z += fractal->transformCommon.juliaC;
    }
  aux.r_dz = aux.r * aux.r_dz * 2.0 + 1.0;
}

// mandelbulbMulti 3D
void MandelbulbMultiIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
  aux.r = z.Length();

  //if (i == mandelbulbMulti.control.startIterations)
  //{
  //  z = mandelbulbMulti.mainRot.RotateVector(z);
  //}
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
  if (aux.r < 1e-21) aux.r = 1e-21;
  if (v3 < 1e-21 && v3 > -1e-21) v3 = (v3 > 0) ? 1e-21 : -1e-21;
  if (fractal->mandelbulbMulti.acosOrasin == sFractalMandelbulbMulti::acos)
  {
    th0 += acos(v1 / aux.r);
  }
  else
  {
    th0 += asin(v1 / aux.r);
  }

  if (fractal->mandelbulbMulti.atanOratan2 == sFractalMandelbulbMulti::atan)
  {
    ph0 += atan(v2 / v3);
  }
  else
  {
    ph0 += atan2(v2, v3);
  }

  double rp = pow(aux.r, fractal->bulb.power - 1.0);
  double th = th0 * fractal->bulb.power;
  double ph = ph0 * fractal->bulb.power;
  double cth = cos(th);
  aux.r_dz = rp * aux.r_dz * fractal->bulb.power + 1.0;
  rp *= aux.r;
  z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;


}

// --------quaternion3D--------------
void Quaternion3DIteration(CVector3 &z, const cFractal *fractal)
{
  CVector3 newz(fractal->quaternion3D.constantFactor.x
                    * (z.x * z.x - z.y * z.y - z.z * z.z),
                fractal->quaternion3D.constantFactor.y * z.x * z.y,
                fractal->quaternion3D.constantFactor.z * z.x * z.z);
  z = newz;
}
// ----------mengerMod
void MengerModIteration( CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
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
  z *= fractal->mengerMod.scaleFactor;
  z.x -= 2.0 * fractal->mengerMod.constantFactor.x;
  z.y -= 2.0 * fractal->mengerMod.constantFactor.y;
  if (z.z > 1.0) z.z -= 2.0 * fractal->mengerMod.constantFactor.z;
  aux.DE *= fractal->mengerMod.scaleFactor;
}


//DarkBeam's aboxMod1, Inspired from a 2D formula proposed by Kali at the forums here;
//http://www.fractalforums.com/new-theories-and-research/kaliset-plus-boxfold-nice-new-2d-fractal/msg33670/#new

void AboxMod1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale = aux.actualScale
			+ fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	z.x = fractal->mandelbox.foldingValue
			- fabs(fabs(z.x + fractal->aboxMod1.foldM.x) - fractal->mandelbox.foldingValue)
			- fabs(fractal->aboxMod1.foldM.x);
	z.y = fractal->mandelbox.foldingValue
			- fabs(fabs(z.y + fractal->aboxMod1.foldM.y) - fractal->mandelbox.foldingValue)
			- fabs(fractal->aboxMod1.foldM.y);
	z.z = fractal->mandelbox.foldingValue
			- fabs(fabs(z.z + fractal->aboxMod1.foldM.z) - fractal->mandelbox.foldingValue)
			- fabs(fractal->aboxMod1.foldM.z);
	// rr = pow(x*x + y*y + z*z + w*w, R_power) <- removed to speedup
	double rr = (z.x * z.x + z.y * z.y + z.z * z.z);
	if (rr < 1e-21) rr = 1e-21;
	double m;
	double sqrtMinR = sqrt(fractal->aboxMod1.minR);
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


//------------AboxModKali  --------------------------------
//http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/
void AboxModKaliIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
  z = fractal->transformCommon.additionConstant111 - fabs(z);
  double rr = z.x * z.x + z.y * z.y + z.z * z.z;
  if(rr < 1e-21) rr = 1e-21;
  double MinR = fractal->mandelbox.foldingSphericalMin;
  if (MinR > -1e-21 && MinR < 1e-21) MinR = (MinR > 0) ? 1e-21 : -1e-21;
  double m;
  if (rr < (MinR)) m = fractal->mandelbox.scale/(MinR);
  else
  {
   if (rr < 1) m =  fractal->mandelbox.scale/rr;
   else m = fractal->mandelbox.scale;
  }
  z = z * m;
  aux.DE = aux.DE * fabs(m) + 1.0;
}

/* GeneralizedFoldBox, ref: http://www.fractalforums.com/new-theories-and-research/generalized-box-fold/ */
void GeneralizedFoldBoxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux)
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
					aux.mboxColor += fractal->mandelbox.color.factor.z;
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
					aux.mboxColor += fractal->mandelbox.color.factor.y;
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
				aux.mboxColor += fractal->mandelbox.color.factor.x;
			}
		}
	} // outside solid

	double r = z.Length();
	double r2 = r * r;

	z += fractal->mandelbox.offset;

	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.mboxDE *= fractal->mandelbox.mboxFactor1;
		aux.mboxColor += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.mboxDE *= tglad_factor2;
		aux.mboxColor += fractal->mandelbox.color.factorSp2;
	}

	z -= fractal->mandelbox.offset;

	if (fractal->mandelbox.mainRotationEnabled)
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	z = z * fractal->mandelbox.scale;
	aux.mboxDE = aux.mboxDE * fabs(fractal->mandelbox.scale) + 1.0;
}

void BoxFolding(CVector3 &z, const sFractalFoldings *foldings, double &foldColor)
{
	if (z.x > foldings->boxLimit)
	{
		z.x = foldings->boxValue - z.x;
		foldColor *= 0.9;
	}
	else if (z.x < -foldings->boxLimit)
	{
		z.x = -foldings->boxValue - z.x;
		foldColor *= 0.9;
	}
	if (z.y > foldings->boxLimit)
	{
		z.y = foldings->boxValue - z.y;
		foldColor *= 0.9;
	}
	else if (z.y < -foldings->boxLimit)
	{
		z.y = -foldings->boxValue - z.y;
		foldColor *= 0.9;
	}
	if (z.z > foldings->boxLimit)
	{
		z.z = foldings->boxValue - z.z;
		foldColor *= 0.9;
	}
	else if (z.z < -foldings->boxLimit)
	{
		z.z = -foldings->boxValue - z.z;
		foldColor *= 0.9;
	}
}

void SphericalFolding(CVector3 &z, const sFractalFoldings *foldings, double &foldColor,
		double &foldDE, double r)
{
	double fR2_2 = foldings->sphericalOuter * foldings->sphericalOuter;
	double mR2_2 = foldings->sphericalInner * foldings->sphericalInner;
	double r2_2 = r * r;
	double fold_factor1_2 = fR2_2 / mR2_2;

	if (r2_2 < mR2_2)
	{
		z = z * fold_factor1_2;
		foldDE *= fold_factor1_2;
		foldColor *= 0.9;
	}
	else if (r2_2 < fR2_2)
	{
		double fold_factor2_2 = fR2_2 / r2_2;
		z = z * fold_factor2_2;
		foldDE *= fold_factor2_2;
		foldColor *= 0.9;
	}
}

//***************** NEW TRANSFORM FORMULAS ******************

void TransformAdditionConstantIteration(CVector3 &z, const cFractal *fractal)
{
  z += fractal->transformCommon.additionConstant000;
}

void TransformAdditionCpixelIteration(CVector3 &z, CVector3 &c, const cFractal *fractal)
{
  z +=  c * fractal->transformCommon.constantMultiplier111;
}
void TransformAdditionCpixelAxisSwapIteration(CVector3 &z, CVector3 &c, const cFractal *fractal)
{
  switch (fractal->mandelbulbMulti.orderOfxyz)
  {
    case sFractalMandelbulbMulti::xyz:
    default:
      c = CVector3( c.x, c.y, c.z);
      break;
    case sFractalMandelbulbMulti::xzy:
      c = CVector3( c.x, c.z, c.y);
      break;
    case sFractalMandelbulbMulti::yxz:
      c = CVector3( c.y, c.x, c.z);
      break;
    case sFractalMandelbulbMulti::yzx:
      c = CVector3( c.y, c.z, c.x);
      break;
    case sFractalMandelbulbMulti::zxy:
      c = CVector3( c.z, c.x, c.y);
      break;
    case sFractalMandelbulbMulti::zyx:
      c = CVector3( c.z, c.y, c.x);
      break;
  }
  z +=  c * fractal->transformCommon.constantMultiplier111;
}
//benesiT1  3D
//http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/

void TransformBenesiT1Iteration(CVector3 &z,  const cFractal *fractal, sExtendedAux &aux)
{
  double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
  z = CVector3((tempXZ - z.y) * SQRT_1_2,
               (tempXZ + z.y) * SQRT_1_2,
               z.x * SQRT_1_3 + z.z * SQRT_2_3);
  z = fabs(z) * fractal->transformCommon.scale3D222;
  if (fractal->transformCommon.rotationEnabled)
  {
    z = fractal->transformCommon.rotationMatrix.RotateVector(z);
  }

  double avgScale = (fabs(fractal->transformCommon.scale3D222.x)
      + fabs(fractal->transformCommon.scale3D222.y) + fabs(fractal->transformCommon.scale3D222.z))
      / 3;  // cheap approximation

  aux.r_dz *= avgScale;
  aux.DE = aux.DE * avgScale + 1.0;
  tempXZ = (z.y + z.x) * SQRT_1_2;

  z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3,
               (z.y - z.x) * SQRT_1_2,
               z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
  z = z - fractal->transformCommon.offset200;
}
//benesiT1Mod  3D based on benesiT1
//http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/

void TransformBenesiT1ModIteration(CVector3 &z,  const cFractal *fractal, sExtendedAux &aux)
{
  double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
  z = CVector3((tempXZ - z.y) * SQRT_1_2,
               (tempXZ + z.y) * SQRT_1_2,
               z.x * SQRT_1_3 + z.z * SQRT_2_3);
  z *= fractal->transformCommon.scale3D333;
  z = (fabs(z + fractal->transformCommon.additionConstant111)
        - fabs(z - fractal->transformCommon.additionConstant111) - z);

  if (fractal->transformCommon.rotationEnabled)
  {
    z = fractal->transformCommon.rotationMatrix.RotateVector(z);
  }

  double avgScale = (fabs(fractal->transformCommon.scale3D333.x)
      + fabs(fractal->transformCommon.scale3D333.y) + fabs(fractal->transformCommon.scale3D333.z))
      / 3;  // cheap approximation

  aux.r_dz *= avgScale;
  aux.DE = aux.DE * avgScale + 1.0;
  tempXZ = (z.y + z.x) * SQRT_1_2;

  z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3,
               (z.y - z.x) * SQRT_1_2,
               z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}
//benesiT2  3D
//http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/

void TransformBenesiT2Iteration(CVector3 &z,  const cFractal *fractal, sExtendedAux &aux)
{
  double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
  z = CVector3((tempXZ - z.y) * SQRT_1_2,
               (tempXZ + z.y) * SQRT_1_2,
               z.x * SQRT_1_3 + z.z * SQRT_2_3);

  //  Change this for different transforms
  CVector3 tempV2 = z;
  tempV2.x = sqrt(z.y * z.y + z.z * z.z);
  tempV2.y = sqrt(z.x * z.x + z.z * z.z);  // switching, squared, sqrt
  tempV2.z = sqrt(z.x * z.x + z.y * z.y);
  z = fabs(tempV2 - fractal->transformCommon.additionConstant111) * fractal->transformCommon.scale3D444;
  double avgScale = (fabs(fractal->transformCommon.scale3D444.x) + fabs(fractal->transformCommon.scale3D444.y)
                  + fabs(fractal->transformCommon.scale3D444.z)) / 3; // cheap approximation

  if (fractal->transformCommon.rotationEnabled)
  {
    z = fractal->transformCommon.rotationMatrix.RotateVector(z);
  }

  //aux.r_dz *= avgScale;
  aux.DE = aux.DE * avgScale + 1.0;
  tempXZ = (z.y + z.x) * SQRT_1_2;

  z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3,
               (z.y - z.x) * SQRT_1_2,
               z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}
void TransformRotationIteration(CVector3 &z, const cFractal *fractal)
{
	z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

void TransformScaleIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	z *= fractal->transformCommon.scale;
	aux.DE *= fractal->transformCommon.scale; //prepared for future analytic DE for hybrids
}

void TransformScale3DIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
  z *= fractal->transformCommon.scale3D111;
  aux.DE *= fractal->transformCommon.scale3D111.Length(); //prepared for future analytic DE for hybrids
}

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

void TransformBoxOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
  if (z.x > 0)
  {
    z.x = z.x + fractal->transformCommon.additionConstant000.x;
    //aux.color += boxOffset.color.factor.x;
  }
  else
  {
    z.x = z.x - fractal->transformCommon.additionConstant000.x;
    //aux.color += boxOffset.color.factor.x;
  }
  if (z.y > 0)
  {
    z.y = z.y + fractal->transformCommon.additionConstant000.y;
    //aux.color += boxOffset.color.factor.y;
  }
  else
  {
    z.y = z.y - fractal->transformCommon.additionConstant000.y;
   // aux.color += boxOffset.color.factor.y;
  }
  if (z.z > 0)
  {
    z.z = z.z + fractal->transformCommon.additionConstant000.z;
   // aux.color += boxOffset.color.factor.z;
  }
  else
  {
    z.z = z.z - fractal->transformCommon.additionConstant000.z;
   // aux.color += boxOffset.color.factor.z;
  }
}

void TransformSphericalOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
  double lengthTempZ = -z.Length();
  if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
  z *= 1 + fractal->transformCommon.offset / lengthTempZ;
  z *= fractal->transformCommon.scale;
  aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
}



