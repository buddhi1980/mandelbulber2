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

cOpenClEngine::cOpenClEngine(cOpenClHardware *_hardware) : QObject(_hardware), hardware(_hardware)
{
#ifdef USE_OPENCL
	program = nullptr;
	kernel = nullptr;
	programsLoaded = false;
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
		optimalJob = CalculateOptimalJob(params);
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
		size_t workGroupSize = 0;
		kernel->getWorkGroupInfo(
			hardware->getSelectedDevice(), CL_KERNEL_WORK_GROUP_SIZE, &workGroupSize);
		qDebug() << "CL_KERNEL_WORK_GROUP_SIZE" << workGroupSize;

		optimalJob.workGroupSize = workGroupSize;

		return true;
	}
	return false;
}

cOpenClEngine::sOptimalJob cOpenClEngine::CalculateOptimalJob(const cParameterContainer *params)
{
	sOptimalJob optJob = optimalJob;

	size_t width = params->Get<int>("image_width");
	size_t height = params->Get<int>("image_height");

	optJob.pixelsPerJob =  optimalJob.workGroupSize * hardware->getSelectedDeviceInformation().maxComputeUnits;
	optJob.numberOfSteps = height * width / optJob.pixelsPerJob + 1;
	optJob.stepSize = (width * height / optJob.numberOfSteps / optJob.pixelsPerJob + 1) * optJob.pixelsPerJob;

	qDebug() << "pixelsPerJob:" <<  optJob.pixelsPerJob;
	qDebug() << "numberOfSteps:" <<  optJob.numberOfSteps;
	qDebug() << "stepSize:" <<  optJob.stepSize;

	return optJob;
}

#endif
