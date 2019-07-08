/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * sFractal struct - container for fractal formula parameters
 */

#include "fractal.h"

#include "algebra.hpp"
#include "parameters.hpp"
#include "system.hpp"

sFractal::sFractal(const cParameterContainer *container)
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
	mandelbox.offset = CVector4(container->Get<CVector3>("mandelbox_offset"), 0.0);
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
	mandelbox.rotationsEnabled = container->Get<bool>("mandelbox_rotations_enabled");
	mandelbox.mainRotationEnabled = container->Get<bool>("mandelbox_main_rotation_enabled");

	mandelboxVary4D.fold = container->Get<double>("mandelbox_vary_fold");
	mandelboxVary4D.minR = container->Get<double>("mandelbox_vary_minr");
	mandelboxVary4D.rPower = container->Get<double>("mandelbox_vary_rpower");
	mandelboxVary4D.scaleVary = container->Get<double>("mandelbox_vary_scale_vary");
	mandelboxVary4D.wadd = container->Get<double>("mandelbox_vary_wadd");

	mandelbox.solid = container->Get<double>("mandelbox_solid");
	mandelbox.melt = container->Get<double>("mandelbox_melt");
	genFoldBox.type =
		enumGeneralizedFoldBoxType(container->Get<int>("mandelbox_generalized_fold_type"));

	foldingIntPow.foldFactor = container->Get<double>("boxfold_bulbpow2_folding_factor");
	foldingIntPow.zFactor = container->Get<double>("boxfold_bulbpow2_z_factor");

	IFS.scale = container->Get<double>("IFS_scale");
	IFS.rotation = container->Get<CVector3>("IFS_rotation");
	IFS.rotationEnabled = container->Get<bool>("IFS_rotation_enabled");
	IFS.offset = CVector4(container->Get<CVector3>("IFS_offset"), 0.0);
	IFS.edge = container->Get<CVector3>("IFS_edge");
	IFS.edgeEnabled = container->Get<bool>("IFS_edge_enabled");

	IFS.absX = container->Get<bool>("IFS_abs_x");
	IFS.absY = container->Get<bool>("IFS_abs_y");
	IFS.absZ = container->Get<bool>("IFS_abs_z");
	IFS.mengerSpongeMode = container->Get<bool>("IFS_menger_sponge_mode");

	for (int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		IFS.direction[i] = CVector4(container->Get<CVector3>("IFS_direction", i), 0.0);
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
	mandelbulbMulti.acosOrAsin =
		enumMulti_acosOrAsin(container->Get<int>("mandelbulbMulti_acos_or_asin"));
	mandelbulbMulti.atanOrAtan2 =
		enumMulti_atanOrAtan2(container->Get<int>("mandelbulbMulti_atan_or_atan2"));

	mandelbulbMulti.acosOrAsinA =
		enumMulti_acosOrAsin(container->Get<int>("mandelbulbMulti_acos_or_asin_A"));
	mandelbulbMulti.atanOrAtan2A =
		enumMulti_atanOrAtan2(container->Get<int>("mandelbulbMulti_atan_or_atan2_A"));

	mandelbulbMulti.orderOfXYZ =
		enumMulti_OrderOfXYZ(container->Get<int>("mandelbulbMulti_order_of_xyz"));
	mandelbulbMulti.orderOfXYZ2 =
		enumMulti_OrderOfXYZ(container->Get<int>("mandelbulbMulti_order_of_xyz_2"));
	mandelbulbMulti.orderOfXYZC =
		enumMulti_OrderOfXYZ(container->Get<int>("mandelbulbMulti_order_of_xyz_C"));

	// sinTan2Trig
	sinTan2Trig.asinOrAcos = enumMulti_asinOrAcos(container->Get<int>("sinTan2Trig_asin_or_acos"));
	sinTan2Trig.atan2OrAtan = enumMulti_atan2OrAtan(container->Get<int>("sinTan2Trig_atan2_or_atan"));
	sinTan2Trig.orderOfZYX = enumMulti_OrderOfZYX(container->Get<int>("sinTan2Trig_order_of_zyx"));

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
	surfBox.offset1A111 = CVector4(container->Get<CVector3>("surfBox_offset1A_111"), 0.0);
	surfBox.offset1B111 = CVector4(container->Get<CVector3>("surfBox_offset1B_111"), 0.0);
	surfBox.offset2A111 = CVector4(container->Get<CVector3>("surfBox_offset2A_111"), 0.0);
	surfBox.offset2B111 = CVector4(container->Get<CVector3>("surfBox_offset2B_111"), 0.0);
	surfBox.offset3A111 = CVector4(container->Get<CVector3>("surfBox_offset3A_111"), 0.0);
	surfBox.offset3B111 = CVector4(container->Get<CVector3>("surfBox_offset3B_111"), 0.0);
	surfBox.offset1A222 = CVector4(container->Get<CVector3>("surfBox_offset1A_222"), 0.0);
	surfBox.offset1B222 = CVector4(container->Get<CVector3>("surfBox_offset1B_222"), 0.0);
	surfBox.scale1Z1 = container->Get<double>("surfBox_scale1Z1");

	// FIVE  surfFolds
	surfFolds.orderOfFolds1 =
		enumMulti_orderOfFolds(container->Get<int>("surfFolds_order_of_folds_1"));
	surfFolds.orderOfFolds2 =
		enumMulti_orderOfFolds(container->Get<int>("surfFolds_order_of_folds_2"));
	surfFolds.orderOfFolds3 =
		enumMulti_orderOfFolds(container->Get<int>("surfFolds_order_of_folds_3"));
	surfFolds.orderOfFolds4 =
		enumMulti_orderOfFolds(container->Get<int>("surfFolds_order_of_folds_4"));
	surfFolds.orderOfFolds5 =
		enumMulti_orderOfFolds(container->Get<int>("surfFolds_order_of_folds_5"));

	// THREE  asurf3Folds
	aSurf3Folds.orderOf3Folds1 =
		enumMulti_orderOf3Folds(container->Get<int>("aSurf3Folds_order_of_folds_1"));
	aSurf3Folds.orderOf3Folds2 =
		enumMulti_orderOf3Folds(container->Get<int>("aSurf3Folds_order_of_folds_2"));
	aSurf3Folds.orderOf3Folds3 =
		enumMulti_orderOf3Folds(container->Get<int>("aSurf3Folds_order_of_folds_3"));

	// combo4 multi
	combo4.combo4 = enumMulti_combo4(container->Get<int>("combo4"));

	// combo5 multi
	combo5.combo5 = enumMulti_combo5(container->Get<int>("combo5"));

	// combo6 multi
	combo6.combo6 = enumMulti_combo6(container->Get<int>("combo6"));

	// benesi mag transforms
	magTransf.orderOfTransf1 =
		enumMulti_orderOfTransf(container->Get<int>("magTransf_order_of_transf_1"));
	magTransf.orderOfTransf2 =
		enumMulti_orderOfTransf(container->Get<int>("magTransf_order_of_transf_2"));
	magTransf.orderOfTransf3 =
		enumMulti_orderOfTransf(container->Get<int>("magTransf_order_of_transf_3"));
	magTransf.orderOfTransf4 =
		enumMulti_orderOfTransf(container->Get<int>("magTransf_order_of_transf_4"));
	magTransf.orderOfTransf5 =
		enumMulti_orderOfTransf(container->Get<int>("magTransf_order_of_transf_5"));

	// basic comboBox
	combo.modeA = enumCombo(container->Get<int>("combo_mode_A"));

	//	combo.mode1 = (sFractalCombo::combo)container->Get<int>("combo_mode_B");
	//	combo.mode2 = (sFractalCombo::combo)container->Get<int>("combo_mode_C");

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
	analyticDE.enabled = container->Get<bool>("analyticDE_enabled");
	analyticDE.enabledFalse = container->Get<bool>("analyticDE_enabled_false");

	foldColor.auxColorEnabled = container->Get<bool>("fold_color_aux_color_enabled");
	foldColor.auxColorEnabledFalse = container->Get<bool>("fold_color_aux_color_enabled_false");

	foldColor.scaleA0 = container->Get<double>("fold_color_scaleA0");
	foldColor.scaleB0 = container->Get<double>("fold_color_scaleB0");
	foldColor.scaleD0 = container->Get<double>("fold_color_scaleD0");
	foldColor.scaleF0 = container->Get<double>("fold_color_scaleF0");
	foldColor.scaleA1 = container->Get<double>("fold_color_scaleA1");
	foldColor.scaleB1 = container->Get<double>("fold_color_scaleB1");

	foldColor.intAx0 = container->Get<int>("fold_color_int_Ax0");
	foldColor.intAy0 = container->Get<int>("fold_color_int_Ay0");
	foldColor.intAz0 = container->Get<int>("fold_color_int_Az0");

	foldColor.distanceEnabledFalse = container->Get<bool>("fold_color_distance_enabled_false");

	// common parameters for transforming formulas
	transformCommon.angle0 = container->Get<double>("transf_angle_0");
	transformCommon.alphaAngleOffset = container->Get<double>("transf_alpha_angle_offset");
	transformCommon.betaAngleOffset = container->Get<double>("transf_beta_angle_offset");
	transformCommon.foldingValue = container->Get<double>("transf_folding_value");
	transformCommon.foldingLimit = container->Get<double>("transf_folding_limit");
	transformCommon.multiplication = container->Get<double>("transf_multiplication");
	transformCommon.minR0 = container->Get<double>("transf_minimum_radius_0");
	transformCommon.minR05 = container->Get<double>("transf_minimum_radius_05");
	transformCommon.minR2p25 = container->Get<double>("transf_minR2_p25");
	transformCommon.maxR2d1 = container->Get<double>("transf_maxR2_1");
	transformCommon.minR06 = container->Get<double>("transf_minimum_radius_06");
	transformCommon.offset = container->Get<double>("transf_offset");
	transformCommon.offset0 = container->Get<double>("transf_offset_0");
	transformCommon.offsetA0 = container->Get<double>("transf_offsetA_0");
	transformCommon.offsetB0 = container->Get<double>("transf_offsetB_0");
	transformCommon.offsetC0 = container->Get<double>("transf_offsetC_0");
	transformCommon.offset0005 = container->Get<double>("transf_offset_0005");
	transformCommon.offset05 = container->Get<double>("transf_offset_05");
	transformCommon.offset1 = container->Get<double>("transf_offset_1");
	transformCommon.offsetA1 = container->Get<double>("transf_offsetA_1");
	transformCommon.offset105 = container->Get<double>("transf_offset_105");
	transformCommon.offset2 = container->Get<double>("transf_offset_2");
	transformCommon.offset4 = container->Get<double>("transf_offset_4");
	transformCommon.pwr05 = container->Get<double>("transf_pwr_05");
	transformCommon.pwr4 = container->Get<double>("transf_pwr_4");
	transformCommon.pwr8 = container->Get<double>("transf_pwr_8");
	transformCommon.pwr8a = container->Get<double>("transf_pwr_8a");
	transformCommon.scale = container->Get<double>("transf_scale");
	transformCommon.scale0 = container->Get<double>("transf_scale_0");
	transformCommon.scale025 = container->Get<double>("transf_scale_025");
	transformCommon.scale05 = container->Get<double>("transf_scale_05");
	transformCommon.scale08 = container->Get<double>("transf_scale_08");
	transformCommon.scale1 = container->Get<double>("transf_scale_1");
	transformCommon.scaleA1 = container->Get<double>("transf_scaleA_1");
	transformCommon.scaleB1 = container->Get<double>("transf_scaleB_1");
	transformCommon.scaleC1 = container->Get<double>("transf_scaleC_1");
	transformCommon.scaleD1 = container->Get<double>("transf_scaleD_1");
	transformCommon.scaleE1 = container->Get<double>("transf_scaleE_1");
	transformCommon.scaleF1 = container->Get<double>("transf_scaleF_1");
	transformCommon.scaleG1 = container->Get<double>("transf_scaleG_1");
	transformCommon.scale015 = container->Get<double>("transf_scale_015");
	transformCommon.scaleA2 = container->Get<double>("transf_scaleA_2");
	transformCommon.scale2 = container->Get<double>("transf_scale_2");
	transformCommon.scale3 = container->Get<double>("transf_scale_3");
	transformCommon.scaleA3 = container->Get<double>("transf_scaleA_3");
	transformCommon.scaleB3 = container->Get<double>("transf_scaleB_3");
	transformCommon.scale4 = container->Get<double>("transf_scale_4");
	transformCommon.scale8 = container->Get<double>("transf_scale_8");

	transformCommon.scaleMain2 = container->Get<double>("transf_scale_main_2");
	transformCommon.scaleVary0 = container->Get<double>("transf_scale_vary_0");

	transformCommon.intA = container->Get<int>("transf_int_A");
	transformCommon.intB = container->Get<int>("transf_int_B");
	transformCommon.int1 = container->Get<int>("transf_int_1");
	transformCommon.int2 = container->Get<int>("transf_int_2");
	transformCommon.int3 = container->Get<int>("transf_int_3");
	transformCommon.int6 = container->Get<int>("transf_int_6");
	transformCommon.int8X = container->Get<int>("transf_int8_X");
	transformCommon.int8Y = container->Get<int>("transf_int8_Y");
	transformCommon.int8Z = container->Get<int>("transf_int8_Z");
	transformCommon.startIterations = container->Get<int>("transf_start_iterations");
	transformCommon.startIterations250 = container->Get<int>("transf_start_iterations_250");
	transformCommon.stopIterations = container->Get<int>("transf_stop_iterations");
	transformCommon.stopIterations15 = container->Get<int>("transf_stop_iterations_15");
	transformCommon.startIterationsA = container->Get<int>("transf_start_iterations_A");
	transformCommon.stopIterationsA = container->Get<int>("transf_stop_iterations_A");
	transformCommon.startIterationsB = container->Get<int>("transf_start_iterations_B");
	transformCommon.stopIterationsB = container->Get<int>("transf_stop_iterations_B");
	transformCommon.startIterationsC = container->Get<int>("transf_start_iterations_C");
	transformCommon.stopIterationsC = container->Get<int>("transf_stop_iterations_C");
	transformCommon.stopIterationsC1 = container->Get<int>("transf_stop_iterations_C1");
	transformCommon.startIterationsD = container->Get<int>("transf_start_iterations_D");
	transformCommon.stopIterationsD = container->Get<int>("transf_stop_iterations_D");
	transformCommon.stopIterationsD1 = container->Get<int>("transf_stop_iterations_D1");
	transformCommon.startIterationsE = container->Get<int>("transf_start_iterations_E");
	transformCommon.stopIterationsE = container->Get<int>("transf_stop_iterations_E");
	transformCommon.startIterationsF = container->Get<int>("transf_start_iterations_F");
	transformCommon.stopIterationsF = container->Get<int>("transf_stop_iterations_F");
	transformCommon.startIterationsG = container->Get<int>("transf_start_iterations_G");
	transformCommon.stopIterationsG = container->Get<int>("transf_stop_iterations_G");
	transformCommon.startIterationsH = container->Get<int>("transf_start_iterations_H");
	transformCommon.stopIterationsH = container->Get<int>("transf_stop_iterations_H");
	transformCommon.startIterationsI = container->Get<int>("transf_start_iterations_I");
	transformCommon.stopIterationsI = container->Get<int>("transf_stop_iterations_I");
	transformCommon.startIterationsJ = container->Get<int>("transf_start_iterations_J");
	transformCommon.stopIterationsJ = container->Get<int>("transf_stop_iterations_J");
	transformCommon.startIterationsK = container->Get<int>("transf_start_iterations_K");
	transformCommon.stopIterationsK = container->Get<int>("transf_stop_iterations_K");

	transformCommon.startIterationsM = container->Get<int>("transf_start_iterations_M");
	transformCommon.stopIterationsM = container->Get<int>("transf_stop_iterations_M");
	transformCommon.startIterationsO = container->Get<int>("transf_start_iterations_O");
	transformCommon.stopIterationsO = container->Get<int>("transf_stop_iterations_O");
	transformCommon.startIterationsP = container->Get<int>("transf_start_iterations_P");
	transformCommon.stopIterationsP1 = container->Get<int>("transf_stop_iterations_P1");
	transformCommon.startIterationsR = container->Get<int>("transf_start_iterations_R");
	transformCommon.stopIterationsR = container->Get<int>("transf_stop_iterations_R");
	transformCommon.startIterationsRV = container->Get<int>("transf_start_iterations_RV");
	transformCommon.stopIterationsRV = container->Get<int>("transf_stop_iterations_RV");
	transformCommon.startIterationsS = container->Get<int>("transf_start_iterations_S");
	transformCommon.stopIterationsS = container->Get<int>("transf_stop_iterations_S");
	transformCommon.startIterationsT = container->Get<int>("transf_start_iterations_T");
	transformCommon.stopIterationsT = container->Get<int>("transf_stop_iterations_T");
	transformCommon.stopIterationsT1 = container->Get<int>("transf_stop_iterationsT_1");
	transformCommon.startIterationsTM = container->Get<int>("transf_start_iterationsTM");
	transformCommon.stopIterationsTM1 = container->Get<int>("transf_stop_iterationsTM_1");

	transformCommon.stopIterations1 = container->Get<int>("transf_stop_iterations_1");

	transformCommon.startIterationsX = container->Get<int>("transf_start_iterations_X");
	transformCommon.stopIterationsX = container->Get<int>("transf_stop_iterations_X");
	transformCommon.startIterationsY = container->Get<int>("transf_start_iterations_Y");
	transformCommon.stopIterationsY = container->Get<int>("transf_stop_iterations_Y");
	transformCommon.startIterationsZ = container->Get<int>("transf_start_iterations_Z");
	transformCommon.stopIterationsZ = container->Get<int>("transf_stop_iterations_Z");

	transformCommon.additionConstant0555 =
		CVector4(container->Get<CVector3>("transf_addition_constant_0555"), 0.0);
	transformCommon.additionConstant0777 =
		CVector4(container->Get<CVector3>("transf_addition_constant_0777"), 0.0);
	transformCommon.additionConstant000 =
		CVector4(container->Get<CVector3>("transf_addition_constant"), 0.0);
	transformCommon.additionConstantA000 =
		CVector4(container->Get<CVector3>("transf_addition_constantA_000"), 0.0);
	transformCommon.additionConstantP000 =
		CVector4(container->Get<CVector3>("transf_addition_constantP_000"), 0.0);
	transformCommon.additionConstant111 =
		CVector4(container->Get<CVector3>("transf_addition_constant_111"), 0.0);
	transformCommon.additionConstantA111 =
		CVector4(container->Get<CVector3>("transf_addition_constantA_111"), 0.0);
	transformCommon.additionConstant222 =
		CVector4(container->Get<CVector3>("transf_addition_constant_222"), 0.0);
	transformCommon.additionConstantNeg100 =
		CVector4(container->Get<CVector3>("transf_addition_constant_neg100"), 0.0);

	transformCommon.constantMultiplier000 =
		CVector4(container->Get<CVector3>("transf_constant_multiplier_000"), 1.0);
	transformCommon.constantMultiplier001 =
		CVector4(container->Get<CVector3>("transf_constant_multiplier_001"), 1.0);
	transformCommon.constantMultiplier010 =
		CVector4(container->Get<CVector3>("transf_constant_multiplier_010"), 1.0);
	transformCommon.constantMultiplier100 =
		CVector4(container->Get<CVector3>("transf_constant_multiplier_100"), 1.0);
	transformCommon.constantMultiplierA100 =
		CVector4(container->Get<CVector3>("transf_constant_multiplierA_100"), 1.0);
	transformCommon.constantMultiplier111 =
		CVector4(container->Get<CVector3>("transf_constant_multiplier_111"), 1.0);
	transformCommon.constantMultiplierA111 =
		CVector4(container->Get<CVector3>("transf_constant_multiplierA_111"), 1.0);
	transformCommon.constantMultiplierB111 =
		CVector4(container->Get<CVector3>("transf_constant_multiplierB_111"), 1.0);
	transformCommon.constantMultiplierC111 =
		CVector4(container->Get<CVector3>("transf_constant_multiplierC_111"), 1.0);
	transformCommon.constantMultiplier121 =
		CVector4(container->Get<CVector3>("transf_constant_multiplier_121"), 1.0);
	transformCommon.constantMultiplier122 =
		CVector4(container->Get<CVector3>("transf_constant_multiplier_122"), 1.0);
	transformCommon.constantMultiplier221 =
		CVector4(container->Get<CVector3>("transf_constant_multiplier_221"), 1.0);
	transformCommon.constantMultiplier222 =
		CVector4(container->Get<CVector3>("transf_constant_multiplier_222"), 1.0);
	transformCommon.constantMultiplier441 =
		CVector4(container->Get<CVector3>("transf_constant_multiplier_441"), 1.0);

	transformCommon.juliaC = CVector4(container->Get<CVector3>("transf_constant_julia_c"), 0.0);
	transformCommon.offset000 = CVector4(container->Get<CVector3>("transf_offset_000"), 0.0);
	transformCommon.offsetA000 = CVector4(container->Get<CVector3>("transf_offsetA_000"), 0.0);
	transformCommon.offsetF000 = CVector4(container->Get<CVector3>("transf_offsetF_000"), 0.0);
	transformCommon.offset010 = CVector4(container->Get<CVector3>("transf_offset_010"), 0.0);
	transformCommon.offset100 = CVector4(container->Get<CVector3>("transf_offset_100"), 0.0);
	transformCommon.offset1105 = CVector4(container->Get<CVector3>("transf_offset_1105"), 0.0);
	transformCommon.offset111 = CVector4(container->Get<CVector3>("transf_offset_111"), 0.0);
	transformCommon.offsetA111 = CVector4(container->Get<CVector3>("transf_offsetA_111"), 0.0);
	transformCommon.offsetB111 = CVector4(container->Get<CVector3>("transf_offsetB_111"), 0.0);
	transformCommon.offsetC111 = CVector4(container->Get<CVector3>("transf_offsetC_111"), 0.0);
	transformCommon.offset200 = CVector4(container->Get<CVector3>("transf_offset_200"), 0.0);
	transformCommon.offsetA200 = CVector4(container->Get<CVector3>("transf_offsetA_200"), 0.0);
	transformCommon.offset222 = CVector4(container->Get<CVector3>("transf_offset_222"), 0.0);
	transformCommon.offsetA222 = CVector4(container->Get<CVector3>("transf_offsetA_222"), 0.0);
	transformCommon.power025 = CVector4(container->Get<CVector3>("transf_power_025"), 0.0);
	transformCommon.power8 = CVector4(container->Get<CVector3>("transf_power_8"), 0.0);

	transformCommon.rotation = container->Get<CVector3>("transf_rotation");
	transformCommon.rotation2 = container->Get<CVector3>("transf_rotation2");
	transformCommon.rotationVary = container->Get<CVector3>("transf_rotationVary");

	transformCommon.rotation44a =
		container->Get<CVector3>("transf_rotation44a"); //...........................
	transformCommon.rotation44b =
		container->Get<CVector3>("transf_rotation44b"); //...........................

	transformCommon.scaleP222 = CVector4(container->Get<CVector3>("transf_scaleP_222"), 1.0);
	transformCommon.scale3D000 = CVector4(container->Get<CVector3>("transf_scale3D_000"), 1.0);
	transformCommon.scale3D111 = CVector4(container->Get<CVector3>("transf_scale3D_111"), 1.0);
	transformCommon.scale3D222 = CVector4(container->Get<CVector3>("transf_scale3D_222"), 1.0);
	transformCommon.scale3Da222 = CVector4(container->Get<CVector3>("transf_scale3Da_222"), 1.0);
	transformCommon.scale3Db222 = CVector4(container->Get<CVector3>("transf_scale3Db_222"), 1.0);
	transformCommon.scale3Dc222 = CVector4(container->Get<CVector3>("transf_scale3Dc_222"), 1.0);
	transformCommon.scale3Dd222 = CVector4(container->Get<CVector3>("transf_scale3Dd_222"), 1.0);
	transformCommon.scale3D333 = CVector4(container->Get<CVector3>("transf_scale3D_333"), 1.0);
	transformCommon.scale3D444 = CVector4(container->Get<CVector3>("transf_scale3D_444"), 1.0);
	transformCommon.vec111 = CVector4(container->Get<CVector3>("transf_vec_111"), 0.0);

	// 4d vec
	transformCommon.additionConstant0000 = container->Get<CVector4>("transf_addition_constant_0000");
	transformCommon.offset0000 = container->Get<CVector4>("transf_offset_0000");
	transformCommon.offsetA0000 = container->Get<CVector4>("transf_offsetA_0000");
	transformCommon.offset1111 = container->Get<CVector4>("transf_offset_1111");
	transformCommon.offsetA1111 = container->Get<CVector4>("transf_offsetA_1111");
	transformCommon.offsetB1111 = container->Get<CVector4>("transf_offsetB_1111");
	transformCommon.offsetNeg1111 = container->Get<CVector4>("transf_offset_neg_1111");
	transformCommon.offset2222 = container->Get<CVector4>("transf_offset_2222");
	transformCommon.additionConstant111d5 =
		container->Get<CVector4>("transf_addition_constant_111d5");
	transformCommon.constantMultiplier1220 =
		container->Get<CVector4>("transf_constant_multiplier_1220");
	transformCommon.scale0000 = container->Get<CVector4>("transf_scale_0000");
	transformCommon.scale1111 = container->Get<CVector4>("transf_scale_1111");

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
	transformCommon.functionEnabledwFalse = container->Get<bool>("transf_function_enabledw_false");
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
	transformCommon.functionEnabledCx = container->Get<bool>("transf_function_enabledCx");
	transformCommon.functionEnabledCy = container->Get<bool>("transf_function_enabledCy");
	transformCommon.functionEnabledCz = container->Get<bool>("transf_function_enabledCz");
	transformCommon.functionEnabledCxFalse = container->Get<bool>("transf_function_enabledCx_false");
	transformCommon.functionEnabledCyFalse = container->Get<bool>("transf_function_enabledCy_false");
	transformCommon.functionEnabledCzFalse = container->Get<bool>("transf_function_enabledCz_false");
	transformCommon.functionEnabledDFalse = container->Get<bool>("transf_function_enabledD_false");
	transformCommon.functionEnabledEFalse = container->Get<bool>("transf_function_enabledE_false");
	transformCommon.functionEnabledFFalse = container->Get<bool>("transf_function_enabledF_false");
	transformCommon.functionEnabledJFalse = container->Get<bool>("transf_function_enabledJ_false");
	transformCommon.functionEnabledKFalse = container->Get<bool>("transf_function_enabledK_false");
	transformCommon.functionEnabledM = container->Get<bool>("transf_function_enabledM");
	transformCommon.functionEnabledMFalse = container->Get<bool>("transf_function_enabledM_false");
	transformCommon.functionEnabledPFalse = container->Get<bool>("transf_function_enabledP_false");
	transformCommon.functionEnabledRFalse = container->Get<bool>("transf_function_enabledR_false");
	transformCommon.functionEnabledSFalse = container->Get<bool>("transf_function_enabledS_false");
	transformCommon.functionEnabledSwFalse = container->Get<bool>("transf_function_enabledSw_false");
	transformCommon.functionEnabledXFalse = container->Get<bool>("transf_function_enabledX_false");
	transformCommon.juliaMode = container->Get<bool>("transf_constant_julia_mode");
	transformCommon.rotationEnabled = container->Get<bool>("transf_rotation_enabled");
	transformCommon.rotation2EnabledFalse = container->Get<bool>("transf_rotation2_enabled_false");
	transformCommon.sphereInversionEnabledFalse =
		container->Get<bool>("transf_sphere_inversion_enabled_false");
	transformCommon.spheresEnabled = container->Get<bool>("transf_spheres_enabled");

	// transformCommon.functionEnabledTempFalse =
	//	container->Get<bool>("transf_function_enabled_temp_false");

	WriteLog("cFractal::RecalculateFractalParams(void)", 2);

	RecalculateFractalParams();
}
void sFractal::RecalculateFractalParams()
{
	IFS.mainRot.SetRotation3(IFS.rotation * (M_PI / 180.0));

	for (int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		IFS.rot[i].SetRotation3(IFS.rotations[i] * (M_PI / 180.0));
		IFS.direction[i].Normalize();
	}

	mandelbox.mainRot.SetRotation2(mandelbox.rotationMain * M_PI_180);

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

	transformCommon.rotationMatrix44.SetRotation44a(
		transformCommon.rotation44a * (M_PI / 180.0)); // ..........................
	transformCommon.rotationMatrix44.SetRotation44b(
		transformCommon.rotation44b * (M_PI / 180.0)); // ...............................
	transformCommon.rotationMatrix.SetRotation2(transformCommon.rotation * (M_PI / 180.0));
	transformCommon.rotationMatrix2.SetRotation2(transformCommon.rotation2 * (M_PI / 180.0));
	transformCommon.rotationMatrixVary.SetRotation2(transformCommon.rotationVary * M_PI_180);

	transformCommon.sqtR = sqrt(transformCommon.minR05);
	transformCommon.mboxFactor1 = 1.0 / transformCommon.sqtR; // for orig. abox asurf

	transformCommon.maxMinR2factor = transformCommon.maxR2d1 / transformCommon.minR2p25;

	// Generalized Fold Box pre calculated vectors
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
