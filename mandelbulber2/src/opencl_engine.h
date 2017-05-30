/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

#include <QtCore>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4005) // macro redefinition
#pragma warning(disable : 4996) // declared deprecated
#endif													// _MSC_VER

#ifdef USE_OPENCL
#ifdef _WIN32
#ifndef _MSC_VER
// clew for cross compile
#include "clew.h"
#include "clew-cl.hpp"
#endif // NOT _MSC_VER
#endif // _WIN32
// OpenCL SDK for all others
#include <CL/cl.hpp>
#endif // USE_OPENCL

#if defined(_MSC_VER)
#pragma warning(pop)
#endif // _MSC_VER

class cOpenClHardware;
class cParameterContainer;

class cOpenClEngine : public QObject
{
	Q_OBJECT

	struct sOptimalJob
	{
		sOptimalJob()
				: numberOfSteps(0),
					workGroupSize(0),
					pixelsPerJob(0),
					stepSize(0),
					workGroupSizeMultiplier(1),
					lastProcessingTime(1.0),
					sizeOfPixel(0),
					jobSizeLimit(0),
					optimalProcessingCycle(0.1)
		{
		}
		size_t numberOfSteps;
		size_t workGroupSize;
		size_t pixelsPerJob;
		size_t stepSize;
		size_t workGroupSizeMultiplier;
		QElapsedTimer timer;
		double lastProcessingTime;
		size_t sizeOfPixel;
		size_t jobSizeLimit;
		double optimalProcessingCycle;
	};

public:
	cOpenClEngine(cOpenClHardware *hardware);
	~cOpenClEngine();

#ifdef USE_OPENCL

	void Lock();
	void Unlock();
	virtual void LoadSourcesAndCompile(const cParameterContainer *params) = 0;
	bool CreateKernel4Program(const cParameterContainer *params);
	virtual bool PreAllocateBuffers(const cParameterContainer *params) = 0;
	bool CreateCommandQueue();

protected:
	static bool checkErr(cl_int err, QString fuctionName);
	bool Build(const QByteArray &programString, QString *errorText);
	bool CreateKernel(cl::Program *prog);
	void InitOptimalJob(const cParameterContainer *params);
	void UpdateOptimalJobStart(int pixelsLeft);
	void UpdateOptimalJobEnd();

	cl::Program *program;
	cl::Kernel *kernel;
	cl::CommandQueue *queue;

	sOptimalJob optimalJob;
	bool programsLoaded;
	bool readyForRendering;

	QString definesCollector;

#endif

	cOpenClHardware *hardware;

private:
	QMutex lock;
	bool locked;
	QByteArray lastProgramHash;
	QByteArray lastBuldParametersHash;
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_H_ */
