/*
 * Copyright (C) Stanislaw Adaszewski, 2016
 * http://algoholic.eu
 */

#include "mesh_export.hpp"
#include "calculate_distance.hpp"
#include "common_math.h"
#include "file_image.hpp"
#include "fractparams.hpp"
#include "fractal_container.hpp"
#include "initparameters.hpp"
#include "nine_fractals.hpp"
#include "marchingcubes.h"
#include "compute_fractal.hpp"

cMeshExport::cMeshExport(int w, int h, int l,
    CVector3 limitMin, CVector3 limitMax,
    QString outputFileName, int maxIter): QObject()
{
	this->w = w;
	this->h = h;
	this->l = l;
	this->limitMin = limitMin;
	this->limitMax = limitMax;
        this->outputFileName = outputFileName;
	this->maxIter = maxIter;
	voxelLayer = new unsigned char[w * h];
	stop = false;
}

cMeshExport::~cMeshExport()
{
	delete[] voxelLayer;
}

struct ProgressFtor {
    cMeshExport *meshExport;

    ProgressFtor(cMeshExport *meshExport) {
        this->meshExport = meshExport;
    }

    void operator()(int i) {
        meshExport->updateProgressAndStatus(i);
    }
};

struct FormulaFtor {
    double dist_thresh;
    cParamRender *params;
    const cNineFractals *fractals;

    FormulaFtor(double dist_thresh, cParamRender *params,
        const cNineFractals *fractals) {

        this->dist_thresh = dist_thresh;
        this->params = params;
        this->fractals = fractals;
    }

    double operator()(double x, double y, double z, double *colorIndex) {
        CVector3 point;
        point.x = x;
        point.y = y;
        point.z = z;

        sDistanceOut distanceOut;
        sDistanceIn distanceIn(point, dist_thresh, false);

        double dist = CalculateDistance(*params, *fractals, distanceIn, &distanceOut);

        if (dist <= dist_thresh) {

            sFractalIn fractIn(point, params->minN, params->N, params->common, -1);
            sFractalOut fractOut;

            Compute<fractal::calcModeColouring>(*fractals, fractIn, &fractOut);

            *colorIndex = fractOut.colorIndex;
            return 1;
        } else {
            *colorIndex = 0;
            return 0;
        }


        // return (double)(dist <= dist_thresh);
    }
};

void cMeshExport::updateProgressAndStatus(int i) {
    QString statusText = " - " + tr("Processing layer %1 of %2")
        .arg(QString::number(i + 1), QString::number(w));

    double percentDone = (double)i / w;

    emit updateProgressAndStatus(
        tr("Mesh Export") + statusText,
        progressText.getText(percentDone), percentDone);
}

void cMeshExport::ProcessVolume()
{
        QScopedPointer<const cNineFractals> fractals(new cNineFractals(gParFractal, gPar));
        QScopedPointer<cParamRender> params(new cParamRender(gPar));

	params->N = maxIter;

	double stepX = (limitMax.x - limitMin.x) * (1.0 / w);
	double stepY = (limitMax.y - limitMin.y) * (1.0 / h);
	double stepZ = (limitMax.z - limitMin.z) * (1.0 / l);
	double dist_thresh = 0.5 * dMin(stepX, stepY, stepZ) / params->detailLevel;

	progressText.ResetTimer();

        double lower[] = {limitMin.x, limitMin.y, limitMin.z};
        double upper[] = {limitMax.x, limitMax.x, limitMax.z};
        vector<double> vertices;
        vector<size_t> polygons;
        vector<double> colorIndices;

        ProgressFtor progressFtor(this);
        FormulaFtor formulaFtor(dist_thresh, params.data(), fractals.data());

        qDebug() << "Starting marching cubes...";

        mc::marching_cubes<double, double[3], FormulaFtor, ProgressFtor>
            (lower, upper, w, h, l, formulaFtor, 0.5, vertices,
            polygons, &stop, progressFtor, colorIndices);

        qDebug() << "Marching cubes done.";

        double maxColorIndex = -1.0 / 0.0;
        for (int i = 0; i < colorIndices.size(); i++) {
            maxColorIndex = qMax(maxColorIndex, colorIndices[i]);
        }
        for (int i = 0; i < colorIndices.size(); i++) {
            colorIndices[i] /= maxColorIndex;
        }

        qDebug() << "Opening..." << outputFileName;

        QFile f(outputFileName);
        if (!f.open(QFile::WriteOnly)) {
            QString statusText = tr("Mesh Export - Failed to open output file!");
            emit updateProgressAndStatus(statusText, progressText.getText(1.0), 1.0);
            emit finished();
            return;
        }
        f.write(QString("ply\n").toLatin1());
        f.write(QString("format ascii 1.0\n").toLatin1());
        f.write(QString("comment Mandelbulber Exported Mesh\n").toLatin1());
        f.write(QString("element vertex %1\n").arg(vertices.size() / 3).toLatin1());
        f.write(QString("property float x\n").toLatin1());
        f.write(QString("property float y\n").toLatin1());
        f.write(QString("property float z\n").toLatin1());
        f.write(QString("property float s\n").toLatin1());
        f.write(QString("property float t\n").toLatin1());
        // f.write(QString("property float red\n").toLatin1());
        // f.write(QString("property float green\n").toLatin1());
        // f.write(QString("property float blue\n").toLatin1());
        f.write(QString("element face %1\n").arg(polygons.size() / 3).toLatin1());
        f.write(QString("property list uchar int vertex_indices\n").toLatin1());
        // f.write(QString("property list uchar float texcoord\n").toLatin1());
        f.write(QString("end_header\n").toLatin1());
        for (int i = 0; i < vertices.size() / 3; i++) {

            //float v[] = {(float) vertices[i * 3], (float) vertices[i * 3 + 1],
                //(float) vertices[i * 3 + 2]};

            //f.write((char*) &v[0], sizeof(float) * 3);
            f.write(QString("%1 %2 %3 ").arg(vertices[i * 3])
                    .arg(vertices[i * 3 + 1]).arg(vertices[i * 3 + 2]).toLatin1());
            f.write(QString("%1 %1\n").arg(colorIndices[i]).toLatin1());

            /* double colorIndex = colorIndices[i];
            int nrCol = floor(colorIndex);
            nrCol = abs(nrCol) % (248 * 256);


            int color_number;
            if (nrCol >= 248 * 256)
            {
                    color_number = nrCol;
            }
            else
            {
                    color_number =
                            (int)(nrCol * input.material->coloring_speed + 256 * input.material->paletteOffset)
                            % 65536;
            }
            colour = input.material->palette.IndexToColour(color_number);


            uchar rgb[3] = {color, color, color};
            f.write((char*) &rgb[0], sizeof(char) * 3); */
        }
        for (int i = 0; i < polygons.size(); i += 3) {
            // uchar n = 3;
            // int p[] = {polygons[i], polygons[i + 1], polygons[i + 2]};
            // f.write((char*) &n, sizeof(uchar));
            // f.write((char*) &polygons[i], 3 * sizeof(int));

            f.write(QString("3 %1 %2 %3\n").arg(polygons[i])
                .arg(polygons[i + 1]).arg(polygons[i + 2]).toLatin1());

            // f.write(QString("6 %1 %1 %2 %2 %3 %3\n").arg(colorIndices[polygons[i]])
                // .arg(colorIndices[polygons[i + 1]]).arg(colorIndices[polygons[i + 2]]).toLatin1());
        }
        f.close();


        QString statusText;
	if (stop)
                statusText = tr("Mesh Export finished - Cancelled export");
	else
                statusText = tr("Mesh Export finished - Processed %1 layers").arg(QString::number(w));
	emit updateProgressAndStatus(statusText, progressText.getText(1.0), 1.0);
	emit finished();
}
