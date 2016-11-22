/*
 * Copyright (C) Stanislaw Adaszewski, 2016
 * http://algoholic.eu
 */

#ifndef MANDELBULBER2_SRC_MESH_EXPORT_HPP_
#define MANDELBULBER2_SRC_MESH_EXPORT_HPP_

#include "algebra.hpp"
#include "progress_text.hpp"
#include <QtCore>

class cMeshExport : public QObject
{
	Q_OBJECT

public:
        cMeshExport(int w, int h, int l, CVector3 limitMin, CVector3 limitMax, QString outputFileName, int maxIter);
        ~cMeshExport();

        void updateProgressAndStatus(int i);

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void finished();

public slots:
	void Stop() { stop = true; }
	void ProcessVolume();

private:
	unsigned char *voxelLayer;
	int w, h, l;
	CVector3 limitMin;
	CVector3 limitMax;
        QString outputFileName;
	int maxIter;
	bool stop;
        cProgressText progressText;
};

#endif /* MANDELBULBER2_SRC_VOXEL_EXPORT_HPP_ */
