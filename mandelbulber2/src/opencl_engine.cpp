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
 * cOpenClEngine - class to load and compile opencl programs
 */

#include "opencl_engine.h"

#include <sstream>
#include <iostream>
#include "system.hpp"
#include "files.h"
#include "fractal_list.hpp"

#ifdef USE_OPENCL
#include "../opencl/mandelbulber_cl_data.h"
#endif

cOpenClEngine::cOpenClEngine(QObject *parent) : cOpenClHardware(parent)
{
#ifdef USE_OPENCL
	mainFractalProgram = nullptr;
	programsLoaded = false;
#endif
}

cOpenClEngine::~cOpenClEngine()
{
#ifdef USE_OPENCL
	if (mainFractalProgram) delete mainFractalProgram;
#endif
}

#ifdef USE_OPENCL
void cOpenClEngine::LoadSourcesAndCompile()
{
	QString progPathHeader("#define INCLUDE_PATH_CL_DATA \"");

// pushing path to mandelbulber_cl_data.h
// using correct slashes is important, because OpenCl compiler would fail
#ifdef WIN32
	progPathHeader += systemData.sharedDir + "opencl\\mandelbulber_cl_data.h\"";
#else
	progPathHeader += systemData.sharedDir + "opencl/mandelbulber_cl_data.h\"";
#endif
	QByteArray progPathHeaderUft8 = progPathHeader.toUtf8();
	qDebug() << progPathHeader;

	programsLoaded = false;

	QByteArray progEngine;
	try
	{
		progEngine = LoadUtf8TextFromFile(systemData.sharedDir + "opencl" + QDir::separator()
																			+ "engines" + QDir::separator() + "test_engine.cl");
		if (progEngine.isEmpty()) throw QString("Can't load main program");

		// Test loading of all fractal kernels
		for (int f = 0; f < fractalList.size(); f++)
		{
			progEngine.append(LoadUtf8TextFromFile(fractalList[f].getOpenCLFilename()));
		}

		//.... here will be loading of more programs
	}
	catch (const QString &ex)
	{
		qCritical() << "OpenCl program error: " << ex;
		return;
	}

	// collecting all parts of program
	cl::Program::Sources sources;
	sources.push_back(
		std::make_pair(progPathHeaderUft8.constData(), size_t(progPathHeader.length())));
	sources.push_back(std::make_pair(progEngine.constData(), size_t(progEngine.length())));

	// creating cl::Program
	cl_int err;
	mainFractalProgram = new cl::Program(*context, sources, &err);
	if (checkErr(err, "cl::Program()"))
	{
		// building OpenCl kernel

		QString errorString;
		if (Build(mainFractalProgram, &errorString))
		{
			programsLoaded = true;
		}
	}
}

bool cOpenClEngine::Build(cl::Program *prog, QString *errorText)
{
	std::string buildParams;
	buildParams = "-w -cl-single-precision-constant -cl-denorms-are-zero ";
	cl_int err;
	err = prog->build(clDevices, buildParams.c_str());

	if (checkErr(err, "program->build()"))
	{
		qDebug() << "OpenCl kernel program successfully compiled";
		return true;
	}
	else
	{
		std::stringstream errorMessageStream;
		errorMessageStream << "OpenCL Build log:\t"
											 << mainFractalProgram->getBuildInfo<CL_PROGRAM_BUILD_LOG>(clDevices[0])
											 << std::endl;
		*errorText = QString::fromStdString(errorMessageStream.str());

		std::string buildLogText;
		buildLogText = errorMessageStream.str();
		std::cerr << buildLogText;
		return false;
	}
}

#endif
