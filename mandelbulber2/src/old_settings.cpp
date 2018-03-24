/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Rayan Hitchman
 *
 * cOldSettings class - importing settings from old Mandelbulber (v1.21)
 */

#include "old_settings.hpp"

#include "camera_target.hpp"
#include "error_message.hpp"
#include "fractal_container.hpp"
#include "fractal_list.hpp"
#include "fractparams.hpp"
#include "initparameters.hpp"
#include "system.hpp"

namespace oldSettings
{

const char *axis_names[] = {"X", "Y", "Z"};
const char *component_names[] = {"alfa", "beta", "gamma"};

cOldSettings::cOldSettings()
{
	oldData = new sParamRender;
	paletteLoadedFromSettingsFile = false;
}

cOldSettings::~cOldSettings()
{
	delete oldData;
}

bool cOldSettings::LoadSettings(const QString &filename)
{
	QString defaultsFilename =
		systemData.sharedDir + "data" + QDir::separator() + "mandelbulber_1.21_defaults.fract";

	if (QFileInfo::exists(defaultsFilename))
	{
		LoadSettings2(defaultsFilename);
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Can't import old settings\n")
																 + QObject::tr("File with default values doesn't exist\n")
																 + defaultsFilename,
			cErrorMessage::errorMessage);
		return false;
	}
	return LoadSettings2(filename);
}

bool cOldSettings::LoadSettings2(const QString &filename)
{
	paletteLoadedFromSettingsFile = false;

	char str1[100];
	char str2[2000];

	FILE *fileSettings;
	fileSettings = fopen(filename.toLocal8Bit(), "r");

	int lineCounter = 0;

	oldData->settingsVersion = -1;

	if (fileSettings)
	{
		while (!feof(fileSettings))
		{
			lineCounter++;
			int c = fscanf(fileSettings, "%s", str1);
			if (c > 0)
			{
				c = fscanf(fileSettings, "%[ ]", str2);
				c = fscanf(fileSettings, "%[^;]", str2);

				LoadOneSetting(str1, str2, oldData);
				c = fscanf(fileSettings, "%[^\n]", str2);
			}
		}
		fclose(fileSettings);

		return true;
	}
	else
	{
		// printf("Can't open settings file: %s\n", filename);
		return false;
	}
}

double cOldSettings::atof2(const char *str)
{
	QString string(str);
	string = string.replace(',', '.');
	return string.toDouble();
}

void cOldSettings::GetPaletteFromString(sRGB *palette, const char *paletteString)
{
	int index = 0;
	for (int i = 0; i < 2000; i++)
	{
		int colour = 0;
		sscanf(&paletteString[i], "%x", reinterpret_cast<unsigned int *>(&colour));
		sRGB rgbColour;
		rgbColour.R = colour / 65536;
		rgbColour.G = (colour / 256) % 256;
		rgbColour.B = colour % 256;
		palette[index] = rgbColour;
		// printf("R = %d, G = %d, B = %d\n", rgbColour.R, rgbColour.G, rgbColour.B);

		while (i < 2000 && paletteString[i] != ' ')
		{
			i++;
		}
		index++;
		if (index == 256)
		{
			paletteLoadedFromSettingsFile = true;
			break;
		}
	}
}

bool cOldSettings::LoadOneSetting(const char *str1, const char *str2, sParamRender *params)
{
	// ****** this function is taken directly from Mandelbulber v1.21 *******

	char str3[100];

	if (!strcmp(str1, "Mandelbulber"))
		params->settingsVersion = atof2(str2);
	else if (!strcmp(str1, "image_width"))
		params->image_width = atoi(str2);
	else if (!strcmp(str1, "image_height"))
		params->image_height = atoi(str2);
	else if (!strcmp(str1, "tiles"))
		params->noOfTiles = atoi(str2);
	else if (!strcmp(str1, "x_min"))
		params->fractal.doubles.amin = atof2(str2);
	else if (!strcmp(str1, "x_max"))
		params->fractal.doubles.amax = atof2(str2);
	else if (!strcmp(str1, "y_min"))
		params->fractal.doubles.bmin = atof2(str2);
	else if (!strcmp(str1, "y_max"))
		params->fractal.doubles.bmax = atof2(str2);
	else if (!strcmp(str1, "z_min"))
		params->fractal.doubles.cmin = atof2(str2);
	else if (!strcmp(str1, "z_max"))
		params->fractal.doubles.cmax = atof2(str2);
	else if (!strcmp(str1, "view_point_x"))
		params->doubles.vp.x = atof2(str2);
	else if (!strcmp(str1, "view_point_y"))
		params->doubles.vp.y = atof2(str2);
	else if (!strcmp(str1, "view_point_z"))
		params->doubles.vp.z = atof2(str2);
	else if (!strcmp(str1, "angle_alfa"))
		params->doubles.alpha = atof2(str2) / 180.0 * M_PI;
	else if (!strcmp(str1, "angle_beta"))
		params->doubles.beta = atof2(str2) / 180.0 * M_PI;
	else if (!strcmp(str1, "angle_gamma"))
		params->doubles.gamma = atof2(str2) / 180.0 * M_PI;
	else if (!strcmp(str1, "zoom"))
		params->doubles.zoom = atof2(str2);
	else if (!strcmp(str1, "perspective"))
		params->doubles.persp = atof2(str2);
	else if (!strcmp(str1, "formula"))
		params->fractal.formula = enumOldFractalFormula(atoi(str2));
	else if (!strcmp(str1, "power"))
		params->fractal.doubles.power = atof2(str2);
	else if (!strcmp(str1, "N"))
		params->fractal.doubles.N = atoi(str2);
	else if (!strcmp(str1, "minN"))
		params->fractal.minN = atoi(str2);
	else if (!strcmp(str1, "fractal_constant_factor"))
		params->fractal.doubles.constantFactor = atof2(str2);
	else if (!strcmp(str1, "quality"))
		params->doubles.quality = atof2(str2);
	else if (!strcmp(str1, "smoothness"))
		params->doubles.smoothness = atof2(str2);
	else if (!strcmp(str1, "julia_mode"))
		params->fractal.juliaMode = atoi(str2);
	else if (!strcmp(str1, "julia_a"))
		params->fractal.doubles.julia.x = atof2(str2);
	else if (!strcmp(str1, "julia_b"))
		params->fractal.doubles.julia.y = atof2(str2);
	else if (!strcmp(str1, "julia_c"))
		params->fractal.doubles.julia.z = atof2(str2);
	else if (!strcmp(str1, "tglad_folding_mode"))
		params->fractal.tgladFoldingMode = atoi(str2);
	else if (!strcmp(str1, "folding_limit"))
		params->fractal.doubles.foldingLimit = atof2(str2);
	else if (!strcmp(str1, "folding_value"))
		params->fractal.doubles.foldingValue = atof2(str2);
	else if (!strcmp(str1, "spherical_folding_mode"))
		params->fractal.sphericalFoldingMode = atoi(str2);
	else if (!strcmp(str1, "spherical_folding_fixed"))
		params->fractal.doubles.foldingSphericalFixed = atof2(str2);
	else if (!strcmp(str1, "spherical_folding_min"))
		params->fractal.doubles.foldingSphericalMin = atof2(str2);
	else if (!strcmp(str1, "iteration_threshold_mode"))
		params->fractal.iterThresh = atoi(str2);
	else if (!strcmp(str1, "analityc_DE_mode"))
		params->fractal.analyticDE = atoi(str2);
	else if (!strcmp(str1, "DE_factor"))
		params->doubles.DE_factor = atof2(str2);
	else if (!strcmp(str1, "brightness"))
		params->doubles.imageAdjustments.brightness = atof2(str2);
	else if (!strcmp(str1, "contrast"))
		params->doubles.imageAdjustments.contrast = atof2(str2);
	else if (!strcmp(str1, "gamma"))
		params->doubles.imageAdjustments.imageGamma = atof2(str2);
	else if (!strcmp(str1, "hdr"))
		params->imageSwitches.hdrEnabled = atoi(str2);
	else if (!strcmp(str1, "ambient"))
		params->doubles.imageAdjustments.ambient = atof2(str2);
	else if (!strcmp(str1, "reflect"))
		params->doubles.imageAdjustments.reflect = atof2(str2);
	else if (!strcmp(str1, "shadows_intensity"))
		params->doubles.imageAdjustments.directLight = atof2(str2);
	else if (!strcmp(str1, "shadows_cone_angle"))
		params->doubles.shadowConeAngle = atof2(str2);
	else if (!strcmp(str1, "ambient_occlusion"))
		params->doubles.imageAdjustments.globalIlum = atof2(str2);
	else if (!strcmp(str1, "ambient_occlusion_quality"))
		params->globalIlumQuality = atoi(str2);
	else if (!strcmp(str1, "ambient_occlusion_fast_tune"))
		params->doubles.fastAoTune = atof2(str2);
	else if (!strcmp(str1, "shading"))
		params->doubles.imageAdjustments.shading = atof2(str2);
	else if (!strcmp(str1, "specular"))
		params->doubles.imageAdjustments.specular = atof2(str2);
	else if (QString(str1).startsWith("glow_"))
	{
		if (!strcmp(str1, "glow_intensity"))
			params->doubles.imageAdjustments.glow_intensity = atof2(str2);
		else if (!strcmp(str1, "glow_color_1_R"))
			params->effectColours.glow_color1.R = atoi(str2);
		else if (!strcmp(str1, "glow_color_1_G"))
			params->effectColours.glow_color1.G = atoi(str2);
		else if (!strcmp(str1, "glow_color_1_B"))
			params->effectColours.glow_color1.B = atoi(str2);
		else if (!strcmp(str1, "glow_color_2_R"))
			params->effectColours.glow_color2.R = atoi(str2);
		else if (!strcmp(str1, "glow_color_2_G"))
			params->effectColours.glow_color2.G = atoi(str2);
		else if (!strcmp(str1, "glow_color_2_B"))
			params->effectColours.glow_color2.B = atoi(str2);
	}
	else if (QString(str1).startsWith("background_color_"))
	{
		if (!strcmp(str1, "background_color_1_R"))
			params->background_color1.R = atoi(str2);
		else if (!strcmp(str1, "background_color_1_G"))
			params->background_color1.G = atoi(str2);
		else if (!strcmp(str1, "background_color_1_B"))
			params->background_color1.B = atoi(str2);
		else if (!strcmp(str1, "background_color_2_R"))
			params->background_color2.R = atoi(str2);
		else if (!strcmp(str1, "background_color_2_G"))
			params->background_color2.G = atoi(str2);
		else if (!strcmp(str1, "background_color_2_B"))
			params->background_color2.B = atoi(str2);
		else if (!strcmp(str1, "background_color_3_R"))
			params->background_color3.R = atoi(str2);
		else if (!strcmp(str1, "background_color_3_G"))
			params->background_color3.G = atoi(str2);
		else if (!strcmp(str1, "background_color_3_B"))
			params->background_color3.B = atoi(str2);
	}
	else if (QString(str1).startsWith("fog_colour_"))
	{
		if (!strcmp(str1, "fog_colour_1_R"))
			params->fogColour1.R = atoi(str2);
		else if (!strcmp(str1, "fog_colour_1_G"))
			params->fogColour1.G = atoi(str2);
		else if (!strcmp(str1, "fog_colour_1_B"))
			params->fogColour1.B = atoi(str2);
		else if (!strcmp(str1, "fog_colour_2_R"))
			params->fogColour2.R = atoi(str2);
		else if (!strcmp(str1, "fog_colour_2_G"))
			params->fogColour2.G = atoi(str2);
		else if (!strcmp(str1, "fog_colour_2_B"))
			params->fogColour2.B = atoi(str2);
		else if (!strcmp(str1, "fog_colour_3_R"))
			params->fogColour3.R = atoi(str2);
		else if (!strcmp(str1, "fog_colour_3_G"))
			params->fogColour3.G = atoi(str2);
		else if (!strcmp(str1, "fog_colour_3_B"))
			params->fogColour3.B = atoi(str2);
	}
	else if (!strcmp(str1, "textured_background"))
		params->texturedBackground = atoi(str2);
	else if (!strcmp(str1, "background_as_fuldome"))
		params->background_as_fulldome = atoi(str2);
	else if (!strcmp(str1, "shadows_enabled"))
		params->shadow = atoi(str2);
	else if (!strcmp(str1, "ambient_occlusion_enabled"))
		params->global_ilumination = atoi(str2);
	else if (!strcmp(str1, "fast_ambient_occlusion_mode"))
		params->fastGlobalIllumination = atoi(str2);
	else if (!strcmp(str1, "fractal_color"))
		params->imageSwitches.coloringEnabled = atoi(str2);
	else if (!strcmp(str1, "coloring_random_seed"))
		params->coloring_seed = atoi(str2);
	else if (!strcmp(str1, "coloring_saturation"))
		params->doubles.colourSaturation = atof2(str2);
	else if (!strcmp(str1, "coloring_speed"))
		params->doubles.imageAdjustments.coloring_speed = atof2(str2);
	else if (!strcmp(str1, "coloring_palette_offset"))
		params->doubles.imageAdjustments.paletteOffset = atof2(str2);
	else if (!strcmp(str1, "slow_shading"))
		params->slowShading = atoi(str2);
	else if (!strcmp(str1, "limits_enabled"))
		params->fractal.limits_enabled = atoi(str2);
	else if (QString(str1).startsWith("post_"))
	{
		if (!strcmp(str1, "post_fog_enabled"))
			params->imageSwitches.fogEnabled = atoi(str2);
		else if (!strcmp(str1, "post_fog_visibility"))
			params->doubles.imageAdjustments.fogVisibility = atof2(str2);
		else if (!strcmp(str1, "post_fog_color_R"))
			params->effectColours.fogColor.R = atoi(str2);
		else if (!strcmp(str1, "post_fog_color_G"))
			params->effectColours.fogColor.G = atoi(str2);
		else if (!strcmp(str1, "post_fog_color_B"))
			params->effectColours.fogColor.B = atoi(str2);
		else if (!strcmp(str1, "post_SSAO_enabled"))
			params->SSAOEnabled = atoi(str2);
		else if (!strcmp(str1, "post_SSAO_quality"))
			params->SSAOQuality = atoi(str2);
		else if (!strcmp(str1, "post_DOF_enabled"))
			params->DOFEnabled = atoi(str2);
		else if (!strcmp(str1, "post_DOF_focus"))
			params->doubles.DOFFocus = atof2(str2);
		else if (!strcmp(str1, "post_DOF_radius"))
			params->doubles.DOFRadius = atof2(str2);
	}
	else if (QString(str1).startsWith("main_light_"))
	{
		if (!strcmp(str1, "main_light_intensity"))
			params->doubles.imageAdjustments.mainLightIntensity = atof2(str2);
		else if (!strcmp(str1, "main_light_alfa"))
			params->doubles.mainLightAlpha = atof2(str2);
		else if (!strcmp(str1, "main_light_beta"))
			params->doubles.mainLightBeta = atof2(str2);
		else if (!strcmp(str1, "main_light_colour_R"))
			params->effectColours.mainLightColour.R = atoi(str2);
		else if (!strcmp(str1, "main_light_colour_G"))
			params->effectColours.mainLightColour.G = atoi(str2);
		else if (!strcmp(str1, "main_light_colour_B"))
			params->effectColours.mainLightColour.B = atoi(str2);
	}
	else if (QString(str1).startsWith("aux_light_"))
	{
		if (!strcmp(str1, "aux_light_intensity"))
			params->doubles.auxLightIntensity = atof2(str2);
		else if (!strcmp(str1, "aux_light_random_seed"))
			params->auxLightRandomSeed = atoi(str2);
		else if (!strcmp(str1, "aux_light_number"))
			params->auxLightNumber = atoi(str2);
		else if (!strcmp(str1, "aux_light_max_dist"))
			params->doubles.auxLightMaxDist = atof2(str2);
		else if (!strcmp(str1, "aux_light_distribution_radius"))
			params->doubles.auxLightDistributionRadius = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_1_x"))
			params->doubles.auxLightPre[0].x = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_1_y"))
			params->doubles.auxLightPre[0].y = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_1_z"))
			params->doubles.auxLightPre[0].z = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_1_intensity"))
			params->doubles.auxLightPreIntensity[0] = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_2_x"))
			params->doubles.auxLightPre[1].x = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_2_y"))
			params->doubles.auxLightPre[1].y = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_2_z"))
			params->doubles.auxLightPre[1].z = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_2_intensity"))
			params->doubles.auxLightPreIntensity[1] = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_3_x"))
			params->doubles.auxLightPre[2].x = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_3_y"))
			params->doubles.auxLightPre[2].y = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_3_z"))
			params->doubles.auxLightPre[2].z = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_3_intensity"))
			params->doubles.auxLightPreIntensity[2] = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_4_x"))
			params->doubles.auxLightPre[3].x = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_4_y"))
			params->doubles.auxLightPre[3].y = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_4_z"))
			params->doubles.auxLightPre[3].z = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_4_intensity"))
			params->doubles.auxLightPreIntensity[3] = atof2(str2);
		else if (!strcmp(str1, "aux_light_predefined_1_enabled"))
			params->auxLightPreEnabled[0] = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_2_enabled"))
			params->auxLightPreEnabled[1] = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_3_enabled"))
			params->auxLightPreEnabled[2] = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_4_enabled"))
			params->auxLightPreEnabled[3] = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_1_colour_R"))
			params->auxLightPreColour[0].R = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_1_colour_G"))
			params->auxLightPreColour[0].G = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_1_colour_B"))
			params->auxLightPreColour[0].B = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_2_colour_R"))
			params->auxLightPreColour[1].R = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_2_colour_G"))
			params->auxLightPreColour[1].G = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_2_colour_B"))
			params->auxLightPreColour[1].B = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_3_colour_R"))
			params->auxLightPreColour[2].R = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_3_colour_G"))
			params->auxLightPreColour[2].G = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_3_colour_B"))
			params->auxLightPreColour[2].B = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_4_colour_R"))
			params->auxLightPreColour[3].R = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_4_colour_G"))
			params->auxLightPreColour[3].G = atoi(str2);
		else if (!strcmp(str1, "aux_light_predefined_4_colour_B"))
			params->auxLightPreColour[3].B = atoi(str2);
		else if (!strcmp(str1, "aux_light_visibility"))
			params->doubles.auxLightVisibility = atof2(str2);
		else if (!strcmp(str1, "aux_light_random_center_X"))
			params->doubles.auxLightRandomCenter.x = atof2(str2);
		else if (!strcmp(str1, "aux_light_random_center_Y"))
			params->doubles.auxLightRandomCenter.y = atof2(str2);
		else if (!strcmp(str1, "aux_light_random_center_Z"))
			params->doubles.auxLightRandomCenter.z = atof2(str2);
	}
	else if (QString(str1).startsWith("IFS_"))
	{
		if (!strcmp(str1, "IFS_scale"))
			params->fractal.IFS.doubles.scale = atof2(str2);
		else if (!strcmp(str1, "IFS_rot_alfa"))
			params->fractal.IFS.doubles.rotationAlfa = atof2(str2);
		else if (!strcmp(str1, "IFS_rot_beta"))
			params->fractal.IFS.doubles.rotationBeta = atof2(str2);
		else if (!strcmp(str1, "IFS_rot_gamma"))
			params->fractal.IFS.doubles.rotationGamma = atof2(str2);
		else if (!strcmp(str1, "IFS_offsetX"))
			params->fractal.IFS.doubles.offset.x = atof2(str2);
		else if (!strcmp(str1, "IFS_offsetY"))
			params->fractal.IFS.doubles.offset.y = atof2(str2);
		else if (!strcmp(str1, "IFS_offsetZ"))
			params->fractal.IFS.doubles.offset.z = atof2(str2);
		else if (!strcmp(str1, "IFS_edgeX"))
			params->fractal.IFS.doubles.edge.x = atof2(str2);
		else if (!strcmp(str1, "IFS_edgeY"))
			params->fractal.IFS.doubles.edge.y = atof2(str2);
		else if (!strcmp(str1, "IFS_edgeZ"))
			params->fractal.IFS.doubles.edge.z = atof2(str2);
		else if (!strcmp(str1, "IFS_absX"))
			params->fractal.IFS.absX = atof(str2);
		else if (!strcmp(str1, "IFS_absY"))
			params->fractal.IFS.absY = atof(str2);
		else if (!strcmp(str1, "IFS_absZ"))
			params->fractal.IFS.absZ = atof(str2);
		else if (!strcmp(str1, "IFS_menger_sponge_mode"))
			params->fractal.IFS.mengerSpongeMode = atof(str2);
		else if (!strcmp(str1, "IFS_folding_mode"))
			params->fractal.IFS.foldingMode = atoi(str2);
		else
		{
			for (int i = 0; i < IFS_VECTOR_COUNT; i++)
			{
				sprintf(str3, "IFS_%d_x", i);
				if (!strcmp(str1, str3))
				{
					params->fractal.IFS.doubles.direction[i].x = atof2(str2);
					break;
				}
				sprintf(str3, "IFS_%d_y", i);
				if (!strcmp(str1, str3))
				{
					params->fractal.IFS.doubles.direction[i].y = atof2(str2);
					break;
				}
				sprintf(str3, "IFS_%d_z", i);
				if (!strcmp(str1, str3))
				{
					params->fractal.IFS.doubles.direction[i].z = atof2(str2);
					break;
				}
				sprintf(str3, "IFS_%d_alfa", i);
				if (!strcmp(str1, str3))
				{
					params->fractal.IFS.doubles.alfa[i] = atof2(str2);
					break;
				}
				sprintf(str3, "IFS_%d_beta", i);
				if (!strcmp(str1, str3))
				{
					params->fractal.IFS.doubles.beta[i] = atof2(str2);
					break;
				}
				sprintf(str3, "IFS_%d_gamma", i);
				if (!strcmp(str1, str3))
				{
					params->fractal.IFS.doubles.gamma[i] = atof2(str2);
					break;
				}
				sprintf(str3, "IFS_%d_distance", i);
				if (!strcmp(str1, str3))
				{
					params->fractal.IFS.doubles.distance[i] = atof2(str2);
					break;
				}
				sprintf(str3, "IFS_%d_intensity", i);
				if (!strcmp(str1, str3))
				{
					params->fractal.IFS.doubles.intensity[i] = atof2(str2);
					break;
				}
				sprintf(str3, "IFS_%d_enabled", i);
				if (!strcmp(str1, str3))
				{
					params->fractal.IFS.enabled[i] = atoi(str2);
					break;
				}
			}
		}
	}
	else if (!strcmp(str1, "start_frame"))
		params->startFrame = atoi(str2);
	else if (!strcmp(str1, "end_frame"))
		params->endFrame = atoi(str2);
	else if (!strcmp(str1, "frames_per_keyframe"))
		params->framesPerKeyframe = atoi(str2);
	else if (!strcmp(str1, "hybrid_cyclic"))
		params->fractal.hybridCyclic = atoi(str2);
	else if (!strcmp(str1, "fish_eye"))
		params->perspectiveType = enumPerspectiveType(atoi(str2));
	else if (!strcmp(str1, "fish_eye_180cut"))
		params->fishEyeCut = atoi(str2);
	else if (!strcmp(str1, "stereo_enabled"))
		params->stereoEnabled = atoi(str2);
	else if (!strcmp(str1, "stereo_eye_distance"))
		params->doubles.stereoEyeDistance = atof2(str2);
	else if (QString(str1).startsWith("mandelbox_"))
	{
		if (!strcmp(str1, "mandelbox_scale"))
			params->fractal.mandelbox.doubles.scale = atof2(str2);
		else if (!strcmp(str1, "mandelbox_folding_limit"))
			params->fractal.mandelbox.doubles.foldingLimit = atof2(str2);
		else if (!strcmp(str1, "mandelbox_folding_value"))
			params->fractal.mandelbox.doubles.foldingValue = atof2(str2);
		else if (!strcmp(str1, "mandelbox_folding_min_radius"))
			params->fractal.mandelbox.doubles.foldingSphericalMin = atof2(str2);
		else if (!strcmp(str1, "mandelbox_folding_fixed_radius"))
			params->fractal.mandelbox.doubles.foldingSphericalFixed = atof2(str2);
		else if (!strcmp(str1, "mandelbox_sharpness"))
			params->fractal.mandelbox.doubles.sharpness = atof2(str2);
		else if (!strcmp(str1, "mandelbox_offset_X"))
			params->fractal.mandelbox.doubles.offset.x = atof2(str2);
		else if (!strcmp(str1, "mandelbox_offset_Y"))
			params->fractal.mandelbox.doubles.offset.y = atof2(str2);
		else if (!strcmp(str1, "mandelbox_offset_Z"))
			params->fractal.mandelbox.doubles.offset.z = atof2(str2);
		else if (!strcmp(str1, "mandelbox_color_R"))
			params->fractal.mandelbox.doubles.colorFactorR = atof2(str2);
		else if (!strcmp(str1, "mandelbox_color_X"))
			params->fractal.mandelbox.doubles.colorFactorX = atof2(str2);
		else if (!strcmp(str1, "mandelbox_color_Y"))
			params->fractal.mandelbox.doubles.colorFactorY = atof2(str2);
		else if (!strcmp(str1, "mandelbox_color_Z"))
			params->fractal.mandelbox.doubles.colorFactorZ = atof2(str2);
		else if (!strcmp(str1, "mandelbox_color_Sp1"))
			params->fractal.mandelbox.doubles.colorFactorSp1 = atof2(str2);
		else if (!strcmp(str1, "mandelbox_color_Sp2"))
			params->fractal.mandelbox.doubles.colorFactorSp2 = atof2(str2);
		else if (!strcmp(str1, "mandelbox_rotation_enabled"))
			params->fractal.mandelbox.rotationsEnabled = atoi(str2);
		else if (!strcmp(str1, "mandelbox_fold_mode"))
			params->fractal.genFoldBox.type = enumGeneralizedFoldBoxType(atoi(str2));
		else if (!strcmp(str1, "mandelbox_solid"))
			params->fractal.mandelbox.doubles.solid = atof2(str2);
		else if (!strcmp(str1, "mandelbox_melt"))
			params->fractal.mandelbox.doubles.melt = atof2(str2);
		else if (!strcmp(str1, "mandelbox_vary_scale_vary"))
			params->fractal.mandelbox.doubles.vary4D.scaleVary = atof2(str2);
		else if (!strcmp(str1, "mandelbox_vary_fold"))
			params->fractal.mandelbox.doubles.vary4D.fold = atof2(str2);
		else if (!strcmp(str1, "mandelbox_vary_minr"))
			params->fractal.mandelbox.doubles.vary4D.minR = atof2(str2);
		else if (!strcmp(str1, "mandelbox_vary_rpower"))
			params->fractal.mandelbox.doubles.vary4D.rPower = atof2(str2);
		else if (!strcmp(str1, "mandelbox_vary_wadd"))
			params->fractal.mandelbox.doubles.vary4D.wadd = atof2(str2);
		else
		{
			int matched = false;
			char buf[100];
			for (int component = 0; component < 3; ++component)
			{
				sprintf(buf, "mandelbox_rotation_main_%s", component_names[component]);
				if (!strcmp(str1, buf))
				{
					params->fractal.mandelbox.doubles.rotationMain[component] = atof2(str2) / 180.0 * M_PI;
					matched = true;
					break;
				}
			}

			for (int fold = 0; fold < MANDELBOX_FOLDS; ++fold)
			{
				for (int axis = 0; axis < 3; ++axis)
				{
					for (int component = 0; component < 3; ++component)
					{
						sprintf(buf, "mandelbox_rotation_%s%d_%s", axis_names[axis], fold + 1,
							component_names[component]);
						if (!strcmp(str1, buf))
						{
							params->fractal.mandelbox.doubles.rotation[fold][axis][component] =
								atof2(str2) / 180.0 * M_PI;
							matched = true;
							break;
						}
					}
					if (matched) break;
				}
				if (matched) break;
			}
		}
	}
	else if (!strcmp(str1, "view_distance_max"))
		params->doubles.viewDistanceMax = atof2(str2);
	else if (!strcmp(str1, "view_distance_min"))
		params->doubles.viewDistanceMin = atof2(str2);
	else if (!strcmp(str1, "interior_mode"))
		params->fractal.interiorMode = atoi(str2);
	else if (!strcmp(str1, "linear_DE_mode"))
		params->fractal.linearDEmode = atoi(str2);
	else if (!strcmp(str1, "constant_DE_threshold"))
		params->fractal.constantDEThreshold = atoi(str2);
	else if (!strcmp(str1, "FoldingIntPow_folding_factor"))
		params->fractal.doubles.FoldingIntPowFoldFactor = atof2(str2);
	else if (!strcmp(str1, "FoldingIntPow_z_factor"))
		params->fractal.doubles.FoldingIntPowZfactor = atof2(str2);
	else if (!strcmp(str1, "penetrating_lights"))
		params->penetratingLights = atoi(str2);
	else if (!strcmp(str1, "raytraced_reflections"))
		params->imageSwitches.raytracedReflections = atoi(str2);
	else if (!strcmp(str1, "reflections_max"))
		params->reflectionsMax = atoi(str2);
	else if (!strcmp(str1, "c_add"))
		params->fractal.doubles.cadd = atof2(str2);
	else if (!strcmp(str1, "volumetric_fog_density"))
		params->doubles.fogDensity = atof2(str2);
	else if (!strcmp(str1, "volumetric_fog_colour_1_distance"))
		params->doubles.fogColour1Distance = atof2(str2);
	else if (!strcmp(str1, "volumetric_fog_colour_2_distance"))
		params->doubles.fogColour2Distance = atof2(str2);
	else if (!strcmp(str1, "volumetric_fog_distance_factor"))
		params->doubles.fogDistanceFactor = atof2(str2);
	else if (!strcmp(str1, "iteration_fog_enable"))
		params->imageSwitches.iterFogEnabled = atoi(str2);
	else if (!strcmp(str1, "iteration_fog_opacity"))
		params->doubles.iterFogOpacity = atof2(str2);
	else if (!strcmp(str1, "iteration_fog_opacity_trim"))
		params->doubles.iterFogOpacityTrim = atof2(str2);
	else if (!strcmp(str1, "primitive_only_plane"))
		params->fractal.primitives.onlyPlane = atoi(str2);
	else if (QString(str1).startsWith("primitive_plane_"))
	{
		if (!strcmp(str1, "primitive_plane_enabled"))
			params->fractal.primitives.planeEnable = atoi(str2);
		else if (!strcmp(str1, "primitive_plane_centre_X"))
			params->fractal.doubles.primitives.planeCentre.x = atof2(str2);
		else if (!strcmp(str1, "primitive_plane_centre_Y"))
			params->fractal.doubles.primitives.planeCentre.y = atof2(str2);
		else if (!strcmp(str1, "primitive_plane_centre_Z"))
			params->fractal.doubles.primitives.planeCentre.z = atof2(str2);
		else if (!strcmp(str1, "primitive_plane_normal_X"))
			params->fractal.doubles.primitives.planeNormal.x = atof2(str2);
		else if (!strcmp(str1, "primitive_plane_normal_Y"))
			params->fractal.doubles.primitives.planeNormal.y = atof2(str2);
		else if (!strcmp(str1, "primitive_plane_normal_Z"))
			params->fractal.doubles.primitives.planeNormal.z = atof2(str2);
		else if (!strcmp(str1, "primitive_plane_colour_R"))
			params->primitivePlaneColour.R = atoi(str2);
		else if (!strcmp(str1, "primitive_plane_colour_G"))
			params->primitivePlaneColour.G = atoi(str2);
		else if (!strcmp(str1, "primitive_plane_colour_B"))
			params->primitivePlaneColour.B = atoi(str2);
		else if (!strcmp(str1, "primitive_plane_reflect"))
			params->doubles.primitivePlaneReflect = atof2(str2);
	}
	else if (QString(str1).startsWith("primitive_box_"))
	{
		if (!strcmp(str1, "primitive_box_enabled"))
			params->fractal.primitives.boxEnable = atoi(str2);
		else if (!strcmp(str1, "primitive_box_centre_X"))
			params->fractal.doubles.primitives.boxCentre.x = atof2(str2);
		else if (!strcmp(str1, "primitive_box_centre_Y"))
			params->fractal.doubles.primitives.boxCentre.y = atof2(str2);
		else if (!strcmp(str1, "primitive_box_centre_Z"))
			params->fractal.doubles.primitives.boxCentre.z = atof2(str2);
		else if (!strcmp(str1, "primitive_box_size_X"))
			params->fractal.doubles.primitives.boxSize.x = atof2(str2);
		else if (!strcmp(str1, "primitive_box_size_Y"))
			params->fractal.doubles.primitives.boxSize.y = atof2(str2);
		else if (!strcmp(str1, "primitive_box_size_Z"))
			params->fractal.doubles.primitives.boxSize.z = atof2(str2);
		else if (!strcmp(str1, "primitive_box_colour_R"))
			params->primitiveBoxColour.R = atoi(str2);
		else if (!strcmp(str1, "primitive_box_colour_G"))
			params->primitiveBoxColour.G = atoi(str2);
		else if (!strcmp(str1, "primitive_box_colour_B"))
			params->primitiveBoxColour.B = atoi(str2);
		else if (!strcmp(str1, "primitive_box_reflect"))
			params->doubles.primitiveBoxReflect = atof2(str2);
	}
	else if (QString(str1).startsWith("primitive_invertedBox_"))
	{
		if (!strcmp(str1, "primitive_invertedBox_enabled"))
			params->fractal.primitives.invertedBoxEnable = atoi(str2);
		else if (!strcmp(str1, "primitive_invertedBox_centre_X"))
			params->fractal.doubles.primitives.invertedBoxCentre.x = atof2(str2);
		else if (!strcmp(str1, "primitive_invertedBox_centre_Y"))
			params->fractal.doubles.primitives.invertedBoxCentre.y = atof2(str2);
		else if (!strcmp(str1, "primitive_invertedBox_centre_Z"))
			params->fractal.doubles.primitives.invertedBoxCentre.z = atof2(str2);
		else if (!strcmp(str1, "primitive_invertedBox_size_X"))
			params->fractal.doubles.primitives.invertedBoxSize.x = atof2(str2);
		else if (!strcmp(str1, "primitive_invertedBox_size_Y"))
			params->fractal.doubles.primitives.invertedBoxSize.y = atof2(str2);
		else if (!strcmp(str1, "primitive_invertedBox_size_Z"))
			params->fractal.doubles.primitives.invertedBoxSize.z = atof2(str2);
		else if (!strcmp(str1, "primitive_invertedBox_colour_R"))
			params->primitiveInvertedBoxColour.R = atoi(str2);
		else if (!strcmp(str1, "primitive_invertedBox_colour_G"))
			params->primitiveInvertedBoxColour.G = atoi(str2);
		else if (!strcmp(str1, "primitive_invertedBox_colour_B"))
			params->primitiveInvertedBoxColour.B = atoi(str2);
		else if (!strcmp(str1, "primitive_invertedBox_reflect"))
			params->doubles.primitiveInvertedBoxReflect = atof2(str2);
	}
	else if (QString(str1).startsWith("primitive_sphere_"))
	{
		if (!strcmp(str1, "primitive_sphere_enabled"))
			params->fractal.primitives.sphereEnable = atoi(str2);
		else if (!strcmp(str1, "primitive_sphere_centre_X"))
			params->fractal.doubles.primitives.sphereCentre.x = atof2(str2);
		else if (!strcmp(str1, "primitive_sphere_centre_Y"))
			params->fractal.doubles.primitives.sphereCentre.y = atof2(str2);
		else if (!strcmp(str1, "primitive_sphere_centre_Z"))
			params->fractal.doubles.primitives.sphereCentre.z = atof2(str2);
		else if (!strcmp(str1, "primitive_sphere_radius"))
			params->fractal.doubles.primitives.sphereRadius = atof2(str2);
		else if (!strcmp(str1, "primitive_sphere_colour_R"))
			params->primitiveSphereColour.R = atoi(str2);
		else if (!strcmp(str1, "primitive_sphere_colour_G"))
			params->primitiveSphereColour.G = atoi(str2);
		else if (!strcmp(str1, "primitive_sphere_colour_B"))
			params->primitiveSphereColour.B = atoi(str2);
		else if (!strcmp(str1, "primitive_sphere_reflect"))
			params->doubles.primitiveSphereReflect = atof2(str2);
	}
	else if (QString(str1).startsWith("primitive_invertedSphere_"))
	{
		if (!strcmp(str1, "primitive_invertedSphere_enabled"))
			params->fractal.primitives.invertedSphereEnable = atoi(str2);
		else if (!strcmp(str1, "primitive_invertedSphere_centre_X"))
			params->fractal.doubles.primitives.invertedSphereCentre.x = atof2(str2);
		else if (!strcmp(str1, "primitive_invertedSphere_centre_Y"))
			params->fractal.doubles.primitives.invertedSphereCentre.y = atof2(str2);
		else if (!strcmp(str1, "primitive_invertedSphere_centre_Z"))
			params->fractal.doubles.primitives.invertedSphereCentre.z = atof2(str2);
		else if (!strcmp(str1, "primitive_invertedSphere_radius"))
			params->fractal.doubles.primitives.invertedSphereRadius = atof2(str2);
		else if (!strcmp(str1, "primitive_invertedSphere_colour_R"))
			params->primitiveInvertedSphereColour.R = atoi(str2);
		else if (!strcmp(str1, "primitive_invertedSphere_colour_G"))
			params->primitiveInvertedSphereColour.G = atoi(str2);
		else if (!strcmp(str1, "primitive_invertedSphere_colour_B"))
			params->primitiveInvertedSphereColour.B = atoi(str2);
		else if (!strcmp(str1, "primitive_invertedSphere_reflect"))
			params->doubles.primitiveInvertedSphereReflect = atof2(str2);
	}
	else if (QString(str1).startsWith("primitive_water_"))
	{
		if (!strcmp(str1, "primitive_water_enabled"))
			params->fractal.primitives.waterEnable = atoi(str2);
		else if (!strcmp(str1, "primitive_water_level"))
			params->fractal.doubles.primitives.waterHeight = atof2(str2);
		else if (!strcmp(str1, "primitive_water_amplitude"))
			params->fractal.doubles.primitives.waterAmplitude = atof2(str2);
		else if (!strcmp(str1, "primitive_water_length"))
			params->fractal.doubles.primitives.waterLength = atof2(str2);
		else if (!strcmp(str1, "primitive_water_iterations"))
			params->fractal.primitives.waterIterations = atoi(str2);
		else if (!strcmp(str1, "primitive_water_rotation"))
			params->fractal.doubles.primitives.waterRotation = atof2(str2);
		else if (!strcmp(str1, "primitive_water_anim_speed"))
			params->fractal.doubles.primitives.waterAnimSpeed = atof2(str2);
		else if (!strcmp(str1, "primitive_water_colour_R"))
			params->primitiveWaterColour.R = atoi(str2);
		else if (!strcmp(str1, "primitive_water_colour_G"))
			params->primitiveWaterColour.G = atoi(str2);
		else if (!strcmp(str1, "primitive_water_colour_B"))
			params->primitiveWaterColour.B = atoi(str2);
		else if (!strcmp(str1, "primitive_water_reflect"))
			params->doubles.primitiveWaterReflect = atof2(str2);
	}
	else if (QString(str1).startsWith("fake_lights_"))
	{
		if (!strcmp(str1, "fake_lights_enabled"))
			params->fakeLightsEnabled = atoi(str2);
		else if (!strcmp(str1, "fake_lights_min_iter"))
			params->fractal.fakeLightsMinIter = atoi(str2);
		else if (!strcmp(str1, "fake_lights_max_iter"))
			params->fractal.fakeLightsMaxIter = atoi(str2);
		else if (!strcmp(str1, "fake_lights_visibility"))
			params->doubles.fakeLightsVisibility = atof2(str2);
		else if (!strcmp(str1, "fake_lights_visibility_size"))
			params->doubles.fakeLightsVisibilitySize = atof2(str2);
		else if (!strcmp(str1, "fake_lights_intensity"))
			params->doubles.fakeLightsIntensity = atof2(str2);
		else if (!strcmp(str1, "fake_lights_orbit_trap_X"))
			params->fractal.doubles.fakeLightsOrbitTrap.x = atof2(str2);
		else if (!strcmp(str1, "fake_lights_orbit_trap_Y"))
			params->fractal.doubles.fakeLightsOrbitTrap.y = atof2(str2);
		else if (!strcmp(str1, "fake_lights_orbit_trap_Z"))
			params->fractal.doubles.fakeLightsOrbitTrap.z = atof2(str2);
	}
	else if (!strcmp(str1, "frame_no"))
		params->fractal.frameNo = atoi(str2);
	else if (!strcmp(str1, "tile_no"))
		params->tileCount = atoi(str2);
	else if (!strcmp(str1, "file_destination"))
		strcpy(params->file_destination, str2);
	else if (!strcmp(str1, "file_background"))
		strcpy(params->file_background, str2);
	else if (!strcmp(str1, "file_envmap"))
		strcpy(params->file_envmap, str2);
	else if (!strcmp(str1, "file_lightmap"))
		strcpy(params->file_lightmap, str2);
	else if (!strcmp(str1, "file_animation_path"))
		strcpy(params->file_path, str2);
	else if (!strcmp(str1, "file_keyframes"))
		strcpy(params->file_keyframes, str2);
	else if (!strcmp(str1, "palette"))
		GetPaletteFromString(params->palette, str2);
	else
	{
		int matched = false;
		char buf[100];
		for (int i = 0; i < 5; ++i)
		{
			sprintf(buf, "volumetric_light_enabled_%d", i);
			if (!strcmp(str1, buf))
			{
				params->volumetricLightEnabled[i] = atoi(str2);
				matched = true;
				break;
			}
			sprintf(buf, "volumetric_light_intensity_%d", i);
			if (!strcmp(str1, buf))
			{
				params->doubles.volumetricLightIntensity[i] = atof2(str2);
				matched = true;
				break;
			}
		}

		for (int i = 1; i <= HYBRID_COUNT; ++i)
		{
			sprintf(buf, "hybrid_formula_%d", i);
			if (!strcmp(str1, buf))
			{
				params->fractal.hybridFormula[i - 1] = enumOldFractalFormula(atoi(str2));
				matched = true;
				break;
			}
			sprintf(buf, "hybrid_iterations_%d", i);
			if (!strcmp(str1, buf))
			{
				params->fractal.hybridIters[i - 1] = atoi(str2);
				matched = true;
				break;
			}
			sprintf(buf, "hybrid_power_%d", i);
			if (!strcmp(str1, buf))
			{
				params->fractal.doubles.hybridPower[i - 1] = atof2(str2);
				matched = true;
				break;
			}
		}

		if (!matched)
		{
			printf("Warning! Unknown parameter: %s %s\n", str1, str2);
			WriteLog("Warning! Unknown parameter:", 1);
			WriteLog(str1, 1);
			return false;
		}
	}
	return true;
}

void cOldSettings::ConvertToNewContainer(cParameterContainer *par, cFractalContainer *fractal) const
{
	// general parameters
	par->ResetAllToDefault();
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		fractal->at(i).ResetAllToDefault();
	DeleteAllPrimitiveParams(par);
	DeleteAllMaterialParams(par);
	InitMaterialParams(1, par);
	par->Set("mat1_is_defined", true);

	par->Set("legacy_coordinate_system", true);

	par->Set("image_width", oldData->image_width);
	par->Set("image_height", oldData->image_height);

	// conversion of camera
	CVector3 vp = oldData->doubles.vp;
	CVector3 cameraRotation(oldData->doubles.alpha, oldData->doubles.beta, oldData->doubles.gamma);
	CVector3 forwardVector(0.0, 1.0, 0.0);
	forwardVector =
		forwardVector.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), cameraRotation.z);
	forwardVector =
		forwardVector.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), cameraRotation.y);
	forwardVector =
		forwardVector.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), cameraRotation.x);

	CVector3 camera, target;
	if (oldData->doubles.zoom <= 1e-13) oldData->doubles.zoom = 1e-13;
	if (oldData->perspectiveType == threePoint)
	{
		camera = vp - forwardVector * (1.0 / oldData->doubles.persp * oldData->doubles.zoom);
		target = vp;
	}
	else
	{
		camera = vp;
		target = vp + forwardVector * (1.0 / oldData->doubles.persp * oldData->doubles.zoom);
	}
	cCameraTarget cameraTarget(camera, target, CVector3(1.0, 0.0, 0.0));
	cameraTarget.SetCameraTargetRotation(camera, target, cameraRotation.z);

	par->Set("camera", camera);
	par->Set("target", target);
	par->Set("camera_top", cameraTarget.GetTopVector());
	par->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);
	par->Set("camera_distance_to_target", cameraTarget.GetDistance());
	par->Set("fov", oldData->doubles.persp);
	par->Set("perspective_type", int(oldData->perspectiveType));
	par->Set("stereo_eye_distance", oldData->doubles.stereoEyeDistance);
	par->Set("stereo_enabled", oldData->stereoEnabled);

	par->Set("formula", 1, int(oldData->fractal.formula));

	par->Set("julia_mode", oldData->fractal.juliaMode);
	par->Set("julia_c", oldData->fractal.doubles.julia);
	par->Set("N", oldData->fractal.doubles.N);
	par->Set("minN", oldData->fractal.minN);
	par->Set("fractal_constant_factor",
		CVector3(oldData->fractal.doubles.constantFactor, oldData->fractal.doubles.constantFactor,
						 oldData->fractal.doubles.constantFactor));
	par->Set("detail_level", oldData->doubles.quality);
	par->Set("DE_thresh", oldData->doubles.quality);
	par->Set("smoothness", oldData->doubles.smoothness);
	par->Set("iteration_threshold_mode", oldData->fractal.iterThresh);
	par->Set("DE_factor", oldData->doubles.DE_factor);
	par->Set("slow_shading", oldData->slowShading);
	par->Set("view_distance_max", oldData->doubles.viewDistanceMax);
	par->Set("view_distance_min", oldData->doubles.viewDistanceMin);
	par->Set("limit_min", CVector3(oldData->fractal.doubles.amin, oldData->fractal.doubles.bmin,
													oldData->fractal.doubles.cmin));
	par->Set("limit_max", CVector3(oldData->fractal.doubles.amax, oldData->fractal.doubles.bmax,
													oldData->fractal.doubles.cmax));
	par->Set("limits_enabled", oldData->fractal.limits_enabled);
	par->Set("interior_mode", oldData->fractal.interiorMode);
	if (oldData->fractal.linearDEmode)
		par->Set("delta_DE_function", int(fractal::linearDEFunction));
	else
		par->Set("delta_DE_function", int(fractal::logarithmicDEFunction));

	par->Set("constant_DE_threshold", oldData->fractal.constantDEThreshold);
	par->Set("hybrid_fractal_enable", false);

	par->Set("brightness", oldData->doubles.imageAdjustments.brightness);
	par->Set("contrast", oldData->doubles.imageAdjustments.contrast);
	par->Set("gamma", oldData->doubles.imageAdjustments.imageGamma);
	par->Set("hdr", oldData->imageSwitches.hdrEnabled);
	par->Set("mat1_reflectance", oldData->doubles.imageAdjustments.reflect);
	par->Set("ambient_occlusion", oldData->doubles.imageAdjustments.globalIlum);
	par->Set("ambient_occlusion_quality", oldData->globalIlumQuality);
	par->Set("ambient_occlusion_fast_tune", oldData->doubles.fastAoTune);
	par->Set("ambient_occlusion_enabled", oldData->global_ilumination || oldData->SSAOEnabled);

	params::enumAOMode AOMode = params::AOModeMultipleRays;
	if (oldData->SSAOEnabled) AOMode = params::AOModeScreenSpace;
	if (oldData->fastGlobalIllumination && !oldData->SSAOEnabled) AOMode = params::AOModeFast;
	par->Set("ambient_occlusion_mode", int(AOMode));
	if (AOMode == params::AOModeScreenSpace)
		par->Set("ambient_occlusion_quality", sqrt(oldData->SSAOQuality));

	par->Set("mat1_shading", oldData->doubles.imageAdjustments.shading);
	par->Set("mat1_specular", oldData->doubles.imageAdjustments.specular);
	par->Set("glow_enabled", (oldData->doubles.imageAdjustments.glow_intensity > 0.0) ? true : false);
	par->Set("glow_intensity", oldData->doubles.imageAdjustments.glow_intensity);
	par->Set("textured_background", oldData->texturedBackground);
	par->Set("textured_background_map_type", oldData->background_as_fulldome);
	par->Set("shadows_enabled", oldData->shadow);
	par->Set("penetrating_lights", oldData->penetratingLights);
	par->Set("raytraced_reflections", oldData->imageSwitches.raytracedReflections);
	par->Set("reflections_max", oldData->reflectionsMax);
	par->Set("env_mapping_enable", (oldData->doubles.imageAdjustments.reflect > 0.0
																	 && !oldData->imageSwitches.raytracedReflections)
																	 ? true
																	 : false);

	par->Set("glow_color", 1, oldData->effectColours.glow_color1);
	par->Set("glow_color", 2, oldData->effectColours.glow_color2);
	par->Set("background_color", 1, oldData->background_color1);
	par->Set("background_color", 2, oldData->background_color2);
	par->Set("background_color", 3, oldData->background_color3);
	par->Set("fog_color", 1, oldData->fogColour1);
	par->Set("fog_color", 2, oldData->fogColour2);
	par->Set("fog_color", 3, oldData->fogColour3);

	par->Set("volumetric_fog_enabled", (oldData->doubles.fogDensity > 0.0) ? true : false);
	par->Set("volumetric_fog_density", oldData->doubles.fogDensity);
	par->Set("volumetric_fog_colour_1_distance", oldData->doubles.fogColour1Distance);
	par->Set("volumetric_fog_colour_2_distance", oldData->doubles.fogColour2Distance);
	par->Set("volumetric_fog_distance_factor", oldData->doubles.fogDistanceFactor);

	par->Set("iteration_fog_enable", oldData->imageSwitches.iterFogEnabled);
	par->Set("iteration_fog_opacity", oldData->doubles.iterFogOpacity);
	par->Set("iteration_fog_opacity_trim", oldData->doubles.iterFogOpacityTrim);
	par->Set("iteration_fog_color_1_maxiter", int(oldData->fractal.doubles.N * 0.33));
	par->Set("iteration_fog_color_2_maxiter", int(oldData->fractal.doubles.N * 0.66));
	par->Set("iteration_fog_color", 1, oldData->fogColour1);
	par->Set("iteration_fog_color", 2, oldData->fogColour2);
	par->Set("iteration_fog_color", 3, oldData->fogColour3);

	par->Set("mat1_use_colors_from_palette", oldData->imageSwitches.coloringEnabled);
	par->Set("mat1_coloring_random_seed", oldData->coloring_seed);
	par->Set("mat1_coloring_saturation", oldData->doubles.colourSaturation);
	par->Set("mat1_coloring_speed", oldData->doubles.imageAdjustments.coloring_speed);
	par->Set("mat1_coloring_palette_size", 256);
	par->Set("mat1_coloring_palette_offset", oldData->doubles.imageAdjustments.paletteOffset);
	par->Set("basic_fog_enabled", oldData->imageSwitches.fogEnabled);
	par->Set("basic_fog_color", oldData->effectColours.fogColor);
	par->Set(
		"basic_fog_visibility", pow(10.0, oldData->doubles.imageAdjustments.fogVisibility / 10 - 16.0));
	par->Set("DOF_enabled", oldData->DOFEnabled);
	par->Set("DOF_focus", pow(10, oldData->doubles.DOFFocus / 10.0 - 16.0));
	par->Set("DOF_radius", oldData->doubles.DOFRadius);

	par->Set("main_light_intensity", oldData->doubles.imageAdjustments.directLight
																		 * oldData->doubles.imageAdjustments.mainLightIntensity);
	par->Set("main_light_alpha", oldData->doubles.mainLightAlpha * 180.0 / M_PI);
	par->Set("main_light_beta", -oldData->doubles.mainLightBeta * 180.0 / M_PI);
	par->Set("main_light_colour", oldData->effectColours.mainLightColour);
	par->Set("shadows_cone_angle", oldData->doubles.shadowConeAngle);
	par->Set("main_light_enable",
		(oldData->doubles.imageAdjustments.mainLightIntensity > 0.0) ? true : false);
	par->Set("main_light_volumetric_intensity", oldData->doubles.volumetricLightIntensity[0]);
	par->Set("main_light_volumetric_enabled", oldData->volumetricLightEnabled[0]);

	par->Set("aux_light_intensity", oldData->doubles.auxLightIntensity);
	par->Set("aux_light_visibility", oldData->doubles.auxLightVisibility);

	for (int i = 0; i < 4; i++)
	{
		par->Set("aux_light_position", i + 1, oldData->doubles.auxLightPre[i]);
		par->Set("aux_light_intensity", i + 1, oldData->doubles.auxLightPreIntensity[i]);
		par->Set("aux_light_enabled", i + 1, oldData->auxLightPreEnabled[i]);
		par->Set("aux_light_colour", i + 1, oldData->auxLightPreColour[i]);
		par->Set(
			"aux_light_volumetric_intensity", i + 1, oldData->doubles.volumetricLightIntensity[i + 1]);
		par->Set("aux_light_volumetric_enabled", i + 1, oldData->volumetricLightEnabled[i + 1]);
	}

	par->Set("random_lights_random_seed", oldData->auxLightRandomSeed);
	par->Set("random_lights_max_distance_from_fractal", oldData->doubles.auxLightMaxDist);
	par->Set("random_lights_intensity", oldData->doubles.auxLightIntensity);
	par->Set("random_lights_distribution_radius", oldData->doubles.auxLightDistributionRadius);
	par->Set("random_lights_distribution_center", oldData->doubles.auxLightRandomCenter);
	par->Set("random_lights_number", oldData->auxLightNumber > 4 ? (oldData->auxLightNumber - 4) : 0);
	par->Set("random_lights_group", oldData->auxLightNumber > 4);

	par->Set("fake_lights_enabled", oldData->fakeLightsEnabled);
	par->Set("fake_lights_intensity", oldData->doubles.fakeLightsIntensity);
	par->Set("fake_lights_visibility", oldData->doubles.fakeLightsVisibility);
	par->Set("fake_lights_visibility_size", oldData->doubles.fakeLightsVisibilitySize);
	par->Set("fake_lights_orbit_trap", oldData->fractal.doubles.fakeLightsOrbitTrap);
	par->Set("fake_lights_min_iter", oldData->fractal.fakeLightsMinIter);
	par->Set("fake_lights_max_iter", oldData->fractal.fakeLightsMaxIter);

	cColorPalette palette;
	for (int i = 0; i < 256; i++)
		palette.AppendColor(oldData->palette[i]);
	par->Set("mat1_surface_color_palette", palette);

	if (oldData->fractal.primitives.boxEnable)
	{
		InitPrimitiveParams(fractal::objBox, "primitive_box_1", par);
		par->Set("primitive_box_1_position", oldData->fractal.doubles.primitives.boxCentre);
		par->Set("primitive_box_1_color", oldData->primitiveBoxColour);
		par->Set("primitive_box_1_reflection", oldData->doubles.primitiveBoxReflect);
		par->Set("primitive_box_1_enabled", oldData->fractal.primitives.boxEnable);
		par->Set("primitive_box_1_size", oldData->fractal.doubles.primitives.boxSize);
	}

	if (oldData->fractal.primitives.invertedBoxEnable)
	{
		InitPrimitiveParams(fractal::objBox, "primitive_box_2", par);
		par->Set("primitive_box_2_position", oldData->fractal.doubles.primitives.invertedBoxCentre);
		par->Set("primitive_box_2_color", oldData->primitiveInvertedBoxColour);
		par->Set("primitive_box_2_reflection", oldData->doubles.primitiveInvertedBoxReflect);
		par->Set("primitive_box_2_enabled", oldData->fractal.primitives.invertedBoxEnable);
		par->Set("primitive_box_2_size", oldData->fractal.doubles.primitives.invertedBoxSize);
		par->Set("primitive_box_2_empty", true);
	}

	if (oldData->fractal.primitives.sphereEnable)
	{
		InitPrimitiveParams(fractal::objSphere, "primitive_sphere_1", par);
		par->Set("primitive_sphere_1_position", oldData->fractal.doubles.primitives.sphereCentre);
		par->Set("primitive_sphere_1_color", oldData->primitiveSphereColour);
		par->Set("primitive_sphere_1_reflection", oldData->doubles.primitiveSphereReflect);
		par->Set("primitive_sphere_1_enabled", oldData->fractal.primitives.sphereEnable);
		par->Set("primitive_sphere_1_radius", oldData->fractal.doubles.primitives.sphereRadius);
	}

	if (oldData->fractal.primitives.invertedSphereEnable)
	{
		InitPrimitiveParams(fractal::objSphere, "primitive_sphere_2", par);
		par->Set(
			"primitive_sphere_2_position", oldData->fractal.doubles.primitives.invertedSphereCentre);
		par->Set("primitive_sphere_2_color", oldData->primitiveInvertedSphereColour);
		par->Set("primitive_sphere_2_reflection", oldData->doubles.primitiveInvertedSphereReflect);
		par->Set("primitive_sphere_2_enabled", oldData->fractal.primitives.invertedSphereEnable);
		par->Set("primitive_sphere_2_radius", oldData->fractal.doubles.primitives.invertedSphereRadius);
		par->Set("primitive_sphere_2_empty", true);
	}

	if (oldData->fractal.primitives.planeEnable)
	{
		InitPrimitiveParams(fractal::objPlane, "primitive_plane_1", par);
		par->Set("primitive_plane_1_position", oldData->fractal.doubles.primitives.planeCentre);
		par->Set("primitive_plane_1_color", oldData->primitivePlaneColour);
		par->Set("primitive_plane_1_reflection", oldData->doubles.primitivePlaneReflect);
		par->Set("primitive_plane_1_enabled", oldData->fractal.primitives.planeEnable);
		CVector3 rotation;
		CVector3 normal = oldData->fractal.doubles.primitives.planeNormal;
		rotation.x = atan2(normal.y, normal.z) * 180.0 / M_PI;
		rotation.y = -atan2(normal.x, sqrt(normal.y * normal.y + normal.z * normal.z)) * 180.0 / M_PI;
		rotation.z = 0;
		par->Set("primitive_plane_1_rotation", rotation);
	}

	if (oldData->fractal.primitives.waterEnable)
	{
		InitPrimitiveParams(fractal::objWater, "primitive_water_1", par);
		par->Set("primitive_water_1_position",
			CVector3(0, 0, oldData->fractal.doubles.primitives.waterHeight));
		par->Set("primitive_water_1_color", oldData->primitiveWaterColour);
		par->Set("primitive_water_1_reflection", oldData->doubles.primitiveWaterReflect);
		par->Set("primitive_water_1_enabled", oldData->fractal.primitives.waterEnable);
		par->Set(
			"primitive_water_1_relative_amplitude", oldData->fractal.doubles.primitives.waterAmplitude
																								/ oldData->fractal.doubles.primitives.waterLength);
		par->Set("primitive_water_1_length", oldData->fractal.doubles.primitives.waterLength);
		par->Set("primitive_water_1_iterations", oldData->fractal.primitives.waterIterations);
		par->Set("primitive_water_1_rotation",
			CVector3(180.0, 0, -oldData->fractal.doubles.primitives.waterRotation));
	}

	// foldings
	par->Set("box_folding", oldData->fractal.tgladFoldingMode);
	par->Set("box_folding_limit", oldData->fractal.doubles.foldingLimit);
	par->Set("box_folding_value", oldData->fractal.doubles.foldingValue);
	par->Set("spherical_folding", oldData->fractal.sphericalFoldingMode);
	par->Set("spherical_folding_outer", oldData->fractal.doubles.foldingSphericalFixed);
	par->Set("spherical_folding_inner", oldData->fractal.doubles.foldingSphericalMin);

	// fractal parameters

	if (oldData->fractal.formula == trig_DE)
	{
		par->Set("formula", 1, int(fractal::mandelbulb));
		fractal->at(0).Set("alpha_angle_offset", 180.0 / oldData->fractal.doubles.power);
		fractal->at(0).Set("beta_angle_offset", 180.0 / oldData->fractal.doubles.power);
	}

	fractal->at(0).Set("power", oldData->fractal.doubles.power);
	fractal->at(0).Set("cadd", oldData->fractal.doubles.cadd);
	fractal->at(0).Set("IFS_scale", oldData->fractal.IFS.doubles.scale);
	fractal->at(0).Set("IFS_rotation",
		CVector3(oldData->fractal.IFS.doubles.rotationAlfa, oldData->fractal.IFS.doubles.rotationBeta,
			oldData->fractal.IFS.doubles.rotationGamma)
			* 180.0 / M_PI);
	fractal->at(0).Set("IFS_offset", oldData->fractal.IFS.doubles.offset);
	fractal->at(0).Set("IFS_edge", oldData->fractal.IFS.doubles.edge);
	fractal->at(0).Set(
		"IFS_edge_enabled", (oldData->fractal.IFS.doubles.edge.Length() > 0) ? true : false);
	fractal->at(0).Set("IFS_abs_x", oldData->fractal.IFS.absX);
	fractal->at(0).Set("IFS_abs_y", oldData->fractal.IFS.absY);
	fractal->at(0).Set("IFS_abs_z", oldData->fractal.IFS.absZ);
	fractal->at(0).Set("IFS_menger_sponge_mode", oldData->fractal.IFS.mengerSpongeMode);

	for (int i = 0; i < IFS_VECTOR_COUNT; i++)
	{
		fractal->at(0).Set("IFS_direction", i, oldData->fractal.IFS.doubles.direction[i]);
		fractal->at(0).Set("IFS_rotations", i,
			CVector3(oldData->fractal.IFS.doubles.alfa[i], oldData->fractal.IFS.doubles.beta[i],
				oldData->fractal.IFS.doubles.gamma[i])
				* 180.0 / M_PI);
		fractal->at(0).Set("IFS_distance", i, oldData->fractal.IFS.doubles.distance[i]);
		fractal->at(0).Set("IFS_intensity", i, oldData->fractal.IFS.doubles.intensity[i]);
		fractal->at(0).Set("IFS_enabled", i, oldData->fractal.IFS.enabled[i]);
	}

	fractal->at(0).Set("mandelbox_scale", oldData->fractal.mandelbox.doubles.scale);
	fractal->at(0).Set("mandelbox_folding_limit", oldData->fractal.mandelbox.doubles.foldingLimit);
	fractal->at(0).Set("mandelbox_folding_value", oldData->fractal.mandelbox.doubles.foldingValue);
	fractal->at(0).Set(
		"mandelbox_folding_min_radius", oldData->fractal.mandelbox.doubles.foldingSphericalMin);
	fractal->at(0).Set(
		"mandelbox_folding_fixed_radius", oldData->fractal.mandelbox.doubles.foldingSphericalFixed);
	fractal->at(0).Set("mandelbox_sharpness", oldData->fractal.mandelbox.doubles.sharpness);
	fractal->at(0).Set("mandelbox_offset", oldData->fractal.mandelbox.doubles.offset);
	fractal->at(0).Set("mandelbox_rotation_main",
		CVector3(oldData->fractal.mandelbox.doubles.rotationMain) * 180.0 / M_PI);

	for (int i = 0; i < 3; i++)
	{
		fractal->at(0).Set("mandelbox_rotation_neg", i + 1,
			CVector3(oldData->fractal.mandelbox.doubles.rotation[0][i]) * 180.0 / M_PI);
		fractal->at(0).Set("mandelbox_rotation_pos", i + 1,
			CVector3(oldData->fractal.mandelbox.doubles.rotation[1][i]) * 180.0 / M_PI);
	}

	fractal->at(0).Set("mandelbox_color", CVector3(oldData->fractal.mandelbox.doubles.colorFactorX,
																					oldData->fractal.mandelbox.doubles.colorFactorY,
																					oldData->fractal.mandelbox.doubles.colorFactorZ));
	fractal->at(0).Set("mandelbox_color_R", oldData->fractal.mandelbox.doubles.colorFactorR);
	fractal->at(0).Set("mandelbox_color_Sp1", oldData->fractal.mandelbox.doubles.colorFactorSp1);
	fractal->at(0).Set("mandelbox_color_Sp2", oldData->fractal.mandelbox.doubles.colorFactorSp2);
	fractal->at(0).Set("mandelbox_rotations_enabled", oldData->fractal.mandelbox.rotationsEnabled);
	fractal->at(0).Set("mandelbox_main_rotation_enabled",
		(CVector3(oldData->fractal.mandelbox.doubles.rotationMain).Length() > 0) ? true : false);
	fractal->at(0).Set("mandelbox_solid", oldData->fractal.mandelbox.doubles.solid);
	fractal->at(0).Set("mandelbox_melt", oldData->fractal.mandelbox.doubles.melt);

	fractal->at(0).Set(
		"mandelbox_vary_scale_vary", oldData->fractal.mandelbox.doubles.vary4D.scaleVary);
	fractal->at(0).Set("mandelbox_vary_fold", oldData->fractal.mandelbox.doubles.vary4D.fold);
	fractal->at(0).Set("mandelbox_vary_minr", oldData->fractal.mandelbox.doubles.vary4D.minR);
	fractal->at(0).Set("mandelbox_vary_rpower", oldData->fractal.mandelbox.doubles.vary4D.rPower);
	fractal->at(0).Set("mandelbox_vary_wadd", oldData->fractal.mandelbox.doubles.vary4D.wadd);

	fractal->at(0).Set("mandelbox_generalized_fold_type", int(oldData->fractal.genFoldBox.type));

	fractal->at(0).Set(
		"boxfold_bulbpow2_folding_factor", oldData->fractal.doubles.FoldingIntPowFoldFactor);
	fractal->at(0).Set("boxfold_bulbpow2_z_factor", oldData->fractal.doubles.FoldingIntPowZfactor);

	for (int i = 0; i < 4; i++)
	{
		fractal->at(i).Set("IFS_rotation_enabled", true);
	}

	fractal->at(1) = fractal->at(2) = fractal->at(3) = fractal->at(0);

	// converting hybrid fractals

	if (oldData->fractal.formula == hybrid)
	{
		QList<int> fractalsListTemp;
		for (int i = 0; i < HYBRID_COUNT; i++)
		{
			if (oldData->fractal.hybridFormula[i] != oldSettings::none)
			{
				fractalsListTemp.append(i);
			}
		}
		bool result = true;
		if (fractalsListTemp.size() <= NUMBER_OF_FRACTALS)
		{
			for (int i = 0; i < fractalsListTemp.size(); i++)
			{
				oldSettings::enumOldFractalFormula formula =
					oldData->fractal.hybridFormula[fractalsListTemp.at(i)];
				bool found = false;
				for (int l = 0; l < fractalList.size(); l++)
				{
					if (formula == oldSettings::enumOldFractalFormula(fractalList.at(l).internalID))
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					par->Set("formula", i + 1, int(oldData->fractal.hybridFormula[fractalsListTemp.at(i)]));
					if (formula == trig_DE)
					{
						par->Set("formula", 1, int(fractal::mandelbulb));
						fractal->at(i).Set("alpha_angle_offset", 180.0 / oldData->fractal.doubles.power);
						fractal->at(i).Set("beta_angle_offset", 180.0 / oldData->fractal.doubles.power);
					}
					par->Set(
						"formula_iterations", i + 1, oldData->fractal.hybridIters[fractalsListTemp.at(i)]);
					switch (formula)
					{
						case trig_DE:
						case trig_optim:
						case xenodreambuie:
						case mandelbulb2:
						case mandelbulb3:
						case mandelbulb4:
						case benesi:
						case bristorbrot:
						{
							fractal->at(i).Set(
								"power", oldData->fractal.doubles.hybridPower[fractalsListTemp.at(i)]);
							break;
						}

						case smoothMandelbox:
						case mandelboxVaryScale4D:
						case tglad:
						{
							fractal->at(i).Set(
								"mandelbox_scale", oldData->fractal.doubles.hybridPower[fractalsListTemp.at(i)]);
							break;
						}

						default: break;
					}

					if (!oldData->fractal.hybridCyclic && i == fractalsListTemp.size() - 1)
					{
						par->Set("formula_iterations", i + 1, int(oldData->fractal.doubles.N));
					}
				}
				else
				{
					result = false;
					break;
				}
			}
		}
		else
		{
			result = false;
		}

		if (!result)
		{
			cErrorMessage::showMessage(
				QObject::tr("Hybrid fractal can't be converted"), cErrorMessage::errorMessage);
		}
		else
		{
			par->Set("hybrid_fractal_enable", true);
		}
	}

	if (oldData->fractal.IFS.foldingMode)
	{
		par->Set("hybrid_fractal_enable", true);
		par->Set("formula", 2, int(fractal::kaleidoscopicIfs));
	}

	if (oldData->fractal.juliaMode)
	{
		par->Set("fractal_constant_factor", CVector3(0.0, 0.0, 0.0));
		for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
		{
			par->Set("fractal_constant_factor", CVector3(1.0, 1.0, 1.0));
		}
	}
}

} /* namespace oldSettings */
