/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cVoxelExport calculates voxel volume and stores data in image layers for 3d reconstruction
 *
 * Copyright (C) 2016 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Sebastian Jennen (sebastian.jennen@gmx.de)
 */

#ifndef VOXEL_EXPORT_H_
#define VOXEL_EXPORT_H_

#include <QtCore>
#include "algebra.hpp"

class cVoxelExport: public QObject
{
Q_OBJECT

public:
	cVoxelExport(int w, int h, int l, CVector3 limitMin, CVector3 limitMax, QString folder, int maxIter);
	~cVoxelExport();

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void finished();

public slots:
	void Stop(){ stop = true; }
	void ProcessVolume();

private:
	bool StoreLayer(int z);

	unsigned char* voxelLayer;
	int w, h, l;
	CVector3 limitMin;
	CVector3 limitMax;
	QString folder;
	int maxIter;
	bool stop;
};


#endif /* VOXEL_EXPORT_H_ */
