/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRenderQueue class - processes queue render request
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
 * Authors: Sebastian Jennen, Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "render_queue.hpp"

#include <QtCore>

#include "progress_text.hpp"
#include "global_data.hpp"
#include "settings.hpp"
#include "error_message.hpp"
#include "initparameters.hpp"

cRenderQueue::cRenderQueue(cImage *_image, RenderedImage *widget) : QObject()
{
	image = _image;
	imageWidget = widget;
	queuePar = new cParameterContainer;
	queueParFractal = new cFractalContainer;
	queueAnimFrames = new cAnimationFrames;
	queueKeyframes = new cKeyframes;

	queuePar->SetContainerName("main");
	InitParams(queuePar);
	for(int i=0; i<NUMBER_OF_FRACTALS; i++)
	{
		queueParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&queueParFractal->at(i));
	}

	queueFlightAnimation = new cFlightAnimation(gMainInterface, queueAnimFrames, image, imageWidget, queuePar, queueParFractal, this);
	queueKeyframeAnimation = new cKeyframeAnimation(gMainInterface, queueKeyframes, image, imageWidget, queuePar, queueParFractal, this);
	QObject::connect(queueFlightAnimation, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double, cProgressText::enumProgressType)),
									 this, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double, cProgressText::enumProgressType)));
	QObject::connect(queueFlightAnimation, SIGNAL(updateProgressHide(cProgressText::enumProgressType)),
									 this, SIGNAL(updateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(queueFlightAnimation, SIGNAL(updateStatistics(cStatistics)),
									 this, SIGNAL(updateStatistics(cStatistics)));
	QObject::connect(queueKeyframeAnimation, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double, cProgressText::enumProgressType)),
									 this, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double, cProgressText::enumProgressType)));
	QObject::connect(queueKeyframeAnimation, SIGNAL(updateProgressHide(cProgressText::enumProgressType)),
									 this, SIGNAL(updateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(queueKeyframeAnimation, SIGNAL(updateStatistics(cStatistics)),
									 this, SIGNAL(updateStatistics(cStatistics)));
}

cRenderQueue::~cRenderQueue()
{
	delete queueAnimFrames;
	delete queueKeyframes;
	delete queueFlightAnimation;
	delete queueKeyframeAnimation;
	delete queueParFractal;
	delete queuePar;
}

void cRenderQueue::slotRenderQueue()
{
	int queueFinished = 0;

	WriteLog("cRenderQueue::slotRenderQueue()");
	gQueue->stopRequest = false;

	while(!gQueue->stopRequest)
	{
		int queueTotalLeft = gQueue->GetQueueSize();
		cQueue::structQueueItem queueItem = gQueue->GetNextFromList();
		if(queueItem.filename == "") break; // last item reached

		emit updateProgressAndStatus(
			QFileInfo(queueItem.filename).fileName(),
			QObject::tr("Queue Item %1 of %2").arg(queueFinished + 1).arg(queueTotalLeft + queueFinished),
			(1.0 * queueFinished / (queueTotalLeft + queueFinished)),
			cProgressText::progress_QUEUE);

		if(QFile::exists(queueItem.filename))
		{
			cSettings parSettings(cSettings::formatFullText);
			parSettings.LoadFromFile(queueItem.filename);
			parSettings.Decode(queuePar, queueParFractal, queueAnimFrames, queueKeyframes);

			queuePar->Set("image_preview_scale", 0);

			bool result = false;
			switch(queueItem.renderType)
			{
				case cQueue::queue_STILL: result = RenderStill(queueItem.filename); break;
				case cQueue::queue_FLIGHT:
				{
					result = RenderFlight();
				}
				break;
				case cQueue::queue_KEYFRAME:
				{
					result = RenderKeyframe();
				}
				break;
			}

			if(result)
			{
				gQueue->RemoveQueueItem(queueItem);
				queueFinished++;
			}
			else
			{
				break;
			}
		}
		else
		{
			cErrorMessage::showMessage("Cannot load file!\n", cErrorMessage::errorMessage);
			qCritical() << "\nSetting file " << queueItem.filename << " not found\n";
		}
	}

	emit updateProgressAndStatus(
		QObject::tr("Queue Render"),
		QObject::tr("Queue Done"),
		1.0,
		cProgressText::progress_QUEUE);

	emit finished();
}

bool cRenderQueue::RenderFlight()
{
	bool result = false;
	if(systemData.noGui)
	{
		// gMainInterface->headless->RenderFlightAnimation();
		result = queueFlightAnimation->RenderFlight(&gQueue->stopRequest);
	}
	else
	{
		result = queueFlightAnimation->RenderFlight(&gQueue->stopRequest);
	}
	return result;
}

bool cRenderQueue::RenderKeyframe()
{
	bool result = false;
	if(systemData.noGui)
	{
		// gMainInterface->headless->RenderKeyframeAnimation();
		result = queueKeyframeAnimation->RenderKeyframes(&gQueue->stopRequest);
	}
	else
	{
		result = queueKeyframeAnimation->RenderKeyframes(&gQueue->stopRequest);
	}
	return result;
}

bool cRenderQueue::RenderStill(const QString& filename)
{
	QString extension;
	enumImageFileType imageFormat = (enumImageFileType) gPar->Get<int>("queue_image_format");
	switch(imageFormat)
	{
		case IMAGE_FILE_TYPE_JPG:
			extension += QString(".jpg");
		break;
		case IMAGE_FILE_TYPE_PNG:
			extension += QString(".png");
		break;
		case IMAGE_FILE_TYPE_EXR:
			extension += QString(".exr");
		break;
	}

	QString saveFilename = QFileInfo(filename).baseName() + extension;

	//setup of rendering engine
	cRenderJob *renderJob = new cRenderJob(queuePar, queueParFractal, image, &gQueue->stopRequest, imageWidget);

	connect(renderJob, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)),
					this, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)));
	connect(renderJob, SIGNAL(updateStatistics(cStatistics)), this, SIGNAL(updateStatistics(cStatistics)));

	cRenderingConfiguration config;
	if(systemData.noGui)
	{
		config.DisableProgressiveRender();
		config.DisableRefresh();
	}
	config.EnableNetRender();
	renderJob->Init(cRenderJob::still, config);

	gQueue->stopRequest = false;

	//render image
	bool result = renderJob->Execute();
	if(!result)
	{
		delete renderJob;
		return false;
	}

	QString fullSaveFilename = gPar->Get<QString>("default_image_path") + QDir::separator() + saveFilename;
	SaveImage(fullSaveFilename, imageFormat, image);

	fullSaveFilename = gPar->Get<QString>("default_image_path") + QDir::separator() + QFileInfo(filename).baseName() + ".fract";
	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.CreateText(queuePar, queueParFractal);
	parSettings.SaveToFile(fullSaveFilename);

	delete renderJob;
	return true;
}
