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
	formula = fractal::none;

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

	for (int i = 1; i <= 3; i++)
	{
		mandelbox.rotation[0][i - 1] = container->Get<CVector3>("mandelbox_rotation_neg", i);
		mandelbox.rotation[1][i - 1] = container->Get<CVector3>("mandelbox_rotation_pos", i);
	}

	mandelbox.color.factor = container->Get<CVector3>("mandelbox_color");
	mandelbox.color.factorR = container->Get<double>("mandelbox_color_R");
	mandelbox.color.factorSp1 = container->Get<double>("mandelbox_color_Sp1");
	mandelbox.color.factorSp2 = container->Get<double>("mandelbox_color_Sp2");
	mandelbox.rotationsEnabled = container->Get<double>("mandelbox_rotations_enabled");
	mandelbox.mainRotationEnabled = container->Get<double>("mandelbox_main_rotation_enabled");

	mandelboxVary4D.fold = container->Get<double>("mandelbox_vary_fold");
	mandelboxVary4D.minR = container->Get<double>("mandelbox_vary_minr");
	mandelboxVary4D.rPower = container->Get<double>("mandelbox_vary_rpower");
	mandelboxVary4D.scaleVary = container->Get<double>("mandelbox_vary_scale_vary");
	mandelboxVary4D.wadd = container->Get<double>("mandelbox_vary_wadd");

	mandelbox.solid = container->Get<double>("mandelbox_solid");
	mandelbox.melt = container->Get<double>("mandelbox_melt");
	genFoldBox.type =
			(fractal::enumGeneralizedFoldBoxType) container->Get<int>("mandelbox_generalized_fold_type");

	foldingIntPow.foldfactor = container->Get<double>("boxfold_bulbpow2_folding_factor");
	foldingIntPow.zFactor = container->Get<double>("boxfold_bulbpow2_z_factor");

	IFS.scale = container->Get<double>("IFS_scale");
	IFS.rotation = container->Get<CVector3>("IFS_rotation");
	IFS.rotationEnabled = container->Get<bool>("IFS_rotation_enabled");
	IFS.offset = container->Get<CVector3>("IFS_offset");
	IFS.edge = container->Get<CVector3>("IFS_edge");
	IFS.edgeEnabled = container->Get<bool>("IFS_edge_enabled");

	IFS.absX = container->Get<bool>("IFS_abs_x");
	IFS.absY = container->Get<bool>("IFS_abs_y");
	IFS.absZ = container->Get<bool>("IFS_abs_z");
	IFS.mengerSpongeMode = container->Get<bool>("IFS_menger_sponge_mode");

	for (int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		IFS.direction[i] = container->Get<CVector3>("IFS_direction", i);
		IFS.rotations[i] = container->Get<CVector3>("IFS_rotations", i);
		IFS.distance[i] = container->Get<double>("IFS_distance", i);
		IFS.intensity[i] = container->Get<double>("IFS_intensity", i);
		IFS.enabled[i] = container->Get<bool>("IFS_enabled", i);
		IFS.direction[i].Normalize();
	}

	aexion.cadd = container->Get<double>("cadd");

	buffalo.preabsx = container->Get<bool>("buffalo_preabs_x");
	buffalo.preabsy = container->Get<bool>("buffalo_preabs_y");
	buffalo.preabsz = container->Get<bool>("buffalo_preabs_z");
	buffalo.absx = container->Get<bool>("buffalo_abs_x");
	buffalo.absy = container->Get<bool>("buffalo_abs_y");
	buffalo.absz = container->Get<bool>("buffalo_abs_z");
	buffalo.posz = container->Get<bool>("buffalo_pos_z");

	msltoeSym2.y_multiplier = container->Get<double>("msltoesym2_y_multiplier");

  mengerMod.constantZ =	container->Get<double>("mengerMod_constantZ");
  mengerMod.factorConstantVect = container->Get<CVector3>("mengerMod_factorConstantVect");
  mengerMod.additionConstant = container->Get<CVector3>("mengerMod_additionConstant");

  quaternion3D.quaternionFactorConstant = container->Get<CVector3>("quaternion3D_factorConstantVect"),
  quaternion3D.constantMultiplierVect = container->Get<CVector3>("quaternion3D_constantMultiplier");
  quaternion3D.additionConstant = container->Get<CVector3>("quaternion3D_additionConstant");


	//  transform parameters-----------------------
	transform.additionConstant1.control.enabled =
			container->Get<bool>("transform_additionConstant1_enabled");
	transform.additionConstant1.control.weightEnabled =
			container->Get<bool>("transform_additionConstant1_weight_enabled");
	transform.additionConstant1.control.startIterations =
			container->Get<int>("transform_additionConstant1_start_iterations");
	transform.additionConstant1.control.stopIterations =
			container->Get<int>("transform_additionConstant1_stop_iterations");
	transform.additionConstant1.control.weight =
			container->Get<double>("transform_additionConstant1_weight");
	transform.additionConstant1.additionConstant =
			container->Get<CVector3>("transform_additionConstant1");
	transform.additionConstant2.additionConstant =
			container->Get<CVector3>("transform_additionConstant2");
	transform.additionConstant2.control.enabled =
			container->Get<bool>("transform_additionConstant2_enabled");
	transform.additionConstant2.control.weightEnabled =
			container->Get<bool>("transform_additionConstant2_weight_enabled");
	transform.additionConstant2.control.startIterations =
			container->Get<int>("transform_additionConstant2_start_iterations");
	transform.additionConstant2.control.stopIterations =
			container->Get<int>("transform_additionConstant2_stop_iterations");
	transform.additionConstant2.control.weight =
			container->Get<double>("transform_additionConstant2_weight");
	//extra for 4D
	transform.additionConstant1.additionConstant4D =
			CVector4(container->Get<CVector3>("transform_additionConstant1"),
							 container->Get<double>("transform_additionConstant1W"));
	transform.additionConstant2.additionConstant4D =
			CVector4(container->Get<CVector3>("transform_additionConstant2"),
							 container->Get<double>("transform_additionConstant2W"));

	transform.boxConstantMultiplier1.control.enabled =
			container->Get<bool>("transform_boxConstantMultiplier1_enabled");
	transform.boxConstantMultiplier1.boxConstantMultiplierFabsEnabledX =
			container->Get<bool>("transform_boxConstantMultiplier1_fabs_enabledX");
	transform.boxConstantMultiplier1.boxConstantMultiplierFabsEnabledY =
			container->Get<bool>("transform_boxConstantMultiplier1_fabs_enabledY");
	transform.boxConstantMultiplier1.boxConstantMultiplierFabsEnabledZ =
			container->Get<bool>("transform_boxConstantMultiplier1_fabs_enabledZ");
	transform.boxConstantMultiplier1.control.weightEnabled =
			container->Get<bool>("transform_boxConstantMultiplier1_weight_enabled");
	transform.boxConstantMultiplier1.control.startIterations =
			container->Get<int>("transform_boxConstantMultiplier1_start_iterations");
	transform.boxConstantMultiplier1.control.stopIterations =
			container->Get<int>("transform_boxConstantMultiplier1_stop_iterations");
	transform.boxConstantMultiplier1.control.weight =
			container->Get<double>("transform_boxConstantMultiplier1_weight");
	transform.boxConstantMultiplier1.boxConstantMultiplier =
			container->Get<CVector3>("transform_boxConstantMultiplier1");
	transform.boxConstantMultiplier2.control.enabled =
			container->Get<bool>("transform_boxConstantMultiplier2_enabled");
	transform.boxConstantMultiplier2.boxConstantMultiplierFabsEnabledX =
			container->Get<bool>("transform_boxConstantMultiplier2_fabs_enabledX");
	transform.boxConstantMultiplier2.boxConstantMultiplierFabsEnabledY =
			container->Get<bool>("transform_boxConstantMultiplier2_fabs_enabledY");
	transform.boxConstantMultiplier2.boxConstantMultiplierFabsEnabledZ =
			container->Get<bool>("transform_boxConstantMultiplier2_fabs_enabledZ");
	transform.boxConstantMultiplier2.control.weightEnabled =
			container->Get<bool>("transform_boxConstantMultiplier2_weight_enabled");
	transform.boxConstantMultiplier2.control.startIterations =
			container->Get<int>("transform_boxConstantMultiplier2_start_iterations");
	transform.boxConstantMultiplier2.control.stopIterations =
			container->Get<int>("transform_boxConstantMultiplier2_stop_iterations");
	transform.boxConstantMultiplier2.control.weight =
			container->Get<double>("transform_boxConstantMultiplier2_weight");
	transform.boxConstantMultiplier2.boxConstantMultiplier =
			container->Get<CVector3>("transform_boxConstantMultiplier2");

	transform.boxFoldOriginal1.control.enabled =
			container->Get<bool>("transform_box_foldOriginal1_enabled");
	transform.boxFoldOriginal1.control.weightEnabled =
			container->Get<bool>("transform_box_foldOriginal1_weight_enabled");
	transform.boxFoldOriginal1.control.oldWeightEnabled =
			container->Get<bool>("transform_box_foldOriginal1_old_weight_enabled");
	transform.boxFoldOriginal1.control.startIterations =
			container->Get<int>("transform_box_foldOriginal1_start_iterations");
	transform.boxFoldOriginal1.control.stopIterations =
			container->Get<int>("transform_box_foldOriginal1_stop_iterations");
	transform.boxFoldOriginal1.foldingLimit =
			container->Get<double>("transform_mandelbox_folding_limit");
	transform.boxFoldOriginal1.foldingValue =
			container->Get<double>("transform_mandelbox_folding_value");
	transform.boxFoldOriginal1.control.weight =
			container->Get<double>("transform_box_foldOriginal1_weight");
	transform.boxFoldOriginal1.control.oldWeight =
			container->Get<double>("transform_box_foldOriginal1_old_weight");
	transform.boxFoldOriginal1.color = mandelbox.color;

	transform.boxFold1.control.enabled = container->Get<bool>("transform_box_fold1_enabled");
	transform.boxFold1.control.weightEnabled =
			container->Get<bool>("transform_box_fold1_weight_enabled");
	transform.boxFold1.control.oldWeightEnabled =
			container->Get<bool>("transform_box_fold1_old_weight_enabled");
	transform.boxFold1.control.startIterations =
			container->Get<int>("transform_box_fold1_start_iterations");
	transform.boxFold1.control.stopIterations =
			container->Get<int>("transform_box_fold1_stop_iterations");
	transform.boxFold1.foldingLimit = container->Get<double>("transform_box_fold1_folding_limit");
	transform.boxFold1.foldingValue = container->Get<double>("transform_box_fold1_folding_value");
	transform.boxFold1.control.weight = container->Get<double>("transform_box_fold1_weight");
	transform.boxFold1.control.oldWeight = container->Get<double>("transform_box_fold1_old_weight");
	transform.boxFold1.color = mandelbox.color;

	transform.boxFold2.control.enabled = container->Get<bool>("transform_box_fold2_enabled");
	transform.boxFold2.control.weightEnabled =
			container->Get<bool>("transform_box_fold2_weight_enabled");
	transform.boxFold2.control.oldWeightEnabled =
			container->Get<bool>("transform_box_fold2_old_weight_enabled");
	transform.boxFold2.control.startIterations =
			container->Get<int>("transform_box_fold2_start_iterations");
	transform.boxFold2.control.stopIterations =
			container->Get<int>("transform_box_fold2_stop_iterations");
	transform.boxFold2.foldingLimit = container->Get<double>("transform_box_fold2_folding_limit");
	transform.boxFold2.foldingValue = container->Get<double>("transform_box_fold2_folding_value");
	transform.boxFold2.control.weight = container->Get<double>("transform_box_fold2_weight");
	transform.boxFold2.control.oldWeight = container->Get<double>("transform_box_fold2_old_weight");
	transform.boxFold2.color = mandelbox.color;

	transform.boxOffset1.control.enabled = container->Get<bool>("transform_boxOffset1_enabled");
	transform.boxOffset1.control.weightEnabled =
			container->Get<bool>("transform_boxOffset1_weight_enabled");
	transform.boxOffset1.control.startIterations =
			container->Get<int>("transform_boxOffset1_start_iterations");
	transform.boxOffset1.control.stopIterations =
			container->Get<int>("transform_boxOffset1_stop_iterations");
	transform.boxOffset1.control.weight = container->Get<double>("transform_boxOffset1_weight");
	transform.boxOffset1.boxOffset = container->Get<CVector3>("transform_boxOffset1");
	transform.boxOffset1.color = mandelbox.color;
	transform.boxOffset2.control.enabled = container->Get<bool>("transform_boxOffset2_enabled");
	transform.boxOffset2.control.weightEnabled =
			container->Get<bool>("transform_boxOffset2_weight_enabled");
	transform.boxOffset2.control.startIterations =
			container->Get<int>("transform_boxOffset2_start_iterations");
	transform.boxOffset2.control.stopIterations =
			container->Get<int>("transform_boxOffset2_stop_iterations");
	transform.boxOffset2.control.weight = container->Get<double>("transform_boxOffset2_weight");
	transform.boxOffset2.boxOffset = container->Get<CVector3>("transform_boxOffset2");
	transform.boxOffset2.color = mandelbox.color;

	transform.constantMultiplierOriginal1.constantMultiplierVect =
			container->Get<CVector3>("transform_constantMultiplierOriginal1");
	transform.constantMultiplierOriginal1.control.enabled =
			container->Get<bool>("transform_constantMultiplierOriginal1_enabled");
	transform.constantMultiplierOriginal1.control.weightEnabled =
			container->Get<bool>("transform_constantMultiplierOriginal1_weight_enabled");
	transform.constantMultiplierOriginal1.control.startIterations =
			container->Get<int>("transform_constantMultiplierOriginal1_start_iterations");
	transform.constantMultiplierOriginal1.control.stopIterations =
			container->Get<int>("transform_constantMultiplierOriginal1_stop_iterations");
	transform.constantMultiplierOriginal1.control.weight =
			container->Get<double>("transform_constantMultiplierOriginal1_weight");
	// for 4D
	transform.constantMultiplierOriginal1.constantMultiplierVect4D =
			CVector4(container->Get<CVector3>("transform_constantMultiplierOriginal1"), 0.0);

	transform.constantMultiplier1.control.enabled =
			container->Get<bool>("transform_constantMultiplier1_enabled");
	transform.constantMultiplier1.control.weightEnabled =
			container->Get<bool>("transform_constantMultiplier1_weight_enabled");
	transform.constantMultiplier1.control.startIterations =
			container->Get<int>("transform_constantMultiplier1_start_iterations");
	transform.constantMultiplier1.control.stopIterations =
			container->Get<int>("transform_constantMultiplier1_stop_iterations");
	transform.constantMultiplier1.constantMultiplierVect =
			container->Get<CVector3>("transform_constantMultiplier1");
	transform.constantMultiplier1.control.weight =
			container->Get<double>("transform_constantMultiplier1_weight");
	// for 4D
	transform.constantMultiplier1.constantMultiplierVect4D =
			CVector4(container->Get<CVector3>("transform_constantMultiplier1"), 0.0);

	transform.constantMultiplier2.control.enabled =
			container->Get<bool>("transform_constantMultiplier2_enabled");
	transform.constantMultiplier2.control.weightEnabled =
			container->Get<bool>("transform_constantMultiplier2_weight_enabled");
	transform.constantMultiplier2.control.startIterations =
			container->Get<int>("transform_constantMultiplier2_start_iterations");
	transform.constantMultiplier2.control.stopIterations =
			container->Get<int>("transform_constantMultiplier2_stop_iterations");
	transform.constantMultiplier2.constantMultiplierVect =
			container->Get<CVector3>("transform_constantMultiplier2");
	transform.constantMultiplier2.control.weight =
			container->Get<double>("transform_constantMultiplier2_weight");

	transform.fabsAddConstant1.control.enabled =
			container->Get<bool>("transform_fabsAddConstant1_enabled");
	transform.fabsAddConstant1.control.weightEnabled =
			container->Get<bool>("transform_fabsAddConstant1_weight_enabled");
	transform.fabsAddConstant1.control.oldWeightEnabled =
			container->Get<bool>("transform_fabsAddConstant1_old_weight_enabled");
	transform.fabsAddConstant1.fabsAddConstantEnabledx =
			container->Get<bool>("transform_fabsAddConstant1_enabled_x");
	transform.fabsAddConstant1.fabsAddConstantEnabledy =
			container->Get<bool>("transform_fabsAddConstant1_enabled_y");
	transform.fabsAddConstant1.fabsAddConstantEnabledz =
			container->Get<bool>("transform_fabsAddConstant1_enabled_z");
	transform.fabsAddConstant1.control.startIterations =
			container->Get<int>("transform_fabsAddConstant1_start_iterations");
	transform.fabsAddConstant1.control.stopIterations =
			container->Get<int>("transform_fabsAddConstant1_stop_iterations");
	transform.fabsAddConstant1.fabsAddConstantA =
			container->Get<CVector3>("transform_fabsAddConstant1A");
	transform.fabsAddConstant1.fabsAddConstantB =
			container->Get<CVector3>("transform_fabsAddConstant1B");
	transform.fabsAddConstant1.fabsAddConstantC =
			container->Get<CVector3>("transform_fabsAddConstant1C");
	transform.fabsAddConstant1.control.weight =
			container->Get<double>("transform_fabsAddConstant1_weight");
	transform.fabsAddConstant1.control.oldWeight =
			container->Get<double>("transform_fabsAddConstant1_old_weight");
	//extra for 4D
	transform.fabsAddConstant1.fabsAddConstant4DA =
			CVector4(container->Get<CVector3>("transform_fabsAddConstant1A"),
							 container->Get<double>("transform_fabsAddConstant1AW"));
	transform.fabsAddConstant1.fabsAddConstant4DB =
			CVector4(container->Get<CVector3>("transform_fabsAddConstant1B"),
							 container->Get<double>("transform_fabsAddConstant1BW"));
	transform.fabsAddConstant1.fabsAddConstant4DC =
			CVector4(container->Get<CVector3>("transform_fabsAddConstant1C"),
							 container->Get<double>("transform_fabsAddConstant1CW"));

	transform.fabsFormulaAB1.control.enabled =
			container->Get<bool>("transform_fabsFormulaAB1_enabled");
	transform.fabsFormulaAB1.control.weightEnabled =
			container->Get<bool>("transform_fabsFormulaAB1_weight_enabled");
	transform.fabsFormulaAB1.control.oldWeightEnabled =
			container->Get<bool>("transform_fabsFormulaAB1_old_weight_enabled");
	transform.fabsFormulaAB1.fabsFormulaABEnabledx =
			container->Get<bool>("transform_fabsFormulaAB1_enabled_x");
	transform.fabsFormulaAB1.fabsFormulaABEnabledy =
			container->Get<bool>("transform_fabsFormulaAB1_enabled_y");
	transform.fabsFormulaAB1.fabsFormulaABEnabledz =
			container->Get<bool>("transform_fabsFormulaAB1_enabled_z");
	transform.fabsFormulaAB1.control.startIterations =
			container->Get<int>("transform_fabsFormulaAB1_start_iterations");
	transform.fabsFormulaAB1.control.stopIterations =
			container->Get<int>("transform_fabsFormulaAB1_stop_iterations");
	transform.fabsFormulaAB1.fabsFormulaABConstantA =
			container->Get<CVector3>("transform_fabsFormulaAB1A");
	transform.fabsFormulaAB1.fabsFormulaABConstantB =
			container->Get<CVector3>("transform_fabsFormulaAB1B");
	transform.fabsFormulaAB1.control.weight =
			container->Get<double>("transform_fabsFormulaAB1_weight");
	transform.fabsFormulaAB1.control.oldWeight =
			container->Get<double>("transform_fabsFormulaAB1_old_weight");
	transform.fabsFormulaAB2.control.enabled =
			container->Get<bool>("transform_fabsFormulaAB2_enabled");
	transform.fabsFormulaAB2.control.weightEnabled =
			container->Get<bool>("transform_fabsFormulaAB2_weight_enabled");
	transform.fabsFormulaAB2.control.oldWeightEnabled =
			container->Get<bool>("transform_fabsFormulaAB2_old_weight_enabled");
	transform.fabsFormulaAB2.fabsFormulaABEnabledx =
			container->Get<bool>("transform_fabsFormulaAB2_enabled_x");
	transform.fabsFormulaAB2.fabsFormulaABEnabledy =
			container->Get<bool>("transform_fabsFormulaAB2_enabled_y");
	transform.fabsFormulaAB2.fabsFormulaABEnabledz =
			container->Get<bool>("transform_fabsFormulaAB2_enabled_z");
	transform.fabsFormulaAB2.control.startIterations =
			container->Get<int>("transform_fabsFormulaAB2_start_iterations");
	transform.fabsFormulaAB2.control.stopIterations =
			container->Get<int>("transform_fabsFormulaAB2_stop_iterations");
	transform.fabsFormulaAB2.fabsFormulaABConstantA =
			container->Get<CVector3>("transform_fabsFormulaAB2A");
	transform.fabsFormulaAB2.fabsFormulaABConstantB =
			container->Get<CVector3>("transform_fabsFormulaAB2B");
	transform.fabsFormulaAB2.control.weight =
			container->Get<double>("transform_fabsFormulaAB2_weight");
	transform.fabsFormulaAB2.control.oldWeight =
			container->Get<double>("transform_fabsFormulaAB2_old_weight");

	transform.fabsFormulaABCD1.fabsFormulaABCDA =
			container->Get<CVector3>("transform_fabsFormulaABCD1A");
	transform.fabsFormulaABCD1.fabsFormulaABCDB =
			container->Get<CVector3>("transform_fabsFormulaABCD1B");
	transform.fabsFormulaABCD1.fabsFormulaABCDC =
			container->Get<CVector3>("transform_fabsFormulaABCD1C");
	transform.fabsFormulaABCD1.fabsFormulaABCDD =
			container->Get<CVector3>("transform_fabsFormulaABCD1D");
	transform.fabsFormulaABCD1.control.enabled =
			container->Get<bool>("transform_fabsFormulaABCD1_enabled");
	transform.fabsFormulaABCD1.control.weightEnabled =
			container->Get<bool>("transform_fabsFormulaABCD1_weight_enabled");
	transform.fabsFormulaABCD1.control.oldWeightEnabled =
			container->Get<bool>("transform_fabsFormulaABCD1_old_weight_enabled");
	transform.fabsFormulaABCD1.fabsFormulaABCDEnabledAx =
			container->Get<bool>("transform_fabsFormulaABCD1_enabled_Ax");
	transform.fabsFormulaABCD1.fabsFormulaABCDEnabledAy =
			container->Get<bool>("transform_fabsFormulaABCD1_enabled_Ay");
	transform.fabsFormulaABCD1.fabsFormulaABCDEnabledAz =
			container->Get<bool>("transform_fabsFormulaABCD1_enabled_Az");
	transform.fabsFormulaABCD1.fabsFormulaABCDEnabledBx =
			container->Get<bool>("transform_fabsFormulaABCD1_enabled_Bx");
	transform.fabsFormulaABCD1.fabsFormulaABCDEnabledBy =
			container->Get<bool>("transform_fabsFormulaABCD1_enabled_By");
	transform.fabsFormulaABCD1.fabsFormulaABCDEnabledBz =
			container->Get<bool>("transform_fabsFormulaABCD1_enabled_Bz");
	transform.fabsFormulaABCD1.control.startIterations =
			container->Get<int>("transform_fabsFormulaABCD1_start_iterations");
	transform.fabsFormulaABCD1.control.stopIterations =
			container->Get<int>("transform_fabsFormulaABCD1_stop_iterations");
	transform.fabsFormulaABCD1.control.weight =
			container->Get<double>("transform_fabsFormulaABCD1_weight");
	transform.fabsFormulaABCD1.control.oldWeight =
			container->Get<double>("transform_fabsFormulaABCD1_old_weight");

	transform.fabsFormulaABCD2.fabsFormulaABCDA =
			container->Get<CVector3>("transform_fabsFormulaABCD2A");
	transform.fabsFormulaABCD2.fabsFormulaABCDB =
			container->Get<CVector3>("transform_fabsFormulaABCD2B");
	transform.fabsFormulaABCD2.fabsFormulaABCDC =
			container->Get<CVector3>("transform_fabsFormulaABCD2C");
	transform.fabsFormulaABCD2.fabsFormulaABCDD =
			container->Get<CVector3>("transform_fabsFormulaABCD2D");
	transform.fabsFormulaABCD2.control.enabled =
			container->Get<bool>("transform_fabsFormulaABCD2_enabled");
	transform.fabsFormulaABCD2.control.weightEnabled =
			container->Get<bool>("transform_fabsFormulaABCD2_weight_enabled");
	transform.fabsFormulaABCD2.control.oldWeightEnabled =
			container->Get<bool>("transform_fabsFormulaABCD2_old_weight_enabled");
	transform.fabsFormulaABCD2.fabsFormulaABCDEnabledAx =
			container->Get<bool>("transform_fabsFormulaABCD2_enabled_Ax");
	transform.fabsFormulaABCD2.fabsFormulaABCDEnabledAy =
			container->Get<bool>("transform_fabsFormulaABCD2_enabled_Ay");
	transform.fabsFormulaABCD2.fabsFormulaABCDEnabledAz =
			container->Get<bool>("transform_fabsFormulaABCD2_enabled_Az");
	transform.fabsFormulaABCD2.fabsFormulaABCDEnabledBx =
			container->Get<bool>("transform_fabsFormulaABCD2_enabled_Bx");
	transform.fabsFormulaABCD2.fabsFormulaABCDEnabledBy =
			container->Get<bool>("transform_fabsFormulaABCD2_enabled_By");
	transform.fabsFormulaABCD2.fabsFormulaABCDEnabledBz =
			container->Get<bool>("transform_fabsFormulaABCD2_enabled_Bz");
	transform.fabsFormulaABCD2.control.startIterations =
			container->Get<int>("transform_fabsFormulaABCD2_start_iterations");
	transform.fabsFormulaABCD2.control.stopIterations =
			container->Get<int>("transform_fabsFormulaABCD2_stop_iterations");
	transform.fabsFormulaABCD2.control.weight =
			container->Get<double>("transform_fabsFormulaABCD2_weight");
	transform.fabsFormulaABCD2.control.oldWeight =
			container->Get<double>("transform_fabsFormulaABCD2_old_weight");
// extra for 4D
	transform.fabsFormulaABCD1.fabsFormulaABCD4DA =
			CVector4(container->Get<CVector3>("transform_fabsFormulaABCD1A"),
							 container->Get<double>("transform_fabsFormulaABCD1AW"));
	transform.fabsFormulaABCD1.fabsFormulaABCD4DB =
			CVector4(container->Get<CVector3>("transform_fabsFormulaABCD1B"),
							 container->Get<double>("transform_fabsFormulaABCD1BW"));
	transform.fabsFormulaABCD1.fabsFormulaABCD4DC =
			CVector4(container->Get<CVector3>("transform_fabsFormulaABCD1C"),
							 container->Get<double>("transform_fabsFormulaABCD1CW"));
	transform.fabsFormulaABCD1.fabsFormulaABCD4DD =
			CVector4(container->Get<CVector3>("transform_fabsFormulaABCD1D"),
							 container->Get<double>("transform_fabsFormulaABCD1DW"));
	transform.fabsFormulaABCD1.fabsFormulaABCDEnabledAw =
			container->Get<bool>("transform_fabsFormulaABCD1_enabled_Aw");
	transform.fabsFormulaABCD1.fabsFormulaABCDEnabledBw =
			container->Get<bool>("transform_fabsFormulaABCD1_enabled_Bw");
	transform.fabsFormulaABCD2.fabsFormulaABCD4DA =
			CVector4(container->Get<CVector3>("transform_fabsFormulaABCD2A"),
							 container->Get<double>("transform_fabsFormulaABCD2AW"));
	transform.fabsFormulaABCD2.fabsFormulaABCD4DB =
			CVector4(container->Get<CVector3>("transform_fabsFormulaABCD2B"),
							 container->Get<double>("transform_fabsFormulaABCD2BW"));
	transform.fabsFormulaABCD2.fabsFormulaABCD4DC =
			CVector4(container->Get<CVector3>("transform_fabsFormulaABCD2C"),
							 container->Get<double>("transform_fabsFormulaABCD2CW"));
	transform.fabsFormulaABCD2.fabsFormulaABCD4DD =
			CVector4(container->Get<CVector3>("transform_fabsFormulaABCD2D"),
							 container->Get<double>("transform_fabsFormulaABCD2DW"));
	transform.fabsFormulaABCD2.fabsFormulaABCDEnabledAw =
			container->Get<bool>("transform_fabsFormulaABCD2_enabled_Aw");
	transform.fabsFormulaABCD2.fabsFormulaABCDEnabledBw =
			container->Get<bool>("transform_fabsFormulaABCD2_enabled_Bw");

	transform.fabsFormulaZAB1.control.enabled =
			container->Get<bool>("transform_fabsFormulaZAB1_enabled");
	transform.fabsFormulaZAB1.control.weightEnabled =
			container->Get<bool>("transform_fabsFormulaZAB1_weight_enabled");
	transform.fabsFormulaZAB1.control.oldWeightEnabled =
			container->Get<bool>("transform_fabsFormulaZAB1_old_weight_enabled");
	transform.fabsFormulaZAB1.fabsFormulaZABEnabledx =
			container->Get<bool>("transform_fabsFormulaZAB1_enabled_x");
	transform.fabsFormulaZAB1.fabsFormulaZABEnabledy =
			container->Get<bool>("transform_fabsFormulaZAB1_enabled_y");
	transform.fabsFormulaZAB1.fabsFormulaZABEnabledz =
			container->Get<bool>("transform_fabsFormulaZAB1_enabled_z");
	transform.fabsFormulaZAB1.control.startIterations =
			container->Get<int>("transform_fabsFormulaZAB1_start_iterations");
	transform.fabsFormulaZAB1.control.stopIterations =
			container->Get<int>("transform_fabsFormulaZAB1_stop_iterations");
	transform.fabsFormulaZAB1.fabsFormulaZABConstantA =
			container->Get<CVector3>("transform_fabsFormulaZAB1A");
	transform.fabsFormulaZAB1.fabsFormulaZABConstantB =
			container->Get<CVector3>("transform_fabsFormulaZAB1B");
	transform.fabsFormulaZAB1.fabsFormulaZABConstantC =
			container->Get<CVector3>("transform_fabsFormulaZAB1C");
	transform.fabsFormulaZAB1.control.weight =
			container->Get<double>("transform_fabsFormulaZAB1_weight");
	transform.fabsFormulaZAB1.control.oldWeight =
			container->Get<double>("transform_fabsFormulaZAB1_old_weight");

	transform.fabsSubConstant1.control.enabled =
			container->Get<bool>("transform_fabsSubConstant1_enabled");
	transform.fabsSubConstant1.control.weightEnabled =
			container->Get<bool>("transform_fabsSubConstant1_weight_enabled");
	transform.fabsSubConstant1.control.oldWeightEnabled =
			container->Get<bool>("transform_fabsSubConstant1_old_weight_enabled");
	transform.fabsSubConstant1.fabsSubConstantEnabledx =
			container->Get<bool>("transform_fabsSubConstant1_enabled_x");
	transform.fabsSubConstant1.fabsSubConstantEnabledy =
			container->Get<bool>("transform_fabsSubConstant1_enabled_y");
	transform.fabsSubConstant1.fabsSubConstantEnabledz =
			container->Get<bool>("transform_fabsSubConstant1_enabled_z");
	transform.fabsSubConstant1.control.startIterations =
			container->Get<int>("transform_fabsSubConstant1_start_iterations");
	transform.fabsSubConstant1.control.stopIterations =
			container->Get<int>("transform_fabsSubConstant1_stop_iterations");
	transform.fabsSubConstant1.fabsSubConstantA =
			container->Get<CVector3>("transform_fabsSubConstant1A");
	transform.fabsSubConstant1.fabsSubConstantB =
			container->Get<CVector3>("transform_fabsSubConstant1B");
	transform.fabsSubConstant1.control.weight =
			container->Get<double>("transform_fabsSubConstant1_weight");

	transform.iterationWeight1.iterationWeightConstantA =
			container->Get<double>("transform_iterationWeight1_constantA");
	transform.iterationWeight1.iterationWeightConstantB =
			container->Get<double>("transform_iterationWeight1_constantB");
	transform.iterationWeight1.iterationWeightConstantZ =
			container->Get<double>("transform_iterationWeight1_constantZ");
	transform.iterationWeight1.control.enabled =
			container->Get<bool>("transform_iterationWeight1_enabled");
	transform.iterationWeight1.control.weightEnabled =
			container->Get<bool>("transform_iterationWeight1_weight_enabled");
	transform.iterationWeight1.iterationWeightIterationA =
			container->Get<int>("transform_iterationWeight1_iterationA");
	transform.iterationWeight1.iterationWeightIterationB =
			container->Get<int>("transform_iterationWeight1_iterationB");
	transform.iterationWeight1.control.weight =
			container->Get<double>("transform_iterationWeight1_weight");

	transform.mandelbulbOriginal1.power = container->Get<double>("transform_power");
	transform.mandelbulbOriginal1.alphaAngleOffset =
			container->Get<double>("transform_alpha_angle_offset");
	transform.mandelbulbOriginal1.betaAngleOffset =
			container->Get<double>("transform_beta_angle_offset");
	transform.mandelbulbOriginal1.control.enabled =
			container->Get<bool>("transform_mandelbulbOriginal1_enabled");
	transform.mandelbulbOriginal1.control.weightEnabled =
			container->Get<bool>("transform_mandelbulbOriginal1_weight_enabled");
	transform.mandelbulbOriginal1.control.startIterations =
			container->Get<int>("transform_mandelbulbOriginal1_start_iterations");
	transform.mandelbulbOriginal1.control.stopIterations =
			container->Get<int>("transform_mandelbulbOriginal1_stop_iterations");
	transform.mandelbulbOriginal1.control.weight =
			container->Get<double>("transform_mandelbulbOriginal1_weight");

	transform.mandelbulb1.power = container->Get<double>("transform_mandelbulb1_power");
	transform.mandelbulb1.alphaAngleOffset =
			container->Get<double>("transform_mandelbulb1_alpha_angle_offset");
	transform.mandelbulb1.betaAngleOffset =
			container->Get<double>("transform_mandelbulb1_beta_angle_offset");
	transform.mandelbulb1.control.enabled = container->Get<bool>("transform_mandelbulb1_enabled");
	transform.mandelbulb1.control.weightEnabled =
			container->Get<bool>("transform_mandelbulb1_weight_enabled");
	transform.mandelbulb1.control.startIterations =
			container->Get<int>("transform_mandelbulb1_start_iterations");
	transform.mandelbulb1.control.stopIterations =
			container->Get<int>("transform_mandelbulb1_stop_iterations");
	transform.mandelbulb1.control.weight = container->Get<double>("transform_mandelbulb1_weight");

	transform.mainRotation1.control.enabled =
			container->Get<bool>("transform_main_rotation1_enabled");
	transform.mainRotation1.control.weightEnabled =
			container->Get<bool>("transform_main_rotation1_weight_enabled");
	transform.mainRotation1.control.startIterations =
			container->Get<int>("transform_main_rotation1_start_iterations");
	transform.mainRotation1.control.stopIterations =
			container->Get<int>("transform_main_rotation1_stop_iterations");
	transform.mainRotation1.mainRotation = container->Get<CVector3>("transform_main_rotation1");
	transform.mainRotation1.control.weight =
			container->Get<double>("transform_main_rotation1_weight");
	transform.mainRotation2.control.enabled =
			container->Get<bool>("transform_main_rotation2_enabled");
	transform.mainRotation2.control.weightEnabled =
			container->Get<bool>("transform_main_rotation2_weight_enabled");
	transform.mainRotation2.control.startIterations =
			container->Get<int>("transform_main_rotation2_start_iterations");
	transform.mainRotation2.control.stopIterations =
			container->Get<int>("transform_main_rotation2_stop_iterations");
	transform.mainRotation2.mainRotation = container->Get<CVector3>("transform_main_rotation2");
	transform.mainRotation2.control.weight =
			container->Get<double>("transform_main_rotation2_weight");
	transform.mainRotation3.control.enabled =
			container->Get<bool>("transform_main_rotation3_enabled");
	transform.mainRotation3.control.weightEnabled =
			container->Get<bool>("transform_main_rotation3_weight_enabled");
	transform.mainRotation3.control.startIterations =
			container->Get<int>("transform_main_rotation3_start_iterations");
	transform.mainRotation3.control.stopIterations =
			container->Get<int>("transform_main_rotation3_stop_iterations");
	transform.mainRotation3.mainRotation = container->Get<CVector3>("transform_main_rotation3");
	transform.mainRotation3.control.weight =
			container->Get<double>("transform_main_rotation3_weight");
	transform.mainRotation4.control.enabled =
			container->Get<bool>("transform_main_rotation4_enabled");
	transform.mainRotation4.control.weightEnabled =
			container->Get<bool>("transform_main_rotation4_weight_enabled");
	transform.mainRotation4.control.startIterations =
			container->Get<int>("transform_main_rotation4_start_iterations");
	transform.mainRotation4.control.stopIterations =
			container->Get<int>("transform_main_rotation4_stop_iterations");
	transform.mainRotation4.mainRotation = container->Get<CVector3>("transform_main_rotation4");
	transform.mainRotation4.control.weight =
			container->Get<double>("transform_main_rotation4_weight");

	transform.mengerSpongeOriginal1.mengerSpongeOriginalConstantZ =
			container->Get<double>("transform_mengerSpongeOriginal1_constantZ");
	transform.mengerSpongeOriginal1.mengerSpongeOriginalFactorConstantVect =
			container->Get<CVector3>("transform_mengerSpongeOriginal1_factorConstantVect");
	transform.mengerSpongeOriginal1.control.enabled =
			container->Get<bool>("transform_mengerSpongeOriginal1_enabled");
	transform.mengerSpongeOriginal1.control.weightEnabled =
			container->Get<bool>("transform_mengerSpongeOriginal1_weight_enabled");
	transform.mengerSpongeOriginal1.control.startIterations =
			container->Get<int>("transform_mengerSpongeOriginal1_start_iterations");
	transform.mengerSpongeOriginal1.control.stopIterations =
			container->Get<int>("transform_mengerSpongeOriginal1_stop_iterations");
	transform.mengerSpongeOriginal1.control.weight =
			container->Get<double>("transform_mengerSpongeOriginal1_weight");
	transform.mengerSponge1.mengerSpongeConstantZ =
			container->Get<double>("transform_mengerSponge1_constantZ");
	transform.mengerSponge1.mengerSpongeFactorConstantVect =
			container->Get<CVector3>("transform_mengerSponge1_factorConstantVect");
	transform.mengerSponge1.control.enabled = container->Get<bool>("transform_mengerSponge1_enabled");
	transform.mengerSponge1.control.weightEnabled =
			container->Get<bool>("transform_mengerSponge1_weight_enabled");
	transform.mengerSponge1.control.startIterations =
			container->Get<int>("transform_mengerSponge1_start_iterations");
	transform.mengerSponge1.control.stopIterations =
			container->Get<int>("transform_mengerSponge1_stop_iterations");
	transform.mengerSponge1.control.weight = container->Get<double>("transform_mengerSponge1_weight");
	transform.mengerSponge2.mengerSpongeConstantZ =
			container->Get<double>("transform_mengerSponge2_constantZ");
	transform.mengerSponge2.mengerSpongeFactorConstantVect =
			container->Get<CVector3>("transform_mengerSponge2_factorConstantVect");
	transform.mengerSponge2.control.enabled = container->Get<bool>("transform_mengerSponge2_enabled");
	transform.mengerSponge2.control.weightEnabled =
			container->Get<bool>("transform_mengerSponge2_weight_enabled");
	transform.mengerSponge2.control.startIterations =
			container->Get<int>("transform_mengerSponge2_start_iterations");
	transform.mengerSponge2.control.stopIterations =
			container->Get<int>("transform_mengerSponge2_stop_iterations");
	transform.mengerSponge2.control.weight = container->Get<double>("transform_mengerSponge2_weight");

	transform.quaternionOriginal1.quaternionOriginalFactorConstant =
			CVector4(container->Get<CVector3>("transform_quaternionOriginal1_factorConstantVect"),
							 container->Get<double>("transform_quaternionOriginal1_factorConstantW"));
	transform.quaternionOriginal1.control.enabled =
			container->Get<bool>("transform_quaternionOriginal1_enabled");
	transform.quaternionOriginal1.control.weightEnabled =
			container->Get<bool>("transform_quaternionOriginal1_weight_enabled");
	transform.quaternionOriginal1.control.startIterations =
			container->Get<int>("transform_quaternionOriginal1_start_iterations");
	transform.quaternionOriginal1.control.stopIterations =
			container->Get<int>("transform_quaternionOriginal1_stop_iterations");
	transform.quaternionOriginal1.control.weight =
			container->Get<double>("transform_quaternionOriginal1_weight");

	transform.quaternion1.quaternionFactorConstant =
			CVector4(container->Get<CVector3>("transform_quaternion1_factorConstantVect"),
							 container->Get<double>("transform_quaternion1_factorConstantW"));
	transform.quaternion1.control.enabled = container->Get<bool>("transform_quaternion1_enabled");
	transform.quaternion1.control.weightEnabled =
			container->Get<bool>("transform_quaternion1_weight_enabled");
	transform.quaternion1.control.startIterations =
			container->Get<int>("transform_quaternion1_start_iterations");
	transform.quaternion1.control.stopIterations =
			container->Get<int>("transform_quaternion1_stop_iterations");
	transform.quaternion1.control.weight = container->Get<double>("transform_quaternion1_weight");

	transform.scaleOriginal1.control.enabled =
			container->Get<bool>("transform_scaleOriginal1_enabled");
	transform.scaleOriginal1.control.startIterations =
			container->Get<int>("transform_scaleOriginal1_start_iterations");
	transform.scaleOriginal1.control.stopIterations =
			container->Get<int>("transform_scaleOriginal1_stop_iterations");
	transform.scaleOriginal1.scaleOriginal = container->Get<double>("transform_mandelbox_scale");

	transform.scale1.control.enabled = container->Get<bool>("transform_scale1_enabled");
	transform.scale1.control.startIterations =
			container->Get<int>("transform_scale1_start_iterations");
	transform.scale1.control.stopIterations = container->Get<int>("transform_scale1_stop_iterations");
	transform.scale1.scale = container->Get<double>("transform_scale1");
	transform.scale2.control.enabled = container->Get<bool>("transform_scale2_enabled");
	transform.scale2.control.startIterations =
			container->Get<int>("transform_scale2_start_iterations");
	transform.scale2.control.stopIterations = container->Get<int>("transform_scale2_stop_iterations");
	transform.scale2.scale = container->Get<double>("transform_scale2");

	transform.sphericalFoldOriginal1.control.enabled =
			container->Get<bool>("transform_sphericalFoldOriginal1_enabled");
	transform.sphericalFoldOriginal1.control.weightEnabled =
			container->Get<bool>("transform_sphericalFoldOriginal1_weight_enabled");
	transform.sphericalFoldOriginal1.radMin =
			container->Get<double>("transform_mandelbox_folding_min_radius");
	transform.sphericalFoldOriginal1.radFixed =
			container->Get<double>("transform_mandelbox_folding_fixed_radius");
	transform.sphericalFoldOriginal1.control.startIterations =
			container->Get<int>("transform_sphericalFoldOriginal1_start_iterations");
	transform.sphericalFoldOriginal1.control.stopIterations =
			container->Get<int>("transform_sphericalFoldOriginal1_stop_iterations");
	transform.sphericalFoldOriginal1.control.weight =
			container->Get<double>("transform_sphericalFoldOriginal1_weight");
	transform.sphericalFoldOriginal1.color = mandelbox.color;

	transform.sphericalFold1.control.enabled =
			container->Get<bool>("transform_sphericalFold1_enabled");
	transform.sphericalFold1.control.weightEnabled =
			container->Get<bool>("transform_sphericalFold1_weight_enabled");
	transform.sphericalFold1.radMin = container->Get<double>("transform_sphericalFold1_min_radius");
	transform.sphericalFold1.radFixed =
			container->Get<double>("transform_sphericalFold1_fixed_radius");
	transform.sphericalFold1.control.startIterations =
			container->Get<int>("transform_sphericalFold1_start_iterations");
	transform.sphericalFold1.control.stopIterations =
			container->Get<int>("transform_sphericalFold1_stop_iterations");
	transform.sphericalFold1.control.weight =
			container->Get<double>("transform_sphericalFold1_weight");
	transform.sphericalFold1.color = mandelbox.color;
	transform.sphericalFold2.control.enabled =
			container->Get<bool>("transform_sphericalFold2_enabled");
	transform.sphericalFold2.control.weightEnabled =
			container->Get<bool>("transform_sphericalFold2_weight_enabled");
	transform.sphericalFold2.radMin = container->Get<double>("transform_sphericalFold2_min_radius");
	transform.sphericalFold2.radFixed =
			container->Get<double>("transform_sphericalFold2_fixed_radius");
	transform.sphericalFold2.control.startIterations =
			container->Get<int>("transform_sphericalFold2_start_iterations");
	transform.sphericalFold2.control.stopIterations =
			container->Get<int>("transform_sphericalFold2_stop_iterations");
	transform.sphericalFold2.control.weight =
			container->Get<double>("transform_sphericalFold2_weight");
	transform.sphericalFold2.color = mandelbox.color;

	transform.sphericalOffset1.control.enabled =
			container->Get<bool>("transform_sphericalOffset1_enabled");
	transform.sphericalOffset1.control.weightEnabled =
			container->Get<bool>("transform_sphericalOffset1_weight_enabled");
	transform.sphericalOffset1.offsetRadius =
			container->Get<double>("transform_sphericalOffset1_offset_radius");
	transform.sphericalOffset1.control.startIterations =
			container->Get<int>("transform_sphericalOffset1_start_iterations");
	transform.sphericalOffset1.control.stopIterations =
			container->Get<int>("transform_sphericalOffset1_stop_iterations");
	transform.sphericalOffset1.scale = container->Get<double>("transform_sphericalOffset1_scale");
	transform.sphericalOffset1.control.weight =
			container->Get<double>("transform_sphericalOffset1_weight");

	transform.sphericalOffset2.control.enabled =
			container->Get<bool>("transform_sphericalOffset2_enabled");
	transform.sphericalOffset2.control.weightEnabled =
			container->Get<bool>("transform_sphericalOffset2_weight_enabled");
	transform.sphericalOffset2.offsetRadius =
			container->Get<double>("transform_sphericalOffset2_offset_radius");
	transform.sphericalOffset2.control.startIterations =
			container->Get<int>("transform_sphericalOffset2_start_iterations");
	transform.sphericalOffset2.control.stopIterations =
			container->Get<int>("transform_sphericalOffset2_stop_iterations");
	transform.sphericalOffset2.scale = container->Get<int>("transform_sphericalOffset2_scale");
	transform.sphericalOffset2.control.weight =
			container->Get<double>("transform_sphericalOffset2_weight");

	transform.variableConstantMultiplier1.constantMultiplier =
			container->Get<CVector3>("transform_variableConstantMultiplier1_constantMultiplier");
	transform.variableConstantMultiplier1.control.enabled =
			container->Get<bool>("transform_variableConstantMultiplier1_enabled");
	transform.variableConstantMultiplier1.control.weightEnabled =
			container->Get<bool>("transform_variableConstantMultiplier1_weight_enabled");
	transform.variableConstantMultiplier1.control.startIterations =
			container->Get<int>("transform_variableConstantMultiplier1_start_iterations");
	transform.variableConstantMultiplier1.control.stopIterations =
			container->Get<int>("transform_variableConstantMultiplier1_stop_iterations");
	transform.variableConstantMultiplier1.control.weight =
			container->Get<double>("transform_variableConstantMultiplier1_weight");
	// extra for variable
	transform.variableConstantMultiplier1.variableConstant =
			container->Get<CVector3>("transform_variableConstantMultiplier1_variableConstantMultiplier");
	transform.variableConstantMultiplier1.variableStartIterations =
			container->Get<int>("transform_variableConstantMultiplier1_variable_start_iterations");
	transform.variableConstantMultiplier1.variableStopIterations =
			container->Get<int>("transform_variableConstantMultiplier1_variable_stop_iterations");

	transform.variableMandelbulbPower1.power =
			container->Get<double>("transform_variableMandelbulbPower1_power");
	transform.variableMandelbulbPower1.alphaAngleOffset =
			container->Get<double>("transform_variableMandelbulbPower1_alpha_angle_offset");
	transform.variableMandelbulbPower1.betaAngleOffset =
			container->Get<double>("transform_variableMandelbulbPower1_beta_angle_offset");
	transform.variableMandelbulbPower1.control.enabled =
			container->Get<bool>("transform_variableMandelbulbPower1_enabled");
	transform.variableMandelbulbPower1.control.weightEnabled =
			container->Get<bool>("transform_variableMandelbulbPower1_weight_enabled");
	transform.variableMandelbulbPower1.control.startIterations =
			container->Get<int>("transform_variableMandelbulbPower1_start_iterations");
	transform.variableMandelbulbPower1.control.stopIterations =
			container->Get<int>("transform_variableMandelbulbPower1_stop_iterations");
	transform.variableMandelbulbPower1.control.weight =
			container->Get<double>("transform_variableMandelbulbPower1_weight");
	// extra for variable
	transform.variableMandelbulbPower1.variableStartIterations =
			container->Get<int>("transform_variableMandelbulbPower1_variable_start_iterations");
	transform.variableMandelbulbPower1.variableStopIterations =
			container->Get<int>("transform_variableMandelbulbPower1_variable_stop_iterations");
	transform.variableMandelbulbPower1.variableConstant =
			container->Get<double>("transform_variableMandelbulbPower1_variableConstant");

	transform.variableScale1.scale = container->Get<CVector3>("transform_variableScale1_scale");
	transform.variableScale1.control.enabled =
			container->Get<bool>("transform_variableScale1_enabled");
	transform.variableScale1.control.weightEnabled =
			container->Get<bool>("transform_variableScale1_weight_enabled");
	transform.variableScale1.control.startIterations =
			container->Get<int>("transform_variableScale1_start_iterations");
	transform.variableScale1.control.stopIterations =
			container->Get<int>("transform_variableScale1_stop_iterations");
	transform.variableScale1.control.weight =
			container->Get<double>("transform_variableScale1_weight");
	// extra for variable
	transform.variableScale1.variableStartIterations =
			container->Get<int>("transform_variableScale1_variable_start_iterations");
	transform.variableScale1.variableStopIterations =
			container->Get<int>("transform_variableScale1_variable_stop_iterations");
	transform.variableScale1.variableConstant =
			container->Get<CVector3>("transform_variableScale1_variableScale");
	transform.variableScale2.scale = container->Get<CVector3>("transform_variableScale2_scale");
	transform.variableScale2.control.enabled =
			container->Get<bool>("transform_variableScale2_enabled");
	transform.variableScale2.control.weightEnabled =
			container->Get<bool>("transform_variableScale2_weight_enabled");
	transform.variableScale2.control.startIterations =
			container->Get<int>("transform_variableScale2_start_iterations");
	transform.variableScale2.control.stopIterations =
			container->Get<int>("transform_variableScale2_stop_iterations");
	transform.variableScale2.control.weight =
			container->Get<double>("transform_variableScale2_weight");
	// extra for variable
	transform.variableScale2.variableStartIterations =
			container->Get<int>("transform_variableScale2_variable_start_iterations");
	transform.variableScale2.variableStopIterations =
			container->Get<int>("transform_variableScale2_variable_stop_iterations");
	transform.variableScale2.variableConstant =
			container->Get<CVector3>("transform_variableScale2_variableScale");

	transform.coloringParameters1.control.enabled =
			container->Get<bool>("transform_coloringParameters1_enabled");
	transform.coloringParameters1.control.startIterations =
			container->Get<int>("transform_coloringParameters1_start_iterations");
	transform.coloringParameters1.control.stopIterations =
			container->Get<int>("transform_coloringParameters1_stop_iterations");
	transform.coloringParameters1.color.factor =
			container->Get<CVector3>("transform_coloringParameters1_mandelbox_color");
	transform.coloringParameters1.color.factorR =
			container->Get<double>("transform_coloringParameters1_mandelbox_color_R");
	transform.coloringParameters1.color.factorSp1 =
			container->Get<double>("transform_coloringParameters1_mandelbox_color_Sp1");
	transform.coloringParameters1.color.factorSp2 =
			container->Get<double>("transform_coloringParameters1_mandelbox_color_Sp2");

	transform.colorTrial1.control.enabled = container->Get<bool>("transform_colorTrial1_enabled");
	transform.colorTrial1.control.startIterations =
			container->Get<int>("transform_colorTrial1_start_iterations");
	transform.colorTrial1.control.stopIterations =
			container->Get<int>("transform_colorTrial1_stop_iterations");

	transform.colorTrial1.minimumRWeight =
			container->Get<double>("transform_colorTrial1_minimumRWeight");
	transform.colorTrial1.axisBiasEnabled =
			container->Get<bool>("transform_colorTrial1_axisBias_enabled");
	transform.colorTrial1.orbitTrapsEnabled =
			container->Get<bool>("transform_colorTrial1_orbitTraps_enabled");
	//transform.colorTrial1.transformSamplingEnabled = container->Get<bool>("transform_colorTrial1_transformSampling_enabled");

	transform.colorTrial1.colorConstant1 =
			container->Get<double>("transform_colorTrial1_colorConstant1");
	transform.colorTrial1.colorConstant2 =
			container->Get<double>("transform_colorTrial1_colorConstant2");
	transform.colorTrial1.colorConstant3 =
			container->Get<double>("transform_colorTrial1_colorConstant3");
	//transform.colorTrial1.sampleConstant1 = container->Get<double>("transform_colorTrial1_sampleConstant1");
	//transform.colorTrial1.sampleConstant2 = container->Get<double>("transform_colorTrial1_sampleConstant2");
	//transform.colorTrial1.sampleConstant3 = container->Get<double>("transform_colorTrial1_sampleConstant3");
	//transform.colorTrial1.sampleConstant4 = container->Get<double>("transform_colorTrial1_sampleConstant4");
	//transform.colorTrial1.sampleConstant5 = container->Get<double>("transform_colorTrial1_sampleConstant5");
	//transform.colorTrial1.sampleConstant6 = container->Get<double>("transform_colorTrial1_sampleConstant6");

	transform.colorTrial1.mainOrbitTrapWeight =
			container->Get<double>("transform_colorTrial1_mainOTWeight");
	transform.colorTrial1.orbitTrap1Weight =
			container->Get<double>("transform_colorTrial1_orbitTrap1Weight");
	transform.colorTrial1.orbitTrap2Weight =
			container->Get<double>("transform_colorTrial1_orbitTrap2Weight");
	transform.colorTrial1.orbitTrap1 = container->Get<CVector3>("transform_colorTrial1_orbitTrap1");
	transform.colorTrial1.orbitTrap2 = container->Get<CVector3>("transform_colorTrial1_orbitTrap2");

	transform.mandelbulbMulti1.power = container->Get<double>("transform_mandelbulbMulti1_power");
	transform.mandelbulbMulti1.alphaAngleOffset =
			container->Get<double>("transform_mandelbulbMulti1_alpha_angle_offset");
	transform.mandelbulbMulti1.betaAngleOffset =
			container->Get<double>("transform_mandelbulbMulti1_beta_angle_offset");
	transform.mandelbulbMulti1.control.enabled =
			container->Get<bool>("transform_mandelbulbMulti1_enabled");
	transform.mandelbulbMulti1.acosOrasin =
			(sTransformMandelbulbMulti::multi_acosOrasin) container->Get<int>("transform_mandelbulbMulti1_acos_or_asin");
	transform.mandelbulbMulti1.atanOratan2 =
			(sTransformMandelbulbMulti::multi_atanOratan2) container->Get<int>("transform_mandelbulbMulti1_atan_or_atan2");
	transform.mandelbulbMulti1.orderOfxyz =
			(sTransformMandelbulbMulti::multi_OrderOfxyz) container->Get<int>("transform_mandelbulbMulti1_order_of_xyz");
	transform.mandelbulbMulti1.mainRotation =
			container->Get<CVector3>("transform_mandelbulbMulti1_main_rotation");
	transform.mandelbulbMulti1.control.weightEnabled =
			container->Get<bool>("transform_mandelbulbMulti1_weight_enabled");
	transform.mandelbulbMulti1.control.startIterations =
			container->Get<int>("transform_mandelbulbMulti1_start_iterations");
	transform.mandelbulbMulti1.control.stopIterations =
			container->Get<int>("transform_mandelbulbMulti1_stop_iterations");
	transform.mandelbulbMulti1.control.weight =
			container->Get<double>("transform_mandelbulbMulti1_weight");
	transform.mandelbulbMulti2.power = container->Get<double>("transform_mandelbulbMulti2_power");
	transform.mandelbulbMulti2.alphaAngleOffset =
			container->Get<double>("transform_mandelbulbMulti2_alpha_angle_offset");
	transform.mandelbulbMulti2.betaAngleOffset =
			container->Get<double>("transform_mandelbulbMulti2_beta_angle_offset");
	transform.mandelbulbMulti2.control.enabled =
			container->Get<bool>("transform_mandelbulbMulti2_enabled");
	transform.mandelbulbMulti2.acosOrasin =
			(sTransformMandelbulbMulti::multi_acosOrasin) container->Get<int>("transform_mandelbulbMulti2_acos_or_asin");
	transform.mandelbulbMulti2.atanOratan2 =
			(sTransformMandelbulbMulti::multi_atanOratan2) container->Get<int>("transform_mandelbulbMulti2_atan_or_atan2");
	transform.mandelbulbMulti2.orderOfxyz =
			(sTransformMandelbulbMulti::multi_OrderOfxyz) container->Get<int>("transform_mandelbulbMulti2_order_of_xyz");
	transform.mandelbulbMulti2.mainRotation =
			container->Get<CVector3>("transform_mandelbulbMulti2_main_rotation");
	transform.mandelbulbMulti2.control.weightEnabled =
			container->Get<bool>("transform_mandelbulbMulti2_weight_enabled");
	transform.mandelbulbMulti2.control.startIterations =
			container->Get<int>("transform_mandelbulbMulti2_start_iterations");
	transform.mandelbulbMulti2.control.stopIterations =
			container->Get<int>("transform_mandelbulbMulti2_stop_iterations");
	transform.mandelbulbMulti2.control.weight =
			container->Get<double>("transform_mandelbulbMulti2_weight");

	transform.benesiMagForwardTransformOne1.control.enabled =
			container->Get<bool>("transform_benesiMagForwardTransformOne1_enabled");
	transform.benesiMagForwardTransformOne1.control.weightEnabled =
			container->Get<bool>("transform_benesiMagForwardTransformOne1_weight_enabled");
	transform.benesiMagForwardTransformOne1.control.startIterations =
			container->Get<int>("transform_benesiMagForwardTransformOne1_start_iterations");
	transform.benesiMagForwardTransformOne1.control.stopIterations =
			container->Get<int>("transform_benesiMagForwardTransformOne1_stop_iterations");
	transform.benesiMagForwardTransformOne1.control.weight =
			container->Get<double>("transform_benesiMagForwardTransformOne1_weight");
	transform.benesiMagForwardTransformOne2.control.enabled =
			container->Get<bool>("transform_benesiMagForwardTransformOne2_enabled");
	transform.benesiMagForwardTransformOne2.control.weightEnabled =
			container->Get<bool>("transform_benesiMagForwardTransformOne2_weight_enabled");
	transform.benesiMagForwardTransformOne2.control.startIterations =
			container->Get<int>("transform_benesiMagForwardTransformOne2_start_iterations");
	transform.benesiMagForwardTransformOne2.control.stopIterations =
			container->Get<int>("transform_benesiMagForwardTransformOne2_stop_iterations");
	transform.benesiMagForwardTransformOne2.control.weight =
			container->Get<double>("transform_benesiMagForwardTransformOne2_weight");
	transform.benesiMagBackTransformOne1.control.enabled =
			container->Get<bool>("transform_benesiMagBackTransformOne1_enabled");
	transform.benesiMagBackTransformOne1.control.weightEnabled =
			container->Get<bool>("transform_benesiMagBackTransformOne1_weight_enabled");
	transform.benesiMagBackTransformOne1.control.startIterations =
			container->Get<int>("transform_benesiMagBackTransformOne1_start_iterations");
	transform.benesiMagBackTransformOne1.control.stopIterations =
			container->Get<int>("transform_benesiMagBackTransformOne1_stop_iterations");
	transform.benesiMagBackTransformOne1.control.weight =
			container->Get<double>("transform_benesiMagBackTransformOne1_weight");
	transform.benesiMagBackTransformOne2.control.enabled =
			container->Get<bool>("transform_benesiMagBackTransformOne2_enabled");
	transform.benesiMagBackTransformOne2.control.weightEnabled =
			container->Get<bool>("transform_benesiMagBackTransformOne2_weight_enabled");
	transform.benesiMagBackTransformOne2.control.startIterations =
			container->Get<int>("transform_benesiMagBackTransformOne2_start_iterations");
	transform.benesiMagBackTransformOne2.control.stopIterations =
			container->Get<int>("transform_benesiMagBackTransformOne2_stop_iterations");
	transform.benesiMagBackTransformOne2.control.weight =
			container->Get<double>("transform_benesiMagBackTransformOne2_weight");

	transform.benesiMagTransformOneEnabled1.control.enabled =
			container->Get<bool>("transform_benesiMagTransformOneEnabled1_enabled");
	transform.benesiMagTransformOneEnabled1.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformOneEnabled1_weight_enabled");
	transform.benesiMagTransformOneEnabled1.offset =
			container->Get<CVector3>("transform_benesiMagTransformOneEnabled1_offset");
	transform.benesiMagTransformOneEnabled1.scale =
			container->Get<CVector3>("transform_benesiMagTransformOneEnabled1_scale");
	transform.benesiMagTransformOneEnabled1.control.startIterations =
			container->Get<int>("transform_benesiMagTransformOneEnabled1_start_iterations");
	transform.benesiMagTransformOneEnabled1.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformOneEnabled1_stop_iterations");
	transform.benesiMagTransformOneEnabled1.control.weight =
			container->Get<double>("transform_benesiMagTransformOneEnabled1_weight");
	transform.benesiMagTransformOne1.control.enabled =
			container->Get<bool>("transform_benesiMagTransformOne1_enabled");
	transform.benesiMagTransformOne1.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformOne1_weight_enabled");
	transform.benesiMagTransformOne1.offset =
			container->Get<CVector3>("transform_benesiMagTransformOne1_offset");
	transform.benesiMagTransformOne1.scale =
			container->Get<CVector3>("transform_benesiMagTransformOne1_scale");
	transform.benesiMagTransformOne1.control.startIterations =
			container->Get<int>("transform_benesiMagTransformOne1_start_iterations");
	transform.benesiMagTransformOne1.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformOne1_stop_iterations");
	transform.benesiMagTransformOne1.control.weight =
			container->Get<double>("transform_benesiMagTransformOne1_weight");
	transform.benesiMagTransformOne2.control.enabled =
			container->Get<bool>("transform_benesiMagTransformOne2_enabled");
	transform.benesiMagTransformOne2.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformOne2_weight_enabled");
	transform.benesiMagTransformOne2.offset =
			container->Get<CVector3>("transform_benesiMagTransformOne2_offset");
	transform.benesiMagTransformOne2.scale =
			container->Get<CVector3>("transform_benesiMagTransformOne2_scale");
	transform.benesiMagTransformOne2.control.startIterations =
			container->Get<int>("transform_benesiMagTransformOne2_start_iterations");
	transform.benesiMagTransformOne2.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformOne2_stop_iterations");
	transform.benesiMagTransformOne2.control.weight =
			container->Get<double>("transform_benesiMagTransformOne2_weight");
	transform.benesiMagTransformOne3.control.enabled =
			container->Get<bool>("transform_benesiMagTransformOne3_enabled");
	transform.benesiMagTransformOne3.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformOne3_weight_enabled");
	transform.benesiMagTransformOne3.offset =
			container->Get<CVector3>("transform_benesiMagTransformOne3_offset");
	transform.benesiMagTransformOne3.scale =
			container->Get<CVector3>("transform_benesiMagTransformOne3_scale");
	transform.benesiMagTransformOne3.control.startIterations =
			container->Get<int>("transform_benesiMagTransformOne3_start_iterations");
	transform.benesiMagTransformOne3.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformOne3_stop_iterations");
	transform.benesiMagTransformOne3.control.weight =
			container->Get<double>("transform_benesiMagTransformOne3_weight");
	transform.benesiMagTransformOnePlusMinus1.control.enabled =
			container->Get<bool>("transform_benesiMagTransformOnePlusMinus1_enabled");
	transform.benesiMagTransformOnePlusMinus1.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformOnePlusMinus1_weight_enabled");
	transform.benesiMagTransformOnePlusMinus1.offset =
			container->Get<CVector3>("transform_benesiMagTransformOnePlusMinus1_offset");
	transform.benesiMagTransformOnePlusMinus1.scale =
			container->Get<CVector3>("transform_benesiMagTransformOnePlusMinus1_scale");
	transform.benesiMagTransformOnePlusMinus1.control.startIterations =
			container->Get<int>("transform_benesiMagTransformOnePlusMinus1_start_iterations");
	transform.benesiMagTransformOnePlusMinus1.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformOnePlusMinus1_stop_iterations");
	transform.benesiMagTransformOnePlusMinus1.control.weight =
			container->Get<double>("transform_benesiMagTransformOnePlusMinus1_weight");
	transform.benesiMagTransformTwo1.control.enabled =
			container->Get<bool>("transform_benesiMagTransformTwo1_enabled");
	transform.benesiMagTransformTwo1.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformTwo1_weight_enabled");
	transform.benesiMagTransformTwo1.offset =
			container->Get<CVector3>("transform_benesiMagTransformTwo1_offset");
	transform.benesiMagTransformTwo1.scale =
			container->Get<CVector3>("transform_benesiMagTransformTwo1_scale");
	transform.benesiMagTransformTwo1.control.startIterations =
			container->Get<int>("transform_benesiMagTransformTwo1_start_iterations");
	transform.benesiMagTransformTwo1.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformTwo1_stop_iterations");
	transform.benesiMagTransformTwo1.control.weight =
			container->Get<double>("transform_benesiMagTransformTwo1_weight");
	transform.benesiMagTransformThree1.control.enabled =
			container->Get<bool>("transform_benesiMagTransformThree1_enabled");
	transform.benesiMagTransformThree1.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformThree1_weight_enabled");
	transform.benesiMagTransformThree1.offset =
			container->Get<CVector3>("transform_benesiMagTransformThree1_offset");
	transform.benesiMagTransformThree1.scale =
			container->Get<CVector3>("transform_benesiMagTransformThree1_scale");
	transform.benesiMagTransformThree1.control.startIterations =
			container->Get<int>("transform_benesiMagTransformThree1_start_iterations");
	transform.benesiMagTransformThree1.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformThree1_stop_iterations");
	transform.benesiMagTransformThree1.control.weight =
			container->Get<double>("transform_benesiMagTransformThree1_weight");
	transform.benesiMagTransformThree2.control.enabled =
			container->Get<bool>("transform_benesiMagTransformThree2_enabled");
	transform.benesiMagTransformThree2.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformThree2_weight_enabled");
	transform.benesiMagTransformThree2.offset =
			container->Get<CVector3>("transform_benesiMagTransformThree2_offset");
	transform.benesiMagTransformThree2.scale =
			container->Get<CVector3>("transform_benesiMagTransformThree2_scale");
	transform.benesiMagTransformThree2.control.startIterations =
			container->Get<int>("transform_benesiMagTransformThree2_start_iterations");
	transform.benesiMagTransformThree2.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformThree2_stop_iterations");
	transform.benesiMagTransformThree2.control.weight =
			container->Get<double>("transform_benesiMagTransformThree2_weight");
	transform.benesiMagTransformFour1.control.enabled =
			container->Get<bool>("transform_benesiMagTransformFour1_enabled");
	transform.benesiMagTransformFour1.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformFour1_weight_enabled");
	transform.benesiMagTransformFour1.offset =
			container->Get<CVector3>("transform_benesiMagTransformFour1_offset");
	transform.benesiMagTransformFour1.scale =
			container->Get<CVector3>("transform_benesiMagTransformFour1_scale");
	transform.benesiMagTransformFour1.control.startIterations =
			container->Get<int>("transform_benesiMagTransformFour1_start_iterations");
	transform.benesiMagTransformFour1.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformFour1_stop_iterations");
	transform.benesiMagTransformFour1.control.weight =
			container->Get<double>("transform_benesiMagTransformFour1_weight");
	transform.benesiMagTransformFour2.control.enabled =
			container->Get<bool>("transform_benesiMagTransformFour2_enabled");
	transform.benesiMagTransformFour2.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformFour2_weight_enabled");
	transform.benesiMagTransformFour2.offset =
			container->Get<CVector3>("transform_benesiMagTransformFour2_offset");
	transform.benesiMagTransformFour2.scale =
			container->Get<CVector3>("transform_benesiMagTransformFour2_scale");
	transform.benesiMagTransformFour2.control.startIterations =
			container->Get<int>("transform_benesiMagTransformFour2_start_iterations");
	transform.benesiMagTransformFour2.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformFour2_stop_iterations");
	transform.benesiMagTransformFour2.control.weight =
			container->Get<double>("transform_benesiMagTransformFour2_weight");
	transform.benesiMagTransformFiveB1.control.enabled =
			container->Get<bool>("transform_benesiMagTransformFiveB1_enabled");
	transform.benesiMagTransformFiveB1.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformFiveB1_weight_enabled");
	transform.benesiMagTransformFiveB1.offset =
			container->Get<CVector3>("transform_benesiMagTransformFiveB1_offset");
	transform.benesiMagTransformFiveB1.scale =
			container->Get<CVector3>("transform_benesiMagTransformFiveB1_scale");
	transform.benesiMagTransformFiveB1.powOne =
			container->Get<CVector3>("transform_benesiMagTransformFiveB1_powOne");
	transform.benesiMagTransformFiveB1.powTwo =
			container->Get<CVector3>("transform_benesiMagTransformFiveB1_powTwo");
	transform.benesiMagTransformFiveB1.control.startIterations =
			container->Get<int>("transform_benesiMagTransformFiveB1_start_iterations");
	transform.benesiMagTransformFiveB1.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformFiveB1_stop_iterations");
	transform.benesiMagTransformFiveB1.control.weight =
			container->Get<double>("transform_benesiMagTransformFiveB1_weight");
	transform.benesiMagTransformFiveB2.control.enabled =
			container->Get<bool>("transform_benesiMagTransformFiveB2_enabled");
	transform.benesiMagTransformFiveB2.control.weightEnabled =
			container->Get<bool>("transform_benesiMagTransformFiveB2_weight_enabled");
	transform.benesiMagTransformFiveB2.offset =
			container->Get<CVector3>("transform_benesiMagTransformFiveB2_offset");
	transform.benesiMagTransformFiveB2.scale =
			container->Get<CVector3>("transform_benesiMagTransformFiveB2_scale");
	transform.benesiMagTransformFiveB2.powOne =
			container->Get<CVector3>("transform_benesiMagTransformFiveB2_powOne");
	transform.benesiMagTransformFiveB2.powTwo =
			container->Get<CVector3>("transform_benesiMagTransformFiveB2_powTwo");
	transform.benesiMagTransformFiveB2.control.startIterations =
			container->Get<int>("transform_benesiMagTransformFiveB2_start_iterations");
	transform.benesiMagTransformFiveB2.control.stopIterations =
			container->Get<int>("transform_benesiMagTransformFiveB2_stop_iterations");
	transform.benesiMagTransformFiveB2.control.weight =
			container->Get<double>("transform_benesiMagTransformFiveB2_weight");

	transform.benesiFastPwr2PineTreeEnabled1.control.enabled =
			container->Get<bool>("transform_benesiFastPwr2PineTreeEnabled1_enabled");
	transform.benesiFastPwr2PineTreeEnabled1.control.weightEnabled =
			container->Get<bool>("transform_benesiFastPwr2PineTreeEnabled1_weight_enabled");
	transform.benesiFastPwr2PineTreeEnabled1.control.startIterations =
			container->Get<int>("transform_benesiFastPwr2PineTreeEnabled1_start_iterations");
	transform.benesiFastPwr2PineTreeEnabled1.control.stopIterations =
			container->Get<int>("transform_benesiFastPwr2PineTreeEnabled1_stop_iterations");
	transform.benesiFastPwr2PineTreeEnabled1.control.weight =
			container->Get<double>("transform_benesiFastPwr2PineTreeEnabled1_weight");
	transform.benesiFastPwr2PineTreeEnabled1.constantMultiplierVect =
			container->Get<CVector3>("transform_benesiFastPwr2PineTreeEnabled1_constantMultiplier");

	transform.benesiFastPwr2PineTree1.control.enabled =
			container->Get<bool>("transform_benesiFastPwr2PineTree1_enabled");
	transform.benesiFastPwr2PineTree1.control.weightEnabled =
			container->Get<bool>("transform_benesiFastPwr2PineTree1_weight_enabled");
	transform.benesiFastPwr2PineTree1.control.startIterations =
			container->Get<int>("transform_benesiFastPwr2PineTree1_start_iterations");
	transform.benesiFastPwr2PineTree1.control.stopIterations =
			container->Get<int>("transform_benesiFastPwr2PineTree1_stop_iterations");
	transform.benesiFastPwr2PineTree1.control.weight =
			container->Get<double>("transform_benesiFastPwr2PineTree1_weight");
	transform.benesiFastPwr2PineTree1.constantMultiplierVect =
			container->Get<CVector3>("transform_benesiFastPwr2PineTree1_constantMultiplier");
	transform.benesiFastPwr2PineTree2.control.enabled =
			container->Get<bool>("transform_benesiFastPwr2PineTree2_enabled");
	transform.benesiFastPwr2PineTree2.control.weightEnabled =
			container->Get<bool>("transform_benesiFastPwr2PineTree2_weight_enabled");
	transform.benesiFastPwr2PineTree2.control.startIterations =
			container->Get<int>("transform_benesiFastPwr2PineTree2_start_iterations");
	transform.benesiFastPwr2PineTree2.control.stopIterations =
			container->Get<int>("transform_benesiFastPwr2PineTree2_stop_iterations");
	transform.benesiFastPwr2PineTree2.control.weight =
			container->Get<double>("transform_benesiFastPwr2PineTree2_weight");
	transform.benesiFastPwr2PineTree2.constantMultiplierVect =
			container->Get<CVector3>("transform_benesiFastPwr2PineTree2_constantMultiplier");
	transform.benesiFastPwr2PineTree3.control.enabled =
			container->Get<bool>("transform_benesiFastPwr2PineTree3_enabled");
	transform.benesiFastPwr2PineTree3.control.weightEnabled =
			container->Get<bool>("transform_benesiFastPwr2PineTree3_weight_enabled");
	transform.benesiFastPwr2PineTree3.control.startIterations =
			container->Get<int>("transform_benesiFastPwr2PineTree3_start_iterations");
	transform.benesiFastPwr2PineTree3.control.stopIterations =
			container->Get<int>("transform_benesiFastPwr2PineTree3_stop_iterations");
	transform.benesiFastPwr2PineTree3.control.weight =
			container->Get<double>("transform_benesiFastPwr2PineTree3_weight");
	transform.benesiFastPwr2PineTree3.constantMultiplierVect =
			container->Get<CVector3>("transform_benesiFastPwr2PineTree3_constantMultiplier");

	transform.boxFoldMulti1.control.enabled =
			container->Get<bool>("transform_box_foldMulti1_enabled");
	transform.boxFoldMulti1.control.weightEnabled =
			container->Get<bool>("transform_box_foldMulti1_weight_enabled");
	transform.boxFoldMulti1.control.startIterations =
			container->Get<int>("transform_box_foldMulti1_start_iterations");
	transform.boxFoldMulti1.control.stopIterations =
			container->Get<int>("transform_box_foldMulti1_stop_iterations");
	transform.boxFoldMulti1.foldingLimit =
			container->Get<CVector3>("transform_box_foldMulti1_folding_limit");
	transform.boxFoldMulti1.foldingValue =
			container->Get<CVector3>("transform_box_foldMulti1_folding_value");
	transform.boxFoldMulti1.control.weight =
			container->Get<double>("transform_box_foldMulti1_weight");
	transform.boxFoldMulti1.color = mandelbox.color;

	transform.boxFoldMulti2.control.enabled =
			container->Get<bool>("transform_box_foldMulti2_enabled");
	transform.boxFoldMulti2.control.weightEnabled =
			container->Get<bool>("transform_box_foldMulti2_weight_enabled");
	transform.boxFoldMulti2.control.startIterations =
			container->Get<int>("transform_box_foldMulti2_start_iterations");
	transform.boxFoldMulti2.control.stopIterations =
			container->Get<int>("transform_box_foldMulti2_stop_iterations");
	transform.boxFoldMulti2.foldingLimit =
			container->Get<CVector3>("transform_box_foldMulti2_folding_limit");
	transform.boxFoldMulti2.foldingValue =
			container->Get<CVector3>("transform_box_foldMulti2_folding_value");
	transform.boxFoldMulti2.control.weight =
			container->Get<double>("transform_box_foldMulti2_weight");
	transform.boxFoldMulti2.color = mandelbox.color;

	transform.cubeSphere1.control.enabled = container->Get<bool>("transform_cubeSphere1_enabled");
	transform.cubeSphere1.control.weightEnabled =
			container->Get<bool>("transform_cubeSphere1_weight_enabled");
	transform.cubeSphere1.control.startIterations =
			container->Get<int>("transform_cubeSphere1_start_iterations");
	transform.cubeSphere1.control.stopIterations =
			container->Get<int>("transform_cubeSphere1_stop_iterations");
	transform.cubeSphere1.control.weight = container->Get<double>("transform_cubeSphere1_weight");
	transform.cubeSphere2.control.enabled = container->Get<bool>("transform_cubeSphere2_enabled");
	transform.cubeSphere2.control.weightEnabled =
			container->Get<bool>("transform_cubeSphere2_weight_enabled");
	transform.cubeSphere2.control.startIterations =
			container->Get<int>("transform_cubeSphere2_start_iterations");
	transform.cubeSphere2.control.stopIterations =
			container->Get<int>("transform_cubeSphere2_stop_iterations");
	transform.cubeSphere2.control.weight = container->Get<double>("transform_cubeSphere2_weight");

	transform.sphereCube1.control.enabled = container->Get<bool>("transform_sphereCube1_enabled");
	transform.sphereCube1.control.weightEnabled =
			container->Get<bool>("transform_sphereCube1_weight_enabled");
	transform.sphereCube1.control.startIterations =
			container->Get<int>("transform_sphereCube1_start_iterations");
	transform.sphereCube1.control.stopIterations =
			container->Get<int>("transform_sphereCube1_stop_iterations");
	transform.sphereCube1.control.weight = container->Get<double>("transform_sphereCube1_weight");
	transform.sphereCube2.control.enabled = container->Get<bool>("transform_sphereCube2_enabled");
	transform.sphereCube2.control.weightEnabled =
			container->Get<bool>("transform_sphereCube2_weight_enabled");
	transform.sphereCube2.control.startIterations =
			container->Get<int>("transform_sphereCube2_start_iterations");
	transform.sphereCube2.control.stopIterations =
			container->Get<int>("transform_sphereCube2_stop_iterations");
	transform.sphereCube2.control.weight = container->Get<double>("transform_sphereCube2_weight");

  transform.aboxModKali1.additionConstant =
      container->Get<CVector3>("transform_aboxModKali1_additionConstant");
  transform.aboxModKali1.radMin = container->Get<double>("transform_aboxModKali1_min_radius");
  transform.aboxModKali1.scale = container->Get<double>("transform_aboxModKali1_scale");
  transform.aboxModKali1.constantMultiplierVect =
      container->Get<CVector3>("transform_aboxModKali1_constantMultiplier");

  transform.fabsBoxMod.constantF1 =
                  container->Get<CVector3>("transform_fabsBoxMod_constantF1");
  transform.fabsBoxMod.constantF2 =
                  container->Get<CVector3>("transform_fabsBoxMod_constantF2");
  transform.fabsBoxMod.constantF3 =
                  container->Get<CVector3>("transform_fabsBoxMod_constantF3");
  transform.fabsBoxMod.control.weightEnabled =
                  container->Get<bool>("transform_fabsBoxMod_weight_enabled");
  transform.fabsBoxMod.constantF1Enabledx =
                  container->Get<bool>("transform_fabsBoxMod_constantF1Enabledx");
  transform.fabsBoxMod.constantF1Enabledy =
                  container->Get<bool>("transform_fabsBoxMod_constantF1Enabledy");
  transform.fabsBoxMod.constantF1Enabledz =
                  container->Get<bool>("transform_fabsBoxMod_constantF1Enabledz");
  transform.fabsBoxMod.constantF2Enabledx =
                  container->Get<bool>("transform_fabsBoxMod_constantF2Enabledx");
  transform.fabsBoxMod.constantF2Enabledy =
                  container->Get<bool>("transform_fabsBoxMod_constantF2Enabledy");
  transform.fabsBoxMod.constantF2Enabledz =
                  container->Get<bool>("transform_fabsBoxMod_constantF2Enabledz");
  transform.fabsBoxMod.control.startIterations =
                  container->Get<int>("transform_fabsBoxMod_start_iterations");
  transform.fabsBoxMod.control.stopIterations =
                  container->Get<int>("transform_fabsBoxMod_stop_iterations");
  transform.fabsBoxMod.control.weight =
                  container->Get<double>("transform_fabsBoxMod_weight");

  transform.aboxModKali1.additionConstant =
      container->Get<CVector3>("transform_aboxModKali1_additionConstant");
  transform.aboxModKali1.radMin = container->Get<double>("transform_aboxModKali1_min_radius");
  transform.aboxModKali1.scale = container->Get<double>("transform_aboxModKali1_scale");
  transform.aboxModKali1.constantMultiplierVect =
      container->Get<CVector3>("transform_aboxModKali1_constantMultiplier");

//----------------------------------

	//platonic_solid

	platonicSolid.frequency = container->Get<double>("platonic_solid_frequency");
	platonicSolid.amplitude = container->Get<double>("platonic_solid_amplitude");
	platonicSolid.rhoMul = container->Get<double>("platonic_solid_rhoMul");

	WriteLog("cFractal::RecalculateFractalParams(void)");
	RecalculateFractalParams();
}
void cFractal::RecalculateFractalParams(void)
{
	IFS.mainRot.SetRotation3(IFS.rotation * (M_PI / 180.0));

	for (int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		IFS.rot[i].SetRotation3(IFS.rotations[i] * (M_PI / 180.0));
		IFS.direction[i].Normalize();
	}

	mandelbox.mainRot.SetRotation2(mandelbox.rotationMain * (M_PI / 180.0));

	for (int fold = 0; fold < MANDELBOX_FOLDS; ++fold)
	{
		for (int axis = 0; axis < 3; ++axis)
		{
			mandelbox.rot[fold][axis].SetRotation2(mandelbox.rotation[fold][axis] * (M_PI / 180.0));
			mandelbox.rotinv[fold][axis] = mandelbox.rot[fold][axis].Transpose();
		}
	}
	mandelbox.fR2 = mandelbox.foldingSphericalFixed * mandelbox.foldingSphericalFixed;
	mandelbox.mR2 = mandelbox.foldingSphericalMin * mandelbox.foldingSphericalMin;
	mandelbox.mboxFactor1 = mandelbox.fR2 / mandelbox.mR2;

	bulb.alphaAngleOffset *= M_PI / 180.0;
	bulb.betaAngleOffset *= M_PI / 180.0;

	transform.mandelbulbOriginal1.alphaAngleOffset *= M_PI / 180.0;
	transform.mandelbulbOriginal1.betaAngleOffset *= M_PI / 180.0;
	transform.mandelbulb1.alphaAngleOffset *= M_PI / 180.0;
	transform.mandelbulb1.betaAngleOffset *= M_PI / 180.0;
	transform.mandelbulbMulti1.alphaAngleOffset *= M_PI / 180.0;
	transform.mandelbulbMulti1.betaAngleOffset *= M_PI / 180.0;
	transform.variableMandelbulbPower1.alphaAngleOffset *= M_PI / 180.0;
	transform.variableMandelbulbPower1.betaAngleOffset *= M_PI / 180.0;

	transform.mainRotation1.mainRot.SetRotation2(transform.mainRotation1.mainRotation
			* (M_PI / 180.0));
	transform.mainRotation2.mainRot.SetRotation2(transform.mainRotation2.mainRotation
			* (M_PI / 180.0));
	transform.mainRotation3.mainRot.SetRotation2(transform.mainRotation3.mainRotation
			* (M_PI / 180.0));
	transform.mainRotation4.mainRot.SetRotation2(transform.mainRotation4.mainRotation
			* (M_PI / 180.0));
	transform.mandelbulbMulti1.mainRot.SetRotation2(transform.mandelbulbMulti1.mainRotation
			* (M_PI / 180.0));
	transform.mandelbulbMulti2.mainRot.SetRotation2(transform.mandelbulbMulti2.mainRotation
			* (M_PI / 180.0));

	transform.sphericalFoldOriginal1.fR2 = transform.sphericalFoldOriginal1.radFixed
			* transform.sphericalFoldOriginal1.radFixed;
	transform.sphericalFoldOriginal1.mR2 = transform.sphericalFoldOriginal1.radMin
			* transform.sphericalFoldOriginal1.radMin;
	transform.sphericalFoldOriginal1.mboxFactor1 = transform.sphericalFoldOriginal1.fR2
			/ transform.sphericalFoldOriginal1.mR2;
	transform.sphericalFold1.fR2 = transform.sphericalFold1.radFixed
			* transform.sphericalFold1.radFixed;
	transform.sphericalFold1.mR2 = transform.sphericalFold1.radMin * transform.sphericalFold1.radMin;
	transform.sphericalFold1.mboxFactor1 = transform.sphericalFold1.fR2
			/ transform.sphericalFold1.mR2;
	transform.sphericalFold2.fR2 = transform.sphericalFold2.radFixed
			* transform.sphericalFold2.radFixed;
	transform.sphericalFold2.mR2 = transform.sphericalFold2.radMin * transform.sphericalFold2.radMin;
	transform.sphericalFold2.mboxFactor1 = transform.sphericalFold2.fR2
			/ transform.sphericalFold2.mR2;

	//Generalized Fold Box precalculated vectors
	double sqrt_i3 = 1.0 / sqrt(3.0);
	genFoldBox.Nv_tet[0] = CVector3(sqrt_i3, sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_tet[1] = CVector3(sqrt_i3, -sqrt_i3, sqrt_i3);
	genFoldBox.Nv_tet[2] = CVector3(-sqrt_i3, sqrt_i3, sqrt_i3);
	genFoldBox.Nv_tet[3] = CVector3(-sqrt_i3, -sqrt_i3, -sqrt_i3);
	genFoldBox.sides_tet = 4;

	genFoldBox.Nv_cube[0] = CVector3(1, 0, 0);
	genFoldBox.Nv_cube[1] = CVector3(-1, 0, 0);
	genFoldBox.Nv_cube[2] = CVector3(0, 1, 0);
	genFoldBox.Nv_cube[3] = CVector3(0, -1, 0);
	genFoldBox.Nv_cube[4] = CVector3(0, 0, 1);
	genFoldBox.Nv_cube[5] = CVector3(0, 0, -1);
	genFoldBox.sides_cube = 6;

	genFoldBox.Nv_oct[0] = CVector3(sqrt_i3, sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_oct[1] = CVector3(sqrt_i3, -sqrt_i3, sqrt_i3);
	genFoldBox.Nv_oct[2] = CVector3(-sqrt_i3, sqrt_i3, sqrt_i3);
	genFoldBox.Nv_oct[3] = CVector3(-sqrt_i3, -sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_oct[4] = CVector3(sqrt_i3, sqrt_i3, sqrt_i3);
	genFoldBox.Nv_oct[5] = CVector3(-sqrt_i3, -sqrt_i3, sqrt_i3);
	genFoldBox.Nv_oct[6] = CVector3(-sqrt_i3, sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_oct[7] = CVector3(sqrt_i3, -sqrt_i3, -sqrt_i3);
	genFoldBox.sides_oct = 8;

	genFoldBox.Nv_oct_cube[0] = CVector3(sqrt_i3, sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_oct_cube[1] = CVector3(sqrt_i3, -sqrt_i3, sqrt_i3);
	genFoldBox.Nv_oct_cube[2] = CVector3(-sqrt_i3, sqrt_i3, sqrt_i3);
	genFoldBox.Nv_oct_cube[3] = CVector3(-sqrt_i3, -sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_oct_cube[4] = CVector3(sqrt_i3, sqrt_i3, sqrt_i3);
	genFoldBox.Nv_oct_cube[5] = CVector3(-sqrt_i3, -sqrt_i3, sqrt_i3);
	genFoldBox.Nv_oct_cube[6] = CVector3(-sqrt_i3, sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_oct_cube[7] = CVector3(sqrt_i3, -sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_oct_cube[8] = CVector3(1, 0, 0);
	genFoldBox.Nv_oct_cube[9] = CVector3(-1, 0, 0);
	genFoldBox.Nv_oct_cube[10] = CVector3(0, 1, 0);
	genFoldBox.Nv_oct_cube[11] = CVector3(0, -1, 0);
	genFoldBox.Nv_oct_cube[12] = CVector3(0, 0, 1);
	genFoldBox.Nv_oct_cube[13] = CVector3(0, 0, -1);
	genFoldBox.sides_oct_cube = 14;

	double aa = ((1.0 + sqrt(5.0)) / 2.0);
	double bb = (1.0 / sqrt(aa * aa + 1.0));
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

	double ff = sqrt(aa * aa + 1.0 / (aa * aa));
	double cc = aa / ff;
	double dd = 1.0 / aa / ff;

	genFoldBox.Nv_icosa[0] = CVector3(sqrt_i3, sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_icosa[1] = CVector3(sqrt_i3, -sqrt_i3, sqrt_i3);
	genFoldBox.Nv_icosa[2] = CVector3(-sqrt_i3, sqrt_i3, sqrt_i3);
	genFoldBox.Nv_icosa[3] = CVector3(-sqrt_i3, -sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_icosa[4] = CVector3(sqrt_i3, sqrt_i3, sqrt_i3);
	genFoldBox.Nv_icosa[5] = CVector3(-sqrt_i3, -sqrt_i3, sqrt_i3);
	genFoldBox.Nv_icosa[6] = CVector3(-sqrt_i3, sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_icosa[7] = CVector3(sqrt_i3, -sqrt_i3, -sqrt_i3);
	genFoldBox.Nv_icosa[8] = CVector3(0, dd, cc);
	genFoldBox.Nv_icosa[9] = CVector3(0, dd, -cc);
	genFoldBox.Nv_icosa[10] = CVector3(0, -dd, cc);
	genFoldBox.Nv_icosa[11] = CVector3(0, -dd, -cc);
	genFoldBox.Nv_icosa[12] = CVector3(cc, 0, dd);
	genFoldBox.Nv_icosa[13] = CVector3(cc, 0, -dd);
	genFoldBox.Nv_icosa[14] = CVector3(-cc, 0, dd);
	genFoldBox.Nv_icosa[15] = CVector3(-cc, 0, -dd);
	genFoldBox.Nv_icosa[16] = CVector3(dd, cc, 0);
	genFoldBox.Nv_icosa[17] = CVector3(dd, -cc, 0);
	genFoldBox.Nv_icosa[18] = CVector3(-dd, cc, 0);
	genFoldBox.Nv_icosa[19] = CVector3(-dd, -cc, 0);
	genFoldBox.sides_icosa = 20;

	double tha = M_PI * 2.0 / 6.0;

	genFoldBox.Nv_box6[0] = CVector3(0, 0, 1);
	genFoldBox.Nv_box6[1] = CVector3(0, 0, -1);
	genFoldBox.Nv_box6[2] = CVector3(cos(tha * 0), sin(tha * 0), 0);
	genFoldBox.Nv_box6[3] = CVector3(cos(tha * 1), sin(tha * 1), 0);
	genFoldBox.Nv_box6[4] = CVector3(cos(tha * 2), sin(tha * 2), 0);
	genFoldBox.Nv_box6[5] = CVector3(cos(tha * 3), sin(tha * 3), 0);
	genFoldBox.Nv_box6[6] = CVector3(cos(tha * 4), sin(tha * 4), 0);
	genFoldBox.Nv_box6[7] = CVector3(cos(tha * 5), sin(tha * 5), 0);
	genFoldBox.sides_box6 = 8;

	double tha5 = M_PI * 2.0 / 5.0;

	genFoldBox.Nv_box5[0] = CVector3(0, 0, 1);
	genFoldBox.Nv_box5[1] = CVector3(0, 0, -1);
	genFoldBox.Nv_box5[2] = CVector3(cos(tha5 * 0), sin(tha5 * 0), 0);
	genFoldBox.Nv_box5[3] = CVector3(cos(tha5 * 1), sin(tha5 * 1), 0);
	genFoldBox.Nv_box5[4] = CVector3(cos(tha5 * 2), sin(tha5 * 2), 0);
	genFoldBox.Nv_box5[5] = CVector3(cos(tha5 * 3), sin(tha5 * 3), 0);
	genFoldBox.Nv_box5[6] = CVector3(cos(tha5 * 4), sin(tha5 * 4), 0);
	genFoldBox.sides_box5 = 7;
}
