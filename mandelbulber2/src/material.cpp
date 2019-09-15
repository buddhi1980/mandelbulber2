/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cMaterial class - container class to hold all information about a material
 */

#include "material.h"

#include "netrender.hpp"
#include "parameters.hpp"
#include "texture_enums.hpp"

cMaterial::cMaterial()
{
	id = -1;
	shading = 0.0;
	specular = 0.0;
	specularWidth = 0.0;
	specularPlasticEnable = false;
	specularMetallic = 0.0;
	specularMetallicRoughness = 0.0;
	specularMetallicWidth = 0.0;
	reflectance = 0.0;
	luminosity = 0.0;
	surfaceRoughness = 0.0;
	coloring_speed = 0.0;
	paletteOffset = 0.0;
	fresnelReflectance = 0.0;
	transparencyIndexOfRefraction = 0.0;
	transparencyOfInterior = 0.0;
	transparencyOfSurface = 0.0;
	useColorsFromPalette = false;
	useColorTexture = false;
	useDiffusionTexture = false;
	useLuminosityTexture = false;
	useDisplacementTexture = false;
	useNormalMapTexture = false;
	useReflectanceTexture = false;
	useTransparencyTexture = false;
	useRoughnessTexture = false;
	iridescenceEnabled = false;
	textureMappingType = texture::mappingPlanar;
	colorTextureIntensity = 0.0;
	diffusionTextureIntensity = 0.0;
	luminosityTextureIntensity = 0.0;
	reflectanceTextureIntensity = 0.0;
	transparencyTextureIntensity = 0.0;
	roughnessTextureIntensity = 0.0;
	displacementTextureHeight = 0.0;
	normalMapTextureFromBumpmap = false;
	normalMapTextureInvertGreen = false;
	normalMapTextureHeight = 0.0;
	metallic = false;
	iridescenceEnabled = false;
	iridescenceIntensity = 0.0;
	iridescenceSubsurfaceThickness = 0.0;
	textureFractalizeStartIteration = 0;
	textureFractalize = false;
	textureFractalizeCubeSize = 0.0;
	roughSurface = false;
	surfaceGradientEnable = false;
	specularGradientEnable = false;
	diffuseGradientEnable = false;
	luminosityGradientEnable = false;
	roughnessGradientEnable = false;
	reflectanceGradientEnable = false;
	transparencyGradientEnable = false;
}

cMaterial::cMaterial(int _id, const cParameterContainer *materialParam, bool loadTextures,
	bool quiet, bool useNetRender)
{
	setParameters(_id, materialParam, loadTextures, quiet, useNetRender);
}

cMaterial::~cMaterial() = default;

// this static list will be use to optimize usage of material parameters
QStringList cMaterial::paramsList = {"color_texture_intensity", "coloring_palette_offset",
	"coloring_speed", "diffusion_texture_intensity", "displacement_texture_height",
	"file_color_texture", "file_diffusion_texture", "file_displacement_texture",
	"file_luminosity_texture", "file_normal_map_texture", "file_reflectance_texture",
	"file_transparency_texture", "file_roughness_texture", "fractal_coloring_add_enabled_false",
	"fractal_coloring_add_max", "fractal_coloring_add_spread", "fractal_coloring_add_start_value",
	"fractal_coloring_algorithm", "fractal_coloring_aux_color_false",
	"fractal_coloring_aux_color_hybrid_weight", "fractal_coloring_aux_color_scale1",
	"fractal_coloring_aux_color_weight", "fractal_coloring_color_4D_enabled_false",
	"fractal_coloring_color_preV215_false", "fractal_coloring_cos_add",
	"fractal_coloring_cos_enabled_false", "fractal_coloring_cos_period",
	"fractal_coloring_cos_start_value", "fractal_coloring_extra_color_enabled_false",
	"fractal_coloring_extra_color_options_false", "fractal_coloring_global_palette_false",
	"fractal_coloring_i_start_value", "fractal_coloring_ic_fabs_enabled_false",
	"fractal_coloring_ic_rad_enabled_false", "fractal_coloring_ic_rad_weight",
	"fractal_coloring_ic_xyz_enabled_false", "fractal_coloring_init_cond_enabled_false",
	"fractal_coloring_initial_color_value", "fractal_coloring_iter_add_scale_enabled_true",
	"fractal_coloring_iter_add_scale", "fractal_coloring_iter_group_enabled_false",
	"fractal_coloring_iter_scale_enabled_false", "fractal_coloring_iter_scale",
	"fractal_coloring_line_direction", "fractal_coloring_max_color_value",
	"fractal_coloring_min_color_value", "fractal_coloring_orbit_trap_scale1",
	"fractal_coloring_orbit_trap_true", "fractal_coloring_orbit_trap_weight",
	"fractal_coloring_parab_enabled_false", "fractal_coloring_parab_scale",
	"fractal_coloring_parab_start_value", "fractal_coloring_rad_div_1e13_false",
	"fractal_coloring_rad_div_de_1e13_false", "fractal_coloring_rad_div_de_enabled_false",
	"fractal_coloring_rad_div_de_squared_false", "fractal_coloring_rad_div_de_scale1",
	"fractal_coloring_rad_div_de_weight", "fractal_coloring_rad_enabled_false",
	"fractal_coloring_rad_squared_enabled_false", "fractal_coloring_rad_weight",
	"fractal_coloring_round_enabled_false", "fractal_coloring_round_scale",
	"fractal_coloring_sphere_radius",
	"fractal_coloring_temp_limit_false", // TODO TESTING
	"fractal_coloring_xyz_000", "fractal_coloring_xyz_bias_enabled_false",
	"fractal_coloring_xyz_div_1e13_false", "fractal_coloring_xyz_fabs_enabled_false",
	"fractal_coloring_xyz_iter_scale", "fractal_coloring_xyz_x_sqrd_enabled_false",
	"fractal_coloring_xyz_y_sqrd_enabled_false", "fractal_coloring_xyz_z_sqrd_enabled_false",
	"fractal_coloring_xyzC_111", "fresnel_reflectance", "iridescence_enabled",
	"iridescence_intensity", "iridescence_subsurface_thickness", "is_defined", "luminosity_color",
	"luminosity_texture_intensity", "luminosity", "metallic", "name",
	"normal_map_texture_from_bumpmap", "normal_map_texture_height", "normal_map_texture_invert_green",
	"reflectance", "reflections_color", "reflectance_texture_intensity", "rough_surface",
	"roughness_texture_intensity", "shading", "specular_color", "specular_metallic_roughness",
	"specular_metallic_width", "specular_metallic", "specular_plastic_enable", "specular_width",
	"specular", "surface_color", "surface_roughness", "texture_center",
	"texture_fractalize_cube_size", "texture_fractalize_start_iteration", "texture_fractalize",
	"texture_mapping_type", "texture_rotation", "texture_scale", "transparency_color",
	"transparency_index_of_refraction", "transparency_interior_color", "transparency_of_interior",
	"transparency_of_surface", "transparency_texture_intensity", "use_color_texture",
	"use_colors_from_palette", "use_diffusion_texture", "use_displacement_texture",
	"use_luminosity_texture", "use_normal_map_texture", "use_reflectance_texture",
	"use_transparency_texture", "use_roughness_texture", "surface_gradient_enable",
	"specular_gradient_enable", "diffuse_gradient_enable", "luminosity_gradient_enable",
	"roughness_gradient_enable", "reflectance_gradient_enable", "transparency_gradient_enable",
	"surface_color_gradient", "specular_gradient", "diffuse_gradient", "luminosity_gradient",
	"roughness_gradient", "reflectance_gradient", "transparency_gradient"};

void cMaterial::setParameters(int _id, const cParameterContainer *materialParam, bool loadTextures,
	bool quiet, bool useNetRender)
{
	id = _id;

	int frameNo = materialParam->Get<int>("frame_no");

	shading = materialParam->Get<float>(Name("shading", id));
	specular = materialParam->Get<float>(Name("specular", id));
	specularWidth = materialParam->Get<float>(Name("specular_width", id));
	specularMetallic = materialParam->Get<float>(Name("specular_metallic", id));
	specularMetallicWidth = materialParam->Get<float>(Name("specular_metallic_width", id));
	specularMetallicRoughness = materialParam->Get<float>(Name("specular_metallic_roughness", id));
	specularColor = toRGBFloat(materialParam->Get<sRGB>(Name("specular_color", id)));
	specularPlasticEnable = materialParam->Get<bool>(Name("specular_plastic_enable", id));
	metallic = materialParam->Get<bool>(Name("metallic", id));
	reflectance = materialParam->Get<float>(Name("reflectance", id));
	luminosity = materialParam->Get<float>(Name("luminosity", id));
	surfaceRoughness = materialParam->Get<float>(Name("surface_roughness", id));
	transparencyIndexOfRefraction =
		materialParam->Get<float>(Name("transparency_index_of_refraction", id));
	transparencyOfInterior = materialParam->Get<float>(Name("transparency_of_interior", id));
	transparencyOfSurface = materialParam->Get<float>(Name("transparency_of_surface", id));
	paletteOffset = materialParam->Get<double>(Name("coloring_palette_offset", id));
	coloring_speed = materialParam->Get<double>(Name("coloring_speed", id));

	color = toRGBFloat(materialParam->Get<sRGB>(Name("surface_color", id)));
	luminosityColor = toRGBFloat(materialParam->Get<sRGB>(Name("luminosity_color", id)));
	transparencyInteriorColor =
		toRGBFloat(materialParam->Get<sRGB>(Name("transparency_interior_color", id)));
	reflectionsColor = toRGBFloat(materialParam->Get<sRGB>(Name("reflections_color", id)));
	transparencyColor = toRGBFloat(materialParam->Get<sRGB>(Name("transparency_color", id)));

	roughSurface = materialParam->Get<bool>(Name("rough_surface", id));

	gradientSurface.SetColorsFromString(
		materialParam->Get<QString>(Name("surface_color_gradient", id)));
	gradientSpecular.SetColorsFromString(materialParam->Get<QString>(Name("specular_gradient", id)));
	gradientDiffuse.SetColorsFromString(materialParam->Get<QString>(Name("diffuse_gradient", id)));
	gradientLuminosity.SetColorsFromString(
		materialParam->Get<QString>(Name("luminosity_gradient", id)));
	gradientRoughness.SetColorsFromString(
		materialParam->Get<QString>(Name("roughness_gradient", id)));
	gradientReflectance.SetColorsFromString(
		materialParam->Get<QString>(Name("reflectance_gradient", id)));
	gradientTransparency.SetColorsFromString(
		materialParam->Get<QString>(Name("transparency_gradient", id)));

	surfaceGradientEnable = materialParam->Get<bool>(Name("surface_gradient_enable", id));
	specularGradientEnable = materialParam->Get<bool>(Name("specular_gradient_enable", id));
	diffuseGradientEnable = materialParam->Get<bool>(Name("diffuse_gradient_enable", id));
	luminosityGradientEnable = materialParam->Get<bool>(Name("luminosity_gradient_enable", id));
	roughnessGradientEnable = materialParam->Get<bool>(Name("roughness_gradient_enable", id));
	reflectanceGradientEnable = materialParam->Get<bool>(Name("reflectance_gradient_enable", id));
	transparencyGradientEnable = materialParam->Get<bool>(Name("transparency_gradient_enable", id));

	textureCenter = materialParam->Get<CVector3>(Name("texture_center", id));
	textureRotation = materialParam->Get<CVector3>(Name("texture_rotation", id));
	textureScale = materialParam->Get<CVector3>(Name("texture_scale", id));
	if (textureScale.x < 1e-20) textureScale.x = 1e-20;
	if (textureScale.y < 1e-20) textureScale.y = 1e-20;
	if (textureScale.z < 1e-20) textureScale.z = 1e-20;

	textureMappingType =
		texture::enumTextureMapping(materialParam->Get<int>(Name("texture_mapping_type", id)));

	fresnelReflectance = materialParam->Get<bool>(Name("fresnel_reflectance", id));
	useColorsFromPalette = materialParam->Get<bool>(Name("use_colors_from_palette", id));

	useColorTexture = materialParam->Get<bool>(Name("use_color_texture", id));
	useDiffusionTexture = materialParam->Get<bool>(Name("use_diffusion_texture", id));
	useLuminosityTexture = materialParam->Get<bool>(Name("use_luminosity_texture", id));
	useDisplacementTexture = materialParam->Get<bool>(Name("use_displacement_texture", id));
	useNormalMapTexture = materialParam->Get<bool>(Name("use_normal_map_texture", id));
	useReflectanceTexture = materialParam->Get<bool>(Name("use_reflectance_texture", id));
	useTransparencyTexture = materialParam->Get<bool>(Name("use_transparency_texture", id));
	useRoughnessTexture = materialParam->Get<bool>(Name("use_roughness_texture", id));
	normalMapTextureFromBumpmap =
		materialParam->Get<bool>(Name("normal_map_texture_from_bumpmap", id));
	normalMapTextureInvertGreen =
		materialParam->Get<bool>(Name("normal_map_texture_invert_green", id));

	colorTextureIntensity = materialParam->Get<float>(Name("color_texture_intensity", id));
	diffusionTextureIntensity = materialParam->Get<float>(Name("diffusion_texture_intensity", id));
	luminosityTextureIntensity = materialParam->Get<float>(Name("luminosity_texture_intensity", id));
	displacementTextureHeight = materialParam->Get<double>(Name("displacement_texture_height", id));
	normalMapTextureHeight = materialParam->Get<double>(Name("normal_map_texture_height", id));
	reflectanceTextureIntensity =
		materialParam->Get<float>(Name("reflectance_texture_intensity", id));
	transparencyTextureIntensity =
		materialParam->Get<float>(Name("transparency_texture_intensity", id));
	roughnessTextureIntensity = materialParam->Get<float>(Name("roughness_texture_intensity", id));

	iridescenceEnabled = materialParam->Get<bool>(Name("iridescence_enabled", id));
	iridescenceIntensity = materialParam->Get<double>(Name("iridescence_intensity", id));
	iridescenceSubsurfaceThickness =
		materialParam->Get<double>(Name("iridescence_subsurface_thickness", id));

	textureFractalize = materialParam->Get<bool>(Name("texture_fractalize", id));
	textureFractalizeCubeSize = materialParam->Get<double>(Name("texture_fractalize_cube_size", id));
	textureFractalizeStartIteration =
		materialParam->Get<int>(Name("texture_fractalize_start_iteration", id));

	fractalColoring.coloringAlgorithm =
		enumFractalColoring(materialParam->Get<int>(Name("fractal_coloring_algorithm", id)));
	fractalColoring.sphereRadius =
		materialParam->Get<double>(Name("fractal_coloring_sphere_radius", id));
	fractalColoring.lineDirection =
		materialParam->Get<CVector4>(Name("fractal_coloring_line_direction", id));
	fractalColoring.color4dEnabledFalse =
		materialParam->Get<bool>(Name("fractal_coloring_color_4D_enabled_false", id));

	fractalColoring.extraColorOptionsEnabledFalse =
		materialParam->Get<bool>(Name("fractal_coloring_extra_color_options_false", id));
	fractalColoring.colorPreV215False =
		materialParam->Get<bool>(Name("fractal_coloring_color_preV215_false", id));
	fractalColoring.hybridAuxColorScale1 =
		materialParam->Get<double>(Name("fractal_coloring_aux_color_scale1", id));
	fractalColoring.hybridOrbitTrapScale1 =
		materialParam->Get<double>(Name("fractal_coloring_orbit_trap_scale1", id));
	fractalColoring.hybridRadDivDeScale1 =
		materialParam->Get<double>(Name("fractal_coloring_rad_div_de_scale1", id));

	fractalColoring.tempLimitFalse =
		materialParam->Get<bool>(Name("fractal_coloring_temp_limit_false", id)); // tempLimit

	// color by numbers
	fractalColoring.extraColorEnabledFalse =
		materialParam->Get<bool>(Name("fractal_coloring_extra_color_enabled_false", id));

	// Initial Conditions
	fractalColoring.initialColorValue =
		materialParam->Get<double>(Name("fractal_coloring_initial_color_value", id));
	fractalColoring.initCondFalse =
		materialParam->Get<bool>(Name("fractal_coloring_init_cond_enabled_false", id));
	fractalColoring.icRadFalse =
		materialParam->Get<bool>(Name("fractal_coloring_ic_rad_enabled_false", id));
	fractalColoring.icXYZFalse =
		materialParam->Get<bool>(Name("fractal_coloring_ic_xyz_enabled_false", id));
	fractalColoring.icFabsFalse =
		materialParam->Get<bool>(Name("fractal_coloring_ic_fabs_enabled_false", id));
	fractalColoring.icRadWeight =
		materialParam->Get<double>(Name("fractal_coloring_ic_rad_weight", id));
	fractalColoring.xyzC111 = materialParam->Get<CVector3>(Name("fractal_coloring_xyzC_111", id));

	// orbitTrap weight control
	fractalColoring.orbitTrapTrue =
		materialParam->Get<bool>(Name("fractal_coloring_orbit_trap_true", id));
	fractalColoring.orbitTrapWeight =
		materialParam->Get<double>(Name("fractal_coloring_orbit_trap_weight", id));
	// fractalColoring.initialMinimumR =
	// materialParam->Get<double>(Name("fractal_coloring_initial_minimumR", id));

	// aux.color
	fractalColoring.auxColorFalse =
		materialParam->Get<bool>(Name("fractal_coloring_aux_color_false", id));
	fractalColoring.auxColorWeight =
		materialParam->Get<double>(Name("fractal_coloring_aux_color_weight", id));
	fractalColoring.auxColorHybridWeight =
		materialParam->Get<double>(Name("fractal_coloring_aux_color_hybrid_weight", id));
	// radius
	fractalColoring.radFalse =
		materialParam->Get<bool>(Name("fractal_coloring_rad_enabled_false", id));
	fractalColoring.radWeight = materialParam->Get<double>(Name("fractal_coloring_rad_weight", id));
	fractalColoring.radSquaredFalse =
		materialParam->Get<bool>(Name("fractal_coloring_rad_squared_enabled_false", id));
	fractalColoring.radDiv1e13False =
		materialParam->Get<bool>(Name("fractal_coloring_rad_div_1e13_false", id));
	// radius/DE
	fractalColoring.radDivDeFalse =
		materialParam->Get<bool>(Name("fractal_coloring_rad_div_de_enabled_false", id));
	fractalColoring.radDivDeWeight =
		materialParam->Get<double>(Name("fractal_coloring_rad_div_de_weight", id));
	fractalColoring.radDivDeSquaredFalse =
		materialParam->Get<bool>(Name("fractal_coloring_rad_div_de_squared_false", id));
	fractalColoring.radDivDE1e13False =
		materialParam->Get<bool>(Name("fractal_coloring_rad_div_de_1e13_false", id));
	// XYZ bias
	fractalColoring.xyzBiasEnabledFalse =
		materialParam->Get<bool>(Name("fractal_coloring_xyz_bias_enabled_false", id));
	fractalColoring.xyz000 = materialParam->Get<CVector3>(Name("fractal_coloring_xyz_000", id));
	fractalColoring.xyzIterScale =
		materialParam->Get<double>(Name("fractal_coloring_xyz_iter_scale", id));
	fractalColoring.xyzXSqrdFalse =
		materialParam->Get<bool>(Name("fractal_coloring_xyz_x_sqrd_enabled_false", id));
	fractalColoring.xyzYSqrdFalse =
		materialParam->Get<bool>(Name("fractal_coloring_xyz_y_sqrd_enabled_false", id));
	fractalColoring.xyzZSqrdFalse =
		materialParam->Get<bool>(Name("fractal_coloring_xyz_z_sqrd_enabled_false", id));
	fractalColoring.xyzFabsFalse =
		materialParam->Get<bool>(Name("fractal_coloring_xyz_fabs_enabled_false", id));
	fractalColoring.xyzDiv1e13False =
		materialParam->Get<bool>(Name("fractal_coloring_xyz_div_1e13_false", id));

	fractalColoring.iterGroupFalse =
		materialParam->Get<bool>(Name("fractal_coloring_iter_group_enabled_false", id));
	fractalColoring.iterScaleFalse =
		materialParam->Get<bool>(Name("fractal_coloring_iter_scale_enabled_false", id));
	fractalColoring.iterAddScaleTrue =
		materialParam->Get<bool>(Name("fractal_coloring_iter_add_scale_enabled_true", id));
	fractalColoring.iterAddScale =
		materialParam->Get<double>(Name("fractal_coloring_iter_add_scale", id));
	fractalColoring.iterScale = materialParam->Get<double>(Name("fractal_coloring_iter_scale", id));
	fractalColoring.iStartValue = materialParam->Get<int>(Name("fractal_coloring_i_start_value", id));

	// global palette controls
	fractalColoring.globalPaletteFalse =
		materialParam->Get<bool>(Name("fractal_coloring_global_palette_false", id));

	fractalColoring.addEnabledFalse =
		materialParam->Get<bool>(Name("fractal_coloring_add_enabled_false", id));
	fractalColoring.addMax = materialParam->Get<double>(Name("fractal_coloring_add_max", id));
	fractalColoring.addSpread = materialParam->Get<double>(Name("fractal_coloring_add_spread", id));
	fractalColoring.addStartValue =
		materialParam->Get<double>(Name("fractal_coloring_add_start_value", id));

	fractalColoring.parabEnabledFalse =
		materialParam->Get<bool>(Name("fractal_coloring_parab_enabled_false", id));
	fractalColoring.parabScale = materialParam->Get<double>(Name("fractal_coloring_parab_scale", id));
	fractalColoring.parabStartValue =
		materialParam->Get<double>(Name("fractal_coloring_parab_start_value", id));

	fractalColoring.cosEnabledFalse =
		materialParam->Get<bool>(Name("fractal_coloring_cos_enabled_false", id));
	fractalColoring.cosPeriod = materialParam->Get<double>(Name("fractal_coloring_cos_period", id));
	fractalColoring.cosAdd = materialParam->Get<double>(Name("fractal_coloring_cos_add", id));
	fractalColoring.cosStartValue =
		materialParam->Get<double>(Name("fractal_coloring_cos_start_value", id));

	fractalColoring.roundEnabledFalse =
		materialParam->Get<bool>(Name("fractal_coloring_round_enabled_false", id));
	fractalColoring.roundScale = materialParam->Get<double>(Name("fractal_coloring_round_scale", id));

	fractalColoring.maxColorValue =
		materialParam->Get<double>(Name("fractal_coloring_max_color_value", id));
	fractalColoring.minColorValue =
		materialParam->Get<double>(Name("fractal_coloring_min_color_value", id));

	if (loadTextures)
	{
		//		if (gNetRender->IsClient())
		//		{
		//			if (useColorTexture)
		//				colorTexture.FromQByteArray(
		//					gNetRender->GetTexture(
		//						materialParam->Get<QString>(Name("file_color_texture", id)), frameNo),
		//					cTexture::useMipmaps);
		//
		//			if (useDiffusionTexture)
		//				diffusionTexture.FromQByteArray(
		//					gNetRender->GetTexture(
		//						materialParam->Get<QString>(Name("file_diffusion_texture", id)), frameNo),
		//					cTexture::useMipmaps);
		//
		//			if (useLuminosityTexture)
		//				luminosityTexture.FromQByteArray(
		//					gNetRender->GetTexture(
		//						materialParam->Get<QString>(Name("file_luminosity_texture", id)), frameNo),
		//					cTexture::useMipmaps);
		//
		//			if (useDisplacementTexture)
		//				displacementTexture.FromQByteArray(
		//					gNetRender->GetTexture(
		//						materialParam->Get<QString>(Name("file_displacement_texture", id)), frameNo),
		//					cTexture::doNotUseMipmaps);
		//
		//			if (useNormalMapTexture)
		//				normalMapTexture.FromQByteArray(
		//					gNetRender->GetTexture(
		//						materialParam->Get<QString>(Name("file_normal_map_texture", id)), frameNo),
		//					cTexture::doNotUseMipmaps);
		//
		//			if (useReflectanceTexture)
		//				reflectanceTexture.FromQByteArray(
		//					gNetRender->GetTexture(
		//						materialParam->Get<QString>(Name("file_reflectance_texture", id)), frameNo),
		//					cTexture::useMipmaps);
		//
		//			if (useTransparencyTexture)
		//				transparencyTexture.FromQByteArray(
		//					gNetRender->GetTexture(
		//						materialParam->Get<QString>(Name("file_transparency_texture", id)), frameNo),
		//					cTexture::useMipmaps);
		//
		//			if (useRoughnessTexture)
		//				roughnessTexture.FromQByteArray(
		//					gNetRender->GetTexture(
		//						materialParam->Get<QString>(Name("file_roughness_texture", id)), frameNo),
		//					cTexture::useMipmaps);
		//		}
		//		else
		//		{
		if (useColorTexture)
			colorTexture = cTexture(materialParam->Get<QString>(Name("file_color_texture", id)),
				cTexture::useMipmaps, frameNo, quiet, useNetRender);

		if (useDiffusionTexture)
			diffusionTexture = cTexture(materialParam->Get<QString>(Name("file_diffusion_texture", id)),
				cTexture::useMipmaps, frameNo, quiet, useNetRender);

		if (useLuminosityTexture)
			luminosityTexture = cTexture(materialParam->Get<QString>(Name("file_luminosity_texture", id)),
				cTexture::useMipmaps, frameNo, quiet, useNetRender);

		if (useDisplacementTexture)
			displacementTexture =
				cTexture(materialParam->Get<QString>(Name("file_displacement_texture", id)),
					cTexture::doNotUseMipmaps, frameNo, quiet, useNetRender);

		if (useNormalMapTexture)
			normalMapTexture = cTexture(materialParam->Get<QString>(Name("file_normal_map_texture", id)),
				cTexture::useMipmaps, frameNo, quiet, useNetRender);

		if (useReflectanceTexture)
			reflectanceTexture =
				cTexture(materialParam->Get<QString>(Name("file_reflectance_texture", id)),
					cTexture::useMipmaps, frameNo, quiet, useNetRender);

		if (useTransparencyTexture)
			transparencyTexture =
				cTexture(materialParam->Get<QString>(Name("file_transparency_texture", id)),
					cTexture::useMipmaps, frameNo, quiet, useNetRender);

		if (useRoughnessTexture)
			roughnessTexture = cTexture(materialParam->Get<QString>(Name("file_roughness_texture", id)),
				cTexture::useMipmaps, frameNo, quiet, useNetRender);
		//		}
	}

	rotMatrix.SetRotation2(textureRotation / 180 * M_PI);
}

void CreateMaterialsMap(const cParameterContainer *params, QMap<int, cMaterial> *materials,
	bool loadTextures, bool quiet, bool useNetRender)
{
	materials->clear();
	QList<QString> listOfParameters = params->GetListOfParameters();
	for (auto &parameterName : listOfParameters)
	{
		if (parameterName.leftRef(3) == "mat")
		{
			int positionOfDash = parameterName.indexOf('_');
			int matIndex = parameterName.midRef(3, positionOfDash - 3).toInt();
			if (parameterName.midRef(positionOfDash + 1) == "is_defined")
			{
				materials->insert(matIndex, cMaterial(matIndex, params, loadTextures, quiet, useNetRender));
			}
		}
	}
}
