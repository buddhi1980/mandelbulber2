/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cSettings class - version compatibility handlers for settings files
 *
 * Implements the compatibility part of cSettings (declared in settings.hpp):
 * Compatibility(), Compatibility2(), Compatibility3() and the helpers used by them.
 * It is a separate translation unit of the same class, not a separate class.
 */

#include "settings.hpp"

#include <QSet>
#include <QString>
#include <QStringList>

#include "animation_frames.hpp"
#include "fractal_container.hpp"
#include "fractal_enums.h"
#include "initparameters.hpp"
#include "keyframes.hpp"
#include "light.h"
#include "material.h"
#include "object_node_type.h"
#include "projection_3d.hpp"
#include "write_log.hpp"

/**
 * Compatibility handler for settings files older than v2.25.
 * Converts old light rotation parameters (alpha/beta) into the new unified rotation vector.
 */
void cSettings::Compatibility3(const std::shared_ptr<cKeyframes> &keyframes,
	const std::shared_ptr<cParameterContainer> &par,
	const std::shared_ptr<cFractalContainer> &fractPar)
{
	if (fileVersion < 2.25 && keyframes)
	{
		// Convert old alpha/beta light rotation parameters to the new rotation vector
		QList<cAnimationFrames::sParameterDescription> listOfAnimatedParameters =
			keyframes->GetListOfParameters();
		for (const auto &animatedParameter : listOfAnimatedParameters)
		{
			QString parameterName = animatedParameter.parameterName;

			if (parameterName == "light1_alpha" || parameterName == "light1_beta")
			{
				keyframes->AddAnimatedParameter("main_light1_rotation", par, fractPar);
				int numberOfKeyframes = keyframes->GetNumberOfFrames();
				for (int i = 0; i < numberOfKeyframes; i++)
				{
					cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(i);
					frame.parameters.Set(
						"main_light1_rotation", CVector3(frame.parameters.Get<double>("main_light1_alpha"),
																			frame.parameters.Get<double>("main_light1_beta"), 0.0));
					keyframes->ModifyFrame(i, frame);
				}
				keyframes->RemoveAnimatedParameter("main_light1_alpha");
				keyframes->RemoveAnimatedParameter("main_light1_beta");
				break;
			}
		}
	}
}

/**
 * Apply version-specific compatibility transformations to parameter names and values.
 * Handles renames, value conversions, and deprecated parameter handling across all
 * Mandelbulber versions. Parameters marked "skip" are ignored by the caller.
 */
void cSettings::Compatibility(QString &name, QString &value) const
{
	// === v2.01 compatibility ===
	if (fileVersion <= 2.01)
	{
		if (name.indexOf("aux_light_predefined") >= 0)
		{
			name.replace("aux_light_predefined", "aux_light");
		}

		if (name == QString("volumetric_light_intensity_0"))
		{
			name = QString("main_light_volumetric_intensity");
		}
		else if (name == QString("volumetric_light_enabled_0"))
		{
			name = QString("main_light_volumetric_enabled");
		}
		else if (name.indexOf("volumetric_light") >= 0)
		{
			name.replace("volumetric_light", "aux_light_volumetric");
		}
	}

	// === v2.04 compatibility ===
	if (fileVersion <= 2.04)
	{
		if (name == QString("fractal_constant_factor"))
		{
			QString newValue = value + " " + value + " " + value;
			value = newValue;
		}
	}

	// === v2.06 compatibility ===
	if (fileVersion <= 2.06)
	{
		if (name == QString("linear_DE_mode"))
		{
			name = QString("delta_DE_function");
			value = QString("1");
		}
	}

	// === v2.071 compatibility: rename material parameters to mat1_* prefix ===
	if (fileVersion <= 2.071)
	{
		if (name == QString("shading")) name.replace("shading", "mat1_shading");
		if (name == QString("specular")) name.replace("specular", "mat1_specular");
		if (name == QString("reflect")) name.replace("reflect", "mat1_reflectance");
		if (name == QString("transparency_of_surface"))
			name.replace("transparency_of_surface", "mat1_transparency_of_surface");
		if (name == QString("transparency_of_interior"))
			name.replace("transparency_of_interior", "mat1_transparency_of_interior");
		if (name == QString("transparency_index_of_refraction"))
			name.replace("transparency_index_of_refraction", "mat1_transparency_index_of_refraction");
		if (name == QString("transparency_interior_color"))
			name.replace("transparency_interior_color", "mat1_transparency_interior_color");
		if (name == QString("fresnel_reflectance"))
			name.replace("fresnel_reflectance", "mat1_fresnel_reflectance");
		if (name == QString("coloring_random_seed"))
			name.replace("coloring_random_seed", "mat1_coloring_random_seed");
		if (name == QString("coloring_saturation"))
			name.replace("coloring_saturation", "mat1_coloring_saturation");
		if (name == QString("coloring_speed")) name.replace("coloring_speed", "mat1_coloring_speed");
		if (name == QString("coloring_palette_size"))
			name.replace("coloring_palette_size", "mat1_coloring_palette_size");
		if (name == QString("coloring_palette_offset"))
			name.replace("coloring_palette_offset", "mat1_coloring_palette_offset");
		if (name == QString("fractal_color"))
			name.replace("fractal_color", "mat1_use_colors_from_palette");
		if (name == QString("surface_color_palette"))
			name.replace("surface_color_palette", "mat1_surface_color_palette");
		if (name == QString("fractal_coloring_algorithm"))
			name.replace("fractal_coloring_algorithm", "mat1_fractal_coloring_algorithm");
		if (name == QString("fractal_coloring_sphere_radius"))
			name.replace("fractal_coloring_sphere_radius", "mat1_fractal_coloring_sphere_radius");
		if (name == QString("fractal_coloring_line_direction"))
			name.replace("fractal_coloring_line_direction", "mat1_fractal_coloring_line_direction");
	}

	// === v2.09 compatibility: swap delta_DE_function values ===
	if (fileVersion < 2.09)
	{
		if (name == QString("delta_DE_function"))
		{
			if (value == QString("0"))
			{
				value = "2";
			}
			else if (value == QString("2"))
			{
				value = "0";
			}
		}
	}

	// === v2.12 compatibility: rename gpu_ to opencl_ ===
	if (fileVersion < 2.12)
	{
		name.replace("gpu_", "opencl_");
	}

	// === v2.13 compatibility: primitive_water amplitude rename ===
	if (fileVersion < 2.13)
	{
		if (name.contains("primitive_water"))
		{
			name.replace("amplitude", "relative_amplitude");
		}
	}

	// === v2.19 compatibility: surface_color_palette -> surface_color_gradient ===
	if (fileVersion < 2.19)
	{
		if (name.contains("surface_color_palette"))
		{
			name.replace("surface_color_palette", "surface_color_gradient");

			QStringList split = value.split(" ");
			int numberOfColors = split.size();

			// Skip trailing invalid color entries
			if (split.last().size() < 6) numberOfColors -= 1;

			double step = 1.0 / numberOfColors;
			QString newValue;
			for (int i = 0; i < numberOfColors; i++)
			{
				int pos = int(i * step * 10000.0);
				if (i > 0) newValue += " ";
				newValue += QString("%1 %2").arg(pos).arg(split[i]);
			}
			value = newValue;
		}

		if (name.contains("luminosity_color_thesame"))
			name.replace("luminosity_color_thesame", "luminosity_gradient_enable");
		if (name.contains("reflections_color_thesame"))
			name.replace("reflections_color_thesame", "reflectance_gradient_enable");
		if (name.contains("transparency_color_thesame"))
			name.replace("transparency_color_thesame", "transparency_gradient_enable");
		if (name.contains("coloring_palette_size")) name = "skip";
		if (name.contains("coloring_random_seed")) name = "skip";
		if (name.contains("coloring_saturation")) name = "skip";
	}

	// === v2.25 compatibility: rename main_light_* to light1_*, aux_light_* to light2_*+ ===
	if (fileVersion < 2.25)
	{
		if (name.contains("main_light_intensity"))
			name.replace("main_light_intensity", "light1_intensity");
		if (name == QString("main_light_visibility"))
			name.replace("main_light_visibility", "light1_visibility");
		if (name.contains("main_light_visibility_size"))
			name.replace("main_light_visibility_size", "light1_size");
		if (name.contains("main_light_contour_sharpness"))
			name.replace("main_light_contour_sharpness", "light1_contour_sharpness");
		if (name.contains("main_light_alpha")) name.replace("main_light_alpha", "light1_alpha");
		if (name.contains("main_light_beta")) name.replace("main_light_beta", "light1_beta");
		if (name.contains("main_light_colour")) name.replace("main_light_colour", "light1_color");
		if (name.contains("penetrating_lights"))
			name.replace("penetrating_lights", "light1_penetrating");
		if (name.contains("shadows_enabled")) name.replace("shadows_enabled", "light1_cast_shadows");
		if (name.contains("shadows_cone_angle"))
			name.replace("shadows_cone_angle", "light1_soft_shadow_cone");
		if (name.contains("main_light_enable")) name.replace("main_light_enable", "light1_enabled");
		if (name.contains("main_light_position_relative"))
			name.replace("main_light_position_relative", "light1_relative_position");
		if (name.contains("main_light_volumetric_intensity"))
			name.replace("main_light_volumetric_intensity", "light1_volumetric_visibility");
		if (name.contains("main_light_volumetric_enabled"))
			name.replace("main_light_volumetric_enabled", "light1_volumetric");

		// Handle aux_light_* -> light2_*+ renaming
		if (name.contains("aux_light"))
		{
			if (name.contains("visibility_size"))
				name.replace("aux_light_visibility_size", "light2_size");
			else if (name.contains("visibility"))
				name.replace("aux_light_visibility", "light2_visibility");
			else if (name.contains("aux_light_place_behind"))
			{
				// No mapping for this parameter
			}
			else
			{
				QStringList split = name.split("_");
				int lightIndex = split.last().toInt();
				QString prefix = QString("light%1").arg(lightIndex + 1);

				if (split.at(2) == "intensity")
					name = QString("%1_intensity").arg(prefix);
				else if (split.at(2) == "position")
					name = QString("%1_position").arg(prefix);
				else if (split.at(2) == "enabled")
					name = QString("%1_enabled").arg(prefix);
				else if (split.at(2) == "colour")
					name = QString("%1_color").arg(prefix);
				else if (split.at(2) == "volumetric")
				{
					if (split.at(3) == "intensity")
						name = QString("%1_volumetric_visibility").arg(prefix);
					else if (split.at(3) == "enabled")
						name = QString("%1_volumetric").arg(prefix);
				}
			}
		}
	}

	// === v2.28 compatibility: rename random_lights_one_color_enable ===
	if (fileVersion < 2.28)
	{
		if (name == QString("random_lights_one_color_enable"))
		{
			name = QString("random_lights_coloring_type");
			if (value.contains("true"))
			{
				value = QString("single");
			}
		}
	}
}

/**
 * Apply version-specific compatibility migrations for settings files.
 * Handles DE function normalization, primitive-to-material migration, gradient renames,
 * FOV conversion, light parameter migrations, and objects tree migration (v2.35+).
 */
void cSettings::Compatibility2(
	std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> fract)
{
	// === v2.06: normalize DE function, enable IFS rotation ===
	if (fileVersion <= 2.06)
	{
		if (fractal::enumDEFunctionType(par->Get<int>("delta_DE_function"))
				!= fractal::linearDEFunction)
			par->Set("delta_DE_function", int(fractal::logarithmicDEFunction));

		if (fract)
		{
			for (int i = 0; i < 4; i++)
			{
				fract->at(i)->Set("IFS_rotation_enabled", true);
			}
		}
	}

	// === v2.071: migrate loaded primitives to materials ===
	if (fileVersion <= 2.071)
	{
		for (int i = 0; i < listOfLoadedPrimitives.size(); i++)
		{
			int materialId = i + 2;
			InitMaterialParams(materialId, par);
			par->Set(cMaterial::Name("is_defined", materialId), true);
			par->Set(cMaterial::Name("name", materialId), listOfLoadedPrimitives[i]);
			par->Set(cMaterial::Name("surface_color", materialId),
				par->Get<sRGB>(listOfLoadedPrimitives[i] + "_color"));
			par->Set(cMaterial::Name("reflectance", materialId),
				par->Get<double>(listOfLoadedPrimitives[i] + "_reflection"));
			par->Set(cMaterial::Name("use_colors_from_palette", materialId), false);
			par->Set(cMaterial::Name("fresnel_reflectance", materialId),
				par->Get<bool>("mat1_fresnel_reflectance"));
			par->Set(cMaterial::Name("transparency_index_of_refraction", materialId),
				par->Get<double>("mat1_transparency_index_of_refraction"));
			par->Set(cMaterial::Name("transparency_of_surface", materialId),
				par->Get<double>("mat1_transparency_of_surface"));
			par->Set(cMaterial::Name("transparency_of_interior", materialId),
				par->Get<double>("mat1_transparency_of_interior"));
			par->Set(cMaterial::Name("transparency_interior_color", materialId),
				par->Get<double>("mat1_transparency_interior_color"));
			par->Set(cMaterial::Name("specular", materialId), par->Get<double>("mat1_specular"));
			par->Set(cMaterial::Name("shading", materialId), par->Get<double>("mat1_shading"));
			par->Set(listOfLoadedPrimitives[i] + "_material_id", materialId);
		}
	}

	// === v2.12: iteration fog boost, water relative amplitude fix ===
	if (fileVersion <= 2.12)
	{
		if (par->Get<bool>("iteration_fog_enable"))
		{
			par->Set("iteration_fog_brightness_boost", 100.0);
		}
		for (int i = 0; i < listOfLoadedPrimitives.size(); i++)
		{
			if (listOfLoadedPrimitives[i].contains("primitive_water"))
			{
				double relativeAmplitude =
					par->Get<double>(listOfLoadedPrimitives[i] + "_relative_amplitude")
					/ par->Get<double>(listOfLoadedPrimitives[i] + "_length");
				par->Set(listOfLoadedPrimitives[i] + "_relative_amplitude", relativeAmplitude);
			}
		}
	}

	// === v2.19: palette offset/speed normalization, gradient migration ===
	if (fileVersion < 2.19)
	{
		QSet<QString> materialList;
		QStringList listOfParameters = par->GetListOfParameters();
		for (QString paramName : listOfParameters)
		{
			if (paramName.left(3) == "mat" && paramName.contains("is_defined"))
			{
				int firstDash = paramName.indexOf("_");
				materialList.insert(paramName.left(firstDash));
			}
		}

		for (QString mat : materialList)
		{
			QString coloringOffsetParameter = mat + "_coloring_palette_offset";
			QString colorPaletteParameter = mat + "_surface_color_gradient";
			QString coloringSpeedParameter = mat + "_coloring_speed";
			int paletteSize = par->Get<QString>(colorPaletteParameter).split(" ").size() / 2;
			par->Set(coloringOffsetParameter, par->Get<double>(coloringOffsetParameter) / paletteSize);
			par->Set(
				coloringSpeedParameter, par->Get<double>(coloringSpeedParameter) * 10.0 / paletteSize);

			if (par->Get<bool>(mat + "_luminosity_gradient_enable"))
				par->Set(mat + "_luminosity_gradient", par->Get<QString>(mat + "_surface_color_gradient"));
			if (par->Get<bool>(mat + "_reflectance_gradient_enable"))
				par->Set(mat + "_reflectance_gradient", par->Get<QString>(mat + "_surface_color_gradient"));
			if (par->Get<bool>(mat + "_transparency_gradient_enable"))
				par->Set(
					mat + "_transparency_gradient", par->Get<QString>(mat + "_surface_color_gradient"));
		}
	}

	// === v2.20: delta_DE_method -> delta_DE_function mapping ===
	if (fileVersion < 2.20)
	{
		if (par->IfExists("delta_DE_method"))
		{
			if (fractal::enumDEMethod(par->Get<int>("delta_DE_method")) == fractal::preferredDEMethod)
			{
				par->Set("delta_DE_function", int(fractal::preferredDEFunction));
			}
		}
	}

	// === v2.21: FOV conversion (radians -> degrees) ===
	if (fileVersion < 2.21)
	{
		params::enumPerspectiveType perspectiveType =
			params::enumPerspectiveType(par->Get<int>("perspective_type"));
		double fov = par->Get<double>("fov");
		if (fov == 53.13) fov = 1.0;

		double fovDegrees = 0.0;
		switch (perspectiveType)
		{
			case params::perspThreePoint: fovDegrees = atan(fov / 2.0) * 360.0 / M_PI; break;
			case params::perspFishEye:
			case params::perspFishEyeCut: fovDegrees = fov * 180.0; break;
			case params::perspEquirectangular: fovDegrees = fov * 360.0; break;
		}
		par->Set("fov", fovDegrees);
	}

	// === v2.22: enable IFS edge if non-zero ===
	if (fileVersion < 2.22)
	{
		if (fract)
		{
			for (int i = 0; i < fract->size(); i++)
			{
				if (fract->at(i)->Get<CVector3>("IFS_edge").Length() > 0)
				{
					fract->at(i)->Set("IFS_edge_enabled", true);
				}
			}
		}
	}

	// === v2.25: light1 rotation vector, light intensity/size corrections, random lights ===
	if (fileVersion < 2.25)
	{
		if (par->IfExists("light1_is_defined"))
		{
			par->Set("light1_rotation",
				CVector3(par->Get<double>("light1_alpha"), par->Get<double>("light1_beta"), 0.0));
		}
		else
		{
			InitLightParams(1, par);
			par->Set("light1_enabled", true);
			par->Set("light1_is_defined", true);
		}

		// Copy light visibility and intensity from light2 to other aux lights
		if (par->IfExists("light2_is_defined"))
		{
			// light2_* parameters are created lazily, so a file may define the flag without
			// the individual values. Fall back to the light defaults instead of reading
			// parameters which are not present in the container.
			double light2Visibility =
				par->IfExists("light2_visibility") ? par->Get<double>("light2_visibility") : 1.0;
			double light2Size = par->IfExists("light2_size") ? par->Get<double>("light2_size") : 0.5;
			for (int i = 3; i <= 5; i++)
			{
				if (par->IfExists(QString("light%1_is_defined").arg(i)))
				{
					par->Set(QString("light%1_visibility").arg(i), light2Visibility);
					par->Set(QString("light%1_size").arg(i), light2Size);
				}
			}
		}

		// Correct intensity and size for aux lights
		for (int i = 2; i <= 5; i++)
		{
			if (par->IfExists(QString("light%1_is_defined").arg(i)))
			{
				if (!par->isDefaultValue(QString("light%1_intensity").arg(i)))
				{
					par->Set(QString("light%1_intensity").arg(i),
						par->Get<double>(QString("light%1_intensity").arg(i)) / 4.0);
				}
				if (!par->isDefaultValue(QString("light%1_size").arg(i)))
				{
					par->Set(
						QString("light%1_size").arg(i), par->Get<double>(QString("light%1_size").arg(i)) * 2.0);
				}
				if (par->Get<double>(QString("light%1_volumetric").arg(i)))
				{
					par->Set(QString("light%1_volumetric_visibility").arg(i),
						par->Get<double>(QString("light%1_volumetric_visibility").arg(i))
							/ par->Get<double>(QString("light%1_intensity").arg(i)));
				}
				par->Set(QString("light%1_penetrating").arg(i), par->Get<bool>("light1_penetrating"));
				par->Set(QString("light%1_cast_shadows").arg(i), par->Get<bool>("light1_cast_shadows"));
			}
		}

		// Correct random lights intensity and size
		if (par->Get<bool>("random_lights_group"))
		{
			int numberOfRandomLights = par->Get<int>("random_lights_number");
			// the aux light size may be undefined in the file, use the light default then
			double refLightSize = par->IfExists("light2_size") ? par->Get<double>("light2_size") : 0.5;
			par->Set("random_lights_intensity",
				par->Get<double>("random_lights_intensity") / (numberOfRandomLights / 4 + 4));
			par->Set("random_lights_size", refLightSize * (sqrt(numberOfRandomLights) / 4));
		}
	}

	// === v2.28: adjust light1 intensity for iteration fog ===
	if (fileVersion < 2.28)
	{
		if (par->IfExists(cLight::Name("is_defined", 1)))
		{
			if (par->Get<bool>("iteration_fog_enable"))
			{
				double iterFogBoost = par->Get<double>("iteration_fog_brightness_boost");
				par->Set(cLight::Name("intensity", 1),
					par->Get<double>(cLight::Name("intensity", 1)) / iterFogBoost);
			}
		}
	}

	// === v2.29: migrate boolean_operators to per-fractal formula_maxiter ===
	// Write directly to fractal containers since cParameterContainer doesn't support
	// dynamic parameter creation on the main container.
	if (fileVersion < 2.29)
	{
		if (par->IfExists("boolean_operators") && par->Get<bool>("boolean_operators"))
		{
			int maxiter = par->Get<int>("N");
			for (int i = 0; i < fract->size(); i++)
			{
				fract->at(i)->Set("formula_maxiter", maxiter);
			}
		}
	}

	// === v2.35: migrate to objects tree (node-based hierarchy) ===
	if (fileVersion < 2.35)
	{
		if (fract)
		{
			DeleteAllNodeParams(par);
			int nextGroupObjectId = 100;
			MigrateToObjectsTree(par, fract, nextGroupObjectId);
			FlattenBooleanAddGroups(par, nextGroupObjectId);
		}
		DeleteTemporaryLegacyPrimitiveTransformParams(par);
		DeleteTemporaryLegacyPrimitiveNodeParams(par);
	}
}

struct sLegacyAnimMap
{
	const char *prefix;
	const char *baseName;
};

static const sLegacyAnimMap gLegacyAnimPrefixes[] = {
	{"main_formula_weight_", "formula_weight"},
	{"main_formula_iterations_", "formula_iterations"},
	{"dont_add_c_constant_", "dont_add_c_constant"},
	{"fractal_constant_factor_", "fractal_constant_factor"},
	{"julia_mode_", "julia_mode"},
	{"julia_c_", "julia_c"},
};

QString cSettings::ConvertLegacyAnimationParamName(const QString &oldName)
{
	// Convert legacy animation parameter names to new format (fileVersion < 2.35)
	// Mirrors MigrateLegacyParamsToFractal() in Compatibility2()
	// Pattern: oldName_N -> fractalN_newName
	for (const auto &entry : gLegacyAnimPrefixes)
	{
		QString prefix = entry.prefix;
		if (oldName.startsWith(prefix))
		{
			int lastUnderscore = oldName.lastIndexOf('_');
			int fractalIndex = oldName.mid(lastUnderscore + 1).toInt();
			return QString("fractal%1_%2").arg(fractalIndex).arg(entry.baseName);
		}
	}
	return oldName;
}

void cSettings::PreCompatibilityMaterials(int matIndex, std::shared_ptr<cParameterContainer> par)
{
	// Sets default metallic/material params for file versions prior to 2.15.
	if (fileVersion < 2.15)
	{
		// Set default metallic value to false for backward compatibility
		par->Set(cMaterial::Name("metallic", matIndex), false);
		// Set default specular value to 1.0 for backward compatibility
		par->Set(cMaterial::Name("specular", matIndex), 1.0);
		// Set default specular_width value to 1.0 for backward compatibility
		par->Set(cMaterial::Name("specular_width", matIndex), 1.0);
	}
}

QStringList cSettings::GetLegacyPrimitiveTypes()
{
	// Returns the list of legacy primitive type names used in old file formats.
	return {"sphere", "box", "cylinder", "cone", "plane", "torus", "rectangle", "circle", "water",
		"prism", "ellipsoid"};
}
