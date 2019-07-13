/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Robert Pancoast (RobertPancoast77@gmail.com)
 *
 * These objects enable an OpenCL backend definition.
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_H_

#include <utility>

#include <QtCore>

#include "error_message.hpp"
#include "include_header_wrapper.hpp"
#include "opencl_input_output_buffer.h"

class cOpenClHardware;
class cParameterContainer;

class cOpenClEngine : public QObject
{
	Q_OBJECT

	struct sOptimalJob
	{
		sOptimalJob() {}

		quint64 workGroupSize{0};
		quint64 workGroupSizeOptimalMultiplier{0};
		quint64 stepSize{0};
		quint64 stepSizeX{0};
		quint64 stepSizeY{0};
		quint64 workGroupSizeMultiplier{1};
		quint64 jobSizeMultiplier{1};
		QElapsedTimer timer;
		double lastProcessingTime{1.0};
		quint64 sizeOfPixel{0};
		quint64 jobSizeLimit{0};
		double optimalProcessingCycle{0.1};
	};

public:
	cOpenClEngine(cOpenClHardware *hardware);
	~cOpenClEngine() override;

#ifdef USE_OPENCL
	void Lock();
	void Unlock();
	static void DeleteKernelCache();
	void Reset();
	virtual bool LoadSourcesAndCompile(const cParameterContainer *params) = 0;
	bool CreateKernel4Program(const cParameterContainer *params);
	virtual bool PreAllocateBuffers(const cParameterContainer *params);
	virtual void RegisterInputOutputBuffers(const cParameterContainer *params) = 0;
	bool WriteBuffersToQueue();
	bool ReadBuffersFromQueue(int deviceIndex);
	bool CreateCommandQueue();
	void SetUseBuildCache(bool useCache) { useBuildCache = useCache; }
	void SetUseFastRelaxedMath(bool usefastMath) { useFastRelaxedMath = usefastMath; }
	void ReleaseMemory();
	bool AssignParametersToKernel(int deviceIndex);
	virtual bool AssignParametersToKernelAdditional(uint argIterator, int deviceIndex)
	{
		Q_UNUSED(argIterator);
		Q_UNUSED(deviceIndex);
		return true;
	}

protected:
	virtual QString GetKernelName() = 0;
	static bool checkErr(cl_int err, QString functionName);
	bool Build(const QByteArray &programString, QString *errorText);
	bool CreateKernels();
	void InitOptimalJob(const cParameterContainer *params);
	void UpdateOptimalJobStart(quint64 pixelsLeft);
	void UpdateOptimalJobEnd();
	virtual size_t CalcNeededMemory() = 0;

	typedef QList<sClInputOutputBuffer> listOfBuffers;
	QList<listOfBuffers> inputBuffers;
	QList<listOfBuffers> outputBuffers;					// separate output buffer for each OpenCL device
	QList<listOfBuffers> inputAndOutputBuffers; // separate input/output buffer for each OpenCL device

	QList<QSharedPointer<cl::Program>> clPrograms;
	QList<QSharedPointer<cl::Kernel>> clKernels;
	QList<QSharedPointer<cl::CommandQueue>> clQueues;

	sOptimalJob optimalJob;
	bool programsLoaded;
	bool readyForRendering;
	bool kernelCreated;

	QString definesCollector;

#endif

	cOpenClHardware *hardware;

private:
	QMutex lock;
	bool locked;
	bool useBuildCache;
	bool useFastRelaxedMath;
	QByteArray lastProgramHash;
	QByteArray lastBuildParametersHash;

signals:
	void showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *);
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_H_ */
