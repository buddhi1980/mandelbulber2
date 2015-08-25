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
	ambientOcclusion = container->Get<double>("ambient_occlusion");
	ambientOcclusionEnabled = container->Get<bool>("ambient_occlusion_enabled");
	ambientOcclusionFastTune = container->Get<double>("ambient_occlusion_fast_tune");
	ambientOcclusionMode = (params::enumAOMode)container->Get<int>("ambient_occlusion_mode");
	ambientOcclusionQuality = container->Get<int>("ambient_occlusion_quality");
  analitycDEMode = container->Get<bool>("analityc_DE_mode");
	auxLightNumber = 0;
	auxLightVisibility = container->Get<double>("aux_light_visibility");
	auxLightVisibilitySize = container->Get<double>("aux_light_visibility_size");
	background_color1 = container->Get<sRGB>("background_color", 1);
	background_color2 = container->Get<sRGB>("background_color", 2);
	background_color3 = container->Get<sRGB>("background_color", 3);
	booleanOperatorsEnabled = container->Get<bool>("boolean_operators");
	camera = container->Get<CVector3>("camera");
	cameraDistanceToTarget = container->Get<double>("camera_distance_to_target");
	coloring_seed = container->Get<int>("coloring_random_seed");
	coloring_speed = container->Get<double>("coloring_speed");
	coloringEnabled = container->Get<bool>("fractal_color");
	colourSaturation = container->Get<double>("coloring_saturation");
	constantDEThreshold = container->Get<bool>("constant_DE_threshold");
	constantFactor = container->Get<double>("fractal_constant_factor");
	DEFactor = container->Get<double>("DE_factor");
	detailLevel = container->Get<double>("detail_level");
	DEThresh = container->Get<double>("DE_thresh");
	DOFEnabled = container->Get<bool>("DOF_enabled");
	DOFFocus = container->Get<double>("DOF_focus");
	DOFRadius = container->Get<double>("DOF_radius");
	envMappingEnable = container->Get<bool>("env_mapping_enable");
	fakeLightsEnabled = container->Get<double>("fake_lights_enabled");
	fakeLightsIntensity = container->Get<double>("fake_lights_intensity");
	fakeLightsVisibility = container->Get<double>("fake_lights_visibility");
	fakeLightsVisibilitySize = container->Get<double>("fake_lights_visibility_size");
	fogColor = container->Get<sRGB>("basic_fog_color");
	fogEnabled = container->Get<bool>("basic_fog_enabled");
	fogVisibility = container->Get<double>("basic_fog_visibility");
	fov = container->Get<double>("fov");
	frameNo = container->Get<int>("frame_no");
	fresnelReflectance = container->Get<bool>("fresnel_reflectance");
	glowColor1 = container->Get<sRGB>("glow_color", 1);
	glowColor2 = container->Get<sRGB>("glow_color", 2);
	glowEnabled = container->Get<bool>("glow_enabled");
	glowIntensity = container->Get<double>("glow_intensity");
	hybridFractalEnable = container->Get<bool>("hybrid_fractal_enable");
	imageAdjustments.brightness = container->Get<double>("brightness");
	imageAdjustments.contrast = container->Get<double>("contrast");
	imageAdjustments.hdrEnabled = container->Get<bool>("hdr");
	imageAdjustments.imageGamma = container->Get<double>("gamma");
	imageHeight = container->Get<int>("image_height");
	imageWidth = container->Get<int>("image_width");
	interiorMode = container->Get<bool>("interior_mode");
	iterFogColor1Maxiter = container->Get<double>("iteration_fog_color_1_maxiter");
	iterFogColor2Maxiter = container->Get<double>("iteration_fog_color_2_maxiter");
	iterFogColour1 = container->Get<sRGB>("iteration_fog_color", 1);
	iterFogColour2 = container->Get<sRGB>("iteration_fog_color", 2);
	iterFogColour3 = container->Get<sRGB>("iteration_fog_color", 3);
	iterFogEnabled = container->Get<bool>("iteration_fog_enable");
	iterFogOpacity = container->Get<double>("iteration_fog_opacity");
	iterFogOpacityTrim = container->Get<double>("iteration_fog_opacity_trim");
	iterThreshMode = container->Get<bool>("iteration_threshold_mode");
	legacyCoordinateSystem = container->Get<bool>("legacy_coordinate_system");
	limitMax = container->Get<CVector3>("limit_max");
	limitMin = container->Get<CVector3>("limit_min");
	limitsEnabled = container->Get<bool>("limits_enabled");
	linearDEmode = container->Get<bool>("linear_DE_mode");
	mainLightAlpha = container->Get<double>("main_light_alpha");
	mainLightBeta = container->Get<double>("main_light_beta");
	mainLightColour = container->Get<sRGB>("main_light_colour");
	mainLightEnable = container->Get<bool>("main_light_enable");
	mainLightIntensity = container->Get<double>("main_light_intensity");
	mainLightPositionAsRelative = container->Get<bool>("main_light_position_relative");
	mainLightVisibility = container->Get<double>("main_light_visibility");
	mainLightVisibilitySize = container->Get<double>("main_light_visibility_size");
	minN = container->Get<int>("minN");
	N = container->Get<int>("N");
	paletteOffset = container->Get<double>("coloring_palette_offset");
	penetratingLights = container->Get<bool>("penetrating_lights");
	perspectiveType = (params::enumPerspectiveType)container->Get<int>("perspective_type");
	raytracedReflections = container->Get<bool>("raytraced_reflections");
	reflect = container->Get<double>("reflect");
	reflectionsMax = container->Get<int>("reflections_max");
	resolution = 0.0;
	shading = container->Get<double>("shading");
	shadow = container->Get<bool>("shadows_enabled");
	shadowConeAngle = container->Get<double>("shadows_cone_angle");
	slowShading = container->Get<bool>("slow_shading");
	smoothness = container->Get<double>("smoothness");
	specular = container->Get<double>("specular");
	target = container->Get<CVector3>("target");
	target = container->Get<CVector3>("target");
	texturedBackground = container->Get<bool>("textured_background");
	texturedBackgroundMapType = (params::enumTextureMapType)container->Get<int>("textured_background_map_type");
	topVector = container->Get<CVector3>("camera_top");
	transparencyIndexOfRefraction = container->Get<double>("transparency_index_of_refraction");
	transparencyInteriorColor = container->Get<sRGB>("transparency_interior_color");
	transparencyOfInterior = container->Get<double>("transparency_of_interior");
	transparencyOfSurface = container->Get<double>("transparency_of_surface");
	viewAngle = container->Get<CVector3>("camera_rotation");
	viewDistanceMax = container->Get<double>("view_distance_max");
	viewDistanceMin = container->Get<double>("view_distance_min");
	volFogColour1 = container->Get<sRGB>("fog_color", 1);
	volFogColour1Distance = container->Get<double>("volumetric_fog_colour_1_distance");
	volFogColour2 = container->Get<sRGB>("fog_color", 2);
	volFogColour2Distance = container->Get<double>("volumetric_fog_colour_2_distance");
	volFogColour3 = container->Get<sRGB>("fog_color", 3);
	volFogDensity = container->Get<double>("volumetric_fog_density");
	volFogDistanceFactor = container->Get<double>("volumetric_fog_distance_factor");
	volFogEnabled = container->Get<bool>("volumetric_fog_enabled");
	volumetricLightEnabled[0] = container->Get<double>("main_light_volumetric_enabled");
	volumetricLightIntensity[0] = container->Get<double>("main_light_volumetric_intensity");

	for(int i = 0; i < 4; ++i)
	{
		auxLightPre[i] = container->Get<CVector3>("aux_light_position", i + 1);
		auxLightPreIntensity[i] = container->Get<double>("aux_light_intensity", i + 1);
		auxLightPreEnabled[i] = container->Get<bool>("aux_light_enabled", i + 1);
		auxLightPreColour[i] = container->Get<sRGB>("aux_light_colour", i + 1);
	}

	for(int i = 1; i <= 4; i++)
	{
		volumetricLightIntensity[i] = container->Get<double>("aux_light_volumetric_intensity", i);
		volumetricLightEnabled[i] = container->Get<double>("aux_light_volumetric_enabled", i);
	}

	volumetricLightAnyEnabled = false;
	for(int i = 0; i <=4; i++)
	{
		if(volumetricLightEnabled[i]) volumetricLightAnyEnabled = true;
	}

	for(int i = 0; i < 3; i++)
	{
		booleanOperator[i] = (params::enumBooleanOperator)container->Get<int>("boolean_operator", i + 1);
	}

	for(int i = 0; i < 4; i++)
	{
		formulaPosition[i] = container->Get<CVector3>("formula_position", i + 1);
		formulaRotation[i] = container->Get<CVector3>("formula_rotation", i + 1);
		formulaRepeat[i] = container->Get<CVector3>("formula_repeat", i + 1);
		formulaScale[i] = 1.0 / container->Get<double>("formula_scale", i + 1);
		mRotFormulaRotation[i].SetRotation2(formulaRotation[i] * (M_PI / 180.0));
	}

	common.constantMultiplier = container->Get<CVector3>("fractal_constant_factor");
	common.fakeLightsMaxIter = container->Get<double>("fake_lights_max_iter");
	common.fakeLightsMinIter = container->Get<double>("fake_lights_min_iter");
	common.fakeLightsOrbitTrap = container->Get<CVector3>("fake_lights_orbit_trap");
	common.foldings.boxEnable = container->Get<bool>("box_folding");
	common.foldings.boxLimit = container->Get<double>("box_folding_limit");
	common.foldings.boxValue = container->Get<double>("box_folding_value");
	common.foldings.sphericalEnable = container->Get<bool>("spherical_folding");
	common.foldings.sphericalInner = container->Get<double>("spherical_folding_inner");
	common.foldings.sphericalOuter = container->Get<double>("spherical_folding_outer");
	common.fractalPosition = container->Get<CVector3>("fractal_position");
	common.fractalRotation = container->Get<CVector3>("fractal_rotation");
	common.juliaC = container->Get<CVector3>("julia_c");
	common.juliaMode = container->Get<bool>("julia_mode");
	common.mRotFractalRotation.SetRotation2(common.fractalRotation / 180.0 * M_PI);
	common.repeat = container->Get<CVector3>("repeat");
	common.bailout = container->Get<double>("bailout");

	//formula = Get<int>("tile_number");
}


