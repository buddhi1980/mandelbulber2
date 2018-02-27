/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
	reflectance = 0.0;
	luminosity = 0.0;
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
	iridescenceEnabled = false;
	textureMappingType = texture::mappingPlanar;
	colorTextureIntensity = 0.0;
	diffusionTextureIntensity = 0.0;
	luminosityTextureIntensity = 0.0;
	displacementTextureHeight = 0.0;
	normalMapTextureFromBumpmap = false;
	normalMapTextureHeight = 0.0;
	metallic = false;
	iridescenceEnabled = false;
	iridescenceIntensity = 0.0;
	iridescenceSubsurfaceThickness = 0.0;
	textureFractalizeStartIteration = 0;
	textureFractalize = false;
	textureFractalizeCubeSize = 0.0;
}

cMaterial::cMaterial(int _id, const cParameterContainer *materialParam, bool quiet)
{
	setParameters(_id, materialParam, quiet);
}

cMaterial::~cMaterial()
{
}

// this static list will be use to optimize usage of material parameters
QStringList cMaterial::paramsList = {
	"is_defined", "name", "shading", "specular", "specular_width", "specular_color", "reflectance",
	"luminosity", "transparency_of_surface", "transparency_of_interior",
	"transparency_index_of_refraction", "surface_color", "transparency_interior_color",
	"luminosity_color", "fresnel_reflectance", "texture_center", "texture_rotation", "texture_scale",
	"coloring_random_seed", "coloring_saturation", "coloring_speed", "coloring_palette_size",
	"coloring_palette_offset", "texture_mapping_type", "use_colors_from_palette",
	"file_color_texture", "file_diffusion_texture", "file_luminosity_texture",
	"file_displacement_texture", "surface_color_palette", "use_color_texture",
	"use_diffusion_texture", "use_luminosity_texture", "use_displacement_texture",
	"use_normal_map_texture", "color_texture_intensity", "luminosity_texture_intensity",
	"diffusion_texture_intensity", "displacement_texture_height", "fractal_coloring_algorithm",
	"fractal_coloring_sphere_radius", "fractal_coloring_line_direction",
	"normal_map_texture_from_bumpmap", "normal_map_texture_height", "normal_map_texture_invert_green",
	"file_normal_map_texture", "texture_fractalize", "texture_fractalize_cube_size",
	"texture_fractalize_start_iteration",

	"fractal_coloring_color_4D_enabled_false", "fractal_coloring_extra_color_enabled_false",
	"metallic", "iridescence_enabled", "iridescence_intensity", "iridescence_subsurface_thickness",

	"fractal_coloring_init_cond_enabled_false", "fractal_coloring_ic_rad_enabled_false",
	"fractal_coloring_ic_xyz_enabled_false", "fractal_coloring_ic_rad_weight",
	"fractal_coloring_xyzC_111", "fractal_coloring_initial_color_value",
	"fractal_coloring_ic_fabs_enabled_false",

	"fractal_coloring_orbit_trap_true", "fractal_coloring_orbit_trap_weight",
	"fractal_coloring_initial_minimumR",

	"fractal_coloring_aux_color_false", "fractal_coloring_aux_color_weight",
	"fractal_coloring_aux_color_hybrid_weight",

	"fractal_coloring_rad_enabled_false", "fractal_coloring_rad_weight",
	"fractal_coloring_rad_squared_enabled_false", "fractal_coloring_rad_div_1e13_false",
	"fractal_coloring_rad_div_de_enabled_false", "fractal_coloring_rad_div_log_de_false",
	"fractal_coloring_rad_div_de_weight", "fractal_coloring_rad_div_de_squared_false",

	"fractal_coloring_xyz_bias_enabled_false", "fractal_coloring_xyz_000",
	"fractal_coloring_xyz_iter_scale", "fractal_coloring_xyz_x_sqrd_enabled_false",
	"fractal_coloring_xyz_y_sqrd_enabled_false", "fractal_coloring_xyz_z_sqrd_enabled_false",
	"fractal_coloring_xyz_fabs_enabled_false", "fractal_coloring_xyz_div_1e13_false",

	"fractal_coloring_iter_scale_enabled_false", "fractal_coloring_iter_add_scale_enabled_true",
	"fractal_coloring_iter_scale", "fractal_coloring_iter_add_scale",
	"fractal_coloring_i_start_value", "fractal_coloring_iter_group_enabled_false",

	"fractal_coloring_global_palette_false",

	"fractal_coloring_add_enabled_false", "fractal_coloring_add_max", "fractal_coloring_add_spread",
	"fractal_coloring_add_start_value",

	"fractal_coloring_parab_enabled_false", "fractal_coloring_parab_scale",
	"fractal_coloring_parab_start_value",

	"fractal_coloring_cos_enabled_false", "fractal_coloring_cos_period", "fractal_coloring_cos_add",
	"fractal_coloring_cos_start_value",

	"fractal_coloring_round_enabled_false", "fractal_coloring_round_scale",

	"fractal_coloring_max_color_value", "fractal_coloring_min_color_value",
};

void cMaterial::setParameters(int _id, const cParameterContainer *materialParam, bool quiet = false)
{
	id = _id;

	shading = materialParam->Get<double>(Name("shading", id));
	specular = materialParam->Get<double>(Name("specular", id));
	specularWidth = materialParam->Get<double>(Name("specular_width", id));
	specularColor = materialParam->Get<sRGB>(Name("specular_color", id));
	metallic = materialParam->Get<bool>(Name("metallic", id));
	reflectance = materialParam->Get<double>(Name("reflectance", id));
	luminosity = materialParam->Get<double>(Name("luminosity", id));
	transparencyIndexOfRefraction =
		materialParam->Get<double>(Name("transparency_index_of_refraction", id));
	transparencyOfInterior = materialParam->Get<double>(Name("transparency_of_interior", id));
	transparencyOfSurface = materialParam->Get<double>(Name("transparency_of_surface", id));
	paletteOffset = materialParam->Get<double>(Name("coloring_palette_offset", id));
	coloring_speed = materialParam->Get<double>(Name("coloring_speed", id));

	color = materialParam->Get<sRGB>(Name("surface_color", id));
	luminosityColor = materialParam->Get<sRGB>(Name("luminosity_color", id));
	transparencyInteriorColor = materialParam->Get<sRGB>(Name("transparency_interior_color", id));

	palette = materialParam->Get<cColorPalette>(Name("surface_color_palette", id));

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
	normalMapTextureFromBumpmap =
		materialParam->Get<bool>(Name("normal_map_texture_from_bumpmap", id));

	colorTextureIntensity = materialParam->Get<double>(Name("color_texture_intensity", id));
	diffusionTextureIntensity = materialParam->Get<double>(Name("diffusion_texture_intensity", id));
	luminosityTextureIntensity = materialParam->Get<double>(Name("luminosity_texture_intensity", id));
	displacementTextureHeight = materialParam->Get<double>(Name("displacement_texture_height", id));
	normalMapTextureHeight = materialParam->Get<double>(Name("normal_map_texture_height", id));

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
		materialParam->Get<CVector3>(Name("fractal_coloring_line_direction", id));

	fractalColoring.color4dEnabledFalse =
		materialParam->Get<bool>(Name("fractal_coloring_color_4D_enabled_false", id));

	// trial
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
		materialParam->Get<double>(Name("fractal_coloring_orbit_trap_true", id));
	fractalColoring.orbitTrapWeight =
		materialParam->Get<double>(Name("fractal_coloring_orbit_trap_weight", id));
	fractalColoring.initialMinimumR =
		materialParam->Get<double>(Name("fractal_coloring_initial_minimumR", id));

	// aux.color
	fractalColoring.auxColorFalse =
		materialParam->Get<double>(Name("fractal_coloring_aux_color_false", id));
	fractalColoring.auxColorWeight =
		materialParam->Get<double>(Name("fractal_coloring_aux_color_weight", id));
	fractalColoring.auxColorHybridWeight =
		materialParam->Get<double>(Name("fractal_coloring_aux_color_hybrid_weight", id));

	// radius functions
	fractalColoring.radFalse =
		materialParam->Get<bool>(Name("fractal_coloring_rad_enabled_false", id));
	fractalColoring.radWeight = materialParam->Get<double>(Name("fractal_coloring_rad_weight", id));
	fractalColoring.radSquaredFalse =
		materialParam->Get<bool>(Name("fractal_coloring_rad_squared_enabled_false", id));
	fractalColoring.radDiv1e13False =
		materialParam->Get<bool>(Name("fractal_coloring_rad_div_1e13_false", id));

	fractalColoring.radDivDeFalse =
		materialParam->Get<bool>(Name("fractal_coloring_rad_div_de_enabled_false", id));
	fractalColoring.radDivLogDeFalse =
		materialParam->Get<bool>(Name("fractal_coloring_rad_div_log_de_false", id));
	fractalColoring.radDivDeWeight =
		materialParam->Get<double>(Name("fractal_coloring_rad_div_de_weight", id));
	fractalColoring.radDivDeSquaredFalse =
		materialParam->Get<bool>(Name("fractal_coloring_rad_div_de_squared_false", id));

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

	if (gNetRender->IsClient())
	{
		if (useColorTexture)
			colorTexture.FromQByteArray(
				gNetRender->GetTexture(materialParam->Get<QString>(Name("file_color_texture", id))),
				cTexture::useMipmaps);

		if (useDiffusionTexture)
			diffusionTexture.FromQByteArray(
				gNetRender->GetTexture(materialParam->Get<QString>(Name("file_diffusion_texture", id))),
				cTexture::useMipmaps);

		if (useLuminosityTexture)
			luminosityTexture.FromQByteArray(
				gNetRender->GetTexture(materialParam->Get<QString>(Name("file_luminosity_texture", id))),
				cTexture::useMipmaps);

		if (useDisplacementTexture)
			displacementTexture.FromQByteArray(
				gNetRender->GetTexture(materialParam->Get<QString>(Name("file_displacement_texture", id))),
				cTexture::doNotUseMipmaps);

		if (useNormalMapTexture)
		{
			normalMapTexture.FromQByteArray(
				gNetRender->GetTexture(materialParam->Get<QString>(Name("file_normal_map_texture", id))),
				cTexture::doNotUseMipmaps);

			if (materialParam->Get<bool>(Name("normal_map_texture_invert_green", id)))
				normalMapTexture.SetInvertGreen(true);
		}
	}
	else
	{
		if (useColorTexture)
			colorTexture = cTexture(
				materialParam->Get<QString>(Name("file_color_texture", id)), cTexture::useMipmaps, quiet);

		if (useDiffusionTexture)
			diffusionTexture = cTexture(materialParam->Get<QString>(Name("file_diffusion_texture", id)),
				cTexture::useMipmaps, quiet);

		if (useLuminosityTexture)
			luminosityTexture = cTexture(materialParam->Get<QString>(Name("file_luminosity_texture", id)),
				cTexture::useMipmaps, quiet);

		if (useDisplacementTexture)
			displacementTexture =
				cTexture(materialParam->Get<QString>(Name("file_displacement_texture", id)),
					cTexture::doNotUseMipmaps, quiet);

		if (useNormalMapTexture)
		{
			normalMapTexture = cTexture(materialParam->Get<QString>(Name("file_normal_map_texture", id)),
				cTexture::useMipmaps, quiet);
			if (materialParam->Get<bool>(Name("normal_map_texture_invert_green", id)))
				normalMapTexture.SetInvertGreen(true);
		}
	}

	rotMatrix.SetRotation2(textureRotation / 180 * M_PI);
}

void CreateMaterialsMap(
	const cParameterContainer *params, QMap<int, cMaterial> *materials, bool quiet)
{
	QList<QString> listOfParameters = params->GetListOfParameters();
	for (int i = 0; i < listOfParameters.size(); i++)
	{
		QString parameterName = listOfParameters.at(i);
		if (parameterName.left(3) == "mat")
		{
			int positionOfDash = parameterName.indexOf('_');
			int matIndex = parameterName.mid(3, positionOfDash - 3).toInt();
			QString shortName = parameterName.mid(positionOfDash + 1);
			if (shortName == "is_defined")
			{
				materials->insert(matIndex, cMaterial(matIndex, params, quiet));
			}
		}
	}
}
