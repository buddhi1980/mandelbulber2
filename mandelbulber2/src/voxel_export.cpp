/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <QScopedPointer>
#include <QVector>
#include <QtCore>

#include "calculate_distance.hpp"
#include "common_math.h"
#include "file_image.hpp"
#include "fractal_container.hpp"
#include "fractparams.hpp"
#include "initparameters.hpp"
#include "nine_fractals.hpp"
#include "object_data.hpp"
#include "opencl_engine_render_fractal.h"
#include "opencl_global.h"
#include "progress_text.hpp"
#include "render_data.hpp"

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
	voxelLayer.reset(new unsigned char[w * h]);
	stop = false;
}

cVoxelExport::~cVoxelExport()
{
	// nothing to delete
}

void cVoxelExport::ProcessVolume()
{
	QScopedPointer<sRenderData> renderData(new sRenderData);
	renderData->objectData.resize(NUMBER_OF_FRACTALS);

	QScopedPointer<cNineFractals> fractals(new cNineFractals(gParFractal, gPar));
	QScopedPointer<sParamRender> params(new sParamRender(gPar, &renderData->objectData));

	CreateMaterialsMap(gPar, &renderData.data()->materials, false, true, false);

	renderData->ValidateObjects();

	params->N = maxIter;

	const double stepX = (limitMax.x - limitMin.x) * (1.0 / w);
	const double stepY = (limitMax.y - limitMin.y) * (1.0 / h);
	const double stepZ = (limitMax.z - limitMin.z) * (1.0 / l);

	double dist_thresh;
	if (!params->constantDEThreshold)
		dist_thresh = 0.5 * dMin(stepX, stepY, stepZ) / params->detailLevel;
	else
		dist_thresh = params->DEThresh;

	cProgressText progressText;
	progressText.ResetTimer();

	bool openClEnabled = false;
	QScopedArrayPointer<double> voxelDistances;

#ifdef USE_OPENCL
	openClEnabled =
		gPar->Get<bool>("opencl_enabled")
		&& cOpenClEngineRenderFractal::enumClRenderEngineMode(gPar->Get<int>("opencl_mode"))
				 != cOpenClEngineRenderFractal::clRenderEngineTypeNone;

	sClMeshExport clMeshParams;
	clMeshParams.distThresh = dist_thresh;
	clMeshParams.limitMax = toClFloat3(limitMax);
	clMeshParams.limitMin = toClFloat3(limitMin);
	clMeshParams.maxiter = params->N;
	clMeshParams.size = toClInt3(w, h, l);
	clMeshParams.sliceHeight = h;
	clMeshParams.sliceWidth = w;
	clMeshParams.coloredMesh = false;

	if (openClEnabled)
	{
		gOpenCl->openClEngineRenderFractal->Lock();
		gOpenCl->openClEngineRenderFractal->SetParameters(
			gPar, gParFractal, params.data(), fractals.data(), renderData.data(), true);
		gOpenCl->openClEngineRenderFractal->SetMeshExportParameters(&clMeshParams);
		if (gOpenCl->openClEngineRenderFractal->LoadSourcesAndCompile(gPar))
		{
			gOpenCl->openClEngineRenderFractal->CreateKernel4Program(gPar);
			WriteLogDouble("OpenCl render fractal - needed mem:",
				gOpenCl->openClEngineRenderFractal->CalcNeededMemory() / 1048576.0, 2);
			gOpenCl->openClEngineRenderFractal->PreAllocateBuffers(gPar);
			gOpenCl->openClEngineRenderFractal->CreateCommandQueue();
		}
		else
		{
			emit finished();
			gOpenCl->openClEngineRenderFractal->ReleaseMemory();
			gOpenCl->openClEngineRenderFractal->Unlock();
			return;
		}

		voxelDistances.reset(new double[w * h]);
	}

#endif // USE_OPENCL

	for (long long z = 0; z < l; z++)
	{
		const QString statusText =
			" - " + tr("Processing layer %1 of %2").arg(QString::number(z + 1), QString::number(l));
		const double percentDone = double(z) / l;
		emit updateProgressAndStatus(
			tr("Voxel Export") + statusText, progressText.getText(percentDone), percentDone);

#ifdef USE_OPENCL
		if (openClEnabled)
		{
			bool result = gOpenCl->openClEngineRenderFractal->Render(
				voxelDistances.data(), nullptr, z, renderData->stopRequest, renderData.data(), 0);

			if (!result)
			{
				gOpenCl->openClEngineRenderFractal->ReleaseMemory();
				gOpenCl->openClEngineRenderFractal->Unlock();
				return;
			}

			for (long long x = 0; x < w; x++)
			{
				for (long long y = 0; y < h; y++)
				{
					long long address = x + y * w;
					voxelLayer[address] = static_cast<unsigned char>(voxelDistances[address] <= dist_thresh);
				}
			}
		}

#endif // USE_OPENCL

		if (!openClEnabled)
		{
#pragma omp parallel for schedule( \
	dynamic, 1) // long long is used because size_t doesn't work with msvc and OpenMP
			for (long long x = 0; x < w; x++)
			{
				CVector3 point;
				for (long long y = 0; y < h; y++)
				{
					if (stop) break;
					point.x = limitMin.x + x * stepX;
					point.y = limitMin.y + y * stepY;
					point.z = limitMin.z + z * stepZ;

					sDistanceOut distanceOut;
					const sDistanceIn distanceIn(point, dist_thresh, false);

					const double dist = CalculateDistance(*params, *fractals, distanceIn, &distanceOut);

					voxelLayer[x + y * w] = static_cast<unsigned char>(dist <= dist_thresh);
				} // for y
			}		// for x
		}			// if not openClEnabled

		if (stop || !StoreLayer(z))
		{
			break;
		}
	}

#ifdef USE_OPENCL
	if (openClEnabled)
	{
		gOpenCl->openClEngineRenderFractal->ReleaseMemory();
		gOpenCl->openClEngineRenderFractal->Unlock();
	}
#endif // USE_OPENCL

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
	const QString filename =
		folder.absolutePath() + QDir::separator() + QString("layer_%1.png").arg(z, 5, 10, QChar('0'));
	if (!ImageFileSavePNG::SavePNGQtBlackAndWhite(filename, voxelLayer.data(), w, h))
	{
		qCritical() << "Cannot write to file " << filename;
		return false;
	}
	return true;
}
