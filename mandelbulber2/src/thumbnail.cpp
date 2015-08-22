/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cThumbnail class - thumbnail rendering
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

#include "thumbnail.hpp"
#include "files.h"
#include <QFileInfo>
#include "settings.hpp"

cThumbnail::cThumbnail(const cParameterContainer *_params, const cFractalContainer *_fractal, int _width, int _height, const QString &_hash = QString())
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

	if(hash.isEmpty())
	{
		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(params, fractal);
		hash = tempSettings.GetHashCode();
	}

	QString thumbnailFileName = systemData.thumbnailDir + hash + QString(".png");
	if(QFileInfo::exists(thumbnailFileName))
	{
		pixmap.load(thumbnailFileName);
	}
	else
	{
		bool stopRequest = false;
		cRenderJob *renderJob = new cRenderJob(params, fractal, image, &stopRequest, qwidget);
		renderJob->UseSizeFromImage(true);
		renderJob->BeQuiet();
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



