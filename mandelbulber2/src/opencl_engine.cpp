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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 *  Created on: 3 maj 2017
 *      Author: krzysztof
 */

#include "opencl_engine.h"

#include <sstream>
#include <iostream>
#include "system.hpp"

#include "opencl_hardware.h"
#include "parameters.hpp"
#include "error_message.hpp"

cOpenClEngine::cOpenClEngine(cOpenClHardware *_hardware) : QObject(_hardware), hardware(_hardware)
{
#ifdef USE_OPENCL
	program = nullptr;
	kernel = nullptr;
	queue = nullptr;
	programsLoaded = false;
	readyForRendering = false;
	kernelCreated = false;
	locked = false;
#endif

	connect(this, SIGNAL(showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *)),
		gErrorMessage, SLOT(slotShowMessage(QString, cErrorMessage::enumMessageType, QWidget *)));
}

cOpenClEngine::~cOpenClEngine()
{
#ifdef USE_OPENCL
	if (program) delete program;
	if (kernel) delete kernel;
	if (queue) delete queue;
#endif
}

#ifdef USE_OPENCL

bool cOpenClEngine::checkErr(cl_int err, QString fuctionName)
{
	if (err != CL_SUCCESS)
	{
		qCritical() << "OpenCl ERROR: " << fuctionName << " (" << err << ")";
		return false;
	}
	else
		return true;
}

bool cOpenClEngine::Build(const QByteArray &programString, QString *errorText)
{
	if (hardware->getClDevices().size() > 0)
	{
		// calculating hash code of the program
		QCryptographicHash hashCryptProgram(QCryptographicHash::Md4);
		hashCryptProgram.addData(programString);
		QByteArray hashProgram = hashCryptProgram.result();

		// calculating hash code of build parameters
		QCryptographicHash hashCryptBuildParams(QCryptographicHash::Md4);
		hashCryptBuildParams.addData(definesCollector.toLocal8Bit());
		QByteArray hashBuildParams = hashCryptBuildParams.result();

		// if program is different than in previous run
		if (!(hashProgram == lastProgramHash && hashBuildParams == lastBuldParametersHash))
		{
			lastBuldParametersHash = hashBuildParams;
			lastProgramHash = hashProgram;

			// collecting all parts of program
			cl::Program::Sources sources;
			sources.push_back(std::make_pair(programString.constData(), size_t(programString.length())));

			// creating cl::Program
			cl_int err;

			if (program) delete program;
			program = new cl::Program(*hardware->getContext(), sources, &err);

			if (checkErr(err, "cl::Program()"))
			{

				std::string buildParams =
					"-w -cl-single-precision-constant -cl-denorms-are-zero -DOPENCL_KERNEL_CODE";
				buildParams += definesCollector.toUtf8().constData();
				qDebug() << "Build parameters: " << buildParams.c_str();
				err = program->build(hardware->getClDevices(), buildParams.c_str());

				if (checkErr(err, "program->build()"))
				{
					qDebug() << "OpenCl kernel program successfully compiled";
					return true;
				}
				else
				{
					std::stringstream errorMessageStream;
					errorMessageStream << "OpenCL Build log:\n"
														 << program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(
																	hardware->getEnabledDevices())
														 << std::endl;

					std::string buildLogText = errorMessageStream.str();

					*errorText = QString::fromStdString(errorMessageStream.str());

					std::cerr << buildLogText;

					emit showErrorMessage(
						QObject::tr("Error during compilation of OpenCL program\n") + errorText->left(500),
						cErrorMessage::errorMessage, nullptr);

					lastBuldParametersHash.clear();
					lastProgramHash.clear();
					return false;
				}
			}
			else
			{
				cErrorMessage::showMessage(
					QObject::tr("OpenCL program cannot be created!"), cErrorMessage::errorMessage);
				return false;
			}
		}
		else
		{
			qDebug() << "Re-compile is not needed";
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool cOpenClEngine::CreateKernel4Program(const cParameterContainer *params)
{
	if (programsLoaded)
	{
		if (CreateKernel(program))
		{
			InitOptimalJob(params);
			return true;
		}
	}
	return false;
}

bool cOpenClEngine::CreateKernel(cl::Program *prog)
{
	cl_int err;
	if (kernel) delete kernel;
	kernel = new cl::Kernel(*prog, "fractal3D", &err);
	if (checkErr(err, "cl::Kernel()"))
	{
		size_t workGroupSize = 0;
		kernel->getWorkGroupInfo(
			hardware->getEnabledDevices(), CL_KERNEL_WORK_GROUP_SIZE, &workGroupSize);
		qDebug() << "CL_KERNEL_WORK_GROUP_SIZE" << workGroupSize;

		optimalJob.workGroupSize = workGroupSize;
		kernelCreated = true;
		return true;
	}
	else
	{
		emit showErrorMessage(
			QObject::tr("Cannot create OpenCL kernel"), cErrorMessage::errorMessage, nullptr);
		kernelCreated = false;
	}
	return false;
}

void cOpenClEngine::InitOptimalJob(const cParameterContainer *params)
{
	size_t width = params->Get<int>("image_width");
	size_t height = params->Get<int>("image_height");
	size_t memoryLimitByUser = params->Get<int>("gpu_memory_limit") * 1024 * 1024;
	size_t pixelCnt = width * height;
	cOpenClDevice::sDeviceInformation deviceInfo = hardware->getSelectedDeviceInformation();

	optimalJob.pixelsPerJob = optimalJob.workGroupSize * deviceInfo.maxComputeUnits;
	optimalJob.numberOfSteps = pixelCnt / optimalJob.pixelsPerJob + 1;
	optimalJob.stepSize =
		(pixelCnt / optimalJob.numberOfSteps / optimalJob.pixelsPerJob + 1) * optimalJob.pixelsPerJob;
	optimalJob.workGroupSizeMultiplier = 1;
	optimalJob.lastProcessingTime = 1.0;

	size_t maxAllocMemSize = deviceInfo.maxMemAllocSize;
	size_t memSize = memoryLimitByUser;
	if (maxAllocMemSize > 0 && maxAllocMemSize * 0.75 < memoryLimitByUser)
	{
		memSize = maxAllocMemSize * 0.75;
	}
	optimalJob.jobSizeLimit = memSize / optimalJob.sizeOfPixel;

	qDebug() << "pixelsPerJob:" << optimalJob.pixelsPerJob;
	qDebug() << "numberOfSteps:" << optimalJob.numberOfSteps;
	qDebug() << "stepSize:" << optimalJob.stepSize;
}

bool cOpenClEngine::CreateCommandQueue()
{
	if (hardware->ContextCreated())
	{
		cl_int err;
		if (queue) delete queue;
		// TODO: support multiple devices
		// TODO: create a separate queue per device
		// iterate through getDevicesInformation[s]
		queue = new cl::CommandQueue(*hardware->getContext(), hardware->getEnabledDevices(), 0, &err);

		if (checkErr(err, "CommandQueue::CommandQueue()"))
		{
			readyForRendering = true;
			return true;
		}
		else
		{
			emit showErrorMessage(
				QObject::tr("Cannot create OpenCL command queue"), cErrorMessage::errorMessage, nullptr);
			readyForRendering = false;
			return false;
		}
	}
	return false;
}

void cOpenClEngine::UpdateOptimalJobStart(int pixelsLeft)
{
	optimalJob.timer.restart();
	optimalJob.timer.start();
	double processingCycleTime = optimalJob.optimalProcessingCycle;

	optimalJob.workGroupSizeMultiplier *= processingCycleTime / optimalJob.lastProcessingTime;

	size_t maxWorkGroupSizeMultiplier = pixelsLeft / optimalJob.workGroupSize;

	if (optimalJob.workGroupSizeMultiplier > maxWorkGroupSizeMultiplier)
		optimalJob.workGroupSizeMultiplier = maxWorkGroupSizeMultiplier;

	if (optimalJob.workGroupSizeMultiplier * optimalJob.workGroupSize > optimalJob.jobSizeLimit)
		optimalJob.workGroupSizeMultiplier = optimalJob.jobSizeLimit / optimalJob.workGroupSize;

	if (optimalJob.workGroupSizeMultiplier < hardware->getSelectedDeviceInformation().maxComputeUnits)
		optimalJob.workGroupSizeMultiplier = hardware->getSelectedDeviceInformation().maxComputeUnits;

	optimalJob.stepSize = optimalJob.workGroupSizeMultiplier * optimalJob.workGroupSize;

	//	qDebug() << "lastProcessingTime" << optimalJob.lastProcessingTime;
	//	qDebug() << "stepSize:" << optimalJob.stepSize;
}

void cOpenClEngine::Reset()
{
	lastBuldParametersHash.clear();
	lastProgramHash.clear();
}

void cOpenClEngine::UpdateOptimalJobEnd()
{
	optimalJob.lastProcessingTime = optimalJob.timer.elapsed() / 1000.0;
}

void cOpenClEngine::Lock()
{
	lock.lock();
	locked = true;
}

void cOpenClEngine::Unlock()
{
	if (locked)
	{
		lock.unlock();
	}
	locked = false;
}

#endif
