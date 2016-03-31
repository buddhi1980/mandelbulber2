/*
 * material.cpp
 *
 *  Created on: 19 mar 2016
 *      Author: krzysztof
 */

#include "material.h"

cMaterial::cMaterial()
{
  // TODO Auto-generated constructor stub
  id = -1;
  shading = 0.0;
  specular = 0.0;
  specularWidth = 0.0;
  reflection = 0.0;
  lightness = 0.0;
  bumpMapIntensity = 0.0;
  coloring_speed = 0.0;
  paletteOffset = 0.0;
  fresnelReflectance = 0.0;
  transparencyIndexOfRefraction = 0.0;
  transparencyOfInterior = 0.0;
  transparencyOfSurface = 0.0;
  useColorsFromPalette = false;
  useColorTexture = false;
  useDiffusionTexture = false;
  useLightnessTexture = false;
  useBumpmapTexture = false;
  textureMappingType = mappingPlanar;
}

cMaterial::cMaterial(int _id, const cParameterContainer &materialParam, bool quiet)
{
  setParameters(_id, materialParam, quiet);
}

cMaterial::~cMaterial()
{
  // TODO Auto-generated destructor stub
}

//this static list will be use to optimize usage of material parameters
QStringList cMaterial::paramsList = {
    "shading",
    "specular",
    "specularWidth",
    "reflection",
    "lightness",
    "bump_map_intensity",
    "transparency_of_surface",
    "transparency_of_interior",
    "transparency_index_of_refraction",
    "surface_color",
    "transparency_interior_color",
    "lightness_color",
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
    "file_lightness_texture",
    "file_bumpmap_texture",
    "surface_color_palette",
};

void cMaterial::setParameters(int _id, const cParameterContainer &materialParam, bool quiet = false)
{
  id = _id;
  shading = materialParam.Get<double>(Name("shading", id));
  specular = materialParam.Get<double>(Name("specular", id));
  specularWidth = materialParam.Get<double>(Name("specularWidth", id));
  reflection = materialParam.Get<double>(Name("reflection", id));
  lightness = materialParam.Get<double>(Name("lightness", id));
  bumpMapIntensity = materialParam.Get<double>(Name("bump_map_intensity", id));
  transparencyIndexOfRefraction = materialParam.Get<double>(Name("transparency_index_of_refraction", id));
  transparencyOfInterior = materialParam.Get<double>(Name("transparency_of_interior", id));
  transparencyOfSurface = materialParam.Get<double>(Name("transparency_of_surface", id));
  paletteOffset = materialParam.Get<double>(Name("coloring_palette_offset", id));
  coloring_speed = materialParam.Get<double>(Name("coloring_speed", id));

  color = materialParam.Get<sRGB>(Name("surface_color", id));
  lightnessColor = materialParam.Get<sRGB>(Name("lightness_color", id));
  transparencyInteriorColor = materialParam.Get<sRGB>(Name("transparency_interior_color", id));

  palette = materialParam.Get<cColorPalette>(Name("transparency_interior_color", id));

  textureCenter = materialParam.Get<CVector3>(Name("texture_center", id));
  textureRotation = materialParam.Get<CVector3>(Name("texture_rotation", id));
  textureScale = materialParam.Get<CVector3>(Name("texture_scale", id));

  textureMappingType = (enumTextureMapping)materialParam.Get<int>(Name("texture_mapping_type", id));

  fresnelReflectance = materialParam.Get<bool>(Name("fresnel_reflectance", id));
  useColorsFromPalette = materialParam.Get<bool>(Name("use_colors_from_palette", id));

  useColorTexture = materialParam.Get<bool>(Name("use_color_texture", id));
  useDiffusionTexture = materialParam.Get<bool>(Name("use_diffusion_texture", id));
  useLightnessTexture = materialParam.Get<bool>(Name("use_lightness_texture", id));
  useBumpmapTexture = materialParam.Get<bool>(Name("use_bumpmap_texture", id));

  if (useColorTexture)
    colorTexture = cTexture(materialParam.Get<QString>("file_color_texture"), quiet);

  if (useDiffusionTexture)
    diffusionTexture = cTexture(materialParam.Get<QString>("file_diffusion_texture"), quiet);

  if (useLightnessTexture)
    lightnessTexture = cTexture(materialParam.Get<QString>("file_lightness_texture"), quiet);

  if (useBumpmapTexture)
    bumpmapTexture = cTexture(materialParam.Get<QString>("file_bumpmap_texture"), quiet);
}
