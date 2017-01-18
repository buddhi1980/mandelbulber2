/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Graeme McLaren,
 *  Sebastian Jennen (jenzebas@gmail.com)
 *
 * functions for calculation single fractal iterations
 */

#include "fractal_formulas.hpp"
#include "common_math.h"
#include "commonparams.hpp"
#include "fractal.h"

#define SQRT_1_3 0.57735026918962576450914878050196
#define SQRT_1_2 0.70710678118654752440084436210485
#define SQRT_2_3 0.81649658092772603273242802490196
#define SQRT_3_2 1.22474487139158904909864203735295
#define SQRT_3_4 0.86602540378443864676372317075294
#define SQRT_3_4d2 0.43301270189221932338186158537647
#define SQRT_3 1.73205080756887729352744634150587

#define FRAC_1_3 0.33333333333333333333333333333333

#ifndef M_PI_180
#define M_PI_180 0.01745329251994329576923690768489
#endif

#ifndef M_PI_8
#define M_PI_8 0.39269908169872415480783042290994
#endif

using namespace fractal;
using std::swap;

/**
 * Classic Mandelbulb fractal.
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void MandelbulbIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	// if (aux.r < 1e-21) aux.r = 1e-21;
	double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	double th = th0 * fractal->bulb.power;
	double ph = ph0 * fractal->bulb.power;
	double cth = cos(th);
	aux.r_dz = (rp * aux.r_dz) * fractal->bulb.power + 1.0;
	rp *= aux.r;
	z.x = cth * cos(ph) * rp;
	z.y = cth * sin(ph) * rp;
	z.z = sin(th) * rp;
}

/**
 * Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 */
void MandelboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->mandelbox.rotationsEnabled)
	{
		CVector3 zRot;

		zRot = fractal->mandelbox.rot[0][0].RotateVector(z);
		if (zRot.x > fractal->mandelbox.foldingLimit)
		{
			zRot.x = fractal->mandelbox.foldingValue - zRot.x;
			z = fractal->mandelbox.rotinv[0][0].RotateVector(zRot);
			aux.color += fractal->mandelbox.color.factor.x;
		}
		else
		{
			zRot = fractal->mandelbox.rot[1][0].RotateVector(z);
			if (zRot.x < -fractal->mandelbox.foldingLimit)
			{
				zRot.x = -fractal->mandelbox.foldingValue - zRot.x;
				z = fractal->mandelbox.rotinv[1][0].RotateVector(zRot);
				aux.color += fractal->mandelbox.color.factor.x;
			}
		}

		zRot = fractal->mandelbox.rot[0][1].RotateVector(z);
		if (zRot.y > fractal->mandelbox.foldingLimit)
		{
			zRot.y = fractal->mandelbox.foldingValue - zRot.y;
			z = fractal->mandelbox.rotinv[0][1].RotateVector(zRot);
			aux.color += fractal->mandelbox.color.factor.y;
		}
		else
		{
			zRot = fractal->mandelbox.rot[1][1].RotateVector(z);
			if (zRot.y < -fractal->mandelbox.foldingLimit)
			{
				zRot.y = -fractal->mandelbox.foldingValue - zRot.y;
				z = fractal->mandelbox.rotinv[1][1].RotateVector(zRot);
				aux.color += fractal->mandelbox.color.factor.y;
			}
		}

		zRot = fractal->mandelbox.rot[0][2].RotateVector(z);
		if (zRot.z > fractal->mandelbox.foldingLimit)
		{
			zRot.z = fractal->mandelbox.foldingValue - zRot.z;
			z = fractal->mandelbox.rotinv[0][2].RotateVector(zRot);
			aux.color += fractal->mandelbox.color.factor.z;
		}
		else
		{
			zRot = fractal->mandelbox.rot[1][2].RotateVector(z);
			if (zRot.z < -fractal->mandelbox.foldingLimit)
			{
				zRot.z = -fractal->mandelbox.foldingValue - zRot.z;
				z = fractal->mandelbox.rotinv[1][2].RotateVector(zRot);
				aux.color += fractal->mandelbox.color.factor.z;
			}
		}
	}
	else
	{
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		if (fabs(z.z) > fractal->mandelbox.foldingLimit)
		{
			z.z = sign(z.z) * fractal->mandelbox.foldingValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}

	double r2 = z.Dot(z);

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

	if (fractal->mandelbox.mainRotationEnabled) z = fractal->mandelbox.mainRot.RotateVector(z);

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
	tempR = sqrt(z.x * z.x + z.y * z.y); //+ 1e-061
	z *= 1.0 / tempR;
	temp = z.x * z.x - z.y * z.y;
	z.y = 2.0 * z.x * z.y;
	z.x = temp;
	z *= tempR;

	tempR = sqrt(z.y * z.y + z.z * z.z); //+ 1e-061
	z *= 1.0 / tempR;
	temp = z.y * z.y - z.z * z.z;
	z.z = 2.0 * z.y * z.z;
	z.y = temp;
	z *= tempR;

	tempR = sqrt(z.x * z.x + z.z * z.z); //+ 1e-061
	z *= 1.0 / tempR;
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

	if (z.x < 0.0) sign2 = -1.0;
	tempR = sqrt(z.x * z.x + z.y * z.y); //+ 1e-061
	z *= 1.0 / tempR;
	temp = z.x * z.x - z.y * z.y;
	z.y = 2.0 * z.x * z.y;
	z.x = temp;
	z *= tempR;

	if (z.x < 0.0) sign = -1.0;
	tempR = sqrt(z.x * z.x + z.z * z.z); //+ 1e-061
	z *= 1.0 / tempR;
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
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	aux.r_dz = rp * aux.r_dz * fractal->bulb.power + 1.0;

	double angZ = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double angY = atan2(z.z, z.x) + fractal->bulb.betaAngleOffset;
	double angX = atan2(z.z, z.y) + fractal->bulb.gammaAngleOffset;

	CRotationMatrix rotM;
	rotM.RotateX(angX * (fractal->bulb.power - 1.0));
	rotM.RotateY(angY * (fractal->bulb.power - 1.0));
	rotM.RotateZ(angZ * (fractal->bulb.power - 1.0));

	z = rotM.RotateVector(z) * rp;
}

/**
 * Classic Mandelbulb Power 2 fractal
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void MandelbulbPower2Iteration(CVector3 &z, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
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

/**
 * Xenodreambuie
 * @reference
 * http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/273/
 */
void XenodreambuieIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	aux.r_dz = rp * aux.r_dz * fractal->bulb.power + 1.0;
	rp *= aux.r;

	double th = atan2(z.y, z.x) + fractal->bulb.betaAngleOffset;
	double ph = acos(z.z / aux.r) + fractal->bulb.alphaAngleOffset;

	if (fabs(ph) > 0.5 * M_PI) ph = sign(ph) * M_PI - ph;

	z.x = rp * cos(th * fractal->bulb.power) * sin(ph * fractal->bulb.power);
	z.y = rp * sin(th * fractal->bulb.power) * sin(ph * fractal->bulb.power);
	z.z = rp * cos(ph * fractal->bulb.power);
}

/**
 * Menger Sponge formula created by Knighty
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/kaleidoscopic-(escape-time-ifs)/
 */
void MengerSpongeIteration(CVector3 &z, sExtendedAux &aux)
{
	z.x = fabs(z.x);
	z.y = fabs(z.y);
	z.z = fabs(z.z);

	if (z.x - z.y < 0.0) swap(z.x, z.y);
	if (z.x - z.z < 0.0) swap(z.x, z.z);
	if (z.y - z.z < 0.0) swap(z.y, z.z);

	z *= 3.0;

	z.x -= 2.0;
	z.y -= 2.0;
	if (z.z > 1.0) z.z -= 2.0;

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

	double r2 = z.Dot(z);
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
	if (fabs(z.x) > fractal->foldingIntPow.foldfactor)
		z.x = sign(z.x) * fractal->foldingIntPow.foldfactor * 2.0 - z.x;
	if (fabs(z.y) > fractal->foldingIntPow.foldfactor)
		z.y = sign(z.y) * fractal->foldingIntPow.foldfactor * 2.0 - z.y;
	if (fabs(z.z) > fractal->foldingIntPow.foldfactor)
		z.z = sign(z.z) * fractal->foldingIntPow.foldfactor * 2.0 - z.z;

	double fR2_2 = 1.0;
	double mR2_2 = 0.25;
	double r2_2 = z.Dot(z);
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

	// INFO remark: changed sequence of operation.
	// adding of C constant was before multiplying by z-factor
}

/**
 * Kaleidoscopic Iterated Function Systems (IFS)
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/kaleidoscopic-(escape-time-ifs)/
 */
void KaleidoscopicIFSIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
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

	if (fractal->IFS.rotationEnabled)
		z = fractal->IFS.mainRot.RotateVector(z - fractal->IFS.offset) + fractal->IFS.offset;

	if (fractal->IFS.edge.x > 0.0) z.x = fractal->IFS.edge.x - fabs(fractal->IFS.edge.x - z.x);
	if (fractal->IFS.edge.y > 0.0) z.y = fractal->IFS.edge.y - fabs(fractal->IFS.edge.y - z.y);
	if (fractal->IFS.edge.z > 0.0) z.z = fractal->IFS.edge.z - fabs(fractal->IFS.edge.z - z.z);

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
 * @reference
 * http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/msg7235/#msg7235
 */
void HypercomplexIteration(CVector3 &z, double &w, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	double newx = z.x * z.x - z.y * z.y - z.z * z.z - w * w;
	double newy = 2.0 * z.x * z.y - 2.0 * w * z.z;
	double newz = 2.0 * z.x * z.z - 2.0 * z.y * w;
	double neww = 2.0 * z.x * w - 2.0 * z.y * z.z;
	z.x = newx;
	z.y = newy;
	z.z = newz;
	w = neww;
}

/**
 * Quaternion fractal
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void QuaternionIteration(CVector3 &z, double &w, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	double newx = z.x * z.x - z.y * z.y - z.z * z.z - w * w;
	double newy = 2.0 * z.x * z.y;
	double newz = 2.0 * z.x * z.z;
	double neww = 2.0 * z.x * w;
	z.x = newx;
	z.y = newy;
	z.z = newz;
	w = neww;
}

/**
 * Formula invented by Benesi
 * @reference http://www.fractalforums.com/index.php?action=profile;u=1170
 */
void BenesiIteration(CVector3 &z, CVector3 c, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	double r1 = z.y * z.y + z.z * z.z;
	double newx;
	if (c.x < 0.0 || z.x < sqrt(r1))
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
 * @reference http://www.fractalforums.com/theory/bristorbrot-3d/
 * by Doug Bristor
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

	if (fractal->buffalo.preabsx) z.x = fabs(z.x);
	if (fractal->buffalo.preabsy) z.y = fabs(z.y);
	if (fractal->buffalo.preabsz) z.z = fabs(z.z);

	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double temp = 1.0 - z2 / (x2 + y2);
	double newx = (x2 - y2) * temp;
	double newy = 2.0 * z.x * z.y * temp;
	double newz = (fractal->buffalo.posz ? 2.0 : -2.0) * z.z * sqrt(x2 + y2);

	z.x = fractal->buffalo.absx ? fabs(newx) : newx;
	z.y = fractal->buffalo.absy ? fabs(newy) : newy;
	z.z = fractal->buffalo.absz ? fabs(newz) : newz;
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
	double newx = x2 - 2.0 * z.y * z.z;
	double newy = z2 + 2.0 * z.x * z.y;
	double newz = y2 + 2.0 * z.x * z.z;
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
	double newx = x2 - 2.0 * z.y * z.z;
	double newy = z2 + 2.0 * z.x * z.y;
	double newz = y2 - 2.0 * z.x * z.z;
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
	double newx = x2 + 2.0 * z.y * z.z;
	double newy = -y2 - 2.0 * z.x * z.z;
	double newz = -z2 + 2.0 * z.x * z.y;
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

// NEW FORMULAS-----------------------------------------------------------------

/**
 * ABoxMod1, a formula from Mandelbulb3D.
 * Inspired from a 2D formula proposed by Kali at Fractal Forums
 * This formula has a c.x c.y SWAP
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * kaliset-plus-boxfold-nice-new-2d-fractal/msg33670/#new
 */
void AboxMod1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale =
		aux.actualScale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	z.x = fractal->mandelbox.foldingValue
				- fabs(fabs(z.x + fractal->transformCommon.additionConstant000.x)
							 - fractal->mandelbox.foldingValue)
				- fabs(fractal->transformCommon.additionConstant000.x);
	z.y = fractal->mandelbox.foldingValue
				- fabs(fabs(z.y + fractal->transformCommon.additionConstant000.y)
							 - fractal->mandelbox.foldingValue)
				- fabs(fractal->transformCommon.additionConstant000.y);
	z.z = fractal->mandelbox.foldingValue
				- fabs(fabs(z.z + fractal->transformCommon.additionConstant000.z)
							 - fractal->mandelbox.foldingValue)
				- fabs(fractal->transformCommon.additionConstant000.z);

	double rr = (z.x * z.x + z.y * z.y + z.z * z.z);
	double sqrtMinR = sqrt(fractal->transformCommon.minR0);
	double dividend = rr < sqrtMinR ? sqrtMinR : min(rr, 1.0);
	double m = aux.actualScale / dividend;
	z *= m;
	aux.DE = aux.DE * fabs(m) + 1.0;
}

/**
 * ABoxMod2, Based on a formula from Mandelbulb3D.  Inspired from a 2D formula proposed by Kali at
 * Fractal Forums
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * kaliset-plus-boxfold-nice-new-2d-fractal/msg33670/#new
 */
void AboxMod2Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
				- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
				- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
				- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z; // default was 1.5
	double temp = fabs(z.z) - fractal->transformCommon.offset05;

	double rr;
	if (temp > 0.0)
		rr = z.x * z.x + z.y * z.y + z.z * z.z; // on top & bottom of cyl
	else
		rr = z.x * z.x + z.y * z.y; // on cyl body

	double sqrtMinR = sqrt(fractal->transformCommon.minR05);
	double dividend = rr < sqrtMinR ? sqrtMinR : min(rr, 1.0);
	double m = aux.actualScale / dividend;
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
	double rr = z.Dot(z);
	double MinR = fractal->transformCommon.minR06;
	double dividend = rr < MinR ? MinR : min(rr, 1.0);
	double m = fractal->transformCommon.scale015 / dividend;
	z = z * m;
	aux.DE = aux.DE * fabs(m) + 1.0;
}

/**
 * ABoxModKaliEiffie, modified  formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/
 */
void AboxModKaliEiffieIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
				- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
				- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
				- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;

	aux.color += fractal->mandelbox.color.factor.x;
	aux.color += fractal->mandelbox.color.factor.y;
	aux.color += fractal->mandelbox.color.factor.z;

	if (fractal->transformCommon.functionEnabledxFalse
			&& i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA) // box fold z.z
	{
		double zLimit =
			fractal->transformCommon.additionConstant111.z * fractal->transformCommon.scale1;
		double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}

	if (fractal->transformCommon.functionEnabled)
	{
		z = CVector3(z.z, z.y, z.x); // swap
	}
	if (z.y > z.x) z = CVector3(z.y, z.x, z.z); // conditional

	double rr = z.Dot(z);
	double MinR = fractal->transformCommon.minR05;
	double m = fractal->transformCommon.scale015;
	if (rr < MinR)
	{
		m = m / MinR;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else
	{
		if (rr < 1.0)
		{
			m = m / rr;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
	z = z * m;

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabled)
	{
		CVector3 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfxyz)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(tempC.x, tempC.y, tempC.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(tempC.x, tempC.z, tempC.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(tempC.y, tempC.x, tempC.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(tempC.y, tempC.z, tempC.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(tempC.z, tempC.x, tempC.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(tempC.z, tempC.y, tempC.x); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfxyz)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(c.x, c.y, c.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(c.x, c.z, c.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(c.y, c.x, c.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(c.y, c.z, c.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(c.z, c.x, c.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(c.z, c.y, c.x); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	if (fractal->transformCommon.rotationEnabled && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	aux.DE = aux.DE * fabs(m) + 1.0;

	aux.foldFactor = fractal->foldColor.compFold0; // fold group weight
	aux.minRFactor = fractal->foldColor.compMinR;	// orbit trap weight

	double scaleColor = fractal->foldColor.colorMin + fabs(m);
	// scaleColor += fabs(fractal->mandelbox.scale);
	aux.scaleFactor = scaleColor * fractal->foldColor.compScale;
}

/**
 * ABoxVS_icen1, a formula from Mandelbulb3D.  Inspired from a 2D formula proposed by Kali at
 * Fractal Forums
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * kaliset-plus-boxfold-nice-new-2d-fractal/msg33670/#new
 */
void AboxVSIcen1Iteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale =
		aux.actualScale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

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
	// if (r2 < 1e-21 && r2 > -1e-21)
	//	r2 = (r2 > 0) ? 1e-21 : -1e-21;
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
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void AexionOctopusModIteration(CVector3 &z, CVector3 c, const cFractal *fractal)
{
	CVector3 tempN;
	tempN.x = z.x * z.z * fractal->transformCommon.scale3D111.x;
	tempN.y = (z.x * z.x - z.z * z.z) * fractal->transformCommon.scale3D111.y;
	tempN.z = z.y;

	if (fractal->transformCommon.functionEnabledAx) tempN.x = fabs(tempN.x);
	if (fractal->transformCommon.functionEnabledAy) tempN.y = fabs(tempN.y);
	if (fractal->transformCommon.functionEnabledAz) tempN.z = fabs(tempN.z);

	z = tempN;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;

		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}
}

/**
 * amazing surf from Mandelbulber3D. Formula proposed by Kali, with features added by Darkbeam
 * @reference ????
 * This formula has a c.x c.y SWAP
 */
void AmazingSurfIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale =
		aux.actualScale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
				- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
				- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	// no z fold

	double rr = z.Dot(z);
	if (fractal->transformCommon.functionEnabledFalse) // force cylinder fold
		rr -= z.z * z.z;

	double sqrtMinR = sqrt(fractal->transformCommon.minR05);
	double dividend = rr < sqrtMinR ? sqrtMinR : min(rr, 1.0);
	double m = aux.actualScale / dividend;
	z *= m * fractal->transformCommon.scale1 + 1.0 * (1.0 - fractal->transformCommon.scale1);
	aux.DE = aux.DE * fabs(m) + 1.0;

	if (fractal->transformCommon.addCpixelEnabledFalse)
		z += CVector3(c.y, c.x, c.z) * fractal->transformCommon.constantMultiplier111;

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

/**
 * Based on Amazing Surf Mod 1 from Mandelbulber3D, a formula proposed by Kali,
 * with features added by Darkbeam
 * @reference ????
 */
void AmazingSurfMod1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale =
		aux.actualScale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	if (fractal->transformCommon.functionEnabledAx)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		aux.color += fractal->mandelbox.color.factor.x;
		aux.color += fractal->mandelbox.color.factor.y;
	}

	// z = fold - fabs( fabs(z) - fold)
	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		z.x = fractal->transformCommon.additionConstant111.x
					- fabs(fabs(z.x) - fractal->transformCommon.additionConstant111.x);
		z.y = fractal->transformCommon.additionConstant111.y
					- fabs(fabs(z.y) - fractal->transformCommon.additionConstant111.y);
		aux.color += fractal->mandelbox.color.factor.x;
		aux.color += fractal->mandelbox.color.factor.y;
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
		aux.color += fractal->mandelbox.color.factor.x;
		aux.color += fractal->mandelbox.color.factor.y;
	}

	// if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
	if (fractal->transformCommon.functionEnabledxFalse)
	{
		if (fabs(z.x) > fractal->transformCommon.additionConstant111.x)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
			// aux.color += fractal->mandelbox.color.factor.x;
		}
		if (fabs(z.y) > fractal->transformCommon.additionConstant111.y)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
			// aux.color += fractal->mandelbox.color.factor.y;
		}
		aux.color += fractal->mandelbox.color.factor.x;
		aux.color += fractal->mandelbox.color.factor.y;
	}

	// z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
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
		aux.color += fractal->mandelbox.color.factor.x;
		aux.color += fractal->mandelbox.color.factor.y;
	}

	z += fractal->transformCommon.additionConstant000;

	double r2;
	r2 = z.Dot(z);
	if (fractal->transformCommon.functionEnabledFalse) // force cylinder fold
		r2 -= z.z * z.z;

	// if (r2 < 1e-21)
	//	r2 = 1e-21;

	double sqrtMinR = fractal->transformCommon.sqtR;
	if (r2 < sqrtMinR)
	{
		z *= fractal->transformCommon.mboxFactor1;
		aux.DE *= fractal->transformCommon.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < 1.0)
	{
		z *= 1.0 / r2;
		aux.DE *= 1.0 / r2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}

	z *= aux.actualScale * fractal->transformCommon.scale1
			 + 1.0 * (1.0 - fractal->transformCommon.scale1);

	aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;
	z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	aux.foldFactor = fractal->foldColor.compFold0; // fold group weight
	aux.minRFactor = fractal->foldColor.compMinR;	// orbit trap weight

	double scaleColor = fractal->foldColor.colorMin + fabs(fractal->mandelbox.scale);
	// scaleColor += fabs(fractal->mandelbox.scale);
	aux.scaleFactor = scaleColor * fractal->foldColor.compScale;
}

/**
 * Amazing Surface Multi
 * Based on Amazing Surf Mod 1 from Mandelbulber3D, a formula proposed by Kali,
 *  with features added by Darkbeam
 */
void AmazingSurfMultiIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	bool functionEnabledN[5] = {fractal->transformCommon.functionEnabledAx,
		fractal->transformCommon.functionEnabledAyFalse,
		fractal->transformCommon.functionEnabledAzFalse,
		fractal->transformCommon.functionEnabledBxFalse,
		fractal->transformCommon.functionEnabledByFalse};
	int startIterationN[5] = {fractal->transformCommon.startIterationsA,
		fractal->transformCommon.startIterationsB, fractal->transformCommon.startIterationsC,
		fractal->transformCommon.startIterationsD, fractal->transformCommon.startIterationsE};
	int stopIterationN[5] = {fractal->transformCommon.stopIterationsA,
		fractal->transformCommon.stopIterationsB, fractal->transformCommon.stopIterationsC,
		fractal->transformCommon.stopIterationsD, fractal->transformCommon.stopIterationsE};
	sFractalSurfFolds::multi_orderOfFolds foldN[5] = {fractal->surfFolds.orderOfFolds1,
		fractal->surfFolds.orderOfFolds2, fractal->surfFolds.orderOfFolds3,
		fractal->surfFolds.orderOfFolds4, fractal->surfFolds.orderOfFolds5};

	for (int f = 0; f < 5; f++)
	{
		if (functionEnabledN[f] && i >= startIterationN[f] && i < stopIterationN[f])
		{
			switch (foldN[f])
			{
				case sFractalSurfFolds::type1:
				default:
					z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
								- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
					z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
								- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
					aux.color += fractal->mandelbox.color.factor.x;
					aux.color += fractal->mandelbox.color.factor.y;
					break;
				case sFractalSurfFolds::type2: // z = fold - fabs( fabs(z) - fold)
					z.x = fractal->transformCommon.additionConstant111.x
								- fabs(fabs(z.x) - fractal->transformCommon.additionConstant111.x);
					z.y = fractal->transformCommon.additionConstant111.y
								- fabs(fabs(z.y) - fractal->transformCommon.additionConstant111.y);
					aux.color += fractal->mandelbox.color.factor.x;
					aux.color += fractal->mandelbox.color.factor.y;
					break;
				case sFractalSurfFolds::type3:
					z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
					z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
					aux.color += fractal->mandelbox.color.factor.x;
					aux.color += fractal->mandelbox.color.factor.y;
					break;
				case sFractalSurfFolds::type4:
					// if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
					if (fabs(z.x) > fractal->transformCommon.additionConstant111.x)
					{
						z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
						// aux.color += fractal->mandelbox.color.factor.x;
					}
					if (fabs(z.y) > fractal->transformCommon.additionConstant111.y)
					{
						z.y = sign(z.x) * fractal->mandelbox.foldingValue - z.y;
						// aux.color += fractal->mandelbox.color.factor.y;
					}
					aux.color += fractal->mandelbox.color.factor.x;
					aux.color += fractal->mandelbox.color.factor.y;
					break;
				case sFractalSurfFolds::type5:
					// z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
					z.x = fractal->transformCommon.offset2
								- fabs(fabs(z.x + fractal->transformCommon.additionConstant111.x)
											 - fractal->transformCommon.offset2)
								- fractal->transformCommon.additionConstant111.x;
					z.y = fractal->transformCommon.offset2
								- fabs(fabs(z.y + fractal->transformCommon.additionConstant111.y)
											 - fractal->transformCommon.offset2)
								- fractal->transformCommon.additionConstant111.y;
					break;
			}
		}
	}

	if (fractal->transformCommon.functionEnabledAxFalse)
	{ // enable z scale
		double zLimit = fractal->transformCommon.foldingLimit * fractal->transformCommon.scale0;
		double zValue = fractal->transformCommon.foldingValue * fractal->transformCommon.scale0;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}
	z += fractal->transformCommon.additionConstant000;

	// standard functions
	if (fractal->transformCommon.functionEnabledAy)
	{
		double r2;
		r2 = z.Dot(z);
		if (fractal->transformCommon.functionEnabledFalse)		// force cylinder fold
			r2 -= z.z * z.z * fractal->transformCommon.scaleB1; // fold weight  ;

		if (fractal->transformCommon.functionEnabledAz && i >= fractal->transformCommon.startIterationsT
				&& i < fractal->transformCommon.stopIterationsT)
		{
			// Abox Spherical fold
			z += fractal->mandelbox.offset;
			double sqrtMinR = fractal->transformCommon.sqtR;

			if (r2 < sqrtMinR)
			{
				z *= fractal->transformCommon.mboxFactor1;
				aux.DE *= fractal->transformCommon.mboxFactor1;
				aux.color += fractal->mandelbox.color.factorSp1;
			}
			else if (r2 < 1.0)
			{
				z *= 1.0 / r2;
				aux.DE *= 1.0 / r2;
				aux.color += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->mandelbox.offset;
		}

		// Mandelbox Spherical fold
		if (fractal->transformCommon.functionEnabledzFalse
				&& i >= fractal->transformCommon.startIterationsM
				&& i < fractal->transformCommon.stopIterationsM)
		{
			// r2 = z.Dot(z);
			z += fractal->mandelbox.offset;
			if (r2 < fractal->mandelbox.mR2) // mR2 = minR^2
			{
				z *= fractal->mandelbox.mboxFactor1; // fR2/mR2
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
		}

		if (i >= fractal->transformCommon.startIterationsS
				&& i < fractal->transformCommon.stopIterationsS)
		{ // scale
			z *= fractal->mandelbox.scale * fractal->transformCommon.scale1
					 + 1.0 * (1.0 - fractal->transformCommon.scale1);
			aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
		}
	}
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfxyz)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(tempC.x, tempC.y, tempC.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(tempC.x, tempC.z, tempC.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(tempC.y, tempC.x, tempC.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(tempC.y, tempC.z, tempC.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(tempC.z, tempC.x, tempC.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(tempC.z, tempC.y, tempC.x); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfxyz)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(c.x, c.y, c.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(c.x, c.z, c.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(c.y, c.x, c.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(c.y, c.z, c.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(c.z, c.x, c.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(c.z, c.y, c.x); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	if (fractal->mandelbox.mainRotationEnabled && i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	aux.foldFactor = fractal->foldColor.compFold0; // fold group weight
	aux.minRFactor = fractal->foldColor.compMinR;	// orbit trap weight

	double scaleColor = fractal->foldColor.colorMin + fabs(fractal->mandelbox.scale);
	// scaleColor += fabs(fractal->mandelbox.scale);
	aux.scaleFactor = scaleColor * fractal->foldColor.compScale;
}

/**
 * benesiFastPwr2PineTree  3D
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void BenesiPineTreeIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 temp = z;
	aux.r = z.Length();
	z *= z;
	double t = 2.0 * temp.x;
	if (z.y + z.z > 0.0)
		t = t / sqrt(z.y + z.z);
	else
		t = 1.0;

	z.x = (z.x - z.y - z.z) + c.x * fractal->transformCommon.constantMultiplier100.x;
	z.z = (t * (z.y - z.z)) + c.y * fractal->transformCommon.constantMultiplier100.y;
	z.y = (2.0 * t * temp.y * temp.z) + c.z * fractal->transformCommon.constantMultiplier100.z;
	aux.r_dz = aux.r * aux.r_dz * 2.0 + 1.0;
}

/**
 * benesiT1PineTree 3D
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void BenesiT1PineTreeIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.benesiT1Enabled && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		if (fractal->transformCommon.rotationEnabled)
		{
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.addCpixelEnabled && i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC)
	{
		CVector3 temp = z;
		aux.r = z.Length();
		z *= z;
		double t = 2.0 * temp.x;
		if (z.y + z.z > 0.0)
			t = t / sqrt(z.y + z.z);
		else
			t = 1.0;
		CVector3 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = CVector3(aux.c.x, aux.c.z, aux.c.y);
			aux.c = tempC;
		}
		else
		{
			tempC = CVector3(c.x, c.z, c.y);
		}
		z.x = (z.x - z.y - z.z) + tempC.x * fractal->transformCommon.constantMultiplier100.x;
		z.z = (t * (z.y - z.z)) + tempC.z * fractal->transformCommon.constantMultiplier100.y;
		z.y = (2.0 * t * temp.y * temp.z) + tempC.y * fractal->transformCommon.constantMultiplier100.z;
		aux.r_dz = aux.r * aux.r_dz * 2.0 + 1.0;
	}

	if (fractal->transformCommon.functionEnabledBxFalse
			&& i >= fractal->transformCommon.startIterationsD
			&& i < fractal->transformCommon.stopIterationsD)
	{
		z = CVector3(z.x * cos(z.y * fractal->transformCommon.scale1),
					z.x * sin(z.y * fractal->transformCommon.scale1), z.z * fractal->transformCommon.scaleC1)
				* fractal->transformCommon.scaleA1;
		aux.r_dz *= fabs(fractal->transformCommon.scaleA1);
	}

	if (fractal->transformCommon.juliaMode)
	{
		z.x += fractal->transformCommon.offset000.x;
		z.y += fractal->transformCommon.offset000.y;
		z.z += fractal->transformCommon.offset000.z;
	}
}

/**
 * benesiMagTransforms
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void BenesiMagTransformsIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.benesiT1Enabled && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL =  1e-21;
		double avgScale = z.Length() / tempL;

		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.addCpixelEnabled && i >= fractal->transformCommon.startIterationsT
			&& i < fractal->transformCommon.stopIterationsT)
	{
		CVector3 temp = z;
		aux.r = z.Length();
		z *= z;
		double t = 2.0 * temp.x;
		if (z.y + z.z > 0.0)
			t = t / sqrt(z.y + z.z);
		else
			t = 1.0;
		if (fractal->transformCommon.addCpixelEnabled)
		{
			CVector3 tempC = c;
			if (fractal->transformCommon.alternateEnabledFalse) // alternate
			{
				tempC = CVector3(aux.c.x, aux.c.z, aux.c.y);
				aux.c = tempC;
			}
			else
			{
				tempC = CVector3(c.x, c.z, c.y);
			}
			z.x = (z.x - z.y - z.z) + tempC.x * fractal->transformCommon.constantMultiplier100.x;
			z.z = (t * (z.y - z.z)) + tempC.z * fractal->transformCommon.constantMultiplier100.y;
			z.y =
				(2.0 * t * temp.y * temp.z) + tempC.y * fractal->transformCommon.constantMultiplier100.z;
		}
		aux.r_dz = aux.r * aux.r_dz * 2.0 + 1.0;
	}
	if (fractal->transformCommon.juliaMode)
	{
		z.x += fractal->transformCommon.juliaC.x * fractal->transformCommon.constantMultiplier100.x;
		z.z += fractal->transformCommon.juliaC.y * fractal->transformCommon.constantMultiplier100.y;
		z.y += fractal->transformCommon.juliaC.z * fractal->transformCommon.constantMultiplier100.z;
	}
	// additional transform slot controls
	bool functionEnabledN[5] = {fractal->transformCommon.functionEnabledAxFalse,
		fractal->transformCommon.functionEnabledAyFalse,
		fractal->transformCommon.functionEnabledAzFalse,
		fractal->transformCommon.functionEnabledBxFalse,
		fractal->transformCommon.functionEnabledByFalse};
	int startIterationN[5] = {fractal->transformCommon.startIterationsA,
		fractal->transformCommon.startIterationsB, fractal->transformCommon.startIterationsC,
		fractal->transformCommon.startIterationsD, fractal->transformCommon.startIterationsE};
	int stopIterationN[5] = {fractal->transformCommon.stopIterationsA,
		fractal->transformCommon.stopIterationsB, fractal->transformCommon.stopIterationsC,
		fractal->transformCommon.stopIterationsD, fractal->transformCommon.stopIterationsE};

	sFractalMagTransforms::multi_orderOfTransf transfN[5] = {fractal->magTransf.orderOfTransf1,
		fractal->magTransf.orderOfTransf2, fractal->magTransf.orderOfTransf3,
		fractal->magTransf.orderOfTransf4, fractal->magTransf.orderOfTransf5};

	for (int f = 0; f < 5; f++)
	{
		if (functionEnabledN[f] && i >= startIterationN[f] && i < stopIterationN[f])
		{
			double tempXZ;
			double tempL;
			CVector3 temp;
			double avgScale;
			CVector3 tempV2;

			switch (transfN[f])
			{
				case sFractalMagTransforms::typeT1:
				default:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector3(
						(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);
					z = fabs(z) * fractal->transformCommon.scale3Da222;
					tempL = temp.Length();
					// if (tempL < 1e-21) tempL = 1e-21;
					avgScale = z.Length() / tempL;
					aux.r_dz *= avgScale;
					aux.DE = aux.DE * avgScale + 1.0;
					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
					z = z - fractal->transformCommon.offsetA200;
					break;

				case sFractalMagTransforms::typeT1Mod:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector3(
						(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);
					z = fabs(z) * fractal->transformCommon.scale3D333;
					tempL = temp.Length();
					// if (tempL < 1e-21) tempL = 1e-21;
					avgScale = z.Length() / tempL;
					aux.r_dz *= avgScale;
					aux.DE = aux.DE * avgScale + 1.0;
					z = (fabs(z + fractal->transformCommon.offset111)
							 - fabs(z - fractal->transformCommon.offset111) - z);
					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
					break;

				case sFractalMagTransforms::typeT2:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector3(
						(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);
					tempV2 = z;
					tempV2.x = sqrt(z.y * z.y + z.z * z.z);
					tempV2.y = sqrt(z.x * z.x + z.z * z.z); // switching, squared, sqrt
					tempV2.z = sqrt(z.x * z.x + z.y * z.y);
					z = fabs(tempV2 - fractal->transformCommon.offsetA111);
					temp = z;
					tempL = temp.Length();
					z = fabs(z) * fractal->transformCommon.scale3D444;
					// if (tempL < 1e-21) tempL = 1e-21;
					avgScale = z.Length() / tempL;
					aux.r_dz *= avgScale;
					aux.DE = aux.DE * avgScale + 1.0;
					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
					break;

				case sFractalMagTransforms::typeT3:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector3(
						(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

					tempV2 = z;
					tempV2.x = (z.y + z.z);
					tempV2.y = (z.x + z.z); // switching
					tempV2.z = (z.x + z.y);
					z = (fabs(tempV2 - fractal->transformCommon.offset222))
							* fractal->transformCommon.scale3Db222;

					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
					break;

				case sFractalMagTransforms::typeT4:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector3(
						(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

					tempV2 = z;
					tempV2.x = (z.y * z.y + z.z * z.z);
					tempV2.y = (z.x * z.x + z.z * z.z); // switching, squared,
					tempV2.z = (z.x * z.x + z.y * z.y);
					z = (fabs(tempV2 - fractal->transformCommon.offsetB111))
							* fractal->transformCommon.scale3Dc222;

					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
					break;

				case sFractalMagTransforms::typeT5b:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector3(
						(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);
					// if (z.x > -1e-21 && z.x < 1e-21)
					//	z.x = (z.x > 0) ? 1e-21 : -1e-21;
					// if (z.y > -1e-21 && z.y < 1e-21)
					//	z.y = (z.y > 0) ? 1e-21 : -1e-21;
					// if (z.z > -1e-21 && z.z < 1e-21)
					//	z.z = (z.z > 0) ? 1e-21 : -1e-21;
					tempV2 = z;
					tempV2.x = fabs(
						pow(pow(z.y, fractal->transformCommon.int8X) + pow(z.z, fractal->transformCommon.int8X),
							fractal->transformCommon.power025.x));
					tempV2.y = fabs(
						pow(pow(z.x, fractal->transformCommon.int8Y) + pow(z.z, fractal->transformCommon.int8Y),
							fractal->transformCommon.power025.y));
					tempV2.z = fabs(
						pow(pow(z.x, fractal->transformCommon.int8Z) + pow(z.y, fractal->transformCommon.int8Z),
							fractal->transformCommon.power025.z));
					z = (fabs(tempV2 - fractal->transformCommon.offsetC111))
							* fractal->transformCommon.scale3Dd222;

					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
					break;
			}
		}
	}
}

/**
 * benesiPwr2 mandelbulbs
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void BenesiPwr2sIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	{
		CVector3 gap = fractal->transformCommon.constantMultiplier000;
		double t;
		double dot1;

		if (fractal->transformCommon.functionEnabledPFalse
				&& i >= fractal->transformCommon.startIterationsP
				&& i < fractal->transformCommon.stopIterationsP1)
		{
			z.y = fabs(z.y);
			z.z = fabs(z.z);
			dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
			t = max(0.0, dot1);
			z.x -= t * -SQRT_3;
			z.y = fabs(z.y - t);

			if (z.y > z.z) swap(z.y, z.z);
			z -= gap * CVector3(SQRT_3_4, 1.5, 1.5);

			if (z.z > z.x) swap(z.z, z.x);
			if (z.x > 0.0)
			{
				z.y = max(0.0, z.y);
				z.z = max(0.0, z.z);
			}
		}
	}

	if (fractal->transformCommon.benesiT1Enabled && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		if (fractal->transformCommon.rotationEnabled)
		{ // rotation inside T1
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.addCpixelEnabled && i >= fractal->transformCommon.startIterationsF
			&& i < fractal->transformCommon.stopIterationsF)
	{											// Bensei origional pwr2
		aux.r = z.Length(); // needed when alternating pwr2s
		aux.r_dz = aux.r_dz * 2.0 * aux.r;
		double r1 = z.y * z.y + z.z * z.z;
		CVector3 newZ = CVector3(0.0, 0.0, 0.0);
		if (c.x < 0.0 || z.x < sqrt(r1))
		{
			newZ.x = z.x * z.x - r1;
		}
		else
		{
			newZ.x = -z.x * z.x + r1;
		}
		r1 = -1.0 / sqrt(r1) * 2.0 * fabs(z.x);
		newZ.y = r1 * (z.y * z.y - z.z * z.z);
		newZ.z = r1 * 2.0 * z.y * z.z;
		z = newZ + (c * fractal->transformCommon.constantMultiplierA100);
	}

	if (fractal->transformCommon.addCpixelEnabledFalse
			&& i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC)
	{ // pine tree
		CVector3 temp = z;
		aux.r = z.Length(); // needed when alternating pwr2s
		z *= z;
		double t = 2.0 * temp.x;
		if (z.y + z.z > 0.0)
			t = t / sqrt(z.y + z.z);
		else
			t = 1.0;
		CVector3 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			tempC = CVector3(tempC.x, tempC.z, tempC.y);
			aux.c = tempC;
		}
		else
		{
			tempC = CVector3(c.x, c.z, c.y);
		}
		z.x = (z.x - z.y - z.z) + tempC.x * fractal->transformCommon.constantMultiplier100.x;
		z.z = (t * (z.y - z.z)) + tempC.z * fractal->transformCommon.constantMultiplier100.y;
		z.y = (2.0 * t * temp.y * temp.z) + tempC.y * fractal->transformCommon.constantMultiplier100.z;
		aux.r_dz = aux.r * aux.r_dz * 2.0 + 1.0;
	}

	if (fractal->transformCommon.functionEnabledBxFalse
			&& i >= fractal->transformCommon.startIterationsD
			&& i < fractal->transformCommon.stopIterationsD)
	{
		z = CVector3(z.x * cos(z.y * fractal->transformCommon.scale1),
					z.x * sin(z.y * fractal->transformCommon.scale1), z.z * fractal->transformCommon.scaleC1)
				* fractal->transformCommon.scaleA1;
		aux.r_dz *= fabs(fractal->transformCommon.scaleA1);
	}

	if (fractal->transformCommon.juliaMode)
	{
		z.x += fractal->transformCommon.offset000.x;
		z.y += fractal->transformCommon.offset000.y;
		z.z += fractal->transformCommon.offset000.z;
	}
}

/**
 * CollatzIteration formula
 * @reference https://mathr.co.uk/blog/2016-04-10_collatz_fractal.html
 *            https://en.wikipedia.org/wiki/Collatz_conjecture#Iterating_on_real_or_complex_numbers
 */
void CollatzIteration(CVector3 &z, sExtendedAux &aux)
{

	CVector3 xV(1.0, 1.0, 1.0);
	z = xV + 4.0 * z - CVector3(xV + 2.0 * z) * z.RotateAroundVectorByAngle(xV, M_PI);
	z /= 4.0;
	aux.DE = aux.DE * 4.0 + 1.0;
}

/**
 * CollatzModIteration formula
 * @reference https://mathr.co.uk/blog/2016-04-10_collatz_fractal.html
 *            https://en.wikipedia.org/wiki/Collatz_conjecture#Iterating_on_real_or_complex_numbers
 */
void CollatzModIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{

	z = fractal->transformCommon.constantMultiplierB111 + fractal->transformCommon.scale4 * z
			- (fractal->transformCommon.constantMultiplier111 + fractal->transformCommon.scale2 * z)
					* z.RotateAroundVectorByAngle(fractal->transformCommon.constantMultiplier111,
							M_PI * fractal->transformCommon.scale1); // * cPI ;

	z *= fractal->transformCommon.scale025;

	aux.DE = aux.DE * 4.0 * fractal->analyticDE.scaleLin + 1.0;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		c = CVector3(c.z, c.y, c.x);
		z += c * fractal->transformCommon.constantMultiplierA111;
	}
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
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
		double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}

	if (i >= fractal->transformCommon.startIterationsB
			&& i < fractal->transformCommon.stopIterationsB) // spherical fold
	{
		double r2 = z.Dot(z);
		// if (r2 < 1e-21 && r2 > -1e-21)
		//	r2 = (r2 > 0) ? 1e-21 : -1e-21;
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
	// if (wr < 1e-21)
	//	wr = 1e-21;
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

	z *= wr; // then add Cpixel constant
}

/**
 * Based on Kalisets1 and KaliDucks, from Mandelbulb 3D, and refer Formula proposed by Kali, with
 * features added by Darkbeam.
 *
 * <b>M3D notes:</b>
 * Try out julias and low R_bailout values of 2 down to 1!
 * You might have to cutoff at z=0 or so, to see something.
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/very-simple-formula-for-fractal-patterns
 */
void Kalisets1Iteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	z = fabs(z);
	double sqs = (z.x * z.x + z.y * z.y + z.z * z.z + 1e-21); // sph inv
	double m;
	double minR = fractal->transformCommon.minR0; //  KaliDucks

	if (sqs < minR)
		m = 1.0 / sqrt(minR);
	else
		m = fractal->transformCommon.scale / sqs; // kalisets

	z = z * m;
	aux.DE = aux.DE * fabs(m) + 1.0;

	if (fractal->transformCommon.addCpixelEnabledFalse)
		z += c * fractal->transformCommon.constantMultiplier111;

	if (fractal->transformCommon.juliaMode) z += fractal->transformCommon.juliaC;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

/**
 * Mandelbox Menger Sponge Hybrid
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 */
void MandelboxMengerIteration(
	CVector3 &z, CVector3 c, int &i, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->mandelbox.rotationsEnabled)
	{
		CVector3 zRot;

		zRot = fractal->mandelbox.rot[0][0].RotateVector(z);
		if (zRot.x > fractal->mandelbox.foldingLimit)
		{
			zRot.x = fractal->mandelbox.foldingValue - zRot.x;
			z = fractal->mandelbox.rotinv[0][0].RotateVector(zRot);
			aux.color += fractal->mandelbox.color.factor.x;
		}
		else
		{
			zRot = fractal->mandelbox.rot[1][0].RotateVector(z);
			if (zRot.x < -fractal->mandelbox.foldingLimit)
			{
				zRot.x = -fractal->mandelbox.foldingValue - zRot.x;
				z = fractal->mandelbox.rotinv[1][0].RotateVector(zRot);
				aux.color += fractal->mandelbox.color.factor.x;
			}
		}

		zRot = fractal->mandelbox.rot[0][1].RotateVector(z);
		if (zRot.y > fractal->mandelbox.foldingLimit)
		{
			zRot.y = fractal->mandelbox.foldingValue - zRot.y;
			z = fractal->mandelbox.rotinv[0][1].RotateVector(zRot);
			aux.color += fractal->mandelbox.color.factor.y;
		}
		else
		{
			zRot = fractal->mandelbox.rot[1][1].RotateVector(z);
			if (zRot.y < -fractal->mandelbox.foldingLimit)
			{
				zRot.y = -fractal->mandelbox.foldingValue - zRot.y;
				z = fractal->mandelbox.rotinv[1][1].RotateVector(zRot);
				aux.color += fractal->mandelbox.color.factor.y;
			}
		}

		zRot = fractal->mandelbox.rot[0][2].RotateVector(z);
		if (zRot.z > fractal->mandelbox.foldingLimit)
		{
			zRot.z = fractal->mandelbox.foldingValue - zRot.z;
			z = fractal->mandelbox.rotinv[0][2].RotateVector(zRot);
			aux.color += fractal->mandelbox.color.factor.z;
		}
		else
		{
			zRot = fractal->mandelbox.rot[1][2].RotateVector(z);
			if (zRot.z < -fractal->mandelbox.foldingLimit)
			{
				zRot.z = -fractal->mandelbox.foldingValue - zRot.z;
				z = fractal->mandelbox.rotinv[1][2].RotateVector(zRot);
				aux.color += fractal->mandelbox.color.factor.z;
			}
		}
	}
	else
	{
		if (i >= fractal->transformCommon.startIterationsA
				&& i < fractal->transformCommon.stopIterationsA)
		{
			if (fabs(z.x) > fractal->mandelbox.foldingLimit)
			{
				z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
				aux.color += fractal->mandelbox.color.factor.x;
			}
			if (fabs(z.y) > fractal->mandelbox.foldingLimit)
			{
				z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
				aux.color += fractal->mandelbox.color.factor.y;
			}
			double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
			double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
			if (fabs(z.z) > zLimit)
			{
				z.z = sign(z.z) * zValue - z.z;
				aux.color += fractal->mandelbox.color.factor.z;
			}
		}
	}

	if (i >= fractal->transformCommon.startIterationsB
			&& i < fractal->transformCommon.stopIterationsB)
	{
		double r2 = z.Dot(z);
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
	}
	if (fractal->mandelbox.mainRotationEnabled && i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	if (i >= fractal->transformCommon.startIterationsS
			&& i < fractal->transformCommon.stopIterationsS)
	{
		z = z * fractal->mandelbox.scale;
		aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfxyz)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(tempC.x, tempC.y, tempC.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(tempC.x, tempC.z, tempC.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(tempC.y, tempC.x, tempC.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(tempC.y, tempC.z, tempC.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(tempC.z, tempC.x, tempC.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(tempC.z, tempC.y, tempC.x); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfxyz)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(c.x, c.y, c.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(c.x, c.z, c.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(c.y, c.x, c.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(c.y, c.z, c.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(c.z, c.x, c.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(c.z, c.y, c.x); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}
	if (fractal->transformCommon.functionEnabled && i >= fractal->transformCommon.startIterationsM
			&& i < fractal->transformCommon.stopIterationsM)
	{
		int count = fractal->transformCommon.int1; // Menger Sponge
		int k;
		for (k = 0; k < count; k++)
		{
			z = fabs(z);
			if (z.x - z.y < 0.0) swap(z.y, z.x);
			if (z.x - z.z < 0.0) swap(z.z, z.x);
			if (z.y - z.z < 0.0) swap(z.z, z.y);
			z *= fractal->transformCommon.scale3;
			z.x -= 2.0 * fractal->transformCommon.constantMultiplierA111.x;
			z.y -= 2.0 * fractal->transformCommon.constantMultiplierA111.y;
			if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplierA111.z;
			aux.DE *= fractal->transformCommon.scale3 * fractal->transformCommon.scaleA1;

			z += fractal->transformCommon.additionConstantA000;

			if (fractal->transformCommon.functionEnabledxFalse) // addCpixel options
			{
				switch (fractal->mandelbulbMulti.orderOfxyzC)
				{
					case sFractalMandelbulbMulti::xyz:
					default: c = CVector3(c.x, c.y, c.z); break;
					case sFractalMandelbulbMulti::xzy: c = CVector3(c.x, c.z, c.y); break;
					case sFractalMandelbulbMulti::yxz: c = CVector3(c.y, c.x, c.z); break;
					case sFractalMandelbulbMulti::yzx: c = CVector3(c.y, c.z, c.x); break;
					case sFractalMandelbulbMulti::zxy: c = CVector3(c.z, c.x, c.y); break;
					case sFractalMandelbulbMulti::zyx: c = CVector3(c.z, c.y, c.x); break;
				}
				z += c * fractal->transformCommon.constantMultiplierB111;
			}
		}
	}
	aux.foldFactor = fractal->foldColor.compFold;
	aux.minRFactor = fractal->foldColor.compMinR;
	double scaleColor = fractal->foldColor.colorMin + fabs(fractal->mandelbox.scale);
	// scaleColor += fabs(fractal->mandelbox.scale);
	aux.scaleFactor = scaleColor * fractal->foldColor.compScale;
}

/**
 * mandelbulbBermarte
 *
 * abs() version of Mandelbulb Kali modified by visual.bermarte
 * @reference http://www.fractalforums.com/theory/mandelbulb-variant/
 */
void MandelbulbBermarteIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);

	double th0;
	double ph0;
	double costh;
	double sinth;
	double zp;

	if (fractal->transformCommon.functionEnabledFalse)
	{
		th0 = acos(z.z / aux.r) + fractal->bulb.betaAngleOffset + 1e-061; // MUST keep exception catch
		ph0 = atan(z.y / z.x) + fractal->bulb.alphaAngleOffset;
		th0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleA1;
		sinth = sin(th0);
		z = aux.r * CVector3(sinth * cos(ph0), sin(ph0) * sinth, cos(th0));
	}
	else
	{
		th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset + 1e-061; // MUST keep exception catch
		ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
		th0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleA1;
		costh = cos(th0);
		z = aux.r * CVector3(costh * sin(ph0), cos(ph0) * costh, sin(th0));
	}
	if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAz) z.z = fabs(z.z);

	if (fractal->transformCommon.functionEnabledxFalse)
	{
		th0 = acos(z.z / aux.r) + fractal->transformCommon.betaAngleOffset
					+ 1e-061; // MUST keep exception catch ??;
		ph0 = atan(z.y / z.x);
		ph0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleB1;
		zp = pow(aux.r, fractal->transformCommon.pwr8);
		sinth = sin(th0);
		z = zp * CVector3(sinth * cos(ph0), sin(ph0) * sinth, cos(th0));
	}
	else
	{
		th0 = asin(z.z / aux.r) + fractal->transformCommon.betaAngleOffset
					+ 1e-061; // MUST keep exception catch ??;
		ph0 = atan2(z.y, z.x);
		ph0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleB1;
		zp = pow(aux.r, fractal->transformCommon.pwr8);
		costh = cos(th0);
		z = zp * CVector3(costh * sin(ph0), cos(ph0) * costh, sin(th0));
	}

	if (fractal->transformCommon.functionEnabledBxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledByFalse) z.y = fabs(z.y);

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		aux.r_dz = pow(aux.r, fractal->transformCommon.pwr8 - fractal->analyticDE.offset1) * aux.r_dz
								 * fractal->transformCommon.pwr8 * fractal->analyticDE.scale1
							 + fractal->analyticDE.offset2;
	}
	else // default, i.e. scale1 & offset1 & offset2 = 1.0
	{
		aux.r_dz =
			pow(aux.r, fractal->transformCommon.pwr8 - 1.0) * fractal->transformCommon.pwr8 * aux.r_dz
			+ 1.0;
	}
}

/**
 * mandelbulb Kali modification
 * @reference http://www.fractalforums.com/theory/mandelbulb-variant/
 */
void MandelbulbKaliIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	double th0 =
		acos(z.z / aux.r) + fractal->bulb.betaAngleOffset + 1e-061; // MUST keep exception catch
	double ph0 = atan(z.y / z.x) + fractal->bulb.alphaAngleOffset;
	th0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleA1;
	double sinth = sin(th0);
	z = aux.r * CVector3(sinth * cos(ph0), sin(ph0) * sinth, cos(th0));

	th0 = acos(z.z / aux.r) + fractal->transformCommon.betaAngleOffset
				+ 1e-061; // MUST keep exception catch ??;
	ph0 = atan(z.y / z.x);
	ph0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleB1;
	double zp = pow(aux.r, fractal->transformCommon.pwr8);
	sinth = sin(th0);
	z = zp * CVector3(sinth * cos(ph0), sin(ph0) * sinth, cos(th0));

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		aux.r_dz = pow(aux.r, fractal->transformCommon.pwr8 - fractal->analyticDE.offset1) * aux.r_dz
								 * fractal->transformCommon.pwr8 * fractal->analyticDE.scale1
							 + fractal->analyticDE.offset2;
	}
	else // default, i.e. scale1 & offset1 & offset2 = 1.0
	{
		aux.r_dz =
			pow(aux.r, fractal->transformCommon.pwr8 - 1.0) * fractal->transformCommon.pwr8 * aux.r_dz
			+ 1.0;
	}
}

/**
 * based on mandelbulb Kali multi
 * @reference http://www.fractalforums.com/theory/mandelbulb-variant/
 */
void MandelbulbKaliMultiIteration(
	CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}
	double costh;
	double sinth;
	double th0 = fractal->bulb.betaAngleOffset + 1e-061; // MUST keep exception catch
	double ph0 = fractal->bulb.alphaAngleOffset;
	CVector3 v;
	switch (fractal->mandelbulbMulti.orderOfxyz)
	{
		case sFractalMandelbulbMulti::xyz:
		default: v = CVector3(z.x, z.y, z.z); break;
		case sFractalMandelbulbMulti::xzy: v = CVector3(z.x, z.z, z.y); break;
		case sFractalMandelbulbMulti::yxz: v = CVector3(z.y, z.x, z.z); break;
		case sFractalMandelbulbMulti::yzx: v = CVector3(z.y, z.z, z.x); break;
		case sFractalMandelbulbMulti::zxy: v = CVector3(z.z, z.x, z.y); break;
		case sFractalMandelbulbMulti::zyx: v = CVector3(z.z, z.y, z.x); break;
	}

	if (fractal->mandelbulbMulti.acosOrasin == sFractalMandelbulbMulti::acos)
		th0 += acos(v.x / aux.r);
	else
		th0 += asin(v.x / aux.r);
	if (fractal->mandelbulbMulti.atanOratan2 == sFractalMandelbulbMulti::atan)
		ph0 += atan(v.y / v.z);
	else
		ph0 += atan2(v.y, v.z);

	th0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleA1;

	if (fractal->transformCommon.functionEnabledBxFalse)
	{
		costh = cos(th0);
		z = aux.r * CVector3(costh * sin(ph0), cos(ph0) * costh, sin(th0));
	}
	else
	{ // cosine mode default
		sinth = sin(th0);
		z = aux.r * CVector3(sinth * cos(ph0), sin(ph0) * sinth, cos(th0));
	}

	if (fractal->transformCommon.functionEnabledxFalse)
	{
		switch (fractal->mandelbulbMulti.orderOfxyz2)
		{
			case sFractalMandelbulbMulti::xyz:
			default: v = CVector3(z.x, z.y, z.z); break;
			case sFractalMandelbulbMulti::xzy: v = CVector3(z.x, z.z, z.y); break;
			case sFractalMandelbulbMulti::yxz: v = CVector3(z.y, z.x, z.z); break;
			case sFractalMandelbulbMulti::yzx: v = CVector3(z.y, z.z, z.x); break;
			case sFractalMandelbulbMulti::zxy: v = CVector3(z.z, z.x, z.y); break;
			case sFractalMandelbulbMulti::zyx: v = CVector3(z.z, z.y, z.x); break;
		}
		if (fractal->mandelbulbMulti.acosOrasinA == sFractalMandelbulbMulti::acos)
			th0 = acos(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						+ 1e-061; // MUST keep exception catch
		else
			th0 += asin(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						 + 1e-061; // MUST keep exception catch;

		if (fractal->mandelbulbMulti.atanOratan2A == sFractalMandelbulbMulti::atan)
			ph0 += atan(v.y / v.z);
		else
			ph0 += atan2(v.y, v.z);
	}
	else
	{
		switch (fractal->mandelbulbMulti.orderOfxyz)
		{
			case sFractalMandelbulbMulti::xyz:
			default: v = CVector3(z.x, z.y, z.z); break;
			case sFractalMandelbulbMulti::xzy: v = CVector3(z.x, z.z, z.y); break;
			case sFractalMandelbulbMulti::yxz: v = CVector3(z.y, z.x, z.z); break;
			case sFractalMandelbulbMulti::yzx: v = CVector3(z.y, z.z, z.x); break;
			case sFractalMandelbulbMulti::zxy: v = CVector3(z.z, z.x, z.y); break;
			case sFractalMandelbulbMulti::zyx: v = CVector3(z.z, z.y, z.x); break;
		}

		if (fractal->mandelbulbMulti.acosOrasin == sFractalMandelbulbMulti::acos)
			th0 = acos(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						+ 1e-061; // MUST keep exception catch ??;
		else
			th0 += asin(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						 + 1e-061; // MUST keep exception catch ??;

		if (fractal->mandelbulbMulti.atanOratan2 == sFractalMandelbulbMulti::atan)
			ph0 += atan(v.y / v.z);
		else
			ph0 += atan2(v.y, v.z);
	}

	ph0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleB1 * 0.5; // 0.5 retain
	double zp = pow(aux.r, fractal->transformCommon.pwr8);

	if (fractal->transformCommon.functionEnabledzFalse)
	{ // sine mode
		costh = cos(th0);
		z = zp * CVector3(costh * sin(ph0), cos(ph0) * costh, sin(th0));
	}
	else
	{ // cosine mode default
		sinth = sin(th0);
		z = zp * CVector3(sinth * cos(ph0), sin(ph0) * sinth, cos(th0));
	}

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		aux.r_dz = pow(aux.r, fractal->transformCommon.pwr8 - fractal->analyticDE.offset1) * aux.r_dz
								 * fractal->transformCommon.pwr8 * fractal->analyticDE.scale1
							 + fractal->analyticDE.offset2;
	}
	else // default, i.e. scale1 & offset1 & offset2 = 1.0
	{
		aux.r_dz =
			pow(aux.r, fractal->transformCommon.pwr8 - 1.0) * fractal->transformCommon.pwr8 * aux.r_dz
			+ 1.0;
	}
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfxyzC)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(tempC.x, tempC.y, tempC.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(tempC.x, tempC.z, tempC.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(tempC.y, tempC.x, tempC.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(tempC.y, tempC.z, tempC.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(tempC.z, tempC.x, tempC.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(tempC.z, tempC.y, tempC.x); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfxyzC)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(c.x, c.y, c.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(c.x, c.z, c.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(c.y, c.x, c.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(c.y, c.z, c.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(c.z, c.x, c.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(c.z, c.y, c.x); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}
}

/**
 * mandelbulbMulti 3D
 */
void MandelbulbMultiIteration(CVector3 &z, CVector3 &c, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r = z.Length();
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	double th0 = fractal->bulb.betaAngleOffset;
	double ph0 = fractal->bulb.alphaAngleOffset;
	CVector3 v;

	switch (fractal->mandelbulbMulti.orderOfxyz)
	{
		case sFractalMandelbulbMulti::xyz:
		default: v = CVector3(z.x, z.y, z.z); break;
		case sFractalMandelbulbMulti::xzy: v = CVector3(z.x, z.z, z.y); break;
		case sFractalMandelbulbMulti::yxz: v = CVector3(z.y, z.x, z.z); break;
		case sFractalMandelbulbMulti::yzx: v = CVector3(z.y, z.z, z.x); break;
		case sFractalMandelbulbMulti::zxy: v = CVector3(z.z, z.x, z.y); break;
		case sFractalMandelbulbMulti::zyx: v = CVector3(z.z, z.y, z.x); break;
	}
	// if (aux.r < 1e-21)
	//	aux.r = 1e-21;
	// if (v3 < 1e-21 && v3 > -1e-21)
	//	v3 = (v3 > 0) ? 1e-21 : -1e-21;

	if (fractal->mandelbulbMulti.acosOrasin == sFractalMandelbulbMulti::acos)
		th0 += acos(v.x / aux.r);
	else
		th0 += asin(v.x / aux.r);

	if (fractal->mandelbulbMulti.atanOratan2 == sFractalMandelbulbMulti::atan)
		ph0 += atan(v.y / v.z);
	else
		ph0 += atan2(v.y, v.z);

	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	double th = th0 * fractal->bulb.power * fractal->transformCommon.scaleA1;
	double ph = ph0 * fractal->bulb.power * fractal->transformCommon.scaleB1;

	aux.r_dz = rp * aux.r_dz * fractal->bulb.power + 1.0;
	rp *= aux.r;

	if (fractal->transformCommon.functionEnabledxFalse)
	{ // cosine mode
		double sinth = th;
		if (fractal->transformCommon.functionEnabledyFalse) sinth = th0;
		sinth = sin(sinth);
		z = rp * CVector3(sinth * sin(ph), cos(ph) * sinth, cos(th));
	}
	else
	{ // sine mode ( default = V2.07))
		double costh = th;
		if (fractal->transformCommon.functionEnabledzFalse) costh = th0;
		costh = cos(costh);
		z = rp * CVector3(costh * cos(ph), sin(ph) * costh, sin(th));
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfxyzC)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(tempC.x, tempC.y, tempC.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(tempC.x, tempC.z, tempC.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(tempC.y, tempC.x, tempC.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(tempC.y, tempC.z, tempC.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(tempC.z, tempC.x, tempC.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(tempC.z, tempC.y, tempC.x); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfxyzC)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(c.x, c.y, c.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(c.x, c.z, c.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(c.y, c.x, c.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(c.y, c.z, c.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(c.z, c.x, c.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(c.z, c.y, c.x); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}
}

/**
 * mandelbulb vary scaleV1
 */
void MandelbulbVaryPowerV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double tempVC = fractal->bulb.power; // constant to be varied

	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
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

	aux.r = z.Length();
	// if (aux.r < 1e-21)
	//	aux.r = 1e-21;
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
 * Menger Cross KIFS
 * from code by Knighty
 * http://www.fractalforums.com/fragmentarium/
 * cross-menger!-can-anyone-do-this/msg93972/#new
	*/
void MengerCrossKIFSIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;
	if (fractal->transformCommon.functionEnabledFalse && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledFFalse) z.x = fabs(z.x);

		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= (t * -SQRT_3) - 0.5;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z.y -= 0.75;
		z -= gap * CVector3(SQRT_3_4, 1.5, 1.5);

		if (z.z > z.x) swap(z.z, z.x);

		z *= fractal->transformCommon.constantMultiplier111; // post scale

		if (fractal->transformCommon.functionEnabledyFalse)
		{
			z.y = max(0.0, z.y) * fractal->transformCommon.scaleA1;
			z.z = max(0.0, z.z) * fractal->transformCommon.scaleB1;
		}

		z += fractal->transformCommon.additionConstant000; // post offset
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

		aux.DE *= fractal->analyticDE.scale1; // tweak
	}

	if (fractal->transformCommon.functionEnabled && i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA)
	{ // CrossMengerKIFS
		float lengthL;
		// z = ( z );//+ preAdd );
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledzFalse) z.x = fabs(z.x);

		lengthL = z.x * SQRT_3_4 - z.y * 0.5;
		if (lengthL < 0.0)
		{
			z.x -= SQRT_3_4 * (2.0 * lengthL);
			z.y -= -lengthL;
		}
		lengthL = z.y;
		if (lengthL < 0.0)
		{
			z.y -= 2.0 * lengthL;
		}
		lengthL = (-z.y + z.z) * SQRT_1_2;
		if (lengthL < 0.0)
		{
			z.y -= -SQRT_1_2 * (2.0 * lengthL);
			z.z -= SQRT_1_2 * (2.0 * lengthL);
		}
		lengthL = z.y;
		if (lengthL < 0.5)
		{
			z.y -= 2.0 * (lengthL - 0.5);
		}
		CVector3 edge = fractal->transformCommon.offset222;
		if (fractal->transformCommon.functionEnabledxFalse)
		{
			if (fractal->transformCommon.functionEnabledAxFalse)
			{
				z.x = edge.x - fabs(edge.x - z.x);
			}
			else
			{
				z.x = edge.x - z.x;
			}

			if (fractal->transformCommon.functionEnabledAyFalse)
			{
				z.y = edge.y - fabs(edge.y - z.y);
			}
			else
			{
				z.y = edge.y - z.y;
			}

			if (fractal->transformCommon.functionEnabledAzFalse)
			{
				z.z = edge.z - fabs(edge.z - z.z);
			}
			else
			{
				z.z = edge.z - z.z;
			}
		}

		z *= fractal->transformCommon.scale3;
		z -= CVector3(SQRT_3_4, 1.5, 1.5) * (fractal->transformCommon.scale3 - 1.0);
		aux.DE *= fractal->transformCommon.scale3;

		z += fractal->transformCommon.offset000; // post offset
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			z.x = fabs(z.x + fractal->transformCommon.offset) + fractal->transformCommon.offset0;
		}
		if (fractal->transformCommon.functionEnabledRFalse
				&& i >= fractal->transformCommon.startIterationsR
				&& i < fractal->transformCommon.stopIterationsR)
		{
			z = fractal->transformCommon.rotationMatrix2.RotateVector(z);
		}
	}
}

/**
 * Menger Cross Mod1
 * from code by Knighty
 * http://www.fractalforums.com/fragmentarium/
 * cross-menger!-can-anyone-do-this/msg93972/#new
 */
void MengerCrossMod1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;
	if (fractal->transformCommon.functionEnabledx && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledFFalse) z.x = fabs(z.x);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3 - (0.5 * SQRT_3_4);

		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z.y -= 1.5;
		z -= gap * CVector3(SQRT_3_4, -1.5, 1.5);

		if (z.z > z.x) swap(z.z, z.x);
		if (fractal->transformCommon.functionEnabledyFalse)
		{
			if (z.x >= 0.0)
			{
				z.y = max(0.0, z.y) * fractal->transformCommon.scaleA1;
				z.z = max(0.0, z.z) * fractal->transformCommon.scaleB1;
			}
		}
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		aux.DE *= fractal->analyticDE.scale1; // tweak
	}

	if (fractal->transformCommon.functionEnabledy && i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA)
	{ // CrossMengerTrick
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledzFalse) z.x = fabs(z.x);

		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5);
		double t = 1.0 * max(0.0, dot1);
		z.x -= t * -SQRT_3;
		if (fractal->transformCommon.functionEnabledXFalse)
			z.y = fabs(z.y) - t;
		else
		{
			z.y = fabs(z.y - t);
		}
		z.x -= SQRT_3_4;

		// Choose nearest corner/edge to get translation symmetry (all y & z code)
		double dy = 0.0;
		double dz = 0.0;
		if (z.y > 0.5 && z.z > 0.5) // if both y & z > 0.5  then =1.5
		{
			dy = 1.5;
			dz = 1.5;
		}
		else if (z.z < z.y)
		{
			dy = 1.5; // and dz is unchanged
		}
		else
			dz = 1.5; // and dy is unchanged

		z.y -= dy;
		z.z -= dz;
		z *= fractal->transformCommon.scale3;
		aux.DE *= fractal->transformCommon.scale3;
		z.y += dy;
		z.z += dz;

		z.x += SQRT_3_4;

		if (fractal->transformCommon.functionEnabledPFalse)
		{
			z.x = fabs(z.x + fractal->transformCommon.offset) + fractal->transformCommon.offset0;
		}
		if (fractal->transformCommon.functionEnabledRFalse
				&& i >= fractal->transformCommon.startIterationsR
				&& i < fractal->transformCommon.stopIterationsR)
		{
			z = fractal->transformCommon.rotationMatrix2.RotateVector(z);
		}
	}
}

/**
 * Menger Sponge formula modified by Mclarekin
 * from code by Knighty
 */
void MengerMod1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	z = fabs(z);
	if (z.x - z.y < 0.0) swap(z.y, z.x);
	if (z.x - z.z < 0.0) swap(z.z, z.x);
	if (z.y - z.z < 0.0) swap(z.z, z.y);
	z *= fractal->transformCommon.scale3;
	z.x -= 2.0 * fractal->transformCommon.constantMultiplier111.x;
	z.y -= 2.0 * fractal->transformCommon.constantMultiplier111.y;
	if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;

	aux.DE *= fractal->transformCommon.scale3;

	if (fractal->transformCommon.rotationEnabled && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	z += fractal->transformCommon.additionConstant000;
	if (fractal->transformCommon.functionEnabledxFalse
			&& i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA) // box offset
	{
		CVector3 temp = z;
		z.x = sign(z.x) * fractal->transformCommon.additionConstantA000.x + z.x;
		z.y = sign(z.y) * fractal->transformCommon.additionConstantA000.y + z.y;
		z.z = sign(z.z) * fractal->transformCommon.additionConstantA000.z + z.z;

		if (fractal->transformCommon.functionEnabledFalse)
		{
			double tempL = temp.Length();
			// if (tempL < 1e-21) tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			aux.r_dz *= avgScale;
			aux.DE = aux.DE * avgScale + 1.0;
		}
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector3 zA = (i == fractal->transformCommon.intA) ? z : CVector3();
		CVector3 zB = (i == fractal->transformCommon.intB) ? z : CVector3();

		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offset)
				+ (zB * fractal->transformCommon.offset0);
		aux.DE *= fractal->transformCommon.scale1;
		aux.r_dz *= fractal->transformCommon.scale1;
	}
}

/**
 * Menger Middle Mod
 */
void MengerMiddleModIteration(
	CVector3 &z, CVector3 &c, int i, const cFractal *fractal, sExtendedAux &aux)
{ // fabs() and menger fold
	z = fabs(z + fractal->transformCommon.additionConstantA000);
	if (z.x - z.y < 0.0) swap(z.y, z.x);
	if (z.x - z.z < 0.0) swap(z.z, z.x);
	if (z.y - z.z < 0.0) swap(z.z, z.y);

	if (fractal->mandelbox.mainRotationEnabled && i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC) // rotation
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	if (fractal->transformCommon.functionEnabledBxFalse
			&& i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA) // box fold
	{
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
		double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}
	if (fractal->transformCommon.functionEnabledByFalse
			&& i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations)
	{ // fabsBoxFold
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

		if (i >= fractal->transformCommon.startIterationsB
				&& i < fractal->transformCommon.stopIterationsB)
		{
			z *= fractal->transformCommon.scaleA1;
			aux.DE *= fabs(fractal->transformCommon.scaleA1);
		}
	}
	// menger scales and offsets
	z *= fractal->transformCommon.scale3;
	z.x -= 2.0 * fractal->transformCommon.constantMultiplier111.x;
	z.y -= 2.0 * fractal->transformCommon.constantMultiplier111.y;
	if (fractal->transformCommon.functionEnabled)
	{
		if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
	}
	else
	{
		z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
	}
	aux.DE *= fractal->transformCommon.scale3;

	if (fractal->transformCommon.addCpixelEnabledFalse) // addCpixel options
	{
		switch (fractal->mandelbulbMulti.orderOfxyzC)
		{
			case sFractalMandelbulbMulti::xyz:
			default: c = CVector3(c.x, c.y, c.z); break;
			case sFractalMandelbulbMulti::xzy: c = CVector3(c.x, c.z, c.y); break;
			case sFractalMandelbulbMulti::yxz: c = CVector3(c.y, c.x, c.z); break;
			case sFractalMandelbulbMulti::yzx: c = CVector3(c.y, c.z, c.x); break;
			case sFractalMandelbulbMulti::zxy: c = CVector3(c.z, c.x, c.y); break;
			case sFractalMandelbulbMulti::zyx: c = CVector3(c.z, c.y, c.x); break;
		}
		z += c * fractal->transformCommon.constantMultiplierC111;
	}
}

/**
 * Menger Sponge and octo
 * from code by Knighty
 */
void MengerOctoIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{ // octo
	if (i >= fractal->transformCommon.startIterationsE
			&& i < fractal->transformCommon.stopIterationsE)
	{
		if (z.x + z.y < 0.0) z = CVector3(-z.y, -z.x, z.z);

		if (z.x + z.z < 0.0) // z.xz = -z.zx;
			z = CVector3(-z.z, z.y, -z.x);

		if (z.x - z.y < 0.0) // z.xy = z.yx;
			z = CVector3(z.y, z.x, z.z);

		if (z.x - z.z < 0.0) // z.xz = z.zx;
			z = CVector3(z.z, z.y, z.x);

		z.x = fabs(z.x);
		z = z * fractal->transformCommon.scale2
				- fractal->transformCommon.offset100 * (fractal->transformCommon.scale2 - 1.0);

		aux.DE *= fractal->transformCommon.scale2;
	}
	// box offset
	if (fractal->transformCommon.functionEnabledxFalse
			&& i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA)
	{
		CVector3 temp = z;
		z.x = sign(z.x) * fractal->transformCommon.additionConstantA000.x + z.x;
		z.y = sign(z.y) * fractal->transformCommon.additionConstantA000.y + z.y;
		z.z = sign(z.z) * fractal->transformCommon.additionConstantA000.z + z.z;

		if (fractal->transformCommon.functionEnabledzFalse)
		{
			double tempL = temp.Length();
			// if (tempL < 1e-21) tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			aux.DE = aux.DE * avgScale + 1.0;
		}
	}
	// spherical fold
	if (fractal->transformCommon.functionEnabledSFalse
			&& i >= fractal->transformCommon.startIterationsS
			&& i < fractal->transformCommon.stopIterationsS)
	{
		double para = 0.0;
		double paraAddP0 = 0.0;
		if (fractal->transformCommon.functionEnabledyFalse)
		{
			// para += paraAddP0 + fractal->transformCommon.minR2p25;
			if (fractal->Cpara.enabledLinear)
			{
				para = fractal->Cpara.para00; // parameter value at iter 0
				double temp0 = para;
				double tempA = fractal->Cpara.paraA;
				double tempB = fractal->Cpara.paraB;
				double tempC = fractal->Cpara.paraC;
				double lengthAB = fractal->Cpara.iterB - fractal->Cpara.iterA;
				double lengthBC = fractal->Cpara.iterC - fractal->Cpara.iterB;
				double grade1 = (tempA - temp0) / fractal->Cpara.iterA;
				double grade2 = (tempB - tempA) / lengthAB;
				double grade3 = (tempC - tempB) / lengthBC;

				// slopes
				if (i < fractal->Cpara.iterA)
				{
					para = temp0 + (i * grade1);
				}
				if (i < fractal->Cpara.iterB && i >= fractal->Cpara.iterA)
				{
					para = tempA + (i - fractal->Cpara.iterA) * grade2;
				}
				if (i >= fractal->Cpara.iterB)
				{
					para = tempB + (i - fractal->Cpara.iterB) * grade3;
				}

				// Curvi part on "true"
				if (fractal->Cpara.enabledCurves)
				{
					double paraAdd = 0.0;
					double paraIt;
					if (lengthAB > 2.0 * fractal->Cpara.iterA) // stop  error, todo fix.
					{
						double curve1 = (grade2 - grade1) / (4.0 * fractal->Cpara.iterA);
						double tempL = lengthAB - fractal->Cpara.iterA;
						double curve2 = (grade3 - grade2) / (4.0 * tempL);
						if (i < 2 * fractal->Cpara.iterA)
						{
							paraIt = tempA - fabs(tempA - i);
							paraAdd = paraIt * paraIt * curve1;
						}
						if (i >= 2 * fractal->Cpara.iterA && i < fractal->Cpara.iterB + tempL)
						{
							paraIt = tempB - fabs(tempB * i);
							paraAdd = paraIt * paraIt * curve2;
						}
						para += paraAdd;
					}
				}
			}
			paraAddP0 = 0.0;
			if (fractal->Cpara.enabledParabFalse)
			{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
				paraAddP0 = fractal->Cpara.parabOffset0 + (i * fractal->Cpara.parabSlope)
										+ (i * i * 0.001 * fractal->Cpara.parabScale);
			}
		}
		para += paraAddP0 + fractal->transformCommon.minR2p25;
		// spherical fold
		double r2 = z.Dot(z);

		z += fractal->mandelbox.offset;

		// transformCommon.maxMinR2factor = transformCommon.maxR2d1 / ;

		// if (r2 < 1e-21) r2 = 1e-21;
		if (r2 < para)
		{
			z *= fractal->transformCommon.maxR2d1 / para;
			aux.DE *= fractal->transformCommon.maxR2d1 / para;
			aux.color += fractal->mandelbox.color.factorSp1;
		}
		else if (r2 < fractal->mandelbox.fR2)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
		z -= fractal->mandelbox.offset;
		z *= fractal->transformCommon.scale08;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale08) + 1.0;
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	// menger
	if (fractal->transformCommon.functionEnabledM && i >= fractal->transformCommon.startIterationsM
			&& i < fractal->transformCommon.stopIterationsM)
	{
		z = fabs(z);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		z *= fractal->transformCommon.scale3;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplier111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplier111.y;
		if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
		aux.DE *= fractal->transformCommon.scale3;
		z += fractal->transformCommon.additionConstant000;
	}
	// iter weight
	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector3 zA = (i == fractal->transformCommon.intA) ? z : CVector3();
		CVector3 zB = (i == fractal->transformCommon.intB) ? z : CVector3();

		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offsetA0)
				+ (zB * fractal->transformCommon.offsetB0);
		aux.DE *= fractal->transformCommon.scale1;
		aux.r_dz *= fractal->transformCommon.scale1;
	}
}

/**
 * Menger Sponge Polynomial Hybrid modified by Mclarekin
 */
void MengerPwr2PolyIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	if (i >= fractal->transformCommon.startIterations && i < fractal->transformCommon.stopIterations1)
	{
		CVector3 partA = z;
		if (fractal->transformCommon.functionEnabledFalse) // fabs
			partA = fabs(z);
		if (fractal->transformCommon.functionEnabledxFalse) // pwr3 or z * fabs(z^2)
			partA *= z;
		partA =
			partA * fractal->transformCommon.scale2 + fractal->transformCommon.constantMultiplier111;

		CVector3 fnZ1 = z;

		if (fractal->transformCommon.functionEnabledBxFalse) // cos(z*Pi)
		{
			double scalePi = M_PI * fractal->transformCommon.scaleC1;
			fnZ1.x = cos(z.x * scalePi);
			fnZ1.y = cos(z.y * scalePi);
			fnZ1.z = cos(z.z * scalePi);
		}

		if (fractal->transformCommon.functionEnabledyFalse) // pi rotation
			fnZ1 = fnZ1.RotateAroundVectorByAngle(
				fractal->transformCommon.constantMultiplier111, M_PI * fractal->transformCommon.scaleB1);

		if (fractal->transformCommon.functionEnabledzFalse) // box offset
		{
			fnZ1.x = (sign(fnZ1.x) * fractal->transformCommon.additionConstant000.x) + fnZ1.x;
			fnZ1.y = (sign(fnZ1.y) * fractal->transformCommon.additionConstant000.y) + fnZ1.y;
			fnZ1.z = (sign(fnZ1.z) * fractal->transformCommon.additionConstant000.z) + fnZ1.z;
		}

		if (fractal->transformCommon.functionEnabledAxFalse) // fabs(fnZ1)
			fnZ1 = fabs(fnZ1);
		CVector3 partB = z;
		if (fractal->transformCommon.functionEnabledAzFalse) partB = fabs(z);
		partB *= fractal->transformCommon.scale4;
		z = fractal->transformCommon.constantMultiplierB111 + partB - partA * fnZ1;
		z *= fractal->transformCommon.scale025;
		aux.DE = aux.DE * 4.0 * fractal->analyticDE.scaleLin + fractal->analyticDE.offsetLin;
	}
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfxyz)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(tempC.x, tempC.y, tempC.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(tempC.x, tempC.z, tempC.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(tempC.y, tempC.x, tempC.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(tempC.y, tempC.z, tempC.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(tempC.z, tempC.x, tempC.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(tempC.z, tempC.y, tempC.x); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfxyz)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(c.x, c.y, c.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(c.x, c.z, c.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(c.y, c.x, c.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(c.y, c.z, c.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(c.z, c.x, c.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(c.z, c.y, c.x); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}

	int count = fractal->transformCommon.int1; // Menger Sponge
	int k;
	for (k = 0; k < count; k++)
	{
		z = fabs(z);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		z *= fractal->transformCommon.scale3;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplierA111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplierA111.y;
		if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplierA111.z;

		aux.DE *= fractal->transformCommon.scale3;

		if (fractal->transformCommon.rotationEnabled && i >= fractal->transformCommon.startIterationsA
				&& i < fractal->transformCommon.stopIterationsA) // rotation
		{
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}
		z += fractal->transformCommon.additionConstantA000;
	}
}

/**
 * Menger Prism Shape
 * from code by Knighty
 * http://www.fractalforums.com/fragmentarium/cross-menger!-can-anyone-do-this/msg93972/#new
 */
void MengerPrismShapeIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;

	if (i >= fractal->transformCommon.startIterationsP
			&& i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector3(SQRT_3_4, 1.5, 1.5);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.benesiT1EnabledFalse && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& i >= fractal->transformCommon.startIterationsD
			&& i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
	}
	if (fractal->transformCommon.functionEnabledFFalse
			&& i >= fractal->transformCommon.startIterationsF
			&& i < fractal->transformCommon.stopIterationsF)
	{
		CVector3 tempA, tempB;

		if (fractal->transformCommon.functionEnabledAx)
			tempA.x = fabs(z.x + fractal->transformCommon.offsetF000.x);

		if (fractal->transformCommon.functionEnabledx)
			tempB.x = fabs(z.x - fractal->transformCommon.offset000.x);

		z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale3D111.x);

		if (fractal->transformCommon.functionEnabledAy)
			tempA.y = fabs(z.y + fractal->transformCommon.offsetF000.y);

		if (fractal->transformCommon.functionEnabledy)
			tempB.y = fabs(z.y - fractal->transformCommon.offset000.y);

		z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale3D111.y);

		if (fractal->transformCommon.functionEnabledAz)
			tempA.z = fabs(z.z + fractal->transformCommon.offsetF000.z);

		if (fractal->transformCommon.functionEnabledz)
			tempB.z = fabs(z.z - fractal->transformCommon.offset000.z);

		z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale3D111.z);

		z += fractal->transformCommon.offsetA000;
	}
	if (fractal->transformCommon.functionEnabled && i >= fractal->transformCommon.startIterationsM
			&& i < fractal->transformCommon.stopIterationsM)
	{
		z = fabs(z);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		z *= fractal->transformCommon.scale3;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplierA111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplierA111.y;
		if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplierA111.z;
		aux.DE *= fractal->transformCommon.scale3 * fractal->transformCommon.scaleA1;

		z += fractal->transformCommon.additionConstantA000;
	}

	aux.DE *= fractal->transformCommon.scaleB1; // not needed but interesting??
}

/**
 * Menger Prism Shape2
 * from code by Knighty
 * http://www.fractalforums.com/fragmentarium/
 * cross-menger!-can-anyone-do-this/msg93972/#new
 */
void MengerPrismShape2Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		z = CVector3{-z.z, z.x, z.y};
	}
	CVector3 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;
	if (fractal->transformCommon.functionEnabledx && i >= fractal->transformCommon.startIterationsP
			&& i < fractal->transformCommon.stopIterationsP1)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			z.x = fabs(z.x);
		}
		if (fractal->transformCommon.functionEnabledCy)
		{
			z.y = fabs(z.y);
		}
		if (fractal->transformCommon.functionEnabledCz)
		{
			z.z = fabs(z.z);
		}
		double tempOff = (fractal->transformCommon.offset0 + SQRT_3_4d2);
		switch (fractal->combo.modeA)
		{
			case sFractalCombo::mode0:
			default:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.x -= t * -SQRT_3 - tempOff;
				z.y = fabs(z.y - t);
				break;
			case sFractalCombo::mode1:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - tempOff;
				z.x = fabs(z.x - t); // x y swap
				break;
			case sFractalCombo::mode2:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.z -= t * -SQRT_3 - tempOff;
				z.y = fabs(z.y - t); // z y swap
				break;
			case sFractalCombo::mode3:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - tempOff;
				z.z = fabs(z.z - t); // z y swap
				break;
			case sFractalCombo::mode4:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - tempOff;
				z.z -= t * -SQRT_3 - tempOff;
				z.x = fabs(z.x - t); // x y swap and other things
				z.y = fabs(z.y - t);
				break;
			case sFractalCombo::mode5:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.x -= t * -SQRT_3 - tempOff;
				z.y -= t * -SQRT_3 - tempOff;
				z.y = fabs(z.y - t); // x y swap and other things
				z.z = fabs(z.z - t);
				break;
			case sFractalCombo::mode6:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - (fractal->transformCommon.offset0);
				z.z -= t * -SQRT_3 - (fractal->transformCommon.offset0);
				z.x = fabs(z.y - t); // x y swap and other things and swizzle
				z.y = fabs(z.x - t);
				break;
			case sFractalCombo::mode7:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.x -= t * -SQRT_3 - (fractal->transformCommon.offset0);
				z.y -= t * -SQRT_3 - (fractal->transformCommon.offset0);
				z.y = fabs(z.z - t); // x y swap and other things and swizzle
				z.z = fabs(z.y - t);
				break;
		}

		if (z.y > z.z) swap(z.y, z.z);
		z.y -= fractal->transformCommon.offsetB0;

		z -= gap * CVector3(SQRT_3_4, 1.5, 1.5);

		if (z.z > z.x) swap(z.z, z.x);

		if (fractal->transformCommon.functionEnabledyFalse)
		{
			z.y = max(0.0, z.y) * fractal->transformCommon.scaleA1;
			z.z = max(0.0, z.z) * fractal->transformCommon.scaleB1;
		}
		if (fractal->transformCommon.functionEnabledzFalse)
		{
			if (z.x >= 0.0)
			{
				z.y = max(0.0, z.y) * fractal->transformCommon.scaleA1;
				z.z = max(0.0, z.z) * fractal->transformCommon.scaleB1;
			}
		}
		z *= fractal->transformCommon.scale1;
		aux.DE *= fractal->transformCommon.scale1;
		aux.DE *= fractal->transformCommon.scaleC1;
	}

	if (fractal->transformCommon.functionEnabledXFalse
			&& i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA)

	{ // CrossMengerTrick
		double dd = aux.DE;
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			z.x = fabs(z.x);
		}
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scaleD1;
		double t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		if (fractal->transformCommon.functionEnabledBzFalse)
		{
			z.y = fabs(z.y) - t;
		}
		else
		{
			z.y = fabs(z.y - t);
		}
		z.x -= SQRT_3_4;
		double dy = 0.0;
		double dz = 0.0;
		if (z.y > 0.5 && z.z > 0.5)
		{
			dy = 1.5;
			dz = 1.5;
		}
		else if (z.z < z.y)
		{
			dy = 1.5;
		}
		else
			dz = 1.5;

		z.y -= dy;
		z.z -= dz;
		z *= fractal->transformCommon.scaleA3;
		aux.DE *= fractal->transformCommon.scaleA3;
		z.y += dy;
		z.z += dz;
		z.x += SQRT_3_4;

		if (fractal->transformCommon.functionEnabledFalse)
		{
			dd *= FRAC_1_3 * fractal->transformCommon.scaleE1;
			z *= dd;
			aux.DE *= dd;
		}
		aux.DE *= fractal->transformCommon.offset1; //.DE tweak cross menger trick part
	}
	if (fractal->transformCommon.functionEnabledPFalse)
	{
		z.x = fabs(z.x + fractal->transformCommon.offset) + fractal->transformCommon.offsetC0;
	}

	// void KIFS(vec3 z)
	//{//Pure KIFS... almost correct

	if (fractal->transformCommon.functionEnabledKFalse
			&& i >= fractal->transformCommon.startIterationsB
			&& i < fractal->transformCommon.stopIterationsB)
	{
		if (fractal->transformCommon.functionEnabledCzFalse)
		{
			z.x = fabs(fractal->transformCommon.offset - z.x) + fractal->transformCommon.offsetC0;
		}

		z.y = fabs(z.y);
		z.z = fabs(z.z);

		double dot2 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scaleF1;
		double t = max(0.0, dot2);

		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);

		z.y = fabs(z.y - 0.5) + 0.5;

		z -= CVector3(0.5 * SQRT_3, 1.5, 1.5);

		z *= fractal->transformCommon.scaleB3;
		aux.DE *= fractal->transformCommon.scaleB3;

		z += CVector3(0.5 * SQRT_3, 1.5, 1.5);
	}

	if (fractal->transformCommon.benesiT1EnabledFalse && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 tempZ = z;
		double tempL = tempZ.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.benesiT1MEnabledFalse
			&& i >= fractal->transformCommon.startIterationsD
			&& i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
	}

	if (fractal->transformCommon.functionEnabledFFalse
			&& i >= fractal->transformCommon.startIterationsF
			&& i < fractal->transformCommon.stopIterationsF)
	{
		CVector3 tempA, tempB;

		if (fractal->transformCommon.functionEnabledAx)
			tempA.x = fabs(z.x + fractal->transformCommon.offsetF000.x);

		if (fractal->transformCommon.functionEnabledx)
			tempB.x = fabs(z.x - fractal->transformCommon.offset000.x);

		z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale3D111.x);

		if (fractal->transformCommon.functionEnabledAy)
			tempA.y = fabs(z.y + fractal->transformCommon.offsetF000.y);

		if (fractal->transformCommon.functionEnabledy)
			tempB.y = fabs(z.y - fractal->transformCommon.offset000.y);

		z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale3D111.y);

		if (fractal->transformCommon.functionEnabledAz)
			tempA.z = fabs(z.z + fractal->transformCommon.offsetF000.z);

		if (fractal->transformCommon.functionEnabledz)
			tempB.z = fabs(z.z - fractal->transformCommon.offset000.z);

		z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale3D111.z);

		z += fractal->transformCommon.offsetA000;
	}

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	// menger
	if (fractal->transformCommon.functionEnabledM && i >= fractal->transformCommon.startIterationsM
			&& i < fractal->transformCommon.stopIterationsM)
	{
		z = fabs(z);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		z *= fractal->transformCommon.scale3;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplierA111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplierA111.y;
		if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplierA111.z;
		aux.DE *= fractal->transformCommon.scale3;

		z += fractal->transformCommon.additionConstantA000;
	}

	aux.DE *= fractal->analyticDE.scale1;
}

/**
 * Menger Smooth
 * // http://www.fractalforums.com/fragmentarium/help-t22583/
 */
void MengerSmoothIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double sc1 = fractal->transformCommon.scale3 - 1.0;		// 3 - 1 = 2, 2/3 = 0.6667;
	double sc2 = sc1 / fractal->transformCommon.scale3;		//  8 - 1 = 7, 7/8 = 0.89ish;
	double OffsetS = fractal->transformCommon.offset0005; //

	if (fractal->transformCommon.functionEnabled)
	{
		// the closer to origin the greater the effect of OffsetSQ
		z = CVector3{sqrt(z.x * z.x + OffsetS), sqrt(z.y * z.y + OffsetS), sqrt(z.z * z.z + OffsetS)};
	}

	double t;
	CVector3 OffsetC = fractal->transformCommon.offset1105;

	t = z.x - z.y;
	t = 0.5 * (t - sqrt(t * t + OffsetS));
	z.x = z.x - t;
	z.y = z.y + t;

	t = z.x - z.z;
	t = 0.5 * (t - sqrt(t * t + OffsetS));
	z.x = z.x - t;
	z.z = z.z + t;

	t = z.y - z.z;
	t = 0.5 * (t - sqrt(t * t + OffsetS));
	z.y = z.y - t;
	z.z = z.z + t;

	z.z = z.z - OffsetC.z * sc2; // sc2 reduces C.z
	z.z = -sqrt(z.z * z.z + OffsetS);
	z.z = z.z + OffsetC.z * sc2;

	z.x = fractal->transformCommon.scale3 * z.x - OffsetC.x * sc1; // sc1 scales up C.x
	z.y = fractal->transformCommon.scale3 * z.y - OffsetC.y * sc1;
	z.z = fractal->transformCommon.scale3 * z.z;

	aux.DE *= fractal->transformCommon.scale3;

	if (fractal->transformCommon.rotationEnabled && i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->transformCommon.functionEnabledzFalse)
	{
		CVector3 zA = (i == fractal->transformCommon.intA) ? z : CVector3();
		CVector3 zB = (i == fractal->transformCommon.intB) ? z : CVector3();
		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offsetA0)
				+ (zB * fractal->transformCommon.offsetB0);
		aux.DE *= fractal->transformCommon.scale1;
		aux.r_dz *= fractal->transformCommon.scale1;
	}
}

/**
 * Menger Smooth Mod1, based on :
 * // http://www.fractalforums.com/fragmentarium/help-t22583/
 */
void MengerSmoothMod1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{

	if (fractal->transformCommon.functionEnabled)
	{
		z = CVector3{sqrt(z.x * z.x + fractal->transformCommon.offset0),
			sqrt(z.y * z.y + fractal->transformCommon.offset0),
			sqrt(z.z * z.z + fractal->transformCommon.offset0)};
	}
	if (fractal->transformCommon.functionEnabledFFalse)
	{
		z = fabs(z);
		double s = fractal->transformCommon.offset;
		z += CVector3(s, s, s);
	}

	double t;
	double ScaleP5 = fractal->transformCommon.scale05;
	CVector3 OffsetC = fractal->transformCommon.constantMultiplier221;
	double OffsetS = fractal->transformCommon.offset0005;

	t = z.x - z.y;
	t = ScaleP5 * (t - sqrt(t * t + OffsetS * fractal->transformCommon.constantMultiplier111.x));
	z.x = z.x - t;
	z.y = z.y + t;

	t = z.x - z.z;
	t = ScaleP5 * (t - sqrt(t * t + OffsetS * fractal->transformCommon.constantMultiplier111.y));
	z.x = z.x - t;
	z.z = z.z + t;

	t = z.y - z.z;
	t = ScaleP5 * (t - sqrt(t * t + OffsetS * fractal->transformCommon.constantMultiplier111.z));
	z.y = z.y - t;
	z.z = z.z + t;

	z.z = z.z - OffsetC.z / 3.0;
	z.z = -sqrt(z.z * z.z + OffsetS);
	z.z = z.z + OffsetC.z / 3.0;

	z.x = fractal->transformCommon.scale3 * z.x - OffsetC.x;
	z.y = fractal->transformCommon.scale3 * z.y - OffsetC.y;
	z.z = fractal->transformCommon.scale3 * z.z;

	aux.DE *= fractal->transformCommon.scale3;

	if (fractal->transformCommon.rotationEnabled && i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA) // box offset
	{
		z.x = sign(z.x) * fractal->transformCommon.additionConstantA000.x + z.x;
		z.y = sign(z.y) * fractal->transformCommon.additionConstantA000.y + z.y;
		z.z = sign(z.z) * fractal->transformCommon.additionConstantA000.z + z.z;
	}

	if (fractal->transformCommon.functionEnabledzFalse)
	{
		CVector3 zA = (i == fractal->transformCommon.intA) ? z : CVector3();
		CVector3 zB = (i == fractal->transformCommon.intB) ? z : CVector3();

		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offsetA0)
				+ (zB * fractal->transformCommon.offsetB0);
		aux.DE *= fractal->transformCommon.scale1;
		aux.r_dz *= fractal->transformCommon.scale1;
	}
}

/**
 * Msltoe Donut formula
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * low-hanging-dessert-an-escape-time-donut-fractal/msg90171/#msg90171
 */
void MsltoeDonutIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double rad2 = fractal->donut.ringThickness;
	double nSect = 2.0 * M_PI / fractal->donut.number;
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
void MsltoeSym2ModIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	CVector3 temp = z;

	if (fabs(z.y) < fabs(z.z)) // then swap
	{
		z.y = temp.z; // making z.y furthest away from axis
		z.z = temp.y;
	}
	if (z.y > z.z) // then change sign of z.x and z.z
	{
		z.x = -z.x;
	}

	CVector3 z2 = z * z;							// squares
	double v3 = (z2.x + z2.y + z2.z); // sum of squares
	// if (v3 < 1e-21 && v3 > -1e-21)
	//	v3 = (v3 > 0) ? 1e-21 : -1e-21;
	double zr = 1.0 - z2.z / v3;
	temp.x = (z2.x - z2.y) * zr;
	temp.y = 2.0 * z.x * z.y * zr * fractal->transformCommon.scale; // scaling temp.y
	temp.z = 2.0 * z.z * sqrt(z2.x + z2.y);
	z = temp + fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;

		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}

	double lengthTempZ = -z.Length();
	// if (lengthTempZ > -1e-21)
	//	lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale1);
}

/**
 * MsltoeSym3Mod based on the formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/15/
 */
void MsltoeSym3ModIteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	CVector3 temp = z;
	if (fabs(z.y) < fabs(z.z)) // then swap
	{
		z.y = temp.z; // making z.y furthest away from axis
		z.z = temp.y;
	}
	if (z.y > z.z) // then change sign of z.x and z.z
	{
		z.x = -z.x;
		z.z = -z.z;
	}
	CVector3 z2 = z * z; // squares
	double v3 = (z2.x + z2.y + z2.z)
							+ fractal->transformCommon.scale0 * fractal->transformCommon.scale0 * z2.y * z2.z;
	; // sum of squares
	// if (v3 < 1e-21 && v3 > -1e-21) v3 = (v3 > 0) ? 1e-21 : -1e-21;
	double zr = 1.0 - z2.z / v3;
	temp.x = (z2.x - z2.y) * zr;
	temp.y = 2.0 * z.x * z.y * zr * fractal->transformCommon.scale; // scaling temp.y
	temp.z = 2.0 * z.z * sqrt(z2.x + z2.y);

	z = temp + fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}
	double lengthTempZ = -z.Length();
	// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
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
			CVector3 temp2 = z;
			double tempL = temp2.Length();
			z *= CVector3(1.0, 2.0, 2.0);
			// if (tempL < 1e-21) tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			// aux.r_dz *= avgScale * fractal->transformCommon.scaleA1;
			double tempAux = aux.r_dz * avgScale;
			aux.r_dz = aux.r_dz + (tempAux - aux.r_dz) * fractal->transformCommon.scaleA1;
		}
		else
		{
			z *= CVector3(1.0, 2.0, 2.0);
		}
	}
}

/**
 * MsltoeJuliaBulb Eiffie. Refer post by Eiffie    Reply #69 on: January 27, 2015
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/60/
 */
void EiffieMsltoeIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	double psi = fabs(fmod(atan2(z.z, z.y) + M_PI + M_PI_8, M_PI_4) - M_PI_8);
	double lengthYZ = sqrt(z.y * z.y + z.z * z.z);

	z.y = cos(psi) * lengthYZ;
	z.z = sin(psi) * lengthYZ;
	aux.r_dz = aux.r_dz * 2.0 * aux.r;

	CVector3 z2 = z * z;
	double rr = z2.x + z2.y + z2.z;
	double m = 1.0 - z2.z / rr;
	CVector3 newz;
	newz.x = (z2.x - z2.y) * m;
	newz.y = 2.0 * z.x * z.y * m * fractal->transformCommon.scale; // scaling y;;
	newz.z = 2.0 * z.z * sqrt(z2.x + z2.y);
	z = newz + fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}
	double lengthTempZ = -z.Length();
	// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	// aux.r_dz *= fabs(fractal->transformCommon.scale1);

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		aux.r_dz *= fabs(fractal->transformCommon.scale1) * fractal->analyticDE.scale1;
	}
	else
	{
		aux.r_dz *= fabs(fractal->transformCommon.scale1);
	}
}

/**
 * Msltoe_Julia_Bulb_Mod2
 * @reference
 * http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/msg14198/#msg14198
 */
void MsltoeSym3Mod2Iteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;

	double theta;
	double phi;
	CVector3 z2 = z * z;
	double r = z2.x + z2.y + z2.z;
	// if (r < 1e-21)
	//	r = 1e-21;
	double r1 = sqrt(r + fractal->transformCommon.scale0 * z2.y * z2.z);
	// if (r1 < 1e-21)
	//	r1 = 1e-21;
	if (z2.z < z2.y)
	{
		theta = 2.0 * atan2(z.y, z.x);
		phi = 2.0 * asin(z.z / r1);
		z.x = r * cos(theta) * cos(phi);
		z.y = r * sin(theta) * cos(phi);
		z.z = -r * sin(phi);
	}
	else
	{
		theta = 2.0 * atan2(z.z, z.x);
		phi = 2.0 * asin(z.y / r1);
		z.x = r * cos(theta) * cos(phi);
		z.y = -r * sin(phi);
		z.z = r * sin(theta) * cos(phi);
	}
	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}
	double lengthTempZ = -z.Length();
	// if (lengthTempZ > -1e-21)
	//	lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale1);
}

/**
 * Msltoe_Julia_Bulb_Mod3
 * @reference
 * http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/msg14320/#msg14320
 */
void MsltoeSym3Mod3Iteration(
	CVector3 &z, CVector3 c, int i, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	CVector3 z1 = z;
	double psi = atan2(z.z, z.y) + M_PI * 2.0;
	double psi2 = 0;
	while (psi > M_PI_8)
	{
		psi -= M_PI_4;
		psi2 -= M_PI_4; // M_PI_4 = pi/4
	}
	double cs = cos(psi2);
	double sn = sin(psi2);
	z1.y = z.y * cs - z.z * sn;
	z1.z = z.y * sn + z.z * cs;
	z.y = z1.y;
	z.z = z1.z;
	CVector3 zs = z * z;
	double zs2 = zs.x + zs.y;
	// if (zs2 < 1e-21)
	//	zs2 = 1e-21;
	double zs3 =
		(zs2 + zs.z) + fractal->transformCommon.scale0 * fractal->transformCommon.scale0 * zs.y * zs.z;
	double zsd = (1.0 - zs.z / zs3);

	z1.x = (zs.x - zs.y) * zsd;
	z1.y = (2.0 * z.x * z.y) * zsd * fractal->transformCommon.scale; // scaling y;
	z1.z = 2.0 * z.z * sqrt(zs2);
	z.x = z1.x;
	z.y = z1.y * cs + z1.z * sn;
	z.z = -z1.y * sn + z1.z * cs;
	z += fractal->transformCommon.additionConstant000;
	if (fractal->transformCommon.addCpixelEnabledFalse) // symmetrical addCpixel
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}
	double lengthTempZ = -z.Length(); // spherical offset
	// if (lengthTempZ > -1e-21)
	//	lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
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
			z *= CVector3(1.0, 2.0, 2.0);
			// if (tempL < 1e-21)
			//	tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			aux.r_dz *= avgScale;
		}
		else
		{
			z *= CVector3(1.0, 2.0, 2.0);
		}
	}
}

/**
 * MsltoeSym4Mod  Based on the formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/15/
 */
void MsltoeSym4ModIteration(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	CVector3 temp = z;
	double tempL = temp.Length();
	// if (tempL < 1e-21)
	//	tempL = 1e-21;
	z *= fractal->transformCommon.scale3D111;

	aux.r_dz *= fabs(z.Length() / tempL);

	if (fabs(z.x) < fabs(z.z)) swap(z.x, z.z);
	if (fabs(z.x) < fabs(z.y)) swap(z.x, z.y);
	if (fabs(z.y) < fabs(z.z)) swap(z.y, z.z);

	if (z.x * z.z < 0.0) z.z = -z.z;
	if (z.x * z.y < 0.0) z.y = -z.y;

	temp.x = z.x * z.x - z.y * z.y - z.z * z.z;
	temp.y = 2.0 * z.x * z.y;
	temp.z = 2.0 * z.x * z.z;

	z = temp + fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}
	double lengthTempZ = -z.Length();
	// if (lengthTempZ > -1e-21)
	//	lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
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
	if (fractal->transformCommon.functionEnabledFalse) // pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.r_dz *= z.Length() / aux.r;
		aux.DE = aux.DE * z.Length() / aux.r + 1.0;
	}
	// Torioidal bulb
	double r1 = fractal->transformCommon.minR05; // default 0.5
	double theta = atan2(z.y, z.x);
	double x1 = r1 * cos(theta);
	double y1 = r1 * sin(theta);
	aux.r = (z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1) + z.z * z.z; //+ 1e-061
	double phi = asin(z.z / sqrt(aux.r));
	double rp = pow(aux.r, fractal->transformCommon.pwr4); // default 4.0

	phi *= fractal->transformCommon.pwr8; // default 8
	theta *= fractal->bulb.power;					// default 9 gives 8 symmetry
	// convert back to cartesian coordinates
	z.x = (r1 + rp * cos(phi)) * cos(theta);
	z.y = (r1 + rp * cos(phi)) * sin(theta);
	z.z = -rp * sin(phi);

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		if (fractal->analyticDE.enabledAuxR2False) aux.r_dz *= aux.r_dz;
		aux.r_dz = pow(aux.r, fractal->transformCommon.pwr4 - fractal->analyticDE.offset1)
								 * fractal->transformCommon.pwr4 * fractal->analyticDE.scale1 * aux.r_dz
							 + fractal->analyticDE.offset2;
	}
	else
	{ // default, i.e. scale1 & offset1 & offset2 = 1.0
		aux.r_dz = pow(aux.r, fractal->transformCommon.pwr4 - 1.0) * aux.r_dz * aux.r_dz
								 * fractal->transformCommon.pwr4
							 + 1.0;
	}

	if (fractal->transformCommon.functionEnabledAxFalse) // spherical offset
	{
		double lengthTempZ = -z.Length();
		// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
		aux.r_dz *= fabs(fractal->transformCommon.scale);
	}
	// then add Cpixel constant vector
}

/**
 * MsltoeToroidalMulti
 * @reference http://www.fractalforums.com/theory/toroidal-coordinates/msg9428/
 */
void MsltoeToroidalMultiIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse) // pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.r_dz *= z.Length() / aux.r;
		aux.DE = aux.DE * z.Length() / aux.r + 1.0;
	}
	// Torioidal bulb multi
	double th0 = fractal->bulb.betaAngleOffset;
	double ph0 = fractal->bulb.alphaAngleOffset;
	double v1, v2, v3;
	switch (fractal->sinTan2Trig.orderOfzyx)
	{
		case sFractalSinTan2Trig::zyx:
		default:
			v1 = z.z;
			v2 = z.y;
			v3 = z.x;
			break;
		case sFractalSinTan2Trig::zxy:
			v1 = z.z;
			v2 = z.x;
			v3 = z.y;
			break;
		case sFractalSinTan2Trig::yzx:
			v1 = z.y;
			v2 = z.z;
			v3 = z.x;
			break;
		case sFractalSinTan2Trig::yxz:
			v1 = z.y;
			v2 = z.x;
			v3 = z.z;
			break;
		case sFractalSinTan2Trig::xzy:
			v1 = z.x;
			v2 = z.z;
			v3 = z.y;
			break;
		case sFractalSinTan2Trig::xyz:
			v1 = z.x;
			v2 = z.y;
			v3 = z.z;
			break;
	}

	if (fractal->sinTan2Trig.atan2Oratan == sFractalSinTan2Trig::atan2)
	{
		ph0 += atan2(v2, v3);
	}
	else
	{
		ph0 += atan(v2 / v3);
	}

	double r1 = fractal->transformCommon.minR05;
	double x1 = r1 * cos(ph0);
	double y1 = r1 * sin(ph0);

	aux.r = (z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1) + z.z * z.z; //+ 1e-061

	double sqrT = aux.r;
	if (fractal->transformCommon.functionEnabledAy) // sqrts
	{
		sqrT = sqrt(aux.r);
	}
	if (fractal->sinTan2Trig.asinOracos == sFractalSinTan2Trig::asin)
	{
		th0 += asin(v1 / sqrT);
	}
	else
	{
		th0 += acos(v1 / sqrT);
	}
	th0 *= fractal->transformCommon.pwr8; // default 8
	ph0 *= fractal->bulb.power;						// default 9 gives 8 symmetry

	double rp = pow(aux.r, fractal->transformCommon.pwr4);

	double costh = cos(th0);
	double sinth = sin(th0);

	if (fractal->transformCommon.functionEnabledzFalse)
	{ // cosine mode
		z.x = (r1 + rp * sinth) * sin(ph0);
		z.y = (r1 + rp * sinth) * cos(ph0);
		z.z = -rp * costh;
	}
	else
	{ // sine mode default
		z.x = (r1 + rp * costh) * cos(ph0);
		z.y = (r1 + rp * costh) * sin(ph0);
		z.z = -rp * sinth;
	}

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		if (fractal->analyticDE.enabledAuxR2False) aux.r_dz *= aux.r_dz;
		aux.r_dz = pow(aux.r, fractal->transformCommon.pwr4 - fractal->analyticDE.offset1)
								 * fractal->transformCommon.pwr4 * fractal->analyticDE.scale1 * aux.r_dz
							 + fractal->analyticDE.offset2;
	}
	else
	{ // default, i.e. scale1 & offset1 & offset2 = 1.0
		aux.r_dz = pow(aux.r, fractal->transformCommon.pwr4 - 1.0) * aux.r_dz * aux.r_dz
								 * fractal->transformCommon.pwr4
							 + 1.0;
	}

	if (fractal->transformCommon.functionEnabledAxFalse) // spherical offset
	{
		double lengthTempZ = -z.Length();
		// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
		aux.r_dz *= fabs(fractal->transformCommon.scale);
	}
	// then add Cpixel constant vector
}
/**
 * Pseudo Kleinian Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */
void PseudoKleinian2Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;

	if (fractal->transformCommon.functionEnabledPFalse
			&& i >= fractal->transformCommon.startIterationsP
			&& i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector3(SQRT_3_4, 1.5, 1.5);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	if (fractal->transformCommon.functionEnabledBxFalse
			&& i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA)
	{
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
		double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}

	CVector3 Csize = fractal->transformCommon.additionConstant0777;
	CVector3 tempZ = z; //  correct c++ version.
	if (z.x > Csize.x) tempZ.x = Csize.x;
	if (z.x < -Csize.x) tempZ.x = -Csize.x;
	if (z.y > Csize.y) tempZ.y = Csize.y;
	if (z.y < -Csize.y) tempZ.y = -Csize.y;
	if (z.z > Csize.z) tempZ.z = Csize.z;
	if (z.z < -Csize.z) tempZ.z = -Csize.z;

	z = tempZ * 2.0 - z;
	double k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
	z *= k;
	aux.DE *= k + fractal->analyticDE.tweak005;

	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	z += fractal->transformCommon.additionConstant000;
	// no bailout

	aux.pseudoKleinianDE = fractal->analyticDE.scale1;
}

/**
 * Pseudo Kleinian Mod1, Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */
void PseudoKleinian3Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 gap = fractal->transformCommon.constantMultiplier000;

	if (fractal->transformCommon.functionEnabledPFalse
			&& i >= fractal->transformCommon.startIterationsP
			&& i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		double dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		double t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector3(SQRT_3_4, 1.5, 1.5);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.benesiT1EnabledFalse && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 tempZ = z;
		double tempL = tempZ.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& i >= fractal->transformCommon.startIterationsD
			&& i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
	}
	if (fractal->transformCommon.functionEnabledFFalse
			&& i >= fractal->transformCommon.startIterationsF
			&& i < fractal->transformCommon.stopIterationsF)
	{
		CVector3 tempA, tempB;

		if (fractal->transformCommon.functionEnabledAx)
			tempA.x = fabs(z.x + fractal->transformCommon.offsetF000.x);

		if (fractal->transformCommon.functionEnabledx)
			tempB.x = fabs(z.x - fractal->transformCommon.offset000.x);

		z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale3D111.x);

		if (fractal->transformCommon.functionEnabledAy)
			tempA.y = fabs(z.y + fractal->transformCommon.offsetF000.y);

		if (fractal->transformCommon.functionEnabledy)
			tempB.y = fabs(z.y - fractal->transformCommon.offset000.y);

		z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale3D111.y);

		if (fractal->transformCommon.functionEnabledAz)
			tempA.z = fabs(z.z + fractal->transformCommon.offsetF000.z);

		if (fractal->transformCommon.functionEnabledz)
			tempB.z = fabs(z.z - fractal->transformCommon.offset000.z);

		z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale3D111.z);

		z += fractal->transformCommon.offsetA000;
	}

	aux.DE *= fractal->transformCommon.scaleB1; // not needed but interesting??
	double k;
	// Pseudo kleinian
	CVector3 Csize = fractal->transformCommon.additionConstant0777;
	if (fractal->transformCommon.functionEnabledAy && i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC)
	{
		CVector3 tempZ = z; //  correct c++ version.

		if (z.x > Csize.x) tempZ.x = Csize.x;
		if (z.x < -Csize.x) tempZ.x = -Csize.x;
		if (z.y > Csize.y) tempZ.y = Csize.y;
		if (z.y < -Csize.y) tempZ.y = -Csize.y;
		if (z.z > Csize.z) tempZ.z = Csize.z;
		if (z.z < -Csize.z) tempZ.z = -Csize.z;

		z.z *= fractal->transformCommon.scaleB1;

		z = tempZ * 2.0 - z;
		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	if (fractal->transformCommon.functionEnabledAyFalse
			&& i >= fractal->transformCommon.startIterationsB
			&& i < fractal->transformCommon.stopIterationsB)
	{
		if (z.x > Csize.x) //  variation from openCL
			z.x = Csize.x * 2.0 - z.x;
		if (z.x < -Csize.x) z.x = -Csize.x * 2.0 - z.x;
		if (z.y > Csize.y) z.y = Csize.y * 2.0 - z.y;
		if (z.y < -Csize.y) z.y = -Csize.y * 2.0 - z.y;
		if (z.z > Csize.z) z.z = Csize.z * 2.0 - z.z;
		if (z.z < -Csize.z) z.z = -Csize.z * 2.0 - z.z;

		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledMFalse
			&& i >= fractal->transformCommon.startIterationsM
			&& i < fractal->transformCommon.stopIterationsM)
	{
		z = fabs(z);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		z *= fractal->transformCommon.scale3;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplierA111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplierA111.y;
		if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplierA111.z;
		aux.DE *= fractal->transformCommon.scale3 * fractal->transformCommon.scaleA1;

		z += fractal->transformCommon.additionConstantA000;
	}
	aux.pseudoKleinianDE = fractal->analyticDE.scale1;
}
/**
 * Pseudo Kleinian std DE Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */
void PseudoKleinian1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;

	if (fractal->transformCommon.functionEnabledPFalse
			&& i >= fractal->transformCommon.startIterationsP
			&& i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector3(SQRT_3_4, 1.5, 1.5);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.benesiT1EnabledFalse && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 tempZ = z;
		double tempL = tempZ.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& i >= fractal->transformCommon.startIterationsD
			&& i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector3(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

		CVector3 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector3(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
	}
	if (fractal->transformCommon.functionEnabledFFalse
			&& i >= fractal->transformCommon.startIterationsF
			&& i < fractal->transformCommon.stopIterationsF)
	{
		CVector3 tempA, tempB;

		if (fractal->transformCommon.functionEnabledAx)
			tempA.x = fabs(z.x + fractal->transformCommon.offsetF000.x);

		if (fractal->transformCommon.functionEnabledx)
			tempB.x = fabs(z.x - fractal->transformCommon.offset000.x);

		z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale3D111.x);

		if (fractal->transformCommon.functionEnabledAy)
			tempA.y = fabs(z.y + fractal->transformCommon.offsetF000.y);

		if (fractal->transformCommon.functionEnabledy)
			tempB.y = fabs(z.y - fractal->transformCommon.offset000.y);

		z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale3D111.y);

		if (fractal->transformCommon.functionEnabledAz)
			tempA.z = fabs(z.z + fractal->transformCommon.offsetF000.z);

		if (fractal->transformCommon.functionEnabledz)
			tempB.z = fabs(z.z - fractal->transformCommon.offset000.z);

		z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale3D111.z);

		z += fractal->transformCommon.offsetA000;
	}
	if (fractal->transformCommon.functionEnabled && i >= fractal->transformCommon.startIterationsM
			&& i < fractal->transformCommon.stopIterationsM)
	{
		z = fabs(z);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		z *= fractal->transformCommon.scale3;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplierA111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplierA111.y;
		if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplierA111.z;
		aux.DE *= fractal->transformCommon.scale3 * fractal->transformCommon.scaleA1;

		z += fractal->transformCommon.additionConstantA000;
	}

	aux.DE *= fractal->transformCommon.scaleB1; // not needed but interesting??
	double k;
	// Pseudo kleinian
	CVector3 Csize = fractal->transformCommon.additionConstant0777;
	if (fractal->transformCommon.functionEnabledAy && i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC)
	{
		CVector3 tempZ = z; //  correct c++ version.

		if (z.x > Csize.x) tempZ.x = Csize.x;
		if (z.x < -Csize.x) tempZ.x = -Csize.x;
		if (z.y > Csize.y) tempZ.y = Csize.y;
		if (z.y < -Csize.y) tempZ.y = -Csize.y;
		if (z.z > Csize.z) tempZ.z = Csize.z;
		if (z.z < -Csize.z) tempZ.z = -Csize.z;

		z *= fractal->transformCommon.scaleB1;
		z = tempZ * 2.0 - z;

		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	if (fractal->transformCommon.functionEnabledAyFalse
			&& i >= fractal->transformCommon.startIterationsB
			&& i < fractal->transformCommon.stopIterationsB)
	{
		if (z.x > Csize.x) //  variation from openCL
			z.x = Csize.x * 2.0 - z.x;
		if (z.x < -Csize.x) z.x = -Csize.x * 2.0 - z.x;
		if (z.y > Csize.y) z.y = Csize.y * 2.0 - z.y;
		if (z.y < -Csize.y) z.y = -Csize.y * 2.0 - z.y;
		if (z.z > Csize.z) z.z = Csize.z * 2.0 - z.z;
		if (z.z < -Csize.z) z.z = -Csize.z * 2.0 - z.z;
		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	z += fractal->transformCommon.additionConstant000;
	// no bailout
}

/**
 * Quaternion3DE - Quaternion fractal with extended controls
 * @reference http://www.fractalforums.com/3d-fractal-generation
 * /true-3d-mandlebrot-type-fractal/
 */
void Quaternion3DIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	z = CVector3(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z);

	double tempL = z.Length();
	z *= fractal->transformCommon.constantMultiplier122;
	// if (tempL < 1e-21) tempL = 1e-21;
	double avgScale = CVector3(z.x, z.y / 2.0, z.z / 2.0).Length() / tempL;
	double tempAux = aux.r_dz * avgScale;
	aux.r_dz = aux.r_dz + (tempAux - aux.r_dz) * fractal->transformCommon.scaleA1;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	z += fractal->transformCommon.additionConstant000;
}

/**
 * RiemannSphereMsltoe
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/
 * riemann-fractals/msg33500/#msg33500
 */
void RiemannSphereMsltoeIteration(CVector3 &z, const cFractal *fractal)
{
	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	double r = z.Length();
	// if (r < 1e-21) r = 1e-21;
	z *= fractal->transformCommon.scale / r;

	double w = 1.0 - z.z;
	// if (w > -1e-21 && w < 1e-21) w = (w > 0) ? 1e-21 : -1e-21;
	w = 1.0 / w;

	double s = z.x * w;
	double t = z.y * w;

	w = 1.0 + s * s + t * t;

	s = fabs(sin(M_PI * s));
	t = fabs(sin(M_PI * t));

	r *= r;
	// if (r < 1e-21)
	//	r = 1e-21;

	if (w > 36) w = 36; // problem with pow()
	r = -0.25 + pow(r, w);

	w = r / (1.0 + s * s + t * t);
	z.x = 2.0 * s;
	z.y = 2.0 * t;
	z.z = -1.0 + s * s + t * t;
	z *= w;

	z += fractal->transformCommon.additionConstant000;
}

/**
 * RiemannSphereMsltoe Variation1
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * revisiting-the-riemann-sphere-%28again%29/
 */
void RiemannSphereMsltoeV1Iteration(CVector3 &z, const cFractal *fractal)
{
	double r = z.Length();
	// if (r < 1e-21) r = 1e-21;
	z *= fractal->transformCommon.scale / r;
	double w = 1.0 - z.z;
	// if (w > -1e-21 && w < 1e-21)
	//	w = (w > 0) ? 1e-21 : -1e-21;
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
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * another-way-to-make-my-riemann-sphere-'bulb'-using-a-conformal-transformation/
 */
void RiemannBulbMsltoeMod2Iteration(CVector3 &z, const cFractal *fractal)
{
	double rad2 = fractal->transformCommon.minR05;
	double r2 = z.x * z.x + z.y * z.y + z.z * z.z; // r2 or point radius squared
	if (r2 < rad2 * rad2)
	{
		if (fractal->transformCommon.functionEnabled)
			// smooth inside
			z *= rad2 * ((r2 * 0.1) + 0.4) * 1.18 * fractal->transformCommon.scaleA1 / r2;
		else
		{
			z *= fractal->transformCommon.constantMultiplier111;
		}
	} // if internal smooth function disabled, then z = z * scale, default vect3(1,1,1)
	else
	{
		// 1st scale variable, default vect3 (1.7, 1.7, 1.7),
		z *= fractal->transformCommon.constantMultiplier222;
		double shift = fractal->transformCommon.offset1;
		// r1 = length^2,  //  + 1e-061
		double r1 = z.x * z.x + (z.y - shift) * (z.y - shift) + z.z * z.z;
		// inversions by length^2
		z.x = z.x / r1;
		z.y = (z.y - shift) / r1;
		z.z = z.z / r1;
		// 2nd scale variable , default = double (3.0)
		z *= fractal->transformCommon.scale3;
		// y offset variable, default = double (1.9);
		z.y = z.y + fractal->transformCommon.offset105;
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
 * Sierpinski3D. made from Darkbeams Sierpinski code from M3D

 */
void Sierpinski3DIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{

	CVector3 temp = z;

	// Normal full tetra-fold;
	if (fractal->transformCommon.functionEnabled)
	{
		if (z.x - z.y < 0.0) swap(z.x, z.y);
		if (z.x - z.z < 0.0) swap(z.x, z.z);
		if (z.y - z.z < 0.0) swap(z.y, z.z);

		if (z.x + z.y < 0.0)
		{
			temp.x = -z.y;
			z.y = -z.x;
			z.x = temp.x;
		}
		if (z.x + z.z < 0.0)
		{
			temp.x = -z.z;
			z.z = -z.x;
			z.x = temp.x;
		}
		if (z.y + z.z < 0.0)
		{
			temp.y = -z.z;
			z.z = -z.y;
			z.y = temp.y;
		}
	}

	// Reversed full tetra-fold;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (z.x + z.y < 0.0)
		{
			temp.x = -z.y;
			z.y = -z.x;
			z.x = temp.x;
		}
		if (z.x + z.z < 0.0)
		{
			temp.x = -z.z;
			z.z = -z.x;
			z.x = temp.x;
		}
		if (z.y + z.z < 0.0)
		{
			temp.y = -z.z;
			z.z = -z.y;
			z.y = temp.y;
		}
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
	}

	z = z * fractal->transformCommon.scaleA2;
	aux.DE *= fractal->transformCommon.scaleA2;

	if (i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC)
	{
		z -= fractal->transformCommon.offset111; // neg offset
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	aux.DE *= fractal->analyticDE.scale1;
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
		if ((c > 0.0) && ((a * c) > b))
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
				if ((c > 0.0) && ((a * c) > b))
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
			if (a_rmin < 0.0)
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
					if ((c > 0.0) && ((a * c) > b))
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
					z = z + z2X * (2.0 * (z2X_mag - melt) / (z2X_mag + .00000001));
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
					z = z + z2X * (2.0 * (z2X_mag - melt) / (z2X_mag + .00000001));
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
				z = z + z2X * (2.0 * (z2X_mag - melt) / (z2X_mag + .00000001));
				aux.color += fractal->mandelbox.color.factor.x;
			}
		}
	} // outside solid

	double r2 = z.Dot(z);

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
		aux.r_dz *= sqrt(fold_factor1_2);
		aux.color *= 0.9;
	}
	else if (r2_2 < fR2_2)
	{
		double fold_factor2_2 = fR2_2 / r2_2;
		z = z * fold_factor2_2;
		aux.DE *= fold_factor2_2;
		aux.r_dz *= sqrt(fold_factor1_2);
		aux.color *= 0.9;
	}
}

/**
 * quadratic iteration in imaginary scator algebra
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * ix-possibly-the-holy-grail-fractal-%28in-fff-lore%29
 *            http://luz.izt.uam.mx/index.html/?q=node/95&language=en
 * @author Manuel Fernandez-Guasti
 */
void FastImagscaPower2Iteration(CVector3 &z)
{
	double x2 = z.x * z.x; //+ 1e-061
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;

	double newx = x2 - y2 - z2 + (y2 * z2) / x2;
	double newy = 2.0 * z.x * z.y * (1.0 - z2 / x2);
	double newz = 2.0 * z.x * z.z * (1.0 - y2 / x2);

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
	CVector3 tempVC = fractal->transformCommon.additionConstant000; // constant to be varied
	if (i < fractal->transformCommon.startIterations250)
	{
		;
	}
	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
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
	z += tempVC;
}

/**
 * Adds Cpixel constant to z vector
 */
void TransformAddCpixelIteration(CVector3 &z, CVector3 c, const cFractal *fractal)
{
	z += c * fractal->transformCommon.constantMultiplier111;
}

/**
 * Adds Cpixel constant to z vector, swapping the Cpixel vector x and y axes
 * disable swap for normal mode
 */
void TransformAddCpixelCxCyAxisSwapIteration(
	CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 tempC = c;
	if (fractal->transformCommon.functionEnabled)
	{
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			tempC = CVector3(tempC.y, tempC.x, tempC.z);
			aux.c = tempC;
		}
		else
		{
			tempC = CVector3(c.y, c.x, c.z);
		}
	}
	z += tempC * fractal->transformCommon.constantMultiplier111;
}

/**
 * Adds Cpixel constant to z vector. Possible to swap Cpixel vector axes.
 */
void TransformAddCpixelAxisSwapIteration(
	CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 tempC = c;
	if (fractal->transformCommon.alternateEnabledFalse) // alternate
	{
		tempC = aux.c;
		switch (fractal->mandelbulbMulti.orderOfxyz)
		{
			case sFractalMandelbulbMulti::xyz:
			default: tempC = CVector3(tempC.x, tempC.y, tempC.z); break;
			case sFractalMandelbulbMulti::xzy: tempC = CVector3(tempC.x, tempC.z, tempC.y); break;
			case sFractalMandelbulbMulti::yxz: tempC = CVector3(tempC.y, tempC.x, tempC.z); break;
			case sFractalMandelbulbMulti::yzx: tempC = CVector3(tempC.y, tempC.z, tempC.x); break;
			case sFractalMandelbulbMulti::zxy: tempC = CVector3(tempC.z, tempC.x, tempC.y); break;
			case sFractalMandelbulbMulti::zyx: tempC = CVector3(tempC.z, tempC.y, tempC.x); break;
		}
		aux.c = tempC;
	}
	else
	{
		switch (fractal->mandelbulbMulti.orderOfxyz)
		{
			case sFractalMandelbulbMulti::xyz:
			default: tempC = CVector3(c.x, c.y, c.z); break;
			case sFractalMandelbulbMulti::xzy: tempC = CVector3(c.x, c.z, c.y); break;
			case sFractalMandelbulbMulti::yxz: tempC = CVector3(c.y, c.x, c.z); break;
			case sFractalMandelbulbMulti::yzx: tempC = CVector3(c.y, c.z, c.x); break;
			case sFractalMandelbulbMulti::zxy: tempC = CVector3(c.z, c.x, c.y); break;
			case sFractalMandelbulbMulti::zyx: tempC = CVector3(c.z, c.y, c.x); break;
		}
	}
	z += tempC * fractal->transformCommon.constantMultiplier111;
}

/**
 * Adds Cpixel constant to z vector, with symmetry
 */
void TransformAddCpixelPosNegIteration(CVector3 &z, CVector3 c, const cFractal *fractal)
{
	CVector3 tempFAB = c;
	if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
	if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
	if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

	tempFAB *= fractal->transformCommon.constantMultiplier111;

	z.x += sign(z.x) * tempFAB.x;
	z.y += sign(z.y) * tempFAB.y;
	z.z += sign(z.z) * tempFAB.z;
}

/**
 * Adds Cpixel constant to z, Cpixel scaled  based on variable iteration parameters.
 */
void TransformAddCpixelVaryV1Iteration(CVector3 &z, CVector3 c, int i, const cFractal *fractal)
{
	CVector3 tempVC = fractal->transformCommon.constantMultiplier111; // constant to be varied
	if (i < fractal->transformCommon.startIterations250)
	{
		;
	}
	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
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
	z += c * tempVC;
}

/**
 * Add exp2(z)
 */
void TransformAddExp2ZIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 tempZ = z;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (fractal->transformCommon.functionEnabledAx)
		{
			tempZ.x = fabs(z.x);
			if (fractal->transformCommon.functionEnabledAxFalse)
			{
				tempZ.x = -tempZ.x;
			}
			tempZ.x = exp2(tempZ.x * fractal->transformCommon.constantMultiplier000.x) - 1.0;
			z.x += sign(z.x) * tempZ.x;
		}
		else
			z.x += exp2(tempZ.x * fractal->transformCommon.constantMultiplier000.x) - 1.0;
	}

	if (fractal->transformCommon.functionEnabledy)
	{
		if (fractal->transformCommon.functionEnabledAy)
		{
			tempZ.y = fabs(z.y);
			if (fractal->transformCommon.functionEnabledAyFalse)
			{
				tempZ.y = -tempZ.y;
			}
			tempZ.y = exp2(tempZ.y * fractal->transformCommon.constantMultiplier000.y) - 1.0;
			z.y += sign(z.y) * tempZ.y;
		}
		else
			z.y += exp2(tempZ.y * fractal->transformCommon.constantMultiplier000.y) - 1.0;
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		if (fractal->transformCommon.functionEnabledAz)
		{
			tempZ.z = fabs(z.z);
			if (fractal->transformCommon.functionEnabledAzFalse)
			{
				tempZ.z = -tempZ.z;
			}
			tempZ.z = exp2(tempZ.z * fractal->transformCommon.constantMultiplier000.z) - 1.0;
			z.z += sign(z.z) * tempZ.z;
		}
		else
			z.z += exp2(tempZ.z * fractal->transformCommon.constantMultiplier000.z) - 1.0;
	}
	aux.DE *= fractal->analyticDE.scale1; // DE tweak

	// DE calc version
	if (fractal->transformCommon.functionEnabledBzFalse)
	{
		CVector3 tempS;
		if (fractal->transformCommon.functionEnabled)
		{
			tempS = fabs(z);
			if (fractal->transformCommon.functionEnabledFalse)
			{
				tempS *= -1.0;
			}
			CVector3 tempT = tempS * fractal->transformCommon.scale0;
			tempS.x = exp2(tempT.x) - 1.0;
			tempS.y = exp2(tempT.y) - 1.0;
			tempS.z = exp2(tempT.z) - 1.0;

			z.x += sign(z.x) * tempS.x;
			z.y += sign(z.y) * tempS.y;
			z.z += sign(z.z) * tempS.z;
		}
		else
		{
			CVector3 tempR = z * fractal->transformCommon.scale0;
			z.x += exp2(tempR.x) - 1.0;
			z.y += exp2(tempR.y) - 1.0;
			z.z += exp2(tempR.z) - 1.0;
		}
		aux.DE = aux.DE + exp2(aux.DE * fractal->transformCommon.scale0) - 1.0;
	}
}

/**
 * benesiT1  3D
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT1Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z =
		CVector3((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

	CVector3 temp = z;
	double tempL = temp.Length();
	z = fabs(z) * fractal->transformCommon.scale3D222;
	// if (tempL < 1e-21) tempL = 1e-21;
	double avgScale = z.Length() / tempL;
	aux.r_dz *= avgScale;
	aux.DE = aux.DE * avgScale + 1.0;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	tempXZ = (z.y + z.x) * SQRT_1_2;

	z = CVector3(
		z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2, z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
	z = z - fractal->transformCommon.offset200;
}

/**
 * benesiT1Mod  3D based on benesiT1
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT1ModIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z =
		CVector3((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

	CVector3 temp = z;
	double tempL = temp.Length();
	z = fabs(z) * fractal->transformCommon.scale3D333;
	// if (tempL < 1e-21) tempL = 1e-21;
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

	z = CVector3(
		z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2, z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * benesi T2
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT2Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z =
		CVector3((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

	CVector3 tempV2 = z;
	tempV2.x = sqrt(z.y * z.y + z.z * z.z);
	tempV2.y = sqrt(z.x * z.x + z.z * z.z); // switching, squared, sqrt
	tempV2.z = sqrt(z.x * z.x + z.y * z.y);

	z = fabs(tempV2 - fractal->transformCommon.additionConstant111);

	CVector3 temp = z;
	double tempL = temp.Length();
	z = fabs(z) * fractal->transformCommon.scale3D444;
	// if (tempL < 1e-21) tempL = 1e-21;
	double avgScale = z.Length() / tempL;
	aux.r_dz *= avgScale;
	aux.DE = aux.DE * avgScale + 1.0;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	tempXZ = (z.y + z.x) * SQRT_1_2;
	z = CVector3(
		z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2, z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * benesi T3
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT3Iteration(CVector3 &z, const cFractal *fractal)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z =
		CVector3((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

	CVector3 tempV2 = z;
	tempV2.x = (z.y + z.z);
	tempV2.y = (z.x + z.z); // switching
	tempV2.z = (z.x + z.y);
	z = (fabs(tempV2 - fractal->transformCommon.additionConstant222))
			* fractal->transformCommon.scale3D222;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	tempXZ = (z.y + z.x) * SQRT_1_2;
	z = CVector3(
		z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2, z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * benesi T4
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT4Iteration(CVector3 &z, const cFractal *fractal)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z =
		CVector3((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

	CVector3 tempV2 = z;
	tempV2.x = (z.y * z.y + z.z * z.z);
	tempV2.y = (z.x * z.x + z.z * z.z); // switching, squared,
	tempV2.z = (z.x * z.x + z.y * z.y);
	z = (fabs(tempV2 - fractal->transformCommon.additionConstant111))
			* fractal->transformCommon.scale3D222;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	tempXZ = (z.y + z.x) * SQRT_1_2;
	z = CVector3(
		z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2, z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * benesi T5
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransformBenesiT5bIteration(CVector3 &z, const cFractal *fractal)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z =
		CVector3((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);

	// if (z.x > -1e-21 && z.x < 1e-21)
	// z.x = (z.x > 0) ? 1e-21 : -1e-21;
	// if (z.y > -1e-21 && z.y < 1e-21)
	// z.y = (z.y > 0) ? 1e-21 : -1e-21;
	// if (z.z > -1e-21 && z.z < 1e-21)
	// z.z = (z.z > 0) ? 1e-21 : -1e-21;
	CVector3 tempV2 = z;
	tempV2.x =
		fabs(pow(pow(z.y, fractal->transformCommon.int8X) + pow(z.z, fractal->transformCommon.int8X),
			fractal->transformCommon.power025.x));
	tempV2.y =
		fabs(pow(pow(z.x, fractal->transformCommon.int8Y) + pow(z.z, fractal->transformCommon.int8Y),
			fractal->transformCommon.power025.y));
	tempV2.z =
		fabs(pow(pow(z.x, fractal->transformCommon.int8Z) + pow(z.y, fractal->transformCommon.int8Z),
			fractal->transformCommon.power025.z));
	z = (fabs(tempV2 - fractal->transformCommon.offsetC111)) * fractal->transformCommon.scale3Dd222;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	tempXZ = (z.y + z.x) * SQRT_1_2;
	z = CVector3(
		z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2, z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * benesi mag forward
 */
void TransformBenesiMagForwardIteration(CVector3 &z)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z =
		CVector3((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3);
}

/**
 * benesi mag backward
 */
void TransformBenesiMagBackwardIteration(CVector3 &z)
{
	double tempXZ = (z.y + z.x) * SQRT_1_2;
	z = CVector3(
		z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2, z.z * SQRT_2_3 - tempXZ * SQRT_1_3);
}

/**
 * Benesi Cube to sphere transform
 * Warps a cube to a sphere; transform made by M.Benesi, optimized by Luca.
 * @reference http://www.fractalforums.com/mathematics/circle2square/
 */
void TransformBenesiCubeSphereIteration(CVector3 &z)
{
	z *= z; // so all now positive

	// if (z.x == 0.0)
	//	z.x = 1e-21;
	// if (z.z == 0.0)
	//	z.z = 1e-21;

	double rCyz = z.y / z.z;

	double rCxyz = (z.y + z.z) / z.x;

	if (rCxyz == -1.0) z.z = 1.0; //+ 1e-21
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
	// if (z.z == 0.0) z.z = 1e-21;
	double rCyz = z.y / z.z;
	if (rCyz < 1.0)
		rCyz = 1.0 / sqrt(rCyz + 1.0);
	else
		rCyz = 1.0 / sqrt(1.0 / rCyz + 1.0);

	z.y *= rCyz;
	z.z *= rCyz;

	// if (z.x == 0.0) z.x = 1e-21;
	double rCxyz = (z.y * z.y + z.z * z.z) / z.x;

	if (rCxyz < 1.0)
		rCxyz = 1.0 / sqrt(rCxyz + 1.0);
	else
		rCxyz = 1.0 / sqrt(1.0 / rCxyz + 1.0);

	z *= rCxyz * SQRT_3_2;
}

/**
 * Box Fold
 */
void TransformBoxFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fabs(z.x) > fractal->mandelbox.foldingLimit)
	{
		z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
		aux.color += fractal->mandelbox.color.factor.x;
	}
	if (fabs(z.y) > fractal->mandelbox.foldingLimit)
	{
		z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
		aux.color += fractal->mandelbox.color.factor.y;
	}
	double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
	double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
	if (fabs(z.z) > zLimit)
	{
		z.z = sign(z.z) * zValue - z.z;
		aux.color += fractal->mandelbox.color.factor.z;
	}
}

/**
 * Box Fold VaryV1. Varies folding limit based on iteration conditions
 */
void TransformBoxFoldVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double limit = fractal->mandelbox.foldingLimit;
	// double value = 2.0 *fractal->mandelbox.foldingLimit;
	double tempVC = limit; // constant to be varied

	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
	{
		tempVC = (tempVC
							+ fractal->transformCommon.offset * (i - fractal->transformCommon.startIterations250)
									/ (fractal->transformCommon.stopIterations
											- fractal->transformCommon.startIterations250));
	}
	if (i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset);
	}

	limit = tempVC;
	double value = fractal->transformCommon.scale2 * limit;

	if (z.x > limit)
	{
		z.x = value - z.x;
		aux.color += fractal->mandelbox.color.factor.x;
	}
	else if (z.x < -limit)
	{
		z.x = -value - z.x;
		aux.color += fractal->mandelbox.color.factor.x;
	}
	if (z.y > limit)
	{
		z.y = value - z.y;
		aux.color += fractal->mandelbox.color.factor.y;
	}
	else if (z.y < -limit)
	{
		z.y = -value - z.y;
		aux.color += fractal->mandelbox.color.factor.y;
	}
	double zLimit = limit * fractal->transformCommon.scale1;
	double zValue = value * fractal->transformCommon.scale1;
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
	aux.DE *= fractal->analyticDE.scale1;
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
		z.x = -fractal->transformCommon.additionConstant222.x - z.x;
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
		z.z = -fractal->transformCommon.additionConstant222.z - z.z;
		aux.color += fractal->mandelbox.color.factor.z;
	}
}

/**
 * Box Offset, set different offset for each axis, added symmetrically about the origin
 */
void TransformBoxOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 temp = z;
	z.x = sign(z.x) * fractal->transformCommon.additionConstant000.x + z.x;
	z.y = sign(z.y) * fractal->transformCommon.additionConstant000.y + z.y;
	z.z = sign(z.z) * fractal->transformCommon.additionConstant000.z + z.z;

	if (fractal->transformCommon.functionEnabledFalse)
	{
		double tempL = temp.Length();
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.r_dz *= avgScale;
		aux.DE = aux.DE * avgScale + 1.0;
	}
}

/**
 * fabs add  constant,  z = fabs( z + constant)
 */
void TransformFabsAddConstantIteration(CVector3 &z, const cFractal *fractal)
{
	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);

	z += fractal->transformCommon.offsetA000;
}

/**
 * fabs. Add fabs constantV2,  z = fabs( z + constant) - fabs( z - constant) - z:
 * tglad's fold
 */
void TransformFabsAddConstantV2Iteration(CVector3 &z, int i, const cFractal *fractal)
{
	z = fabs(z + fractal->transformCommon.additionConstant000)
			- fabs(z - fractal->transformCommon.additionConstant000) - z;

	if (fractal->transformCommon.functionEnabledFalse
			&& i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA)
	{
		CVector3 limit = fractal->transformCommon.additionConstant000;
		CVector3 length = 2 * limit;
		CVector3 tgladS = 2 / (4 * limit);
		CVector3 Add;
		if (fabs(z.x) < limit.x) Add.x = z.x * z.x * tgladS.x;
		if (fabs(z.y) < limit.y) Add.y = z.y * z.y * tgladS.y;
		if (fabs(z.z) < limit.z) Add.z = z.z * z.z * tgladS.z;
		if (fabs(z.x) > limit.x && fabs(z.x) < length.x)
			Add.x = (length.x - fabs(z.x)) * (length.x - fabs(z.x)) * tgladS.x;
		if (fabs(z.y) > limit.y && fabs(z.y) < length.y)
			Add.y = (length.y - fabs(z.y)) * (length.y - fabs(z.y)) * tgladS.y;
		if (fabs(z.z) > limit.z && fabs(z.z) < length.z)
			Add.z = (length.z - fabs(z.z)) * (length.z - fabs(z.z)) * tgladS.z;
		Add *= fractal->transformCommon.scale3D111;
		z.x = (z.x - (sign(z.x) * (Add.x)));
		z.y = (z.y - (sign(z.y) * (Add.y)));
		z.z = (z.z - (sign(z.z) * (Add.z)));
	}
}

/**
* fabs add conditional
*/
void TransformFabsAddConditionalIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledx)
	{
		z.x = sign(z.x) * (fractal->transformCommon.offset111.x - fabs(z.x)
												+ fabs(z.x) * fractal->transformCommon.offset000.x);
	}

	if (fractal->transformCommon.functionEnabledy)
	{
		z.y = sign(z.y) * (fractal->transformCommon.offset111.y - fabs(z.y)
												+ fabs(z.y) * fractal->transformCommon.offset000.y);
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		z.z = sign(z.z) * (fractal->transformCommon.offset111.z - fabs(z.z)
												+ fabs(z.z) * fractal->transformCommon.offset000.z);

	}																			// aux.DE = aux.DE * l/L;
	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}

/**
 * fabs. Negative fabs constant,  z = - fabs( z + constant)
 */
void TransformNegFabsAddConstantIteration(CVector3 &z, const cFractal *fractal)
{
	z += fractal->transformCommon.additionConstant000;
	if (fractal->transformCommon.functionEnabledx) z.x = -fabs(z.x);

	if (fractal->transformCommon.functionEnabledy) z.y = -fabs(z.y);

	if (fractal->transformCommon.functionEnabledz) z.z = -fabs(z.z);
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

	z += fractal->transformCommon.offsetA000;
}

/**
 * folding tetra3D from M3D (Luca GN 2011):
 * Code taken from the forums, KIFS original thread
 * side note - if you disable the 1st half, 2nd half will be
 * done even if you disable it... (to avoid a NOP transform)
 */
void TransformFoldingTetra3DIteration(CVector3 &z, const cFractal *fractal)
{
	double x1;
	double y1;
	if (fractal->transformCommon.functionEnabledx)
	{
		if (z.x + z.y < 0.0)
		{
			x1 = -z.y;
			z.y = -z.x;
			z.x = x1;
		}
		if (z.x + z.z < 0.0)
		{
			x1 = -z.z;
			z.z = -z.x;
			z.x = x1;
		}
		if (z.y + z.z < 0.0)
		{
			y1 = -z.z;
			z.z = -z.y;
			z.y = y1;
		}
	}
	if (fractal->transformCommon.functionEnabledy)
	{
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
	}
}

/**
 * iteration weight. Influence fractal based on the weight of
 * Z values after different iterations
 */
void TransformIterationWeightIteration(
	CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 zA = (i == fractal->transformCommon.intA) ? z : CVector3();
	CVector3 zB = (i == fractal->transformCommon.intB) ? z : CVector3();

	z = (z * fractal->transformCommon.scale) + (zA * fractal->transformCommon.offset)
			+ (zB * fractal->transformCommon.offset0);
	aux.DE *= fractal->transformCommon.scale;
	aux.r_dz *= fractal->transformCommon.scale;
}

/**
 * Inverse cylindrical coords, very easy transform
 * Formula by Luca GN 2011
 */
void TransformInvCylindricalIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double newZx = z.x;
	double newZy = z.y;

	if (fractal->transformCommon.functionEnabledFalse) newZx = z.x * cos(z.y);
	if (fractal->transformCommon.functionEnabledxFalse) newZy = z.x * sin(z.y);

	z = CVector3(z.x * cos(newZy * fractal->transformCommon.scale1),
				newZx * sin(z.y * fractal->transformCommon.scale1), z.z * fractal->transformCommon.scaleC1)
			* fractal->transformCommon.scaleA1;

	aux.DE = aux.DE * fabs(fractal->transformCommon.scaleA1) * fractal->transformCommon.scaleB1
					 + fractal->transformCommon.offset1;
	aux.r_dz *= fabs(fractal->transformCommon.scaleA1) * fractal->transformCommon.scaleB1;
}

/**
 * Linear Combine transform from Mandelbulb3D.
 * Can create multiple combination for the addition of Cpixel
 */
void TransformLinCombineCxyz(CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 temp = c;
	CVector3 mulX = fractal->transformCommon.constantMultiplier100;
	CVector3 mulY = fractal->transformCommon.constantMultiplier010;
	CVector3 mulZ = fractal->transformCommon.constantMultiplier001;
	c.x = mulX.x * temp.x + mulX.y * temp.y + mulX.z * temp.z;
	c.y = mulY.x * temp.x + mulY.y * temp.y + mulY.z * temp.z;
	c.z = mulZ.x * temp.x + mulZ.y * temp.y + mulZ.z * temp.z;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfxyzC)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(tempC.x, tempC.y, tempC.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(tempC.x, tempC.z, tempC.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(tempC.y, tempC.x, tempC.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(tempC.y, tempC.z, tempC.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(tempC.z, tempC.x, tempC.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(tempC.z, tempC.y, tempC.x); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfxyzC)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(c.x, c.y, c.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(c.x, c.z, c.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(c.y, c.x, c.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(c.y, c.z, c.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(c.z, c.x, c.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(c.z, c.y, c.x); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
}
/**
 * Transform Menger Fold
 * Menger Sponge formula created by Knighty
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/kaleidoscopic-(escape-time-ifs)/
 */
void TransformMengerFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledx)
	{ // fabs() and menger fold
		z = fabs(z + fractal->transformCommon.additionConstantA000);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
	}
	if (fractal->transformCommon.functionEnabledy)
	{ // menger scales and offsets
		z *= fractal->transformCommon.scale3;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplier111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplier111.y;
		if (fractal->transformCommon.functionEnabled)
		{
			if (z.z > 1.0) // z.z < 1.0  z.z = z.z
				z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
		}
		else
		{
			z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
		}
		aux.DE *= fractal->transformCommon.scale3;
	}
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
 * Octo
 */
void TransformOctoFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (z.x + z.y < 0.0) z = CVector3(-z.y, -z.x, z.z);

	if (z.x + z.z < 0.0) // z.xz = -z.zx;
		z = CVector3(-z.z, z.y, -z.x);

	if (z.x - z.y < 0.0) // z.xy = z.yx;
		z = CVector3(z.y, z.x, z.z);

	if (z.x - z.z < 0.0) // z.xz = z.zx;
		z = CVector3(z.z, z.y, z.x);

	z.x = fabs(z.x);
	z = z * fractal->transformCommon.scale2
			- fractal->transformCommon.offset100 * (fractal->transformCommon.scale2 - 1.0);

	aux.DE *= fractal->transformCommon.scale2;
}

/**
 * platonic solid
 * @reference
 * http://www.fractalforums.com/3d-fractal-generation/platonic-dimensions/msg36528/#msg36528
 */
void TransformPlatonicSolidIteration(CVector3 &z, const cFractal *fractal)
{
	double rho = sqrt(z.Length()); // the radius
	double theta =
		cos(fractal->platonicSolid.frequency * z.x) * sin(fractal->platonicSolid.frequency * z.y)
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
 * z = (Az^2 + Bz + C ) / D; ((VectA +ScaleA * z) * fn(z) + scaleB * z + VectC ) * ScaleD
 */
void TransformPwr2PolynomialIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 partA = z;
	if (fractal->transformCommon.functionEnabledFalse) // fabs
		partA = fabs(z);
	if (fractal->transformCommon.functionEnabledxFalse) // pwr3 or z * fabs(z^2)
		partA *= z;
	partA = partA * fractal->transformCommon.scale2 + fractal->transformCommon.constantMultiplier111;

	CVector3 fnZ1 = z;
	if (fractal->transformCommon.functionEnabledBxFalse) // cos(z*Pi)
	{
		double scalePi = M_PI * fractal->transformCommon.scaleC1;
		fnZ1.x = cos(z.x * scalePi);
		fnZ1.y = cos(z.y * scalePi);
		fnZ1.z = cos(z.z * scalePi);
	}
	if (fractal->transformCommon.functionEnabledyFalse) // pi rotation
		fnZ1 = fnZ1.RotateAroundVectorByAngle(fractal->transformCommon.constantMultiplier111,
			M_PI * fractal->transformCommon.scale0);				// * cPI ;
	if (fractal->transformCommon.functionEnabledzFalse) // box offset
	{
		fnZ1.x = fnZ1.x + sign(fnZ1.x) * fractal->transformCommon.additionConstant000.x;
		fnZ1.y = fnZ1.y + sign(fnZ1.y) * fractal->transformCommon.additionConstant000.y;
		fnZ1.z = fnZ1.z + sign(fnZ1.z) * fractal->transformCommon.additionConstant000.z;
	}

	if (fractal->transformCommon.functionEnabledAxFalse) // fabs fnZ1
		fnZ1 = fabs(fnZ1);

	CVector3 partB = z;
	if (fractal->transformCommon.functionEnabledAzFalse) partB = fabs(z);
	partB *= fractal->transformCommon.scale4;
	z = fractal->transformCommon.constantMultiplierB111 + partB - partA * fnZ1;
	z *= fractal->transformCommon.scale025;

	aux.DE = aux.DE * 4.0 * fractal->analyticDE.scaleLin + fractal->analyticDE.offsetLin;
}

/**
 * Quaternion Fold Transform
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void TransformQuaternionFoldIteration(
	CVector3 &z, CVector3 c, const cFractal *fractal, sExtendedAux &aux)
{
	z = CVector3(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z);
	if (fractal->transformCommon.functionEnabledFalse)
	{
		aux.r_dz = aux.r_dz * 2.0 * aux.r;
		double tempL = z.Length();
		z *= fractal->transformCommon.constantMultiplier122;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = CVector3(z.x, z.y / 2.0, z.z / 2.0).Length() / tempL;
		double tempAux = aux.r_dz * avgScale;
		aux.r_dz = aux.r_dz + (tempAux - aux.r_dz) * fractal->transformCommon.scaleA1;
		z += fractal->transformCommon.additionConstant000;
	}
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector3 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfxyzC)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(tempC.x, tempC.y, tempC.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(tempC.x, tempC.z, tempC.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(tempC.y, tempC.x, tempC.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(tempC.y, tempC.z, tempC.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(tempC.z, tempC.x, tempC.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(tempC.z, tempC.y, tempC.x); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfxyzC)
			{
				case sFractalMandelbulbMulti::xyz:
				default: tempC = CVector3(c.x, c.y, c.z); break;
				case sFractalMandelbulbMulti::xzy: tempC = CVector3(c.x, c.z, c.y); break;
				case sFractalMandelbulbMulti::yxz: tempC = CVector3(c.y, c.x, c.z); break;
				case sFractalMandelbulbMulti::yzx: tempC = CVector3(c.y, c.z, c.x); break;
				case sFractalMandelbulbMulti::zxy: tempC = CVector3(c.z, c.x, c.y); break;
				case sFractalMandelbulbMulti::zyx: tempC = CVector3(c.z, c.y, c.x); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}
}

/**
 * Reciprocal3  based on Darkbeam's code from M3D,
 */
void TransformReciprocal3Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 tempZ = z;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.x = (1.0 / fractal->transformCommon.offset111.x)
								- 1.0 / (fabs(z.x) + fractal->transformCommon.offset111.x);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.x = (fractal->transformCommon.offsetA111.x)
								- 1.0 / (fabs(z.x) + fractal->transformCommon.offset111.x);

		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.x = (1.0 / fractal->transformCommon.offset111.x)
								+ (1.0 / fractal->transformCommon.offsetA111.x)
								- 1.0 / (fabs(z.x * M1) + fractal->transformCommon.offset111.x)
								- 1.0 / ((z.x * z.x * M2) + fractal->transformCommon.offsetA111.x);
		}
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.x = fractal->transformCommon.offsetB111.x
								- 1.0 / (fabs(z.x * M1) + fractal->transformCommon.offset111.x)
								- 1.0 / ((z.x * z.x * M2) + fractal->transformCommon.offsetA111.x);
		}

		tempZ.x += fabs(z.x) * fractal->transformCommon.offset000.x; // function slope
		z.x = sign(z.x) * tempZ.x;
	}

	if (fractal->transformCommon.functionEnabledy)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.y = (1.0 / fractal->transformCommon.offset111.y)
								- 1.0 / (fabs(z.y) + fractal->transformCommon.offset111.y);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.y = (fractal->transformCommon.offsetA111.y)
								- 1.0 / (fabs(z.y) + fractal->transformCommon.offset111.y);

		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.y = (1.0 / fractal->transformCommon.offset111.y)
								+ (1.0 / fractal->transformCommon.offsetA111.y)
								- 1.0 / (fabs(z.y * M1) + fractal->transformCommon.offset111.y)
								- 1.0 / ((z.y * z.y * M2) + fractal->transformCommon.offsetA111.y);
		}

		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.y = fractal->transformCommon.offsetB111.y
								- 1.0 / (fabs(z.y * M1) + fractal->transformCommon.offset111.y)
								- 1.0 / ((z.y * z.y * M2) + fractal->transformCommon.offsetA111.y);
		}
		tempZ.y += fabs(z.y) * fractal->transformCommon.offset000.y;
		z.y = sign(z.y) * tempZ.y;
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.z = (1.0 / fractal->transformCommon.offset111.z)
								- 1.0 / (fabs(z.z) + fractal->transformCommon.offset111.z);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.z = (fractal->transformCommon.offsetA111.z)
								- 1.0 / (fabs(z.z) + fractal->transformCommon.offset111.z);

		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.z = (1.0 / fractal->transformCommon.offset111.z)
								+ (1.0 / fractal->transformCommon.offsetA111.z)
								- 1.0 / (fabs(z.z * M1) + fractal->transformCommon.offset111.z)
								- 1.0 / ((z.z * z.z * M2) + fractal->transformCommon.offsetA111.z);
		}
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.z = fractal->transformCommon.offsetB111.z
								- 1.0 / (fabs(z.z * M1) + fractal->transformCommon.offset111.z)
								- 1.0 / ((z.z * z.z * M2) + fractal->transformCommon.offsetA111.z);
		}

		tempZ.z += fabs(z.z) * fractal->transformCommon.offset000.z;
		z.z = sign(z.z) * tempZ.z;
	}
	// aux.DE = aux.DE * l/L;
	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}

/**
 * rotation
 */
void TransformRotationIteration(CVector3 &z, const cFractal *fractal)
{
	z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

/**
 * rotation folding plane
 */
void TransformRotationFoldingPlane(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	CVector3 zRot;

	zRot = fractal->mandelbox.rot[0][0].RotateVector(z);
	if (zRot.x > fractal->mandelbox.foldingLimit)
	{
		zRot.x = fractal->mandelbox.foldingValue - zRot.x;
		z = fractal->mandelbox.rotinv[0][0].RotateVector(zRot);
		aux.color += fractal->mandelbox.color.factor.x;
	}
	else
	{
		zRot = fractal->mandelbox.rot[1][0].RotateVector(z);
		if (zRot.x < -fractal->mandelbox.foldingLimit)
		{
			zRot.x = -fractal->mandelbox.foldingValue - zRot.x;
			z = fractal->mandelbox.rotinv[1][0].RotateVector(zRot);
			aux.color += fractal->mandelbox.color.factor.x;
		}
	};

	zRot = fractal->mandelbox.rot[0][1].RotateVector(z);
	if (zRot.y > fractal->mandelbox.foldingLimit)
	{
		zRot.y = fractal->mandelbox.foldingValue - zRot.y;
		z = fractal->mandelbox.rotinv[0][1].RotateVector(zRot);
		aux.color += fractal->mandelbox.color.factor.y;
	}
	else
	{
		zRot = fractal->mandelbox.rot[1][1].RotateVector(z);
		if (zRot.y < -fractal->mandelbox.foldingLimit)
		{
			zRot.y = -fractal->mandelbox.foldingValue - zRot.y;
			z = fractal->mandelbox.rotinv[1][1].RotateVector(zRot);
			aux.color += fractal->mandelbox.color.factor.y;
		}
	}

	zRot = fractal->mandelbox.rot[0][2].RotateVector(z);
	if (zRot.z > fractal->mandelbox.foldingLimit)
	{
		zRot.z = fractal->mandelbox.foldingValue - zRot.z;
		z = fractal->mandelbox.rotinv[0][2].RotateVector(zRot);
		aux.color += fractal->mandelbox.color.factor.z;
	}
	else
	{
		zRot = fractal->mandelbox.rot[1][2].RotateVector(z);
		if (zRot.z < -fractal->mandelbox.foldingLimit)
		{
			zRot.z = -fractal->mandelbox.foldingValue - zRot.z;
			z = fractal->mandelbox.rotinv[1][2].RotateVector(zRot);
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}
}

/**
 * Rpow3 from M3D.
 * Does a power of 3 on the current length of the  vector.
 */
void TransformRpow3Iteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double sqrRout = z.Dot(z) * fractal->transformCommon.scale;

	z *= sqrRout;
	aux.DE = aux.DE * fabs(sqrRout) + fractal->analyticDE.offset1;
}

/**
 * rotation variation v1. Rotation angles vary based on iteration parameters.
 */
void TransformRotationVaryV1Iteration(CVector3 &z, int i, const cFractal *fractal)
{
	CVector3 tempVC = fractal->transformCommon.rotation; // constant to be varied

	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
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

	tempVC *= M_PI / 180.0;

	z = z.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), tempVC.x);
	z = z.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), tempVC.y);
	z = z.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), tempVC.z);
}

/**
 * rotatedAbs & Rotated Folding transform from M3D
 * - Rotate by the given angles
 *- fold
 *- RotateBack by the given angles
 */

void TransformRotatedFoldingIteration(CVector3 &z, const cFractal *fractal)
{
	z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.functionEnabled)
	{
		if (fractal->transformCommon.functionEnabledx)
			z.x = fabs(z.x + fractal->transformCommon.offset000.x) - fractal->transformCommon.offset000.x;
		if (fractal->transformCommon.functionEnabledy)
			z.y = fabs(z.y + fractal->transformCommon.offset000.y) - fractal->transformCommon.offset000.y;
		if (fractal->transformCommon.functionEnabledz)
			z.z = fabs(z.z + fractal->transformCommon.offset000.z) - fractal->transformCommon.offset000.z;
	}

	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		if (fractal->transformCommon.functionEnabledAx)
			z.x = fabs(z.x + fractal->transformCommon.offset111.x)
						- fabs(z.x - fractal->transformCommon.offset111.x) - z.x;
		if (fractal->transformCommon.functionEnabledAy)
			z.y = fabs(z.y + fractal->transformCommon.offset111.y)
						- fabs(z.y - fractal->transformCommon.offset111.y) - z.y;
		if (fractal->transformCommon.functionEnabledAz)
			z.z = fabs(z.z + fractal->transformCommon.offset111.z)
						- fabs(z.z - fractal->transformCommon.offset111.z) - z.z;
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		if (fractal->transformCommon.functionEnabledBx)
		{
			if (fabs(z.x) > fractal->mandelbox.foldingLimit)
			{
				z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
				// aux.color += fractal->mandelbox.color.factor.x;
			}
		}
		if (fractal->transformCommon.functionEnabledBy)
		{
			if (fabs(z.y) > fractal->mandelbox.foldingLimit)
			{
				z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
				// aux.color += fractal->mandelbox.color.factor.y;
			}
		}
		if (fractal->transformCommon.functionEnabledBz)
		{
			double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
			double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
			if (fabs(z.z) > zLimit)
			{
				z.z = sign(z.z) * zValue - z.z;
				// aux.color += fractal->mandelbox.color.factor.z;
			}
		}
	}
	z = fractal->transformCommon.rotationMatrix2.RotateVector(z);
}

/**
 * scale - simple scale
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
	double tempVC = fractal->transformCommon.scale; // constant to be varied

	if (i >= fractal->transformCommon.startIterations250
			&& i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
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
	aux.DE = aux.DE * fabs(tempVC) + 1.0; // prepared for future analytic DE for hybrids
	aux.r_dz *= fabs(tempVC);
}
/**
 *  Scale varies smoothly based on iteration parameters.
 */
void TransformScaleVaryVCLIteration(CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double para = 0.0;

	if (fractal->Cpara.enabledLinear)
	{
		para = fractal->Cpara.para0; // parameter value at iter 0
		double temp0 = para;
		double tempA = fractal->Cpara.paraA;
		double tempB = fractal->Cpara.paraB;
		double tempC = fractal->Cpara.paraC;
		double lengthAB = fractal->Cpara.iterB - fractal->Cpara.iterA;
		double lengthBC = fractal->Cpara.iterC - fractal->Cpara.iterB;
		double grade1 = (tempA - temp0) / fractal->Cpara.iterA;
		double grade2 = (tempB - tempA) / lengthAB;
		double grade3 = (tempC - tempB) / lengthBC;

		// slopes
		if (i < fractal->Cpara.iterA)
		{
			para = temp0 + (i * grade1);
		}
		if (i < fractal->Cpara.iterB && i >= fractal->Cpara.iterA)
		{
			para = tempA + (i - fractal->Cpara.iterA) * grade2;
		}
		if (i >= fractal->Cpara.iterB)
		{
			para = tempB + (i - fractal->Cpara.iterB) * grade3;
		}

		// Curvi part on "true"
		if (fractal->Cpara.enabledCurves)
		{
			double paraAdd = 0.0;
			double paraIt;
			if (lengthAB > 2.0 * fractal->Cpara.iterA) // stop  error, todo fix.
			{
				double curve1 = (grade2 - grade1) / (4.0 * fractal->Cpara.iterA);
				double tempL = lengthAB - fractal->Cpara.iterA;
				double curve2 = (grade3 - grade2) / (4.0 * tempL);
				if (i < 2 * fractal->Cpara.iterA)
				{
					paraIt = tempA - fabs(tempA - i);
					paraAdd = paraIt * paraIt * curve1;
				}
				if (i >= 2 * fractal->Cpara.iterA && i < fractal->Cpara.iterB + tempL)
				{
					paraIt = tempB - fabs(tempB * i);
					paraAdd = paraIt * paraIt * curve2;
				}
				para += paraAdd;
			}
		}
	}
	// Parabolic
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset + (i * fractal->Cpara.parabSlope)
								+ (i * i * 0.001 * fractal->Cpara.parabScale);
	}
	para += paraAddP0;

	z *= para; // using the parameter
	aux.DE = aux.DE * fabs(para) + 1.0;
	aux.r_dz *= fabs(para);
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
 * spherical invert
 * from M3D. Formula by Luca GN 2011, updated May 2012.
 */
void TransformSphereInvIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double r2Temp = z.Dot(z);
	z += fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;

	double r2 = z.Dot(z);
	if (fractal->transformCommon.functionEnabledyFalse) r2 = r2Temp;
	double mode = r2;
	if (fractal->transformCommon.functionEnabledFalse) // Mode 1
	{
		if (r2 > fractal->transformCommon.minRneg1) mode = 1.0f;
		if (r2 < fractal->mandelbox.fR2 && r2 > fractal->transformCommon.minRneg1)
			mode = fractal->transformCommon.minRneg1;
		if (r2 < fractal->mandelbox.fR2 && r2 < fractal->transformCommon.minRneg1)
			mode = fractal->transformCommon.minRneg1;
	}
	if (fractal->transformCommon.functionEnabledxFalse) // Mode 2
	{
		if (r2 > fractal->transformCommon.minRneg1) mode = 1.0f;
		if (r2 < fractal->mandelbox.fR2 && r2 > fractal->transformCommon.minRneg1)
			mode = fractal->transformCommon.minRneg1;
		if (r2 < fractal->mandelbox.fR2 && r2 < fractal->transformCommon.minRneg1)
			mode = 2.0 * fractal->transformCommon.minRneg1 - r2;
	}
	mode = 1.0 / mode;
	z *= mode;
	aux.DE *= mode;

	z -= fractal->mandelbox.offset + fractal->transformCommon.additionConstant000;
}

/**
 * inverted sphere z & c- A transform from M3D
 *
 */
void TransformSphereInvCIteration(CVector3 &z, CVector3 c, const cFractal *fractal)
{
	c *= fractal->transformCommon.constantMultiplier111;
	double rSqrL = c.x * c.x + c.y * c.y + c.z * c.z;
	// if (rSqrL < 1e-21) rSqrL = 1e-21;
	rSqrL = 1.0 / rSqrL;
	c *= rSqrL;

	rSqrL = z.x * z.x + z.y * z.y + z.z * z.z;
	// if (rSqrL < 1e-21) rSqrL = 1e-21;
	rSqrL = 1.0 / rSqrL;
	z *= rSqrL;
}

/**
 * spherical fold MBox
 */
void TransformSphericalFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double r2 = z.Dot(z);

	z += fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;													// beta
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0; // beta
	// if (r2 < 1e-21) r2 = 1e-21;
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
}

/**
 * spherical fold ABox
 * from Fractal Forums and M3D
 */
void TransformSphericalFoldAboxIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double r2 = z.Dot(z);
	z += fractal->mandelbox.offset;
	double sqrtMinR = fractal->transformCommon.sqtR;
	if (r2 < sqrtMinR)
	{
		z *= fractal->transformCommon.mboxFactor1;
		aux.DE *= fractal->transformCommon.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < 1.0)
	{
		z *= 1.0 / r2;
		aux.DE *= 1.0 / r2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
	z -= fractal->mandelbox.offset;
}

/**
 * spherical fold varyV1 MBox type
 */
void TransformSphericalFoldVaryV1Iteration(
	CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double r2 = z.Dot(z);
	double tempVCf = fractal->mandelbox.foldingSphericalFixed; // constant to be varied
	double tempVCm = fractal->mandelbox.foldingSphericalMin;

	if (i >= fractal->transformCommon.startIterationsA
			&& i < fractal->transformCommon.stopIterationsA)
	{
		tempVCf = (tempVCf
							 + fractal->transformCommon.offset * (i - fractal->transformCommon.startIterationsA)
									 / (fractal->transformCommon.stopIterationsA
											 - fractal->transformCommon.startIterationsA));
	}
	if (i >= fractal->transformCommon.stopIterationsA)
	{
		tempVCf = (tempVCf + fractal->transformCommon.offset);
	}
	if (i >= fractal->transformCommon.startIterationsB
			&& i < fractal->transformCommon.stopIterationsB)
	{
		tempVCm = (tempVCm
							 + fractal->transformCommon.offset0 * (i - fractal->transformCommon.startIterationsB)
									 / (fractal->transformCommon.stopIterationsB
											 - fractal->transformCommon.startIterationsB));
	}
	if (i >= fractal->transformCommon.stopIterationsB)
	{
		tempVCm = tempVCm + fractal->transformCommon.offset0;
	}

	z += fractal->mandelbox.offset;

	tempVCm *= tempVCm;
	tempVCf *= tempVCf;

	if (r2 < tempVCm)
	{
		z *= tempVCf / tempVCm;
		aux.DE *= tempVCf / tempVCm;
		;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < tempVCf)
	{
		double tglad_factor2 = tempVCf / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
	z -= fractal->mandelbox.offset;
}

/**
 * spherical fold varyVCL MBox type
 */
void TransformSpherFoldVaryVCLIteration(
	CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double para = 0.0;
	if (fractal->Cpara.enabledLinear)
	{
		para = fractal->Cpara.para00; // parameter value at iter 0
		double temp0 = para;
		double tempA = fractal->Cpara.paraA;
		double tempB = fractal->Cpara.paraB;
		double tempC = fractal->Cpara.paraC;
		double lengthAB = fractal->Cpara.iterB - fractal->Cpara.iterA;
		double lengthBC = fractal->Cpara.iterC - fractal->Cpara.iterB;
		double grade1 = (tempA - temp0) / fractal->Cpara.iterA;
		double grade2 = (tempB - tempA) / lengthAB;
		double grade3 = (tempC - tempB) / lengthBC;

		// slopes
		if (i < fractal->Cpara.iterA)
		{
			para = temp0 + (i * grade1);
		}
		if (i < fractal->Cpara.iterB && i >= fractal->Cpara.iterA)
		{
			para = tempA + (i - fractal->Cpara.iterA) * grade2;
		}
		if (i >= fractal->Cpara.iterB)
		{
			para = tempB + (i - fractal->Cpara.iterB) * grade3;
		}

		// Curvi part on "true"
		if (fractal->Cpara.enabledCurves)
		{
			double paraAdd = 0.0;
			double paraIt;
			if (lengthAB > 2.0 * fractal->Cpara.iterA) // stop  error, todo fix.
			{
				double curve1 = (grade2 - grade1) / (4.0 * fractal->Cpara.iterA);
				double tempL = lengthAB - fractal->Cpara.iterA;
				double curve2 = (grade3 - grade2) / (4.0 * tempL);
				if (i < 2 * fractal->Cpara.iterA)
				{
					paraIt = tempA - fabs(tempA - i);
					paraAdd = paraIt * paraIt * curve1;
				}
				if (i >= 2 * fractal->Cpara.iterA && i < fractal->Cpara.iterB + tempL)
				{
					paraIt = tempB - fabs(tempB * i);
					paraAdd = paraIt * paraIt * curve2;
				}
				para += paraAdd;
			}
		}
	}
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (i * fractal->Cpara.parabSlope)
								+ (i * i * 0.001 * fractal->Cpara.parabScale);
	}
	para += paraAddP0 + fractal->transformCommon.minR2p25;

	// spherical fold
	double r2 = z.Dot(z);

	z += fractal->mandelbox.offset;

	// if (r2 < 1e-21) r2 = 1e-21;
	if (r2 < para)
	{
		z *= fractal->transformCommon.maxR2d1 / para;
		aux.DE *= fractal->transformCommon.maxR2d1 / para;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->transformCommon.maxR2d1 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
	z -= fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
}

/**
 * spherical radial offset.
 */
void TransformSphericalOffsetIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	double para = fractal->transformCommon.offset;
	if (fractal->transformCommon.functionEnabled)
	{
		double lengthTempZ = -z.Length();
		// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 + para / lengthTempZ;
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
		aux.r_dz *= fabs(fractal->transformCommon.scale);
	}
}

/**
 * spherical radial offset Curvilinear.
 */
void TransformSphericalOffsetVCLIteration(
	CVector3 &z, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double para = fractal->Cpara.para00;
	double paraAdd = 0.0;
	double paraDot = 0.0;
	double paraAddP0 = 0.0;
	// curvilinear mode
	if (fractal->transformCommon.functionEnabled)
	{
		if (fractal->Cpara.enabledLinear)
		{
			para = fractal->Cpara.para00; // parameter value at iter 0
			double temp0 = para;
			double tempA = fractal->Cpara.paraA0;
			double tempB = fractal->Cpara.paraB0;
			double tempC = fractal->Cpara.paraC0;
			double lengthAB = fractal->Cpara.iterB - fractal->Cpara.iterA;
			double lengthBC = fractal->Cpara.iterC - fractal->Cpara.iterB;
			double grade1 = (tempA - temp0) / fractal->Cpara.iterA;
			double grade2 = (tempB - tempA) / lengthAB;
			double grade3 = (tempC - tempB) / lengthBC;

			// slopes
			if (i < fractal->Cpara.iterA)
			{
				para = temp0 + (i * grade1);
			}
			if (i < fractal->Cpara.iterB && i >= fractal->Cpara.iterA)
			{
				para = tempA + (i - fractal->Cpara.iterA) * grade2;
			}
			if (i >= fractal->Cpara.iterB)
			{
				para = tempB + (i - fractal->Cpara.iterB) * grade3;
			}

			// Curvi part on "true"
			if (fractal->Cpara.enabledCurves)
			{
				// double paraAdd = 0.0;
				double paraIt;
				if (lengthAB > 2.0 * fractal->Cpara.iterA) // stop  error, todo fix.
				{
					double curve1 = (grade2 - grade1) / (4.0 * fractal->Cpara.iterA);
					double tempL = lengthAB - fractal->Cpara.iterA;
					double curve2 = (grade3 - grade2) / (4.0 * tempL);
					if (i < 2 * fractal->Cpara.iterA)
					{
						paraIt = tempA - fabs(tempA - i);
						paraAdd = paraIt * paraIt * curve1;
					}
					if (i >= 2 * fractal->Cpara.iterA && i < fractal->Cpara.iterB + tempL)
					{
						paraIt = tempB - fabs(tempB * i);
						paraAdd = paraIt * paraIt * curve2;
					}
				}
				para += paraAdd;
			}
		}
	}
	// Parabolic
	// double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (i * fractal->Cpara.parabSlope)
								+ (i * i * 0.001 * fractal->Cpara.parabScale);
		para += paraAddP0;
	}

	// using the parameter
	// z *= 1.0 + para / -z.Dot(z);

	if (fractal->transformCommon.functionEnabledFalse)
	{
		paraDot = fractal->transformCommon.offset0;
		para += paraDot;
	}

	z *= 1.0 + para / -z.Dot(z);
	// post scale
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	aux.r_dz *= fabs(fractal->transformCommon.scale);

	aux.DE =
		aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0; // DE tweak  or aux.DE +=
}

/**
 * spherical pwr fold
 */
void TransformSphericalPwrFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	// if (z.x > -1e-21 && z.x < 1e-21) z.x = (z.x > 0) ? 1e-21 : -1e-21;
	// if (z.y > -1e-21 && z.y < 1e-21) z.y = (z.y > 0) ? 1e-21 : -1e-21;
	// if (z.z > -1e-21 && z.z < 1e-21) z.z = (z.z > 0) ? 1e-21 : -1e-21;
	double r2 = pow(pow(z.x, fractal->transformCommon.pwr4) + pow(z.y, fractal->transformCommon.pwr4)
										+ pow(z.z, fractal->transformCommon.pwr4),
		fractal->transformCommon.pwr05);

	// if (r2 < 1e-21 && r2 > -1e-21)
	//	r2 = (r2 > 0) ? 1e-21 : -1e-21;
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
 * Surf Box Fold
 * allows different fold types for each axis.
 * amazing surface if one axis has no folds enabled.
 * made from various folds found in the posts at fractalforums.com
 */
void TransformSurfBoxFoldIteration(CVector3 &z, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->surfBox.enabledX1)
	{
		z.x =
			fabs(z.x + fractal->surfBox.offset1A111.x) - fabs(z.x - fractal->surfBox.offset1A111.x) - z.x;
	}
	if (fractal->surfBox.enabledX2False)
	{ // z = fold - fabs( fabs(z) - fold)
		z.x = fractal->surfBox.offset1B111.x - fabs(fabs(z.x) - fractal->surfBox.offset1B111.x);
	}
	if (fractal->surfBox.enabledX3False)
	{
		z.x = fabs(z.x + fractal->surfBox.offset2A111.x);
	}
	if (fractal->surfBox.enabledX4False)
	{ // if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
		if (z.x > fractal->surfBox.offset2B111.x)
		{
			z.x = fractal->surfBox.offset1A222.x - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		else if (z.x < -fractal->surfBox.offset2B111.x)
		{
			z.x = -fractal->surfBox.offset1A222.x - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
	}
	if (fractal->surfBox.enabledX5False)
	{ // z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
		z.x = fractal->surfBox.offset1B222.x
					- fabs(fabs(z.x + fractal->surfBox.offset3A111.x) - fractal->surfBox.offset1B222.x)
					- fractal->surfBox.offset3A111.x;
	}

	if (fractal->surfBox.enabledY1)
	{
		z.y =
			fabs(z.y + fractal->surfBox.offset1A111.y) - fabs(z.y - fractal->surfBox.offset1A111.y) - z.y;
	}
	if (fractal->surfBox.enabledY2False)
	{ // z = fold - fabs( fabs(z) - fold)
		z.y = fractal->surfBox.offset1B111.y - fabs(fabs(z.y) - fractal->surfBox.offset1B111.y);
	}
	if (fractal->surfBox.enabledY3False)
	{
		z.y = fabs(z.y + fractal->surfBox.offset2A111.y);
	}
	if (fractal->surfBox.enabledY4False)
	{ // if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
		if (z.y > fractal->surfBox.offset2B111.y)
		{
			z.y = fractal->surfBox.offset1A222.y - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		else if (z.y < -fractal->surfBox.offset2B111.y)
		{
			z.y = -fractal->surfBox.offset1A222.y - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
	}
	if (fractal->surfBox.enabledY5False)
	{ // z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
		z.y = fractal->surfBox.offset1B222.y
					- fabs(fabs(z.y + fractal->surfBox.offset3A111.y) - fractal->surfBox.offset1B222.y)
					- fractal->surfBox.offset3A111.y;
	}

	if (fractal->surfBox.enabledZ1)
	{
		z.z =
			fabs(z.z + fractal->surfBox.offset1A111.z) - fabs(z.z - fractal->surfBox.offset1A111.z) - z.z;
	}
	if (fractal->surfBox.enabledZ2False)
	{ // z = fold - fabs( fabs(z) - fold)
		z.z = fractal->surfBox.offset1B111.z - fabs(fabs(z.z) - fractal->surfBox.offset1B111.z);
	}
	if (fractal->surfBox.enabledZ3False)
	{
		z.z = fabs(z.z + fractal->surfBox.offset2A111.z);
	}

	if (fractal->surfBox.enabledZ4False)
	{ // if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
		double limit = fractal->surfBox.offset2B111.z;
		double value = fractal->surfBox.offset1A222.z;
		double zLimit = limit * fractal->surfBox.scale1Z1;
		double zValue = value * fractal->surfBox.scale1Z1;
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

	if (fractal->surfBox.enabledZ5False)
	{ // z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
		z.z = fractal->surfBox.offset1B222.z
					- fabs(fabs(z.z + fractal->surfBox.offset3A111.z) - fractal->surfBox.offset1B222.z)
					- fractal->surfBox.offset3A111.z;
	}
	aux.DE *= fractal->analyticDE.scale1; // tweak
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

	if (fractal->transformCommon.functionEnabledAyFalse)
	{ // z = fold - fabs( fabs(z) - fold)
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

	if (fractal->transformCommon.functionEnabledxFalse)
	{ // if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
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

	if (fractal->transformCommon.functionEnabledyFalse)
	{ // z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
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

/**
 * z vector - axis swap
 */
void TransformZvectorAxisSwapIteration(CVector3 &z, int i, const cFractal *fractal)
{
	switch (fractal->mandelbulbMulti.orderOfxyz)
	{
		case sFractalMandelbulbMulti::xyz:
		default: z = CVector3(z.x, z.y, z.z); break;
		case sFractalMandelbulbMulti::xzy: z = CVector3(z.x, z.z, z.y); break;
		case sFractalMandelbulbMulti::yxz: z = CVector3(z.y, z.x, z.z); break;
		case sFractalMandelbulbMulti::yzx: z = CVector3(z.y, z.z, z.x); break;
		case sFractalMandelbulbMulti::zxy: z = CVector3(z.z, z.x, z.y); break;
		case sFractalMandelbulbMulti::zyx: z = CVector3(z.z, z.y, z.x); break;
	}
	if (fractal->transformCommon.functionEnabledxFalse) z.x = -z.x;
	if (fractal->transformCommon.functionEnabledyFalse) z.y = -z.y;
	if (fractal->transformCommon.functionEnabledzFalse) z.z = -z.z;

	if (fractal->transformCommon.functionEnabled && i >= fractal->transformCommon.startIterations
			&& i < fractal->transformCommon.stopIterations)
	{
		double xtemp = SQRT_1_2 * (z.x - z.y);
		z.y = SQRT_1_2 * (z.y + z.x);
		z.x = xtemp;
	}
}

//-------------------------------- 4D ----------------------------------------------

/**
 * Formula based on Mandelbox (ABox). Extended to 4 dimensions
 */
void Abox4DIteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (i * fractal->Cpara.parabSlope)
								+ (i * i * 0.001 * fractal->Cpara.parabScale);
		z4D.w += paraAddP0;
	}

	CVector4 oldz = z4D;
	z4D.x = fabs(z4D.x + fractal->transformCommon.offset1111.x)
					- fabs(z4D.x - fractal->transformCommon.offset1111.x) - z4D.x;
	z4D.y = fabs(z4D.y + fractal->transformCommon.offset1111.y)
					- fabs(z4D.y - fractal->transformCommon.offset1111.y) - z4D.y;
	z4D.z = fabs(z4D.z + fractal->transformCommon.offset1111.z)
					- fabs(z4D.z - fractal->transformCommon.offset1111.z) - z4D.z;
	z4D.w = fabs(z4D.w + fractal->transformCommon.offset1111.w)
					- fabs(z4D.w - fractal->transformCommon.offset1111.w) - z4D.w;

	if (z4D.x != oldz.x) aux.color += fractal->mandelbox.color.factor4D.x;
	if (z4D.y != oldz.y) aux.color += fractal->mandelbox.color.factor4D.y;
	if (z4D.z != oldz.z) aux.color += fractal->mandelbox.color.factor4D.z;
	if (z4D.w != oldz.w) aux.color += fractal->mandelbox.color.factor4D.w;

	double rr = pow(
		z4D.x * z4D.x + z4D.y * z4D.y + z4D.z * z4D.z + z4D.w * z4D.w, fractal->mandelboxVary4D.rPower);

	z4D += fractal->transformCommon.offset0000;
	if (rr < fractal->transformCommon.minR2p25)
	{
		z4D *= fractal->transformCommon.maxMinR2factor;
		aux.DE *= fractal->transformCommon.maxMinR2factor;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		z4D *= fractal->transformCommon.maxR2d1 / rr;
		aux.DE *= fractal->transformCommon.maxR2d1 / rr;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
	z4D -= fractal->transformCommon.offset0000;

	z4D *= fractal->mandelbox.scale;
	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z4D;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z4D.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z4D.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z4D;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z4D.y = tp.y * cos(beta) + tp.z * sin(beta);
			z4D.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z4D;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z4D.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z4D.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z4D;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z4D.x = tp.x * cos(delta) + tp.w * sin(delta);
			z4D.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z4D;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z4D.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z4D.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z4D;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z4D.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z4D.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
	z4D += fractal->transformCommon.additionConstant0000;
}

/**
 * Bristorbrot formula 4D
 * @reference https://code.google.com/archive/p/fractaldimension/
 * by Doug Bristor
 */
void Bristorbrot4DIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux)
{
	aux.r_dz = aux.r_dz * 2.0 * aux.r;
	double newx = z4D.x * z4D.x - z4D.y * z4D.y - z4D.z * z4D.z - z4D.w * z4D.w;
	double newy = z4D.y * (2.0 * z4D.x - z4D.z - z4D.w);
	double newz = z4D.z * (2.0 * z4D.x + z4D.y - z4D.w);
	double neww = z4D.w * (2.0 * z4D.x + z4D.y + z4D.z);
	z4D.x = newx;
	z4D.y = newy;
	z4D.z = newz;
	z4D.w = neww;

	z4D += fractal->transformCommon.additionConstant0000;
}

/**
 * from Syntopia & Darkbeams Menger4 code from M3D
 */
void Menger4DIteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux)
{
	if (i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC)
	{
		z4D += fractal->transformCommon.additionConstant0000; // offset
	}

	z4D = fabs(z4D);
	if (z4D.x - z4D.y < 0.0) swap(z4D.y, z4D.x);
	if (z4D.x - z4D.z < 0.0) swap(z4D.z, z4D.x);
	if (z4D.y - z4D.z < 0.0) swap(z4D.z, z4D.y);
	if (z4D.x - z4D.w < 0.0) swap(z4D.w, z4D.x);
	if (z4D.y - z4D.w < 0.0) swap(z4D.w, z4D.y);
	if (z4D.z - z4D.w < 0.0) swap(z4D.w, z4D.z);

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z4D;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z4D.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z4D.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z4D;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z4D.y = tp.y * cos(beta) + tp.z * sin(beta);
			z4D.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z4D;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z4D.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z4D.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z4D;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z4D.x = tp.x * cos(delta) + tp.w * sin(delta);
			z4D.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z4D;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z4D.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z4D.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z4D;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z4D.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z4D.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}

	double scaleM = fractal->transformCommon.scale3;
	CVector4 offsetM = fractal->transformCommon.additionConstant111d5;
	z4D.x = scaleM * z4D.x - offsetM.x;
	z4D.y = scaleM * z4D.y - offsetM.y;
	z4D.w = scaleM * z4D.w - offsetM.w;
	z4D.z -= 0.5 * offsetM.z / scaleM;
	z4D.z = -fabs(-z4D.z);
	z4D.z += 0.5 * offsetM.z / scaleM;
	z4D.z *= scaleM;
	aux.DE *= scaleM;

	if (fractal->transformCommon.functionEnabledSFalse
			&& i >= fractal->transformCommon.startIterationsS
			&& i < fractal->transformCommon.stopIterationsS)
	{
		double r2 = z4D.Dot(z4D);
		// if (r2 < 1e-21 && r2 > -1e-21) r2 = (r2 > 0) ? 1e-21 : -1e-21;

		if (r2 < fractal->mandelbox.mR2)
		{
			z4D *= fractal->transformCommon.maxMinR2factor;
			aux.DE *= fractal->transformCommon.maxMinR2factor;
			aux.color += fractal->mandelbox.color.factorSp1;
		}
		else if (r2 < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / r2;
			z4D *= tglad_factor2;
			double scaleM = fractal->transformCommon.scale2;
			CVector4 offsetM = fractal->transformCommon.additionConstant111d5;
			z4D.x = scaleM * z4D.x - offsetM.x * (scaleM - 1.0);
			z4D.y = scaleM * z4D.y - offsetM.y * (scaleM - 1.0);
			z4D.w = scaleM * z4D.w - offsetM.w * (scaleM - 1.0);
			z4D.z -= 0.5 * offsetM.z * (scaleM - 1.0) / scaleM;
			z4D.z = -fabs(-z4D.z);
			z4D.z += 0.5 * offsetM.z * (scaleM - 1.0) / scaleM;
			z4D.z = scaleM * z4D.z;
			aux.DE *= scaleM * fractal->analyticDE.scale1;

			aux.DE *= tglad_factor2;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector4 zA4 = (i == fractal->transformCommon.intA) ? z4D : CVector4();
		CVector4 zB4 = (i == fractal->transformCommon.intB) ? z4D : CVector4();

		z4D = (z4D * fractal->transformCommon.scale) + (zA4 * fractal->transformCommon.offset)
					+ (zB4 * fractal->transformCommon.offset0);
		aux.DE *= fractal->transformCommon.scale;
		aux.r_dz *= fractal->transformCommon.scale;
	}

	aux.DE *= fractal->analyticDE.scale1;
}

/**
 * Menger4D MOD1   from Syntopia & Darkbeams Menger4 code from M3D
 */
void Menger4Dmod1Iteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (i * fractal->Cpara.parabSlope)
								+ (i * i * 0.001 * fractal->Cpara.parabScale);
		z4D.w += paraAddP0;
	}

	if (i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC)
	{
		z4D += fractal->transformCommon.additionConstant0000; // offset
	}

	z4D = fabs(z4D);
	if (z4D.x - z4D.y < 0.0) swap(z4D.y, z4D.x);
	if (z4D.x - z4D.z < 0.0) swap(z4D.z, z4D.x);
	if (z4D.y - z4D.z < 0.0) swap(z4D.z, z4D.y);
	if (z4D.x - z4D.w < 0.0) swap(z4D.w, z4D.x);
	if (z4D.y - z4D.w < 0.0) swap(z4D.w, z4D.y);
	if (z4D.z - z4D.w < 0.0) swap(z4D.w, z4D.z);

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z4D;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z4D.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z4D.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z4D;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z4D.y = tp.y * cos(beta) + tp.z * sin(beta);
			z4D.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z4D;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z4D.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z4D.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z4D;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z4D.x = tp.x * cos(delta) + tp.w * sin(delta);
			z4D.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z4D;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z4D.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z4D.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z4D;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z4D.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z4D.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
	double scaleM = fractal->transformCommon.scale3;
	CVector4 offsetM = fractal->transformCommon.additionConstant111d5;
	z4D.x = scaleM * z4D.x - offsetM.x;
	z4D.y = scaleM * z4D.y - offsetM.y;
	z4D.w = scaleM * z4D.w - offsetM.w;
	if (fractal->transformCommon.functionEnabledz && i >= fractal->transformCommon.startIterationsM
			&& i < fractal->transformCommon.stopIterationsM)
	{
		z4D.z -= 0.5 * offsetM.z / scaleM;
		z4D.z = -fabs(-z4D.z);
		z4D.z += 0.5 * offsetM.z / scaleM;
	}
	else
	{
		z4D.w = scaleM * z4D.w - offsetM.w;
	}
	z4D.z *= scaleM;
	aux.DE *= scaleM;

	if (fractal->transformCommon.functionEnabledSFalse
			&& i >= fractal->transformCommon.startIterationsS
			&& i < fractal->transformCommon.stopIterationsS)
	{
		double r2 = 0.;
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			r2 = z4D.x * z4D.x + z4D.y * z4D.y;
		}
		if (fractal->transformCommon.functionEnabledByFalse)
		{
			r2 = z4D.x * z4D.x + z4D.y * z4D.y + z4D.z * z4D.z;
		}
		if (fractal->transformCommon.functionEnabledBz)
		//{	r2 = z4D.Dot(z4D) ;}
		{
			r2 = z4D.x * z4D.x + z4D.y * z4D.y + z4D.z * z4D.z + z4D.w * z4D.w;
		}
		// if (r2 < 1e-21 && r2 > -1e-21) r2 = (r2 > 0) ? 1e-21 : -1e-21;

		if (r2 < fractal->mandelbox.mR2)
		{
			z4D *= fractal->transformCommon.maxMinR2factor;
			aux.DE *= fractal->transformCommon.maxMinR2factor;
			aux.color += fractal->mandelbox.color.factorSp1;
		}
		else if (r2 < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / r2;
			z4D *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector4 zA4 = (i == fractal->transformCommon.intA) ? z4D : CVector4();
		CVector4 zB4 = (i == fractal->transformCommon.intB) ? z4D : CVector4();

		z4D = (z4D * fractal->transformCommon.scale) + (zA4 * fractal->transformCommon.offset)
					+ (zB4 * fractal->transformCommon.offset0);
		aux.DE *= fractal->transformCommon.scale;
	}

	aux.DE *= fractal->analyticDE.scale1;
}

/**
 * Darkbeams MixPinski4 from M3D
 *A strange but intriguing fractal, that mixes Sierpinski and Menger folds.
 *The amazing thing is that in 3D it does not work so well! LUCA GN 2011
 */
void MixPinski4DIteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux)
{
	if (i >= fractal->transformCommon.startIterationsS
			&& i < fractal->transformCommon.stopIterationsS)
	{
		double temp;
		if (z4D.x + z4D.y < 0.0)
		{
			temp = z4D.x;
			z4D.x = -z4D.y;
			z4D.y = -temp;
		}

		if (z4D.x + z4D.z < 0.0)
		{
			temp = z4D.x;
			z4D.x = -z4D.z;
			z4D.z = -temp;
		}

		if (z4D.y + z4D.z < 0.0)
		{
			temp = z4D.z;
			z4D.z = -z4D.y;
			z4D.y = -temp;
		}

		if (z4D.x + z4D.w < 0.0)
		{
			temp = z4D.x;
			z4D.x = -z4D.w;
			z4D.w = -temp;
		}

		if (z4D.y + z4D.w < 0.0)
		{
			temp = z4D.y;
			z4D.y = -z4D.w;
			z4D.w = -temp;
		}

		if (z4D.z + z4D.w < 0.0)
		{
			temp = z4D.z;
			z4D.z = -z4D.w;
			z4D.w = -temp;
		}
		z4D = z4D * fractal->transformCommon.scale1;
		z4D.x = z4D.x - (fractal->transformCommon.scale1 - 1.0); // ?

		aux.DE *= fractal->transformCommon.scale1;
	}

	if (i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC)
	{
		z4D += fractal->transformCommon.additionConstant0000; // offset
	}
	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z4D;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z4D.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z4D.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z4D;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z4D.y = tp.y * cos(beta) + tp.z * sin(beta);
			z4D.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z4D;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z4D.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z4D.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z4D;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z4D.x = tp.x * cos(delta) + tp.w * sin(delta);
			z4D.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z4D;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z4D.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z4D.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z4D;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z4D.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z4D.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}

	double scaleM = fractal->transformCommon.scale2;
	CVector4 offsetM = fractal->transformCommon.additionConstant111d5;
	z4D.x = scaleM * z4D.x - offsetM.x * (scaleM - 1.0);
	z4D.y = scaleM * z4D.y - offsetM.y * (scaleM - 1.0);
	z4D.w = scaleM * z4D.w - offsetM.w * (scaleM - 1.0);
	z4D.z -= 0.5 * offsetM.z * (scaleM - 1.0) / scaleM;
	z4D.z = -fabs(-z4D.z);
	z4D.z += 0.5 * offsetM.z * (scaleM - 1.0) / scaleM;
	z4D.z = scaleM * z4D.z;
	aux.DE *= scaleM * fractal->analyticDE.scale1;
}

/**
 * Sierpinski4D.from Syntopia & Darkbeams code

 */
void Sierpinski4DIteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux)
{

	double temp;
	if (z4D.x + z4D.y < 0.0)
	{
		temp = z4D.x;
		z4D.x = -z4D.y;
		z4D.y = -temp;
	}

	if (z4D.x + z4D.z < 0.0)
	{
		temp = z4D.x;
		z4D.x = -z4D.z;
		z4D.z = -temp;
	}

	if (z4D.y + z4D.z < 0.0)
	{
		temp = z4D.z;
		z4D.z = -z4D.y;
		z4D.y = -temp;
	}

	if (z4D.x + z4D.w < 0.0)
	{
		temp = z4D.x;
		z4D.x = -z4D.w;
		z4D.w = -temp;
	}

	if (z4D.y + z4D.w < 0.0)
	{
		temp = z4D.y;
		z4D.y = -z4D.w;
		z4D.w = -temp;
	}

	if (z4D.z + z4D.w < 0.0)
	{
		temp = z4D.z;
		z4D.z = -z4D.w;
		z4D.w = -temp;
	}

	z4D = z4D * fractal->transformCommon.scaleA2;
	aux.DE *= fractal->transformCommon.scaleA2;

	if (i >= fractal->transformCommon.startIterationsC
			&& i < fractal->transformCommon.stopIterationsC)
	{
		z4D -= fractal->transformCommon.offset1111; // neg offset
	}

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z4D;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z4D.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z4D.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z4D;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z4D.y = tp.y * cos(beta) + tp.z * sin(beta);
			z4D.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z4D;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z4D.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z4D.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z4D;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z4D.x = tp.x * cos(delta) + tp.w * sin(delta);
			z4D.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z4D;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z4D.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z4D.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z4D;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z4D.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z4D.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
	aux.DE *= fractal->analyticDE.scale1;
}

/**
 * Quaternion4D
 */
void Quaternion4DIteration(CVector4 &z4D, int i, const cFractal *fractal)
{
	z4D = CVector4(z4D.x * z4D.x - z4D.y * z4D.y - z4D.z * z4D.z - z4D.w * z4D.w, z4D.x * z4D.y,
		z4D.x * z4D.z, z4D.w);
	z4D *= fractal->transformCommon.constantMultiplier1220;
	z4D += fractal->transformCommon.additionConstant0000;

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z4D;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z4D.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z4D.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z4D;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z4D.y = tp.y * cos(beta) + tp.z * sin(beta);
			z4D.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z4D;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z4D.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z4D.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z4D;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z4D.x = tp.x * cos(delta) + tp.w * sin(delta);
			z4D.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z4D;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z4D.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z4D.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z4D;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z4D.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z4D.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
}

/**
 * Formula based on Mandelbox (ABox). Extended to 4 dimensions and with variable scale parameter.
 */
void MandelboxVaryScale4DIteration(CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux)
{
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (i * fractal->Cpara.parabSlope)
								+ (i * i * 0.001 * fractal->Cpara.parabScale);
		z4D.w += paraAddP0;
	}

	aux.actualScale =
		aux.actualScale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);
	CVector4 oldz = z4D;
	z4D.x = fabs(z4D.x + fractal->mandelboxVary4D.fold) - fabs(z4D.x - fractal->mandelboxVary4D.fold)
					- z4D.x;
	z4D.y = fabs(z4D.y + fractal->mandelboxVary4D.fold) - fabs(z4D.y - fractal->mandelboxVary4D.fold)
					- z4D.y;
	z4D.z = fabs(z4D.z + fractal->mandelboxVary4D.fold) - fabs(z4D.z - fractal->mandelboxVary4D.fold)
					- z4D.z;
	z4D.w = fabs(z4D.w + fractal->mandelboxVary4D.fold) - fabs(z4D.w - fractal->mandelboxVary4D.fold)
					- z4D.w;

	if (z4D.x != oldz.x) aux.color += fractal->mandelbox.color.factor4D.x;
	if (z4D.y != oldz.y) aux.color += fractal->mandelbox.color.factor4D.y;
	if (z4D.z != oldz.z) aux.color += fractal->mandelbox.color.factor4D.z;
	if (z4D.w != oldz.w) aux.color += fractal->mandelbox.color.factor4D.w;

	double rr = pow(
		z4D.x * z4D.x + z4D.y * z4D.y + z4D.z * z4D.z + z4D.w * z4D.w, fractal->mandelboxVary4D.rPower);
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
	aux.DE = aux.DE * fabs(m) + 1.0;
	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& i >= fractal->transformCommon.startIterationsR
			&& i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z4D;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z4D.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z4D.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z4D;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z4D.y = tp.y * cos(beta) + tp.z * sin(beta);
			z4D.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z4D;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z4D.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z4D.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z4D;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z4D.x = tp.x * cos(delta) + tp.w * sin(delta);
			z4D.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z4D;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z4D.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z4D.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z4D;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z4D.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z4D.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
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

	if (fractal->transformCommon.functionEnabledx) z4D.x = fabs(z4D.x);
	if (fractal->transformCommon.functionEnabledy) z4D.y = fabs(z4D.y);
	if (fractal->transformCommon.functionEnabledz) z4D.z = fabs(z4D.z);
	if (fractal->transformCommon.functionEnabled) z4D.w = fabs(z4D.w);
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
* fabs add conditional4D
*/
void TransformFabsAddConditional4DIteration(
	CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledx)
	{
		z4D.x = sign(z4D.x) * (fractal->transformCommon.offset1111.x - fabs(z4D.x)
														+ fabs(z4D.x) * fractal->transformCommon.additionConstant0000.x);
	}

	if (fractal->transformCommon.functionEnabledy)
	{
		z4D.y = sign(z4D.y) * (fractal->transformCommon.offset1111.y - fabs(z4D.y)
														+ fabs(z4D.y) * fractal->transformCommon.additionConstant0000.y);
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		z4D.z = sign(z4D.z) * (fractal->transformCommon.offset1111.z - fabs(z4D.z)
														+ fabs(z4D.z) * fractal->transformCommon.additionConstant0000.z);
	}

	if (fractal->transformCommon.functionEnabledw)
	{
		z4D.w = sign(z4D.w) * (fractal->transformCommon.offset1111.w - fabs(z4D.w)
														+ fabs(z4D.w) * fractal->transformCommon.additionConstant0000.w);
	}
	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}

/**
 * iteration weight 4D
 */
void TransformIterationWeight4DIteration(
	CVector4 &z4D, int i, const cFractal *fractal, sExtendedAux &aux)
{
	CVector4 zA4 = (i == fractal->transformCommon.intA) ? z4D : CVector4();
	CVector4 zB4 = (i == fractal->transformCommon.intB) ? z4D : CVector4();

	z4D = (z4D * fractal->transformCommon.scale) + (zA4 * fractal->transformCommon.offset)
				+ (zB4 * fractal->transformCommon.offset0);
	aux.DE *= fractal->transformCommon.scale;
	aux.r_dz *= fractal->transformCommon.scale;
}

/**
 * Reciprocal4D from M3D, Darkbeam's code
 */
void TransformReciprocal4DIteration(CVector4 &z4D, const cFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempZ = z4D;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.x = (1.0 / fractal->transformCommon.offset1111.x)
								- 1.0 / (fabs(z4D.x) + fractal->transformCommon.offset1111.x);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.x = (fractal->transformCommon.offsetA1111.x)
								- 1.0 / (fabs(z4D.x) + fractal->transformCommon.offset1111.x);

		tempZ.x += fabs(z4D.x) * fractal->transformCommon.additionConstant0000.x; // function slope
		z4D.x = sign(z4D.x) * tempZ.x;
	}

	if (fractal->transformCommon.functionEnabledy)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.y = (1.0 / fractal->transformCommon.offset1111.y)
								- 1.0 / (fabs(z4D.y) + fractal->transformCommon.offset1111.y);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.y = (fractal->transformCommon.offsetA1111.y)
								- 1.0 / (fabs(z4D.y) + fractal->transformCommon.offset1111.y);

		tempZ.y += fabs(z4D.y) * fractal->transformCommon.additionConstant0000.y;
		z4D.y = sign(z4D.y) * tempZ.y;
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.z = (1.0 / fractal->transformCommon.offset1111.z)
								- 1.0 / (fabs(z4D.z) + fractal->transformCommon.offset1111.z);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.z = (fractal->transformCommon.offsetA1111.z)
								- 1.0 / (fabs(z4D.z) + fractal->transformCommon.offset1111.z);

		tempZ.z += fabs(z4D.z) * fractal->transformCommon.additionConstant0000.z;
		z4D.z = sign(z4D.z) * tempZ.z;
	}

	if (fractal->transformCommon.functionEnabledw)
	{
		if (fractal->transformCommon.functionEnabledAw)
			tempZ.w = (1.0 / fractal->transformCommon.offset1111.w)
								- 1.0 / (fabs(z4D.w) + fractal->transformCommon.offset1111.w);

		if (fractal->transformCommon.functionEnabledAwFalse)
			tempZ.w = (fractal->transformCommon.offsetA1111.w)
								- 1.0 / (fabs(z4D.w) + fractal->transformCommon.offset1111.w);

		tempZ.z += fabs(z4D.w) * fractal->transformCommon.additionConstant0000.w;
		z4D.z = sign(z4D.w) * tempZ.w;
	}
	// aux.DE = aux.DE * l/L;
	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}

/**
 * rotation 4D
 */
void TransformRotation4DIteration(CVector4 &z4D, const cFractal *fractal)
// temp 4D rotation
{
	CVector4 tp;
	if (fractal->transformCommon.rotation44a.x != 0)
	{
		tp = z4D;
		double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
		z4D.x = tp.x * cos(alpha) + tp.y * sin(alpha);
		z4D.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
	}
	if (fractal->transformCommon.rotation44a.y != 0)
	{
		tp = z4D;
		double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
		z4D.y = tp.y * cos(beta) + tp.z * sin(beta);
		z4D.z = tp.y * -sin(beta) + tp.z * cos(beta);
	}
	if (fractal->transformCommon.rotation44a.z != 0)
	{
		tp = z4D;
		double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
		z4D.x = tp.x * cos(gamma) + tp.z * sin(gamma);
		z4D.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
	}
	if (fractal->transformCommon.rotation44b.x != 0)
	{
		tp = z4D;
		double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
		z4D.x = tp.x * cos(delta) + tp.w * sin(delta);
		z4D.w = tp.x * -sin(delta) + tp.w * cos(delta);
	}
	if (fractal->transformCommon.rotation44b.y != 0)
	{
		tp = z4D;
		double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
		z4D.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
		z4D.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
	}
	if (fractal->transformCommon.rotation44b.z != 0)
	{
		tp = z4D;
		double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
		z4D.z = tp.z * cos(zeta) + tp.w * sin(zeta);
		z4D.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
	}
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
	// double r2 = z4D.Dot(z4D);
	double r2 = z4D.x * z4D.x + z4D.y * z4D.y;
	// if (r2 < 1e-21 && r2 > -1e-21) r2 = (r2 > 0) ? 1e-21 : -1e-21;
	r2 += z4D.z * z4D.z;
	r2 += z4D.w * z4D.w;
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
