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
  cMaterial(int _id, const cParameterContainer &materialParam, bool quiet);
  ~cMaterial();
  void setParameters(int _id, const cParameterContainer &materialParam, bool quiet);

static QString Name(const QString &name, int materialId) {return QString("mat%1_").arg(materialId) + name;}

static QStringList paramsList;

enum enumTextureMapping
{
  mappingSpherical = 0,
  mappingCubic = 1,
  mappingCylindrical = 2,
  mappingPlanar = 3
};

  int id;

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

  enumTextureMapping textureMappingType;
  bool fresnelReflectance;
  bool useColorsFromPalette;

  bool useColorTexture;
  bool useDiffusionTexture;
  bool useLightnessTexture;
  bool useBumpmapTexture;

};



#endif /* MANDELBULBER2_SRC_MATERIAL_H_ */
