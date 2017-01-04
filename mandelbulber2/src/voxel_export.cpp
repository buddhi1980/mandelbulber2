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

#include "voxel_export.hpp"
#include "calculate_distance.hpp"
#include "common_math.h"
#include "file_image.hpp"
#include "fractparams.hpp"
#include "fractal_container.hpp"
#include "initparameters.hpp"
#include "progress_text.hpp"
#include "nine_fractals.hpp"

cVoxelExport::cVoxelExport(
	int w, int h, int l, CVector3 limitMin, CVector3 limitMax, QDir folder, int maxIter)
		: QObject()
{
	this->w = w;
	this->h = h;
	this->l = l;
	this->limitMin = limitMin;
	this->limitMax = limitMax;
	this->folder = folder;
	this->maxIter = maxIter;
	voxelLayer = new unsigned char[w * h];
	stop = false;
}

cVoxelExport::~cVoxelExport()
{
	delete[] voxelLayer;
}

void cVoxelExport::ProcessVolume()
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
		QString statusText =
			" - " + tr("Processing layer %1 of %2").arg(QString::number(z + 1), QString::number(l));
		double percentDone = double(z) / l;
		emit updateProgressAndStatus(
			tr("Voxel Export") + statusText, progressText.getText(percentDone), percentDone);

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

				voxelLayer[x + y * w] = static_cast<unsigned char>(dist <= dist_thresh);
			}
		}

		if (stop || !StoreLayer(z))
		{
			break;
		}
	}

	delete fractals;
	delete params;
	QString statusText;
	if (stop)
		statusText = tr("Voxel Export finished - Cancelled export");
	else
		statusText = tr("Voxel Export finished - Processed %1 layers").arg(QString::number(l));
	emit updateProgressAndStatus(statusText, progressText.getText(1.0), 1.0);
	emit finished();
}

bool cVoxelExport::StoreLayer(int z) const
{
	QString filename =
		folder.absolutePath() + QDir::separator() + QString("layer_%1.png").arg(z, 5, 10, QChar('0'));
	if (!ImageFileSavePNG::SavePNGQtBlackAndWhite(filename, voxelLayer, w, h))
	{
		qCritical() << "Cannot write to file " << filename;
		return false;
	}
	return true;
}
