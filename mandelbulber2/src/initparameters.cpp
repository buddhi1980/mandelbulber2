/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * InitParams function - initialization of all parameters
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

#include "fractparams.hpp"
#include "fractal.h"
#include "fractal_list.hpp"
#include "parameters.hpp"
#include "system.hpp"
#include "color_palette.hpp"
#include "netrender.hpp"
#include "files.h"
#include <QtCore>
#include "material.h"

//definition of all parameters
void InitParams(cParameterContainer *par)
{
	using namespace parameterContainer;
	WriteLog("Parameters initialization started: " + par->GetContainerName());

	//image
	par->addParam("image_width", 800, 32, 65535, morphNone, paramStandard);
	par->addParam("image_height", 600, 32, 65535, morphNone, paramStandard);
	par->addParam("tiles", 1, 1, 64, morphNone, paramStandard);
	par->addParam("tile_number", 0, morphNone, paramStandard);
	par->addParam("image_proportion", 0, morphNone, paramNoSave);

	//flight animation
	par->addParam("flight_first_to_render", 0, 0, 99999, morphNone, paramStandard);
	par->addParam("flight_last_to_render", 1000, 0, 99999, morphNone, paramStandard);

	par->addParam("frame_no", 0, 0, 99999, morphNone, paramOnlyForNet);
	par->addParam("flight_speed", 0.01, 0.0, 100.0, morphLinear, paramStandard);
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
	par->addParam("anim_flight_dir", systemData.dataDirectory + "images" + QDir::separator(), morphNone, paramStandard);

	//keyframe animation
	par->addParam("frames_per_keyframe", 100, 1, 99999, morphNone, paramStandard);
	par->addParam("keyframe_first_to_render", 0, 0, 99999, morphNone, paramStandard);
	par->addParam("keyframe_last_to_render", 1000, 0, 99999, morphNone, paramStandard);
	par->addParam("show_keyframe_thumbnails", false, morphNone, paramApp);
	par->addParam("keyframe_animation_image_type", 0, morphNone, paramApp);
	par->addParam("anim_keyframe_dir", systemData.dataDirectory + "images" + QDir::separator(), morphNone, paramStandard);
	par->addParam("keyframe_collision_thresh", 1.0e-6, 1e-15, 1.0e2, morphNone, paramStandard);
	par->addParam("keyframe_auto_validate", true, morphNone, paramApp);
	par->addParam("keyframe_constant_target_distance", 0.1, 1e-10, 1.0e2, morphNone, paramStandard);

	//camera
	par->addParam("camera", CVector3(3.0, -6.0, 2.0), morphAkima, paramStandard);
	par->addParam("target", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("camera_top", CVector3(-0.1277753, 0.2555506, 0.958314), morphAkima, paramStandard); //internal vector which represents top direction
	par->addParam("camera_rotation", CVector3(26.5650, -16.60154, 0.0), morphAkimaAngle, paramStandard);
	par->addParam("camera_distance_to_target", 7.0, 0.0, 1e15, morphAkima, paramStandard);
	par->addParam("fov", 1.0, 0.0, 100.0, morphAkima, paramStandard);
	par->addParam("perspective_type", 0, morphLinear, paramStandard);
	par->addParam("stereo_eye_distance", 1.0, 0.0, 1e15, morphAkima, paramStandard);
	par->addParam("stereo_enabled", false, morphLinear, paramStandard);
	par->addParam("legacy_coordinate_system", false, morphNone, paramStandard);
	par->addParam("sweet_spot_horizontal_angle", 0.0, -180.0, 180.0, morphAkima, paramStandard);
	par->addParam("sweet_spot_vertical_angle", 0.0, -90.0, 90.0, morphAkima, paramStandard);

	//fractal formula selections
	par->addParam("formula", 1, (int) fractal::mandelbulb, morphNone, paramStandard);
	par->addParam("formula_iterations", 1, 1, 1, 65536, morphNone, paramStandard);

	for (int i = 2; i <= NUMBER_OF_FRACTALS; i++)
	{
		par->addParam("formula", i, (int) fractal::none, morphNone, paramStandard);
		par->addParam("formula_iterations", i, 1, 1, 65536, morphNone, paramStandard);
	}

	for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
	{
		par->addParam("formula_weight", i, 1.0, 0.0, 1.0, morphAkima, paramStandard);
		par->addParam("formula_start_iteration", i, 0, 0, 65536, morphAkima, paramStandard);
		par->addParam("formula_stop_iteration", i, 250, 0, 65536, morphAkima, paramStandard);
		par->addParam("julia_mode", i, false, morphLinear, paramStandard);
		par->addParam("julia_c", i, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
		par->addParam("fractal_constant_factor", i, CVector3(1.0, 1.0, 1.0), morphLinear, paramStandard);
	}

	//boolean operators
	par->addParam("boolean_operators", false, morphLinear, paramStandard);
	for(int i = 1; i < NUMBER_OF_FRACTALS; i++)
	{
		par->addParam("boolean_operator", i, (int) params::booleanOperatorOR, morphLinear, paramStandard);
	}

	//fractal transform
	for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
	{
		par->addParam("formula_position", i, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
		par->addParam("formula_rotation", i, CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
		par->addParam("formula_repeat", i, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
		par->addParam("formula_scale", i, 1.0, morphAkima, paramStandard);
		par->addParam("dont_add_c_constant", i, false, morphLinear, paramStandard);
		par->addParam("check_for_bailout", i, true, morphLinear, paramStandard);
	}

	//general fractal and engine
	par->addParam("julia_mode", false, morphLinear, paramStandard);
	par->addParam("julia_c", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("N", 250, 1, 65536, morphLinear, paramStandard);
	par->addParam("minN", 1, 0, 65536, morphLinear, paramStandard);
	par->addParam("fractal_constant_factor", CVector3(1.0, 1.0, 1.0), morphLinear, paramStandard);
	par->addParam("detail_level", 1.0, 1e-8, 1e8, morphLinear, paramStandard);
	par->addParam("DE_thresh", 0.01, 1e-15, 1e5, morphLinear, paramStandard); //old name was 'quality'
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
	par->addParam("interior_mode", false, morphLinear, paramStandard);
	par->addParam("constant_DE_threshold", false, morphLinear, paramStandard);
	par->addParam("hybrid_fractal_enable", false, morphNone, paramStandard);
	par->addParam("bailout", 1e2, 1.0, 1e15, morphLinear, paramStandard);
	par->addParam("repeat_from", 1, 1, 9, morphLinear, paramStandard);
	par->addParam("delta_DE_function", (int)fractal::preferedDEfunction, 0, 2, morphNone, paramStandard);
	par->addParam("delta_DE_method", (int)fractal::preferedDEMethod, 0, 1, morphNone, paramStandard);
	par->addParam("use_default_bailout", true, morphNone, paramStandard);

	//fractal coloring
	par->addParam("fractal_coloring_algorithm", (int)fractal::fractalColoringStandard, 0, 4, morphNone, paramStandard);
	par->addParam("fractal_coloring_sphere_radius", 1.0, 0.0, 1e20, morphNone, paramStandard);
	par->addParam("fractal_coloring_line_direction", CVector3(1.0, 0.0, 0.0), morphNone, paramStandard);

	//foldings
	par->addParam("box_folding", false, morphLinear, paramStandard);
	par->addParam("box_folding_limit", 1.0, morphLinear, paramStandard);
	par->addParam("box_folding_value", 2.0, morphLinear, paramStandard);
	par->addParam("spherical_folding", false, morphLinear, paramStandard);
	par->addParam("spherical_folding_outer", 1.0, morphLinear, paramStandard);
	par->addParam("spherical_folding_inner", 0.5, morphLinear, paramStandard);
	par->addParam("c_add", -1.3, morphLinear, paramStandard);

	//image effects
	par->addParam("brightness", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("contrast", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("gamma", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("hdr", false, morphLinear, paramStandard);
	par->addParam("reflect", 0.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("ambient_occlusion", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_quality", 4, 1, 10, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_fast_tune", 1.0, 1e-5, 1e5, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_enabled", false, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_mode", (int) params::AOmodeScreenSpace, morphLinear, paramStandard);
	par->addParam("SSAO_random_mode", false, morphLinear, paramStandard);
	par->addParam("shading", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("specular", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("glow_enabled", true, morphLinear, paramStandard);
	par->addParam("glow_intensity", 0.2, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("textured_background", false, morphLinear, paramStandard);
	par->addParam("textured_background_map_type", (int) params::mapEquirectangular, morphNone, paramStandard);
	par->addParam("background_brightness", 1.0, 0.0, 1000.0, morphLinear, paramStandard);
	par->addParam("shadows_enabled", true, morphLinear, paramStandard);
	par->addParam("penetrating_lights", true, morphLinear, paramStandard);
	par->addParam("raytraced_reflections", false, morphLinear, paramStandard);
	par->addParam("reflections_max", 5, 0, 10, morphLinear, paramStandard);
	par->addParam("env_mapping_enable", false, morphLinear, paramStandard);
	par->addParam("transparency_of_surface", 0.0, 0.0, 1.0, morphLinear, paramStandard);
	par->addParam("transparency_of_interior", 1.0, 0.0, 1.0, morphLinear, paramStandard);
	par->addParam("transparency_interior_color", sRGB(65535, 65535, 65535), morphLinear, paramStandard);
	par->addParam("transparency_index_of_refraction", 1.5, 0.0, 10.0, morphLinear, paramStandard);
	par->addParam("fresnel_reflectance", false, morphLinear, paramStandard);

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

	//coloring
	par->addParam("fractal_color", true, morphLinear, paramStandard);
	par->addParam("coloring_random_seed", 269259, morphLinear, paramStandard);
	par->addParam("coloring_saturation", 1.0, 0.0, 1000.0, morphLinear, paramStandard);
	par->addParam("coloring_speed", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("coloring_palette_size", 10, 1, 255, morphLinear, paramStandard);
	par->addParam("coloring_palette_offset", 0.0, 0.0, 256.0, morphLinear, paramStandard);

	//fog
	par->addParam("basic_fog_enabled", false, morphLinear, paramStandard);
	par->addParam("basic_fog_visibility", 20.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("basic_fog_color", sRGB(59399, 61202, 65535), morphLinear, paramStandard);
	par->addParam("DOF_enabled", false, morphLinear, paramStandard);
	par->addParam("DOF_focus", 6.0, 0.0, 200.0, morphLinear, paramStandard);
	par->addParam("DOF_radius", 10.0, 0.0, 200.0, morphLinear, paramStandard);
	par->addParam("DOF_HDR", false, morphLinear, paramStandard);

	//main light
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

	//aux lights
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

	//random aux light
	par->addParam("random_lights_number", 20, 0, 99999, morphLinear, paramStandard);
	par->addParam("random_lights_random_seed", 1234, 0, 99999, morphLinear, paramStandard);
	par->addParam("random_lights_max_distance_from_fractal", 0.1, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("random_lights_intensity", 0.1, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("random_lights_distribution_radius", 3.0, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("random_lights_distribution_center", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam("random_lights_group", false, morphLinear, paramStandard);

	//fake lights
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

	//OpenCL Support
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

	//files
	par->addParam("file_destination", QDir::toNativeSeparators(systemData.dataDirectory + "images" + QDir::separator() + "image"), morphNone, paramStandard);
	par->addParam("file_background", QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator() + "background.jpg"), morphNone, paramStandard);
	par->addParam("file_envmap", QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator() + "envmap.jpg"), morphNone, paramStandard);
	par->addParam("file_lightmap", QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator() + "lightmap.jpg"), morphNone, paramStandard);
	par->addParam("file_animation_path", QDir::toNativeSeparators(systemData.dataDirectory + "paths" + QDir::separator() + "path.txt"), morphNone, paramStandard);
	par->addParam("file_keyframes", QDir::toNativeSeparators(systemData.dataDirectory + "keyframes" + QDir::separator() + "keyframe"), morphNone, paramStandard);

	//color palette
	cColorPalette palette(par->Get<int>("coloring_palette_size"), par->Get<int>("coloring_random_seed"), 1.0);
	par->addParam("surface_color_palette", palette, morphLinear, paramStandard);

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
	par->addParam("netrender_mode", (int) CNetRender::netRenderServer, morphNone, paramApp);
	par->addParam("netrender_client_remote_address", QString("localhost"), morphNone, paramApp);
	par->addParam("netrender_client_remote_port", 5555, morphNone, paramApp);
	par->addParam("netrender_server_local_port", 5555, morphNone, paramApp);

	par->addParam("default_image_path", systemData.dataDirectory + "images", morphNone, paramApp);
	par->addParam("default_textures_path", systemData.sharedDir + "textures", morphNone, paramApp);
	par->addParam("default_settings_path", systemData.dataDirectory + "settings", morphNone, paramApp);

	par->addParam("show_queue_thumbnails", false, morphNone, paramApp);
	par->addParam("queue_image_format", 0, morphNone, paramApp);

	par->addParam("quit_do_not_ask_again", false, morphNone, paramApp);

	//measurement
	par->addParam("meas_point", CVector3(0.0,0.0,0.0), morphNone, paramNoSave);
	par->addParam("meas_distance_from_last", 0.0, morphNone, paramNoSave);
	par->addParam("meas_distance_from_camera", 0.0, morphNone, paramNoSave);

	//----------------------- preferences ---------------------
	par->addParam("language", QString("unknown"), morphNone, paramApp);
	par->addParam("ui_style_type", -1, morphNone, paramApp);
	par->addParam("ui_skin", -1, morphNone, paramApp);
	par->addParam("ui_font_size", 9, 5, 50, morphNone, paramApp);
	par->addParam("toolbar_icon_size", 40, 20, 100, morphNone, paramApp);

	//image file configuration
	par->addParam("color_enabled", true, morphNone, paramApp);
	par->addParam("alpha_enabled", false, morphNone, paramApp);
	par->addParam("zbuffer_enabled", false, morphNone, paramApp);

	par->addParam("color_quality", (int) IMAGE_CHANNEL_QUALITY_8, morphNone, paramApp);
	par->addParam("alpha_quality", (int) IMAGE_CHANNEL_QUALITY_8, morphNone, paramApp);
	par->addParam("zbuffer_quality", (int) IMAGE_CHANNEL_QUALITY_32, morphNone, paramApp);

	par->addParam("color_postfix", QString(""), morphNone, paramApp);
	par->addParam("alpha_postfix", QString("_alpha"), morphNone, paramApp);
	par->addParam("zbuffer_postfix", QString("_zbuffer"), morphNone, paramApp);

	par->addParam("append_alpha_png", true, morphNone, paramApp);
	par->addParam("jpeg_quality", 95, 1, 100, morphNone, paramApp);

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

	WriteLog("Parameters initialization finished");
}

//definition of all parameters
void InitFractalParams(cParameterContainer *par)
{
	WriteLog("Fractal parameters initialization started: " + par->GetContainerName());

	par->addParam("power", 9.0, morphAkima, paramStandard);
	par->addParam("alpha_angle_offset", 0.0, morphAkimaAngle, paramStandard);
	par->addParam("beta_angle_offset", 0.0, morphAkimaAngle, paramStandard);
	par->addParam("gamma_angle_offset", 0.0, morphAkimaAngle, paramStandard);
	par->addParam("cadd", -1.3, morphLinear, paramStandard);

	//IFS formula
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

	//Mandelbox
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
		par->addParam("mandelbox_rotation_neg", i, CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
		par->addParam("mandelbox_rotation_pos", i, CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
	}
	par->addParam("mandelbox_color", CVector3(0.03, 0.05, 0.07), morphLinear, paramStandard);
  par->addParam("mandelbox_color_4D", CVector4(0.03, 0.05, 0.07, 0.0), morphLinear, paramStandard);//  to fix
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

	//FoldingIntPow
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
  par->addParam("mandelbulbMulti_order_of_xyz", 0, morphNone, paramStandard);

  //donut
	par->addParam("donut_ring_radius", 1.0, morphAkima, paramStandard);
	par->addParam("donut_ring_thickness", 0.1, morphAkima, paramStandard);
	par->addParam("donut_factor", 3.0, morphAkima, paramStandard);
	par->addParam("donut_number", 9.0, morphAkima, paramStandard);

  //common parameters for transforming formulas

  par->addParam("transf_offset", 0.0, morphAkima, paramStandard);
  par->addParam("transf_offset_0", 0.0, morphAkima, paramStandard);
  par->addParam("transf_offset_05", 0.5, morphAkima, paramStandard);
  par->addParam("transf_offset_2", 2.0, morphAkima, paramStandard);
  par->addParam("transf_multiplication", 2.0, morphAkima, paramStandard);
  par->addParam("transf_minimum_radius_0", 0.0, morphAkima, paramStandard);
  par->addParam("transf_minimum_radius_05", 0.5, morphAkima, paramStandard);
  par->addParam("transf_minimum_radius_06", 0.6, morphAkima, paramStandard);
  par->addParam("transf_pwr_05", 0.5, morphAkima, paramStandard);
  par->addParam("transf_pwr_4", 4.0, morphAkima, paramStandard);
  par->addParam("transf_pwr_8", 8.0, morphAkima, paramStandard);
  par->addParam("transf_pwr_8a", 8.0, morphAkima, paramStandard);
  par->addParam("transf_scale", 1.0, morphAkima, paramStandard);
  par->addParam("transf_scale_0", 0.0, morphAkima, paramStandard);
  par->addParam("transf_scale_1", 1.0, morphAkima, paramStandard);
  par->addParam("transf_scaleA_1", 1.0, morphAkima, paramStandard);
  par->addParam("transf_scale_015", 1.5, morphAkima, paramStandard);
  par->addParam("transf_scale_3", 3.0, morphAkima, paramStandard);


  par->addParam("transf_int_A", 0, morphLinear, paramStandard);
  par->addParam("transf_int_B", 0, morphLinear, paramStandard);
  par->addParam("transf_start_iterations", 0, morphLinear, paramStandard);
  par->addParam("transf_start_iterations_250", 250, morphLinear, paramStandard);
  par->addParam("transf_stop_iterations", 250, morphLinear, paramStandard);
  par->addParam("transf_start_iterations_A", 0, morphLinear, paramStandard);
  par->addParam("transf_stop_iterations_A", 250, morphLinear, paramStandard);
  par->addParam("transf_start_iterations_B", 0, morphLinear, paramStandard);
  par->addParam("transf_stop_iterations_B", 250, morphLinear, paramStandard);
  par->addParam("transf_start_iterations_C", 0, morphLinear, paramStandard);
  par->addParam("transf_stop_iterations_C", 250, morphLinear, paramStandard);



  par->addParam("transf_addition_constant_0555", CVector3(0.5, 0.5, 0.5), morphAkima, paramStandard);
  par->addParam("transf_addition_constant", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
  par->addParam("transf_addition_constantA_000", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
  par->addParam("transf_addition_constant_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
  par->addParam("transf_addition_constantA_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
  par->addParam("transf_addition_constant_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);
  par->addParam("transf_addition_constant_neg100", CVector3(-1.0, 0.0, 0.0), morphAkima, paramStandard);
  par->addParam("transf_constant_julia_c", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_constant_factor", CVector3(1.0, 2.0, 2.0), morphAkima, paramStandard);
  par->addParam("transf_constant_multiplier_000", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_constant_multiplier_001", CVector3(0.0, 0.0, 1.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_constant_multiplier_010", CVector3(0.0, 1.0, 0.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_constant_multiplier_100", CVector3(1.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_constant_multiplier_111", CVector3(1.0, 1.0, 1.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_constant_multiplier_121", CVector3(1.0, 2.0, 1.0), morphAkima, paramStandard);
  par->addParam("transf_constant_multiplier_122", CVector3(1.0, 2.0, 2.0), morphAkima, paramStandard);
  par->addParam("transf_constant_multiplier_221", CVector3(2.0, 2.0, 1.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_constant_multiplier_222", CVector3(2.0, 2.0, 2.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_constant_multiplier_441", CVector3(4.0, 4.0, 1.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_offset_000", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
  par->addParam("transf_offset_200", CVector3(2.0, 0.0, 0.0), morphAkima, paramStandard);
  par->addParam("transf_power_025", CVector3(0.25, 0.25, 0.25), morphAkimaAngle, paramStandard);
  par->addParam("transf_power_8", CVector3(8.0, 8.0, 8.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_rotation", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
  par->addParam("transf_scale3D_111", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
  par->addParam("transf_scale3D_222", CVector3(2.0, 2.0, 2.0), morphAkima, paramStandard);
  par->addParam("transf_scale3D_333", CVector3(3.0, 3.0, 3.0), morphAkima, paramStandard);
  par->addParam("transf_scale3D_444", CVector3(4.0, 4.0, 4.0), morphAkima, paramStandard);

  par->addParam("transf_addition_constant_0000", CVector4(0.0, 0.0, 0.0, 0.0), morphAkima, paramStandard);
  par->addParam("transf_constant_multiplier_1220", CVector4(1.0, 2.0, 2.0, 0.0), morphAkima, paramStandard);

  par->addParam("transf_addCpixel_enabled", true, morphLinear, paramStandard);
  par->addParam("transf_addCpixel_enabled_false", false, morphLinear, paramStandard);
  par->addParam("transf_benesi_T1_enabled", true, morphLinear, paramStandard);
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
  par->addParam("transf_rotation_enabled", false, morphLinear, paramStandard);

	// platonic_solid

	par->addParam("platonic_solid_frequency", 1.0, morphAkima, paramStandard);
	par->addParam("platonic_solid_amplitude", 1.0, morphAkima, paramStandard);
	par->addParam("platonic_solid_rhoMul", 1.0, morphAkima, paramStandard);

	par->addParam("info", false, morphNone, paramStandard); //dummy parameter for information groupbox

	WriteLog("Fractal parameters initialization finished");
}

void InitPrimitiveParams(fractal::enumObjectType objectType, const QString primitiveName, cParameterContainer *par)
{
	par->addParam(QString(primitiveName) + "_position", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam(QString(primitiveName) + "_rotation", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	par->addParam(QString(primitiveName) + "_color", sRGB(32000, 32000, 32000), morphAkima, paramStandard);
	par->addParam(QString(primitiveName) + "_reflection", 0.0, 0.0, 1.0, morphAkima, paramStandard);
	par->addParam(QString(primitiveName) + "_enabled", false, morphAkima, paramStandard);

	switch (objectType)
	{
		case fractal::objBox:
			par->addParam(QString(primitiveName) + "_size", CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_rounding", 1e-15, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_empty", false, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);
			break;
		case fractal::objCircle:
			par->addParam(QString(primitiveName) + "_radius", 1.0, morphAkima, paramStandard);
			break;
		case fractal::objCylinder:
			par->addParam(QString(primitiveName) + "_radius", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_height", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_caps", true, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_empty", false, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);
			break;
		case fractal::objCone:
			par->addParam(QString(primitiveName) + "_radius", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_height", 1.0, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_caps", true, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_empty", false, morphAkima, paramStandard);
			par->addParam(QString(primitiveName) + "_repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);
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
			par->addParam(QString(primitiveName) + "_repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);
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
			par->addParam(QString(primitiveName) + "_repeat", CVector3(0.0, 0.0, 0.0), morphLinear, paramStandard);
			break;

		default:
			break;
	}
}

void InitMaterialParams(int materialId, cParameterContainer *par)
{
  par->addParam(cMaterial::Name("shading", materialId), 1.0, 0.0, 1e15, morphAkima, paramStandard);
  par->addParam(cMaterial::Name("specular", materialId), 1.0, 0.0, 1e15, morphAkima, paramStandard);
  par->addParam(cMaterial::Name("specularWidth", materialId), 1.0, 1e-10, 1e15, morphAkima, paramStandard);
  par->addParam(cMaterial::Name("reflection", materialId), 0.0, 0.0, 1e15, morphAkima, paramStandard);
  par->addParam(cMaterial::Name("lightness", materialId), 0.0, 0.0, 1e15, morphAkima, paramStandard);
  par->addParam(cMaterial::Name("bump_map_intensity", materialId), 1.0, morphAkima, paramStandard);
  par->addParam(cMaterial::Name("transparency_of_surface", materialId), 0.0, 0.0, 1.0, morphAkima, paramStandard);
  par->addParam(cMaterial::Name("transparency_of_interior", materialId), 1.0, 0.0, 1.0, morphAkima, paramStandard);
  par->addParam(cMaterial::Name("transparency_index_of_refraction", materialId), 1.5, 0.0, 100.0, morphAkima, paramStandard);
  par->addParam(cMaterial::Name("surface_color", materialId), sRGB(50000, 50000, 50000), morphAkima, paramStandard);
  par->addParam(cMaterial::Name("transparency_interior_color", materialId), sRGB(65535, 65535, 65535), morphLinear, paramStandard);
  par->addParam(cMaterial::Name("lightness_color", materialId), sRGB(65535, 65535, 65535), morphLinear, paramStandard);
  par->addParam(cMaterial::Name("fresnel_reflectance", materialId), false, morphLinear, paramStandard);
  par->addParam(cMaterial::Name("texture_center", materialId), CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
  par->addParam(cMaterial::Name("texture_rotation", materialId), CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
  par->addParam(cMaterial::Name("texture_scale", materialId), CVector3(1.0, 1.0, 1.0), morphAkima, paramStandard);
  par->addParam(cMaterial::Name("coloring_random_seed", materialId), 269259, morphLinear, paramStandard);
  par->addParam(cMaterial::Name("coloring_saturation", materialId), 1.0, 0.0, 1000.0, morphLinear, paramStandard);
  par->addParam(cMaterial::Name("coloring_speed", materialId), 1.0, 0.0, 1e15, morphLinear, paramStandard);
  par->addParam(cMaterial::Name("coloring_palette_size", materialId), 10, 1, 255, morphLinear, paramStandard);
  par->addParam(cMaterial::Name("coloring_palette_offset", materialId), 0.0, 0.0, 256.0, morphLinear, paramStandard);
  par->addParam(cMaterial::Name("texture_mapping_type", materialId), (int)cMaterial::mappingPlanar, morphNone, paramStandard);
  par->addParam(cMaterial::Name("use_colors_from_palette", materialId), true, morphLinear, paramStandard);

  par->addParam(cMaterial::Name("file_color_texture", materialId),
                QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator()
                    + "diffu_texture.jpg"),
                morphNone,
                paramStandard);
  par->addParam(cMaterial::Name("file_diffusion_texture", materialId),
                QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator()
                    + "diffusion_texture.jpg"),
                morphNone,
                paramStandard);
  par->addParam(cMaterial::Name("file_lightness_texture", materialId),
                QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator()
                    + "lightness_texture.jpg"),
                morphNone,
                paramStandard);
  par->addParam(cMaterial::Name("file_bumpmap_texture", materialId),
                QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator()
                    + "bumpmap_texture.jpg"),
                morphNone,
                paramStandard);

  cColorPalette palette(par->Get<int>(cMaterial::Name("coloring_palette_size", materialId)),
                        par->Get<int>(cMaterial::Name("coloring_random_seed", materialId)),
                        1.0);
  par->addParam(cMaterial::Name("surface_color_palette", materialId), palette, morphLinear, paramStandard);
}

void DeletePrimitiveParams(fractal::enumObjectType objectType, const QString primitiveName, cParameterContainer *par)
{
	par->DeleteParameter(QString(primitiveName) + "_position");
	par->DeleteParameter(QString(primitiveName) + "_rotation");
	par->DeleteParameter(QString(primitiveName) + "_color");
	par->DeleteParameter(QString(primitiveName) + "_reflection");
	par->DeleteParameter(QString(primitiveName) + "_enabled");

	switch (objectType)
	{
		case fractal::objBox:
			par->DeleteParameter(QString(primitiveName) + "_size");
			par->DeleteParameter(QString(primitiveName) + "_rounding");
			par->DeleteParameter(QString(primitiveName) + "_empty");
			par->DeleteParameter(QString(primitiveName) + "_repeat");
			break;
		case fractal::objCircle:
			par->DeleteParameter(QString(primitiveName) + "_radius");
			break;
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
		case fractal::objPlane:
			par->DeleteParameter(QString(primitiveName) + "_empty");
			break;
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

		default:
			break;
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
