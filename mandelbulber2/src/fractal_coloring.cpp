/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Graeme McLaren
 *
 * CalculateColorIndex(...) - Calculation of the color index of a point
 * based on miscellaneous criteria
 */
#include "fractal_coloring.hpp"

#include "fractal.h"

using namespace fractal;

sFractalColoring::sFractalColoring()
{
	addEnabledFalse = false;
	auxColorFalse = false;
	color4dEnabledFalse = false;
	cosEnabledFalse = false;
	extraColorEnabledFalse = false;
	globalPaletteFalse = false;
	icFabsFalse = false;
	icRadFalse = false;
	icXYZFalse = false;
	initCondFalse = false;
	iterAddScaleTrue = false;
	iterGroupFalse = false;
	iterScaleFalse = false;
	orbitTrapTrue = false;
	parabEnabledFalse = false;
	radDiv1e13False = false;
	radDivDeFalse = false;
	radDivDeSquaredFalse = false;
	radDivLogDeFalse = false;
	radFalse = false;
	radSquaredFalse = false;
	roundEnabledFalse = false;
	xyzBiasEnabledFalse = false;
	xyzDiv1e13False = false;
	xyzFabsFalse = false;
	xyzXSqrdFalse = false;
	xyzYSqrdFalse = false;
	xyzZSqrdFalse = false;

	coloringAlgorithm = fractalColoring_None;
	iStartValue = 0;

	CVector3 lineDirection;
	CVector3 xyz000;
	CVector3 xyzC111;

	addMax = 0.0;
	addSpread = 0.0;
	addStartValue = 0.0;
	auxColorHybridWeight = 0.0;
	auxColorWeight = 0.0;
	cosAdd = 0.0;
	cosPeriod = 0.0;
	cosStartValue = 0.0;
	icRadWeight = 0.0;
	initialColorValue = 0.0;
	initialMinimumR = 0.0;
	iterAddScale = 0.0;
	iterScale = 0.0;
	maxColorValue = 0.0;
	minColorValue = 0.0;
	orbitTrapWeight = 0.0;
	parabScale = 0.0;
	parabStartValue = 0.0;
	radDivDeWeight = 0.0;
	radWeight = 0.0;
	roundScale = 0.0;
	sphereRadius = 0.0;
	xyzIterScale = 0.0;
}

double CalculateColorIndex(bool isHybrid, double r, CVector4 z, double minimumR,
	const sExtendedAux &extendedAux, const sFractalColoring &fractalColoring,
	fractal::enumColoringFunction coloringFunction, const sFractal *defaultFractal)
{
	double colorIndex = 0.0;

	// color by numbers
	if (fractalColoring.extraColorEnabledFalse)
	{
		double colorValue = 0.0;

		// initial color value
		colorValue = fractalColoring.initialColorValue;

		// colorValue initial condition components
		if (fractalColoring.initCondFalse)
		{
			double initColorValue = 0.0;
			CVector3 xyzC = CVector3(extendedAux.c.x, extendedAux.c.y, extendedAux.c.z);
			if (fractalColoring.icRadFalse) initColorValue = xyzC.Length() * fractalColoring.icRadWeight;

			if (fractalColoring.icXYZFalse)
			{
				if (fractalColoring.icFabsFalse)
				{
					xyzC = xyzC * fractalColoring.xyzC111;
				}
				else
				{
					xyzC = fabs(xyzC) * fractalColoring.xyzC111;
				}
				initColorValue += xyzC.x + xyzC.y + xyzC.z;
			}
			colorValue += initColorValue;
		}

		// orbit trap component
		if (fractalColoring.orbitTrapTrue) colorValue += minimumR * fractalColoring.orbitTrapWeight;

		// auxiliary color components
		if (fractalColoring.auxColorFalse)
			colorValue += extendedAux.color * fractalColoring.auxColorWeight // aux.color
										+ extendedAux.colorHybrid													 // transf_hybrid_color inputs
												* fractalColoring.auxColorHybridWeight;

		// radius components (historic)
		if (fractalColoring.radFalse)
		{
			double rad = r;
			if (fractalColoring.radDiv1e13False) rad /= 1e13;
			if (fractalColoring.radSquaredFalse) rad = r * r;
			colorValue += rad * fractalColoring.radWeight;
		}

		// radius / DE components (historic)
		if (fractalColoring.radDivDeFalse)
		{
			double distEst = fabs(extendedAux.DE);
			if (fractalColoring.radDivLogDeFalse) distEst = fabs(extendedAux.r_dz);
			double radDE = r; // r /DE // was named r2
			if (fractalColoring.radDivDeSquaredFalse) radDE = r * r;
			colorValue += radDE * fractalColoring.radDivDeWeight / distEst;
		}

		double addValue = 0.0;
		// XYZ bias (example of a basic input)
		double xyzValue = 0.0;
		if (fractalColoring.xyzBiasEnabledFalse)
		{
			CVector3 xyzAxis = CVector3(z.x, z.y, z.z);
			if (fractalColoring.xyzDiv1e13False) xyzAxis /= 1e13; // mult rounding error ?

			if (fractalColoring.xyzFabsFalse)
			{
				xyzAxis = xyzAxis * fractalColoring.xyz000;
			}
			else
			{
				xyzAxis = fabs(xyzAxis) * fractalColoring.xyz000;
			}
			if (fractalColoring.xyzXSqrdFalse) xyzAxis.x *= xyzAxis.x;
			if (fractalColoring.xyzYSqrdFalse) xyzAxis.y *= xyzAxis.y;
			if (fractalColoring.xyzZSqrdFalse) xyzAxis.z *= xyzAxis.z;

			xyzValue = (xyzAxis.x + xyzAxis.y + xyzAxis.z)
								 * (1.0 + (fractalColoring.xyzIterScale * extendedAux.i));
		}

		addValue += xyzValue; // addValue accumulates outputs

		colorValue += addValue; // all extra inputs

		// colorValue iteration components
		if (fractalColoring.iterGroupFalse)
		{
			// Iter ADD,  this allows the input to be influenced by iteration number
			if (fractalColoring.iterAddScaleTrue && extendedAux.i > fractalColoring.iStartValue)
			{
				int iUse = extendedAux.i - fractalColoring.iStartValue;
				colorValue += fractalColoring.iterAddScale * iUse;
			}
			// Iter SCALE,
			if (fractalColoring.iterScaleFalse && extendedAux.i >= fractalColoring.iStartValue)
			{
				int iUse = extendedAux.i - fractalColoring.iStartValue;
				colorValue *= (iUse * fractalColoring.iterScale) + 1.0;
			}
		}

		// final colorValue controls
		if (fractalColoring.globalPaletteFalse)
		{
			// // add curve function
			if (fractalColoring.addEnabledFalse)
			{
				if (colorValue > fractalColoring.addStartValue)
				{
					colorValue +=
						(1.0
							- 1.0 / (1.0
												+ (colorValue - fractalColoring.addStartValue) / fractalColoring.addSpread))
						* fractalColoring.addMax;
				}
			}

			// parabolic function
			if (fractalColoring.parabEnabledFalse)
			{
				if (colorValue > fractalColoring.parabStartValue)
				{
					double parab = colorValue - fractalColoring.cosStartValue;
					parab = parab * parab * fractalColoring.parabScale;
					colorValue += parab;
				}
			}

			// trig function
			if (fractalColoring.cosEnabledFalse)
			{
				if (colorValue > fractalColoring.cosStartValue)
				{
					double trig = (0.5
													- 0.5 * cos((colorValue - fractalColoring.cosStartValue) * M_PI
																			/ (fractalColoring.cosPeriod * 2.0)))
												* fractalColoring.cosAdd;
					colorValue += trig;
				}
			}

			// round function
			if (fractalColoring.roundEnabledFalse)
			{
				double roundScale = fractalColoring.roundScale;
				colorValue /= roundScale;
				colorValue = round(colorValue) * roundScale;
			}
		}

		// palette max min controls
		double minCV = fractalColoring.minColorValue;
		double maxCV = fractalColoring.maxColorValue;
		if (colorValue < minCV) colorValue = minCV;
		if (colorValue > maxCV) colorValue = maxCV;

		colorIndex = colorValue * 256.0; // convert to colorValue units
	}

	// Historic HYBRID MODE coloring
	else if (isHybrid)
	{
		double mboxDE;
		mboxDE = extendedAux.DE;
		double r2 = r / fabs(mboxDE);
		if (r2 > 20) r2 = 20;
		if (minimumR > 100) minimumR = 100;
		double mboxColor;
		mboxColor = extendedAux.color;
		if (mboxColor > 1000) mboxColor = 1000;
		colorIndex = (minimumR * 1000.0 + mboxColor * 100 + r2 * 5000.0);
	}

	// NORMAL MODE Coloring
	else
	{

		switch (coloringFunction)
		{
			case coloringFunctionABox:
				colorIndex =
					extendedAux.color * 100.0														 // folds part
					+ r * defaultFractal->mandelbox.color.factorR / 1e13 // abs z part
					+ ((fractalColoring.coloringAlgorithm != fractalColoring_Standard) ? minimumR * 1000.0
																																						 : 0.0);
				break;
			case coloringFunctionIFS: colorIndex = minimumR * 1000.0; break;
			case coloringFunctionAmazingSurf: colorIndex = minimumR * 200.0; break;
			case coloringFunctionABox2:
			{
				double mboxDE;
				mboxDE = extendedAux.DE;
				double r2 = r / fabs(mboxDE);
				if (r2 > 20) r2 = 20;
				colorIndex = extendedAux.color * 100.0 * extendedAux.foldFactor		// folds part
										 + r * defaultFractal->mandelbox.color.factorR / 1e13 // abs z part
										 + extendedAux.scaleFactor * r2 * 5000.0 // for backwards compatibility
										 //	+ ((fractalColoring.coloringAlgorithm != fractalColoring_Standard) ?
										 // minimumR * 1000.0 : 0.0);
										 + minimumR * extendedAux.minRFactor * 1000.0; // orbit trap
				break;
			}
			case coloringFunctionDonut: colorIndex = extendedAux.color * 2000.0 / extendedAux.i; break;
			case coloringFunctionDefault: colorIndex = minimumR * 5000.0; break;
			case coloringFunctionUndefined: colorIndex = 0.0; break;
		}
	}

	return colorIndex;
}
