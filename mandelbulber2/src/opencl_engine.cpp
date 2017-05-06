/*
 * opencl_engine.cpp
 *
 *  Created on: 3 maj 2017
 *      Author: krzysztof
 */

#include "opencl_engine.h"

#include <sstream>
#include <iostream>
#include "system.hpp"

#include "opencl_hardware.h"

#ifdef USE_OPENCL
#include "../opencl/mandelbulber_cl_data.h"
#endif

cOpenClEngine::cOpenClEngine(cOpenClHardware *_hardware) : QObject(_hardware), hardware(_hardware)
{
#ifdef USE_OPENCL
	program = nullptr;
	kernel = nullptr;
	programsLoaded = false;
	workGroupSize = 0;
#endif
}

cOpenClEngine::~cOpenClEngine()
{
#ifdef USE_OPENCL
	if (program) delete program;
	if (kernel) delete kernel;
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

bool cOpenClEngine::Build(cl::Program *prog, QString *errorText)
{
	std::string buildParams;
	buildParams = "-w -cl-single-precision-constant -cl-denorms-are-zero ";
	cl_int err;
	err = prog->build(hardware->getClDevices(), buildParams.c_str());

	if (checkErr(err, "program->build()"))
	{
		qDebug() << "OpenCl kernel program successfully compiled";
		return true;
	}
	else
	{
		std::stringstream errorMessageStream;
		errorMessageStream << "OpenCL Build log:\t"
											 << program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(hardware->getSelectedDevice())
											 << std::endl;
		*errorText = QString::fromStdString(errorMessageStream.str());

		std::string buildLogText;
		buildLogText = errorMessageStream.str();
		std::cerr << buildLogText;
		return false;
	}
}

bool cOpenClEngine::CreateKernel4Program(const cParameterContainer *params)
{
	if (CreateKernel(program))
	{
		AllocateBuffers(params);
		return true;
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
		kernel->getWorkGroupInfo(
			hardware->getSelectedDevice(), CL_KERNEL_WORK_GROUP_SIZE, &workGroupSize);
		qDebug() << "CL_KERNEL_WORK_GROUP_SIZE" << workGroupSize;

		return true;
	}
	return false;
}

#endif
