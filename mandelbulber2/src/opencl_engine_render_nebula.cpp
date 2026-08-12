/*
 * opencl_engine_render_nebula.cpp
 *
 *  Created on: 4 maj 2025
 *      Author: krzysztof
 */

#include "opencl_engine_render_nebula.h"

#include <memory>
#include "common_math.h"
#include <map>
#include <QApplication>

#include "camera_target.hpp"
#include "cimage.hpp"
#include "parameters.hpp"
#include "files.h"
#include "fractal.h"
#include "fractparams.hpp"
#include "fractal_container.hpp"
#include "hybrid_fractal_sequences.h"
#include "global_data.hpp"
#include "hybrid_fractal_sequences.h"
#include "opencl_hardware.h"
#include "opencl_input_output_buffer.h"
#include "progress_text.hpp"
#include "system_data.hpp"
#include "system_directories.hpp"
#include "write_log.hpp"

static QByteArray StripNonAscii(const QByteArray &data)
{
	QByteArray result;
	result.reserve(data.size());
	for (char c : data)
	{
		if (static_cast<unsigned char>(c) < 128)
		{
			result.append(c);
		}
	}
	return result;
}

cOpenClEngineRenderNebula::cOpenClEngineRenderNebula(cOpenClHardware *_hardware)
		: cOpenClEngine(_hardware)
{
#ifdef USE_OPENCL
	customFormulaCodes.clear();
#endif
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
	std::shared_ptr<sParamRender> paramRender, std::shared_ptr<cHybridFractalSequences> fractals,
	const cHybridFractalSequences &hybridSequences)
{
	constantInBuffer.reset(new sClInConstants);

	definesCollector.clear();

	listOfUsedFormulas.clear();
	customFormulaCodes.clear();

	int formulaIndex = 0;
	for (int s = 0; s < hybridSequences.GetNumberOfSequences(); s++)
	{
		const cHybridFractalSequences::sSequence *seq = hybridSequences.GetSequence(s);
		for (int f = 0; f < seq->numberOfFractalsInTheSequence; f++)
		{
			const cHybridFractalSequences::sFractalData &fractData = seq->fractData[f];
			QString formulaName =
				QString::fromStdString(fractData.fractalFormulaObject->getInternalName());

			if (formulaName == "custom")
			{
				formulaName += QString::number(formulaIndex);
				QString formulaCode =
					fractalContainer->at(fractData.objectId - 1)->Get<QString>("formula_code");

				if (formulaCode.contains("CustomIteration("))
				{
					formulaCode =
						formulaCode.replace("CustomIteration", QString("Custom%1Iteration").arg(formulaIndex));
					formulaCode.replace("__constant sFractalCl *fractal", "__global sFractalCl *fractal");
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
					emit EmitErrorMessage(
						QObject::tr("Custom formula %1 has missing function name CustomIteration()!")
							.arg(formulaIndex),
						cErrorMessage::errorMessage, nullptr);
				}
				customFormulaCodes.append(formulaCode);
			}

			listOfUsedFormulas.append(formulaName);
			formulaIndex++;
		}
	}

	if (listOfUsedFormulas.isEmpty())
	{
		listOfUsedFormulas.append("mandelbulb");
	}

	// adding #defines to the list
	for (int i = 0; i < listOfUsedFormulas.size(); i++)
	{
		QString internalID = toCamelCase(listOfUsedFormulas.at(i));
		QString functionName;
		if (internalID != "" && internalID != "None")
		{
			functionName = internalID.left(1).toUpper() + internalID.mid(1) + "Iteration";
		}
		else
		{
			functionName = "DummyIteration";
		}
		definesCollector += " -DFORMULA_ITER_" + QString::number(i) + "=" + functionName;
	}

	listOfUsedFormulas.removeDuplicates(); // eliminate duplicates

	// check for hybrid and iteration weight
	bool useHybrid = false;
	bool weightUsed = false;
	for (int s = 0; s < hybridSequences.GetNumberOfSequences(); s++)
	{
		const cHybridFractalSequences::sSequence *seq = hybridSequences.GetSequence(s);
		if (seq->isHybrid)
		{
			useHybrid = true;
		}
		for (int f = 0; f < seq->numberOfFractalsInTheSequence; f++)
		{
			if (seq->fractData[f].formulaWeight != 1.0)
			{
				weightUsed = true;
			}
		}
	}
	if (useHybrid) definesCollector += " -DIS_HYBRID";
	if (weightUsed) definesCollector += " -DITERATION_WEIGHT";

	definesCollector += " -DNEBULA_MODE";
	definesCollector += " -DMAX_ITERATIONS=" + QString::number(paramRender->N);

	if (paramRender->common.foldings.boxEnable) definesCollector += " -DBOX_FOLDING";
	if (paramRender->common.foldings.sphericalEnable) definesCollector += " -DSPHERICAL_FOLDING";

	if (paramRender->limitsEnabled) definesCollector += " -DLIMITS_ENABLED";

	switch (paramRender->nebulaColorMixing)
	{
		case 0: // lighten
			definesCollector += " -DNEBULA_COLOR_MIXING_LIGHTEN";
			break;
		case 1: // darken
			definesCollector += " -DNEBULA_COLOR_MIXING_DARKEN";
			break;
		case 2: // darken by brighness
			definesCollector += " -DNEBULA_COLOR_MIXING_DARKEN_BY_BRIGHTNESS";
			break;
		default: break;
	}

	if (paramRender->nebulaXAxisColorsEnabled) definesCollector += " -DNEBULA_X_AXIS_COLORS";
	if (paramRender->nebulaYAxisColorsEnabled) definesCollector += " -DNEBULA_Y_AXIS_COLORS";
	if (paramRender->nebulaZAxisColorsEnabled) definesCollector += " -DNEBULA_Z_AXIS_COLORS";
	if (paramRender->nebulaIterationsColorsEnabled) definesCollector += " -DNEBULA_ITERATIONS_COLORS";
	if (paramRender->nebulaGridDomainEnabled) definesCollector += " -DNEBULA_GRID_DOMAIN_ENABLED";

	// copy all cl parameters to constant buffer
	constantInBuffer->params = clCopySParamRenderCl(*paramRender);

	constantInBuffer->params.viewAngle = toClFloat3(paramRender->viewAngle * M_PI / 180.0);

	numberOfPixels = quint64(paramRender->imageWidth) * quint64(paramRender->imageHeight);

	zBufferDefault = (paramRender->camera - paramRender->target).Length();

	dynamicData.reset(new cOpenClDynamicData(8));
	dynamicData->ReserveHeader();
	dynamicData->BuildNebulaGradientsData(paramRender.get());
	dynamicData->BuildNebulaSequenceData(&hybridSequences);
	dynamicData->BuildHybridSequencesData(&hybridSequences);
	dynamicData->BuildFractalData(&hybridSequences);
	dynamicData->FillHeader();

	inBuffer = dynamicData->GetData();

	// Populate renderData for shader
	renderData.nebulaSequencesCount = hybridSequences.GetNumberOfSequences();
	renderData.nebulaSequencesDataOffset =
		(intptr_t)dynamicData->GetItemOffset(1); // nebulaSequencesItemIndex = 1
	renderData.hybridSequencesCount = hybridSequences.GetNumberOfSequences();
	renderData.hybridSequencesDataOffset =
		(intptr_t)dynamicData->GetItemOffset(6); // hybridSequencesItemIndex = 6
	renderData.fractalsCount = 0;
	renderData.fractalsDataOffset = (intptr_t)dynamicData->GetItemOffset(7); // fractalsItemIndex = 7
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

		// Generate formula switch code from listOfUsedFormulas
		QByteArray formulaSwitchCode;
		if (listOfUsedFormulas.isEmpty())
		{
			formulaSwitchCode.append("		z = MandelbulbIteration(z, fractal, &aux);");
		}
		else
		{
			QString internalID = toCamelCase(listOfUsedFormulas.at(0));
			QString functionName;
			if (internalID != "" && internalID != "None")
			{
				functionName = internalID.left(1).toUpper() + internalID.mid(1) + "Iteration";
			}
			else
			{
				functionName = "DummyIteration";
			}
			formulaSwitchCode.append(("		z = " + functionName + "(z, fractal, &aux);").toUtf8());
		}

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

		CreateListOfIncludes(
			clHeaderFiles, openclPathSlash, params, openclEnginePath, programEngine, QByteArray());

		// main engine
		QString mainEngineFileName = "nebula.cl";
		QString engineFullFileName = openclEnginePath + mainEngineFileName;
		programEngine.append(LoadUtf8TextFromFile(engineFullFileName));

		// Replace PLACEHOLDER_FOR_FORMULA_ITER with actual formula code
		if (!formulaSwitchCode.isEmpty())
		{
			int placeholderPos = programEngine.indexOf("// PLACEHOLDER_FOR_FORMULA_ITER");
			if (placeholderPos >= 0)
			{
				int placeholderLen = strlen("// PLACEHOLDER_FOR_FORMULA_ITER");
				programEngine.replace(placeholderPos, placeholderLen, formulaSwitchCode);
			}
		}

		// Write combined OpenCL source for debugging compiler errors
		{
			QString combinedPath = systemDirectories.GetDataDirectoryHidden() + "openclKernelCombined.cl";
			QFile combinedFile(combinedPath);
			if (combinedFile.open(QIODevice::WriteOnly))
			{
				combinedFile.write(programEngine);
			}
		}

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

	// inputBuffers[0] << sClInputOutputBuffer(sizeof(cl_int), jobSize, "random-numbers");
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
			emit EmitErrorMessage(
				QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for constants")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}

		WriteLog(QString("Allocating OpenCL buffer for dynamic data"), 2);

		inCLBuffer.append(std::shared_ptr<cl::Buffer>(new cl::Buffer(*hardware->getContext(0),
			CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size_t(inBuffer.size()), inBuffer.data(), &err)));
		if (!checkErr(err,
					"Buffer::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, "
					"sizeof(sClInBuff), inBuffer, &err)"))
		{
			emit EmitErrorMessage(
				QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for variable data")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}

		WriteLog(QString("Allocating OpenCL buffer for render data"), 2);

		inCLRenderDataBuffer.append(std::shared_ptr<cl::Buffer>(
			new cl::Buffer(*hardware->getContext(0), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
				size_t(sizeof(sNebulaRenderData)), &renderData, &err)));
		if (!checkErr(
					err, "cl::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY, sizeof(sNebulaRenderData))"))
		{
			emit EmitErrorMessage(
				QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for render data")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}
	return true;
}

bool cOpenClEngineRenderNebula::WriteBuffersToQueue()
{
	cOpenClEngine::WriteBuffersToQueue();
	cl_int err = 0;

	WriteLog(QString("Writing OpenCL input buffer"), 2);
	err = clQueues.at(0)->enqueueWriteBuffer(
		*inCLBuffer[0], CL_TRUE, 0, inBuffer.size(), inBuffer.data());
	if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(inCLBuffer)"))
	{
		emit EmitErrorMessage(
			QObject::tr("Cannot enqueue writing OpenCL %1").arg(QObject::tr("input buffers")),
			cErrorMessage::errorMessage, nullptr);
		return false;
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
		emit EmitErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("constant inOut")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = clKernels.at(deviceIndex)
					->setArg(argIterator++, *inCLBuffer[deviceIndex]); // input data in global memory
	if (!checkErr(err, "kernel->setArg(1, *inCLBuffer)"))
	{
		emit EmitErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("input inOut")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	// Write updated renderData to buffer (used by SetParameters, not passed to kernel)
	cl_int writeErr = clQueues.at(0)->enqueueWriteBuffer(*inCLRenderDataBuffer[deviceIndex], CL_FALSE,
		0, sizeof(sNebulaRenderData), &renderData, nullptr, nullptr);
	if (!checkErr(writeErr, "enqueueWriteBuffer(renderData)"))
	{
		emit EmitErrorMessage(QObject::tr("Cannot write OpenCL %1").arg(QObject::tr("render data")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	cl_int4 randomSeed4 = {std::rand(), std::rand(), std::rand(), std::rand()};
	err = clKernels.at(deviceIndex)->setArg(argIterator++, randomSeed4); // random seed
	if (!checkErr(err, "kernel->setArg(4, initRandomSeed)"))
	{
		emit EmitErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("random seed")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderNebula::ProcessQueue(qint64 offset)
{
	optimalJob.stepSize = jobSize;

	cl_int err = clQueues.at(0)->enqueueNDRangeKernel(
		*clKernels.at(0), cl::NDRange(offset), cl::NDRange(optimalJob.stepSize), cl::NullRange);
	if (!checkErr(err, "CommandQueue::enqueueNDRangeKernel()"))
	{
		emit EmitErrorMessage(QObject::tr("Cannot enqueue OpenCL rendering jobs. Error %1").arg(err),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = clQueues.at(0)->finish();
	if (!checkErr(err, "CommandQueue::finish() - enqueueNDRangeKernel"))
	{
		emit EmitErrorMessage(
			QObject::tr("Cannot finish rendering nebula"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderNebula::Render(std::shared_ptr<cImage> image, bool *stopRequest)
{
	quint64 width = image->GetWidth();
	quint64 height = image->GetHeight();

	float brighnessMultiplierInit = constantInBuffer->params.nebulaBrighness;

	cProgressText progressText;
	progressText.ResetTimer();

	emit updateProgressAndStatus(
		tr("OpenCl - rendering nebula fractal"), progressText.getText(0.0), 0.0);

	for (quint64 y = 0; y < height; y++)
	{
		for (quint64 x = 0; x < width; x++)
		{
			quint64 i = x + y * width;
			cl_float4 black = {0.0f, 0.0f, 0.0f, 0.0f};

			reinterpret_cast<cl_float4 *>(inputAndOutputBuffers[0][inOutImageBufferIndex].ptr.get())[i] =
				black;

			image->PutPixelZBuffer(x, y, zBufferDefault);
		}
	}

	image->SetFastPreview(true);

	// writing data to queue
	if (!WriteBuffersToQueue()) return false;

	double brightnessMultiplier = brighnessMultiplierInit;

	QElapsedTimer timerForOptimalJobSize;
	jobSize = optimalJob.workGroupSize * optimalJob.jobSizeMultiplier;

	qint64 maxSamples =
		qint64(constantInBuffer->params.nebulaNumberOfSamplesPerPixel) * width * height;

	QElapsedTimer timerForImageRefresh;
	int nextRefreshCounter = 2;
	int refreshCounter = 0;
	double lastProcessingTime = 0.1; // initial guess

	for (qint64 totalSamplesCounter = 0; totalSamplesCounter < maxSamples;
		totalSamplesCounter += jobSize)
	{
		timerForOptimalJobSize.restart();

		bool lastPass = false;

		// assign parameters to kernel
		if (!AssignParametersToKernel(0)) return false;

		nextRefreshCounter--;

		// calculate next job size
		refreshCounter++;
		double optimalTime = min(refreshCounter * 0.1, 1.0);

		jobSize = optimalTime / lastProcessingTime * jobSize;
		if (jobSize > 0.1 * maxSamples) jobSize = 0.1 * maxSamples; // limit to 20% of total samples)

		jobSize /= (optimalJob.workGroupSize * optimalJob.jobSizeMultiplier);
		if (jobSize < 1) jobSize = 1;
		jobSize *= (optimalJob.workGroupSize * optimalJob.jobSizeMultiplier);
		if (jobSize > maxSamples - totalSamplesCounter)
		{
			lastPass = true;
			jobSize = maxSamples - totalSamplesCounter - 1;
		}
		if (jobSize < 1) jobSize = 1;

		// processing queue
		if (!ProcessQueue(totalSamplesCounter)) return false;

		double percentDone = double(totalSamplesCounter) / maxSamples;
		emit updateProgressAndStatus(tr("OpenCl - rendering nebula"),
			progressText.getText(percentDone)
				+ QString(" (%1 mln samples)").arg(totalSamplesCounter / 1000000),
			percentDone);

		if ((*stopRequest || systemData.globalStopRequest) && refreshCounter > 5)
		{
			return false;
		}

		lastProcessingTime = timerForOptimalJobSize.nsecsElapsed() / 1.0e9;

		float brightness;
		if (totalSamplesCounter > 0)
		{
			brightness = (brightnessMultiplier * width * height) / totalSamplesCounter
									 / sqrt(constantInBuffer->params.N);
		}
		else
		{
			brightness = 1.0f;
		}
		// qDebug() << "cOpenClEngineRenderNebula::Render(): brightness = " << brightness;

		if (nextRefreshCounter == 0 || lastPass || refreshCounter < 10)
		{
			if (!ReadBuffersFromQueue(0)) return false;

			timerForImageRefresh.restart();

			double totalBrigtnessSum = 0.0;

			for (quint64 y = 0; y < height; y++)
			{
				for (quint64 x = 0; x < width; x++)
				{
					cl_float4 colorCl = reinterpret_cast<cl_float4 *>(
						inputAndOutputBuffers[0][inOutImageBufferIndex].ptr.get())[x + y * width];

					sRGBFloat color(
						colorCl.s0 * brightness, colorCl.s1 * brightness, colorCl.s2 * brightness);

					if (constantInBuffer->params.nebulaConstantBrighness
							&& totalSamplesCounter > 0) // darken by brightness
					{
						totalBrigtnessSum += color.R + color.G + color.B;
						// check if not a number
						if (color.R != color.R || color.G != color.G || color.B != color.B)
						{
							qDebug() << "color is not a number: " << color.R << color.G << color.B;
							color = sRGBFloat(0.0f, 0.0f, 0.0f);
						}
					}

					image->PutPixelPostImage(x, y, color);
					image->PutPixelAlpha(x, y, 65535);

					float zDepth = colorCl.s3;
					image->PutPixelZBuffer(x, y, zDepth);
				}
			}

			if (constantInBuffer->params.nebulaConstantBrighness) // darken by brightness
			{
				double averageBrighness = totalBrigtnessSum / (width * height * 3.0);
				qDebug() << "totalBrigtnessSum = " << totalBrigtnessSum;
				if (averageBrighness > 0)
				{
					double brighnessChange = (0.015 * brighnessMultiplierInit) / averageBrighness;
					brightnessMultiplier = clamp(brighnessChange * brightnessMultiplier, 1e-6, 1e6);
					qDebug() << "brightnessMultiplier = " << brightnessMultiplier
									 << " averageBrighness = " << averageBrighness
									 << " brighnessChange = " << brighnessChange;
				}
			}

			image->CompileImage();

			signalSmallPartRendered(lastProcessingTime);

			// processing application events
			gApplication->processEvents();

			if (image->IsPreview() && (refreshCounter >= 5 || lastPass))
			{
				if (lastPass) image->SetFastPreview(false);

				WriteLog("image->ConvertTo8bit()", 2);
				image->ConvertTo8bitChar();
				WriteLog("image->UpdatePreview()", 2);
				image->UpdatePreview();
				WriteLog("image->GetImageWidget()->update()", 2);
				emit updateImage();
			}

			double imageRefreshTime = timerForImageRefresh.nsecsElapsed() / 1.0e9;
			nextRefreshCounter = int(10.0 * imageRefreshTime) + 1;
		}
		if (lastPass) break;
	}

	return true;
}

void cOpenClEngineRenderNebula::ReleaseMemory()
{
	constantInBuffer.reset();
	inCLConstBuffer.clear();
	inCLRenderDataBuffer.clear();
	cOpenClEngine::ReleaseMemory();
	dynamicData.reset();
	inBuffer.clear();
	inCLBuffer.clear();
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
	clHeaderFiles.append("nebula_sequence_cl.h");
	clHeaderFiles.append("hybrid_sequence_cl.h");
	clHeaderFiles.append("input_data_structures.h");
}

void cOpenClEngineRenderNebula::CreateListOfIncludes(const QStringList &clHeaderFiles,
	const QString &openclPathSlash, std::shared_ptr<const cParameterContainer> params,
	const QString &openclEnginePath, QByteArray &programEngine, const QByteArray &formulaSwitchCode)
{
	for (int i = 0; i < clHeaderFiles.size(); i++)
	{
		programEngine.append(
			StripNonAscii(LoadUtf8TextFromFile(openclPathSlash + clHeaderFiles.at(i))));
		programEngine.append("\n");
	}
	if (params->Get<bool>("box_folding") || params->Get<bool>("spherical_folding"))
	{
		programEngine.append(LoadUtf8TextFromFile(openclEnginePath + "basic_foldings.cl"));
		programEngine.append("\n");
	}
	// fractal formulas - only actually used
	for (int i = 0; i < listOfUsedFormulas.size(); i++)
	{
		QString formulaName = listOfUsedFormulas.at(i);
		if (formulaName != "" && formulaName != "none")
		{
			if (formulaName.startsWith("custom"))
			{
				programEngine.append(LoadUtf8TextFromFile(
					systemDirectories.GetOpenCLTempFolder() + QDir::separator() + formulaName + ".cl"));
				programEngine.append("\n");
			}
			else
			{
				programEngine.append(
					LoadUtf8TextFromFile(systemDirectories.sharedDir + "formula" + QDir::separator()
															 + "opencl" + QDir::separator() + formulaName + ".cl"));
				programEngine.append("\n");
			}
		}
	}
	// insert dynamically generated formula switch code
	if (!formulaSwitchCode.isEmpty())
	{
		int placeholderPos = programEngine.indexOf("// PLACEHOLDER_FOR_FORMULA_ITER");
		if (placeholderPos >= 0)
		{
			int placeholderLen = strlen("// PLACEHOLDER_FOR_FORMULA_ITER");
			programEngine.replace(placeholderPos, placeholderLen, formulaSwitchCode);
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
