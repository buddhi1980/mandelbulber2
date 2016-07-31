/*
 * material.cpp
 *
 *  Created on: 19 mar 2016
 *      Author: krzysztof
 */

#include "material.h"
#include "netrender.hpp"

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
  textureMappingType = mappingPlanar;
  colorTextureIntensity = 0.0;
  diffussionTextureIntensity = 0.0;
  luminosityTextureIntensity = 0.0;
  displacementTextureHeight = 0.0;
  normalMapTextureFromBumpmap = false;
  normalMapTextureHeight = 0.0;
}

cMaterial::cMaterial(int _id, const cParameterContainer *materialParam, bool quiet)
{
  setParameters(_id, materialParam, quiet);
}

cMaterial::~cMaterial()
{

}

//this static list will be use to optimize usage of material parameters
QStringList cMaterial::paramsList = {
		"is_defined",
		"name",
    "shading",
    "specular",
    "specular_width",
		"specular_color",
    "reflectance",
    "luminosity",
    "transparency_of_surface",
    "transparency_of_interior",
    "transparency_index_of_refraction",
    "surface_color",
    "transparency_interior_color",
    "luminosity_color",
    "fresnel_reflectance",
    "texture_center",
    "texture_rotation",
    "texture_scale",
    "coloring_random_seed",
    "coloring_saturation",
    "coloring_speed",
    "coloring_palette_size",
    "coloring_palette_offset",
    "texture_mapping_type",
    "use_colors_from_palette",
    "file_color_texture",
    "file_diffusion_texture",
    "file_luminosity_texture",
    "file_displacement_texture",
    "surface_color_palette",
		"use_color_texture",
		"use_diffusion_texture",
		"use_luminosity_texture",
		"use_displacement_texture",
		"use_normal_map_texture",
		"color_texture_intensity",
		"luminosity_texture_intensity",
		"diffusion_texture_intensity",
		"displacement_texture_height",
		"fractal_coloring_algorithm",
		"fractal_coloring_sphere_radius",
		"fractal_coloring_line_direction",
		"normal_map_texture_from_bumpmap",
		"normal_map_texture_height",
		"normal_map_texture_invert_green",
		"file_normal_map_texture"
};

void cMaterial::setParameters(int _id, const cParameterContainer *materialParam, bool quiet = false)
{
  id = _id;
  shading = materialParam->Get<double>(Name("shading", id));
  specular = materialParam->Get<double>(Name("specular", id));
  specularWidth = materialParam->Get<double>(Name("specular_width", id));
  specularColor = materialParam->Get<sRGB>(Name("specular_color", id));
  reflectance = materialParam->Get<double>(Name("reflectance", id));
  luminosity = materialParam->Get<double>(Name("luminosity", id));
  transparencyIndexOfRefraction = materialParam->Get<double>(Name("transparency_index_of_refraction", id));
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
	if(textureScale.x < 1e-20) textureScale.x = 1e-20;
	if(textureScale.y < 1e-20) textureScale.y = 1e-20;
	if(textureScale.z < 1e-20) textureScale.z = 1e-20;

  textureMappingType = (enumTextureMapping)materialParam->Get<int>(Name("texture_mapping_type", id));

  fresnelReflectance = materialParam->Get<bool>(Name("fresnel_reflectance", id));
  useColorsFromPalette = materialParam->Get<bool>(Name("use_colors_from_palette", id));

  useColorTexture = materialParam->Get<bool>(Name("use_color_texture", id));
  useDiffusionTexture = materialParam->Get<bool>(Name("use_diffusion_texture", id));
  useLuminosityTexture = materialParam->Get<bool>(Name("use_luminosity_texture", id));
  useDisplacementTexture = materialParam->Get<bool>(Name("use_displacement_texture", id));
  useNormalMapTexture = materialParam->Get<bool>(Name("use_normal_map_texture", id));
  normalMapTextureFromBumpmap = materialParam->Get<bool>(Name("normal_map_texture_from_bumpmap", id));

  colorTextureIntensity = materialParam->Get<double>(Name("color_texture_intensity", id));
  diffussionTextureIntensity = materialParam->Get<double>(Name("diffusion_texture_intensity", id));
  luminosityTextureIntensity = materialParam->Get<double>(Name("luminosity_texture_intensity", id));
  displacementTextureHeight = materialParam->Get<double>(Name("displacement_texture_height", id));
  normalMapTextureHeight = materialParam->Get<double>(Name("normal_map_texture_height", id));

	fractalColoring.coloringAlgorithm =
			(sFractalColoring::enumFractalColoringAlgorithm) materialParam->Get<int>(Name(	"fractal_coloring_algorithm",																																									id));
  fractalColoring.sphereRadius = materialParam->Get<double>(Name("fractal_coloring_sphere_radius", id));
  fractalColoring.lineDirection = materialParam->Get<CVector3>(Name("fractal_coloring_line_direction", id));

  if(gNetRender->IsClient())
  {
  	if (useColorTexture)
  	colorTexture.FromQByteArray(gNetRender->GetTexture(materialParam->Get<QString>(Name("file_color_texture", id))),
																cTexture::useMipmaps);

  	if (useDiffusionTexture)
  		diffusionTexture.FromQByteArray(gNetRender->GetTexture(materialParam->Get<QString>(Name("file_diffusion_texture", id))),
																			cTexture::useMipmaps);

  	if (useLuminosityTexture)
  		luminosityTexture.FromQByteArray(gNetRender->GetTexture(materialParam->Get<QString>(Name("file_luminosity_texture", id))),
																			 cTexture::useMipmaps);

  	 if (useDisplacementTexture)
  		 displacementTexture.FromQByteArray(gNetRender->GetTexture(materialParam->Get<QString>(Name("file_displacement_texture", id))),
																					cTexture::doNotUseMipmaps);

     if (useNormalMapTexture)
     {
    	 normalMapTexture.FromQByteArray(gNetRender->GetTexture(materialParam->Get<QString>(Name("file_normal_map_texture", id))),
																			 cTexture::doNotUseMipmaps);

       if(materialParam->Get<bool>(Name("normal_map_texture_invert_green", id))) normalMapTexture.SetInvertGreen(true);
     }
  }
  else
  {
    if (useColorTexture)
      colorTexture = cTexture(materialParam->Get<QString>(Name("file_color_texture", id)), cTexture::useMipmaps, quiet);

    if (useDiffusionTexture)
      diffusionTexture = cTexture(materialParam->Get<QString>(Name("file_diffusion_texture", id)), cTexture::useMipmaps, quiet);

    if (useLuminosityTexture)
      luminosityTexture = cTexture(materialParam->Get<QString>(Name("file_luminosity_texture", id)), cTexture::useMipmaps, quiet);

    if (useDisplacementTexture)
      displacementTexture = cTexture(materialParam->Get<QString>(Name("file_displacement_texture", id)), cTexture::doNotUseMipmaps, quiet);

    if (useNormalMapTexture)
    {
      normalMapTexture = cTexture(materialParam->Get<QString>(Name("file_normal_map_texture", id)), cTexture::useMipmaps, quiet);
      if(materialParam->Get<bool>(Name("normal_map_texture_invert_green", id))) normalMapTexture.SetInvertGreen(true);
    }
  }

  rotMatrix.SetRotation2(textureRotation / 180 * M_PI);
}

void CreateMaterialsMap(const cParameterContainer *params, QMap<int, cMaterial> *materials, bool quiet)
{
	QList<QString> listOfParameters = params->GetListOfParameters();
	for (int i = 0; i < listOfParameters.size(); i++)
	{
		QString parameterName = listOfParameters.at(i);
		if (parameterName.left(3) == "mat")
		{
			int positionOfDash = parameterName.indexOf('_');
			int matIndex = parameterName.mid(3, positionOfDash - 3).toInt();
			QString shortName = parameterName.mid(positionOfDash+1);
			if(shortName == "is_defined")
			{
				materials->insert(matIndex, cMaterial(matIndex, params, quiet));
			}
		}
	}
}
