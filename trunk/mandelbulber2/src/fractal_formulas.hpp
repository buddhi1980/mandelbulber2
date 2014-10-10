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

#ifndef FRACTAL_FORMULAS_HPP_
#define FRACTAL_FORMULAS_HPP_

#include "four_fractals.hpp"

struct sMandelbulbAux
{
	double r_dz;
	double r;
};

struct sMandelboxAux
{
	double mboxDE;
	double mboxColor;
	double actualScale;
};

struct sIFSAux
{
	double ifsDE;
};

struct sAexionAux
{
	CVector3 c;
	double cw;
	int iterNo;
};

void MandelbulbIteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux);
void Mandelbulbulb2Iteration(CVector3 &z, sMandelbulbAux &aux);
void Mandelbulbulb3Iteration(CVector3 &z, sMandelbulbAux &aux);
void Mandelbulbulb4Iteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux);
void MandelbulbulbPower2Iteration(CVector3 &z);
void XenodreambuieIteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux);
void MandelboxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux);
void SmoothMandelboxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux);
void BoxFoldBulbPow2Iteration(CVector3 &z, const cFractal *fractal);
void MengerSpongeIteration(CVector3 &z, sIFSAux &aux);
void KaleidoscopicIFSIteration(CVector3 &z, const cFractal *fractal, sIFSAux &aux);
void MandelboxVaryScale4DIteration(CVector3 &z, double &w, const cFractal *fractal, sMandelboxAux &aux);
void AexionIteration(CVector3 &z, double &w, const cFractal *fractal, sAexionAux &aux);
void HypercomplexIteration(CVector3 &z, double &w);
void QuaternionIteration(CVector3 &z, double &w);
void BenesiIteration(CVector3 &z, CVector3 &c);
void BristorbrotIteration(CVector3 &z);

#endif /* FRACTAL_FORMULAS_HPP_ */
