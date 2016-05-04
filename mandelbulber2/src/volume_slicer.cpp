/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cVolume calculates voxel volume and stores data in image slices for 3d reconstruction
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

#include "volume_slicer.hpp"
#include "calculate_distance.hpp"
#include "common_math.h"
#include "progress_text.hpp"
#include "nine_fractals.hpp"
#include "file_image.hpp"
#include "initparameters.hpp"

cVolumeSlicer::cVolumeSlicer(int w, int h, int l, CVector3 limitMin, CVector3 limitMax, QString folder, int maxIter): QObject()
{
	this->w = w;
	this->h = h;
	this->l = l;
	this->limitMin = limitMin;
	this->limitMax = limitMax;
	this->folder = folder;
	this->maxIter = maxIter;
	voxelSlice = new unsigned char[w * h];
	stop = false;
}

cVolumeSlicer::~cVolumeSlicer()
{
	delete voxelSlice;
}

void cVolumeSlicer::ProcessVolume()
{
	const cNineFractals *fractals = new cNineFractals(gParFractal, gPar);
	cParamRender *params = new cParamRender(gPar);

	params->N = maxIter;

	double stepX = (limitMax.x - limitMin.x) * (1.0 / w);
	double stepY = (limitMax.y - limitMin.y) * (1.0 / h);
	double stepZ = (limitMax.z - limitMin.z) * (1.0 / l);
	double dist_thresh = 0.5 * dMin(stepX, stepY, stepZ) / params->detailLevel;

	cProgressText progressText;
	progressText.ResetTimer();

	for (int z = 0; z < l; z++)
	{
		QString statusText = " - " + tr("Processing slice %1 of %2").arg(	QString::number(z + 1),
																																QString::number(l));
		double percentDone = (double) z / l;
		emit updateProgressAndStatus(tr("Volume Slicing") + statusText, progressText.getText(percentDone), percentDone);

		#pragma omp parallel for
		for (int x = 0; x < w; x++)
		{
			CVector3 point;
			for (int y = 0; y < h; y++)
			{
				if (stop) break;
				point.x = limitMin.x + x * stepX;
				point.y = limitMin.y + y * stepY;
				point.z = limitMin.z + z * stepZ;

				sDistanceOut distanceOut;
				sDistanceIn distanceIn(point, dist_thresh, false);

				double dist = CalculateDistance(*params, *fractals, distanceIn, &distanceOut);

				voxelSlice[x + y * w] = dist > dist_thresh;
			}
		}

		if (stop || !StoreSlice(z))
		{
			break;
		}
	}

	delete fractals;
	delete params;
	QString statusText;
	if(stop)
		statusText = tr("Volume Slicing finished - Cancelled slicer");
	else
		statusText = tr("Volume Slicing finished - Processed %1 slices").arg(QString::number(l));
	emit updateProgressAndStatus(statusText, progressText.getText(1.0), 1.0);
	emit finished();
}

bool cVolumeSlicer::StoreSlice(int z){
	QString filename = folder + QString("strip_%1.png").arg(z, 5, 10, QChar('0'));
	if(!ImageFileSavePNG::SavePNGQtBlackAndWhite(filename, voxelSlice, w, h))
	{
		qCritical() << "Cannot write to file " << filename;
		return false;
	}
	return true;
}

