/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * dynamic data for textures loaded to OpenCL kernel
 */

#ifndef MANDELBULBER2_SRC_OPENCL_TEXTURES_DATA_H_
#define MANDELBULBER2_SRC_OPENCL_TEXTURES_DATA_H_

#ifdef USE_OPENCL

#include "opencl_abstract_dynamic_data.h"

struct sTextures;
class cMaterial;
class cTexture;

class cOpenClTexturesData : public cOpenClAbstractDynamicData
{
public:
	cOpenClTexturesData(int numberOfTextures);
	~cOpenClTexturesData();

	static int CheckNumberOfTextures(
		const sTextures &textures, const QMap<int, cMaterial> &materials);
	void BuildAllTexturesData(const sTextures &textures, const QMap<int, cMaterial> &materials,
		QMap<QString, int> *textureIndexes);
	QString GetDefinesCollector() const;

private:
	static bool CountTexture(
		const cTexture *texture, bool grey16bit, QSet<QString> *listOfTextures, int *counter);
	void BuildTextureData(const cTexture *texture, int textureIndex, bool grey16bit);

	int numberOfTextures;
	bool useColorTexture;
	bool useDiffussionTexture;
	bool useLuminosityTexture;
	bool useNormalMapTexture;
	bool useNormalMapTextureFromBumpmap;
	bool useDisplacementMap;
	bool useReflectanceTexture;
	bool useTransparencyTexture;
	bool useRoughnessTexture;

	bool usePlanarMapping;
	bool useCylindicalMapping;
	bool useSphericalMapping;
	bool useCubicMapping;
};

#endif // USE_OPENCL

#endif /* MANDELBULBER2_SRC_OPENCL_TEXTURES_DATA_H_ */
