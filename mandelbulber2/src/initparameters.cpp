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
#include "parameters.hpp"
#include "system.hpp"
#include <QtCore>

//definition of all parameters
void InitParams(parameters::container *par)
{

	WriteLog("Parameters initialization started");

	//image
	par->addParam("image_width", 800, 32, 1000000, false);
	par->addParam("image_height", 600, 32, 1000000, false);
	par->addParam("tiles", 1, 1, 64, false);
	par->addParam("tile_number", 0, false);
	par->addParam("image_proportion", 0, false);

	//animation
	par->addParam("start_frame", 0, 0, 99999, false);
	par->addParam("end_frame", 1000, 0, 99999, false);
	par->addParam("frames_per_keyframe", 100, 1, 99999, false);
	par->addParam("frame_no", 0, 0, 99999, false);
	par->addParam("flight_speed", 0.1, 1e-15, 1e15, false);

	//render control
	par->addParam("play_mode", false, false);
	par->SetToSave("play_mode", false);
	par->addParam("anim_mode", false, false);
	par->SetToSave("anim_mode", false);
	par->addParam("record_mode", false, false);
	par->SetToSave("record_mode", false);
	par->addParam("continue_record_mode", false, false);
	par->SetToSave("continue_record_mode", false);

	//camera
	par->addParam("view_point", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("view_angle", CVector3(-20.0, 30.0, 0.0), true);
	par->addParam("target", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("zoom", 2.5, 0.0, 1e15, true);
	par->addParam("fov", 0.5, 0.0, 100.0, true);
	par->addParam("perspective_type", 0, false);
	par->addParam("fish_eye_180cut", false, false);
	par->addParam("stereo_eye_distance", 1.0, true);
	par->addParam("stereo_enabled", false, false);

	//general fractal and engine
	par->addParam("julia_mode", false, true);
	par->addParam("julia_c", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("N", 250, 0, 65536, true);
	par->addParam("minN", 1, 0, 65536, true);
	par->addParam("fractal_constant_factor", 1.0, true);
	par->addParam("detail_level", 1.0, true);
	par->addParam("DE_thresh", 1.0, true); //old name was 'quality'
	par->addParam("smoothness", 1.0, true);
	par->addParam("iteration_threshold_mode", false, true);
	par->addParam("analityc_DE_mode", true, false);
	par->addParam("DE_factor", 1.0, 0.0, 1e15, true);
	par->addParam("slow_shading", false, true);
	par->addParam("view_distance_max", 50.0, 0.0, 1e15, true);
	par->addParam("view_distance_min", 1e-15, 1e-15, 1e15, true);
	par->addParam("limit_min", CVector3(-10.0, -10.0, -10.0), true);
	par->addParam("limit_max", CVector3(10.0, 10.0, 10.0), true);
	par->addParam("limits_enabled", false, true);
	par->addParam("interior_mode", false, true);
	par->addParam("linear_DE_mode", false, true);
	par->addParam("constant_DE_threshold", false, true);

	//foldings
	par->addParam("tglad_folding_mode", false, true);
	par->addParam("folding_limit", 1.0, true);
	par->addParam("folding_value", 2.0, true);
	par->addParam("spherical_folding_mode", false, true);
	par->addParam("spherical_folding_fixed", 1.0, true);
	par->addParam("spherical_folding_min", 0.5, true);
	par->addParam("c_add", -1.3, true);
	par->addParam("IFS_folding_mode", false, true);

	//image effects
	par->addParam("brightness", 1.0, 0.0, 1e15, true);
	par->addParam("contrast", 1.0, 0.0, 1e15, true);
	par->addParam("gamma", 1.0, 0.0, 1e15, true);
	par->addParam("hdr", false, true);
	par->addParam("ambient", 0.0, 0.0, 1e15, true);
	par->addParam("reflect", 0.0, 0.0, 1e15, true);
	par->addParam("ambient_occlusion", 1.0, 0.0, 1e15, true);
	par->addParam("ambient_occlusion_quality", 4, 1, 10, true);
	par->addParam("ambient_occlusion_fast_tune", 1.0, true);
	par->addParam("ambient_occlusion_enabled", false, true);
	par->addParam("ambient_occlusion_mode", (int)params::AOmodeFast, false);
	par->addParam("shading", 1.0, 0.0, 1e15, true);
	par->addParam("specular", 1.0, 0.0, 1e15, true);
	par->addParam("glow_enabled", true, true);
	par->addParam("glow_intensity", 0.2, 0.0, 1e15, true);
	par->addParam("textured_background", false, true);
	par->addParam("textured_background_map_type", (int)params::mapEquirectangular, true);
	par->addParam("shadows_enabled", true, true);
	par->addParam("penetrating_lights", true, true);
	par->addParam("raytraced_reflections", false, true);
	par->addParam("reflections_max", 5, 0, 10, false);

	par->addParam("glow_color", 1, sRGB(40984, 44713, 49490), true);
	par->addParam("glow_color", 2, sRGB(57192, 60888, 62408), true);
	par->addParam("background_color", 1, sRGB(0, 38306, 65535), true);
	par->addParam("background_color", 2, sRGB(65535, 65535, 65535), true);
	par->addParam("background_color", 3, sRGB(0, 10000, 500), true);
	par->addParam("fog_color", 1, sRGB(20000, 20000, 20000), true);
	par->addParam("fog_color", 2, sRGB(0, 30000, 65535), true);
	par->addParam("fog_color", 3, sRGB(65535, 65535, 65535), true);

	par->addParam("volumetric_fog_enabled", false, true);
	par->addParam("volumetric_fog_density", 0.5, 0.0, 1e15, true);
	par->addParam("volumetric_fog_colour_1_distance", 1.0, true);
	par->addParam("volumetric_fog_colour_2_distance", 2.0, true);
	par->addParam("volumetric_fog_distance_factor", 1.0, true);

	par->addParam("iteration_fog_enable", false, true);
	par->addParam("iteration_fog_opacity", 1000.0, 0.0, 1e15, true);
	par->addParam("iteration_fog_opacity_trim", 4.0, 0.0, 1000.0, true);
	par->addParam("iteration_fog_color_1_maxiter", 8.0, 0.0, 1000.0, true);
	par->addParam("iteration_fog_color_2_maxiter", 12.0, 0.0, 1000.0, true);
	par->addParam("iteration_fog_color", 1, sRGB(65535, 65535, 65535), true);
	par->addParam("iteration_fog_color", 2, sRGB(65535, 65535, 65535), true);
	par->addParam("iteration_fog_color", 3, sRGB(65535, 65535, 65535), true);

	//coloring
	par->addParam("fractal_color", true, true);
	par->addParam("coloring_random_seed", 123456, false);
	par->addParam("coloring_saturation", 1.0, true);
	par->addParam("coloring_speed", 1.0, true);
	par->addParam("coloring_palette_offset", 0.0, 0.0, 256.0, true);

	//fog
	par->addParam("basic_fog_enabled", false, true);
	par->addParam("basic_fog_visibility", 20.0, true);
	par->addParam("basic_fog_color", sRGB(59399, 61202, 65535), true);
	par->addParam("post_SSAO_enabled", true, true);
	par->addParam("DOF_enabled", false, true);
	par->addParam("DOF_focus", 166.0, 0.0, 200.0, true);
	par->addParam("DOF_radius", 10.0, 0.0, 200.0, true);

	//main light
	par->addParam("main_light_intensity", 1.0, true);
	par->addParam("main_light_visibility", 1.0, true);
	par->addParam("main_light_visibility_size", 1.0, true);
	par->addParam("main_light_alpha", -45.0, true);
	par->addParam("main_light_beta", 45.0, true);
	par->addParam("main_light_colour", sRGB(65535, 65535, 65535), true);
	par->addParam("shadows_cone_angle", 1.0, 0.0, 180.0, true);
	par->addParam("main_light_enable", true, true);
	par->addParam("main_light_position_relative", true, true);

	//aux lights
	par->addParam("aux_light_intensity", 1.0, true);
	par->addParam("aux_light_random_seed", 1234, false);
	par->addParam("aux_light_number", 0, 0, 9999, false);
	par->addParam("aux_light_max_dist", 0.1, 0.0, 1e15, false);
	par->addParam("aux_light_distribution_radius", 3.0, 0.0, 1e15, false);
	par->addParam("aux_light_random_center", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("aux_light_visibility", 1.0, true);
	par->addParam("aux_light_visibility_size", 1.0, true);

	par->addParam("aux_light_predefined_position", 1, CVector3(3.0, -3.0, -3.0), true);
	par->addParam("aux_light_predefined_position", 2, CVector3(-3.0, -3.0, 0.0), true);
	par->addParam("aux_light_predefined_position", 3, CVector3(-3.0, 3.0, 1.0), true);
	par->addParam("aux_light_predefined_position", 4, CVector3(0.0, -1.0, 3.0), true);
	par->addParam("aux_light_predefined_intensity", 1, 1.3, true);
	par->addParam("aux_light_predefined_intensity", 2, 1.0, true);
	par->addParam("aux_light_predefined_intensity", 3, 3, true);
	par->addParam("aux_light_predefined_intensity", 4, 2, true);
	par->addParam("aux_light_predefined_enabled", 1, false, true);
	par->addParam("aux_light_predefined_enabled", 2, false, true);
	par->addParam("aux_light_predefined_enabled", 3, false, true);
	par->addParam("aux_light_predefined_enabled", 4, false, true);
	par->addParam("aux_light_predefined_colour", 1, sRGB(45761, 53633, 59498), true);
	par->addParam("aux_light_predefined_colour", 2, sRGB(62875, 55818, 50083), true);
	par->addParam("aux_light_predefined_colour", 3, sRGB(64884, 64928, 48848), true);
	par->addParam("aux_light_predefined_colour", 4, sRGB(52704, 62492, 45654), true);

	for(int i=0; i<5; i++)
	{
		par->addParam("volumetric_light_intensity", i, 100.0, true);
		par->addParam("volumetric_light_enabled", i, false, true);
	}

	//fake lights
	par->addParam("fake_lights_enabled", false, true);
	par->addParam("fake_lights_intensity", 1.0, true);
	par->addParam("fake_lights_visibility", 1.0, true);
	par->addParam("fake_lights_visibility_size", 5.0, true);
	par->addParam("fake_lights_orbit_trap", CVector3(2.0, 0.0, 0.0), true);
	par->addParam("fake_lights_min_iter", 1, 0, 250, true);
	par->addParam("fake_lights_max_iter", 2, 0, 250, true);

	//primitives
	par->addParam("primitive_only_plane", false, false);

	par->addParam("primitive_plane_enabled", false, false);
	par->addParam("primitive_plane_centre", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("primitive_plane_normal", CVector3(0.0, 0.0, -1.0), true);
	par->addParam("primitive_plane_colour", sRGB(20000, 20000, 20000), true);
	par->addParam("primitive_plane_reflect", 0.0, 0.0, 1e15, true);

	par->addParam("primitive_box_enabled", false, true);
	par->addParam("primitive_box_centre", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("primitive_box_size", CVector3(2.0, 2.0, 2.0), true);
	par->addParam("primitive_box_colour", sRGB(20000, 20000, 20000), true);
	par->addParam("primitive_box_reflect", 0.0, 0.0, 1e15, true);

	par->addParam("primitive_invertedBox_enabled", false, true);
	par->addParam("primitive_invertedBox_centre", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("primitive_invertedBox_size", CVector3(10.0, 10.0, 10.0), true);
	par->addParam("primitive_invertedBox_colour", sRGB(20000, 20000, 20000), true);
	par->addParam("primitive_invertedBox_reflect", 0.0, 0.0, 1e15, true);

	par->addParam("primitive_sphere_enabled", false, true);
	par->addParam("primitive_sphere_centre", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("primitive_sphere_radius", 1.5, true);
	par->addParam("primitive_sphere_colour", sRGB(20000, 20000, 20000), true);
	par->addParam("primitive_sphere_reflect", 0.0, 0.0, 1e15, true);

	par->addParam("primitive_invertedSphere_enabled", false, true);
	par->addParam("primitive_invertedSphere_centre", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("primitive_invertedSphere_radius", 5.0, true);
	par->addParam("primitive_invertedSphere_colour", sRGB(20000, 20000, 20000), true);
	par->addParam("primitive_invertedSphere_reflect", 0.0, 0.0, 1e15, true);

	par->addParam("primitive_water_enabled", false, true);
	par->addParam("primitive_water_level", 0.0, true);
	par->addParam("primitive_water_amplitude", 0.02, true);
	par->addParam("primitive_water_length", 0.2, true);
	par->addParam("primitive_water_iterations", 5, 1, 250, true);
	par->addParam("primitive_water_rotation", 0.0, true);
	par->addParam("primitive_water_anim_speed", 0.1, true);
	par->addParam("primitive_water_colour", sRGB(0, 5000, 10000), true);
	par->addParam("primitive_water_reflect", 0.7, 0.0, 1e15, true);

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
	par->addParam("file_destination", QString("images/image"), false);
	par->addParam("file_background", (QString(systemData.sharedDir)+"textures/background.jpg"), false);
	par->addParam("file_envmap", (QString(systemData.sharedDir)+"textures/envmap.jpg"), false);
	par->addParam("file_lightmap", (QString(systemData.sharedDir)+"textures/lightmap.jpg"), false);
	par->addParam("file_animation_path", QString("paths/path.txt"), false);
	par->addParam("file_keyframes", QString("keyframes/keyframe"), false);

	//color palette
	//sRGB palette[256];
	//srand(par->Get<int>("coloring_random_seed"));
	//NewPalette(palette, 1.0);
	//par->addParam("palette", palette, false);

	//----------------------- application parameters ---------------------
	par->addParam("net_render_client_port", QString("5555"), true);
	par->SetAsAppParam("net_render_client_port", true);
	par->addParam("net_render_client_IP", QString("10.0.0.4"), true);
	par->SetAsAppParam("net_render_client_IP", true);
	par->addParam("net_render_server_port", QString("5555"), true);
	par->SetAsAppParam("net_render_server_port", true);
	par->addParam("light_manual_placement_dist", 0.1, 1e-15, 1e15, true);
	par->SetAsAppParam("light_manual_placement_dist", true);
	par->addParam("camera_movenent_step_de", 0.5, 1e-15, 1e5, false);
	par->SetAsAppParam("camera_movenent_step_de", true);
	par->addParam("camera_movenent_step_absolute", 0.5, 1e-15, 1e15, false);
	par->SetAsAppParam("camera_movenent_step_absolute", true);
	par->addParam("camera_rotation_step", 0.5, 1e-15, 360.0, false);
	par->SetAsAppParam("camera_rotation_step", true);
	par->addParam("camera_mouse_click_step", 0.5, 1e-15, 1e5, false);
	par->SetAsAppParam("camera_mouse_click_step", true);
	par->addParam("camera_straight_rotation", false, false);
	par->SetAsAppParam("camera_straight_rotation", true);
	par->addParam("camera_absolute_distance_mode", false, false);
	par->SetAsAppParam("camera_absolute_distance_mode", true);
	par->addParam("camera_go_to_surface_mode", false, false);
	par->SetAsAppParam("camera_go_to_surface_mode", true);
	par->addParam("auto_save_images", false, false);
	par->SetAsAppParam("auto_save_images", true);
	par->addParam("save_image_format", 0, false);
	par->SetAsAppParam("save_image_format", true);

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
void InitFractalParams(parameters::container *par)
{
	WriteLog("Fractal parameters initialization started");

	par->addParam("formula", (int)fractal::trig_optim, false);

	par->addParam("power", 9.0, true);
	par->addParam("alpha_angle_offset", 0.0, true);
	par->addParam("beta_angle_offset", 0.0, true);

	//IFS formula
	par->addParam("IFS_scale", 2.0, true);
	par->addParam("IFS_rot_alpha", 0.0, true);
	par->addParam("IFS_rot_beta", 0.0, true);
	par->addParam("IFS_rot_gamma", 0.0, true);
	par->addParam("IFS_offset", CVector3(1.0, 0.0, 0.0), true);
	par->addParam("IFS_edge", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("IFS_abs_X", false, true);
	par->addParam("IFS_abs_Y", false, true);
	par->addParam("IFS_abs_Z", false, true);
	par->addParam("IFS_menger_sponge_mode", false, true);

	for(int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		par->addParam("IFS_direction", i, CVector3(1.0, 0.0, 0.0), true);
		par->addParam("IFS_alpha", i, 0.0, true);
		par->addParam("IFS_beta", i, 0.0, true);
		par->addParam("IFS_gamma", i, 0.0, true);
		par->addParam("IFS_distance", i, 0.0, true);
		par->addParam("IFS_intensity", i, 1.0, true);
		par->addParam("IFS_enabled", i, false, true);
	}

	//Mandelbox
	par->addParam("mandelbox_scale", 2.0, true);
	par->addParam("mandelbox_folding_limit", 1.0, true);
	par->addParam("mandelbox_folding_value", 2.0, true);
	par->addParam("mandelbox_folding_min_radius", 0.5, true);
	par->addParam("mandelbox_folding_fixed_radius", 1.0, true);
	par->addParam("mandelbox_sharpness", 3.0, true);
	par->addParam("mandelbox_offset", CVector3(0.0, 0.0, 0.0), true);
	par->addParam("mandelbox_rotation_main", CVector3(0.0, 0.0, 0.0), true);
	for(int i = 1; i<=3; i++)
	{
		par->addParam("mandelbox_rotation_neg", i, CVector3(0.0, 0.0, 0.0), true);
		par->addParam("mandelbox_rotation_pos", i, CVector3(0.0, 0.0, 0.0), true);
	}
	par->addParam("mandelbox_color", CVector3(0.03, 0.05, 0.07), true);
	par->addParam("mandelbox_color_R", 0.0, true);
	par->addParam("mandelbox_color_Sp1", 0.2, true);
	par->addParam("mandelbox_color_Sp2", 0.2, true);
	par->addParam("mandelbox_rotation_enabled", false, true);
	par->addParam("mandelbox_fold_mode", true, true);
	par->addParam("mandelbox_solid", 1.0, true);
	par->addParam("mandelbox_melt", 0.0, true);

	par->addParam("mandelbox_vary_scale_vary", 0.1, true);
	par->addParam("mandelbox_vary_fold", 1.0, true);
	par->addParam("mandelbox_vary_minr", 0.5, true);
	par->addParam("mandelbox_vary_rpower", 1.0, true);
	par->addParam("mandelbox_vary_wadd", 0.0, true);


	//FoldingIntPow
	par->addParam("foldingIntPow_folding_factor", 2.0, true);
	par->addParam("foldingIntPow_z_factor", 5.0, true);
	WriteLog("Fractal parameters initialization finished");
}

void PutParamsToStruct(parameters::container *container, sParamRender *params)
{
	params->analitycDEMode = container->Get<bool>("analityc_DE_mode");
	params->constantFactor = container->Get<double>("fractal_constant_factor");
	params->DEFactor = container->Get<double>("DE_factor");
	params->DEThresh = container->Get<double>("DE_thresh");
	params->fishEyeCut = container->Get<bool>("fish_eye_180cut");
	params->fov = container->Get<double>("fov");
	params->imageHeight = container->Get<int>("image_height");
	params->imageWidth = container->Get<int>("image_width");
	params->iterThreshMode = container->Get<bool>("iteration_threshold_mode");
	params->juliaC = container->Get<CVector3>("julia_c");
	params->juliaMode = container->Get<bool>("julia_mode");
	params->limitMax = container->Get<CVector3>("limit_max");
	params->limitMin = container->Get<CVector3>("limit_min");
	params->limitsEnabled = container->Get<bool>("limits_enabled");
	params->minN = container->Get<int>("minN");
	params->N = container->Get<int>("N");
	params->noOfTiles = container->Get<int>("tiles");
	params->perspectiveType = (params::enumPerspectiveType)container->Get<int>("perspective_type");
	params->slowShading = container->Get<bool>("slow_shading");
	params->smoothness = container->Get<double>("smoothness");
	params->target = container->Get<CVector3>("target");
	params->tileNumber = container->Get<int>("tile_number");
	params->viewAngle = container->Get<CVector3>("view_angle");
	params->viewDistanceMax = container->Get<double>("view_distance_max");
	params->viewDistanceMin = container->Get<double>("view_distance_min");
	params->vp = container->Get<CVector3>("view_point");
	params->zoom = container->Get<double>("zoom");
	params->interiorMode = container->Get<bool>("limits_enabled");
	params->linearDEmode = container->Get<bool>("linear_DE_mode");
	params->constantDEThreshold = container->Get<bool>("constant_DE_threshold");
	params->imageAdjustments.brightness = container->Get<double>("brightness");
	params->imageAdjustments.contrast = container->Get<double>("contrast");
	params->imageAdjustments.imageGamma = container->Get<double>("gamma");
	params->imageAdjustments.hdrEnabled = container->Get<bool>("hdr");
	params->ambient = container->Get<double>("ambient");
	params->reflect = container->Get<double>("reflect");
	params->ambientOcclusion = container->Get<double>("ambient_occlusion");
	params->ambientOcclusionQuality = container->Get<int>("ambient_occlusion_quality");
	params->ambientOcclusionFastTune = container->Get<double>("ambient_occlusion_fast_tune");
	params->ambientOcclusionEnabled = container->Get<bool>("ambient_occlusion_enabled");
	params->ambientOcclusionMode = (params::enumAOMode)container->Get<int>("ambient_occlusion_quality");
	params->shading = container->Get<double>("shading");
	params->specular = container->Get<double>("specular");
	params->glowEnabled = container->Get<bool>("glow_enabled");
	params->glowIntensity = container->Get<double>("glow_intensity");
	params->texturedBackground = container->Get<bool>("textured_background");
	params->texturedBackgroundMapType = (params::enumTextureMapType)container->Get<int>("textured_background_map_type");
	params->shadow = container->Get<bool>("shadows_enabled");
	params->penetratingLights = container->Get<bool>("penetrating_lights");
	params->raytracedReflections = container->Get<bool>("raytraced_reflections");
	params->reflectionsMax = container->Get<int>("reflections_max");
	params->glowColor1 = container->Get<sRGB>("glow_color", 1);
	params->glowColor2 = container->Get<sRGB>("glow_color", 2);
	params->background_color1 = container->Get<sRGB>("background_color", 1);
	params->background_color2 = container->Get<sRGB>("background_color", 2);
	params->background_color3 = container->Get<sRGB>("background_color", 3);
	params->volFogColour1 = container->Get<sRGB>("fog_color", 1);
	params->volFogColour2 = container->Get<sRGB>("fog_color", 2);
	params->volFogColour3 = container->Get<sRGB>("fog_color", 3);
	params->volFogEnabled = container->Get<bool>("volumetric_fog_enabled");
	params->volFogDensity = container->Get<double>("volumetric_fog_density");
	params->volFogColour1Distance = container->Get<double>("volumetric_fog_colour_1_distance");
	params->volFogColour2Distance = container->Get<double>("volumetric_fog_colour_2_distance");
	params->volFogDistanceFactor = container->Get<double>("volumetric_fog_distance_factor");
	params->iterFogEnabled = container->Get<bool>("iteration_fog_enable");
	params->iterFogOpacity = container->Get<double>("iteration_fog_opacity");
	params->iterFogOpacityTrim = container->Get<double>("iteration_fog_opacity_trim");
	params->iterFogColor1Maxiter = container->Get<double>("iteration_fog_color_1_maxiter");
	params->iterFogColor2Maxiter = container->Get<double>("iteration_fog_color_2_maxiter");
	params->iterFogColour1 = container->Get<sRGB>("iteration_fog_color", 1);
	params->iterFogColour1 = container->Get<sRGB>("iteration_fog_color", 2);
	params->iterFogColour1 = container->Get<sRGB>("iteration_fog_color", 3);
	params->coloringEnabled = container->Get<bool>("fractal_color");
	params->coloring_seed = container->Get<int>("coloring_random_seed");
	params->colourSaturation = container->Get<double>("coloring_saturation");
	params->coloring_speed = container->Get<double>("coloring_speed");
	params->paletteOffset = container->Get<double>("coloring_palette_offset");
	params->fogEnabled = container->Get<bool>("basic_fog_enabled");
	params->fogVisibility = container->Get<double>("basic_fog_visibility");
	params->fogColor = container->Get<sRGB>("basic_fog_color");
	params->SSAOEnabled = container->Get<bool>("post_SSAO_enabled");
	params->DOFEnabled = container->Get<bool>("DOF_enabled");
	params->DOFFocus = container->Get<double>("DOF_focus");
	params->DOFRadius = container->Get<double>("DOF_radius");
	params->mainLightIntensity = container->Get<double>("main_light_intensity");
	params->mainLightVisibility = container->Get<double>("main_light_visibility");
	params->mainLightVisibilitySize = container->Get<double>("main_light_visibility_size");
	params->mainLightAlpha = container->Get<double>("main_light_alpha");
	params->mainLightBeta = container->Get<double>("main_light_beta");
	params->mainLightColour = container->Get<sRGB>("main_light_colour");
	params->shadowConeAngle = container->Get<double>("shadows_cone_angle");
	params->mainLightEnable = container->Get<bool>("main_light_enable");
	params->mainLightPositionAsRelative = container->Get<bool>("main_light_position_relative");

	for(int i = 0; i < 4; ++i)
	{
		params->auxLightPre[i] = container->Get<CVector3>("view_point", i + 1);
		params->auxLightPreIntensity[i] = container->Get<double>("aux_light_predefined_intensity", i + 1);
		params->auxLightPreEnabled[i] = container->Get<bool>("aux_light_predefined_enabled", i + 1);
		params->auxLightPreColour[i] = container->Get<sRGB>("aux_light_predefined_colour", i + 1);
	}

	for(int i = 0; i < 5; i++)
	{
		params->volumetricLightIntensity[i] = container->Get<double>("volumetric_light_intensity", i);
		params->volumetricLightEnabled[i] = container->Get<double>("volumetric_light_enabled", i);
	}

	params->fakeLightsEnabled = container->Get<double>("fake_lights_enabled");
	params->fakeLightsIntensity = container->Get<double>("fake_lights_intensity");
	params->fakeLightsVisibility = container->Get<double>("fake_lights_visibility");
	params->fakeLightsVisibilitySize = container->Get<double>("fake_lights_visibility_size");
	params->fakeLightsOrbitTrap = container->Get<CVector3>("fake_lights_orbit_trap");
	params->fakeLightsMinIter = container->Get<double>("fake_lights_min_iter");
	params->fakeLightsMaxIter = container->Get<double>("fake_lights_max_iter");

	params->primitives.plane.onlyPlane = container->Get<bool>("primitive_only_plane");
	params->primitives.plane.enable = container->Get<bool>("primitive_plane_enabled");
	params->primitives.plane.centre = container->Get<CVector3>("primitive_plane_centre");
	params->primitives.plane.normal = container->Get<CVector3>("primitive_plane_normal");
	params->primitives.plane.colour = container->Get<sRGB>("primitive_plane_colour");
	params->primitives.plane.reflect = container->Get<double>("primitive_plane_reflect");

	params->primitives.box.enable = container->Get<bool>("primitive_box_enabled");
	params->primitives.box.centre = container->Get<CVector3>("primitive_box_centre");
	params->primitives.box.size = container->Get<CVector3>("primitive_box_size");
	params->primitives.box.colour = container->Get<sRGB>("primitive_box_colour");
	params->primitives.box.reflect = container->Get<double>("primitive_box_reflect");

	params->primitives.invertedBox.enable = container->Get<bool>("primitive_invertedBox_enabled");
	params->primitives.invertedBox.centre = container->Get<CVector3>("primitive_invertedBox_centre");
	params->primitives.invertedBox.size = container->Get<CVector3>("primitive_invertedBox_size");
	params->primitives.invertedBox.colour = container->Get<sRGB>("primitive_invertedBox_colour");
	params->primitives.invertedBox.reflect = container->Get<double>("primitive_invertedBox_reflect");

	params->primitives.sphere.enable = container->Get<bool>("primitive_sphere_enabled");
	params->primitives.sphere.centre = container->Get<CVector3>("primitive_sphere_centre");
	params->primitives.sphere.radius = container->Get<double>("primitive_sphere_radius");
	params->primitives.sphere.colour = container->Get<sRGB>("primitive_sphere_colour");
	params->primitives.sphere.reflect = container->Get<double>("primitive_sphere_reflect");

	params->primitives.invertedSphere.enable = container->Get<bool>("primitive_invertedSphere_enabled");
	params->primitives.invertedSphere.centre = container->Get<CVector3>("primitive_invertedSphere_centre");
	params->primitives.invertedSphere.radius = container->Get<double>("primitive_invertedSphere_radius");
	params->primitives.invertedSphere.colour = container->Get<sRGB>("primitive_invertedSphere_colour");
	params->primitives.invertedSphere.reflect = container->Get<double>("primitive_invertedSphere_reflect");

	params->primitives.water.enable = container->Get<bool>("primitive_water_enabled");
	params->primitives.water.level = container->Get<double>("primitive_water_level");
	params->primitives.water.amplitude = container->Get<double>("primitive_water_amplitude");
	params->primitives.water.length = container->Get<double>("primitive_water_length");
	params->primitives.water.iterations = container->Get<int>("primitive_water_iterations");
	params->primitives.water.rotation = container->Get<double>("primitive_water_rotation");
	params->primitives.water.animSpeed = container->Get<double>("primitive_water_anim_speed");
	params->primitives.water.colour = container->Get<sRGB>("primitive_water_colour");
	params->primitives.water.reflect = container->Get<double>("primitive_water_reflect");

	//params->formula = Get<int>("tile_number");
}
