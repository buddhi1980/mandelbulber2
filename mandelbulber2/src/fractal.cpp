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

	mandelbox.solid = container->Get<double>("mandelbox_solid");
	mandelbox.melt = container->Get<double>("mandelbox_melt");
	genFoldBox.type = (fractal::enumGeneralizedFoldBoxType)container->Get<int>("mandelbox_generalized_fold_type");

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

	buffalo.preabsx = container->Get<bool>("buffalo_preabs_x");
	buffalo.preabsy = container->Get<bool>("buffalo_preabs_y");
	buffalo.preabsz = container->Get<bool>("buffalo_preabs_z");
	buffalo.absx = container->Get<bool>("buffalo_abs_x");
	buffalo.absy = container->Get<bool>("buffalo_abs_y");
	buffalo.absz = container->Get<bool>("buffalo_abs_z");
	buffalo.posz = container->Get<bool>("buffalo_pos_z");

	msltoeSym2.y_multiplier = container->Get<double>("msltoesym2_y_multiplier");

  transform.additionConstant1.control.enabled = container->Get<bool>("transform_additionConstant1_enabled");
  transform.additionConstant1.control.weightEnabled = container->Get<bool>("transform_additionConstant1_weight_enabled");
  transform.additionConstant1.control.startIterations = container->Get<int>("transform_additionConstant1_start_iterations");
  transform.additionConstant1.control.stopIterations = container->Get<int>("transform_additionConstant1_stop_iterations");
  transform.additionConstant1.control.weight = container->Get<double>("transform_additionConstant1_weight");
  transform.additionConstant1.additionConstant = container->Get<CVector3>("transform_additionConstant1");
  transform.additionConstant2.additionConstant = container->Get<CVector3>("transform_additionConstant2");
  transform.additionConstant2.control.enabled = container->Get<bool>("transform_additionConstant2_enabled");
  transform.additionConstant2.control.weightEnabled = container->Get<bool>("transform_additionConstant2_weight_enabled");
  transform.additionConstant2.control.startIterations = container->Get<int>("transform_additionConstant2_start_iterations");
  transform.additionConstant2.control.stopIterations = container->Get<int>("transform_additionConstant2_stop_iterations");
  transform.additionConstant2.control.weight = container->Get<double>("transform_additionConstant2_weight");
  //extra for 4D
  transform.additionConstant1.additionConstant4D = CVector4(container->Get<CVector3>("transform_additionConstant1"), container->Get<double>("transform_additionConstant1W"));
  transform.additionConstant2.additionConstant4D = CVector4(container->Get<CVector3>("transform_additionConstant2"), container->Get<double>("transform_additionConstant2W"));


  transform.boxConstantMultiplier1.control.enabled = container->Get<bool>("transform_boxConstantMultiplier1_enabled");
  transform.boxConstantMultiplier1.boxConstantMultiplierFabsEnabledX = container->Get<bool>("transform_boxConstantMultiplier1_fabs_enabledX");
  transform.boxConstantMultiplier1.boxConstantMultiplierFabsEnabledY = container->Get<bool>("transform_boxConstantMultiplier1_fabs_enabledY");
  transform.boxConstantMultiplier1.boxConstantMultiplierFabsEnabledZ = container->Get<bool>("transform_boxConstantMultiplier1_fabs_enabledZ");
  transform.boxConstantMultiplier1.control.weightEnabled = container->Get<bool>("transform_boxConstantMultiplier1_weight_enabled");
  transform.boxConstantMultiplier1.control.startIterations = container->Get<int>("transform_boxConstantMultiplier1_start_iterations");
  transform.boxConstantMultiplier1.control.stopIterations = container->Get<int>("transform_boxConstantMultiplier1_stop_iterations");
  transform.boxConstantMultiplier1.control.weight = container->Get<double>("transform_boxConstantMultiplier1_weight");
  transform.boxConstantMultiplier1.boxConstantMultiplier = container->Get<CVector3>("transform_boxConstantMultiplier1");
  transform.boxConstantMultiplier2.control.enabled = container->Get<bool>("transform_boxConstantMultiplier2_enabled");
  transform.boxConstantMultiplier2.boxConstantMultiplierFabsEnabledX = container->Get<bool>("transform_boxConstantMultiplier2_fabs_enabledX");
  transform.boxConstantMultiplier2.boxConstantMultiplierFabsEnabledY = container->Get<bool>("transform_boxConstantMultiplier2_fabs_enabledY");
  transform.boxConstantMultiplier2.boxConstantMultiplierFabsEnabledZ = container->Get<bool>("transform_boxConstantMultiplier2_fabs_enabledZ");
  transform.boxConstantMultiplier2.control.weightEnabled = container->Get<bool>("transform_boxConstantMultiplier2_weight_enabled");
  transform.boxConstantMultiplier2.control.startIterations = container->Get<int>("transform_boxConstantMultiplier2_start_iterations");
  transform.boxConstantMultiplier2.control.stopIterations = container->Get<int>("transform_boxConstantMultiplier2_stop_iterations");
  transform.boxConstantMultiplier2.control.weight = container->Get<double>("transform_boxConstantMultiplier2_weight");
  transform.boxConstantMultiplier2.boxConstantMultiplier = container->Get<CVector3>("transform_boxConstantMultiplier2");

  transform.boxFoldOriginal1.control.enabled = container->Get<bool>("transform_box_foldOriginal1_enabled");
  transform.boxFoldOriginal1.control.weightEnabled = container->Get<bool>("transform_box_foldOriginal1_weight_enabled");
  transform.boxFoldOriginal1.control.oldWeightEnabled = container->Get<bool>("transform_box_foldOriginal1_old_weight_enabled");
  transform.boxFoldOriginal1.control.startIterations = container->Get<int>("transform_box_foldOriginal1_start_iterations");
  transform.boxFoldOriginal1.control.stopIterations = container->Get<int>("transform_box_foldOriginal1_stop_iterations");
  transform.boxFoldOriginal1.foldingLimit = container->Get<double>("transform_mandelbox_folding_limit");
  transform.boxFoldOriginal1.foldingValue = container->Get<double>("transform_mandelbox_folding_value");
  transform.boxFoldOriginal1.control.weight = container->Get<double>("transform_box_foldOriginal1_weight");
  transform.boxFoldOriginal1.control.oldWeight = container->Get<double>("transform_box_foldOriginal1_old_weight");



  transform.boxFold1.control.enabled = container->Get<bool>("transform_box_fold1_enabled");
  transform.boxFold1.control.weightEnabled = container->Get<bool>("transform_box_fold1_weight_enabled");
  transform.boxFold1.control.oldWeightEnabled = container->Get<bool>("transform_box_fold1_old_weight_enabled");
  transform.boxFold1.control.startIterations = container->Get<int>("transform_box_fold1_start_iterations");
  transform.boxFold1.control.stopIterations = container->Get<int>("transform_box_fold1_stop_iterations");
  transform.boxFold1.foldingLimit = container->Get<double>("transform_box_fold1_folding_limit");
  transform.boxFold1.foldingValue = container->Get<double>("transform_box_fold1_folding_value");
  transform.boxFold1.control.weight = container->Get<double>("transform_box_fold1_weight");
  transform.boxFold1.control.oldWeight = container->Get<double>("transform_box_fold1_old_weight");
  transform.boxFold2.control.enabled = container->Get<bool>("transform_box_fold2_enabled");
  transform.boxFold2.control.weightEnabled = container->Get<bool>("transform_box_fold2_weight_enabled");
  transform.boxFold2.control.oldWeightEnabled = container->Get<bool>("transform_box_fold2_old_weight_enabled");
  transform.boxFold2.control.startIterations = container->Get<int>("transform_box_fold2_start_iterations");
  transform.boxFold2.control.stopIterations = container->Get<int>("transform_box_fold2_stop_iterations");
  transform.boxFold2.foldingLimit = container->Get<double>("transform_box_fold2_folding_limit");
  transform.boxFold2.foldingValue = container->Get<double>("transform_box_fold2_folding_value");
  transform.boxFold2.control.weight = container->Get<double>("transform_box_fold2_weight");
  transform.boxFold2.control.oldWeight = container->Get<double>("transform_box_fold2_old_weight");

  transform.boxOffset1.control.enabled = container->Get<bool>("transform_boxOffset1_enabled");
  transform.boxOffset1.control.weightEnabled = container->Get<bool>("transform_boxOffset1_weight_enabled");
  transform.boxOffset1.control.startIterations = container->Get<int>("transform_boxOffset1_start_iterations");
  transform.boxOffset1.control.stopIterations = container->Get<int>("transform_boxOffset1_stop_iterations");
  transform.boxOffset1.control.weight = container->Get<double>("transform_boxOffset1_weight");
  transform.boxOffset1.boxOffset = container->Get<CVector3>("transform_boxOffset1");
  transform.boxOffset2.control.enabled = container->Get<bool>("transform_boxOffset2_enabled");
  transform.boxOffset2.control.weightEnabled = container->Get<bool>("transform_boxOffset2_weight_enabled");
  transform.boxOffset2.control.startIterations = container->Get<int>("transform_boxOffset2_start_iterations");
  transform.boxOffset2.control.stopIterations = container->Get<int>("transform_boxOffset2_stop_iterations");
  transform.boxOffset2.control.weight = container->Get<double>("transform_boxOffset2_weight");
  transform.boxOffset2.boxOffset = container->Get<CVector3>("transform_boxOffset2");



  transform.constantMultiplierOriginal1.constantMultiplierVect = container->Get<CVector3>("transform_constantMultiplierOriginal1");
  transform.constantMultiplierOriginal1.control.enabled = container->Get<bool>("transform_constantMultiplierOriginal1_enabled");
  transform.constantMultiplierOriginal1.control.weightEnabled = container->Get<bool>("transform_constantMultiplierOriginal1_weight_enabled");
  transform.constantMultiplierOriginal1.control.startIterations = container->Get<int>("transform_constantMultiplierOriginal1_start_iterations");
  transform.constantMultiplierOriginal1.control.stopIterations = container->Get<int>("transform_constantMultiplierOriginal1_stop_iterations");
  transform.constantMultiplierOriginal1.control.weight = container->Get<double>("transform_constantMultiplierOriginal1_weight");
  // for 4D
  transform.constantMultiplierOriginal1.constantMultiplierVect4D = CVector4(container->Get<CVector3>("transform_constantMultiplierOriginal1"), 0.0);

  transform.constantMultiplier1.control.enabled = container->Get<bool>("transform_constantMultiplier1_enabled");
  transform.constantMultiplier1.control.weightEnabled = container->Get<bool>("transform_constantMultiplier1_weight_enabled");
  transform.constantMultiplier1.control.startIterations = container->Get<int>("transform_constantMultiplier1_start_iterations");
  transform.constantMultiplier1.control.stopIterations = container->Get<int>("transform_constantMultiplier1_stop_iterations");
  transform.constantMultiplier1.constantMultiplierVect = container->Get<CVector3>("transform_constantMultiplier1");
  transform.constantMultiplier1.control.weight = container->Get<double>("transform_constantMultiplier1_weight");
  // for 4D
  transform.constantMultiplier1.constantMultiplierVect4D = CVector4(container->Get<CVector3>("transform_constantMultiplier1"), 0.0);

  transform.constantMultiplier2.control.enabled = container->Get<bool>("transform_constantMultiplier2_enabled");
  transform.constantMultiplier2.control.weightEnabled = container->Get<bool>("transform_constantMultiplier2_weight_enabled");
  transform.constantMultiplier2.control.startIterations = container->Get<int>("transform_constantMultiplier2_start_iterations");
  transform.constantMultiplier2.control.stopIterations = container->Get<int>("transform_constantMultiplier2_stop_iterations");
  transform.constantMultiplier2.constantMultiplierVect = container->Get<CVector3>("transform_constantMultiplier2");
  transform.constantMultiplier2.control.weight = container->Get<double>("transform_constantMultiplier2_weight");

  transform.fabsAddConstant1.control.enabled = container->Get<bool>("transform_fabsAddConstant1_enabled");
  transform.fabsAddConstant1.control.weightEnabled = container->Get<bool>("transform_fabsAddConstant1_weight_enabled");
  transform.fabsAddConstant1.control.oldWeightEnabled = container->Get<bool>("transform_fabsAddConstant1_old_weight_enabled");
  transform.fabsAddConstant1.fabsAddConstantEnabledx = container->Get<bool>("transform_fabsAddConstant1_enabled_x");
  transform.fabsAddConstant1.fabsAddConstantEnabledy = container->Get<bool>("transform_fabsAddConstant1_enabled_y");
  transform.fabsAddConstant1.fabsAddConstantEnabledy = container->Get<bool>("transform_fabsAddConstant1_enabled_z");
  transform.fabsAddConstant1.control.startIterations = container->Get<int>("transform_fabsAddConstant1_start_iterations");
  transform.fabsAddConstant1.control.stopIterations = container->Get<int>("transform_fabsAddConstant1_stop_iterations");
  transform.fabsAddConstant1.fabsAddConstantA = container->Get<CVector3>("transform_fabsAddConstant1A");
  transform.fabsAddConstant1.fabsAddConstantB = container->Get<CVector3>("transform_fabsAddConstant1B");
  transform.fabsAddConstant1.fabsAddConstantC = container->Get<CVector3>("transform_fabsAddConstant1C");
  transform.fabsAddConstant1.control.weight = container->Get<double>("transform_fabsAddConstant1_weight");
  transform.fabsAddConstant1.control.oldWeight = container->Get<double>("transform_fabsAddConstant1_old_weight");
  //extra for 4D
  transform.fabsAddConstant1.fabsAddConstant4DA = CVector4(container->Get<CVector3>("transform_fabsAddConstant1A"), container->Get<double>("transform_fabsAddConstant1AW"));
  transform.fabsAddConstant1.fabsAddConstant4DB = CVector4(container->Get<CVector3>("transform_fabsAddConstant1B"), container->Get<double>("transform_fabsAddConstant1BW"));
  transform.fabsAddConstant1.fabsAddConstant4DC = CVector4(container->Get<CVector3>("transform_fabsAddConstant1C"), container->Get<double>("transform_fabsAddConstant1CW"));

  transform.fabsFormulaABCD1.fabsFormulaABCDA = container->Get<CVector3>("transform_fabsFormulaABCD1A");
  transform.fabsFormulaABCD1.fabsFormulaABCDB = container->Get<CVector3>("transform_fabsFormulaABCD1B");
  transform.fabsFormulaABCD1.fabsFormulaABCDC = container->Get<CVector3>("transform_fabsFormulaABCD1C");
  transform.fabsFormulaABCD1.fabsFormulaABCDD = container->Get<CVector3>("transform_fabsFormulaABCD1D");
  transform.fabsFormulaABCD1.control.enabled = container->Get<bool>("transform_fabsFormulaABCD1_enabled");
  transform.fabsFormulaABCD1.control.weightEnabled = container->Get<bool>("transform_fabsFormulaABCD1_weight_enabled");
  transform.fabsFormulaABCD1.fabsFormulaABCDEnabledAx = container->Get<bool>("transform_fabsFormulaABCD1_enabled_Ax");
  transform.fabsFormulaABCD1.fabsFormulaABCDEnabledAy = container->Get<bool>("transform_fabsFormulaABCD1_enabled_Ay");
  transform.fabsFormulaABCD1.fabsFormulaABCDEnabledAz = container->Get<bool>("transform_fabsFormulaABCD1_enabled_Az");
  transform.fabsFormulaABCD1.fabsFormulaABCDEnabledBx = container->Get<bool>("transform_fabsFormulaABCD1_enabled_Bx");
  transform.fabsFormulaABCD1.fabsFormulaABCDEnabledBy = container->Get<bool>("transform_fabsFormulaABCD1_enabled_By");
  transform.fabsFormulaABCD1.fabsFormulaABCDEnabledBz = container->Get<bool>("transform_fabsFormulaABCD1_enabled_Bz");
  transform.fabsFormulaABCD1.control.startIterations = container->Get<int>("transform_fabsFormulaABCD1_start_iterations");
  transform.fabsFormulaABCD1.control.stopIterations = container->Get<int>("transform_fabsFormulaABCD1_stop_iterations");
  transform.fabsFormulaABCD1.control.weight = container->Get<double>("transform_fabsFormulaABCD1_weight");
  transform.fabsFormulaABCD2.fabsFormulaABCDA = container->Get<CVector3>("transform_fabsFormulaABCD2A");
  transform.fabsFormulaABCD2.fabsFormulaABCDB = container->Get<CVector3>("transform_fabsFormulaABCD2B");
  transform.fabsFormulaABCD2.fabsFormulaABCDC = container->Get<CVector3>("transform_fabsFormulaABCD2C");
  transform.fabsFormulaABCD2.fabsFormulaABCDD = container->Get<CVector3>("transform_fabsFormulaABCD2D");
  transform.fabsFormulaABCD2.control.enabled = container->Get<bool>("transform_fabsFormulaABCD2_enabled");
  transform.fabsFormulaABCD2.control.weightEnabled = container->Get<bool>("transform_fabsFormulaABCD2_weight_enabled");
  transform.fabsFormulaABCD2.fabsFormulaABCDEnabledAx = container->Get<bool>("transform_fabsFormulaABCD2_enabled_Ax");
  transform.fabsFormulaABCD2.fabsFormulaABCDEnabledAy = container->Get<bool>("transform_fabsFormulaABCD2_enabled_Ay");
  transform.fabsFormulaABCD2.fabsFormulaABCDEnabledAz = container->Get<bool>("transform_fabsFormulaABCD2_enabled_Az");
  transform.fabsFormulaABCD2.fabsFormulaABCDEnabledBx = container->Get<bool>("transform_fabsFormulaABCD2_enabled_Bx");
  transform.fabsFormulaABCD2.fabsFormulaABCDEnabledBy = container->Get<bool>("transform_fabsFormulaABCD2_enabled_By");
  transform.fabsFormulaABCD2.fabsFormulaABCDEnabledBz = container->Get<bool>("transform_fabsFormulaABCD2_enabled_Bz");
  transform.fabsFormulaABCD2.control.startIterations = container->Get<int>("transform_fabsFormulaABCD2_start_iterations");
  transform.fabsFormulaABCD2.control.stopIterations = container->Get<int>("transform_fabsFormulaABCD2_stop_iterations");
  transform.fabsFormulaABCD2.control.weight = container->Get<double>("transform_fabsFormulaABCD2_weight");
// extra for 4D
  transform.fabsFormulaABCD1.fabsFormulaABCD4DA = CVector4(container->Get<CVector3>("transform_fabsFormulaABCD1A"), container->Get<double>("transform_fabsFormulaABCD1AW")) ;
  transform.fabsFormulaABCD1.fabsFormulaABCD4DB = CVector4(container->Get<CVector3>("transform_fabsFormulaABCD1B"), container->Get<double>("transform_fabsFormulaABCD1BW"));
  transform.fabsFormulaABCD1.fabsFormulaABCD4DC = CVector4(container->Get<CVector3>("transform_fabsFormulaABCD1C"), container->Get<double>("transform_fabsFormulaABCD1CW"));
  transform.fabsFormulaABCD1.fabsFormulaABCD4DD = CVector4(container->Get<CVector3>("transform_fabsFormulaABCD1D"), container->Get<double>("transform_fabsFormulaABCD1DW"));
  transform.fabsFormulaABCD1.fabsFormulaABCDEnabledAw = container->Get<bool>("transform_fabsFormulaABCD1_enabled_Aw");
  transform.fabsFormulaABCD1.fabsFormulaABCDEnabledBw = container->Get<bool>("transform_fabsFormulaABCD1_enabled_Bw");
  transform.fabsFormulaABCD2.fabsFormulaABCD4DA = CVector4(container->Get<CVector3>("transform_fabsFormulaABCD2A"), container->Get<double>("transform_fabsFormulaABCD2AW")) ;
  transform.fabsFormulaABCD2.fabsFormulaABCD4DB = CVector4(container->Get<CVector3>("transform_fabsFormulaABCD2B"), container->Get<double>("transform_fabsFormulaABCD2BW"));
  transform.fabsFormulaABCD2.fabsFormulaABCD4DC = CVector4(container->Get<CVector3>("transform_fabsFormulaABCD2C"), container->Get<double>("transform_fabsFormulaABCD2CW"));
  transform.fabsFormulaABCD2.fabsFormulaABCD4DD = CVector4(container->Get<CVector3>("transform_fabsFormulaABCD2D"), container->Get<double>("transform_fabsFormulaABCD2DW"));
  transform.fabsFormulaABCD2.fabsFormulaABCDEnabledAw = container->Get<bool>("transform_fabsFormulaABCD2_enabled_Aw");
  transform.fabsFormulaABCD2.fabsFormulaABCDEnabledBw = container->Get<bool>("transform_fabsFormulaABCD2_enabled_Bw");

  transform.iterationWeight1.iterationWeightConstantA = container->Get<double>("transform_iterationWeight1_constantA");
  transform.iterationWeight1.iterationWeightConstantB = container->Get<double>("transform_iterationWeight1_constantB");
  transform.iterationWeight1.iterationWeightConstantZ = container->Get<double>("transform_iterationWeight1_constantZ");
  transform.iterationWeight1.control.enabled = container->Get<bool>("transform_iterationWeight1_enabled");
  transform.iterationWeight1.control.weightEnabled = container->Get<bool>("transform_iterationWeight1_weight_enabled");
  transform.iterationWeight1.iterationWeightIterationA = container->Get<int>("transform_iterationWeight1_iterationA");
  transform.iterationWeight1.iterationWeightIterationB = container->Get<int>("transform_iterationWeight1_iterationB");
  transform.iterationWeight1.control.weight = container->Get<double>("transform_iterationWeight1_weight");

  transform.mainRotation1.control.enabled = container->Get<bool>("transform_main_rotation1_enabled");
  transform.mainRotation1.control.weightEnabled = container->Get<bool>("transform_main_rotation1_weight_enabled");
  transform.mainRotation1.control.startIterations = container->Get<int>("transform_main_rotation1_start_iterations");
  transform.mainRotation1.control.stopIterations = container->Get<int>("transform_main_rotation1_stop_iterations");
  transform.mainRotation1.mainRotation = container->Get<CVector3>("transform_main_rotation1");
  transform.mainRotation1.control.weight = container->Get<double>("transform_main_rotation1_weight");
  transform.mainRotation2.control.enabled = container->Get<bool>("transform_main_rotation2_enabled");
  transform.mainRotation2.control.weightEnabled = container->Get<bool>("transform_main_rotation2_weight_enabled");
  transform.mainRotation2.control.startIterations = container->Get<int>("transform_main_rotation2_start_iterations");
  transform.mainRotation2.control.stopIterations = container->Get<int>("transform_main_rotation2_stop_iterations");
  transform.mainRotation2.mainRotation = container->Get<CVector3>("transform_main_rotation2");
  transform.mainRotation2.control.weight = container->Get<double>("transform_main_rotation2_weight");
  transform.mainRotation3.control.enabled = container->Get<bool>("transform_main_rotation3_enabled");
  transform.mainRotation3.control.weightEnabled = container->Get<bool>("transform_main_rotation3_weight_enabled");
  transform.mainRotation3.control.startIterations = container->Get<int>("transform_main_rotation3_start_iterations");
  transform.mainRotation3.control.stopIterations = container->Get<int>("transform_main_rotation3_stop_iterations");
  transform.mainRotation3.mainRotation = container->Get<CVector3>("transform_main_rotation3");
  transform.mainRotation3.control.weight = container->Get<double>("transform_main_rotation3_weight");

  transform.mengerSpongeOriginal1.mengerSpongeOriginalConstantZ = container->Get<double>("transform_mengerSpongeOriginal1_constantZ");
  transform.mengerSpongeOriginal1.mengerSpongeOriginalFactorConstantVect = container->Get<CVector3>("transform_mengerSpongeOriginal1_factorConstantVect");
  transform.mengerSpongeOriginal1.control.enabled = container->Get<bool>("transform_mengerSpongeOriginal1_enabled");
  transform.mengerSpongeOriginal1.control.weightEnabled = container->Get<bool>("transform_mengerSpongeOriginal1_weight_enabled");
  transform.mengerSpongeOriginal1.control.startIterations = container->Get<int>("transform_mengerSpongeOriginal1_start_iterations");
  transform.mengerSpongeOriginal1.control.stopIterations = container->Get<int>("transform_mengerSpongeOriginal1_stop_iterations");
  transform.mengerSpongeOriginal1.control.weight = container->Get<double>("transform_mengerSpongeOriginal1_weight");
  transform.mengerSponge1.mengerSpongeConstantZ = container->Get<double>("transform_mengerSponge1_constantZ");
  transform.mengerSponge1.mengerSpongeFactorConstantVect = container->Get<CVector3>("transform_mengerSponge1_factorConstantVect");
  transform.mengerSponge1.control.enabled = container->Get<bool>("transform_mengerSponge1_enabled");
  transform.mengerSponge1.control.weightEnabled = container->Get<bool>("transform_mengerSponge1_weight_enabled");
  transform.mengerSponge1.control.startIterations = container->Get<int>("transform_mengerSponge1_start_iterations");
  transform.mengerSponge1.control.stopIterations = container->Get<int>("transform_mengerSponge1_stop_iterations");
  transform.mengerSponge1.control.weight = container->Get<double>("transform_mengerSponge1_weight");
  transform.mengerSponge2.mengerSpongeConstantZ = container->Get<double>("transform_mengerSponge2_constantZ");
  transform.mengerSponge2.mengerSpongeFactorConstantVect = container->Get<CVector3>("transform_mengerSponge2_factorConstantVect");
  transform.mengerSponge2.control.enabled = container->Get<bool>("transform_mengerSponge2_enabled");
  transform.mengerSponge2.control.weightEnabled = container->Get<bool>("transform_mengerSponge2_weight_enabled");
  transform.mengerSponge2.control.startIterations = container->Get<int>("transform_mengerSponge2_start_iterations");
  transform.mengerSponge2.control.stopIterations = container->Get<int>("transform_mengerSponge2_stop_iterations");
  transform.mengerSponge2.control.weight = container->Get<double>("transform_mengerSponge2_weight");

  transform.quaternionOriginal1.quaternionOriginalFactorConstant = CVector4(container->Get<CVector3>("transform_quaternionOriginal1_factorConstantVect"), container->Get<double>("transform_quaternionOriginal1_factorConstantW"));
  transform.quaternionOriginal1.control.enabled = container->Get<bool>("transform_quaternionOriginal1_enabled");
  transform.quaternionOriginal1.control.weightEnabled = container->Get<bool>("transform_quaternionOriginal1_weight_enabled");
  transform.quaternionOriginal1.control.startIterations = container->Get<int>("transform_quaternionOriginal1_start_iterations");
  transform.quaternionOriginal1.control.stopIterations = container->Get<int>("transform_quaternionOriginal1_stop_iterations");
  transform.quaternionOriginal1.control.weight = container->Get<double>("transform_quaternionOriginal1_weight");

  transform.quaternion1.quaternionFactorConstant = CVector4(container->Get<CVector3>("transform_quaternion1_factorConstantVect"), container->Get<double>("transform_quaternion1_factorConstantW"));
  transform.quaternion1.control.enabled = container->Get<bool>("transform_quaternion1_enabled");
  transform.quaternion1.control.weightEnabled = container->Get<bool>("transform_quaternion1_weight_enabled");
  transform.quaternion1.control.startIterations = container->Get<int>("transform_quaternion1_start_iterations");
  transform.quaternion1.control.stopIterations = container->Get<int>("transform_quaternion1_stop_iterations");
  transform.quaternion1.control.weight = container->Get<double>("transform_quaternion1_weight");

  transform.scale1.control.enabled = container->Get<bool>("transform_scale1_enabled");
  transform.scale1.control.startIterations = container->Get<int>("transform_scale1_start_iterations");
  transform.scale1.control.stopIterations = container->Get<int>("transform_scale1_stop_iterations");
  transform.scale1.scale = container->Get<double>("transform_scale1");
  transform.scale2.control.enabled = container->Get<bool>("transform_scale2_enabled");
  transform.scale2.control.startIterations = container->Get<int>("transform_scale2_start_iterations");
  transform.scale2.control.stopIterations = container->Get<int>("transform_scale2_stop_iterations");
  transform.scale2.scale = container->Get<double>("transform_scale2");
  transform.sphericalFold1.control.enabled = container->Get<bool>("transform_sphericalFold1_enabled");
  transform.sphericalFold1.control.weightEnabled = container->Get<bool>("transform_sphericalFold1_weight_enabled");
  transform.sphericalFold1.radMin = container->Get<double>("transform_sphericalFold1_min_radius");
  transform.sphericalFold1.radFixed = container->Get<double>("transform_sphericalFold1_fixed_radius");
  transform.sphericalFold1.control.startIterations = container->Get<int>("transform_sphericalFold1_start_iterations");
  transform.sphericalFold1.control.stopIterations = container->Get<int>("transform_sphericalFold1_stop_iterations");
  transform.sphericalFold1.control.weight = container->Get<double>("transform_sphericalFold1_weight");
  transform.sphericalFold2.control.enabled = container->Get<bool>("transform_sphericalFold2_enabled");
  transform.sphericalFold2.control.weightEnabled = container->Get<bool>("transform_sphericalFold2_weight_enabled");
  transform.sphericalFold2.radMin = container->Get<double>("transform_sphericalFold2_min_radius");
  transform.sphericalFold2.radFixed = container->Get<double>("transform_sphericalFold2_fixed_radius");
  transform.sphericalFold2.control.startIterations = container->Get<int>("transform_sphericalFold2_start_iterations");
  transform.sphericalFold2.control.stopIterations = container->Get<int>("transform_sphericalFold2_stop_iterations");
  transform.sphericalFold2.control.weight = container->Get<double>("transform_sphericalFold2_weight");

  transform.sphericalOffset1.control.enabled = container->Get<bool>("transform_sphericalOffset1_enabled");
  transform.sphericalOffset1.control.weightEnabled = container->Get<bool>("transform_sphericalOffset1_weight_enabled");
  transform.sphericalOffset1.offsetRadius = container->Get<double>("transform_sphericalOffset1_offset_radius");
  transform.sphericalOffset1.control.startIterations = container->Get<int>("transform_sphericalOffset1_start_iterations");
  transform.sphericalOffset1.control.stopIterations = container->Get<int>("transform_sphericalOffset1_stop_iterations");
  transform.sphericalOffset1.scale = container->Get<double>("transform_sphericalOffset1_scale");
  transform.sphericalOffset1.control.weight = container->Get<double>("transform_sphericalOffset1_weight");

  transform.sphericalOffset2.control.enabled = container->Get<bool>("transform_sphericalOffset2_enabled");
  transform.sphericalOffset2.control.weightEnabled = container->Get<bool>("transform_sphericalOffset2_weight_enabled");
  transform.sphericalOffset2.offsetRadius = container->Get<double>("transform_sphericalOffset2_offset_radius");
  transform.sphericalOffset2.control.startIterations = container->Get<int>("transform_sphericalOffset2_start_iterations");
  transform.sphericalOffset2.control.stopIterations = container->Get<int>("transform_sphericalOffset2_stop_iterations");
  transform.sphericalOffset2.scale = container->Get<int>("transform_sphericalOffset2_scale");
  transform.sphericalOffset2.control.weight = container->Get<double>("transform_sphericalOffset2_weight");

//----------------------------------
  mandelbulb5.mandelbulb1Power = container->Get<double>("mandelbulb5_mandelbulb1_power");
  mandelbulb5.mandelbulb1AlphaAngleOffset = container->Get<double>("mandelbulb5_mandelbulb1_alpha_angle_offset");
  mandelbulb5.mandelbulb1BetaAngleOffset = container->Get<double>("mandelbulb5_mandelbulb1_beta_angle_offset");
  mandelbulb5.mandelbulb1Enabled = container->Get<bool>("mandelbulb5_mandelbulb1_enabled");
  //mandelbulb5.mandelbulb1WeightEnabled = container->Get<bool>("mandelbulb5_mandelbulb1_weight_enabled");
  mandelbulb5.mandelbulb1StartIterations = container->Get<int>("mandelbulb5_mandelbulb1_start_iterations");
  mandelbulb5.mandelbulb1StopIterations = container->Get<int>("mandelbulb5_mandelbulb1_stop_iterations");
  //mandelbulb5.mandelbulb1Weight = container->Get<double>("mandelbulb5_mandelbulb1_weight");


  mandelbulb5.constantMultiplier1Enabled = container->Get<bool>("mandelbulb5_constantMultiplier1_enabled");
  mandelbulb5.constantMultiplier1WeightEnabled = container->Get<bool>("mandelbulb5_constantMultiplier1_weight_enabled");
  mandelbulb5.constantMultiplier1StartIterations = container->Get<int>("mandelbulb5_constantMultiplier1_start_iterations");
  mandelbulb5.constantMultiplier1StopIterations = container->Get<int>("mandelbulb5_constantMultiplier1_stop_iterations");
  mandelbulb5.constantMultiplier1Vect = container->Get<CVector3>("mandelbulb5_constantMultiplier1");
  mandelbulb5.constantMultiplier1Weight = container->Get<double>("mandelbulb5_constantMultiplier1_weight");

  mandelbulb5.fabsSubConstant1A = container->Get<CVector3>("mandelbulb5_fabsSubConstant1A");
  mandelbulb5.fabsSubConstant1B = container->Get<CVector3>("mandelbulb5_fabsSubConstant1B");
  mandelbulb5.fabsSubConstant1Enabled = container->Get<bool>("mandelbulb5_fabsSubConstant1_enabled");
  mandelbulb5.fabsSubConstant1WeightEnabled = container->Get<bool>("mandelbulb5_fabsSubConstant1_weight_enabled");
  mandelbulb5.fabsSubConstant1OldWeightEnabled = container->Get<bool>("mandelbulb5_fabsSubConstant1_old_weight_enabled");
  mandelbulb5.fabsSubConstant1Enabledx = container->Get<bool>("mandelbulb5_fabsSubConstant1_enabled_x");
  mandelbulb5.fabsSubConstant1Enabledy = container->Get<bool>("mandelbulb5_fabsSubConstant1_enabled_y");
  mandelbulb5.fabsSubConstant1Enabledz = container->Get<bool>("mandelbulb5_fabsSubConstant1_enabled_z");
  mandelbulb5.fabsSubConstant1StartIterations = container->Get<int>("mandelbulb5_fabsSubConstant1_start_iterations");
  mandelbulb5.fabsSubConstant1StopIterations = container->Get<int>("mandelbulb5_fabsSubConstant1_stop_iterations");
  mandelbulb5.fabsSubConstant1Weight = container->Get<double>("mandelbulb5_fabsSubConstant1_weight");
  mandelbulb5.fabsSubConstant1OldWeight = container->Get<double>("mandelbulb5_fabsSubConstant1_old_weight");

  mandelbulb5.fabsFormulaZAB1A = container->Get<CVector3>("mandelbulb5_fabsFormulaZAB1A");
  mandelbulb5.fabsFormulaZAB1B = container->Get<CVector3>("mandelbulb5_fabsFormulaZAB1B");
  mandelbulb5.fabsFormulaZAB1C = container->Get<CVector3>("mandelbulb5_fabsFormulaZAB1C");
  mandelbulb5.fabsFormulaZAB1Enabled = container->Get<bool>("mandelbulb5_fabsFormulaZAB1_enabled");
  mandelbulb5.fabsFormulaZAB1WeightEnabled = container->Get<bool>("mandelbulb5_fabsFormulaZAB1_weight_enabled");
  mandelbulb5.fabsFormulaZAB1OldWeightEnabled = container->Get<bool>("mandelbulb5_fabsFormulaZAB1_old_weight_enabled");
  mandelbulb5.fabsFormulaZAB1Enabledx = container->Get<bool>("mandelbulb5_fabsFormulaZAB1_enabled_x");
  mandelbulb5.fabsFormulaZAB1Enabledy = container->Get<bool>("mandelbulb5_fabsFormulaZAB1_enabled_y");
  mandelbulb5.fabsFormulaZAB1Enabledz = container->Get<bool>("mandelbulb5_fabsFormulaZAB1_enabled_z");
  mandelbulb5.fabsFormulaZAB1StartIterations = container->Get<int>("mandelbulb5_fabsFormulaZAB1_start_iterations");
  mandelbulb5.fabsFormulaZAB1StopIterations = container->Get<int>("mandelbulb5_fabsFormulaZAB1_stop_iterations");
  mandelbulb5.fabsFormulaZAB1Weight = container->Get<double>("mandelbulb5_fabsFormulaZAB1_weight");
  mandelbulb5.fabsFormulaZAB1OldWeight = container->Get<double>("mandelbulb5_fabsFormulaZAB1_old_weight");

  mandelbulb5.additionConstant1 = container->Get<CVector3>("mandelbulb5_additionConstant1");
  mandelbulb5.additionConstant1Enabled = container->Get<bool>("mandelbulb5_additionConstant1_enabled");
  mandelbulb5.additionConstant1WeightEnabled = container->Get<bool>("mandelbulb5_additionConstant1_weight_enabled");
  mandelbulb5.additionConstant1StartIterations = container->Get<int>("mandelbulb5_additionConstant1_start_iterations");
  mandelbulb5.additionConstant1StopIterations = container->Get<int>("mandelbulb5_additionConstant1_stop_iterations");
  mandelbulb5.additionConstant1Weight = container->Get<double>("mandelbulb5_additionConstant1_weight");

  mandelbulb5.fabsFormulaAB1A = container->Get<CVector3>("mandelbulb5_fabsFormulaAB1A");
  mandelbulb5.fabsFormulaAB1B = container->Get<CVector3>("mandelbulb5_fabsFormulaAB1B");
  mandelbulb5.fabsFormulaAB1Enabled = container->Get<bool>("mandelbulb5_fabsFormulaAB1_enabled");
  mandelbulb5.fabsFormulaAB1WeightEnabled = container->Get<bool>("mandelbulb5_fabsFormulaAB1_weight_enabled");
  mandelbulb5.fabsFormulaAB1OldWeightEnabled = container->Get<bool>("mandelbulb5_fabsFormulaAB1_old_weight_enabled");
  mandelbulb5.fabsFormulaAB1Enabledx = container->Get<bool>("mandelbulb5_fabsFormulaAB1_enabled_x");
  mandelbulb5.fabsFormulaAB1Enabledy = container->Get<bool>("mandelbulb5_fabsFormulaAB1_enabled_y");
  mandelbulb5.fabsFormulaAB1Enabledz = container->Get<bool>("mandelbulb5_fabsFormulaAB1_enabled_z");
  mandelbulb5.fabsFormulaAB1StartIterations = container->Get<int>("mandelbulb5_fabsFormulaAB1_start_iterations");
  mandelbulb5.fabsFormulaAB1StopIterations = container->Get<int>("mandelbulb5_fabsFormulaAB1_stop_iterations");
  mandelbulb5.fabsFormulaAB1Weight = container->Get<double>("mandelbulb5_fabsFormulaAB1_weight");
  mandelbulb5.fabsFormulaAB1OldWeight = container->Get<double>("mandelbulb5_fabsFormulaAB1_old_weight");

  mandelbulb5.mainRotation2Enabled = container->Get<bool>("mandelbulb5_main_rotation2_enabled");
  mandelbulb5.mainRotation2WeightEnabled = container->Get<bool>("mandelbulb5_main_rotation2_weight_enabled");
  mandelbulb5.mainRotation2StartIterations = container->Get<int>("mandelbulb5_main_rotation2_start_iterations");
  mandelbulb5.mainRotation2StopIterations = container->Get<int>("mandelbulb5_main_rotation2_stop_iterations");
  mandelbulb5.mainRotation2 = container->Get<CVector3>("mandelbulb5_main_rotation2");
  mandelbulb5.mainRotation2Weight = container->Get<double>("mandelbulb5_main_rotation2_weight");

  mandelbulb5.fabsFormulaABCD1A = container->Get<CVector3>("mandelbulb5_fabsFormulaABCD1A");
  mandelbulb5.fabsFormulaABCD1B = container->Get<CVector3>("mandelbulb5_fabsFormulaABCD1B");
  mandelbulb5.fabsFormulaABCD1C = container->Get<CVector3>("mandelbulb5_fabsFormulaABCD1C");
  mandelbulb5.fabsFormulaABCD1D = container->Get<CVector3>("mandelbulb5_fabsFormulaABCD1D");
  mandelbulb5.fabsFormulaABCD1Enabled = container->Get<bool>("mandelbulb5_fabsFormulaABCD1_enabled");
  mandelbulb5.fabsFormulaABCD1WeightEnabled = container->Get<bool>("mandelbulb5_fabsFormulaABCD1_weight_enabled");
  mandelbulb5.fabsFormulaABCD1OldWeightEnabled = container->Get<bool>("mandelbulb5_fabsFormulaABCD1_old_weight_enabled");
  mandelbulb5.fabsFormulaABCD1EnabledAx = container->Get<bool>("mandelbulb5_fabsFormulaABCD1_enabled_Ax");
  mandelbulb5.fabsFormulaABCD1EnabledAy = container->Get<bool>("mandelbulb5_fabsFormulaABCD1_enabled_Ay");
  mandelbulb5.fabsFormulaABCD1EnabledAz = container->Get<bool>("mandelbulb5_fabsFormulaABCD1_enabled_Az");
  mandelbulb5.fabsFormulaABCD1EnabledBx = container->Get<bool>("mandelbulb5_fabsFormulaABCD1_enabled_Bx");
  mandelbulb5.fabsFormulaABCD1EnabledBy = container->Get<bool>("mandelbulb5_fabsFormulaABCD1_enabled_By");
  mandelbulb5.fabsFormulaABCD1EnabledBz = container->Get<bool>("mandelbulb5_fabsFormulaABCD1_enabled_Bz");
  mandelbulb5.fabsFormulaABCD1StartIterations = container->Get<int>("mandelbulb5_fabsFormulaABCD1_start_iterations");
  mandelbulb5.fabsFormulaABCD1StopIterations = container->Get<int>("mandelbulb5_fabsFormulaABCD1_stop_iterations");
  mandelbulb5.fabsFormulaABCD1Weight = container->Get<double>("mandelbulb5_fabsFormulaABCD1_weight");
  mandelbulb5.fabsFormulaABCD1OldWeight = container->Get<double>("mandelbulb5_fabsFormulaABCD1_old_weight");


  mandelbulb5.mandelbulb2Power = container->Get<double>("mandelbulb5_mandelbulb2_power");
  mandelbulb5.mandelbulb2AlphaAngleOffset = container->Get<double>("mandelbulb5_mandelbulb2_alpha_angle_offset");
  mandelbulb5.mandelbulb2BetaAngleOffset = container->Get<double>("mandelbulb5_mandelbulb2_beta_angle_offset");
  mandelbulb5.mandelbulb2Enabled = container->Get<bool>("mandelbulb5_mandelbulb2_enabled");
  //mandelbulb5.mandelbulb2WeightEnabled = container->Get<bool>("mandelbulb5_mandelbulb2_weight_enabled");
  mandelbulb5.mandelbulb2StartIterations = container->Get<int>("mandelbulb5_mandelbulb2_start_iterations");
  mandelbulb5.mandelbulb2StopIterations = container->Get<int>("mandelbulb5_mandelbulb2_stop_iterations");
  //mandelbulb5.mandelbulb2Weight = container->Get<double>("mandelbulb5_mandelbulb2_weight");

  mandelbulb5.mainRotation3Enabled = container->Get<bool>("mandelbulb5_main_rotation3_enabled");
  mandelbulb5.mainRotation3WeightEnabled = container->Get<bool>("mandelbulb5_main_rotation3_weight_enabled");
  mandelbulb5.mainRotation3StartIterations = container->Get<int>("mandelbulb5_main_rotation3_start_iterations");
  mandelbulb5.mainRotation3StopIterations = container->Get<int>("mandelbulb5_main_rotation3_stop_iterations");
  mandelbulb5.mainRotation3 = container->Get<CVector3>("mandelbulb5_main_rotation3");
  mandelbulb5.mainRotation3Weight = container->Get<double>("mandelbulb5_main_rotation3_weight");

  mandelbulb5.constantMultiplier2Vect = container->Get<CVector3>("mandelbulb5_constantMultiplier2");
  mandelbulb5.constantMultiplier2Enabled = container->Get<bool>("mandelbulb5_constantMultiplier2_enabled");
  mandelbulb5.constantMultiplier2WeightEnabled = container->Get<bool>("mandelbulb5_constantMultiplier2_weight_enabled");
  mandelbulb5.constantMultiplier2StartIterations = container->Get<int>("mandelbulb5_constantMultiplier2_start_iterations");
  mandelbulb5.constantMultiplier2StopIterations = container->Get<int>("mandelbulb5_constantMultiplier2_stop_iterations");
  mandelbulb5.constantMultiplier2Weight = container->Get<double>("mandelbulb5_constantMultiplier2_weight");

  mandelbulb5.additionConstant2 = container->Get<CVector3>("mandelbulb5_additionConstant2");
  mandelbulb5.additionConstant2Enabled = container->Get<bool>("mandelbulb5_additionConstant2_enabled");
  mandelbulb5.additionConstant2WeightEnabled = container->Get<bool>("mandelbulb5_additionConstant2_weight_enabled");
  mandelbulb5.additionConstant2StartIterations = container->Get<int>("mandelbulb5_additionConstant2_start_iterations");
  mandelbulb5.additionConstant2StopIterations = container->Get<int>("mandelbulb5_additionConstant2_stop_iterations");
  mandelbulb5.additionConstant2Weight = container->Get<double>("mandelbulb5_additionConstant2_weight");

  mandelbulb5.fabsFormulaAB2A = container->Get<CVector3>("mandelbulb5_fabsFormulaAB2A");
  mandelbulb5.fabsFormulaAB2B = container->Get<CVector3>("mandelbulb5_fabsFormulaAB2B");
  mandelbulb5.fabsFormulaAB2Enabled = container->Get<bool>("mandelbulb5_fabsFormulaAB2_enabled");
  mandelbulb5.fabsFormulaAB2WeightEnabled = container->Get<bool>("mandelbulb5_fabsFormulaAB2_weight_enabled");
  mandelbulb5.fabsFormulaAB2OldWeightEnabled = container->Get<bool>("mandelbulb5_fabsFormulaAB2_old_weight_enabled");
  mandelbulb5.fabsFormulaAB2Enabledx = container->Get<bool>("mandelbulb5_fabsFormulaAB2_enabled_x");
  mandelbulb5.fabsFormulaAB2Enabledy = container->Get<bool>("mandelbulb5_fabsFormulaAB2_enabled_y");
  mandelbulb5.fabsFormulaAB2Enabledz = container->Get<bool>("mandelbulb5_fabsFormulaAB2_enabled_z");
  mandelbulb5.fabsFormulaAB2StartIterations = container->Get<int>("mandelbulb5_fabsFormulaAB2_start_iterations");
  mandelbulb5.fabsFormulaAB2StopIterations = container->Get<int>("mandelbulb5_fabsFormulaAB2_stop_iterations");
  mandelbulb5.fabsFormulaAB2Weight = container->Get<double>("mandelbulb5_fabsFormulaAB2_weight");
  mandelbulb5.fabsFormulaAB2OldWeight = container->Get<double>("mandelbulb5_fabsFormulaAB2_old_weight");

  mandelbulb5.boxFold2Enabled = container->Get<bool>("mandelbulb5_box_fold2_enabled");
  mandelbulb5.boxFold2WeightEnabled = container->Get<bool>("mandelbulb5_box_fold2_weight_enabled");
  mandelbulb5.boxFold2OldWeightEnabled = container->Get<bool>("mandelbulb5_box_fold2_old_weight_enabled");
  mandelbulb5.boxFold2StartIterations = container->Get<int>("mandelbulb5_box_fold2_start_iterations");
  mandelbulb5.boxFold2StopIterations = container->Get<int>("mandelbulb5_box_fold2_stop_iterations");
  mandelbulb5.boxFold2FoldingLimit = container->Get<double>("mandelbulb5_box_fold2_folding_limit");
  mandelbulb5.boxFold2FoldingValue = container->Get<double>("mandelbulb5_box_fold2_folding_value");
  mandelbulb5.boxFold2Weight = container->Get<double>("mandelbulb5_box_fold2_weight");
  mandelbulb5.boxFold2OldWeight = container->Get<double>("mandelbulb5_box_fold2_old_weight");


    //mandelbox103
  mandelbox103.sphericalFold1Enabled = container->Get<bool>("mandelbox103_sphericalFold1_enabled");
  mandelbox103.sphericalFold1WeightEnabled = container->Get<bool>("mandelbox103_sphericalFold1_weight_enabled");
  mandelbox103.sphericalFold1RadMin = container->Get<double>("mandelbox103_sphericalFold1_min_radius");
  mandelbox103.sphericalFold1RadFixed = container->Get<double>("mandelbox103_sphericalFold1_fixed_radius");
  mandelbox103.sphericalFold1StartIterations = container->Get<int>("mandelbox103_sphericalFold1_start_iterations");
  mandelbox103.sphericalFold1StopIterations = container->Get<int>("mandelbox103_sphericalFold1_stop_iterations");
  mandelbox103.sphericalFold1Weight = container->Get<double>("mandelbox103_sphericalFold1_weight");
  mandelbox103.scale1Enabled = container->Get<bool>("mandelbox103_scale1_enabled");
  mandelbox103.scale1StartIterations = container->Get<int>("mandelbox103_scale1_start_iterations");
  mandelbox103.scale1StopIterations = container->Get<int>("mandelbox103_scale1_stop_iterations");
  mandelbox103.scale1 = container->Get<double>("mandelbox103_scale1");
  mandelbox103.mainRotation1Enabled = container->Get<bool>("mandelbox103_main_rotation1_enabled");
  mandelbox103.mainRotation1WeightEnabled = container->Get<bool>("mandelbox103_main_rotation1_weight_enabled");
  mandelbox103.mainRotation1StartIterations = container->Get<int>("mandelbox103_main_rotation1_start_iterations");
  mandelbox103.mainRotation1StopIterations = container->Get<int>("mandelbox103_main_rotation1_stop_iterations");
  mandelbox103.mainRotation1 = container->Get<CVector3>("mandelbox103_main_rotation1");
  mandelbox103.mainRotation1Weight = container->Get<double>("mandelbox103_main_rotation1_weight");

  mandelbox103.constantMultiplier1Enabled = container->Get<bool>("mandelbox103_constantMultiplier1_enabled");
  mandelbox103.constantMultiplier1WeightEnabled = container->Get<bool>("mandelbox103_constantMultiplier1_weight_enabled");
  mandelbox103.constantMultiplier1StartIterations = container->Get<int>("mandelbox103_constantMultiplier1_start_iterations");
  mandelbox103.constantMultiplier1StopIterations = container->Get<int>("mandelbox103_constantMultiplier1_stop_iterations");
  mandelbox103.constantMultiplier1Vect = container->Get<CVector3>("mandelbox103_constantMultiplier1");
  mandelbox103.constantMultiplier1Weight = container->Get<double>("mandelbox103_constantMultiplier1_weight");
  mandelbox103.additionConstant1Enabled = container->Get<bool>("mandelbox103_additionConstant1_enabled");
  mandelbox103.additionConstant1WeightEnabled = container->Get<bool>("mandelbox103_additionConstant1_weight_enabled");
  mandelbox103.additionConstant1StartIterations = container->Get<int>("mandelbox103_additionConstant1_start_iterations");
  mandelbox103.additionConstant1StopIterations = container->Get<int>("mandelbox103_additionConstant1_stop_iterations");
  mandelbox103.additionConstant1Weight = container->Get<double>("mandelbox103_additionConstant1_weight");
  mandelbox103.additionConstant1 = container->Get<CVector3>("mandelbox103_additionConstant1");

  mandelbox103.fabsFormulaABCD1A = container->Get<CVector3>("mandelbox103_fabsFormulaABCD1A");
  mandelbox103.fabsFormulaABCD1B = container->Get<CVector3>("mandelbox103_fabsFormulaABCD1B");
  mandelbox103.fabsFormulaABCD1C = container->Get<CVector3>("mandelbox103_fabsFormulaABCD1C");
  mandelbox103.fabsFormulaABCD1D = container->Get<CVector3>("mandelbox103_fabsFormulaABCD1D");
  mandelbox103.fabsFormulaABCD1Enabled = container->Get<bool>("mandelbox103_fabsFormulaABCD1_enabled");
  mandelbox103.fabsFormulaABCD1WeightEnabled = container->Get<bool>("mandelbox103_fabsFormulaABCD1_weight_enabled");
  mandelbox103.fabsFormulaABCD1EnabledAx = container->Get<bool>("mandelbox103_fabsFormulaABCD1_enabled_Ax");
  mandelbox103.fabsFormulaABCD1EnabledAy = container->Get<bool>("mandelbox103_fabsFormulaABCD1_enabled_Ay");
  mandelbox103.fabsFormulaABCD1EnabledAz = container->Get<bool>("mandelbox103_fabsFormulaABCD1_enabled_Az");
  mandelbox103.fabsFormulaABCD1EnabledBx = container->Get<bool>("mandelbox103_fabsFormulaABCD1_enabled_Bx");
  mandelbox103.fabsFormulaABCD1EnabledBy = container->Get<bool>("mandelbox103_fabsFormulaABCD1_enabled_By");
  mandelbox103.fabsFormulaABCD1EnabledBz = container->Get<bool>("mandelbox103_fabsFormulaABCD1_enabled_Bz");
  mandelbox103.fabsFormulaABCD1StartIterations = container->Get<int>("mandelbox103_fabsFormulaABCD1_start_iterations");
  mandelbox103.fabsFormulaABCD1StopIterations = container->Get<int>("mandelbox103_fabsFormulaABCD1_stop_iterations");
  mandelbox103.fabsFormulaABCD1Weight = container->Get<double>("mandelbox103_fabsFormulaABCD1_weight");

  mandelbox103.sphericalFold2RadMin = container->Get<double>("mandelbox103_sphericalFold2_min_radius");
  mandelbox103.sphericalFold2RadFixed = container->Get<double>("mandelbox103_sphericalFold2_fixed_radius");
  mandelbox103.sphericalFold2Enabled = container->Get<bool>("mandelbox103_sphericalFold2_enabled");
  mandelbox103.sphericalFold2WeightEnabled = container->Get<bool>("mandelbox103_sphericalFold2_weight_enabled");
  mandelbox103.sphericalFold2StartIterations = container->Get<int>("mandelbox103_sphericalFold2_start_iterations");
  mandelbox103.sphericalFold2StopIterations = container->Get<int>("mandelbox103_sphericalFold2_stop_iterations");
  mandelbox103.sphericalFold2Weight = container->Get<double>("mandelbox103_sphericalFold2_weight");
  mandelbox103.scale2Enabled = container->Get<bool>("mandelbox103_scale2_enabled");
  mandelbox103.scale2StartIterations = container->Get<int>("mandelbox103_scale2_start_iterations");
  mandelbox103.scale2StopIterations = container->Get<int>("mandelbox103_scale2_stop_iterations");
  mandelbox103.scale2 = container->Get<double>("mandelbox103_scale2");

  mandelbox103.mainRotation2Enabled = container->Get<bool>("mandelbox103_main_rotation2_enabled");
  mandelbox103.mainRotation2WeightEnabled = container->Get<bool>("mandelbox103_main_rotation2_weight_enabled");
  mandelbox103.mainRotation2StartIterations = container->Get<int>("mandelbox103_main_rotation2_start_iterations");
  mandelbox103.mainRotation2StopIterations = container->Get<int>("mandelbox103_main_rotation2_stop_iterations");
  mandelbox103.mainRotation2 = container->Get<CVector3>("mandelbox103_main_rotation2");
  mandelbox103.mainRotation2Weight = container->Get<double>("mandelbox103_main_rotation2_weight");


  mandelbox103.constantMultiplier2Vect = container->Get<CVector3>("mandelbox103_constantMultiplier2");
  mandelbox103.constantMultiplier2Enabled = container->Get<bool>("mandelbox103_constantMultiplier2_enabled");
  mandelbox103.constantMultiplier2WeightEnabled = container->Get<bool>("mandelbox103_constantMultiplier2_weight_enabled");
  mandelbox103.constantMultiplier2StartIterations = container->Get<int>("mandelbox103_constantMultiplier2_start_iterations");
  mandelbox103.constantMultiplier2StopIterations = container->Get<int>("mandelbox103_constantMultiplier2_stop_iterations");
  mandelbox103.constantMultiplier2Weight = container->Get<double>("mandelbox103_constantMultiplier2_weight");
  mandelbox103.additionConstant2 = container->Get<CVector3>("mandelbox103_additionConstant2");
  mandelbox103.additionConstant2Enabled = container->Get<bool>("mandelbox103_additionConstant2_enabled");
  mandelbox103.additionConstant2WeightEnabled = container->Get<bool>("mandelbox103_additionConstant2_weight_enabled");
  mandelbox103.additionConstant2StartIterations = container->Get<int>("mandelbox103_additionConstant2_start_iterations");
  mandelbox103.additionConstant2StopIterations = container->Get<int>("mandelbox103_additionConstant2_stop_iterations");
  mandelbox103.additionConstant2Weight = container->Get<double>("mandelbox103_additionConstant2_weight");

  mandelbox103.mainRotation3Enabled = container->Get<bool>("mandelbox103_main_rotation3_enabled");
  mandelbox103.mainRotation3WeightEnabled = container->Get<bool>("mandelbox103_main_rotation3_weight_enabled");
  mandelbox103.mainRotation3StartIterations = container->Get<int>("mandelbox103_main_rotation3_start_iterations");
  mandelbox103.mainRotation3StopIterations = container->Get<int>("mandelbox103_main_rotation3_stop_iterations");
  mandelbox103.mainRotation3 = container->Get<CVector3>("mandelbox103_main_rotation3");
  mandelbox103.mainRotation3Weight = container->Get<double>("mandelbox103_main_rotation3_weight");

  mandelbox103.fabsFormulaABCD2A = container->Get<CVector3>("mandelbox103_fabsFormulaABCD2A");
  mandelbox103.fabsFormulaABCD2B = container->Get<CVector3>("mandelbox103_fabsFormulaABCD2B");
  mandelbox103.fabsFormulaABCD2C = container->Get<CVector3>("mandelbox103_fabsFormulaABCD2C");
  mandelbox103.fabsFormulaABCD2D = container->Get<CVector3>("mandelbox103_fabsFormulaABCD2D");
  mandelbox103.fabsFormulaABCD2Enabled = container->Get<bool>("mandelbox103_fabsFormulaABCD2_enabled");
  mandelbox103.fabsFormulaABCD2WeightEnabled = container->Get<bool>("mandelbox103_fabsFormulaABCD2_weight_enabled");
  mandelbox103.fabsFormulaABCD2EnabledAx = container->Get<bool>("mandelbox103_fabsFormulaABCD2_enabled_Ax");
  mandelbox103.fabsFormulaABCD2EnabledAy = container->Get<bool>("mandelbox103_fabsFormulaABCD2_enabled_Ay");
  mandelbox103.fabsFormulaABCD2EnabledAz = container->Get<bool>("mandelbox103_fabsFormulaABCD2_enabled_Az");
  mandelbox103.fabsFormulaABCD2EnabledBx = container->Get<bool>("mandelbox103_fabsFormulaABCD2_enabled_Bx");
  mandelbox103.fabsFormulaABCD2EnabledBy = container->Get<bool>("mandelbox103_fabsFormulaABCD2_enabled_By");
  mandelbox103.fabsFormulaABCD2EnabledBz = container->Get<bool>("mandelbox103_fabsFormulaABCD2_enabled_Bz");
  mandelbox103.fabsFormulaABCD2StartIterations = container->Get<int>("mandelbox103_fabsFormulaABCD2_start_iterations");
  mandelbox103.fabsFormulaABCD2StopIterations = container->Get<int>("mandelbox103_fabsFormulaABCD2_stop_iterations");
  mandelbox103.fabsFormulaABCD2Weight = container->Get<double>("mandelbox103_fabsFormulaABCD2_weight");

  mandelbox103.iterationWeight1ConstantA = container->Get<double>("mandelbox103_iterationWeight1_constantA");
  mandelbox103.iterationWeight1ConstantB = container->Get<double>("mandelbox103_iterationWeight1_constantB");
  mandelbox103.iterationWeight1ConstantZ = container->Get<double>("mandelbox103_iterationWeight1_constantZ");
  mandelbox103.iterationWeight1Enabled = container->Get<bool>("mandelbox103_iterationWeight1_enabled");
  mandelbox103.iterationWeight1WeightEnabled = container->Get<bool>("mandelbox103_iterationWeight1_weight_enabled");
  mandelbox103.iterationWeight1IterationA = container->Get<int>("mandelbox103_iterationWeight1_iterationA");
  mandelbox103.iterationWeight1IterationB = container->Get<int>("mandelbox103_iterationWeight1_iterationB");
  mandelbox103.iterationWeight1Weight = container->Get<double>("mandelbox103_iterationWeight1_weight");

  //quaternion104



  //mengerSponge105





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

  mandelbulb5.mandelbulb1AlphaAngleOffset *= M_PI / 180.0;
  mandelbulb5.mandelbulb1BetaAngleOffset *= M_PI / 180.0;
  mandelbulb5.mandelbulb2AlphaAngleOffset *= M_PI / 180.0;
  mandelbulb5.mandelbulb2BetaAngleOffset *= M_PI / 180.0;


  mandelbox103.sphericalFold1fR2 = mandelbox103.sphericalFold1RadFixed * mandelbox103.sphericalFold1RadFixed;
  mandelbox103.sphericalFold1mR2 = mandelbox103.sphericalFold1RadMin * mandelbox103.sphericalFold1RadMin;
  mandelbox103.sphericalFold1MboxFactor1 = mandelbox103.sphericalFold1fR2 / mandelbox103.sphericalFold1mR2;
  mandelbox103.sphericalFold2fR2 = mandelbox103.sphericalFold2RadFixed * mandelbox103.sphericalFold2RadFixed;
  mandelbox103.sphericalFold2mR2 = mandelbox103.sphericalFold2RadMin * mandelbox103.sphericalFold2RadMin;
  mandelbox103.sphericalFold2MboxFactor1 = mandelbox103.sphericalFold2fR2 / mandelbox103.sphericalFold2mR2;


  quaternion104.sphericalFold1fR2 = quaternion104.sphericalFold1RadFixed * quaternion104.sphericalFold1RadFixed;
  quaternion104.sphericalFold1mR2 = quaternion104.sphericalFold1RadMin * quaternion104.sphericalFold1RadMin;
  quaternion104.sphericalFold1MboxFactor1 = quaternion104.sphericalFold1fR2 / quaternion104.sphericalFold1mR2;
  quaternion104.sphericalFold2fR2 = quaternion104.sphericalFold2RadFixed * quaternion104.sphericalFold2RadFixed;
  quaternion104.sphericalFold2mR2 = quaternion104.sphericalFold2RadMin * quaternion104.sphericalFold2RadMin;
  quaternion104.sphericalFold2MboxFactor1 = quaternion104.sphericalFold2fR2 / quaternion104.sphericalFold2mR2;



  transform.mainRotation1.mainRot.SetRotation2(transform.mainRotation1.mainRotation * (M_PI / 180.0));
  transform.mainRotation2.mainRot.SetRotation2(transform.mainRotation2.mainRotation * (M_PI / 180.0));
  transform.mainRotation3.mainRot.SetRotation2(transform.mainRotation3.mainRotation * (M_PI / 180.0));


  transform.sphericalFold1.fR2 = transform.sphericalFold1.radFixed * transform.sphericalFold1.radFixed;
  transform.sphericalFold1.mR2 = transform.sphericalFold1.radMin * transform.sphericalFold1.radMin;
  transform.sphericalFold1.mboxFactor1 = transform.sphericalFold1.fR2 / transform.sphericalFold1.mR2;
  transform.sphericalFold2.fR2 = transform.sphericalFold2.radFixed * transform.sphericalFold2.radFixed;
  transform.sphericalFold2.mR2 = transform.sphericalFold2.radMin * transform.sphericalFold2.radMin;
  transform.sphericalFold2.mboxFactor1 = transform.sphericalFold2.fR2 / transform.sphericalFold2.mR2;





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
