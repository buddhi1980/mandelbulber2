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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "fractal_formulas.hpp"
#include "fractal.h"
#include "common_math.h"

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
			aux.mboxColor += fractal->mandelbox.colorFactor.x;
			lockout = true;
		}
		z = fractal->mandelbox.rotinv[0][0].RotateVector(z);

		z = fractal->mandelbox.rot[1][0].RotateVector(z);
		if (!lockout && z.x < -fractal->mandelbox.foldingLimit)
		{
			z.x = -fractal->mandelbox.foldingValue - z.x;
			aux.mboxColor += fractal->mandelbox.colorFactor.x;
		}
		z = fractal->mandelbox.rotinv[1][0].RotateVector(z);

		lockout = false;
		z = fractal->mandelbox.rot[0][1].RotateVector(z);
		if (z.y > fractal->mandelbox.foldingLimit)
		{
			z.y = fractal->mandelbox.foldingValue - z.y;
			aux.mboxColor += fractal->mandelbox.colorFactor.y;
			lockout = true;
		}
		z = fractal->mandelbox.rotinv[0][1].RotateVector(z);

		z = fractal->mandelbox.rot[1][1].RotateVector(z);
		if (!lockout && z.y < -fractal->mandelbox.foldingLimit)
		{
			z.y = -fractal->mandelbox.foldingValue - z.y;
			aux.mboxColor += fractal->mandelbox.colorFactor.y;
		}
		z = fractal->mandelbox.rotinv[1][1].RotateVector(z);

		lockout = false;
		z = fractal->mandelbox.rot[0][2].RotateVector(z);
		if (z.z > fractal->mandelbox.foldingLimit)
		{
			z.z = fractal->mandelbox.foldingValue - z.z;
			aux.mboxColor += fractal->mandelbox.colorFactor.z;
			lockout = true;
		}
		z = fractal->mandelbox.rotinv[0][2].RotateVector(z);

		z = fractal->mandelbox.rot[1][2].RotateVector(z);
		if (!lockout && z.z < -fractal->mandelbox.foldingLimit)
		{
			z.z = -fractal->mandelbox.foldingValue - z.z;
			aux.mboxColor += fractal->mandelbox.colorFactor.z;
		}
		z = fractal->mandelbox.rotinv[1][2].RotateVector(z);
	}
	else
	{
		if (z.x > fractal->mandelbox.foldingLimit)
		{
			z.x = fractal->mandelbox.foldingValue - z.x;
			aux.mboxColor += fractal->mandelbox.colorFactor.x;
		}
		else if (z.x < -fractal->mandelbox.foldingLimit)
		{
			z.x = -fractal->mandelbox.foldingValue - z.x;
			aux.mboxColor += fractal->mandelbox.colorFactor.x;
		}
		if (z.y > fractal->mandelbox.foldingLimit)
		{
			z.y = fractal->mandelbox.foldingValue - z.y;
			aux.mboxColor += fractal->mandelbox.colorFactor.y;
		}
		else if (z.y < -fractal->mandelbox.foldingLimit)
		{
			z.y = -fractal->mandelbox.foldingValue - z.y;
			aux.mboxColor += fractal->mandelbox.colorFactor.y;
		}
		if (z.z > fractal->mandelbox.foldingLimit)
		{
			z.z = fractal->mandelbox.foldingValue - z.z;
			aux.mboxColor += fractal->mandelbox.colorFactor.z;
		}
		else if (z.z < -fractal->mandelbox.foldingLimit)
		{
			z.z = -fractal->mandelbox.foldingValue - z.z;
			aux.mboxColor += fractal->mandelbox.colorFactor.z;
		}
	}

	double r2 = z.Dot(z);

	z += fractal->mandelbox.offset;

	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.mboxDE *= fractal->mandelbox.mboxFactor1;
		aux.mboxColor += fractal->mandelbox.colorFactorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.mboxDE *= tglad_factor2;
		aux.mboxColor += fractal->mandelbox.colorFactorSp2;
	}

	z -= fractal->mandelbox.offset;

	if(fractal->mandelbox.mainRotationEnabled)
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
	aux.mboxColor += (zk1 + zk2) * fractal->mandelbox.colorFactor.x;

	double zk3 = SmoothConditionAGreaterB(z.y, fractal->mandelbox.foldingLimit, sm);
	double zk4 = SmoothConditionALessB(z.y, -fractal->mandelbox.foldingLimit, sm);
	z.y = z.y * (1.0 - zk3) + (fractal->mandelbox.foldingValue - z.y) * zk3;
	z.y = z.y * (1.0 - zk4) + (-fractal->mandelbox.foldingValue - z.y) * zk4;
	aux.mboxColor += (zk3 + zk4) * fractal->mandelbox.colorFactor.y;

	double zk5 = SmoothConditionAGreaterB(z.z, fractal->mandelbox.foldingLimit, sm);
	double zk6 = SmoothConditionALessB(z.z, -fractal->mandelbox.foldingLimit, sm);
	z.z = z.z * (1.0 - zk5) + (fractal->mandelbox.foldingValue - z.z) * zk5;
	z.z = z.z * (1.0 - zk6) + (-fractal->mandelbox.foldingValue - z.z) * zk6;
	aux.mboxColor += (zk5 + zk6) * fractal->mandelbox.colorFactor.z;

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
	aux.mboxColor += rk1 * fractal->mandelbox.colorFactorSp1;
	aux.mboxColor += rk21 * fractal->mandelbox.colorFactorSp2;

	z = fractal->mandelbox.mainRot.RotateVector(z);
	z = z * fractal->mandelbox.scale;

	aux.mboxDE = aux.mboxDE * fabs(fractal->mandelbox.scale) + 1.0;
}

void BoxFoldBulbPow2Iteration(CVector3 &z, const cFractal *fractal)
{
	if (z.x > fractal->foldingIntPow.foldfactor) z.x = fractal->foldingIntPow.foldfactor * 2.0 - z.x;
	else if (z.x < -fractal->foldingIntPow.foldfactor) z.x = -fractal->foldingIntPow.foldfactor * 2.0 - z.x;

	if (z.y > fractal->foldingIntPow.foldfactor) z.y = fractal->foldingIntPow.foldfactor * 2.0 - z.y;
	else if (z.y < -fractal->foldingIntPow.foldfactor) z.y = -fractal->foldingIntPow.foldfactor * 2.0 - z.y;

	if (z.z > fractal->foldingIntPow.foldfactor) z.z = fractal->foldingIntPow.foldfactor * 2.0 - z.z;
	else if (z.z < -fractal->foldingIntPow.foldfactor) z.z = -fractal->foldingIntPow.foldfactor * 2.0 - z.z;

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
				z -= fractal->IFS.direction[i] * (2.0 * (length - fractal->IFS.distance[i]) * fractal->IFS.intensity[i]);
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
		if (z.z > 0.5 * fractal->IFS.offset.z * (fractal->IFS.scale - 1.0)) z.z -= fractal->IFS.offset.z * (fractal->IFS.scale - 1.0);
	}
	else
	{
		z -= fractal->IFS.offset * (fractal->IFS.scale - 1.0);
	}

	aux.ifsDE *= fractal->IFS.scale;
}

void MandelboxVaryScale4DIteration(CVector3 &z, double &w, const cFractal *fractal, sMandelboxAux &aux)
{
	aux.actualScale = aux.actualScale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);
	CVector3 oldz = z;
	z.x = fabs(z.x + fractal->mandelboxVary4D.fold) - fabs(z.x - fractal->mandelboxVary4D.fold) - z.x;
	z.y = fabs(z.y + fractal->mandelboxVary4D.fold) - fabs(z.y - fractal->mandelboxVary4D.fold) - z.y;
	z.z = fabs(z.z + fractal->mandelboxVary4D.fold) - fabs(z.z - fractal->mandelboxVary4D.fold) - z.z;
	w = fabs(w + fractal->mandelboxVary4D.fold) - fabs(w - fractal->mandelboxVary4D.fold) - w;
	if (z.x != oldz.x) aux.mboxColor += fractal->mandelbox.colorFactor.x;
	if (z.y != oldz.y) aux.mboxColor += fractal->mandelbox.colorFactor.y;
	if (z.z != oldz.z) aux.mboxColor += fractal->mandelbox.colorFactor.z;
	double rr = pow(z.x * z.x + z.y * z.y + z.z * z.z + w * w, fractal->mandelboxVary4D.rPower);
	double m = aux.actualScale;
	if (rr < fractal->mandelboxVary4D.minR * fractal->mandelboxVary4D.minR)
	{
		m = aux.actualScale / (fractal->mandelboxVary4D.minR * fractal->mandelboxVary4D.minR);
		aux.mboxColor += fractal->mandelbox.colorFactorSp1;
	}
	else if (rr < 1.0)
	{
		m = aux.actualScale / rr;
		aux.mboxColor += fractal->mandelbox.colorFactorSp2;
	}
	z = z * m;
	w = w * m + fractal->mandelboxVary4D.wadd;
	aux.mboxDE = aux.mboxDE * fabs(m) + 1.0;
}

void AexionIteration(CVector3 &z, double &w, const cFractal *fractal, sAexionAux &aux)
{
	if(aux.iterNo == 0)
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
	CVector3 newz(z.x * z.x - z.y * z.y - z.z * z.z - w * w, 2.0 * z.x * z.y - 2.0 * w * z.z, 2.0 * z.x * z.z - 2.0 * z.y * w);
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
	double r1 = z.y*z.y + z.z*z.z;
	double newx = 0;
	if(c.x < 0 || z.x < sqrt(r1))
	{
		newx = z.x*z.x - r1;
	}
	else
	{
		newx = -z.x*z.x + r1;
	}
	r1 = - 1.0/sqrt(r1) * 2.0 * fabs(z.x);
	double newy = r1 * (z.y*z.y - z.z*z.z);
	double newz = r1 * 2.0 * z.y * z.z;

	z.x = newx;
	z.y = newy;
	z.z = newz;
}

void BristorbrotIteration(CVector3 &z)
{
	double newx = z.x*z.x - z.y*z.y - z.z*z.z;
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
    double newx =  x2 + 2 * z.y * z.z;
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

void Mandelbulb5Iteration(CVector3 &z, CVector3 &c, int &i, const cFractal *fractal, sMandelbulbAux &aux)
{
    CVector3 temp = z;
    CVector3 tempA = z * 0;
    CVector3 tempB = z * 0;


    //boxFold1
  if (fractal->mandelbulb5.boxFold1Enabled && i >= fractal->mandelbulb5.boxFold1StartIterations && i < fractal->mandelbulb5.boxFold1StopIterations)
  {
    temp = z;
    if (z.x > fractal->mandelbulb5.boxFold1FoldingLimit) z.x = fractal->mandelbulb5.boxFold1FoldingValue - z.x;
    else if (z.x < -fractal->mandelbulb5.boxFold1FoldingLimit) z.x = -fractal->mandelbulb5.boxFold1FoldingValue - z.x;
    if (z.y > fractal->mandelbulb5.boxFold1FoldingLimit) z.y = fractal->mandelbulb5.boxFold1FoldingValue - z.y;
    else if (z.y < -fractal->mandelbulb5.boxFold1FoldingLimit) z.y = -fractal->mandelbulb5.boxFold1FoldingValue - z.y;
    if (z.z > fractal->mandelbulb5.boxFold1FoldingLimit) z.z = fractal->mandelbulb5.boxFold1FoldingValue - z.z;
    else if (z.z < -fractal->mandelbulb5.boxFold1FoldingLimit) z.z = -fractal->mandelbulb5.boxFold1FoldingValue - z.z;
    //old weight function
    if (fractal->mandelbulb5.boxFold1OldWeightEnabled)
    {
      z = temp + ((z - temp)  * (fractal-> mandelbulb5.boxFold1OldWeight));
    }
    //weight function
    if (fractal->mandelbulb5.boxFold1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.boxFold1Weight);
    }
  }
    //mainRotation1
  if (fractal->mandelbulb5.mainRotation1Enabled && i >= fractal->mandelbulb5.mainRotation1StartIterations && i < fractal->mandelbulb5.mainRotation1StopIterations)
  {
    temp = z;
    z = fractal->mandelbulb5.mainRot1.RotateVector(z);
    //weight function
    if (fractal->mandelbulb5.mainRotation1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.mainRotation1Weight);
    }
  }
    //(fabs( z + const1A.) * const1.B) + z * constC.;
  if (fractal->mandelbulb5.fabsAddConstant1Enabled && i >= fractal->mandelbulb5.fabsAddConstant1StartIterations
      && i < fractal->mandelbulb5.fabsAddConstant1StopIterations)
  {
    temp = z;
    z += fractal->mandelbulb5.fabsAddConstant1A;
    if (fractal->mandelbulb5.fabsAddConstant1Enabledx)
    {
      z.x = fabs(z.x);
    }
    if (fractal->mandelbulb5.fabsAddConstant1Enabledy)
    {
      z.y = fabs(z.y);
    }
    if (fractal->mandelbulb5.fabsAddConstant1Enabledz)
    {
      z.z = fabs(z.z);
    }
    z = ( z * fractal->mandelbulb5.fabsAddConstant1B ) + ( temp * fractal->mandelbulb5.fabsAddConstant1C );
    //old weight function
    if (fractal->mandelbulb5.fabsAddConstant1OldWeightEnabled)
    {
      z = temp + ((z - temp)  * (fractal-> mandelbulb5.fabsAddConstant1OldWeight));
    }
    //weight function
    if (fractal->mandelbulb5.fabsAddConstant1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.fabsAddConstant1Weight);
    }
  }
  //MAIN FORMULA 1
  if (fractal->mandelbulb5.mandelbulb1Enabled && i >= fractal->mandelbulb5.mandelbulb1StartIterations && i < fractal->mandelbulb5.mandelbulb1StopIterations)
  {
    temp = z;
    aux.r = z.Length();
    double th0 = asin(z.z / aux.r) + fractal->mandelbulb5.mandelbulb1BetaAngleOffset;
    double ph0 = atan2(z.y, z.x) + fractal->mandelbulb5.mandelbulb1AlphaAngleOffset;
    double rp = pow(aux.r, fractal->mandelbulb5.mandelbulb1Power - 1.0);
    double th = th0 * fractal->mandelbulb5.mandelbulb1Power;
    double ph = ph0 * fractal->mandelbulb5.mandelbulb1Power;
    double cth = cos(th);
    aux.r_dz = rp * aux.r_dz * fractal->mandelbulb5.mandelbulb1Power + 1.0;
    rp *= aux.r;
    z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;
  }
      // z = z + ( c * const.); 1
  if (fractal->mandelbulb5.constantMultiplier1Enabled && i >= fractal->mandelbulb5.constantMultiplier1StartIterations && i < fractal->mandelbulb5.constantMultiplier1StopIterations)
  {
    temp = z;
    z += c * fractal->mandelbulb5.constantMultiplier1Vect;
    //weight function
    if (fractal->mandelbulb5.constantMultiplier1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.constantMultiplier1Weight);
    }
  }
    //  -fabs( z - constA.) * const.B;
  if (fractal->mandelbulb5.fabsSubConstant1Enabled && i >= fractal->mandelbulb5.fabsSubConstant1StartIterations
      && i < fractal->mandelbulb5.fabsSubConstant1StopIterations)
  {
    temp = z;
    z -= fractal->mandelbulb5.fabsSubConstant1A;
    if (fractal->mandelbulb5.fabsSubConstant1Enabledx)
    {
      z.x = - fabs(z.x );
    }
    if (fractal->mandelbulb5.fabsSubConstant1Enabledy)
    {
      z.y = - fabs(z.y );
    }
    if (fractal->mandelbulb5.fabsSubConstant1Enabledz)
    {
      z.z = - fabs(z.z );
    }
    z *= fractal->mandelbulb5.fabsSubConstant1B;
    //old weight function
    if (fractal->mandelbulb5.fabsSubConstant1OldWeightEnabled)
    {
      z = temp + ((z - temp)  * (fractal-> mandelbulb5.fabsSubConstant1OldWeight));
    }
    //weight function
    if (fractal->mandelbulb5.fabsSubConstant1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.fabsSubConstant1Weight);
    }
  }
  // z = fabs( z + const.A ) + ( z * const.B ) + const.C; 1
  if (fractal->mandelbulb5.fabsFormulaZAB1Enabled && i >= fractal->mandelbulb5.fabsFormulaZAB1StartIterations
      && i < fractal->mandelbulb5.fabsFormulaZAB1StopIterations)
  {
    temp = z;
    z +=  fractal->mandelbulb5.fabsFormulaZAB1A;
    if (fractal->mandelbulb5.fabsFormulaZAB1Enabledx)
    {
      z.x = fabs(z.x);
    }
    if (fractal->mandelbulb5.fabsFormulaZAB1Enabledy)
    {
      z.y = fabs(z.y);
    }
    if (fractal->mandelbulb5.fabsFormulaZAB1Enabledz)
    {
      z.z = fabs(z.z);
    }
    z+= (temp * fractal->mandelbulb5.fabsFormulaZAB1B) + fractal->mandelbulb5.fabsFormulaZAB1C;
    //old weight function
    if (fractal->mandelbulb5.fabsFormulaZAB1OldWeightEnabled)
    {
      z = temp + ((z - temp)  * (fractal-> mandelbulb5.fabsFormulaZAB1OldWeight));
    }
    //weight function
    if (fractal->mandelbulb5.fabsFormulaZAB1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.fabsFormulaZAB1Weight);
    }
  }
  // z = z + const; 1
  if (fractal->mandelbulb5.additionConstant1Enabled && i >= fractal->mandelbulb5.additionConstant1StartIterations && i < fractal->mandelbulb5.additionConstant1StopIterations)
  {
    temp = z;
    z += fractal->mandelbulb5.additionConstant1;
    //weight function
    if (fractal->mandelbulb5.additionConstant1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.additionConstant1Weight);
    }
  }

  //  z = fabs( z + constA.) - fabs( z - constB.) - z; 1
  if (fractal->mandelbulb5.fabsFormulaAB1Enabled && i >= fractal->mandelbulb5.fabsFormulaAB1StartIterations
        && i < fractal->mandelbulb5.fabsFormulaAB1StopIterations)
  {
    temp = z;
    if (fractal->mandelbulb5.fabsFormulaAB1Enabledx)
    {
    z.x = fabs(z.x + fractal->mandelbulb5.fabsFormulaAB1A.x)-fabs(z.x - fractal->mandelbulb5.fabsFormulaAB1B.x) - z.x;
    }
    if (fractal->mandelbulb5.fabsFormulaAB1Enabledy)
    {
    z.y = fabs(z.y + fractal->mandelbulb5.fabsFormulaAB1A.y)-fabs(z.y - fractal->mandelbulb5.fabsFormulaAB1B.y) - z.y;
    }
    if (fractal->mandelbulb5.fabsFormulaAB1Enabledz)
    {
    z.z = fabs(z.z + fractal->mandelbulb5.fabsFormulaAB1A.z)-fabs(z.z - fractal->mandelbulb5.fabsFormulaAB1B.z) - z.z;
    }
    //old weight function
    if (fractal->mandelbulb5.fabsFormulaAB1OldWeightEnabled)
    {
      z = temp + ((z - temp)  * (fractal-> mandelbulb5.fabsFormulaAB1OldWeight));
    }
    //weight function
    if (fractal->mandelbulb5.fabsFormulaAB1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.fabsFormulaAB1Weight);
    }
  }
  //mainRotation; 2
  if (fractal->mandelbulb5.mainRotation2Enabled && i >= fractal->mandelbulb5.mainRotation2StartIterations && i < fractal->mandelbulb5.mainRotation2StopIterations)
  {
    temp = z;
    z = fractal->mandelbulb5.mainRot2.RotateVector(z);
    //weight function
    if (fractal->mandelbulb5.mainRotation2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.mainRotation2Weight);
    }
  }
  //  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 1
  if (fractal->mandelbulb5.fabsFormulaABCD1Enabled && i >= fractal->mandelbulb5.fabsFormulaABCD1StartIterations
      && i < fractal->mandelbulb5.fabsFormulaABCD1StopIterations)
  {
    temp = z;
    if (fractal->mandelbulb5.fabsFormulaABCD1EnabledAx)
    {
      tempA.x = fabs(z.x + fractal->mandelbulb5.fabsFormulaABCD1A.x);
    }
    if (fractal->mandelbulb5.fabsFormulaABCD1EnabledBx)
    {
      tempB.x = fabs(z.x - fractal->mandelbulb5.fabsFormulaABCD1B.x);
    }
    z.x = tempA.x - tempB.x + (z.x * fractal->mandelbulb5.fabsFormulaABCD1C.x + fractal->mandelbulb5.fabsFormulaABCD1D.x);
    if (fractal->mandelbulb5.fabsFormulaABCD1EnabledAy)
    {
      tempA.y = fabs(z.y + fractal->mandelbulb5.fabsFormulaABCD1A.y);
    }
    if (fractal->mandelbulb5.fabsFormulaABCD1EnabledBy)
    {
      tempB.y = fabs(z.y - fractal->mandelbulb5.fabsFormulaABCD1B.y);
    }
    z.y = tempA.y - tempB.y + (z.y * fractal->mandelbulb5.fabsFormulaABCD1C.y + fractal->mandelbulb5.fabsFormulaABCD1D.y);
    if (fractal->mandelbulb5.fabsFormulaABCD1EnabledAz)
    {
      tempA.z = fabs(z.z + fractal->mandelbulb5.fabsFormulaABCD1A.z);
    }
    if (fractal->mandelbulb5.fabsFormulaABCD1EnabledBz)
    {
      tempB.z = fabs(z.z - fractal->mandelbulb5.fabsFormulaABCD1B.z);
    }
    z.z = tempA.z - tempB.z + (z.z * fractal->mandelbulb5.fabsFormulaABCD1C.z + fractal->mandelbulb5.fabsFormulaABCD1D.z);
    //old weight function
    if (fractal->mandelbulb5.fabsFormulaABCD1OldWeightEnabled)
    {
      z = temp + ((z - temp)  * (fractal-> mandelbulb5.fabsFormulaABCD1OldWeight));
    }
    //weight function
    if (fractal->mandelbulb5.fabsFormulaABCD1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.fabsFormulaABCD1Weight);
    }
  }


  //MAIN FORMULA 2
  if (fractal->mandelbulb5.mandelbulb2Enabled && i >= fractal->mandelbulb5.mandelbulb2StartIterations && i < fractal->mandelbulb5.mandelbulb2StopIterations)
  {
    temp = z;
    aux.r = z.Length();
    double th0 = asin(z.z / aux.r) + fractal->mandelbulb5.mandelbulb2BetaAngleOffset;
    double ph0 = atan2(z.y, z.x) + fractal->mandelbulb5.mandelbulb2AlphaAngleOffset;
    double rp = pow(aux.r, fractal->mandelbulb5.mandelbulb2Power - 1.0);
    double th = th0 * fractal->mandelbulb5.mandelbulb2Power;
    double ph = ph0 * fractal->mandelbulb5.mandelbulb2Power;
    double cth = cos(th);
    aux.r_dz = rp * aux.r_dz * fractal->mandelbulb5.mandelbulb2Power + 1.0;
    rp *= aux.r;
    z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;
  }
  //mainRotation; 3
  if (fractal->mandelbulb5.mainRotation3Enabled && i >= fractal->mandelbulb5.mainRotation3StartIterations && i < fractal->mandelbulb5.mainRotation3StopIterations)
  {
    temp = z;
    z = fractal->mandelbulb5.mainRot3.RotateVector(z);
    //weight function
    if (fractal->mandelbulb5.mainRotation3WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.mainRotation3Weight);
    }
  }
    // z = z + c * const; 2
  if (fractal->mandelbulb5.constantMultiplier2Enabled && i >= fractal->mandelbulb5.constantMultiplier2StartIterations && i < fractal->mandelbulb5.constantMultiplier2StopIterations)
  {
    temp = z;
    z += c * fractal->mandelbulb5.constantMultiplier2Vect;
    //weight function
    if (fractal->mandelbulb5.constantMultiplier2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.constantMultiplier2Weight);
    }
  }

  // z = z + const; 2
  if (fractal->mandelbulb5.additionConstant2Enabled && i >= fractal->mandelbulb5.additionConstant2StartIterations && i < fractal->mandelbulb5.additionConstant2StopIterations)
  {
    temp = z;
    z += fractal->mandelbulb5.additionConstant2;
    //weight function
    if (fractal->mandelbulb5.additionConstant2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.additionConstant2Weight);
    }
  }

    //  z = fabs( z + constA.) - fabs( z - constB.) - z; 2
  if (fractal->mandelbulb5.fabsFormulaAB2Enabled && i >= fractal->mandelbulb5.fabsFormulaAB2StartIterations
        && i < fractal->mandelbulb5.fabsFormulaAB2StopIterations)
  {
    temp = z;
    if (fractal->mandelbulb5.fabsFormulaAB2Enabledx)
    {
    z.x = fabs(z.x + fractal->mandelbulb5.fabsFormulaAB2A.x)-fabs(z.x - fractal->mandelbulb5.fabsFormulaAB2B.x) -z.x;
    }
    if (fractal->mandelbulb5.fabsFormulaAB2Enabledy)
    {
    z.y = fabs(z.y + fractal->mandelbulb5.fabsFormulaAB2A.y)-fabs(z.y - fractal->mandelbulb5.fabsFormulaAB2B.y) -z.y;
    }
    if (fractal->mandelbulb5.fabsFormulaAB2Enabledz)
    {
    z.z = fabs(z.z + fractal->mandelbulb5.fabsFormulaAB2A.z)-fabs(z.z - fractal->mandelbulb5.fabsFormulaAB2B.z) -z.z;
    }
    //old weight function
    if (fractal->mandelbulb5.fabsFormulaAB2OldWeightEnabled)
    {
      z = temp + ((z - temp)  * (fractal-> mandelbulb5.fabsFormulaAB2OldWeight));
    }
    //weight function
    if (fractal->mandelbulb5.fabsFormulaAB2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.fabsFormulaAB2Weight);
    }
  }
    //boxFold; 2
  if (fractal->mandelbulb5.boxFold2Enabled && i >= fractal->mandelbulb5.boxFold2StartIterations && i < fractal->mandelbulb5.boxFold2StopIterations)
  {
    temp = z;
    if (z.x > fractal->mandelbulb5.boxFold2FoldingLimit) z.x = fractal->mandelbulb5.boxFold2FoldingValue - z.x;
    else if (z.x < -fractal->mandelbulb5.boxFold2FoldingLimit) z.x = -fractal->mandelbulb5.boxFold2FoldingValue - z.x;
    if (z.y > fractal->mandelbulb5.boxFold2FoldingLimit) z.y = fractal->mandelbulb5.boxFold2FoldingValue - z.y;
    else if (z.y < -fractal->mandelbulb5.boxFold2FoldingLimit) z.y = -fractal->mandelbulb5.boxFold2FoldingValue - z.y;
    if (z.z > fractal->mandelbulb5.boxFold2FoldingLimit) z.z = fractal->mandelbulb5.boxFold2FoldingValue - z.z;
    else if (z.z < -fractal->mandelbulb5.boxFold2FoldingLimit) z.z = -fractal->mandelbulb5.boxFold2FoldingValue - z.z;
    //old weight function
    if (fractal->mandelbulb5.boxFold2OldWeightEnabled)
    {
      z = temp + ((z - temp)  * (fractal-> mandelbulb5.boxFold2OldWeight));
    }
    //weight function
    if (fractal->mandelbulb5.boxFold2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbulb5.boxFold2Weight);
    }
  }
}

void Mandelbox103Iteration(CVector3 &z, CVector3 &c, int &i, const cFractal *fractal, sMandelboxAux &aux)
{
    CVector3 temp = z;
    double temp2 = aux.mboxDE;
    CVector3 tempA = z * 0;
    CVector3 tempB = z * 0;
    CVector3 zA = z * 0;
    CVector3 zB = z * 0;

    //boxFold1
  if (fractal->mandelbox103.boxFold1Enabled && i >= fractal->mandelbox103.boxFold1StartIterations && i < fractal->mandelbox103.boxFold1StopIterations)
  {
    temp = z;
    if (z.x > fractal->mandelbox103.boxFold1FoldingLimit) z.x = fractal->mandelbox103.boxFold1FoldingValue - z.x;
    else if (z.x < -fractal->mandelbox103.boxFold1FoldingLimit) z.x = -fractal->mandelbox103.boxFold1FoldingValue - z.x;
    if (z.y > fractal->mandelbox103.boxFold1FoldingLimit) z.y = fractal->mandelbox103.boxFold1FoldingValue - z.y;
    else if (z.y < -fractal->mandelbox103.boxFold1FoldingLimit) z.y = -fractal->mandelbox103.boxFold1FoldingValue - z.y;
    if (z.z > fractal->mandelbox103.boxFold1FoldingLimit) z.z = fractal->mandelbox103.boxFold1FoldingValue - z.z;
    else if (z.z < -fractal->mandelbox103.boxFold1FoldingLimit) z.z = -fractal->mandelbox103.boxFold1FoldingValue - z.z;
    //weight function
    if (fractal->mandelbox103.boxFold1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbox103.boxFold1Weight);
    }
  }
  // sphericalFold1
  if (fractal->mandelbox103.sphericalFold1Enabled && i >= fractal->mandelbox103.sphericalFold1StartIterations && i < fractal->mandelbox103.sphericalFold1StopIterations)
  {
    temp = z;
    double r2 = z.Dot(z);
    if (r2 < fractal->mandelbox103.sphericalFold1mR2)
      {
      z *= fractal->mandelbox103.sphericalFold1MboxFactor1;
      aux.mboxDE *= fractal->mandelbox103.sphericalFold1MboxFactor1;
    }
    else if (r2 < fractal->mandelbox103.sphericalFold1fR2)
    {
      double tglad_factor2 = fractal->mandelbox103.sphericalFold1fR2 / r2;
      z *= tglad_factor2;
      aux.mboxDE *= tglad_factor2;
    }
    //weight function
      if (fractal->mandelbox103.sphericalFold1WeightEnabled)
      {
        z = SmoothCVector(temp, z, fractal-> mandelbox103.sphericalFold1Weight);
        double nkaux = 1.0 - ( fractal-> mandelbox103.sphericalFold1Weight);
        aux.mboxDE = ( temp2 * nkaux )  + ( aux.mboxDE  *  fractal-> mandelbox103.sphericalFold1Weight);
      }
  }
  //scale; 1
  if (fractal->mandelbox103.scale1Enabled && i >= fractal->mandelbox103.scale1StartIterations && i < fractal->mandelbox103.scale1StopIterations)
  {
    temp = z;
    z = z * fractal->mandelbox103.scale1;
    aux.mboxDE = aux.mboxDE * fabs(fractal->mandelbox103.scale1) + 1.0;
  }
    //mainRotation1
  if (fractal->mandelbox103.mainRotation1Enabled && i >= fractal->mandelbox103.mainRotation1StartIterations && i < fractal->mandelbox103.mainRotation1StopIterations)
  {
    temp = z;
    z = fractal->mandelbox103.mainRot1.RotateVector(z);
    if (fractal->mandelbox103.mainRotation1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbox103.mainRotation1Weight);
    }
  }
    //(fabs( z + const1A.) * const1.B) + z * constC.;
  if (fractal->mandelbox103.fabsAddConstant1Enabled && i >= fractal->mandelbox103.fabsAddConstant1StartIterations
      && i < fractal->mandelbox103.fabsAddConstant1StopIterations)
  {
    temp = z;
    z += fractal->mandelbox103.fabsAddConstant1A;
    if (fractal->mandelbox103.fabsAddConstant1Enabledx)
    {
      z.x = fabs(z.x);
    }
    if (fractal->mandelbox103.fabsAddConstant1Enabledy)
    {
      z.y = fabs(z.y);
    }
    if (fractal->mandelbox103.fabsAddConstant1Enabledz)
    {
      z.z = fabs(z.z);
    }
    z = ( z * fractal->mandelbox103.fabsAddConstant1B ) + ( temp * fractal->mandelbox103.fabsAddConstant1C );
    //weight function
    if (fractal->mandelbox103.fabsAddConstant1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbox103.fabsAddConstant1Weight);
    }
  }
      // z = z + ( c * const.); 1
  if (fractal->mandelbox103.constantMultiplier1Enabled && i >= fractal->mandelbox103.constantMultiplier1StartIterations && i < fractal->mandelbox103.constantMultiplier1StopIterations)
  {
    temp = z;
    z += c * fractal->mandelbox103.constantMultiplier1Vect;
    //weight function
    if (fractal->mandelbox103.constantMultiplier1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbox103.constantMultiplier1Weight);
    }
  }
  // z = z + const; 1
  if (fractal->mandelbox103.additionConstant1Enabled && i >= fractal->mandelbox103.additionConstant1StartIterations && i < fractal->mandelbox103.additionConstant1StopIterations)
  {
    temp = z;
    z += fractal->mandelbox103.additionConstant1;
    //weight function
    if (fractal->mandelbox103.additionConstant1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbox103.additionConstant1Weight);
    }
  }
  //  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 1
  if (fractal->mandelbox103.fabsFormulaABCD1Enabled && i >= fractal->mandelbox103.fabsFormulaABCD1StartIterations
      && i < fractal->mandelbox103.fabsFormulaABCD1StopIterations)
  {
    temp = z;
    if (fractal->mandelbox103.fabsFormulaABCD1EnabledAx)
    {
      tempA.x = fabs(z.x + fractal->mandelbox103.fabsFormulaABCD1A.x);
    }
    if (fractal->mandelbox103.fabsFormulaABCD1EnabledBx)
    {
      tempB.x = fabs(z.x - fractal->mandelbox103.fabsFormulaABCD1B.x);
    }
    z.x = tempA.x - tempB.x + (z.x * fractal->mandelbox103.fabsFormulaABCD1C.x + fractal->mandelbox103.fabsFormulaABCD1D.x);
    if (fractal->mandelbox103.fabsFormulaABCD1EnabledAy)
    {
      tempA.y = fabs(z.y + fractal->mandelbox103.fabsFormulaABCD1A.y);
    }
    if (fractal->mandelbox103.fabsFormulaABCD1EnabledBy)
    {
      tempB.y = fabs(z.y - fractal->mandelbox103.fabsFormulaABCD1B.y);
    }
    z.y = tempA.y - tempB.y + (z.y * fractal->mandelbox103.fabsFormulaABCD1C.y + fractal->mandelbox103.fabsFormulaABCD1D.y);
    if (fractal->mandelbox103.fabsFormulaABCD1EnabledAz)
    {
      tempA.z = fabs(z.z + fractal->mandelbox103.fabsFormulaABCD1A.z);
    }
    if (fractal->mandelbox103.fabsFormulaABCD1EnabledBz)
    {
      tempB.z = fabs(z.z - fractal->mandelbox103.fabsFormulaABCD1B.z);
    }
    z.z = tempA.z - tempB.z + (z.z * fractal->mandelbox103.fabsFormulaABCD1C.z + fractal->mandelbox103.fabsFormulaABCD1D.z);
    //weight function
    if (fractal->mandelbox103.fabsFormulaABCD1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbox103.fabsFormulaABCD1Weight);
    }
  }
  //MAIN FORMULA
    //boxFold; 2
  if (fractal->mandelbox103.boxFold2Enabled && i >= fractal->mandelbox103.boxFold2StartIterations && i < fractal->mandelbox103.boxFold2StopIterations)
  {
    temp = z;
    if (z.x > fractal->mandelbox103.boxFold2FoldingLimit) z.x = fractal->mandelbox103.boxFold2FoldingValue - z.x;
    else if (z.x < -fractal->mandelbox103.boxFold2FoldingLimit) z.x = -fractal->mandelbox103.boxFold2FoldingValue - z.x;
    if (z.y > fractal->mandelbox103.boxFold2FoldingLimit) z.y = fractal->mandelbox103.boxFold2FoldingValue - z.y;
    else if (z.y < -fractal->mandelbox103.boxFold2FoldingLimit) z.y = -fractal->mandelbox103.boxFold2FoldingValue - z.y;
    if (z.z > fractal->mandelbox103.boxFold2FoldingLimit) z.z = fractal->mandelbox103.boxFold2FoldingValue - z.z;
    else if (z.z < -fractal->mandelbox103.boxFold2FoldingLimit) z.z = -fractal->mandelbox103.boxFold2FoldingValue - z.z;
    //weight function
    if (fractal->mandelbox103.boxFold2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbox103.boxFold2Weight);
    }
  }
  // sphericalFold2
  if (fractal->mandelbox103.sphericalFold2Enabled && i >= fractal->mandelbox103.sphericalFold2StartIterations && i < fractal->mandelbox103.sphericalFold2StopIterations)
  {
    temp = z;
    temp2 = aux.mboxDE;
    double r2 = z.Dot(z);
    if (r2 < fractal->mandelbox103.sphericalFold2mR2)
      {
      z *= fractal->mandelbox103.sphericalFold2MboxFactor1;
      aux.mboxDE *= fractal->mandelbox103.sphericalFold2MboxFactor1;
    }
    else if (r2 < fractal->mandelbox103.sphericalFold2fR2)
    {
      double tglad_factor2 = fractal->mandelbox103.sphericalFold2fR2 / r2;
      z *= tglad_factor2;
      aux.mboxDE *= tglad_factor2;
    }
    //weight function
    if (fractal->mandelbox103.sphericalFold2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbox103.sphericalFold2Weight);
      //aux.mboxDE = temp2 + ( aux.mboxDE - temp2) * ( fractal-> mandelbox103.sphericalFold2Weight);
      double nkaux = 1.0 - ( fractal-> mandelbox103.sphericalFold2Weight);
      aux.mboxDE = ( temp2 * nkaux )  + ( aux.mboxDE  *  fractal-> mandelbox103.sphericalFold2Weight);
    }
  }
  //scale; 2
  if (fractal->mandelbox103.scale2Enabled && i >= fractal->mandelbox103.scale2StartIterations && i < fractal->mandelbox103.scale2StopIterations)
  {
    z = z * fractal->mandelbox103.scale2;
    aux.mboxDE = aux.mboxDE * fabs(fractal->mandelbox103.scale2) + 1.0;
  }
  //mainRotation2
  if (fractal->mandelbox103.mainRotation2Enabled && i >= fractal->mandelbox103.mainRotation2StartIterations && i < fractal->mandelbox103.mainRotation2StopIterations)
  {
    temp = z;
    z = fractal->mandelbox103.mainRot2.RotateVector(z);
    //weight function
    if (fractal->mandelbox103.mainRotation2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbox103.mainRotation2Weight);
    }
  }
    // z = z + c * const; 2
  if (fractal->mandelbox103.constantMultiplier2Enabled && i >= fractal->mandelbox103.constantMultiplier2StartIterations && i < fractal->mandelbox103.constantMultiplier2StopIterations)
  {
    temp = z;
    z += c * fractal->mandelbox103.constantMultiplier2Vect;
    //weight function
    if (fractal->mandelbox103.constantMultiplier2WeightEnabled)
    {
    z = SmoothCVector(temp, z, fractal-> mandelbox103.constantMultiplier2Weight);
    }
  }
  // z = z + const; 2
  if (fractal->mandelbox103.additionConstant2Enabled && i >= fractal->mandelbox103.additionConstant2StartIterations && i < fractal->mandelbox103.additionConstant2StopIterations)
  {
    temp = z;
    z += fractal->mandelbox103.additionConstant2;
    //weight function
    if (fractal->mandelbox103.additionConstant2WeightEnabled)
    {
    z = SmoothCVector(temp, z, fractal-> mandelbox103.additionConstant2Weight);
    }
  }
  //mainRotation; 3
  if (fractal->mandelbox103.mainRotation3Enabled && i >= fractal->mandelbox103.mainRotation3StartIterations && i < fractal->mandelbox103.mainRotation3StopIterations)
  {
    temp = z;
    z = fractal->mandelbox103.mainRot3.RotateVector(z);
    //weight function
    if (fractal->mandelbox103.mainRotation3WeightEnabled)
    {
    z = SmoothCVector(temp, z, fractal-> mandelbox103.mainRotation3Weight);
    }
  }
  //  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 2
  if (fractal->mandelbox103.fabsFormulaABCD2Enabled && i >= fractal->mandelbox103.fabsFormulaABCD2StartIterations
      && i < fractal->mandelbox103.fabsFormulaABCD2StopIterations)
  {
    temp = z;
    if (fractal->mandelbox103.fabsFormulaABCD2EnabledAx)
    {
      tempA.x = fabs(z.x + fractal->mandelbox103.fabsFormulaABCD2A.x);
    }
    if (fractal->mandelbox103.fabsFormulaABCD2EnabledBx)
    {
      tempB.x = fabs(z.x - fractal->mandelbox103.fabsFormulaABCD2B.x);
    }
    z.x = tempA.x - tempB.x + (z.x * fractal->mandelbox103.fabsFormulaABCD2C.x + fractal->mandelbox103.fabsFormulaABCD2D.x);
    if (fractal->mandelbox103.fabsFormulaABCD2EnabledAy)
    {
      tempA.y = fabs(z.y + fractal->mandelbox103.fabsFormulaABCD2A.y);
    }
    if (fractal->mandelbox103.fabsFormulaABCD2EnabledBy)
    {
      tempB.y = fabs(z.y - fractal->mandelbox103.fabsFormulaABCD2B.y);
    }
    z.y = tempA.y - tempB.y + (z.y * fractal->mandelbox103.fabsFormulaABCD2C.y + fractal->mandelbox103.fabsFormulaABCD2D.y);
    if (fractal->mandelbox103.fabsFormulaABCD2EnabledAz)
    {
      tempA.z = fabs(z.z + fractal->mandelbox103.fabsFormulaABCD2A.z);
    }
    if (fractal->mandelbox103.fabsFormulaABCD2EnabledBz)
    {
      tempB.z = fabs(z.z - fractal->mandelbox103.fabsFormulaABCD2B.z);
    }
    z.z = tempA.z - tempB.z + (z.z * fractal->mandelbox103.fabsFormulaABCD2C.z + fractal->mandelbox103.fabsFormulaABCD2D.z);
    //weight function
    if (fractal->mandelbox103.fabsFormulaABCD2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> mandelbox103.fabsFormulaABCD2Weight);
    }
  }
  // Iteration weight z  =  (  z * const.Z) + (  zA * Const.A) + ( zB * Const.B);1
  if (fractal->mandelbox103.iterationWeight1Enabled )
  {
    temp = z;
    if ( i== fractal->mandelbox103.iterationWeight1IterationA)
    {
      zA = z;
    }
    if (i == (fractal->mandelbox103.iterationWeight1IterationB))
    {
      zB = z;
    }
    z = (z * fractal->mandelbox103.iterationWeight1ConstantZ) + (zA * fractal->mandelbox103.iterationWeight1ConstantA) + ( zB * fractal->mandelbox103.iterationWeight1ConstantB);
    //weight function
    if (fractal->mandelbox103.iterationWeight1WeightEnabled)
    {
    z = SmoothCVector(temp, z, fractal-> mandelbox103.iterationWeight1Weight);
    }
  }
}
void Quaternion104Iteration(CVector4 &z, const CVector4 &c, int &i, const cFractal *fractal)
{
    CVector4 temp = z;
    CVector4 tempA;
    CVector4 tempB;
    CVector4 zA;
    CVector4 zB;


    //boxFold1
  if (fractal->quaternion104.boxFold1Enabled && i >= fractal->quaternion104.boxFold1StartIterations && i < fractal->quaternion104.boxFold1StopIterations)
  {
    temp = z;
    if (z.x > fractal->quaternion104.boxFold1FoldingLimit) z.x = fractal->quaternion104.boxFold1FoldingValue - z.x;
    else if (z.x < -fractal->quaternion104.boxFold1FoldingLimit) z.x = -fractal->quaternion104.boxFold1FoldingValue - z.x;
    if (z.y > fractal->quaternion104.boxFold1FoldingLimit) z.y = fractal->quaternion104.boxFold1FoldingValue - z.y;
    else if (z.y < -fractal->quaternion104.boxFold1FoldingLimit) z.y = -fractal->quaternion104.boxFold1FoldingValue - z.y;
    if (z.z > fractal->quaternion104.boxFold1FoldingLimit) z.z = fractal->quaternion104.boxFold1FoldingValue - z.z;
    else if (z.z < -fractal->quaternion104.boxFold1FoldingLimit) z.z = -fractal->quaternion104.boxFold1FoldingValue - z.z;
    if (z.w > fractal->quaternion104.boxFold1FoldingLimit) z.w = fractal->quaternion104.boxFold1FoldingValue - z.w;
    else if (z.w < -fractal->quaternion104.boxFold1FoldingLimit) z.w = -fractal->quaternion104.boxFold1FoldingValue - z.w;

    //weight function
    if (fractal->quaternion104.boxFold1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.boxFold1Weight);
    }
  }
  // sphericalFold1
  if (fractal->quaternion104.sphericalFold1Enabled && i >= fractal->quaternion104.sphericalFold1StartIterations && i < fractal->quaternion104.sphericalFold1StopIterations)
  {
    temp = z;
    double r2 = z.Dot(z);
    if (r2 < fractal->quaternion104.sphericalFold1mR2)
    {
      z *= fractal->quaternion104.sphericalFold1MboxFactor1;
    }
    else if (r2 < fractal->quaternion104.sphericalFold1fR2)
    {
      double tglad_factor2 = fractal->quaternion104.sphericalFold1fR2 / r2;
      z *= tglad_factor2;
    }

    //weight function
    if (fractal->quaternion104.sphericalFold1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.sphericalFold1Weight);
    }
  }
  //scale; 1
  if (fractal->quaternion104.scale1Enabled && i >= fractal->quaternion104.scale1StartIterations && i < fractal->quaternion104.scale1StopIterations)
  {
    z = z * fractal->quaternion104.scale1;
  }
    //mainRotation1
  if (fractal->quaternion104.mainRotation1Enabled && i >= fractal->quaternion104.mainRotation1StartIterations && i < fractal->quaternion104.mainRotation1StopIterations)
  {
    temp = z;
    //TODO 4D rotation
    z = CVector4(fractal->quaternion104.mainRot1.RotateVector(z.GetXYZ()), z.w);
    if (fractal->quaternion104.mainRotation1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.mainRotation1Weight);
    }
  }
    //(fabs( z + const1A.) * const1.B) + z * constC.;
  if (fractal->quaternion104.fabsAddConstant1Enabled && i >= fractal->quaternion104.fabsAddConstant1StartIterations
      && i < fractal->quaternion104.fabsAddConstant1StopIterations)
  {
    temp = z;
    z += fractal->quaternion104.fabsAddConstant1A;

    if (fractal->quaternion104.fabsAddConstant1Enabledx)
    {
      z.x = fabs(z.x);
    }
    if (fractal->quaternion104.fabsAddConstant1Enabledy)
    {
      z.y = fabs(z.y);
    }
    if (fractal->quaternion104.fabsAddConstant1Enabledz)
    {
      z.z = fabs(z.z);
    }
    if (fractal->quaternion104.fabsAddConstant1Enabledw)
    {
      z.w = fabs(z.w);
    }
    z = ( z * fractal->quaternion104.fabsAddConstant1B ) + ( temp * fractal->quaternion104.fabsAddConstant1C );
    //weight function
    if (fractal->quaternion104.fabsAddConstant1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.fabsAddConstant1Weight);
    }
  }
  // quaternion; 1
  if (fractal->quaternion104.quaternion1Enabled && i >= fractal->quaternion104.quaternion1StartIterations && i < fractal->quaternion104.quaternion1StopIterations)
  {
    temp = z;
    CVector4 newz( 	fractal->quaternion104.quaternion1FactorConstantVect.x * (z.x * z.x - z.y * z.y - z.z * z.z - z.w * z.w),
    								fractal->quaternion104.quaternion1FactorConstantVect.y * z.x * z.y,
										fractal->quaternion104.quaternion1FactorConstantVect.z * z.x * z.z,
                    fractal->quaternion104.quaternion1FactorConstantVect.w * z.w
										);
    z = newz;

    //weight function
    if (fractal->quaternion104.quaternion1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.quaternion1Weight);
    }
  }
      // z = z + ( c * const.); 1
  if (fractal->quaternion104.constantMultiplier1Enabled && i >= fractal->quaternion104.constantMultiplier1StartIterations && i < fractal->quaternion104.constantMultiplier1StopIterations)
  {
    temp = z;
    z += c * fractal->quaternion104.constantMultiplier1Vect;
    //weight function
    if (fractal->quaternion104.constantMultiplier1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.constantMultiplier1Weight);
    }
  }
  // z = z + const; 1
  if (fractal->quaternion104.additionConstant1Enabled && i >= fractal->quaternion104.additionConstant1StartIterations && i < fractal->quaternion104.additionConstant1StopIterations)
  {
    temp = z;

    z += fractal->quaternion104.additionConstant1;
    //weight function
    if (fractal->quaternion104.additionConstant1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.additionConstant1Weight);
    }
  }
  //  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 1
  if (fractal->quaternion104.fabsFormulaABCD1Enabled && i >= fractal->quaternion104.fabsFormulaABCD1StartIterations
      && i < fractal->quaternion104.fabsFormulaABCD1StopIterations)
  {
    temp = z;
    tempA = z * 0;
    tempB = z * 0;
    if (fractal->quaternion104.fabsFormulaABCD1EnabledAx)
    {
      tempA.x = fabs(z.x + fractal->quaternion104.fabsFormulaABCD1A.x);
    }
    if (fractal->quaternion104.fabsFormulaABCD1EnabledBx)
    {
      tempB.x = fabs(z.x - fractal->quaternion104.fabsFormulaABCD1B.x);
    }
    z.x = tempA.x - tempB.x + (z.x * fractal->quaternion104.fabsFormulaABCD1C.x + fractal->quaternion104.fabsFormulaABCD1D.x);
    if (fractal->quaternion104.fabsFormulaABCD1EnabledAy)
    {
      tempA.y = fabs(z.y + fractal->quaternion104.fabsFormulaABCD1A.y);
    }
    if (fractal->quaternion104.fabsFormulaABCD1EnabledBy)
    {
      tempB.y = fabs(z.y - fractal->quaternion104.fabsFormulaABCD1B.y);
    }
    z.y = tempA.y - tempB.y + (z.y * fractal->quaternion104.fabsFormulaABCD1C.y + fractal->quaternion104.fabsFormulaABCD1D.y);
    if (fractal->quaternion104.fabsFormulaABCD1EnabledAz)
    {
      tempA.z = fabs(z.z + fractal->quaternion104.fabsFormulaABCD1A.z);
    }
    if (fractal->quaternion104.fabsFormulaABCD1EnabledBz)
    {
      tempB.z = fabs(z.z - fractal->quaternion104.fabsFormulaABCD1B.z);
    }
    z.z = tempA.z - tempB.z + (z.z * fractal->quaternion104.fabsFormulaABCD1C.z + fractal->quaternion104.fabsFormulaABCD1D.z);
    if (fractal->quaternion104.fabsFormulaABCD1EnabledAw)
    {
      tempA.w = fabs(z.w + fractal->quaternion104.fabsFormulaABCD1A.w);
    }
    if (fractal->quaternion104.fabsFormulaABCD1EnabledBw)
    {
      tempB.w = fabs(z.w - fractal->quaternion104.fabsFormulaABCD1B.w);
    }
    z.w = tempA.w - tempB.w + (z.w * fractal->quaternion104.fabsFormulaABCD1C.w + fractal->quaternion104.fabsFormulaABCD1D.w);

    //weight function
    if (fractal->quaternion104.fabsFormulaABCD1WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.fabsFormulaABCD1Weight);
    }
  }
  //mainRotation2
  if (fractal->quaternion104.mainRotation2Enabled && i >= fractal->quaternion104.mainRotation2StartIterations && i < fractal->quaternion104.mainRotation2StopIterations)
  {
    temp = z;
    //TODO 4D rotation
    z = CVector4(fractal->quaternion104.mainRot2.RotateVector(z.GetXYZ()), z.w);
    //weight function
    if (fractal->quaternion104.mainRotation2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.mainRotation2Weight);
    }
  }
  //MAIN FORMULA

  // quaternion; 2
  if (fractal->quaternion104.quaternion2Enabled && i >= fractal->quaternion104.quaternion2StartIterations && i < fractal->quaternion104.quaternion2StopIterations)
  {
    temp = z;
    CVector4 newz( 	fractal->quaternion104.quaternion2FactorConstantVect.x * (z.x * z.x - z.y * z.y - z.z * z.z - z.w * z.w),
    								fractal->quaternion104.quaternion2FactorConstantVect.y * z.x * z.y,
										fractal->quaternion104.quaternion2FactorConstantVect.z * z.x * z.z,
                    fractal->quaternion104.quaternion2FactorConstantVect.w * z.w);
    z = newz;

    //weight function
    if (fractal->quaternion104.quaternion2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.quaternion2Weight);
    }
  }

    //boxFold; 2
  if (fractal->quaternion104.boxFold2Enabled && i >= fractal->quaternion104.boxFold2StartIterations && i < fractal->quaternion104.boxFold2StopIterations)
  {
    temp = z;
    if (z.x > fractal->quaternion104.boxFold2FoldingLimit) z.x = fractal->quaternion104.boxFold2FoldingValue - z.x;
    else if (z.x < -fractal->quaternion104.boxFold2FoldingLimit) z.x = -fractal->quaternion104.boxFold2FoldingValue - z.x;
    if (z.y > fractal->quaternion104.boxFold2FoldingLimit) z.y = fractal->quaternion104.boxFold2FoldingValue - z.y;
    else if (z.y < -fractal->quaternion104.boxFold2FoldingLimit) z.y = -fractal->quaternion104.boxFold2FoldingValue - z.y;
    if (z.z > fractal->quaternion104.boxFold2FoldingLimit) z.z = fractal->quaternion104.boxFold2FoldingValue - z.z;
    else if (z.z < -fractal->quaternion104.boxFold2FoldingLimit) z.z = -fractal->quaternion104.boxFold2FoldingValue - z.z;
    if (z.w > fractal->quaternion104.boxFold2FoldingLimit) z.w = fractal->quaternion104.boxFold2FoldingValue - z.w;
    else if (z.w < -fractal->quaternion104.boxFold2FoldingLimit) z.w = -fractal->quaternion104.boxFold2FoldingValue - z.w;
    //weight function
    if (fractal->quaternion104.boxFold2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.boxFold2Weight);
    }
  }
  // sphericalFold2
  if (fractal->quaternion104.sphericalFold2Enabled && i >= fractal->quaternion104.sphericalFold2StartIterations && i < fractal->quaternion104.sphericalFold2StopIterations)
  {
    temp = z;
    //temp2 = aux.mboxDE;
    double r2 = z.Dot(z);
    if (r2 < fractal->quaternion104.sphericalFold2mR2)
      {
      z *= fractal->quaternion104.sphericalFold2MboxFactor1;
    }
    else if (r2 < fractal->quaternion104.sphericalFold2fR2)
    {
      double tglad_factor2 = fractal->quaternion104.sphericalFold2fR2 / r2;
      z *= tglad_factor2;
      //aux.mboxDE *= tglad_factor2;
    }
    //weight function
    if (fractal->quaternion104.sphericalFold2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.sphericalFold2Weight);
    }
  }
  //scale; 2
  if (fractal->quaternion104.scale2Enabled && i >= fractal->quaternion104.scale2StartIterations && i < fractal->quaternion104.scale2StopIterations)
  {
    z = z * fractal->quaternion104.scale2;
  }


  //mainRotation; 3
  if (fractal->quaternion104.mainRotation3Enabled && i >= fractal->quaternion104.mainRotation3StartIterations && i < fractal->quaternion104.mainRotation3StopIterations)
  {
    temp = z;
    z = CVector4(fractal->quaternion104.mainRot3.RotateVector(z.GetXYZ()), z.w);
    //weight function
    if (fractal->quaternion104.mainRotation3WeightEnabled)
    {
    z = SmoothCVector(temp, z, fractal-> quaternion104.mainRotation3Weight);
    }
  }

    // z = z + c * const; 2
  if (fractal->quaternion104.constantMultiplier2Enabled && i >= fractal->quaternion104.constantMultiplier2StartIterations && i < fractal->quaternion104.constantMultiplier2StopIterations)
  {
    temp = z;
    z += c * fractal->quaternion104.constantMultiplier2Vect;
    //weight function
    if (fractal->quaternion104.constantMultiplier2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.constantMultiplier2Weight);
    }
  }
  // z = z + const; 2
  if (fractal->quaternion104.additionConstant2Enabled && i >= fractal->quaternion104.additionConstant2StartIterations && i < fractal->quaternion104.additionConstant2StopIterations)
  {
    temp = z;
    z += fractal->quaternion104.additionConstant2;
    //weight function
    if (fractal->quaternion104.additionConstant2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.additionConstant2Weight);
    }
  }
  //  z = fabs( z + constA.) - fabs( z - constB.) + ( z * constC  + constD); 2
  if (fractal->quaternion104.fabsFormulaABCD2Enabled && i >= fractal->quaternion104.fabsFormulaABCD2StartIterations
      && i < fractal->quaternion104.fabsFormulaABCD2StopIterations)
  {
    temp = z;
    tempA = z * 0;
    tempB = z * 0;
    if (fractal->quaternion104.fabsFormulaABCD2EnabledAx)
    {
      tempA.x = fabs(z.x + fractal->quaternion104.fabsFormulaABCD2A.x);
    }
    if (fractal->quaternion104.fabsFormulaABCD2EnabledBx)
    {
      tempB.x = fabs(z.x - fractal->quaternion104.fabsFormulaABCD2B.x);
    }
    z.x = tempA.x - tempB.x + (z.x * fractal->quaternion104.fabsFormulaABCD2C.x + fractal->quaternion104.fabsFormulaABCD2D.x);
    if (fractal->quaternion104.fabsFormulaABCD2EnabledAy)
    {
      tempA.y = fabs(z.y + fractal->quaternion104.fabsFormulaABCD2A.y);
    }
    if (fractal->quaternion104.fabsFormulaABCD2EnabledBy)
    {
      tempB.y = fabs(z.y - fractal->quaternion104.fabsFormulaABCD2B.y);
    }
    z.y = tempA.y - tempB.y + (z.y * fractal->quaternion104.fabsFormulaABCD2C.y + fractal->quaternion104.fabsFormulaABCD2D.y);
    if (fractal->quaternion104.fabsFormulaABCD2EnabledAz)
    {
      tempA.z = fabs(z.z + fractal->quaternion104.fabsFormulaABCD2A.z);
    }
    if (fractal->quaternion104.fabsFormulaABCD2EnabledBz)
    {
      tempB.z = fabs(z.z - fractal->quaternion104.fabsFormulaABCD2B.z);
    }
    z.z = tempA.z - tempB.z + (z.z * fractal->quaternion104.fabsFormulaABCD2C.z + fractal->quaternion104.fabsFormulaABCD2D.z);
    if (fractal->quaternion104.fabsFormulaABCD2EnabledAw)
    {
      tempA.w = fabs(z.w + fractal->quaternion104.fabsFormulaABCD2A.w);
    }
    if (fractal->quaternion104.fabsFormulaABCD2EnabledBw)
    {
      tempB.w = fabs(z.w - fractal->quaternion104.fabsFormulaABCD2B.w);
    }
    z.w = tempA.w - tempB.w + (z.w * fractal->quaternion104.fabsFormulaABCD2C.w + fractal->quaternion104.fabsFormulaABCD2D.w);

    //weight function
    if (fractal->quaternion104.fabsFormulaABCD2WeightEnabled)
    {
      z = SmoothCVector(temp, z, fractal-> quaternion104.fabsFormulaABCD2Weight);
    }
  }
  // Iteration weight z  =  (  z * const.Z) + (  zA * Const.A) + ( zB * Const.B);1
  if (fractal->quaternion104.iterationWeight1Enabled )
  {
    temp = z;
    if ( i== fractal->quaternion104.iterationWeight1IterationA)
    {
      zA = z;
    }
    if (i == (fractal->quaternion104.iterationWeight1IterationB))
    {
      zB = z;
    }
    z = (z * fractal->quaternion104.iterationWeight1ConstantZ) + (zA * fractal->quaternion104.iterationWeight1ConstantA) + ( zB * fractal->quaternion104.iterationWeight1ConstantB);
    //weight function
    if (fractal->quaternion104.iterationWeight1WeightEnabled)
    {
    z = SmoothCVector(temp, z, fractal-> quaternion104.iterationWeight1Weight);
    }
  }
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
					aux.mboxColor += fractal->mandelbox.colorFactor.z;
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
					aux.mboxColor += fractal->mandelbox.colorFactor.y;
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
				aux.mboxColor += fractal->mandelbox.colorFactor.x;
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
		aux.mboxColor += fractal->mandelbox.colorFactorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.mboxDE *= tglad_factor2;
		aux.mboxColor += fractal->mandelbox.colorFactorSp2;
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

void SphericalFolding(CVector3 &z, const sFractalFoldings *foldings, double &foldColor, double &foldDE, double r)
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

