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

#ifndef MANDELBULBER2_SRC_MATERIAL_H_
#define MANDELBULBER2_SRC_MATERIAL_H_

#include <QMap>

#include "color_gradient.h"
#include "color_structures.hpp"
#include "fractal_coloring.hpp"
#include "texture.hpp"
#include "texture_enums.hpp"

// forward declarations
class cParameterContainer;

class cMaterial
{
public:
	cMaterial();
	cMaterial(int _id, const cParameterContainer *materialParam, bool loadTextures, bool quiet);
	~cMaterial();
	void setParameters(
		int _id, const cParameterContainer *materialParam, bool loadTextures, bool quiet);

	static QString Name(const QString &name, int materialId)
	{
		return QString("mat%1_").arg(materialId) + name;
	}

	static QStringList paramsList;

	int id;
	int textureFractalizeStartIteration;

	float shading;
	float specular;
	float specularWidth;
	float specularMetallic;
	float specularMetallicRoughness;
	float specularMetallicWidth;
	float reflectance;
	float luminosity;
	float surfaceRoughness;
	float transparencyIndexOfRefraction;
	float transparencyOfInterior;
	float transparencyOfSurface;
	double paletteOffset;
	double coloring_speed;
	float colorTextureIntensity;
	float diffusionTextureIntensity;
	float luminosityTextureIntensity;
	float reflectanceTextureIntensity;
	float transparencyTextureIntensity;
	float roughnessTextureIntensity;
	double displacementTextureHeight;
	double normalMapTextureHeight;
	double iridescenceIntensity;
	double iridescenceSubsurfaceThickness;
	double textureFractalizeCubeSize;

	sRGB color;
	sRGB luminosityColor;
	sRGB transparencyInteriorColor;
	sRGB reflectionsColor;
	sRGB specularColor;
	sRGB transparencyColor;

	cColorGradient gradientSurface;
	cColorGradient gradientSpecular;
	cColorGradient gradientDiffuse;
	cColorGradient gradientLuminosity;
	cColorGradient gradientRoughness;
	cColorGradient gradientReflectance;
	cColorGradient gradientTransparency;

	cTexture colorTexture;
	cTexture diffusionTexture;
	cTexture luminosityTexture;
	cTexture displacementTexture;
	cTexture normalMapTexture;
	cTexture reflectanceTexture;
	cTexture transparencyTexture;
	cTexture roughnessTexture;

	CVector3 textureCenter;
	CVector3 textureRotation;
	CVector3 textureScale;

	CRotationMatrix rotMatrix;

	texture::enumTextureMapping textureMappingType;
	bool fresnelReflectance;
	bool useColorsFromPalette;
	bool specularPlasticEnable;
	bool metallic;
	bool roughSurface;

	bool useColorTexture;
	bool useDiffusionTexture;
	bool useLuminosityTexture;
	bool useDisplacementTexture;
	bool useNormalMapTexture;
	bool normalMapTextureFromBumpmap;
	bool normalMapTextureInvertGreen;
	bool useReflectanceTexture;
	bool useTransparencyTexture;
	bool useRoughnessTexture;
	bool iridescenceEnabled;
	bool textureFractalize;

	bool surfaceGradientEnable;
	bool specularGradientEnable;
	bool diffuseGradientEnable;
	bool luminosityGradientEnable;
	bool roughnessGradientEnable;
	bool reflectanceGradientEnable;
	bool transparencyGradientEnable;

	sFractalColoring fractalColoring;
};

void CreateMaterialsMap(const cParameterContainer *params, QMap<int, cMaterial> *materials,
	bool loadTextures, bool quiet);

#endif /* MANDELBULBER2_SRC_MATERIAL_H_ */
