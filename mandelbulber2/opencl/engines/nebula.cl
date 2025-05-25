/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Nebula dractal renderig kernel
 */

//------------------ MAIN RENDER FUNCTION --------------------
kernel void Nebula(__global float4 *inImage, _global float4 *outImage,
	__constant sClInConstants *consts, int initRandomSeed)
{
	const int index = get_global_id(0);
	int sortBufferSize = params.width * params.height;

	int ii = sortedZBuffer[sortBufferSize - index - 1].i;
	int x = ii % params.width;
	int y = ii / params.width;
	int2 scr = (int2){x, y};

	int randomSeed = initRandomSeed;

	float4 point;
	point.x = (Random(10000, &randomSeed) / 5000.0f - 1.0f) * 2.0f;
	point.y = (Random(10000, &randomSeed) / 5000.0f - 1.0f) * 2.0f;
	point.z = (Random(10000, &randomSeed) / 5000.0f - 1.0f) * 2.0f;

	point.w = consts->sequence.initialWAxis[0];
	int maxN = consts->sequence.formulaMaxiter[0];

	float4 z = point;
	float4 c = z;
	int i;

	// formula init
	sExtendedAuxCl aux;

	aux.c = c;
	aux.const_c = c;
	aux.old_z = z;
	aux.pos_neg = 1.0f;
	aux.r = length(z);
	aux.DE = 1.0f;
	aux.DE0 = 0.0;
	aux.dist = 1000.0f;
	aux.pseudoKleinianDE = 1.0f;
	aux.actualScale = consts->fractal[fractalIndex].mandelbox.scale;
	aux.actualScaleA = 0.0f;
	aux.color = 1.0f;
	aux.colorHybrid = 0.0f;
	aux.temp1000 = 1000.0f;

	int fractalIndex = 0;
	int sequence = 0;
	__constant sFractalCl *fractal;
	__constant sFractalCl *defaultFractal = &consts->fractal[fractalIndex];

	// loop
	for (i = 0; i < maxN; i++)
	{

#if defined(IS_HYBRID)
		sequence = consts->sequence.hybridSequence[min(i, 249)];
#else
		sequence = 0;
#endif

		fractal = &consts->fractal[sequence];

		aux.i = i;

#ifdef ITERATION_WEIGHT
		if (consts->sequence.formulaWeight[sequence] > 0)
		{
#endif

#if defined(IS_HYBRID)
			switch (sequence)
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
#else	 // not HYBRID
		z = FORMULA_ITER_0(z, fractal, &aux);
#endif // defined(IS_HYBRID)

#ifdef ITERATION_WEIGHT
		}
#endif

		if (aux.r < 0.0f) // if was run DummyIteration
		{
			float high = consts->sequence.bailout[sequence] * 10.0f;
			z = high;
			aux.r = length(z);
			out.distance = 10.0f;
			out.iters = 1;
			out.z = z;
			out.colorIndex = 0.0f;
			return out;
		}

		if (consts->sequence.addCConstant[sequence])
		{
			switch (fractal->formula)
			{
				case 64: // aboxMod1
				case 73: // amazingSurf
				{
					if (consts->sequence.juliaEnabled[sequence])
					{
						float4 juliaC = consts->sequence.juliaConstant[sequence]
														* consts->sequence.constantMultiplier[sequence];
						z += (float4){juliaC.y, juliaC.x, juliaC.z, juliaC.w};
					}
					else
					{
						z += (float4){aux.const_c.y, aux.const_c.x, aux.const_c.z, aux.const_c.w}
								 * consts->sequence.constantMultiplier[sequence];
					}
					break;
				}

				default:
				{
					if (consts->sequence.juliaEnabled[sequence])
					{
						z += consts->sequence.juliaConstant[sequence]
								 * consts->sequence.constantMultiplier[sequence];
					}
					else
					{
						z += aux.const_c * consts->sequence.constantMultiplier[sequence];
					}
				}
			}
		}

#ifdef ITERATION_WEIGHT
		if (consts->sequence.isHybrid)
		{
			float k = consts->sequence.formulaWeight[sequence];
			if (k < 1.0f)
			{
				z = SmoothCVector(tempZ, z, k);
				float kn = 1.0f - k;
				aux.DE = aux.DE * k + tempAuxDE * kn;
				aux.color = aux.color * k + tempAuxColor * kn;
			}
		}
#endif

		// calculate r

		float2 screenPoint =
			(float2){z.x * 0.2f + params.width * 0.5f, z.y * 0.2f + params.height * 0.5f};
		screenPoint = clamp(screenPoint, 0.0f, (float2){params.width - 1, params.height - 1});
		int screenIndex = (int)(screenPoint.x + screenPoint.y * params.width);

		float4 old = out[screenIndex];

		float4 outPixel;
		outPixel.s0 = old.s0 + fabs(point.z);
		outPixel.s1 = old.s1 + fabs(point.y);
		outPixel.s2 = old.s2 + fabs(point.z);
		outPixel.s3 = old.s3 + 1.0f;
	};

	aux.r = length(z);

	// escape conditions
	if (consts->sequence.checkForBailout[sequence])
	{
		if (aux.r > consts->sequence.bailout[sequence])
		{
			out.maxiter = false;
			break;
		}
	}
}
}
