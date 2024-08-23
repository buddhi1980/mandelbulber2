/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cThumbnailWidget - promoted QWidget for displaying of fractals in an auto-rendering thumbnail
 *
 * This class holds an cImage and fractal settings can be assigned with AssignParameters().
 * The class then asynchronously renders the fractal as a thumbnail and displays it.
 * The fractal thumbnails can also be cached in filesystem for faster loading.
 * Signals for progress and render finish can be connected, see also usage in PreviewFileDialog.
 */

#include "thumbnail_widget.h"

#include <QImage>
#include <QPaintEvent>

#include "src/cimage.hpp"
#include "src/common_math.h"
#include "src/files.h"
#include "src/global_data.hpp"
#include "src/interface.hpp"
#include "src/opencl_engine_render_fractal.h"
#include "src/render_job.hpp"
#include "src/rendering_configuration.hpp"
#include "src/settings.hpp"
#include "src/stereo.h"
#include "src/system_data.hpp"
#include "src/system_directories.hpp"
#include "src/wait.hpp"

cThumbnailWidget::cThumbnailWidget(QWidget *parent) : QWidget(parent)
{
	Init(parent);
}

cThumbnailWidget::cThumbnailWidget(int _width, int _height, double _oversample, QWidget *parent)
		: QWidget(parent)
{
	Init(parent);
	SetSize(_width, _height, _oversample);
}

void cThumbnailWidget::Init(QWidget *parent)
{
	Q_UNUSED(parent);

	tWidth = 0;
	tHeight = 0;
	oversample = 0;
	progressBar = nullptr;
	stopRequest = false;
	isRendered = false;
	isFullyRendered = false;
	hasParameters = false;
	disableTimer = false;
	disableThumbnailCache = false;
	connect(this, SIGNAL(renderRequest()), this, SLOT(slotRender()));
	params.reset(new cParameterContainer);
	fractal.reset(new cFractalContainer);
	useOneCPUCore = false;
	useForrandomizer = false;

	timer = new QTimer(this);
	timer->setSingleShot(true);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotRandomRender()));

	lastRenderTime = 0.0;

	instanceIndex = instanceCount;
	instanceCount++;
	// qDebug() << "cThumbnailWidget constructed" << instanceCount;
}

void cThumbnailWidget::SetSize(int _width, int _height, double _oversample)
{
	tWidth = _width;
	tHeight = _height;
	oversample = _oversample;
	image.reset(new cImage(tWidth * oversample, tHeight * oversample, true));
	image->CreatePreview(1.0 / oversample, tWidth, tWidth, this);
	setFixedWidth(tWidth);
	setFixedHeight(tHeight);
}

cThumbnailWidget::~cThumbnailWidget()
{
	stopRequest = true;
	if (image)
	{
		// qDebug() << "cThumbnailWidget trying to delete" << instanceIndex;
		while (image->IsUsed())
		{
			stopRequest = true;
		}
		image.reset();
		// qDebug() << "cThumbnailWidget image deleted" << instanceIndex;
	}

	instanceCount--;
	// qDebug() << "cThumbnailWidget destructed" << instanceIndex;
}

void cThumbnailWidget::paintEvent(QPaintEvent *event)
{
	if (image)
	{
		event->accept();
		if (hasParameters)
		{
			if (!isRendered)
			{
				isRendered = true;
				timer->stop();
				if (!disableRenderOnPaint)
				{
					emit renderRequest();
				}
			}
		}
		image->RedrawInWidget(this);
	}
}

void cThumbnailWidget::AssignParameters(std::shared_ptr<const cParameterContainer> _params,
	std::shared_ptr<const cFractalContainer> _fractal)
{
	isFullyRendered = false;
	// qDebug() << "AssignParameters";
	if (image)
	{
		params.reset(new cParameterContainer);
		fractal.reset(new cFractalContainer);
		*params = *_params;
		*fractal = *_fractal;
		params->Set("detail_level", std::min(params->Get<double>("image_width") / tWidth / oversample
																					 * params->Get<double>("detail_level"),
																	4.0));
		params->Set("image_height", tHeight * oversample);
		params->Set("stereo_mode", int(cStereo::stereoRedCyan));
		params->Set("antialiasing_enabled", false);
		if (params->Get<bool>("DOF_monte_carlo"))
		{
			params->Set("MC_denoiser_enable", true);
			params->Set("MC_denoiser_strength", 1);
			params->Set("MC_denoiser_preserve_geometry", true);
			params->Set("DOF_max_noise", params->Get<double>("DOF_max_noise") * 10.0);
			params->Set("DOF_min_samples", 5);
		}

		if (fractal->isUsedCustomFormula())
		{
			params->Set("opencl_mode", int(cOpenClEngineRenderFractal::clRenderEngineTypeFull));
			params->Set("opencl_enabled", true);
		}

		else if (params->Get<bool>("opencl_enabled"))
		{
			if (params->Get<int>("opencl_mode") > 0)
			{
				double distance =
					cInterface::GetDistanceForPoint(params->Get<CVector3>("camera"), params, fractal);
				if (distance < 1e-5 && !useForrandomizer)
				{
					params->Set("opencl_mode", 0);
				}

				if (distance < 1e-5 && useForrandomizer)
				{
					isRendered = true;
					isFullyRendered = true;
					params.reset();
					fractal.reset();
					// alloc image in case if samething wnat read it
					image->ChangeSize(tWidth * oversample, tHeight * oversample, sImageOptional());
					image->ClearImage();
					emit signalZeroDistance();
					emit signalFinished();
					return;
				}
			}
		}

		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(params, fractal);
		oldHash = hash;
		hash = tempSettings.GetHashCode();

		if (hash != oldHash)
		{
			stopRequest = true;
			while (image->IsUsed())
			{
				// just wait and pray
			}

			emit settingsChanged();

			isRendered = false;
			hasParameters = true;

			QString thumbnailFileName = GetThumbnailFileName();
			if (QFileInfo::exists(thumbnailFileName) && !disableThumbnailCache)
			{
				fileTouch(thumbnailFileName);
				stopRequest = true;
				isRendered = true;
				isFullyRendered = true;
				while (image->IsUsed())
				{
					// just wait and pray
				}

				double dpiScale = devicePixelRatioF();

				QPixmap pixmap;
				pixmap.load(thumbnailFileName);
				pixmap = pixmap.scaled(
					tWidth * dpiScale, tHeight * dpiScale, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				QImage qImage = pixmap.toImage();
				qImage = qImage.convertToFormat(QImage::Format_RGB888);

				std::vector<sRGB8> &preview = image->GetPreviewPrimary();
				std::vector<sRGB8> &preview2 = image->GetPreview();

				int bWidth = qImage.width();
				int bHeight = qImage.height();

				if (!qImage.isNull())
				{
					for (int y = 0; y < bHeight; y++)
					{
						sRGB8 *line = reinterpret_cast<sRGB8 *>(qImage.scanLine(y));
						for (int x = 0; x < bWidth; x++)
						{
							sRGB8 pixel(quint8(line[x].R), quint8(line[x].G), quint8(line[x].B));
							preview[x + y * bWidth] = pixel;
							preview2[x + y * bWidth] = pixel;
						}
					}
				}

				params.reset();
				fractal.reset();
				emit thumbnailRendered();
				emit signalFinished();
			}
			else
			{
				if (!disableTimer)
				{
					// render thumbnail after random time. It forces rendering of widgets when they are not
					// visible. It makes rendering of widgets when they are idle.

					timer->start(Random(3600000) + 30000); // one hour for all thumbnails
				}
			}
		}
	}
	else
	{
		qCritical() << "Image not yet allocated!";
	}
}

void cThumbnailWidget::slotRender()
{
	// qDebug() << "slotRender";
	if (image && params)
	{
		stopRequest = true;
		while (image->IsUsed())
		{
			// just wait and pray
			Wait(100);
		}

		// random wait to not generate to many events at exactly the same time
		Wait(Random(100) + 50);

		if (!disableTimer
				&& (cRenderJob::GetRunningJobCount() > systemData.numberOfThreads * 2
						|| (params->Get<bool>("opencl_enabled") && cRenderJob::IfOpenCLBusy())))
		{
			// try again after some random time
			if (visibleRegion().isEmpty())
			{
				timer->start(Random(600000) + 1);
			}
			else
			{
				timer->start(Random(1000) + 1);
			}
			return;
		}

		stopRequest = false;

		cRenderJob *renderJob =
			new cRenderJob(params, fractal, image, &stopRequest, static_cast<QWidget *>(this));
		connect(renderJob, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
			this, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
		connect(renderJob, SIGNAL(updateImage()), this, SLOT(update()));
		connect(renderJob, &cRenderJob::signalTotalRenderTime, this,
			&cThumbnailWidget::signalTotalRenderTime);

		renderingTimeTimer.start();
		renderJob->UseSizeFromImage(true);

		cRenderingConfiguration config;
		if (useOneCPUCore) config.DisableMultiThread();
		config.EnableIgnoreErrors();
		config.DisableNetRender();

		renderJob->Init(cRenderJob::still, config);

		QThread *thread = new QThread;
		renderJob->moveToThread(thread);
		QObject::connect(thread, SIGNAL(started()), renderJob, SLOT(slotExecute()));

		thread->setObjectName("ThumbnailWorker");
		thread->start();

		QObject::connect(renderJob, SIGNAL(finished()), renderJob, SLOT(deleteLater()));
		QObject::connect(renderJob, SIGNAL(finished()), thread, SLOT(quit()));
		QObject::connect(renderJob, SIGNAL(fullyRendered(const QString &, const QString &)), this,
			SLOT(slotFullyRendered()));
		QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
		connect(thread, &QThread::finished, this, &cThumbnailWidget::signalFinished);
	}
	else
	{
		qCritical() << "Parameters or image not yet allocated!";
	}
}

void cThumbnailWidget::slotFullyRendered()
{
	isRendered = true;
	if (!disableThumbnailCache)
	{
		QImage qImage(static_cast<const uchar *>(image->ConvertTo8bitChar()), int(image->GetWidth()),
			int(image->GetHeight()), int(image->GetWidth() * sizeof(sRGB8)), QImage::Format_RGB888);
		QPixmap pixmap;
		pixmap.convertFromImage(qImage);

		QString thumbnailFileName = GetThumbnailFileName();
		pixmap.save(thumbnailFileName, "PNG");
	}
	lastRenderTime = renderingTimeTimer.nsecsElapsed() / 1e9;
	params.reset();
	fractal.reset();
	emit thumbnailRendered();
	isFullyRendered = true;
	// qDebug() << "fullyRendered";
}

void cThumbnailWidget::slotRandomRender()
{
	if (cRenderJob::GetRunningJobCount() > systemData.numberOfThreads * 2)
	{
		// if it's to busy, render later
		timer->start(Random(1000) * 10 + 1);
	}
	else
	{
		if (!disableTimer)
		{
			isRendered = true;
			slotRender();
		}
	}
}

void cThumbnailWidget::slotSetMinimumSize(int width, int height)
{
	setMinimumSize(width, height);
}

QString cThumbnailWidget::GetThumbnailFileName() const
{
	return systemDirectories.GetThumbnailsFolder() + QDir::separator() + hash + QString(".png");
}

int cThumbnailWidget::instanceCount = 0;
