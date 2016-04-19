/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cVolume calculates voxel volume and stores data in image slices for 3d reconstruction
 *
 * Copyright (C) 2014 Krzysztof Marczak
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "volume.hpp"
#include "calculate_distance.hpp"

void cVolume::CalculateVolume()
{
	cParamRender *params = new cParamRender(gPar);
	cNineFractals *fractals = new cNineFractals(gParFractal, gPar);
	sDistanceOut out;

	for(int z = 0; z < l; z++)
	{
		for(int x = 0; x < w; x++)
		{
			for(int y = 0; y < h; y++)
			{
				CVector3 point = tlf;
				point.x += (brb.x - tlf.x) * (x * 1.0 / w);
				point.y += (brb.y - tlf.y) * (y * 1.0 / h);
				point.z += (brb.z - tlf.z) * (z * 1.0 / l);

				sDistanceIn in(point, 0, false);
				double dist = CalculateDistance(*params, *fractals, in, &out);
				voxel[x + y * w + z * w * h] = dist <= 0 ? 1 : 0;
			}
		}
	}
	qDebug() << "finished!";
}

void cVolume::Store(QString folder){
	for(int z = 0; z < l; z++)
	{
		QString filename = folder + QString("strip_%1.png").arg(z, 5, 10, QChar('0'));
		ImageFileSavePNG::SavePNGQtBlackAndWhite(filename, voxel, w, h, w * h * z);
	}
}
