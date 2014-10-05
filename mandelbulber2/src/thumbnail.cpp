/*
 * thumbnail.cpp
 *
 *  Created on: Sep 14, 2014
 *      Author: krzysztof
 */

#include "thumbnail.hpp"
#include "files.h"
#include <QFileInfo>

cThumbnail::cThumbnail(const cParameterContainer *_params, const cParameterContainer *_fractal, int _width, int _height, const QString &_hash = QString())
	: params(_params), fractal(_fractal), width(_width), height(_height), hash(_hash)
{
	image = NULL;
	qwidget = NULL;
	image = new cImage(width, height);
	//image->CreatePreview(1.0, width, height, qwidget);
}

cThumbnail::~cThumbnail()
{
	delete image;
}

QPixmap cThumbnail::Render()
{
	QPixmap pixmap;

	QString thumbnailFileName = systemData.thumbnailDir + hash + QString(".png");
	if(QFileInfo::exists(thumbnailFileName))
	{
		pixmap.load(thumbnailFileName);
	}
	else
	{
		cRenderJob *renderJob = new cRenderJob(params, fractal, image, qwidget);
		renderJob->UseSizeFromImage(true);
		renderJob->Init(cRenderJob::still);
		renderJob->Execute();
		QImage qimage((const uchar*)image->ConvertTo8bit(), width, height, width*sizeof(sRGB8), QImage::Format_RGB888);
		pixmap.convertFromImage(qimage);
		delete renderJob;
		pixmap.save(thumbnailFileName, "PNG");
	}
	return pixmap;
}

void cThumbnail::Save(QString filename)
{
	SaveJPEGQt(filename, image->ConvertTo8bit(), image->GetWidth(), image->GetHeight(), 85);
}


