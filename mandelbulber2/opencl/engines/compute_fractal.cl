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
 * main fractal function for opencl
 */

#ifndef MANDELBULBER2_OPENCL_ENGINES_COMPUTE_FRACTAL_CL_
#define MANDELBULBER2_OPENCL_ENGINES_COMPUTE_FRACTAL_CL_

#ifndef FORMULA_ITER_0
#define FORMULA_ITER_0 DummyIteration
#endif /*FORMULA_ITER_0*/
#ifndef FORMULA_ITER_1
#define FORMULA_ITER_1 DummyIteration
#endif /*FORMULA_ITER_1*/
#ifndef FORMULA_ITER_2
#define FORMULA_ITER_2 DummyIteration
#endif /*FORMULA_ITER_2*/
#ifndef FORMULA_ITER_3
#define FORMULA_ITER_3 DummyIteration
#endif /*FORMULA_ITER_3*/
#ifndef FORMULA_ITER_4
#define FORMULA_ITER_4 DummyIteration
#endif /*FORMULA_ITER_4*/
#ifndef FORMULA_ITER_5
#define FORMULA_ITER_5 DummyIteration
#endif /*FORMULA_ITER_5*/
#ifndef FORMULA_ITER_6
#define FORMULA_ITER_6 DummyIteration
#endif /*FORMULA_ITER_6*/
#ifndef FORMULA_ITER_7
#define FORMULA_ITER_7 DummyIteration
#endif /*FORMULA_ITER_7*/
#ifndef FORMULA_ITER_8
#define FORMULA_ITER_8 DummyIteration
#endif /*FORMULA_ITER_8*/
#ifndef FORMULA_ITER_9
#define FORMULA_ITER_9 DummyIteration
#endif /*FORMULA_ITER_9*/

typedef struct
{
	float4 z;
	float iters;
	float distance;
	float colorIndex;
	float orbitTrapR;
	bool maxiter;
} formulaOut;

typedef enum {
	calcModeNormal = 0,
	calcModeColouring = 1,
	calcModeFake_AO = 2,
	calcModeDeltaDE1 = 3,
	calcModeDeltaDE2 = 4,
	calcModeOrbitTrap = 5
} enumCalculationModeCl;

float4 DummyIteration(float4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	return z;
}

formulaOut Fractal(__constant sClInConstants *consts, float3 point, sClCalcParams *calcParam,
	enumCalculationModeCl mode)
{
	// begin
	float dist = 0.0f;
	int N = calcParam->N;

	if (calcParam->normalCalculationMode) N *= 5;

	float4 z;
	z.x = point.x;
	z.y = point.y;
	z.z = point.z;
	z.w = 0.0f;

	float w = 0;

	float4 c = z;
	int i;

	formulaOut out;
	out.maxiter = consts->params.iterThreshMode;

	float colorMin = 1e8f;
	float orbitTrapTotal = 0.0f;

	// formula init
	sExtendedAuxCl aux;
	// TODO copy aux

	aux.c = c;
	aux.const_c = c;
	aux.old_z = z;
	aux.sum_z = z;
	aux.cw = 0.0f;

	aux.r_dz = 1.0f;
	aux.r = length(z);
	aux.DE = 1.0f;
	aux.pseudoKleinianDE = 1.0f;

	aux.actualScale =
		consts->fractal[0].mandelbox.scale; // TODO: forcedFormulaIndex has to be added here
	aux.actualScaleA = 0.0f;

	aux.color = 1.0f;
	aux.colorHybrid = 0.0f;
	aux.foldFactor = 0.0f;
	aux.minRFactor = 0.0f;
	aux.scaleFactor = 0.0f;
	aux.oldHybridFactor = 1.0f;
	aux.temp100 = 100.0f;
	aux.addDist = 0.0;

	int formulaIndex = 0;
	__constant sFractalCl *fractal;

	float4 lastZ = 0.0f;
	float4 lastLastZ = 0.0f;

	// loop
	for (i = 0; i < N; i++)
	{
		formulaIndex = consts->sequence.hybridSequence[i];
		fractal = &consts->fractal[formulaIndex];

		aux.i = i;

		lastLastZ = lastZ;
		lastZ = z;

		switch (formulaIndex)
		{
			case 0: z = FORMULA_ITER_0(z, fractal, &aux); break;
			case 1: z = FORMULA_ITER_1(z, fractal, &aux); break;
			case 2: z = FORMULA_ITER_2(z, fractal, &aux); break;
			case 3: z = FORMULA_ITER_3(z, fractal, &aux); break;
			case 4: z = FORMULA_ITER_4(z, fractal, &aux); break;
			case 5: z = FORMULA_ITER_5(z, fractal, &aux); break;
			case 6: z = FORMULA_ITER_6(z, fractal, &aux); break;
			case 7: z = FORMULA_ITER_7(z, fractal, &aux); break;
			case 8: z = FORMULA_ITER_8(z, fractal, &aux); break;
		}

		if (consts->sequence.addCConstant[formulaIndex])
		{
			switch (fractal->formula)
			{
				case 64: // aboxMod1
				case 73: // amazingSurf
				{
					if (consts->sequence.juliaEnabled[formulaIndex])
					{
						float4 juliaC = consts->sequence.juliaConstant[formulaIndex]
														* consts->sequence.constantMultiplier[formulaIndex];
						z += (float4){juliaC.y, juliaC.x, juliaC.z, juliaC.w};
					}
					else
					{
						z += (float4){aux.const_c.y, aux.const_c.x, aux.const_c.z, aux.const_c.w}
								 * consts->sequence.constantMultiplier[formulaIndex];
					}
					break;
				}

				default:
				{
					if (consts->sequence.juliaEnabled[formulaIndex])
					{
						z += consts->sequence.juliaConstant[formulaIndex]
								 * consts->sequence.constantMultiplier[formulaIndex];
					}
					else
					{
						z += aux.const_c * consts->sequence.constantMultiplier[formulaIndex];
					}
				}
			}
		}

		// calculate r
		aux.r = length(z);

		// escape conditions
		if (consts->sequence.checkForBailout[formulaIndex])
		{
			// mode normal or deltaDE center point
			if (mode == calcModeNormal || mode == calcModeDeltaDE1)
			{
				if (aux.r > consts->sequence.bailout[formulaIndex])
				{
					out.maxiter = false;
					break;
				}

				if (consts->sequence.useAdditionalBailoutCond[formulaIndex])
				{
					if (length(z - lastZ) / aux.r < 0.1f / consts->sequence.bailout[formulaIndex])
					{
						out.maxiter = false;
						break;
					}

					if (length(z - lastLastZ) / aux.r < 0.1f / consts->sequence.bailout[formulaIndex])
					{
						out.maxiter = false;
						break;
					}
				}
			}
			else if (mode == calcModeDeltaDE2)
			{
				if (i == calcParam->deltaDEMaxN) break;
			}
			else if (mode == calcModeColouring)
			{
				// TODO another coloring modes
				// TODO exception for mandelbox
				if (fractal->formula != 8) // not mandelbox
				{
					if (aux.r < colorMin) colorMin = aux.r;
				}
				if (aux.r > 1e15f || length(z - lastZ) / aux.r < 1e-15f) break;
			}
#ifdef FAKE_LIGHTS
			else if (mode == calcModeOrbitTrap)
			{
				float4 delta = z - (float4){consts->params.common.fakeLightsOrbitTrap.x,
														 consts->params.common.fakeLightsOrbitTrap.y,
														 consts->params.common.fakeLightsOrbitTrap.z, 0.0f};
				float distance = length(delta);
				if (i >= consts->params.common.fakeLightsMinIter
						&& i <= consts->params.common.fakeLightsMaxIter)
					orbitTrapTotal += (1.0f / (distance * distance));
				if (distance > 1000.0f)
				{
					out.orbitTrapR = orbitTrapTotal;
					break;
				}
			}
#endif // FAKE_LIGHTS
		}
	}

// calculate estimated distance

#ifdef ANALYTIC_LOG_DE
	dist = 0.5f * aux.r * native_log(aux.r) / (aux.r_dz);
#elif ANALYTIC_LINEAR_DE
	dist = (aux.r - 2.0f) / fabs(aux.DE);
#elif ANALYTIC_PSEUDO_KLEINIAN_DE
	float rxy = length(z.xy);
	dist = max(rxy - aux.pseudoKleinianDE, fabs(rxy * z.z) / aux.r) / (aux.DE);
#elif ANALYTIC_JOS_KLEINIAN_DE
	if (fractal->transformCommon.functionEnabled)
		z.y = min(z.y, fractal->transformCommon.foldingValue - z.y);

	dist =
		min(z.y, fractal->analyticDE.tweak005) / max(aux.pseudoKleinianDE, fractal->analyticDE.offset1);
#else
	dist = length(z);
#endif

	if (mode == calcModeColouring)
	{
		float r2 = aux.r / fabs(aux.DE);
		r2 = min(r2, 20.0f);

		if (consts->sequence.isHybrid)
		{
			colorMin = min(colorMin, 100.0f);
			float mBoxColor = aux.color;
			mBoxColor = min(mBoxColor, 1000.0f);
			out.colorIndex =
				((colorMin * 1000.0f + mBoxColor * 100.0f + r2 * 5000.0f) * aux.oldHybridFactor)
				+ aux.colorHybrid + (colorMin * 1000.0f * aux.minRFactor);
		}
		else
		{
			switch (consts->sequence.coloringFunction[formulaIndex])
			{
				case clColoringFunctionABox:
					out.colorIndex = aux.color * 100.0f + aux.r * fractal->mandelbox.color.factorR / 1.0e13f;
					// TODO another coloring modes
					break;
				case clColoringFunctionIFS: out.colorIndex = colorMin * 1000.0f; break;
				case clColoringFunctionAmazingSurf: out.colorIndex = colorMin * 200.0f; break;
				case clColoringFunctionABox2:
					out.colorIndex = aux.color * 100.0f * aux.foldFactor
													 + aux.r * fractal->mandelbox.color.factorR / 1e13f
													 + aux.scaleFactor * r2 * 5000.0f + aux.minRFactor * 1000.0f;
					break;
				case clColoringFunctionDonut: out.colorIndex = aux.color * 2000.0f / i; break;
				case clColoringFunctionDefault: out.colorIndex = colorMin * 5000.0f; break;
			}
		}
	}

	// end
	if (dist < 0.0f) dist = 0.0f;
	out.distance = dist;
	out.iters = i;
	out.z = z;

	return out;
}

#endif /* MANDELBULBER2_OPENCL_ENGINES_COMPUTE_FRACTAL_CL_ */
