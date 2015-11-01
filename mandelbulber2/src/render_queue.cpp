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
	queueFlightAnimation = new cFlightAnimation(gMainInterface, queueAnimFrames, image, imageWidget, queuePar, queueParFractal, this);
	queueKeyframeAnimation = new cKeyframeAnimation(gMainInterface, queueKeyframes, image, this);
	queuePar->SetContainerName("main");
	InitParams(queuePar);
	for(int i=0; i<NUMBER_OF_FRACTALS; i++)
	{
		queueParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&queueParFractal->at(i));
	}
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
		int queueTotalLeft = gQueue->GetListFromQueueFile().size(); //FIXME this is not thread safe!

		emit updateProgressAndStatus(
			QObject::tr("Queue Render"),
			QObject::tr("Queue Item %1 of %2").arg(queueFinished + 1).arg(queueTotalLeft + queueFinished),
			1.0 * (queueFinished / (queueTotalLeft + queueFinished)),
			cProgressText::progress_QUEUE);
		cQueue::structQueueItem queueItem = gQueue->GetNextFromList(); //FIXME this is not thread safe!
		if(queueItem.filename == "") break; // last item reached


		if(QFile::exists(queueItem.filename))
		{
			cSettings parSettings(cSettings::formatFullText);
			parSettings.LoadFromFile(queueItem.filename);
			parSettings.Decode(queuePar, queueParFractal, queueAnimFrames, queueKeyframes);
			if(!systemData.noGui)
			{
				emit updateUI();
			}

			queuePar->Set("image_preview_scale", 0);

			bool result = false;
			switch(queueItem.renderType)
			{
				case cQueue::queue_STILL: result = RenderStill(queueItem.filename); break;
				case cQueue::queue_FLIGHT: RenderFlight(); break;
				case cQueue::queue_KEYFRAME: RenderKeyframe(); break;
			}

			if(result)
			{
				gQueue->RemoveFromList(queueItem); //FIXME this is not thread safe!
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

void cRenderQueue::RenderFlight()
{
	if(systemData.noGui)
	{
		gMainInterface->headless->RenderFlightAnimation();
	}
	else
	{
		queueFlightAnimation->RenderFlight(&gQueue->stopRequest);
	}
}

void cRenderQueue::RenderKeyframe()
{
	if(systemData.noGui)
	{
		gMainInterface->headless->RenderKeyframeAnimation();
	}
	else
	{
		gKeyframeAnimation->RenderKeyframes();
	}
}

bool cRenderQueue::RenderStill(const QString& filename)
{
	QString saveFilename = QFileInfo(filename).baseName() + ".png";

	//setup of rendering engine
	cRenderJob *renderJob = new cRenderJob(queuePar, queueParFractal, image, &gQueue->stopRequest, imageWidget);
	if(gMainInterface->mainWindow)
	{
		connect(renderJob, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)), gMainInterface->mainWindow, SLOT(slotUpdateProgressAndStatus(const QString&, const QString&, double)));
		connect(renderJob, SIGNAL(updateStatistics(cStatistics)), gMainInterface->mainWindow, SLOT(slotUpdateStatistics(cStatistics)));
	}
	if(gMainInterface->headless)
	{
		connect(renderJob, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)), gMainInterface->headless, SLOT(slotUpdateProgressAndStatus(const QString&, const QString&, double)));
		connect(renderJob, SIGNAL(updateStatistics(cStatistics)), gMainInterface->headless, SLOT(slotUpdateStatistics(cStatistics)));
	}

	cRenderingConfiguration config;

	renderJob->Init(cRenderJob::still, config);

	if(imageWidget != NULL)
		imageWidget->setMinimumSize(image->GetPreviewWidth(), image->GetPreviewHeight());

	gQueue->stopRequest = false;

	//render image
	bool result = renderJob->Execute();
	if(!result) return false;

	QString fullSaveFilename = gPar->Get<QString>("default_image_path") + QDir::separator() + saveFilename;
	SaveImage(fullSaveFilename, IMAGE_FILE_TYPE_PNG, image);

	delete renderJob;
	return true;
}

void cRenderQueue::slotUpdateProgressAndStatus(const QString &text, const QString &progressText, double progress, cProgressText::enumProgressType progressType)
{
	emit updateProgressAndStatus(text, progressText, progress, progressType);
}
