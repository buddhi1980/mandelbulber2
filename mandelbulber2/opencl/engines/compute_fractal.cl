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

typedef struct
{
	float4 z;
	float iters;
	float distance;
	float colourIndex;
	bool maxiter;
} formulaOut;

float4 DummyIteration(float4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	return z;
}

formulaOut Fractal(__constant sClInConstants *consts, float3 point, sClCalcParams *calcParam)
{
	// begin
	float dist = 0.0f;
	int N = calcParam->N;
	float4 z;
	z.x = point.x;
	z.y = point.y;
	z.z = point.z;
	z.w = 0.0f;

	float w = 0;

	float4 c = z;
	int i;
	formulaOut out;
	float colourMin = 1e8f;

	// formula init
	sExtendedAuxCl aux;
	// TODO copy aux
	aux.r_dz = 1.0f;
	aux.r = length(z);
	aux.color = 1.0f;
	aux.actualScale = 2.0f; // fractals.GetFractal(fractalIndex)->mandelbox.scale;
	aux.DE = 1.0f;
	aux.c = c;
	aux.const_c = c;
	aux.cw = 0.0f;
	aux.foldFactor = 0.0f;
	aux.minRFactor = 0.0f;
	aux.scaleFactor = 0.0f;
	aux.pseudoKleinianDE = 1.0f;

	// loop
	for (i = 0; i < N; i++)
	{
		int formulaIndex = consts->sequence.hybridSequence[i];
		__constant sFractalCl *fractal = &consts->fractal[formulaIndex];

		aux.i = i;
		float4 lastZ = z;

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
			if (consts->sequence.juliaEnabled[formulaIndex])
			{
				z += consts->sequence.juliaConstant[formulaIndex]
						 * consts->sequence.constantMultiplier[formulaIndex];
			}
			else
			{
				z += c * consts->sequence.constantMultiplier[formulaIndex];
			}
		}

// calculate r
#ifdef ANALYTIC_PSEUDO_KLEINIAN_DE
		aux.r = length(z.xy);
#else
		aux.r = length(z);
#endif

		if (aux.r < colourMin) colourMin = aux.r;

		if (consts->sequence.checkForBailout[formulaIndex])
		{
			if (aux.r > consts->sequence.bailout[formulaIndex])
			{
				break;
			}

			if (consts->sequence.useAdditionalBailoutCond[formulaIndex])
			{
				if (length(z - lastZ) / aux.r < 0.1f / consts->sequence.bailout[formulaIndex])
				{
					break;
				}
			}
		}
	}

// calculate estimated distance

#ifdef ANALYTIC_LOG_DE
	dist = 0.5f * aux.r * native_log(aux.r) / (aux.r_dz);
#elif ANALYTIC_LINEAR_DE
	dist = (aux.r - 2.0f) / fabs(aux.DE);
#elif ANALYTIC_PSEUDO_KLEINIAN_DE
	dist = max(aux.r - aux.pseudoKleinianDE, fabs(aux.r * z.z) / aux.r) / (aux.DE);
#else
	dist = length(z);
#endif

	out.colourIndex = colourMin * 5000.0f;

	// end
	if (dist < 0.0f) dist = 0.0f;
	out.distance = dist;
	out.iters = i;
	out.z = z;

	return out;
}

#endif /* MANDELBULBER2_OPENCL_ENGINES_COMPUTE_FRACTAL_CL_ */
