/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 *  Created on: 3 maj 2017
 *      Author: krzysztof
 */

#include "opencl_engine.h"

#include <iostream>
#include <sstream>

#include "error_message.hpp"
#include "opencl_hardware.h"
#include "parameters.hpp"
#include "system.hpp"

cOpenClEngine::cOpenClEngine(cOpenClHardware *_hardware) : QObject(_hardware), hardware(_hardware)
{
#ifdef USE_OPENCL
	programsLoaded = false;
	readyForRendering = false;
	kernelCreated = false;
	locked = false;
	useBuildCache = true;
	useFastRelaxedMath = false;
#endif

	connect(this, SIGNAL(showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *)),
		gErrorMessage, SLOT(slotShowMessage(QString, cErrorMessage::enumMessageType, QWidget *)));
}

cOpenClEngine::~cOpenClEngine()
{
#ifdef USE_OPENCL
#endif
}

#ifdef USE_OPENCL

bool cOpenClEngine::checkErr(cl_int err, QString functionName)
{
	if (err != CL_SUCCESS)
	{
		qCritical() << "OpenCl ERROR: " << functionName << " (" << err << ")";
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

		definesCollector += " -DCODEHASH=" + QString(hashProgram.toHex());

		if (!useBuildCache) DeleteKernelCache();

		// if program is different than in previous run
		if (!(hashProgram == lastProgramHash && hashBuildParams == lastBuildParametersHash
					&& useBuildCache))
		{
			lastBuildParametersHash = hashBuildParams;
			lastProgramHash = hashProgram;

			// collecting all parts of program
			cl::Program::Sources sources;
			sources.emplace_back(programString.constData(), size_t(programString.length()));

			// creating cl::Program
			cl_int err;

			// Creates a program from source strings and Context.
			// Context initialized with support for multiple devices.
			// Therefore cl::Program initialized with device vector
			// Does not compile or link the program.
			program.reset(new cl::Program(*hardware->getContext(), sources, &err));

			if (checkErr(err, "cl::Program()"))
			{
				std::string buildParams =
					"-w -cl-single-precision-constant -cl-denorms-are-zero -cl-mad-enable";

				if (useFastRelaxedMath) buildParams += " -cl-fast-relaxed-math";

				buildParams.append(" -DOPENCL_KERNEL_CODE");

				buildParams += definesCollector.toUtf8().constData();

				WriteLogString("Build parameters", buildParams.c_str(), 2);

				// cl::Program::Build (compiles and links) a multi-device program executable
				// compiles and links for multiple devices simultaneously
				err = program->build(hardware->getClDevices(), buildParams.c_str());

				if (checkErr(err, "program->build()"))
				{
					WriteLog("OpenCl kernel program successfully compiled", 2);
					return true;
				}
				else
				{
					std::stringstream errorMessageStream;
					// TODO: update this error message to support multi-GPU
					errorMessageStream << "OpenCL Build log:\n"
														 << program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(
																	hardware->getEnabledDevices().at(0))
														 << std::endl;

					std::string buildLogText = errorMessageStream.str();

					*errorText = QString::fromStdString(errorMessageStream.str());

					std::cerr << buildLogText;

					emit showErrorMessage(
						QObject::tr("Error during compilation of OpenCL program\n") + errorText->left(500),
						cErrorMessage::errorMessage, nullptr);

					lastBuildParametersHash.clear();
					lastProgramHash.clear();
					return false;
				}
			}
			else
			{
				cErrorMessage::showMessage(
					QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("program")),
					cErrorMessage::errorMessage);
				return false;
			}
		}
		else
		{
			WriteLog("Re-compile is not needed", 2);
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
		// TODO: kernel
		if (CreateKernel(program.data()))
		{
			InitOptimalJob(params);
			return true;
		}
	}
	return false;
}

bool cOpenClEngine::CreateKernel(cl::Program *program)
{
	cl_int err;
	kernel.reset(new cl::Kernel(*program, GetKernelName().toLatin1().constData(), &err));
	if (checkErr(err, "cl::Kernel()"))
	{
		size_t workGroupSize = 0;

		// sets values for optimalJob
		// TODO: support multiple devices
		// TODO: create a optimalJob per device
		// iterate through getEnabledDevices
		// kernel->getWorkGroupInfo  workGroupSize
		kernel->getWorkGroupInfo(
			hardware->getEnabledDevices().at(0), CL_KERNEL_WORK_GROUP_SIZE, &workGroupSize);
		WriteLogSizeT("CL_KERNEL_WORK_GROUP_SIZE", workGroupSize, 2);

		size_t workGroupSizeOptimalMultiplier = 0;

		// TODO: support multiple devices
		// kernel->getWorkGroupInfo  workGroupSizeOptimalMultiplier
		kernel->getWorkGroupInfo(hardware->getEnabledDevices().at(0),
			CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &workGroupSizeOptimalMultiplier);
		WriteLogSizeT(
			"CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE", workGroupSizeOptimalMultiplier, 2);

		optimalJob.workGroupSize = workGroupSize;
		optimalJob.workGroupSizeOptimalMultiplier = workGroupSizeOptimalMultiplier;

		kernelCreated = true;
		return true;
	}
	else
	{
		emit showErrorMessage(QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("kernel")),
			cErrorMessage::errorMessage, nullptr);
		kernelCreated = false;
	}
	return false;
}

void cOpenClEngine::InitOptimalJob(const cParameterContainer *params)
{
	size_t width = params->Get<int>("image_width");
	size_t height = params->Get<int>("image_height");
	size_t memoryLimitByUser = params->Get<int>("opencl_memory_limit") * 1024 * 1024;
	size_t pixelCnt = width * height;
	
	// TODO: support multi-GPU
	// TODO: create a optimalJob per device
	// iterate through getSelectedDevicesInformation
	// requires deviceInfo.maxMemAllocSize for each device
	// for now, we use the same optimal job for all GPU
	// we will enable multi-gpu, but require the exact same gpu model
	// *this requires update*
	cOpenClDevice::sDeviceInformation deviceInfo = hardware->getSelectedDevicesInformation().at(0);

	optimalJob.stepSize = optimalJob.workGroupSize * optimalJob.workGroupSizeOptimalMultiplier;

	int exp = log(sqrt(optimalJob.stepSize + 1)) / log(2);

	optimalJob.stepSizeX = pow(2, exp);
	optimalJob.stepSizeY = optimalJob.stepSize / optimalJob.stepSizeX;

	//	optimalJob.stepSizeX = 1;
	//	optimalJob.stepSizeY = 1;

	optimalJob.workGroupSizeMultiplier = optimalJob.workGroupSizeOptimalMultiplier;
	optimalJob.lastProcessingTime = 1.0;

	size_t maxAllocMemSize = deviceInfo.maxMemAllocSize;
	size_t memSize = memoryLimitByUser;
	if (maxAllocMemSize > 0 && maxAllocMemSize * 0.75 < memoryLimitByUser)
	{
		memSize = maxAllocMemSize * 0.75;
	}
	if (optimalJob.sizeOfPixel != 0)
	{
		optimalJob.jobSizeLimit = memSize / optimalJob.sizeOfPixel;
	}
	else
	{
		optimalJob.jobSizeLimit = pixelCnt;
	}

	WriteLogSizeT("cOpenClEngine::InitOptimalJob(): stepSize", optimalJob.stepSize, 2);
	WriteLogSizeT("cOpenClEngine::InitOptimalJob(): stepSizeX", optimalJob.stepSizeX, 2);
	WriteLogSizeT("cOpenClEngine::InitOptimalJob(): stepSizeY", optimalJob.stepSizeY, 2);
}

bool cOpenClEngine::CreateCommandQueue()
{
	if (hardware->ContextCreated())
	{
		cl_int err;
		// TODO: support multiple devices
		// TODO: create a separate queue per device
		// iterate through getEnabledDevices
		queue.reset(
			new cl::CommandQueue(*hardware->getContext(), hardware->getEnabledDevices().at(0), 0, &err));

		if (checkErr(err, "CommandQueue::CommandQueue()"))
		{
			readyForRendering = true;
			return true;
		}
		else
		{
			emit showErrorMessage(
				QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("command queue")),
				cErrorMessage::errorMessage, nullptr);
			readyForRendering = false;
			return false;
		}
	}
	return false;
}

void cOpenClEngine::UpdateOptimalJobStart(size_t pixelsLeft)
{
	optimalJob.timer.restart();
	optimalJob.timer.start();
	double processingCycleTime = optimalJob.optimalProcessingCycle;

	optimalJob.workGroupSizeMultiplier *= processingCycleTime / optimalJob.lastProcessingTime;

	qint64 maxWorkGroupSizeMultiplier = pixelsLeft / optimalJob.workGroupSize;

	if (optimalJob.workGroupSizeMultiplier > maxWorkGroupSizeMultiplier)
		optimalJob.workGroupSizeMultiplier = maxWorkGroupSizeMultiplier;

	if (optimalJob.workGroupSizeMultiplier * optimalJob.workGroupSize > optimalJob.jobSizeLimit)
		optimalJob.workGroupSizeMultiplier = optimalJob.jobSizeLimit / optimalJob.workGroupSize;

	if (optimalJob.workGroupSizeMultiplier < optimalJob.workGroupSizeOptimalMultiplier)
		optimalJob.workGroupSizeMultiplier = optimalJob.workGroupSizeOptimalMultiplier;

	optimalJob.stepSize = optimalJob.workGroupSizeMultiplier * optimalJob.workGroupSize;

	//	qDebug() << "lastProcessingTime" << optimalJob.lastProcessingTime;
	//	qDebug() << "stepSize:" << optimalJob.stepSize;
}

void cOpenClEngine::Reset()
{
	lastBuildParametersHash.clear();
	lastProgramHash.clear();
	definesCollector.clear();
}

void cOpenClEngine::UpdateOptimalJobEnd()
{
	optimalJob.lastProcessingTime = optimalJob.timer.nsecsElapsed() / 1e9;
}

void cOpenClEngine::Lock()
{
	locked = true;
	lock.lock();
}

void cOpenClEngine::Unlock()
{

	lock.unlock();

	locked = false;
}

void cOpenClEngine::DeleteKernelCache()
{
// Delete NVIDIA driver build cache
#ifdef _WIN32
	QDir dir(QDir::homePath() + "/AppData/Roaming/NVIDIA/ComputeCache/");
#else
	QDir dir(QDir::homePath() + "/.nv/ComputeCache/");
#endif
	if (dir.exists()) dir.removeRecursively();
	if (!dir.exists()) QDir().mkdir(dir.absolutePath());
}

bool cOpenClEngine::PreAllocateBuffers(const cParameterContainer *params)
{
	ReleaseMemory();
	RegisterInputOutputBuffers(params);

	cl_int err;

	if (hardware->ContextCreated())
	{
		for (auto &inputAndOutputBuffer : inputAndOutputBuffers)
		{
			inputAndOutputBuffer.ptr.reset(
				new char[inputAndOutputBuffer.size()], sClInputOutputBuffer::Deleter);
			inputAndOutputBuffer.clPtr.reset(
				new cl::Buffer(*hardware->getContext(), CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
					inputAndOutputBuffer.size(), inputAndOutputBuffer.ptr.data(), &err));
			if (!checkErr(err, "new cl::Buffer(...) for " + inputAndOutputBuffer.name))
			{
				emit showErrorMessage(
					QObject::tr("OpenCL %1 cannot be created!").arg(inputAndOutputBuffer.name),
					cErrorMessage::errorMessage, nullptr);
				return false;
			}
		}

		for (auto &outputBuffer : outputBuffers)
		{
			outputBuffer.ptr.reset(new char[outputBuffer.size()], sClInputOutputBuffer::Deleter);
			outputBuffer.clPtr.reset(
				new cl::Buffer(*hardware->getContext(), CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
					outputBuffer.size(), outputBuffer.ptr.data(), &err));
			if (!checkErr(err, "new cl::Buffer(...) for " + outputBuffer.name))
			{
				emit showErrorMessage(QObject::tr("OpenCL %1 cannot be created!").arg(outputBuffer.name),
					cErrorMessage::errorMessage, nullptr);
				return false;
			}
		}

		for (auto &inputBuffer : inputBuffers)
		{
			inputBuffer.ptr.reset(new char[inputBuffer.size()], sClInputOutputBuffer::Deleter);
			inputBuffer.clPtr.reset(new cl::Buffer(*hardware->getContext(),
				CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, inputBuffer.size(), inputBuffer.ptr.data(), &err));
			if (!checkErr(err, "new cl::Buffer(...) for " + inputBuffer.name))
			{
				emit showErrorMessage(QObject::tr("OpenCL %1 cannot be created!").arg(inputBuffer.name),
					cErrorMessage::errorMessage, nullptr);
				return false;
			}
		}
	}
	else
	{
		emit showErrorMessage(
			QObject::tr("OpenCL context is not ready"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

void cOpenClEngine::ReleaseMemory()
{
	for (auto &outputBuffer : outputBuffers)
	{
		outputBuffer.ptr.reset();
		outputBuffer.clPtr.reset();
	}
	for (auto &inputBuffer : inputBuffers)
	{
		inputBuffer.ptr.reset();
		inputBuffer.clPtr.reset();
	}
	for (auto &inputAndOutputBuffer : inputAndOutputBuffers)
	{
		inputAndOutputBuffer.ptr.reset();
		inputAndOutputBuffer.clPtr.reset();
	}
	inputBuffers.clear();
	outputBuffers.clear();
	inputAndOutputBuffers.clear();
}

bool cOpenClEngine::WriteBuffersToQueue()
{
	for (auto &inputBuffer : inputBuffers)
	{
		cl_int err = queue->enqueueWriteBuffer(
			*inputBuffer.clPtr, CL_TRUE, 0, inputBuffer.size(), inputBuffer.ptr.data());
		if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(...) for " + inputBuffer.name))
		{
			emit showErrorMessage(QObject::tr("Cannot enqueue writing OpenCL %1").arg(inputBuffer.name),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}
	for (auto &inputAndOutputBuffer : inputAndOutputBuffers)
	{
		cl_int err = queue->enqueueWriteBuffer(*inputAndOutputBuffer.clPtr, CL_TRUE, 0,
			inputAndOutputBuffer.size(), inputAndOutputBuffer.ptr.data());
		if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(...) for " + inputAndOutputBuffer.name))
		{
			emit showErrorMessage(
				QObject::tr("Cannot enqueue writing OpenCL %1").arg(inputAndOutputBuffer.name),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	int err = queue->finish();
	if (!checkErr(err, "CommandQueue::finish() - write buffers"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot finish writing OpenCL buffers"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngine::ReadBuffersFromQueue()
{
	for (auto &outputBuffer : outputBuffers)
	{
		cl_int err = queue->enqueueReadBuffer(
			*outputBuffer.clPtr, CL_TRUE, 0, outputBuffer.size(), outputBuffer.ptr.data());
		if (!checkErr(err, "CommandQueue::enqueueReadBuffer() for " + outputBuffer.name))
		{
			emit showErrorMessage(
				QObject::tr("Cannot enqueue reading OpenCL buffers %1").arg(outputBuffer.name),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}
	for (auto &inputAndOutputBuffer : inputAndOutputBuffers)
	{
		cl_int err = queue->enqueueReadBuffer(*inputAndOutputBuffer.clPtr, CL_TRUE, 0,
			inputAndOutputBuffer.size(), inputAndOutputBuffer.ptr.data());
		if (!checkErr(err, "CommandQueue::enqueueReadBuffer() for " + inputAndOutputBuffer.name))
		{
			emit showErrorMessage(
				QObject::tr("Cannot enqueue reading OpenCL buffers %1").arg(inputAndOutputBuffer.name),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	int err = queue->finish();
	if (!checkErr(err, "CommandQueue::finish() - read buffers"))
	{
		emit showErrorMessage(QObject::tr("Cannot finish reading OpenCL output buffers"),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngine::AssignParametersToKernel()
{
	int argIterator = 0;
	for (auto &inputBuffer : inputBuffers)
	{
		int err = kernel->setArg(argIterator++, *inputBuffer.clPtr);
		if (!checkErr(
					err, "kernel->setArg(" + QString::number(argIterator) + ") for " + inputBuffer.name))
		{
			emit showErrorMessage(QObject::tr("Cannot set OpenCL argument for %1").arg(inputBuffer.name),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}
	for (auto &outputBuffer : outputBuffers)
	{
		int err = kernel->setArg(argIterator++, *outputBuffer.clPtr);
		if (!checkErr(
					err, "kernel->setArg(" + QString::number(argIterator) + ") for " + outputBuffer.name))
		{
			emit showErrorMessage(QObject::tr("Cannot set OpenCL argument for %1").arg(outputBuffer.name),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}
	for (auto &inputAndOutputBuffer : inputAndOutputBuffers)
	{
		int err = kernel->setArg(argIterator++, *inputAndOutputBuffer.clPtr);
		if (!checkErr(err,
					"kernel->setArg(" + QString::number(argIterator) + ") for " + inputAndOutputBuffer.name))
		{
			emit showErrorMessage(
				QObject::tr("Cannot set OpenCL argument for %1").arg(inputAndOutputBuffer.name),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}
	return AssignParametersToKernelAdditional(argIterator);
}

#endif
