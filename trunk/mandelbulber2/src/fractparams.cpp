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

cParamRender::cParamRender(const cParameterContainer *container)
{
  analitycDEMode = container->Get<bool>("analityc_DE_mode");
	constantFactor = container->Get<double>("fractal_constant_factor");
	DEFactor = container->Get<double>("DE_factor");
	DEThresh = container->Get<double>("DE_thresh");
	detailLevel = container->Get<double>("detail_level");
	fov = container->Get<double>("fov");
	imageHeight = container->Get<int>("image_height");
	imageWidth = container->Get<int>("image_width");
	iterThreshMode = container->Get<bool>("iteration_threshold_mode");
	juliaC = container->Get<CVector3>("julia_c");
	juliaMode = container->Get<bool>("julia_mode");
	hybridFractalEnable = container->Get<bool>("hybrid_fractal_enable");
	limitMax = container->Get<CVector3>("limit_max");
	limitMin = container->Get<CVector3>("limit_min");
	limitsEnabled = container->Get<bool>("limits_enabled");
	minN = container->Get<int>("minN");
	N = container->Get<int>("N");
	perspectiveType = (params::enumPerspectiveType)container->Get<int>("perspective_type");
	slowShading = container->Get<bool>("slow_shading");
	smoothness = container->Get<double>("smoothness");
	target = container->Get<CVector3>("target");
	viewDistanceMax = container->Get<double>("view_distance_max");
	viewDistanceMin = container->Get<double>("view_distance_min");
	camera = container->Get<CVector3>("camera");
	target = container->Get<CVector3>("target");
	viewAngle = container->Get<CVector3>("camera_rotation");
	topVector = container->Get<CVector3>("camera_top");
	cameraDistanceToTarget = container->Get<double>("camera_distance_to_target");
	interiorMode = container->Get<bool>("interior_mode");
	linearDEmode = container->Get<bool>("linear_DE_mode");
	constantDEThreshold = container->Get<bool>("constant_DE_threshold");
	imageAdjustments.brightness = container->Get<double>("brightness");
	imageAdjustments.contrast = container->Get<double>("contrast");
	imageAdjustments.imageGamma = container->Get<double>("gamma");
	imageAdjustments.hdrEnabled = container->Get<bool>("hdr");
	reflect = container->Get<double>("reflect");
	ambientOcclusion = container->Get<double>("ambient_occlusion");
	ambientOcclusionQuality = container->Get<int>("ambient_occlusion_quality");
	ambientOcclusionFastTune = container->Get<double>("ambient_occlusion_fast_tune");
	ambientOcclusionEnabled = container->Get<bool>("ambient_occlusion_enabled");
	ambientOcclusionMode = (params::enumAOMode)container->Get<int>("ambient_occlusion_mode");
	shading = container->Get<double>("shading");
	specular = container->Get<double>("specular");
	glowEnabled = container->Get<bool>("glow_enabled");
	glowIntensity = container->Get<double>("glow_intensity");
	texturedBackground = container->Get<bool>("textured_background");
	texturedBackgroundMapType = (params::enumTextureMapType)container->Get<int>("textured_background_map_type");
	shadow = container->Get<bool>("shadows_enabled");
	penetratingLights = container->Get<bool>("penetrating_lights");
	raytracedReflections = container->Get<bool>("raytraced_reflections");
	reflectionsMax = container->Get<int>("reflections_max");
	envMappingEnable = container->Get<bool>("env_mapping_enable");
	glowColor1 = container->Get<sRGB>("glow_color", 1);
	glowColor2 = container->Get<sRGB>("glow_color", 2);
	background_color1 = container->Get<sRGB>("background_color", 1);
	background_color2 = container->Get<sRGB>("background_color", 2);
	background_color3 = container->Get<sRGB>("background_color", 3);
	volFogColour1 = container->Get<sRGB>("fog_color", 1);
	volFogColour2 = container->Get<sRGB>("fog_color", 2);
	volFogColour3 = container->Get<sRGB>("fog_color", 3);
	volFogEnabled = container->Get<bool>("volumetric_fog_enabled");
	volFogDensity = container->Get<double>("volumetric_fog_density");
	volFogColour1Distance = container->Get<double>("volumetric_fog_colour_1_distance");
	volFogColour2Distance = container->Get<double>("volumetric_fog_colour_2_distance");
	volFogDistanceFactor = container->Get<double>("volumetric_fog_distance_factor");
	iterFogEnabled = container->Get<bool>("iteration_fog_enable");
	iterFogOpacity = container->Get<double>("iteration_fog_opacity");
	iterFogOpacityTrim = container->Get<double>("iteration_fog_opacity_trim");
	iterFogColor1Maxiter = container->Get<double>("iteration_fog_color_1_maxiter");
	iterFogColor2Maxiter = container->Get<double>("iteration_fog_color_2_maxiter");
	iterFogColour1 = container->Get<sRGB>("iteration_fog_color", 1);
	iterFogColour1 = container->Get<sRGB>("iteration_fog_color", 2);
	iterFogColour1 = container->Get<sRGB>("iteration_fog_color", 3);
	coloringEnabled = container->Get<bool>("fractal_color");
	coloring_seed = container->Get<int>("coloring_random_seed");
	colourSaturation = container->Get<double>("coloring_saturation");
	coloring_speed = container->Get<double>("coloring_speed");
	paletteOffset = container->Get<double>("coloring_palette_offset");
	fogEnabled = container->Get<bool>("basic_fog_enabled");
	fogVisibility = container->Get<double>("basic_fog_visibility");
	fogColor = container->Get<sRGB>("basic_fog_color");
	DOFEnabled = container->Get<bool>("DOF_enabled");
	DOFFocus = container->Get<double>("DOF_focus");
	DOFRadius = container->Get<double>("DOF_radius");
	mainLightIntensity = container->Get<double>("main_light_intensity");
	mainLightVisibility = container->Get<double>("main_light_visibility");
	mainLightVisibilitySize = container->Get<double>("main_light_visibility_size");
	mainLightAlpha = container->Get<double>("main_light_alpha");
	mainLightBeta = container->Get<double>("main_light_beta");
	mainLightColour = container->Get<sRGB>("main_light_colour");
	shadowConeAngle = container->Get<double>("shadows_cone_angle");
	mainLightEnable = container->Get<bool>("main_light_enable");
	mainLightPositionAsRelative = container->Get<bool>("main_light_position_relative");
	resolution = 0.0;

	for(int i = 0; i < 4; ++i)
	{
		auxLightPre[i] = container->Get<CVector3>("aux_light_predefined_position", i + 1);
		auxLightPreIntensity[i] = container->Get<double>("aux_light_predefined_intensity", i + 1);
		auxLightPreEnabled[i] = container->Get<bool>("aux_light_predefined_enabled", i + 1);
		auxLightPreColour[i] = container->Get<sRGB>("aux_light_predefined_colour", i + 1);
	}

	for(int i = 0; i < 5; i++)
	{
		volumetricLightIntensity[i] = container->Get<double>("volumetric_light_intensity", i);
		volumetricLightEnabled[i] = container->Get<double>("volumetric_light_enabled", i);
	}

	fakeLightsEnabled = container->Get<double>("fake_lights_enabled");
	fakeLightsIntensity = container->Get<double>("fake_lights_intensity");
	fakeLightsVisibility = container->Get<double>("fake_lights_visibility");
	fakeLightsVisibilitySize = container->Get<double>("fake_lights_visibility_size");
	fakeLightsOrbitTrap = container->Get<CVector3>("fake_lights_orbit_trap");
	fakeLightsMinIter = container->Get<double>("fake_lights_min_iter");
	fakeLightsMaxIter = container->Get<double>("fake_lights_max_iter");

	frameNo = container->Get<int>("frame_no");

	primitives.plane.onlyPlane = container->Get<bool>("primitive_only_plane");
	primitives.plane.enable = container->Get<bool>("primitive_plane_enabled");
	primitives.plane.centre = container->Get<CVector3>("primitive_plane_centre");
	primitives.plane.normal = container->Get<CVector3>("primitive_plane_normal");
	primitives.plane.colour = container->Get<sRGB>("primitive_plane_colour");
	primitives.plane.reflect = container->Get<double>("primitive_plane_reflect");

	primitives.box.enable = container->Get<bool>("primitive_box_enabled");
	primitives.box.centre = container->Get<CVector3>("primitive_box_centre");
	primitives.box.size = container->Get<CVector3>("primitive_box_size");
	primitives.box.colour = container->Get<sRGB>("primitive_box_colour");
	primitives.box.reflect = container->Get<double>("primitive_box_reflect");

	primitives.invertedBox.enable = container->Get<bool>("primitive_invertedBox_enabled");
	primitives.invertedBox.centre = container->Get<CVector3>("primitive_invertedBox_centre");
	primitives.invertedBox.size = container->Get<CVector3>("primitive_invertedBox_size");
	primitives.invertedBox.colour = container->Get<sRGB>("primitive_invertedBox_colour");
	primitives.invertedBox.reflect = container->Get<double>("primitive_invertedBox_reflect");

	primitives.sphere.enable = container->Get<bool>("primitive_sphere_enabled");
	primitives.sphere.centre = container->Get<CVector3>("primitive_sphere_centre");
	primitives.sphere.radius = container->Get<double>("primitive_sphere_radius");
	primitives.sphere.colour = container->Get<sRGB>("primitive_sphere_colour");
	primitives.sphere.reflect = container->Get<double>("primitive_sphere_reflect");

	primitives.invertedSphere.enable = container->Get<bool>("primitive_invertedSphere_enabled");
	primitives.invertedSphere.centre = container->Get<CVector3>("primitive_invertedSphere_centre");
	primitives.invertedSphere.radius = container->Get<double>("primitive_invertedSphere_radius");
	primitives.invertedSphere.colour = container->Get<sRGB>("primitive_invertedSphere_colour");
	primitives.invertedSphere.reflect = container->Get<double>("primitive_invertedSphere_reflect");

	primitives.water.enable = container->Get<bool>("primitive_water_enabled");
	primitives.water.level = container->Get<double>("primitive_water_level");
	primitives.water.amplitude = container->Get<double>("primitive_water_amplitude");
	primitives.water.length = container->Get<double>("primitive_water_length");
	primitives.water.iterations = container->Get<int>("primitive_water_iterations");
	primitives.water.rotation = container->Get<double>("primitive_water_rotation");
	primitives.water.animSpeed = container->Get<double>("primitive_water_anim_speed");
	primitives.water.colour = container->Get<sRGB>("primitive_water_colour");
	primitives.water.reflect = container->Get<double>("primitive_water_reflect");

	auxLightDistributionRadius = container->Get<double>("aux_light_distribution_radius");
	auxLightIntensity = container->Get<double>("aux_light_intensity");
	auxLightNumber = container->Get<int>("aux_light_number");
	auxLightMaxDist = container->Get<double>("aux_light_max_dist");
	auxLightVisibility = container->Get<double>("aux_light_visibility");
	auxLightRandomSeed = container->Get<int>("aux_light_random_seed");

	//formula = Get<int>("tile_number");
}
