/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
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
 * cFractal class - container for fractal formula parameters
 */

#include "fractal.h"
#include "algebra.hpp"
#include "system.hpp"
#include "parameters.hpp"

cFractal::cFractal(const cParameterContainer *container)
{
	// WriteLog("cFractal::cFractal(const cParameterContainer *container)");
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
	mandelbox.color.factor4D = container->Get<CVector4>("mandelbox_color_4D");
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
		(fractal::enumGeneralizedFoldBoxType)container->Get<int>("mandelbox_generalized_fold_type");

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

	donut.ringRadius = container->Get<double>("donut_ring_radius");
	donut.ringThickness = container->Get<double>("donut_ring_thickness");
	donut.factor = container->Get<double>("donut_factor");
	donut.number = container->Get<double>("donut_number");

	//----------------------------------

	// platonic_solid
	platonicSolid.frequency = container->Get<double>("platonic_solid_frequency");
	platonicSolid.amplitude = container->Get<double>("platonic_solid_amplitude");
	platonicSolid.rhoMul = container->Get<double>("platonic_solid_rhoMul");

	// mandelbulb multi
	mandelbulbMulti.acosOrasin =
		(sFractalMandelbulbMulti::multi_acosOrasin)container->Get<int>("mandelbulbMulti_acos_or_asin");
	mandelbulbMulti.atanOratan2 = (sFractalMandelbulbMulti::multi_atanOratan2)container->Get<int>(
		"mandelbulbMulti_atan_or_atan2");

	mandelbulbMulti.acosOrasinA = (sFractalMandelbulbMulti::multi_acosOrasin)container->Get<int>(
		"mandelbulbMulti_acos_or_asin_A");
	mandelbulbMulti.atanOratan2A = (sFractalMandelbulbMulti::multi_atanOratan2)container->Get<int>(
		"mandelbulbMulti_atan_or_atan2_A");

	mandelbulbMulti.orderOfxyz =
		(sFractalMandelbulbMulti::multi_OrderOfxyz)container->Get<int>("mandelbulbMulti_order_of_xyz");
	mandelbulbMulti.orderOfxyz2 = (sFractalMandelbulbMulti::multi_OrderOfxyz)container->Get<int>(
		"mandelbulbMulti_order_of_xyz_2");
	mandelbulbMulti.orderOfxyzC = (sFractalMandelbulbMulti::multi_OrderOfxyz)container->Get<int>(
		"mandelbulbMulti_order_of_xyz_C");

	// sinTan2Trig
	sinTan2Trig.asinOracos =
		(sFractalSinTan2Trig::multi_asinOracos)container->Get<int>("sinTan2Trig_asin_or_acos");
	sinTan2Trig.atan2Oratan =
		(sFractalSinTan2Trig::multi_atan2Oratan)container->Get<int>("sinTan2Trig_atan2_or_atan");
	sinTan2Trig.orderOfzyx =
		(sFractalSinTan2Trig::multi_OrderOfzyx)container->Get<int>("sinTan2Trig_order_of_zyx");

	// surfBox
	surfBox.enabledX1 = container->Get<bool>("surfBox_enabledX1");
	surfBox.enabledY1 = container->Get<bool>("surfBox_enabledY1");
	surfBox.enabledZ1 = container->Get<bool>("surfBox_enabledZ1");
	surfBox.enabledX2False = container->Get<bool>("surfBox_enabledX2_false");
	surfBox.enabledY2False = container->Get<bool>("surfBox_enabledY2_false");
	surfBox.enabledZ2False = container->Get<bool>("surfBox_enabledZ2_false");
	surfBox.enabledX3False = container->Get<bool>("surfBox_enabledX3_false");
	surfBox.enabledY3False = container->Get<bool>("surfBox_enabledY3_false");
	surfBox.enabledZ3False = container->Get<bool>("surfBox_enabledZ3_false");
	surfBox.enabledX4False = container->Get<bool>("surfBox_enabledX4_false");
	surfBox.enabledY4False = container->Get<bool>("surfBox_enabledY4_false");
	surfBox.enabledZ4False = container->Get<bool>("surfBox_enabledZ4_false");
	surfBox.enabledX5False = container->Get<bool>("surfBox_enabledX5_false");
	surfBox.enabledY5False = container->Get<bool>("surfBox_enabledY5_false");
	surfBox.enabledZ5False = container->Get<bool>("surfBox_enabledZ5_false");
	surfBox.offset1A111 = container->Get<CVector3>("surfBox_offset1A_111");
	surfBox.offset1B111 = container->Get<CVector3>("surfBox_offset1B_111");
	surfBox.offset2A111 = container->Get<CVector3>("surfBox_offset2A_111");
	surfBox.offset2B111 = container->Get<CVector3>("surfBox_offset2B_111");
	surfBox.offset3A111 = container->Get<CVector3>("surfBox_offset3A_111");
	surfBox.offset3B111 = container->Get<CVector3>("surfBox_offset3B_111");
	surfBox.offset1A222 = container->Get<CVector3>("surfBox_offset1A_222");
	surfBox.offset1B222 = container->Get<CVector3>("surfBox_offset1B_222");
	surfBox.scale1Z1 = container->Get<double>("surfBox_scale1Z1");

	// surfFolds
	surfFolds.orderOfFolds1 =
		(sFractalSurfFolds::multi_orderOfFolds)container->Get<int>("surfFolds_order_of_folds_1");
	surfFolds.orderOfFolds2 =
		(sFractalSurfFolds::multi_orderOfFolds)container->Get<int>("surfFolds_order_of_folds_2");
	surfFolds.orderOfFolds3 =
		(sFractalSurfFolds::multi_orderOfFolds)container->Get<int>("surfFolds_order_of_folds_3");
	surfFolds.orderOfFolds4 =
		(sFractalSurfFolds::multi_orderOfFolds)container->Get<int>("surfFolds_order_of_folds_4");
	surfFolds.orderOfFolds5 =
		(sFractalSurfFolds::multi_orderOfFolds)container->Get<int>("surfFolds_order_of_folds_5");

	// bensei mag transforms
	magTransf.orderOfTransf1 =
		(sFractalMagTransforms::multi_orderOfTransf)container->Get<int>("magTransf_order_of_transf_1");
	magTransf.orderOfTransf2 =
		(sFractalMagTransforms::multi_orderOfTransf)container->Get<int>("magTransf_order_of_transf_2");
	magTransf.orderOfTransf3 =
		(sFractalMagTransforms::multi_orderOfTransf)container->Get<int>("magTransf_order_of_transf_3");
	magTransf.orderOfTransf4 =
		(sFractalMagTransforms::multi_orderOfTransf)container->Get<int>("magTransf_order_of_transf_4");
	magTransf.orderOfTransf5 =
		(sFractalMagTransforms::multi_orderOfTransf)container->Get<int>("magTransf_order_of_transf_5");

	// for curvilinear parameter
	Cpara.enabledLinear = container->Get<bool>("Cpara_enabledLinear");
	Cpara.enabledCurves = container->Get<bool>("Cpara_enabledCurves");
	Cpara.enabledParabFalse = container->Get<bool>("Cpara_enabledParab_false");
	Cpara.enabledParaAddP0 = container->Get<bool>("Cpara_enabledParaAddP0");
	Cpara.para00 = container->Get<double>("Cpara_para00");
	Cpara.paraA0 = container->Get<double>("Cpara_paraA0");
	Cpara.paraB0 = container->Get<double>("Cpara_paraB0");
	Cpara.paraC0 = container->Get<double>("Cpara_paraC0");
	Cpara.parabOffset0 = container->Get<double>("Cpara_parab_offset0");
	Cpara.para0 = container->Get<double>("Cpara_para0");
	Cpara.paraA = container->Get<double>("Cpara_paraA");
	Cpara.paraB = container->Get<double>("Cpara_paraB");
	Cpara.paraC = container->Get<double>("Cpara_paraC");
	Cpara.parabOffset = container->Get<double>("Cpara_parab_offset");
	Cpara.parabSlope = container->Get<double>("Cpara_parab_slope");
	Cpara.parabScale = container->Get<double>("Cpara_parab_scale");
	Cpara.iterA = container->Get<int>("Cpara_iterA");
	Cpara.iterB = container->Get<int>("Cpara_iterB");
	Cpara.iterC = container->Get<int>("Cpara_iterC");

	analyticDE.scale1 = container->Get<double>("analyticDE_scale_1");
	analyticDE.tweak005 = container->Get<double>("analyticDE_tweak_005");
	analyticDE.offset0 = container->Get<double>("analyticDE_offset_0");
	analyticDE.offset1 = container->Get<double>("analyticDE_offset_1");
	analyticDE.offset2 = container->Get<double>("analyticDE_offset_2");
	analyticDE.enabledFalse = container->Get<bool>("analyticDE_enabled_false");
	analyticDE.enabledAuxR2False = container->Get<bool>("analyticDE_enabled_auxR2_false");
	analyticDE.scaleLin = container->Get<double>("analyticDE_scale_linear");
	analyticDE.offsetLin = container->Get<double>("analyticDE_offset_linear");

	foldColor.colorMin = container->Get<double>("fold_color_color_min");
	foldColor.compFold = container->Get<double>("fold_color_comp_fold");
	foldColor.compFold0 = container->Get<double>("fold_color_comp_fold0");
	foldColor.compMinR = container->Get<double>("fold_color_comp_minR");
	foldColor.compScale = container->Get<double>("fold_color_comp_scale");

	// common parameters for transforming formulas
	transformCommon.alphaAngleOffset = container->Get<double>("transf_alpha_angle_offset");
	transformCommon.betaAngleOffset = container->Get<double>("transf_beta_angle_offset");
	transformCommon.foldingValue = container->Get<double>("transf_folding_value");
	transformCommon.foldingLimit = container->Get<double>("transf_folding_limit");
	transformCommon.multiplication = container->Get<double>("transf_multiplication");
	transformCommon.minRneg1 = container->Get<double>("transf_minimum_radius_neg1");
	transformCommon.minR0 = container->Get<double>("transf_minimum_radius_0");
	transformCommon.minR05 = container->Get<double>("transf_minimum_radius_05");
	transformCommon.minR2p25 = container->Get<double>("transf_minR2_p25");
	transformCommon.maxR2d1 = container->Get<double>("transf_maxR2_1");
	transformCommon.minR06 = container->Get<double>("transf_minimum_radius_06");
	transformCommon.offset = container->Get<double>("transf_offset");
	transformCommon.offset0 = container->Get<double>("transf_offset_0");
	transformCommon.offset05 = container->Get<double>("transf_offset_05");
	transformCommon.offset1 = container->Get<double>("transf_offset_1");
	transformCommon.offset105 = container->Get<double>("transf_offset_105");
	transformCommon.offset2 = container->Get<double>("transf_offset_2");
	transformCommon.pwr05 = container->Get<double>("transf_pwr_05");
	transformCommon.pwr4 = container->Get<double>("transf_pwr_4");
	transformCommon.pwr8 = container->Get<double>("transf_pwr_8");
	transformCommon.pwr8a = container->Get<double>("transf_pwr_8a");
	transformCommon.scale = container->Get<double>("transf_scale");
	transformCommon.scale0 = container->Get<double>("transf_scale_0");
	transformCommon.scale025 = container->Get<double>("transf_scale_025");
	transformCommon.scale1 = container->Get<double>("transf_scale_1");
	transformCommon.scaleA1 = container->Get<double>("transf_scaleA_1");
	transformCommon.scaleB1 = container->Get<double>("transf_scaleB_1");
	transformCommon.scaleC1 = container->Get<double>("transf_scaleC_1");
	transformCommon.scaleD1 = container->Get<double>("transf_scaleD_1");
	transformCommon.scaleE1 = container->Get<double>("transf_scaleE_1");
	transformCommon.scaleA2 = container->Get<double>("transf_scaleA_2");
	transformCommon.scale015 = container->Get<double>("transf_scale_015");
	transformCommon.scale2 = container->Get<double>("transf_scale_2");
	transformCommon.scale3 = container->Get<double>("transf_scale_3");
	transformCommon.scaleA3 = container->Get<double>("transf_scaleA_3");
	transformCommon.scale4 = container->Get<double>("transf_scale_4");
	transformCommon.scale8 = container->Get<double>("transf_scale_8");

	transformCommon.intA = container->Get<int>("transf_int_A");
	transformCommon.intB = container->Get<int>("transf_int_B");
	transformCommon.int1 = container->Get<int>("transf_int_1");
	transformCommon.int8X = container->Get<int>("transf_int8_X");
	transformCommon.int8Y = container->Get<int>("transf_int8_Y");
	transformCommon.int8Z = container->Get<int>("transf_int8_Z");
	transformCommon.startIterations = container->Get<int>("transf_start_iterations");
	transformCommon.startIterations250 = container->Get<int>("transf_start_iterations_250");
	transformCommon.stopIterations = container->Get<int>("transf_stop_iterations");
	transformCommon.startIterationsA = container->Get<int>("transf_start_iterations_A");
	transformCommon.stopIterationsA = container->Get<int>("transf_stop_iterations_A");
	transformCommon.startIterationsB = container->Get<int>("transf_start_iterations_B");
	transformCommon.stopIterationsB = container->Get<int>("transf_stop_iterations_B");
	transformCommon.startIterationsC = container->Get<int>("transf_start_iterations_C");
	transformCommon.stopIterationsC = container->Get<int>("transf_stop_iterations_C");
	transformCommon.startIterationsD = container->Get<int>("transf_start_iterations_D");
	transformCommon.stopIterationsD = container->Get<int>("transf_stop_iterations_D");
	transformCommon.startIterationsE = container->Get<int>("transf_start_iterations_E");
	transformCommon.stopIterationsE = container->Get<int>("transf_stop_iterations_E");
	transformCommon.startIterationsF = container->Get<int>("transf_start_iterations_F");
	transformCommon.stopIterationsF = container->Get<int>("transf_stop_iterations_F");
	transformCommon.startIterationsP = container->Get<int>("transf_start_iterations_P");
	transformCommon.stopIterationsP1 = container->Get<int>("transf_stop_iterations_P1");
	transformCommon.startIterationsR = container->Get<int>("transf_start_iterations_R");
	transformCommon.stopIterationsR = container->Get<int>("transf_stop_iterations_R");
	transformCommon.startIterationsS = container->Get<int>("transf_start_iterations_S");
	transformCommon.stopIterationsS = container->Get<int>("transf_stop_iterations_S");
	transformCommon.startIterationsT = container->Get<int>("transf_start_iterations_T");
	transformCommon.stopIterationsT = container->Get<int>("transf_stop_iterations_T");
	transformCommon.startIterationsM = container->Get<int>("transf_start_iterations_M");
	transformCommon.stopIterationsM = container->Get<int>("transf_stop_iterations_M");
	transformCommon.stopIterations1 = container->Get<int>("transf_stop_iterations_1");
	transformCommon.stopIterationsT1 = container->Get<int>("transf_stop_iterationsT_1");
	transformCommon.stopIterationsTM1 = container->Get<int>("transf_stop_iterationsTM_1");

	transformCommon.additionConstant0555 = container->Get<CVector3>("transf_addition_constant_0555");
	transformCommon.additionConstant0777 = container->Get<CVector3>("transf_addition_constant_0777");
	transformCommon.additionConstant000 = container->Get<CVector3>("transf_addition_constant");
	transformCommon.additionConstantA000 = container->Get<CVector3>("transf_addition_constantA_000");
	transformCommon.additionConstantP000 = container->Get<CVector3>("transf_addition_constantP_000");
	transformCommon.additionConstant111 = container->Get<CVector3>("transf_addition_constant_111");
	transformCommon.additionConstantA111 = container->Get<CVector3>("transf_addition_constantA_111");
	transformCommon.additionConstant222 = container->Get<CVector3>("transf_addition_constant_222");
	transformCommon.additionConstantNeg100 =
		container->Get<CVector3>("transf_addition_constant_neg100");
	transformCommon.constantMultiplier000 =
		container->Get<CVector3>("transf_constant_multiplier_000");
	transformCommon.constantMultiplier001 =
		container->Get<CVector3>("transf_constant_multiplier_001");
	transformCommon.constantMultiplier010 =
		container->Get<CVector3>("transf_constant_multiplier_010");
	transformCommon.constantMultiplier100 =
		container->Get<CVector3>("transf_constant_multiplier_100");
	transformCommon.constantMultiplierA100 =
		container->Get<CVector3>("transf_constant_multiplierA_100");
	transformCommon.constantMultiplier111 =
		container->Get<CVector3>("transf_constant_multiplier_111");
	transformCommon.constantMultiplierA111 =
		container->Get<CVector3>("transf_constant_multiplierA_111");
	transformCommon.constantMultiplierB111 =
		container->Get<CVector3>("transf_constant_multiplierB_111");
	transformCommon.constantMultiplierC111 =
		container->Get<CVector3>("transf_constant_multiplierC_111");
	transformCommon.constantMultiplier121 =
		container->Get<CVector3>("transf_constant_multiplier_121");
	transformCommon.constantMultiplier122 =
		container->Get<CVector3>("transf_constant_multiplier_122");
	transformCommon.constantMultiplier221 =
		container->Get<CVector3>("transf_constant_multiplier_221");
	transformCommon.constantMultiplier222 =
		container->Get<CVector3>("transf_constant_multiplier_222");
	transformCommon.constantMultiplier441 =
		container->Get<CVector3>("transf_constant_multiplier_441");
	transformCommon.juliaC = container->Get<CVector3>("transf_constant_julia_c");
	transformCommon.offset000 = container->Get<CVector3>("transf_offset_000");
	transformCommon.offsetA000 = container->Get<CVector3>("transf_offsetA_000");
	transformCommon.offsetF000 = container->Get<CVector3>("transf_offsetF_000");
	transformCommon.offset100 = container->Get<CVector3>("transf_offset_100");
	transformCommon.offset111 = container->Get<CVector3>("transf_offset_111");
	transformCommon.offsetA111 = container->Get<CVector3>("transf_offsetA_111");
	transformCommon.offsetB111 = container->Get<CVector3>("transf_offsetB_111");
	transformCommon.offsetC111 = container->Get<CVector3>("transf_offsetC_111");
	transformCommon.offset200 = container->Get<CVector3>("transf_offset_200");
	transformCommon.offsetA200 = container->Get<CVector3>("transf_offsetA_200");
	transformCommon.offset222 = container->Get<CVector3>("transf_offset_222");

	transformCommon.power025 = container->Get<CVector3>("transf_power_025");
	transformCommon.power8 = container->Get<CVector3>("transf_power_8");
	transformCommon.rotation = container->Get<CVector3>("transf_rotation");
	transformCommon.rotation2 = container->Get<CVector3>("transf_rotation2");
	transformCommon.rotation44a = container->Get<CVector3>("transf_rotation44a"); //...........................
	transformCommon.rotation44b = container->Get<CVector3>("transf_rotation44b"); //...........................
	transformCommon.scale3D111 = container->Get<CVector3>("transf_scale3D_111");
	transformCommon.scale3D222 = container->Get<CVector3>("transf_scale3D_222");
	transformCommon.scale3Da222 = container->Get<CVector3>("transf_scale3Da_222");
	transformCommon.scale3Db222 = container->Get<CVector3>("transf_scale3Db_222");
	transformCommon.scale3Dc222 = container->Get<CVector3>("transf_scale3Dc_222");
	transformCommon.scale3Dd222 = container->Get<CVector3>("transf_scale3Dd_222");
	transformCommon.scale3D333 = container->Get<CVector3>("transf_scale3D_333");
	transformCommon.scale3D444 = container->Get<CVector3>("transf_scale3D_444");

	transformCommon.additionConstant0000 = container->Get<CVector4>("transf_addition_constant_0000");
	transformCommon.offset1111 = container->Get<CVector4>("transf_offset_1111");
	transformCommon.offsetA1111 = container->Get<CVector4>("transf_offsetA_1111");
	transformCommon.additionConstant111d5 =
		container->Get<CVector4>("transf_addition_constant_111d5");
	transformCommon.constantMultiplier1220 =
		container->Get<CVector4>("transf_constant_multiplier_1220");

	transformCommon.addCpixelEnabled = container->Get<bool>("transf_addCpixel_enabled");
	transformCommon.addCpixelEnabledFalse = container->Get<bool>("transf_addCpixel_enabled_false");
	transformCommon.alternateEnabledFalse = container->Get<bool>("transf_alternate_enabled_false");
	transformCommon.benesiT1Enabled = container->Get<bool>("transf_benesi_T1_enabled");
	transformCommon.benesiT1EnabledFalse = container->Get<bool>("transf_benesi_T1_enabled_false");
	transformCommon.benesiT1MEnabledFalse = container->Get<bool>("transf_benesi_T1M_enabled_false");
	transformCommon.functionEnabled = container->Get<bool>("transf_function_enabled");
	transformCommon.functionEnabledFalse = container->Get<bool>("transf_function_enabled_false");
	transformCommon.functionEnabledx = container->Get<bool>("transf_function_enabledx");
	transformCommon.functionEnabledy = container->Get<bool>("transf_function_enabledy");
	transformCommon.functionEnabledz = container->Get<bool>("transf_function_enabledz");
	transformCommon.functionEnabledw = container->Get<bool>("transf_function_enabledw");
	transformCommon.functionEnabledxFalse = container->Get<bool>("transf_function_enabledx_false");
	transformCommon.functionEnabledyFalse = container->Get<bool>("transf_function_enabledy_false");
	transformCommon.functionEnabledzFalse = container->Get<bool>("transf_function_enabledz_false");
	transformCommon.functionEnabledAx = container->Get<bool>("transf_function_enabledAx");
	transformCommon.functionEnabledAy = container->Get<bool>("transf_function_enabledAy");
	transformCommon.functionEnabledAz = container->Get<bool>("transf_function_enabledAz");
	transformCommon.functionEnabledAw = container->Get<bool>("transf_function_enabledAw");
	transformCommon.functionEnabledAxFalse = container->Get<bool>("transf_function_enabledAx_false");
	transformCommon.functionEnabledAyFalse = container->Get<bool>("transf_function_enabledAy_false");
	transformCommon.functionEnabledAzFalse = container->Get<bool>("transf_function_enabledAz_false");
	transformCommon.functionEnabledAwFalse = container->Get<bool>("transf_function_enabledAw_false");
	transformCommon.functionEnabledBx = container->Get<bool>("transf_function_enabledBx");
	transformCommon.functionEnabledBy = container->Get<bool>("transf_function_enabledBy");
	transformCommon.functionEnabledBz = container->Get<bool>("transf_function_enabledBz");
	transformCommon.functionEnabledBxFalse = container->Get<bool>("transf_function_enabledBx_false");
	transformCommon.functionEnabledByFalse = container->Get<bool>("transf_function_enabledBy_false");
	transformCommon.functionEnabledBzFalse = container->Get<bool>("transf_function_enabledBz_false");
	transformCommon.functionEnabledCxFalse = container->Get<bool>("transf_function_enabledCx_false");
	transformCommon.functionEnabledCyFalse = container->Get<bool>("transf_function_enabledCy_false");
	transformCommon.functionEnabledCzFalse = container->Get<bool>("transf_function_enabledCz_false");
	transformCommon.functionEnabledFFalse = container->Get<bool>("transf_function_enabledF_false");
	transformCommon.functionEnabledKFalse = container->Get<bool>("transf_function_enabledK_false");
	transformCommon.functionEnabledM = container->Get<bool>("transf_function_enabledM");
	transformCommon.functionEnabledMFalse = container->Get<bool>("transf_function_enabledM_false");
	transformCommon.functionEnabledPFalse = container->Get<bool>("transf_function_enabledP_false");
	transformCommon.functionEnabledRFalse = container->Get<bool>("transf_function_enabledR_false");
	transformCommon.functionEnabledSFalse = container->Get<bool>("transf_function_enabledS_false");
	transformCommon.functionEnabledXFalse = container->Get<bool>("transf_function_enabledX_false");
	transformCommon.juliaMode = container->Get<bool>("transf_constant_julia_mode");
	transformCommon.rotationEnabled = container->Get<bool>("transf_rotation_enabled");

	WriteLog("cFractal::RecalculateFractalParams(void)", 2);

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
	transformCommon.alphaAngleOffset *= M_PI / 180.0;
	transformCommon.betaAngleOffset *= M_PI / 180.0;

	transformCommon.rotationMatrix44.SetRotation44a(transformCommon.rotation44a * (M_PI / 180.0)); // ..........................
	transformCommon.rotationMatrix44.SetRotation44b(transformCommon.rotation44b * (M_PI / 180.0)); // ...............................
	transformCommon.rotationMatrix.SetRotation2(transformCommon.rotation * (M_PI / 180.0));
	transformCommon.rotationMatrix2.SetRotation2(transformCommon.rotation2 * (M_PI / 180.0));
	transformCommon.tempRotmatrix.SetRotation2(transformCommon.rotation * (M_PI / 180.0));

	transformCommon.sqtR = sqrt(transformCommon.minR05);
	transformCommon.mboxFactor1 = 1.0 / transformCommon.sqtR; // hmmm??
	transformCommon.maxMinR2factor = transformCommon.maxR2d1 / transformCommon.minR2p25;

	// Generalized Fold Box precalculated vectors
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
