/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * cVoxelExport calculates voxel volume and stores data in image layers for 3d reconstruction
 *
 * This class calculates the volume in between the limitMin and limitMax points
 * with a resolution of w * h * l. for each voxel ProcessVolume() determines if the point
 * is inside the fractal, or not. The result is saved in layers of X-Y planes in StoreLayer
 * to the output folder as a black-and-white PNG file.
 */

#ifndef MANDELBULBER2_SRC_VOXEL_EXPORT_HPP_
#define MANDELBULBER2_SRC_VOXEL_EXPORT_HPP_

#include "algebra.hpp"
#include <QtCore>

class cVoxelExport : public QObject
{
	Q_OBJECT

public:
	cVoxelExport(int w, int h, int l, CVector3 limitMin, CVector3 limitMax, QDir folder, int maxIter);
	~cVoxelExport();

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void finished();

public slots:
	void Stop() { stop = true; }
	void ProcessVolume();

private:
	bool StoreLayer(int z) const;

	unsigned char *voxelLayer;
	int w, h, l;
	CVector3 limitMin;
	CVector3 limitMax;
	QDir folder;
	int maxIter;
	bool stop;
};

#endif /* MANDELBULBER2_SRC_VOXEL_EXPORT_HPP_ */
