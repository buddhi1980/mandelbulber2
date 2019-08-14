/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include "common_params.hpp"
#include "fractal.h"

using namespace fractal;
using std::swap;

/**
 * Classic Mandelbulb fractal.
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void MandelbulbIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// if (aux.r < 1e-21) aux.r = 1e-21;
	const double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	const double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	const double th = th0 * fractal->bulb.power;
	const double ph = ph0 * fractal->bulb.power;
	const double cth = cos(th);
	aux.DE = (rp * aux.DE) * fractal->bulb.power + 1.0;
	rp *= aux.r;
	z.x = cth * cos(ph) * rp;
	z.y = cth * sin(ph) * rp;
	z.z = sin(th) * rp;
}

/**
 * Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 * This formula contains aux.color
 */
void MandelboxIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->mandelbox.rotationsEnabled)
	{
		CVector4 zRot;
		// cast vector to array pointer for address taking of components in opencl
		double *zRotP = reinterpret_cast<double *>(&zRot);
		const double *colP = reinterpret_cast<const double *>(&fractal->mandelbox.color.factor);
		for (int dim = 0; dim < 3; dim++)
		{
			// handle each dimension x, y and z sequentially in pointer var dim
			double *rotDim = (dim == 0) ? &zRotP[0] : ((dim == 1) ? &zRotP[1] : &zRotP[2]);
			const double *colorFactor = (dim == 0) ? &colP[0] : ((dim == 1) ? &colP[1] : &colP[2]);

			zRot = fractal->mandelbox.rot[0][dim].RotateVector(z);
			if (*rotDim > fractal->mandelbox.foldingLimit)
			{
				*rotDim = fractal->mandelbox.foldingValue - *rotDim;
				z = fractal->mandelbox.rotinv[0][dim].RotateVector(zRot);
				aux.color += *colorFactor;
			}
			else
			{
				zRot = fractal->mandelbox.rot[1][dim].RotateVector(z);
				if (*rotDim < -fractal->mandelbox.foldingLimit)
				{
					*rotDim = -fractal->mandelbox.foldingValue - *rotDim;
					z = fractal->mandelbox.rotinv[1][dim].RotateVector(zRot);
					aux.color += *colorFactor;
				}
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

	const double r2 = z.Dot(z);

	z += fractal->mandelbox.offset;

	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		const double tglad_factor2 = fractal->mandelbox.fR2 / r2;
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
void Mandelbulb2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	aux.DE = aux.DE * 2.0 * aux.r;

	double tempR = sqrt(z.x * z.x + z.y * z.y); //+ 1e-061
	z *= 1.0 / tempR;
	double temp = z.x * z.x - z.y * z.y;
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

	z *= aux.r;
}

/**
 * Fractal formula created by Buddhi
 */
void Mandelbulb3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	aux.DE = aux.DE * 2.0 * aux.r;

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

	z *= aux.r;
}

/**
 * Fractal formula created by Buddhi
 */
void Mandelbulb4Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;

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
 * 3D Version of the 2D Eye Fractal created by biberino, modified mclarekin
 */
void MandelbulbEyeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r;

	// pre abs
	if (fractal->buffalo.preabsx) z.x = fabs(z.x);
	if (fractal->buffalo.preabsy) z.y = fabs(z.y);
	if (fractal->buffalo.preabsz) z.z = fabs(z.z);

	// bulb
	CVector4 zz = z * z;
	double rr = zz.x + zz.y + zz.z;
	double temp = sqrt(zz.x + zz.y);
	double theta1 = atan2(temp, z.z) * fractal->transformCommon.scaleB1;
	double theta2 = atan2(temp, -z.z) * fractal->transformCommon.scaleC1;
	temp = theta1 + theta2;

	double phi1 = atan2(z.y, z.x);
	double phi2 = 0.0;
	if (!fractal->transformCommon.functionEnabledFalse)
		phi2 = atan2(-z.y, z.x);
	else
		phi2 = M_PI - phi1;
	phi1 *= fractal->transformCommon.scale1;
	phi1 += phi2;

	temp = rr * sin(theta1 + theta2);
	z.x = temp * cos(phi1);
	z.y = temp * sin(phi1);
	z.z = rr * cos(theta1 + theta2);

	// post abs
	z.x = fractal->buffalo.absx ? fabs(z.x) : z.x;
	z.y = fractal->buffalo.absy ? fabs(z.y) : z.y;
	z.z = fractal->buffalo.absz ? fabs(z.z) : z.z;

	// offset
	z += fractal->transformCommon.additionConstantA000;

	// analyticDE controls
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}

/**
 * Classic Mandelbulb Power 2 fractal
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void MandelbulbPower2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	aux.DE = aux.DE * 2.0 * aux.r;
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
void XenodreambuieIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;
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
void MengerSpongeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	z.x = fabs(z.x);
	z.y = fabs(z.y);
	z.z = fabs(z.z);

	if (z.x - z.y < 0.0) swap(z.x, z.y);
	if (z.x - z.z < 0.0) swap(z.x, z.z);
	if (z.y - z.z < 0.0) swap(z.y, z.z);

	z *= fractal->transformCommon.scale3;

	z.x -= 2.0;
	z.y -= 2.0;
	if (z.z > 1.0) z.z -= 2.0;

	aux.DE *= fractal->transformCommon.scale3;
}

/**
 * Menger Sponge formula created by Knighty,
 * Modulus Operation applied by mancoast
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/escape-time-algorithm-defined-using-the-quotient-and-remainder-functions/
 */
void ModulusMengerSpongeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	z.x = fabs(z.x);
	z.y = fabs(z.y);
	z.z = fabs(z.z);

	if (z.x - z.y < 0.0) swap(z.x, z.y);
	if (z.x - z.z < 0.0) swap(z.x, z.z);
	if (z.y - z.z < 0.0) swap(z.y, z.z);

	z *= fractal->transformCommon.scale3;

	z.x -= 2.0;
	z.y -= 2.0;

	if (fmod(z.z, M_PI) > 2.0) z.z -= 2.0;

	aux.DE *= fractal->transformCommon.scale3;
}

/**
 * Classic Mandelbulb fractal with a Modulus Operation applied by mancoast
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/escape-time-algorithm-defined-using-the-quotient-and-remainder-functions/
 */
void ModulusMandelbulbIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// if (aux.r < 1e-21) aux.r = 1e-21;
	const double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	const double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	const double th = th0 * fractal->bulb.power;
	const double ph = ph0 * fractal->bulb.power;
	const double cth = cos(th);
	aux.DE = (rp * aux.DE) * fractal->bulb.power + 1.0;
	rp *= aux.r;
	z.x = cth * cos(ph) * rp;
	z.y = fmod(cth * sin(ph) * rp, 2.0);
	z.z = sin(th) * rp;
}

/**
 * Smooth Mandelbox created by Buddhi
 * @reference http://www.fractalforums.com/3d-fractal-generation/mandelbox-with-'smooth'-conditions/
 * This formula contains aux.color
 */
void MandelboxSmoothIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
void BoxFoldBulbPow2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	(void)aux;
	if (fabs(z.x) > fractal->foldingIntPow.foldFactor)
		z.x = sign(z.x) * fractal->foldingIntPow.foldFactor * 2.0 - z.x;
	if (fabs(z.y) > fractal->foldingIntPow.foldFactor)
		z.y = sign(z.y) * fractal->foldingIntPow.foldFactor * 2.0 - z.y;
	if (fabs(z.z) > fractal->foldingIntPow.foldFactor)
		z.z = sign(z.z) * fractal->foldingIntPow.foldFactor * 2.0 - z.z;

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
	CVector4 zTemp;
	zTemp.x = (x2 - y2) * temp;
	zTemp.y = 2.0 * z.x * z.y * temp;
	zTemp.z = -2.0 * z.z * sqrt(x2 + y2);
	zTemp.w = z.w;
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
void KaleidoscopicIfsIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
	aux.DE *= fabs(fractal->IFS.scale);
}

/**
 * Aexion's Quadray Sets from FractalForums
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/quadray-sets/msg31458/#msg31458
 */
void AexionIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i == 0)
	{
		double cx = fabs(aux.c.x + aux.c.y + aux.c.z) + fractal->aexion.cadd;
		double cy = fabs(-aux.c.x - aux.c.y + aux.c.z) + fractal->aexion.cadd;
		double cz = fabs(-aux.c.x + aux.c.y - aux.c.z) + fractal->aexion.cadd;
		double cw = fabs(aux.c.x - aux.c.y - aux.c.z) + fractal->aexion.cadd;
		aux.c.x = cx;
		aux.c.y = cy;
		aux.c.z = cz;
		aux.cw = cw;
		double tempX = fabs(z.x + z.y + z.z) + fractal->aexion.cadd;
		double tempY = fabs(-z.x - z.y + z.z) + fractal->aexion.cadd;
		double tempZ = fabs(-z.x + z.y - z.z) + fractal->aexion.cadd;
		double tempW = fabs(z.x - z.y - z.z) + fractal->aexion.cadd;
		z.x = tempX;
		z.y = tempY;
		z.z = tempZ;
		z.w = tempW;
	}
	double tempX = z.x * z.x - z.y * z.y + 2.0 * z.w * z.z + aux.c.x;
	double tempY = z.y * z.y - z.x * z.x + 2.0 * z.w * z.z + aux.c.y;
	double tempZ = z.z * z.z - z.w * z.w + 2.0 * z.x * z.y + aux.c.z;
	double tempW = z.w * z.w - z.z * z.z + 2.0 * z.x * z.y + aux.cw;
	z.x = tempX;
	z.y = tempY;
	z.z = tempZ;
	z.w = tempW;
}

/**
 * 3D Mandelbrot formula invented by David Makin
 * http://www.fractalgallery.co.uk/ and https://www.facebook.com/david.makin.7
 * @reference
 *
 http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/msg7235/#msg7235

 */
void HypercomplexIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	aux.DE = aux.DE * 2.0 * aux.r;
	double newx = z.x * z.x - z.y * z.y - z.z * z.z - z.w * z.w;
	double newy = 2.0 * z.x * z.y - 2.0 * z.w * z.z;
	double newz = 2.0 * z.x * z.z - 2.0 * z.y * z.w;
	double neww = 2.0 * z.x * z.w - 2.0 * z.y * z.z;
	z.x = newx;
	z.y = newy;
	z.z = newz;
	z.w = neww;
}

/**
 * Quaternion fractal
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void QuaternionIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	aux.DE = aux.DE * 2.0 * aux.r;
	double newx = z.x * z.x - z.y * z.y - z.z * z.z - z.w * z.w;
	double newy = 2.0 * z.x * z.y;
	double newz = 2.0 * z.x * z.z;
	double neww = 2.0 * z.x * z.w;
	z.x = newx;
	z.y = newy;
	z.z = newz;
	z.w = neww;
}

/**
 * Formula invented by Benesi
 * @reference http://www.fractalforums.com/index.php?action=profile;u=1170
 */
void BenesiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	CVector4 c = aux.const_c;
	aux.DE = aux.DE * 2.0 * aux.r;
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
void BristorbrotIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	aux.DE = aux.DE * 2.0 * aux.r;
	double newx = z.x * z.x - z.y * z.y - z.z * z.z;
	double newy = z.y * (2.0 * z.x - z.z);
	double newz = z.z * (2.0 * z.x + z.y);
	z.x = newx;
	z.y = newy;
	z.z = newz;
}

/**
 * Bristorbrot V2 formula
 * @reference http://www.fractalforums.com/theory/bristorbrot-3d/
 * by Doug Bristor
 */
void Bristorbrot2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r;

	CVector4 zOrig = z;
	CVector4 zz = z * z;
	CVector4 zNew = z;

	// pre abs
	CVector4 zFabs = fabs(z);
	if (fractal->buffalo.preabsx) zOrig.x = zFabs.x;
	if (fractal->buffalo.preabsy) zOrig.y = zFabs.y;
	if (fractal->buffalo.preabsz) zOrig.z = zFabs.z;

	// Bristorbrot V2 formula
	double signT = 1.0; // signT as "sign" is an operation   sign()

	// conditional operation which can create cuts
	if (fractal->transformCommon.functionEnabledFalse && zz.z >= zz.y)
		signT = -1.0; // creates fractal surface modification 2019

	// is preabs is used on z.x, it will be !< 0.0
	if (!fractal->buffalo.preabsy && fractal->transformCommon.functionEnabledxFalse && zOrig.x < 0.0)
		signT = -signT;

	// flips signT but same as using -1.0 scales
	if (fractal->transformCommon.functionEnabledAwFalse) signT = -signT;

	double tmpy = z.y;
	if (fractal->transformCommon.functionEnabledyFalse) tmpy = zFabs.y;
	double tmpz = z.z;
	if (fractal->transformCommon.functionEnabledzFalse) tmpz = zFabs.z;

	zNew.x = zz.x - zz.y - zz.z;
	zNew.y = zOrig.y * (zOrig.x * 2.0 - tmpz * signT * fractal->transformCommon.scaleB1);
	zNew.z = zOrig.z * (zOrig.x * 2.0 + tmpy * signT * fractal->transformCommon.scaleC1);
	z = zNew;

	// post abs
	z.x = fractal->buffalo.absx ? fabs(z.x) : z.x;
	z.y = fractal->buffalo.absy ? fabs(z.y) : z.y;
	z.z = fractal->buffalo.absz ? fabs(z.z) : z.z;

	// offset
	z += fractal->transformCommon.additionConstantA000;

	// analyticDE controls
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}

/**
 * From M3D. A formula made by Trafassel, the original Ide's Formula thread
 * @reference http://www.fractalforums.com/videos/formula-21-%28julia-set-interpretation%29/
 */
void IdesIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	if (fabs(z.x) < 2.5) z.x = z.x * .9;
	if (fabs(z.z) < 2.5) z.z = z.z * .9;

	CVector4 z2 = z * z;
	CVector4 newZ;
	newZ.x = fractal->transformCommon.constantMultiplier121.x * z2.x
					 - fractal->transformCommon.additionConstant0555.x * (z2.y + z2.z);
	newZ.y = fractal->transformCommon.constantMultiplier121.y * z.x * z.y * z.z;
	newZ.z = fractal->transformCommon.constantMultiplier121.z * z2.z
					 - fractal->transformCommon.additionConstant0555.z * (z2.x + z2.y);
	newZ.w = z.w;
	z = newZ;
}

/**
 * From M3D. A formula made by Trafassel, the original Ide's Formula thread
 * @reference http://www.fractalforums.com/videos/formula-21-%28julia-set-interpretation%29/
 */
void Ides2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	CVector4 z2 = z * z;
	CVector4 newZ;
	newZ.x = fractal->transformCommon.constantMultiplier121.x * z2.x
					 - fractal->transformCommon.additionConstant0555.x * (z2.y + z2.z);
	newZ.y = fractal->transformCommon.constantMultiplier121.y * z.x * z.y * z.z;
	newZ.z = fractal->transformCommon.constantMultiplier121.z * z2.z
					 - fractal->transformCommon.additionConstant0555.z * (z2.x + z2.y);
	newZ.w = z.w;
	z = newZ + z;
}

/**
 * Buffalo fractal
 * @reference /http://www.fractalforums.com/gallery-b177/buffalo-fractals
 */
void BuffaloIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r;

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
 * Fractal formula created by Robert Pancoast
 */
void CoastalbrotIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	double temp = aux.r;
	temp = pow(temp, 7.7);
	aux.DE = temp * aux.DE * 7.7;
	temp *= aux.r;

	z.x = sin(sin(sin(M_PI / 3 + z.x * M_PI)));
	z.y = sin(sin(sin(M_PI / 3 + z.y * M_PI)));
	z.z = sin(sin(sin(M_PI / 3 + z.z * M_PI)));

	z *= temp;
}

/**
 * From M3D
 * @reference http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/
 */
void QuickDudleyIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	Q_UNUSED(aux);

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
void QuickDudleyModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

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
void LkmitchIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	Q_UNUSED(aux);

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
 * http://www.fractalgallery.co.uk/ and https://www.facebook.com/david.makin.7
 * @reference http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/
 */
void Makin3d2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	Q_UNUSED(aux);

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
 * aBoxMod1, a variation of Mandelbox fractal known as AmazingBox or ABox,
 * invented by Tom Lowe in 2010, the variation by DarkBeam
 *
 * This formula has a different box fold to the standard Tglad fold
 * This formula has a c.x c.y SWAP (in compute_fractals.cpp)
 * In V2.11 minimum radius is MinimumR2, for settings made in
 * older versions, you need to use the square root of the old parameter.
 *
 * based on DarkBeam's Mandelbulb3D code
 *
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 * This formula contains aux.color and aux.actualScale
 */
void AboxMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	CVector4 c = aux.const_c;
	CVector4 oldZ = z;
	z.x = fractal->mandelbox.foldingValue
				- fabs(fabs(z.x + fractal->transformCommon.additionConstant000.x)
							 - fractal->mandelbox.foldingValue)
				- fabs(fractal->transformCommon.additionConstant000.x);
	z.y = fractal->mandelbox.foldingValue
				- fabs(fabs(z.y + fractal->transformCommon.additionConstant000.y)
							 - fractal->mandelbox.foldingValue)
				- fabs(fractal->transformCommon.additionConstant000.y);

	if (fractal->transformCommon.functionEnabledz)
	{
		z.z = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1
					- fabs(fabs(z.z + fractal->transformCommon.additionConstant000.z)
								 - fractal->mandelbox.foldingValue * fractal->transformCommon.scale1)
					- fabs(fractal->transformCommon.additionConstant000.z);
		if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor.z;
	}
	if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor.x;
	if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor.y;

	double rr = (z.x * z.x + z.y * z.y + z.z * z.z);
	if (fractal->transformCommon.functionEnabledFalse)
	{
		rr = pow(rr, fractal->mandelboxVary4D.rPower);
	}

	if (rr < fractal->transformCommon.minR0)
	{
		double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR0;
		z *= tglad_factor1;
		aux.DE *= tglad_factor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
	z *= aux.actualScale;
	aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;

	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}

	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
}

/**
 * aBoxMod2, a variation of the Mandelbox fractal known as AmazingBox or ABox,
 * invented by Tom Lowe in 2010, this variation by DarkBeam
 *
 * The formula Cylinder Half Size transform changes the spherical fold
 * In V2.11 minimum radius is MinimumR2, for settings made in
 * older versions, you need to use the square root of the old parameter.
 * V.12 added full Mbox color controls
 *
 * based on DarkBeam's Mandelbulb3D code
 *
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 * This formula contains aux.color and aux.actualScale
 */
void AboxMod2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	aux.actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);
	// Tglad Fold
	CVector4 oldZ = z;
	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
				- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
				- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	if (fractal->transformCommon.functionEnabledM)
	{
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
	}
	if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
	if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
	if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
	/*	double rr;
		if (temp > 0.0)
			rr = z.x * z.x + z.y * z.y + z.z * z.z; // on top & bottom of cyl. z.z should be tempZ
		else
			rr = z.x * z.x + z.y * z.y; // on cyl body*/
	// cylinder half size
	double tempZ = fabs(z.z) - fractal->transformCommon.offset05;
	double rr = z.x * z.x + z.y * z.y;
	if (tempZ > 0.0) rr = rr + (tempZ * tempZ * fractal->transformCommon.scale1);
	// rPower
	if (fractal->transformCommon.functionEnabledFalse)
	{
		rr = pow(rr, fractal->mandelboxVary4D.rPower);
	}
	// Spherical Fold
	if (rr < fractal->transformCommon.minR2p25)
	{
		z *= fractal->transformCommon.maxMinR2factor;
		aux.DE *= fractal->transformCommon.maxMinR2factor;
		colorAdd += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		colorAdd += fractal->mandelbox.color.factorSp2;
	}
	// Scale
	z *= aux.actualScale;
	aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	// rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	// color updated v2.14
	if (fractal->foldColor.auxColorEnabled)
	{
		aux.color += colorAdd;
	}
}

/**
 * ABoxMod11,
 * The Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * Variations from DarkBeam, Buddhi, Eiffie and mclarekin
 * based on DarkBeam's Mandelbulb3D code
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 * This formula contains aux.color and aux.actualScale
 */
void AboxMod11Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	aux.actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);
	CVector4 c = aux.const_c;

	// tglad fold
	CVector4 oldZ = z;
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabled)
		{
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		}

		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
	}
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		CVector4 limit = fractal->transformCommon.additionConstant111;
		CVector4 length = 2.0 * limit;
		CVector4 tgladS = 1.0 / length;
		CVector4 Add = CVector4(0.0, 0.0, 0.0, 0.0);
		if (fabs(z.x) < limit.x) Add.x = z.x * z.x * tgladS.x;
		if (fabs(z.y) < limit.y) Add.y = z.y * z.y * tgladS.y;
		if (fabs(z.z) < limit.z) Add.z = z.z * z.z * tgladS.z;
		if (fabs(z.x) > limit.x && fabs(z.x) < length.x)
			Add.x = (length.x - fabs(z.x)) * (length.x - fabs(z.x)) * tgladS.x;
		if (fabs(z.y) > limit.y && fabs(z.y) < length.y)
			Add.y = (length.y - fabs(z.y)) * (length.y - fabs(z.y)) * tgladS.y;
		if (fabs(z.z) > limit.z && fabs(z.z) < length.z)
			Add.z = (length.z - fabs(z.z)) * (length.z - fabs(z.z)) * tgladS.z;
		Add *= fractal->transformCommon.scale3D000;
		z.x = (z.x - (sign(z.x) * (Add.x)));
		z.y = (z.y - (sign(z.y) * (Add.y)));
		z.z = (z.z - (sign(z.z) * (Add.z)));
	}
	// swap
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		z = CVector4(z.z, z.y, z.x, z.w);
	}

	// spherical fold
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
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
				if (aux.i < fractal->Cpara.iterA)
				{
					para = temp0 + (aux.i * grade1);
				}
				if (aux.i < fractal->Cpara.iterB && aux.i >= fractal->Cpara.iterA)
				{
					para = tempA + (aux.i - fractal->Cpara.iterA) * grade2;
				}
				if (aux.i >= fractal->Cpara.iterB)
				{
					para = tempB + (aux.i - fractal->Cpara.iterB) * grade3;
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
						if (aux.i < 2 * fractal->Cpara.iterA)
						{
							paraIt = tempA - fabs(tempA - aux.i);
							paraAdd = paraIt * paraIt * curve1;
						}
						if (aux.i >= 2 * fractal->Cpara.iterA && aux.i < fractal->Cpara.iterB + tempL)
						{
							paraIt = tempB - fabs(tempB * aux.i);
							paraAdd = paraIt * paraIt * curve2;
						}
						para += paraAdd;
					}
				}
			}
			paraAddP0 = 0.0;
			if (fractal->Cpara.enabledParabFalse)
			{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
				paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope)
										+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
			}
		}
		para += paraAddP0 + fractal->transformCommon.minR2p25;
		// spherical fold
		double rr = z.Dot(z);

		z += fractal->mandelbox.offset;

		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < para)
		{
			double tglad_factor1 = fractal->transformCommon.maxR2d1 / para;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
			colorAdd += fractal->mandelbox.color.factorSp1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			colorAdd += fractal->mandelbox.color.factorSp2;
		}
		z -= fractal->mandelbox.offset;
	}

	// scale, incl DarkBeam's Scale vary
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z *= aux.actualScale;

		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;
		else
			aux.DE =
				aux.DE * fabs(aux.actualScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
	// offset
	z += fractal->transformCommon.additionConstant000;

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	// rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	// tweak scale
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		z *= fractal->transformCommon.scaleG1;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scaleG1);
	}

	// color updated v2.14
	if (fractal->foldColor.auxColorEnabled)
	{
		aux.color += colorAdd;
	}
}

/**
 * ABoxMod12,
 * The Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * Variations from DarkBeam, Buddhi, Eiffie and mclarekin
 * based on DarkBeam's Mandelbulb3D code
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 * This formula contains aux.color and aux.actualScaleA
 */
void AboxMod12Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	// tglad fold
	CVector4 oldZ = z;
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabled)
		{
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		}
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
		}
	}
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		CVector4 limit = fractal->transformCommon.additionConstant111;
		CVector4 length = 2.0 * limit;
		CVector4 tgladS = 1.0 / length;
		CVector4 Add = CVector4(0.0, 0.0, 0.0, 0.0);
		if (fabs(z.x) < limit.x) Add.x = z.x * z.x * tgladS.x;
		if (fabs(z.y) < limit.y) Add.y = z.y * z.y * tgladS.y;
		if (fabs(z.z) < limit.z) Add.z = z.z * z.z * tgladS.z;
		if (fabs(z.x) > limit.x && fabs(z.x) < length.x)
			Add.x = (length.x - fabs(z.x)) * (length.x - fabs(z.x)) * tgladS.x;
		if (fabs(z.y) > limit.y && fabs(z.y) < length.y)
			Add.y = (length.y - fabs(z.y)) * (length.y - fabs(z.y)) * tgladS.y;
		if (fabs(z.z) > limit.z && fabs(z.z) < length.z)
			Add.z = (length.z - fabs(z.z)) * (length.z - fabs(z.z)) * tgladS.z;
		Add *= fractal->transformCommon.scale3D000;
		z.x = (z.x - (sign(z.x) * (Add.x)));
		z.y = (z.y - (sign(z.y) * (Add.y)));
		z.z = (z.z - (sign(z.z) * (Add.z)));
	}
	// swap
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		z = CVector4(z.z, z.y, z.x, z.w);
	}
	// spherical folds
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double minR2 = fractal->transformCommon.minR2p25;
		double addR = 0.0;
		double m = 1.0;
		oldZ = z;

		// spherical fold with xyz bias option
		if (fractal->transformCommon.functionEnabledAx)
		{
			if (aux.i >= fractal->transformCommon.startIterationsE
					&& aux.i < fractal->transformCommon.stopIterationsE)
			{
				CVector4 xyzBias = fractal->transformCommon.constantMultiplier000;
				if (fractal->transformCommon.functionEnabledAzFalse)
				{
					xyzBias *= aux.c * aux.c;
				}
				else
				{
					xyzBias *= fabs(aux.c);
				}
				addR = (xyzBias.x + xyzBias.y + xyzBias.z);
				minR2 += addR;
				if (fractal->transformCommon.functionEnabledAz && minR2 > fractal->transformCommon.maxR2d1)
				{ // stop overlapping potential
					minR2 = fractal->transformCommon.maxR2d1;
				}
			}
			double rr = z.Dot(z);
			z += fractal->transformCommon.offset000;
			if (rr < minR2)
			{
				m *= fractal->transformCommon.maxR2d1 / minR2;
				colorAdd += fractal->mandelbox.color.factorSp1;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				m *= fractal->transformCommon.maxR2d1 / rr;
				colorAdd += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->transformCommon.offset000;
			// scale
			z *= m;
			aux.DE *= fabs(m);
		}

		// cuboid
		if (fractal->transformCommon.functionEnabledAyFalse
				&& aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			CVector4 temp3;
			CVector4 R2;
			double MinR2 = fractal->transformCommon.minR2p25;
			CVector4 limitMinR2 = fractal->transformCommon.scaleP222;

			double rr = z.Dot(z);
			z += fractal->transformCommon.offset000;

			// if (aux.i >= fractal->transformCommon.startIterationsD
			//		&& aux.i < fractal->transformCommon.stopIterationsD)
			//{
			if (fractal->transformCommon.functionEnabledAxFalse)
				temp3 = z * z;
			else
				temp3 = fabs(z);

			if (temp3.x < limitMinR2.x && temp3.y < limitMinR2.y && temp3.z < limitMinR2.z)
			{ // if inside cuboid
				R2.x = limitMinR2.x / temp3.x;
				R2.y = limitMinR2.y / temp3.y;
				R2.z = limitMinR2.z / temp3.z;
				double First = min(R2.x, min(R2.y, R2.z));
				MinR2 = rr * First;

				// if (fractal->transformCommon.functionEnabled && MinR2 > fractal->transformCommon.maxR2d1)

				if (MinR2 > fractal->transformCommon.maxR2d1)
				{ // stop overlapping potential
					MinR2 = fractal->transformCommon.maxR2d1;
				}

				m *= fractal->transformCommon.maxR2d1 / MinR2;
				aux.color += fractal->mandelbox.color.factorSp1;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				m *= fractal->transformCommon.maxR2d1 / rr;
				aux.color += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->transformCommon.offset000;
			// scale
			z *= m;
			aux.DE *= fabs(m);
		}
		// if not use standard Mbox sphere fold

		if (z.x == oldZ.x && z.y == oldZ.y && z.z == oldZ.z && z.w == oldZ.w)
		{
			double MinR2 = fractal->transformCommon.minR2p25;
			double rr = z.Dot(z);
			z += fractal->transformCommon.offset000;
			if (rr < MinR2)
			{
				m *= fractal->transformCommon.maxR2d1 / MinR2;
				aux.color += fractal->mandelbox.color.factorSp1;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				m *= fractal->transformCommon.maxR2d1 / rr;
				aux.color += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->transformCommon.offset000;
			// scale
			z *= m;
			aux.DE *= fabs(m);
		}
	}

	double useScale = fractal->mandelbox.scale;
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		useScale += aux.actualScaleA;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;

		// update actualScale for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
		if (fractal->transformCommon.functionEnabledMFalse)
			aux.actualScaleA = -vary;
		else
			aux.actualScaleA = aux.actualScaleA - vary;
	}
	else
	{
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;
	}

	// offset
	z += fractal->transformCommon.additionConstant000;
	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	// rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	// color updated v2.13
	if (fractal->foldColor.auxColorEnabled)
	{
		aux.color += colorAdd;
	}
}

/**
 * ABoxMod13,
 * The Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * Variations from DarkBeam, Buddhi, Eiffie and mclarekin
 * based on DarkBeam's Mandelbulb3D code
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 * This formula contains aux.color, aux.pos_neg and aux.actualScaleA
 */
void AboxMod13Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	// invert c
	if (fractal->transformCommon.functionEnabledAzFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		if (fractal->transformCommon.functionEnabledAyFalse)
		{
			aux.c *= fractal->transformCommon.scale3D111;
			double rSqrL = aux.c.Dot(aux.c);
			rSqrL = 1.0 / rSqrL;
			aux.c *= rSqrL;
			z += aux.c;
		}
		else
		{
			c *= fractal->transformCommon.scale3D111;
			double rSqrL = c.Dot(c);
			rSqrL = 1.0 / rSqrL;
			z += c * rSqrL;
		}
	}

	// tglad fold
	CVector4 oldZ = z;
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabled)
		{
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		}
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
		}
	}
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		CVector4 limit = fractal->transformCommon.additionConstant111;
		CVector4 length = 2.0 * limit;
		CVector4 tgladS = 1.0 / length;
		CVector4 Add = CVector4(0.0, 0.0, 0.0, 0.0);
		if (fabs(z.x) < limit.x) Add.x = z.x * z.x * tgladS.x;
		if (fabs(z.y) < limit.y) Add.y = z.y * z.y * tgladS.y;
		if (fabs(z.z) < limit.z) Add.z = z.z * z.z * tgladS.z;
		if (fabs(z.x) > limit.x && fabs(z.x) < length.x)
			Add.x = (length.x - fabs(z.x)) * (length.x - fabs(z.x)) * tgladS.x;
		if (fabs(z.y) > limit.y && fabs(z.y) < length.y)
			Add.y = (length.y - fabs(z.y)) * (length.y - fabs(z.y)) * tgladS.y;
		if (fabs(z.z) > limit.z && fabs(z.z) < length.z)
			Add.z = (length.z - fabs(z.z)) * (length.z - fabs(z.z)) * tgladS.z;
		Add *= fractal->transformCommon.scale3D000;
		z.x = (z.x - (sign(z.x) * (Add.x)));
		z.y = (z.y - (sign(z.y) * (Add.y)));
		z.z = (z.z - (sign(z.z) * (Add.z)));
	}
	// swap
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		z = CVector4(z.z, z.y, z.x, z.w);
	}
	// spherical fold
	double rr = z.Dot(z);

	z += fractal->mandelbox.offset;

	// if (r2 < 1e-21) r2 = 1e-21;
	if (rr < fractal->transformCommon.minR2p25)
	{
		double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
		z *= tglad_factor1;
		aux.DE *= tglad_factor1;
		colorAdd += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		colorAdd += fractal->mandelbox.color.factorSp2;
	}
	z -= fractal->mandelbox.offset;

	double useScale = fractal->mandelbox.scale;
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		useScale += aux.actualScaleA;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;

		// update actualScale for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
		if (fractal->transformCommon.functionEnabledMFalse)
			aux.actualScaleA = -vary;
		else
			aux.actualScaleA = aux.actualScaleA - vary;
	}
	else
	{
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;
	}

	// offset
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		if (fractal->transformCommon.functionEnabledAxFalse)
		{
			CVector4 offsetAlt = aux.pos_neg * fractal->transformCommon.additionConstant000;
			z += offsetAlt;
			aux.pos_neg *= -1.0 * fractal->transformCommon.scale1;
		}
		else
		{
			z += fractal->transformCommon.additionConstant000;
		}
	}

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{

			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	// rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	// color updated v2.13
	if (fractal->foldColor.auxColorEnabled)
	{
		aux.color += colorAdd;
	}
}

/**
 * ABoxModKali, a formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/
 */
void AboxModKaliIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// 3D Rotation
	if (fractal->mandelbox.mainRotationEnabled)
	{
		CVector4 tempVC = CVector4(fractal->mandelbox.rotationMain, 0.0); // constant to be varied
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			if (aux.i >= fractal->transformCommon.startIterations
					&& aux.i < fractal->transformCommon.stopIterations
					&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations
							 != 0))
			{
				double iterationRange =
					fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations;
				double currentIteration = (aux.i - fractal->transformCommon.startIterations);
				tempVC += fractal->transformCommon.offset000 * currentIteration / iterationRange;
			}

			if (aux.i >= fractal->transformCommon.stopIterations)
			{
				tempVC += tempVC + fractal->transformCommon.offset000;
			}
		}

		tempVC *= M_PI_180;

		if (tempVC.x != 0.0) z = z.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), tempVC.x);
		if (tempVC.y != 0.0) z = z.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), tempVC.y);
		if (tempVC.z != 0.0) z = z.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), tempVC.z);
	}

	z = fractal->transformCommon.additionConstant0555 - fabs(z);
	double rr = z.Dot(z);
	double MinR = fractal->transformCommon.minR06;
	double dividend = rr < MinR ? MinR : min(rr, 1.0);
	double m = fractal->transformCommon.scale015 / dividend;
	z = z * m;
	aux.DE = aux.DE * fabs(m) + 1.0;
}

/**
 * ABoxModKaliEiffie, modified formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/
 * This formula contains aux.color
 */
void AboxModKaliEiffieIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	CVector4 oldZ = z;
	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
				- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
				- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	if (fractal->transformCommon.functionEnabledM)
	{
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
	}

	if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
	if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
	if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box fold z.z
	{
		double zLimit =
			fractal->transformCommon.additionConstant111.z * fractal->transformCommon.scale1;
		double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
			colorAdd += fractal->mandelbox.color.factor.z;
		}
	}

	if (fractal->transformCommon.functionEnabled)
	{
		z = CVector4(z.z, z.y, z.x, z.w); // swap
	}
	if (z.y > z.x) z = CVector4(z.y, z.x, z.z, z.w); // conditional

	double rr = z.Dot(z);
	double MinR = fractal->transformCommon.minR05;
	double m = fractal->transformCommon.scale015;
	if (rr < MinR)
	{
		m = m / MinR;
		colorAdd += fractal->mandelbox.color.factorSp1;
	}
	else
	{
		if (rr < 1.0)
		{
			m = m / rr;
			colorAdd += fractal->mandelbox.color.factorSp2;
		}
	}
	z = z * m;
	aux.DE = aux.DE * fabs(m) + 1.0;

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->foldColor.auxColorEnabled)
	{
		aux.color += colorAdd;
	}
}

/**
 * ABoxModKaliV2, a formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/
 */
void AboxModKaliV2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;

	// fold
	CVector4 oldZ = z;
	z.x = fractal->transformCommon.additionConstant0555.x - fabs(z.x);
	z.y = fractal->transformCommon.additionConstant0555.y - fabs(z.y);
	if (fractal->transformCommon.functionEnabledM)
	{
		z.z = fractal->transformCommon.additionConstant0555.z - fabs(z.z);
	}
	CVector4 postZ = z;

	// spherical fold & scaling
	double rr = z.Dot(z);
	// z += fractal->transformCommon.additionConstant000;
	double m = fractal->transformCommon.scale015;
	if (rr < fractal->transformCommon.minR2p25)
	{
		m *= fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
		colorAdd += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		m *= fractal->transformCommon.maxR2d1 / rr;
		colorAdd += fractal->mandelbox.color.factorSp2;
	}
	// z -= fractal->transformCommon.additionConstant000;
	z = z * m;
	aux.DE = aux.DE * fabs(m) + 1.0;

	z += fractal->transformCommon.additionConstant000;

	// rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	z += fractal->transformCommon.additionConstant000;

	// add cpixel symmetrical
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;

		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}

	// sign options
	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x *= sign(oldZ.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y *= sign(oldZ.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z *= sign(oldZ.z);
	}

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	// aux.color controls
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		postZ -= oldZ;
		if (postZ.x < 1.0) aux.color += fractal->mandelbox.color.factor.x;
		if (postZ.y < 1.0) aux.color += fractal->mandelbox.color.factor.y;
		if (postZ.z < 1.0) aux.color += fractal->mandelbox.color.factor.z;
		aux.color += colorAdd;
	}
}

/**
 * ABoxModKali V3, a formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/
 */
void AboxModKaliV3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterations15)
	{
		z = fractal->transformCommon.offset111 - fabs(z);
		double rr = z.Dot(z);
		double MinR = fractal->transformCommon.minR0;
		double dividend = rr < MinR ? MinR : min(rr, 1.0);
		double m = -fractal->transformCommon.scale015 / dividend;
		z = z * m;
		aux.DE = aux.DE * fabs(m) + 1.0;
	}

	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z += fractal->transformCommon.offsetA111;
	}

	// TODO add the options
}

/**
 * ABoxVS_icen1, a formula from Mandelbulb3D.
 * Inspired from a 2D formula proposed by Kali at Fractal Forums
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/kaliset-plus-boxfold-nice-new-2d-fractal/msg33670/#new
 * This formula contains aux.color and aux.actualScale
 */
void AboxVSIcen1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);
	CVector4 c = aux.const_c;
	CVector4 oldZ = z;
	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
				- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
				- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
				- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
	if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor.x;
	if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor.y;
	if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor.z;

	if (fractal->transformCommon.juliaMode)
	{
		z += c * fractal->transformCommon.constantMultiplier111;
	}
	else
	{
		z += fractal->transformCommon.juliaC;
	}

	double rr = z.Dot(z);
	// if (r2 < 1e-21 && r2 > -1e-21)
	//	r2 = (r2 > 0) ? 1e-21 : -1e-21;
	if (rr < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / rr;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}

	z *= aux.actualScale; // aux.actualScale;
	aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;

	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

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
 * ABoxSurfBox,
 * based on DarkBeam's code "Mixed fold by Luca 2016".
 * @reference
 * http:// http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/
 * httpwww-shaperich-comproshred-elite-review/
 * This formula contains aux.color and aux.actualScaleA
 */
void AboxSurfBoxIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;

	CVector4 c = aux.const_c;

	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		// tglad fold
		CVector4 foldMod = fractal->transformCommon.offset222;
		CVector4 fold = fractal->transformCommon.additionConstant111;
		CVector4 sg = z;
		sg.x = sign(z.x);
		sg.y = sign(z.y);
		sg.z = sign(z.z);

		CVector4 folder = z;
		CVector4 Tglad = z;

		Tglad.x = fabs(z.x + fold.x) - fabs(z.x - fold.x) - z.x;
		Tglad.y = fabs(z.y + fold.y) - fabs(z.y - fold.y) - z.y;

		folder.x = sg.x * (z.x - Tglad.x);
		folder.y = sg.y * (z.y - Tglad.y);
		folder.x = fabs(folder.x);
		folder.y = fabs(folder.y);
		folder.x = min(folder.x, foldMod.x);
		folder.y = min(folder.y, foldMod.y);
		z.x -= sg.x * folder.x;
		z.y -= sg.y * folder.y;

		if (fractal->transformCommon.functionEnabled)
		{
			Tglad.z = fabs(z.z + fold.z) - fabs(z.z - fold.z) - z.z;
			folder.z = sg.z * (z.z - Tglad.z);
			folder.z = fabs(folder.z);
			folder.z = min(folder.z, foldMod.z);
			z.z -= sg.z * folder.z;
		}
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
		zCol = z;
	}

	// swap
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		z = CVector4(z.z, z.y, z.x, z.w);
	}
	double rr = z.Dot(z);
	rrCol = rr;
	// spherical fold
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		// spherical fold
		rr = z.Dot(z);

		z += fractal->mandelbox.offset;

		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < fractal->transformCommon.minR2p25)
		{
			double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
		}
		z -= fractal->mandelbox.offset;
	}

	double useScale = fractal->mandelbox.scale;
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		useScale += aux.actualScaleA;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;

		// update actualScale for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
		if (fractal->transformCommon.functionEnabledMFalse)
			aux.actualScaleA = -vary;
		else
			aux.actualScaleA = aux.actualScaleA - vary;
	}
	else
	{
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;
	}

	// offset
	z += fractal->transformCommon.additionConstant000;

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	// rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	// tweak scale
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		z *= fractal->transformCommon.scaleG1;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scaleG1);
	}

	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd += fractal->mandelbox.color.factor.x
										* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
			if (zCol.y != oldZ.y)
				colorAdd += fractal->mandelbox.color.factor.y
										* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
			if (zCol.z != oldZ.z)
				colorAdd += fractal->mandelbox.color.factor.z
										* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

			if (rrCol < fractal->transformCommon.maxR2d1)
			{
				if (rrCol < fractal->transformCommon.minR2p25)
					colorAdd +=
						fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
						+ fractal->mandelbox.color.factorSp2
								* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
				else
					colorAdd +=
						fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			}
		}
		else
		{
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;

			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1;
			else if (rrCol < fractal->transformCommon.maxR2d1)
				colorAdd += fractal->mandelbox.color.factorSp2;
		}
		aux.color += colorAdd;
	}
}

/**
 * aexion octopus. Based on Aex_Octo from Mandelbulber3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void AexionOctopusModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	CVector4 tempN;
	tempN.x = z.x * z.z * fractal->transformCommon.scale3D111.x;
	tempN.y = (z.x * z.x - z.z * z.z) * fractal->transformCommon.scale3D111.y;
	tempN.z = z.y;
	tempN.w = z.w;

	if (fractal->transformCommon.functionEnabledAx) tempN.x = fabs(tempN.x);
	if (fractal->transformCommon.functionEnabledAy) tempN.y = fabs(tempN.y);
	if (fractal->transformCommon.functionEnabledAz) tempN.z = fabs(tempN.z);

	z = tempN;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempFAB = c;
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
 * amazing surf from Mandelbulber3D. Formula proposed by Kali, with features added by DarkBeam
 *
 * This formula has a c.x c.y SWAP
 *
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void AmazingSurfIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// update aux.actualScale
	aux.actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	CVector4 c = aux.const_c;
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

	/*if (rr < sqrtMinR)
	{
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < 1.0)
	{
		aux.color += fractal->mandelbox.color.factorSp2;
	}*/
	// use aux.actualScale
	double m = aux.actualScale / dividend;

	z *= (m - 1.0) * fractal->transformCommon.scale1 + 1.0;
	// z *= m * fractal->transformCommon.scale1 + 1.0 * (1.0 - fractal->transformCommon.scale1);
	aux.DE = aux.DE * fabs(m) + 1.0;

	if (fractal->transformCommon.addCpixelEnabledFalse)
		z += CVector4(c.y, c.x, c.z, c.w) * fractal->transformCommon.constantMultiplier111;

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

/**
 * Based on Amazing Surf Mod 1 from Mandelbulber3D, a formula proposed by Kali,
 * with features added by DarkBeam
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 * This formula contains aux.color and aux.actualScale
 */
void AmazingSurfMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	aux.actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);
	CVector4 oldZ = z;
	if (fractal->transformCommon.functionEnabledAx)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
	}

	// z = fold - fabs( fabs(z) - fold)
	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		z.x = fractal->transformCommon.additionConstant111.x
					- fabs(fabs(z.x) - fractal->transformCommon.additionConstant111.x);
		z.y = fractal->transformCommon.additionConstant111.y
					- fabs(fabs(z.y) - fractal->transformCommon.additionConstant111.y);
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
	}

	// if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
	if (fractal->transformCommon.functionEnabledxFalse)
	{
		if (fabs(z.x) > fractal->transformCommon.additionConstant111.x)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
		}
		if (fabs(z.y) > fractal->transformCommon.additionConstant111.y)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
		}
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
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
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
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
		colorAdd += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < 1.0)
	{
		z *= 1.0 / r2;
		aux.DE *= 1.0 / r2;
		colorAdd += fractal->mandelbox.color.factorSp2;
	}

	z *= aux.actualScale * fractal->transformCommon.scale1
			 + 1.0 * (1.0 - fractal->transformCommon.scale1);
	aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	aux.color += colorAdd;
}

/**
 * asurf mod2
 * modification of amazing surf from M3D. Formula proposed by Kali, with features added by DarkBeam
 *
 * Note for the original version apply a c.x c.y SWAP
 *
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 *
 * This formula contains aux.color and aux.actualScaleA
 */
void AmazingSurfMod2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;
	CVector4 c = aux.const_c;

	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
	{
		CVector4 tempC = aux.const_c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		if (fractal->transformCommon.rotationEnabled
				&& aux.i >= fractal->transformCommon.startIterationsG
				&& aux.i < fractal->transformCommon.stopIterationsG)
		{
			tempC = fractal->transformCommon.rotationMatrix.RotateVector(tempC);
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}

	// invert c
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			aux.c *= fractal->transformCommon.scale3D111;
			double rSqrL = aux.c.Dot(aux.c);
			rSqrL = 1.0 / rSqrL;
			aux.c *= rSqrL;
			z += aux.c;
		}
		else
		{
			c *= fractal->transformCommon.scale3D111;
			double rSqrL = c.Dot(c);
			rSqrL = 1.0 / rSqrL;
			z += c * rSqrL;
		}
	}

	oldZ = z;

	bool functionEnabledN[3] = {fractal->transformCommon.functionEnabledAx,
		fractal->transformCommon.functionEnabledAyFalse,
		fractal->transformCommon.functionEnabledAzFalse};
	int startIterationN[3] = {fractal->transformCommon.startIterationsA,
		fractal->transformCommon.startIterationsB, fractal->transformCommon.startIterationsC};
	int stopIterationN[3] = {fractal->transformCommon.stopIterationsA,
		fractal->transformCommon.stopIterationsB, fractal->transformCommon.stopIterationsC};
	enumMulti_orderOf3Folds foldN[3] = {fractal->aSurf3Folds.orderOf3Folds1,
		fractal->aSurf3Folds.orderOf3Folds2, fractal->aSurf3Folds.orderOf3Folds3};

	for (int f = 0; f < 3; f++)
	{
		if (functionEnabledN[f] && aux.i >= startIterationN[f] && aux.i < stopIterationN[f])
		{
			switch (foldN[f])
			{
				case multi_orderOf3Folds_type1: // tglad fold
				default:
					z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
								- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
					z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
								- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;

					if (fractal->transformCommon.functionEnabledCzFalse
							&& aux.i >= fractal->transformCommon.startIterationsT
							&& aux.i < fractal->transformCommon.stopIterationsT1)
					{
						CVector4 limit = fractal->transformCommon.additionConstant111;
						CVector4 length = 2.0 * limit;
						CVector4 tgladS = 1.0 / length;
						CVector4 Add = CVector4(0.0, 0.0, 0.0, 0.0);
						if (fabs(z.x) < limit.x) Add.x = z.x * z.x * tgladS.x;
						if (fabs(z.y) < limit.y) Add.y = z.y * z.y * tgladS.y;
						if (fabs(z.x) > limit.x && fabs(z.x) < length.x)
							Add.x = (length.x - fabs(z.x)) * (length.x - fabs(z.x)) * tgladS.x;
						if (fabs(z.y) > limit.y && fabs(z.y) < length.y)
							Add.y = (length.y - fabs(z.y)) * (length.y - fabs(z.y)) * tgladS.y;
						Add *= fractal->transformCommon.scale3D000;
						z.x = (z.x - (sign(z.x) * (Add.x)));
						z.y = (z.y - (sign(z.y) * (Add.y)));
					}
					zCol = z;
					break;
				case multi_orderOf3Folds_type2: // z = fold - fabs( fabs(z) - fold)
					z.x = fractal->transformCommon.offset111.x
								- fabs(fabs(z.x) - fractal->transformCommon.offset111.x);
					z.y = fractal->transformCommon.offset111.y
								- fabs(fabs(z.y) - fractal->transformCommon.offset111.y);
					zCol = z;
					break;
				case multi_orderOf3Folds_type3:
					// z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold), darkbeam's
					z.x = fractal->transformCommon.offset2
								- fabs(fabs(z.x + fractal->transformCommon.offsetA111.x)
											 - fractal->transformCommon.offset2)
								- fractal->transformCommon.offsetA111.x;
					z.y = fractal->transformCommon.offset2
								- fabs(fabs(z.y + fractal->transformCommon.offsetA111.y)
											 - fractal->transformCommon.offset2)
								- fractal->transformCommon.offsetA111.y;
					zCol = z;
					break;
			}
		}
	}

	// enable z axis
	if (fractal->transformCommon.functionEnabledAxFalse)
	{
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		zCol.z = z.z;
	}

	// swap
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		z = CVector4(z.y, z.x, z.z, z.w);
	}

	// offset
	if (fractal->transformCommon.functionEnabledBzFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			CVector4 temp = fractal->transformCommon.additionConstant000;
			CVector4 temp2 = temp * temp;

			z.x -= ((temp.x * temp2.x) / ((z.x * z.x) + (temp2.x)) - 2.0 * temp.x)
						 * fractal->transformCommon.scale1;
			z.y -= ((temp.y * temp2.y) / ((z.y * z.y) + (temp2.y)) - 2.0 * temp.y)
						 * fractal->transformCommon.scale1;
			z.z -= ((temp.z * temp2.z) / ((z.z * z.z) + (temp2.z)) - 2.0 * temp.z)
						 * fractal->transformCommon.scale1;
		}
		else if (fractal->transformCommon.functionEnabledByFalse)
		{
			CVector4 temp = fractal->transformCommon.additionConstant000;
			CVector4 temp2 = temp * temp;

			z.x -= ((temp2.x) / ((z.x * z.x) + (temp2.x)) - 2.0 * temp.x)
						 * fractal->transformCommon.scale1; // * sign(z.x);
			z.y -= ((temp2.y) / ((z.y * z.y) + (temp2.y)) - 2.0 * temp.y)
						 * fractal->transformCommon.scale1; // * sign(z.y);
			z.z -= ((temp2.z) / ((z.z * z.z) + (temp2.z)) - 2.0 * temp.z)
						 * fractal->transformCommon.scale1; // * sign(z.z);
		}
	}
	else
		z += fractal->transformCommon.additionConstant000;

	// standard functions
	if (fractal->transformCommon.functionEnabledAy)
	{
		double rr = z.Dot(z);
		rrCol = rr;
		if (fractal->transformCommon.functionEnabledFalse)		// force cylinder fold
			rr -= z.z * z.z * fractal->transformCommon.scaleB1; // fold weight

		// Mandelbox Spherical fold
		if (aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
		{
			z += fractal->mandelbox.offset;

			// if (r2 < 1e-21) r2 = 1e-21;
			if (rr < fractal->transformCommon.minR2p25)
			{
				double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
				z *= tglad_factor1;
				aux.DE *= tglad_factor1;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
				z *= tglad_factor2;
				aux.DE *= tglad_factor2;
			}
			z -= fractal->mandelbox.offset;
		}

		// scale
		double useScale = 1.0;
		if (aux.i >= fractal->transformCommon.startIterationsS
				&& aux.i < fractal->transformCommon.stopIterationsS)
		{
			useScale = aux.actualScaleA + fractal->mandelbox.scale;

			z *= useScale;

			if (!fractal->analyticDE.enabledFalse)
				aux.DE = aux.DE * fabs(useScale) + 1.0;
			else // testing for log
				aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

			if (fractal->transformCommon.functionEnabledFFalse
					&& aux.i >= fractal->transformCommon.startIterationsY
					&& aux.i < fractal->transformCommon.stopIterationsY)
			{
				// update actualScaleA for next iteration
				double vary = fractal->transformCommon.scaleVary0
											* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
				if (fractal->transformCommon.functionEnabledMFalse)
					aux.actualScaleA = -vary;
				else
					aux.actualScaleA = aux.actualScaleA - vary;
			}
		}
	}

	if (fractal->mandelbox.mainRotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->mandelbox.color.factor.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->mandelbox.color.factor.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->mandelbox.color.factor.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

		if (rrCol < fractal->transformCommon.maxR2d1)
		{
			colorAdd += fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
										+ fractal->mandelbox.color.factorSp2
												* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
		}
		aux.color += colorAdd;
	}
}

/**
 * asurf mod3  beta
 * amazing surf from Mandelbulber3D. Formula proposed by Kali, with features added by DarkBeam
 *
 * Note for the original version apply a c.x c.y SWAP
 *
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void AmazingSurfMod3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;
	CVector4 c = aux.const_c;

	oldZ = z;

	bool functionEnabledN[3] = {fractal->transformCommon.functionEnabledAx,
		fractal->transformCommon.functionEnabledAyFalse,
		fractal->transformCommon.functionEnabledAzFalse};
	int startIterationN[3] = {fractal->transformCommon.startIterationsA,
		fractal->transformCommon.startIterationsB, fractal->transformCommon.startIterationsC};
	int stopIterationN[3] = {fractal->transformCommon.stopIterationsA,
		fractal->transformCommon.stopIterationsB, fractal->transformCommon.stopIterationsC};
	enumMulti_orderOf3Folds foldN[3] = {fractal->aSurf3Folds.orderOf3Folds1,
		fractal->aSurf3Folds.orderOf3Folds2, fractal->aSurf3Folds.orderOf3Folds3};

	for (int f = 0; f < 3; f++)
	{
		if (functionEnabledN[f] && aux.i >= startIterationN[f] && aux.i < stopIterationN[f])
		{
			switch (foldN[f])
			{
				case multi_orderOf3Folds_type1: // tglad fold
				default:
					if (fractal->transformCommon.functionEnabledCx)
					{
						z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
									- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
						z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
									- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
						zCol = z;
					}
					break;
				case multi_orderOf3Folds_type2:
					if (fractal->transformCommon.functionEnabledDFalse)
					{
						// tglad fold
						CVector4 foldMod = fractal->transformCommon.offsetA222;
						CVector4 fold = fractal->transformCommon.offset111;
						CVector4 sg = z;
						sg.x = sign(z.x);
						sg.y = sign(z.y);

						CVector4 folder = z;
						CVector4 Tglad = z;

						Tglad.x = fabs(z.x + fold.x) - fabs(z.x - fold.x) - z.x;
						Tglad.y = fabs(z.y + fold.y) - fabs(z.y - fold.y) - z.y;

						folder.x = sg.x * (z.x - Tglad.x);
						folder.y = sg.y * (z.y - Tglad.y);
						folder.x = fabs(folder.x);
						folder.y = fabs(folder.y);
						folder.x = min(folder.x, foldMod.x);
						folder.y = min(folder.y, foldMod.y);
						z.x -= sg.x * folder.x;
						z.y -= sg.y * folder.y;
						zCol = z;
					}
					break;
				case multi_orderOf3Folds_type3:
					if (fractal->transformCommon.functionEnabledEFalse)
					{
						// z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold), darkbeam's
						z.x = fractal->transformCommon.offset222.x
									- fabs(fabs(z.x + fractal->transformCommon.offsetA111.x)
												 - fractal->transformCommon.offset222.x)
									- fractal->transformCommon.offsetA111.x;
						z.y = fractal->transformCommon.offset222.y
									- fabs(fabs(z.y + fractal->transformCommon.offsetA111.y)
												 - fractal->transformCommon.offset222.y)
									- fractal->transformCommon.offsetA111.y;
						zCol = z;
					}
					break;
			}
		}
	}

	// enable z axis
	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsZ
			&& aux.i < fractal->transformCommon.stopIterationsZ)
	{
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		zCol.z = z.z;
	}

	// swizzle, swap xy alternating
	if (fractal->transformCommon.functionEnabledSwFalse
			&& aux.i >= fractal->transformCommon.startIterationsI
			&& aux.i < fractal->transformCommon.stopIterationsI)
	{
		z = CVector4(z.y, z.x, z.z, z.w);
	}
	// change y sign if iter is even
	if (fractal->transformCommon.functionEnabledJFalse
			&& aux.i >= fractal->transformCommon.startIterationsJ
			&& aux.i < fractal->transformCommon.stopIterationsJ)
	{
		if (aux.i % 2 < 1) z.y = -z.y;
	}

	// change z sign if iter is even
	if (fractal->transformCommon.functionEnabledKFalse
			&& aux.i >= fractal->transformCommon.startIterationsK
			&& aux.i < fractal->transformCommon.stopIterationsK)
	{
		if (aux.i % 2 < 1) z.z = -z.z;
	}

	// offset
	if (fractal->transformCommon.functionEnabledBzFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			CVector4 temp = fractal->transformCommon.additionConstant000;
			CVector4 temp2 = temp * temp;

			z.x -= ((temp.x * temp2.x) / ((z.x * z.x) + (temp2.x)) - 2.0 * temp.x)
						 * fractal->transformCommon.scale1;
			z.y -= ((temp.y * temp2.y) / ((z.y * z.y) + (temp2.y)) - 2.0 * temp.y)
						 * fractal->transformCommon.scale1;
			z.z -= ((temp.z * temp2.z) / ((z.z * z.z) + (temp2.z)) - 2.0 * temp.z)
						 * fractal->transformCommon.scale1;
		}
		if (fractal->transformCommon.functionEnabledByFalse)
		{
			CVector4 temp = fractal->transformCommon.additionConstant000;
			CVector4 temp2 = temp * temp;

			z.x -= ((temp2.x) / ((z.x * z.x) + (temp2.x)) - 2.0 * temp.x)
						 * fractal->transformCommon.scale1; // * sign(z.x);
			z.y -= ((temp2.y) / ((z.y * z.y) + (temp2.y)) - 2.0 * temp.y)
						 * fractal->transformCommon.scale1; // * sign(z.y);
			z.z -= ((temp2.z) / ((z.z * z.z) + (temp2.z)) - 2.0 * temp.z)
						 * fractal->transformCommon.scale1; // * sign(z.z);
		}
	}
	else if (aux.i >= fractal->transformCommon.startIterationsO
					 && aux.i < fractal->transformCommon.stopIterationsO)
		z += fractal->transformCommon.additionConstant000;

	// standard functions
	if (fractal->transformCommon.functionEnabledAy)
	{
		double rr = z.Dot(z);
		rrCol = rr;
		if (fractal->transformCommon.functionEnabledFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD) // force cylinder fold
			rr -= z.z * z.z * fractal->transformCommon.scaleB1;		 // fold weight

		// Mandelbox Spherical fold
		if (aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
		{
			z += fractal->mandelbox.offset;

			// if (r2 < 1e-21) r2 = 1e-21;
			if (rr < fractal->transformCommon.minR0)
			{
				double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR0;
				z *= tglad_factor1;
				aux.DE *= tglad_factor1;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
				z *= tglad_factor2;
				aux.DE *= tglad_factor2;
			}
			z -= fractal->mandelbox.offset;
		}

		// scale
		double useScale = 1.0;
		if (aux.i >= fractal->transformCommon.startIterationsS
				&& aux.i < fractal->transformCommon.stopIterationsS)
		{
			useScale = aux.actualScaleA + fractal->mandelbox.scale;

			z *= useScale;

			if (!fractal->analyticDE.enabledFalse)
				aux.DE = aux.DE * fabs(useScale) + 1.0;
			else
				aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

			if (fractal->transformCommon.functionEnabledFFalse
					&& aux.i >= fractal->transformCommon.startIterationsY
					&& aux.i < fractal->transformCommon.stopIterationsY)
			{
				// update actualScaleA for next iteration
				double vary = fractal->transformCommon.scaleVary0
											* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
				if (fractal->transformCommon.functionEnabledMFalse)
					aux.actualScaleA = -vary;
				else
					aux.actualScaleA = aux.actualScaleA - vary;
			}
		}
	}

	if (fractal->mandelbox.mainRotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->mandelbox.color.factor.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->mandelbox.color.factor.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->mandelbox.color.factor.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

		if (rrCol < fractal->transformCommon.maxR2d1)
		{
			colorAdd += fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
										+ fractal->mandelbox.color.factorSp2
												* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
		}
		aux.color += colorAdd;
	}
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
	{
		CVector4 tempC = aux.const_c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		if (fractal->transformCommon.rotationEnabled
				&& aux.i >= fractal->transformCommon.startIterationsG
				&& aux.i < fractal->transformCommon.stopIterationsG)
		{
			tempC = fractal->transformCommon.rotationMatrix.RotateVector(tempC);
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}

	// invert c
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			aux.c *= fractal->transformCommon.scale3D111;
			double rSqrL = aux.c.Dot(aux.c);
			rSqrL = 1.0 / rSqrL;
			aux.c *= rSqrL;
			z += aux.c;
		}
		else
		{
			c *= fractal->transformCommon.scale3D111;
			double rSqrL = c.Dot(c);
			rSqrL = 1.0 / rSqrL;
			z += c * rSqrL;
		}
	}
}

/**
 * Amazing Surface Multi
 * Based on Amazing Surf Mod 1 from Mandelbulber3D, a formula proposed by Kali,
 * with features added by DarkBeam
 * This formula contains aux.color
 */
void AmazingSurfMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	CVector4 oldZ = z;
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
	enumMulti_orderOfFolds foldN[5] = {fractal->surfFolds.orderOfFolds1,
		fractal->surfFolds.orderOfFolds2, fractal->surfFolds.orderOfFolds3,
		fractal->surfFolds.orderOfFolds4, fractal->surfFolds.orderOfFolds5};

	for (int f = 0; f < 5; f++)
	{
		if (functionEnabledN[f] && aux.i >= startIterationN[f] && aux.i < stopIterationN[f])
		{
			switch (foldN[f])
			{
				case multi_orderOfFolds_type1: // tglad fold
				default:
					z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
								- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
					z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
								- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
					if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
					if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
					break;
				case multi_orderOfFolds_type2: // z = fold - fabs( fabs(z) - fold)
					z.x = fractal->transformCommon.additionConstant111.x
								- fabs(fabs(z.x) - fractal->transformCommon.additionConstant111.x);
					z.y = fractal->transformCommon.additionConstant111.y
								- fabs(fabs(z.y) - fractal->transformCommon.additionConstant111.y);
					if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
					if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
					break;
				case multi_orderOfFolds_type3:
					z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
					z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
					break;
				case multi_orderOfFolds_type4:
					// if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
					if (fabs(z.x) > fractal->transformCommon.additionConstant111.x)
					{
						z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
						colorAdd += fractal->mandelbox.color.factor.x;
					}
					if (fabs(z.y) > fractal->transformCommon.additionConstant111.y)
					{
						z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
						colorAdd += fractal->mandelbox.color.factor.y;
					}
					break;
				case multi_orderOfFolds_type5:
					// z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
					z.x = fractal->transformCommon.offset2
								- fabs(fabs(z.x + fractal->transformCommon.additionConstant111.x)
											 - fractal->transformCommon.offset2)
								- fractal->transformCommon.additionConstant111.x;
					z.y = fractal->transformCommon.offset2
								- fabs(fabs(z.y + fractal->transformCommon.additionConstant111.y)
											 - fractal->transformCommon.offset2)
								- fractal->transformCommon.additionConstant111.y;
					if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
					if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
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
		}
	}
	z += fractal->transformCommon.additionConstant000;

	// standard functions
	if (fractal->transformCommon.functionEnabledAy)
	{
		double rr;
		rr = z.Dot(z);
		if (fractal->transformCommon.functionEnabledFalse)		// force cylinder fold
			rr -= z.z * z.z * fractal->transformCommon.scaleB1; // fold weight;

		if (fractal->transformCommon.functionEnabledAz
				&& aux.i >= fractal->transformCommon.startIterationsT
				&& aux.i < fractal->transformCommon.stopIterationsT)
		{
			// Abox Spherical fold
			z += fractal->mandelbox.offset;
			double sqrtMinR = fractal->transformCommon.sqtR;

			if (rr < sqrtMinR)
			{
				z *= fractal->transformCommon.mboxFactor1;
				aux.DE *= fractal->transformCommon.mboxFactor1;
				colorAdd += fractal->mandelbox.color.factorSp1;
			}
			else if (rr < 1.0)
			{
				z *= 1.0 / rr;
				aux.DE *= 1.0 / rr;
				colorAdd += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->mandelbox.offset;
		}

		// Mandelbox Spherical fold
		if (fractal->transformCommon.functionEnabledzFalse
				&& aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
		{
			// r2 = z.Dot(z);
			z += fractal->mandelbox.offset;
			if (rr < fractal->mandelbox.mR2) // mR2 = minR^2
			{
				z *= fractal->mandelbox.mboxFactor1; // fR2/mR2
				aux.DE *= fractal->mandelbox.mboxFactor1;
				colorAdd += fractal->mandelbox.color.factorSp1;
			}
			else if (rr < fractal->mandelbox.fR2)
			{
				double tglad_factor2 = fractal->mandelbox.fR2 / rr;
				z *= tglad_factor2;
				aux.DE *= tglad_factor2;
				colorAdd += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->mandelbox.offset;
		}

		if (aux.i >= fractal->transformCommon.startIterationsS
				&& aux.i < fractal->transformCommon.stopIterationsS)
		{ // scale
			z *= fractal->mandelbox.scale * fractal->transformCommon.scale1
					 + 1.0 * (1.0 - fractal->transformCommon.scale1);
			aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
		}
	}
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	if (fractal->mandelbox.mainRotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	aux.color += colorAdd;
}

/**
 * benesiFastPwr2PineTree 3D
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void BenesiPineTreeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c * fractal->transformCommon.constantMultiplier100;
	CVector4 zz = z * z;
	aux.r = sqrt(zz.x + zz.y + zz.z); // needed when alternating pwr2s
	aux.DE = aux.r * aux.DE * 2.0 + 1.0;

	double t = 1.0;
	double temp = zz.y + zz.z;
	if (temp > 0.0) t = 2.0 * z.x / sqrt(temp);
	temp = z.z;
	z.x = (zz.x - zz.y - zz.z);
	z.y = (2.0 * t * z.y * temp);
	z.z = (t * (zz.y - zz.z));
	// c.yz swap
	z.x += c.x;
	z.z += c.y;
	z.y += c.z;

	if (fractal->transformCommon.angle0 != 0)
	{
		double tempY = z.y;
		double beta = fractal->transformCommon.angle0 * M_PI_180;
		z.y = z.y * cos(beta) + z.z * sin(beta);
		z.z = tempY * -sin(beta) + z.z * cos(beta);
	}
}

/**
 * benesiT1PineTree 3D
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void BenesiT1PineTreeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.benesiT1Enabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{

		// Benesi mag transform T1
		if (fractal->transformCommon.benesiT1Enabled
				&& aux.i >= fractal->transformCommon.startIterations
				&& aux.i < fractal->transformCommon.stopIterations)
		{
			double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
			z = CVector4(
				(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

			double tempL = z.Length();
			z = fabs(z) * fractal->transformCommon.scale3D222;
			// if (tempL < 1e-21) tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			aux.DE = aux.DE * avgScale + 1.0;

			if (fractal->transformCommon.rotationEnabled)
			{ // rotation inside T1
				z = fractal->transformCommon.rotationMatrix.RotateVector(z);
			}

			tempXZ = (z.y + z.x) * SQRT_1_2;

			z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
				z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
			z = z - fractal->transformCommon.offset200;
		}
	}
	//  Benesi pine tree pwr2
	if (fractal->transformCommon.addCpixelEnabled
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 zz = z * z;
		aux.r = sqrt(zz.x + zz.y + zz.z); // needed when alternating pwr2s
		aux.DE = aux.r * aux.DE * 2.0 + 1.0;

		double t = 1.0;
		double temp = zz.y + zz.z;
		if (temp > 0.0) t = 2.0 * z.x / sqrt(temp);
		temp = z.z;
		z.x = (zz.x - zz.y - zz.z);
		z.y = (2.0 * t * z.y * temp);
		z.z = (t * (zz.y - zz.z));

		// swap c.yz then add cPixel
		CVector4 tempC = aux.const_c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c * fractal->transformCommon.constantMultiplier100;
			tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w);
			aux.c = tempC;
		}
		else
		{
			tempC *= fractal->transformCommon.constantMultiplier100;
			tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w);
		}
		z += tempC;
	}

	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		z = CVector4(z.x * cos(z.y * fractal->transformCommon.scale1),
					z.x * sin(z.y * fractal->transformCommon.scale1), z.z * fractal->transformCommon.scaleC1,
					z.w)
				* fractal->transformCommon.scaleA1;
		aux.DE *= fabs(fractal->transformCommon.scaleA1);
	}

	if (fractal->transformCommon.juliaMode)
	{
		z.x += fractal->transformCommon.offset000.x;
		z.y += fractal->transformCommon.offset000.y;
		z.z += fractal->transformCommon.offset000.z;
	}
	if (fractal->transformCommon.rotation2EnabledFalse)
	{
		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);
	}
}

/**
 * benesiMagTransforms
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void BenesiMagTransformsIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	// benesi T1
	if (fractal->transformCommon.benesiT1Enabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		double tempL = z.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;
		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
		z = z - fractal->transformCommon.offset200;
	}
	// rotation
	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	//  Benesi pine tree pwr2
	if (fractal->transformCommon.addCpixelEnabled
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		CVector4 zz = z * z;
		aux.r = sqrt(zz.x + zz.y + zz.z); // needed when alternating pwr2s
		aux.DE = aux.r * aux.DE * 2.0 + 1.0;

		double t = 1.0;
		double temp = zz.y + zz.z;
		if (temp > 0.0) t = 2.0 * z.x / sqrt(temp);
		temp = z.z;
		z.x = (zz.x - zz.y - zz.z);
		z.y = (2.0 * t * z.y * temp);
		z.z = (t * (zz.y - zz.z));

		// swap c.yz then add cPixel
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c * fractal->transformCommon.constantMultiplier100;
			tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w);
			aux.c = tempC;
		}
		else
		{
			tempC *= fractal->transformCommon.constantMultiplier100;
			tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w);
		}
		z += tempC;
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

	enumMulti_orderOfTransf transfN[5] = {fractal->magTransf.orderOfTransf1,
		fractal->magTransf.orderOfTransf2, fractal->magTransf.orderOfTransf3,
		fractal->magTransf.orderOfTransf4, fractal->magTransf.orderOfTransf5};

	for (int f = 0; f < 5; f++)
	{
		if (functionEnabledN[f] && aux.i >= startIterationN[f] && aux.i < stopIterationN[f])
		{
			double tempXZ;
			double tempL;
			CVector4 temp;
			double avgScale;
			CVector4 tempV2;

			switch (transfN[f])
			{
				case multi_orderOfTransf_typeT1:
				default:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
					z = fabs(z) * fractal->transformCommon.scale3Da222;
					tempL = temp.Length();
					// if (tempL < 1e-21) tempL = 1e-21;
					avgScale = z.Length() / tempL;
					aux.DE = aux.DE * avgScale + 1.0;
					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					z = z - fractal->transformCommon.offsetA200;
					break;

				case multi_orderOfTransf_typeT1Mod:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
					z = fabs(z) * fractal->transformCommon.scale3D333;
					tempL = temp.Length();
					// if (tempL < 1e-21) tempL = 1e-21;
					avgScale = z.Length() / tempL;
					aux.DE = aux.DE * avgScale + 1.0;
					z = (fabs(z + fractal->transformCommon.offset111)
							 - fabs(z - fractal->transformCommon.offset111) - z);
					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					break;

				case multi_orderOfTransf_typeT2:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
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
					aux.DE = aux.DE * avgScale + 1.0;
					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					break;

				case multi_orderOfTransf_typeT3:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

					tempV2 = z;
					tempV2.x = (z.y + z.z);
					tempV2.y = (z.x + z.z); // switching
					tempV2.z = (z.x + z.y);
					z = (fabs(tempV2 - fractal->transformCommon.offset222))
							* fractal->transformCommon.scale3Db222;

					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					break;

				case multi_orderOfTransf_typeT4:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

					tempV2 = z;
					tempV2.x = (z.y * z.y + z.z * z.z);
					tempV2.y = (z.x * z.x + z.z * z.z); // switching, squared,
					tempV2.z = (z.x * z.x + z.y * z.y);
					z = (fabs(tempV2 - fractal->transformCommon.offsetB111))
							* fractal->transformCommon.scale3Dc222;

					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					break;

				case multi_orderOfTransf_typeT5b:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
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
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					break;
			}
		}
	}
}

/**
 * benesiPwr2 mandelbulb
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void BenesiPwr2MandelbulbIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	// Prism shape
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		CVector4 gap = fractal->transformCommon.constantMultiplier000;
		double t;
		double dot1;
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);

		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}
	// Benesi mag transform T1
	if (fractal->transformCommon.benesiT1Enabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		double tempL = z.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		if (fractal->transformCommon.rotationEnabled)
		{ // rotation inside T1
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.addCpixelEnabled
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{ // Benesi original pwr2
		CVector4 zz = z * z;
		double rrYZ = zz.y + zz.z;
		double lenYZ = sqrt(rrYZ);
		double temp = zz.x + rrYZ;
		aux.r = sqrt(temp); // aux.r needed when alternating pwr2s
		aux.DE = aux.DE * 2.0 * aux.r;
		CVector4 newZ = z;
		temp = zz.x - rrYZ;
		newZ.x = -sign(c.x) * temp;
		if (z.x < lenYZ) newZ.x = temp;
		rrYZ = -1.0 / lenYZ * 2.0 * fabs(z.x);
		newZ.y = rrYZ * (zz.y - zz.z);
		newZ.z = rrYZ * 2.0 * z.y * z.z;
		z = newZ + (c * fractal->transformCommon.constantMultiplierA100);
	}
	//  Benesi pine tree pwr2
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 zz = z * z;
		aux.r = sqrt(zz.x + zz.y + zz.z); // needed when alternating pwr2s
		aux.DE = aux.r * aux.DE * 2.0 + 1.0;

		double t = 1.0;
		double temp = zz.y + zz.z;
		if (temp > 0.0) t = 2.0 * z.x / sqrt(temp);
		temp = z.z;
		z.x = (zz.x - zz.y - zz.z);
		z.y = (2.0 * t * z.y * temp);
		z.z = (t * (zz.y - zz.z));

		// swap c.yz then add cPixel
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c * fractal->transformCommon.constantMultiplier100;
			tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w);
			aux.c = tempC;
		}
		else
		{
			tempC *= fractal->transformCommon.constantMultiplier100;
			tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w);
		}
		z += tempC;
	}

	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		z = CVector4(z.x * cos(z.y * fractal->transformCommon.scale1),
					z.x * sin(z.y * fractal->transformCommon.scale1), z.z * fractal->transformCommon.scaleC1,
					z.w)
				* fractal->transformCommon.scaleA1;
		aux.DE *= fabs(fractal->transformCommon.scaleA1);
	}

	if (fractal->transformCommon.juliaMode && aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.rotation2EnabledFalse)
		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * BoxBulb power 2 V2 with scaling of z axis
 * This formula contains aux.color and aux.actualScaleA
 */
void BoxFoldBulbPow2V2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;
	// tglad fold
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		zCol = z;
	}

	// spherical fold
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);
		rrCol = rr;

		z += fractal->mandelbox.offset;

		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < fractal->transformCommon.minR2p25)
		{
			double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
		}
		z -= fractal->mandelbox.offset;
	}
	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{

		useScale = aux.actualScaleA + fractal->transformCommon.scale;

		z *= useScale;
		aux.DE = aux.DE * fabs(useScale);

		if (aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			// update actualScale for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.r * aux.DE * 16.0 * fractal->analyticDE.scale1
							 * sqrt(fractal->foldingIntPow.zFactor * fractal->foldingIntPow.zFactor + 2.0
											+ fractal->analyticDE.offset2)
							 / SQRT_3
						 + fractal->analyticDE.offset1;

		z *= 2.0;
		double x2 = z.x * z.x;
		double y2 = z.y * z.y;
		double z2 = z.z * z.z;
		double temp = 1.0 - z2 / (x2 + y2);
		CVector4 zTemp;
		zTemp.x = (x2 - y2) * temp;
		zTemp.y = 2.0 * z.x * z.y * temp;
		zTemp.z = -2.0 * z.z * sqrt(x2 + y2);
		zTemp.w = z.w;
		z = zTemp;
		z.z *= fractal->foldingIntPow.zFactor;
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->mandelbox.color.factor.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->mandelbox.color.factor.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->mandelbox.color.factor.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

		if (rrCol < fractal->transformCommon.maxR2d1)
		{
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
										+ fractal->mandelbox.color.factorSp2
												* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
			else
				colorAdd += fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
		}

		aux.color += colorAdd;
	}
}

/**
 * BoxBulb power 2 V3  with scaling of z axis
 * This formula contains aux.color and aux.actualScaleA
 */
void BoxFoldBulbPow2V3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		double rr = 1.0;
		z += fractal->transformCommon.offset000;
		rr = z.Dot(z);
		z *= fractal->transformCommon.scaleG1 / rr;
		aux.DE *= (fractal->transformCommon.scaleG1 / rr);
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleA1;
		aux.DE *= fractal->transformCommon.scaleA1;
	}

	// CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;

	// tglad fold
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		zCol = z;
	}

	// spherical fold
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);
		rrCol = rr;
		z += fractal->mandelbox.offset;

		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < fractal->transformCommon.minR2p25)
		{
			double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
		}
		z -= fractal->mandelbox.offset;
	}
	// scale
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{

		double useScale = aux.actualScaleA + fractal->transformCommon.scale;

		z *= useScale;
		aux.DE = aux.DE * fabs(useScale);

		if (aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			// update actualScale for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsTM
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale;
		z = (fabs(z + fractal->transformCommon.additionConstantA111)
				 - fabs(z - fractal->transformCommon.additionConstantA111) - z);
		tempXZ = (z.y + z.x) * SQRT_1_2;
		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	}

	// octo
	if (fractal->transformCommon.functionEnabledDFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)

	{
		if (z.x + z.y < 0.0) z = CVector4(-z.y, -z.x, z.z, z.w);

		if (z.x + z.z < 0.0) // z.xz = -z.zx;
			z = CVector4(-z.z, z.y, -z.x, z.w);

		if (z.x - z.y < 0.0) // z.xy = z.yx;
			z = CVector4(z.y, z.x, z.z, z.w);

		if (z.x - z.z < 0.0) // z.xz = z.zx;
			z = CVector4(z.z, z.y, z.x, z.w);

		z.x = fabs(z.x);
		z = z * fractal->transformCommon.scaleA2
				- fractal->transformCommon.offset100 * (fractal->transformCommon.scaleA2 - 1.0);
		aux.DE *= fabs(fractal->transformCommon.scaleA2);
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{ // fabs() and menger fold
		z = fabs(z + fractal->transformCommon.additionConstantA000);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
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
	}

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();

		if (fractal->analyticDE.enabledFalse)
		{
			aux.DE = aux.r * aux.DE * 10.0 * fractal->analyticDE.scale1
								 * sqrt(fractal->foldingIntPow.zFactor * fractal->foldingIntPow.zFactor + 2.0
												+ fractal->analyticDE.offset2)
							 + fractal->analyticDE.offset1;
		}
		else
		{
			aux.DE = aux.r * aux.DE * 16.0 * fractal->analyticDE.scale1
								 * sqrt(fractal->foldingIntPow.zFactor * fractal->foldingIntPow.zFactor + 2.0
												+ fractal->analyticDE.offset2)
								 / SQRT_3
							 + fractal->analyticDE.offset1;
		}

		z *= 2.0;
		double x2 = z.x * z.x;
		double y2 = z.y * z.y;
		double z2 = z.z * z.z;
		double temp = 1.0 - z2 / (x2 + y2);
		CVector4 zTemp;
		zTemp.x = (x2 - y2) * temp;
		zTemp.y = 2.0 * z.x * z.y * temp;
		zTemp.z = -2.0 * z.z * sqrt(x2 + y2);
		zTemp.w = z.w;
		z = zTemp;
		z.z *= fractal->foldingIntPow.zFactor;
	}
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->mandelbox.color.factor.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->mandelbox.color.factor.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->mandelbox.color.factor.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

		if (rrCol < fractal->transformCommon.maxR2d1)
		{
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
										+ fractal->mandelbox.color.factorSp2
												* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
			else
				colorAdd += fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
		}

		aux.color += colorAdd;
	}
}

/**
 * BoxBulb power 2  menger with scaling of z axis
 * This formula contains aux.color
 */
void BoxFoldBulbMengerIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;
	// tglad fold
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		zCol = z;
	}

	// spherical fold
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);
		rrCol = rr;
		z += fractal->mandelbox.offset;

		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < fractal->transformCommon.minR2p25)
		{
			double tglad_factor1 = fractal->transformCommon.maxMinR2factor;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
		}
		z -= fractal->mandelbox.offset;
	}
	// scale
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z *= fractal->transformCommon.scale;
		aux.DE *= fabs(fractal->transformCommon.scale);
	}

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		z += c * fractal->transformCommon.constantMultiplierC111;
	}

	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsTM
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale;
		z = (fabs(z + fractal->transformCommon.additionConstantA111)
				 - fabs(z - fractal->transformCommon.additionConstantA111) - z);
		tempXZ = (z.y + z.x) * SQRT_1_2;
		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	}

	// octo
	if (fractal->transformCommon.functionEnabledDFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)

	{
		if (z.x + z.y < 0.0) z = CVector4(-z.y, -z.x, z.z, z.w);

		if (z.x + z.z < 0.0) // z.xz = -z.zx;
			z = CVector4(-z.z, z.y, -z.x, z.w);

		if (z.x - z.y < 0.0) // z.xy = z.yx;
			z = CVector4(z.y, z.x, z.z, z.w);

		if (z.x - z.z < 0.0) // z.xz = z.zx;
			z = CVector4(z.z, z.y, z.x, z.w);

		z.x = fabs(z.x);
		z = z * fractal->transformCommon.scaleA2
				- fractal->transformCommon.offset100 * (fractal->transformCommon.scaleA2 - 1.0);
		aux.DE *= fabs(fractal->transformCommon.scaleA2);
	}

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	// pow2 bulb
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();

		if (fractal->analyticDE.enabledFalse)
		{
			aux.DE = aux.r * aux.DE * 5.0 * fractal->analyticDE.scale1 * fractal->transformCommon.scaleG1
								 * sqrt(fractal->foldingIntPow.zFactor * fractal->foldingIntPow.zFactor + 2.0
												+ fractal->analyticDE.offset2)
							 + fractal->analyticDE.offset1;
		}
		else
		{
			aux.DE = aux.r * aux.DE * 8.0 * fractal->analyticDE.scale1 * fractal->transformCommon.scaleG1
								 * sqrt(fractal->foldingIntPow.zFactor * fractal->foldingIntPow.zFactor + 2.0
												+ fractal->analyticDE.offset2)
								 / SQRT_3
							 + fractal->analyticDE.offset1;
		}
		z *= fractal->transformCommon.scaleG1;
		double x2 = z.x * z.x;
		double y2 = z.y * z.y;
		double z2 = z.z * z.z;
		double temp = 1.0 - z2 / (x2 + y2);
		CVector4 zTemp;
		zTemp.x = (x2 - y2) * temp;
		zTemp.y = 2.0 * z.x * z.y * temp;
		zTemp.z = -2.0 * z.z * sqrt(x2 + y2);
		zTemp.w = z.w;
		z = zTemp;
		z += fractal->transformCommon.offset000;
		z.z *= fractal->foldingIntPow.zFactor;
	}
	// menger sponge
	if (fractal->transformCommon.functionEnabledM
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		z = fabs(z + fractal->transformCommon.additionConstantA000);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
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
	}
	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->mandelbox.color.factor.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->mandelbox.color.factor.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->mandelbox.color.factor.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

		if (rrCol < fractal->transformCommon.maxR2d1)
		{
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
										+ fractal->mandelbox.color.factorSp2
												* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
			else
				colorAdd += fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
		}

		aux.color += colorAdd;
	}
}

/**
 * BoxFold Quaternion
 * This formula contains aux.color and aux.actualScale
 * Sometimes Delta DE Linear works best.
 */
void BoxFoldQuatIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;
	// tglad fold
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		zCol = z;
	}

	// spherical fold
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{

		double rr = z.Dot(z);
		rrCol = rr;
		z += fractal->mandelbox.offset;

		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < fractal->transformCommon.minR2p25)
		{
			double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
		}
		z -= fractal->mandelbox.offset;
	}
	// scale
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{

		double useScale = aux.actualScaleA + fractal->transformCommon.scale;

		z *= useScale;
		aux.DE = aux.DE * fabs(useScale);

		if (aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			// update actualScale for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsTM
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale;
		z = (fabs(z + fractal->transformCommon.additionConstantA111)
				 - fabs(z - fractal->transformCommon.additionConstantA111) - z);
		tempXZ = (z.y + z.x) * SQRT_1_2;
		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	}

	// octo
	if (fractal->transformCommon.functionEnabledDFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)

	{
		if (z.x + z.y < 0.0) z = CVector4(-z.y, -z.x, z.z, z.w);

		if (z.x + z.z < 0.0) // z.xz = -z.zx;
			z = CVector4(-z.z, z.y, -z.x, z.w);

		if (z.x - z.y < 0.0) // z.xy = z.yx;
			z = CVector4(z.y, z.x, z.z, z.w);

		if (z.x - z.z < 0.0) // z.xz = z.zx;
			z = CVector4(z.z, z.y, z.x, z.w);

		z.x = fabs(z.x);
		z = z * fractal->transformCommon.scaleA2
				- fractal->transformCommon.offset100 * (fractal->transformCommon.scaleA2 - 1.0);

		aux.DE *= fabs(fractal->transformCommon.scaleA2);
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{ // fabs() and menger fold
		z = fabs(z + fractal->transformCommon.additionConstantA000);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		// menger scales and offsets
		z *= fractal->transformCommon.scale2;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplier111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplier111.y;
		z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
		aux.DE *= fractal->transformCommon.scale2;
	}

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;

		if (fractal->analyticDE.enabledFalse)
		{
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
		}
		z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, z.w);

		double tempL = z.Length();
		z *= fractal->transformCommon.constantMultiplier122;
		// if (tempL < 1e-21) tempL = 1e-21;
		CVector4 tempAvgScale = CVector4(z.x, z.y / 2.0, z.z / 2.0, z.w);
		double avgScale = tempAvgScale.Length() / tempL;
		double tempAux = aux.DE * avgScale;
		aux.DE = aux.DE + (tempAux - aux.DE) * fractal->transformCommon.scaleA1;

		if (fractal->transformCommon.functionEnabledAxFalse)
		{
			CVector4 offsetAlt = aux.pos_neg * fractal->transformCommon.additionConstant000;
			z += offsetAlt;
			aux.pos_neg *= -fractal->transformCommon.scale1;
		}
		else
		{
			z += fractal->transformCommon.additionConstant000;
		}
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->mandelbox.color.factor.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->mandelbox.color.factor.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->mandelbox.color.factor.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

		if (rrCol < fractal->transformCommon.maxR2d1)
		{
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
										+ fractal->mandelbox.color.factorSp2
												* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
			else
				colorAdd += fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
		}

		aux.color += colorAdd;
	}
}

/**
 * CollatzIteration formula
 * @reference https://mathr.co.uk/blog/2016-04-10_collatz_fractal.html
 *            https://en.wikipedia.org/wiki/Collatz_conjecture#Iterating_on_real_or_complex_numbers
 */
void CollatzIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	CVector4 xV = CVector4(1.0, 1.0, 1.0, 0.0);
	CVector4 temp = xV + z * 2.0;
	temp *= z.RotateAroundVectorByAngle(xV.GetXYZ(), M_PI);
	z = xV + z * 4.0 - temp;
	z /= 4.0;
	aux.DE = aux.DE * 4.0 + 1.0;
}

/**
 * CollatzModIteration formula
 * @reference https://mathr.co.uk/blog/2016-04-10_collatz_fractal.html
 *            https://en.wikipedia.org/wiki/Collatz_conjecture#Iterating_on_real_or_complex_numbers
 */
void CollatzModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	CVector4 constantMult = CVector4(fractal->transformCommon.constantMultiplierB111.x,
		fractal->transformCommon.constantMultiplierB111.y,
		fractal->transformCommon.constantMultiplierB111.z, 0.0);

	z = constantMult + fractal->transformCommon.scale4 * z
			- (fractal->transformCommon.constantMultiplier111 + fractal->transformCommon.scale2 * z)
					* z.RotateAroundVectorByAngle(fractal->transformCommon.constantMultiplier111.GetXYZ(),
							M_PI * fractal->transformCommon.scale1); // * cPI ;

	z *= fractal->transformCommon.scale025;

	aux.DE = aux.DE * 4.0 * fractal->analyticDE.scale1 + 1.0;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		c = CVector4(c.z, c.y, c.x, c.w);
		z += c * fractal->transformCommon.constantMultiplierA111;
	}
}

/**
 * Modified Mandelbox (ABox) formula
 * This formula contains aux.color and aux.actualScaleA
 */
void FoldBoxMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		CVector4 tempA, tempB;
		CVector4 oldZ = z;
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
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
	}

	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box fold
	{
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
			colorAdd += fractal->mandelbox.color.factor.x;
		}
		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
			colorAdd += fractal->mandelbox.color.factor.y;
		}
		double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
		double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
			colorAdd += fractal->mandelbox.color.factor.z;
		}
	}

	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB) // spherical fold
	{
		double r2 = z.Dot(z);
		// if (r2 < 1e-21 && r2 > -1e-21)
		//	r2 = (r2 > 0) ? 1e-21 : -1e-21;
		if (r2 < fractal->mandelbox.mR2)
		{
			z *= fractal->mandelbox.mboxFactor1;
			aux.DE *= fractal->mandelbox.mboxFactor1;
			colorAdd += fractal->mandelbox.color.factorSp1;
		}
		else if (r2 < fractal->mandelbox.fR2)
		{
			double tglad_factor2 = fractal->mandelbox.fR2 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			colorAdd += fractal->mandelbox.color.factorSp2;
		}
	}

	// scale
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		double useScale = aux.actualScaleA + fractal->mandelbox.scale;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;

		// update actualScale for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
		if (fractal->transformCommon.functionEnabledMFalse)
			aux.actualScaleA = -vary;
		else
			aux.actualScaleA = aux.actualScaleA - vary;
	}
	else
	{
		z *= fractal->mandelbox.scale;
		aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
	}
	if (fractal->mandelbox.mainRotationEnabled && aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	// color updated v2.14
	if (fractal->foldColor.auxColorEnabled)
	{
		aux.color += colorAdd;
	}
}

/**
 * ifs XY
 */
void IfsXYIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// CVector4 oldZ = z;
	// double fillet = fractal->transformCommon.offset0;
	CVector4 boxSize = fractal->transformCommon.additionConstant0555;

	double xOffset = fractal->transformCommon.offset0;
	double yOffset = fractal->transformCommon.offset05;

	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
		z -= boxSize;

	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
		z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledAyFalse
			&& aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
		z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledAzFalse
			&& aux.i >= fractal->transformCommon.startIterationsZ
			&& aux.i < fractal->transformCommon.stopIterationsZ)
		z.z = fabs(z.z);

	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsJ
			&& aux.i < fractal->transformCommon.stopIterationsJ)
	{
		z.x = fabs(z.x);
		int poly = fractal->transformCommon.int3;
		double psi = fabs(fmod(atan(z.y / z.x) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledBy
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
		if (z.y > z.x) swap(z.x, z.y);

	if (fractal->transformCommon.functionEnabledBx
			&& aux.i >= fractal->transformCommon.startIterationsI
			&& aux.i < fractal->transformCommon.stopIterationsI)
		z = z - boxSize;

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
		if (z.x < xOffset) z.x = fabs(z.x - xOffset) + xOffset;

	if (fractal->transformCommon.functionEnabledy
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
		if (z.y < yOffset) z.y = fabs(z.y - yOffset) + yOffset;

	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z.x -= fractal->transformCommon.offset1;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y -= fractal->transformCommon.offsetA1;

	if (fractal->transformCommon.functionEnabledByFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
		if (z.y > z.x) swap(z.x, z.y);

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		useScale = aux.actualScaleA + fractal->transformCommon.scale2;

		z *= useScale;

		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fabs(useScale) + 1.0;
		else
			aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

		if (fractal->transformCommon.functionEnabledFFalse
				&& aux.i >= fractal->transformCommon.startIterationsK
				&& aux.i < fractal->transformCommon.stopIterationsK)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	if (aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
		z += fractal->transformCommon.offset111;

	// rotation
	if (aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * IQ-Bulb from Mandelbulb 3D and Inigo Quilez
 * @reference http://iquilezles.org/www/articles/mandelbulb/mandelbulb.htm
 */
void IqBulbIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// extract polar coordinates
	double wr = aux.r;
	double wo = acos(z.y / wr);
	double wi = atan2(z.x, z.z);

	// scale and rotate the point
	wr = pow(wr, fractal->transformCommon.pwr8 - 1.0);
	aux.DE = wr * aux.DE * fractal->transformCommon.pwr8 + 1.0;
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
 * JosLeys-Kleinian formula
 * @reference
 * http://www.fractalforums.com/3d-fractal-generation/an-escape-tim-algorithm-for-kleinian-group-limit-sets/msg98248/#msg98248
 * This formula contains aux.color and aux.DE
 */
void JosKleinianIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// sphere inversion slot#1 iter == 0
	if (fractal->transformCommon.sphereInversionEnabledFalse)
	{
		if (aux.i < 1)
		{
			double rr = 1.0;
			z += fractal->transformCommon.offset000;
			rr = z.Dot(z);
			z *= fractal->transformCommon.maxR2d1 / rr;
			z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
			aux.DE *= (fractal->transformCommon.maxR2d1 / rr) * fractal->analyticDE.scale1;
		}
	}

	// kleinian
	double a = fractal->transformCommon.foldingValue;
	double b = fractal->transformCommon.offset;
	double f = sign(b);

	CVector4 box_size = fractal->transformCommon.offset111;

	CVector3 box1 = CVector3(2.0 * box_size.x, a * box_size.y, 2.0 * box_size.z);
	CVector3 box2 = CVector3(-box_size.x, -box_size.y + 1.0, -box_size.z);
	CVector3 wrapped = wrap(z.GetXYZ(), box1, box2);

	z = CVector4(wrapped.x, wrapped.y, wrapped.z, z.w);

	// If above the separation line, rotate by 180deg about (-b/2, a/2)
	if (z.y >= a * (0.5 + 0.2 * sin(f * M_PI * (z.x + b * 0.5) / box_size.x)))
		z = CVector4(-b, a, 0., z.w) - z; // z.xy = vec2(-b, a) - z.xy;

	double rr = z.Dot(z);

	CVector4 colorVector = CVector4(z.x, z.y, z.z, rr);
	aux.color = min(aux.color, colorVector.Length()); // For coloring

	double iR = 1.0 / rr;
	z *= -iR;
	z.x = -b - z.x;
	z.y = a + z.y;

	aux.DE *= fabs(iR);
}

/**
 * JosLeys-Kleinian V2 formula
 * @reference
 * http://www.fractalforums.com/3d-fractal-generation/an-escape-tim-algorithm-for-kleinian-group-limit-sets/msg98248/#msg98248
 * This formula contains aux.color and aux.DE
 */
void JosKleinianV2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		double rr = 1.0;
		z += fractal->transformCommon.offset000;
		rr = z.Dot(z);
		z *= fractal->transformCommon.maxR2d1 / rr;
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleA1;
		// double r = sqrt(rr);
		aux.DE *= (fractal->transformCommon.maxR2d1 / rr) * fractal->analyticDE.scale1
							* fractal->transformCommon.scaleA1;
	}

	if (fractal->transformCommon.functionEnabledCyFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		CVector4 oldZ = z;
		CVector4 trigZ = CVector4(0.0, 0.0, 0.0, 0.0);
		CVector4 scaleZ = z * fractal->transformCommon.constantMultiplierC111;

		if (fractal->transformCommon.functionEnabledAx)
		{
			if (!fractal->transformCommon.functionEnabledAxFalse)
				trigZ.x = sin(scaleZ.x);
			else
				trigZ.x = cos(scaleZ.x); // scale =0, cos = 1
		}
		if (fractal->transformCommon.functionEnabledAy)
		{
			if (!fractal->transformCommon.functionEnabledAyFalse)
				trigZ.y = sin(scaleZ.y);
			else
				trigZ.y = cos(scaleZ.y);
		}
		if (fractal->transformCommon.functionEnabledAz)
		{
			if (!fractal->transformCommon.functionEnabledAzFalse)
				trigZ.z = sin(scaleZ.z);
			else
				trigZ.z = cos(scaleZ.z);
		}

		z = trigZ * fractal->transformCommon.scale;
		if (fractal->transformCommon.functionEnabledFalse)
		{
			z.x = z.x * fractal->transformCommon.scale / (fabs(oldZ.x) + 1.0);
			z.y = z.y * fractal->transformCommon.scale / (fabs(oldZ.y) + 1.0);
			z.z = z.z * fractal->transformCommon.scale / (fabs(oldZ.z) + 1.0);
			// aux.DE = aux.DE * z.Length() / oldZ.Length();
		}
	}

	if (fractal->transformCommon.functionEnabledJFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (z.z > z.x) swap(z.x, z.z);
	}

	// kleinian
	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		double a = fractal->transformCommon.foldingValue;
		double b = fractal->transformCommon.offset;
		double c = fractal->transformCommon.offsetA0;
		double f = sign(b);

		// wrap
		CVector4 box_size = fractal->transformCommon.offset111;
		CVector3 box1 = CVector3(2.0 * box_size.x, a * box_size.y, 2.0 * box_size.z);
		CVector3 box2 = CVector3(-box_size.x, -box_size.y + 1.0, -box_size.z);
		CVector3 wrapped = wrap(z.GetXYZ(), box1, box2);

		z = CVector4(wrapped.x, wrapped.y, wrapped.z, z.w);
		/*{
			z.x += box_size.x;
			z.z += box_size.z;
			z.x = z.x - 2.0 * box_size.x * floor(z.x / 2.0 * box_size.x) - box_size.x;
			z.z = z.z - 2.0 * box_size.z * floor(z.z / 2.0 * box_size.z) - box_size.z;
			z.y += box_size.y - 1.0;
			z.y = z.y - a * box_size.y * floor(z.y / a * box_size.y);
			z.y -= (box_size.y - 1.0);
		}*/

		if (z.y >= a * (0.5 + 0.2 * sin(f * M_PI * (z.x + b * 0.5) / box_size.x)))
		{
			z.x = -z.x - b;
			z.y = -z.y + a;
			z.z = -z.z - c;
		}

		double rr = z.Dot(z);

		CVector4 colorVector = CVector4(z.x, z.y, z.z, rr);
		aux.color = min(aux.color, colorVector.Length()); // For coloring

		double iR = 1.0 / rr;
		z *= -iR; // invert and mirror
		z.x = -z.x - b;
		z.y = a + z.y;
		z.z = -z.z - c;

		aux.DE *= fabs(iR);
	}

	if (fractal->transformCommon.functionEnabledEFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		z.z = sign(z.z)
					* (fractal->transformCommon.offset1 - fabs(z.z)
							+ fabs(z.z) * fractal->transformCommon.scale0);
	}
}

/**
 * Based on Kalisets1 and KaliDucks, from Mandelbulb 3D, and refer Formula proposed by Kali, with
 * features added by DarkBeam.
 *
 * <b>M3D notes:</b>
 * Try out julia and low R_bailout values of 2 down to 1!
 * You might have to cutoff at z=0 or so, to see something.
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/very-simple-formula-for-fractal-patterns
 */
void Kalisets1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

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
 * This formula contains aux.color
 */
void MandelboxMengerIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	if (fractal->mandelbox.rotationsEnabled)
	{
		CVector4 zRot;
		// cast vector to array pointer for address taking of components in opencl
		double *zRotP = (double *)&zRot;
		const double *colP = (const double *)&fractal->mandelbox.color.factor;
		for (int dim = 0; dim < 3; dim++)
		{
			// handle each dimension x, y and z sequentially in pointer var dim
			double *rotDim = (dim == 0) ? &zRotP[0] : ((dim == 1) ? &zRotP[1] : &zRotP[2]);
			const double *colorFactor = (dim == 0) ? &colP[0] : ((dim == 1) ? &colP[1] : &colP[2]);

			zRot = fractal->mandelbox.rot[0][dim].RotateVector(z);
			if (*rotDim > fractal->mandelbox.foldingLimit)
			{
				*rotDim = fractal->mandelbox.foldingValue - *rotDim;
				z = fractal->mandelbox.rotinv[0][dim].RotateVector(zRot);
				colorAdd += *colorFactor;
			}
			else
			{
				zRot = fractal->mandelbox.rot[1][dim].RotateVector(z);
				if (*rotDim < -fractal->mandelbox.foldingLimit)
				{
					*rotDim = -fractal->mandelbox.foldingValue - *rotDim;
					z = fractal->mandelbox.rotinv[1][dim].RotateVector(zRot);
					colorAdd += *colorFactor;
				}
			}
		}
	}
	else
	{
		if (aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA)
		{
			if (fabs(z.x) > fractal->mandelbox.foldingLimit)
			{
				z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
				colorAdd += fractal->mandelbox.color.factor.x;
			}
			if (fabs(z.y) > fractal->mandelbox.foldingLimit)
			{
				z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
				colorAdd += fractal->mandelbox.color.factor.y;
			}
			double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
			double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
			if (fabs(z.z) > zLimit)
			{
				z.z = sign(z.z) * zValue - z.z;
				colorAdd += fractal->mandelbox.color.factor.z;
			}
		}
	}

	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		double r2 = z.Dot(z);
		z += fractal->mandelbox.offset;
		if (r2 < fractal->mandelbox.mR2)
		{
			z *= fractal->mandelbox.mboxFactor1;
			aux.DE *= fractal->mandelbox.mboxFactor1;
			colorAdd += fractal->mandelbox.color.factorSp1;
		}
		else if (r2 < fractal->mandelbox.fR2)
		{
			double tglad_factor2 = fractal->mandelbox.fR2 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			colorAdd += fractal->mandelbox.color.factorSp2;
		}
		z -= fractal->mandelbox.offset;
	}
	if (fractal->mandelbox.mainRotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		z = z * fractal->mandelbox.scale;
		aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	if (fractal->transformCommon.functionEnabled && aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
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
			aux.DE *= fabs(fractal->transformCommon.scale3 * fractal->transformCommon.scaleA1);

			z += fractal->transformCommon.additionConstantA000;

			if (fractal->transformCommon.functionEnabledxFalse) // addCpixel options
			{
				switch (fractal->mandelbulbMulti.orderOfXYZC)
				{
					case multi_OrderOfXYZ_xyz:
					default: c = CVector4(c.x, c.y, c.z, c.w); break;
					case multi_OrderOfXYZ_xzy: c = CVector4(c.x, c.z, c.y, c.w); break;
					case multi_OrderOfXYZ_yxz: c = CVector4(c.y, c.x, c.z, c.w); break;
					case multi_OrderOfXYZ_yzx: c = CVector4(c.y, c.z, c.x, c.w); break;
					case multi_OrderOfXYZ_zxy: c = CVector4(c.z, c.x, c.y, c.w); break;
					case multi_OrderOfXYZ_zyx: c = CVector4(c.z, c.y, c.x, c.w); break;
				}
				z += c * fractal->transformCommon.constantMultiplierB111;
			}
		}
	}
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	aux.color += colorAdd;
}

/**
 * Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * Variable parameters over iteration time
 * Based on work by Tglad, Buddhi, DarkBeam
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 * This formula contains aux.color and aux.actualScaleA
 */
void MandelboxVariableIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;
	CVector4 limit4 = fractal->transformCommon.additionConstant111;
	CVector4 value4 = 2.0 * fractal->transformCommon.additionConstant111;
	if (fractal->transformCommon.functionEnabledFalse)
		value4 = fractal->transformCommon.additionConstant222;

	if (fractal->mandelbox.rotationsEnabled)
	{ // TODO evaluate implementation of vec3 limit and value
		/*	CVector4 zRot;
			// cast vector to array pointer for address taking of components in opencl
			double *zRotP = reinterpret_cast<double *>(&zRot);
			const double *colP = reinterpret_cast<const double *>(&fractal->mandelbox.color.factor);
			for (int dim = 0; dim < 3; dim++)
			{
				// handle each dimension x, y and z sequentially in pointer var dim
				double *rotDim = (dim == 0) ? &zRotP[0] : ((dim == 1) ? &zRotP[1] : &zRotP[2]);
				const double *colorFactor = (dim == 0) ? &colP[0] : ((dim == 1) ? &colP[1] : &colP[2]);

				zRot = fractal->mandelbox.rot[0][dim].RotateVector(z);
				if (*rotDim > fractal->mandelbox.foldingLimit)
				{
					*rotDim = fractal->mandelbox.foldingValue - *rotDim;
					z = fractal->mandelbox.rotinv[0][dim].RotateVector(zRot);
					aux.color += *colorFactor;
				}
				else
				{
					zRot = fractal->mandelbox.rot[1][dim].RotateVector(z);
					if (*rotDim < -fractal->mandelbox.foldingLimit)
					{
						*rotDim = -fractal->mandelbox.foldingValue - *rotDim;
						z = fractal->mandelbox.rotinv[1][dim].RotateVector(zRot);
						aux.color += *colorFactor;
					}
				}
			}*/
	}
	else
	{
		if (!fractal->transformCommon.functionEnabledCyFalse)
		{
			z = fabs(z + limit4) - fabs(z - limit4) - z;
			zCol = z;
		}
		else //  variable limit values
		{
			if (fractal->transformCommon.functionEnabledAx)
			{
				if (aux.i > fractal->transformCommon.startIterationsC)
				{
					limit4.x *= (1.0
												- 1.0
														/ (1.0
																+ (aux.i - fractal->transformCommon.startIterationsC)
																		/ fractal->transformCommon.offsetA000.x))
											* fractal->transformCommon.scale3D111.x;
				}
				z.x = fabs(z.x + limit4.x) - fabs(z.x - limit4.x) - z.x;
			}
			if (fractal->transformCommon.functionEnabledAy)
			{
				if (aux.i > fractal->transformCommon.startIterationsY)
				{
					limit4.y *= (1.0
												- 1.0
														/ (1.0
																+ (aux.i - fractal->transformCommon.startIterationsY)
																		/ fractal->transformCommon.offsetA000.y))
											* fractal->transformCommon.scale3D111.y;
				}
				z.y = fabs(z.y + limit4.y) - fabs(z.y - limit4.y) - z.y;
			}
			if (fractal->transformCommon.functionEnabledAz)
			{
				if (aux.i > fractal->transformCommon.startIterationsZ)
				{
					limit4.z *= (1.0
												- 1.0
														/ (1.0
																+ (aux.i - fractal->transformCommon.startIterationsZ)
																		/ fractal->transformCommon.offsetA000.z))
											* fractal->transformCommon.scale3D111.z;
				}
				z.z = fabs(z.z + limit4.z) - fabs(z.z - limit4.z) - z.z;
			}
			zCol = z;
		}
	}

	// spherical folding
	double maxR2use = fractal->transformCommon.maxR2d1;
	double minR2use = fractal->transformCommon.minR2p25;
	// vary maxR2
	if (fractal->transformCommon.functionEnabledEFalse)
	{
		if (aux.i > fractal->transformCommon.startIterationsA)
		{
			maxR2use *= (1.0
										- 1.0
												/ (1.0
														+ (aux.i - fractal->transformCommon.startIterationsB)
																/ fractal->transformCommon.offsetA0))
									* fractal->transformCommon.scaleA1;
		}
	}
	// vary minR2
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		if (aux.i > fractal->transformCommon.startIterationsA)
		{
			minR2use *= (1.0
										- 1.0
												/ (1.0
														+ (aux.i - fractal->transformCommon.startIterationsA)
																/ fractal->transformCommon.offset0))
									* fractal->transformCommon.scale1;
		}
	}

	const double rr = z.Dot(z);
	rrCol = rr;
	// Mandelbox Spherical fold

	z += fractal->mandelbox.offset;

	// if (r2 < 1e-21) r2 = 1e-21;
	if (rr < minR2use)
	{
		double tglad_factor1 = maxR2use / minR2use;
		z *= tglad_factor1;
		aux.DE *= tglad_factor1;
	}
	else if (rr < maxR2use)
	{
		double tglad_factor2 = maxR2use / rr;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
	}
	z -= fractal->mandelbox.offset;

	// 3D Rotation
	if (fractal->mandelbox.mainRotationEnabled) // z = fractal->mandelbox.mainRot.RotateVector(z);
	{
		CVector4 tempVC = CVector4(fractal->mandelbox.rotationMain, 0.0); // constant to be varied
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			if (aux.i >= fractal->transformCommon.startIterations
					&& aux.i < fractal->transformCommon.stopIterations
					&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations
							 != 0))
			{
				double iterationRange =
					fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations;
				double currentIteration = (aux.i - fractal->transformCommon.startIterations);
				tempVC += fractal->transformCommon.offset000 * currentIteration / iterationRange;
			}

			if (aux.i >= fractal->transformCommon.stopIterations)
			{
				tempVC += tempVC + fractal->transformCommon.offset000;
			}
		}

		tempVC *= M_PI_180;

		z = z.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), tempVC.x);
		z = z.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), tempVC.y);
		z = z.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), tempVC.z);
	}

	// scale
	double useScale = 1.0;
	{
		useScale = aux.actualScaleA + fractal->mandelbox.scale;

		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;
		if (fractal->transformCommon.functionEnabledFFalse
				&& aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}
	// add cpixel
	CVector4 c = aux.const_c;

	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
	{
		CVector4 tempC = aux.const_c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		// rotate c
		if (fractal->transformCommon.rotationEnabled
				&& aux.i >= fractal->transformCommon.startIterationsG
				&& aux.i < fractal->transformCommon.stopIterationsG)
		{
			tempC = fractal->transformCommon.rotationMatrix.RotateVector(tempC);
		}
		// vary c
		if (fractal->transformCommon.functionEnabledMFalse)
		{
			if (fractal->transformCommon.functionEnabledx)
			{
				if (aux.i > fractal->transformCommon.startIterationsM)
				{
					tempC.x *= (1.0
											 - 1.0
													 / (1.0
															 + (aux.i - fractal->transformCommon.startIterationsM)
																	 / fractal->transformCommon.offsetF000.x))
										 * fractal->transformCommon.constantMultiplierB111.x;
				}
			}
			if (fractal->transformCommon.functionEnabledy)
			{
				if (aux.i > fractal->transformCommon.startIterationsO)
				{
					tempC.y *= (1.0
											 - 1.0
													 / (1.0
															 + (aux.i - fractal->transformCommon.startIterationsO)
																	 / fractal->transformCommon.offsetF000.y))
										 * fractal->transformCommon.constantMultiplierB111.y;
				}
			}
			if (fractal->transformCommon.functionEnabledz)
			{
				if (aux.i > fractal->transformCommon.startIterationsP)
				{
					tempC.z *= (1.0
											 - 1.0
													 / (1.0
															 + (aux.i - fractal->transformCommon.startIterationsP)
																	 / fractal->transformCommon.offsetF000.z))
										 * fractal->transformCommon.constantMultiplierB111.z;
				}
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	// color updated v2.13 & mode2 v2.14
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd += fractal->mandelbox.color.factor.x
										* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
			if (zCol.y != oldZ.y)
				colorAdd += fractal->mandelbox.color.factor.y
										* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
			if (zCol.z != oldZ.z)
				colorAdd += fractal->mandelbox.color.factor.z
										* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

			if (rrCol < fractal->transformCommon.maxR2d1)
			{
				if (rrCol < fractal->transformCommon.minR2p25)
					colorAdd +=
						fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
						+ fractal->mandelbox.color.factorSp2
								* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
				else
					colorAdd +=
						fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			}
		}
		else
		{
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;

			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1;
			else if (rrCol < fractal->transformCommon.maxR2d1)
				colorAdd += fractal->mandelbox.color.factorSp2;
		}
		aux.color += colorAdd;
	}
}

/**
 * MandelbulbAbsPower2Iteration
 * @reference /http://www.fractalforums.com/gallery-b177/buffalo-fractals
 */
void MandelbulbAbsPower2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	// pre rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	// pre-offset
	if (fractal->transformCommon.functionEnabledDFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		z += fractal->transformCommon.offsetA000;
	}

	aux.DE = aux.DE * 2.0 * aux.r;

	// pre abs. abs(z.x) and abs(z.y) effect newy. abs(z.z) effects newz
	if (fractal->buffalo.preabsx) z.x = fabs(z.x);
	if (fractal->buffalo.preabsy) z.y = fabs(z.y);
	if (fractal->buffalo.preabsz) z.z = fabs(z.z);

	CVector4 zz = z * z;
	CVector4 newZ = z;
	double temp = 1.0 - zz.z / (zz.x + zz.y);
	newZ.x = (zz.x - zz.y) * temp;
	newZ.y = 2.0 * z.x * z.y * temp;
	newZ.z = (fractal->buffalo.posz ? 2.0 : -2.0) * z.z * sqrt(zz.x + zz.y);
	z = newZ;

	z.x = fractal->buffalo.absx ? fabs(z.x) : z.x;
	z.y = fractal->buffalo.absy ? fabs(z.y) : z.y;
	z.z = fractal->buffalo.absz ? fabs(z.z) : z.z;

	// offset
	if (fractal->transformCommon.functionEnabledM
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		z += fractal->transformCommon.offset000;
	}
	// rotation
	if (fractal->transformCommon.functionEnabled && aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}

	// final abs
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
	{
		if (fractal->transformCommon.functionEnabledCxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledCyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledCzFalse) z.z = fabs(z.z);
	}

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * MandelbulbAbsPower2Iteration
 */
void MandelbulbAtan2Power2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r;

	if (fractal->buffalo.preabsx) z.x = fabs(z.x);
	if (fractal->buffalo.preabsy) z.y = fabs(z.y);
	if (fractal->buffalo.preabsz) z.z = fabs(z.z);
	// bitwise ??
	CVector4 zz = z * z;
	double lenXY = sqrt(zz.x + zz.y);
	double theta1 = atan2(lenXY, z.z);
	double theta2 = -atan2(lenXY, -z.z);
	double phi = atan2(z.y, z.x) * fractal->transformCommon.scale1;

	double rr = z.Dot(z);
	double sinth = sin(2.0 * theta1);
	z.x = sinth * cos(2.0 * phi);
	z.y = sinth * sin(2.0 * phi);
	z.z = -cos(theta1 + theta2);
	z *= rr;

	z.x = fractal->buffalo.absx ? fabs(z.x) : z.x;
	z.y = fractal->buffalo.absy ? fabs(z.y) : z.y;
	z.z = fractal->buffalo.absz ? fabs(z.z) : z.z;

	z += fractal->transformCommon.additionConstantA000;
}

/**
 * mandelbulbBermarte
 *
 * abs() version of Mandelbulb Kali modified by visual.bermarte
 * @reference http://www.fractalforums.com/theory/mandelbulb-variant/
 */
void MandelbulbBermarteIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
		z = aux.r * CVector4(sinth * cos(ph0), sin(ph0) * sinth, cos(th0), 0.0);
	}
	else
	{
		th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset + 1e-061; // MUST keep exception catch
		ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
		th0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleA1;
		costh = cos(th0);
		z = aux.r * CVector4(costh * sin(ph0), cos(ph0) * costh, sin(th0), 0.0);
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
		z = zp * CVector4(sinth * cos(ph0), sin(ph0) * sinth, cos(th0), 0.0);
	}
	else
	{
		th0 = asin(z.z / aux.r) + fractal->transformCommon.betaAngleOffset
					+ 1e-061; // MUST keep exception catch ??;
		ph0 = atan2(z.y, z.x);
		ph0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleB1;
		zp = pow(aux.r, fractal->transformCommon.pwr8);
		costh = cos(th0);
		z = zp * CVector4(costh * sin(ph0), cos(ph0) * costh, sin(th0), 0.0);
	}

	if (fractal->transformCommon.functionEnabledBxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledByFalse) z.y = fabs(z.y);

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		aux.DE = pow(aux.r, fractal->transformCommon.pwr8 - fractal->analyticDE.offset1) * aux.DE
							 * fractal->transformCommon.pwr8 * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset2;
	}
	else // default, i.e. scale1 & offset1 & offset2 = 1.0
	{
		aux.DE =
			pow(aux.r, fractal->transformCommon.pwr8 - 1.0) * fractal->transformCommon.pwr8 * aux.DE
			+ 1.0;
	}
}
/**
 * mandelbulb juliabulb hybrid 3D
 * constructed from Mandelbulb, Msltoe - Julia Bulb Eiffie & Msltoe - Sym4 Mod formulas.
 * note: an Offset Radius of 0.1 can sometimes improve the DE statistic
 */
void MandelbulbJuliabulbIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// mandelbulb multi
	if (aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		aux.r = z.Length();
		if (fractal->transformCommon.functionEnabledFalse)
		{
			if (fractal->transformCommon.functionEnabledAxFalse
					&& aux.i >= fractal->transformCommon.startIterationsX
					&& aux.i < fractal->transformCommon.stopIterationsX)
				z.x = fabs(z.x);
			if (fractal->transformCommon.functionEnabledAyFalse
					&& aux.i >= fractal->transformCommon.startIterationsY
					&& aux.i < fractal->transformCommon.stopIterationsY)
				z.y = fabs(z.y);
			if (fractal->transformCommon.functionEnabledAzFalse
					&& aux.i >= fractal->transformCommon.startIterationsZ
					&& aux.i < fractal->transformCommon.stopIterationsZ)
				z.z = fabs(z.z);
		}

		double th0 = fractal->bulb.betaAngleOffset;
		double ph0 = fractal->bulb.alphaAngleOffset;

		CVector3 v;
		switch (fractal->sinTan2Trig.orderOfZYX)
		{
			case multi_OrderOfZYX_zyx:
			default: v = CVector3(z.z, z.y, z.x); break;
			case multi_OrderOfZYX_zxy: v = CVector3(z.z, z.x, z.y); break;
			case multi_OrderOfZYX_yzx: v = CVector3(z.y, z.z, z.x); break;
			case multi_OrderOfZYX_yxz: v = CVector3(z.y, z.x, z.z); break;
			case multi_OrderOfZYX_xzy: v = CVector3(z.x, z.z, z.y); break;
			case multi_OrderOfZYX_xyz: v = CVector3(z.x, z.y, z.z); break;
		}

		if (fractal->sinTan2Trig.asinOrAcos == multi_asinOrAcos_asin)
			th0 += asin(v.x / aux.r);
		else
			th0 += acos(v.x / aux.r);

		if (fractal->sinTan2Trig.atan2OrAtan == multi_atan2OrAtan_atan2)
			ph0 += atan2(v.y, v.z);
		else
			ph0 += atan(v.y / v.z);

		double rp = pow(aux.r, fractal->bulb.power - 1.0);
		double th = th0 * fractal->bulb.power * fractal->transformCommon.scaleA1;
		double ph = ph0 * fractal->bulb.power * fractal->transformCommon.scaleB1;

		aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;
		rp *= aux.r;

		if (fractal->transformCommon.functionEnabledxFalse)
		{ // cosine mode
			double sinth = th;
			if (fractal->transformCommon.functionEnabledyFalse) sinth = th0;
			sinth = sin(sinth);
			z = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		else
		{ // sine mode
			double costh = th;
			if (fractal->transformCommon.functionEnabledzFalse) costh = th0;
			costh = cos(costh);
			z = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}
	}

	// sym4
	if (fractal->transformCommon.functionEnabled && aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;
		CVector4 temp = z;
		double tempL = temp.Length();
		// if (tempL < 1e-21)
		//	tempL = 1e-21;
		z *= fractal->transformCommon.scale3D111;

		aux.DE *= fabs(z.Length() / tempL);

		if (fabs(z.x) < fabs(z.z)) swap(z.x, z.z);
		if (fabs(z.x) < fabs(z.y)) swap(z.x, z.y);
		if (fabs(z.y) < fabs(z.z)) swap(z.y, z.z);

		if (z.x * z.z < 0.0) z.z = -z.z;
		if (z.x * z.y < 0.0) z.y = -z.y;

		temp.x = z.x * z.x - z.y * z.y - z.z * z.z;
		temp.y = 2.0 * z.x * z.y;
		temp.z = 2.0 * z.x * z.z;

		z = temp + fractal->transformCommon.offsetF000;
	}

	// sym3 msltoe eiffie
	if (fractal->transformCommon.functionEnabledEFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		aux.r = z.Length();
		double psi = fabs(fmod(atan2(z.z, z.y) + M_PI + M_PI_8, M_PI_4) - M_PI_8);
		double lengthYZ = sqrt(z.y * z.y + z.z * z.z);

		z.y = cos(psi) * lengthYZ;
		z.z = sin(psi) * lengthYZ;
		aux.DE = aux.DE * 2.0 * aux.r;

		CVector4 z2 = z * z;
		double rr = z2.x + z2.y + z2.z;
		double m = 1.0 - z2.z / rr;
		CVector4 temp;
		temp.x = (z2.x - z2.y) * m;
		temp.y = 2.0 * z.x * z.y * m * fractal->transformCommon.scale; // scaling y;;
		temp.z = 2.0 * z.z * sqrt(z2.x + z2.y);
		temp.w = z.w;
		z = temp + fractal->transformCommon.additionConstant000;
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 c = aux.const_c;
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}
	// radial offset
	double lengthTempZ = -z.Length();
	// if (lengthTempZ > -1e-21)
	//	lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
	// scale
	z *= fractal->transformCommon.scale1;
	aux.DE *= fabs(fractal->transformCommon.scale1);
}
/**
 * mandelbulb Kali modification
 * @reference http://www.fractalforums.com/theory/mandelbulb-variant/
 */
void MandelbulbKaliIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
	z = aux.r * CVector4(sinth * cos(ph0), sin(ph0) * sinth, cos(th0), 0.0);

	th0 = acos(z.z / aux.r) + fractal->transformCommon.betaAngleOffset
				+ 1e-061; // MUST keep exception catch ??;
	ph0 = atan(z.y / z.x);
	ph0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleB1;
	double zp = pow(aux.r, fractal->transformCommon.pwr8);
	sinth = sin(th0);
	z = zp * CVector4(sinth * cos(ph0), sin(ph0) * sinth, cos(th0), 0.0);

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		aux.DE = pow(aux.r, fractal->transformCommon.pwr8 - fractal->analyticDE.offset1) * aux.DE
							 * fractal->transformCommon.pwr8 * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset2;
	}
	else // default, i.e. scale1 & offset1 & offset2 = 1.0
	{
		aux.DE =
			pow(aux.r, fractal->transformCommon.pwr8 - 1.0) * fractal->transformCommon.pwr8 * aux.DE
			+ 1.0;
	}
}

/**
 * based on mandelbulb Kali multi
 * @reference http://www.fractalforums.com/theory/mandelbulb-variant/
 */
void MandelbulbKaliMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

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
	CVector4 v;
	switch (fractal->mandelbulbMulti.orderOfXYZ)
	{
		case multi_OrderOfXYZ_xyz:
		default: v = CVector4(z.x, z.y, z.z, z.w); break;
		case multi_OrderOfXYZ_xzy: v = CVector4(z.x, z.z, z.y, z.w); break;
		case multi_OrderOfXYZ_yxz: v = CVector4(z.y, z.x, z.z, z.w); break;
		case multi_OrderOfXYZ_yzx: v = CVector4(z.y, z.z, z.x, z.w); break;
		case multi_OrderOfXYZ_zxy: v = CVector4(z.z, z.x, z.y, z.w); break;
		case multi_OrderOfXYZ_zyx: v = CVector4(z.z, z.y, z.x, z.w); break;
	}

	if (fractal->mandelbulbMulti.acosOrAsin == multi_acosOrAsin_acos)
		th0 += acos(v.x / aux.r);
	else
		th0 += asin(v.x / aux.r);
	if (fractal->mandelbulbMulti.atanOrAtan2 == multi_atanOrAtan2_atan)
		ph0 += atan(v.y / v.z);
	else
		ph0 += atan2(v.y, v.z);

	th0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleA1;

	if (fractal->transformCommon.functionEnabledBxFalse)
	{
		costh = cos(th0);
		z = aux.r * CVector4(costh * sin(ph0), cos(ph0) * costh, sin(th0), 0.0);
	}
	else
	{ // cosine mode default
		sinth = sin(th0);
		z = aux.r * CVector4(sinth * cos(ph0), sin(ph0) * sinth, cos(th0), 0.0);
	}

	if (fractal->transformCommon.functionEnabledxFalse)
	{
		switch (fractal->mandelbulbMulti.orderOfXYZ2)
		{
			case multi_OrderOfXYZ_xyz:
			default: v = CVector4(z.x, z.y, z.z, z.w); break;
			case multi_OrderOfXYZ_xzy: v = CVector4(z.x, z.z, z.y, z.w); break;
			case multi_OrderOfXYZ_yxz: v = CVector4(z.y, z.x, z.z, z.w); break;
			case multi_OrderOfXYZ_yzx: v = CVector4(z.y, z.z, z.x, z.w); break;
			case multi_OrderOfXYZ_zxy: v = CVector4(z.z, z.x, z.y, z.w); break;
			case multi_OrderOfXYZ_zyx: v = CVector4(z.z, z.y, z.x, z.w); break;
		}
		if (fractal->mandelbulbMulti.acosOrAsinA == multi_acosOrAsin_acos)
			th0 = acos(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						+ 1e-061; // MUST keep exception catch
		else
			th0 += asin(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						 + 1e-061; // MUST keep exception catch;

		if (fractal->mandelbulbMulti.atanOrAtan2A == multi_atanOrAtan2_atan)
			ph0 += atan(v.y / v.z);
		else
			ph0 += atan2(v.y, v.z);
	}
	else
	{
		switch (fractal->mandelbulbMulti.orderOfXYZ)
		{
			case multi_OrderOfXYZ_xyz:
			default: v = CVector4(z.x, z.y, z.z, z.w); break;
			case multi_OrderOfXYZ_xzy: v = CVector4(z.x, z.z, z.y, z.w); break;
			case multi_OrderOfXYZ_yxz: v = CVector4(z.y, z.x, z.z, z.w); break;
			case multi_OrderOfXYZ_yzx: v = CVector4(z.y, z.z, z.x, z.w); break;
			case multi_OrderOfXYZ_zxy: v = CVector4(z.z, z.x, z.y, z.w); break;
			case multi_OrderOfXYZ_zyx: v = CVector4(z.z, z.y, z.x, z.w); break;
		}

		if (fractal->mandelbulbMulti.acosOrAsin == multi_acosOrAsin_acos)
			th0 = acos(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						+ 1e-061; // MUST keep exception catch ??;
		else
			th0 += asin(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						 + 1e-061; // MUST keep exception catch ??;

		if (fractal->mandelbulbMulti.atanOrAtan2 == multi_atanOrAtan2_atan)
			ph0 += atan(v.y / v.z);
		else
			ph0 += atan2(v.y, v.z);
	}

	ph0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleB1 * 0.5; // 0.5 retain
	double zp = pow(aux.r, fractal->transformCommon.pwr8);

	if (fractal->transformCommon.functionEnabledzFalse)
	{ // sine mode
		costh = cos(th0);
		z = zp * CVector4(costh * sin(ph0), cos(ph0) * costh, sin(th0), 0.0);
	}
	else
	{ // cosine mode default
		sinth = sin(th0);
		z = zp * CVector4(sinth * cos(ph0), sin(ph0) * sinth, cos(th0), 0.0);
	}

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		aux.DE = pow(aux.r, fractal->transformCommon.pwr8 - fractal->analyticDE.offset1) * aux.DE
							 * fractal->transformCommon.pwr8 * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset2;
	}
	else // default, i.e. scale1 & offset1 & offset2 = 1.0
	{
		aux.DE =
			pow(aux.r, fractal->transformCommon.pwr8 - 1.0) * fractal->transformCommon.pwr8 * aux.DE
			+ 1.0;
	}
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}
}

/**
 * mandelbulbMulti 3D
 */
void MandelbulbMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}
	double th0 = fractal->bulb.betaAngleOffset;
	double ph0 = fractal->bulb.alphaAngleOffset;
	CVector4 v;

	switch (fractal->mandelbulbMulti.orderOfXYZ)
	{
		case multi_OrderOfXYZ_xyz:
		default: v = CVector4(z.x, z.y, z.z, z.w); break;
		case multi_OrderOfXYZ_xzy: v = CVector4(z.x, z.z, z.y, z.w); break;
		case multi_OrderOfXYZ_yxz: v = CVector4(z.y, z.x, z.z, z.w); break;
		case multi_OrderOfXYZ_yzx: v = CVector4(z.y, z.z, z.x, z.w); break;
		case multi_OrderOfXYZ_zxy: v = CVector4(z.z, z.x, z.y, z.w); break;
		case multi_OrderOfXYZ_zyx: v = CVector4(z.z, z.y, z.x, z.w); break;
	}
	// if (aux.r < 1e-21)
	//	aux.r = 1e-21;
	// if (v3 < 1e-21 && v3 > -1e-21)
	//	v3 = (v3 > 0) ? 1e-21 : -1e-21;

	if (fractal->mandelbulbMulti.acosOrAsin == multi_acosOrAsin_acos)
		th0 += acos(v.x / aux.r);
	else
		th0 += asin(v.x / aux.r);

	if (fractal->mandelbulbMulti.atanOrAtan2 == multi_atanOrAtan2_atan)
		ph0 += atan(v.y / v.z);
	else
		ph0 += atan2(v.y, v.z);

	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	double th = th0 * fractal->bulb.power * fractal->transformCommon.scaleA1;
	double ph = ph0 * fractal->bulb.power * fractal->transformCommon.scaleB1;

	aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;
	rp *= aux.r;

	if (fractal->transformCommon.functionEnabledxFalse)
	{ // cosine mode
		double sinth = th;
		if (fractal->transformCommon.functionEnabledyFalse) sinth = th0;
		sinth = sin(sinth);
		z = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
	}
	else
	{ // sine mode ( default = V2.07))
		double costh = th;
		if (fractal->transformCommon.functionEnabledzFalse) costh = th0;
		costh = cos(costh);
		z = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}
}
/**
 * mandelbulbMulti2 3D
 */
void MandelbulbMulti2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse
				&& aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
			z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse
				&& aux.i >= fractal->transformCommon.startIterationsY
				&& aux.i < fractal->transformCommon.stopIterationsY)
			z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse
				&& aux.i >= fractal->transformCommon.startIterationsZ
				&& aux.i < fractal->transformCommon.stopIterationsZ)
			z.z = fabs(z.z);
	}

	double th0 = fractal->bulb.betaAngleOffset;
	double ph0 = fractal->bulb.alphaAngleOffset;

	CVector3 v;
	switch (fractal->sinTan2Trig.orderOfZYX)
	{
		case multi_OrderOfZYX_zyx:
		default: v = CVector3(z.z, z.y, z.x); break;
		case multi_OrderOfZYX_zxy: v = CVector3(z.z, z.x, z.y); break;
		case multi_OrderOfZYX_yzx: v = CVector3(z.y, z.z, z.x); break;
		case multi_OrderOfZYX_yxz: v = CVector3(z.y, z.x, z.z); break;
		case multi_OrderOfZYX_xzy: v = CVector3(z.x, z.z, z.y); break;
		case multi_OrderOfZYX_xyz: v = CVector3(z.x, z.y, z.z); break;
	}

	if (fractal->sinTan2Trig.asinOrAcos == multi_asinOrAcos_asin)
		th0 += asin(v.x / aux.r);
	else
		th0 += acos(v.x / aux.r);

	if (fractal->sinTan2Trig.atan2OrAtan == multi_atan2OrAtan_atan2)
		ph0 += atan2(v.y, v.z);
	else
		ph0 += atan(v.y / v.z);

	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	double th = th0 * fractal->bulb.power * fractal->transformCommon.scaleA1;
	double ph = ph0 * fractal->bulb.power * fractal->transformCommon.scaleB1;

	aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;
	rp *= aux.r;

	if (fractal->transformCommon.functionEnabledxFalse)
	{ // cosine mode
		double sinth = th;
		if (fractal->transformCommon.functionEnabledyFalse) sinth = th0;
		sinth = sin(sinth);
		z = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
	}
	else
	{ // sine mode ( default = V2.07))
		double costh = th;
		if (fractal->transformCommon.functionEnabledzFalse) costh = th0;
		costh = cos(costh);
		z = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
	}

	if (fractal->transformCommon.functionEnabledKFalse)
	{
		if (fractal->transformCommon.functionEnabledDFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD)
			swap(z.x, z.y);
		if (fractal->transformCommon.functionEnabledEFalse
				&& aux.i >= fractal->transformCommon.startIterationsE
				&& aux.i < fractal->transformCommon.stopIterationsE)
			swap(z.x, z.z);

		// swap
		if (fractal->transformCommon.functionEnabledBxFalse) z.x = -z.x;
		if (fractal->transformCommon.functionEnabledByFalse) z.y = -z.y;
		if (fractal->transformCommon.functionEnabledBzFalse) z.z = -z.z;
	}
	z += fractal->transformCommon.additionConstant000;
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
}

/**
 * MandelbulbPow2 v2  buffalo , Quick Dudley types and makin3D-2
 * @reference http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/
 * and http://www.fractalgallery.co.uk/ and https://www.facebook.com/david.makin.7
 */
void MandelbulbPow2V2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// Q_UNUSED(aux);

	aux.DE = aux.DE * aux.r * 2.0;
	CVector4 c = aux.const_c;
	//  pre-abs
	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (fractal->transformCommon.functionEnabledCxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledCyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledCzFalse) z.z = fabs(z.z);
	}
	// swap axis
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		CVector4 v = z;
		switch (fractal->mandelbulbMulti.orderOfXYZ2)
		{
			case multi_OrderOfXYZ_xyz:
			default: v = CVector4(z.x, z.y, z.z, z.w); break;
			case multi_OrderOfXYZ_xzy: v = CVector4(z.x, z.z, z.y, z.w); break;
			case multi_OrderOfXYZ_yxz: v = CVector4(z.y, z.x, z.z, z.w); break;
			case multi_OrderOfXYZ_yzx: v = CVector4(z.y, z.z, z.x, z.w); break;
			case multi_OrderOfXYZ_zxy: v = CVector4(z.z, z.x, z.y, z.w); break;
			case multi_OrderOfXYZ_zyx: v = CVector4(z.z, z.y, z.x, z.w); break;
		}
		z = v;
	}

	// Pow2
	CVector4 oldZ = z;

	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 zz = z * z;
		if (fractal->transformCommon.functionEnabledDFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD)
			zz *= fractal->transformCommon.constantMultiplierA111;
		CVector4 Scale2 = CVector4(2.0, 2.0, 2.0, 1.0);
		if (fractal->transformCommon.functionEnabledEFalse
				&& aux.i >= fractal->transformCommon.startIterationsE
				&& aux.i < fractal->transformCommon.stopIterationsE)
			Scale2 = fractal->transformCommon.constantMultiplier222;

		CVector4 v4 = z;
		switch (fractal->combo4.combo4)
		{
			case multi_combo4_type1:
			default:
				if (fractal->transformCommon.functionEnabledXFalse)
					v4.x = fabs(zz.x - zz.y - zz.z) * -Scale2.x * 0.5;
				else
					v4.x = (zz.x - zz.y - zz.z) * -Scale2.x * 0.5;
				oldZ = fabs(oldZ);
				v4.y = oldZ.x * oldZ.y * -Scale2.y;
				v4.z = oldZ.x * oldZ.z * -Scale2.z;
				break;
			// lkmitch/quick dudley type
			case multi_combo4_type2:
				v4.x = zz.x - Scale2.x * oldZ.y * oldZ.z;
				v4.y = zz.z + Scale2.y * oldZ.x * oldZ.y;
				v4.z = zz.y - Scale2.z * oldZ.x * oldZ.z;
				break;
			// makin 3D-2 type
			case multi_combo4_type3:
				v4.x = zz.x + Scale2.x * oldZ.y * oldZ.z;
				v4.y = -zz.y - Scale2.y * oldZ.x * oldZ.z;
				v4.z = -zz.z + Scale2.z * oldZ.x * oldZ.y;
				aux.DE += 1.0;
				break;
			// buffalo V2
			case multi_combo4_type4:
				oldZ = fabs(oldZ);
				v4.x = (zz.x - zz.y - zz.z) * Scale2.x * 0.5 - oldZ.x;
				v4.y = oldZ.x * oldZ.y * Scale2.y - oldZ.y;
				v4.z = oldZ.x * oldZ.z * Scale2.z - oldZ.z;
				break;
		}
		z = v4;
	}

	//  post abs
	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		if (fractal->transformCommon.functionEnabledBxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledByFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledBzFalse) z.z = fabs(z.z);
	}

	// offset or juliaC
	if (aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		z += fractal->transformCommon.offset000;
	}

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	// rotation
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * mix integer powers 1234
 */
void MandelbulbPower1234Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse
				&& aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
			z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse
				&& aux.i >= fractal->transformCommon.startIterationsY
				&& aux.i < fractal->transformCommon.stopIterationsY)
			z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse
				&& aux.i >= fractal->transformCommon.startIterationsZ
				&& aux.i < fractal->transformCommon.stopIterationsZ)
			z.z = fabs(z.z);
	}
	double r = aux.r;
	double de1 = 0.0;
	double de2 = 0.0;
	double de3 = 0.0;
	double de4 = 0.0;
	CVector4 zer0s = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 newZ1 = zer0s;
	CVector4 newZ2 = zer0s;
	CVector4 newZ3 = zer0s;
	CVector4 newZ4 = zer0s;

	double th0 = fractal->bulb.betaAngleOffset;
	double ph0 = fractal->bulb.alphaAngleOffset;

	if (!fractal->transformCommon.functionEnabledxFalse)
	{
		th0 += asin(z.z / r);
		ph0 += atan2(z.y, z.x);
	}
	else
	{
		th0 += acos(z.z / r);
		ph0 += atan(z.y / z.x);
	}

	CVector4 w8ts = fractal->transformCommon.offset1111;
	double rp = r;

	if (fractal->transformCommon.functionEnabledx) // 0ne
	{
		newZ1 = z * w8ts.x;
		de1 = w8ts.x;
	}

	if (fractal->transformCommon.functionEnabledy) // two
	{
		de2 = (rp * aux.DE * 2.0 + 1.0) * w8ts.y;
		rp *= r;
		double th = th0 * 2.0 * fractal->transformCommon.scaleA1;
		double ph = ph0 * 2.0 * fractal->transformCommon.scaleB1;

		if (!fractal->transformCommon.functionEnabledxFalse)
		{
			double costh = cos(th);
			newZ2 = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}
		else
		{
			double sinth = sin(th);
			newZ2 = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		newZ2 *= w8ts.y;
	}

	if (fractal->transformCommon.functionEnabledz) // three
	{
		rp = r * r;
		de3 = (rp * aux.DE * 3.0 + 1.0) * w8ts.z;
		rp *= r;
		double th = th0 * 3.0 * fractal->transformCommon.scaleA1;
		double ph = ph0 * 3.0 * fractal->transformCommon.scaleB1;

		if (!fractal->transformCommon.functionEnabledxFalse)
		{
			double costh = cos(th);
			newZ3 = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}
		else
		{
			double sinth = sin(th);
			newZ3 = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		newZ3 *= w8ts.z;
	}

	if (fractal->transformCommon.functionEnabledw) // four
	{
		rp = r * r * r;
		de4 = (rp * aux.DE * 4.0 + 1.0) * w8ts.w;
		rp *= r;
		double th = th0 * 4.0 * fractal->transformCommon.scaleA1;
		double ph = ph0 * 4.0 * fractal->transformCommon.scaleB1;

		if (!fractal->transformCommon.functionEnabledxFalse)
		{
			double costh = cos(th);
			newZ4 = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}
		else
		{
			double sinth = sin(th);
			newZ4 = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		newZ4 *= w8ts.w;
	}
	z = newZ1 + newZ2 + newZ3 + newZ4;
	aux.DE = fabs(de1 + de2 + de3 + de4);

	if (fractal->transformCommon.functionEnabledKFalse)
	{
		if (fractal->transformCommon.functionEnabledDFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD)
			swap(z.x, z.y);
		if (fractal->transformCommon.functionEnabledEFalse
				&& aux.i >= fractal->transformCommon.startIterationsE
				&& aux.i < fractal->transformCommon.stopIterationsE)
			swap(z.x, z.z);

		// swap
		if (fractal->transformCommon.functionEnabledBxFalse) z.x = -z.x;
		if (fractal->transformCommon.functionEnabledByFalse) z.y = -z.y;
		if (fractal->transformCommon.functionEnabledBzFalse) z.z = -z.z;
	}

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * mandelbulb Quaternion
 *
 */
void MandelbulbQuatIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		CVector4 c = aux.const_c;
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}

	// Quaternion fold
	if (aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		aux.r = z.Length();
		z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, 0.0);
		aux.DE = aux.DE * 2.0 * aux.r;
		double tempL = z.Length();
		z *= fractal->transformCommon.constantMultiplier122;
		// if (tempL < 1e-21) tempL = 1e-21;
		CVector3 tempAvgScale = CVector3(z.x, z.y / 2.0, z.z / 2.0);
		double avgScale = tempAvgScale.Length() / tempL;
		double tempAux = aux.DE * avgScale;
		aux.DE = aux.DE + (tempAux - aux.DE) * fractal->analyticDE.scale1;
		z += fractal->transformCommon.offset000;
		// if (!fractal->analyticDE.enabledFalse)
		//	aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;
		// else
		//	aux.DE =
		//		aux.DE * fabs(aux.actualScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
	// sym4
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;
		CVector4 temp = z;
		double tempL = temp.Length();
		// if (tempL < 1e-21)
		//	tempL = 1e-21;
		z *= fractal->transformCommon.scale3D111;

		aux.DE *= z.Length() / tempL;

		if (fabs(z.x) < fabs(z.z)) swap(z.x, z.z);
		if (fabs(z.x) < fabs(z.y)) swap(z.x, z.y);
		if (fabs(z.y) < fabs(z.z)) swap(z.y, z.z);

		if (z.x * z.z < 0.0) z.z = -z.z;
		if (z.x * z.y < 0.0) z.y = -z.y;

		temp.x = z.x * z.x - z.y * z.y - z.z * z.z;
		temp.y = 2.0 * z.x * z.y;
		temp.z = 2.0 * z.x * z.z;

		z = temp + fractal->transformCommon.offsetF000;
		// radial offset
		double lengthTempZ = -z.Length();
		// if (lengthTempZ > -1e-21)
		//	lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;

		// scale
		z *= fractal->transformCommon.scale1;
		aux.DE *= fabs(fractal->transformCommon.scale1);
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	// mandelbulb multi
	if (aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		aux.r = z.Length();
		if (fractal->transformCommon.functionEnabledFalse)
		{
			if (fractal->transformCommon.functionEnabledAxFalse
					&& aux.i >= fractal->transformCommon.startIterationsX
					&& aux.i < fractal->transformCommon.stopIterationsX)
				z.x = fabs(z.x);
			if (fractal->transformCommon.functionEnabledAyFalse
					&& aux.i >= fractal->transformCommon.startIterationsY
					&& aux.i < fractal->transformCommon.stopIterationsY)
				z.y = fabs(z.y);
			if (fractal->transformCommon.functionEnabledAzFalse
					&& aux.i >= fractal->transformCommon.startIterationsZ
					&& aux.i < fractal->transformCommon.stopIterationsZ)
				z.z = fabs(z.z);
		}

		double th0 = fractal->bulb.betaAngleOffset;
		double ph0 = fractal->bulb.alphaAngleOffset;

		CVector3 v;
		switch (fractal->sinTan2Trig.orderOfZYX)
		{
			case multi_OrderOfZYX_zyx:
			default: v = CVector3(z.z, z.y, z.x); break;
			case multi_OrderOfZYX_zxy: v = CVector3(z.z, z.x, z.y); break;
			case multi_OrderOfZYX_yzx: v = CVector3(z.y, z.z, z.x); break;
			case multi_OrderOfZYX_yxz: v = CVector3(z.y, z.x, z.z); break;
			case multi_OrderOfZYX_xzy: v = CVector3(z.x, z.z, z.y); break;
			case multi_OrderOfZYX_xyz: v = CVector3(z.x, z.y, z.z); break;
		}

		if (fractal->sinTan2Trig.asinOrAcos == multi_asinOrAcos_asin)
			th0 += asin(v.x / aux.r);
		else
			th0 += acos(v.x / aux.r);

		if (fractal->sinTan2Trig.atan2OrAtan == multi_atan2OrAtan_atan2)
			ph0 += atan2(v.y, v.z);
		else
			ph0 += atan(v.y / v.z);

		double rp = pow(aux.r, fractal->bulb.power - 1.0);
		double th = th0 * fractal->bulb.power * fractal->transformCommon.scaleA1;
		double ph = ph0 * fractal->bulb.power * fractal->transformCommon.scaleB1;

		aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;
		rp *= aux.r;

		if (fractal->transformCommon.functionEnabledxFalse)
		{ // cosine mode
			double sinth = th;
			if (fractal->transformCommon.functionEnabledyFalse) sinth = th0;
			sinth = sin(sinth);
			z = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		else
		{ // sine mode
			double costh = th;
			if (fractal->transformCommon.functionEnabledzFalse) costh = th0;
			costh = cos(costh);
			z = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}

		if (fractal->transformCommon.functionEnabledKFalse)
		{
			if (fractal->transformCommon.functionEnabledDFalse) swap(z.x, z.y);
			if (fractal->transformCommon.functionEnabledEFalse) swap(z.x, z.z);

			// swap
			if (fractal->transformCommon.functionEnabledBxFalse) z.x = -z.x;
			if (fractal->transformCommon.functionEnabledByFalse) z.y = -z.y;
			if (fractal->transformCommon.functionEnabledBzFalse) z.z = -z.z;
		}
	}
	z += fractal->transformCommon.additionConstant000;
}

/**
 * mandelbulb vary scaleV1
 */
void MandelbulbVaryPowerV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double tempVC = fractal->bulb.power; // constant to be varied

	if (aux.i >= fractal->transformCommon.startIterations250
			&& aux.i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
	{
		int iterationRange =
			fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250;
		int currentIteration = (aux.i - fractal->transformCommon.startIterations250);
		tempVC += fractal->transformCommon.offset0 * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset0);
	}

	// if (aux.r < 1e-21)
	//	aux.r = 1e-21;
	double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, tempVC - 1.0);
	double th = th0 * tempVC;
	double ph = ph0 * tempVC;
	double cth = cos(th);
	aux.DE = rp * aux.DE * tempVC + 1.0;
	rp *= aux.r;
	z = CVector4(cth * cos(ph), cth * sin(ph), sin(th), 0.0) * rp;
}

/**
 * Mandeltorus by Aexion
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/mandeldonuts/
 */
void MandeltorusIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1) // pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.DE *= z.Length() / aux.r;
	}

	const double power1 = fractal->transformCommon.pwr8;	// Longitude power, symmetry
	const double power2 = fractal->transformCommon.pwr8a; // Latitude power

	const double rh = sqrt(z.x * z.x + z.z * z.z);
	double rh1 = 0.0;
	double rh2 = 0.0;
	const double phi = atan2(z.z, z.x);
	const double phipow = phi * power1;

	const double theta = atan2(rh, z.y);

	if (!fractal->transformCommon.functionEnabledzFalse) // mode 1
	{
		const double thetapow = theta * power2; // mode1

		const double px = z.x - cos(phi) * 1.5;
		const double pz = z.z - sin(phi) * 1.5;

		const double rhrad = sqrt(px * px + pz * pz + z.y * z.y);

		rh1 = pow(rhrad, power2);
		rh2 = pow(rhrad, power1);

		const double sintheta = sin(thetapow) * rh2; // mode1

		z.x = sintheta * cos(phipow);
		z.z = sintheta * sin(phipow);
		z.y = cos(thetapow) * rh1; // mode 1
	}
	else // mode 2
	{
		const double px = z.x - cos(phi) * 1.5;
		const double pz = z.z - sin(phi) * 1.5;

		const double rhrad = sqrt(px * px + pz * pz + z.y * z.y);

		const double tangle = atan2(sqrt(px * px + pz * pz), z.y) * power2; // mode2

		rh1 = pow(rhrad, power2);
		rh2 = pow(rhrad, power1);

		const double sintheta = (1.5 + cos(tangle)) * rh2; // mode2
		z.x = sintheta * cos(phipow);
		z.z = sintheta * sin(phipow);
		z.y = sin(tangle) * rh1; // mode 2
	}

	// DEcalc
	double temp = rh2 * (power1 - fractal->analyticDE.offset2);
	if (fractal->transformCommon.functionEnabledAyFalse)
		temp = min(temp, rh1 * (power2 - fractal->analyticDE.offset2));

	if (!fractal->analyticDE.enabledFalse)
	{
		aux.DE = temp * aux.DE + 1.0;
	}
	else
	{
		aux.DE = temp * aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
}

/**
 * Menger Cross KIFS
 * from code by Knighty
 * http://www.fractalforums.com/fragmentarium/
 * cross-menger!-can-anyone-do-this/msg93972/#new
 */
void MengerCrossKIFSIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
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
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);

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

	if (fractal->transformCommon.functionEnabled && aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{ // CrossMengerKIFS
		double lengthL;
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
		CVector4 edge = fractal->transformCommon.offset222;
		if (fractal->transformCommon.functionEnabledxFalse)
		{
			z.x = edge.x - z.x;
			z.y = edge.y - z.y;
			z.z = edge.z - z.z;
		}

		z *= fractal->transformCommon.scale3;
		z -= CVector4(SQRT_3_4, 1.5, 1.5, 0.0) * (fractal->transformCommon.scale3 - 1.0);
		aux.DE *= fractal->transformCommon.scale3;

		z += fractal->transformCommon.offset000; // post offset
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			z.x = fabs(z.x + fractal->transformCommon.offset) + fractal->transformCommon.offset0;
		}
		if (fractal->transformCommon.functionEnabledRFalse
				&& aux.i >= fractal->transformCommon.startIterationsR
				&& aux.i < fractal->transformCommon.stopIterationsR)
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
void MengerCrossMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 gap = fractal->transformCommon.constantMultiplier000;

	if (fractal->transformCommon.functionEnabledx && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledFFalse) z.x = fabs(z.x);
		double dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		double t = max(0.0, dot1);
		z.x -= t * -SQRT_3 - (0.5 * SQRT_3_4);

		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z.y -= 1.5;
		z -= gap * CVector4(SQRT_3_4, -1.5, 1.5, 0.0);

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

	if (fractal->transformCommon.functionEnabledy
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{ // CrossMengerTrick
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledzFalse) z.x = fabs(z.x);

		double dot1 = (z.x * -SQRT_3_4 + z.y * 0.5);
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
				&& aux.i >= fractal->transformCommon.startIterationsR
				&& aux.i < fractal->transformCommon.stopIterationsR)
		{
			z = fractal->transformCommon.rotationMatrix2.RotateVector(z);
		}
	}
}

/**
 * Menger Sponge formula modified by Mclarekin
 * from code by Knighty
 */
void MengerMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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

	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	z += fractal->transformCommon.additionConstant000;

	// boxoffset
	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box offset
	{
		CVector4 temp = z;
		z.x = sign(z.x) * fractal->transformCommon.additionConstantA000.x + z.x;
		z.y = sign(z.y) * fractal->transformCommon.additionConstantA000.y + z.y;
		z.z = sign(z.z) * fractal->transformCommon.additionConstantA000.z + z.z;

		if (fractal->transformCommon.functionEnabledFalse)
		{
			double tempL = temp.Length();
			// if (tempL < 1e-21) tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			aux.DE = aux.DE * avgScale + 1.0;
		}
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector4 zA = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
		CVector4 zB = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offset)
				+ (zB * fractal->transformCommon.offset0);
		aux.DE *= fractal->transformCommon.scale1;
	}
}

/**
 * Menger Middle Mod
 */
void MengerMiddleModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		double rr = 1.0;
		z += fractal->transformCommon.offset000;
		rr = z.Dot(z);
		z *= fractal->transformCommon.maxR2d1 / rr;
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleB1;
		// double r = sqrt(rr);
		aux.DE *= (fractal->transformCommon.maxR2d1 / rr) * fractal->analyticDE.scale1
							* fractal->transformCommon.scaleB1;
	}

	// fabs() and menger fold
	z = fabs(z + fractal->transformCommon.additionConstantA000);
	if (z.x - z.y < 0.0) swap(z.y, z.x);
	if (z.x - z.z < 0.0) swap(z.z, z.x);
	if (z.y - z.z < 0.0) swap(z.z, z.y);

	if (fractal->mandelbox.mainRotationEnabled && aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC) // rotation
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box fold
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
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{ // fabsBoxFold
		CVector4 tempA, tempB;

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

		if (aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			z *= fractal->transformCommon.scaleA1;
			aux.DE *= fabs(fractal->transformCommon.scaleA1);
		}
	}
	// menger scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		useScale = aux.actualScaleA + fractal->transformCommon.scale3;

		z *= useScale;

		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fabs(useScale) + 1.0;
		else // testing for log
			aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

		if (fractal->transformCommon.functionEnabledFFalse
				&& aux.i >= fractal->transformCommon.startIterationsY
				&& aux.i < fractal->transformCommon.stopIterationsY)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	// menger offsets
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

	if (fractal->transformCommon.addCpixelEnabledFalse) // addCpixel options
	{
		switch (fractal->mandelbulbMulti.orderOfXYZC)
		{
			case multi_OrderOfXYZ_xyz:
			default: aux.c = CVector4(aux.c.x, aux.c.y, aux.c.z, aux.c.w); break;
			case multi_OrderOfXYZ_xzy: aux.c = CVector4(aux.c.x, aux.c.z, aux.c.y, aux.c.w); break;
			case multi_OrderOfXYZ_yxz: aux.c = CVector4(aux.c.y, aux.c.x, aux.c.z, aux.c.w); break;
			case multi_OrderOfXYZ_yzx: aux.c = CVector4(aux.c.y, aux.c.z, aux.c.x, aux.c.w); break;
			case multi_OrderOfXYZ_zxy: aux.c = CVector4(aux.c.z, aux.c.x, aux.c.y, aux.c.w); break;
			case multi_OrderOfXYZ_zyx: aux.c = CVector4(aux.c.z, aux.c.y, aux.c.x, aux.c.w); break;
		}
		z += aux.c * fractal->transformCommon.constantMultiplierC111;
	}
}

/**
 * Menger Sponge and octo
 * from code by Knighty
 */
void MengerOctoIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{ // octo
	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		if (z.x + z.y < 0.0) z = CVector4(-z.y, -z.x, z.z, z.w);

		if (z.x + z.z < 0.0) // z.xz = -z.zx;
			z = CVector4(-z.z, z.y, -z.x, z.w);

		if (z.x - z.y < 0.0) // z.xy = z.yx;
			z = CVector4(z.y, z.x, z.z, z.w);

		if (z.x - z.z < 0.0) // z.xz = z.zx;
			z = CVector4(z.z, z.y, z.x, z.w);

		z.x = fabs(z.x);
		z = z * fractal->transformCommon.scale2
				- fractal->transformCommon.offset100 * (fractal->transformCommon.scale2 - 1.0);

		aux.DE *= fractal->transformCommon.scale2;
	}
	// box offset
	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		CVector4 temp = z;
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
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
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
				if (aux.i < fractal->Cpara.iterA)
				{
					para = temp0 + (aux.i * grade1);
				}
				if (aux.i < fractal->Cpara.iterB && aux.i >= fractal->Cpara.iterA)
				{
					para = tempA + (aux.i - fractal->Cpara.iterA) * grade2;
				}
				if (aux.i >= fractal->Cpara.iterB)
				{
					para = tempB + (aux.i - fractal->Cpara.iterB) * grade3;
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
						if (aux.i < 2 * fractal->Cpara.iterA)
						{
							paraIt = tempA - fabs(tempA - aux.i);
							paraAdd = paraIt * paraIt * curve1;
						}
						if (aux.i >= 2 * fractal->Cpara.iterA && aux.i < fractal->Cpara.iterB + tempL)
						{
							paraIt = tempB - fabs(tempB * aux.i);
							paraAdd = paraIt * paraIt * curve2;
						}
						para += paraAdd;
					}
				}
			}
			paraAddP0 = 0.0;
			if (fractal->Cpara.enabledParabFalse)
			{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
				paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope)
										+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
			}
		}
		para += paraAddP0 + fractal->transformCommon.minR2p25;
		// spherical fold
		double rr = z.Dot(z);

		z += fractal->mandelbox.offset;

		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < para)
		{
			double tglad_factor1 = fractal->transformCommon.maxR2d1 / para;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
			aux.color += fractal->mandelbox.color.factorSp1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
		z -= fractal->mandelbox.offset;
		z *= fractal->transformCommon.scale08;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale08);
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	// menger
	if (fractal->transformCommon.functionEnabledM
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
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
		CVector4 zA = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
		CVector4 zB = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offsetA0)
				+ (zB * fractal->transformCommon.offsetB0);
		aux.DE *= fractal->transformCommon.scale1;
	}
}

/**
 * Menger Sponge Polynomial Hybrid modified by Mclarekin
 */
void MengerPwr2PolyIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		CVector4 partA = z;
		if (fractal->transformCommon.functionEnabledFalse) // fabs
			partA = fabs(z);
		if (fractal->transformCommon.functionEnabledxFalse) // pwr3 or z * fabs(z^2)
			partA *= z;
		partA =
			partA * fractal->transformCommon.scale2 + fractal->transformCommon.constantMultiplier111;

		CVector4 fnZ1 = z;

		if (fractal->transformCommon.functionEnabledBxFalse) // cos(z*Pi)
		{
			double scalePi = M_PI * fractal->transformCommon.scaleC1;
			fnZ1.x = cos(z.x * scalePi);
			fnZ1.y = cos(z.y * scalePi);
			fnZ1.z = cos(z.z * scalePi);
		}

		if (fractal->transformCommon.functionEnabledyFalse) // pi rotation
			fnZ1 = fnZ1.RotateAroundVectorByAngle(fractal->transformCommon.constantMultiplier111.GetXYZ(),
				M_PI * fractal->transformCommon.scaleB1);

		if (fractal->transformCommon.functionEnabledzFalse) // box offset
		{
			fnZ1.x = (sign(fnZ1.x) * fractal->transformCommon.additionConstant000.x) + fnZ1.x;
			fnZ1.y = (sign(fnZ1.y) * fractal->transformCommon.additionConstant000.y) + fnZ1.y;
			fnZ1.z = (sign(fnZ1.z) * fractal->transformCommon.additionConstant000.z) + fnZ1.z;
		}

		if (fractal->transformCommon.functionEnabledAxFalse) // fabs(fnZ1)
			fnZ1 = fabs(fnZ1);
		CVector4 partB = z;
		if (fractal->transformCommon.functionEnabledAzFalse) partB = fabs(z);
		partB *= fractal->transformCommon.scale4;
		CVector4 constantMult = CVector4(fractal->transformCommon.constantMultiplierB111.x,
			fractal->transformCommon.constantMultiplierB111.y,
			fractal->transformCommon.constantMultiplierB111.z, 0.0);
		z = constantMult + partB - partA * fnZ1;
		z *= fractal->transformCommon.scale025;
		aux.DE = aux.DE * 4.0 * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
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

		if (fractal->transformCommon.rotationEnabled
				&& aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA) // rotation
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
void MengerPrismShapeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;

	if (aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.benesiT1EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	}
	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		CVector4 tempA, tempB;

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
	if (fractal->transformCommon.functionEnabled && aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
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
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * Menger Prism Shape2
 * from code by Knighty
 * http://www.fractalforums.com/fragmentarium/
 * cross-menger!-can-anyone-do-this/msg93972/#new
 */
void MengerPrismShape2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		z = CVector4(-z.z, z.x, z.y, z.w);
	}
	CVector4 gap = fractal->transformCommon.constantMultiplier000;

	if (fractal->transformCommon.functionEnabledx
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		double t;
		double dot1;
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
			case combo_mode0:
			default:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.x -= t * -SQRT_3 - tempOff;
				z.y = fabs(z.y - t);
				break;
			case combo_mode1:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - tempOff;
				z.x = fabs(z.x - t); // x y swap
				break;
			case combo_mode2:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.z -= t * -SQRT_3 - tempOff;
				z.y = fabs(z.y - t); // z y swap
				break;
			case combo_mode3:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - tempOff;
				z.z = fabs(z.z - t); // z y swap
				break;
			case combo_mode4:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - tempOff;
				z.z -= t * -SQRT_3 - tempOff;
				z.x = fabs(z.x - t); // x y swap and other things
				z.y = fabs(z.y - t);
				break;
			case combo_mode5:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.x -= t * -SQRT_3 - tempOff;
				z.y -= t * -SQRT_3 - tempOff;
				z.y = fabs(z.y - t); // x y swap and other things
				z.z = fabs(z.z - t);
				break;
			case combo_mode6:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - (fractal->transformCommon.offset0);
				z.z -= t * -SQRT_3 - (fractal->transformCommon.offset0);
				z.x = fabs(z.y - t); // x y swap and other things and swizzle
				z.y = fabs(z.x - t);
				break;
			case combo_mode7:
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

		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);

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
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)

	{ // CrossMengerTrick
		double dd = aux.DE;
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			z.x = fabs(z.x);
		}
		double dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scaleD1;
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
		// else if ((z.y - 1.5) * (z.y - 1.5) + z.z * z.z < z.y * z.y + (z.z - 1.5) * (z.z - 1.5))
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
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
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

		z -= CVector4(0.5 * SQRT_3, 1.5, 1.5, 0.0);

		z *= fractal->transformCommon.scaleB3;
		aux.DE *= fractal->transformCommon.scaleB3;

		z += CVector4(0.5 * SQRT_3, 1.5, 1.5, 0.0);
	}

	if (fractal->transformCommon.benesiT1EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 tempZ = z;
		double tempL = tempZ.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.benesiT1MEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	}

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		CVector4 tempA, tempB;

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
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	// menger
	if (fractal->transformCommon.functionEnabledM
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
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
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * Menger Smooth
 * http://www.fractalforums.com/fragmentarium/help-t22583/
 */
void MengerSmoothIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double sc1 = fractal->transformCommon.scale3 - 1.0;		// 3 - 1 = 2, 2/3 = 0.6667;
	double sc2 = sc1 / fractal->transformCommon.scale3;		//  8 - 1 = 7, 7/8 = 0.89ish;
	double OffsetS = fractal->transformCommon.offset0005; //

	if (fractal->transformCommon.functionEnabled)
	{
		// the closer to origin the greater the effect of OffsetSQ
		z = CVector4(
			sqrt(z.x * z.x + OffsetS), sqrt(z.y * z.y + OffsetS), sqrt(z.z * z.z + OffsetS), z.w);
	}

	double t;
	CVector4 OffsetC = fractal->transformCommon.offset1105;

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

	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->transformCommon.functionEnabledzFalse)
	{
		CVector4 zA = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
		CVector4 zB = (aux.i == fractal->transformCommon.intB) ? z : CVector4();
		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offsetA0)
				+ (zB * fractal->transformCommon.offsetB0);
		aux.DE *= fractal->transformCommon.scale1;
	}
}

/**
 * Menger Smooth Mod1, based on :
 * http://www.fractalforums.com/fragmentarium/help-t22583/
 */
void MengerSmoothMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	if (fractal->transformCommon.functionEnabled)
	{
		z = CVector4(sqrt(z.x * z.x + fractal->transformCommon.offset0),
			sqrt(z.y * z.y + fractal->transformCommon.offset0),
			sqrt(z.z * z.z + fractal->transformCommon.offset0), z.w);
	}
	if (fractal->transformCommon.functionEnabledFFalse)
	{
		z = fabs(z);
		double s = fractal->transformCommon.offset;
		z += CVector4(s, s, s, 0.0);
	}

	double t;
	double ScaleP5 = fractal->transformCommon.scale05;
	CVector4 OffsetC = fractal->transformCommon.constantMultiplier221;
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

	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box offset
	{
		z.x = sign(z.x) * fractal->transformCommon.additionConstantA000.x + z.x;
		z.y = sign(z.y) * fractal->transformCommon.additionConstantA000.y + z.y;
		z.z = sign(z.z) * fractal->transformCommon.additionConstantA000.z + z.z;
	}

	if (fractal->transformCommon.functionEnabledzFalse)
	{
		CVector4 zA = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
		CVector4 zB = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offsetA0)
				+ (zB * fractal->transformCommon.offsetB0);
		aux.DE *= fractal->transformCommon.scale1;
	}
}

/**
 * menger poly fold
 * @reference
 * https://fractalforums.org/fragmentarium/17/polyfoldsym-pre-transform/2684
 */
void MengerPolyFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		// pre abs
		if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);

		if (fractal->transformCommon.functionEnabledCx)
		{
			if (fractal->transformCommon.functionEnabledAxFalse && z.y < 0.0) z.x = -z.x;
			int poly = fractal->transformCommon.int8X;
			double psi = fabs(fmod(atan(z.y / z.x) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
			double len = sqrt(z.x * z.x + z.y * z.y);
			z.x = cos(psi) * len;
			z.y = sin(psi) * len;
		}

		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			if (fractal->transformCommon.functionEnabledAyFalse && z.z < 0.0) z.y = -z.y;
			int poly = fractal->transformCommon.int8Y;
			double psi = fabs(fmod(atan(z.z / z.y) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
			double len = sqrt(z.y * z.y + z.z * z.z);
			z.y = cos(psi) * len;
			z.z = sin(psi) * len;
		}

		if (fractal->transformCommon.functionEnabledCzFalse)
		{
			if (fractal->transformCommon.functionEnabledAzFalse && z.x < 0.0) z.z = -z.z;
			int poly = fractal->transformCommon.int8Z;
			double psi = fabs(fmod(atan(z.x / z.z) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
			double len = sqrt(z.z * z.z + z.x * z.x);
			z.z = cos(psi) * len;
			z.x = sin(psi) * len;
		}

		z += fractal->transformCommon.additionConstant000;

		// rotation
		if (fractal->transformCommon.rotation2EnabledFalse)
		{
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}
	}

	// Menger Sponge
	z = fabs(z);
	if (z.x - z.y < 0.0) swap(z.y, z.x);
	if (z.x - z.z < 0.0) swap(z.z, z.x);
	if (z.y - z.z < 0.0) swap(z.z, z.y);
	z *= fractal->transformCommon.scale3;
	z.x -= 2.0 * fractal->transformCommon.constantMultiplierA111.x;
	z.y -= 2.0 * fractal->transformCommon.constantMultiplierA111.y;
	if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplierA111.z;
	aux.DE *= fabs(fractal->transformCommon.scale3 * fractal->transformCommon.scaleA1);

	z += fractal->transformCommon.additionConstantA000;

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}

/**
 * Msltoe Donut formula
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * low-hanging-dessert-an-escape-time-donut-fractal/msg90171/#msg90171
 */
void MsltoeDonutIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double radius2 = fractal->donut.ringThickness;
	double nSect = M_PI_2x / fractal->donut.number;
	double fact = fractal->donut.factor;

	double R = sqrt(z.x * z.x + z.y * z.y);
	double R2 = fractal->donut.ringRadius - R;
	double t = R2 * R2 + z.z * z.z - radius2 * radius2;

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
void MsltoeSym2ModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	aux.DE = aux.DE * 2.0 * aux.r;
	CVector4 temp = z;

	if (fabs(z.y) < fabs(z.z)) // then swap
	{
		z.y = temp.z; // making z.y furthest away from axis
		z.z = temp.y;
	}
	if (z.y > z.z) // then change sign of z.x and z.z
	{
		z.x = -z.x;
	}

	CVector4 z2 = z * z;							// squares
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
		CVector4 tempFAB = c;
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
	aux.DE *= fabs(fractal->transformCommon.scale1);
}

/**
 * MsltoeSym3Mod based on the formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/15/
 */
void MsltoeSym3ModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	aux.DE = aux.DE * 2.0 * aux.r;
	CVector4 temp = z;
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
	CVector4 z2 = z * z; // squares
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
		CVector4 tempFAB = c;
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

	if (fractal->transformCommon.functionEnabledFalse // quaternion fold
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;
		z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, z.w);

		if (fractal->analyticDE.enabledFalse)
		{
			CVector4 temp2 = z;
			double tempL = temp2.Length();
			z *= CVector4(1.0, 2.0, 2.0, 1.0);
			// if (tempL < 1e-21) tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			// aux.DE *= avgScale * fractal->transformCommon.scaleA1;
			double tempAux = aux.DE * avgScale;
			aux.DE = aux.DE + (tempAux - aux.DE) * fractal->analyticDE.scale1;
		}
		else
		{
			z *= CVector4(1.0, 2.0, 2.0, 1.0);
		}
	}
}

/**
 * MsltoeJuliaBulb Eiffie. Refer post by Eiffie    Reply #69 on: January 27, 2015
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/60/
 */
void EiffieMsltoeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double psi = fabs(fmod(atan2(z.z, z.y) + M_PI + M_PI_8, M_PI_4) - M_PI_8);
	double lengthYZ = sqrt(z.y * z.y + z.z * z.z);

	z.y = cos(psi) * lengthYZ;
	z.z = sin(psi) * lengthYZ;
	aux.DE = aux.DE * 2.0 * aux.r;

	CVector4 z2 = z * z;
	double rr = z2.x + z2.y + z2.z;
	double m = 1.0 - z2.z / rr;
	CVector4 temp;
	temp.x = (z2.x - z2.y) * m;
	temp.y = 2.0 * z.x * z.y * m * fractal->transformCommon.scale; // scaling y;;
	temp.z = 2.0 * z.z * sqrt(z2.x + z2.y);
	temp.w = z.w;
	z = temp + fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempFAB = c;
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
	/*aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	// aux.DE *= fabs(fractal->transformCommon.scale1);

	if (fractal->analyticDE.enabledFalse)
	{ // analytic DE adjustment
		aux.DE *= fabs(fractal->transformCommon.scale1) * fractal->analyticDE.scale1;
	}
	else
	{
		aux.DE *= fabs(fractal->transformCommon.scale1);
	}*/
	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
}

/**
 * Msltoe_Julia_Bulb_Mod2
 * @reference
 * http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/msg14198/#msg14198
 */
void MsltoeSym3Mod2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	aux.DE = aux.DE * 2.0 * aux.r;

	double theta;
	double phi;
	CVector4 z2 = z * z;
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
		CVector4 tempFAB = c;
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
	aux.DE *= fabs(fractal->transformCommon.scale1);
}

/**
 * Msltoe_Julia_Bulb_Mod3
 * @reference
 * http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/msg14320/#msg14320
 */
void MsltoeSym3Mod3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	aux.DE = aux.DE * 2.0 * aux.r;
	CVector4 z1 = z;
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
	CVector4 zs = z * z;
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
		CVector4 tempFAB = c;
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
	aux.DE *= fabs(fractal->transformCommon.scale1);

	if (fractal->transformCommon.functionEnabledFalse // quaternion fold
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;
		z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, z.w);
		if (fractal->analyticDE.enabledFalse)
		{
			CVector4 temp = z;
			double tempL = temp.Length();
			z *= CVector4(1.0, 2.0, 2.0, 1.0);
			// if (tempL < 1e-21)
			//	tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			aux.DE *= avgScale;
		}
		else
		{
			z *= CVector4(1.0, 2.0, 2.0, 1.0);
		}
	}
}

/**
 * MsltoeSym4Mod  Based on the formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/15/
 * This formula contains aux.color
 */
void MsltoeSym4ModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	CVector4 oldZ = z;
	aux.DE = aux.DE * 2.0 * aux.r;
	CVector4 temp = z;
	double tempL = temp.Length();
	// if (tempL < 1e-21)
	//	tempL = 1e-21;
	z *= fractal->transformCommon.scale3D111;

	aux.DE *= fabs(z.Length() / tempL);

	if (fabs(z.x) < fabs(z.z)) swap(z.x, z.z);
	if (fabs(z.x) < fabs(z.y)) swap(z.x, z.y);
	if (fabs(z.y) < fabs(z.z)) swap(z.y, z.z);

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor.z;
	}
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
		CVector4 tempFAB = c;
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
	aux.DE *= fabs(fractal->transformCommon.scale1);
}

/**
 * MsltoeToroidal
 * @reference http://www.fractalforums.com/theory/toroidal-coordinates/msg9428/
 */
void MsltoeToroidalIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1) // pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.DE *= z.Length() / aux.r;
	}

	// Toroidal bulb
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
	double r1RpCosPhi = r1 + rp * cos(phi);
	z.x = r1RpCosPhi * cos(theta);
	z.y = r1RpCosPhi * sin(theta);
	z.z = -rp * sin(phi);

	// DEcalc
	if (!fractal->analyticDE.enabledFalse)
	{
		aux.DE = rp * aux.DE * (fractal->transformCommon.pwr4 + 1.0) + 1.0;
	}
	else
	{
		aux.DE = rp * aux.DE * (fractal->transformCommon.pwr4 + fractal->analyticDE.offset2)
							 * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
	}

	if (fractal->transformCommon.functionEnabledAxFalse) // spherical offset
	{
		double lengthTempZ = -z.Length();
		// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	}
	// then add Cpixel constant vector
}

/**
 * MsltoeToroidalMulti
 * @reference http://www.fractalforums.com/theory/toroidal-coordinates/msg9428/
 */
void MsltoeToroidalMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1) // pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.DE = aux.DE * z.Length() / aux.r;
	}
	// Toroidal bulb multi
	double th0 = fractal->bulb.betaAngleOffset;
	double ph0 = fractal->bulb.alphaAngleOffset;
	double v1, v2, v3;
	switch (fractal->sinTan2Trig.orderOfZYX)
	{
		case multi_OrderOfZYX_zyx:
		default:
			v1 = z.z;
			v2 = z.y;
			v3 = z.x;
			break;
		case multi_OrderOfZYX_zxy:
			v1 = z.z;
			v2 = z.x;
			v3 = z.y;
			break;
		case multi_OrderOfZYX_yzx:
			v1 = z.y;
			v2 = z.z;
			v3 = z.x;
			break;
		case multi_OrderOfZYX_yxz:
			v1 = z.y;
			v2 = z.x;
			v3 = z.z;
			break;
		case multi_OrderOfZYX_xzy:
			v1 = z.x;
			v2 = z.z;
			v3 = z.y;
			break;
		case multi_OrderOfZYX_xyz:
			v1 = z.x;
			v2 = z.y;
			v3 = z.z;
			break;
	}

	switch (fractal->sinTan2Trig.atan2OrAtan)
	{
		case multi_atan2OrAtan_atan2: ph0 += atan2(v2, v3); break;
		case multi_atan2OrAtan_atan: ph0 += atan(v2 / v3); break;
	}

	double r1 = fractal->transformCommon.minR05;
	double x1 = r1 * cos(ph0);
	double y1 = r1 * sin(ph0);

	aux.r = (z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1) + z.z * z.z; //+ 1e-061

	double sqrT = aux.r;
	if (fractal->transformCommon.functionEnabledAy) // sqrt
	{
		sqrT = sqrt(aux.r);
	}

	switch (fractal->sinTan2Trig.asinOrAcos)
	{
		case multi_asinOrAcos_asin: th0 += asin(v1 / sqrT); break;
		case multi_asinOrAcos_acos: th0 += acos(v1 / sqrT); break;
	}

	th0 *= fractal->transformCommon.pwr8; // default 8
	ph0 *= fractal->bulb.power;						// default 9 gives 8 symmetry

	double rp = pow(aux.r, fractal->transformCommon.pwr4);

	double costh = cos(th0);
	double sinth = sin(th0);
	double r1RpCosTh = r1 + rp * costh;
	double r1RpSinTh = r1 + rp * sinth;

	if (fractal->transformCommon.functionEnabledzFalse)
	{ // cosine mode
		z.x = r1RpSinTh * sin(ph0);
		z.y = r1RpSinTh * cos(ph0);
		z.z = -rp * costh;
	}
	else
	{ // sine mode default

		z.x = r1RpCosTh * cos(ph0);
		z.y = r1RpCosTh * sin(ph0);
		z.z = -rp * sinth;
	}

	// DEcalc
	if (!fractal->analyticDE.enabledFalse)
	{
		aux.DE = rp * aux.DE * (fractal->transformCommon.pwr4 + 1.0) + 1.0;
	}
	else
	{
		aux.DE = rp * aux.DE * (fractal->transformCommon.pwr4 + fractal->analyticDE.offset2)
							 * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
	}

	if (fractal->transformCommon.functionEnabledAxFalse) // spherical offset
	{
		double lengthTempZ = -z.Length();
		// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	}
	// then add Cpixel constant vector
}
/**
 * Pseudo Kleinian Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */

void PseudoKleinianIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// sphere inversion slot#1 iter == 0 added v2.17
	if (fractal->transformCommon.sphereInversionEnabledFalse)
	{
		if (aux.i < 1)
		{
			double rr = 1.0;
			z += fractal->transformCommon.offset000;
			rr = z.Dot(z);
			z *= fractal->transformCommon.maxR2d1 / rr;
			z += fractal->transformCommon.additionConstantA000 - fractal->transformCommon.offset000;
			// double r = sqrt(rr);
			aux.DE = aux.DE * (fractal->transformCommon.maxR2d1 / rr) + fractal->analyticDE.offset0;
		}
	}

	CVector4 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;
	// prism shape
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	// box fold abs() tglad fold added v2.17
	if (fractal->transformCommon.functionEnabledByFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabledBy)
		{
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		}
	}

	// box fold
	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
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
	// PseudoKleinian
	CVector4 cSize = fractal->transformCommon.additionConstant0777;
	CVector4 tempZ = z; //  correct c++ version.
	if (z.x > cSize.x) tempZ.x = cSize.x;
	if (z.x < -cSize.x) tempZ.x = -cSize.x;
	if (z.y > cSize.y) tempZ.y = cSize.y;
	if (z.y < -cSize.y) tempZ.y = -cSize.y;
	if (z.z > cSize.z) tempZ.z = cSize.z;
	if (z.z < -cSize.z) tempZ.z = -cSize.z;

	z = tempZ * 2.0 - z;
	double k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
	z *= k;
	aux.DE *= k + fractal->analyticDE.tweak005;
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	// offset
	z += fractal->transformCommon.additionConstant000;

	aux.pseudoKleinianDE = fractal->analyticDE.scale1;
}

/**
 * Pseudo Kleinian Mod1, Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */
void PseudoKleinianMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 gap = fractal->transformCommon.constantMultiplier000;

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		double dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		double t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.benesiT1EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 tempZ = z;
		double tempL = tempZ.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	}
	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		CVector4 tempA, tempB;

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

	double k;
	// Pseudo kleinian
	CVector4 cSize = fractal->transformCommon.additionConstant0777;
	if (fractal->transformCommon.functionEnabledAy
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 tempZ = z; //  correct c++ version. non conditional mult 2.0

		if (z.x > cSize.x) tempZ.x = cSize.x;
		if (z.x < -cSize.x) tempZ.x = -cSize.x;
		if (z.y > cSize.y) tempZ.y = cSize.y;
		if (z.y < -cSize.y) tempZ.y = -cSize.y;
		if (z.z > cSize.z) tempZ.z = cSize.z;
		if (z.z < -cSize.z) tempZ.z = -cSize.z;

		z.z *= fractal->transformCommon.scaleB1;
		aux.DE *= fractal->transformCommon.scaleB1;

		z = tempZ * 2.0 - z;
		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	if (fractal->transformCommon.functionEnabledAyFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		//  variation from openCL conditional mult 2.0
		if (z.x > cSize.x) z.x = cSize.x * 2.0 - z.x;
		if (z.x < -cSize.x) z.x = -cSize.x * 2.0 - z.x;
		if (z.y > cSize.y) z.y = cSize.y * 2.0 - z.y;
		if (z.y < -cSize.y) z.y = -cSize.y * 2.0 - z.y;
		if (z.z > cSize.z) z.z = cSize.z * 2.0 - z.z;
		if (z.z < -cSize.z) z.z = -cSize.z * 2.0 - z.z;

		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledMFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
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
	aux.pseudoKleinianDE = fractal->analyticDE.scale1; // pK DE
}
/**
 * Pseudo Kleinian Mod2, Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */
void PseudoKleinianMod2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	CVector4 oldZ = z;
	CVector4 zCol = z;
	double rrCol = 0.0;
	double colorAdd = 0.0;
	// spherical fold
	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double para = 0.0;
		double paraAddP0 = 0.0;
		if (fractal->transformCommon.functionEnabledyFalse)
		{
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
				if (aux.i < fractal->Cpara.iterA)
				{
					para = temp0 + (aux.i * grade1);
				}
				if (aux.i < fractal->Cpara.iterB && aux.i >= fractal->Cpara.iterA)
				{
					para = tempA + (aux.i - fractal->Cpara.iterA) * grade2;
				}
				if (aux.i >= fractal->Cpara.iterB)
				{
					para = tempB + (aux.i - fractal->Cpara.iterB) * grade3;
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
						if (aux.i < 2 * fractal->Cpara.iterA)
						{
							paraIt = tempA - fabs(tempA - aux.i);
							paraAdd = paraIt * paraIt * curve1;
						}
						if (aux.i >= 2 * fractal->Cpara.iterA && aux.i < fractal->Cpara.iterB + tempL)
						{
							paraIt = tempB - fabs(tempB * aux.i);
							paraAdd = paraIt * paraIt * curve2;
						}
						para += paraAdd;
					}
				}
			}
			paraAddP0 = 0.0;
			if (fractal->Cpara.enabledParabFalse)
			{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
				paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope)
										+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
			}
		}
		para += paraAddP0 + fractal->transformCommon.minR2p25;

		// spherical fold
		double rr = z.Dot(z);
		rrCol = rr;
		z += fractal->mandelbox.offset;

		// if (rr < 1e-21) rr = 1e-21;
		if (rr < para)
		{
			double tglad_factor1 = fractal->transformCommon.maxR2d1 / para;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
		}
		else if (rr < fractal->transformCommon.maxR2d1) // fractal->mandelbox.fR2
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
		}
		z -= fractal->mandelbox.offset;
		z *= fractal->transformCommon.scale1;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + fractal->analyticDE.offset0;
	}

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		CVector4 gap = fractal->transformCommon.constantMultiplier000;
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		double dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		double t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.benesiT1EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, 0.0);

		CVector4 tempZ = z;
		double tempL = tempZ.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, 0.0);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, 0.0);

		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		oldZ = z;
		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);
		zCol = z;
		/*if (fractal->foldColor.auxColorEnabledFalse)
		{
			if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
		}*/
		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, 0.0);
	}

	double k;
	// Pseudo kleinian
	CVector4 cSize = fractal->transformCommon.additionConstant0777;
	if (fractal->transformCommon.functionEnabledAy
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 tempZ = z; //  correct c++ version. non conditional mult 2.0

		if (z.x > cSize.x) tempZ.x = cSize.x;
		if (z.x < -cSize.x) tempZ.x = -cSize.x;
		if (z.y > cSize.y) tempZ.y = cSize.y;
		if (z.y < -cSize.y) tempZ.y = -cSize.y;
		if (z.z > cSize.z) tempZ.z = cSize.z;
		if (z.z < -cSize.z) tempZ.z = -cSize.z;

		z = tempZ * 2.0 - z;
		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	if (fractal->transformCommon.functionEnabledAyFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		//  variation from openCL  conditional mult 2.0
		if (z.x > cSize.x) z.x = cSize.x * 2.0 - z.x;
		if (z.x < -cSize.x) z.x = -cSize.x * 2.0 - z.x;
		if (z.y > cSize.y) z.y = cSize.y * 2.0 - z.y;
		if (z.y < -cSize.y) z.y = -cSize.y * 2.0 - z.y;
		if (z.z > cSize.z) z.z = cSize.z * 2.0 - z.z;
		if (z.z < -cSize.z) z.z = -cSize.z * 2.0 - z.z;

		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		oldZ = z;
		z = fabs(z + fractal->transformCommon.offsetA000)
				- fabs(z - fractal->transformCommon.offsetA000) - z;
		zCol = z;

		if (fractal->transformCommon.functionEnabledFalse
				&& aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA)
		{
			CVector4 limit = fractal->transformCommon.offsetA000;
			CVector4 length = 2.0 * limit;
			CVector4 tgladS = 1.0 / length;
			CVector4 Add = CVector4(0.0, 0.0, 0.0, 0.0);
			;
			if (fabs(z.x) < limit.x) Add.x = z.x * z.x * tgladS.x;
			if (fabs(z.y) < limit.y) Add.y = z.y * z.y * tgladS.y;
			if (fabs(z.z) < limit.z) Add.z = z.z * z.z * tgladS.z;
			if (fabs(z.x) > limit.x && fabs(z.x) < length.x)
				Add.x = (length.x - fabs(z.x)) * (length.x - fabs(z.x)) * tgladS.x;
			if (fabs(z.y) > limit.y && fabs(z.y) < length.y)
				Add.y = (length.y - fabs(z.y)) * (length.y - fabs(z.y)) * tgladS.y;
			if (fabs(z.z) > limit.z && fabs(z.z) < length.z)
				Add.z = (length.z - fabs(z.z)) * (length.z - fabs(z.z)) * tgladS.z;
			Add *= fractal->transformCommon.scale3D000;
			z.x = (z.x - (sign(z.x) * (Add.x)));
			z.y = (z.y - (sign(z.y) * (Add.y)));
			z.z = (z.z - (sign(z.z) * (Add.z)));
		}
	}
	if (fractal->transformCommon.addCpixelEnabledFalse) // symmetrical addCpixel
	{
		CVector4 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.offsetF000;
		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}
	aux.pseudoKleinianDE = fractal->analyticDE.scale1; // pK DE
	// aux.pseudoKleinianZZ = fractal->transformCommon.scale0; // pK z.z * z.z * scale0

	// color updated v2.13 & mode2 v2.14
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd += fractal->mandelbox.color.factor.x
										* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
			if (zCol.y != oldZ.y)
				colorAdd += fractal->mandelbox.color.factor.y
										* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
			if (zCol.z != oldZ.z)
				colorAdd += fractal->mandelbox.color.factor.z
										* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

			if (rrCol < fractal->transformCommon.maxR2d1)
			{
				if (rrCol < fractal->transformCommon.minR2p25)
					colorAdd +=
						fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
						+ fractal->mandelbox.color.factorSp2
								* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
				else
					colorAdd +=
						fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			}
		}
		else
		{
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;

			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1;
			else if (rrCol < fractal->transformCommon.maxR2d1)
				colorAdd += fractal->mandelbox.color.factorSp2;
		}
		aux.color += colorAdd;
	}
}

/**
 * Pseudo Kleinian std DE Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */
void PseudoKleinianStdDEIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.benesiT1EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 tempZ = z;
		double tempL = tempZ.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	}
	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		CVector4 tempA, tempB;

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
	if (fractal->transformCommon.functionEnabled && aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
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

	aux.DE *= fractal->transformCommon.scaleB1;
	double k;
	// Pseudo kleinian
	CVector4 cSize = fractal->transformCommon.additionConstant0777;
	if (fractal->transformCommon.functionEnabledAy
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 tempZ = z; //  correct c++ version.

		if (z.x > cSize.x) tempZ.x = cSize.x;
		if (z.x < -cSize.x) tempZ.x = -cSize.x;
		if (z.y > cSize.y) tempZ.y = cSize.y;
		if (z.y < -cSize.y) tempZ.y = -cSize.y;
		if (z.z > cSize.z) tempZ.z = cSize.z;
		if (z.z < -cSize.z) tempZ.z = -cSize.z;

		z *= fractal->transformCommon.scaleB1;
		z = tempZ * 2.0 - z;

		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	if (fractal->transformCommon.functionEnabledAyFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		//  variation from openCL
		if (z.x > cSize.x) z.x = cSize.x * 2.0 - z.x;
		if (z.x < -cSize.x) z.x = -cSize.x * 2.0 - z.x;
		if (z.y > cSize.y) z.y = cSize.y * 2.0 - z.y;
		if (z.y < -cSize.y) z.y = -cSize.y * 2.0 - z.y;
		if (z.z > cSize.z) z.z = cSize.z * 2.0 - z.z;
		if (z.z < -cSize.z) z.z = -cSize.z * 2.0 - z.z;
		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	z += fractal->transformCommon.additionConstant000;
	// aux.pseudoKleinianZZ = fractal->transformCommon.scale0; // pK z.z * z.z * scale0
}

/**
 * Quaternion3DE - Quaternion fractal with extended controls
 * @reference http://www.fractalforums.com/3d-fractal-generation
 * /true-3d-mandlebrot-type-fractal/
 */
void Quaternion3dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	aux.DE = aux.DE * 2.0 * aux.r;
	z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, z.w);

	double tempL = z.Length();
	z *= fractal->transformCommon.constantMultiplier122;
	// if (tempL < 1e-21) tempL = 1e-21;
	CVector4 tempAvgScale = CVector4(z.x, z.y / 2.0, z.z / 2.0, z.w);
	double avgScale = tempAvgScale.Length() / tempL;
	double tempAux = aux.DE * avgScale;
	aux.DE = aux.DE + (tempAux - aux.DE) * fractal->transformCommon.scaleA1;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	z += fractal->transformCommon.additionConstant000;
}

/**
 * RiemannSphereMsltoe
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/
 * riemann-fractals/msg33500/#msg33500
 */
void RiemannSphereMsltoeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	double r = aux.r; // z.Length();
	// if (r < 1e-21) r = 1e-21;
	z *= fractal->transformCommon.scale / r;

	double q = 1.0 / (1.0 - z.z);
	double s = z.x * q;
	double t = z.y * q;

	double p = 1.0 + s * s + t * t;

	s = fabs(sin(M_PI * s + fractal->transformCommon.offsetA0));
	t = fabs(sin(M_PI * t + fractal->transformCommon.offsetB0));

	r *= r;
	// if (r < 1e-21)
	//	r = 1e-21;

	if (p > 36) p = 36; // problem with pow()
	r = -0.25 + pow(r, p);

	z.x = 2.0 * s;
	z.y = 2.0 * t;
	z.z = -1.0 + s * s + t * t;
	z *= r / (1.0 + s * s + t * t);

	z += fractal->transformCommon.additionConstant000;
}

/**
 * RiemannSphereMsltoe Version1
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * revisiting-the-riemann-sphere-%28again%29/
 */
void RiemannSphereMsltoeV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double r = aux.r; // z.Length();
	// if (r < 1e-21) r = 1e-21;
	z *= fractal->transformCommon.scale / r;
	double q = 1.0 / (1.0 - z.z);
	CVector4 t3;
	t3.x = z.x * q;
	t3.y = z.y * q;

	t3.z = (r - 1.5) * (1.0 + t3.x * t3.x + t3.y * t3.y);

	if (fractal->transformCommon.rotationEnabled)
		t3 = fractal->transformCommon.rotationMatrix.RotateVector(t3);

	t3.x = t3.x - floor(t3.x + 0.5);
	t3.y = t3.y - floor(t3.y + 0.5);
	t3.w = z.w;

	z = t3 * fractal->transformCommon.constantMultiplier441;

	z += fractal->transformCommon.additionConstant000;
}
/**
 * RiemannSphereMsltoe Version2
 * @reference
 * http://www.fractalforums.com/theory/alternate-co-ordinate-systems/msg11688/#msg11688
 */
void RiemannSphereMsltoeV2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double theta = 0.0;
	double phi = 0.0;
	double rx;
	double rz;
	double r = aux.r;

	// rotate
	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	// invert and scale
	z *= fractal->transformCommon.scale08 / r;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale08) / r + 1.0; //  /r
	// if (fabs(z.x) < 1e-21) z.x = 1e-21;
	// if (fabs(z.z) < 1e-21) z.z = 1e-21;

	rx = z.x / (z.y - 1.0);
	theta = 8.0 * atan2(2.0 * rx, rx * rx - 1.0);
	rz = z.z / (z.y - 1.0);
	phi = 8.0 * atan2(2.0 * rz, rz * rz - 1.0);

	theta *= fractal->transformCommon.scaleA1;
	phi *= fractal->transformCommon.scaleB1;

	rx = sin(theta) / (1.0 + cos(theta));
	rz = sin(phi) / (1.0 + cos(phi));
	double rXZ = rx * rx + rz * rz;
	double d = 2.0 / (rXZ + 1.0);

	double a1 = rx * d;
	double b1 = (rXZ - 1.0) * 0.5 * d;
	double c1 = rz * d;

	double rrrr = r * r * r * r;

	z.x = a1 * rrrr;
	z.y = b1 * rrrr;
	z.z = c1 * rrrr;

	z += fractal->transformCommon.offset010;

	if (fractal->analyticDE.enabled)
	{
		aux.DE =
			aux.DE * 8.0 * fractal->analyticDE.scale1 * z.Length() / r + fractal->analyticDE.offset1;
	}
}
/**
 * RiemannBulbMsltoe Mod2
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * another-way-to-make-my-riemann-sphere-'bulb'-using-a-conformal-transformation/
 */
void RiemannBulbMsltoeMod2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double radius2 = fractal->transformCommon.minR05;
	double rr = z.x * z.x + z.y * z.y + z.z * z.z; // r2 or point radius squared
	if (rr < radius2 * radius2)
	{
		if (fractal->transformCommon.functionEnabled)
			// smooth inside
			z *= radius2 * ((rr * 0.1) + 0.4) * 1.18 * fractal->transformCommon.scaleA1 / rr;
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
 * RiemannSphereHobold
 * @reference https://fractalforums.org/fractal-mathematics-and-new-theories/28/
 * riemandelettuce-without-trigonometry/2996/msg16097#msg16097
 */
void RiemannSphereHoboldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	z *=  fractal->transformCommon.scale08 / aux.r; // normalize vector to unit length => project onto sphere




	// find X-related iso-plane: polar projection onto unit circle
	double Kx = 2.0 * z.x * (1.0 - z.y) / ((z.y - 2.0) * z.y + z.x * z.x + 1.0);
	double Ky = 1.0 - 2.0 * ((z.y - 2.0) * z.y + 1.0) /
		((z.y - 2.0) * z.y + z.x * z.x + 1.0);

	// doubled point
	double K2x = -2.0 * Kx * Ky;
	double K2y = -(Ky * Ky - Kx * Kx);

	// two more doublings (for total power eight)
	Kx = -2.0 * K2x * K2y;
	Ky = -(K2y * K2y - K2x * K2x);
	K2x = -2.0 * Kx * Ky;
	K2y = -(Ky * Ky - Kx * Kx);

	// (relevant) normal vector coordinates of doubled point plane
	double n1x = K2y - 1.0;
	double n1y = -K2x;

	n1x += fractal->transformCommon.offsetA0;

	// find Z-related iso-plane: polar projection onto unit circle
	double Kz = 2.0 * z.z * (1.0 - z.y) / ((z.y - 2.0) * z.y + z.z * z.z + 1.0);
	Ky = 1.0 - 2.0 * ((z.y - 2.0) * z.y + 1.0) / ((z.y - 2.0) * z.y + z.z * z.z + 1.0);

	// doubled point
	double K2z = -2.0 * Kz * Ky;
	K2y = -(Ky * Ky - Kz * Kz);

	// two more doublings (for total power eight)
	Kz = -2.0 * K2z * K2y;
	Ky = -(K2y * K2y - K2z * K2z);
	K2z = -2.0 * Kz * Ky;
	K2y = -(Ky * Ky - Kz * Kz);

	// (relevant) normal vector coordinates of doubled point plane
	double n2y = -K2z;
	double n2z = K2y - 1.0;

	n2z += fractal->transformCommon.offsetB0;


	// compute position of doubled point as intersection of planes and sphere
	// solved ray parameter
	double nt = 2.0 * (n1x * n1x * n2z * n2z) / ((n1x * n1x + n1y * n1y) * n2z * n2z
				+ n1x * n1x * n2y * n2y);

	// doubled point position
	z.y = 1.0 - nt;
	z.x = n1y * (1.0 - z.y) / n1x;
	z.z = n2y * (1.0 - z.y) / n2z;

	// raise original length to the power, then add constant
	z *= aux.r * aux.r * aux.r * aux.r; // for 8th power

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	if (fractal->analyticDE.enabled)
	{
		aux.DE = fractal->analyticDE.offset1 + aux.DE * fabs( fractal->transformCommon.scale08) / aux.r;
		aux.DE *= 8.0 * fractal->analyticDE.scale1 * z.Length() / aux.r;
	}
}

/**
 * quadratic iteration in imaginary scator algebra
 * Use stop at maximum iteration (at maxiter)for the image to rendered correctly
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * ix-possibly-the-holy-grail-fractal-%28in-fff-lore%29
 * https://luz.izt.uam.mx/drupal/en/fractals/ix
 * @author Manuel Fernandez-Guasti
 */
void ScatorPower2ImaginaryIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	Q_UNUSED(aux);

	double x2 = z.x * z.x; // + 1e-061;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;

	double newx = x2 - y2 - z2 + (y2 * z2) / x2;
	double newy = 2.0 * z.x * z.y * (1.0 - z2 / x2);
	double newz = 2.0 * z.x * z.z * (1.0 - y2 / x2);

	z.x = newx;
	z.y = newy;
	z.z = newz;
}

/**
 * quadratic iteration in real scator algebra
 * Use stop at maximum iteration (at maxiter)for the image to rendered correctly
 * @reference
 * https://luz.izt.uam.mx/drupal/en/fractals/hun
 * @author Manuel Fernandez-Guasti
 */
void ScatorPower2RealIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	Q_UNUSED(aux);

	double x2 = z.x * z.x; //+ 1e-061
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;

	double newx = x2 + y2 + z2 + (y2 * z2) / x2;
	double newy = 2.0 * z.x * z.y * (1.0 + z2 / x2);
	double newz = 2.0 * z.x * z.z * (1.0 + y2 / x2);

	z.x = newx;
	z.y = newy;
	z.z = newz;
}

/**
 * quadratic iteration in real or imaginary scator algebra
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * ix-possibly-the-holy-grail-fractal-%28in-fff-lore%29
 * https://luz.izt.uam.mx/drupal/en/fractals/ix
 * @author Manuel Fernandez-Guasti
 * This formula contains aux.DE
 */
void ScatorPower2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;

	// Scator real enabled by default
	CVector4 zz = z * z;
	CVector4 newZ = z;
	if (fractal->transformCommon.functionEnabledFalse)
	{ // scator imaginary
		newZ.x = zz.x - zz.y - zz.z;
		newZ.y = z.x * z.y;
		newZ.z = z.x * z.z;
		newZ *= fractal->transformCommon.constantMultiplier122;
		newZ.x += (zz.y * zz.z) / zz.x;
		newZ.y *= (1.0 - zz.z / zz.x);
		newZ.z *= (1.0 - zz.y / zz.x);
	}
	else
	{ // scator real
		newZ.x = zz.x + zz.y + zz.z;
		newZ.y = z.x * z.y;
		newZ.z = z.x * z.z;
		newZ *= fractal->transformCommon.constantMultiplier122;
		newZ.x += (zz.y * zz.z) / zz.x;
		newZ.y *= (1.0 + zz.z / zz.x);
		newZ.z *= (1.0 + zz.y / zz.x);
	}
	z = newZ;

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		CVector4 c = aux.const_c;
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}

	if (fractal->analyticDE.enabled)
	{
		double r = aux.r; // r = sqrt(zz.x + zz.y + zz.z + (zz.y * zz.z) / zz.x);
		double vecDE = 0.0;
		if (!fractal->analyticDE.enabledFalse)
		{
			if (fractal->transformCommon.functionEnabledXFalse)
			{
				r = oldZ.Length();
			}
			aux.DE = r * aux.DE * 2.0 * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
		}
		else
		{
			vecDE = fractal->transformCommon.scaleA1 * z.Length() / oldZ.Length();
			aux.DE =
				max(r, vecDE) * aux.DE * 2.0 * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
		}
	}
}

/**
 * quadratic iteration in real or imaginary scator algebra
 * using r = length
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * ix-possibly-the-holy-grail-fractal-%28in-fff-lore%29
 * https://luz.izt.uam.mx/drupal/en/fractals/ix
 * @author Manuel Fernandez-Guasti
 * This formula contains aux.DE
 */
void ScatorPower2StdRIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;

	// Scator real enabled
	CVector4 zz = z * z;
	CVector4 newZ = z;
	if (fractal->transformCommon.functionEnabledFalse)
	{ // scator imaginary
		newZ.x = zz.x - zz.y - zz.z;
		newZ.y = z.x * z.y;
		newZ.z = z.x * z.z;
		newZ *= fractal->transformCommon.constantMultiplier122;
		newZ.x += (zz.y * zz.z) / zz.x;
		newZ.y *= (1.0 - zz.z / zz.x);
		newZ.z *= (1.0 - zz.y / zz.x);
	}
	else
	{ // scator real
		newZ.x = zz.x + zz.y + zz.z;
		newZ.y = z.x * z.y;
		newZ.z = z.x * z.z;
		newZ *= fractal->transformCommon.constantMultiplier122;
		newZ.x += (zz.y * zz.z) / zz.x;
		newZ.y *= (1.0 + zz.z / zz.x);
		newZ.z *= (1.0 + zz.y / zz.x);
	}
	z = newZ;

	if (fractal->analyticDE.enabled)
	{
		double r = aux.r; // r = length;
		double vecDE = 0.0;
		if (!fractal->analyticDE.enabledFalse)
		{
			if (fractal->transformCommon.functionEnabledXFalse)
			{
				r = sqrt(zz.x + zz.y + zz.z + (zz.y * zz.z) / zz.x);
			}
			aux.DE = r * aux.DE * 2.0 * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
		}
		else
		{
			vecDE = fractal->transformCommon.scaleA1 * z.Length() / oldZ.Length();
			aux.DE =
				max(r, vecDE) * aux.DE * 2.0 * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
		}
	}
}

/**
 * Sierpinski3D. made from DarkBeam's Sierpinski code from M3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void Sierpinski3dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	CVector4 temp = z;

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

	z *= fractal->transformCommon.scaleA2;

	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z -= fractal->transformCommon.offset111; // neg offset
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (!fractal->analyticDE.enabledFalse)
		aux.DE *= fabs(fractal->transformCommon.scale2);
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale2) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset0;
}

/**
 * Vicsek
 */
void VicsekIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;

	// octo
	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		if (z.x + z.y < 0.0) z = CVector4(-z.y, -z.x, z.z, z.w);

		if (z.x + z.z < 0.0) // z.xz = -z.zx;
			z = CVector4(-z.z, z.y, -z.x, z.w);

		if (z.x - z.y < 0.0) // z.xy = z.yx;
			z = CVector4(z.y, z.x, z.z, z.w);

		if (z.x - z.z < 0.0) // z.xz = z.zx;
			z = CVector4(z.z, z.y, z.x, z.w);

		z.x = fabs(z.x);
		z = z * fractal->transformCommon.scale2
				- fractal->transformCommon.offset100 * (fractal->transformCommon.scale2 - 1.0);

		aux.DE *= fractal->transformCommon.scale2;
	}

	// spherical fold
	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);
		rrCol = rr; // test ooooooooooooooooooooooooooooooooo
		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < fractal->transformCommon.minR2p25)
		{
			double tglad_factor1 = fractal->transformCommon.maxMinR2factor;
			// double tglad_factor1 = fractal->transformCommon.maxR2d1 /fractal->transformCommon.minR2p25;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
			aux.color += fractal->mandelbox.color.factorSp1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
		z *= fractal->transformCommon.scale08;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale08);
	}

	// Vicsek
	if (fractal->transformCommon.functionEnabledM
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		zCol = z; // test ooooooooooooooooooooooooooooooooo
		z = fabs(z);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		z *= fractal->transformCommon.scale3;
		aux.DE *= fractal->transformCommon.scale3;

		CVector4 limit = fractal->transformCommon.offset111;
		if (z.x > limit.x * 0.5) z.x -= limit.x * fractal->transformCommon.scaleA1;
		if (z.y > limit.y * 0.5) z.y -= limit.y;
		if (z.z > limit.z) z.z -= 2.0 * limit.z;
		z.x += fractal->transformCommon.offset0;
		zCol = z; // test ooooooooooooooooooooooooooooooooo
	}

	// 45 rot XY
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		double xTemp = SQRT_1_2 * (z.x - z.y);
		z.y = SQRT_1_2 * (z.y + z.x);
		z.x = xTemp;
	}

	// iter weight
	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector4 zA = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
		CVector4 zB = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offsetA0)
				+ (zB * fractal->transformCommon.offsetB0);
		aux.DE *= fractal->transformCommon.scale1;
	}

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	// aux color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd += fractal->mandelbox.color.factor.x
										* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
			if (zCol.y != oldZ.y)
				colorAdd += fractal->mandelbox.color.factor.y
										* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
			if (zCol.z != oldZ.z)
				colorAdd += fractal->mandelbox.color.factor.z
										* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

			if (rrCol < fractal->transformCommon.maxR2d1)
			{
				if (rrCol < fractal->transformCommon.minR2p25)
					colorAdd +=
						fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
						+ fractal->mandelbox.color.factorSp2
								* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
				else
					colorAdd +=
						fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			}
		}
		else
		{
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;

			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1;
			else if (rrCol < fractal->transformCommon.maxR2d1)
				colorAdd += fractal->mandelbox.color.factorSp2;
		}
		aux.color += colorAdd;
	}
}

/**
 * GeneralizedFoldBoxIteration - Quaternion fractal with extended controls
 * @reference http://www.fractalforums.com/new-theories-and-research/generalized-box-fold/
 * This formula contains aux.color
 */
void GeneralizedFoldBoxIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector3 zXYZ = z.GetXYZ();
	int i;
	const CVector3 *Nv;
	int sides;

	switch (fractal->genFoldBox.type)
	{
		default:
		case generalizedFoldBoxType_foldTet:
			Nv = fractal->genFoldBox.Nv_tet;
			sides = fractal->genFoldBox.sides_tet;
			break;
		case generalizedFoldBoxType_foldCube:
			Nv = fractal->genFoldBox.Nv_cube;
			sides = fractal->genFoldBox.sides_cube;
			break;
		case generalizedFoldBoxType_foldOct:
			Nv = fractal->genFoldBox.Nv_oct;
			sides = fractal->genFoldBox.sides_oct;
			break;
		case generalizedFoldBoxType_foldDodeca:
			Nv = fractal->genFoldBox.Nv_dodeca;
			sides = fractal->genFoldBox.sides_dodeca;
			break;
		case generalizedFoldBoxType_foldOctCube:
			Nv = fractal->genFoldBox.Nv_oct_cube;
			sides = fractal->genFoldBox.sides_oct_cube;
			break;
		case generalizedFoldBoxType_foldIcosa:
			Nv = fractal->genFoldBox.Nv_icosa;
			sides = fractal->genFoldBox.sides_icosa;
			break;
		case generalizedFoldBoxType_foldBox6:
			Nv = fractal->genFoldBox.Nv_box6;
			sides = fractal->genFoldBox.sides_box6;
			break;
		case generalizedFoldBoxType_foldBox5:
			Nv = fractal->genFoldBox.Nv_box5;
			sides = fractal->genFoldBox.sides_box5;
			break;
	}

	double melt = fractal->mandelbox.melt;
	double solid = fractal->mandelbox.solid;

	// Find the closest cutting plane if any that cuts the line between the origin and z.
	// Line is parameterized as X = Y + L*a;
	// Cutting plane is X.Dot(Nv) = Solid.
	// (Y + L*a).Dot(Nv) = solid.
	// a = (solid - Y.Dot(Nv))/L.Dot(Nv) = b/c
	CVector3 L = zXYZ;
	double a = 1.0;
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
		CVector3 X_m = zXYZ - Nv_m * (zXYZ.Dot(Nv_m) - solid);

		// Find any plane (Nv_r) closest to X_m that cuts the line between Nv_m and X_m.
		// Nv_m cross Nv_r will define a possible rotation axis.
		// a = (solid - Y.Dot(Nv)/L.Dot(Nv) = b/c.
		L = X_m - Nv_m;
		Y = Nv_m;
		a = 1.0;
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
			double a_rmin = (zXYZ.Dot(L_r) - Xmr_intersect.Dot(L_r)) / (L_r.Dot(L_r));

			// force a_rmin to be positive. I think I made an even number of sign errors here.
			if (a_rmin < 0.0)
			{
				a_rmin = -a_rmin;
				L_r = L_r * (-1.0);
			}
			CVector3 X_r = Xmr_intersect + L_r * a_rmin;

			// Find any plane (Nv_i) closest to Xmr_intersect that cuts the line between
			// Xmr_intersect and X_r. This will define a possible inversion point.
			// a = (solid - Y.Dot(Nv)/L.Dot(Nv) = b/c.
			L = X_r - Xmr_intersect;
			Y = Xmr_intersect;
			a = 1.0;
			side = -1;

			for (i = 0; i < sides; i++)
			{
				if ((i != side_m) && (i != side_r))
				{
					b = solid - Y.Dot(Nv[i]);
					c = L.Dot(Nv[i]);
					// A bit subtle here. a must be positive and I want to avoid divide by zero.
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
				CVector3 z2X = X_i - zXYZ;
				// Is z above the melt layer.
				if (z2X.Dot(z2X) > (melt * melt))
				{
					double z2X_mag = z2X.Length();
					zXYZ += z2X * (2.0 * (z2X_mag - melt) / (z2X_mag + .00000001));
					aux.color += fractal->mandelbox.color.factor.z;
				}
			}
			else
			{
				// Only rotation line possible but still need to check for melt.
				// Is z above the melt layer.
				CVector3 z2X = X_r - zXYZ;
				if (z2X.Dot(z2X) > (melt * melt))
				{
					double z2X_mag = z2X.Length();
					zXYZ += z2X * (2.0 * (z2X_mag - melt) / (z2X_mag + .00000001));
					aux.color += fractal->mandelbox.color.factor.y;
				}
			}
		}
		else
		{
			// Only mirror plane possible but still need to check for melt.
			CVector3 z2X = X_m - zXYZ;
			if (z2X.Dot(z2X) > (melt * melt))
			{
				double z2X_mag = z2X.Length();
				zXYZ += z2X * (2.0 * (z2X_mag - melt) / (z2X_mag + .00000001));
				aux.color += fractal->mandelbox.color.factor.x;
			}
		}
	} // outside solid

	double r2 = zXYZ.Dot(zXYZ);

	z = CVector4(zXYZ, z.w);

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

	z *= fractal->mandelbox.scale;
	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
}

void BoxFolding(CVector4 &z, const sFractalFoldings *foldings, sExtendedAux &aux)
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

void SphericalFolding(CVector4 &z, const sFractalFoldings *foldings, sExtendedAux &aux)
{
	double fR2_2 = foldings->sphericalOuter * foldings->sphericalOuter;
	double mR2_2 = foldings->sphericalInner * foldings->sphericalInner;
	double r2_2 = aux.r * aux.r;
	double fold_factor1_2 = fR2_2 / mR2_2;

	if (r2_2 < mR2_2)
	{
		z = z * fold_factor1_2;
		aux.DE *= fold_factor1_2;
		aux.color *= 0.9;
	}
	else if (r2_2 < fR2_2)
	{
		double fold_factor2_2 = fR2_2 / r2_2;
		z = z * fold_factor2_2;
		aux.DE *= fold_factor2_2;
		aux.color *= 0.9;
	}
}

// NEW TRANSFORM FORMULAS-----------------------------------------------------------------
/**
 * abs add  constant,  z = fabs( z + pre-offset) + post-offset
 */
void TransfAbsAddConstantIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);

	z += fractal->transformCommon.offsetA000;
}

/**
 * abs add tglad fold,  z = fabs( z + constant) -fabs( z - constant) - z:
 * with a fold tweak option
 * This formula contains aux.color
 */
void TransfAbsAddTgladFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 limit = fractal->transformCommon.additionConstant000;
	double colorAdd = 0.0;
	CVector4 oldZ = z;
	z = fabs(z + limit) - fabs(z - limit) - z;

	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		CVector4 length = 2.0 * limit;
		CVector4 tgladS = 1.0 / length;
		CVector4 Add = CVector4(0.0, 0.0, 0.0, 0.0);
		;
		if (fabs(z.x) < limit.x) Add.x = z.x * z.x * tgladS.x;
		if (fabs(z.y) < limit.y) Add.y = z.y * z.y * tgladS.y;
		if (fabs(z.z) < limit.z) Add.z = z.z * z.z * tgladS.z;

		if (fabs(z.x) > limit.x && fabs(z.x) < length.x)
			Add.x = (length.x - fabs(z.x)) * (length.x - fabs(z.x)) * tgladS.x;
		if (fabs(z.y) > limit.y && fabs(z.y) < length.y)
			Add.y = (length.y - fabs(z.y)) * (length.y - fabs(z.y)) * tgladS.y;
		if (fabs(z.z) > limit.z && fabs(z.z) < length.z)
			Add.z = (length.z - fabs(z.z)) * (length.z - fabs(z.z)) * tgladS.z;
		Add *= fractal->transformCommon.offset000;
		z.x = (z.x - (sign(z.x) * (Add.x)));
		z.y = (z.y - (sign(z.y) * (Add.y)));
		z.z = (z.z - (sign(z.z) * (Add.z)));
	}
	// aux.color mode 1
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
	}
	// mode 2
	if (fractal->transformCommon.functionEnabledCxFalse)
	{
		if (fabs(z.x) > limit.x)
		{
			colorAdd += fractal->mandelbox.color.factor.x;
		}
		else
		{
			colorAdd += fractal->mandelbox.color.factor.x * (1.0 - (limit.x - fabs(z.x)) / limit.x);
		}

		if (fabs(z.y) > limit.y)
		{
			colorAdd += fractal->mandelbox.color.factor.y;
		}
		else
		{
			colorAdd += fractal->mandelbox.color.factor.y * (1.0 - (limit.y - fabs(z.y)) / limit.y);
		}

		if (fabs(z.z) > limit.z)
		{
			colorAdd += fractal->mandelbox.color.factor.z;
		}
		else
		{
			colorAdd += fractal->mandelbox.color.factor.z * (1.0 - (limit.z - fabs(z.z)) / limit.z);
		}
	}

	// mode 3
	if (fractal->transformCommon.functionEnabledCyFalse)
	{
		// double valMinusLim = limit;
		if (z.x != oldZ.x)
		{
			colorAdd += fractal->mandelbox.color.factor.x * (fabs(z.x) - limit.x) / limit.x;
		}
		if (z.y != oldZ.y)
		{
			colorAdd += fractal->mandelbox.color.factor.y * (fabs(z.y) - limit.y) / limit.y;
		}
		if (z.z != oldZ.z)
		{
			colorAdd += fractal->mandelbox.color.factor.z * (fabs(z.z) - limit.z) / limit.z;
		}
	}
	aux.color += colorAdd;
}

/**
 * abs add conditional.
 */
void TransfAbsAddConditionalIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledx)
	{
		z.x = sign(z.x)
					* (fractal->transformCommon.offset111.x - fabs(z.x)
							+ fabs(z.x) * fractal->transformCommon.scale3D000.x);
	}
	/*{
		double signZx = sign(z.x);
		z.x = (signZx * fractal->transformCommon.offset111.x)
			- fabs(z.x) * (signZx - fractal->transformCommon.offset000.x);
	}*/
	/*{
		double Absx = fabs(z.x);
		z.x = sign(z.x) * (fractal->transformCommon.offset111.x - Absx
			+ Absx * fractal->transformCommon.offset000.x);
	}*/

	if (fractal->transformCommon.functionEnabledy)
	{
		z.y = sign(z.y)
					* (fractal->transformCommon.offset111.y - fabs(z.y)
							+ fabs(z.y) * fractal->transformCommon.offset000.y);
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		z.z = sign(z.z)
					* (fractal->transformCommon.offset111.z - fabs(z.z)
							+ fabs(z.z) * fractal->transformCommon.offset000.z);
	}

	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}

/**
 * abs add conditional2.
 */
void TransfAbsAddConditional2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 Offset = fractal->transformCommon.offset111;
	CVector4 OffsetA = fractal->transformCommon.offsetA111;

	if (fractal->transformCommon.functionEnabled) OffsetA = Offset;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (z.x < OffsetA.x) z.x = (fabs(z.x) - Offset.x) + Offset.x;
	}
	if (fractal->transformCommon.functionEnabledy)
	{
		if (z.y < OffsetA.y) z.y = (fabs(z.y) - Offset.y) + Offset.y;
	}
	if (fractal->transformCommon.functionEnabledz)
	{
		if (z.z < OffsetA.z) z.z = (fabs(z.z) - Offset.z) + Offset.z;
	}

	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}

/**
 * abs Negative abs constant,  z = - abs( z + constant)
 */
void TransfNegAbsAddConstantIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	z += fractal->transformCommon.additionConstant000;
	if (fractal->transformCommon.functionEnabledx) z.x = -fabs(z.x);

	if (fractal->transformCommon.functionEnabledy) z.y = -fabs(z.y);

	if (fractal->transformCommon.functionEnabledz) z.z = -fabs(z.z);
}

/**
 * abs Multi - Multiple parameters for abs functions
 */
void TransfAbsAddMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	CVector4 tempA = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 tempB = CVector4(0.0, 0.0, 0.0, 0.0);

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

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * Adds c constant to z vector
 */
void TransfAddConstantIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	z += fractal->transformCommon.additionConstant000;
	z += fractal->transformCommon.additionConstantA000;
}

/**
 * Adds c constant to z vector
 */
void TransfAddConstantMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);
	// std offset
	z += fractal->transformCommon.additionConstantA000;
	// polynomial
	if (fractal->transformCommon.functionEnabledBx
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		CVector4 temp = fractal->transformCommon.additionConstant000;
		CVector4 temp2 = temp * temp;
		CVector4 temp3 = z * z * fractal->transformCommon.scaleA1;
		z.x -=
			((temp.x * temp2.x) / (temp3.x + temp2.x) - 2.0 * temp.x) * fractal->transformCommon.scale1;
		z.y -=
			((temp.y * temp2.y) / (temp3.y + temp2.y) - 2.0 * temp.y) * fractal->transformCommon.scale1;
		z.z -=
			((temp.z * temp2.z) / (temp3.z + temp2.z) - 2.0 * temp.z) * fractal->transformCommon.scale1;
	}
	else if (fractal->transformCommon.functionEnabledByFalse
					 && aux.i >= fractal->transformCommon.startIterationsX
					 && aux.i < fractal->transformCommon.stopIterationsX)
	{
		CVector4 temp = fractal->transformCommon.additionConstant000;
		CVector4 temp2 = temp * temp;
		CVector4 temp3 = z * z * fractal->transformCommon.scaleA1;
		z.x -= ((temp2.x) / (temp3.x + temp2.x) - 2.0 * temp.x)
					 * fractal->transformCommon.scale1; // * sign(z.x);
		z.y -= ((temp2.y) / (temp3.y + temp2.y) - 2.0 * temp.y)
					 * fractal->transformCommon.scale1; // * sign(z.y);
		z.z -= ((temp2.z) / (temp3.z + temp2.z) - 2.0 * temp.z)
					 * fractal->transformCommon.scale1; // * sign(z.z);
	}
}

/**
 * Adds c constant to z vector. C addition constant varies based on iteration parameters.
 */
void TransfAddConstantVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempVC = fractal->transformCommon.additionConstant000; // constant to be varied
	if (aux.i < fractal->transformCommon.startIterations250)
	{
		;
	}
	if (aux.i >= fractal->transformCommon.startIterations250
			&& aux.i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
	{
		int iterationRange =
			fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250;
		int currentIteration = (aux.i - fractal->transformCommon.startIterations250);
		tempVC += fractal->transformCommon.offset000 * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset000);
	}
	z += tempVC;
}

/**
 * Adds Cpixel constant to z vector
 */
void TransfAddCpixelIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += aux.const_c * fractal->transformCommon.constantMultiplier111;
}

/**
 * Adds Cpixel constant to z vector, swapping the Cpixel vector x and y axes
 * disable swap for normal mode
 */
void TransfAddCpixelCxCyAxisSwapIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	CVector4 tempC = c;
	if (fractal->transformCommon.functionEnabled)
	{
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w);
			aux.c = tempC;
		}
		else
		{
			tempC = CVector4(c.y, c.x, c.z, c.w);
		}
	}
	z += tempC * fractal->transformCommon.constantMultiplier111;
}

/**
 * Adds Cpixel constant to z vector. Possible to swap Cpixel vector axes.
 */
void TransfAddCpixelAxisSwapIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	CVector4 tempC = c;
	if (fractal->transformCommon.alternateEnabledFalse) // alternate
	{
		tempC = aux.c;
		switch (fractal->mandelbulbMulti.orderOfXYZ)
		{
			case multi_OrderOfXYZ_xyz:
			default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
			case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
			case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
			case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
			case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
			case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
		}
		aux.c = tempC;
	}
	else
	{
		switch (fractal->mandelbulbMulti.orderOfXYZ)
		{
			case multi_OrderOfXYZ_xyz:
			default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
			case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
			case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
			case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
			case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
			case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
		}
	}
	z += tempC * fractal->transformCommon.constantMultiplier111;
}

/**
 * Adds rotated Cpixel constant to z vector. Possible to swap Cpixel vector axes.
 */
void TransfAddCpixelRotatedIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	CVector4 tempC = c;
	if (fractal->transformCommon.alternateEnabledFalse) // alternate
	{
		tempC = aux.c;
		switch (fractal->mandelbulbMulti.orderOfXYZ)
		{
			case multi_OrderOfXYZ_xyz:
			default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
			case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
			case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
			case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
			case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
			case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
		}
		aux.c = tempC;
	}
	else
	{
		switch (fractal->mandelbulbMulti.orderOfXYZ)
		{
			case multi_OrderOfXYZ_xyz:
			default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
			case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
			case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
			case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
			case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
			case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
		}
	}
	if (aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		tempC = fractal->transformCommon.rotationMatrix.RotateVector(tempC);
	}
	z += tempC * fractal->transformCommon.constantMultiplier111;
}

/**
 * Adds Cpixel constant to z vector, scator algebra
 * based on Manuel's math
 * @reference
 * https://luz.izt.uam.mx/drupal/en/fractals/hun
 * @author Manuel Fernandez-Guasti
 */
void TransfAddCpixelScatorIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 tempC = aux.const_c;
	if (fractal->transformCommon.functionEnabledSwFalse) swap(tempC.x, tempC.z);

	CVector4 cc = tempC * tempC;
	CVector4 newC = tempC;
	double limitA = fractal->transformCommon.scale0;

	if (fractal->transformCommon.functionEnabledRFalse)
	{
		cc = fabs(tempC);
	}

	// scator algebra
	if (cc.x < limitA)
	{
		double temp = 1.0 / cc.x - 1.0;
		cc.x = temp;
	}

	if (!fractal->transformCommon.functionEnabledFalse)
	{																	// real
		newC.x += (cc.y * cc.z) / cc.x; // all pos
		newC.y *= (1.0 + cc.z / cc.x);
		newC.z *= (1.0 + cc.y / cc.x);
		newC *= fractal->transformCommon.constantMultiplier111;
		if (fractal->transformCommon.functionEnabledSwFalse) swap(newC.x, newC.z);

		if (!fractal->transformCommon.functionEnabledSFalse)
		{
			z += newC;
		}
		else
		{
			z.x += sign(z.x) * newC.x;
			z.y += sign(z.y) * newC.y;
			z.z += sign(z.z) * newC.z;
		}
	}
	else
	{																	// imaginary
		newC.x += (cc.y * cc.z) / cc.x; // pos
		newC.y *= (1.0 - cc.z / cc.x);	// pos  neg
		newC.z *= (1.0 - cc.y / cc.x);	// pos  neg
		newC *= fractal->transformCommon.constantMultiplier111;
		if (fractal->transformCommon.functionEnabledy) newC.y = fabs(newC.y);
		if (fractal->transformCommon.functionEnabledz) newC.z = fabs(newC.z);

		if (fractal->transformCommon.functionEnabledSwFalse) swap(newC.x, newC.z);

		// add Cpixel
		if (!fractal->transformCommon.functionEnabledSFalse)
		{
			z += newC;
		}
		else
		{
			z.x += sign(z.x) * newC.x;
			z.y += sign(z.y) * newC.y;
			z.z += sign(z.z) * newC.z;
		}
	}
	// DE calculations
	if (fractal->analyticDE.enabledFalse)
	{
		double vecDE = z.Length() / oldZ.Length();
		aux.DE = aux.DE * vecDE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
}

/**
 * Adds Cpixel constant sin or cos
 */
void TransfAddCpixelSinOrCosIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 trigC = aux.const_c * fractal->transformCommon.constantMultiplierC111; // freq
	CVector4 trigC0 = trigC;

	if (!fractal->transformCommon.functionEnabledBxFalse)
	{
		// apply simple trig
		if (fractal->transformCommon.functionEnabledAx) trigC.x = sin(trigC0.x);
		if (fractal->transformCommon.functionEnabledAxFalse) trigC.x = cos(trigC0.x);
		if (fractal->transformCommon.functionEnabledAy) trigC.y = sin(trigC0.y);
		if (fractal->transformCommon.functionEnabledAyFalse) trigC.y = cos(trigC0.y);
		if (fractal->transformCommon.functionEnabledAz) trigC.z = sin(trigC0.z);
		if (fractal->transformCommon.functionEnabledAzFalse) trigC.z = cos(trigC0.z);
	}
	else
	{
		// mode2
		if (fractal->transformCommon.functionEnabledAx)
			trigC.x = (1.0 + sin(trigC0.x)) * sign(trigC.x) / 2.0;
		if (fractal->transformCommon.functionEnabledAxFalse)
			trigC.x = (1.0 + cos(trigC0.x)) * sign(trigC.x) / 2.0;
		if (fractal->transformCommon.functionEnabledAy)
			trigC.y = (1.0 + sin(trigC0.y)) * sign(trigC.y) / 2.0;
		if (fractal->transformCommon.functionEnabledAyFalse)
			trigC.y = (1.0 + cos(trigC0.y)) * sign(trigC.y) / 2.0;
		if (fractal->transformCommon.functionEnabledAz)
			trigC.z = (1.0 + sin(trigC0.z)) * sign(trigC.z) / 2.0;
		if (fractal->transformCommon.functionEnabledAzFalse)
			trigC.z = (1.0 + cos(trigC0.z)) * sign(trigC.z) / 2.0;
		// mode3
		if (fractal->transformCommon.functionEnabledByFalse) trigC *= trigC0;
	}
	trigC *= fractal->transformCommon.constantMultiplier111; // ampl

	// add cPixel
	if (!fractal->transformCommon.functionEnabledFalse)
	{
		z += trigC;
	}
	else
	{ // symmetical
		trigC = fabs(trigC);

		z.x += sign(z.x) * trigC.x;
		z.y += sign(z.y) * trigC.y;
		z.z += sign(z.z) * trigC.z;
	}

	// DE tweak (sometimes hepls)
	if (fractal->analyticDE.enabledFalse)
	{
		double tweakDE = z.Length() / oldZ.Length();
		aux.DE = aux.DE * tweakDE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}

/**
 * Adds Cpixel constant to z vector, with symmetry
 */
void TransfAddCpixelSymmetricalIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempFAB = aux.const_c;
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
void TransfAddCpixelVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempVC = fractal->transformCommon.constantMultiplier111; // constant to be varied
	if (aux.i < fractal->transformCommon.startIterations250)
	{
		;
	}
	if (aux.i >= fractal->transformCommon.startIterations250
			&& aux.i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
	{

		int iterationRange =
			fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250;
		int currentIteration = (aux.i - fractal->transformCommon.startIterations250);
		tempVC += fractal->transformCommon.offset000 * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset000);
	}
	z += aux.const_c * tempVC;
}

/**
 * Add exp2(z)
 * z vector version  and the xyz axis version can be used together
 */
void TransfAddExp2ZIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempZ = z;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (fractal->transformCommon.functionEnabledAx)
		{
			tempZ.x = fabs(z.x);
			tempZ.x = exp2(tempZ.x * -fractal->transformCommon.constantMultiplier000.x) - 1.0;
			z.x += sign(z.x) * tempZ.x;
		}
		else if (fractal->transformCommon.functionEnabledAxFalse)
		{
			tempZ.x = fabs(z.x);
			tempZ.x = exp2(tempZ.x * -fractal->transformCommon.constantMultiplier000.x) - 1.0;
			z.x -= sign(z.x) * tempZ.x;
		}
		else // no fabs
			z.x += exp2(tempZ.x * fractal->transformCommon.constantMultiplier000.x) - 1.0;
	}

	if (fractal->transformCommon.functionEnabledy)
	{
		if (fractal->transformCommon.functionEnabledAy)
		{
			tempZ.y = fabs(z.y);
			tempZ.y = exp2(tempZ.y * -fractal->transformCommon.constantMultiplier000.y) - 1.0;
			z.y += sign(z.y) * tempZ.y;
		}
		else if (fractal->transformCommon.functionEnabledAyFalse)
		{
			tempZ.y = fabs(z.y);
			tempZ.y = exp2(tempZ.y * -fractal->transformCommon.constantMultiplier000.y) - 1.0;
			z.y -= sign(z.y) * tempZ.y;
		}
		else
			z.y += exp2(tempZ.y * fractal->transformCommon.constantMultiplier000.y) - 1.0;
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		if (fractal->transformCommon.functionEnabledAz)
		{
			tempZ.z = fabs(z.z);
			tempZ.z = exp2(tempZ.z * -fractal->transformCommon.constantMultiplier000.z) - 1.0;
			z.z += sign(z.z) * tempZ.z;
		}
		else if (fractal->transformCommon.functionEnabledAzFalse)
		{
			tempZ.z = fabs(z.z);
			tempZ.z = exp2(tempZ.z * -fractal->transformCommon.constantMultiplier000.z) - 1.0;
			z.z -= sign(z.z) * tempZ.z;
		}
		else
			z.z += exp2(tempZ.z * fractal->transformCommon.constantMultiplier000.z) - 1.0;
	}

	// vector z version
	if (fractal->transformCommon.functionEnabledBzFalse)
	{
		CVector4 tempS;
		if (fractal->transformCommon.functionEnabled)
		{
			tempS = fabs(z);
			CVector4 tempT = tempS * -fractal->transformCommon.scale0;
			tempS.x = exp2(tempT.x) - 1.0;
			tempS.y = exp2(tempT.y) - 1.0;
			tempS.z = exp2(tempT.z) - 1.0;

			z.x += sign(z.x) * tempS.x;
			z.y += sign(z.y) * tempS.y;
			z.z += sign(z.z) * tempS.z;
		}
		else if (fractal->transformCommon.functionEnabledFalse)
		{
			tempS = fabs(z);
			CVector4 tempT = tempS * -fractal->transformCommon.scale0;
			tempS.x = exp2(tempT.x) - 1.0;
			tempS.y = exp2(tempT.y) - 1.0;
			tempS.z = exp2(tempT.z) - 1.0;

			z.x -= sign(z.x) * tempS.x;
			z.y -= sign(z.y) * tempS.y;
			z.z -= sign(z.z) * tempS.z;
		}
		else
		{
			CVector4 tempR = z * fractal->transformCommon.scale0;
			z.x += exp2(tempR.x) - 1.0;
			z.y += exp2(tempR.y) - 1.0;
			z.z += exp2(tempR.z) - 1.0;
		}
	}
	if (fractal->transformCommon.functionEnabledByFalse)
		aux.DE = aux.DE + exp2(aux.DE * -fractal->transformCommon.scale0) - 1.0;
	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}

/**
 * benesiT1  3D
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransfBenesiT1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector4(
		(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

	CVector4 temp = z;
	double tempL = temp.Length();
	z = fabs(z) * fractal->transformCommon.scale3D222;
	// if (tempL < 1e-21) tempL = 1e-21;
	double avgScale = z.Length() / tempL;
	aux.DE = aux.DE * avgScale + 1.0;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	tempXZ = (z.y + z.x) * SQRT_1_2;

	z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
		z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	z = z - fractal->transformCommon.offset200;
}

/**
 * benesiT1Mod  3D based on benesiT1
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransfBenesiT1ModIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector4(
		(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

	CVector4 temp = z;
	double tempL = temp.Length();
	z = fabs(z) * fractal->transformCommon.scale3D333;
	// if (tempL < 1e-21) tempL = 1e-21;
	double avgScale = z.Length() / tempL;
	aux.DE = aux.DE * avgScale + 1.0;

	z = (fabs(z + fractal->transformCommon.additionConstant111)
			 - fabs(z - fractal->transformCommon.additionConstant111) - z);

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	tempXZ = (z.y + z.x) * SQRT_1_2;

	z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
		z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
}

/**
 * benesi T2
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransfBenesiT2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector4(
		(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

	CVector4 tempV2 = z;
	tempV2.x = sqrt(z.y * z.y + z.z * z.z);
	tempV2.y = sqrt(z.x * z.x + z.z * z.z); // switching, squared, sqrt
	tempV2.z = sqrt(z.x * z.x + z.y * z.y);

	z = fabs(tempV2 - fractal->transformCommon.additionConstant111);

	CVector4 temp = z;
	double tempL = temp.Length();
	z = fabs(z) * fractal->transformCommon.scale3D444;
	// if (tempL < 1e-21) tempL = 1e-21;
	double avgScale = z.Length() / tempL;
	aux.DE = aux.DE * avgScale + 1.0;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	tempXZ = (z.y + z.x) * SQRT_1_2;
	z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
		z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
}

/**
 * benesi T3
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransfBenesiT3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector4(
		(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

	CVector4 tempV2 = z;
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
	z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
		z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
}

/**
 * benesi T4
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransfBenesiT4Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector4(
		(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

	CVector4 tempV2 = z;
	tempV2.x = (z.y * z.y + z.z * z.z);
	tempV2.y = (z.x * z.x + z.z * z.z); // switching, squared,
	tempV2.z = (z.x * z.x + z.y * z.y);
	z = (fabs(tempV2 - fractal->transformCommon.additionConstant111))
			* fractal->transformCommon.scale3D222;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	tempXZ = (z.y + z.x) * SQRT_1_2;
	z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
		z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
}

/**
 * benesi T5
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransfBenesiT5bIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector4(
		(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

	// if (z.x > -1e-21 && z.x < 1e-21)
	// z.x = (z.x > 0) ? 1e-21 : -1e-21;
	// if (z.y > -1e-21 && z.y < 1e-21)
	// z.y = (z.y > 0) ? 1e-21 : -1e-21;
	// if (z.z > -1e-21 && z.z < 1e-21)
	// z.z = (z.z > 0) ? 1e-21 : -1e-21;
	CVector4 tempV2 = z;
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
	z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
		z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
}

/**
 * benesi mag forward
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransfBenesiMagForwardIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	Q_UNUSED(aux);

	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector4(
		(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
}

/**
 * benesi mag backward
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
 */
void TransfBenesiMagBackwardIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	Q_UNUSED(aux);

	double tempXZ = (z.y + z.x) * SQRT_1_2;
	z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
		z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
}

/**
 * Benesi Cube to sphere transform
 * Warps a cube to a sphere; transform made by M.Benesi, optimized by Luca.
 * @reference http://www.fractalforums.com/mathematics/circle2square/
 */
void TransfBenesiCubeSphereIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	Q_UNUSED(aux);

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
void TransfBenesiSphereCubeIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	Q_UNUSED(aux);

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
 * Blockify
 * based on a block of Fragmentarium code, from Adam Nixon
 * analytic aux.DE
 */
void TransfBlockifyIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	double master = fractal->transformCommon.scale / 100.0;
	CVector4 bSize = fractal->transformCommon.constantMultiplier111 * master;
	// bsize maybe shortened to a double??

	if (!fractal->transformCommon.functionEnabledFalse)
	{
		if (!fractal->transformCommon.functionEnabledDFalse)
		{
			// if (fractal->transformCommon.functionEnabledCx) z.x = floor(z.x / bSize.x) * bSize.x;
			// if (fractal->transformCommon.functionEnabledCy) z.y = floor(z.y / bSize.y) * bSize.y;
			// if (fractal->transformCommon.functionEnabledCz) z.z = floor(z.z / bSize.z) * bSize.z;
			if (fractal->transformCommon.functionEnabledCx) z.x = (floor(z.x / bSize.x) + 0.5) * bSize.x;
			if (fractal->transformCommon.functionEnabledCy) z.y = (floor(z.y / bSize.y) + 0.5) * bSize.y;
			if (fractal->transformCommon.functionEnabledCz) z.z = (floor(z.z / bSize.z) + 0.5) * bSize.z;
			// if (fractal->transformCommon.functionEnabledCx) z.x = (trunc(z.x / bSize.x) + sign(z.x) *
			// 0.5) * bSize.x;
			// if (fractal->transformCommon.functionEnabledCy) z.y = (trunc(z.y / bSize.y) + sign(z.y) *
			// 0.5) * bSize.y;
			// if (fractal->transformCommon.functionEnabledCz) z.z = (trunc(z.z / bSize.z) + sign(z.z) *
			// 0.5) * bSize.z;
		}
		else // normalize
		{
			double rr = z.Dot(z);
			z /= rr;
			bSize /= 100.0;
			if (fractal->transformCommon.functionEnabledCx) z.x = floor(z.x / bSize.x) * bSize.x;
			if (fractal->transformCommon.functionEnabledCy) z.y = floor(z.y / bSize.y) * bSize.y;
			if (fractal->transformCommon.functionEnabledCz) z.z = floor(z.z / bSize.z) * bSize.z;
			z *= rr;
		}
	}
	else // radial
	{
		double rr = z.Dot(z);
		if (fractal->transformCommon.functionEnabledRFalse) rr = sqrt(rr); // z.Length();
		if (fractal->transformCommon.functionEnabledBxFalse) rr = z.x * z.x + z.y * z.y;
		if (fractal->transformCommon.functionEnabledByFalse) rr = z.y * z.y + z.z * z.z;
		if (fractal->transformCommon.functionEnabledBzFalse) rr = z.z * z.z + z.x * z.x;
		z /= rr;
		rr = floor(rr / master) * master;
		z *= rr;
	}

	// DE thing that has no effect, too small diff?
	if (fractal->transformCommon.functionEnabled)
	{
		double AN = z.Length() / oldZ.Length();
		aux.DE = aux.DE * AN; // * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}

	// post scale
	z *= fractal->transformCommon.scale1;
	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE *= fractal->transformCommon.scale1;
		else
			aux.DE = aux.DE * fractal->transformCommon.scale1 * fractal->analyticDE.scale1
							 + fractal->analyticDE.offset0;
	}
}

/**
 * Box Fold
 * This formula contains aux.color
 */
void TransfBoxFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	double colorAdd = 0.0;
	if (fabs(z.x) > fractal->mandelbox.foldingLimit)
	{
		z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
	}
	if (fabs(z.y) > fractal->mandelbox.foldingLimit)
	{
		z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
	}
	double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
	double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
	if (fabs(z.z) > zLimit)
	{
		z.z = sign(z.z) * zValue - z.z;
	}
	// aux.colo mode 1
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
	}

	// mode 2
	if (fractal->transformCommon.functionEnabledCxFalse)
	{
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			colorAdd += fractal->mandelbox.color.factor.x;
		}
		else
		{
			colorAdd +=
				fractal->mandelbox.color.factor.x
				* (1.0 - (fractal->mandelbox.foldingLimit - fabs(z.x)) / fractal->mandelbox.foldingLimit);
		}

		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			colorAdd += fractal->mandelbox.color.factor.y;
		}
		else
		{
			colorAdd +=
				fractal->mandelbox.color.factor.y
				* (1.0 - (fractal->mandelbox.foldingLimit - fabs(z.y)) / fractal->mandelbox.foldingLimit);
		}

		if (fabs(z.z) > fractal->mandelbox.foldingLimit)
		{
			colorAdd += fractal->mandelbox.color.factor.z;
		}
		else
		{
			colorAdd +=
				fractal->mandelbox.color.factor.z
				* (1.0 - (fractal->mandelbox.foldingLimit - fabs(z.z)) / fractal->mandelbox.foldingLimit);
		}
		aux.color += colorAdd;
	}

	// mode 3
	if (fractal->transformCommon.functionEnabledCyFalse)
	{
		double valMinusLim = fractal->mandelbox.foldingValue - fractal->mandelbox.foldingLimit;
		if (z.x != oldZ.x)
		{
			colorAdd += fractal->mandelbox.color.factor.x * (fabs(z.x) - fractal->mandelbox.foldingLimit)
									/ valMinusLim;
		}
		if (z.y != oldZ.y)
		{
			colorAdd += fractal->mandelbox.color.factor.y * (fabs(z.y) - fractal->mandelbox.foldingLimit)
									/ valMinusLim;
		}
		if (z.z != oldZ.z)
		{
			colorAdd += fractal->mandelbox.color.factor.z * (fabs(z.z) - fractal->mandelbox.foldingLimit)
									/ valMinusLim;
		}
		aux.color += colorAdd;
	}
}

/**
 * Box Fold VaryV1. Varies folding limit based on iteration conditions
 * This formula contains aux.color
 */
void TransfBoxFoldVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	double limit = fractal->mandelbox.foldingLimit;
	double tempVC = limit; // constant to be varied

	if (aux.i >= fractal->transformCommon.startIterations250
			&& aux.i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
	{
		int iterationRange =
			fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250;
		int currentIteration = (aux.i - fractal->transformCommon.startIterations250);
		tempVC += fractal->transformCommon.offset * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset);
	}

	limit = tempVC;
	double value = fractal->transformCommon.scale2 * limit;

	if (z.x > limit)
	{
		z.x = value - z.x;
	}
	else if (z.x < -limit)
	{
		z.x = -value - z.x;
	}
	if (z.y > limit)
	{
		z.y = value - z.y;
		;
	}
	else if (z.y < -limit)
	{
		z.y = -value - z.y;
	}
	double zLimit = limit * fractal->transformCommon.scale1;
	double zValue = value * fractal->transformCommon.scale1;
	if (z.z > zLimit)
	{
		z.z = zValue - z.z;
	}
	else if (z.z < -zLimit)
	{
		z.z = -zValue - z.z;
	}
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor.z;
	}
	aux.DE *= fractal->analyticDE.scale1;
}

/**
 * Box Fold XYZ, set different folding parameters for each axis
 * This formula contains aux.color
 */
void TransfBoxFoldXYZIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 Value4 = fractal->transformCommon.additionConstant222;
	if (fractal->transformCommon.functionEnabledFalse)
		Value4 = 2.0 * fractal->transformCommon.additionConstant111;

	if (z.x > fractal->transformCommon.additionConstant111.x)
	{
		z.x = Value4.x - z.x;
	}
	else if (z.x < -fractal->transformCommon.additionConstant111.x)
	{
		z.x = -Value4.x - z.x;
	}
	if (z.y > fractal->transformCommon.additionConstant111.y)
	{
		z.y = Value4.y - z.y;
	}
	else if (z.y < -fractal->transformCommon.additionConstant111.y)
	{
		z.y = -Value4.y - z.y;
	}
	if (z.z > fractal->transformCommon.additionConstant111.z)
	{
		z.z = Value4.z - z.z;
	}
	else if (z.z < -fractal->transformCommon.additionConstant111.z)
	{
		z.z = -Value4.z - z.z;
	}
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor.z;
	}
}

/**
 * Box Offset, set different offset for each axis, added symmetrically about the origin
 * will cause cuts along axis
 */
void TransfBoxOffsetIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 temp = z;
	z.x = sign(z.x) * fractal->transformCommon.additionConstant000.x + z.x;
	z.y = sign(z.y) * fractal->transformCommon.additionConstant000.y + z.y;
	z.z = sign(z.z) * fractal->transformCommon.additionConstant000.z + z.z;

	if (fractal->transformCommon.functionEnabledFalse)
	{
		double tempL = temp.Length();
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;
	}
}

/**
 * diagonal fold conditional
 */
void TransfDiagonalFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (!fractal->transformCommon.functionEnabledAxFalse)
	{
		if (fractal->transformCommon.functionEnabledAx)
		{
			if (z.x > z.y) swap(z.y, z.x);
		}
		if (fractal->transformCommon.functionEnabledAyFalse)
		{
			if (z.y > z.z) swap(z.z, z.y);
		}
		if (fractal->transformCommon.functionEnabledAzFalse)
		{
			if (z.z > z.x) swap(z.x, z.z);
		}
	}
	else
	{
		if (fractal->transformCommon.functionEnabledAx
				&& aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA)
		{
			if (z.x > z.y) swap(z.y, z.x);
		}
		if (fractal->transformCommon.functionEnabledAyFalse
				&& aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			if (z.y > z.z) swap(z.z, z.y);
		}
		if (fractal->transformCommon.functionEnabledAzFalse
				&& aux.i >= fractal->transformCommon.startIterationsC
				&& aux.i < fractal->transformCommon.stopIterationsC)
		{
			if (z.z > z.x) swap(z.x, z.z);
		}
	}

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}

/**
 * folding tetra3D from M3D (Luca GN 2011):
 * Code taken from the forums, KIFS original thread
 * side note - if you disable the 1st half, 2nd half will be
 * done even if you disable it... (to avoid a NOP transform)
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfFoldingTetra3dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

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
 * Gnarl deformation in 2D (from Mark Townsend), and Luca 2011
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfGnarlIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);
	CVector4 oldZ = z;
	CVector4 tempZ = z;
	double Scale = fractal->transformCommon.scale1;
	double stepX = fractal->transformCommon.scale3D000.x; //-0.1;
	double stepY = fractal->transformCommon.scale3D000.y;
	double stepZ = fractal->transformCommon.scale3D000.z;
	double Alpha = fractal->transformCommon.rotation.x; // 2.0;
	double Beta = fractal->transformCommon.rotation.y;	//-4.0;
	double Gamma = fractal->transformCommon.rotation.z; //-0.1;

	if (fractal->transformCommon.functionEnabledAx)
	{
		tempZ.x = z.x - stepX * sin(z.y + sin(Alpha * (z.y + sin(Beta * z.y))));
		tempZ.y = z.y - stepY * sin(z.x + sin(Alpha * (z.x + sin(Beta * z.x))));
		z = tempZ;
	}

	if (fractal->transformCommon.functionEnabledAxFalse)
	{
		double xx = z.x * z.x;
		tempZ.x = z.x + stepX * (sin(Gamma * (z.y - xx) + sin(Alpha * (z.y + Beta * cos(z.y)))));
		tempZ.y = z.y + stepY * (sin(Gamma * (z.y + xx) - Alpha * sin(xx + Beta * cos(xx))));
		z = tempZ;
	}

	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		double xx = z.x * z.x;
		double yy = z.y * z.y;
		tempZ.y = xx + stepY * (sin(yy * sqrt(fabs(z.y)) - Alpha * sin((yy + sin(Beta * yy)))));
		tempZ.x = yy - stepX * (sin(xx * sqrt(fabs(xx)) + sin(Alpha * (xx + sin(Beta * xx)))));
		z = tempZ;
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		tempZ.x = z.x - stepX * sin(z.z + sin(Alpha * (z.z + sin(Beta * z.z))));
		tempZ.y = z.y - stepY * sin(z.x + sin(Alpha * (z.x + sin(Beta * z.x))));
		tempZ.z = (z.z - stepZ * sin(z.y + sin(Alpha * (z.y + sin(Beta * z.y))))) * Scale;
		z = tempZ;
	}
	z.x *= Scale;
	z.y *= Scale;

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fabs(Scale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}

/**
 * iteration weight. Influence fractal based on the weight of
 * Z values after different iterations
 */
void TransfIterationWeightIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zA = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
	CVector4 zB = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

	z = (z * fractal->transformCommon.scale) + (zA * fractal->transformCommon.offset)
			+ (zB * fractal->transformCommon.offset0);
	aux.DE *= fractal->transformCommon.scale;
}

/**
 * Inverse cylindrical coordinates, very easy transform
 * Formula by Luca GN 2011
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfInvCylindricalIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double newZx = z.x;
	double newZy = z.y;

	if (fractal->transformCommon.functionEnabledFalse) newZx = z.x * cos(z.y);
	if (fractal->transformCommon.functionEnabledxFalse) newZy = z.x * sin(z.y);

	z = CVector4(z.x * cos(newZy * fractal->transformCommon.scale1),
				newZx * sin(z.y * fractal->transformCommon.scale1), z.z * fractal->transformCommon.scaleC1,
				z.w)
			* fractal->transformCommon.scaleA1;

	aux.DE = aux.DE * fabs(fractal->transformCommon.scaleA1) * fractal->transformCommon.scaleB1
					 + fractal->transformCommon.offset1;
}

/**
 * Linear Combine transform from Mandelbulb3D.
 * Can create multiple combination for the addition of Cpixel
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfLinCombineCXYZIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	CVector4 temp = c;
	CVector4 mulX = fractal->transformCommon.constantMultiplier100;
	CVector4 mulY = fractal->transformCommon.constantMultiplier010;
	CVector4 mulZ = fractal->transformCommon.constantMultiplier001;
	c.x = mulX.x * temp.x + mulX.y * temp.y + mulX.z * temp.z;
	c.y = mulY.x * temp.x + mulY.y * temp.y + mulY.z * temp.z;
	c.z = mulZ.x * temp.x + mulZ.y * temp.y + mulZ.z * temp.z;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
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
 * analytic aux.DE
 */
void TransfMengerFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
			if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
		}
		else
		{
			z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
		}
		if (fractal->analyticDE.enabled)
		{
			if (!fractal->analyticDE.enabledFalse)
				aux.DE *= fractal->transformCommon.scale3;
			else
				aux.DE = aux.DE * fractal->transformCommon.scale3 * fractal->analyticDE.scale1
								 + fractal->analyticDE.offset0;
		}
	}
}

/**
 * multiple angle
 */
void TransfMultipleAngleIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double th0 = asin(z.z / aux.r);
	double ph0 = atan2(z.y, z.x);
	double th = th0 * fractal->transformCommon.multiplication;
	double ph = ph0 * fractal->transformCommon.multiplication;
	double cth = cos(th);
	z = CVector4(cth * cos(ph), cth * sin(ph), sin(th), 0.0) * aux.r;
}

/**
 * Octo
 * analytic aux.DE
 */
void TransfOctoFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (z.x + z.y < 0.0) z = CVector4(-z.y, -z.x, z.z, z.w);

	if (z.x + z.z < 0.0) // z.xz = -z.zx;
		z = CVector4(-z.z, z.y, -z.x, z.w);

	if (z.x - z.y < 0.0) // z.xy = z.yx;
		z = CVector4(z.y, z.x, z.z, z.w);

	if (z.x - z.z < 0.0) // z.xz = z.zx;
		z = CVector4(z.z, z.y, z.x, z.w);

	z.x = fabs(z.x);
	z = z * fractal->transformCommon.scale2
			- fractal->transformCommon.offset100 * (fractal->transformCommon.scale2 - 1.0);

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE *= fractal->transformCommon.scale2;
		else
			aux.DE = aux.DE * fractal->transformCommon.scale2 * fractal->analyticDE.scale1
							 + fractal->analyticDE.offset0;
	}
}

/**
 * Parabolic Fold by mclarekin
 */
void TransfParabFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 temp = z;

	CVector4 slope2 = fractal->transformCommon.constantMultiplier111;
	CVector4 lengthAdd = fractal->transformCommon.additionConstant000;
	CVector4 factorP;

	factorP.x = -slope2.x / (lengthAdd.x * 2.0);
	factorP.y = -slope2.y / (lengthAdd.y * 2.0);
	factorP.z = -slope2.z / (lengthAdd.z * 2.0);

	if (temp.x > 0)
	{
		if (temp.x < lengthAdd.x)
		{
			z.x = z.x * z.x * factorP.x;
		}
		else
		{
			z.x = (z.x - lengthAdd.x / 2.0) * slope2.x;
		}
	}
	if (temp.y > 0)
	{
		if (temp.y < lengthAdd.y)
		{
			z.y = z.y * z.y * factorP.y;
		}
		else
		{
			z.y = (z.y - lengthAdd.y / 2.0) * slope2.y;
		}
	}
	if (temp.z > 0)
	{
		if (temp.z < lengthAdd.z)
		{
			z.z = z.z * z.z * factorP.z;
		}
		else
		{
			z.z = (z.z - lengthAdd.z / 2.0) * slope2.z;
		}
	}

	if (temp.x < 0)
	{
		if (temp.x > -lengthAdd.x)
		{
			z.x = z.x * z.x * -factorP.x;
		}
		else
		{
			z.x = (z.x + lengthAdd.x / 2.0) * slope2.x;
		}
	}
	if (temp.y < 0)
	{
		if (temp.y > -lengthAdd.y)
		{
			z.y = z.y * z.y * -factorP.y;
		}
		else
		{
			z.y = (z.y + lengthAdd.y / 2.0) * slope2.y;
		}
	}

	if (temp.z < 0)
	{

		if (temp.z > -lengthAdd.z)
		{
			z.z = z.z * z.z * -factorP.z;
		}
		else
		{
			z.z = (z.z + lengthAdd.z / 2.0) * slope2.z;
		}
	}

	if (fractal->transformCommon.functionEnabledxFalse)
	{
		z = (z - temp) * fractal->transformCommon.scale3D111;
	}
	if (fractal->transformCommon.functionEnabledyFalse)
	{
		z = (z - temp) * temp * fractal->transformCommon.scale3D111;

		z.x = sign(temp.x) * z.x;
		z.y = sign(temp.y) * z.y;
		z.z = sign(temp.z) * z.z;
	}
	if (fractal->transformCommon.functionEnabledzFalse)
	{
		z = (z - temp) * temp * fractal->transformCommon.scale3D111;
	}

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / temp.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}

/**
 * platonic solid - from DarkBeam's code
 * @reference
 * http://www.fractalforums.com/3d-fractal-generation/platonic-dimensions/msg36528/#msg36528
 */
void TransfPlatonicSolidIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double platonicR = 0.0;
	double rho = 0.0;
	if (!fractal->transformCommon.functionEnabledFalse)
	{
		rho = sqrt(aux.r); // the radius
		double theta =
			cos(fractal->platonicSolid.frequency * z.x) * sin(fractal->platonicSolid.frequency * z.y)
			+ cos(fractal->platonicSolid.frequency * z.y) * sin(fractal->platonicSolid.frequency * z.z)
			+ cos(fractal->platonicSolid.frequency * z.z) * sin(fractal->platonicSolid.frequency * z.x);
		platonicR = theta * fractal->platonicSolid.amplitude + rho * fractal->platonicSolid.rhoMul;
		z *= platonicR;
	}
	else
	{
		double rho1 = 0.0;
		double rho2 = 0.0;
		double rho3 = 0.0;
		if (fractal->transformCommon.functionEnabledx) rho1 = sqrt(aux.r);
		if (fractal->transformCommon.functionEnabledyFalse) rho2 = aux.r;
		if (fractal->transformCommon.functionEnabledzFalse) rho3 = aux.r * aux.r;

		if (fractal->transformCommon.functionEnabledAxFalse)
			rho = rho1 + (rho2 - rho1) * fractal->transformCommon.scale1;
		else if (fractal->transformCommon.functionEnabledAyFalse)
			rho = rho2 + (rho3 - rho2) * fractal->transformCommon.scale1;
		else
			rho = rho1 + rho2 + rho3;

		double theta =
			cos(fractal->platonicSolid.frequency * z.x) * sin(fractal->platonicSolid.frequency * z.y)
			+ cos(fractal->platonicSolid.frequency * z.y) * sin(fractal->platonicSolid.frequency * z.z)
			+ cos(fractal->platonicSolid.frequency * z.z) * sin(fractal->platonicSolid.frequency * z.x);
		// theta is pos or neg && < 3.0

		if (fractal->transformCommon.functionEnabledAzFalse) theta = fabs(theta);

		platonicR = theta * fractal->platonicSolid.amplitude + rho * fractal->platonicSolid.rhoMul;

		z *= platonicR; //  can be neg
	}

	if (fractal->analyticDE.enabled)
	{
		aux.DE = aux.DE * fabs(platonicR) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
}

/**
 * poly fold atan
 * @reference
 * https://fractalforums.org/fragmentarium/17/polyfoldsym-pre-transform/2684
 */
void TransfPolyFoldAtanIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	// pre abs
	if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);

	if (fractal->transformCommon.functionEnabledCx)
	{
		if (fractal->transformCommon.functionEnabledAxFalse && z.y < 0.0) z.x = -z.x;
		int poly = fractal->transformCommon.int8X;
		double psi = fabs(fmod(atan(z.y / z.x) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledCyFalse)
	{
		if (fractal->transformCommon.functionEnabledAyFalse && z.z < 0.0) z.y = -z.y;
		int poly = fractal->transformCommon.int8Y;
		double psi = fabs(fmod(atan(z.z / z.y) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
		double len = sqrt(z.y * z.y + z.z * z.z);
		z.y = cos(psi) * len;
		z.z = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledCzFalse)
	{
		if (fractal->transformCommon.functionEnabledAzFalse && z.x < 0.0) z.z = -z.z;
		int poly = fractal->transformCommon.int8Z;
		double psi = fabs(fmod(atan(z.x / z.z) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
		double len = sqrt(z.z * z.z + z.x * z.x);
		z.z = cos(psi) * len;
		z.x = sin(psi) * len;
	}

	// addition constant
	z += fractal->transformCommon.additionConstant000;

	// rotation
	if (fractal->transformCommon.rotation2EnabledFalse)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	// DE tweaks
	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}

/**
 * poly fold atan2
 * @reference
 * https://fractalforums.org/fragmentarium/17/polyfoldsym-pre-transform/2684
 */
void TransfPolyFoldAtan2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	// pre abs
	if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);

	if (fractal->transformCommon.functionEnabledCx)
	{
		if (fractal->transformCommon.functionEnabledAxFalse && z.y < 0.0) z.x = -z.x;
		int poly = fractal->transformCommon.int8X;
		double psi = fabs(fmod(atan2(z.y, z.x) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledCyFalse)
	{
		if (fractal->transformCommon.functionEnabledAyFalse && z.z < 0.0) z.y = -z.y;
		int poly = fractal->transformCommon.int8Y;
		double psi = fabs(fmod(atan2(z.z, z.y) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
		double len = sqrt(z.y * z.y + z.z * z.z);
		z.y = cos(psi) * len;
		z.z = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledCzFalse)
	{
		if (fractal->transformCommon.functionEnabledAzFalse && z.x < 0.0) z.z = -z.z;
		int poly = fractal->transformCommon.int8Z;
		double psi = fabs(fmod(atan2(z.x, z.z) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
		double len = sqrt(z.z * z.z + z.x * z.x);
		z.z = cos(psi) * len;
		z.x = sin(psi) * len;
	}

	// addition constant
	z += fractal->transformCommon.additionConstant000;

	// rotation
	if (fractal->transformCommon.rotation2EnabledFalse)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	// DE tweaks
	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}

/**
 * poly fold sym DarkBeam's version
 * @reference
 * DarkBeam (luca) http://www.fractalforums.com/mandelbulber/
 * _polyfold_sym-and-polyfoldsymifs-in-mandelbulber-2/msg98162/#msg98162
 */
void TransfPolyFoldSymXYIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	// pre abs
	if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);

	int order = fractal->transformCommon.int6;
	double div2PI = (double)order / M_PI_2x;

	bool cy = false;
	int sector;
	if (!fractal->transformCommon.functionEnabledFalse)
		sector = (int)(-div2PI * atan(z.x / z.y));
	else
		sector = (int)(-div2PI * atan2(z.x, z.y));

	if (sector & 1) cy = true; // parity   if (sector & 1) is a "bit check", true = odd
	double angle = (double)(sector / div2PI);
	// z.xy = rotate(z.xy,angle); // sin
	double tempZx = z.x;
	z.x = z.x * cos(angle) - z.y * sin(angle);
	z.y = tempZx * sin(angle) + z.y * cos(angle);
	if (cy) z.y = -z.y;
	// if ((order&1) && (sector == 0)) z.y = fabs(z.y); // more continuous?

	// addition constant
	z.x += fractal->transformCommon.offset0;
	z.y += fractal->transformCommon.offsetA0;

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}

/**
 * poly fold sym multi DarkBeam's version
 * @reference
 * DarkBeam (luca) http://www.fractalforums.com/mandelbulber/
 * _polyfold_sym-and-polyfoldsymifs-in-mandelbulber-2/msg98162/#msg98162
 */
void TransfPolyFoldSymMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	// pre abs
	if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);

	int order = fractal->transformCommon.int6;
	double div2PI = (double)order / M_PI_2x;
	double temp = 0.0;
	int sector;
	if (fractal->transformCommon.functionEnabledCx)
	{
		bool cy = false;
		if (!fractal->transformCommon.functionEnabledAxFalse)
			sector = (int)(-div2PI * atan(z.x / z.y));
		else
			sector = (int)(-div2PI * atan2(z.x, z.y));
		if (sector & 1) cy = true;
		double angle = (double)(sector / div2PI);
		temp = z.x;
		z.x = z.x * cos(angle) - z.y * sin(angle);
		z.y = temp * sin(angle) + z.y * cos(angle);
		if (cy == true) z.y = -z.y;
		/*if (fractal->transformCommon.functionEnabledFalse)
		{
			if ((order&1) && (sector == 0)) z.y = fabs(z.y); // more continuous?
			else if (cy == true) z.y = -z.y;
		}
		else if (cy == true) z.y = -z.y;*/
	}
	if (fractal->transformCommon.functionEnabledCyFalse)
	{
		bool cz = false;
		if (!fractal->transformCommon.functionEnabledAyFalse)
			sector = (int)(-div2PI * atan(z.y / z.z));
		else
			sector = (int)(-div2PI * atan2(z.y, z.z));
		if (sector & 1) cz = true;
		double angle = (double)(sector / div2PI);
		temp = z.y;
		z.y = z.y * cos(angle) - z.z * sin(angle);
		z.z = temp * sin(angle) + z.z * cos(angle);
		if (cz == true) z.z = -z.z;
	}
	if (fractal->transformCommon.functionEnabledCzFalse)
	{
		bool cx = false;
		if (!fractal->transformCommon.functionEnabledAzFalse)
			sector = (int)(-div2PI * atan(z.z / z.x));
		else
			sector = (int)(-div2PI * atan2(z.z, z.x));
		if (sector & 1) cx = true;
		double angle = (double)(sector / div2PI);
		temp = z.z;
		z.z = z.z * cos(angle) - z.x * sin(angle);
		z.x = temp * sin(angle) + z.x * cos(angle);
		if (cx == true) z.x = -z.x;
	}

	z += fractal->transformCommon.additionConstant000;

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}

/**
 * z(n+1) = z(n) * abs(z(n)) ^ p-1
 * analytic aux.r-dz
 */
void TransfRPowerIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;
	z *= rp;
}

/**
 * z = (Az^2 + Bz + C ) / D; ((VectA +ScaleA * z) * fn(z) + scaleB * z + VectC ) * ScaleD
 */
void TransfPwr2PolynomialIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 partA = z;
	if (fractal->transformCommon.functionEnabledFalse) // fabs
		partA = fabs(z);
	if (fractal->transformCommon.functionEnabledxFalse) // pwr3 or z * fabs(z^2)
		partA *= z;
	partA = partA * fractal->transformCommon.scale2 + fractal->transformCommon.constantMultiplier111;

	CVector4 fnZ1 = z;
	if (fractal->transformCommon.functionEnabledBxFalse) // cos(z*Pi)
	{
		double scalePi = M_PI * fractal->transformCommon.scaleC1;
		fnZ1.x = cos(z.x * scalePi);
		fnZ1.y = cos(z.y * scalePi);
		fnZ1.z = cos(z.z * scalePi);
	}
	if (fractal->transformCommon.functionEnabledyFalse) // pi rotation
		fnZ1 = fnZ1.RotateAroundVectorByAngle(fractal->transformCommon.constantMultiplier111.GetXYZ(),
			M_PI * fractal->transformCommon.scale0);				// * cPI ;
	if (fractal->transformCommon.functionEnabledzFalse) // box offset
	{
		fnZ1.x = fnZ1.x + sign(fnZ1.x) * fractal->transformCommon.additionConstant000.x;
		fnZ1.y = fnZ1.y + sign(fnZ1.y) * fractal->transformCommon.additionConstant000.y;
		fnZ1.z = fnZ1.z + sign(fnZ1.z) * fractal->transformCommon.additionConstant000.z;
	}

	if (fractal->transformCommon.functionEnabledAxFalse) // fabs fnZ1
		fnZ1 = fabs(fnZ1);

	CVector4 partB = z;
	if (fractal->transformCommon.functionEnabledAzFalse) partB = fabs(z);
	partB *= fractal->transformCommon.scale4;

	CVector4 constantMult = CVector4(fractal->transformCommon.constantMultiplierB111.x,
		fractal->transformCommon.constantMultiplierB111.y,
		fractal->transformCommon.constantMultiplierB111.z, 0.0);

	z = constantMult + partB - partA * fnZ1;
	z *= fractal->transformCommon.scale025;

	aux.DE = aux.DE * 4.0 * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}

/**
 * Quaternion Fold Transform
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */
void TransfQuaternionFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	// quat fold
	z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, z.w);

	// quat scale and DE fudge

	if (fractal->transformCommon.functionEnabledFalse)
	{
		z *= fractal->transformCommon.constantMultiplier122;
		aux.DE *= 2.0;
	}

	// offset
	z += fractal->transformCommon.additionConstant000;

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}

	// tweaking DE
	aux.DE = aux.DE * aux.r * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * Reciprocal3  based on DarkBeam's code from M3D,
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfReciprocal3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempZ = z;

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
	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}

/**
 * rotation folding plane
 */
void TransfRotationFoldingPlaneIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zRot;
	// cast vector to array pointer for address taking of components in opencl
	double *zRotP = (double *)&zRot;
	const double *colP = (const double *)&fractal->mandelbox.color.factor;
	for (int dim = 0; dim < 3; dim++)
	{
		// handle each dimension x, y and z sequentially in pointer var dim
		double *rotDim = (dim == 0) ? &zRotP[0] : ((dim == 1) ? &zRotP[1] : &zRotP[2]);
		const double *colorFactor = (dim == 0) ? &colP[0] : ((dim == 1) ? &colP[1] : &colP[2]);

		zRot = fractal->mandelbox.rot[0][dim].RotateVector(z);
		if (*rotDim > fractal->mandelbox.foldingLimit)
		{
			*rotDim = fractal->mandelbox.foldingValue - *rotDim;
			z = fractal->mandelbox.rotinv[0][dim].RotateVector(zRot);
			aux.color += *colorFactor;
		}
		else
		{
			zRot = fractal->mandelbox.rot[1][dim].RotateVector(z);
			if (*rotDim < -fractal->mandelbox.foldingLimit)
			{
				*rotDim = -fractal->mandelbox.foldingValue - *rotDim;
				z = fractal->mandelbox.rotinv[1][dim].RotateVector(zRot);
				aux.color += *colorFactor;
			}
		}
	}
}

/**
 * rotation
 */
void TransfRotationIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}

/**
 * rotate about vec3
 */
void TransfRotateAboutVec3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double useAngle = fractal->transformCommon.angle0;

	if (fractal->transformCommon.functionEnabledEFalse)
	{
		if (aux.i > fractal->transformCommon.startIterationsRV)
		{
			useAngle *= (1.0
										- 1.0
												/ (1.0
														+ (aux.i - fractal->transformCommon.startIterationsRV)
																/ fractal->transformCommon.offset0))
									* fractal->transformCommon.scale1;
		}

		CVector4 v = fractal->transformCommon.vec111;
		v = v / v.Length(); // normalise
		double c = cos(useAngle * M_PI_180);
		double s = sin(useAngle * M_PI_180);
		CVector4 rotVec = CVector4(0.0, 0.0, 0.0, z.w);

		rotVec.x = z.x * (c + (1.0 - c) * v.x * v.x) + z.y * ((1.0 - c) * v.x * v.y + s * v.z)
							 + z.z * ((1.0 - c) * v.x * v.z - s * v.y);
		rotVec.y = z.x * ((1.0 - c) * v.x * v.y - s * v.z) + z.y * (c + (1.0 - c) * v.y * v.y)
							 + z.z * ((1.0 - c) * v.y * v.z + s * v.x);
		rotVec.z = z.x * ((1.0 - c) * v.x * v.z + s * v.y) + z.y * ((1.0 - c) * v.y * v.z - s * v.x)
							 + z.z * (c + (1.0 - c) * v.z * v.z);
		z = rotVec;
	}
	else
	{
		CVector4 v = fractal->transformCommon.vec111;
		v = v / v.Length(); // normalise
		double c = cos(useAngle * M_PI_180);
		double s = sin(useAngle * M_PI_180);
		CVector4 rotVec = CVector4(0.0, 0.0, 0.0, z.w);

		rotVec.x = z.x * (c + (1.0 - c) * v.x * v.x) + z.y * ((1.0 - c) * v.x * v.y + s * v.z)
							 + z.z * ((1.0 - c) * v.x * v.z - s * v.y);
		rotVec.y = z.x * ((1.0 - c) * v.x * v.y - s * v.z) + z.y * (c + (1.0 - c) * v.y * v.y)
							 + z.z * ((1.0 - c) * v.y * v.z + s * v.x);
		rotVec.z = z.x * ((1.0 - c) * v.x * v.z + s * v.y) + z.y * ((1.0 - c) * v.y * v.z - s * v.x)
							 + z.z * (c + (1.0 - c) * v.z * v.z);
		z = rotVec;

		/*	CVector4 v = fractal->transformCommon.vec111;
			v = v / v.Length(); // normalise
			double c = cos(useAngle * M_PI_180);
			double s = sin(useAngle * M_PI_180);
			CVector4 rotVec = CVector4(0.0, 0.0, 0.0, z.w);

			CMatrix44 rotM = CMatrix44(c + (1.0 - c) * v.x * v.x, (1.0 - c) * v.x * v.y - s * v.z, (1.0 -
			c) * v.x * v.z + s * v.y, 0.0, (1.0 - c) * v.x * v.y + s * v.z, c + (1.0 - c) * v.y * v.y,
			(1.0
			- c) * v.y * v.z - s * v.x, 0.0, (1.0 - c) * v.x * v.z - s * v.y, (1.0 - c) * v.y * v.z + s *
			v.x, c + (1.0 - c) * v.z * v.z, 0.0, 0.0, 0.0, 0.0, 0.0
				);
			z *= rotM;*/
	}

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * rotate angles with iteration start/stop controls
 */
void TransfRotationIterControlsIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);
	CVector4 tp;
	if (fractal->transformCommon.rotation44a.y != 0
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		tp = z;
		double alpha = fractal->transformCommon.rotation44a.y * -M_PI_180;
		z.y = tp.y * cos(alpha) + tp.z * sin(alpha);
		z.z = tp.y * -sin(alpha) + tp.z * cos(alpha);
	}

	if (fractal->transformCommon.rotation44a.z != 0
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		tp = z;
		double beta = fractal->transformCommon.rotation44a.z * M_PI_180;
		z.x = tp.x * cos(beta) + tp.z * sin(beta);
		z.z = tp.x * -sin(beta) + tp.z * cos(beta);
	}

	if (fractal->transformCommon.rotation44a.x != 0
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		tp = z;
		double gamma = fractal->transformCommon.rotation44a.x * -M_PI_180;
		z.x = tp.x * cos(gamma) + tp.y * sin(gamma);
		z.y = tp.x * -sin(gamma) + tp.y * cos(gamma);
	}
}

/**
 * rotation variation v1. Rotation angles vary linearly between iteration parameters.
 * After the stop iteration, alpha = alpha + variable alpha.
 */
void TransfRotationVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempVC = CVector4(fractal->transformCommon.rotation, 0.0); // constant to be varied
	int iterationRange =
		fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250;

	if (aux.i >= fractal->transformCommon.startIterations250 && iterationRange != 0)
	{
		if (aux.i < fractal->transformCommon.stopIterations)
		{
			int currentIteration = (aux.i - fractal->transformCommon.startIterations250);
			double dCurrentIteration = (double)currentIteration;
			double dIterationRange = (double)iterationRange;
			tempVC += fractal->transformCommon.offset000 * dCurrentIteration / dIterationRange;
		}
		else
		{
			tempVC += fractal->transformCommon.offset000;
		}
	}

	tempVC *= M_PI_180;

	z = z.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), tempVC.x);
	z = z.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), tempVC.y);
	z = z.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), tempVC.z);
}

/**
 * rotatedAbs & Rotated Folding transform from M3D
 * - Rotate by the given angles
 *- fold
 *- RotateBack by the given angles
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

void TransfRotationFoldingIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

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
 * Rpow3 from M3D.
 * Does a power of 3 on the current length of the  vector.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfRpow3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double sqrRout = z.Dot(z) * fractal->transformCommon.scale;

	z *= sqrRout;
	aux.DE = aux.DE * fabs(sqrRout) + fractal->analyticDE.offset1;
}

/**
 * scale - simple scale
 */
void TransfScaleIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z *= fractal->transformCommon.scale;

	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
}

/**
 * scale vary V2.12.- based on DarkBeam's maths
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfScaleVaryV212Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			aux.actualScaleA = fabs(aux.actualScaleA);
		}
		aux.actualScaleA =
			fractal->transformCommon.scaleMain2 + fractal->transformCommon.scaleVary0 * aux.actualScaleA;
		double temp = aux.actualScaleA;
		if (fractal->transformCommon.functionEnabledByFalse) // limits
		{
			if (aux.actualScaleA < fractal->transformCommon.offset0)
				temp = fractal->transformCommon.offset0;
			if (aux.actualScaleA > fractal->transformCommon.offset4)
				temp = fractal->transformCommon.offset4;
		}

		z *= temp;
		aux.DE = aux.DE * fabs(temp) + 1.0;
	}

	else if (aux.i < fractal->transformCommon.startIterations)
	{
		z *= fractal->transformCommon.scaleMain2;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scaleMain2 + 1.0);
	}
	else
	{
		if (fractal->transformCommon.functionEnabledBzFalse)
		{
			aux.actualScaleA = fractal->transformCommon.scaleMain2;
		}
		z *= aux.actualScaleA;
		aux.DE = aux.DE * fabs(aux.actualScaleA + 1.0);
	}
}

/**
 * scale vary multi- based on DarkBeam's maths
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfScaleVaryMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			aux.actualScaleA = fabs(aux.actualScaleA);
		}
		double base = fractal->transformCommon.scaleMain2;
		double vary = (aux.actualScaleA - fractal->transformCommon.offset1);

		if (fractal->transformCommon.functionEnabled)
		{
			aux.actualScaleA = base + fractal->transformCommon.scaleVary0 * vary;
		}
		else if (fractal->transformCommon.functionEnabledCxFalse)
		{
			base = aux.actualScaleA;
			aux.actualScaleA = base + fractal->transformCommon.scale0 * vary;
		}

		else if (fractal->transformCommon.functionEnabledCyFalse)
		{
			double base2 = base
										 + fractal->transformCommon.scaleVary0
												 * (aux.actualScaleA - fractal->transformCommon.offset1);

			aux.actualScaleA =
				base2
				+ fractal->transformCommon.scale0 * (aux.actualScaleA - fractal->transformCommon.offset1);
		}

		else if (fractal->transformCommon.functionEnabledCzFalse)
		{

			base = aux.actualScaleA;
			double base2 =
				base
				+ fractal->transformCommon.scale0 * (aux.actualScaleA - fractal->transformCommon.offset1);

			aux.actualScaleA = base2
												 + fractal->transformCommon.scaleVary0
														 * (aux.actualScaleA - fractal->transformCommon.offset1);
		}
		double temp = aux.actualScaleA;
		if (fractal->transformCommon.functionEnabledByFalse) // limits
		{
			if (aux.actualScaleA < fractal->transformCommon.offset0)
				temp = fractal->transformCommon.offset0;
			if (aux.actualScaleA > fractal->transformCommon.offset4)
				temp = fractal->transformCommon.offset4;
		}
		z *= temp;
		aux.DE = aux.DE * fabs(temp) + 1.0;
	}
	else if (aux.i < fractal->transformCommon.startIterations)
	{
		z *= fractal->transformCommon.scaleMain2;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scaleMain2) + 1.0;
	}
	else
	{
		if (fractal->transformCommon.functionEnabledBzFalse)
		{
			aux.actualScaleA = fractal->transformCommon.scaleMain2;
		}
		z *= aux.actualScaleA;
		aux.DE = aux.DE * fabs(aux.actualScaleA) + 1.0;
	}
}

/**
 * scale variation v1. Scale varies based on iteration parameters.
 */
void TransfScaleVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double tempVC = fractal->transformCommon.scale; // constant to be varied

	if (aux.i >= fractal->transformCommon.startIterations250
			&& aux.i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					 != 0))
	{
		int iterationRange =
			fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250;
		int currentIteration = (aux.i - fractal->transformCommon.startIterations250);
		tempVC += fractal->transformCommon.offset0 * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset0);
	}
	z *= tempVC;
	aux.DE = aux.DE * fabs(tempVC) + 1.0;
}
/**
 *  Scale varies smoothly based on iteration parameters.
 */
void TransfScaleVaryVCLIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
		if (aux.i < fractal->Cpara.iterA)
		{
			para = temp0 + (aux.i * grade1);
		}
		if (aux.i < fractal->Cpara.iterB && aux.i >= fractal->Cpara.iterA)
		{
			para = tempA + (aux.i - fractal->Cpara.iterA) * grade2;
		}
		if (aux.i >= fractal->Cpara.iterB)
		{
			para = tempB + (aux.i - fractal->Cpara.iterB) * grade3;
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
				if (aux.i < 2 * fractal->Cpara.iterA)
				{
					paraIt = tempA - fabs(tempA - aux.i);
					paraAdd = paraIt * paraIt * curve1;
				}
				if (aux.i >= 2 * fractal->Cpara.iterA && aux.i < fractal->Cpara.iterB + tempL)
				{
					paraIt = tempB - fabs(tempB * aux.i);
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
		paraAddP0 = fractal->Cpara.parabOffset + (aux.i * fractal->Cpara.parabSlope)
								+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
	}
	para += paraAddP0;

	z *= para; // using the parameter
	aux.DE = aux.DE * fabs(para) + 1.0;
}

/**
 * scale 3d. Scale each axis individually
 */
void TransfScale3dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z *= fractal->transformCommon.scale3D111;
	aux.DE = aux.DE * z.Length() / aux.r + 1.0;
}

/**
 * sin or cos z
 */
void TransfSinOrCosIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 trigZ = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 scaleZ = z * fractal->transformCommon.constantMultiplierC111;

	if (fractal->transformCommon.functionEnabledAx)
	{
		if (!fractal->transformCommon.functionEnabledAxFalse)
			trigZ.x = sin(scaleZ.x);
		else
			trigZ.x = cos(scaleZ.x); // scale =0, cos = 1
	}
	if (fractal->transformCommon.functionEnabledAy)
	{
		if (!fractal->transformCommon.functionEnabledAyFalse)
			trigZ.y = sin(scaleZ.y);
		else
			trigZ.y = cos(scaleZ.y);
	}
	if (fractal->transformCommon.functionEnabledAz)
	{
		if (!fractal->transformCommon.functionEnabledAzFalse)
			trigZ.z = sin(scaleZ.z);
		else
			trigZ.z = cos(scaleZ.z);
	}

	z = trigZ * fractal->transformCommon.scale;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		z.x = z.x * fractal->transformCommon.scale / (fabs(oldZ.x) + 1.0);
		z.y = z.y * fractal->transformCommon.scale / (fabs(oldZ.y) + 1.0);
		z.z = z.z * fractal->transformCommon.scale / (fabs(oldZ.z) + 1.0);
		// aux.DE = aux.DE * z.Length() / oldZ.Length();
	}
	//   if z == oldZ    z = oldZ * fractal->transformCommon.scale;
	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
}

/**
 * sin and cos
 */
void TransfSinAndCosIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// Q_UNUSED(aux);
	CVector4 oldZ = z;
	CVector4 sinZ = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 cosZ = CVector4(0.0, 0.0, 0.0, 0.0);

	CVector4 scaleZ = z * fractal->transformCommon.constantMultiplierC111;

	if (fractal->transformCommon.functionEnabledAx) sinZ.x = sin(scaleZ.x); // scale =0, sin = 0
	if (fractal->transformCommon.functionEnabledAy) sinZ.y = sin(scaleZ.y);
	if (fractal->transformCommon.functionEnabledAz) sinZ.z = sin(scaleZ.z);

	if (fractal->transformCommon.functionEnabledAxFalse) cosZ.x = cos(scaleZ.x); // scale =0, cos = 1
	if (fractal->transformCommon.functionEnabledAyFalse) cosZ.y = cos(scaleZ.y);
	if (fractal->transformCommon.functionEnabledAzFalse) cosZ.z = cos(scaleZ.z);

	switch (fractal->combo6.combo6)
	{
		case multi_combo6_type1:
		default: z = (sinZ + cosZ); break;
		case multi_combo6_type2: z = sinZ * cosZ; break;
		case multi_combo6_type3: z = oldZ + (sinZ + cosZ); break;
		case multi_combo6_type4: z = oldZ + (sinZ * cosZ); break;
		case multi_combo6_type5: z = oldZ * (sinZ + cosZ); break;
		case multi_combo6_type6: z = oldZ * (sinZ * cosZ); break;
	}

	// post scale
	if (!fractal->transformCommon.functionEnabledFalse)
		z *= fractal->transformCommon.scale;
	else
	{
		z.x = z.x * fractal->transformCommon.scale / (fabs(oldZ.x) + 1.0);
		z.y = z.y * fractal->transformCommon.scale / (fabs(oldZ.y) + 1.0);
		z.z = z.z * fractal->transformCommon.scale / (fabs(oldZ.z) + 1.0);
		// aux.DE = aux.DE * z.Length() / oldZ.Length();
	}

	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
}

/**
 * max sin - cos
 */
void TransfSinAndCosMaxIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 maxZ = z;
	CVector4 sinZ = z;
	CVector4 cosZ = z;
	CVector4 sinCosZ = z;

	if (fractal->transformCommon.functionEnabledAx)
	{
		sinZ.x = sin(oldZ.x * M_PI_2x / fractal->transformCommon.constantMultiplierA111.x)
						 * fractal->transformCommon.scaleA1; // freq
		cosZ.x = cos(oldZ.x * M_PI_2x / fractal->transformCommon.constantMultiplierB111.x)
						 * fractal->transformCommon.scaleB1;
		sinCosZ.x = sinZ.x * cosZ.x * fractal->transformCommon.scaleC1;
		maxZ.x = max(max(sinZ.x, cosZ.x), sinCosZ.x);

		if (fractal->transformCommon.functionEnabledFalse) maxZ.x /= (fabs(oldZ.x) + 1.0);
	}

	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		sinZ.y = sin(oldZ.y * M_PI_2x / fractal->transformCommon.constantMultiplierA111.y)
						 * fractal->transformCommon.scaleA1; // freq
		cosZ.y = cos(oldZ.y * M_PI_2x / fractal->transformCommon.constantMultiplierB111.y)
						 * fractal->transformCommon.scaleB1;
		sinCosZ.y = sinZ.y * cosZ.y * fractal->transformCommon.scaleC1;
		maxZ.y = max(max(sinZ.y, cosZ.y), sinCosZ.y);

		if (fractal->transformCommon.functionEnabledFalse) maxZ.y /= (fabs(oldZ.y) + 1.0);
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		sinZ.z = sin(oldZ.z * M_PI_2x / fractal->transformCommon.constantMultiplierA111.z)
						 * fractal->transformCommon.scaleA1; // freq
		cosZ.z = cos(oldZ.z * M_PI_2x / fractal->transformCommon.constantMultiplierB111.z)
						 * fractal->transformCommon.scaleB1;
		sinCosZ.z = sinZ.z * cosZ.z * fractal->transformCommon.scaleC1;
		maxZ.z = max(max(sinZ.z, cosZ.z), sinCosZ.z);

		if (fractal->transformCommon.functionEnabledFalse) maxZ.z /= (fabs(oldZ.z) + 1.0);
	}

	// post scale
	maxZ *= fractal->transformCommon.scale;
	aux.DE *= fabs(fractal->transformCommon.scale);

	if (fractal->transformCommon.functionEnabledMFalse)
	{
		switch (fractal->combo4.combo4)
		{
			case multi_combo4_type1:
			default: maxZ += oldZ; break;
			case multi_combo4_type2: maxZ *= oldZ; break;
			case multi_combo4_type3: maxZ += fabs(oldZ); break;
			case multi_combo4_type4: maxZ *= fabs(oldZ); break;
		}
	}

	z = maxZ;

	// analytic tweaks
	if (fractal->analyticDE.enabledFalse) // temp
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
}

/**
 * spherical invert (modes updated v2.15)
 * from M3D. Formula by Luca GN 2011, updated May 2012.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfSphericalInvIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;

	if (!fractal->transformCommon.functionEnabledzFalse)
	{
		double r2Inv = 1.0 / z.Dot(z);
		z *= r2Inv;
		aux.DE *= r2Inv;
	}
	else // conditional
	{
		double rr = z.Dot(z);
		z += fractal->mandelbox.offset;
		if (rr < fractal->mandelbox.foldingSphericalFixed)
		{
			double mode = 0.0;
			if (fractal->transformCommon.functionEnabledFalse) // Mode 1 minR0
			{
				if (rr < fractal->transformCommon.minR0) mode = fractal->transformCommon.minR0;
			}
			if (fractal->transformCommon.functionEnabledxFalse) // Mode 2
			{
				if (rr < fractal->transformCommon.minR0) mode = 2.0 * fractal->transformCommon.minR0 - rr;
			}
			mode = 1.0 / mode;
			z *= mode;
			aux.DE *= fabs(mode);
		}
	}
	z -= fractal->mandelbox.offset + fractal->transformCommon.additionConstant000;
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}

/**
 * spherical invert ( v2.17)
 * from M3D. Formula by Luca GN 2011, updated May 2012.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfSphericalInvV2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double rr = 0.0;
	// unconditional mode
	if (fractal->transformCommon.functionEnabledCz)
	{
		if (aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD1)
		{
			z += fractal->transformCommon.offset000;
			rr = z.Dot(z);
			z *= fractal->transformCommon.maxR2d1 / rr;
			z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
			// double r = sqrt(rr);
			aux.DE = (fractal->transformCommon.maxR2d1) / rr;
		}
	}

	// conditional modes
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		rr = z.Dot(z);
		double mode = rr;
		z += fractal->mandelbox.offset;

		if (rr < fractal->mandelbox.foldingSphericalFixed)
		{
			mode = 0.0;
			if (fractal->transformCommon.functionEnabledFalse) // Mode 1 minR0
			{
				if (rr < fractal->transformCommon.minR0) mode = fractal->transformCommon.minR0;
			}
			if (fractal->transformCommon.functionEnabledxFalse) // Mode 2
			{
				if (rr < fractal->transformCommon.minR0) mode = 2.0 * fractal->transformCommon.minR0 - rr;
			}
			mode = 1.0 / mode;
			z *= mode;
			aux.DE *= fabs(mode);
		}
		z -= fractal->mandelbox.offset;
	}

	// other modes
	if (fractal->transformCommon.functionEnabledCyFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		rr = z.Dot(z);
		double mode = rr;
		if (rr < fractal->transformCommon.scaleE1) // < maxRR
		{
			double lengthAB = fractal->transformCommon.scaleE1 - fractal->transformCommon.offsetC0;

			if (fractal->transformCommon.functionEnabledyFalse) // Mode 3a, linear addition 0.0 at Max,
			{
				if (rr < fractal->transformCommon.offsetC0)
					mode += rr * (fractal->transformCommon.offset0 / fractal->transformCommon.offsetC0);
				else
					mode +=
						(fractal->transformCommon.scaleE1 - rr) * fractal->transformCommon.offset0 / lengthAB;
			}

			if (fractal->transformCommon.functionEnabledzFalse) // Mode 3b
			{

				if (rr > fractal->transformCommon.offsetC0)
					mode += fractal->transformCommon.offsetB0 * (fractal->transformCommon.scaleE1 - rr);
				else
					mode += fractal->transformCommon.offsetA0 * (fractal->transformCommon.offsetC0 - rr)
									+ fractal->transformCommon.offsetB0 * lengthAB;
			}

			if (fractal->transformCommon.functionEnabledwFalse) // Mode 3c, basic parabolic curve
			{

				double halfLen = fractal->transformCommon.scaleE1 / 2.0;
				double slope = 2.0 / fractal->transformCommon.scaleE1;
				double factor = slope / fractal->transformCommon.scaleE1;
				double parab = 0.0;

				if (rr < halfLen)
				{
					parab = rr * rr * factor * fractal->transformCommon.scaleG1;
					mode += rr * slope * fractal->transformCommon.scaleF1 - parab;
				}
				else
				{
					double temp = fractal->transformCommon.scaleE1 - rr;
					parab = temp * temp * factor * fractal->transformCommon.scaleG1;
					mode += temp * slope * fractal->transformCommon.scaleF1 - parab;
				}
			}

			/*if (fractal->transformCommon.functionEnabledwFalse) // Mode 3d
			{
				mode = rr + fractal->transformCommon.offset0 * (fractal->mandelbox.foldingSphericalFixed -
			rr);
				if (rr < fractal->transformCommon.minR0)
					mode -= rr * (fractal->transformCommon.offset0
									* (fractal->mandelbox.foldingSphericalFixed - fractal->transformCommon.minR0))
							/ fractal->transformCommon.minR0;
			}*/
			mode = 1.0 / mode;
			z *= mode;
			aux.DE *= fabs(mode);
		}
	}

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}

/**
 * inverted sphere z & c- A transform from M3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 * updated v2.12
 */
void TransfSphericalInvCIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double rSqrL;
	CVector4 tempC;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		tempC = aux.c;
		tempC *= fractal->transformCommon.constantMultiplier111;
		rSqrL = tempC.Dot(tempC);
		// if (rSqrL < 1e-21) rSqrL = 1e-21;
		rSqrL = 1.0 / rSqrL;
		tempC *= rSqrL;
		aux.c = tempC;
	}
	else
	{
		tempC = aux.const_c;
		tempC *= fractal->transformCommon.constantMultiplier111;
		rSqrL = tempC.Dot(tempC);
		// if (rSqrL < 1e-21) rSqrL = 1e-21;
		rSqrL = 1.0 / rSqrL;
		tempC *= rSqrL;
	}

	if (fractal->transformCommon.functionEnabledAwFalse)
	{
		rSqrL = z.Dot(z);
		// if (rSqrL < 1e-21) rSqrL = 1e-21;
		rSqrL = 1.0 / rSqrL;
		z *= rSqrL;
		aux.DE *= rSqrL;
		z += tempC;
	}
	z += tempC;
}

/**
 * spherical fold MBox
 * This formula contains aux.color
 */
void TransfSphericalFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double r2 = z.Dot(z);
	z += fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;

	// if (r2 < 1e-21) r2 = 1e-21;
	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
	z -= fractal->mandelbox.offset;
	// Analytic DE tweak
	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset0;
}

/**
 * spherical fold ABox
 * from Fractal Forums and M3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 * This formula contains aux.color
 */
void TransfSphericalFoldAboxIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double r2 = z.Dot(z);
	z += fractal->mandelbox.offset;
	double sqrtMinR = fractal->transformCommon.sqtR;
	if (r2 < sqrtMinR)
	{
		z *= fractal->transformCommon.mboxFactor1;
		aux.DE *= fractal->transformCommon.mboxFactor1;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (r2 < 1.0)
	{
		z *= 1.0 / r2;
		aux.DE *= 1.0 / r2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
	z -= fractal->mandelbox.offset;
}

/**
 * spherical fold varyV1 MBox type
 * This formula contains aux.color
 */
void TransfSphericalFoldVaryV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double r2 = z.Dot(z);
	double tempVCf = fractal->mandelbox.foldingSphericalFixed; // constant to be varied
	double tempVCm = fractal->mandelbox.foldingSphericalMin;

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		int iterationRange =
			fractal->transformCommon.stopIterationsA - fractal->transformCommon.startIterationsA;
		int currentIteration = (aux.i - fractal->transformCommon.startIterationsA);
		tempVCf += fractal->transformCommon.offset * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterationsA)
	{
		tempVCf = (tempVCf + fractal->transformCommon.offset);
	}
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{

		int iterationRange =
			fractal->transformCommon.stopIterationsB - fractal->transformCommon.startIterationsB;
		int currentIteration = (aux.i - fractal->transformCommon.startIterationsB);
		tempVCm += fractal->transformCommon.offset0 * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterationsB)
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
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (r2 < tempVCf)
	{
		double tglad_factor2 = tempVCf / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
	z -= fractal->mandelbox.offset;
}

/**
 * spherical fold Parab, coded by mclarekin
 * @reference
 * http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/smooth-spherical-fold/msg101051/#new
 * This formula contains aux.color and aux.actualScaleA
 */
void TransfSphericalFoldParabIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double m = 1.0;
	double rr;
	// spherical fold
	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		rr = z.Dot(z);
		double tempM = rr;
		m = fractal->transformCommon.maxMinR2factor;
		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < fractal->transformCommon.minR2p25)
		{
			if (fractal->transformCommon.functionEnabledAyFalse && m > tempM) m = tempM + (tempM - m);
			z *= m;
			aux.DE = aux.DE * m;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
				aux.color += fractal->mandelbox.color.factorSp1;
			}
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			m = fractal->transformCommon.maxR2d1 / rr;
			if (fractal->transformCommon.functionEnabledAyFalse && m > tempM) m = tempM + (tempM - m);
			z *= m;
			aux.DE = aux.DE * m;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
				aux.color += fractal->mandelbox.color.factorSp2;
			}
		}
	}
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		rr = z.Dot(z);
		z += fractal->mandelbox.offset;
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
		double maxScale = fractal->transformCommon.scale4;
		double midPoint = (maxScale - 1.0) * 0.5;
		rr += fractal->transformCommon.offset0;
		double maxR2 = fractal->transformCommon.scale1;
		double halfMax = maxR2 * 0.5;
		double factor = midPoint / (halfMax * halfMax);

		double tempM = rr + fractal->transformCommon.offsetA0;
		if (rr < halfMax)
		{
			m = maxScale - (rr * rr) * factor;
			if (fractal->transformCommon.functionEnabledAxFalse && m > tempM) m = tempM + (tempM - m);
			z *= m;
			aux.DE = aux.DE * m;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
				aux.color += fractal->mandelbox.color.factorSp1;
			}
		}
		else if (rr < maxR2)
		{
			m = 1.0 + (maxR2 - rr) * (maxR2 - rr) * factor;
			if (fractal->transformCommon.functionEnabledAxFalse && m > tempM) m = tempM + (tempM - m);
			z *= m;
			aux.DE = aux.DE * m;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
				aux.color += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->mandelbox.offset;
		}
	}

	double useScale = fractal->transformCommon.scaleA1;
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		useScale += aux.actualScaleA;
		z *= useScale;

		aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

		// update actualScale for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
		if (fractal->transformCommon.functionEnabledMFalse)
			aux.actualScaleA = -vary;
		else
			aux.actualScaleA = aux.actualScaleA - vary;
	}
	else
	{
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
}

/**
 * spherical fold Cuboid
 * This has a box shape MinR2 condition
 * This formula contains aux.color
 */
void TransfSphericalFoldCuboidIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 temp3;
	CVector4 R2;
	double minR2 = fractal->transformCommon.minR2p25;
	CVector4 limitMinR2 = fractal->transformCommon.scaleP222;
	double m = fractal->transformCommon.scale;

	double rr = z.Dot(z);
	z += fractal->transformCommon.offset000;

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (fractal->transformCommon.functionEnabledAxFalse)
			temp3 = z * z;
		else
			temp3 = fabs(z);

		if (temp3.x < limitMinR2.x && temp3.y < limitMinR2.y && temp3.z < limitMinR2.z)
		{ // if inside cuboid
			R2.x = limitMinR2.x / temp3.x;
			R2.y = limitMinR2.y / temp3.y;
			R2.z = limitMinR2.z / temp3.z;
			double First = min(R2.x, min(R2.y, R2.z));
			minR2 = rr * First;

			if (fractal->transformCommon.functionEnabled && minR2 > fractal->transformCommon.maxR2d1)
			{ // stop overlapping potential
				minR2 = fractal->transformCommon.maxR2d1;
			}

			m *= fractal->transformCommon.maxR2d1 / minR2;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
				aux.color += fractal->mandelbox.color.factorSp1;
			}
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			m *= fractal->transformCommon.maxR2d1 / rr;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
				aux.color += fractal->mandelbox.color.factorSp2;
			}
		}
	}
	else if (rr < minR2)
	{
		m *= fractal->transformCommon.maxR2d1 / minR2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		m *= fractal->transformCommon.maxR2d1 / rr;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
	z -= fractal->transformCommon.offset000;

	// scale
	z *= m;
	aux.DE = aux.DE * fabs(m) + 1.0;
}

/**
 * spherical fold CHS Cylinder Half Size. DarkBeam s code from M3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 * This formula contains aux.color
 */
void TransfSphericalFoldCHSIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double tempZ = fabs(z.z) - fractal->transformCommon.offset05;
	double rr;
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		rr = z.x * z.x + z.y * z.y;
		if (tempZ > 0.0) rr = rr + (tempZ * tempZ * fractal->transformCommon.scale1);
	}
	else
	{
		rr = z.Dot(z);
	}

	z += fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;

	// Spherical Fold
	if (rr < fractal->transformCommon.minR2p25)
	{
		z *= fractal->transformCommon.maxMinR2factor;
		aux.DE *= fractal->transformCommon.maxMinR2factor;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
	z -= fractal->mandelbox.offset;
}

/**
 * spherical fold varyVCL MBox type
 * This formula contains aux.color
 */
void TransfSphericalFoldVaryVCLIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
		if (aux.i < fractal->Cpara.iterA)
		{
			para = temp0 + (aux.i * grade1);
		}
		if (aux.i < fractal->Cpara.iterB && aux.i >= fractal->Cpara.iterA)
		{
			para = tempA + (aux.i - fractal->Cpara.iterA) * grade2;
		}
		if (aux.i >= fractal->Cpara.iterB)
		{
			para = tempB + (aux.i - fractal->Cpara.iterB) * grade3;
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
				if (aux.i < 2 * fractal->Cpara.iterA)
				{
					paraIt = tempA - fabs(tempA - aux.i);
					paraAdd = paraIt * paraIt * curve1;
				}
				if (aux.i >= 2 * fractal->Cpara.iterA && aux.i < fractal->Cpara.iterB + tempL)
				{
					paraIt = tempB - fabs(tempB * aux.i);
					paraAdd = paraIt * paraIt * curve2;
				}
				para += paraAdd;
			}
		}
	}
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope)
								+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
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
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->transformCommon.maxR2d1 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
	z -= fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale);
}

/**
 * spherical fold XYZ Bias
 * deforms the MinR2 condition
 * This formula contains aux.color
 */
void TransfSphericalFoldXYZBiasIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 xyzBias;
	double minR2 = fractal->transformCommon.minR2p25;
	double MaxR2 = fractal->transformCommon.maxR2d1;
	double m = fractal->transformCommon.scale;

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		xyzBias = fabs(aux.c) * fractal->transformCommon.constantMultiplier000;
		minR2 = minR2 + (xyzBias.x + xyzBias.y + xyzBias.z);
	}

	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		CVector4 cSquared = aux.c * aux.c;
		xyzBias = cSquared * fractal->transformCommon.scale3D000;
		minR2 = minR2 + (xyzBias.x + xyzBias.y + xyzBias.z);
	}

	if (fractal->transformCommon.functionEnabled && minR2 > MaxR2)
	{
		minR2 = MaxR2; // stop overlapping potential
	}

	double rr = z.Dot(z);
	z += fractal->transformCommon.offset000;

	if (rr < minR2)
	{
		m *= fractal->transformCommon.maxR2d1 / minR2;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		m *= fractal->transformCommon.maxR2d1 / rr;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
	z -= fractal->transformCommon.offset000;

	z *= m;
	aux.DE = aux.DE * fabs(m) + 1.0;
}

/**
 * spherical radial offset.
 * This formula contains analytic aux.DE and aux.r-dz
 */
void TransfSphericalOffsetIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// if (-z.Length() > -1e-21) -z.Length() = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / -z.Length();
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fractal->transformCommon.scale + fractal->analyticDE.offset1;
}

/**
 * spherical radial offset Curvilinear.
 * This formula contains analytic aux.DE and aux.r-dz
 */
void TransfSphericalOffsetVCLIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double para = fractal->Cpara.para00;
	double paraAdd = 0.0;
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
			if (aux.i < fractal->Cpara.iterA)
			{
				para = temp0 + (aux.i * grade1);
			}
			if (aux.i < fractal->Cpara.iterB && aux.i >= fractal->Cpara.iterA)
			{
				para = tempA + (aux.i - fractal->Cpara.iterA) * grade2;
			}
			if (aux.i >= fractal->Cpara.iterB)
			{
				para = tempB + (aux.i - fractal->Cpara.iterB) * grade3;
			}

			// Curvi part on "true"
			if (fractal->Cpara.enabledCurves)
			{
				double paraIt;
				if (lengthAB > 2.0 * fractal->Cpara.iterA) // stop  error, todo fix.
				{
					double curve1 = (grade2 - grade1) / (4.0 * fractal->Cpara.iterA);
					double tempL = lengthAB - fractal->Cpara.iterA;
					double curve2 = (grade3 - grade2) / (4.0 * tempL);
					if (aux.i < 2 * fractal->Cpara.iterA)
					{
						paraIt = tempA - fabs(tempA - aux.i);
						paraAdd = paraIt * paraIt * curve1;
					}
					if (aux.i >= 2 * fractal->Cpara.iterA && aux.i < fractal->Cpara.iterB + tempL)
					{
						paraIt = tempB - fabs(tempB * aux.i);
						paraAdd = paraIt * paraIt * curve2;
					}
				}
				para += paraAdd;
			}
		}
	}
	// Parabolic
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope)
								+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
		para += paraAddP0;
	}
	// para offset
	para += fractal->transformCommon.offset0;

	double div = 0.0;
	// dot mode
	if (fractal->transformCommon.functionEnabledFalse)
	{
		div = z.Dot(z);
	}
	else
	{
		div = z.Length();
	}

	// using the parameter
	z *= 1.0 + para / -div;

	// post scale
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fractal->transformCommon.scale + 1.0;

	// DE tweak
	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * spherical pwr fold
 * This formula contains aux.color and analytic aux.DE
 */
void TransfSphericalPwrFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// if (z.x > -1e-21 && z.x < 1e-21) z.x = (z.x > 0) ? 1e-21 : -1e-21;
	// if (z.y > -1e-21 && z.y < 1e-21) z.y = (z.y > 0) ? 1e-21 : -1e-21;
	// if (z.z > -1e-21 && z.z < 1e-21) z.z = (z.z > 0) ? 1e-21 : -1e-21;
	double rr = pow(pow(z.x, fractal->transformCommon.pwr4) + pow(z.y, fractal->transformCommon.pwr4)
										+ pow(z.z, fractal->transformCommon.pwr4),
		fractal->transformCommon.pwr05);

	// if (rr < 1e-21 && rr > -1e-21)
	//	rr = (rr > 0) ? 1e-21 : -1e-21;
	if (rr < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (rr < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / rr;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
}

/**
 * Surf Box Fold
 * allows different fold types for each axis.
 * amazing surface if one axis has no folds enabled.
 * made from various folds found in the posts at fractalforums.com
 * This formula contains aux.color, analytic aux.DE and aux.r-dz
 */
void TransfSurfBoxFoldIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->surfBox.enabledX1)
	{
		z.x =
			fabs(z.x + fractal->surfBox.offset1A111.x) - fabs(z.x - fractal->surfBox.offset1A111.x) - z.x;
	}
	if (fractal->surfBox.enabledX2False)
	{ // z = fold - fabs( fabs(z) - fold), Tglad fold
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
	{ // z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)// DarkBeam's Abox mod1 fold
		z.z = fractal->surfBox.offset1B222.z
					- fabs(fabs(z.z + fractal->surfBox.offset3A111.z) - fractal->surfBox.offset1B222.z)
					- fractal->surfBox.offset3A111.z;
	}
	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0; // tweak
}

/**
 * Surf Box Fold v2
 * based on DarkBeam's code "Mixed fold by Luca 2016".
 * @reference
 * http:// http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/
 * httpwww-shaperich-comproshred-elite-review
 * This formula contains aux.color,
 */
void TransfSurfBoxFoldV2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double colorAdd = 0.0;
	CVector4 oldZ = z;
	CVector4 foldMod = fractal->transformCommon.offset222;
	CVector4 fold = fractal->transformCommon.additionConstant111;
	CVector4 sg = z;
	sg.x = sign(z.x);
	sg.y = sign(z.y);
	sg.z = sign(z.z);

	CVector4 folder = z;
	CVector4 Tglad = z;

	Tglad.x = fabs(z.x + fold.x) - fabs(z.x - fold.x) - z.x;
	Tglad.y = fabs(z.y + fold.y) - fabs(z.y - fold.y) - z.y;

	folder.x = sg.x * (z.x - Tglad.x);
	folder.y = sg.y * (z.y - Tglad.y);
	folder.x = fabs(folder.x);
	folder.y = fabs(folder.y);
	folder.x = min(folder.x, foldMod.x);
	folder.y = min(folder.y, foldMod.y);
	z.x -= sg.x * folder.x;
	z.y -= sg.y * folder.y;

	if (fractal->transformCommon.functionEnabled)
	{
		Tglad.z = fabs(z.z + fold.z) - fabs(z.z - fold.z) - z.z;
		folder.z = sg.z * (z.z - Tglad.z);
		folder.z = fabs(folder.z);
		folder.z = min(folder.z, foldMod.z);
		z.z -= sg.z * folder.z;
	}

	// analyic DE tweak
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		CVector4 zCol = z;
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd += fractal->mandelbox.color.factor.x
										* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
			if (zCol.y != oldZ.y)
				colorAdd += fractal->mandelbox.color.factor.y
										* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
			if (zCol.z != oldZ.z)
				colorAdd += fractal->mandelbox.color.factor.z
										* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);
		}
		else
		{
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
		}
		aux.color += colorAdd;
	}
}

/**
 * TransfSurfFoldMultiIteration
 * Based on amazing_surf_mod1 from Mandelbulb3D
 * This formula contains aux.color, analytic aux.DE and aux.r-dz
 */
void TransfSurfFoldMultiIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0; // tweak
}

/**
 * z vector - axis swap
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfZvectorAxisSwapIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	switch (fractal->mandelbulbMulti.orderOfXYZ)
	{
		case multi_OrderOfXYZ_xyz:
		default: z = CVector4(z.x, z.y, z.z, z.w); break;
		case multi_OrderOfXYZ_xzy: z = CVector4(z.x, z.z, z.y, z.w); break;
		case multi_OrderOfXYZ_yxz: z = CVector4(z.y, z.x, z.z, z.w); break;
		case multi_OrderOfXYZ_yzx: z = CVector4(z.y, z.z, z.x, z.w); break;
		case multi_OrderOfXYZ_zxy: z = CVector4(z.z, z.x, z.y, z.w); break;
		case multi_OrderOfXYZ_zyx: z = CVector4(z.z, z.y, z.x, z.w); break;
	}
	if (fractal->transformCommon.functionEnabledxFalse) z.x = -z.x;
	if (fractal->transformCommon.functionEnabledyFalse) z.y = -z.y;
	if (fractal->transformCommon.functionEnabledzFalse) z.z = -z.z;

	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		double xTemp = SQRT_1_2 * (z.x - z.y);
		z.y = SQRT_1_2 * (z.y + z.x);
		z.x = xTemp;
	}
}

//-------------------------------- 4D ----------------------------------------------

/**
 * Formula based on Mandelbox (ABox). Extended to 4 dimensions
 * This formula contains aux.color and aux.actualScale
 */
void Abox4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;

	aux.actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	// parabolic = paraOffset + iter *slope + (iter *iter *scale)
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{
		double parabScale = 0.0;
		if (fractal->Cpara.parabScale != 0.0)
			parabScale = aux.i * aux.i * 0.001 * fractal->Cpara.parabScale;
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope) + (parabScale);
		z.w += paraAddP0;
	}

	CVector4 oldZ = z;
	z.x = fabs(z.x + fractal->transformCommon.offset1111.x)
				- fabs(z.x - fractal->transformCommon.offset1111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.offset1111.y)
				- fabs(z.y - fractal->transformCommon.offset1111.y) - z.y;
	z.z = fabs(z.z + fractal->transformCommon.offset1111.z)
				- fabs(z.z - fractal->transformCommon.offset1111.z) - z.z;
	z.w = fabs(z.w + fractal->transformCommon.offset1111.w)
				- fabs(z.w - fractal->transformCommon.offset1111.w) - z.w;
	zCol = z;

	double rr = z.Dot(z);
	rrCol = rr;
	if (fractal->mandelboxVary4D.rPower != 1.0) rr = pow(rr, fractal->mandelboxVary4D.rPower);

	z += fractal->transformCommon.offset0000;
	if (rr < fractal->transformCommon.minR2p25)
	{
		z *= fractal->transformCommon.maxMinR2factor;
		aux.DE *= fractal->transformCommon.maxMinR2factor;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		z *= fractal->transformCommon.maxR2d1 / rr;
		aux.DE *= fractal->transformCommon.maxR2d1 / rr;
	}
	z -= fractal->transformCommon.offset0000;

	z *= aux.actualScale;
	aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z.y = tp.y * cos(beta) + tp.z * sin(beta);
			z.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z.x = tp.x * cos(delta) + tp.w * sin(delta);
			z.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
	z += fractal->transformCommon.additionConstant0000;

	if (fractal->foldColor.auxColorEnabled)
	{
		if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor4D.x;
		if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor4D.y;
		if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor4D.z;
		if (zCol.w != oldZ.w) colorAdd += fractal->mandelbox.color.factor4D.w;

		if (rrCol < fractal->transformCommon.minR2p25)
			colorAdd += fractal->mandelbox.color.factorSp1;
		else if (rrCol < fractal->transformCommon.maxR2d1)
			colorAdd += fractal->mandelbox.color.factorSp2;

		aux.color += colorAdd;
	}
}

/**
 * Bristorbrot formula 4D
 * @reference https://code.google.com/archive/p/fractaldimension/
 * by Doug Bristor
 */
void Bristorbrot4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r;
	double newx = z.x * z.x - z.y * z.y - z.z * z.z - z.w * z.w;
	double newy = z.y * (2.0 * z.x - z.z - z.w);
	double newz = z.z * (2.0 * z.x + z.y - z.w);
	double neww = z.w * (2.0 * z.x + z.y + z.z);
	z.x = newx;
	z.y = newy;
	z.z = newz;
	z.w = neww;

	z += fractal->transformCommon.additionConstant0000;
}

/**
 * boxFold4dBulb.
 * This formula contains aux.color and aux.actualScaleA
 */
void BoxFold4dBulbPow2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;

	// parabolic.w = paraOffset + iter *slope + (iter *iter *scale)
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{
		double parabScale = 0.0;
		if (fractal->Cpara.parabScale != 0.0)
			parabScale = aux.i * aux.i * 0.001 * fractal->Cpara.parabScale;
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope) + (parabScale);
		z.w += paraAddP0;
	}

	// sinusoidal w
	double sinAdd = 0.0;
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		sinAdd = sin((aux.i + fractal->transformCommon.offset0) / fractal->transformCommon.scaleA1)
						 * fractal->transformCommon.scaleC1;
		z.w += sinAdd;
	}

	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		oldZ = z;
		z.x = fabs(z.x + fractal->transformCommon.offset1111.x)
					- fabs(z.x - fractal->transformCommon.offset1111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.offset1111.y)
					- fabs(z.y - fractal->transformCommon.offset1111.y) - z.y;
		z.z = fabs(z.z + fractal->transformCommon.offset1111.z)
					- fabs(z.z - fractal->transformCommon.offset1111.z) - z.z;
		z.w = fabs(z.w + fractal->transformCommon.offset1111.w)
					- fabs(z.w - fractal->transformCommon.offset1111.w) - z.w;
		zCol = z;
	}

	// sph fold
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);

		// r power
		if (fractal->mandelboxVary4D.rPower != 1.0) rr = pow(rr, fractal->mandelboxVary4D.rPower);
		rrCol = rr;
		// spherical fold
		z += fractal->transformCommon.offset0000;
		if (rr < fractal->transformCommon.minR2p25)
		{
			z *= fractal->transformCommon.maxMinR2factor;
			aux.DE *= fractal->transformCommon.maxMinR2factor;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			z *= fractal->transformCommon.maxR2d1 / rr;
			aux.DE *= fractal->transformCommon.maxR2d1 / rr;
		}

		z -= fractal->transformCommon.offset0000;
	}

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{

		useScale = aux.actualScaleA + fractal->transformCommon.scale;

		z *= useScale;
		aux.DE = aux.DE * fabs(useScale);

		if (fractal->transformCommon.functionEnabledSFalse
				&& aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			// update actualScale for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z.y = tp.y * cos(beta) + tp.z * sin(beta);
			z.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z.x = tp.x * cos(delta) + tp.w * sin(delta);
			z.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
	// offset
	z += fractal->transformCommon.additionConstant0000;

	// bulb pow2
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.r * aux.DE * 16.0 * fractal->analyticDE.scale1
							 * sqrt(fractal->foldingIntPow.zFactor * fractal->foldingIntPow.zFactor + 2.0
											+ fractal->analyticDE.offset2)
							 / SQRT_3
						 + fractal->analyticDE.offset1;

		z = z * 2.0;
		double x2 = z.x * z.x;
		double y2 = z.y * z.y;
		double z2 = z.z * z.z;
		double temp = 1.0 - z2 / (x2 + y2);
		CVector4 zTemp;
		zTemp.x = (x2 - y2) * temp;
		zTemp.y = 2.0 * z.x * z.y * temp;
		zTemp.z = -2.0 * z.z * sqrt(x2 + y2);
		zTemp.w = z.w;
		z = zTemp;
		z.z *= fractal->foldingIntPow.zFactor;
	}

	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd +=
					fractal->mandelbox.color.factor.x * (fabs(zCol.x) - fractal->mandelbox.color.factor4D.x);
			if (zCol.y != oldZ.y)
				colorAdd +=
					fractal->mandelbox.color.factor.y * (fabs(zCol.y) - fractal->mandelbox.color.factor4D.y);
			if (zCol.z != oldZ.z)
				colorAdd +=
					fractal->mandelbox.color.factor.z * (fabs(zCol.z) - fractal->mandelbox.color.factor4D.z);
			if (zCol.w != oldZ.w)
				colorAdd +=
					fractal->mandelbox.color.factor.z * (fabs(zCol.w) - fractal->mandelbox.color.factor4D.w);
			if (rrCol < fractal->transformCommon.maxR2d1)
			{
				if (rrCol < fractal->transformCommon.minR2p25)
					colorAdd +=
						fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
						+ fractal->mandelbox.color.factorSp2
								* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
				else
					colorAdd +=
						fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			}
		}
		else
		{
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor4D.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor4D.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor4D.z;
			if (zCol.w != oldZ.w) colorAdd += fractal->mandelbox.color.factor4D.w;
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1;
			else if (rrCol < fractal->transformCommon.maxR2d1)
				colorAdd += fractal->mandelbox.color.factorSp2;
		}
		aux.color += colorAdd;
	}
}

/**
 * Formula based on Mandelbox (ABox). Extended to 4 dimensions and with variable scale parameter.
 * This formula contains aux.color and aux.actualScale
 */
void MandelboxVaryScale4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope)
								+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
		z.w += paraAddP0;
	}

	CVector4 oldZ = z;
	z.x = fabs(z.x + fractal->mandelboxVary4D.fold) - fabs(z.x - fractal->mandelboxVary4D.fold) - z.x;
	z.y = fabs(z.y + fractal->mandelboxVary4D.fold) - fabs(z.y - fractal->mandelboxVary4D.fold) - z.y;
	z.z = fabs(z.z + fractal->mandelboxVary4D.fold) - fabs(z.z - fractal->mandelboxVary4D.fold) - z.z;
	z.w = fabs(z.w + fractal->mandelboxVary4D.fold) - fabs(z.w - fractal->mandelboxVary4D.fold) - z.w;

	if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x;
	if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y;
	if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z;
	if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w;

	double rr = pow(z.x * z.x + z.y * z.y + z.z * z.z + z.w * z.w, fractal->mandelboxVary4D.rPower);
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
	z *= m;
	aux.DE = aux.DE * fabs(m) + 1.0;

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z.y = tp.y * cos(beta) + tp.z * sin(beta);
			z.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z.x = tp.x * cos(delta) + tp.w * sin(delta);
			z.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
}

/**
 * from Syntopia & DarkBeam's Menger4 code from M3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void Menger4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z += fractal->transformCommon.additionConstant0000; // offset
	}

	z = fabs(z);
	if (z.x - z.y < 0.0) swap(z.y, z.x);
	if (z.x - z.z < 0.0) swap(z.z, z.x);
	if (z.y - z.z < 0.0) swap(z.z, z.y);
	if (z.x - z.w < 0.0) swap(z.w, z.x);
	if (z.y - z.w < 0.0) swap(z.w, z.y);
	if (z.z - z.w < 0.0) swap(z.w, z.z);

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z.y = tp.y * cos(beta) + tp.z * sin(beta);
			z.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z.x = tp.x * cos(delta) + tp.w * sin(delta);
			z.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}

	double scaleM = fractal->transformCommon.scale3;
	CVector4 offsetM = fractal->transformCommon.additionConstant111d5;
	z.x = scaleM * z.x - offsetM.x;
	z.y = scaleM * z.y - offsetM.y;
	z.w = scaleM * z.w - offsetM.w;
	z.z -= 0.5 * offsetM.z / scaleM;
	z.z = -fabs(-z.z);
	z.z += 0.5 * offsetM.z / scaleM;
	z.z *= scaleM;
	aux.DE *= scaleM;

	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double r2 = z.Dot(z);
		// if (r2 < 1e-21 && r2 > -1e-21) r2 = (r2 > 0) ? 1e-21 : -1e-21;

		if (r2 < fractal->transformCommon.minR2p25)
		{
			z *= fractal->transformCommon.maxMinR2factor;
			aux.DE *= fractal->transformCommon.maxMinR2factor;
			aux.color += fractal->mandelbox.color.factorSp1;
		}
		else if (r2 < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector4 zA4 = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
		CVector4 zB4 = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

		z = (z * fractal->transformCommon.scale) + (zA4 * fractal->transformCommon.offset)
				+ (zB4 * fractal->transformCommon.offset0);
		aux.DE *= fractal->transformCommon.scale1;
	}

	aux.DE *= fractal->analyticDE.scale1;
}

/**
 * Menger4D MOD1   from Syntopia & DarkBeam's Menger4 code from M3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void Menger4dMod1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope)
								+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
		z.w += paraAddP0;
	}

	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z += fractal->transformCommon.additionConstant0000; // offset
	}

	z = fabs(z);
	if (z.x - z.y < 0.0) swap(z.y, z.x);
	if (z.x - z.z < 0.0) swap(z.z, z.x);
	if (z.y - z.z < 0.0) swap(z.z, z.y);
	if (z.x - z.w < 0.0) swap(z.w, z.x);
	if (z.y - z.w < 0.0) swap(z.w, z.y);
	if (z.z - z.w < 0.0) swap(z.w, z.z);

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z.y = tp.y * cos(beta) + tp.z * sin(beta);
			z.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z.x = tp.x * cos(delta) + tp.w * sin(delta);
			z.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
	double scaleM = fractal->transformCommon.scale3;
	CVector4 offsetM = fractal->transformCommon.additionConstant111d5;
	z.x = scaleM * z.x - offsetM.x;
	z.y = scaleM * z.y - offsetM.y;
	z.w = scaleM * z.w - offsetM.w;
	if (fractal->transformCommon.functionEnabledz
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		z.z -= 0.5 * offsetM.z / scaleM;
		z.z = -fabs(-z.z);
		z.z += 0.5 * offsetM.z / scaleM;
	}
	else
	{
		z.w = scaleM * z.w - offsetM.w;
	}
	z.z *= scaleM;
	aux.DE *= scaleM;

	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double r2 = 0.;
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			r2 = z.x * z.x + z.y * z.y;
		}
		if (fractal->transformCommon.functionEnabledByFalse)
		{
			r2 = z.x * z.x + z.y * z.y + z.z * z.z;
		}
		if (fractal->transformCommon.functionEnabledBz)
		//{	r2 = z.Dot(z) ;}
		{
			r2 = z.x * z.x + z.y * z.y + z.z * z.z + z.w * z.w;
		}
		// if (r2 < 1e-21 && r2 > -1e-21) r2 = (r2 > 0) ? 1e-21 : -1e-21;

		if (r2 < fractal->transformCommon.minR2p25)
		{
			z *= fractal->transformCommon.maxMinR2factor;
			aux.DE *= fractal->transformCommon.maxMinR2factor;
			aux.color += fractal->mandelbox.color.factorSp1;
		}
		else if (r2 < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector4 zA4 = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
		CVector4 zB4 = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

		z = (z * fractal->transformCommon.scale) + (zA4 * fractal->transformCommon.offset)
				+ (zB4 * fractal->transformCommon.offset0);
		aux.DE *= fractal->transformCommon.scale1;
	}

	aux.DE *= fractal->analyticDE.scale1;
}

/**
 * DarkBeam's MixPinski4 from M3D
 * A strange but intriguing fractal, that mixes Sierpinski and Menger folds.
 * The amazing thing is that in 3D it does not work so well! LUCA GN 2011
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void MixPinski4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double temp;
		if (z.x + z.y < 0.0)
		{
			temp = z.x;
			z.x = -z.y;
			z.y = -temp;
		}

		if (z.x + z.z < 0.0)
		{
			temp = z.x;
			z.x = -z.z;
			z.z = -temp;
		}

		if (z.y + z.z < 0.0)
		{
			temp = z.z;
			z.z = -z.y;
			z.y = -temp;
		}

		if (z.x + z.w < 0.0)
		{
			temp = z.x;
			z.x = -z.w;
			z.w = -temp;
		}

		if (z.y + z.w < 0.0)
		{
			temp = z.y;
			z.y = -z.w;
			z.w = -temp;
		}

		if (z.z + z.w < 0.0)
		{
			temp = z.z;
			z.z = -z.w;
			z.w = -temp;
		}
		z *= fractal->transformCommon.scale1;
		aux.DE *= fabs(fractal->transformCommon.scale1);
	}

	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z += fractal->transformCommon.additionConstant0000; // offset
	}
	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z.y = tp.y * cos(beta) + tp.z * sin(beta);
			z.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z.x = tp.x * cos(delta) + tp.w * sin(delta);
			z.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
	if (aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		double scaleM = fractal->transformCommon.scale2;
		CVector4 offsetM = fractal->transformCommon.additionConstant111d5;
		z.x = scaleM * z.x - offsetM.x * (scaleM - 1.0);
		z.y = scaleM * z.y - offsetM.y * (scaleM - 1.0);
		z.w = scaleM * z.w - offsetM.w * (scaleM - 1.0);
		z.z -= 0.5 * offsetM.z * (scaleM - 1.0) / scaleM;
		z.z = -fabs(-z.z);
		z.z += 0.5 * offsetM.z * (scaleM - 1.0) / scaleM;
		z.z = scaleM * z.z;
		aux.DE *= fabs(scaleM) * fractal->analyticDE.scale1;
	}
}

/**
 * Sierpinski4D.from Syntopia & DarkBeam's code
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void Sierpinski4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp;
	if (z.x + z.y < 0.0)
	{
		temp = z.x;
		z.x = -z.y;
		z.y = -temp;
	}

	if (z.x + z.z < 0.0)
	{
		temp = z.x;
		z.x = -z.z;
		z.z = -temp;
	}

	if (z.y + z.z < 0.0)
	{
		temp = z.z;
		z.z = -z.y;
		z.y = -temp;
	}

	if (z.x + z.w < 0.0)
	{
		temp = z.x;
		z.x = -z.w;
		z.w = -temp;
	}

	if (z.y + z.w < 0.0)
	{
		temp = z.y;
		z.y = -z.w;
		z.w = -temp;
	}

	if (z.z + z.w < 0.0)
	{
		temp = z.z;
		z.z = -z.w;
		z.w = -temp;
	}

	z *= fractal->transformCommon.scaleA2;

	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z -= fractal->transformCommon.offset1111; // neg offset
	}

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z.y = tp.y * cos(beta) + tp.z * sin(beta);
			z.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z.x = tp.x * cos(delta) + tp.w * sin(delta);
			z.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}

	if (!fractal->analyticDE.enabledFalse) // testing
		aux.DE *= fabs(fractal->transformCommon.scaleA2);
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scaleA2) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset0;
}

/**
 * Quaternion4D
 */
void Quaternion4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z - z.w * z.w, z.x * z.y, z.x * z.z, z.w);
	z *= fractal->transformCommon.constantMultiplier1220;
	z += fractal->transformCommon.additionConstant0000;

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z.y = tp.y * cos(beta) + tp.z * sin(beta);
			z.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z.x = tp.x * cos(delta) + tp.w * sin(delta);
			z.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
}

/**
 * QuaternionCubic4d - Quaternion fractal with extended controls
 * @reference DarkBeam's cubic quat implementation
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void QuaternionCubic4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);
	if (fractal->transformCommon.functionEnabledwFalse) z.w = fabs(z.w);

	CVector4 mul = z * fractal->transformCommon.offset1111;
	CVector4 oldZ = z;
	CVector4 auxC = aux.c;
	z.x = mul.x * z.x - mul.y * z.y - mul.z * z.z - mul.w * z.w
				+ (auxC.y * fractal->transformCommon.offsetA0);
	z.y = oldZ.y * oldZ.x;
	z.z = oldZ.z * oldZ.x;
	z.w = oldZ.w * oldZ.x;
	z *= fractal->transformCommon.constantMultiplier1220;

	z.y += auxC.x * fractal->transformCommon.offsetB0;

	z += fractal->transformCommon.additionConstant0000;

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z.y = tp.y * cos(beta) + tp.z * sin(beta);
			z.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z.x = tp.x * cos(delta) + tp.w * sin(delta);
			z.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
}

/**
 * abs add  constant,  z = abs( z + pre-offset) + post-offset
 */
void TransfAbsAddConstant4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	z += fractal->transformCommon.additionConstant0000;

	if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);
	if (fractal->transformCommon.functionEnabled) z.w = fabs(z.w);

	z += fractal->transformCommon.offset0000;
}

/**
 * abs.  Add abs constantV2,  z = abs( z + constant) - abs( z - constant) - z:
 * This formula contains aux.color
 */
void TransfAbsAddTgladFold4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 limit = fractal->transformCommon.additionConstant0000;

	CVector4 oldZ = z;

	if (fractal->transformCommon.functionEnabledAx
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
		z.x = fabs(z.x + limit.x) - fabs(z.x - limit.x) - z.x;

	if (fractal->transformCommon.functionEnabledAy
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
		z.y = fabs(z.y + limit.y) - fabs(z.y - limit.y) - z.y;

	if (fractal->transformCommon.functionEnabledAz
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
		z.z = fabs(z.z + limit.z) - fabs(z.z - limit.z) - z.z;

	if (fractal->transformCommon.functionEnabledAw
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
		z.w = fabs(z.w + limit.w) - fabs(z.w - limit.w) - z.w;

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (!fractal->transformCommon.functionEnabledCxFalse)
		{
			if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x;
			if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y;
			if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z;
			if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w;
		}
		else
		{
			if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x * (fabs(z.x) - limit.x);
			if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y * (fabs(z.y) - limit.y);
			if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z * (fabs(z.z) - limit.z);
			if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w * (fabs(z.w) - limit.w);
		}
	}
}

/**
 * abs add conditional4D
 */
void TransfAbsAddConditional4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledx)
	{
		z.x = sign(z.x)
					* (fractal->transformCommon.offset1111.x - fabs(z.x)
							+ fabs(z.x) * fractal->transformCommon.scale0000.x);
	}

	if (fractal->transformCommon.functionEnabledy)
	{
		z.y = sign(z.y)
					* (fractal->transformCommon.offset1111.y - fabs(z.y)
							+ fabs(z.y) * fractal->transformCommon.scale0000.y);
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		z.z = sign(z.z)
					* (fractal->transformCommon.offset1111.z - fabs(z.z)
							+ fabs(z.z) * fractal->transformCommon.scale0000.z);
	}

	if (fractal->transformCommon.functionEnabledw)
	{
		z.w = sign(z.w)
					* (fractal->transformCommon.offset1111.w - fabs(z.w)
							+ fabs(z.w) * fractal->transformCommon.scale0000.w);
	}
	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}

/**
 * abs Multi - Multiple parameters for abs functions
 */
void TransfAbsAddMulti4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempA = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 tempB = CVector4(0.0, 0.0, 0.0, 0.0);

	if (fractal->transformCommon.functionEnabledAx)
		tempA.x = fabs(z.x + fractal->transformCommon.additionConstant0000.x);

	if (fractal->transformCommon.functionEnabledx)
		tempB.x = fabs(z.x - fractal->transformCommon.offset0000.x);

	z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale1111.x);

	if (fractal->transformCommon.functionEnabledAy)
		tempA.y = fabs(z.y + fractal->transformCommon.additionConstant0000.y);

	if (fractal->transformCommon.functionEnabledy)
		tempB.y = fabs(z.y - fractal->transformCommon.offset0000.y);

	z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale1111.y);

	if (fractal->transformCommon.functionEnabledAz)
		tempA.z = fabs(z.z + fractal->transformCommon.additionConstant0000.z);

	if (fractal->transformCommon.functionEnabledz)
		tempB.z = fabs(z.z - fractal->transformCommon.offset0000.z);

	z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale1111.z);

	if (fractal->transformCommon.functionEnabledAw)
		tempA.w = fabs(z.w + fractal->transformCommon.additionConstant0000.w);

	if (fractal->transformCommon.functionEnabledz)
		tempB.w = fabs(z.w - fractal->transformCommon.offset0000.w);

	z.w = tempA.w - tempB.w - (z.w * fractal->transformCommon.scale1111.w);

	// post-offset
	z += fractal->transformCommon.offsetA0000;

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}

/**
 * Adds c constant to z vector 4D
 */
void TransfAddConstant4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	z += fractal->transformCommon.additionConstant0000;
}

/**
 * box fold 4D
 * This formula contains aux.color
 */
void TransfBoxFold4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	if (z.x > fractal->mandelbox.foldingLimit)
	{
		z.x = fractal->mandelbox.foldingValue - z.x;
	}
	else if (z.x < -fractal->mandelbox.foldingLimit)
	{
		z.x = -fractal->mandelbox.foldingValue - z.x;
	}
	if (z.y > fractal->mandelbox.foldingLimit)
	{
		z.y = fractal->mandelbox.foldingValue - z.y;
	}
	else if (z.y < -fractal->mandelbox.foldingLimit)
	{
		z.y = -fractal->mandelbox.foldingValue - z.y;
	}
	if (z.z > fractal->mandelbox.foldingLimit)
	{
		z.z = fractal->mandelbox.foldingValue - z.z;
	}
	else if (z.z < -fractal->mandelbox.foldingLimit)
	{
		z.z = -fractal->mandelbox.foldingValue - z.z;
	}
	if (z.w > fractal->mandelbox.foldingLimit)
	{
		z.w = fractal->mandelbox.foldingValue - z.w;
	}
	else if (z.w < -fractal->mandelbox.foldingLimit)
	{
		z.w = -fractal->mandelbox.foldingValue - z.w;
	}
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x;
		if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y;
		if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z;
		if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w;
	}
}

/**
 * box fold 4D Tglad
 * This formula contains aux.color
 */
void TransfBoxFold4dTgladIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 limit = fractal->transformCommon.offset1111;

	if (!fractal->transformCommon.functionEnabledCyFalse)
	{
		z.x = fabs(z.x + limit.x) - fabs(z.x - limit.x) - z.x;
		z.y = fabs(z.y + limit.y) - fabs(z.y - limit.y) - z.y;
		z.z = fabs(z.z + limit.z) - fabs(z.z - limit.z) - z.z;
		z.w = fabs(z.w + limit.w) - fabs(z.w - limit.w) - z.w;
	}
	else
	{
		if (fractal->transformCommon.functionEnabledAx)
		{
			if (aux.i > fractal->transformCommon.startIterationsA)
			{
				limit.x *= (1.0
										 - 1.0
												 / (1.0
														 + (aux.i - fractal->transformCommon.startIterationsA)
																 / fractal->transformCommon.offset0000.x))
									 * fractal->transformCommon.scale1111.x;
			}
			z.x = fabs(z.x + limit.x) - fabs(z.x - limit.x) - z.x;
		}
		if (fractal->transformCommon.functionEnabledAy)
		{
			if (aux.i > fractal->transformCommon.startIterationsB)
			{
				limit.y *= (1.0
										 - 1.0
												 / (1.0
														 + (aux.i - fractal->transformCommon.startIterationsB)
																 / fractal->transformCommon.offset0000.y))
									 * fractal->transformCommon.scale1111.y;
			}
			z.y = fabs(z.y + limit.y) - fabs(z.y - limit.y) - z.y;
		}
		if (fractal->transformCommon.functionEnabledAz)
		{
			if (aux.i > fractal->transformCommon.startIterationsB)
			{
				limit.z *= (1.0
										 - 1.0
												 / (1.0
														 + (aux.i - fractal->transformCommon.startIterationsC)
																 / fractal->transformCommon.offset0000.z))
									 * fractal->transformCommon.scale1111.z;
			}
			z.z = fabs(z.z + limit.z) - fabs(z.z - limit.z) - z.z;
		}
		if (fractal->transformCommon.functionEnabledAw)
		{
			if (aux.i > fractal->transformCommon.startIterationsB)
			{
				limit.w *= (1.0
										 - 1.0
												 / (1.0
														 + (aux.i - fractal->transformCommon.startIterationsD)
																 / fractal->transformCommon.offset0000.w))
									 * fractal->transformCommon.scale1111.w;
			}
			z.w = fabs(z.w + limit.w) - fabs(z.w - limit.w) - z.w;
		}
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (!fractal->transformCommon.functionEnabledCxFalse)
		{
			if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x;
			if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y;
			if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z;
			if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w;
		}
		else
		{
			if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x * (fabs(z.x) - limit.x);
			if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y * (fabs(z.y) - limit.y);
			if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z * (fabs(z.z) - limit.z);
			if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w * (fabs(z.w) - limit.w);
		}
	}
}

/**
 * Box Tiling 4d
 */
void TransfBoxTiling4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 size = fractal->transformCommon.offset2222;
	CVector4 oldZ = z;

	if (!fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledx && size.x != 0.0)
		{
			z.x -= round(z.x / size.x) * size.x;
		}
		if (fractal->transformCommon.functionEnabledyFalse && size.y != 0.0)
		{
			z.y -= round(z.y / size.y) * size.y;
		}
		if (fractal->transformCommon.functionEnabledzFalse && size.z != 0.0)
		{
			z.z -= round(z.z / size.z) * size.z;
		}
		if (fractal->transformCommon.functionEnabledwFalse && size.w != 0.0)
		{
			z.w -= round(z.w / size.w) * size.w;
		}
	}
	else
	{
		CVector4 repeatPos = fractal->transformCommon.offsetA1111;
		CVector4 repeatNeg = fractal->transformCommon.offsetB1111;

		if (fractal->transformCommon.functionEnabledx && z.x < (repeatPos.x + 0.5) * size.x
				&& z.x > (repeatNeg.x + 0.5) * -size.x && size.x != 0.0)
		{
			z.x -= round(z.x / size.x) * size.x;
		}
		if (fractal->transformCommon.functionEnabledyFalse && z.y < (repeatPos.y + 0.5) * size.y
				&& z.y > (repeatNeg.y + 0.5) * -size.y && size.y != 0.0)
		{
			z.y -= round(z.y / size.y) * size.y;
		}
		if (fractal->transformCommon.functionEnabledzFalse && z.z < (repeatPos.z + 0.5) * size.z
				&& z.z > (repeatNeg.z + 0.5) * -size.z && size.z != 0.0)
		{
			z.z -= round(z.z / size.z) * size.z;
		}
		if (fractal->transformCommon.functionEnabledwFalse && z.w < (repeatPos.w + 0.5) * size.w
				&& z.w > (repeatNeg.w + 0.5) * -size.w && size.w != 0.0)
		{
			z.w -= round(z.w / size.w) * size.w;
		}
	}

	if (fractal->transformCommon.functionEnabledBxFalse)
	{
		z.x = z.x * fractal->transformCommon.scale1 / (fabs(oldZ.x) + 1.0);
		z.y = z.y * fractal->transformCommon.scale1 / (fabs(oldZ.y) + 1.0);
		z.z = z.z * fractal->transformCommon.scale1 / (fabs(oldZ.z) + 1.0);
		z.w = z.w * fractal->transformCommon.scale1 / (fabs(oldZ.w) + 1.0);
	}

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			aux.DE = aux.DE * z.Length() * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}

/**
 * box wrap 4d
 */
void TransfBoxWrap4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 box_size = fractal->transformCommon.offset1111;
	CVector4 wrap_mode = z;
	CVector4 oldZ = z;

	if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);
	if (fractal->transformCommon.functionEnabledwFalse) z.w = fabs(z.w);

	if (fractal->transformCommon.functionEnabledx)
	{
		z.x += box_size.x;
		if (!fractal->transformCommon.functionEnabledFalse)
			wrap_mode.x = fmod(z.x, 2.0 * box_size.x);
		else
			wrap_mode.x = z.x - 2.0 * box_size.x * floor(z.x / 2.0 * box_size.x);
		z.x = wrap_mode.x - box_size.x;
	}
	if (fractal->transformCommon.functionEnabledByFalse)
	{
		z.y += box_size.y;
		if (!fractal->transformCommon.functionEnabledFalse)
			wrap_mode.y = fmod(z.y, 2.0 * box_size.y);
		else
			wrap_mode.y = z.y - 2.0 * box_size.y * floor(z.y / 2.0 * box_size.y);
		z.y = wrap_mode.y - box_size.y;
	}
	if (fractal->transformCommon.functionEnabledBzFalse)
	{
		z.z += box_size.z;
		if (!fractal->transformCommon.functionEnabledFalse)
			wrap_mode.z = fmod(z.z, 2.0 * box_size.z);
		else
			wrap_mode.z = z.z - 2.0 * box_size.z * floor(z.z / 2.0 * box_size.z);
		z.z = wrap_mode.z - box_size.z;
	}

	if (fractal->transformCommon.functionEnabledCxFalse)
	{
		z.w += box_size.w;
		if (!fractal->transformCommon.functionEnabledFalse)
			wrap_mode.w = fmod(z.w, 2.0 * box_size.w);
		else
			wrap_mode.w = z.w - 2.0 * box_size.w * floor(z.w / 2.0 * box_size.w);
		z.w = wrap_mode.w - box_size.w;
	}
	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		z.x = z.x * fractal->transformCommon.scale1 / (fabs(oldZ.x) + 1.0);
		z.y = z.y * fractal->transformCommon.scale1 / (fabs(oldZ.y) + 1.0);
		z.z = z.z * fractal->transformCommon.scale1 / (fabs(oldZ.z) + 1.0);
		z.w = z.w * fractal->transformCommon.scale1 / (fabs(oldZ.w) + 1.0);
	}
	if (fractal->transformCommon.functionEnabledAxFalse) z.x *= sign(oldZ.x);
	if (fractal->transformCommon.functionEnabledAyFalse) z.y *= sign(oldZ.y);
	if (fractal->transformCommon.functionEnabledAzFalse) z.z *= sign(oldZ.z);
	if (fractal->transformCommon.functionEnabledAwFalse) z.w *= sign(oldZ.w);

	aux.DE *= fractal->analyticDE.scale1;
}

/**
 * clamp 4d
 */
void TransfClamp4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 upper = fractal->transformCommon.offset1111;
	CVector4 lower = fractal->transformCommon.offsetNeg1111;
	CVector4 scale = fractal->transformCommon.scale0000;
	CVector4 oldZ = z;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
		if (z.x < lower.x) z.x = lower.x;
		if (z.x > upper.x) z.x = upper.x;
		z.x = oldZ.x * scale.x - z.x;
	}
	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
		if (z.y < lower.y) z.y = lower.y;
		if (z.y > upper.y) z.y = upper.y;
		z.y = oldZ.y * scale.y - z.y;
	}
	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);
		if (z.z < lower.z) z.z = lower.z;
		if (z.z > upper.z) z.z = upper.z;
		z.z = oldZ.z * scale.z - z.z;
	}

	if (fractal->transformCommon.functionEnabledAwFalse)
	{
		if (fractal->transformCommon.functionEnabledwFalse) z.w = fabs(z.w);
		if (z.w < lower.w) z.w = lower.w;
		if (z.w > upper.w) z.w = upper.w;
		z.w = oldZ.w * scale.w - z.w;
	}

	aux.DE *= fractal->analyticDE.scale1;
}

/**
 * iteration weight 4D
 */
void TransfIterationWeight4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zA4 = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
	CVector4 zB4 = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

	z = (z * fractal->transformCommon.scale) + (zA4 * fractal->transformCommon.offset)
			+ (zB4 * fractal->transformCommon.offset0);
	aux.DE *= fractal->transformCommon.scale;
}

/**
 * offset s curve
 * This formula contains aux.color
 */
void TransfOffsetSCurve4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 limit = fractal->transformCommon.additionConstant0000 / 2.0;
	CVector4 oldZ = z;

	CVector4 temp = fractal->transformCommon.additionConstant0000 / -2.0;
	CVector4 temp2 = temp * temp;
	CVector4 offS = CVector4(0.0, 0.0, 0.0, 0.0);
	if (z.w < 1e-15 && z.w > -1e-15) z.w = (z.w > 0) ? 1e-15 : -1e-15;

	if (fractal->transformCommon.functionEnabledAx)
	{
		offS.x += ((8.0 * temp.x * temp2.x) / ((z.x * z.x) + (4.0 * temp2.x)) - 2.0 * temp.x)
							* sign(z.x) * fractal->transformCommon.scale1;
		z.x += offS.x;
	}

	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		offS.y += ((8.0 * temp.y * temp2.y) / ((z.y * z.y) + (4.0 * temp2.y)) - 2.0 * temp.y)
							* sign(z.y) * fractal->transformCommon.scale1;
		z.y += offS.y;
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		offS.z += ((8.0 * temp.z * temp2.z) / ((z.z * z.z) + (4.0 * temp2.z)) - 2.0 * temp.z)
							* sign(z.z) * fractal->transformCommon.scale1;
		z.z += offS.z;
	}

	if (fractal->transformCommon.functionEnabledAwFalse)
	{
		offS.w += ((8.0 * temp.w * temp2.w) / ((z.w * z.w) + (4.0 * temp2.w)) - 2.0 * temp.w)
							* sign(z.w) * fractal->transformCommon.scale1;
		z.w += offS.w;
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (!fractal->transformCommon.functionEnabledCxFalse)
		{
			if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x * (fabs(z.x) - limit.x);
			if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y * (fabs(z.y) - limit.y);
			if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z * (fabs(z.z) - limit.z);
			if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w * (fabs(z.w) - limit.w);
		}
		else
		{
			aux.color += fractal->mandelbox.color.factor4D.x * offS.x;
			aux.color += fractal->mandelbox.color.factor4D.y * offS.y;
			aux.color += fractal->mandelbox.color.factor4D.z * offS.z;
			aux.color += fractal->mandelbox.color.factor4D.w * offS.w;
		}
	}
}

/**
 * Reciprocal4D from M3D, DarkBeam's code
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */
void TransfReciprocal4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempZ = z;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.x = (1.0 / fractal->transformCommon.offset1111.x)
								- 1.0 / (fabs(z.x) + fractal->transformCommon.offset1111.x);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.x = (fractal->transformCommon.offsetA1111.x)
								- 1.0 / (fabs(z.x) + fractal->transformCommon.offset1111.x);

		tempZ.x += fabs(z.x) * fractal->transformCommon.additionConstant0000.x; // function slope
		z.x = sign(z.x) * tempZ.x;
	}

	if (fractal->transformCommon.functionEnabledy)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.y = (1.0 / fractal->transformCommon.offset1111.y)
								- 1.0 / (fabs(z.y) + fractal->transformCommon.offset1111.y);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.y = (fractal->transformCommon.offsetA1111.y)
								- 1.0 / (fabs(z.y) + fractal->transformCommon.offset1111.y);

		tempZ.y += fabs(z.y) * fractal->transformCommon.additionConstant0000.y;
		z.y = sign(z.y) * tempZ.y;
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.z = (1.0 / fractal->transformCommon.offset1111.z)
								- 1.0 / (fabs(z.z) + fractal->transformCommon.offset1111.z);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.z = (fractal->transformCommon.offsetA1111.z)
								- 1.0 / (fabs(z.z) + fractal->transformCommon.offset1111.z);

		tempZ.z += fabs(z.z) * fractal->transformCommon.additionConstant0000.z;
		z.z = sign(z.z) * tempZ.z;
	}

	if (fractal->transformCommon.functionEnabledw)
	{
		if (fractal->transformCommon.functionEnabledAw)
			tempZ.w = (1.0 / fractal->transformCommon.offset1111.w)
								- 1.0 / (fabs(z.w) + fractal->transformCommon.offset1111.w);

		if (fractal->transformCommon.functionEnabledAwFalse)
			tempZ.w = (fractal->transformCommon.offsetA1111.w)
								- 1.0 / (fabs(z.w) + fractal->transformCommon.offset1111.w);

		tempZ.z += fabs(z.w) * fractal->transformCommon.additionConstant0000.w;
		z.z = sign(z.w) * tempZ.w;
	}

	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}

/**
 * rotation 4D
 */
void TransfRotation4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	CVector4 tp;
	if (fractal->transformCommon.rotation44a.x != 0)
	{
		tp = z;
		double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
		z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
		z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
	}
	if (fractal->transformCommon.rotation44a.y != 0)
	{
		tp = z;
		double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
		z.y = tp.y * cos(beta) + tp.z * sin(beta);
		z.z = tp.y * -sin(beta) + tp.z * cos(beta);
	}
	if (fractal->transformCommon.rotation44a.z != 0)
	{
		tp = z;
		double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
		z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
		z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
	}
	if (fractal->transformCommon.rotation44b.x != 0)
	{
		tp = z;
		double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
		z.x = tp.x * cos(delta) + tp.w * sin(delta);
		z.w = tp.x * -sin(delta) + tp.w * cos(delta);
	}
	if (fractal->transformCommon.rotation44b.y != 0)
	{
		tp = z;
		double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
		z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
		z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
	}
	if (fractal->transformCommon.rotation44b.z != 0)
	{
		tp = z;
		double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
		z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
		z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
	}
}

/**
 * scale 4D
 * This formula contains aux.actualScaleA
 */
void TransfScale4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double useScale = 1.0;
	useScale = aux.actualScaleA + fractal->transformCommon.scale;

	z *= useScale;

	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(useScale) + 1.0;
	else
		aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
	{
		// update actualScaleA for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
		if (fractal->transformCommon.functionEnabledMFalse)
			aux.actualScaleA = -vary;
		else
			aux.actualScaleA = aux.actualScaleA - vary;
	}
}

/**
 * spherical fold 4D
 * This formula contains aux.color
 */
void TransfSphericalFold4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// double r2 = z.Dot(z);
	// double r2 = z.x * z.x + z.y * z.y;
	// if (r2 < 1e-21 && r2 > -1e-21) r2 = (r2 > 0) ? 1e-21 : -1e-21;
	// r2 += z.z * z.z;
	double rr = z.Dot(z);
	z += fractal->transformCommon.offset0000;
	if (rr < fractal->transformCommon.minR2p25)
	{
		z *= fractal->transformCommon.maxMinR2factor;
		aux.DE *= fractal->transformCommon.maxMinR2factor;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		z *= fractal->transformCommon.maxR2d1 / rr;
		aux.DE *= fractal->transformCommon.maxR2d1 / rr;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		};
	}
	z -= fractal->transformCommon.offset0000;
}

/**
 * Surf Box Fold v2 4d
 * based on DarkBeam's code "Mixed fold by Luca 2016".
 * @reference
 * http:// http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/
 * httpwww-shaperich-comproshred-elite-review
 * This formula contains aux.color,
 */
void TransfSurfBoxFoldV24dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double colorAdd = 0.0;
	CVector4 oldZ = z;
	CVector4 foldMod = fractal->transformCommon.offset2222;
	CVector4 fold = fractal->transformCommon.offset1111;
	CVector4 sg = z;
	sg.x = sign(z.x);
	sg.y = sign(z.y);
	sg.z = sign(z.z);
	sg.w = sign(z.w);

	CVector4 folder = z;
	CVector4 Tglad = z;
	Tglad = fabs(z + fold) - fabs(z - fold) - z;
	folder = sg * (z - Tglad);
	folder = fabs(folder);

	folder.x = min(folder.x, foldMod.x);
	folder.y = min(folder.y, foldMod.y);
	folder.z = min(folder.z, foldMod.z);
	folder.w = min(folder.w, foldMod.w);

	z -= sg * folder;

	// analyic DE tweak
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		CVector4 zCol = z;
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd += fractal->mandelbox.color.factor4D.x
										* (fabs(zCol.x) - fractal->transformCommon.offset1111.x);
			if (zCol.y != oldZ.y)
				colorAdd += fractal->mandelbox.color.factor4D.y
										* (fabs(zCol.y) - fractal->transformCommon.offset1111.y);
			if (zCol.z != oldZ.z)
				colorAdd += fractal->mandelbox.color.factor4D.z
										* (fabs(zCol.z) - fractal->transformCommon.offset1111.z);
			if (zCol.w != oldZ.w)
				colorAdd += fractal->mandelbox.color.factor4D.z
										* (fabs(zCol.w) - fractal->transformCommon.offset1111.w);
		}
		else
		{
			if (zCol.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x;
			if (zCol.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y;
			if (zCol.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z;
			if (zCol.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w;
		}
		aux.color += colorAdd;
	}
}


/**
 * DIFSBoxDiagonalV1Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 *
 */
void DIFSBoxDiagonalV1Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	CVector4 boxSize = fractal->transformCommon.additionConstant111;

	// diag 1
	if (aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
		if (z.x > z.y) swap(z.x, z.y);

	//abs and boxfoldxy
	if (aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
		z.x = fabs(z.x);
	if (aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
		z.y = fabs(z.y);

	if (aux.i >= fractal->transformCommon.startIterationsI
			&& aux.i < fractal->transformCommon.stopIterationsI)
	{
		z.x -= boxSize.x;
		z.y -= boxSize.y;
	}

	double xOffset = fractal->transformCommon.offsetC0;
	double yOffset = fractal->transformCommon.offsetD0;

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
		if (z.x < xOffset) z.x = fabs(z.x - xOffset) + xOffset;

	if (fractal->transformCommon.functionEnabledyFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
		if (z.y < yOffset) z.y = fabs(z.y - yOffset) + yOffset;

	// reverse offset part 1
	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z.x -= fractal->transformCommon.offsetA0;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y -= fractal->transformCommon.offsetB0;

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		useScale = aux.actualScaleA + fractal->transformCommon.scale2;

		z *= useScale;

		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fabs(useScale) + 1.0;
		else
			aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

		if (fractal->transformCommon.functionEnabledFFalse
				&& aux.i >= fractal->transformCommon.startIterationsK
				&& aux.i < fractal->transformCommon.stopIterationsK)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	// reverse offset part 2
	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z.x += fractal->transformCommon.offsetA0;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y += fractal->transformCommon.offsetB0;

	// offset
		z += fractal->transformCommon.offset001;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	CVector4 zc = z;
	 zc = fabs(zc) - boxSize;
	double zcd = 1.0;
	zcd = max(zc.x, max(zc.y, zc.z));
	if (zcd > 0.0)
	{
		zc.x = max(zc.x, 0.0);
		zc.y = max(zc.y, 0.0);
		zc.z = max(zc.z, 0.0);
		zcd = zc.Length();
	}
	aux.dist = min(aux.dist, zcd / aux.DE);


	if (fractal->foldColor.auxColorEnabled)
	{

		colorAdd += fractal->mandelbox.color.factor.x * aux.dist;
		colorAdd += fractal->mandelbox.color.factor.y;
		colorAdd += fractal->mandelbox.color.factor.z * z.x * z.y;
		colorAdd += fractal->mandelbox.color.factorSp1 * max(zc.x, zc.y);
		//colorAdd += fractal->mandelbox.color.factorSp2 * trunc(max(zc.x, zc.y));
		//colorAdd += fractal->mandelbox.color.factorSp2 * round(max(zc.x, zc.y));
		colorAdd += fractal->mandelbox.color.factorSp2 * useScale;


		//aux.colorHybrid += colorAdd;

	if (!fractal->foldColor.auxColorEnabledFalse)
		aux.color += colorAdd;
	else
		aux.color = max(colorAdd, aux.color);

	}
}

/**
 * DIFSBoxDiagonalV2Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 *
 */
void DIFSBoxDiagonalV2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	CVector4 boxSize = fractal->transformCommon.additionConstant111;

	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
		z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledAyFalse
			&& aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
		z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledAzFalse
			&& aux.i >= fractal->transformCommon.startIterationsZ
			&& aux.i < fractal->transformCommon.stopIterationsZ)
		z.z = fabs(z.z);

	if (fractal->transformCommon.functionEnabledBx
			&& aux.i >= fractal->transformCommon.startIterationsI
			&& aux.i < fractal->transformCommon.stopIterationsI)
	{
		z.x -= boxSize.x;
		z.y -= boxSize.y;
	}

	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
		z -= boxSize;

	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsJ
			&& aux.i < fractal->transformCommon.stopIterationsJ)
	{
		z.x = fabs(z.x);
		int poly = fractal->transformCommon.int3;
		double psi = fabs(fmod(atan(z.y / z.x) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}

	// diag 1
	if (fractal->transformCommon.functionEnabledBy
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
		if (z.x > z.y) swap(z.x, z.y);





/*
	//double xOffset = fractal->transformCommon.offset0;
	//double yOffset = fractal->transformCommon.offset05;

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
		if (z.x < xOffset) z.x = fabs(z.x - xOffset) + xOffset;

	if (fractal->transformCommon.functionEnabledy
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
		if (z.y < yOffset) z.y = fabs(z.y - yOffset) + yOffset;
*/

	// diag 2
	if (fractal->transformCommon.functionEnabledByFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
		if (z.y > z.x) swap(z.x, z.y);


	// reverse offset part 1
	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z.x -= fractal->transformCommon.offsetA0;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y -= fractal->transformCommon.offsetB0;

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		useScale = aux.actualScaleA + fractal->transformCommon.scale2;

		z *= useScale;

		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fabs(useScale) + 1.0;
		else
			aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

		if (fractal->transformCommon.functionEnabledFFalse
				&& aux.i >= fractal->transformCommon.startIterationsK
				&& aux.i < fractal->transformCommon.stopIterationsK)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	// reverse offset part 2
	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z.x += fractal->transformCommon.offsetA0;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y += fractal->transformCommon.offsetB0;

	// offset2
		z -= fractal->transformCommon.offset002;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	CVector4 zc = z;
	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
		{
			 zc = fabs(zc) - boxSize;
			double zcd = 1.0;
			zcd = max(zc.x, max(zc.y, zc.z));
			if (zcd > 0.0)
			{
				zc.x = max(zc.x, 0.0);
				zc.y = max(zc.y, 0.0);
				zc.z = max(zc.z, 0.0);
				zcd = zc.Length();
			}
			aux.dist = min(aux.dist, zcd / aux.DE);
		}

		else
		{
			/*double dist = max(z.x, max(z.y, z.z));
				if (dist > 0.0)
				{
					zc.x = max(z.x, 0.0);
					zc.y = max(z.y, 0.0);
					if(fractal->transformCommon.functionEnabledz)
						zc.z = z.y;
					zc.z = max(zc.z, 0.0);
					dist = max(dist, zc.Length());
				}
			double maxDist = dist;
			maxDist = max(maxDist, z.Length());
			aux.DE = aux.DE * maxDist / z.Length() * fractal->analyticDE.scale1 +
			fractal->analyticDE.offset0; if(fractal->transformCommon.functionEnabledyFalse)
			{
				z = zc;
			}*/
			/*if (fractal->transformCommon.functionEnabledFalse)
				zc  =  fabs(zc) - boxSize;
			double zcd = 1.0;
			zcd = max(zc.x, max(zc.y, zc.z));
				if (zcd > 0.0)
				{
					zc.x = max(zc.x, 0.0);
					zc.y = max(zc.y, 0.0);
					zc.z = max(zc.z, 0.0);
					zcd = zc.Length();
				}
				aux.dist = min(aux.dist, zcd / aux.DE);*/
			if (fractal->transformCommon.functionEnabledBx) zc = fabs(zc) - boxSize;
			double zcd = 1.0;
			zcd = max(zc.x, max(zc.y, zc.z));
			if (zcd > 0.0)
			{
				zc.x = max(zc.x, 0.0);
				zc.y = max(zc.y, 0.0);
				zc.z = max(zc.z, 0.0);
				zcd = zc.Length();
			}
			aux.dist = min(aux.dist, zcd / aux.DE);
			double distDE = (z.Length()) /aux.DE;
			aux.dist =  aux.dist + (distDE - aux.dist) * (1.0 -  pow(1.0 - fractal->transformCommon.scale1, fractal->transformCommon.scaleB1));
		}
					aux.temp1000 *= fractal->transformCommon.scaleA1;
	}

	if (fractal->foldColor.auxColorEnabled)
	{

		colorAdd = fractal->mandelbox.color.factor.x * aux.dist;
		colorAdd += fractal->mandelbox.color.factor.y;
		colorAdd += fractal->mandelbox.color.factor.z * z.x * z.y;
		aux.color += colorAdd;
	}
}


/**
 * DIFSBoxDiagonalV3Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 *
 */
void DIFSBoxDiagonalV3Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	CVector4 boxSize = fractal->transformCommon.additionConstant0555;

	double xOffset = fractal->transformCommon.offset0;
	double yOffset = fractal->transformCommon.offset05;

	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
		z -= boxSize;

	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
		z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledAyFalse
			&& aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
		z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledAzFalse
			&& aux.i >= fractal->transformCommon.startIterationsZ
			&& aux.i < fractal->transformCommon.stopIterationsZ)
		z.z = fabs(z.z);

	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsJ
			&& aux.i < fractal->transformCommon.stopIterationsJ)
	{
		z.x = fabs(z.x);
		int poly = fractal->transformCommon.int3;
		double psi = fabs(fmod(atan(z.y / z.x) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledBy
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
		if (z.y > z.x) swap(z.x, z.y);

	if (fractal->transformCommon.functionEnabledBx
			&& aux.i >= fractal->transformCommon.startIterationsI
			&& aux.i < fractal->transformCommon.stopIterationsI)
		z = z - boxSize;

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
		if (z.x < xOffset) z.x = fabs(z.x - xOffset) + xOffset;

	if (fractal->transformCommon.functionEnabledy
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
		if (z.y < yOffset) z.y = fabs(z.y - yOffset) + yOffset;

	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z.x -= fractal->transformCommon.offset1;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y -= fractal->transformCommon.offsetA1;

	if (fractal->transformCommon.functionEnabledByFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
		if (z.y > z.x) swap(z.x, z.y);

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		useScale = aux.actualScaleA + fractal->transformCommon.scale2;

		z *= useScale;

		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fabs(useScale) + 1.0;
		else
			aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

		if (fractal->transformCommon.functionEnabledFFalse
				&& aux.i >= fractal->transformCommon.startIterationsK
				&& aux.i < fractal->transformCommon.stopIterationsK)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	if (aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
		z += fractal->transformCommon.offset111;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	// dIFS DE
	CVector4 zc = z;
	if (fractal->transformCommon.functionEnabledBx) zc = fabs(zc) - boxSize;
	double zcd = 1.0;
	zcd = max(zc.x, max(zc.y, zc.z));
	if (zcd > 0.0)
	{
		zc.x = max(zc.x, 0.0);
		zc.y = max(zc.y, 0.0);
		zc.z = max(zc.z, 0.0);
		zcd = zc.Length();
	}
	aux.dist = min(aux.dist, zcd / aux.DE);

	if (fractal->foldColor.auxColorEnabled) //TODO
	{
		colorAdd = fractal->mandelbox.color.factor.x * aux.dist;
		colorAdd += fractal->mandelbox.color.factor.y;
		colorAdd += fractal->mandelbox.color.factor.z * z.x * z.y;
		aux.color += colorAdd;
	}
}

//  experimental testing
/**
 * Hybrid Color Trial
 *
 * bailout may need to be adjusted with some formulas
 */
void TransfHybridColorIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double linearOffset = 0.0;
	double boxTrap = 0.0;
	double sphereTrap = 0.0;
	double planeBias = 0.0;
	double twoPoints = 0.0;
	double componentMaster = 0.0;

	// max linear offset
	if (fractal->transformCommon.functionEnabledMFalse)
	{
		double temp30 = 0.0;
		CVector4 temp31 = z;
		if (fractal->transformCommon.functionEnabledM) temp31 = fabs(temp31);

		temp30 = max(max(temp31.x, temp31.y), temp31.z);
		temp30 *= fractal->foldColor.scaleA1;
		linearOffset = temp30;
	}

	// box trap
	if (fractal->surfBox.enabledX2False)
	{
		CVector4 box = fractal->transformCommon.scale3D444;
		CVector4 temp35 = z;
		double temp39 = 0.0;
		if (fractal->transformCommon.functionEnabledCx) temp35 = fabs(temp35);

		temp35 = box - temp35;
		double temp36 = max(max(temp35.x, temp35.y), temp35.z);
		double temp37 = min(min(temp35.x, temp35.y), temp35.z);
		temp36 = temp36 + temp37 * fractal->transformCommon.offsetB0;
		temp36 *= fractal->transformCommon.scaleC1;

		if (fractal->surfBox.enabledY2False)
		{
			CVector4 temp38 = aux.c;

			if (fractal->transformCommon.functionEnabledCz) temp38 = fabs(temp38);
			temp38 = box - temp38;

			temp39 = max(max(temp38.x, temp38.y), temp38.z);
			double temp40 = min(min(temp38.x, temp38.y), temp38.z);
			temp39 = temp39 + temp40 * fractal->transformCommon.offsetA0;
			temp39 *= fractal->transformCommon.scaleE1;
		}
		boxTrap = temp36 + temp39;
	}

	// sphere trap
	if (fractal->transformCommon.functionEnabledzFalse)
	{
		double sphereRR = fractal->transformCommon.maxR2d1;
		double temp45 = z.Dot(z);
		double temp46 = sphereRR - temp45;
		double temp47 = temp46;
		double temp51 = temp46;
		if (fractal->transformCommon.functionEnabledAx) temp51 = fabs(temp51);
		temp51 *= fractal->transformCommon.scaleF1;

		if (fractal->transformCommon.functionEnabledyFalse && temp45 > sphereRR)
		{
			temp46 *= temp46 * fractal->transformCommon.scaleG1;
		}
		if (fractal->transformCommon.functionEnabledPFalse && temp45 < sphereRR)
		{
			temp47 *= temp47 * fractal->transformCommon.scaleB1;
		}
		sphereTrap = temp51 + temp47 + temp46;
	}

	// plane bias
	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		CVector4 tempP = z;
		if (fractal->transformCommon.functionEnabledEFalse)
		{
			tempP.x = tempP.x * tempP.y;
			tempP.x *= tempP.x;
		}
		else
		{
			tempP.x = fabs(tempP.x * tempP.y);
		}
		if (fractal->transformCommon.functionEnabledFFalse)
		{
			tempP.y = tempP.y * tempP.z;
			tempP.y *= tempP.y;
		}
		else
		{
			tempP.y = fabs(tempP.y * tempP.z);
		}

		if (fractal->transformCommon.functionEnabledKFalse)
		{
			tempP.z = tempP.z * tempP.x;
			tempP.z *= tempP.z;
		}
		else
		{
			tempP.z = fabs(tempP.z * tempP.x);
		}

		tempP = tempP * fractal->transformCommon.scale3D000;
		planeBias = tempP.x + tempP.y + tempP.z;
	}

	// two points
	if (fractal->transformCommon.functionEnabledBxFalse)
	{
		CVector4 PtOne = z - fractal->transformCommon.offset000;
		CVector4 PtTwo = z - fractal->transformCommon.offsetA000;
		double distOne = PtOne.Length(); // * weight
		double distTwo = PtTwo.Length();
		twoPoints = min(distOne, distTwo);
		if (fractal->transformCommon.functionEnabledAxFalse)
		{
			CVector4 PtThree = z - fractal->transformCommon.offsetF000;
			double distThree = PtThree.Length();
			twoPoints = min(twoPoints, distThree);
		}
		twoPoints *= fractal->transformCommon.scaleA1;
	}

	// build  componentMaster
	componentMaster = (linearOffset + boxTrap + sphereTrap + planeBias + twoPoints);

	aux.colorHybrid = componentMaster;
}

/**
 * Hybrid Color Trial2
 *
 *
 * bailout may need to be adjusted with some formulas
 */
void TransfHybridColor2Iteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z.x += 0.000000001; // so not detected as a  zero change in z ?????
	if (aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		double componentMaster = 0.0;
		double orbitPoints = 0.0;

		double totalDist = 0.0;
		double totalR = 0.0;
		double lastVec = 0.0;
		// double auxColor = 0.0;

		// double distEst = 0.0;
		// double planeBias = 0.0;
		// double factorR = fractal->mandelbox.color.factorR;

		// double lengthIter = 0.0;
		// double boxTrap = 0.0;
		// double sphereTrap = 0.0;
		// double lastDist = 0.0;
		// double addI = 0.0;

		// summation of r
		if (fractal->transformCommon.functionEnabledMFalse)
		{
			double total = aux.addDist;
			double newR = z.Length(); // aux.r?
			totalR = (total + newR) * fractal->transformCommon.scaleD1;
			aux.addDist = totalR;
		}

		// max distance travelled
		if (fractal->transformCommon.functionEnabledSFalse)
		{
			CVector4 oldPt = aux.old_z;
			CVector4 newPt = z;
			CVector4 diffZ = oldPt - newPt;
			double dist = diffZ.Length();
			aux.addDist += dist;
			totalDist = aux.addDist * fractal->foldColor.scaleC1;
			aux.old_z = z;
		}

		// last two  z lengths
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			if (aux.i < fractal->transformCommon.stopIterationsM)
			{
				double lastZ = aux.addDist;
				double newZ = z.Length();

				if (fractal->transformCommon.functionEnabledAzFalse) lastVec = newZ / lastZ;
				if (fractal->transformCommon.functionEnabledByFalse) lastVec = lastZ / newZ;
				if (fractal->transformCommon.functionEnabledBzFalse) lastVec = fabs(lastZ - newZ);

				lastVec *= fractal->transformCommon.scaleB1;
				aux.addDist = newZ;
			}
		}

		// orbitTrap points
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			CVector4 PtOne = z - fractal->transformCommon.offset000;
			CVector4 PtTwo = z - fractal->transformCommon.offsetA000;
			double distOne = PtOne.Length(); // * weight
			double distTwo = PtTwo.Length();
			orbitPoints = min(distOne, distTwo);
			if (fractal->transformCommon.functionEnabledAxFalse)
			{
				CVector4 PtThree = z - fractal->transformCommon.offsetF000;
				double distThree = PtThree.Length();
				orbitPoints = min(orbitPoints, distThree);
			}
			if (fractal->transformCommon.functionEnabledAyFalse)
			{
				CVector4 PtFour = z - fractal->transformCommon.additionConstantA000;
				double distFour = PtFour.Length();
				orbitPoints = min(orbitPoints, distFour);
			}
			orbitPoints *= fractal->transformCommon.scaleA1;
		}

		// build  componentMaster
		componentMaster = (totalDist + orbitPoints + lastVec + totalR);
		componentMaster *= fractal->transformCommon.scale;

		if (!fractal->transformCommon.functionEnabledFalse)
		{
			aux.temp1000 = min(aux.temp1000, componentMaster);
			aux.colorHybrid = aux.temp1000;
		}
		else
		{
			aux.colorHybrid = componentMaster;
		}
	}
}

/**
 * Testing  fragmentarium code, mdifs by knighty (jan 2012)
 *
 */
void TestingIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	// CVector4 oldZ = z;
	// double fillet = fractal->transformCommon.offset0;
	CVector4 boxSize = fractal->transformCommon.additionConstant0555;

	double xOffset = fractal->transformCommon.offset0;
	double yOffset = fractal->transformCommon.offset05;

	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
		z -= boxSize;

	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
		z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledAyFalse
			&& aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
		z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledAzFalse
			&& aux.i >= fractal->transformCommon.startIterationsZ
			&& aux.i < fractal->transformCommon.stopIterationsZ)
		z.z = fabs(z.z);

	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsJ
			&& aux.i < fractal->transformCommon.stopIterationsJ)
	{
		z.x = fabs(z.x);
		int poly = fractal->transformCommon.int3;
		double psi = fabs(fmod(atan(z.y / z.x) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledBy
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
		if (z.y > z.x) swap(z.x, z.y);

	if (fractal->transformCommon.functionEnabledBx
			&& aux.i >= fractal->transformCommon.startIterationsI
			&& aux.i < fractal->transformCommon.stopIterationsI)
		z = z - boxSize;

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
		if (z.x < xOffset) z.x = fabs(z.x - xOffset) + xOffset;

	if (fractal->transformCommon.functionEnabledy
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
		if (z.y < yOffset) z.y = fabs(z.y - yOffset) + yOffset;

	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z.x -= fractal->transformCommon.offset1;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y -= fractal->transformCommon.offsetA1;

	if (fractal->transformCommon.functionEnabledByFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
		if (z.y > z.x) swap(z.x, z.y);

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		useScale = aux.actualScaleA + fractal->transformCommon.scale2;

		z *= useScale;

		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fabs(useScale) + 1.0;
		else
			aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

		if (fractal->transformCommon.functionEnabledFFalse
				&& aux.i >= fractal->transformCommon.startIterationsK
				&& aux.i < fractal->transformCommon.stopIterationsK)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	if (aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
		z += fractal->transformCommon.offset111;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	CVector4 zc = z;

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
		{
			if (fractal->transformCommon.functionEnabledBx) zc = fabs(zc) - boxSize;
			double zcd = 1.0;
			zcd = max(zc.x, max(zc.y, zc.z));
			if (zcd > 0.0)
			{
				zc.x = max(zc.x, 0.0);
				zc.y = max(zc.y, 0.0);
				zc.z = max(zc.z, 0.0);
				zcd = zc.Length();
			}
			aux.dist = min(aux.dist, zcd / aux.DE);
		}

		else
		{
			/*double dist = max(z.x, max(z.y, z.z));
				if (dist > 0.0)
				{
					zc.x = max(z.x, 0.0);
					zc.y = max(z.y, 0.0);
					if(fractal->transformCommon.functionEnabledz)
						zc.z = z.y;
					zc.z = max(zc.z, 0.0);
					dist = max(dist, zc.Length());
				}
			double maxDist = dist;
			maxDist = max(maxDist, z.Length());
			aux.DE = aux.DE * maxDist / z.Length() * fractal->analyticDE.scale1 +
			fractal->analyticDE.offset0; if(fractal->transformCommon.functionEnabledyFalse)
			{
				z = zc;
			}*/
			/*if (fractal->transformCommon.functionEnabledFalse)
				zc  =  fabs(zc) - boxSize;
			double zcd = 1.0;
			zcd = max(zc.x, max(zc.y, zc.z));
				if (zcd > 0.0)
				{
					zc.x = max(zc.x, 0.0);
					zc.y = max(zc.y, 0.0);
					zc.z = max(zc.z, 0.0);
					zcd = zc.Length();
				}
				aux.dist = min(aux.dist, zcd / aux.DE);*/
			if (fractal->transformCommon.functionEnabledBx) zc = fabs(zc) - boxSize;
			double zcd = 1.0;
			zcd = max(zc.x, max(zc.y, zc.z));
			if (zcd > 0.0)
			{
				zc.x = max(zc.x, 0.0);
				zc.y = max(zc.y, 0.0);
				zc.z = max(zc.z, 0.0);
				zcd = zc.Length();
			}
			aux.dist = min(aux.dist, zcd / aux.DE);
			double distDE = (z.Length()) /aux.DE;
			aux.dist =  aux.dist + (distDE - aux.dist) * (1.0 -  pow(1.0 - fractal->transformCommon.scale1, fractal->transformCommon.scaleB1));
		}
					aux.temp1000 *= fractal->transformCommon.scaleA1;
	}

	if (fractal->foldColor.auxColorEnabled)
	{

		colorAdd = fractal->mandelbox.color.factor.x * aux.dist;
		colorAdd += fractal->mandelbox.color.factor.y;
		colorAdd += fractal->mandelbox.color.factor.z * z.x * z.y;
		aux.color += colorAdd;
	}
}

/**
 * 4D boxbulb, Formula based on Mandelbox (ABox). Extended to 4 dimensions and with variable scale
 * parameter.
 * This formula contains aux.color and aux.actualScale
 */
void Testing4dIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;

	// parabolic.w = paraOffset + iter *slope + (iter *iter *scale)
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{
		double parabScale = 0.0;
		if (fractal->Cpara.parabScale != 0.0)
			parabScale = aux.i * aux.i * 0.001 * fractal->Cpara.parabScale;
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope) + (parabScale);
		z.w += paraAddP0;
	}

	// sinusoidal w
	double sinAdd = 0.0;
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		sinAdd = sin((aux.i + fractal->transformCommon.offset0) / fractal->transformCommon.scaleA1)
						 * fractal->transformCommon.scaleC1;
		z.w += sinAdd;
	}

	/*	CVector4 temp = fractal->transformCommon.offset0000;
		CVector4 temp2 = temp * temp;

		z.x += ((8.0 * temp.x * temp2.x) / ((z.x * z.x) + (4.0 * temp2.x)) - 2.0 * temp.x) * sign(z.x) *
		fractal->transformCommon.scale1;
		z.y += ((8.0 * temp.y * temp2.y) / ((z.y * z.y) + (4.0 * temp2.y)) - 2.0 * temp.y) * sign(z.y) *
		fractal->transformCommon.scale1;
		z.z += ((8.0 * temp.z * temp2.z) / ((z.z * z.z) + (4.0 * temp2.z)) - 2.0 * temp.z) * sign(z.z) *
		fractal->transformCommon.scale1;
		z.w += ((8.0 * temp.w * temp2.w) / ((z.w * z.w) + (4.0 * temp2.w)) - 2.0 * temp.w) * sign(z.w) *
		fractal->transformCommon.scale1*/

	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		oldZ = z;
		z.x = fabs(z.x + fractal->transformCommon.offset1111.x)
					- fabs(z.x - fractal->transformCommon.offset1111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.offset1111.y)
					- fabs(z.y - fractal->transformCommon.offset1111.y) - z.y;
		z.z = fabs(z.z + fractal->transformCommon.offset1111.z)
					- fabs(z.z - fractal->transformCommon.offset1111.z) - z.z;
		z.w = fabs(z.w + fractal->transformCommon.offset1111.w)
					- fabs(z.w - fractal->transformCommon.offset1111.w) - z.w;
		zCol = z;
	}
	CVector4 temp = fractal->transformCommon.offset0000;
	CVector4 temp2 = temp * temp;
	// if (z.w < 1e-016) z.w = 1e-016;
	if (z.w < 1e-21 && z.w > -1e-21) z.w = (z.w > 0) ? 1e-21 : -1e-21;
	z.x += ((8.0 * temp.x * temp2.x) / ((z.x * z.x) + (4.0 * temp2.x)) - 2.0 * temp.x) * sign(z.x)
				 * fractal->transformCommon.scale1;
	z.y += ((8.0 * temp.y * temp2.y) / ((z.y * z.y) + (4.0 * temp2.y)) - 2.0 * temp.y) * sign(z.y)
				 * fractal->transformCommon.scale1;
	z.z += ((8.0 * temp.z * temp2.z) / ((z.z * z.z) + (4.0 * temp2.z)) - 2.0 * temp.z) * sign(z.z)
				 * fractal->transformCommon.scale1;
	z.w += ((8.0 * temp.w * temp2.w) / ((z.w * z.w) + (4.0 * temp2.w)) - 2.0 * temp.w) * sign(z.w)
				 * fractal->transformCommon.scale1;

	// r power
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);
		rrCol = rr;
		if (fractal->mandelboxVary4D.rPower != 1.0) rr = pow(rr, fractal->mandelboxVary4D.rPower);

		// spherical fold
		// z += fractal->transformCommon.offset0000;
		if (rr < fractal->transformCommon.minR2p25)
		{
			z *= fractal->transformCommon.maxMinR2factor;
			aux.DE *= fractal->transformCommon.maxMinR2factor;
			// colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rr);
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			z *= fractal->transformCommon.maxR2d1 / rr;
			aux.DE *= fractal->transformCommon.maxR2d1 / rr;
			// colorAdd += fractal->mandelbox.color.factorSp2 * (rr - fractal->transformCommon.minR2p25);
		}
		/*else
			colorAdd += fractal->mandelbox.color.factorSp2
							* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);*/

		// z -= fractal->transformCommon.offset0000;
	}

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{

		useScale = aux.actualScaleA + fractal->transformCommon.scale;

		z *= useScale;
		aux.DE = aux.DE * fabs(useScale);

		if (aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			// update actualScale for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		CVector4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
			z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
			z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
			z.y = tp.y * cos(beta) + tp.z * sin(beta);
			z.z = tp.y * -sin(beta) + tp.z * cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
			z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
			z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
			z.x = tp.x * cos(delta) + tp.w * sin(delta);
			z.w = tp.x * -sin(delta) + tp.w * cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
			z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
			z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
			z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
			z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
		}
	}
	z += fractal->transformCommon.additionConstant0000;

	/*CVector4 temp = fractal->transformCommon.offset0000;
	CVector4 temp2 = temp * temp;

	z.x += ((8.0 * temp.x * temp2.x) / ((z.x * z.x) + (4.0 * temp2.x)) - 2.0 * temp.x) * sign(z.x)
				 * fractal->transformCommon.scale1;
	z.y += ((8.0 * temp.y * temp2.y) / ((z.y * z.y) + (4.0 * temp2.y)) - 2.0 * temp.y) * sign(z.y)
				 * fractal->transformCommon.scale1;
	z.z += ((8.0 * temp.z * temp2.z) / ((z.z * z.z) + (4.0 * temp2.z)) - 2.0 * temp.z) * sign(z.z)
				 * fractal->transformCommon.scale1;
	z.w += ((8.0 * temp.w * temp2.w) / ((z.w * z.w) + (4.0 * temp2.w)) - 2.0 * temp.w) * sign(z.w)
				 * fractal->transformCommon.scale1;*/

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.r * aux.DE * 16.0 * fractal->analyticDE.scale1
							 * sqrt(fractal->foldingIntPow.zFactor * fractal->foldingIntPow.zFactor + 2.0
											+ fractal->analyticDE.offset2)
							 / SQRT_3
						 + fractal->analyticDE.offset1;

		z = z * 2.0;
		double x2 = z.x * z.x;
		double y2 = z.y * z.y;
		double z2 = z.z * z.z;
		double temp = 1.0 - z2 / (x2 + y2);
		CVector4 zTemp;
		zTemp.x = (x2 - y2) * temp;
		zTemp.y = 2.0 * z.x * z.y * temp;
		zTemp.z = -2.0 * z.z * sqrt(x2 + y2);
		zTemp.w = z.w;
		z = zTemp;
		z.z *= fractal->foldingIntPow.zFactor;
	}

	// color updated v2.13 & mode2 v2.14
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd +=
					fractal->mandelbox.color.factor.x * (fabs(zCol.x) - fractal->mandelbox.color.factor4D.x);
			if (zCol.y != oldZ.y)
				colorAdd +=
					fractal->mandelbox.color.factor.y * (fabs(zCol.y) - fractal->mandelbox.color.factor4D.y);
			if (zCol.z != oldZ.z)
				colorAdd +=
					fractal->mandelbox.color.factor.z * (fabs(zCol.z) - fractal->mandelbox.color.factor4D.z);
			if (zCol.w != oldZ.w)
				colorAdd +=
					fractal->mandelbox.color.factor.z * (fabs(zCol.w) - fractal->mandelbox.color.factor4D.w);
			if (rrCol < fractal->transformCommon.maxR2d1)
			{
				if (rrCol < fractal->transformCommon.minR2p25)
					colorAdd +=
						fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
						+ fractal->mandelbox.color.factorSp2
								* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
				else
					colorAdd +=
						fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			}
		}
		else
		{
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor4D.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor4D.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor4D.z;
			if (zCol.w != oldZ.w) colorAdd += fractal->mandelbox.color.factor4D.w;
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1;
			else if (rrCol < fractal->transformCommon.maxR2d1)
				colorAdd += fractal->mandelbox.color.factorSp2;
		}
		aux.color += colorAdd;
	}
}

/**
 * testing log
 */
void TestingLogIteration(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse
				&& aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
			z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse
				&& aux.i >= fractal->transformCommon.startIterationsY
				&& aux.i < fractal->transformCommon.stopIterationsY)
			z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse
				&& aux.i >= fractal->transformCommon.startIterationsZ
				&& aux.i < fractal->transformCommon.stopIterationsZ)
			z.z = fabs(z.z);
	}
	double r = aux.r;
	double de1 = 0.0;
	double de2 = 0.0;
	double de3 = 0.0;
	double de4 = 0.0;
	CVector4 zer0s = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 newZ1 = zer0s;
	CVector4 newZ2 = zer0s;
	CVector4 newZ3 = zer0s;
	CVector4 newZ4 = zer0s;

	double th0 = fractal->bulb.betaAngleOffset;
	double ph0 = fractal->bulb.alphaAngleOffset;

	if (!fractal->transformCommon.functionEnabledxFalse)
	{
		th0 += asin(z.z / r);
		ph0 += atan2(z.y, z.x);
	}
	else
	{
		th0 += acos(z.z / r);
		ph0 += atan(z.y / z.x);
	}

	CVector4 w8ts = fractal->transformCommon.offset1111;
	double rp = r;

	if (fractal->transformCommon.functionEnabledx) // 0ne
	{
		newZ1 = z * w8ts.x;
		de1 = w8ts.x;
	}

	if (fractal->transformCommon.functionEnabledy) // two
	{
		de2 = (rp * aux.DE * 2.0 + 1.0) * w8ts.y;
		rp *= r;
		double th = th0 * 2.0 * fractal->transformCommon.scaleA1;
		double ph = ph0 * 2.0 * fractal->transformCommon.scaleB1;

		if (!fractal->transformCommon.functionEnabledxFalse)
		{
			double costh = cos(th);
			newZ2 = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}
		else
		{
			double sinth = sin(th);
			newZ2 = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		newZ2 *= w8ts.y;
	}

	if (fractal->transformCommon.functionEnabledz) // three
	{
		rp = r * r;
		de3 = (rp * aux.DE * 3.0 + 1.0) * w8ts.z;
		rp *= r;
		double th = th0 * 3.0 * fractal->transformCommon.scaleA1;
		double ph = ph0 * 3.0 * fractal->transformCommon.scaleB1;

		if (!fractal->transformCommon.functionEnabledxFalse)
		{
			double costh = cos(th);
			newZ3 = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}
		else
		{
			double sinth = sin(th);
			newZ3 = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		newZ3 *= w8ts.z;
	}

	if (fractal->transformCommon.functionEnabledw) // four
	{
		rp = r * r * r;
		de4 = (rp * aux.DE * 4.0 + 1.0) * w8ts.w;
		rp *= r;
		double th = th0 * 4.0 * fractal->transformCommon.scaleA1;
		double ph = ph0 * 4.0 * fractal->transformCommon.scaleB1;

		if (!fractal->transformCommon.functionEnabledxFalse)
		{
			double costh = cos(th);
			newZ4 = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}
		else
		{
			double sinth = sin(th);
			newZ4 = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		newZ4 *= w8ts.w;
	}
	z = newZ1 + newZ2 + newZ3 + newZ4;
	aux.DE = fabs(de1 + de2 + de3 + de4);

	if (fractal->transformCommon.functionEnabledKFalse)
	{
		if (fractal->transformCommon.functionEnabledDFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD)
			swap(z.x, z.y);
		if (fractal->transformCommon.functionEnabledEFalse
				&& aux.i >= fractal->transformCommon.startIterationsE
				&& aux.i < fractal->transformCommon.stopIterationsE)
			swap(z.x, z.z);

		// swap
		if (fractal->transformCommon.functionEnabledBxFalse) z.x = -z.x;
		if (fractal->transformCommon.functionEnabledByFalse) z.y = -z.y;
		if (fractal->transformCommon.functionEnabledBzFalse) z.z = -z.z;
	}

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
