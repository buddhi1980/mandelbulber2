/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Compute - function fractal computation
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

#include "compute_fractal.hpp"
#include "fractal_formulas.hpp"
#include "common_math.h"


//temporary functions for performance profiling
/*
long int perf = 0;
int perfCount = 0;
inline unsigned long int rdtsc()
{
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (unsigned long int)ts.tv_sec * 1000000000LL + (unsigned long int)ts.tv_nsec;
}
*/

using namespace fractal;

template<fractal::enumCalculationMode Mode>
void Compute(const cFourFractals &four, const sFractalIn &in, sFractalOut *out)
{
	//QTextStream outStream(stdout);
	//clock_t tim;
	//tim = rdtsc();

    //repeat, move and rotate
    CVector3 point2 = in.point.mod(in.common.repeat) - in.common.fractalPosition;
	point2 = in.common.mRotFractalRotation.RotateVector(point2);

	CVector3 z = point2;
	double r = z.Length();
	CVector3 c = z;
	double minimumR = 1e20;
  double w = 0.0;
	double orbitTrapTotal = 0.0;
	double foldColor = 1.0;
	double foldDE = 1.0;

	out->maxiter = true;

	int fractalIndex = 0;
	if(in.forcedFormulaIndex >=0) fractalIndex = in.forcedFormulaIndex;

	const cFractal *defaultFractal = four.GetFractal(fractalIndex);

	sMandelbulbAux bulbAux[4];
	sMandelboxAux mandelboxAux[4];
	sIFSAux ifsAux[4];
	sAexionAux aexionAux[4];
	int maxFractals = (four.IsHybrid() || in.forcedFormulaIndex >= 0) ? 4 : 1;
	for(int i = 0; i < maxFractals; i++)
	{
		bulbAux[i].r_dz = 1.0;
		bulbAux[i].r = r;
		mandelboxAux[i].mboxDE = 1.0;
		mandelboxAux[i].mboxColor = 1.0;
		mandelboxAux[i].actualScale = four.GetFractal(i)->mandelbox.scale;
		ifsAux[i].ifsDE = 1.0;
		aexionAux[i].c = c;
		aexionAux[i].cw = 0;
	}

	//main iteration loop
	int i;
	for (i = 0; i < in.maxN; i++)
	{
		//foldings
		if(in.common.foldings.boxEnable)
		{
			BoxFolding(z, &in.common.foldings, foldColor);
			r = z.Length();
		}

		if(in.common.foldings.sphericalEnable)
		{
			SphericalFolding(z, &in.common.foldings, foldColor, foldDE, r);
			r = z.Length();
		}

		//hybrid fractal sequence
		int sequence;
		if(in.forcedFormulaIndex >= 0)
		{
			sequence = in.forcedFormulaIndex;
		}
		else
		{
			sequence = four.GetSequence(i);
		}
		const cFractal *fractal = four.GetFractal(sequence);

		//temporary vector for weight function
		CVector3 tempZ = z;

		if (!four.IsHybrid() || four.GetWeight(sequence) > 0.0)
		{
			//calls for fractal formulas
			switch (fractal->formula)
			{
				case mandelbulb:
				{
					bulbAux[sequence].r = r;
					MandelbulbIteration(z, fractal, bulbAux[sequence]);
					break;
				}
				case mandelbulb2:
				{
					bulbAux[sequence].r = r;
					Mandelbulb2Iteration(z, bulbAux[sequence]);
					break;
				}
				case mandelbulb3:
				{
					bulbAux[sequence].r = r;
					Mandelbulb3Iteration(z, bulbAux[sequence]);
					break;
				}
				case mandelbulb4:
				{
					bulbAux[sequence].r = r;
					Mandelbulb4Iteration(z, fractal, bulbAux[sequence]);
					break;
				}
				case fast_mandelbulb_power2:
				{
					MandelbulbPower2Iteration(z);
					break;
				}
				case xenodreambuie:
				{
					bulbAux[sequence].r = r;
					XenodreambuieIteration(z, fractal, bulbAux[sequence]);
					break;
				}
				case mandelbox:
				{
					MandelboxIteration(z, fractal, mandelboxAux[sequence]);
					break;
				}
				case mandelboxVaryScale4D:
				{
					MandelboxVaryScale4DIteration(z, w, fractal, mandelboxAux[sequence]);
					break;
				}
				case smoothMandelbox:
				{
					SmoothMandelboxIteration(z, fractal, mandelboxAux[sequence]);
					break;
				}
				case boxFoldBulbPow2:
				{
					BoxFoldBulbPow2Iteration(z, fractal);
					break;
				}
				case menger_sponge:
				{
					MengerSpongeIteration(z, ifsAux[sequence]);
					break;
				}
				case kaleidoscopicIFS:
				{
					KaleidoscopicIFSIteration(z, fractal, ifsAux[sequence]);
					break;
				}
				case aexion:
				{
					aexionAux[sequence].iterNo = i;
					AexionIteration(z, w, fractal, aexionAux[sequence]);
					break;
				}
				case hypercomplex:
				{
					HypercomplexIteration(z, w);
					break;
				}
				case quaternion:
				{
					QuaternionIteration(z, w);
					break;
				}
				case benesi:
				{
					BenesiIteration(z, c);
					break;
				}
				case bristorbrot:
				{
					BristorbrotIteration(z);
					break;
				}
				case ides:
				{
					IdesIteration(z);
					break;
				}
				case ides2:
				{
					Ides2Iteration(z, c);
					break;
				}
				case buffalo:
				{
					BuffaloIteration(z, fractal);
					break;
				}
				case quickdudley:
				{
					QuickDudleyIteration(z);
					break;
				}
				case lkmitch:
				{
					LkmitchIteration(z);
					break;
				}
				case makin3d2:
				{
					Makin3D2Iteration(z);
					break;
				}
				case msltoesym2:
				{
					MsltoeSym2(z, fractal);
					break;
				}
				case generalizedFoldBox:
				{
					GeneralizedFoldBoxIteration(z, fractal, mandelboxAux[sequence]);
					break;
				}
				case mandelbulb5:
				{
					bulbAux[sequence].r = r;
					Mandelbulb5Iteration(z, c, i, fractal, bulbAux[sequence]);
					break;
				}
      case mandelbox103:
      {
        Mandelbox103Iteration(z, c, i, fractal, mandelboxAux[sequence]);
        break;
      }
      case quaternion104:
      {
        CVector4 z4D(z, w);
      	Quaternion104Iteration(z4D, CVector4(c, 0.0), i, fractal);
        z = z4D.GetXYZ();
        w = z4D.w;
        break;
      }

				default:
					z = CVector3(0.0, 0.0, 0.0);
					break;
			}
		}

		//addition of constant
		switch(fractal->formula)
		{
			case menger_sponge:
			case kaleidoscopicIFS:
			case aexion:
      case mandelbulb5:
      case mandelbox103:
      case quaternion104:
			{
				break;
			}
			default:
			{
				if(in.common.juliaMode)
				{
					z += in.common.juliaC;
				}
				else
				{
					z += c * in.common.constantMultiplier;
				}
				break;
			}
		}

		if(four.IsHybrid())
		{
			z = SmoothCVector(tempZ, z, four.GetWeight(sequence));
		}

		//r calculation
		r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z + w * w);

		//escape conditions
		if (Mode == calcModeNormal)
		{
			if (r > in.common.bailout)
			{
				out->maxiter = false;
				break;
			}
		}
		else if (Mode == calcModeDeltaDE1)
		{
			if (r > in.common.bailout)
			{
				out->maxiter = false;
				break;
			}
		}
		else if (Mode == calcModeDeltaDE2)
		{
			if (i == in.maxN)
				break;
		}
		else if (Mode == calcModeColouring)
		{
			if (fractal->formula != mandelbox)
			{
				if (r < minimumR) minimumR = r;
			}
			if (r > 1e15)
				break;
		}
		else if (Mode == calcModeOrbitTrap)
		{
			CVector3 delta = z - in.common.fakeLightsOrbitTrap;
			double distance = delta.Length();
			if (i >= in.common.fakeLightsMinIter && i <= in.common.fakeLightsMaxIter) orbitTrapTotal += (1.0f / (distance * distance));
			if (distance > 1000)
			{
				out->orbitTrapR = orbitTrapTotal;
				break;
			}
		}
	}

	//final calculations
	if(Mode == calcModeNormal)
	{
		switch (defaultFractal->formula)
		{
			case mandelbulb:
			case mandelbulb5:

				out->distance = 0.5 * r * log(r) / bulbAux[fractalIndex].r_dz;
				break;
			case mandelbox:
			case smoothMandelbox:
			case mandelboxVaryScale4D:
			case generalizedFoldBox:
      case mandelbox103:
				out->distance = r / fabs(mandelboxAux[fractalIndex].mboxDE * foldDE);
				break;
			case menger_sponge:
			case kaleidoscopicIFS:
				out->distance = (r - 2.0) / (ifsAux[fractalIndex].ifsDE * foldDE);
				break;

			default:
				out->distance = -1.0;
				break;
		}
	}
	//color calculation
	else if (Mode == calcModeColouring)
	{
		if (four.IsHybrid())
		{
			if (minimumR > 100) minimumR = 100;


			double mboxColor = 0.0;
			double mboxDE = 1.0;
			for (int h = 0; h < 4; h++)
			{
				mboxColor += mandelboxAux[h].mboxColor;
				mboxDE *= mandelboxAux[h].mboxDE;
			}

			double r2 = r / fabs(mboxDE);
			if(r2 > 20) r2 = 20;

			if (mboxColor > 1000) mboxColor = 1000;

			out->colorIndex = minimumR * 1000.0 + mboxColor * 100 + r2 * 5000.0;
		}
		else
		{
			switch (defaultFractal->formula)
			{
				case mandelbox:
				case smoothMandelbox:
				case mandelboxVaryScale4D:
				case generalizedFoldBox:
					out->colorIndex = mandelboxAux[fractalIndex].mboxColor * 100.0 + r * defaultFractal->mandelbox.colorFactorR * foldColor;
					break;
        case mandelbox103:
          out->colorIndex = minimumR * i * 30;
        break;
				case menger_sponge:
				case kaleidoscopicIFS:
					out->colorIndex = minimumR * 1000.0;
					break;

				default:
					out->colorIndex = minimumR * 5000.0;
					break;
			}

		}
	}
	else
	{
		out->distance = 0.0;
	}

	out->iters = i + 1;
	out->z = z;
	//tim = rdtsc() - tim; perf+= tim; perfCount++; outStream << (double)perf/perfCount - 560.0 << endl;
	//------------- 3249 ns for all calculation  ----------------
}

template void Compute<calcModeNormal>(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeDeltaDE1>(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeDeltaDE2>(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeColouring>(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);
template void Compute<calcModeOrbitTrap>(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);
