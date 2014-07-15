/*********************************************************
 /                   MANDELBULBER
 / definition of structures for image and effect
 / parameters
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 ********************************************************/
#include "fractparams.hpp"
#include "fractal.h"
#include "fractal_list.hpp"
#include "parameters.hpp"
#include "system.hpp"
#include "color_palette.hpp"
#include <QtCore>

//definition of all parameters
void InitParams(cParameterContainer *par)
{
using namespace parameterContainer;
	WriteLog("Parameters initialization started");

	//image
	par->addParam("image_width", 800, 32, 1000000, morphNone, paramStandard);
	par->addParam("image_height", 600, 32, 1000000, morphNone, paramStandard);
	par->addParam("tiles", 1, 1, 64, morphNone, paramStandard);
	par->addParam("tile_number", 0, morphNone, paramStandard);
	par->addParam("image_proportion", 0, morphNone, paramNoSave);

	//animation
	par->addParam("start_frame", 0, 0, 99999, morphNone, paramStandard);
	par->addParam("end_frame", 1000, 0, 99999, morphNone, paramStandard);
	par->addParam("frames_per_keyframe", 100, 1, 99999, morphNone, paramStandard);
	par->addParam("frame_no", 0, 0, 99999, morphNone, paramNoSave);
	par->addParam("flight_speed", 0.1, 1e-15, 1e15, morphNone, paramNoSave);

	//render control
	par->addParam("play_mode", false, morphNone, paramNoSave);
	par->addParam("anim_mode", false, morphNone, paramNoSave);
	par->addParam("record_mode", false, morphNone, paramNoSave);
	par->addParam("continue_record_mode", false, morphNone, paramNoSave);

	//camera

	par->addParam("camera", CVector3(0.0, 6.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("target", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("camera_top", CVector3(0.0, 0.0, 1.0), morphcatMullRom, paramStandard); //internal vector which represents top direction
	par->addParam("camera_rotation", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("camera_distance_to_target", 3.0, 0.0, 1e15, morphcatMullRom, paramStandard);
	par->addParam("fov", 0.5, 0.0, 100.0, morphcatMullRom, paramStandard);
	par->addParam("perspective_type", 0, morphLinear, paramStandard);
	par->addParam("stereo_eye_distance", 1.0, morphcatMullRom, paramStandard);
	par->addParam("stereo_enabled", false, morphLinear, paramStandard);

	//general fractal and engine
	par->addParam("julia_mode", false, morphLinear, paramStandard);
	par->addParam("julia_c", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("N", 250, 1, 65536, morphLinear, paramStandard);
	par->addParam("minN", 1, 0, 65536, morphLinear, paramStandard);
	par->addParam("fractal_constant_factor", 1.0, morphLinear, paramStandard);
	par->addParam("detail_level", 1.0, morphLinear, paramStandard);
	par->addParam("DE_thresh", 1.0, morphLinear, paramStandard); //old name was 'quality'
	par->addParam("smoothness", 1.0, morphLinear, paramStandard);
	par->addParam("iteration_threshold_mode", false, morphNone, paramStandard);
	par->addParam("analityc_DE_mode", true, morphNone, paramStandard);
	par->addParam("DE_factor", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("slow_shading", false, morphLinear, paramStandard);
	par->addParam("view_distance_max", 50.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("view_distance_min", 1e-15, 1e-15, 1e15, morphLinear, paramStandard);
	par->addParam("limit_min", CVector3(-10.0, -10.0, -10.0), morphLinear, paramStandard);
	par->addParam("limit_max", CVector3(10.0, 10.0, 10.0), morphLinear, paramStandard);
	par->addParam("limits_enabled", false, morphLinear, paramStandard);
	par->addParam("interior_mode", false, morphLinear, paramStandard);
	par->addParam("linear_DE_mode", false, morphLinear, paramStandard);
	par->addParam("constant_DE_threshold", false, morphLinear, paramStandard);
	par->addParam("hybrid_fractal_enable", false, morphNone, paramStandard);

	//foldings
	par->addParam("tglad_folding_mode", false, morphLinear, paramStandard);
	par->addParam("folding_limit", 1.0, morphLinear, paramStandard);
	par->addParam("folding_value", 2.0, morphLinear, paramStandard);
	par->addParam("spherical_folding_mode", false, morphLinear, paramStandard);
	par->addParam("spherical_folding_fixed", 1.0, morphLinear, paramStandard);
	par->addParam("spherical_folding_min", 0.5, morphLinear, paramStandard);
	par->addParam("c_add", -1.3, morphLinear, paramStandard);
	par->addParam("IFS_folding_mode", false, morphLinear, paramStandard);
	par->addParam("mandelbox_fold_mode", true, morphLinear, paramStandard);

	//image effects
	par->addParam("brightness", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("contrast", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("gamma", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("hdr", false, morphLinear, paramStandard);
	par->addParam("reflect", 0.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("ambient_occlusion", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_quality", 4, 1, 10, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_fast_tune", 1.0, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_enabled", false, morphLinear, paramStandard);
	par->addParam("ambient_occlusion_mode", (int)params::AOmodeFast, morphLinear, paramStandard);
	par->addParam("shading", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("specular", 1.0, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("glow_enabled", true, morphLinear, paramStandard);
	par->addParam("glow_intensity", 0.2, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("textured_background", false, morphLinear, paramStandard);
	par->addParam("textured_background_map_type", (int)params::mapEquirectangular, morphNone, paramStandard);
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
	par->addParam("fog_color", 1, sRGB(20000, 20000, 20000), morphLinear, paramStandard);
	par->addParam("fog_color", 2, sRGB(0, 30000, 65535), morphLinear, paramStandard);
	par->addParam("fog_color", 3, sRGB(65535, 65535, 65535), morphLinear, paramStandard);

	par->addParam("volumetric_fog_enabled", false, morphLinear, paramStandard);
	par->addParam("volumetric_fog_density", 0.5, 0.0, 1e15, morphLinear, paramStandard);
	par->addParam("volumetric_fog_colour_1_distance", 1.0, morphLinear, paramStandard);
	par->addParam("volumetric_fog_colour_2_distance", 2.0, morphLinear, paramStandard);
	par->addParam("volumetric_fog_distance_factor", 1.0, morphLinear, paramStandard);

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
	par->addParam("coloring_random_seed", 123456, morphLinear, paramStandard);
	par->addParam("coloring_saturation", 1.0, morphLinear, paramStandard);
	par->addParam("coloring_speed", 1.0, morphLinear, paramStandard);
	par->addParam("coloring_palette_size", 10, 1, 255, morphLinear, paramStandard);
	par->addParam("coloring_palette_offset", 0.0, 0.0, 256.0, morphLinear, paramStandard);

	//fog
	par->addParam("basic_fog_enabled", false, morphLinear, paramStandard);
	par->addParam("basic_fog_visibility", 20.0, morphLinear, paramStandard);
	par->addParam("basic_fog_color", sRGB(59399, 61202, 65535), morphLinear, paramStandard);
	par->addParam("DOF_enabled", false, morphLinear, paramStandard);
	par->addParam("DOF_focus", 166.0, 0.0, 200.0, morphLinear, paramStandard);
	par->addParam("DOF_radius", 10.0, 0.0, 200.0, morphLinear, paramStandard);

	//main light
	par->addParam("main_light_intensity", 1.0, morphLinear, paramStandard);
	par->addParam("main_light_visibility", 1.0, morphLinear, paramStandard);
	par->addParam("main_light_visibility_size", 1.0, morphLinear, paramStandard);
	par->addParam("main_light_alpha", -45.0, morphcatMullRom, paramStandard);
	par->addParam("main_light_beta", 45.0,  morphcatMullRom, paramStandard);
	par->addParam("main_light_colour", sRGB(65535, 65535, 65535), morphLinear, paramStandard);
	par->addParam("shadows_cone_angle", 1.0, 0.0, 180.0, morphLinear, paramStandard);
	par->addParam("main_light_enable", true, morphLinear, paramStandard);
	par->addParam("main_light_position_relative", true, morphLinear, paramStandard);

	//aux lights
	par->addParam("aux_light_intensity", 1.0, morphLinear, paramStandard);
	par->addParam("aux_light_random_seed", 1234, morphNone, paramStandard);
	par->addParam("aux_light_number", 0, 0, 9999, morphNone, paramStandard);
	par->addParam("aux_light_max_dist", 0.1, 0.0, 1e15, morphNone, paramStandard);
	par->addParam("aux_light_distribution_radius", 3.0, 0.0, 1e15, morphNone, paramStandard);
	par->addParam("aux_light_random_center", CVector3(0.0, 0.0, 0.0), morphNone, paramStandard);
	par->addParam("aux_light_visibility", 1.0, morphLinear, paramStandard);
	par->addParam("aux_light_visibility_size", 1.0, morphLinear, paramStandard);

	par->addParam("aux_light_predefined_position", 1, CVector3(3.0, -3.0, 3.0), morphcatMullRom, paramStandard);
	par->addParam("aux_light_predefined_position", 2, CVector3(-3.0, -3.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("aux_light_predefined_position", 3, CVector3(-3.0, 3.0, -1.0), morphcatMullRom, paramStandard);
	par->addParam("aux_light_predefined_position", 4, CVector3(0.0, -1.0, -3.0), morphcatMullRom, paramStandard);
	par->addParam("aux_light_predefined_intensity", 1, 1.3, morphLinear, paramStandard);
	par->addParam("aux_light_predefined_intensity", 2, 1.0, morphLinear, paramStandard);
	par->addParam("aux_light_predefined_intensity", 3, 3, morphLinear, paramStandard);
	par->addParam("aux_light_predefined_intensity", 4, 2, morphLinear, paramStandard);
	par->addParam("aux_light_predefined_enabled", 1, false, morphLinear, paramStandard);
	par->addParam("aux_light_predefined_enabled", 2, false, morphLinear, paramStandard);
	par->addParam("aux_light_predefined_enabled", 3, false, morphLinear, paramStandard);
	par->addParam("aux_light_predefined_enabled", 4, false, morphLinear, paramStandard);
	par->addParam("aux_light_predefined_colour", 1, sRGB(45761, 53633, 59498), morphLinear, paramStandard);
	par->addParam("aux_light_predefined_colour", 2, sRGB(62875, 55818, 50083), morphLinear, paramStandard);
	par->addParam("aux_light_predefined_colour", 3, sRGB(64884, 64928, 48848), morphLinear, paramStandard);
	par->addParam("aux_light_predefined_colour", 4, sRGB(52704, 62492, 45654), morphLinear, paramStandard);

	for(int i=0; i<5; i++)
	{
		par->addParam("volumetric_light_intensity", i, 100.0, morphLinear, paramStandard);
		par->addParam("volumetric_light_enabled", i, false, morphLinear, paramStandard);
	}

	//fake lights
	par->addParam("fake_lights_enabled", false, morphLinear, paramStandard);
	par->addParam("fake_lights_intensity", 1.0, morphLinear, paramStandard);
	par->addParam("fake_lights_visibility", 1.0, morphLinear, paramStandard);
	par->addParam("fake_lights_visibility_size", 5.0, morphLinear, paramStandard);
	par->addParam("fake_lights_orbit_trap", CVector3(2.0, 0.0, 0.0), morphLinear, paramStandard);
	par->addParam("fake_lights_min_iter", 1, 0, 250, morphLinear, paramStandard);
	par->addParam("fake_lights_max_iter", 2, 0, 250, morphLinear, paramStandard);

	//primitives
	par->addParam("primitive_only_plane", false, morphNone, paramStandard);

	par->addParam("primitive_plane_enabled", false, morphLinear, paramStandard);
	par->addParam("primitive_plane_centre", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("primitive_plane_normal", CVector3(0.0, 0.0, -1.0), morphcatMullRom, paramStandard);
	par->addParam("primitive_plane_colour", sRGB(20000, 20000, 20000), morphLinear, paramStandard);
	par->addParam("primitive_plane_reflect", 0.0, 0.0, 1e15, morphLinear, paramStandard);

	par->addParam("primitive_box_enabled", false, morphLinear, paramStandard);
	par->addParam("primitive_box_centre", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("primitive_box_size", CVector3(2.0, 2.0, 2.0), morphcatMullRom, paramStandard);
	par->addParam("primitive_box_colour", sRGB(20000, 20000, 20000), morphLinear, paramStandard);
	par->addParam("primitive_box_reflect", 0.0, 0.0, 1e15, morphLinear, paramStandard);

	par->addParam("primitive_invertedBox_enabled", false, morphLinear, paramStandard);
	par->addParam("primitive_invertedBox_centre", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("primitive_invertedBox_size", CVector3(10.0, 10.0, 10.0), morphcatMullRom, paramStandard);
	par->addParam("primitive_invertedBox_colour", sRGB(20000, 20000, 20000), morphLinear, paramStandard);
	par->addParam("primitive_invertedBox_reflect", 0.0, 0.0, 1e15, morphLinear, paramStandard);

	par->addParam("primitive_sphere_enabled", false, morphLinear, paramStandard);
	par->addParam("primitive_sphere_centre", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("primitive_sphere_radius", 1.5, morphcatMullRom, paramStandard);
	par->addParam("primitive_sphere_colour", sRGB(20000, 20000, 20000), morphLinear, paramStandard);
	par->addParam("primitive_sphere_reflect", 0.0, 0.0, 1e15, morphLinear, paramStandard);

	par->addParam("primitive_invertedSphere_enabled", false, morphLinear, paramStandard);
	par->addParam("primitive_invertedSphere_centre", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("primitive_invertedSphere_radius", 5.0, morphcatMullRom, paramStandard);
	par->addParam("primitive_invertedSphere_colour", sRGB(20000, 20000, 20000), morphLinear, paramStandard);
	par->addParam("primitive_invertedSphere_reflect", 0.0, 0.0, 1e15, morphLinear, paramStandard);

	par->addParam("primitive_water_enabled", false, morphLinear, paramStandard);
	par->addParam("primitive_water_level", 0.0, morphLinear, paramStandard);
	par->addParam("primitive_water_amplitude", 0.02, morphLinear, paramStandard);
	par->addParam("primitive_water_length", 0.2, morphLinear, paramStandard);
	par->addParam("primitive_water_iterations", 5, 1, 250, morphLinear, paramStandard);
	par->addParam("primitive_water_rotation", 0.0, morphLinear, paramStandard);
	par->addParam("primitive_water_anim_speed", 0.1, morphLinear, paramStandard);
	par->addParam("primitive_water_colour", sRGB(0, 5000, 10000), morphLinear, paramStandard);
	par->addParam("primitive_water_reflect", 0.7, 0.0, 1e15, morphLinear, paramStandard);

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
	par->addParam("file_destination", QString("images/image"), morphNone, paramStandard);
	par->addParam("file_background", (QString(systemData.sharedDir)+"textures/background.jpg"), morphNone, paramStandard);
	par->addParam("file_envmap", (QString(systemData.sharedDir)+"textures/envmap.jpg"), morphNone, paramStandard);
	par->addParam("file_lightmap", (QString(systemData.sharedDir)+"textures/lightmap.jpg"), morphNone, paramStandard);
	par->addParam("file_animation_path", QString("paths/path.txt"), morphNone, paramStandard);
	par->addParam("file_keyframes", QString("keyframes/keyframe"), morphNone, paramStandard);

	//color palette
	cColorPalette palette(par->Get<int>("coloring_palette_size"), par->Get<int>("coloring_random_seed"), 1.0);
	par->addParam("palette", &palette, morphLinear, paramStandard);

	//----------------------- application parameters ---------------------
	par->addParam("net_render_client_port", QString("5555"), morphNone, paramApp);
	par->addParam("net_render_client_IP", QString("10.0.0.4"), morphNone, paramApp);
	par->addParam("net_render_server_port", QString("5555"), morphNone, paramApp);
	par->addParam("light_manual_placement_dist", 0.1, 1e-15, 1e15, morphNone, paramApp);

	par->addParam("camera_movenent_step", 0.5, 1e-15, 1e5, morphNone, paramApp);
	par->addParam("camera_rotation_step", 15.0, 1e-15, 360.0, morphNone, paramApp);
	par->addParam("camera_straight_rotation", 0, morphNone, paramApp);
	par->addParam("camera_absolute_distance_mode", 0, morphNone, paramApp);
	par->addParam("camera_movement_mode", 0, morphNone, paramApp);
	par->addParam("camera_rotation_mode", 0, morphNone, paramApp);
	par->addParam("mouse_click_function", 0, morphNone, paramApp);
	par->addParam("show_cursor", false, morphNone, paramApp);

	par->addParam("auto_save_images", false, morphNone, paramApp);
	par->addParam("save_image_format", 0, morphNone, paramApp);
	par->addParam("image_preview_scale", 3, 0, 6, morphNone, paramApp);

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
	WriteLog("Fractal parameters initialization started");
	par->addParam("formula", (int)fractal::mandelbulb, morphNone, paramStandard);

	//***************************** NEEDED PARAMETER for iteration number for only this formula !!! ******************

	//fractal formula selections
	par->addParam("formula", 1, (int)fractal::mandelbulb, morphNone, paramNoSave);
	par->addParam("formula", 2, (int)fractal::none, morphNone, paramNoSave);
	par->addParam("formula", 3, (int)fractal::none, morphNone, paramNoSave);
	par->addParam("formula", 4, (int)fractal::none, morphNone, paramNoSave);
	par->addParam("formula_iterations", 1, 1, 1, 65536, morphNone, paramNoSave);
	par->addParam("formula_iterations", 2, 1, 1, 65536, morphNone, paramNoSave);
	par->addParam("formula_iterations", 3, 1, 1, 65536, morphNone, paramNoSave);
	par->addParam("formula_iterations", 4, 1, 1, 65536, morphNone, paramNoSave);

	par->addParam("power", 9.0, morphcatMullRom, paramStandard);
	par->addParam("alpha_angle_offset", 0.0, morphcatMullRom, paramStandard);
	par->addParam("beta_angle_offset", 0.0, morphcatMullRom, paramStandard);
	par->addParam("gamma_angle_offset", 0.0, morphcatMullRom, paramStandard);
	par->addParam("cadd", 0.0, morphLinear, paramStandard);

	//IFS formula
	par->addParam("IFS_scale", 2.0, morphcatMullRom, paramStandard);
	par->addParam("IFS_rotation", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("IFS_offset", CVector3(1.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("IFS_edge", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("IFS_abs_X", false, morphLinear, paramStandard);
	par->addParam("IFS_abs_Y", false, morphLinear, paramStandard);
	par->addParam("IFS_abs_Z", false, morphLinear, paramStandard);
	par->addParam("IFS_menger_sponge_mode", false, morphLinear, paramStandard);

	for(int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		par->addParam("IFS_direction", i, CVector3(1.0, 0.0, 0.0), morphcatMullRom, paramStandard);
		par->addParam("IFS_rotations", i, CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
		par->addParam("IFS_distance", i, 0.0, morphcatMullRom, paramStandard);
		par->addParam("IFS_intensity", i, 1.0, morphcatMullRom, paramStandard);
		par->addParam("IFS_enabled", i, false, morphLinear, paramStandard);
	}

	//Mandelbox
	par->addParam("mandelbox_scale", 2.0, morphcatMullRom, paramStandard);
	par->addParam("mandelbox_folding_limit", 1.0, morphcatMullRom, paramStandard);
	par->addParam("mandelbox_folding_value", 2.0, morphcatMullRom, paramStandard);
	par->addParam("mandelbox_folding_min_radius", 0.5, morphcatMullRom, paramStandard);
	par->addParam("mandelbox_folding_fixed_radius", 1.0, morphcatMullRom, paramStandard);
	par->addParam("mandelbox_sharpness", 3.0, morphcatMullRom, paramStandard);
	par->addParam("mandelbox_offset", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	par->addParam("mandelbox_rotation_main", CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	for(int i = 1; i<=3; i++)
	{
		par->addParam("mandelbox_rotation_neg", i, CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
		par->addParam("mandelbox_rotation_pos", i, CVector3(0.0, 0.0, 0.0), morphcatMullRom, paramStandard);
	}
	par->addParam("mandelbox_color", CVector3(0.03, 0.05, 0.07), morphLinear, paramStandard);
	par->addParam("mandelbox_color_R", 0.0, morphLinear, paramStandard);
	par->addParam("mandelbox_color_Sp1", 0.2, morphLinear, paramStandard);
	par->addParam("mandelbox_color_Sp2", 0.2, morphLinear, paramStandard);
	par->addParam("mandelbox_main_rotation_enabled", false, morphLinear, paramStandard);
	par->addParam("mandelbox_rotations_enabled", false, morphLinear, paramStandard);

	par->addParam("mandelbox_solid", 1.0, morphLinear, paramStandard);
	par->addParam("mandelbox_melt", 0.0, morphLinear, paramStandard);

	par->addParam("mandelbox_vary_scale_vary", 0.1, morphLinear, paramStandard);
	par->addParam("mandelbox_vary_fold", 1.0, morphLinear, paramStandard);
	par->addParam("mandelbox_vary_minr", 0.5, morphLinear, paramStandard);
	par->addParam("mandelbox_vary_rpower", 1.0, morphLinear, paramStandard);
	par->addParam("mandelbox_vary_wadd", 0.0, morphLinear, paramStandard);


	//FoldingIntPow
	par->addParam("boxfold_bulbpow2_folding_factor", 2.0, morphLinear, paramStandard);
	par->addParam("boxfold_bulbpow2_z_factor", 5.0, morphLinear, paramStandard);
	WriteLog("Fractal parameters initialization finished");
}


