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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * sRenderData struct - container for auxiliary data
 */

#ifndef MANDELBULBER2_SRC_RENDER_DATA_HPP_
#define MANDELBULBER2_SRC_RENDER_DATA_HPP_

#include "lights.hpp"
#include "material.h"
#include "object_data.hpp"
#include "region.hpp"
#include "rendering_configuration.hpp"
#include "statistics.h"
#include "stereo.h"
#include "texture.hpp"

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

	QMap<int, cMaterial> materials; // 'int' is an ID
	QVector<cObjectData> objectData;
	cStereo stereo;

	void ValidateObjects()
	{
		for (cObjectData &object : objectData)
		{
			// check if material assigned to the object is defined
			int materialId = object.materialId;

			if (materials.size() == 0)
			{
				qCritical() << "No materials defined! Adding empty material";
				materials.insert(1, cMaterial());
			}

			if (!materials.contains(materialId))
			{
				QList<int> keys = materials.keys();
				std::sort(keys.begin(), keys.end());
				int substituteMaterialId = keys.first();
				qCritical() << QString("Material #%1 is not defined. Will be substitubed by material #%2")
												 .arg(materialId)
												 .arg(substituteMaterialId);
				object.materialId = substituteMaterialId;
			}
		}
	}
};

#endif /* MANDELBULBER2_SRC_RENDER_DATA_HPP_ */
