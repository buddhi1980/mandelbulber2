/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com), Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cRenderQueue class - processes queue render request
 */

#include "render_queue.hpp"

#include <QtCore>

#include "animation_flight.hpp"
#include "animation_frames.hpp"
#include "animation_keyframes.hpp"
#include "error_message.hpp"
#include "file_image.hpp"
#include "files.h"
#include "fractal_container.hpp"
#include "global_data.hpp"
#include "initparameters.hpp"
#include "interface.hpp"
#include "keyframes.hpp"
#include "parameters.hpp"
#include "progress_text.hpp"
#include "queue.hpp"
#include "render_job.hpp"
#include "rendered_image_widget.hpp"
#include "rendering_configuration.hpp"
#include "settings.hpp"

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
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, queuePar);

	/*******************************************************************/
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		queueParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&queueParFractal->at(i));
	}

	queueFlightAnimation = new cFlightAnimation(
		gMainInterface, queueAnimFrames, image, imageWidget, queuePar, queueParFractal, this);
	queueKeyframeAnimation = new cKeyframeAnimation(
		gMainInterface, queueKeyframes, image, imageWidget, queuePar, queueParFractal, this);
	QObject::connect(queueFlightAnimation,
		SIGNAL(updateProgressAndStatus(
			const QString &, const QString &, double, cProgressText::enumProgressType)),
		this, SIGNAL(updateProgressAndStatus(
						const QString &, const QString &, double, cProgressText::enumProgressType)));
	QObject::connect(queueFlightAnimation,
		SIGNAL(updateProgressHide(cProgressText::enumProgressType)), this,
		SIGNAL(updateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(queueFlightAnimation, SIGNAL(updateStatistics(cStatistics)), this,
		SIGNAL(updateStatistics(cStatistics)));
	QObject::connect(queueKeyframeAnimation,
		SIGNAL(updateProgressAndStatus(
			const QString &, const QString &, double, cProgressText::enumProgressType)),
		this, SIGNAL(updateProgressAndStatus(
						const QString &, const QString &, double, cProgressText::enumProgressType)));
	QObject::connect(queueKeyframeAnimation,
		SIGNAL(updateProgressHide(cProgressText::enumProgressType)), this,
		SIGNAL(updateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(queueKeyframeAnimation, SIGNAL(updateStatistics(cStatistics)), this,
		SIGNAL(updateStatistics(cStatistics)));
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

	WriteLog("cRenderQueue::slotRenderQueue()", 2);
	gQueue->stopRequest = false;

	while (!gQueue->stopRequest)
	{
		int queueTotalLeft = gQueue->GetQueueSize();
		cQueue::structQueueItem queueItem = gQueue->GetNextFromList();
		if (queueItem.filename == "") break; // last item reached

		emit updateProgressAndStatus(QFileInfo(queueItem.filename).fileName(),
			QObject::tr("Queue Item %1 of %2").arg(queueFinished + 1).arg(queueTotalLeft + queueFinished),
			(1.0 * queueFinished / (queueTotalLeft + queueFinished)), cProgressText::progress_QUEUE);

		if (QFile::exists(queueItem.filename))
		{
			*queuePar = *gPar;
			cSettings parSettings(cSettings::formatFullText);
			parSettings.LoadFromFile(queueItem.filename);
			parSettings.Decode(queuePar, queueParFractal, queueAnimFrames, queueKeyframes);

			queuePar->Set("image_preview_scale", 0);
			queuePar->Set("limit_CPU_cores", systemData.numberOfThreads);
			queuePar->Set("threads_priority", int(systemData.threadsPriority));

			bool result = false;
			switch (queueItem.renderType)
			{
				case cQueue::queue_STILL: result = RenderStill(queueItem); break;
				case cQueue::queue_FLIGHT: result = RenderFlight(queueItem); break;
				case cQueue::queue_KEYFRAME: result = RenderKeyframe(queueItem); break;
			}

			if (result)
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
		QObject::tr("Queue Render"), QObject::tr("Queue Done"), 1.0, cProgressText::progress_QUEUE);

	emit finished();
}

bool cRenderQueue::RenderFlight(const cQueue::structQueueItem &queueItem) const
{
	bool result;

	QString fullSaveFilename = gPar->Get<QString>("anim_keyframe_dir") + QDir::separator()
														 + QFileInfo(queueItem.filename).baseName() + ".fract";
	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.CreateText(queuePar, queueParFractal);
	parSettings.SaveToFile(fullSaveFilename);

	if (systemData.noGui)
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

bool cRenderQueue::RenderKeyframe(const cQueue::structQueueItem &queueItem) const
{
	bool result;

	QString fullSaveFilename = gPar->Get<QString>("anim_keyframe_dir") + QDir::separator()
														 + QFileInfo(queueItem.filename).baseName() + ".fract";
	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.CreateText(queuePar, queueParFractal);
	parSettings.SaveToFile(fullSaveFilename);

	if (systemData.noGui)
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

bool cRenderQueue::RenderStill(const cQueue::structQueueItem &queueItem)
{
	ImageFileSave::enumImageFileType imageFormat =
		ImageFileSave::enumImageFileType(gPar->Get<int>("queue_image_format"));
	QString extension = ImageFileSave::ImageFileExtension(imageFormat);
	QString saveFilename = QFileInfo(queueItem.filename).baseName() + "." + extension;

	// setup of rendering engine
	cRenderJob *renderJob =
		new cRenderJob(queuePar, queueParFractal, image, &gQueue->stopRequest, imageWidget);

	connect(renderJob, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
		this, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(
		renderJob, SIGNAL(updateStatistics(cStatistics)), this, SIGNAL(updateStatistics(cStatistics)));
	connect(renderJob, SIGNAL(updateImage()), this, SIGNAL(updateImage()));

	cRenderingConfiguration config;
	if (systemData.noGui)
	{
		config.DisableProgressiveRender();
		config.DisableRefresh();
	}
	config.EnableNetRender();
	renderJob->Init(cRenderJob::still, config);

	gQueue->stopRequest = false;

	// render image
	bool result = renderJob->Execute();
	if (!result)
	{
		delete renderJob;
		return false;
	}

	QString fullSaveFilename =
		gPar->Get<QString>("default_image_path") + QDir::separator() + saveFilename;
	SaveImage(fullSaveFilename, imageFormat, image, this);

	fullSaveFilename = gPar->Get<QString>("default_image_path") + QDir::separator()
										 + QFileInfo(queueItem.filename).baseName() + ".fract";
	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.CreateText(queuePar, queueParFractal);
	parSettings.SaveToFile(fullSaveFilename);

	delete renderJob;
	return true;
}
