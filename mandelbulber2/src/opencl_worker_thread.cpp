/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * thread for one of GPUs in multi-gpu rendering
 */

#ifdef USE_OPENCL

#include "opencl_worker_thread.h"

#include <algorithm>
#include <memory>

#include <QDebug>
#include <QVector>

#include "algebra.hpp"
#include "opencl_engine.h"
#include "opencl_hardware.h"
#include "opencl_scheduler.h"
#include "opencl_worker_output_queue.h"
#include "system_data.hpp"
#include "wait.hpp"

using std::min;

cOpenClWorkerThread::cOpenClWorkerThread(
	cOpenClEngine *engine, const std::shared_ptr<cOpenClScheduler> scheduler, int _deviceIndex)
		: QObject(), deviceIndex(_deviceIndex)
{
	hardware = nullptr;
	pixelMask = nullptr;
	this->scheduler = scheduler;
	optimalStepX = 0;
	optimalStepY = 0;
	this->engine = engine;
	imageWidth = 0;
	imageHeight = 0;
	maxMonteCarloSamples = 1;
	stopRequest = nullptr;
	reservedGpuTime = 0.0;
	finishedWithSuccess = false;
	antiAliasingDepth = 0;
	isFullEngine = false;
	maxWorkgroupSize = 0;
}

cOpenClWorkerThread::~cOpenClWorkerThread()
{
	// nothing to destroy
}

bool cOpenClWorkerThread::checkErr(cl_int err, QString functionName)
{
	if (err != CL_SUCCESS)
	{
		qCritical() << "OpenCl ERROR: " << functionName << " (" << err << ")";
		return false;
	}
	else
		return true;
}

void cOpenClWorkerThread::ProcessRenderingLoop()
{
	int startTile = deviceIndex;
	if (startTile >= scheduler->getTileSequence()->length())
	{
		finishedWithSuccess = true;
		emit finished();
		return;
	}

	QElapsedTimer openclProcessingTime;
	qint64 openclprocessingTimeNanoSeconds = 0;

	int actualAADepth = 0;
	int actualAARepeatIndex = 0;

	for (int monteCarloLoop = 1; monteCarloLoop <= maxMonteCarloSamples; monteCarloLoop++)
	{
		scheduler->ReserveTile(startTile, monteCarloLoop);

		for (int tile = startTile; !scheduler->AllDone(monteCarloLoop);
				 tile = scheduler->GetNextTileToRender(tile, monteCarloLoop))
		{
			if (tile < 0) break;
			if (!scheduler->IsTileEnabled(tile)) continue;

			// refresh parameters (needed to update random seed)
			engine->AssignParametersToKernel(deviceIndex);

			// TODO calculation of aa depth and index
			if (isFullEngine)
			{
				AddAntiAliasingParameters(actualAADepth, actualAARepeatIndex);
			}

			quint64 gridX = scheduler->getTileSequence()->at(tile).x();
			quint64 gridY = scheduler->getTileSequence()->at(tile).y();
			quint64 jobX = gridX * optimalStepX;
			quint64 jobY = gridY * optimalStepY;
			quint64 pixelsLeftX = imageWidth - jobX;
			quint64 pixelsLeftY = imageHeight - jobY;
			quint64 jobWidth = min(optimalStepX, pixelsLeftX);
			quint64 jobHeight = min(optimalStepY, pixelsLeftY);
			quint64 sequenceSize = 0;

			if (isFullEngine && jobWidth > 0 && jobHeight > 0)
			{
				sequenceSize = UpdatePixelSequence(jobX, jobY, jobWidth, jobHeight, imageWidth);
			}
			//			qDebug() << "starting tile" << tile << gridX << gridY << jobX << jobY << jobWidth
			//							 << jobHeight;

			if (*stopRequest || systemData.globalStopRequest)
			{
				emit finished();
				finishedWithSuccess = false;
				return;
			}

			if (jobX < imageWidth && jobY < imageHeight)
			{
				openclProcessingTime.restart();
				int result = ProcessClQueue(jobX, jobY, pixelsLeftX, pixelsLeftY, sequenceSize);
				if (!result)
				{
					emit finished();
					finishedWithSuccess = false;
					return;
				}

				result = engine->ReadBuffersFromQueue(deviceIndex);
				if (!result)
				{
					emit finished();
					finishedWithSuccess = false;
					return;
				}

				//				qDebug() << "finished tile" << tile << gridX << gridY << jobX << jobY << jobWidth
				//								 << jobHeight;

				openclprocessingTimeNanoSeconds = openclProcessingTime.nsecsElapsed();

				quint64 outputItemSize = outputBuffers.at(outputIndex).itemSize;
				quint64 outputItemlength = outputBuffers.at(outputIndex).length;
				cOpenCLWorkerOutputQueue::sClDataBuffer dataBuffer(outputItemSize, outputItemlength);

				char *startPtr = outputBuffers.at(outputIndex).ptr.get();
				char *endPtr = startPtr + outputBuffers.at(outputIndex).size();
				dataBuffer.data.assign(startPtr, endPtr);

				cOpenCLWorkerOutputQueue::sClSingleOutput outputDataForQueue;
				outputDataForQueue.jobX = jobX;
				outputDataForQueue.jobY = jobY;
				outputDataForQueue.gridX = gridX;
				outputDataForQueue.gridY = gridY;
				outputDataForQueue.tileIndex = tile;
				outputDataForQueue.jobWidth = jobWidth;
				outputDataForQueue.jobHeight = jobHeight;
				outputDataForQueue.monteCarloLoop = monteCarloLoop;
				outputDataForQueue.aaDepth = actualAADepth;
				outputDataForQueue.outputBuffers.append(dataBuffer);
				outputDataForQueue.pixelSequence = inPixelSequenceBuffer;
				outputDataForQueue.sequenceSize = sequenceSize;

				outputQueue->AddToQueue(&outputDataForQueue);
			}

			// slow down to reduce length of queue
			int queueLength = outputQueue->getQueueLength();
			if (queueLength > 200)
			{
				Wait((queueLength - 200));
			}
		} // next tile

		if (*stopRequest || systemData.globalStopRequest)
		{
			emit finished();
			finishedWithSuccess = false;
			return;
		}

		if (monteCarloLoop == 1) actualAADepth++;

		actualAARepeatIndex++;
		int numberOfAARepeats = int(pow(9.0, double((actualAADepth - 1) / 2))) * 4;
		// qDebug() << numberOfAARepeats;

		if (actualAARepeatIndex >= numberOfAARepeats)
		{
			actualAADepth++;
			actualAARepeatIndex = 0;
		}
	} // next monteCarloLoop

	finishedWithSuccess = true;
	emit finished();
}

bool cOpenClWorkerThread::ProcessClQueue(
	quint64 jobX, quint64 jobY, quint64 pixelsLeftX, quint64 pixelsLeftY, quint64 sequenceSize)
{
	size_t stepSizeX = optimalStepX;
	if (pixelsLeftX < stepSizeX) stepSizeX = pixelsLeftX;
	size_t stepSizeY = optimalStepY;
	if (pixelsLeftY < stepSizeY) stepSizeY = pixelsLeftY;

	cl_int err = 0;

	if (sequenceSize == 0)
	{
		err = clQueue->enqueueNDRangeKernel(
			*clKernel, cl::NDRange(jobX, jobY), cl::NDRange(stepSizeX, stepSizeY), cl::NullRange);
	}
	else
	{
		err = clQueue->enqueueNDRangeKernel(
			*clKernel, cl::NDRange(jobX + jobY * imageWidth), cl::NDRange(sequenceSize), cl::NullRange);
	}
	if (!checkErr(err, "CommandQueue::enqueueNDRangeKernel()"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot enqueue OpenCL rendering jobs"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClWorkerThread::AddAntiAliasingParameters(int actualDepth, int repeatIndex)
{
	CVector2<float> offset;

	if (actualDepth > 0)
	{
		int gridSize = int(pow(3.0, double((actualDepth - 1) / 2)));
		int positionIndex = repeatIndex / 4;

		int xIndex, yIndex;
		float baseOffsetX, baseOffsetY;
		float smallOffsetX = 0;
		float smallOffsetY = 0;

		xIndex = positionIndex % gridSize;
		yIndex = positionIndex / gridSize;

		//		qDebug() << repeatIndex << positionIndex << xIndex << yIndex;

		baseOffsetX = (xIndex - gridSize / 2) / float(gridSize);
		baseOffsetY = (yIndex - gridSize / 2) / float(gridSize);

		float gridOffset = 1.0 / gridSize / 3.0;
		if (actualDepth % 2 == 0)
		{
			switch (repeatIndex % 4)
			{
				case 0:
				{
					smallOffsetX = -gridOffset;
					smallOffsetY = 0;
					break;
				}
				case 1:
				{
					smallOffsetX = gridOffset;
					smallOffsetY = 0;
					break;
				}
				case 2:
				{
					smallOffsetX = 0;
					smallOffsetY = -gridOffset;
					break;
				}
				case 3:
				{
					smallOffsetX = 0;
					smallOffsetY = gridOffset;
					break;
				}
			}
		}
		else
		{
			switch (repeatIndex % 4)
			{
				case 0:
				{
					smallOffsetX = -gridOffset;
					smallOffsetY = -gridOffset;
					break;
				}
				case 1:
				{
					smallOffsetX = gridOffset;
					smallOffsetY = -gridOffset;
					break;
				}
				case 2:
				{
					smallOffsetX = -gridOffset;
					smallOffsetY = gridOffset;
					break;
				}
				case 3:
				{
					smallOffsetX = gridOffset;
					smallOffsetY = gridOffset;
					break;
				}
			}
		}

		offset.x = baseOffsetX + smallOffsetX;
		offset.y = baseOffsetY + smallOffsetY;

		//		qDebug() << "grid" << gridSize << "small" << smallOffsetX << smallOffsetY << "base"
		//						 << baseOffsetX << baseOffsetY << "offset" << offset.x << offset.y;
	}
	else
	{
		offset.x = 0.0f;
		offset.y = 0.0f;
	}

	cl_float2 antiAliasingOffset = {{offset.x, offset.y}};
	cl_int err = clKernel->setArg(7, antiAliasingOffset);
	if (!checkErr(err, "kernel->setArg(7, cl_int(actualDepth))"))
	{
		emit showErrorMessage(tr("Cannot set OpenCL argument for %1").arg(tr("antiAliasingDepth")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

quint64 cOpenClWorkerThread::UpdatePixelSequence(
	quint64 jobX, quint64 jobY, quint64 jobWidth, quint64 jobHeight, qint64 imageWidth)
{
	cl_int err = 0;

	inPixelSequenceBuffer.resize(jobWidth * jobHeight);
	quint64 sequenceSize = 0;

	int subTileSize = sqrt(maxWorkgroupSize);

	if (pixelMask->size() > 0)
	{
		quint64 sequenceIndex = 0;
		for (quint64 y = 0; y < jobHeight; y += subTileSize)
		{
			for (quint64 x = 0; x < jobWidth; x += subTileSize)
			{
				bool havePixel = false;
				for (quint64 yy = y; yy < min(y + subTileSize, jobHeight); yy++)
				{
					for (quint64 xx = x; xx < min(x + subTileSize, jobWidth); xx++)
					{
						if (pixelMask->at((xx + jobX) + (yy + jobY) * imageWidth))
						{
							havePixel = true;
							break;
						}
					}
					if (havePixel) break;
				}

				for (quint64 yy = y; yy < min(y + subTileSize, jobHeight); yy++)
				{
					for (quint64 xx = x; xx < min(x + subTileSize, jobWidth); xx++)
					{
						if (havePixel)
						{
							inPixelSequenceBuffer[sequenceIndex] = xx + yy * jobWidth;
							sequenceIndex++;
						}
					}
				}
			}
		}
		sequenceSize = sequenceIndex;
	}
	else
	{
		quint64 sequenceIndex = 0;
		for (quint64 y = 0; y < jobHeight; y += subTileSize)
		{
			for (quint64 x = 0; x < jobWidth; x += subTileSize)
			{
				for (quint64 yy = y; yy < min(y + subTileSize, jobHeight); yy++)
				{
					for (quint64 xx = x; xx < min(x + subTileSize, jobWidth); xx++)
					{
						inPixelSequenceBuffer[sequenceIndex] = xx + yy * jobWidth;
						sequenceIndex++;
					}
				}
			}
		}
		sequenceSize = sequenceIndex;
	}

	if (sequenceSize == 0)
	{
		sequenceSize = 1;
		inPixelSequenceBuffer[0] = 0;
	}

	inClPixelSequenceBuffer.reset(
		new cl::Buffer(*hardware->getContext(deviceIndex), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sequenceSize * sizeof(cl_int), inPixelSequenceBuffer.data(), &err));

	if (!checkErr(err, "new cl::Buffer(...) for pixel mask"))
	{
		emit showErrorMessage(QObject::tr("OpenCL bufer for pixel mask cannot be created!"),
			cErrorMessage::errorMessage, nullptr);
		qDebug() << "jobWidth" << jobWidth << "jobHeight" << jobHeight << "sequence size"
						 << sequenceSize;
		return 0;
	}

	err = clQueue->enqueueWriteBuffer(*inClPixelSequenceBuffer.get(), CL_TRUE, 0,
		sequenceSize * sizeof(cl_int), inPixelSequenceBuffer.data());
	if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(...) for pixel mask"))
	{
		emit showErrorMessage(QObject::tr("Cannot enqueue writing OpenCL pixel mask"),
			cErrorMessage::errorMessage, nullptr);
		return 0;
	}

	err = clKernel->setArg(8, cl_int(sequenceSize));
	if (!checkErr(err, "kernel->setArg(8, sequenceSize)"))
	{
		emit showErrorMessage(tr("Cannot set OpenCL argument for %1").arg(tr("sequenceSize")),
			cErrorMessage::errorMessage, nullptr);
		return 0;
	}

	err = clKernel->setArg(9, cl_int(jobWidth));
	if (!checkErr(err, "kernel->setArg(8, jobWidth)"))
	{
		emit showErrorMessage(tr("Cannot set OpenCL argument for %1").arg(tr("jobWidth")),
			cErrorMessage::errorMessage, nullptr);
		return 0;
	}

	err = clKernel->setArg(10, *inClPixelSequenceBuffer.get());
	if (!checkErr(err, "kernel->setArg(8, inClPixelSequenceBuffer)"))
	{
		emit showErrorMessage(
			tr("Cannot set OpenCL argument for %1").arg(tr("inClPixelSequenceBuffer")),
			cErrorMessage::errorMessage, nullptr);
		return 0;
	}

	return sequenceSize;
}
#endif // USE_OPENCL
