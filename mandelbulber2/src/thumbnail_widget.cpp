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

#include "common_math.h"
#include "system.hpp"

cThumbnailWidget::cThumbnailWidget(int _width, int _height, QObject *_parentWithProgressBar, QWidget *parent) : QWidget(parent)
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
	parentWithProgressBar = _parentWithProgressBar;
	connect(this, SIGNAL(renderRequest()), this, SLOT(slotRender()));
	params = new cParameterContainer;
	fractal = new cFractalContainer;
	useOneCPUCore = false;

	timer = new QTimer(parent);
	timer->setSingleShot(true);
}

cThumbnailWidget::~cThumbnailWidget()
{
	stopRequest = true;
	while(image->IsUsed()) { }

	delete image;
	delete timer;
	if(params) delete params;
	if(fractal) delete fractal;
	qDebug() << "cThumbnailWidget destructor";
}

void cThumbnailWidget::paintEvent(QPaintEvent *event)
{
	if (hasParameters)
	{
		if(!isRendered)
		{
			isRendered = true;
			timer->stop();
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
	*params = _params;
	*fractal = _fractal;
	cSettings tempSettings(cSettings::formatCondensedText);
	tempSettings.CreateText(params, fractal);
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
		sRGB8 *previewPointer = (sRGB8*)image->GetPreviewPrimaryPtr();
		sRGB8 *preview2Pointer = (sRGB8*)image->GetPreviewPtr();
		memcpy(previewPointer, bitmap, sizeof(sRGB8) * bwidth * bheight);
		memcpy(preview2Pointer, bitmap, sizeof(sRGB8) * bwidth * bheight);
		emit thumbnailRendered();
	}
	else
	{
		//render thumbnail after random time. It forces rendering of widgets when they are not visible. It makes rendering of widgets when iddle
    connect(timer, SIGNAL(timeout()), this, SLOT(slotRandomRender()));
    timer->start(Random(100000)*10 + 1);
	}
}

void cThumbnailWidget::slotRender()
{
	stopRequest = true;
	while(image->IsUsed())
	{
		//just wait and pray
		Wait(100);
	}
	Wait(Random(100) + 50);
	stopRequest = false;

	cRenderJob *renderJob = new cRenderJob(params, fractal, image, &stopRequest, parentWithProgressBar, (QWidget*)this);

	renderJob->UseSizeFromImage(true);
	renderJob->Init(cRenderJob::still);

	if(useOneCPUCore)	renderJob->ForceNumberOfThreads(1);

	QThread *thread = new QThread;
	renderJob->moveToThread(thread);
	QObject::connect(thread, SIGNAL(started()), renderJob, SLOT(slotExecute()));

	while(renderJob->GetRunningJobCount() > systemData.numberOfThreads * 5)
	{
	}
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

void cThumbnailWidget::slotRandomRender()
{
	if(cRenderJob::GetRunningJobCount() > systemData.numberOfThreads)
	{
		//if it's to bussy, render later
		timer->start(Random(100000)*10 + 1);
	}
	else
	{
		isRendered = true;
		slotRender();
	}
}
