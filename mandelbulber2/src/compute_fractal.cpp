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
void Compute(const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out)
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
	if (in.forcedFormulaIndex >= 0) fractalIndex = in.forcedFormulaIndex;

	const cFractal *defaultFractal = fractals.GetFractal(fractalIndex);

	sExtendedAux extendedAux[NUMBER_OF_FRACTALS];
	int maxFractal = (in.forcedFormulaIndex >= 0) ? in.forcedFormulaIndex : fractals.GetMaxFractalIndex();
	int minFractal = (in.forcedFormulaIndex >= 0) ? in.forcedFormulaIndex : 0;
	for (int i = minFractal; i <= maxFractal; i++)
	{
		extendedAux[i].r_dz = 1.0;
		extendedAux[i].r = r;
		extendedAux[i].color = 1.0;
		extendedAux[i].actualScale = fractals.GetFractal(i)->mandelbox.scale;
		extendedAux[i].DE = 1.0;
		extendedAux[i].c = c;
		extendedAux[i].cw = 0;
		extendedAux[i].newR = 1e+20;
		extendedAux[i].axisBias = 1e+20;
		extendedAux[i].orbitTraps = 1e+20;
		extendedAux[i].transformSampling = 1e+20;
	}

	//main iteration loop
	int i;
	int sequence = 0;
	int lastSequnce = 0;

	for (i = 0; i < in.maxN; i++)
	{
		//foldings
		if (in.common.foldings.boxEnable)
		{
			BoxFolding(z, &in.common.foldings, foldColor);
			r = z.Length();
		}

		if (in.common.foldings.sphericalEnable)
		{
			SphericalFolding(z, &in.common.foldings, foldColor, foldDE, r);
			r = z.Length();
		}

		//hybrid fractal sequence
		if (in.forcedFormulaIndex >= 0)
		{
			sequence = in.forcedFormulaIndex;
		}
		else
		{
			sequence = fractals.GetSequence(i);
		}

		//for optimized DE calculation
		if(fractals.UseOptimizedDE())
		{
			extendedAux[sequence] = extendedAux[lastSequnce];
			lastSequnce = sequence;
		}

		const cFractal *fractal = fractals.GetFractal(sequence);

		//temporary vector for weight function
		CVector3 tempZ = z;

		extendedAux[sequence].r = r;

		if (!fractals.IsHybrid() || fractals.GetWeight(sequence) > 0.0)
		{
			//calls for fractal formulas
			switch (fractal->formula)
			{
				case mandelbulb:
				{
					MandelbulbIteration(z, fractal, extendedAux[sequence]);
					break;
				}
				case mandelbulb2:
				{
					Mandelbulb2Iteration(z, extendedAux[sequence]);
					break;
				}
				case mandelbulb3:
				{
					Mandelbulb3Iteration(z, extendedAux[sequence]);
					break;
				}
				case mandelbulb4:
				{
					Mandelbulb4Iteration(z, fractal, extendedAux[sequence]);
					break;
				}
				case fast_mandelbulb_power2:
				{
					MandelbulbPower2Iteration(z, extendedAux[sequence]);
					break;
				}
				case xenodreambuie:
				{
					XenodreambuieIteration(z, fractal, extendedAux[sequence]);
					break;
				}
				case mandelbox:
				{
					MandelboxIteration(z, fractal, extendedAux[sequence]);
					break;
				}
				case mandelboxVaryScale4D:
				{
					MandelboxVaryScale4DIteration(z, w, fractal, extendedAux[sequence]);
					break;
				}
				case smoothMandelbox:
				{
					SmoothMandelboxIteration(z, fractal, extendedAux[sequence]);
					break;
				}
				case boxFoldBulbPow2:
				{
					BoxFoldBulbPow2Iteration(z, fractal);
					break;
				}
				case menger_sponge:
				{
					MengerSpongeIteration(z, extendedAux[sequence]);
					break;
				}
				case kaleidoscopicIFS:
				{
					KaleidoscopicIFSIteration(z, fractal, extendedAux[sequence]);
					break;
				}
				case aexion:
				{
					AexionIteration(z, w, i, fractal, extendedAux[sequence]);
					break;
				}
				case hypercomplex:
				{
					HypercomplexIteration(z, w, extendedAux[sequence]);
					break;
				}
				case quaternion:
				{
					QuaternionIteration(z, w, extendedAux[sequence]);
					break;
				}
				case benesi:
				{
					BenesiIteration(z, c, extendedAux[sequence]);
					break;
				}
				case bristorbrot:
				{
					BristorbrotIteration(z, extendedAux[sequence]);
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
					BuffaloIteration(z, fractal, extendedAux[sequence]);
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
          MsltoeSym2Iteration(z, fractal, extendedAux[sequence]);
					break;
				}
        case msltoesym3:
        {
          MsltoeSym3Iteration(z, fractal, extendedAux[sequence]);
          break;
        }
        case msltoesym4:
        {
          MsltoeSym4Iteration(z, fractal, extendedAux[sequence]);
          break;
        }
				case generalizedFoldBox:
				{
					GeneralizedFoldBoxIteration(z, fractal, extendedAux[sequence]);
					break;
				}
				case mandelbulb5:
				{
					Mandelbulb5Iteration(z, c, minimumR, i, fractal, extendedAux[sequence]);
					break;
				}
				case mandelbox103:
				{
					Mandelbox103Iteration(z, c, minimumR, i, fractal, extendedAux[sequence]);
					break;
				}
				case quaternion104:
				{
					CVector4 z4D(z, w);
					Quaternion104Iteration(z4D, CVector4(c, 0.0), i, fractal, extendedAux[sequence]);
					z = z4D.GetXYZ();
					w = z4D.w;
					break;
				}
				case mengerSponge105:
				{
					MengerSponge105Iteration(z, c, minimumR, i, fractal, extendedAux[sequence]);
					break;
				}
				case mandelbulb6Beta:
				{
					Mandelbulb6BetaIteration(z, c, minimumR, i, fractal, extendedAux[sequence]);
					break;
				}
				case benesiTransforms:
				{
					BenesiTransformsIteration(z, c, minimumR, i, fractal, extendedAux[sequence]);
					break;
				}
        case fabsBoxMod:
        {
          FabsBoxModIteration(z, c, i, fractal, extendedAux[sequence]);
          break;
        }
        case aboxModKali:
        {
          AboxModKaliIteration(z, fractal, extendedAux[sequence]);
          break;
        }
        case mengerMod:
        {
          MengerModIteration(z, fractal, extendedAux[sequence]);
          break;
        }
        case quaternion3D:
        {
          Quaternion3DIteration(z, fractal);
          break;
        }
        case aexionOctopus:
        {
          AexionOctopusIteration(z, fractal);
          break;
        }
        case aboxMod1:
        {
          AboxMod1Iteration(z, fractal, extendedAux[sequence]);
          break;
        }
        case mandelbulbMulti:
        {
          MandelbulbMultiIteration(z, fractal, extendedAux[sequence]);
          break;
        }
        case benesiPineTree:
        {
          BenesiPineTreeIteration(z, c, fractal, extendedAux[sequence]);
          break;
        }
        case benesiT1PineTree:
        {
          BenesiT1PineTreeIteration(z, c, i, fractal, extendedAux[sequence]);
          break;
        }
        case eiffieMsltoe:
        {
        	EiffieMsltoeIteration(z, fractal, extendedAux[sequence]);
          break;
        }
        case riemannSphereMsltoe:
        {
          RiemannSphereMsltoeIteration(z, fractal);
          break;
        }
        case riemannSphereMsltoeV1:
        {
          RiemannSphereMsltoeV1Iteration(z, fractal);
          break;
        }


        //transforms ------------------------------------------------------------------------------------------
        case transfAdditionConstant:
        {
        	TransformAdditionConstantIteration(z, fractal);
        	break;
        }
        case transfAddCpixel:
        {
          TransformAddCpixelIteration(z, c, fractal);
          break;
        }
        case transfAddCpixelAxisSwap:
        {
          TransformAddCpixelAxisSwapIteration(z, c, fractal);
          break;
        }
        case transfAddCpixelPosNeg:
        {
          TransformAddCpixelPosNegIteration(z, c, fractal);
          break;
        }
        case transfBenesiT1:
        {
          TransformBenesiT1Iteration(z, fractal, extendedAux[sequence]);
          break;
        }
        case transfBenesiT1Mod:
        {
          TransformBenesiT1ModIteration(z, fractal, extendedAux[sequence]);
          break;
        }
        case transfBenesiT2:
        {
          TransformBenesiT2Iteration(z, fractal, extendedAux[sequence]);
          break;
        }
        case transfBenesiT3:
        {
          TransformBenesiT3Iteration(z, fractal);
          break;
        }
        case transfBenesiT4:
        {
          TransformBenesiT4Iteration(z, fractal);
          break;
        }
        case transfBenesiT5b:
        {
          TransformBenesiT5bIteration(z, fractal);
          break;
        }
        case transfBenesiMagForward:
        {
          TransformBenesiMagForwardIteration(z);
          break;
        }
        case transfBenesiMagBackward:
        {
          TransformBenesiMagBackwardIteration(z);
          break;
        }
        case transfBenesiCubeSphere:
        {
          TransformBenesiCubeSphereIteration(z);
          break;
        }
        case transfBenesiSphereCube:
        {
        TransformBenesiSphereCubeIteration(z);
        break;
        }
        case transfBoxOffset:
        {
          TransformBoxOffsetIteration(z, fractal, extendedAux[sequence]);
          break;
        }
          case transfRotation:
        {
        	TransformRotationIteration(z, fractal);
        	break;
        }
        case transfScale:
        {
        	TransformScaleIteration(z, fractal, extendedAux[sequence]);
        	break;
        }
        case transfScale3D:
        {
        	TransformScale3DIteration(z, fractal, extendedAux[sequence]);
        	break;
        }
				case platonicSolid:
				{
					TransformPlatonicSolidIteration(z, fractal);
					break;
				}

        case transfSphericalOffset:
        {
          TransformSphericalOffsetIteration(z, fractal, extendedAux[sequence]);
          break;
        }


				default:
					z = CVector3(0.0, 0.0, 0.0);
					break;
			}
		}

		//addition of constant
		if (!fractals.IsDontAddCContant(sequence))
		{
			switch (fractal->formula)
			{
				case menger_sponge:
				case kaleidoscopicIFS:
				case aexion:
        case msltoesym2:
        case msltoesym3:
        case msltoesym4:
				case mandelbulb5:
				case mandelbox103:
				case quaternion104:
				case mengerSponge105:
				case mandelbulb6Beta:
				case benesiTransforms:
				case fabsBoxMod:
				case mengerMod:
				case aexionOctopus:
        case benesiPineTree:
        case benesiT1PineTree:
        case eiffieMsltoe:
        case riemannSphereMsltoe:
        case riemannSphereMsltoeV1:
        case transfAdditionConstant:
        case transfAddCpixel:
        case transfAddCpixelAxisSwap:
        case transfAddCpixelPosNeg:
        case transfBenesiT1:
        case transfBenesiT1Mod:
        case transfBenesiT2:
        case transfBenesiT3:
        case transfBenesiT4:
        case transfBenesiT5b:
        case transfBenesiMagForward:
        case transfBenesiMagBackward:
        case transfBenesiCubeSphere:
        case transfBenesiSphereCube:
        case transfBoxOffset:
				case transfRotation:
				case transfScale:
				case transfScale3D:
        case transfSphericalOffset:
				{
					break;
				}
        case aboxMod1:
        {
					if (in.common.juliaMode)
					{
						z += CVector3(in.common.juliaC.y, in.common.juliaC.x, in.common.juliaC.z);
					}
					else
					{
	          z += CVector3(c.y, c.x, c.z) * in.common.constantMultiplier;
					}
					break;
        }

				default:
				{
					if (in.common.juliaMode)
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
		}

		if (fractals.IsHybrid())
		{
			z = SmoothCVector(tempZ, z, fractals.GetWeight(sequence));
		}

		//r calculation
		r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z + w * w);

		//escape conditions
		if (fractals.IsCheckForBaiout(sequence))
		{
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
				if (i == in.maxN) break;
			}
			else if (Mode == calcModeColouring)
			{
				if (fractal->formula != mandelbox)
				{
					if (r < minimumR) minimumR = r;
				}
				if (r > 1e15) break;
			}
			else if (Mode == calcModeOrbitTrap)
			{
				CVector3 delta = z - in.common.fakeLightsOrbitTrap;
				double distance = delta.Length();
				if (i >= in.common.fakeLightsMinIter && i <= in.common.fakeLightsMaxIter) orbitTrapTotal +=
						(1.0f / (distance * distance));
				if (distance > 1000)
				{
					out->orbitTrapR = orbitTrapTotal;
					break;
				}
			}
		}
	}

	//final calculations
	if (Mode == calcModeNormal)
	{
		switch (defaultFractal->formula)
		{
			case benesi:
			case benesiPineTree:
			case benesiT1PineTree:
			case bristorbrot:
			case buffalo:
			case eiffieMsltoe:
			case fast_mandelbulb_power2:
			case hypercomplex:
			case mandelbulb:
			case mandelbulb2:
			case mandelbulb3:
			case mandelbulb4:
			case mandelbulb5:
			case mandelbulb6Beta:
			case mandelbulbMulti:
			case msltoesym2:
			case msltoesym3:
			case msltoesym4:
			case quaternion:
			case xenodreambuie:

				out->distance = 0.5 * r * log(r) / extendedAux[sequence].r_dz;
				break;

			case mandelbox:
			case smoothMandelbox:
			case mandelboxVaryScale4D:
			case generalizedFoldBox:
			case mandelbox103:
			case mengerSponge105:
      case fabsBoxMod:
      case aboxModKali:
      case mengerMod:
      case aboxMod1:

				out->distance = r / fabs(extendedAux[sequence].DE * foldDE);
				break;

			case kaleidoscopicIFS:
			case menger_sponge:
				out->distance = (r - 2.0) / (extendedAux[sequence].DE * foldDE);
				break;

			default:
				out->distance = -1.0;
				break;
		}
	}
	//color calculation
	else if (Mode == calcModeColouring)
	{
		if (fractals.IsHybrid())
		{
			if (minimumR > 100) minimumR = 100;

			double mboxColor = 0.0;
			double mboxDE = 1.0;
			for (int h = minFractal; h <= maxFractal; h++)
			{
				mboxColor += extendedAux[h].color;
				mboxDE *= extendedAux[h].DE; //mboxDE *= mandelboxAux[h].mboxDE + extendedAux[h].color;
			}

			double r2 = r / fabs(mboxDE);
			if (r2 > 20) r2 = 20;

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
					out->colorIndex = extendedAux[fractalIndex].color * 100.0
							+ r * defaultFractal->mandelbox.color.factorR * foldColor;
					break;

        case fabsBoxMod:
            out->colorIndex = extendedAux[fractalIndex].color * 100.0 + r * defaultFractal->mandelbox.color.factorR * foldColor;
            break;


					//case mandelbox103:
					//out->colorIndex = extendedAux[fractalIndex].color * 100.0 + r * defaultFractal->mandelbox.color.factorR * foldColor;
					//	break;

					//case mengerSponge105:
					//out->colorIndex = extendedAux[fractalIndex].color * 100.0 + r * defaultFractal->mandelbox.color.factorR * foldColor + minimumR * 1000.0;;
					//	break;

					//case mandelbulb5:
					//	out->colorIndex = extendedAux[fractalIndex].color * 100.0 + r * defaultFractal->mandelbox.color.factorR * foldColor + minimumR * 5000.0;
					//	break;

				case mandelbulb5:
				case mandelbox103:
				case mandelbulb6Beta:
				case benesiTransforms:
				case mengerSponge105:
					out->colorIndex = extendedAux[fractalIndex].newR;
					break;

        case mengerMod:
        case aboxModKali:
        case aboxMod1:
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

template void Compute<calcModeNormal>(const cNineFractals &fractals, const sFractalIn &in,
		sFractalOut *out);
template void Compute<calcModeDeltaDE1>(const cNineFractals &fractals, const sFractalIn &in,
		sFractalOut *out);
template void Compute<calcModeDeltaDE2>(const cNineFractals &fractals, const sFractalIn &in,
		sFractalOut *out);
template void Compute<calcModeColouring>(const cNineFractals &fractals, const sFractalIn &in,
		sFractalOut *out);
template void Compute<calcModeOrbitTrap>(const cNineFractals &fractals, const sFractalIn &in,
		sFractalOut *out);
