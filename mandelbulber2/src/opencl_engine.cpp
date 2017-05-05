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
#include "files.h"

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
		if(Build(mainFractalProgram, &errorString))
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
											 << mainFractalProgram->getBuildInfo<CL_PROGRAM_BUILD_LOG>(clDevices[0]) << std::endl;
		*errorText = QString::fromStdString(errorMessageStream.str());

		std::string buildLogText;
		buildLogText = errorMessageStream.str();
		std::cerr << buildLogText;
		return false;
	}
}

#endif
