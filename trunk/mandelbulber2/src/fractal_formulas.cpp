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

void Mandelbulbulb2Iteration(CVector3 &z, sMandelbulbAux &aux)
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

void Mandelbulbulb3Iteration(CVector3 &z, sMandelbulbAux &aux)
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

void Mandelbulbulb4Iteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux)
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

void MandelbulbulbPower2Iteration(CVector3 &z)
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
	double fR2_2 = foldings->sphericalOuther * foldings->sphericalOuther;
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
