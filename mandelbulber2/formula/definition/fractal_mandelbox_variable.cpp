/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * Variable parameters over iteration time
 * Based on work by Tglad, Buddhi, DarkBeam
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 * This formula contains aux.color and aux.actualScaleA
 */

#include "all_fractal_definitions.h"

cFractalMandelboxVariable::cFractalMandelboxVariable() : cAbstractFractal()
{
	nameInComboBox = "Mandelbox - Variable";
	internalName = "mandelbox_variable";
	internalID = fractal::mandelboxVariable;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalMandelboxVariable::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
		if (aux.i > fractal->transformCommon.startIterationsB)
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
