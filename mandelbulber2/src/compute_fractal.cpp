/*
 * compute_fractal.hpp
 *
 *  Created on: Apr 26, 2014
 *      Author: krzysztof
 */

#include "compute_fractal.hpp"
#include "fractal_formulas.hpp"


//temporary functions for performance profiling
long int perf = 0;
int perfCount = 0;
inline unsigned long int rdtsc()
{
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (unsigned long int)ts.tv_sec * 1000000000LL + (unsigned long int)ts.tv_nsec;
}

template<fractal::enumCalculationMode Mode>
void Compute(const cFourFractals &four, const sFractalIn &in, sFractalOut *out)
{
	//QTextStream outStream(stdout);
	//clock_t tim;
	//tim = rdtsc();

	CVector3 z = in.point;
	double r = z.Length();
	CVector3 c = z;
	double minimumR = 1e20;

	sMandelbulbAux bulbAux;
	bulbAux.r_dz = 1.0;
	bulbAux.r = r;

	out->maxiter = true;

	const cFractal *defaultFractal = four.GetFractal(0);
	//------------------- 86.7 ns for preparation ---------------

	double power = defaultFractal->bulb.power;

	//main iteration loop
	int i;
	for (i = 0; i < in.maxN; i++)
	{

		//here will be hybrid fractal sequence. Now it uses only formula #0
		const cFractal *fractal = four.GetFractal(0);

		//calls for fractal formulas
		switch (fractal->formula)
		{
			case fractal::mandelbulb:
			{
				bulbAux.r = r;
				MandelbulbIteration(z, fractal, bulbAux);
				//-------------- 1456ns for MandelbulbIteration (when in function) -----------------
				//-------------- 1410ns for MandelbulbIteration (when directly) -----------------

				break;
			}
			default:
				z = CVector3(0.0, 0.0, 0.0);
				break;
		}

		z += c;

		//length of z vector
		r = z.Length();

		//escape conditions
		if (Mode == fractal::normal)
		{
			if (r > 1e2)
			{
				out->maxiter = false;
				break;
			}
		}
		else if (Mode == fractal::deltaDE1)
		{
			if (r > 1e10)
				out->maxiter = false;
				break;
		}
		else if (Mode == fractal::deltaDE2)
		{
			if (i == in.maxN)
				break;
		}
		else if (Mode == fractal::colouring)
		{
			if (r < minimumR) minimumR = r;
			if (r > 1e15)
				break;
		}
		//---------------- 1680 ns for iteration -------------

	}

	//final calculations
	if(Mode == fractal::normal)
	{
		switch (defaultFractal->formula)
		{
			case fractal::mandelbulb:
				out->distance = 0.5 * r * log(r) / bulbAux.r_dz;
				break;

			default:
				out->distance = -1.0;
				break;
		}
	}
	//color calculation
	else if(Mode == fractal::colouring)
	{
		switch (defaultFractal->formula)
		{
			case fractal::mandelbulb:
				out->colorIndex = minimumR * 5000.0;
				break;

			default:
				out->colorIndex = 0.0;
				break;
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

template void Compute<fractal::normal>(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);
template void Compute<fractal::deltaDE1>(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);
template void Compute<fractal::deltaDE2>(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);
template void Compute<fractal::colouring>(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);


//========================= OLD ========================

/**
 * Compute the fractal at the point, in one of the various modes
 *
 * Mode: normal: Returns distance
 *		 fake_ao: Returns minimum radius
 *		 colouring: Returns colour index
 *		 delta_DE1, delta_DE2: Returns radius
 */

/*
template<int Mode>
double Compute(CVector3 z, const cFractal &par, int *iter_count)
{

	int L;
	double distance = 0;

	double w = 0;
	double constantw = 0;

	CVector3 dz(1.0, 0.0, 0.0);
	CVector3 one(1.0, 0.0, 0.0);
	double r_dz = 1;
	double ph_dz = 0;
	double th_dz = 0;
	double p = par.power; //mandelbulb power
	int N = par.N;

	CVector3 constant;

	double fixedRadius = par.mandelbox.foldingSphericalFixed;
	double fR2 = fixedRadius * fixedRadius;
	double minRadius = par.mandelbox.foldingSphericalMin;
	double mR2 = minRadius * minRadius;
	double tglad_factor1 = fR2 / mR2;

	double tgladDE = 1.0;
	double orbitTrapTotal = 0.0;
	double scale = par.mandelbox.scale;

	fractal::enumFractalFormula actualFormula = par.formula;
	if (actualFormula == fractal::kaleidoscopic || actualFormula == fractal::menger_sponge)
		tgladDE = 1.0;

	double tgladColor = 1.0;

	if (par.juliaMode)
	{
		constant = par.julia;
	}
	else
	{
		constant = z * par.constantFactor;
	}

	bool hybridEnabled = false;
	if (actualFormula == fractal::hybrid) hybridEnabled = true;

	double r = z.Length();

	double min = 1e200;
	for (L = 0; L < N; L++)
	{
		if (hybridEnabled)
		{
			int tempL = L;
			if(tempL > (int)par.formulaSequence.size()-1) tempL = (int)par.formulaSequence.size()-1;
			actualFormula = par.formulaSequence[tempL];
			p = par.hybridPowerSequence[tempL];
			scale = p;
		}

		if (par.IFS.foldingMode)
		{
			if (par.IFS.absX) z.x = fabs(z.x);
			if (par.IFS.absY) z.y = fabs(z.y);
			if (par.IFS.absZ) z.z = fabs(z.z);

			for (int i = 0; i < IFS_VECTOR_COUNT; i++)
			{
				if (par.IFS.enabled[i])
				{
					z = par.IFS.rot[i].RotateVector(z);
					double length = z.Dot(par.IFS.direction[i]);

					if (length < par.IFS.distance[i])
					{
						z -= par.IFS.direction[i] * 2.0 * (length - par.IFS.distance[i]);
					}

				}
			}

			z = par.IFS.mainRot.RotateVector(z - par.IFS.offset) + par.IFS.offset;
			z *= par.IFS.scale;
			z -= par.IFS.offset * (par.IFS.scale - 1.0);

			r = z.Length();
		}

		if (par.tgladFoldingMode)
		{
			if (z.x > par.foldingLimit)
			{
				z.x = par.foldingValue - z.x;
				tgladColor *= 0.9;
			}
			else if (z.x < -par.foldingLimit)
			{
				z.x = -par.foldingValue - z.x;
				tgladColor *= 0.9;
			}
			if (z.y > par.foldingLimit)
			{
				z.y = par.foldingValue - z.y;
				tgladColor *= 0.9;
			}
			else if (z.y < -par.foldingLimit)
			{
				z.y = -par.foldingValue - z.y;
				tgladColor *= 0.9;
			}
			if (z.z > par.foldingLimit)
			{
				z.z = par.foldingValue - z.z;
				tgladColor *= 0.9;
			}
			else if (z.z < -par.foldingLimit)
			{
				z.z = -par.foldingValue - z.z;
				tgladColor *= 0.9;
			}
			r = z.Length();
		}

		if (par.sphericalFoldingMode)
		{
			double fR2_2 = par.foldingSphericalFixed * par.foldingSphericalFixed;
			double mR2_2 = par.foldingSphericalMin * par.foldingSphericalMin;
			double r2_2 = r * r;
			double tglad_factor1_2 = fR2_2 / mR2_2;

			if (r2_2 < mR2_2)
			{
				z = z * tglad_factor1_2;
				tgladDE *= tglad_factor1_2;
				tgladColor += 1;
			}
			else if (r2_2 < fR2_2)
			{
				double tglad_factor2_2 = fR2_2 / r2_2;
				z = z * tglad_factor2_2;
				tgladDE *= tglad_factor2_2;
				tgladColor += 10;
			}
			r = z.Length();
		}

		switch (actualFormula)
		{
			case fractal::trig_DE:
			{
				double r1 = pow(r, p - 1);
				double r2 = r1 * r;
				double th = z.GetAlfa();
				double ph = -z.GetBeta();
				if (Mode == 0)
				{
					double p_r1_rdz = p * r1 * r_dz;
					double ph_phdz = (p - 1.0) * ph + ph_dz;
					double th_thdz = (p - 1.0) * th + th_dz;
					CVector3 rot(th_thdz, ph_phdz);
					dz = rot * p_r1_rdz + one;
					r_dz = dz.Length();
					th_dz = dz.GetAlfa();
					ph_dz = -dz.GetBeta();
				}
				CVector3 rot(p * th, p * ph);
				z = rot * r2 + constant;
				r = z.Length();
				break;
			}
			case fractal::trig_optim:
			{
				//optimisation based on: http://www.fractalforums.com/mandelbulb-implementation/realtime-renderingoptimisations/
				double th0 = asin(z.z / r);
				double ph0 = atan2(z.y, z.x);
				double rp = pow(r, p - 1.0);
				double th = th0 * p;
				double ph = ph0 * p;
				double cth = cos(th);
				r_dz = rp * r_dz * p + 1.0;
				rp *= r;
				z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp + constant;
				r = z.Length();
				break;
			}
			case fractal::mandelbulb2:
			{
				double temp, tempR;
				tempR = sqrt(z.x * z.x + z.y * z.y);
				z *= (1.0 / tempR);
				temp = z.x * z.x - z.y * z.y;
				z.y = 2.0 * z.x * z.y;
				z.x = temp;
				z *= tempR;

				tempR = sqrt(z.y * z.y + z.z * z.z);
				z *= (1.0 / tempR);
				temp = z.y * z.y - z.z * z.z;
				z.z = 2.0 * z.y * z.z;
				z.y = temp;
				z *= tempR;

				tempR = sqrt(z.x * z.x + z.z * z.z);
				z *= (1.0 / tempR);
				temp = z.x * z.x - z.z * z.z;
				z.z = 2.0 * z.x * z.z;
				z.x = temp;
				z *= tempR;

				z = z * r;
				z += constant;
				r = z.Length();
				break;
			}
			case fractal::mandelbulb3:
			{
				double temp, tempR;

				double sign = 1.0;
				double sign2 = 1.0;

				if (z.x < 0) sign2 = -1.0;
				tempR = sqrt(z.x * z.x + z.y * z.y);
				z *= (1.0 / tempR);
				temp = z.x * z.x - z.y * z.y;
				z.y = 2.0 * z.x * z.y;
				z.x = temp;
				z *= tempR;

				if (z.x < 0) sign = -1.0;
				tempR = sqrt(z.x * z.x + z.z * z.z);
				z *= (1.0 / tempR);
				temp = z.x * z.x - z.z * z.z;
				z.z = 2.0 * z.x * z.z * sign2;
				z.x = temp * sign;
				z *= tempR;

				z = z * r;
				z += constant;
				r = z.Length();
				break;
			}
			case fractal::mandelbulb4:
			{
				double rp = pow(r, p - 1);

				double angZ = atan2(z.y, z.x);
				double angY = atan2(z.z, z.x);
				double angX = atan2(z.z, z.y);

				CRotationMatrix rotM;
				rotM.RotateX(angX * (p - 1));
				rotM.RotateY(angY * (p - 1));
				rotM.RotateZ(angZ * (p - 1));

				z = rotM.RotateVector(z) * rp + constant;
				r = z.Length();
				break;
			}
			case fractal::xenodreambuie:
			{
				double rp = pow(r, p);
				double th = atan2(z.y, z.x);
				double ph = acos(z.z / r);
				if (ph > 0.5 * M_PI)
				{
					ph = M_PI - ph;
				}
				else if (ph < -0.5 * M_PI)
				{
					ph = -M_PI - ph;
				}
				z.x = rp * cos(th * p) * sin(ph * p);
				z.y = rp * sin(th * p) * sin(ph * p);
				z.z = rp * cos(ph * p);
				z = z + constant;

				r = z.Length();
				break;
			}
			case fractal::fast_trig:
			{
				double x2 = z.x * z.x;
				double y2 = z.y * z.y;
				double z2 = z.z * z.z;
				double temp = 1.0 - z2 / (x2 + y2);
				double newx = (x2 - y2) * temp;
				double newy = 2.0 * z.x * z.y * temp;
				double newz = -2.0 * z.z * sqrt(x2 + y2);
				z.x = newx + constant.x;
				z.y = newy + constant.y;
				z.z = newz + constant.z;
				r = z.Length();
				break;
			}
			case fractal::minus_fast_trig:
			{
				double x2 = z.x * z.x;
				double y2 = z.y * z.y;
				double z2 = z.z * z.z;
				double temp = 1.0 - z2 / (x2 + y2);
				double newx = (x2 - y2) * temp;
				double newy = 2.0 * z.x * z.y * temp;
				double newz = 2.0 * z.z * sqrt(x2 + y2);
				z.x = newx + constant.x;
				z.y = newy + constant.y;
				z.z = newz + constant.z;
				r = z.Length();
				break;
			}
			case fractal::hypercomplex:
			{
				CVector3 newz(z.x * z.x - z.y * z.y - z.z * z.z - w * w, 2.0 * z.x * z.y - 2.0 * w * z.z, 2.0 * z.x * z.z - 2.0 * z.y * w);
				double neww = 2.0 * z.x * w - 2.0 * z.y * z.z;
				z = newz + constant;
				w = neww;
				r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z + w * w);
				break;
			}
			case fractal::quaternion:
			{
				CVector3 newz(z.x * z.x - z.y * z.y - z.z * z.z - w * w, 2.0 * z.x * z.y, 2.0 * z.x * z.z);
				double neww = 2.0 * z.x * w;
				z = newz + constant;
				w = neww;
				r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z + w * w);
				break;
			}
			case fractal::menger_sponge:
			{
				double temp;
				z.x = fabs(z.x);
				z.y = fabs(z.y);
				z.z = fabs(z.z);
				if (z.x - z.y < 0)
				{
					temp = z.y;
					z.y = z.x;
					z.x = temp;
				}
				if (z.x - z.z < 0)
				{
					temp = z.z;
					z.z = z.x;
					z.x = temp;
				}
				if (z.y - z.z < 0)
				{
					temp = z.z;
					z.z = z.y;
					z.y = temp;
				}

				if (Mode == fractal::colouring)
				{
					double length2 = z.Length();
					if (length2 < min) min = length2;
				}

				z *= 3.0;

				z.x -= 2.0;
				z.y -= 2.0;
				if (z.z > 1.0) z.z -= 2.0;
				r = z.Length();
				tgladDE *= 3.0;
				break;
			}
			case fractal::tglad:
			{
				if (par.mandelbox.rotationsEnabled)
				{
					bool lockout = false;
					z = par.mandelbox.rot[0][0].RotateVector(z);
					if (z.x > par.mandelbox.foldingLimit)
					{
						z.x = par.mandelbox.foldingValue - z.x;
						tgladColor += par.mandelbox.colorFactorX;
						lockout = true;
					}
					z = par.mandelbox.rotinv[0][0].RotateVector(z);

					z = par.mandelbox.rot[1][0].RotateVector(z);
					if (!lockout && z.x < -par.mandelbox.foldingLimit)
					{
						z.x = -par.mandelbox.foldingValue - z.x;
						tgladColor += par.mandelbox.colorFactorX;
					}
					z = par.mandelbox.rotinv[1][0].RotateVector(z);

					lockout = false;
					z = par.mandelbox.rot[0][1].RotateVector(z);
					if (z.y > par.mandelbox.foldingLimit)
					{
						z.y = par.mandelbox.foldingValue - z.y;
						tgladColor += par.mandelbox.colorFactorY;
						lockout = true;
					}
					z = par.mandelbox.rotinv[0][1].RotateVector(z);

					z = par.mandelbox.rot[1][1].RotateVector(z);
					if (!lockout && z.y < -par.mandelbox.foldingLimit)
					{
						z.y = -par.mandelbox.foldingValue - z.y;
						tgladColor += par.mandelbox.colorFactorY;
					}
					z = par.mandelbox.rotinv[1][1].RotateVector(z);

					lockout = false;
					z = par.mandelbox.rot[0][2].RotateVector(z);
					if (z.z > par.mandelbox.foldingLimit)
					{
						z.z = par.mandelbox.foldingValue - z.z;
						tgladColor += par.mandelbox.colorFactorZ;
						lockout = true;
					}
					z = par.mandelbox.rotinv[0][2].RotateVector(z);

					z = par.mandelbox.rot[1][2].RotateVector(z);
					if (!lockout && z.z < -par.mandelbox.foldingLimit)
					{
						z.z = -par.mandelbox.foldingValue - z.z;
						tgladColor += par.mandelbox.colorFactorZ;
					}
					z = par.mandelbox.rotinv[1][2].RotateVector(z);
				}
				else
				{
					if (z.x > par.mandelbox.foldingLimit)
					{
						z.x = par.mandelbox.foldingValue - z.x;
						tgladColor += par.mandelbox.colorFactorX;
					}
					else if (z.x < -par.mandelbox.foldingLimit)
					{
						z.x = -par.mandelbox.foldingValue - z.x;
						tgladColor += par.mandelbox.colorFactorX;
					}
					if (z.y > par.mandelbox.foldingLimit)
					{
						z.y = par.mandelbox.foldingValue - z.y;
						tgladColor += par.mandelbox.colorFactorY;
					}
					else if (z.y < -par.mandelbox.foldingLimit)
					{
						z.y = -par.mandelbox.foldingValue - z.y;
						tgladColor += par.mandelbox.colorFactorY;
					}
					if (z.z > par.mandelbox.foldingLimit)
					{
						z.z = par.mandelbox.foldingValue - z.z;
						tgladColor += par.mandelbox.colorFactorZ;
					}
					else if (z.z < -par.mandelbox.foldingLimit)
					{
						z.z = -par.mandelbox.foldingValue - z.z;
						tgladColor += par.mandelbox.colorFactorZ;
					}
				}

				r = z.Length();
				double r2 = r * r;

				z += par.mandelbox.offset;

				if (r2 < mR2)
				{
					z *= tglad_factor1;
					tgladDE *= tglad_factor1;
					tgladColor += par.mandelbox.colorFactorSp1;
				}
				else if (r2 < fR2)
				{
					double tglad_factor2 = fR2 / r2;
					z *= tglad_factor2;
					tgladDE *= tglad_factor2;
					tgladColor += par.mandelbox.colorFactorSp2;
				}

				z -= par.mandelbox.offset;

				z = par.mandelbox.mainRot.RotateVector(z);

				z = z * scale + constant;
				tgladDE = tgladDE*fabs(scale)+1.0;

				r = z.Length();
				break;
			}

			case fractal::generalizedFoldBox:
			{
				//Reference: http://www.fractalforums.com/new-theories-and-research/generalized-box-fold/msg36503/#msg36503

				int i;
				const CVector3 *Nv;
				int sides;

				Nv = par.genFoldBox.Nv_tet;
				sides = par.genFoldBox.sides_tet;

				if (par.genFoldBox.type == fractal::foldCube)
				{
					Nv = par.genFoldBox.Nv_cube;
					sides = par.genFoldBox.sides_cube;
				}
				else if (par.genFoldBox.type == fractal::foldOct)
				{
					Nv = par.genFoldBox.Nv_oct;
					sides = par.genFoldBox.sides_oct;
				}
				else if (par.genFoldBox.type == fractal::foldDodeca)
				{
					Nv = par.genFoldBox.Nv_dodeca;
					sides = par.genFoldBox.sides_dodeca;
				}
				else if (par.genFoldBox.type == fractal::foldOctCube)
				{
					Nv = par.genFoldBox.Nv_oct_cube;
					sides = par.genFoldBox.sides_oct_cube;
				}
				else if (par.genFoldBox.type == fractal::foldIcosa)
				{
					Nv = par.genFoldBox.Nv_icosa;
					sides = par.genFoldBox.sides_icosa;
				}
				else if (par.genFoldBox.type == fractal::foldBox6)
				{
					Nv = par.genFoldBox.Nv_box6;
					sides = par.genFoldBox.sides_box6;
				}
				else if (par.genFoldBox.type == fractal::foldBox5)
				{
					Nv = par.genFoldBox.Nv_box5;
					sides = par.genFoldBox.sides_box5;
				}

        double melt = par.mandelbox.melt;
        double solid = par.mandelbox.solid;

				// Find the closest cutting plane if any that cuts the line between the origin and z.
				// Line is parameterized as X = Y + L*a;
				// Cutting plane is X.Dot(Nv) = Solid.
				// (Y + L*a).Dot(Nv) = solid.
				// a = (solid - Y.Dot(Nv))/L.Dot(Nv) = b/c
				CVector3 L = z;
				double a = 1;
				CVector3 Y; // Y is the origin in this case.
				int side = -1;
				double b, c;

				for (i = 0; i < sides; i++)
				{
					b = solid;
					c = L.Dot(Nv[i]);
					// A bit subtile here. a_r must be positive and I want to avoid divide by zero.
					if ((c > 0) && ((a * c) > b))
					{
						side = i;
						a = b / c;
					}
				}

				// If z is above the foldingValue we may have to fold. Else early out.
				if (side != -1)
				{ // mirror check
					int side_m = side;
					CVector3 Nv_m = Nv[side_m];
					CVector3 X_m = z - Nv_m * (z.Dot(Nv_m) - solid);

					// Find any plane (Nv_r) closest to X_m that cuts the line between Nv_m and X_m.
					// Nv_m cross Nv_r will define a possible rotation axis.
					// a = (solid - Y.Dot(Nv)/L.Dot(Nv) = b/c.
					L = X_m - Nv_m;
					Y = Nv_m;
					a = 1;
					side = -1;

					for (i = 0; i < sides; i++)
					{
						if (i != side_m)
						{
							b = solid - Y.Dot(Nv[i]);
							c = L.Dot(Nv[i]);
							// A bit subtile here. a_r must be positive and I want to avoid divide by zero.
							if ((c > 0) && ((a * c) > b))
							{
								side = i;
								a = b / c;
							}
						}
					}

					// Was a cutting plane found?
					if (side != -1)
					{ // rotation check
						CVector3 Xmr_intersect = Y + L * a;
						int side_r = side;
						CVector3 Nv_r = Nv[side_r];
						// The axis of rotation is define by the cross product of Nv_m and Nv_r and
						// the intersection of the line between Nv_m and Nv_r and  Xmr_intersect.
						CVector3 L_r = Nv_m.Cross(Nv_r);
						// The closest point betwee z and the line of rotation can be found by minimizing
						// the square of the distance (D) between z and the line
						// X = Xmr_intersect + L_r * a_rmin.
						// Setting dD/da_rmin equal to zero and solving for a_rmin.
						double a_rmin = (z.Dot(L_r) - Xmr_intersect.Dot(L_r)) / (L_r.Dot(L_r));

						// force a_rmin to be positive. I think I made an even number of sign errors here.
						if (a_rmin < 0)
						{
							a_rmin = -a_rmin;
							L_r = L_r * (-1);
						}
						CVector3 X_r = Xmr_intersect + L_r * a_rmin;

						// Find any plane (Nv_i) closest to Xmr_intersect that cuts the line between
						// Xmr_intersect and X_r. This will define a possible inversion point.
						// a = (solid - Y.Dot(Nv)/L.Dot(Nv) = b/c.
						L = X_r - Xmr_intersect;
						Y = Xmr_intersect;
						a = 1;
						side = -1;

						for (i = 0; i < sides; i++)
						{
							if ((i != side_m) && (i != side_r))
							{
								b = solid - Y.Dot(Nv[i]);
								c = L.Dot(Nv[i]);
								// A bit subtile here. a must be positive and I want to avoid divide by zero.
								if ((c > 0) && ((a * c) > b))
								{
									side = i;
									a = b / c;
								}
							}
						}

						if (side != -1)
						{ // inversion check
							// Only inversion point possible but still need to check for melt.

							CVector3 X_i = Y + L * a;
							CVector3 z2X = X_i - z;
							// Is z above the melt layer.
							if (z2X.Dot(z2X) > (melt * melt))
							{
								double z2X_mag = z2X.Length();
								z = z + z2X * (2 * (z2X_mag - melt) / (z2X_mag + .00000001));
								tgladColor += par.mandelbox.colorFactorZ;
							}
						}
						else
						{
							// Only rotation line possible but still need to check for melt.
							// Is z above the melt layer.
							CVector3 z2X = X_r - z;
							if (z2X.Dot(z2X) > (melt * melt))
							{
								double z2X_mag = z2X.Length();
								z = z + z2X * (2 * (z2X_mag - melt) / (z2X_mag + .00000001));
								tgladColor += par.mandelbox.colorFactorY;
							}
						}

					}
					else
					{
						// Only mirror plane possible but still need to check for melt.
						CVector3 z2X = X_m - z;
						if (z2X.Dot(z2X) > (melt * melt))
						{
							double z2X_mag = z2X.Length();
							z = z + z2X * (2 * (z2X_mag - melt) / (z2X_mag + .00000001));
							tgladColor += par.mandelbox.colorFactorX;
						}
					}
				} // outside solid

				r = z.Length();
				double r2 = r * r;

				z += par.mandelbox.offset;

				if (r2 < mR2)
				{
					z *= tglad_factor1;
					tgladDE *= tglad_factor1;
					tgladColor += par.mandelbox.colorFactorSp1;
				}
				else if (r2 < fR2)
				{
					double tglad_factor2 = fR2 / r2;
					z *= tglad_factor2;
					tgladDE *= tglad_factor2;
					tgladColor += par.mandelbox.colorFactorSp2;
				}

				z -= par.mandelbox.offset;

				z = par.mandelbox.mainRot.RotateVector(z);

				z = z * scale + constant;
				tgladDE = tgladDE * fabs(scale) + 1.0;

				r = z.Length();
				break;
			}

			case fractal::smoothMandelbox:
			{
				double sm = par.mandelbox.sharpness;

				double zk1 = SmoothConditionAGreaterB(z.x, par.mandelbox.foldingLimit,sm);
				double zk2 = SmoothConditionALessB(z.x, -par.mandelbox.foldingLimit,sm);
				z.x = z.x * (1.0 - zk1) + (par.mandelbox.foldingValue - z.x) * zk1;
				z.x = z.x * (1.0 - zk2) + (-par.mandelbox.foldingValue - z.x) * zk2;
				tgladColor += (zk1 + zk2) * par.mandelbox.colorFactorX;

				double zk3 = SmoothConditionAGreaterB(z.y, par.mandelbox.foldingLimit,sm);
				double zk4 = SmoothConditionALessB(z.y, -par.mandelbox.foldingLimit,sm);
				z.y = z.y * (1.0 - zk3) + (par.mandelbox.foldingValue - z.y) * zk3;
				z.y = z.y * (1.0 - zk4) + (-par.mandelbox.foldingValue - z.y) * zk4;
				tgladColor += (zk3 + zk4) * par.mandelbox.colorFactorY;

				double zk5 = SmoothConditionAGreaterB(z.z, par.mandelbox.foldingLimit,sm);
				double zk6 = SmoothConditionALessB(z.z, -par.mandelbox.foldingLimit,sm);
				z.z = z.z * (1.0 - zk5) + (par.mandelbox.foldingValue - z.z) * zk5;
				z.z = z.z * (1.0 - zk6) + (-par.mandelbox.foldingValue - z.z) * zk6;
				tgladColor += (zk5 + zk6) * par.mandelbox.colorFactorZ;

				r = z.Length();
				double r2 = r * r;
				double tglad_factor2 = fR2 / r2;
				double rk1 = SmoothConditionALessB(r2, mR2, sm);
				double rk2 = SmoothConditionALessB(r2, fR2, sm);
				double rk21 = (1.0 - rk1) * rk2;

				z = z * (1.0 - rk1) + z * (tglad_factor1 * rk1);
				z = z * (1.0 - rk21) + z * (tglad_factor2 * rk21);
				tgladDE = tgladDE * (1.0 - rk1) + tgladDE * (tglad_factor1 * rk1);
				tgladDE = tgladDE * (1.0 - rk21) + tgladDE * (tglad_factor2 * rk21);
				tgladColor += rk1 * par.mandelbox.colorFactorSp1;
				tgladColor += rk21 * par.mandelbox.colorFactorSp2;

				z = par.mandelbox.mainRot.RotateVector(z);
				z = z * scale + constant;

				tgladDE = tgladDE * fabs(scale) + 1.0;
				r = z.Length();
				break;
			}
			case fractal::foldingIntPow2:
			{
				if (z.x > par.FoldingIntPowFoldFactor) z.x = par.FoldingIntPowFoldFactor * 2.0 - z.x;
				else if (z.x < -par.FoldingIntPowFoldFactor) z.x = -par.FoldingIntPowFoldFactor * 2.0 - z.x;

				if (z.y > par.FoldingIntPowFoldFactor) z.y = par.FoldingIntPowFoldFactor * 2.0 - z.y;
				else if (z.y < -par.FoldingIntPowFoldFactor) z.y = -par.FoldingIntPowFoldFactor * 2.0 - z.y;

				if (z.z > par.FoldingIntPowFoldFactor) z.z = par.FoldingIntPowFoldFactor * 2.0 - z.z;
				else if (z.z < -par.FoldingIntPowFoldFactor) z.z = -par.FoldingIntPowFoldFactor * 2.0 - z.z;

				r = z.Length();

				double fR2_2 = 1.0;
				double mR2_2 = 0.25;
				double r2_2 = r * r;
				double tglad_factor1_2 = fR2_2 / mR2_2;

				if (r2_2 < mR2_2)
				{
					z = z * tglad_factor1_2;
				}
				else if (r2_2 < fR2_2)
				{
					double tglad_factor2_2 = fR2_2 / r2_2;
					z = z * tglad_factor2_2;
				}

				z = z * 2.0;
				double x2 = z.x * z.x;
				double y2 = z.y * z.y;
				double z2 = z.z * z.z;
				double temp = 1.0 - z2 / (x2 + y2);
				double newx = (x2 - y2) * temp;
				double newy = 2.0 * z.x * z.y * temp;
				double newz = -2.0 * z.z * sqrt(x2 + y2);
				z.x = newx + constant.x;
				z.y = newy + constant.y;
				z.z = newz + constant.z;
				z.z *= par.FoldingIntPowZfactor;
				r = z.Length();
				break;
			}
			case fractal::kaleidoscopic:
			{

				if (par.IFS.absX) z.x = fabs(z.x);
				if (par.IFS.absY) z.y = fabs(z.y);
				if (par.IFS.absZ) z.z = fabs(z.z);

				for (int i = 0; i < IFS_VECTOR_COUNT; i++)
				{
					if (par.IFS.enabled[i])
					{
						z = par.IFS.rot[i].RotateVector(z);
						double length = z.Dot(par.IFS.direction[i]);

						if (length < par.IFS.distance[i])
						{
							z -= par.IFS.direction[i] * (2.0 * (length - par.IFS.distance[i]) * par.IFS.intensity[i]);
						}

					}
				}
				z = par.IFS.mainRot.RotateVector(z - par.IFS.offset) + par.IFS.offset;

				if(par.IFS.edge.x > 0) z.x = par.IFS.edge.x - fabs(par.IFS.edge.x - z.x);
				if(par.IFS.edge.y > 0) z.y = par.IFS.edge.y - fabs(par.IFS.edge.y - z.y);
				if(par.IFS.edge.z > 0) z.z = par.IFS.edge.z - fabs(par.IFS.edge.z - z.z);

				if (Mode == fractal::colouring)
				{
					double length2 = z.Length();
					if (length2 < min) min = length2;
				}

				z *= par.IFS.scale;
				if(par.IFS.mengerSpongeMode)
				{
					z.x -= par.IFS.offset.x * (par.IFS.scale - 1.0);
					z.y -= par.IFS.offset.y * (par.IFS.scale - 1.0);
					if (z.z > 0.5 * par.IFS.offset.z * (par.IFS.scale - 1.0)) z.z -= par.IFS.offset.z * (par.IFS.scale - 1.0);
				}
				else
				{
					z -= par.IFS.offset * (par.IFS.scale - 1.0);
				}

				tgladDE *= par.IFS.scale;
				r = z.Length();

				break;
			}
			case fractal::mandelboxVaryScale4D:
			{
				scale = scale + par.mandelbox.vary4D.scaleVary * (fabs(scale) - 1.0);
				CVector3 oldz = z;
				z.x = fabs(z.x + par.mandelbox.vary4D.fold) - fabs(z.x - par.mandelbox.vary4D.fold) - z.x;
				z.y = fabs(z.y + par.mandelbox.vary4D.fold) - fabs(z.y - par.mandelbox.vary4D.fold) - z.y;
				z.z = fabs(z.z + par.mandelbox.vary4D.fold) - fabs(z.z - par.mandelbox.vary4D.fold) - z.z;
				w = fabs(w + par.mandelbox.vary4D.fold) - fabs(w - par.mandelbox.vary4D.fold) - w;
				if(z.x != oldz.x) tgladColor += par.mandelbox.colorFactorX;
				if(z.y != oldz.y) tgladColor += par.mandelbox.colorFactorY;
				if(z.z != oldz.z) tgladColor += par.mandelbox.colorFactorZ;
				double rr = pow(z.x * z.x + z.y * z.y + z.z * z.z + w * w, par.mandelbox.vary4D.rPower);
				double m = scale;
				if (rr < par.mandelbox.vary4D.minR * par.mandelbox.vary4D.minR)
				{
					m = scale / (par.mandelbox.vary4D.minR * par.mandelbox.vary4D.minR);
					tgladColor += par.mandelbox.colorFactorSp1;
				}
				else if (rr < 1.0)
				{
					m = scale / rr;
					tgladColor += par.mandelbox.colorFactorSp2;
				}
				z = z * m + constant;
				w = w * m + par.mandelbox.vary4D.wadd;
				tgladDE = tgladDE * fabs(m) + 1.0;
				r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z + w * w);
				break;
			}
			case fractal::aexion:
			{
				if(L == 0)
				{
					double cx = fabs(constant.x + constant.y + constant.z) + par.cadd;
					double cy = fabs(-constant.x - constant.y + constant.z) + par.cadd;
					double cz = fabs(-constant.x + constant.y - constant.z) + par.cadd;
					double cw = fabs(constant.x - constant.y - constant.z) + par.cadd;
					constant.x = cx;
					constant.y = cy;
					constant.z = cz;
					constantw = cw;
					double tempx = fabs(z.x + z.y + z.z) + par.cadd;
					double tempy = fabs(-z.x - z.y + z.z) + par.cadd;
					double tempz = fabs(-z.x + z.y - z.z) + par.cadd;
					double tempw = fabs(z.x - z.y - z.z) + par.cadd;
					z.x = tempx;
					z.y = tempy;
					z.z = tempz;
					w = tempw;
				}
				double tempx = z.x * z.x - z.y * z.y + 2.0 * w * z.z + constant.x;
				double tempy = z.y * z.y - z.x * z.x + 2.0 * w * z.z + constant.y;
				double tempz = z.z * z.z - w * w + 2.0 * z.x * z.y + constant.z;
				double tempw = w * w - z.z * z.z + 2.0 * z.x * z.y + constantw;
				z.x = tempx;
				z.y = tempy;
				z.z = tempz;
				w = tempw;
				r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z + w * w);
				break;
			}
			case fractal::benesi:
			{
				double r1 = z.y*z.y + z.z*z.z;
				double newx = 0;
				if(constant.x < 0 || z.x < sqrt(r1))
				{
					newx = z.x*z.x - r1;
				}
				else
				{
					newx = -z.x*z.x + r1;
				}
				r1 = - 1.0/sqrt(r1) * 2.0 * fabs(z.x);
				double newy = r1 * (z.y*z.y - z.z*z.z);
				double newz = r1 * 2.0 * z.y * z.z;

				z.x = newx + constant.x;
				z.y = newy + constant.y;
				z.z = newz + constant.z;

				r = z.Length();
				break;
			}
			case fractal::bristorbrot:
			{
				double newx = z.x*z.x - z.y*z.y - z.z*z.z;
				double newy = z.y * (2.0 * z.x - z.z);
				double newz = z.z * (2.0 * z.x + z.y);

				z.x = newx + constant.x;
				z.y = newy + constant.y;
				z.z = newz + constant.z;

				r = z.Length();
				break;
			}
			case fractal::invertX:
			{
				z.x = z.x >= 0.0 ? z.x*z.x/(fabs(z.x) + p) : -z.x*z.x/(fabs(z.x) + p);
				r = z.Length();
				break;
			}
			case fractal::invertY:
			{
				z.y = z.y >= 0.0 ? z.y*z.y/(fabs(z.y) + p) : -z.y*z.y/(fabs(z.y) + p);
				r = z.Length();
				break;
			}
			case fractal::invertZ:
			{
				z.z = z.z >= 0.0 ? z.z*z.z/(fabs(z.z) + p) : -z.z*z.z/(fabs(z.z) + p);
				r = z.Length();
				break;
			}
			case fractal::invertR:
			{
				double rInv = r*r/(r + p);
				z.x = z.x / r * rInv;
				z.y = z.y / r * rInv;
				z.z = z.z / r * rInv;
				r = z.Length();
				break;
			}
			case fractal::sphericalFold:
			{
				double rr = r*r;
				double pp = p*p;
				if (rr < pp)
				{
					z.x = 1.0 / pp;
					z.y = 1.0 / pp;
					z.z = 1.0 / pp;
				}
				else if (rr < pp*4.0)
				{
					z.x = 1.0 / rr;
					z.y = 1.0 / rr;
					z.z = 1.0 / rr;
				}
				r = z.Length();
				break;
			}
			case fractal::powXYZ:
			{
				z.x = z.x >= 0 ? pow(z.x,p) : -pow(-z.x,p);
				z.y = z.y >= 0 ? pow(z.y,p) : -pow(-z.y,p);
				z.z = z.z >= 0 ? pow(z.z,p) : -pow(-z.z,p);
				r = z.Length();
				break;
			}
			case fractal::scaleX:
			{
				z.x = z.x * p;
				r = z.Length();
				break;
			}
			case fractal::scaleY:
			{
				z.y = z.y * p;
				r = z.Length();
				break;
			}
			case fractal::scaleZ:
			{
				z.z = z.z * p;
				r = z.Length();
				break;
			}
			case fractal::offsetX:
			{
				z.x = z.x + p;
				r = z.Length();
				break;
			}
			case fractal::offsetY:
			{
				z.y = z.y + p;
				r = z.Length();
				break;
			}
			case fractal::offsetZ:
			{
				z.z = z.z + p;
				r = z.Length();
				break;
			}
			case fractal::angleMultiplyX:
			{
				double angle = atan2(z.z,z.y)*p;
				double tempR = sqrt(z.z*z.z + z.y*z.y);
				z.y = tempR * cos(angle);
				z.z = tempR * sin(angle);
				r = z.Length();
				break;
			}
			case fractal::angleMultiplyY:
			{
				double angle = atan2(z.z,z.x)*p;
				double tempR = sqrt(z.z*z.z + z.x*z.x);
				z.x = tempR * cos(angle);
				z.z = tempR * sin(angle);
				r = z.Length();
				break;
			}
			case fractal::angleMultiplyZ:
			{
				double angle = atan2(z.y,z.x)*p;
				double tempR = sqrt(z.x*z.x + z.y*z.y);
				z.x = tempR * cos(angle);
				z.y = tempR * sin(angle);
				r = z.Length();
				break;
			}
			case fractal::hybrid:
				break;
			case fractal::none:
				break;
			case fractal::ocl_custom:
				break;
		}

		//************************** iteration terminate conditions *****************
		if (Mode == fractal::deltaDE1)
		{
			if (r > 1e10)
				break;
		}
		else if (Mode == fractal::deltaDE2)
		{
			if (L == *iter_count)
				break;
		}

		if (Mode == fractal::orbitTrap)
		{
			CVector3 delta = z - par.fakeLightsOrbitTrap;
			distance = delta.Length();
			if (L >= par.fakeLightsMinIter && L <= par.fakeLightsMaxIter) orbitTrapTotal += (1.0f/(distance*distance));
			if (distance > 1000)
			{
				distance = orbitTrapTotal;
				break;
			}
		}

		if (actualFormula == fractal::menger_sponge || actualFormula == fractal::kaleidoscopic)
		{
			if (r > 1000)
			{
				distance = (r - 2.0) / tgladDE;
				break;
			}
		}
		else if (actualFormula == fractal::tglad || actualFormula == fractal::smoothMandelbox || actualFormula == fractal::mandelboxVaryScale4D || actualFormula == fractal::generalizedFoldBox)
		{
			if (r > 1024)
			{
				distance = r / fabs(tgladDE);
				break;
			}
		}
		else
		{
			if (Mode == fractal::normal) //condition for all other trigonometric and hypercomplex fractals
			{
				if (r > 1e2)
				{
					distance = 0.5 * r * log(r) / r_dz;
					break;
				}
			}
			else if (Mode == fractal::fake_AO) //mode 2
			{
				if (r < min) min = r;
				if (r > 1e15)
				{
					distance = min;
					break;
				}
			}
			else if (Mode == fractal::colouring) //mode 1
			{
				if(par.primitives.onlyPlane)
				{
					distance = z.Length();
					if (distance > 1e15)
					{
						distance = (L - log(log(r) / log((double)N)) / log(p))/100.0;
						break;
					}
				}
				else
				{
					distance = z.Length();
					if (distance < min) min = distance;
					if (distance > 1e15)
					{
						distance = min;
						break;
					}
				}
			}
		}
	}

	//************ return values *****************
	//N_counter += L + 1;
	//Loop_counter++;

	//if (L < 64)
		//histogram[L]++;
	//else
		//histogram[63]++;

	if (iter_count != NULL)
		*iter_count = L;

	if (Mode == fractal::normal)
	{
		if (L == N)
			distance = 0;
		return distance;
	}

	if (Mode == fractal::deltaDE1 || Mode == fractal::deltaDE2)
		return r;

	if (Mode == fractal::fake_AO)
		return distance;

	if (Mode == fractal::orbitTrap)
		return distance;

	if (Mode == fractal::colouring)
	{
		if (par.formula == fractal::hybrid)
		{
			if (min > 100)
				min = 100;
			if (distance > 20)
				distance = 20;
			if (tgladColor > 1000)
				tgladColor = 1000;

			return distance * 5000.0 + tgladColor * 100.0 + min * 1000.0;
		}
		else if (actualFormula == fractal::tglad || actualFormula == fractal::smoothMandelbox || actualFormula == fractal::mandelboxVaryScale4D || actualFormula == fractal::generalizedFoldBox)
			return tgladColor * 100.0 + z.Length()*par.mandelbox.colorFactorR;
		else if (actualFormula == fractal::kaleidoscopic || actualFormula == fractal::menger_sponge)
			return min * 1000.0;
		else
			return distance * 5000.0;
	}
}

*/

/*

// force template instantiation
template double Compute<fractal::normal>(CVector3, const cFractal&, int*);
template double Compute<fractal::colouring>(CVector3, const cFractal&, int*);
template double Compute<fractal::fake_AO>(CVector3, const cFractal&, int*);
template double Compute<fractal::deltaDE1>(CVector3, const cFractal&, int*);
template double Compute<fractal::deltaDE2>(CVector3, const cFractal&, int*);
template double Compute<fractal::orbitTrap>(CVector3, const cFractal&, int*);
*/
