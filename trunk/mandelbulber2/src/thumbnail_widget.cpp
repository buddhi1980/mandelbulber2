/*
 * thumbnail_widget.cpp
 *
 *  Created on: Jan 11, 2015
 *      Author: krzysztof
 */

#include "thumbnail_widget.h"
#include "settings.hpp"
#include <QPaintEvent>
#include "render_job.hpp"
#include "global_data.hpp"
#include <QImage>
#include "system.hpp"

cThumbnailWidget::cThumbnailWidget(int _width, int _height, QWidget *parent) : QWidget(parent)
{
	tWidth = _width;
	tHeight = _height;
	image = new cImage(tWidth, tHeight);
	image->CreatePreview(1.0, width(), height(), this);
	progressBar = NULL;
	setFixedWidth(tWidth);
	setFixedHeight(tHeight);
	stopRequest = false;
	isRendered = false;
	hasParameters = false;
	connect(this, SIGNAL(renderRequest()), this, SLOT(slotRender()));
}

cThumbnailWidget::~cThumbnailWidget()
{
	stopRequest = true;
	while(image->IsUsed()) { }

	delete image;
}

void cThumbnailWidget::paintEvent(QPaintEvent *event)
{
	if (hasParameters)
	{
		if(!isRendered)
		{
			isRendered = true;
			emit renderRequest();
		}
	}
	image->RedrawInWidget(this);
}

void cThumbnailWidget::AssignParameters(const cParameterContainer &_params, const cFractalContainer &_fractal)
{
	stopRequest = true;
	while(image->IsUsed())
	{
		//just wait and pray
	}
	params = _params;
	fractal = _fractal;
	cSettings tempSettings(cSettings::formatCondensedText);
	tempSettings.CreateText(&params, &fractal);
	hash = tempSettings.GetHashCode();
	isRendered = false;
	hasParameters = true;

	QString thumbnailFileName = systemData.thumbnailDir + hash + QString(".png");
	if(QFileInfo::exists(thumbnailFileName))
	{
		stopRequest = true;
		isRendered = true;
		while(image->IsUsed())
		{
			//just wait and pray
		}

		QPixmap pixmap;
		pixmap.load(thumbnailFileName);
		pixmap = pixmap.scaled(tWidth, tHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QImage qimage = pixmap.toImage();
		qimage = qimage.convertToFormat(QImage::Format_RGB888);
		sRGB8 *bitmap;
		bitmap = (sRGB8*)(qimage.bits());
		int bwidth = qimage.width();
		int bheight = qimage.height();
		sRGB8 *previewPointer = (sRGB8*)image->GetPreviewPtr();
		memcpy(previewPointer, bitmap, sizeof(sRGB8) * bwidth * bheight);
		emit thumbnailRendered();
	}
}

void cThumbnailWidget::AssignProgressBar(QProgressBar *_progressBar)
{
	progressBar = _progressBar;
}

void cThumbnailWidget::slotRender()
{
	stopRequest = true;
	while(image->IsUsed())
	{
		//just wait and pray
	}
	stopRequest = false;
	cRenderJob *renderJob = new cRenderJob(&params, &fractal, image, &stopRequest, parent(), (QWidget*)this);
	renderJob->UseSizeFromImage(true);
	renderJob->Init(cRenderJob::still);

	QThread *thread = new QThread;
	renderJob->moveToThread(thread);
	QObject::connect(thread, SIGNAL(started()), renderJob, SLOT(slotExecute()));
	thread->start();

	QObject::connect(renderJob, SIGNAL(finished()), renderJob, SLOT(deleteLater()));
	QObject::connect(renderJob, SIGNAL(finished()), thread, SLOT(quit()));
	QObject::connect(renderJob, SIGNAL(fullyRendered()), this, SLOT(slotFullyRendered()));
}

void cThumbnailWidget::slotFullyRendered()
{
	QImage qImage((const uchar*)image->ConvertTo8bit(), image->GetWidth(), image->GetHeight(), image->GetWidth()*sizeof(sRGB8), QImage::Format_RGB888);
	QPixmap pixmap;
	pixmap.convertFromImage(qImage);

	QString thumbnailFileName = systemData.thumbnailDir + hash + QString(".png");
	pixmap.save(thumbnailFileName, "PNG");

	emit thumbnailRendered();
}
