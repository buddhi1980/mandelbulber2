/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cParamRender class - container for scene parameters
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

cParamRender::cParamRender(const cParameterContainer *container) : primitives(container)
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
	common.juliaC = container->Get<CVector3>("julia_c");
	common.juliaMode = container->Get<bool>("julia_mode");
	common.constantMultiplier = container->Get<double>("fractal_constant_factor");
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
	iterFogColour2 = container->Get<sRGB>("iteration_fog_color", 2);
	iterFogColour3 = container->Get<sRGB>("iteration_fog_color", 3);
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
		auxLightPre[i] = container->Get<CVector3>("aux_light_position", i + 1);
		auxLightPreIntensity[i] = container->Get<double>("aux_light_intensity", i + 1);
		auxLightPreEnabled[i] = container->Get<bool>("aux_light_enabled", i + 1);
		auxLightPreColour[i] = container->Get<sRGB>("aux_light_colour", i + 1);
	}

	volumetricLightIntensity[0] = container->Get<double>("main_light_volumetric_intensity");
	volumetricLightEnabled[0] = container->Get<double>("main_light_volumetric_enabled");
	for(int i = 1; i <= 4; i++)
	{
		volumetricLightIntensity[i] = container->Get<double>("aux_light_volumetric_intensity", i);
		volumetricLightEnabled[i] = container->Get<double>("aux_light_volumetric_enabled", i);
	}

	auxLightNumber = 0;
	auxLightVisibility = container->Get<double>("aux_light_visibility");
	auxLightVisibilitySize = container->Get<double>("aux_light_visibility_size");

	fakeLightsEnabled = container->Get<double>("fake_lights_enabled");
	fakeLightsIntensity = container->Get<double>("fake_lights_intensity");
	fakeLightsVisibility = container->Get<double>("fake_lights_visibility");
	fakeLightsVisibilitySize = container->Get<double>("fake_lights_visibility_size");
	common.fakeLightsOrbitTrap = container->Get<CVector3>("fake_lights_orbit_trap");
	common.fakeLightsMinIter = container->Get<double>("fake_lights_min_iter");
	common.fakeLightsMaxIter = container->Get<double>("fake_lights_max_iter");

	frameNo = container->Get<int>("frame_no");

	legacyCoordinateSystem = container->Get<bool>("legacy_coordinate_system");

	//formula = Get<int>("tile_number");
}
