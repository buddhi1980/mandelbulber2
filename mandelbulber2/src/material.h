/*
 * material.h
 *
 *  Created on: 19 mar 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_MATERIAL_H_
#define MANDELBULBER2_SRC_MATERIAL_H_

#include "color_structures.hpp"
#include "parameters.hpp"
#include "texture.hpp"

class cMaterial
{
public:
  cMaterial();
  cMaterial(const cParameterContainer &materialParam);
  ~cMaterial();
  void setParameters(const cParameterContainer &materialParam);

static QString Name(const QString &name, int materialId) {return QString("mat%1_").arg(materialId) + name;}

static QStringList paramsList;

enum cTextureMapping
{
  mappingSpherical = 0,
  mappingCubic = 1,
  mappingCylindrical = 2,
  mappingPlanar = 3
};

  double shading;
  double specular;
  double specularWidth;
  double reflection;
  double lightness;
  double bumpMapIntensity;
  double transparencyIndexOfRefraction;
  double transparencyOfInterior;
  double transparencyOfSurface;
  double paletteOffset;
  double coloring_speed;

  sRGB color;
  sRGB lightnessColor;
  sRGB transparencyInteriorColor;

  cColorPalette palette;

  cTexture colorTexture;
  cTexture diffusionTexture;
  cTexture lightnessTexture;
  cTexture bumpmapTexture;

  CVector3 textureCenter;
  CVector3 textureRotation;
  CVector3 textureScale;

  cTextureMapping textureMappingType;
  bool fresnelReflectance;
  bool useColorsFromPalette;
};



#endif /* MANDELBULBER2_SRC_MATERIAL_H_ */
