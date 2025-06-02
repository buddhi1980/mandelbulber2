/*
 * opencl_engine_render_nebula.cpp
 *
 *  Created on: 4 maj 2025
 *      Author: krzysztof
 */

#include "opencl_engine_render_nebula.h"

#include <memory>
#include <map>

#include "camera_target.hpp"
#include "parameters.hpp"
#include "files.h"
#include "fractal.h"
#include "fractparams.hpp"
#include "fractal_container.hpp"
#include "nine_fractals.hpp"
#include "opencl_hardware.h"
#include "opencl_input_output_buffer.h"
#include "system_data.hpp"
#include "system_directories.hpp"
#include "write_log.hpp"

cOpenClEngineRenderNebula::cOpenClEngineRenderNebula(cOpenClHardware *_hardware)
		: cOpenClEngine(_hardware)
{
	customFormulaCodes.reserve(NUMBER_OF_FRACTALS);
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		customFormulaCodes.append(QString());
	}
}

cOpenClEngineRenderNebula::~cOpenClEngineRenderNebula()
{
#ifdef USE_OPENCL
	ReleaseMemory();
#endif
}

#ifdef USE_OPENCL

void cOpenClEngineRenderNebula::SetParameters(
	std::shared_ptr<const cParameterContainer> paramContainer,
	std::shared_ptr<const cFractalContainer> fractalContainer,
	std::shared_ptr<sParamRender> paramRender, std::shared_ptr<cNineFractals> fractals)
{
	constantInBuffer.reset(new sClInConstants);

	definesCollector.clear();

	// creating list of used formulas
	listOfUsedFormulas.clear();
	// creating list of used formulas
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractal::enumFractalFormula fractalFormula = fractals->GetFractal(i)->formula;
		int listIndex = cNineFractals::GetIndexOnFractalList(fractalFormula);
		QString formulaName = newFractalList.at(listIndex)->getInternalName();
		if (formulaName == "custom")
		{
			formulaName += QString::number(i);
			QString formulaCode = fractalContainer->at(i)->Get<QString>("formula_code");

			if (formulaCode.contains("CustomIteration("))
			{
				formulaCode = formulaCode.replace("CustomIteration", QString("Custom%1Iteration").arg(i));
				QFile qFile(
					systemDirectories.GetOpenCLTempFolder() + QDir::separator() + formulaName + ".cl");
				if (qFile.open(QIODevice::WriteOnly))
				{
					qFile.write(formulaCode.toUtf8());
					qFile.close();
				}
			}
			else
			{
				emit showErrorMessage(
					QObject::tr("Custom formula %1 has missing function name CustomIteration()!").arg(i),
					cErrorMessage::errorMessage, nullptr);
			}
			customFormulaCodes[i] = formulaCode;
		}
		else
		{
			customFormulaCodes[i] = QString();
		}
		listOfUsedFormulas.append(formulaName);
	}
	// adding #defines to the list
	for (int i = 0; i < listOfUsedFormulas.size(); i++)
	{
		QString internalID = toCamelCase(listOfUsedFormulas.at(i));
		if (internalID != "" && internalID != "None")
		{
			QString functionName = internalID.left(1).toUpper() + internalID.mid(1) + "Iteration";
			definesCollector += " -DFORMULA_ITER_" + QString::number(i) + "=" + functionName;
		}
	}

	listOfUsedFormulas.removeDuplicates(); // eliminate duplicates

	if (paramRender->common.foldings.boxEnable) definesCollector += " -DBOX_FOLDING";
	if (paramRender->common.foldings.sphericalEnable) definesCollector += " -DSPHERICAL_FOLDING";

	bool weightUsed = false;
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		if (fractals->GetWeight(i) != 1.0)
		{
			weightUsed = true;
		}
	}
	if (weightUsed) definesCollector += " -DITERATION_WEIGHT";

	// copy all cl parameters to constant buffer
	constantInBuffer->params = clCopySParamRenderCl(*paramRender);

	constantInBuffer->params.viewAngle = toClFloat3(paramRender->viewAngle * M_PI / 180.0);

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		constantInBuffer->fractal[i] = clCopySFractalCl(*fractals->GetFractal(i));
	}

	fractals->CopyToOpenclData(&constantInBuffer->sequence);

	numberOfPixels = quint64(paramRender->imageWidth) * quint64(paramRender->imageHeight);
}

bool cOpenClEngineRenderNebula::LoadSourcesAndCompile(
	std::shared_ptr<const cParameterContainer> params, QString *compilerErrorOutput)
{
	programsLoaded = false;
	readyForRendering = false;

	QByteArray programEngine;
	try
	{
		QString openclPath = systemDirectories.sharedDir + "opencl" + QDir::separator();
		QString openclEnginePath = openclPath + "engines" + QDir::separator();

		QStringList clHeaderFiles;
		CreateListOfHeaderFiles(clHeaderFiles);

		// pass through define constants
		programEngine.append("#define USE_OPENCL 1\n");

		if (params->Get<bool>("opencl_precision"))
			programEngine.append(
				QString("#define DOUBLE_PRECISION " + QString::number(1) + "\n").toUtf8());

#ifdef _WIN32
		QString openclPathSlash = openclPath.replace("/", "\\"); // replace single slash with backslash
#else
		QString openclPathSlash = openclPath;
#endif

		CreateListOfIncludes(clHeaderFiles, openclPathSlash, params, openclEnginePath, programEngine);

		// main engine
		QString mainEngineFileName = "nebula.cl";
		QString engineFullFileName = openclEnginePath + mainEngineFileName;
		programEngine.append(LoadUtf8TextFromFile(engineFullFileName));

		// adding hash code for custom formulas to the end of code to force recompile
		QCryptographicHash hashCryptProgram(QCryptographicHash::Md4);
		for (QString code : customFormulaCodes)
		{
			hashCryptProgram.addData(code.toUtf8());
		}
		QByteArray hashCustomPrograms = hashCryptProgram.result();
		QString hashText = QString("//%1").arg(QString(hashCustomPrograms.toHex()));
		programEngine.append(hashText.toUtf8());
	}
	catch (const QString &ex)
	{
		qCritical() << "OpenCl program error: " << ex;
		return false;
	}

	SetUseBuildCache(!params->Get<bool>("opencl_disable_build_cache"));
	SetUseFastRelaxedMath(params->Get<bool>("opencl_use_fast_relaxed_math"));

	// building OpenCl kernel
	QString errorString;
	bool quiet = (compilerErrorOutput) ? true : false;

	QElapsedTimer timer;
	timer.start();
	if (Build(programEngine, &errorString, quiet))
	{
		programsLoaded = true;
	}
	else
	{
		programsLoaded = false;
		WriteLog(errorString, 0);
	}

	if (compilerErrorOutput) *compilerErrorOutput = errorString;

	WriteLogDouble(
		"cOpenClEngineRenderFractal: Opencl kernel build time [s]", timer.nsecsElapsed() / 1.0e9, 2);

	return programsLoaded;
}

void cOpenClEngineRenderNebula::RegisterInputOutputBuffers(
	std::shared_ptr<const cParameterContainer> params)
{
	Q_UNUSED(params);
	inputAndOutputBuffers[0] << sClInputOutputBuffer(
		sizeof(cl_float4), numberOfPixels, "image-buffer");
}

bool cOpenClEngineRenderNebula::PreAllocateBuffers(
	std::shared_ptr<const cParameterContainer> params)
{
	cOpenClEngine::PreAllocateBuffers(params);

	cl_int err;

	if (hardware->ContextCreated())
	{
		WriteLog(QString("Allocating OpenCL buffer for constants"), 2);

		inCLConstBuffer.append(std::shared_ptr<cl::Buffer>(
			new cl::Buffer(*hardware->getContext(0), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
				sizeof(sClInConstants), constantInBuffer.get(), &err)));
		if (!checkErr(err,
					"cl::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, "
					"sizeof(sClInConstants), constantInBuffer, &err)"))
		{
			emit showErrorMessage(
				QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for constants")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}
	return true;
}

bool cOpenClEngineRenderNebula::AssignParametersToKernelAdditional(
	uint argIterator, int deviceIndex)
{
	int err =
		clKernels.at(deviceIndex)
			->setArg(argIterator++,
				*inCLConstBuffer[deviceIndex]); // input inOut in constant memory (faster than global)
	if (!checkErr(err, "kernel->setArg(2, *inCLConstBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("constant inOut")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = clKernels.at(deviceIndex)->setArg(argIterator++, std::rand()); // random seed
	if (!checkErr(err, "kernel->setArg(4, initRandomSeed)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("random seed")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}
	return true;
}

bool cOpenClEngineRenderNebula::ProcessQueue(quint64 pixelsLeft, quint64 pixelIndex)
{
	return true;
}

bool cOpenClEngineRenderNebula::Render(std::shared_ptr<cImage> image, bool *stopRequest)
{
	return true;
}

void cOpenClEngineRenderNebula::ReleaseMemory()
{
	cOpenClEngine::ReleaseMemory();
}

QString cOpenClEngineRenderNebula::GetKernelName()
{
	return QString("Nebula");
}

size_t cOpenClEngineRenderNebula::CalcNeededMemory()
{
	return 0;
}

void cOpenClEngineRenderNebula::CreateListOfHeaderFiles(QStringList &clHeaderFiles)
{
	clHeaderFiles.append("defines_cl.h");
	clHeaderFiles.append("opencl_typedefs.h");
	clHeaderFiles.append("opencl_algebra.h");
	clHeaderFiles.append("common_params_cl.hpp");
	clHeaderFiles.append("image_adjustments_cl.h");
	clHeaderFiles.append("fractal_cl.h");
	clHeaderFiles.append("fractparams_cl.hpp");
	clHeaderFiles.append("fractal_sequence_cl.h");
	clHeaderFiles.append("input_data_structures.h");
}

void cOpenClEngineRenderNebula::CreateListOfIncludes(const QStringList &clHeaderFiles,
	const QString &openclPathSlash, std::shared_ptr<const cParameterContainer> params,
	const QString &openclEnginePath, QByteArray &programEngine)
{
	for (int i = 0; i < clHeaderFiles.size(); i++)
	{
		AddInclude(programEngine, openclPathSlash + clHeaderFiles.at(i));
	}
	if (params->Get<bool>("box_folding") || params->Get<bool>("spherical_folding"))
	{
		AddInclude(programEngine, openclEnginePath + "basic_foldings.cl");
	}
	// fractal formulas - only actually used
	for (int i = 0; i < listOfUsedFormulas.size(); i++)
	{
		QString formulaName = listOfUsedFormulas.at(i);
		if (formulaName != "" && formulaName != "none")
		{
			if (formulaName.startsWith("custom"))
			{
				AddInclude(programEngine,
					systemDirectories.GetOpenCLTempFolder() + QDir::separator() + formulaName + ".cl");
			}
			else
			{
				AddInclude(programEngine, systemDirectories.sharedDir + "formula" + QDir::separator()
																		+ "opencl" + QDir::separator() + formulaName + ".cl");
			}
		}
	}
}

QString cOpenClEngineRenderNebula::toCamelCase(const QString &s)
{
	QStringList upperCaseLookup({"Vs", "Kifs", "De", "Xy", "Xyz", "Cxyz", "Vcl", "Chs", "Difs"});
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
	QStringList parts = s.split('_', QString::SkipEmptyParts);
#else
	QStringList parts = s.split('_', Qt::SkipEmptyParts);
#endif
	for (int i = 0; i < parts.size(); ++i)
	{
		parts[i].replace(0, 1, parts[i][0].toUpper());

		// rewrite to known capital names in iteration function names
		if (upperCaseLookup.contains(parts[i]))
		{
			parts[i] = parts[i].toUpper();
		}
	}
	return parts.join("");
}

#endif
