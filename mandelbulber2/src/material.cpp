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
  textureMappingType = mappingPlanar;
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
