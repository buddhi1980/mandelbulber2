/*
 * fractal_formulas.hpp
 *
 *  Created on: Apr 27, 2014
 *      Author: krzysztof
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
