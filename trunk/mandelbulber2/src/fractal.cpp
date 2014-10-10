/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cFractal class - container for fractal formula parameters
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

#include "fractal.h"
#include "algebra.hpp"
#include "system.hpp"

cFractal::cFractal(const cParameterContainer *container)
{
	//WriteLog("cFractal::cFractal(const cParameterContainer *container)");

	bulb.power = container->Get<double>("power");
	bulb.alphaAngleOffset = container->Get<double>("alpha_angle_offset");
	bulb.betaAngleOffset = container->Get<double>("beta_angle_offset");
	bulb.gammaAngleOffset = container->Get<double>("gamma_angle_offset");

	mandelbox.scale = container->Get<double>("mandelbox_scale");
	mandelbox.foldingLimit = container->Get<double>("mandelbox_folding_limit");
	mandelbox.foldingValue = container->Get<double>("mandelbox_folding_value");
	mandelbox.foldingSphericalMin = container->Get<double>("mandelbox_folding_min_radius");
	mandelbox.foldingSphericalFixed = container->Get<double>("mandelbox_folding_fixed_radius");
	mandelbox.sharpness = container->Get<double>("mandelbox_sharpness");
	mandelbox.offset = container->Get<CVector3>("mandelbox_offset");
	mandelbox.rotationMain = container->Get<CVector3>("mandelbox_rotation_main");

	for(int i=1; i<=3; i++)
	{
		mandelbox.rotation[0][i-1] = container->Get<CVector3>("mandelbox_rotation_neg", i);
		mandelbox.rotation[1][i-1] = container->Get<CVector3>("mandelbox_rotation_pos", i);
	}

	mandelbox.colorFactor = container->Get<CVector3>("mandelbox_color");
	mandelbox.colorFactorR = container->Get<double>("mandelbox_color_R");
	mandelbox.colorFactorSp1 = container->Get<double>("mandelbox_color_Sp1");
	mandelbox.colorFactorSp2 = container->Get<double>("mandelbox_color_Sp2");
	mandelbox.rotationsEnabled = container->Get<double>("mandelbox_rotations_enabled");
	mandelbox.mainRotationEnabled = container->Get<double>("mandelbox_main_rotation_enabled");

	mandelboxVary4D.fold = container->Get<double>("mandelbox_vary_fold");
	mandelboxVary4D.minR = container->Get<double>("mandelbox_vary_minr");
	mandelboxVary4D.rPower = container->Get<double>("mandelbox_vary_rpower");
	mandelboxVary4D.scaleVary = container->Get<double>("mandelbox_vary_scale_vary");
	mandelboxVary4D.wadd = container->Get<double>("mandelbox_vary_wadd");

	foldingIntPow.foldfactor = container->Get<double>("boxfold_bulbpow2_folding_factor");
	foldingIntPow.zFactor = container->Get<double>("boxfold_bulbpow2_z_factor");

	IFS.scale = container->Get<double>("IFS_scale");
	IFS.rotation = container->Get<CVector3>("IFS_rotation");
	IFS.rotationEnabled =  container->Get<bool>("IFS_rotation_enabled");
	IFS.offset = container->Get<CVector3>("IFS_offset");
	IFS.edge = container->Get<CVector3>("IFS_edge");
	IFS.edgeEnabled = container->Get<bool>("IFS_edge_enabled");

	IFS.absX = container->Get<bool>("IFS_abs_x");
	IFS.absY = container->Get<bool>("IFS_abs_y");
	IFS.absZ = container->Get<bool>("IFS_abs_z");
	IFS.mengerSpongeMode = container->Get<bool>("IFS_menger_sponge_mode");

	for(int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		IFS.direction[i] = container->Get<CVector3>("IFS_direction", i);
		IFS.rotations[i] = container->Get<CVector3>("IFS_rotations", i);
		IFS.distance[i] = container->Get<double>("IFS_distance", i);
		IFS.intensity[i] = container->Get<double>("IFS_intensity", i);
		IFS.enabled[i] = container->Get<bool>("IFS_enabled", i);
		IFS.direction[i].Normalize();
	}

	aexion.cadd = container->Get<double>("cadd");

	WriteLog("cFractal::RecalculateFractalParams(void)");
	RecalculateFractalParams();
}

void cFractal::RecalculateFractalParams(void)
{
	IFS.mainRot.SetRotation(IFS.rotation * (M_PI / 180.0));

	for (int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		IFS.rot[i].SetRotation(IFS.rotations[i] * (M_PI / 180.0));
		IFS.direction[i].Normalize();
	}

	mandelbox.mainRot.SetRotation(mandelbox.rotationMain * (M_PI / 180.0));

	for (int fold = 0; fold < MANDELBOX_FOLDS; ++fold)
	{
		for (int axis = 0; axis < 3; ++axis)
		{
			mandelbox.rot[fold][axis].SetRotation(mandelbox.rotation[fold][axis] * (M_PI / 180.0));
			mandelbox.rotinv[fold][axis] = mandelbox.rot[fold][axis].Transpose();
		}
	}
	mandelbox.fR2 = mandelbox.foldingSphericalFixed * mandelbox.foldingSphericalFixed;
	mandelbox.mR2 = mandelbox.foldingSphericalMin * mandelbox.foldingSphericalMin;
	mandelbox.mboxFactor1 = mandelbox.fR2 / mandelbox.mR2;

	bulb.alphaAngleOffset *= M_PI / 180.0;
	bulb.betaAngleOffset *= M_PI / 180.0;

	//Generalized Fold Box precalculated vectors
	double sqrt_i3 = 1.0/sqrt(3.0);
	genFoldBox.Nv_tet[0] = CVector3( sqrt_i3, sqrt_i3,-sqrt_i3);
	genFoldBox.Nv_tet[1] = CVector3( sqrt_i3,-sqrt_i3, sqrt_i3);
	genFoldBox.Nv_tet[2] = CVector3(-sqrt_i3, sqrt_i3, sqrt_i3);
	genFoldBox.Nv_tet[3] = CVector3(-sqrt_i3,-sqrt_i3,-sqrt_i3);
	genFoldBox.sides_tet = 4;

	genFoldBox.Nv_cube[0] = CVector3( 1, 0, 0);
	genFoldBox.Nv_cube[1] = CVector3(-1, 0, 0);
	genFoldBox.Nv_cube[2] = CVector3( 0, 1, 0);
	genFoldBox.Nv_cube[3] = CVector3( 0,-1, 0);
	genFoldBox.Nv_cube[4] = CVector3( 0, 0, 1);
	genFoldBox.Nv_cube[5] = CVector3( 0, 0,-1);
	genFoldBox.sides_cube = 6;

  genFoldBox.Nv_oct[0] = CVector3( sqrt_i3, sqrt_i3,-sqrt_i3);
  genFoldBox.Nv_oct[1] = CVector3( sqrt_i3,-sqrt_i3, sqrt_i3);
  genFoldBox.Nv_oct[2] = CVector3(-sqrt_i3, sqrt_i3, sqrt_i3);
  genFoldBox.Nv_oct[3] = CVector3(-sqrt_i3,-sqrt_i3,-sqrt_i3);
  genFoldBox.Nv_oct[4] = CVector3( sqrt_i3, sqrt_i3, sqrt_i3);
  genFoldBox.Nv_oct[5] = CVector3(-sqrt_i3,-sqrt_i3, sqrt_i3);
  genFoldBox.Nv_oct[6] = CVector3(-sqrt_i3, sqrt_i3,-sqrt_i3);
  genFoldBox.Nv_oct[7] = CVector3( sqrt_i3,-sqrt_i3,-sqrt_i3);
	genFoldBox.sides_oct = 8;

  genFoldBox.Nv_oct_cube[0] = CVector3( sqrt_i3, sqrt_i3,-sqrt_i3);
  genFoldBox.Nv_oct_cube[1] = CVector3( sqrt_i3,-sqrt_i3, sqrt_i3);
  genFoldBox.Nv_oct_cube[2] = CVector3(-sqrt_i3, sqrt_i3, sqrt_i3);
  genFoldBox.Nv_oct_cube[3] = CVector3(-sqrt_i3,-sqrt_i3,-sqrt_i3);
  genFoldBox.Nv_oct_cube[4] = CVector3( sqrt_i3, sqrt_i3, sqrt_i3);
  genFoldBox.Nv_oct_cube[5] = CVector3(-sqrt_i3,-sqrt_i3, sqrt_i3);
  genFoldBox.Nv_oct_cube[6] = CVector3(-sqrt_i3, sqrt_i3,-sqrt_i3);
  genFoldBox.Nv_oct_cube[7] = CVector3( sqrt_i3,-sqrt_i3,-sqrt_i3);
  genFoldBox.Nv_oct_cube[8] = CVector3( 1, 0, 0);
  genFoldBox.Nv_oct_cube[9] = CVector3(-1, 0, 0);
  genFoldBox.Nv_oct_cube[10] = CVector3( 0, 1, 0);
  genFoldBox.Nv_oct_cube[11] = CVector3( 0,-1, 0);
  genFoldBox.Nv_oct_cube[12] = CVector3( 0, 0, 1);
  genFoldBox.Nv_oct_cube[13] = CVector3( 0, 0,-1);
	genFoldBox.sides_oct_cube = 14;

	double aa = ((1.0+sqrt(5.0))/2.0);
	double bb = (1.0/sqrt(aa*aa+1.0));
  genFoldBox.Nv_dodeca[0] = CVector3(0, bb, aa * bb);
	genFoldBox.Nv_dodeca[1] = CVector3(0, bb, -aa * bb);
	genFoldBox.Nv_dodeca[2] = CVector3(0, -bb, aa * bb);
	genFoldBox.Nv_dodeca[3] = CVector3(0, -bb, -aa * bb);
	genFoldBox.Nv_dodeca[4] = CVector3(bb, aa * bb, 0);
	genFoldBox.Nv_dodeca[5] = CVector3(bb, -aa * bb, 0);
	genFoldBox.Nv_dodeca[6] = CVector3(-bb, aa * bb, 0);
	genFoldBox.Nv_dodeca[7] = CVector3(-bb, -aa * bb, 0);
	genFoldBox.Nv_dodeca[8] = CVector3(aa * bb, 0, bb);
	genFoldBox.Nv_dodeca[9] = CVector3(-aa * bb, 0, bb);
	genFoldBox.Nv_dodeca[10] = CVector3(aa * bb, 0, -bb);
	genFoldBox.Nv_dodeca[11] = CVector3(-aa * bb, 0, -bb);
	genFoldBox.sides_dodeca = 12;

  double ff = sqrt(aa*aa+1.0/(aa*aa));
  double cc = aa/ff;
  double dd = 1.0/aa/ff;

  genFoldBox.Nv_icosa[0] = CVector3( sqrt_i3, sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_icosa[1] = CVector3( sqrt_i3,-sqrt_i3, sqrt_i3);
	genFoldBox.Nv_icosa[2] = CVector3( -sqrt_i3, sqrt_i3, sqrt_i3);
	genFoldBox.Nv_icosa[3] = CVector3( -sqrt_i3,-sqrt_i3,-sqrt_i3);
	genFoldBox.Nv_icosa[4] = CVector3( sqrt_i3, sqrt_i3, sqrt_i3);
	genFoldBox.Nv_icosa[5] = CVector3( -sqrt_i3,-sqrt_i3, sqrt_i3);
	genFoldBox.Nv_icosa[6] = CVector3( -sqrt_i3, sqrt_i3,-sqrt_i3);
	genFoldBox.Nv_icosa[7] = CVector3( sqrt_i3,-sqrt_i3,-sqrt_i3);
	genFoldBox.Nv_icosa[8] = CVector3( 0, dd, cc);
	genFoldBox.Nv_icosa[9] = CVector3( 0, dd,-cc);
	genFoldBox.Nv_icosa[10] = CVector3( 0,-dd, cc);
	genFoldBox.Nv_icosa[11] = CVector3( 0,-dd,-cc);
	genFoldBox.Nv_icosa[12] = CVector3( cc, 0, dd);
	genFoldBox.Nv_icosa[13] = CVector3( cc, 0,-dd);
	genFoldBox.Nv_icosa[14] = CVector3( -cc, 0, dd);
	genFoldBox.Nv_icosa[15] = CVector3( -cc, 0,-dd);
	genFoldBox.Nv_icosa[16] = CVector3( dd, cc, 0);
	genFoldBox.Nv_icosa[17] = CVector3( dd,-cc, 0);
	genFoldBox.Nv_icosa[18] = CVector3( -dd, cc, 0);
	genFoldBox.Nv_icosa[19] = CVector3( -dd,-cc, 0);
	genFoldBox.sides_icosa = 20;

	double tha = M_PI*2.0/6.0;

  genFoldBox.Nv_box6[0] = CVector3(0, 0, 1);
	genFoldBox.Nv_box6[1] = CVector3(0, 0, -1);
	genFoldBox.Nv_box6[2] = CVector3(cos(tha * 0), sin(tha * 0), 0);
	genFoldBox.Nv_box6[3] = CVector3(cos(tha * 1), sin(tha * 1), 0);
	genFoldBox.Nv_box6[4] = CVector3(cos(tha * 2), sin(tha * 2), 0);
	genFoldBox.Nv_box6[5] = CVector3(cos(tha * 3), sin(tha * 3), 0);
	genFoldBox.Nv_box6[6] = CVector3(cos(tha * 4), sin(tha * 4), 0);
	genFoldBox.Nv_box6[7] = CVector3(cos(tha * 5), sin(tha * 5), 0);
	genFoldBox.sides_box6 = 8;

	double tha5 = M_PI*2.0/5.0;

  genFoldBox.Nv_box5[0] = CVector3(0, 0, 1);
	genFoldBox.Nv_box5[1] = CVector3(0, 0, -1);
	genFoldBox.Nv_box5[2] = CVector3(cos(tha5 * 0), sin(tha5 * 0), 0);
	genFoldBox.Nv_box5[3] = CVector3(cos(tha5 * 1), sin(tha5 * 1), 0);
	genFoldBox.Nv_box5[4] = CVector3(cos(tha5 * 2), sin(tha5 * 2), 0);
	genFoldBox.Nv_box5[5] = CVector3(cos(tha5 * 3), sin(tha5 * 3), 0);
	genFoldBox.Nv_box5[6] = CVector3(cos(tha5 * 4), sin(tha5 * 4), 0);
	genFoldBox.sides_box5 = 7;

}
