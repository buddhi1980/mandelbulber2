/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2019 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_SRC_OPENCL_WORKER_THREAD_H_
#define MANDELBULBER2_SRC_OPENCL_WORKER_THREAD_H_

#ifdef USE_OPENCL

#include <QObject>

#include "error_message.hpp"
#include "include_header_wrapper.hpp"
#include "opencl_input_output_buffer.h"

class cOpenCLWorkerOutputQueue;
class cOpenClScheduler;
class cOpenClEngine;

class cOpenClWorkerThread : public QObject
{
	Q_OBJECT

public:
	cOpenClWorkerThread(
		cOpenClEngine *engine, const QSharedPointer<cOpenClScheduler> scheduler, int deviceIndex);
	~cOpenClWorkerThread() override;
	bool ProcessClQueue(quint64 jobX, quint64 jobY, quint64 pixelsLeftX, quint64 pixelsLeftY);

	void setImageHeight(quint64 imageHeight) { this->imageHeight = imageHeight; }
	void setImageWidth(quint64 imageWidth) { this->imageWidth = imageWidth; }
	void setOptimalStepX(quint64 optimalStepX) { this->optimalStepX = optimalStepX; }
	void setOptimalStepY(quint64 optimalStepY) { this->optimalStepY = optimalStepY; }
	void setClKernel(const QSharedPointer<cl::Kernel> &clKernel) { this->clKernel = clKernel; }
	void setClQueue(const QSharedPointer<cl::CommandQueue> &clQueue) { this->clQueue = clQueue; }
	void setInputAndOutputBuffers(const QList<sClInputOutputBuffer> &inputAndOutputBuffers)
	{
		this->inputAndOutputBuffers = inputAndOutputBuffers;
	}
	void setOutputBuffers(const QList<sClInputOutputBuffer> &outputBuffers)
	{
		this->outputBuffers = outputBuffers;
	}
	void setOutputQueue(const QSharedPointer<cOpenCLWorkerOutputQueue> &outputQueue)
	{
		this->outputQueue = outputQueue;
	}
	void setMaxMonteCarloSamples(int maxMonteCarloSamples)
	{
		this->maxMonteCarloSamples = maxMonteCarloSamples;
	}
	void setStopRequest(bool *stopRequest) { this->stopRequest = stopRequest; }
	void setReservedGpuTime(double reservedGpuTime) { this->reservedGpuTime = reservedGpuTime; }
	bool wasFishedWithSuccess() { return finishedWithSuccess; }

private:
	static bool checkErr(cl_int err, QString functionName);

	QSharedPointer<cl::Kernel> clKernel;
	QSharedPointer<cl::CommandQueue> clQueue;

	QSharedPointer<cOpenCLWorkerOutputQueue> outputQueue;
	QSharedPointer<cOpenClScheduler> scheduler;
	QList<sClInputOutputBuffer> outputBuffers;
	QList<sClInputOutputBuffer> inputAndOutputBuffers;
	bool *stopRequest;

	const int outputIndex = 0;

	cOpenClEngine *engine;

	quint64 optimalStepX;
	quint64 optimalStepY;
	quint64 imageWidth;
	quint64 imageHeight;
	double reservedGpuTime;
	int maxMonteCarloSamples;
	bool finishedWithSuccess;

	const int deviceIndex;

public slots:
	void ProcessRenderingLoop();

signals:
	void showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *);
	void finished();
};

#endif // USE_OPENCL

#endif /* MANDELBULBER2_SRC_OPENCL_WORKER_THREAD_H_ */
