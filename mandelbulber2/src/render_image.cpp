/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <QtCore>

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
#include "system.hpp"

cRenderer::cRenderer(const sParamRender *_params, const cNineFractals *_fractal,
	sRenderData *_renderData, cImage *_image)
		: QObject()
{
	params = _params;
	fractal = _fractal;
	data = _renderData;
	image = _image;
	scheduler = nullptr;
	netRenderAckReceived = true;
}

cRenderer::~cRenderer()
{
	if (scheduler) delete scheduler;
}

bool cRenderer::RenderImage()
{
	WriteLog("cRenderer::RenderImage()", 2);

	if (image->IsAllocated())
	{
		image->SetImageParameters(params->imageAdjustments);

		int progressiveSteps;
		if (data->configuration.UseProgressive())
			progressiveSteps =
				int(log(double(max(image->GetWidth(), image->GetHeight()))) / log(2.0)) - 3;
		else
			progressiveSteps = 0;

		if (progressiveSteps < 0) progressiveSteps = 0;
		int progressive = pow(2.0, double(progressiveSteps) - 1);
		if (progressive == 0) progressive = 1;

		// prepare multiple threads
		QThread **thread = new QThread *[data->configuration.GetNumberOfThreads()];
		cRenderWorker::sThreadData *threadData =
			new cRenderWorker::sThreadData[data->configuration.GetNumberOfThreads()];
		cRenderWorker **worker = new cRenderWorker *[data->configuration.GetNumberOfThreads()];

		if (scheduler) delete scheduler;
		scheduler = new cScheduler(data->screenRegion, progressive);

		cProgressText progressText;
		progressText.ResetTimer();

		for (int i = 0; i < data->configuration.GetNumberOfThreads(); i++)
		{
			threadData[i].id = i + 1;
			if (data->configuration.UseNetRender())
			{
				if (i < data->netRenderStartingPositions.size())
				{
					threadData[i].startLine = data->netRenderStartingPositions.at(i);
				}
				else
				{
					threadData[i].startLine = data->screenRegion.y1;
					qCritical() << "NetRender - Missing starting positions data";
				}
			}
			else
			{
				threadData[i].startLine =
					(data->screenRegion.height / data->configuration.GetNumberOfThreads() * i
						+ data->screenRegion.y1)
					/ scheduler->GetProgressiveStep() * scheduler->GetProgressiveStep();
			}
			threadData[i].scheduler = scheduler;
		}

		QString statusText;
		QString progressTxt;

		QElapsedTimer timerRefresh;
		timerRefresh.start();
		qint64 lastRefreshTime = 0;
		QList<int> listToRefresh;
		QList<int> listToSend;

		QElapsedTimer timerProgressRefresh;
		timerProgressRefresh.start();

		WriteLog("Start rendering", 2);
		do
		{
			WriteLogDouble("Progressive loop", scheduler->GetProgressiveStep(), 2);
			for (int i = 0; i < data->configuration.GetNumberOfThreads(); i++)
			{
				WriteLog(QString("Thread ") + QString::number(i) + " create", 3);
				thread[i] = new QThread;
				worker[i] = new cRenderWorker(
					params, fractal, &threadData[i], data, image); // Warning! not needed to delete object
				worker[i]->moveToThread(thread[i]);
				QObject::connect(thread[i], SIGNAL(started()), worker[i], SLOT(doWork()));
				QObject::connect(worker[i], SIGNAL(finished()), thread[i], SLOT(quit()));
				QObject::connect(worker[i], SIGNAL(finished()), worker[i], SLOT(deleteLater()));
				thread[i]->setObjectName("RenderWorker #" + QString::number(i));
				thread[i]->start();
				thread[i]->setPriority(GetQThreadPriority(systemData.threadsPriority));
				WriteLog(QString("Thread ") + QString::number(i) + " started", 3);
			}

			while (!scheduler->AllLinesDone())
			{
				gApplication->processEvents();

				if (*data->stopRequest || progressText.getTime() > data->configuration.GetMaxRenderTime()
						|| systemData.globalStopRequest)
				{
					scheduler->Stop();
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
				double percentDone = scheduler->PercentDone();
				data->lastPercentage = percentDone;
				statusText = QObject::tr("Rendering image");
				progressTxt = progressText.getText(percentDone);
				data->statistics.time = progressText.getTime();

				if (timerProgressRefresh.elapsed() > 1000)
				{
					emit updateProgressAndStatus(statusText, progressTxt, percentDone);
					emit updateStatistics(data->statistics);
					timerProgressRefresh.restart();
				}

				// refresh image
				if (listToRefresh.size() > 0)
				{
					if (timerRefresh.elapsed() > lastRefreshTime
							&& (scheduler->GetProgressivePass() > 1 || !data->configuration.UseProgressive()))
					{
						timerRefresh.restart();

						emit updateProgressAndStatus(statusText, progressTxt, percentDone);
						emit updateStatistics(data->statistics);

						QSet<int> set_listToRefresh = listToRefresh.toSet(); // removing duplicates
						listToRefresh = set_listToRefresh.toList();
						qSort(listToRefresh);
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

						image->ConvertTo8bit();

						if (data->configuration.UseImageRefresh())
						{
							image->UpdatePreview(&listToRefresh);
							emit updateImage();
						}

						// sending rendered lines to NetRender server
						if (data->configuration.UseNetRender() && gNetRender->IsClient()
								&& gNetRender->GetStatus() == CNetRender::netRender_WORKING)
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
									emit sendRenderedLines(listToSend, renderedLinesData);
									emit NotifyClientStatus();
									netRenderAckReceived = false;
									listToSend.clear();
								}
							}
						}

						if (data->configuration.UseNetRender() && gNetRender->IsServer())
						{
							QList<int> toDoList = scheduler->CreateDoneList();
							if (toDoList.size() > data->configuration.GetNumberOfThreads())
							{
								for (int c = 0; c < gNetRender->GetClientCount(); c++)
								{
									emit SendToDoList(c, toDoList);
								}
							}
						}

						lastRefreshTime = timerRefresh.elapsed() * data->configuration.GetRefreshRate()
															/ (listToRefresh.size());

						if (lastRefreshTime < 100)
							lastRefreshTime = 100; // the shortest refresh time can be 100ms

						// do not refresh and send data too often
						if (data->configuration.UseNetRender())
						{
							if (lastRefreshTime < 500) lastRefreshTime = 500;
						}

						timerRefresh.restart();
						listToRefresh.clear();
					} // timerRefresh
				}		// isPreview
			}			// while scheduler

			for (int i = 0; i < data->configuration.GetNumberOfThreads(); i++)
			{
				while (thread[i]->isRunning())
				{
					gApplication->processEvents();
				};
				WriteLog(QString("Thread ") + QString::number(i) + " finished", 2);
				delete thread[i];
			}
		} while (scheduler->ProgressiveNextStep());

		// send last rendered lines
		if (data->configuration.UseNetRender() && gNetRender->IsClient()
				&& gNetRender->GetStatus() == CNetRender::netRender_WORKING)
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
					emit sendRenderedLines(listToSend, renderedLinesData);
					emit NotifyClientStatus();
					netRenderAckReceived = false;
					listToSend.clear();
				}
			}
		}

		if (data->configuration.UseNetRender())
		{
			if (gNetRender->IsServer())
			{
				emit StopAllClients();
			}
		}

		image->NullPostEffect();

		if (!(gNetRender->IsClient() && data->configuration.UseNetRender()))
		{
			if (params->ambientOcclusionEnabled
					&& params->ambientOcclusionMode == params::AOModeScreenSpace)
			{
				cRenderSSAO rendererSSAO(params, data, image);
				connect(&rendererSSAO,
					SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
					SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
				connect(&rendererSSAO, SIGNAL(updateImage()), this, SIGNAL(updateImage()));

				if (data->stereo.isEnabled() && (data->stereo.GetMode() == cStereo::stereoLeftRight
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
			if (params->DOFEnabled && !*data->stopRequest && !params->DOFMonteCarlo)
			{
				cPostRenderingDOF dof(image);
				connect(&dof, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
					this, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
				connect(&dof, SIGNAL(updateImage()), this, SIGNAL(updateImage()));

				if (data->stereo.isEnabled() && (data->stereo.GetMode() == cStereo::stereoLeftRight
																					|| data->stereo.GetMode() == cStereo::stereoTopBottom))
				{
					cRegion<int> region;
					region = data->stereo.GetRegion(
						CVector2<int>(image->GetWidth(), image->GetHeight()), cStereo::eyeLeft);
					dof.Render(region, params->DOFRadius * (image->GetWidth() + image->GetHeight()) / 2000.0,
						params->DOFFocus, params->DOFNumberOfPasses, params->DOFBlurOpacity,
						params->DOFMaxRadius, data->stopRequest);
					region = data->stereo.GetRegion(
						CVector2<int>(image->GetWidth(), image->GetHeight()), cStereo::eyeRight);
					dof.Render(region, params->DOFRadius * (image->GetWidth() + image->GetHeight()) / 2000.0,
						params->DOFFocus, params->DOFNumberOfPasses, params->DOFBlurOpacity,
						params->DOFMaxRadius, data->stopRequest);
				}
				else
				{
					dof.Render(data->screenRegion,
						params->DOFRadius * (image->GetWidth() + image->GetHeight()) / 2000.0, params->DOFFocus,
						params->DOFNumberOfPasses, params->DOFBlurOpacity, params->DOFMaxRadius,
						data->stopRequest);
				}
			}

			if (params->hdrBlurEnabled)
			{
				cPostEffectHdrBlur *hdrBlur = new cPostEffectHdrBlur(image);
				hdrBlur->SetParameters(params->hdrBlurRadius, params->hdrBlurIntensity);
				connect(hdrBlur, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
					this, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
				hdrBlur->Render(data->stopRequest);
				delete hdrBlur;
			}
		}

		// refresh image at end
		WriteLog("image->CompileImage()", 2);
		image->CompileImage();

		if (image->IsPreview())
		{
			WriteLog("image->ConvertTo8bit()", 2);
			image->ConvertTo8bit();
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

		if (data->configuration.UseNetRender())
		{
			if (gNetRender->IsClient())
			{
				gNetRender->SetStatus(CNetRender::netRender_READY);
				emit NotifyClientStatus();
			}
		}

		delete[] thread;
		delete[] threadData;
		delete[] worker;

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
	if (y >= 0 && y < image->GetHeight())
	{
		int width = image->GetWidth();
		sAllImageData *lineOfImage = new sAllImageData[width];
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
		}
		lineData->append(reinterpret_cast<char *>(lineOfImage), CastSizeToInt(dataSize));
		delete[] lineOfImage;
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
		if (y >= 0 && y < image->GetHeight())
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
