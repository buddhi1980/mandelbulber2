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
#include "compute_fractal.hpp"

cVolumeSlicer::cVolumeSlicer(int w, int h, int l, CVector3 tlf, CVector3 brb, QString folder, int maxIter): QObject()
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
}

cVolumeSlicer::~cVolumeSlicer()
{
	delete voxelSlice;
}

void cVolumeSlicer::ProcessVolume()
{
	const cNineFractals *fractals = new cNineFractals(gParFractal, gPar);
	cParamRender *params = new cParamRender(gPar);

	double stepX = (brb.x - tlf.x) * (1.0 / w);
	double stepY = (brb.y - tlf.y) * (1.0 / h);
	double stepZ = (brb.z - tlf.z) * (1.0 / l);

	for(int z = 0; z < l; z++)
	{
		QString progressText = tr("Processing slice %1 of %2").arg(QString::number(z + 1), QString::number(l));
		emit updateProgressAndStatus(tr("Volume Slicing"), progressText, (double) z / l);

#pragma omp parallel for
		for(int x = 0; x < w; x++)
		{
			sFractalOut fractOut;
			CVector3 point;
			for(int y = 0; y < h; y++)
			{
				if(stop) break;
				point.x = tlf.x + x * stepX;
				point.y = tlf.y + y * stepY;
				point.z = tlf.z + z * stepZ;

				if(fractOut.distance > fabs(stepY))
				{
					// since last voxel was at least stepY away from surface
					fractOut.distance -= fabs(stepY);
				}
				else
				{
					const sFractalIn fractIn(point, params->minN, this->maxIter, params->common, -1);
					Compute<fractal::calcModeNormal>(*fractals, fractIn, &fractOut);
				}
				// if maxiter reached -> inside fractal -> paint 0 (black)
				// otherwise outside fractal -> paint 1 (white)
				voxelSlice[x + y * w] = fractOut.maxiter ? 0 : 1;
			}
		}

		if(stop || !StoreSlice(z))
		{
			break;
		}
	}

	delete fractals;
	delete params;
	emit updateProgressAndStatus(tr("Volume Slicing"), "Finished", 1.0);
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

