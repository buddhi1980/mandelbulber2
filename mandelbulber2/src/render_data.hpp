/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * sRenderData struct - container for auxiliary data
 */

#ifndef MANDELBULBER2_SRC_RENDER_DATA_HPP_
#define MANDELBULBER2_SRC_RENDER_DATA_HPP_

#include <memory>
#include <vector>

#include <QDebug>

#include "hybrid_fractal_sequences.h"
#include "lights.hpp"
#include "material.h"
#include "object_data.hpp"
#include "perlin_noise_octaves.h"
#include "region.hpp"
#include "rendering_configuration.hpp"
#include "statistics.h"
#include "stereo.h"
#include "texture.hpp"
#include "objects_tree.h"

struct sTextures
{
	cTexture backgroundTexture;
	cTexture envmapTexture;
	cTexture lightmapTexture;
	QList<cTexture *> textureList;

	sTextures()
	{
		textureList.append(&backgroundTexture);
		textureList.append(&envmapTexture);
		textureList.append(&lightmapTexture);
	};
};

struct sRenderData
{
	sRenderData() {}

	int rendererID{0};
	cRegion<int> screenRegion;
	cRegion<double> imageRegion;
	sTextures textures;
	cLights lights;
	bool *stopRequest{nullptr};
	double lastPercentage{1.0};
	double reduceDetail{1.0};
	cStatistics statistics;
	QVector<int> netRenderStartingPositions;
	cRenderingConfiguration configuration;

	std::unique_ptr<cPerlinNoiseOctaves> perlinNoise;
	std::vector<cMaterial> materials; // indexed by material ID
	std::vector<cObjectData> objectData;
	cStereo stereo;
	std::vector<cObjectsTree::sNodeDataForRendering> nodesDataForRendering;
	cHybridFractalSequences hybridFractalSequences;

	void ValidateObjects()
	{
		for (cObjectData &object : objectData)
		{
			int materialId = object.materialId;

			if (materials.empty())
			{
				qCritical() << "No materials defined! Adding empty material";
				materials.resize(2); // index 0 unused, index 1 = default empty material
			}

			if (materialId < 0 || materialId >= static_cast<int>(materials.size()))
			{
				qCritical() << QString("Material #%1 is not defined. Will be substituted by material #1")
												 .arg(materialId);
				object.materialId = 1;
			}
		}
	}
};

#endif /* MANDELBULBER2_SRC_RENDER_DATA_HPP_ */
