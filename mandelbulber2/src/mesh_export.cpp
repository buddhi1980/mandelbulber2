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
 * Authors: Stanislaw Adaszewski
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
#include "initparameters.hpp"
#include "marchingcubes.h"
#include "nine_fractals.hpp"

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

cMeshExport::~cMeshExport()
{
}

struct ProgressFtor
{
	cMeshExport *meshExport;

	ProgressFtor(cMeshExport *meshExport) { this->meshExport = meshExport; }

	void operator()(int i) const { meshExport->updateProgressAndStatus(i); }
};

struct FormulaFtor
{
	double dist_thresh;
	sParamRender *params;
	const cNineFractals *fractals;
	FormulaFtor(double dist_thresh, sParamRender *params, const cNineFractals *fractals)
	{

		this->dist_thresh = dist_thresh;
		this->params = params;
		this->fractals = fractals;
	}

#ifdef USE_OFFLOAD
	__declspec(target(mic))
#endif // USE_OFFLOAD

		double
		operator()(double x, double y, double z, double *colorIndex) const
	{
		CVector3 point;
		point.x = x;
		point.y = y;
		point.z = z;

		sDistanceOut distanceOut;
		sDistanceIn distanceIn(point, dist_thresh, false);

		double dist = CalculateDistance(*params, *fractals, distanceIn, &distanceOut);

		// if (dist <= dist_thresh) {

		sFractalIn fractIn(point, params->minN, params->N, params->common, -1);
		sFractalOut fractOut;

		Compute<fractal::calcModeColouring>(*fractals, fractIn, &fractOut);

		*colorIndex = fractOut.colorIndex;
		//    return 1;
		// } else {
		//    *colorIndex = 0;
		//    return 0;
		// }

		return dist;

		// return (double)(dist <= dist_thresh);
	}
};

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
	QScopedPointer<const cNineFractals> fractals(new cNineFractals(gParFractal, gPar));
	QScopedPointer<sParamRender> params(new sParamRender(gPar));

	params->N = maxIter;

	double stepX = (limitMax.x - limitMin.x) * (1.0 / w);
	double stepY = (limitMax.y - limitMin.y) * (1.0 / h);
	double stepZ = (limitMax.z - limitMin.z) * (1.0 / l);
	double dist_thresh = 0.5 * dMin(stepX, stepY, stepZ) / params->detailLevel;

	progressText.ResetTimer();

	double lower[] = {limitMin.x, limitMin.y, limitMin.z};
	double upper[] = {limitMax.x, limitMax.y, limitMax.z};
	vector<double> vertices;
	vector<long long> polygons;
	vector<double> colorIndices;

	ProgressFtor progressFtor(this);
	FormulaFtor formulaFtor(dist_thresh, params.data(), fractals.data());

	WriteLog("Starting marching cubes...", 2);

	mc::marching_cubes<double, double[3], FormulaFtor, ProgressFtor>(lower, upper, w, h, l,
		formulaFtor, dist_thresh, vertices, polygons, &stop, progressFtor, colorIndices);

	WriteLog("Marching cubes done.", 2);

	cColorPalette palette = gPar->Get<cColorPalette>("mat1_surface_color_palette");
	std::vector<sRGB8> colorsRGB;

	for (unsigned int i = 0; i < colorIndices.size(); i++)
	{
		sRGB color = palette.IndexToColour(colorIndices[i]);
		sRGB8 color8(color.R, color.G, color.B);
		colorsRGB.push_back(color8);
	}

	// Save to file
	MeshFileSave::structSaveMeshData meshData(vertices, polygons, colorsRGB);
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
