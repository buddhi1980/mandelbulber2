/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * GeneralizedFoldBoxIteration - Quaternion fractal with extended controls
 * @reference http://www.fractalforums.com/new-theories-and-research/generalized-box-fold/
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalGeneralizedFoldBox::cFractalGeneralizedFoldBox() : cAbstractFractal()
{
	nameInComboBox = "Generalized Fold Box";
	internalName = "generalized_fold_box";
	internalID = fractal::generalizedFoldBox;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalGeneralizedFoldBox::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector3 zXYZ = z.GetXYZ();
	int i;
	const CVector3 *Nv;
	int sides;

	switch (fractal->genFoldBox.type)
	{
		default:
		case generalizedFoldBoxType_foldTet:
			Nv = fractal->genFoldBox.Nv_tet;
			sides = fractal->genFoldBox.sides_tet;
			break;
		case generalizedFoldBoxType_foldCube:
			Nv = fractal->genFoldBox.Nv_cube;
			sides = fractal->genFoldBox.sides_cube;
			break;
		case generalizedFoldBoxType_foldOct:
			Nv = fractal->genFoldBox.Nv_oct;
			sides = fractal->genFoldBox.sides_oct;
			break;
		case generalizedFoldBoxType_foldDodeca:
			Nv = fractal->genFoldBox.Nv_dodeca;
			sides = fractal->genFoldBox.sides_dodeca;
			break;
		case generalizedFoldBoxType_foldOctCube:
			Nv = fractal->genFoldBox.Nv_oct_cube;
			sides = fractal->genFoldBox.sides_oct_cube;
			break;
		case generalizedFoldBoxType_foldIcosa:
			Nv = fractal->genFoldBox.Nv_icosa;
			sides = fractal->genFoldBox.sides_icosa;
			break;
		case generalizedFoldBoxType_foldBox6:
			Nv = fractal->genFoldBox.Nv_box6;
			sides = fractal->genFoldBox.sides_box6;
			break;
		case generalizedFoldBoxType_foldBox5:
			Nv = fractal->genFoldBox.Nv_box5;
			sides = fractal->genFoldBox.sides_box5;
			break;
	}

	double melt = fractal->mandelbox.melt;
	double solid = fractal->mandelbox.solid;

	// Find the closest cutting plane if any that cuts the line between the origin and z.
	// Line is parameterized as X = Y + L*a;
	// Cutting plane is X.Dot(Nv) = Solid.
	// (Y + L*a).Dot(Nv) = solid.
	// a = (solid - Y.Dot(Nv))/L.Dot(Nv) = b/c
	CVector3 L = zXYZ;
	double a = 1.0;
	CVector3 Y; // Y is the origin in this case.
	int side = -1;
	double b, c;

	for (i = 0; i < sides; i++)
	{
		b = solid;
		c = L.Dot(Nv[i]);
		// A bit subtle here. a_r must be positive and I want to avoid divide by zero.
		if ((c > 0.0) && ((a * c) > b))
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
		CVector3 X_m = zXYZ - Nv_m * (zXYZ.Dot(Nv_m) - solid);

		// Find any plane (Nv_r) closest to X_m that cuts the line between Nv_m and X_m.
		// Nv_m cross Nv_r will define a possible rotation axis.
		// a = (solid - Y.Dot(Nv)/L.Dot(Nv) = b/c.
		L = X_m - Nv_m;
		Y = Nv_m;
		a = 1.0;
		side = -1;

		for (i = 0; i < sides; i++)
		{
			if (i != side_m)
			{
				b = solid - Y.Dot(Nv[i]);
				c = L.Dot(Nv[i]);
				// A bit subtle here. a_r must be positive and I want to avoid divide by zero.
				if ((c > 0.0) && ((a * c) > b))
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
			// The closest point between z and the line of rotation can be found by minimizing
			// the square of the distance (D) between z and the line
			// X = Xmr_intersect + L_r * a_rmin.
			// Setting dD/da_rmin equal to zero and solving for a_rmin.
			double a_rmin = (zXYZ.Dot(L_r) - Xmr_intersect.Dot(L_r)) / (L_r.Dot(L_r));

			// force a_rmin to be positive. I think I made an even number of sign errors here.
			if (a_rmin < 0.0)
			{
				a_rmin = -a_rmin;
				L_r = L_r * (-1.0);
			}
			CVector3 X_r = Xmr_intersect + L_r * a_rmin;

			// Find any plane (Nv_i) closest to Xmr_intersect that cuts the line between
			// Xmr_intersect and X_r. This will define a possible inversion point.
			// a = (solid - Y.Dot(Nv)/L.Dot(Nv) = b/c.
			L = X_r - Xmr_intersect;
			Y = Xmr_intersect;
			a = 1.0;
			side = -1;

			for (i = 0; i < sides; i++)
			{
				if ((i != side_m) && (i != side_r))
				{
					b = solid - Y.Dot(Nv[i]);
					c = L.Dot(Nv[i]);
					// A bit subtle here. a must be positive and I want to avoid divide by zero.
					if ((c > 0.0) && ((a * c) > b))
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
				CVector3 z2X = X_i - zXYZ;
				// Is z above the melt layer.
				if (z2X.Dot(z2X) > (melt * melt))
				{
					double z2X_mag = z2X.Length();
					zXYZ += z2X * (2.0 * (z2X_mag - melt) / (z2X_mag + .00000001));
					aux.color += fractal->mandelbox.color.factor.z;
				}
			}
			else
			{
				// Only rotation line possible but still need to check for melt.
				// Is z above the melt layer.
				CVector3 z2X = X_r - zXYZ;
				if (z2X.Dot(z2X) > (melt * melt))
				{
					double z2X_mag = z2X.Length();
					zXYZ += z2X * (2.0 * (z2X_mag - melt) / (z2X_mag + .00000001));
					aux.color += fractal->mandelbox.color.factor.y;
				}
			}
		}
		else
		{
			// Only mirror plane possible but still need to check for melt.
			CVector3 z2X = X_m - zXYZ;
			if (z2X.Dot(z2X) > (melt * melt))
			{
				double z2X_mag = z2X.Length();
				zXYZ += z2X * (2.0 * (z2X_mag - melt) / (z2X_mag + .00000001));
				aux.color += fractal->mandelbox.color.factor.x;
			}
		}
	} // outside solid

	double r2 = zXYZ.Dot(zXYZ);

	z = CVector4(zXYZ, z.w);

	z += fractal->mandelbox.offset;

	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}

	z -= fractal->mandelbox.offset;

	if (fractal->mandelbox.mainRotationEnabled)
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	z *= fractal->mandelbox.scale;
	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
}
