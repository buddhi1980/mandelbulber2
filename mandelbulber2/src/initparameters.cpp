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
 * InitParams function - initialization of all parameters
 */

#include "color_palette.hpp"
#include "files.h"
#include "fractal.h"
#include "fractal_list.hpp"
#include "fractparams.hpp"
#include "material.h"
#include "netrender.hpp"
#include "parameters.hpp"
#include "stereo.h"
#include "system.hpp"
#include <QtCore>
#include "texture_enums.hpp"

cParameterContainer *gPar = NULL;

// definition of all parameters
void InitParams(cParameterContainer *par)
{
	using namespace parameterContainer;

	WriteLog("Parameters initialization started: " + par->GetContainerName(), 3);

	// image
	par->addParam("image_width", 800, 5, 65535, morphNone, paramStandard);
	par->addParam("image_height", 600, 5, 65535, morphNone, paramStandard);
	par->addParam("tiles", 1, 1, 64, morphNone, paramStandard);
	par->addParam("tile_number", 0, morphNone, paramStandard);
	par->addParam("image_proportion", 0, morphNone, paramNoSave);

	// flight animation
	par->addParam("flight_first_to_render", 0, 0, 99999, morphNone, paramStandard);
	par->addParam("flight_last_to_render", 1000, 0, 99999, morphNone, paramStandard);

	par->addParam("frame_no", 0, 0, 99999, morphNone, paramOnlyForNet);
	par->addParam("flight_speed", 0.01, -1000.0, 1000.0, morphLinear, paramStandard);
	par->addParam("flight_inertia", 5.0, 0.01, 1000.0, morphLinear, paramStandard);
	par->addParam("flight_speed_control", 0, morphNone, paramStandard);
	par->addParam("flight_roll_speed", 10.0, morphNone, paramStandard);
	par->addParam("flight_rotation_speed", 10.0, morphNone, paramStandard);
	par->addParam("flight_show_thumbnails", false, morphNone, paramApp);
	par->addParam("flight_add_speeds", true, morphNone, paramStandard);
	par->addParam("flight_movement_speed_vector", CVector3(0.0, 0.0, 0.0), morphNone, paramStandard);
	par->addParam("flight_rotation_speed_vector", CVector3(0.0, 0.0, 0.0), morphNone, paramStandard);
	par->addParam("flight_sec_per_frame", 1.0, morphNone, paramApp);
	par->addParam("flight_animation_image_type", 0, morphNone, paramApp);
	par->addParam("anim_flight_dir", systemData.GetImagesFolder() + QDir::separator(),
		morphNone, paramStandard);

	// keyframe animation
	par->addParam("frames_per_keyframe", 100, 1, 99999, morphNone, paramStandard);
	par->addParam("keyframe_first_to_render", 0, 0, 99999, morphNone, paramStandard);
	par->addParam("keyframe_last_to_render", 1000, 0, 99999, morphNone, paramStandard);
	par->addParam("show_keyframe_thumbnails", false, morphNone, paramApp);
	par->addParam("keyframe_animation_image_type", 0, morphNone, paramApp);
	par->addParam("anim_keyframe_dir", systemData.GetImagesFolder() + QDir::separator(),
		morphNone, paramStandard);
	par->addParam("keyframe_collision_thresh", 1.0e-6, 1e-15, 1.0e2, morphNone, paramStandard);
	par->addParam("keyframe_auto_validate", true, morphNone, paramApp);
	par->addParam("keyframe_constant_target_distance", 0.1, 1e-10, 1.0e2, morphNone, paramStandard);

	// camera
	par->addParam("camera", CVector3(3.0, -6.0, 2.0), morphAkima, paramStandard);
	par->addParam("target", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	// internal vector which represents top direction
	par->addParam("camera_top", CVector3(-0.1277753, 0.2555506, 0.958314), morphAkima, paramStandard);
	par->addParam(
		"camera_rotation", CVector3(26.5650, -16.60154, 0.0), morphAkimaAngle, paramStandard);
	par->addParam("camera_distance_to_target", 7.0, 0.0, 1e15, morphAkima, paramStandard);
	par->addParam("fov", 1.0, 0.0, 100.0, morphAkima, paramStandard);
	par->addParam("perspective_type", 0, morphLinear, paramStandard);
	par->addParam("legacy_coordinate_system", false, morphNone, paramStandard);
	par->addParam("sweet_spot_horizontal_angle", 0.0, -180.0, 180.0, morphAkima, paramStandard);
	par->addParam("sweet_spot_vertical_angle", 0.0, -90.0, 90.0, morphAkima, paramStandard);

	// fractal formula selections
	par->addParam("formula", 1, (int)fractal::mandelbulb, morphNone, paramStandard);
	par->addParam("formula_iterations", 1, 1, 1, 65536, morphNone, paramStandard);

	for (int i = 2; i <= NUMBER_OF_FRACTALS; i++)
	{
		par->addParam("formula", i, (int)fractal::none, morphNone, paramStandard);
		par->addParam("formula_iterations", i, 1, 1, 65536, morphNone, paramStandard);
	}

	for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
	{
		par->addParam("fractal_enable", i, true, morphLinear, paramStandard);
		par->addParam("formula_weight", i, 1.0, 0.0, 1.0, morphAkima, paramStandard);
		par->addParam("formula_start_iteration", i, 0, 0, 65536, morphAkima, paramStandard);
		par->addParam("formula_stop_iteration", i, 250, 0, 65536, morphAkima, paramStandard);
		par->addParam("julia_mode", i, false, morphLinear, paramStandard);
		par->addParam("julia_c", i, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
		par->addParam(
			"fractal_constant_factor", i, CVector3(1.0, 1.0, 1.0), morphLinear, paramStandard);
		par->addParam("initial_waxis", i, 0.0, morphAkima, paramStandard);
	}

	// boolean operators
	par->addParam("boolean_operators", false, morphLinear, paramStandard);
	for (int i = 1; i < NUMBER_OF_FRACTALS; i++)
	{
		par->addParam(
			"boolean_operator", i, (int)params::booleanOperatorOR, morphLinear, paramStandard);
	}

	// fractal transform
	for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
	{
		par->addParam("formula_position", i, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
		par->addParam("formula_rotation", i, CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
		par->addParam("formula_repeat", i, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
		par->addParam("formula_scale", i, 1.0, morphAkima, paramStandard);
		par->addParam("dont_add_c_constant", i, false, morphLinear, paramStandard);
		par->addParam("check_for_bailout", i, true, morphLinear, paramStandard);
		par->addParam("formula_material_id", i, 1, morphLinear, paramStandard);
	}
	par->addParam("formula_material_id", 1, morphLinear, paramStandard);

	// general fractal and engine
	par->addParam("julia_mode", false, morphLinear, paramStandard);
	par->addParam("julia_c", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("N", 250, 1, 99999, morphLinear, paramStandard);
	par->addParam("minN", 1, 0, 99999, morphLinear, paramStandard);
	par->addParam("fractal_constant_factor", CVector3(1.0, 1.0, 1.0), morphLinear, paramStandard);
	par->addParam("detail_level", 1.0, 1e-8, 1e8, morphLinear, paramStandard);
	// DE_thresh: old name was 'quality'
	par->addParam("DE_thresh", 0.01, 1e-15, 1e5, morphLinear, paramStandard);
	par->addParam("smoothness", 1.0, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("iteration_threshold_mode", false, morphNone, paramStandard);
	par->addParam("analityc_DE_mode", true, morphNone, paramStandard);
	par->addParam("DE_factor", 1.0, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("slow_shading", false, morphLinear, paramStandard);
	par->addParam("view_distance_max", 50.0, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("view_distance_min", 1e-15, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("limit_min", CVector3(-10.0, -10.0, -10.0), morphLinear, paramStandard);
	par->addParam("limit_max", CVector3(10.0, 10.0, 10.0), morphLinear, paramStandard);
	par->addParam("limits_enabled", false, morphLinear, paramStandard);
	par->addParam("limit_outer_bounding", 100.0, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("interior_mode", false, morphLinear, paramStandard);
	par->addParam("constant_DE_threshold", false, morphLinear, paramStandard);
	par->addParam("hybrid_fractal_enable", false, morphNone, paramStandard);
	par->addParam("bailout", 1e2, 1.0, 1e15, morphLinear, paramStandard);
	par->addParam("repeat_from", 1, 1, 9, morphLinear, paramStandard);
	par->addParam(
		"delta_DE_function", (int)fractal::preferedDEfunction, 0, 3, morphNone, paramStandard);
	par->addParam("delta_DE_method", (int)fractal::preferedDEMethod, 0, 1, morphNone, paramStandard);
	par->addParam("use_default_bailout", true, morphNone, paramStandard);
	par->addParam("initial_waxis", 0.0, morphAkima, paramStandard);

	// stereoscopic
	par->addParam("stereo_enabled", false, morphLinear, paramStandard);
	par->addParam("stereo_eye_distance", 0.1, 1e-15, 1e15, morphAkima, paramStandard);
	par->addParam("stereo_mode", (int)cStereo::stereoLeftRight, morphLinear, paramStandard);
	par->addParam("stereo_swap_eyes", false, morphLinear, paramStandard);
	par->addParam("stereo_infinite_correction", 0.0, 0.0, 10.0, morphAkima, paramStandard);
	par->addParam("stereo_actual_eye", (int)cStereo::eyeNone, morphAkima, paramOnlyForNet);

	// volume slicing
	par->addParam("voxel_limit_min", CVector3(-1.0, -1.0, -1.0), morphLinear, paramStandard);
	par->addParam("voxel_limit_max", CVector3(1.0, 1.0, 1.0), morphLinear, paramStandard);
	par->addParam("voxel_custom_limit_enabled", false, morphLinear, paramStandard);
	par->addParam("voxel_samples_x", 100, 2, 65535, morphLinear, paramStandard);
	par->addParam("voxel_samples_y", 100, 2, 65535, morphLinear, paramStandard);
	par->addParam("voxel_samples_z", 100, 2, 65535, morphLinear, paramStandard);
	par->addParam("voxel_max_iter", 30, 1, 10000, morphLinear, paramStandard);
	par->addParam("voxel_image_path",
		QDir::toNativeSeparators(systemData.GetSlicesFolder() + QDir::separator()), morphNone,
		paramStandard);
	par->addParam("voxel_show_information", true, morphLinear, paramApp);

    // mesh export
    par->addParam("mesh_output_filename", systemData.GetSlicesFolder() +
        QDir::separator() + "output.ply", morphNone, paramStandard);

	// foldings
	par->addParam("box_folding", false, morphLinear, paramStandard);
	par->addParam("box_folding_limit", 1.0, morphLinear, paramStandard);
	par->addParam("box_folding_value", 2.0, morphLinear, paramStandard);
	par->addParam("spherical_folding", false, morphLinear, paramStandard);
	par->addParam("spherical_folding_outer", 1.0, morphLinear, paramStandard);
	par->addParam("spherical_folding_inner", 0.5, morphLinear, paramStandard);
	par->addParam("c_add", -1.3, morphLinear, paramStandard);

	// image effects
	par->addParam("brightness", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("contrast", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("gamma", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("hdr", false, morphLinear, paramStandard);
	par->addParam("ambient_occlusion", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_quality", 4, 1, 10, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_fast_tune", 1.0, 1e-5, 1e5, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_enabled", false, morphLinear, paramStandard);
	par->addParam(
		"ambient_occlusion_mode", (int)params::AOmodeScreenSpace, morphLinear, paramStandard);
	par->addParam("SSAO_random_mode", false, morphLinear, paramStandard);
	par->addParam("glow_enabled", true, morphLinear, paramStandard);
	par->addParam("glow_intensity", 0.2, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("textured_background", false, morphLinear, paramStandard);
	par->addParam(
		"textured_background_map_type", (int)params::mapEquirectangular, morphNone, paramStandard);
	par->addParam("background_brightness", 1.0, 0.0, 1000.0, morphLinear, paramStandard);
	par->addParam("shadows_enabled", true, morphLinear, paramStandard);
	par->addParam("penetrating_lights", true, morphLinear, paramStandard);
	par->addParam("raytraced_reflections", false, morphLinear, paramStandard);
	par->addParam("reflections_max", 5, 0, 10, morphLinear, paramStandard);
	par->addParam("env_mapping_enable", false, morphLinear, paramStandard);

	par->addParam("glow_color", 1, sRGB(40984, 44713, 49490), morphLinear, paramStandard);
	par->addParam("glow_color", 2, sRGB(57192, 60888, 62408), morphLinear, paramStandard);
	par->addParam("background_color", 1, sRGB(0, 38306, 65535), morphLinear, paramStandard);
	par->addParam("background_color", 2, sRGB(65535, 65535, 65535), morphLinear, paramStandard);
	par->addParam("background_color", 3, sRGB(0, 10000, 500), morphLinear, paramStandard);
	par->addParam("fog_color", 1, sRGB(30000, 30000, 30000), morphLinear, paramStandard);
	par->addParam("fog_color", 2, sRGB(0, 30000, 65535), morphLinear, paramStandard);
	par->addParam("fog_color", 3, sRGB(65535, 65535, 65535), morphLinear, paramStandard);

	par->addParam("volumetric_fog_enabled", false, morphLinear, paramStandard);
	par->addParam("volumetric_fog_density", 0.5, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("volumetric_fog_colour_1_distance", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("volumetric_fog_colour_2_distance", 2.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("volumetric_fog_distance_factor", 1.0, 0.0, 1e15, morphLinear, paramStandard);

	par->addParam("iteration_fog_enable", false, morphLinear, paramStandard);
	par->addParam("iteration_fog_opacity", 1000.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("iteration_fog_opacity_trim", 4.0, 0.0, 1000.0, morphLinear, paramStandard);
	par->addParam("iteration_fog_color_1_maxiter", 8.0, 0.0, 1000.0, morphLinear, paramStandard);
	par->addParam("iteration_fog_color_2_maxiter", 12.0, 0.0, 1000.0, morphLinear, paramStandard);
	par->addParam("iteration_fog_color", 1, sRGB(65535, 65535, 65535), morphLinear, paramStandard);
	par->addParam("iteration_fog_color", 2, sRGB(65535, 65535, 65535), morphLinear, paramStandard);
	par->addParam("iteration_fog_color", 3, sRGB(65535, 65535, 65535), morphLinear, paramStandard);

	// fog
	par->addParam("basic_fog_enabled", false, morphLinear, paramStandard);
	par->addParam("basic_fog_visibility", 20.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("basic_fog_color", sRGB(59399, 61202, 65535), morphLinear, paramStandard);
	par->addParam("DOF_enabled", false, morphLinear, paramStandard);
	par->addParam("DOF_focus", 6.0, 0.0, 200.0, morphLinear, paramStandard);
	par->addParam("DOF_radius", 10.0, 0.0, 200.0, morphLinear, paramStandard);
	par->addParam("DOF_HDR", false, morphLinear, paramStandard);
	par->addParam("DOF_number_of_passes", 1, 1, 10, morphLinear, paramStandard);
	par->addParam("DOF_blur_opacity", 4.0, 0.01, 10.0, morphLinear, paramStandard);
	par->addParam("DOF_monte_carlo", false, morphLinear, paramStandard);
	par->addParam("DOF_samples", 100, morphLinear, paramStandard);

	// main light
	par->addParam("main_light_intensity", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("main_light_visibility", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("main_light_visibility_size", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("main_light_alpha", -45.0, morphAkimaAngle, paramStandard);
	par->addParam("main_light_beta", 45.0, morphAkimaAngle, paramStandard);
	par->addParam("main_light_colour", sRGB(65535, 65535, 65535), morphLinear, paramStandard);
	par->addParam("shadows_cone_angle", 1.0, 0.0, 180.0, morphLinear, paramStandard);
	par->addParam("main_light_enable", true, morphLinear, paramStandard);
	par->addParam("main_light_position_relative", true, morphLinear, paramStandard);
	par->addParam("main_light_volumetric_intensity", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("main_light_volumetric_enabled", false, morphLinear, paramStandard);

	// aux lights
	par->addParam("aux_light_intensity", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("aux_light_visibility", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("aux_light_visibility_size", 1.0, 0.0, 1e15, morphLinear, paramStandard);

	par->addParam("aux_light_position", 1, CVector3(3.0, -3.0, 3.0), morphAkima, paramStandard);
	par->addParam("aux_light_position", 2, CVector3(-3.0, -3.0, 0.0), morphAkima, paramStandard);
	par->addParam("aux_light_position", 3, CVector3(-3.0, 3.0, -1.0), morphAkima, paramStandard);
	par->addParam("aux_light_position", 4, CVector3(0.0, -1.0, -3.0), morphAkima, paramStandard);
	par->addParam("aux_light_intensity", 1, 1.3, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("aux_light_intensity", 2, 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("aux_light_intensity", 3, 3.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("aux_light_intensity", 4, 2.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("aux_light_enabled", 1, false, morphLinear, paramStandard);
	par->addParam("aux_light_enabled", 2, false, morphLinear, paramStandard);
	par->addParam("aux_light_enabled", 3, false, morphLinear, paramStandard);
	par->addParam("aux_light_enabled", 4, false, morphLinear, paramStandard);
	par->addParam("aux_light_colour", 1, sRGB(45761, 53633, 59498), morphLinear, paramStandard);
	par->addParam("aux_light_colour", 2, sRGB(62875, 55818, 50083), morphLinear, paramStandard);
	par->addParam("aux_light_colour", 3, sRGB(64884, 64928, 48848), morphLinear, paramStandard);
	par->addParam("aux_light_colour", 4, sRGB(52704, 62492, 45654), morphLinear, paramStandard);

	for (int i = 1; i <= 4; i++)
	{
		par->addParam("aux_light_volumetric_intensity", i, 1.0, 0.0, 1e15, morphLinear, paramStandard);
		par->addParam("aux_light_volumetric_enabled", i, false, morphLinear, paramStandard);
	}

	// random aux light
	par->addParam("random_lights_number", 20, 0, 99999, morphLinear, paramStandard);
	par->addParam("random_lights_random_seed", 1234, 0, 99999, morphLinear, paramStandard);
	par->addParam(
		"random_lights_max_distance_from_fractal", 0.1, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("random_lights_intensity", 0.1, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("random_lights_distribution_radius", 3.0, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam(
		"random_lights_distribution_center", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("random_lights_group", false, morphLinear, paramStandard);

	// fake lights
	par->addParam("fake_lights_enabled", false, morphLinear, paramStandard);
	par->addParam("fake_lights_intensity", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("fake_lights_visibility", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("fake_lights_visibility_size", 5.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("fake_lights_orbit_trap", CVector3(2.0, 0.0, 0.0), morphLinear, paramStandard);
	par->addParam("fake_lights_min_iter", 1, 0, 250, morphLinear, paramStandard);
	par->addParam("fake_lights_max_iter", 2, 0, 250, morphLinear, paramStandard);

	par->addParam("all_primitives_position", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("all_primitives_rotation", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("fractal_position", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("fractal_rotation", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);

// OpenCL Support
#ifdef CLSUPPORT
	par->addParam("ocl_custom_DE_mode", false, false);
	par->addParam("ocl_custom_formula_name", QString("example"), false);
	for (int i = 0; i < 15; ++i)
	{
		par->addParam("ocl_custom_par", i, 0.0, true);
	}
	par->addParam("ocl_delta_DE_step", 1e-5, 1e-10, 1e10, true);
	par->addParam("ocl_DOF_method", 0, false);
	par->addParam("ocl_use_custom_formula", 0, false);
#endif

	// files
	par->addParam("file_destination",
		QDir::toNativeSeparators(systemData.GetImagesFolder() + QDir::separator() + "image"),
		morphNone, paramStandard);
	par->addParam("file_background", QDir::toNativeSeparators(systemData.sharedDir + "textures"
																														+ QDir::separator() + "background.jpg"),
		morphNone, paramStandard);
	par->addParam("file_envmap",
		QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator() + "envmap.jpg"),
		morphNone, paramStandard);
	par->addParam("file_lightmap", QDir::toNativeSeparators(systemData.sharedDir + "textures"
																													+ QDir::separator() + "lightmap.jpg"),
		morphNone, paramStandard);
	/* unused in Mandebulber v2
	par->addParam("file_animation_path",
		QDir::toNativeSeparators(systemData.dataDirectory + "paths" + QDir::separator() + "path.txt"),
		morphNone, paramStandard);
	par->addParam("file_keyframes", QDir::toNativeSeparators(systemData.dataDirectory + "keyframes"
																													 + QDir::separator() + "keyframe"),
		morphNone, paramStandard);
	*/
	par->addParam("description", QString(""), morphNone, paramStandard);

	//----------------------- application parameters ---------------------
	par->addParam("net_render_client_port", QString("5555"), morphNone, paramApp);
	par->addParam("net_render_client_IP", QString("10.0.0.4"), morphNone, paramApp);
	par->addParam("net_render_server_port", QString("5555"), morphNone, paramApp);
	par->addParam("aux_light_manual_placement_dist", 0.1, 1e-15, 1e15, morphNone, paramApp);

	par->addParam("camera_movement_step", 0.5, 1e-15, 1e5, morphNone, paramApp);
	par->addParam("camera_rotation_step", 15.0, 1e-15, 360.0, morphNone, paramApp);
	par->addParam("camera_straight_rotation", 0, morphNone, paramApp);
	par->addParam("camera_absolute_distance_mode", 0, morphNone, paramApp);
	par->addParam("camera_movement_mode", 0, morphNone, paramApp);
	par->addParam("camera_rotation_mode", 0, morphNone, paramApp);
	par->addParam("mouse_click_function", 1, morphNone, paramNoSave);
	par->addParam("show_cursor", true, morphNone, paramApp);

	par->addParam("auto_save_images", false, morphNone, paramApp);
	par->addParam("save_image_format", 0, morphNone, paramApp);
	par->addParam("image_preview_scale", 3, 0, 6, morphNone, paramApp);
	par->addParam("netrender_mode", (int)CNetRender::netRenderServer, morphNone, paramApp);
	par->addParam("netrender_client_remote_address", QString("localhost"), morphNone, paramApp);
	par->addParam("netrender_client_remote_port", 5555, morphNone, paramApp);
	par->addParam("netrender_server_local_port", 5555, morphNone, paramApp);

	par->addParam("default_image_path", systemData.GetImagesFolder(), morphNone, paramApp);
	par->addParam("default_textures_path", systemData.sharedDir + "textures", morphNone, paramApp);
	par->addParam(
		"default_settings_path", systemData.GetSettingsFolder(), morphNone, paramApp);

	par->addParam("show_queue_thumbnails", false, morphNone, paramApp);
	par->addParam("queue_image_format", 0, morphNone, paramApp);

	par->addParam("quit_do_not_ask_again", false, morphNone, paramApp);
	par->addParam("description_popup_do_not_show_again", false, morphNone, paramApp);
	par->addParam("system_tray_notify", false, morphNone, paramApp);

	par->addParam("auto_refresh", false, morphNone, paramApp);
	par->addParam("connect_detail_level", false, morphNone, paramNoSave);
	par->addParam("connect_detail_level_2", false, morphNone, paramNoSave);

	par->addParam("julia_preview_distance", 3.0, 1e-10, 1000.0, morphNone, paramApp);

	// measurement
	par->addParam("meas_point", CVector3(0.0, 0.0, 0.0), morphNone, paramNoSave);
	par->addParam("meas_distance_from_last", 0.0, morphNone, paramNoSave);
	par->addParam("meas_distance_from_camera", 0.0, morphNone, paramNoSave);

	//----------------------- preferences ---------------------
	par->addParam("language", QString("unknown"), morphNone, paramApp);
	par->addParam("ui_style_type", -1, morphNone, paramApp);
	par->addParam("ui_skin", -1, morphNone, paramApp);
	par->addParam("ui_font_size", 9, 5, 50, morphNone, paramApp);
	par->addParam("toolbar_icon_size", 40, 20, 100, morphNone, paramApp);
	par->addParam("limit_CPU_cores", get_cpu_count(), 1, get_cpu_count(), morphNone, paramApp);

	// image file configuration
	par->addParam("color_enabled", true, morphNone, paramApp);
	par->addParam("alpha_enabled", false, morphNone, paramApp);
	par->addParam("zbuffer_enabled", false, morphNone, paramApp);
	par->addParam("normal_enabled", false, morphNone, paramApp);

	par->addParam("color_quality", (int)ImageFileSave::IMAGE_CHANNEL_QUALITY_8, morphNone, paramApp);
	par->addParam("alpha_quality", (int)ImageFileSave::IMAGE_CHANNEL_QUALITY_8, morphNone, paramApp);
	par->addParam(
		"zbuffer_quality", (int)ImageFileSave::IMAGE_CHANNEL_QUALITY_32, morphNone, paramApp);
	par->addParam(
		"normal_quality", (int)ImageFileSave::IMAGE_CHANNEL_QUALITY_32, morphNone, paramApp);

	par->addParam("color_postfix", QString(""), morphNone, paramApp);
	par->addParam("alpha_postfix", QString("_alpha"), morphNone, paramApp);
	par->addParam("zbuffer_postfix", QString("_zbuffer"), morphNone, paramApp);
	par->addParam("normal_postfix", QString("_normal"), morphNone, paramApp);

	par->addParam("append_alpha_png", true, morphNone, paramApp);
	par->addParam("jpeg_quality", 95, 1, 100, morphNone, paramApp);

	par->addParam("logging_verbosity", 1, 0, 3, morphNone, paramApp);
	par->addParam("threads_priority", 2, 0, 3, morphNone, paramApp);

#ifdef CLSUPPORT
	par->addParam("openCL_use_CPU", false, true);
	par->SetAsAppParam("openCL_use_CPU", true);
	par->addParam("openCL_platform_index", 0, true);
	par->SetAsAppParam("openCL_platform_index", true);
	par->addParam("openCL_device_index", 0, true);
	par->SetAsAppParam("openCL_device_index", true);
	par->addParam("openCL_engine", 0, true);
	par->SetAsAppParam("openCL_engine", true);
	par->addParam("openCL_cycle_time", 1.0, 0.02, 60.0, true);
	par->SetAsAppParam("openCL_cycle_time", true);
	par->addParam("openCL_memory_limit", 256, true);
	par->SetAsAppParam("openCL_memory_limit", true);
#ifdef WIN32
	par->addParam("openCL_text_editor", QString("notepad.exe"), true);
	par->SetAsAppParam("openCL_text_editor", true);
#else
	par->addParam("openCL_text_editor", QString("/usr/bin/kate"), true);
	par->SetAsAppParam("openCL_text_editor", true);
#endif
#endif

	WriteLog("Parameters initialization finished", 3);
}

// definition of all parameters
void InitFractalParams(cParameterContainer *par)
{
	WriteLog("Fractal parameters initialization started: " + par->GetContainerName(), 3);

	par->addParam("power", 9.0, morphAkima, paramStandard);
	par->addParam("alpha_angle_offset", 0.0, morphAkimaAngle, paramStandard);
	par->addParam("beta_angle_offset", 0.0, morphAkimaAngle, paramStandard);
	par->addParam("gamma_angle_offset", 0.0, morphAkimaAngle, paramStandard);
	par->addParam("cadd", -1.3, morphLinear, paramStandard);

	// IFS formula
	par->addParam("IFS_scale", 2.0, morphAkima, paramStandard);
	par->addParam("IFS_rotation", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
	par->addParam("IFS_rotation_enabled", false, morphLinear, paramStandard);
	par->addParam("IFS_offset", CVector3(1.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("IFS_edge", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("IFS_edge_enabled", false, morphLinear, paramStandard);
	par->addParam("IFS_abs_x", false, morphLinear, paramStandard);
	par->addParam("IFS_abs_y", false, morphLinear, paramStandard);
	par->addParam("IFS_abs_z", false, morphLinear, paramStandard);
	par->addParam("IFS_menger_sponge_mode", false, morphLinear, paramStandard);

	for (int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		par->addParam("IFS_direction", i, CVector3(1.0, 0.0, 0.0), morphAkima, paramStandard);
		par->addParam("IFS_rotations", i, CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
		par->addParam("IFS_distance", i, 0.0, morphAkima, paramStandard);
		par->addParam("IFS_intensity", i, 1.0, morphAkima, paramStandard);
		par->addParam("IFS_enabled", i, false, morphLinear, paramStandard);
	}

	// Mandelbox
	par->addParam("mandelbox_scale", 2.0, morphAkima, paramStandard);
	par->addParam("mandelbox_folding_limit", 1.0, morphAkima, paramStandard);
	par->addParam("mandelbox_folding_value", 2.0, morphAkima, paramStandard);
	par->addParam("mandelbox_folding_min_radius", 0.5, morphAkima, paramStandard);
	par->addParam("mandelbox_folding_fixed_radius", 1.0, morphAkima, paramStandard);
	par->addParam("mandelbox_sharpness", 3.0, morphAkima, paramStandard);
	par->addParam("mandelbox_offset", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("mandelbox_rotation_main", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
	for (int i = 1; i <= 3; i++)
	{
		par->addParam(
			"mandelbox_rotation_neg", i, CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
		par->addParam(
			"mandelbox_rotation_pos", i, CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
	}
	par->addParam("mandelbox_color", CVector3(0.03, 0.05, 0.07), morphLinear, paramStandard);
	par->addParam(
		"mandelbox_color_4D", CVector4(0.03, 0.05, 0.07, 0.0), morphLinear, paramStandard); //  to fix
	par->addParam("mandelbox_color_R", 0.0, morphLinear, paramStandard);
	par->addParam("mandelbox_color_Sp1", 0.2, morphLinear, paramStandard);
	par->addParam("mandelbox_color_Sp2", 0.2, morphLinear, paramStandard);
	par->addParam("mandelbox_main_rotation_enabled", false, morphLinear, paramStandard);
	par->addParam("mandelbox_rotations_enabled", false, morphLinear, paramStandard);

	par->addParam("mandelbox_solid", 1.0, morphLinear, paramStandard);
	par->addParam("mandelbox_melt", 0.0, morphLinear, paramStandard);

	par->addParam("mandelbox_vary_scale_vary", 0.0, morphLinear, paramStandard);
	par->addParam("mandelbox_vary_fold", 1.0, morphLinear, paramStandard);
	par->addParam("mandelbox_vary_minr", 0.5, morphLinear, paramStandard);
	par->addParam("mandelbox_vary_rpower", 1.0, morphLinear, paramStandard);
	par->addParam("mandelbox_vary_wadd", 0.0, morphLinear, paramStandard);

	par->addParam("mandelbox_generalized_fold_type", 0, morphNone, paramStandard);

	// FoldingIntPow
	par->addParam("boxfold_bulbpow2_folding_factor", 2.0, morphLinear, paramStandard);
	par->addParam("boxfold_bulbpow2_z_factor", 5.0, morphLinear, paramStandard);

	par->addParam("buffalo_preabs_x", false, morphLinear, paramStandard);
	par->addParam("buffalo_preabs_y", false, morphLinear, paramStandard);
	par->addParam("buffalo_preabs_z", false, morphLinear, paramStandard);
	par->addParam("buffalo_abs_x", false, morphLinear, paramStandard);
	par->addParam("buffalo_abs_y", false, morphLinear, paramStandard);
	par->addParam("buffalo_abs_z", false, morphLinear, paramStandard);
	par->addParam("buffalo_pos_z", false, morphLinear, paramStandard);

	par->addParam("aboxMod1_foldM", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);

	// mandelbulbMulti
	par->addParam("mandelbulbMulti_acos_or_asin", 0, morphNone, paramStandard);
	par->addParam("mandelbulbMulti_atan_or_atan2", 0, morphNone, paramStandard);

	par->addParam("mandelbulbMulti_acos_or_asin_A", 0, morphNone, paramStandard);
	par->addParam("mandelbulbMulti_atan_or_atan2_A", 0, morphNone, paramStandard);

	par->addParam("mandelbulbMulti_order_of_xyz", 0, morphNone, paramStandard);
	par->addParam("mandelbulbMulti_order_of_xyz_2", 0, morphNone, paramStandard);
	par->addParam("mandelbulbMulti_order_of_xyz_C", 0, morphNone, paramStandard);

	// sinTan2Trig
	par->addParam("sinTan2Trig_asin_or_acos", 0, morphNone, paramStandard);
	par->addParam("sinTan2Trig_atan2_or_atan", 0, morphNone, paramStandard);
	par->addParam("sinTan2Trig_order_of_zyx", 0, morphNone, paramStandard);

	// surfBox
	par->addParam("surfBox_enabledX1", true, morphLinear, paramStandard);
	par->addParam("surfBox_enabledY1", true, morphLinear, paramStandard);
	par->addParam("surfBox_enabledZ1", true, morphLinear, paramStandard);
	par->addParam("surfBox_enabledX2_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledY2_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledZ2_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledX3_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledY3_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledZ3_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledX4_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledY4_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledZ4_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledX5_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledY5_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_enabledZ5_false", false, morphLinear, paramStandard);
	par->addParam("surfBox_offset1A_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("surfBox_offset1B_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("surfBox_offset2A_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("surfBox_offset2B_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("surfBox_offset3A_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("surfBox_offset3B_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("surfBox_offset1A_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);
	par->addParam("surfBox_offset1B_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);
	par->addParam("surfBox_scale1Z1", 1.0, morphAkima, paramStandard);

	// SurfFold
	par->addParam("surfFolds_order_of_folds_1", 0, morphNone, paramStandard);
	par->addParam("surfFolds_order_of_folds_2", 0, morphNone, paramStandard);
	par->addParam("surfFolds_order_of_folds_3", 0, morphNone, paramStandard);
	par->addParam("surfFolds_order_of_folds_4", 0, morphNone, paramStandard);
	par->addParam("surfFolds_order_of_folds_5", 0, morphNone, paramStandard);

	// Benesi Mag Transforms
	par->addParam("magTransf_order_of_transf_1", 0, morphNone, paramStandard);
	par->addParam("magTransf_order_of_transf_2", 0, morphNone, paramStandard);
	par->addParam("magTransf_order_of_transf_3", 0, morphNone, paramStandard);
	par->addParam("magTransf_order_of_transf_4", 0, morphNone, paramStandard);
	par->addParam("magTransf_order_of_transf_5", 0, morphNone, paramStandard);

	// donut
	par->addParam("donut_ring_radius", 1.0, morphAkima, paramStandard);
	par->addParam("donut_ring_thickness", 0.1, morphAkima, paramStandard);
	par->addParam("donut_factor", 3.0, morphAkima, paramStandard);
	par->addParam("donut_number", 9.0, morphAkima, paramStandard);

	// curvilinear
	par->addParam("Cpara_enabledLinear", true, morphLinear, paramStandard);
	par->addParam("Cpara_enabledCurves", true, morphLinear, paramStandard);
	par->addParam("Cpara_enabledParab_false", false, morphLinear, paramStandard);
	par->addParam("Cpara_enabledParaAddP0", true, morphLinear, paramStandard);
	par->addParam("Cpara_para0", 1.0, morphAkima, paramStandard);
	par->addParam("Cpara_paraA", 1.0, morphAkima, paramStandard);
	par->addParam("Cpara_paraB", 1.0, morphAkima, paramStandard);
	par->addParam("Cpara_paraC", 1.0, morphAkima, paramStandard);
	par->addParam("Cpara_parab_offset", 1.0, morphAkima, paramStandard);
	par->addParam("Cpara_parab_slope", 0.0, morphAkima, paramStandard);
	par->addParam("Cpara_parab_scale", 0.0, morphAkima, paramStandard);

	par->addParam("Cpara_iterA", 250, morphLinear, paramStandard);
	par->addParam("Cpara_iterB", 250, morphLinear, paramStandard);
	par->addParam("Cpara_iterC", 250, morphLinear, paramStandard);

	// analyticDE controls
	par->addParam("analyticDE_tweak_005", 0.05, morphAkima, paramStandard);
	par->addParam("analyticDE_offset_1", 1.0, morphAkima, paramStandard);
	par->addParam("analyticDE_offset_2", 1.0, morphAkima, paramStandard);
	par->addParam("analyticDE_scale_1", 1.0, morphAkima, paramStandard);
	par->addParam("analyticDE_enabled_false", false, morphLinear, paramStandard);
	par->addParam("analyticDE_enabled_auxR2_false", false, morphLinear, paramStandard);
	par->addParam("analyticDE_offset_linear", 1.0, morphAkima, paramStandard);
	par->addParam("analyticDE_scale_linear", 1.0, morphAkima, paramStandard);

	// color controls
	par->addParam("fold_color_color_min", 0.0, morphAkima, paramStandard);
	par->addParam("fold_color_comp_fold", 1.0, morphAkima, paramStandard);
	par->addParam("fold_color_comp_fold0", 0.0, morphAkima, paramStandard);
	par->addParam("fold_color_comp_minR", 1.0, morphAkima, paramStandard);
	par->addParam("fold_color_comp_scale", 0.0, morphAkima, paramStandard);

	// common parameters for transforming formulas
	par->addParam("transf_alpha_angle_offset", 0.0, morphAkimaAngle, paramStandard);
	par->addParam("transf_beta_angle_offset", 0.0, morphAkimaAngle, paramStandard);
	par->addParam("transf_folding_value", 2.0, morphAkima, paramStandard);
	par->addParam("transf_folding_limit", 1.0, morphAkima, paramStandard);
	par->addParam("transf_multiplication", 2.0, morphAkima, paramStandard);
	par->addParam("transf_minimum_radius_neg1", -1.0, morphAkima, paramStandard);
	par->addParam("transf_minimum_radius_0", 0.0, morphAkima, paramStandard);
	par->addParam("transf_minimum_radius_05", 0.5, morphAkima, paramStandard);
	par->addParam("transf_minimum_radius_06", 0.6, morphAkima, paramStandard);
	par->addParam("transf_offset", 0.0, morphAkima, paramStandard);
	par->addParam("transf_offset_0", 0.0, morphAkima, paramStandard);
	par->addParam("transf_offset_05", 0.5, morphAkima, paramStandard);
	par->addParam("transf_offset_1", 1.0, morphAkima, paramStandard);
	par->addParam("transf_offset_105", 1.5, morphAkima, paramStandard);
	par->addParam("transf_offset_2", 2.0, morphAkima, paramStandard);
	par->addParam("transf_pwr_05", 0.5, morphAkima, paramStandard);
	par->addParam("transf_pwr_4", 4.0, morphAkima, paramStandard);
	par->addParam("transf_pwr_8", 8.0, morphAkima, paramStandard);
	par->addParam("transf_pwr_8a", 8.0, morphAkima, paramStandard);
	par->addParam("transf_scale", 1.0, morphAkima, paramStandard);
	par->addParam("transf_scale_0", 0.0, morphAkima, paramStandard);
	par->addParam("transf_scale_025", 0.25, morphAkima, paramStandard);
	par->addParam("transf_scale_1", 1.0, morphAkima, paramStandard);
	par->addParam("transf_scaleA_1", 1.0, morphAkima, paramStandard);
	par->addParam("transf_scaleB_1", 1.0, morphAkima, paramStandard);
	par->addParam("transf_scaleC_1", 1.0, morphAkima, paramStandard);
	par->addParam("transf_scaleD_1", 1.0, morphAkima, paramStandard);
	par->addParam("transf_scaleE_1", 1.0, morphAkima, paramStandard);
	par->addParam("transf_scaleA_2", 2.0, morphAkima, paramStandard);
	par->addParam("transf_scale_015", 1.5, morphAkima, paramStandard);
	par->addParam("transf_scale_2", 2.0, morphAkima, paramStandard);
	par->addParam("transf_scale_3", 3.0, morphAkima, paramStandard);
	par->addParam("transf_scaleA_3", 3.0, morphAkima, paramStandard);
	par->addParam("transf_scale_4", 4.0, morphAkima, paramStandard);
	par->addParam("transf_scale_8", 8.0, morphAkima, paramStandard);

	par->addParam("transf_int_A", 0, morphLinear, paramStandard);
	par->addParam("transf_int_B", 0, morphLinear, paramStandard);
	par->addParam("transf_int_1", 1, morphLinear, paramStandard);
	par->addParam("transf_int8_X", 8, morphLinear, paramStandard);
	par->addParam("transf_int8_Y", 8, morphLinear, paramStandard);
	par->addParam("transf_int8_Z", 8, morphLinear, paramStandard);

	par->addParam("transf_start_iterations", 0, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_250", 250, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations", 250, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_A", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_A", 250, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_B", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_B", 250, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_C", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_C", 250, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_D", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_D", 250, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_E", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_E", 250, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_F", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_F", 250, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_P", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_P1", 1, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_R", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_R", 250, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_S", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_S", 250, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_T", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_T", 250, morphLinear, paramStandard);
	par->addParam("transf_start_iterations_M", 0, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_M", 250, morphLinear, paramStandard);
	par->addParam("transf_stop_iterations_1", 1, morphLinear, paramStandard);
	par->addParam("transf_stop_iterationsT_1", 1, morphLinear, paramStandard);
	par->addParam("transf_stop_iterationsTM_1", 1, morphLinear, paramStandard);

	par->addParam(
		"transf_addition_constant_0555", CVector3(0.5, 0.5, 0.5), morphAkima, paramStandard);
	par->addParam(
		"transf_addition_constant_0777", CVector3(0.7, 0.7, 0.7), morphAkima, paramStandard);
	par->addParam("transf_addition_constant", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam(
		"transf_addition_constantA_000", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam(
		"transf_addition_constantP_000", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("transf_addition_constant_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam(
		"transf_addition_constantA_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("transf_addition_constant_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);
	par->addParam(
		"transf_addition_constant_neg100", CVector3(-1.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("transf_constant_julia_c", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
	par->addParam("transf_constant_factor", CVector3(1.0, 2.0, 2.0), morphAkima, paramStandard);
	par->addParam(
		"transf_constant_multiplier_000", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplier_001", CVector3(0.0, 0.0, 1.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplier_010", CVector3(0.0, 1.0, 0.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplier_100", CVector3(1.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplierA_100", CVector3(1.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplier_111", CVector3(1.0, 1.0, 1.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplierA_111", CVector3(1.0, 1.0, 1.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplierB_111", CVector3(1.0, 1.0, 1.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplierC_111", CVector3(1.0, 1.0, 1.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplier_121", CVector3(1.0, 2.0, 1.0), morphAkima, paramStandard);
	par->addParam(
		"transf_constant_multiplier_122", CVector3(1.0, 2.0, 2.0), morphAkima, paramStandard);
	par->addParam(
		"transf_constant_multiplier_221", CVector3(2.0, 2.0, 1.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplier_222", CVector3(2.0, 2.0, 2.0), morphAkimaAngle, paramStandard);
	par->addParam(
		"transf_constant_multiplier_441", CVector3(4.0, 4.0, 1.0), morphAkimaAngle, paramStandard);

	par->addParam("transf_offset_000", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("transf_offsetA_000", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("transf_offsetF_000", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("transf_offset_100", CVector3(1.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("transf_offset_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("transf_offsetA_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("transf_offsetB_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("transf_offsetC_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("transf_offset_200", CVector3(2.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("transf_offsetA_200", CVector3(2.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("transf_offset_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);

	par->addParam("transf_power_025", CVector3(0.25, 0.25, 0.25), morphAkimaAngle, paramStandard);
	par->addParam("transf_power_8", CVector3(8.0, 8.0, 8.0), morphAkimaAngle, paramStandard);
	par->addParam("transf_rotation", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
	par->addParam("transf_rotation2", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
	par->addParam("transf_scale3D_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
	par->addParam("transf_scale3D_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);
	par->addParam("transf_scale3Da_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);
	par->addParam("transf_scale3Db_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);
	par->addParam("transf_scale3Dc_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);
	par->addParam("transf_scale3Dd_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);
	par->addParam("transf_scale3D_333", CVector3(3.0, 3.0, 3.0), morphAkima, paramStandard);
	par->addParam("transf_scale3D_444", CVector3(4.0, 4.0, 4.0), morphAkima, paramStandard);

	par->addParam(
		"transf_addition_constant_0000", CVector4(0.0, 0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam(
		"transf_addition_constant_111d5", CVector4(1.0, 1.0, 1.0, 0.5), morphAkima, paramStandard);
	par->addParam(
		"transf_constant_multiplier_1220", CVector4(1.0, 2.0, 2.0, 0.0), morphAkima, paramStandard);

	par->addParam("transf_addCpixel_enabled", true, morphLinear, paramStandard);
	par->addParam("transf_addCpixel_enabled_false", false, morphLinear, paramStandard);
	par->addParam("transf_alternate_enabled_false", false, morphLinear, paramStandard);
	par->addParam("transf_benesi_T1_enabled", true, morphLinear, paramStandard);
	par->addParam("transf_benesi_T1_enabled_false", false, morphLinear, paramStandard);
	par->addParam("transf_benesi_T1M_enabled_false", false, morphLinear, paramStandard);
	par->addParam("transf_constant_julia_mode", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabled", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabled_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledx", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabledy", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabledz", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabledx_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledy_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledz_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledAx", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabledAy", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabledAz", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabledAx_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledAy_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledAz_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledBx", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabledBy", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabledBz", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabledBx_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledBy_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledBz_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledCx_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledCy_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledCz_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledF_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledK_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledM", true, morphLinear, paramStandard);
	par->addParam("transf_function_enabledM_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledR_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledP_false", false, morphLinear, paramStandard);
	par->addParam("transf_function_enabledX_false", false, morphLinear, paramStandard);
	par->addParam("transf_rotation_enabled", false, morphLinear, paramStandard);

	// platonic_solid

	par->addParam("platonic_solid_frequency", 1.0, morphAkima, paramStandard);
	par->addParam("platonic_solid_amplitude", 1.0, morphAkima, paramStandard);
	par->addParam("platonic_solid_rhoMul", 1.0, morphAkima, paramStandard);

	par->addParam("info", false, morphNone, paramStandard); // dummy parameter for information
																													// groupbox

	WriteLog("Fractal parameters initialization finished", 3);
}

void InitPrimitiveParams(
	fractal::enumObjectType objectType, const QString primitiveName, cParameterContainer *par)
{
	par->addParam(
		QString(primitiveName) + "_position", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam(
		QString(primitiveName) + "_rotation", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);

	// left to keep compatibility with older versions
	par->addParam(
		QString(primitiveName) + "_color", sRGB(32000, 32000, 32000), morphAkima, paramStandard);
	par->addParam(QString(primitiveName) + "_reflection", 0.0, 0.0, 1.0, morphAkima, paramStandard);

	par->addParam(QString(primitiveName) + "_enabled", false, morphAkima, paramStandard);
	par->addParam(QString(primitiveName) + "_material_id", 1, morphNone, paramStandard);

	switch (objectType)
	{
		case fractal::objBox:
			par->addParam(
				QString(primitiveName) + "_size", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_rounding", 1e-15, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_empty", false, morphAkima, paramStandard);
			par->addParam(
				QString(primitiveName) + "_repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);
			break;
		case fractal::objCircle:
			par->addParam(QString(primitiveName) + "_radius", 1.0, morphAkima, paramStandard);
			break;
		case fractal::objCylinder:
			par->addParam(QString(primitiveName) + "_radius", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_height", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_caps", true, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_empty", false, morphAkima, paramStandard);
			par->addParam(
				QString(primitiveName) + "_repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);
			break;
		case fractal::objCone:
			par->addParam(QString(primitiveName) + "_radius", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_height", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_caps", true, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_empty", false, morphAkima, paramStandard);
			par->addParam(
				QString(primitiveName) + "_repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);
			break;
		case fractal::objPlane:
			par->addParam(QString(primitiveName) + "_empty", false, morphAkima, paramStandard);
			break;
		case fractal::objRectangle:
			par->addParam(QString(primitiveName) + "_width", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_height", 1.0, morphAkima, paramStandard);
			break;
		case fractal::objSphere:
			par->addParam(QString(primitiveName) + "_radius", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_empty", false, morphAkima, paramStandard);
			par->addParam(
				QString(primitiveName) + "_repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);
			break;
		case fractal::objWater:
			par->addParam(QString(primitiveName) + "_amplitude", 0.02, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_length", 0.1, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_anim_speed", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_iterations", 5, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_empty", false, morphAkima, paramStandard);
			break;
		case fractal::objTorus:
			par->addParam(QString(primitiveName) + "_radius", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_tube_radius", 0.2, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_empty", false, morphAkima, paramStandard);
			par->addParam(
				QString(primitiveName) + "_repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);
			break;

		default: break;
	}
}

void InitMaterialParams(int materialId, cParameterContainer *par)
{
	par->addParam(cMaterial::Name("is_defined", materialId), false, morphNone, paramStandard);
	par->addParam(cMaterial::Name("name", materialId), QString("material %1").arg(materialId),
		morphNone, paramStandard);
	par->addParam(cMaterial::Name("shading", materialId), 1.0, 0.0, 1e15, morphAkima, paramStandard);
	par->addParam(cMaterial::Name("specular", materialId), 1.0, 0.0, 1e15, morphAkima, paramStandard);
	par->addParam(
		cMaterial::Name("specular_width", materialId), 1.0, 1e-10, 1e15, morphAkima, paramStandard);
	par->addParam(cMaterial::Name("specular_color", materialId), sRGB(65535, 65535, 65535),
		morphAkima, paramStandard);
	par->addParam(
		cMaterial::Name("reflectance", materialId), 0.0, 0.0, 1e15, morphAkima, paramStandard);
	par->addParam(
		cMaterial::Name("luminosity", materialId), 0.0, 0.0, 1e15, morphAkima, paramStandard);
	par->addParam(cMaterial::Name("transparency_of_surface", materialId), 0.0, 0.0, 1.0, morphAkima,
		paramStandard);
	par->addParam(cMaterial::Name("transparency_of_interior", materialId), 1.0, 0.0, 1.0, morphAkima,
		paramStandard);
	par->addParam(cMaterial::Name("transparency_index_of_refraction", materialId), 1.5, 0.0, 100.0,
		morphAkima, paramStandard);
	par->addParam(cMaterial::Name("surface_color", materialId), sRGB(50000, 50000, 50000), morphAkima,
		paramStandard);
	par->addParam(cMaterial::Name("transparency_interior_color", materialId),
		sRGB(65535, 65535, 65535), morphLinear, paramStandard);
	par->addParam(cMaterial::Name("luminosity_color", materialId), sRGB(65535, 65535, 65535),
		morphLinear, paramStandard);
	par->addParam(
		cMaterial::Name("fresnel_reflectance", materialId), false, morphLinear, paramStandard);
	par->addParam(cMaterial::Name("texture_center", materialId), CVector3(0.0, 0.0, 0.0), morphAkima,
		paramStandard);
	par->addParam(cMaterial::Name("texture_rotation", materialId), CVector3(0.0, 0.0, 0.0),
		morphAkimaAngle, paramStandard);
	par->addParam(cMaterial::Name("texture_scale", materialId), CVector3(1.0, 1.0, 1.0), morphAkima,
		paramStandard);
	par->addParam(
		cMaterial::Name("coloring_random_seed", materialId), 269259, morphLinear, paramStandard);
	par->addParam(cMaterial::Name("coloring_saturation", materialId), 1.0, 0.0, 1000.0, morphLinear,
		paramStandard);
	par->addParam(
		cMaterial::Name("coloring_speed", materialId), 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam(
		cMaterial::Name("coloring_palette_size", materialId), 10, 1, 255, morphLinear, paramStandard);
	par->addParam(cMaterial::Name("coloring_palette_offset", materialId), 0.0, 0.0, 256.0,
		morphLinear, paramStandard);
	par->addParam(cMaterial::Name("texture_mapping_type", materialId), (int)texture::mappingPlanar,
		morphNone, paramStandard);
	par->addParam(
		cMaterial::Name("use_colors_from_palette", materialId), true, morphLinear, paramStandard);
	par->addParam(
		cMaterial::Name("use_color_texture", materialId), false, morphLinear, paramStandard);
	par->addParam(
		cMaterial::Name("use_diffusion_texture", materialId), false, morphLinear, paramStandard);
	par->addParam(
		cMaterial::Name("use_luminosity_texture", materialId), false, morphLinear, paramStandard);
	par->addParam(
		cMaterial::Name("use_displacement_texture", materialId), false, morphLinear, paramStandard);
	par->addParam(
		cMaterial::Name("use_normal_map_texture", materialId), false, morphLinear, paramStandard);
	par->addParam(cMaterial::Name("normal_map_texture_from_bumpmap", materialId), false, morphLinear,
		paramStandard);
	par->addParam(cMaterial::Name("color_texture_intensity", materialId), 1.0, 0.0, 1e10, morphAkima,
		paramStandard);
	par->addParam(cMaterial::Name("luminosity_texture_intensity", materialId), 1.0, 0.0, 10.0,
		morphAkima, paramStandard);
	par->addParam(cMaterial::Name("diffusion_texture_intensity", materialId), 1.0, 0.0, 1.0,
		morphAkima, paramStandard);
	par->addParam(cMaterial::Name("displacement_texture_height", materialId), 0.1, 1e-12, 1e4,
		morphAkima, paramStandard);
	par->addParam(cMaterial::Name("normal_map_texture_height", materialId), 1.0, 1e-4, 1e4,
		morphAkima, paramStandard);
	par->addParam(cMaterial::Name("normal_map_texture_invert_green", materialId), false, morphNone,
		paramStandard);
	par->addParam(cMaterial::Name("fractal_coloring_algorithm", materialId),
		(int)sFractalColoring::fractalColoringStandard, 0, 4, morphNone, paramStandard);
	par->addParam(cMaterial::Name("fractal_coloring_sphere_radius", materialId), 1.0, 0.0, 1e20,
		morphNone, paramStandard);
	par->addParam(cMaterial::Name("fractal_coloring_line_direction", materialId),
		CVector3(1.0, 0.0, 0.0), morphNone, paramStandard);

	par->addParam(cMaterial::Name("file_color_texture", materialId),
		QDir::toNativeSeparators(
									systemData.sharedDir + "textures" + QDir::separator() + "color_texture.jpg"),
		morphNone, paramStandard);
	par->addParam(cMaterial::Name("file_diffusion_texture", materialId),
		QDir::toNativeSeparators(
									systemData.sharedDir + "textures" + QDir::separator() + "diffusion_texture.jpg"),
		morphNone, paramStandard);
	par->addParam(cMaterial::Name("file_luminosity_texture", materialId),
		QDir::toNativeSeparators(
									systemData.sharedDir + "textures" + QDir::separator() + "luminosity_texture.jpg"),
		morphNone, paramStandard);
	par->addParam(cMaterial::Name("file_displacement_texture", materialId),
		QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator()
														 + "displacement_texture.jpg"),
		morphNone, paramStandard);

	par->addParam(cMaterial::Name("file_normal_map_texture", materialId),
		QDir::toNativeSeparators(
									systemData.sharedDir + "textures" + QDir::separator() + "normal_map_texture.jpg"),
		morphNone, paramStandard);

	cColorPalette palette(par->Get<int>(cMaterial::Name("coloring_palette_size", materialId)),
		par->Get<int>(cMaterial::Name("coloring_random_seed", materialId)), 1.0);
	par->addParam(
		cMaterial::Name("surface_color_palette", materialId), palette, morphLinear, paramStandard);
}

void DeletePrimitiveParams(
	fractal::enumObjectType objectType, const QString primitiveName, cParameterContainer *par)
{
	par->DeleteParameter(QString(primitiveName) + "_position");
	par->DeleteParameter(QString(primitiveName) + "_rotation");
	par->DeleteParameter(QString(primitiveName) + "_color");
	par->DeleteParameter(QString(primitiveName) + "_reflection");
	par->DeleteParameter(QString(primitiveName) + "_enabled");
	par->DeleteParameter(QString(primitiveName) + "_material_id");

	switch (objectType)
	{
		case fractal::objBox:
			par->DeleteParameter(QString(primitiveName) + "_size");
			par->DeleteParameter(QString(primitiveName) + "_rounding");
			par->DeleteParameter(QString(primitiveName) + "_empty");
			par->DeleteParameter(QString(primitiveName) + "_repeat");
			break;
		case fractal::objCircle: par->DeleteParameter(QString(primitiveName) + "_radius"); break;
		case fractal::objCylinder:
			par->DeleteParameter(QString(primitiveName) + "_radius");
			par->DeleteParameter(QString(primitiveName) + "_height");
			par->DeleteParameter(QString(primitiveName) + "_caps");
			par->DeleteParameter(QString(primitiveName) + "_empty");
			par->DeleteParameter(QString(primitiveName) + "_repeat");
			break;
		case fractal::objCone:
			par->DeleteParameter(QString(primitiveName) + "_radius");
			par->DeleteParameter(QString(primitiveName) + "_height");
			par->DeleteParameter(QString(primitiveName) + "_caps");
			par->DeleteParameter(QString(primitiveName) + "_empty");
			par->DeleteParameter(QString(primitiveName) + "_repeat");
			break;
		case fractal::objPlane: par->DeleteParameter(QString(primitiveName) + "_empty"); break;
		case fractal::objRectangle:
			par->DeleteParameter(QString(primitiveName) + "_width");
			par->DeleteParameter(QString(primitiveName) + "_height");
			break;
		case fractal::objSphere:
			par->DeleteParameter(QString(primitiveName) + "_radius");
			par->DeleteParameter(QString(primitiveName) + "_empty");
			par->DeleteParameter(QString(primitiveName) + "_repeat");
			break;
		case fractal::objTorus:
			par->DeleteParameter(QString(primitiveName) + "_radius");
			par->DeleteParameter(QString(primitiveName) + "_tube_radius");
			par->DeleteParameter(QString(primitiveName) + "_empty");
			par->DeleteParameter(QString(primitiveName) + "_repeat");
			break;
		case fractal::objWater:
			par->DeleteParameter(QString(primitiveName) + "_amplitude");
			par->DeleteParameter(QString(primitiveName) + "_length");
			par->DeleteParameter(QString(primitiveName) + "_anim_speed");
			par->DeleteParameter(QString(primitiveName) + "_iterations");
			par->DeleteParameter(QString(primitiveName) + "_empty");
			break;

		default: break;
	}
}

void DeleteAllPrimitiveParams(cParameterContainer *par)
{
	QList<QString> list = par->GetListOfParameters();
	for (int i = 0; i < list.size(); i++)
	{
		QString parameterName = list.at(i);
		if (parameterName.left(parameterName.indexOf('_')) == "primitive")
		{
			par->DeleteParameter(parameterName);
		}
	}
}

void DeleteAllMaterialParams(cParameterContainer *par)
{
	QList<QString> list = par->GetListOfParameters();
	for (int i = 0; i < list.size(); i++)
	{
		QString parameterName = list.at(i);
		if (parameterName.left(3) == "mat")
		{
			par->DeleteParameter(parameterName);
		}
	}
}
