/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * sParamRender struct - container for scene parameters
 */

#include "fractparams.hpp"

#include "object_data.hpp"
#include "parameters.hpp"

sParamRender::sParamRender(
	const std::shared_ptr<cParameterContainer> container, QVector<cObjectData> *objectData)
		: primitives(container, objectData)
{
	advancedQuality = container->Get<bool>("advanced_quality");
	absMaxMarchingStep = container->Get<double>("abs_max_marching_step");
	absMinMarchingStep = container->Get<double>("abs_min_marching_step");
	allPrimitivesInvisibleAlpha = container->Get<bool>("all_primitives_invisible_alpha");
	antialiasingAdaptive = container->Get<bool>("antialiasing_adaptive");
	antialiasingEnabled = container->Get<bool>("antialiasing_enabled");
	antialiasingOclDepth = container->Get<int>("antialiasing_ocl_depth");
	antialiasingSize = container->Get<int>("antialiasing_size");
	ambientOcclusion = container->Get<float>("ambient_occlusion");
	ambientOcclusionEnabled = container->Get<bool>("ambient_occlusion_enabled");
	ambientOcclusionColor = toRGBFloat(container->Get<sRGB>("ambient_occlusion_color"));
	ambientOcclusionFastTune = container->Get<double>("ambient_occlusion_fast_tune");
	ambientOcclusionMode = params::enumAOMode(container->Get<int>("ambient_occlusion_mode"));
	ambientOcclusionQuality = container->Get<int>("ambient_occlusion_quality");
	ambientOcclusionLightMapRotation =
		container->Get<CVector3>("ao_light_map_rotation") / 180.0 * M_PI;
	mRotAmbientOcclusionLightMapRotation.SetRotation(ambientOcclusionLightMapRotation);
	background3ColorsEnable = container->Get<bool>("background_3_colors_enable");
	background_color1 = toRGBFloat(container->Get<sRGB>("background_color", 1));
	background_color2 = toRGBFloat(container->Get<sRGB>("background_color", 2));
	background_color3 = toRGBFloat(container->Get<sRGB>("background_color", 3));
	background_brightness = container->Get<double>("background_brightness");
	background_gamma = container->Get<double>("background_gamma");
	backgroundHScale = container->Get<double>("background_h_scale");
	backgroundVScale = container->Get<double>("background_v_scale");
	backgroundTextureOffsetX = container->Get<double>("background_texture_offset_x");
	backgroundTextureOffsetY = container->Get<double>("background_texture_offset_y");
	backgroundVScale = container->Get<double>("background_v_scale");
	backgroundRotation = container->Get<CVector3>("background_rotation");
	booleanOperatorsEnabled = container->Get<bool>("boolean_operators");
	camera = container->Get<CVector3>("camera");
	cameraDistanceToTarget = container->Get<double>("camera_distance_to_target");
	cloudsAmbientLight = container->Get<double>("clouds_ambient_light");
	cloudsCastShadows = container->Get<bool>("clouds_cast_shadows");
	cloudsCenter = container->Get<CVector3>("clouds_center");
	cloudsColor = toRGBFloat(container->Get<sRGB>("clouds_color"));
	cloudsDEMultiplier = container->Get<double>("clouds_DE_multiplier");
	cloudsDensity = container->Get<double>("clouds_density");
	cloudsDEApproaching = container->Get<double>("clouds_DE_approaching");
	cloudsDetailAccuracy = container->Get<double>("clouds_detail_accuracy");
	cloudsDistance = container->Get<double>("clouds_distance");
	cloudsDistanceLayer = container->Get<double>("clouds_distance_layer");
	cloudsDistanceMode = container->Get<bool>("clouds_distance_mode");
	cloudsEnable = container->Get<bool>("clouds_enable");
	cloudsLightsBoost = container->Get<double>("clouds_lights_boost");
	cloudsPeriod = container->Get<double>("clouds_period");
	cloudsPlaneShape = container->Get<bool>("clouds_plane_shape");
	cloudsHeight = container->Get<double>("clouds_height");
	cloudsIterations = container->Get<int>("clouds_noise_iterations");
	cloudsOpacity = container->Get<double>("clouds_opacity");
	cloudsRandomSeed = container->Get<int>("clouds_random_seed");
	cloudsRotation = container->Get<CVector3>("clouds_rotation");
	cloudsSpeed = container->Get<CVector3>("clouds_speed");
	cloudsSharpEdges = container->Get<bool>("clouds_sharp_edges");
	cloudsSharpness = container->Get<double>("clouds_sharpness");
	constantDEThreshold = container->Get<bool>("constant_DE_threshold");
	constantFactor = container->Get<double>("fractal_constant_factor");
	DEFactor = container->Get<double>("DE_factor");
	delta_DE_function = fractal::enumDEFunctionType(container->Get<int>("delta_DE_function"));
	delta_DE_method = fractal::enumDEMethod(container->Get<int>("delta_DE_method"));
	deltaDERelativeDelta = container->Get<double>("deltade_relative_delta");
	detailLevel = container->Get<double>("detail_level");
	detailSizeMax = container->Get<double>("detail_size_max");
	detailSizeMin = container->Get<double>("detail_size_min");
	DEThresh = container->Get<double>("DE_thresh");
	distanceFogShadows = container->Get<bool>("distance_fog_shadows");
	DOFEnabled = container->Get<bool>("DOF_enabled");
	DOFFocus = container->Get<double>("DOF_focus");
	DOFRadius = container->Get<double>("DOF_radius");
	DOFMaxRadius = container->Get<double>("DOF_max_radius");
	DOFHDRMode = container->Get<bool>("DOF_HDR");
	DOFMonteCarlo = container->Get<bool>("DOF_monte_carlo");
	DOFMonteCarloGlobalIllumination = container->Get<bool>("DOF_MC_global_illumination");
	DOFNumberOfPasses = container->Get<int>("DOF_number_of_passes");
	DOFSamples = container->Get<int>("DOF_samples");
	DOFMinSamples = container->Get<int>("DOF_min_samples");
	DOFBlurOpacity = container->Get<double>("DOF_blur_opacity");
	DOFMaxNoise = container->Get<double>("DOF_max_noise");
	DOFMonteCarloChromaticAberration = container->Get<bool>("DOF_MC_CA_enable");
	DOFMonteCarloCADispersionGain = container->Get<float>("DOF_MC_CA_dispersion_gain");
	DOFMonteCarloCACameraDispersion = container->Get<float>("DOF_MC_CA_camera_dispersion");
	envMappingEnable = container->Get<bool>("env_mapping_enable");
	fakeLightsColor = toRGBFloat(container->Get<sRGB>("fake_lights_color"));
	fakeLightsColor2 = toRGBFloat(container->Get<sRGB>("fake_lights_color_2"));
	fakeLightsColor3 = toRGBFloat(container->Get<sRGB>("fake_lights_color_3"));
	fakeLightsEnabled = container->Get<bool>("fake_lights_enabled");
	fakeLightsIntensity = container->Get<double>("fake_lights_intensity");
	fakeLightsVisibility = container->Get<double>("fake_lights_visibility");
	fakeLightsVisibilitySize = container->Get<double>("fake_lights_visibility_size");
	fillLightColor = toRGBFloat(container->Get<sRGB>("fill_light_color"));
	fogColor = toRGBFloat(container->Get<sRGB>("basic_fog_color"));
	fogEnabled = container->Get<bool>("basic_fog_enabled");
	fogVisibility = container->Get<double>("basic_fog_visibility");
	fogCastShadows = container->Get<bool>("basic_fog_cast_shadows");
	perspectiveType = params::enumPerspectiveType(container->Get<int>("perspective_type"));
	fov = CalcFOV(container->Get<double>("fov"), perspectiveType);
	frameNo = container->Get<int>("frame_no");
	glowColor1 = toRGBFloat(container->Get<sRGB>("glow_color", 1));
	glowColor2 = toRGBFloat(container->Get<sRGB>("glow_color", 2));
	glowEnabled = container->Get<bool>("glow_enabled");
	glowIntensity = container->Get<float>("glow_intensity");
	hdrBlurEnabled = container->Get<bool>("hdr_blur_enabled");
	hdrBlurRadius = container->Get<double>("hdr_blur_radius");
	hdrBlurIntensity = container->Get<double>("hdr_blur_intensity");
	hybridFractalEnable = container->Get<bool>("hybrid_fractal_enable");
	imageAdjustments.brightness = container->Get<float>("brightness");
	imageAdjustments.contrast = container->Get<float>("contrast");
	imageAdjustments.hdrEnabled = container->Get<bool>("hdr");
	imageAdjustments.imageGamma = container->Get<float>("gamma");
	imageAdjustments.saturation = container->Get<float>("saturation");
	imageHeight = container->Get<int>("image_height");
	imageWidth = container->Get<int>("image_width");
	interiorMode = container->Get<bool>("interior_mode");
	iterFogBrightnessBoost = container->Get<float>("iteration_fog_brightness_boost");
	iterFogColor1Maxiter = container->Get<float>("iteration_fog_color_1_maxiter");
	iterFogColor2Maxiter = container->Get<float>("iteration_fog_color_2_maxiter");
	iterFogColour1 = toRGBFloat(container->Get<sRGB>("iteration_fog_color", 1));
	iterFogColour2 = toRGBFloat(container->Get<sRGB>("iteration_fog_color", 2));
	iterFogColour3 = toRGBFloat(container->Get<sRGB>("iteration_fog_color", 3));
	iterFogEnabled = container->Get<bool>("iteration_fog_enable");
	iterFogOpacity = container->Get<double>("iteration_fog_opacity");
	iterFogOpacityTrim = container->Get<float>("iteration_fog_opacity_trim");
	iterFogOpacityTrimHigh = container->Get<float>("iteration_fog_opacity_trim_high");
	iterFogShadows = container->Get<bool>("iteration_fog_shadows");
	legacyCoordinateSystem = container->Get<bool>("legacy_coordinate_system");
	limitMax = container->Get<CVector3>("limit_max");
	limitMin = container->Get<CVector3>("limit_min");
	limitsEnabled = container->Get<bool>("limits_enabled");
	maxRaymarchingSteps = container->Get<int>("max_raymarching_steps");
	minN = container->Get<int>("minN");
	monteCarloSoftShadows = container->Get<bool>("MC_soft_shadows_enable");
	monteCarloGIRadianceLimit = container->Get<float>("MC_GI_radiance_limit");
	monteCarloGIVolumetric = container->Get<bool>("MC_global_illumination_volumetric");
	monteCarloGIOfVolumetric = container->Get<bool>("MC_fog_illumination");
	monteCarloDenoiserEnable = container->Get<bool>("MC_denoiser_enable");
	monteCarloDenoiserStrength = container->Get<int>("MC_denoiser_strength");
	monteCarloDenoiserPreserveGeometry = container->Get<bool>("MC_denoiser_preserve_geometry");
	monteCarloPixelLevelOptimization = container->Get<bool>("MC_pixel_level_optimization");
	N = container->Get<int>("N");
	nebulaBrighness = container->Get<float>("nebula_brightness");
	nebulaConstantBrighness = container->Get<bool>("nebula_constant_brightness");
	nebulaColorMixing = container->Get<int>("nebula_color_mixing");
	nebulaNumberOfSamplesPerPixel = container->Get<int>("nebula_samples_per_pixel");
	nebulaMinIteration = container->Get<int>("nebula_min_iteration");
	nebulaInnerEnabled = container->Get<bool>("nebula_inner_enabled");
	nebulaOuterEnabled = container->Get<bool>("nebula_outer_enabled");
	nebulaXAxisColorsEnabled = container->Get<bool>("nebula_x_axis_colors_enabled");
	nebulaYAxisColorsEnabled = container->Get<bool>("nebula_y_axis_colors_enabled");
	nebulaZAxisColorsEnabled = container->Get<bool>("nebula_z_axis_colors_enabled");
	nebulaIterationsColorsEnabled = container->Get<bool>("nebula_iterations_colors_enabled");
	nebulaGridDomainEnabled = container->Get<bool>("nebula_grid_domain_enabled");
	nebulaXGridSize = container->Get<float>("nebula_x_grid_size");
	nebulaYGridSize = container->Get<float>("nebula_y_grid_size");
	nebulaZGridSize = container->Get<float>("nebula_z_grid_size");
	postChromaticAberrationEnabled = container->Get<bool>("post_chromatic_aberration_enabled");
	postChromaticAberrationRadius = container->Get<float>("post_chromatic_aberration_radius");
	postChromaticAberrationIntensity = container->Get<float>("post_chromatic_aberration_intensity");
	postChromaticAberrationReverse = container->Get<bool>("post_chromatic_aberration_reverse");
	rayleighScatteringBlue = container->Get<float>("rayleigh_scattering_blue");
	rayleighScatteringRed = container->Get<float>("rayleigh_scattering_red");
	raytracedReflections = container->Get<bool>("raytraced_reflections");
	reflectionsMax = container->Get<int>("reflections_max");
	relMaxMarchingStep = container->Get<double>("rel_max_marching_step");
	relMinMarchingStep = container->Get<double>("rel_min_marching_step");
	repeatFrom = container->Get<int>("repeat_from");
	resolution = 0.0;
	slowShading = container->Get<bool>("slow_shading");
	smoothness = container->Get<double>("smoothness");
	SSAO_random_mode = container->Get<bool>("SSAO_random_mode");
	stereoEyeDistance = container->Get<double>("stereo_eye_distance");
	stereoInfiniteCorrection = container->Get<double>("stereo_infinite_correction");
	stereoSwapEyes = container->Get<bool>("stereo_swap_eyes");
	sweetSpotHAngle = container->Get<double>("sweet_spot_horizontal_angle") / 180.0 * M_PI;
	sweetSpotVAngle = container->Get<double>("sweet_spot_vertical_angle") / 180.0 * M_PI;
	target = container->Get<CVector3>("target");
	target = container->Get<CVector3>("target");
	texturedBackground = container->Get<bool>("textured_background");
	texturedBackgroundMapType =
		params::enumTextureMapType(container->Get<int>("textured_background_map_type"));
	topVector = container->Get<CVector3>("camera_top");
	useDefaultBailout = container->Get<bool>("use_default_bailout");
	viewAngle = container->Get<CVector3>("camera_rotation");
	viewDistanceMax = container->Get<double>("view_distance_max");
	viewDistanceMin = container->Get<double>("view_distance_min");
	volFogColour1 = toRGBFloat(container->Get<sRGB>("fog_color", 1));
	volFogColour1Distance = container->Get<double>("volumetric_fog_colour_1_distance");
	volFogColour2 = toRGBFloat(container->Get<sRGB>("fog_color", 2));
	volFogColour2Distance = container->Get<double>("volumetric_fog_colour_2_distance");
	volFogColour3 = toRGBFloat(container->Get<sRGB>("fog_color", 3));
	volFogDensity = container->Get<float>("volumetric_fog_density");
	volFogDistanceFactor = container->Get<double>("volumetric_fog_distance_factor");
	volFogDistanceFromSurface = container->Get<double>("volumetric_fog_distance_from_surface");
	volFogEnabled = container->Get<bool>("volumetric_fog_enabled");
	volumetricLightDEFactor = container->Get<double>("volumetric_light_DE_Factor");

	mRotBackgroundRotation.SetRotation(backgroundRotation * M_PI / 180.0);
	mRotCloudsRotation.SetRotation2(cloudsRotation * M_PI / 180.0);

	nebulaXAxisColors.SetColorsFromString(container->Get<QString>("nebula_x_axis_colors"));
	nebulaYAxisColors.SetColorsFromString(container->Get<QString>("nebula_y_axis_colors"));
	nebulaZAxisColors.SetColorsFromString(container->Get<QString>("nebula_z_axis_colors"));
	nebulaIterationsColors.SetColorsFromString(container->Get<QString>("nebula_iterations_colors"));

	for (int i = 0; i < NUMBER_OF_FRACTALS - 1; i++)
	{
		booleanOperator[i] =
			params::enumBooleanOperator(container->Get<int>("boolean_operator", i + 1));
	}

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		formulaPosition[i] = container->Get<CVector3>("formula_position", i + 1);
		formulaRotation[i] = container->Get<CVector3>("formula_rotation", i + 1);
		formulaRepeat[i] = container->Get<CVector3>("formula_repeat", i + 1);
		formulaScale[i] = 1.0 / container->Get<double>("formula_scale", i + 1);
		mRotFormulaRotation[i].SetRotation2(formulaRotation[i] * (M_PI / 180.0));
		formulaMaterialId[i] = container->Get<int>("formula_material_id", i + 1);
		smoothDeCombineEnable[i] = container->Get<bool>("smooth_de_combine_enable", i + 1);
		smoothDeCombineDistance[i] = container->Get<double>("smooth_de_combine_distance", i + 1);

		if (objectData)
		{
			cObjectData oneObjectData;
			oneObjectData.position = formulaPosition[i];
			oneObjectData.size = CVector3(1.0, 1.0, 1.0) / formulaScale[i];
			oneObjectData.repeat = formulaRepeat[i];
			oneObjectData.SetRotation(formulaRotation[i]);
			oneObjectData.materialId = formulaMaterialId[i];
			oneObjectData.objectType = fractal::objFractal;
			(*objectData)[i] = oneObjectData;
		}
	}

	if (!booleanOperatorsEnabled && objectData)
	{
		formulaMaterialId[0] = container->Get<int>("formula_material_id");
		(*objectData)[0].materialId = formulaMaterialId[0];
		(*objectData)[0].position = container->Get<CVector3>("fractal_position");
		(*objectData)[0].repeat = container->Get<CVector3>("repeat");
		(*objectData)[0].size = CVector3(1.0, 1.0, 1.0);
		(*objectData)[0].SetRotation(container->Get<CVector3>("fractal_rotation"));
		(*objectData)[0].objectType = fractal::objFractal;
	}

	common.fakeLightsColor2Enabled = container->Get<bool>("fake_lights_color_2_enabled");
	common.fakeLightsColor3Enabled = container->Get<bool>("fake_lights_color_3_enabled");
	common.fakeLightsMaxIter = container->Get<int>("fake_lights_max_iter");
	common.fakeLightsMinIter = container->Get<int>("fake_lights_min_iter");
	common.fakeLightsOrbitTrap = container->Get<CVector3>("fake_lights_orbit_trap");
	common.fakeLightsOrbitTrapShape =
		params::enumFakeLightsShape(container->Get<int>("fake_lights_orbit_trap_shape"));
	common.fakeLightsOrbitTrapSize = container->Get<double>("fake_lights_orbit_trap_size");
	common.fakeLightsThickness = container->Get<double>("fake_lights_thickness");
	common.fakeLightsRelativeCenter = container->Get<bool>("fake_lights_relative_center");
	common.fakeLightsRotation = container->Get<CVector3>("fake_lights_orbit_rotation");
	common.foldings.boxEnable = container->Get<bool>("box_folding");
	common.foldings.boxLimit = container->Get<double>("box_folding_limit");
	common.foldings.boxValue = container->Get<double>("box_folding_value");
	common.foldings.sphericalEnable = container->Get<bool>("spherical_folding");
	common.foldings.sphericalInner = container->Get<double>("spherical_folding_inner");
	common.foldings.sphericalOuter = container->Get<double>("spherical_folding_outer");
	common.fractalPosition = container->Get<CVector3>("fractal_position");
	common.fractalRotation = container->Get<CVector3>("fractal_rotation");
	common.mRotFractalRotation.SetRotation2(common.fractalRotation / 180.0 * M_PI);
	common.repeat = container->Get<CVector3>("repeat");
	common.iterThreshMode = iterThreshMode = container->Get<bool>("iteration_threshold_mode");
	common.linearDEOffset = container->Get<double>("linear_DE_offset");

	common.mRotFakeLightsRotation.SetRotation2(common.fakeLightsRotation * M_PI / 180.0);

	// formula = Get<int>("tile_number");
}
