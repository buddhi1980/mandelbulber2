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
 * CalculateColorIndex() function
 */

#ifdef USE_FRACTAL_COLORING

cl_float CalculateColorIndex(bool isHybrid, cl_float r, cl_float4 z, cl_float minimumR,
	const sExtendedAuxCl *extendedAux, __global sFractalColoringCl *fractalColoring,
	cl_int coloringFunction, __constant sFractalCl *defaultFractal)
{
	float colorIndex = 0.0f;
	// color by numbers

	if (fractalColoring->extraColorEnabledFalse)
	{
#ifdef USE_EXTRA_COLORING
		float colorValue = 0.0f;
		// initial color value
		colorValue = fractalColoring->initialColorValue;

		// colorValue initial condition components
		if (fractalColoring->initCondFalse)
		{
			float initColorValue = 0.0f;
			float3 xyzC = (float3){extendedAux->c.x, extendedAux->c.y, extendedAux->c.z};
			if (fractalColoring->icRadFalse) initColorValue = length(xyzC) * fractalColoring->icRadWeight;

			if (fractalColoring->icXYZFalse)
			{
				if (fractalColoring->icFabsFalse)
				{
					xyzC = xyzC * fractalColoring->xyzC111;
				}
				else
				{
					xyzC = fabs(xyzC) * fractalColoring->xyzC111;
				}
				initColorValue += xyzC.x + xyzC.y + xyzC.z;
			}
			colorValue += initColorValue;
		}

		// orbit trap component
		if (fractalColoring->orbitTrapTrue) colorValue += minimumR * fractalColoring->orbitTrapWeight;

		// auxiliary color components
		if (fractalColoring->auxColorFalse)
			colorValue += extendedAux->color * fractalColoring->auxColorWeight // aux.color
										+ extendedAux->colorHybrid // transf_hybrid_color inputs
												* fractalColoring->auxColorHybridWeight;

		// radius components (historic)
		if (fractalColoring->radFalse)
		{
			float rad = r;
			if (fractalColoring->radDiv1e13False) rad /= 1e13f;
			if (fractalColoring->radSquaredFalse) rad = r * r;
			colorValue += rad * fractalColoring->radWeight;
		}

		// radius / DE components (historic)
		if (fractalColoring->radDivDeFalse)
		{
			float distEst = fabs(extendedAux->DE);
			if (fractalColoring->radDivLogDeFalse) distEst = fabs(extendedAux->r_dz);
			float radDE = r; // r /DE // was named r2
			if (fractalColoring->radDivDeSquaredFalse) radDE = r * r;
			colorValue += radDE * fractalColoring->radDivDeWeight / distEst;
		}

		float addValue = 0.0f;
		// XYZ bias (example of a basic input)
		float xyzValue = 0.0f;
		if (fractalColoring->xyzBiasEnabledFalse)
		{
			float3 xyzAxis = (float3){z.x, z.y, z.z};
			if (fractalColoring->xyzDiv1e13False) xyzAxis /= 1e13f; // mult rounding error ?

			if (fractalColoring->xyzFabsFalse)
			{
				xyzAxis = xyzAxis * fractalColoring->xyz000;
			}
			else
			{
				xyzAxis = fabs(xyzAxis) * fractalColoring->xyz000;
			}
			if (fractalColoring->xyzXSqrdFalse) xyzAxis.x *= xyzAxis.x;
			if (fractalColoring->xyzYSqrdFalse) xyzAxis.y *= xyzAxis.y;
			if (fractalColoring->xyzZSqrdFalse) xyzAxis.z *= xyzAxis.z;

			xyzValue = (xyzAxis.x + xyzAxis.y + xyzAxis.z)
								 * (1.0f + (fractalColoring->xyzIterScale * extendedAux->i));
		}

		addValue += xyzValue; // addValue accumulates outputs

		colorValue += addValue; // all extra inputs

		// colorValue iteration components
		if (fractalColoring->iterGroupFalse)
		{
			// Iter ADD,  this allows the input to be influenced by iteration number
			if (fractalColoring->iterAddScaleTrue && extendedAux->i > fractalColoring->iStartValue)
			{
				int iUse = extendedAux->i - fractalColoring->iStartValue;
				colorValue += fractalColoring->iterAddScale * iUse;
			}
			// Iter SCALE,
			if (fractalColoring->iterScaleFalse && extendedAux->i >= fractalColoring->iStartValue)
			{
				int iUse = extendedAux->i - fractalColoring->iStartValue;
				colorValue *= (iUse * fractalColoring->iterScale) + 1.0f;
			}
		}

		// final colorValue controls
		if (fractalColoring->globalPaletteFalse)
		{
			// // add curve function
			if (fractalColoring->addEnabledFalse)
			{
				if (colorValue > fractalColoring->addStartValue)
				{
					colorValue += (1.0f
													- 1.0f / (1.0f
																		 + (colorValue - fractalColoring->addStartValue)
																				 / fractalColoring->addSpread))
												* fractalColoring->addMax;
				}
			}

			// parabolic function
			if (fractalColoring->parabEnabledFalse)
			{
				if (colorValue > fractalColoring->parabStartValue)
				{
					float parab = colorValue - fractalColoring->cosStartValue;
					parab = parab * parab * fractalColoring->parabScale;
					colorValue += parab;
				}
			}

			// trig function
			if (fractalColoring->cosEnabledFalse)
			{
				if (colorValue > fractalColoring->cosStartValue)
				{
					float trig = (0.5f
												 - 0.5f * cos((colorValue - fractalColoring->cosStartValue) * M_PI
																			/ (fractalColoring->cosPeriod * 2.0f)))
											 * fractalColoring->cosAdd;
					colorValue += trig;
				}
			}

			// round function
			if (fractalColoring->roundEnabledFalse)
			{
				float roundScale = fractalColoring->roundScale;
				colorValue /= roundScale;
				colorValue = round(colorValue) * roundScale;
			}
		}

		// palette max min controls
		float minCV = fractalColoring->minColorValue;
		float maxCV = fractalColoring->maxColorValue;
		if (colorValue < minCV) colorValue = minCV;
		if (colorValue > maxCV) colorValue = maxCV;

		colorIndex = colorValue * 256.0f; // convert to colorValue units
#endif																// USE_EXTRA_COLORING
	}

	// Historic HYBRID MODE coloring
	else if (isHybrid)
	{
		float mboxDE;
		mboxDE = extendedAux->DE;
		float r2 = min(r / fabs(mboxDE), 20.0f);
		minimumR = min(100.0f, minimumR);
		float mboxColor = min(extendedAux->color, 1000.0f);
		colorIndex = (minimumR * 1000.0f + mboxColor * 100.0f + r2 * 5000.0f);
	}

	// NORMAL MODE Coloring
	else
	{

		switch (coloringFunction)
		{
			case clColoringFunctionABox:
				colorIndex =
					extendedAux->color * 100.0f													 // folds part
					+ r * defaultFractal->mandelbox.color.factorR / 1e13 // abs z part
					+ ((fractalColoring->coloringAlgorithm != fractalColoringCl_Standard) ? minimumR * 1000.0f
																																								: 0.0f);
				break;
			case clColoringFunctionIFS: colorIndex = minimumR * 1000.0f; break;
			case clColoringFunctionAmazingSurf: colorIndex = minimumR * 200.0f; break;
			case clColoringFunctionABox2:
			{
				float mboxDE;
				mboxDE = extendedAux->DE;
				float r2 = r / fabs(mboxDE);
				if (r2 > 20.0f) r2 = 20.0f;
				colorIndex = extendedAux->color * 100.0f * extendedAux->foldFactor // folds part
										 + r * defaultFractal->mandelbox.color.factorR / 1e13f // abs z part
										 + extendedAux->scaleFactor * r2 * 5000.0f // for backwards compatibility
										 //	+ ((fractalColoring->coloringAlgorithm != fractalColoring_Standard) ?
										 // minimumR * 1000.0 : 0.0);
										 + minimumR * extendedAux->minRFactor * 1000.0f; // orbit trap
				break;
			}
			case clColoringFunctionDonut:
				colorIndex = extendedAux->color * 2000.0f / extendedAux->i;
				break;
			case clColoringFunctionDefault: colorIndex = minimumR * 5000.0f; break;
			case clColoringFunctionUndefined: colorIndex = 0.0f; break;
		}
	}

	return colorIndex;
}

#endif // USE_FRACTAL_COLORING