/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

#include "color_palette.hpp"
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
	cMaterial(int _id, const cParameterContainer *materialParam, bool quiet);
	~cMaterial();
	void setParameters(int _id, const cParameterContainer *materialParam, bool quiet);

	static QString Name(const QString &name, int materialId)
	{
		return QString("mat%1_").arg(materialId) + name;
	}

	static QStringList paramsList;

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
	double displacementTextureHeight;
	double normalMapTextureHeight;

	sRGB color;
	sRGB luminosityColor;
	sRGB transparencyInteriorColor;
	sRGB specularColor;

	cColorPalette palette;

	cTexture colorTexture;
	cTexture diffusionTexture;
	cTexture luminosityTexture;
	cTexture displacementTexture;
	cTexture normalMapTexture;

	CVector3 textureCenter;
	CVector3 textureRotation;
	CVector3 textureScale;

	CRotationMatrix rotMatrix;

	texture::enumTextureMapping textureMappingType;
	bool fresnelReflectance;
	bool useColorsFromPalette;

	bool useColorTexture;
	bool useDiffusionTexture;
	bool useLuminosityTexture;
	bool useDisplacementTexture;
	bool useNormalMapTexture;
	bool normalMapTextureFromBumpmap;

	sFractalColoring fractalColoring;
};

void CreateMaterialsMap(
	const cParameterContainer *params, QMap<int, cMaterial> *materials, bool quiet);

#endif /* MANDELBULBER2_SRC_MATERIAL_H_ */
