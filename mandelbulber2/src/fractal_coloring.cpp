/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * TODO: description
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
		if (fractalColoring.icRadFalse) initColorValue = xyzC.Length() * fractalColoring.icRadWeight;
		if (fractalColoring.icXYZFalse)
		{
			xyzC = fabs(xyzC) * fractalColoring.xyzC111;
			initColorValue += xyzC.x + xyzC.y + xyzC.z;
		}
		colorValue = initColorValue * 256.0;
	}

	if (fractalColoring.radFalse)
	{
		rad1000 = r;
		if (fractalColoring.radSquaredFalse) rad1000 = r * r;
		rad1000 *= 1000.0 * fractalColoring.radWeight;
	}

	if (fractalColoring.radDivDeFalse)
	{
		radDE5000 = r; // r /DE // was named r2
		if (fractalColoring.radDivDeSquaredFalse) radDE5000 = r * r;
		radDE5000 *= 5000.0 * fractalColoring.radDivDeWeight / fabs(extendedAux.DE);
	}

	double addValue = 0.0;

	// example of a basic input
	double xyzValue = 0.0;
	if (fractalColoring.xyzBiasEnabledFalse)
	{
		CVector3 xyzAxis = 0;
		xyzAxis = fabs(CVector3(z.x, z.y, z.z)) * fractalColoring.xyz000;

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
			colorValue += minR5000 * fractalColoring.orbitTrapWeight					// orbit trap only
										+ auxColorValue100 * fractalColoring.auxColorWeight // aux.color
										+ rad1000 + radDE5000 + addValue										// all extra inputs
										+ extendedAux.colorHybrid; // transf_hybrid_color inputs
			if (fractalColoring.iterGroupFalse)
			{
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
			}
			// "pseudo" global palette controls
			colorValue /= 256.0;

			if (fractalColoring.globalPaletteFalse)
			{
				if (fractalColoring.addEnabledFalse)
				{ // add curve inv
					if (colorValue > fractalColoring.addStartValue)
					{
						colorValue += (1.0
														- 1.0 / (1.0
																			+ (colorValue - fractalColoring.addStartValue)
																					/ fractalColoring.addSpread))
													* fractalColoring.addMax;
					}
				}

				if (fractalColoring.parabEnabledFalse)
				{
					if (colorValue > fractalColoring.parabStartValue)
					{
						double parab = colorValue - fractalColoring.cosStartValue;
						parab = parab * parab * fractalColoring.parabScale;
						colorValue += parab;
					}
				}
				//=(+0.5-0.5*COS((B43)*0.5PI()/e$rr ))*(E$36)
				if (fractalColoring.cosEnabledFalse)
				{ // trig palette
					if (colorValue > fractalColoring.cosStartValue)
					{
						double trig = (0.5
														- 0.5 * cos((colorValue - fractalColoring.cosStartValue) * M_PI
																				/ (fractalColoring.cosPeriod * 2.0)))
													* fractalColoring.cosAdd;
						colorValue += trig;
					}
				}

				if (fractalColoring.roundEnabledFalse)
				{
					double roundScale = fractalColoring.roundScale;
					colorValue /= roundScale;
					colorValue = round(colorValue) * roundScale;
				}
			}

			colorValue *= 256.0;

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
								: 0.0);*/ // temp
				// removed
				break;

			case coloringFunctionDonut: colorIndex = auxColorValue100 * 20.0 / extendedAux.i; break;

			case coloringFunctionDefault: colorIndex = minR5000; break;

			case coloringFunctionGeneral:
				//					if (minR5000 > 1e5) minR5000 = 1e5; // limit is only in hybrid mode?
				if (fractalColoring.extraColorEnabledFalse)
				{
					colorValue += minR5000 * fractalColoring.orbitTrapWeight					// orbit trap only
												+ auxColorValue100 * fractalColoring.auxColorWeight // aux.color
												+ rad1000																						// radius
												+ radDE5000																					// r /DE
												+ addValue;																					// all extra inputs

					if (fractalColoring.iterGroupFalse)
					{
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
					}

					// "pseudo" global palette controls
					colorValue /= 256.0;

					if (fractalColoring.globalPaletteFalse)
					{
						if (fractalColoring.addEnabledFalse)
						{ // add curve inv
							if (colorValue > fractalColoring.addStartValue)
							{
								colorValue += (1.0
																- 1.0 / (1.0
																					+ (colorValue - fractalColoring.addStartValue)
																							/ fractalColoring.addSpread))
															* fractalColoring.addMax;
							}
						}

						if (fractalColoring.parabEnabledFalse)
						{
							if (colorValue > fractalColoring.parabStartValue)
							{
								double parab = colorValue - fractalColoring.cosStartValue;
								parab = parab * parab * fractalColoring.parabScale;
								colorValue += parab;
							}
						}
						//=(+0.5-0.5*COS((B43)*0.5PI()/e$rr ))*(E$36)
						if (fractalColoring.cosEnabledFalse)
						{ // trig palette
							if (colorValue > fractalColoring.cosStartValue)
							{
								double trig = (0.5
																- 0.5 * cos((colorValue - fractalColoring.cosStartValue) * M_PI
																						/ (fractalColoring.cosPeriod * 2.0)))
															* fractalColoring.cosAdd;
								colorValue += trig;
							}
						}

						if (fractalColoring.roundEnabledFalse)
						{
							double roundScale = fractalColoring.roundScale;
							colorValue /= roundScale;
							colorValue = round(colorValue) * roundScale;
						}
					}

					colorValue *= 256.0;

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
