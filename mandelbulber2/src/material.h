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
#include "fractal_coloring.hpp"

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

enum enumTextureSelection
{
	texColor = 0,
	texDiffuse = 1,
	texLuminosity = 2,
	texBumpmap = 3,
};

  int id;

  double shading;
  double specular;
  double specularWidth;
  double reflectance;
  double luminosity;
  double transparencyIndexOfRefraction;
  double transparencyOfInterior;
  double transparencyOfSurface;
  double paletteOffset;
  double coloring_speed;
  double colorTextureIntensity;
  double diffussionTextureIntensity;
  double luminosityTextureIntensity;
  double bumpmapTextureHeight;

  sRGB color;
  sRGB luminosityColor;
  sRGB transparencyInteriorColor;
  sRGB specularColor;

  cColorPalette palette;

  cTexture colorTexture;
  cTexture diffusionTexture;
  cTexture luminosityTexture;
  cTexture bumpmapTexture;

  CVector3 textureCenter;
  CVector3 textureRotation;
  CVector3 textureScale;

  CRotationMatrix rotMatrix;

  enumTextureMapping textureMappingType;
  bool fresnelReflectance;
  bool useColorsFromPalette;

  bool useColorTexture;
  bool useDiffusionTexture;
  bool useLuminosityTexture;
  bool useBumpmapTexture;

  sFractalColoring fractalColoring;
};



#endif /* MANDELBULBER2_SRC_MATERIAL_H_ */
