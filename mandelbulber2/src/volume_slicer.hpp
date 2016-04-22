/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cVolumeSlicer calculates voxel volume and stores data in image slices for 3d reconstruction
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

#ifndef VOLUME_SLICER_H_
#define VOLUME_SLICER_H_

#include "algebra.hpp"
#include "global_data.hpp"
#include "system.hpp"

class cVolumeSlicer: public QObject
{
Q_OBJECT

public:
	cVolumeSlicer(int w, int h, int l, CVector3 tlf, CVector3 brb, QString folder, int maxIter) :
		QObject()
	{
		this->w = w;
		this->h = h;
		this->l = l;
		this->tlf = tlf;
		this->brb = brb;
		this->folder = folder;
		this->maxIter = maxIter;
		voxelSlice = new unsigned char[w * h];
		stop = false;
		emit updateProgressAndStatus(tr("Idle"), "", 0.0);
	}

	~cVolumeSlicer()
	{
		delete voxelSlice;
	}

	bool StoreSlice(int z);

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void finished();

public slots:
	void Stop(){ stop = true; }
	void ProcessVolume();

private:
	unsigned char* voxelSlice;
	int w, h, l;
	CVector3 tlf;
	CVector3 brb;
	QString folder;
	int maxIter;
	bool stop;
};


#endif /* VOLUME_SLICER_H_ */
