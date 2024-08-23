/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cRenderer class - calculates image using multiple CPU cores and does post processing
 */

#include "render_image.hpp"

#include <algorithm>
#include <memory>

#include "ao_modes.h"
#include "cast.hpp"
#include "dof.hpp"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "netrender.hpp"
#include "post_effect_hdr_blur.h"
#include "progress_text.hpp"
#include "render_data.hpp"
#include "render_ssao.h"
#include "render_worker.hpp"
#include "scheduler.hpp"
#include "stereo.h"
#include "system_data.hpp"
#include "wait.hpp"
#include "write_log.hpp"

cRenderer::cRenderer(std::shared_ptr<const sParamRender> _params,
	std::shared_ptr<const cNineFractals> _fractal, std::shared_ptr<sRenderData> _renderData,
	std::shared_ptr<cImage> _image)
		: QObject()
{
	params = _params;
	fractal = _fractal;
	data = _renderData;
	image = _image;
	netRenderAckReceived = true;
}

cRenderer::~cRenderer()
{
	// nothing to delete
}

int cRenderer::InitProgresiveSteps()
{
	int progressiveSteps;
	if (data->configuration.UseProgressive())
		progressiveSteps = int(log(double(max(image->GetWidth(), image->GetHeight()))) / log(2.0)) - 2;
	else
		progressiveSteps = 0;

	if (progressiveSteps < 0) progressiveSteps = 0;
	if (progressiveSteps > 5) progressiveSteps = 5;

	int progressive = int(pow(2.0, double(progressiveSteps) - 1));
	if (progressive == 0) progressive = 1;

	return progressive;
}

void cRenderer::InitializeThreadData(
	std::vector<std::shared_ptr<cRenderWorker::sThreadData>> &threadData)
{
	for (uint i = 0; i < threadData.size(); i++)
	{
		threadData[i].reset(new cRenderWorker::sThreadData());
		threadData[i]->id = i + 1;
		if (data->configuration.UseNetRender() && !gNetRender->IsAnimation())
		{
			if (i < data->netRenderStartingPositions.size())
			{
				threadData[i]->startLine = data->netRenderStartingPositions.at(i);
			}
			else
			{
				threadData[i]->startLine = data->screenRegion.y1;
				qCritical() << "NetRender - Missing starting positions data";
			}
		}
		else
		{
			threadData[i]->startLine =
				(data->screenRegion.height / data->configuration.GetNumberOfThreads() * i
					+ data->screenRegion.y1)
				/ scheduler->GetProgressiveStep() * scheduler->GetProgressiveStep();
		}
		threadData[i]->scheduler = scheduler;
	}
}

void cRenderer::LaunchThreads(std::vector<QThread *> &threads,
	std::vector<cRenderWorker *> &workers,
	std::vector<std::shared_ptr<cRenderWorker::sThreadData>> &threadsData)
{
	for (int i = 0; i < data->configuration.GetNumberOfThreads(); i++)
	{
		WriteLog(QString("Thread ") + QString::number(i) + " create", 3);
		threads[i] = new QThread;
		workers[i] = new cRenderWorker(
			params, fractal, threadsData[i], data, image); // Warning! not needed to delete object
		workers[i]->moveToThread(threads[i]);
		QObject::connect(threads[i], SIGNAL(started()), workers[i], SLOT(doWork()));
		QObject::connect(workers[i], SIGNAL(finished()), threads[i], SLOT(quit()));
		QObject::connect(workers[i], SIGNAL(finished()), workers[i], SLOT(deleteLater()));
		threads[i]->setObjectName("RenderWorker #" + QString::number(i));
		threads[i]->start();
		threads[i]->setPriority(systemData.GetQThreadPriority(systemData.threadsPriority));
		WriteLog(QString("Thread ") + QString::number(i) + " started", 3);
	}
}

void cRenderer::TerminateRendering()
{
	scheduler->Stop();
	image->CompileImage();
	image->ConvertTo8bitChar();
	if (data->configuration.UseImageRefresh())
	{
		image->SetFastPreview(true);
		image->UpdatePreview();
		updateImage();
	}
}

double cRenderer::PeriodicUpdateStatusAndProgressBar(QString &statusText, QString &progressTxt,
	cProgressText &progressText, QElapsedTimer &timerProgressRefresh)
{
	// status bar and progress bar
	double percentDone = scheduler->PercentDone();
	data->lastPercentage = percentDone;
	statusText = QObject::tr("Rendering image");
	progressTxt = progressText.getText(percentDone);
	data->statistics.time = progressText.getTime();
	if (timerProgressRefresh.elapsed() > 1000)
	{
		updateProgressAndStatus(statusText, progressTxt, percentDone);
		updateStatistics(data->statistics);
		timerProgressRefresh.restart();
	}
	return percentDone;
}

QSet<int> cRenderer::UpdateImageDuringRendering(QList<int> &listToRefresh, QList<int> &listToSend)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
	QSet<int> set_listToRefresh = listToRefresh.toSet(); // removing duplicates
#else
	QSet<int> set_listToRefresh(listToRefresh.begin(), listToRefresh.end());
#endif
	listToRefresh = set_listToRefresh.values();
	std::sort(listToRefresh.begin(), listToRefresh.end());
	listToSend += listToRefresh;
	image->NullPostEffect(&listToRefresh);
	if (data->configuration.UseRenderTimeEffects())
	{
		if (params->ambientOcclusionEnabled
				&& params->ambientOcclusionMode == params::AOModeScreenSpace)
		{
			cRenderSSAO rendererSSAO(params, data, image);
			rendererSSAO.setProgressive(scheduler->GetProgressiveStep());
			rendererSSAO.RenderSSAO(&listToRefresh);
		}
	}
	image->CompileImage(&listToRefresh);
	image->ConvertTo8bitChar();
	if (data->configuration.UseImageRefresh())
	{
		image->SetFastPreview(true);
		image->UpdatePreview(&listToRefresh);
		updateImage();
	}
	return set_listToRefresh;
}

void cRenderer::SendRenderedLinesToNetRender(QList<int> &listToSend)
{
	// sending rendered lines to NetRender server
	if (data->configuration.UseNetRender() && gNetRender->IsClient()
			&& gNetRender->GetStatus() == netRenderSts_WORKING && !gNetRender->IsAnimation())
	{
		// If ACK was already received, then server is ready to take new data.
		if (netRenderAckReceived)
		{
			QList<QByteArray> renderedLinesData;
			for (int i = 0; i < listToSend.size(); i++)
			{
				// avoid sending already rendered lines
				if (scheduler->IsLineDoneByServer(listToSend.at(i)))
				{
					listToSend.removeAt(i);
					i--;
					continue;
				}
				// creating data set to send
				QByteArray lineData;
				CreateLineData(listToSend.at(i), &lineData);
				renderedLinesData.append(lineData);
			}
			// sending data
			if (listToSend.size() > 0)
			{
				sendRenderedLines(listToSend, renderedLinesData);
				NotifyClientStatus();
				netRenderAckReceived = false;
				listToSend.clear();
			}
		}
	}
}

void cRenderer::UpdateNetRenderToDoList()
{
	if (data->configuration.UseNetRender() && gNetRender->IsServer() && !gNetRender->IsAnimation())
	{
		QList<int> toDoList = scheduler->CreateDoneList();
		if (toDoList.size() > data->configuration.GetNumberOfThreads())
		{
			for (int c = 0; c < gNetRender->GetClientCount(); c++)
			{
				SendToDoList(c, toDoList);
			}
		}
	}
}

void cRenderer::SendRenderedLinesToNetRenderAfterRendering(QList<int> listToSend)
{
	// send last rendered lines
	if (data->configuration.UseNetRender() && gNetRender->IsClient()
			&& gNetRender->GetStatus() == netRenderSts_WORKING && !gNetRender->IsAnimation())
	{
		if (netRenderAckReceived)
		{
			QList<QByteArray> renderedLinesData;
			for (int i = 0; i < listToSend.size(); i++)
			{
				// avoid sending already rendered lines
				if (scheduler->IsLineDoneByServer(listToSend.at(i)))
				{
					listToSend.removeAt(i);
					i--;
					continue;
				}
				QByteArray lineData;
				CreateLineData(listToSend.at(i), &lineData);
				renderedLinesData.append(lineData);
			}
			if (listToSend.size() > 0)
			{
				sendRenderedLines(listToSend, renderedLinesData);
				NotifyClientStatus();
				netRenderAckReceived = false;
				listToSend.clear();
			}
		}
	}
}

void cRenderer::RenderSSAO()
{
	cRenderSSAO rendererSSAO(params, data, image);
	connect(&rendererSSAO, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
		this, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(&rendererSSAO, SIGNAL(updateImage()), this, SIGNAL(updateImage()));
	if (data->stereo.isEnabled()
			&& (data->stereo.GetMode() == cStereo::stereoLeftRight
					|| data->stereo.GetMode() == cStereo::stereoTopBottom))
	{
		cRegion<int> region;
		region = data->stereo.GetRegion(
			CVector2<int>(image->GetWidth(), image->GetHeight()), cStereo::eyeLeft);
		rendererSSAO.SetRegion(region);
		rendererSSAO.RenderSSAO();
		region = data->stereo.GetRegion(
			CVector2<int>(image->GetWidth(), image->GetHeight()), cStereo::eyeRight);
		rendererSSAO.SetRegion(region);
		rendererSSAO.RenderSSAO();
	}
	else
	{
		rendererSSAO.RenderSSAO();
	}
}

void cRenderer::RenderDOF()
{
	cPostRenderingDOF dof(image);
	connect(&dof, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(&dof, SIGNAL(updateImage()), this, SIGNAL(updateImage()));
	if (data->stereo.isEnabled()
			&& (data->stereo.GetMode() == cStereo::stereoLeftRight
					|| data->stereo.GetMode() == cStereo::stereoTopBottom))
	{
		cRegion<int> region;
		region = data->stereo.GetRegion(
			CVector2<int>(image->GetWidth(), image->GetHeight()), cStereo::eyeLeft);
		dof.Render(region, params->DOFRadius * (region.width + region.height) / 2000.0,
			params->DOFFocus, params->DOFNumberOfPasses, params->DOFBlurOpacity, params->DOFMaxRadius,
			data->stopRequest);
		region = data->stereo.GetRegion(
			CVector2<int>(image->GetWidth(), image->GetHeight()), cStereo::eyeRight);
		dof.Render(region, params->DOFRadius * (region.width + region.height) / 2000.0,
			params->DOFFocus, params->DOFNumberOfPasses, params->DOFBlurOpacity, params->DOFMaxRadius,
			data->stopRequest);
	}
	else
	{
		dof.Render(data->screenRegion,
			params->DOFRadius * (image->GetWidth() + image->GetHeight()) / 2000.0, params->DOFFocus,
			params->DOFNumberOfPasses, params->DOFBlurOpacity, params->DOFMaxRadius, data->stopRequest);
	}
}

void cRenderer::RenderHDRBlur()
{
	std::unique_ptr<cPostEffectHdrBlur> hdrBlur(new cPostEffectHdrBlur(image));
	hdrBlur->SetParameters(params->hdrBlurRadius, params->hdrBlurIntensity);
	connect(hdrBlur.get(), SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
		this, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	hdrBlur->Render(data->stopRequest);
}

bool cRenderer::RenderImage()
{
	WriteLog("cRenderer::RenderImage()", 2);

	if (image->IsAllocated())
	{
		image->SetImageParameters(params->imageAdjustments);

		image->SetFastPreview(true);

		int progressive = InitProgresiveSteps();

		cProgressText progressText;
		progressText.ResetTimer();

		// prepare multiple threads
		std::vector<QThread *> threads(data->configuration.GetNumberOfThreads());
		std::vector<std::shared_ptr<cRenderWorker::sThreadData>> threadsData(
			data->configuration.GetNumberOfThreads());
		std::vector<cRenderWorker *> workers(data->configuration.GetNumberOfThreads());

		scheduler.reset(new cScheduler(data->screenRegion, progressive));

		InitializeThreadData(threadsData);

		QString statusText;
		QString progressTxt;

		QElapsedTimer timerRefresh;
		timerRefresh.start();
		qint64 lastRefreshTime = 10;
		QList<int> listToRefresh;
		QList<int> listToSend;

		QElapsedTimer timerProgressRefresh;
		timerProgressRefresh.start();

		QElapsedTimer pureRenderingTime;
		pureRenderingTime.start();

		WriteLog("Start rendering", 2);
		do
		{
			WriteLogDouble("Progressive loop", scheduler->GetProgressiveStep(), 2);

			LaunchThreads(threads, workers, threadsData);

			while (!scheduler->AllLinesDone())
			{
				gApplication->processEvents();

				if (*data->stopRequest || progressText.getTime() > data->configuration.GetMaxRenderTime()
						|| systemData.globalStopRequest)
				{
					TerminateRendering();
				}

				Wait(10); // wait 10ms

				if (data->configuration.UseRefreshRenderedList())
				{
					// get list of last rendered lines
					QList<int> list = scheduler->GetLastRenderedLines();
					// create list of lines for image refresh
					listToRefresh += list;
				}

				// status bar and progress bar
				double percentDone = PeriodicUpdateStatusAndProgressBar(
					statusText, progressTxt, progressText, timerProgressRefresh);

				int firstPassToRefresh = (params->ambientOcclusionEnabled
																	 && params->ambientOcclusionMode == params::AOModeScreenSpace)
																	 ? 1
																	 : 0;

				// refresh image
				if (listToRefresh.size() > 0)
				{
					if (timerRefresh.elapsed() > lastRefreshTime
							&& (scheduler->GetProgressivePass() > firstPassToRefresh
									|| !data->configuration.UseProgressive()))
					{
						timerRefresh.restart();

						emit updateProgressAndStatus(statusText, progressTxt, percentDone);
						emit updateStatistics(data->statistics);

						QSet<int> set_listToRefresh = UpdateImageDuringRendering(listToRefresh, listToSend);

						// sending rendered lines to NetRender server
						SendRenderedLinesToNetRender(listToSend);

						UpdateNetRenderToDoList();

						lastRefreshTime = timerRefresh.elapsed() * data->configuration.GetRefreshRate()
															/ (listToRefresh.size());

						if (data->configuration.UseNetRender())
						{
							if (lastRefreshTime > 1000) lastRefreshTime = 1000;
						}

						if (lastRefreshTime < 100)
							lastRefreshTime = 100; // the shortest refresh time can be 100ms

						// do not refresh and send data too often
						if (data->configuration.UseNetRender())
						{
							if (lastRefreshTime < 500) lastRefreshTime = 500;
						}

						if (!*data->stopRequest)
							emit signalSmallPartRendered(pureRenderingTime.elapsed() / 1000.0);

						timerRefresh.restart();
						listToRefresh.clear();
					} // timerRefresh
				}		// isPreview
			}			// while scheduler

			for (int i = 0; i < data->configuration.GetNumberOfThreads(); i++)
			{
				while (threads[i]->isRunning())
				{
					gApplication->processEvents();
				};
				WriteLog(QString("Thread ") + QString::number(i) + " finished", 2);
				delete threads[i];
			}

		} while (scheduler->ProgressiveNextStep());

		// send last rendered lines
		SendRenderedLinesToNetRenderAfterRendering(listToSend);

		if (data->configuration.UseNetRender())
		{
			if (gNetRender->IsServer() && !gNetRender->IsAnimation())
			{
				emit StopAllClients();
			}
		}

		image->NullPostEffect();

		// post efects
		if (!((gNetRender->IsClient() && !gNetRender->IsAnimation())
					&& data->configuration.UseNetRender()))
		{
			if (params->ambientOcclusionEnabled
					&& params->ambientOcclusionMode == params::AOModeScreenSpace)
			{
				RenderSSAO();
			}
			if (params->DOFEnabled && !*data->stopRequest && !params->DOFMonteCarlo
					&& !systemData.globalStopRequest)
			{
				RenderDOF();
			}

			if (params->hdrBlurEnabled)
			{
				RenderHDRBlur();
			}
		}

		// refresh image at end
		WriteLog("image->CompileImage()", 2);
		image->CompileImage();

		if (image->IsPreview())
		{
			image->SetFastPreview(*data->stopRequest || data->configuration.GetMaxRenderTime() < 1e49);

			WriteLog("image->ConvertTo8bit()", 2);
			image->ConvertTo8bitChar();
			WriteLog("image->UpdatePreview()", 2);
			image->UpdatePreview();
			WriteLog("image->GetImageWidget()->update()", 2);
			emit updateImage();
		}

		WriteLog("Rendering finished", 2);

		// status bar and progress bar
		double percentDone = 1.0;
		statusText = QObject::tr("Idle");
		progressTxt = progressText.getText(percentDone);

		// update histograms
		data->statistics.time = progressText.getTime();
		emit updateStatistics(data->statistics);
		emit updateProgressAndStatus(statusText, progressTxt, percentDone);
		emit signalTotalRenderTime(progressText.getTime());

		if (data->configuration.UseNetRender())
		{
			if (gNetRender->IsClient() && !gNetRender->IsAnimation())
			{
				gNetRender->SetStatus(netRenderSts_READY);
				emit NotifyClientStatus();
			}
		}

		WriteLog("cRenderer::RenderImage(): memory released", 2);

		if (*data->stopRequest || systemData.globalStopRequest)
			return false;
		else
			return true;
	}
	else
	{
		qCritical() << "Image not allocated!";
		return false;
	}
}

void cRenderer::CreateLineData(int y, QByteArray *lineData) const
{
	if (y >= 0 && y < int(image->GetHeight()))
	{
		int width = image->GetWidth();
		std::vector<sAllImageData> lineOfImage(width);
		size_t dataSize = sizeof(sAllImageData) * width;
		for (int x = 0; x < width; x++)
		{
			lineOfImage[x].imageFloat = image->GetPixelImage(x, y);
			lineOfImage[x].alphaBuffer = image->GetPixelAlpha(x, y);
			lineOfImage[x].colourBuffer = image->GetPixelColor(x, y);
			lineOfImage[x].zBuffer = image->GetPixelZBuffer(x, y);
			lineOfImage[x].opacityBuffer = image->GetPixelOpacity(x, y);
			if (image->GetImageOptional()->optionalNormal)
				lineOfImage[x].normalFloat = image->GetPixelNormal(x, y);
			if (image->GetImageOptional()->optionalNormalWorld)
				lineOfImage[x].normalFloat = image->GetPixelNormalWorld(x, y);
			if (image->GetImageOptional()->optionalSpecular)
				lineOfImage[x].normalSpecular = image->GetPixelSpecular(x, y);
			if (image->GetImageOptional()->optionalWorld)
				lineOfImage[x].worldPosition = image->GetPixelWorld(x, y);
			if (image->GetImageOptional()->optionalShadows)
				lineOfImage[x].shadows = image->GetPixelShadows(x, y);
			if (image->GetImageOptional()->optionalGlobalIlluination)
				lineOfImage[x].globalIllumination = image->GetPixelGlobalIllumination(x, y);
		}
		lineData->append(reinterpret_cast<char *>(lineOfImage.data()), CastSizeToInt(dataSize));
	}
	else
	{
		qCritical() << "cRenderer::CreateLineData(int y, QByteArray *lineData): wrong line:" << y;
	}
}

void cRenderer::NewLinesArrived(QList<int> lineNumbers, QList<QByteArray> lines) const
{
	for (int i = 0; i < lineNumbers.size(); i++)
	{
		int y = lineNumbers.at(i);
		if (y >= 0 && y < int(image->GetHeight()))
		{
			sAllImageData *lineOfImage = (sAllImageData *)lines.at(i).data();
			int width = image->GetWidth();
			for (int x = 0; x < width; x++)
			{
				image->PutPixelImage(x, y, lineOfImage[x].imageFloat);
				image->PutPixelAlpha(x, y, lineOfImage[x].alphaBuffer);
				image->PutPixelColor(x, y, lineOfImage[x].colourBuffer);
				image->PutPixelZBuffer(x, y, lineOfImage[x].zBuffer);
				image->PutPixelOpacity(x, y, lineOfImage[x].opacityBuffer);
				if (image->GetImageOptional()->optionalNormal)
					image->PutPixelNormal(x, y, lineOfImage[x].normalFloat);
				if (image->GetImageOptional()->optionalNormalWorld)
					image->PutPixelNormalWorld(x, y, lineOfImage[x].normalFloatWorld);
				if (image->GetImageOptional()->optionalSpecular)
					image->PutPixelSpecular(x, y, lineOfImage[x].normalSpecular);
				if (image->GetImageOptional()->optionalWorld)
					image->PutPixelWorld(x, y, lineOfImage[x].worldPosition);
				if (image->GetImageOptional()->optionalDiffuse)
					image->PutPixelDiffuse(x, y,
						sRGBFloat(lineOfImage[x].colourBuffer.R / 255.0, lineOfImage[x].colourBuffer.G / 255.0,
							lineOfImage[x].colourBuffer.B / 255.0));
				if (image->GetImageOptional()->optionalShadows)
					image->PutPixelShadows(x, y, lineOfImage[x].shadows);
				if (image->GetImageOptional()->optionalGlobalIlluination)
					image->PutPixelGlobalIllumination(x, y, lineOfImage[x].globalIllumination);
			}
		}
		else
		{
			qCritical() << "cRenderer::NewLinesArrived(QList<int> lineNumbers, QList<QByteArray> lines): "
										 "wrong line number:"
									<< y;
			return;
		}
	}

	scheduler->MarkReceivedLines(lineNumbers);
}

void cRenderer::ToDoListArrived(QList<int> toDo) const
{
	scheduler->UpdateDoneLines(toDo);
}

void cRenderer::AckReceived()
{
	netRenderAckReceived = true;
}
