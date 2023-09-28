/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * GeneralizedFoldBoxIteration - Prototype Tinker Towers
 * @reference http://www.fractalforums.com/new-theories-and-research/tinker-towers/
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTinkerTowers::cFractalTinkerTowers() : cAbstractFractal()
{
	nameInComboBox = "Tinker Towers";
	internalName = "tinker_towers";
	internalID = fractal::tinkerTowers;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTinkerTowers::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double t = 0.0;
	CVector3 tv = CVector3(0.0, 0.0, 0.0);
	CVector3 zXYZ = z.GetXYZ();

	CVector3 u_zXYZ = CVector3(0.0, 0.0, 1.0); // angel does matter if mag_zXYZ==0
	double mag_zXYZ = 0.0;
	if((zXYZ.Dot(zXYZ)) > 0.0)
	{
		u_zXYZ = zXYZ / sqrt(zXYZ.Dot(zXYZ));
		mag_zXYZ = sqrt(zXYZ.Dot(zXYZ));
	}

	double flat = 0.0;
	int sides;
	int i;


	// By defination u_Fv are unit vectors (rays) that define the orentation of the facets that cut a unit sphere.
	// The Nv vectors need to be scaled by a number between 0 and 1, mag_Fv, in order to cut the unit sphere.
	// They don't have to be scaled by the same amount, but every facit must contain its definining u_Fv vector.

	const CVector3 *u_Fv;

	switch (fractal->genFoldBox.type)
	{
		default:
		case generalizedFoldBoxType_foldTet:
			u_Fv = fractal->genFoldBox.Nv_tet;
			sides = fractal->genFoldBox.sides_tet;
			break;
		case generalizedFoldBoxType_foldCube:
			u_Fv = fractal->genFoldBox.Nv_cube;
			sides = fractal->genFoldBox.sides_cube;
			break;
		case generalizedFoldBoxType_foldOct:
			u_Fv = fractal->genFoldBox.Nv_oct;
			sides = fractal->genFoldBox.sides_oct;
			break;
		case generalizedFoldBoxType_foldDodeca:
			u_Fv = fractal->genFoldBox.Nv_dodeca;
			sides = fractal->genFoldBox.sides_dodeca;
			break;
		case generalizedFoldBoxType_foldOctCube:
			u_Fv = fractal->genFoldBox.Nv_oct_cube;
			sides = fractal->genFoldBox.sides_oct_cube;
			break;
		case generalizedFoldBoxType_foldIcosa:
			u_Fv = fractal->genFoldBox.Nv_icosa;
			sides = fractal->genFoldBox.sides_icosa;
			break;
		case generalizedFoldBoxType_foldBox6:
			u_Fv = fractal->genFoldBox.Nv_box6;
			sides = fractal->genFoldBox.sides_box6;
			break;
		case generalizedFoldBoxType_foldBox5:
			u_Fv = fractal->genFoldBox.Nv_box5;
			sides = fractal->genFoldBox.sides_box5;
			break;
	}

	// melt is going to be used to control the u_Fv vector height.
	// The height must be between 0 and 1 to cut the unit sphere
	// The magnetude does not need to be equal, but u_Fv must not be cut from the face.
	double melt = fractal->transformCommon.offset05;
	double mag_Fv[64];
	for (i = 0; i < sides; i++)
	{
		mag_Fv[i] = melt;
	}

	//
	// Scale is used as the power.
	double power = fractal->mandelbox.scale;
	// fractal->transformCommon.functionEnabledAFalse is used for selecting fractal(disabled) or target map(enabled).
//	double solid = fractal->mandelbox.solid;


	// Find the lowest cutting plane that cuts the ray from the origin through zXYZ.
	// The parameterized equation for this ray is L_Z(h) = h * u_zXYX.
	// The plane equation is X dot u_Fv =  mag_Fv;
	// The value of h at the intersection point is h = mag_Fv/(u_zXYZ dot u_Fv)

	int side = -1; // Assume no facet found before unit sphere.
	double h = 1.0;    // so h=1.
	double my_h;
	for (i = 0; i < sides; i++)
	{
		my_h = 2; // just needs to be >1
		double u_zXYZ_dot_u_Fvi = u_zXYZ.Dot(u_Fv[i]);
		if(u_zXYZ_dot_u_Fvi > 0.0)
		{
			my_h = mag_Fv[i] / u_zXYZ_dot_u_Fvi;
		}
		if(my_h < h)
		{
			h = my_h;
			side = i;
		}
	}
	// Did we hit a cutting plane in the unit sphere.
	// If so calculate the ratio of the distance from u_Fv through the point
	// of intersection of ray (Zc) to either another cutting plane or
	// the unit sphere if there are none.
	// Zc = h * zXYZ
	double w , my_w;
	int edge = -1;
	CVector3 Zc;
	// The intersection point is Zc.
	Zc = h * u_zXYZ;

	if (side != -1)
	{
		// The parameterized formula for the line from u_Fv through Zi is
		// L_u_Fv_Zc(w) = w * unit_vector(Zc - mag_Fv * u_Fv).

		// Using dot product to estimate ratio of u_Fv[side] to Zc to NV[side] to edge of facit
		// u_Fv[side] dot u_Fv[side] = 1;
		// u_Fv[side] dot Zc = h;
		// u_Fv[side] dot unclipped circle = mag_Fv.

		tv = (Zc - mag_Fv[side] * u_Fv[side]);
		t = tv.Dot(Zc - mag_Fv[side] * u_Fv[side]); // mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
		double D_u_Fv_to_Zc = sqrt(t);
		// double D_u_Fv_to_Zc = sqrt((Zc - mag_Fv[side] * u_Fv[side]).Dot(Zc - mag_Fv[side] * u_Fv[side]));

		CVector3 u_Fv_to_Zc = (Zc - mag_Fv[side] * u_Fv[side]);
		//CVector3 u_Fv_to_Zc;
		if (D_u_Fv_to_Zc > 0.0)
		{
			u_Fv_to_Zc = u_Fv_to_Zc / D_u_Fv_to_Zc;
		}

		// Assume no cutting plane before the unit sphere.
		// Find the distancd from u_Fv to edge of sphere.
		w = sqrt(1.0 - mag_Fv[side] * mag_Fv[side]);

		for (i = 0; i < sides; i++)
		{
			// Don't check side found in step 1.
			if(side != i)
			{
			// avoid dividing by zero.
				if( (u_Fv_to_Zc.Dot(u_Fv[i])) > 0.0)
				{
				// Distance from mag_Fv * u_Fv to cutting plane i.
					my_w = (mag_Fv[i] - mag_Fv[side] * (u_Fv[side].Dot(u_Fv[i]))) / (u_Fv_to_Zc.Dot(u_Fv[i]));

					if(my_w < w)
					{
						w = my_w;
						edge = i;
					}
				}
			}
		}
		// w is the distance between u_Fv[side] and either an edge (cutting plane mag_Fv * u_Fv[edge])
		// The ratio of the distance (|mag_nv * u_Fv - Zc|)/w is the radial parameter for the patch map.

		double D = 0.0;
		CVector3  Axis = CVector3(0.0, 0.0, 1.0); // angel does not matter if D=0;
		//D = sqrt((Zc - mag_Fv * u_Fv[side]).Dot(Zc - mag_Fv * u_Fv[side]))/w;
		if (w > 0.0)
		{
			D = D_u_Fv_to_Zc / w;
			Axis = u_Fv[side].Cross(zXYZ);
			Axis = Axis / sqrt(Axis.Dot(Axis));
		}
		flat = Zc.Dot(u_zXYZ);
		if (!fractal->transformCommon.functionEnabledAFalse)
		{
			//if(D>1.0){printf("HERE");}
			// double rot_angle = 3.14159*(1-D*D)*(1-D*D);  // force detail away from edges and center.
			//double rot_angle = 3.14159*(1-D)*(1-D); // Move detail toward center.
			//double rot_angle = 3.14159*(1-D*D); // Move detail tword edge.
			//double rot_angle = 3.14159*(1-.3 * D*D - .7*D);  // Mixing
			double rot_angle = M_PI * (1.0 - D);  // Linear

			//Sign of rot_angel determines the patch includes self
			//rot_angle = rot_angle * ((side & 2)-1); // mix it up
			//rot_angle = rot_angle;  // Exclude self
			rot_angle = -rot_angle; // Include self

			//zXYZ = zXYZ *(1.0 - .0*flat);  // Does not play well with power DE
			//zXYZ =  zXYZ.RotateAroundVectorByAngle(u_Fv[side], 3.14159/3.0); // Taffy
//			zXYZ = zXYZ.RotateAroundVectorByAngle(Axis, rot_angle);  // php angel4 ??
		}
		else
		{
			zXYZ = zXYZ / flat;
			double ramp = 5.0 * D - (int)(5.0 * D); // (int)
			double saw = -1.0 + 2.0 * fabs(ramp - 0.5); // fabs
			saw = (saw + 0.6) + fabs(saw + 0.6); // fabs
			saw = 0.02 * saw * saw;
			double rings = 1.0 - (saw * (1.0 - 0.1 * ramp));
			if (D < 0.02) rings = rings * 0.98;
			zXYZ = zXYZ * rings;
		}

	}

	// zXYZ ray hits unit sphere first.

	double rp = pow(mag_zXYZ, power - 1.0);



	aux.DE = rp * aux.DE * power + 1.0;
	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	zXYZ = zXYZ * rp;

	z = CVector4(zXYZ, z.w);
}
