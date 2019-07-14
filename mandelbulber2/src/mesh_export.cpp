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
 * Authors: Stanislaw Adaszewski, Sebastian Jennen (jenzebas@gmail.com)
 *
 * cMeshExport - exports the fractal volume in ply format.
 */

#include "mesh_export.hpp"

#include "calculate_distance.hpp"
#include "common_math.h"
#include "compute_fractal.hpp"
#include "file_image.hpp"
#include "file_mesh.hpp"
#include "fractal_container.hpp"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "initparameters.hpp"
#include "marchingcubes.h"
#include "material.h"
#include "nine_fractals.hpp"
#include "render_data.hpp"

cMeshExport::cMeshExport(int w, int h, int l, CVector3 limitMin, CVector3 limitMax,
	QString outputFileName, int maxIter, MeshFileSave::structSaveMeshConfig meshConfig)
		: QObject()
{
	this->w = w;
	this->h = h;
	this->l = l;
	this->limitMin = limitMin;
	this->limitMax = limitMax;
	this->outputFileName = outputFileName;
	this->maxIter = maxIter;
	this->meshConfig = meshConfig;
	stop = false;
}

cMeshExport::~cMeshExport() = default;

void cMeshExport::updateProgressAndStatus(int i)
{
	QString statusText =
		" - " + tr("Processing layer %1 of %2").arg(QString::number(i + 1), QString::number(w));

	double percentDone = double(i) / w;

	emit updateProgressAndStatus(
		tr("Mesh Export") + statusText, progressText.getText(percentDone), percentDone);
}

void cMeshExport::ProcessVolume()
{
	QScopedPointer<sRenderData> renderData(new sRenderData);
	renderData->objectData.resize(NUMBER_OF_FRACTALS);

	QScopedPointer<cNineFractals> fractals(new cNineFractals(gParFractal, gPar));
	QScopedPointer<sParamRender> params(new sParamRender(gPar, &renderData->objectData));

	CreateMaterialsMap(gPar, &renderData.data()->materials, false, true);

	renderData->ValidateObjects();

	params->N = maxIter;

	// calculate uniform mesh step
	double sizeX = (limitMax.x - limitMin.x);
	double sizeY = (limitMax.y - limitMin.y);
	double sizeZ = (limitMax.z - limitMin.z);
	double maxSize = dMax(sizeX, sizeY, sizeZ);
	double step = maxSize / w;

	double dist_thresh;
	if (!params->constantDEThreshold)
		dist_thresh = 0.5 * step / params->detailLevel;
	else
		dist_thresh = params->DEThresh;

	// extension by one at each side
	double extension = step + dist_thresh;
	limitMax += CVector3(extension, extension, extension);
	limitMin -= CVector3(extension, extension, extension);

	w = int(sizeX / step);
	h = int(sizeY / step);
	l = int(sizeZ / step);

	limitMax.x = limitMin.x + w * step;
	limitMax.y = limitMin.y + h * step;
	limitMax.z = limitMin.z + l * step;

	// update fractal limits to calculated box
	params.data()->limitMin = limitMin + CVector3(extension, extension, extension);
	params.data()->limitMax = limitMax - CVector3(extension, extension, extension);

	progressText.ResetTimer();

	vector<double> vertices;
	vector<long long> polygons;
	vector<double> colorIndices;

	WriteLog("Starting marching cubes...", 2);
	MarchingCubes *marchingCube;
	try
	{
		marchingCube =
			new MarchingCubes(gPar, gParFractal, params.data(), fractals.data(), renderData.data(), w, h,
				l, limitMin, limitMax, dist_thresh, &stop, vertices, polygons, colorIndices);
	}
	catch (std::bad_alloc &ba)
	{
		QString errorMessage = QString("bad_alloc caught in MarchingCubes: ") + ba.what()
													 + ", maybe required mesh dimension to big?";
		qCritical() << errorMessage;
		emit updateProgressAndStatus(errorMessage, "Error occured", 0.0);
		emit finished();
		return;
	}

	QThread *thread = new QThread();
	marchingCube->moveToThread(thread);
	QObject::connect(
		marchingCube, SIGNAL(updateProgressAndStatus(int)), this, SLOT(updateProgressAndStatus(int)));
	connect(thread, SIGNAL(started()), marchingCube, SLOT(RunMarchingCube()));
	connect(marchingCube, SIGNAL(finished()), thread, SLOT(quit()));
	connect(thread, SIGNAL(finished()), marchingCube, SLOT(deleteLater()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	thread->start();
	while (!thread->isFinished())
	{
		gApplication->processEvents();
		Wait(100); // wait till finished
	}

	WriteLog("Marching cubes done.", 2);

	std::vector<sRGB8> colorsRGB;
	cColorGradient gradient;
	gradient.SetColorsFromString(gPar->Get<QString>("mat1_surface_color_gradient"));
	double colorSpeed = gPar->Get<double>("mat1_coloring_speed");
	double colorOffset = gPar->Get<double>("mat1_coloring_palette_offset");

	for (double colorIndice : colorIndices)
	{
		double nrCol = fmod(fabs(colorIndice), 248.0 * 256.0); // kept for compatibility
		double colorPosition = fmod(nrCol / 256.0 / 10.0 * colorSpeed + colorOffset, 1.0);
		sRGB color = gradient.GetColor(colorPosition, false);
		sRGB8 color8(uchar(color.R), uchar(color.G), uchar(color.B));
		colorsRGB.push_back(color8);
	}

	// Save to file
	MeshFileSave::structSaveMeshData meshData(&vertices, &polygons, &colorsRGB);
	MeshFileSave *meshFileSave = MeshFileSave::create(outputFileName, meshConfig, meshData);
	QObject::connect(meshFileSave,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	meshFileSave->SaveMesh();

	QString statusText;
	if (stop)
		statusText = tr("Mesh Export finished - Cancelled export");
	else
		statusText = tr("Mesh Export finished - Processed %1 layers").arg(QString::number(w));
	emit updateProgressAndStatus(statusText, progressText.getText(1.0), 1.0);
	emit finished();
}
