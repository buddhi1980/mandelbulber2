/*
 * thumbnail.cpp
 *
 *  Created on: Sep 14, 2014
 *      Author: krzysztof
 */

#include "thumbnail.hpp"
#include "files.h"


cThumbnail::cThumbnail(const cParameterContainer *_params, const cParameterContainer *_fractal, int _width, int _height) : params(_params), fractal(_fractal), width(_width), height(_height)
{
	image = NULL;
	qwidget = NULL;
	image = new cImage(width, height);
}

cThumbnail::~cThumbnail()
{
	delete image;
}

void cThumbnail::Render()
{
	cRenderJob *renderJob = new cRenderJob(params, fractal, image, qwidget);
	renderJob->UseSizeFromImage(true);
	renderJob->Init(cRenderJob::still);
	renderJob->Execute();
}

void cThumbnail::Save(QString filename)
{
	SaveJPEGQt(filename, image->ConvertTo8bit(), image->GetWidth(), image->GetHeight(), 85);
}


