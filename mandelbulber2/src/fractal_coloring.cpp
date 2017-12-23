/*
 * fractal_coloring.cpp
 *
 *  Created on: 19 gru 2017
 *      Author: krzysztof
 */
#include "fractal_coloring.hpp"
#include "fractal.h"

using namespace fractal;

double CalculateColorIndex(bool isHybrid, double r, CVector4 z, double minimumR,
	const sExtendedAux &extendedAux, const sFractalColoring &fractalColoring,
	fractal::enumColoringFunction coloringFunction, const sFractal *defaultFractal)
{
	double colorIndex = 0.0;

	double minR1000 = minimumR * 1000.0;								 // limited at 100,000 hybrid mode
	double minR5000 = minimumR * 5000.0;								 // DEFAULT
	double auxColorValue100 = extendedAux.color * 100.0; // limited at 100,000,
	double radDE5000 = 0.0;
	double rad1000 = 0.0;
	double colorValue = 0.0;

	if (fractalColoring.initCondFalse)
	{
	CVector3 xyzC = CVector3(extendedAux.c.x, extendedAux.c.y, extendedAux.c.z);

	double initColorValue = 0.0;

	initColorValue = xyzC.Length() * fractalColoring.icRadWeight;

	 xyzC = fabs(xyzC) * fractalColoring.xyzC111;
	initColorValue += xyzC.x + xyzC.y + xyzC.z;

	colorValue = initColorValue *1000.0;
	}

	if (fractalColoring.radFalse)
	{
		rad1000 = r;
		if (fractalColoring.radSquaredFalse) rad1000 = r * r;
		rad1000 *= 1000.0 * fractalColoring.radWeight;
	}

	if (fractalColoring.radDivDeFalse)
	{
		radDE5000 = r;
		if (fractalColoring.radSquaredFalse) radDE5000 = r * r;
		radDE5000 *= 5000.0 * fractalColoring.radDivDeWeight / fabs(extendedAux.DE);
		; // r /DE // was named r2
	}

	double addValue = 0.0;

	// example of a basic input
	double xyzValue = 0.0;
	if (fractalColoring.xyzBiasEnabledFalse)
	{
		CVector3 xyzAxis = 0;
		if (fractalColoring.cModeEnabledFalse)
		{
			xyzAxis =
				fabs(CVector3(extendedAux.c.x, extendedAux.c.y, extendedAux.c.z)) * fractalColoring.xyz000;
		}
		else
		{
			xyzAxis = fabs(CVector3(z.x, z.y, z.z)) * fractalColoring.xyz000;
		}
		if (fractalColoring.xyzXsqrdFalse) xyzAxis.x *= xyzAxis.x;
		if (fractalColoring.xyzYsqrdFalse) xyzAxis.y *= xyzAxis.y;
		if (fractalColoring.xyzZsqrdFalse) xyzAxis.z *= xyzAxis.z;

		// xyzAxis *=  fractalColoring.xyz000;
		xyzValue = (xyzAxis.x + xyzAxis.y + xyzAxis.z) * fractalColoring.xyzIterScale;
	}

	addValue += xyzValue; // addValue accumulates outputs

	// HYBRID MODE
	if (isHybrid)
	{

		//*new hybrid*
		if (fractalColoring.extraColorEnabledFalse)
		{
			colorValue += minR5000 * fractalColoring.orbitTrapWeight					 // orbit trap only
										 + auxColorValue100 * fractalColoring.auxColorWeight // aux.color
										 + rad1000 + radDE5000 + addValue										 // all extra inputs
										 + extendedAux.colorHybrid; // transf_hybrid_color inputs

			// Iter ADD,  this allows the input to be influenced by iteration number
			if (fractalColoring.iterAddScaleFalse && extendedAux.i > fractalColoring.iStartValue)
			{
				int iUse = extendedAux.i - fractalColoring.iStartValue;
				if (fractalColoring.iSquaredEnabledFalse) iUse *= iUse;
				if (fractalColoring.iInvertEnabledFalse) iUse = 1.0 / iUse;
				colorValue += fractalColoring.iterAddScale * iUse;
			}

			// Iter SCALE,
			if (fractalColoring.iterScaleFalse && extendedAux.i >= fractalColoring.iStartValue)
			{
				int iUse = extendedAux.i - fractalColoring.iStartValue;
				if (fractalColoring.iSquaredEnabledFalse) iUse *= iUse;

				if (fractalColoring.iInvertEnabledFalse)
				{
					if (fractalColoring.iSquaredEnabledFalse)
						colorValue *= (1.0 + 1.0 / (iUse + 1.0) / fractalColoring.iterScale);
					else
						colorValue *= (1.0 + 1.0 / (extendedAux.i + 1.0) / fractalColoring.iterScale);
				}
				else
				{
					colorValue *= (iUse * fractalColoring.iterScale) + 1.0;
				}
			}
			// "pseudo" palette controls
			if (fractalColoring.cosEnabledFalse)
			{ // trig palette
				double trig = 128 * -fractalColoring.cosAdd
											* (cos(colorValue * 2.0 * M_PI / fractalColoring.cosPeriod) - 1.0);
				colorValue += trig;
			}
			double minCV = fractalColoring.minColorValue;
			double maxCV = fractalColoring.maxColorValue;
			if (colorValue < minCV) colorValue = minCV;
			if (colorValue > maxCV) colorValue = maxCV;

			colorIndex = colorValue;
		}
		else
		{
			//*old hybrid*
			minR1000 = min(minR1000, 1e5);
			// if (minR1000 > 100000.0) minR1000 = 100000.0; // limit is only in old hybrid mode?
			if (auxColorValue100 > 1e5) auxColorValue100 = 1e5; // limit
			radDE5000 = 5000.0 * r / fabs(extendedAux.DE);			// was named r2
			if (radDE5000 > 1e5) radDE5000 = 1e5;
			double oldHybridValue = (minR1000 + auxColorValue100 + radDE5000); // old hybrid code
			colorIndex = oldHybridValue;
		}
	}
	// NORMAL MODE
	else
	{
		switch (coloringFunction)
		{
			case coloringFunctionABox:
				colorIndex =
					auxColorValue100																		 // aux.color
					+ r * defaultFractal->mandelbox.color.factorR / 1e13 // radius scale
					+ ((fractalColoring.coloringAlgorithm != sFractalColoring::fractalColoringStandard)
								? minR1000
								: 0.0);
				break;

			case coloringFunctionIFS: colorIndex = minR1000; break;

			case coloringFunctionAmazingSurf: colorIndex = minR1000 * 0.2; break;

			case coloringFunctionABox2:
				radDE5000 = 5000.0 * r / fabs(extendedAux.DE);					 // was named r2
				colorIndex = minR1000 * extendedAux.minRFactor					 // orbit trap DEFAULT
										 + auxColorValue100 * extendedAux.foldFactor // aux.color
										 + r * extendedAux.radiusFactor / 1e13			 // radius// this may be replaced
										 + radDE5000 * extendedAux.scaleFactor // r/DE  for backwards compatibility
										 + addValue;													 // all extra inputs

				/*+ ((fractalColoring.coloringAlgorithm != sFractalColoring::fractalColoringStandard)
								? minimumR * extendedAux.minRFactor * 1000.0
								: 0.0);*/ // temp removed
				break;

			case coloringFunctionDonut: colorIndex = auxColorValue100 * 20.0 / extendedAux.i; break;

			case coloringFunctionDefault: colorIndex = minR5000; break;

			case coloringFunctionGeneral:
				//					if (minR5000 > 1e5) minR5000 = 1e5; // limit is only in hybrid mode?
				if (fractalColoring.extraColorEnabledFalse)
				{
					colorValue += minR5000 * fractalColoring.orbitTrapWeight					 // orbit trap only
												 + auxColorValue100 * fractalColoring.auxColorWeight // aux.color
												 + rad1000																					 // radius
												 + radDE5000																				 // r /DE
												 + addValue;																				 // all extra inputs

					// Iter ADD,  this allows the input to be influenced by iteration number
					if (fractalColoring.iterAddScaleFalse && extendedAux.i > fractalColoring.iStartValue)
					{
						int iUse = extendedAux.i - fractalColoring.iStartValue;
						if (fractalColoring.iSquaredEnabledFalse) iUse *= iUse;
						if (fractalColoring.iInvertEnabledFalse) iUse = 1.0 / iUse;
						colorValue += fractalColoring.iterAddScale * iUse;
					}

					// Iter SCALE,
					if (fractalColoring.iterScaleFalse && extendedAux.i >= fractalColoring.iStartValue)
					{
						int iUse = extendedAux.i - fractalColoring.iStartValue;
						if (fractalColoring.iSquaredEnabledFalse) iUse *= iUse;

						if (fractalColoring.iInvertEnabledFalse)
						{
							if (fractalColoring.iSquaredEnabledFalse)
								colorValue *= (1.0 + 1.0 / (iUse + 1.0) / fractalColoring.iterScale);
							else
								colorValue *= (1.0 + 1.0 / (extendedAux.i + 1.0) / fractalColoring.iterScale);
						}
						else
						{
							colorValue *= (iUse * fractalColoring.iterScale) + 1.0;
						}
					}
					if (fractalColoring.cosEnabledFalse)
					{ // trig
																								if (colorValue > fractalColoring.cosStartValue)
                                                {
																										double useValue = colorValue;
                                                    useValue -= fractalColoring.cosStartValue;
                                                    double trig = 128 * -fractalColoring.cosAdd
                                                        * (cos(useValue * 2.0 * M_PI / fractalColoring.cosPeriod) - 1.0);
						colorValue += trig;
                                                }
					}
					double minCV = fractalColoring.minColorValue;
					double maxCV = fractalColoring.maxColorValue;
					if (colorValue < minCV) colorValue = minCV;
					if (colorValue > maxCV) colorValue = maxCV;

					colorIndex = colorValue;
				}
				else
				{
					colorIndex = minR5000;
				}
				break;
		}
	}

	return colorIndex;
}
