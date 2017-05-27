/*
 * opencl_engine_render_fractal.cpp
 *
 *  Created on: 6 maj 2017
 *      Author: krzysztof
 */

#include "cimage.hpp"
#include "files.h"
#include "fractal_formulas.hpp"
#include "fractal_list.hpp"
#include "fractal.h"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "nine_fractals.hpp"
#include "opencl_engine_render_fractal.h"

#include "camera_target.hpp"
#include "opencl_hardware.h"
#include "parameters.hpp"
#include "progress_text.hpp"

#ifdef USE_OPENCL
#include "../opencl/fractal_cl.h"
#include "../opencl/fractparams_cl.hpp"
#endif

cOpenClEngineRenderFractal::cOpenClEngineRenderFractal(cOpenClHardware *_hardware)
		: cOpenClEngine(_hardware)
{
#ifdef USE_OPENCL
	constantInBuffer = nullptr;
	inCLConstBuffer = nullptr;

	inBuffer = nullptr;
	inCLBuffer = nullptr;

	rgbbuff = nullptr;
	outCL = nullptr;

	optimalJob.sizeOfPixel = sizeof(sClPixel);

#endif
}

cOpenClEngineRenderFractal::~cOpenClEngineRenderFractal()
{
#ifdef USE_OPENCL
	if (constantInBuffer) delete constantInBuffer;
	if (inCLConstBuffer) delete inCLConstBuffer;
	if (inBuffer) delete inBuffer;
	if (inCLBuffer) delete inCLBuffer;
	if (rgbbuff) delete rgbbuff;
	if (outCL) delete outCL;
#endif
}

#ifdef USE_OPENCL
void cOpenClEngineRenderFractal::LoadSourcesAndCompile(const cParameterContainer *params)
{
	programsLoaded = false;
	readyForRendering = false;

	emit updateProgressAndStatus(tr("OpenCl - initializing"), tr("Compiling sources"), 0.0);

	QByteArray progEngine;
	try
	{
		QString openclPath = systemData.sharedDir + "opencl" + QDir::separator();
		QString openclEnginePath = openclPath + "engines" + QDir::separator();

		// passthrough define constants
		progEngine.append("#define USE_OPENCL 1\n");
		progEngine.append("#define NUMBER_OF_FRACTALS " + QString::number(NUMBER_OF_FRACTALS) + "\n");

		progEngine.append("#define SQRT_1_3 " + QString::number(SQRT_1_3) + "\n");
		progEngine.append("#define SQRT_1_2 " + QString::number(SQRT_1_2) + "\n");
		progEngine.append("#define SQRT_2_3 " + QString::number(SQRT_2_3) + "\n");
		progEngine.append("#define SQRT_3_2 " + QString::number(SQRT_3_2) + "\n");
		progEngine.append("#define SQRT_3_4 " + QString::number(SQRT_3_4) + "\n");
		progEngine.append("#define SQRT_3_4d2 " + QString::number(SQRT_3_4d2) + "\n");
		progEngine.append("#define SQRT_3 " + QString::number(SQRT_3) + "\n");
		progEngine.append("#define FRAC_1_3 " + QString::number(FRAC_1_3) + "\n");
		progEngine.append("#define M_PI_180 " + QString::number(M_PI_180) + "\n");
		progEngine.append("#define M_PI_8 " + QString::number(M_PI_8) + "\n");

		progEngine.append("#define IFS_VECTOR_COUNT " + QString::number(IFS_VECTOR_COUNT) + "\n");
		progEngine.append("#define HYBRID_COUNT " + QString::number(HYBRID_COUNT) + "\n");
		progEngine.append("#define MANDELBOX_FOLDS " + QString::number(MANDELBOX_FOLDS) + "\n");
		progEngine.append("#define Q_UNUSED(x) (void)x;\n");

		QStringList clHeaderFiles;
		clHeaderFiles.append("opencl_typedefs.h");			// definitions of common opencl types
		clHeaderFiles.append("opencl_algebra.h");				// algebra for kernels
		clHeaderFiles.append("common_params_cl.hpp");		// common parameters
		clHeaderFiles.append("image_adjustments_cl.h"); // image adjustments
		clHeaderFiles.append("fractal_cl.h");						// fractal data structures
		clHeaderFiles.append("fractparams_cl.hpp");			// rendering data structures
		clHeaderFiles.append("fractal_sequence_cl.h");	// sequence of fractal formulas
		clHeaderFiles.append("mandelbulber_cl_data.h"); // main data structures

		for (int i = 0; i < clHeaderFiles.size(); i++)
		{
			progEngine.append("#include \"" + openclPath + clHeaderFiles.at(i) + "\"\n");
		}

		// fractal formulas - only actually used
		for (int i = 0; i < listOfUsedFormulas.size(); i++)
		{
			QString formulaName = listOfUsedFormulas.at(i);
			if (formulaName != "")
			{
				progEngine.append("#include \"" + systemData.sharedDir + "formula" + QDir::separator()
													+ "opencl" + QDir::separator() + formulaName + ".cl\"\n");
			}
		}

		// compute fractal
		progEngine.append("#include \"" + openclEnginePath + "compute_fractal.cl\"\n");

		// calculate dustance
		progEngine.append("#include \"" + openclEnginePath + "calculate_distance.cl\"\n");

		// main engine
		QString engineFileName;
		switch (enumClRenderEngineMode(params->Get<int>("gpu_mode")))
		{
			case clRenderEngineTypeFast: engineFileName = "fast_engine.cl"; break;
			case clRenderEngineTypeNormal: engineFileName = "normal_engine.cl"; break;
			case clRenderEngineTypeFull: engineFileName = "full_engine.cl"; break;
		}
		QString engineFullFileName = openclEnginePath + engineFileName;
		progEngine.append(LoadUtf8TextFromFile(engineFullFileName));
	}
	catch (const QString &ex)
	{
		qCritical() << "OpenCl program error: " << ex;
		return;
	}

	// collecting all parts of program
	cl::Program::Sources sources;
	sources.push_back(std::make_pair(progEngine.constData(), size_t(progEngine.length())));

	// creating cl::Program
	cl_int err;

	if (program) delete program;
	program = new cl::Program(*hardware->getContext(), sources, &err);
	if (checkErr(err, "cl::Program()"))
	{
		// building OpenCl kernel

		QString errorString;

		QElapsedTimer timer;
		timer.start();
		if (Build(program, &errorString))
		{
			programsLoaded = true;
		}
		else
		{
			programsLoaded = false;
			WriteLog(errorString, 0);
		}
		qDebug() << "Opencl build time [s]" << timer.nsecsElapsed() / 1.0e9;
	}
}

void cOpenClEngineRenderFractal::SetParameters(
	const cParameterContainer *paramContainer, const cFractalContainer *fractalContainer)
{
	if (constantInBuffer) delete constantInBuffer;
	constantInBuffer = new sClInConstants;

	definesCollector.clear();

	// TODO Write function to copy parameters from sParamRender to sClParamRender
	// Would be good to write php script for it
	sParamRender *paramRender = new sParamRender(paramContainer);
	cNineFractals *fractals = new cNineFractals(fractalContainer, paramContainer);

	// update camera rotation data (needed for simplified calculation in opencl kernel)
	cCameraTarget cameraTarget(paramRender->camera, paramRender->target, paramRender->topVector);
	paramRender->viewAngle = cameraTarget.GetRotation() * 180.0 / M_PI;

	// temporary code to copy general parameters
	constantInBuffer->params = clCopySParamRenderCl(*paramRender);

	// TODO
	constantInBuffer->params.viewAngle = toClFloat3(paramRender->viewAngle * M_PI / 180.0);

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		constantInBuffer->fractal[i] = clCopySFractalCl(*fractals->GetFractal(i));
	}

	fractals->CopyToOpenclData(&constantInBuffer->sequence);

	// define distance estimation method
	fractal::enumDEType deType = fractals->GetDEType(0);
	fractal::enumDEFunctionType deFunctionType = fractals->GetDEFunctionType(0);

	if (deType == fractal::analyticDEType)
	{
		definesCollector += " -DANALYTIC_DE";
		switch (deFunctionType)
		{
			case fractal::linearDEFunction: definesCollector += " -DANALYTIC_LINEAR_DE"; break;
			case fractal::logarithmicDEFunction: definesCollector += " -DANALYTIC_LOG_DE"; break;
			case fractal::pseudoKleinianDEFunction:
				definesCollector += " -DANALYTIC_PSEUDO_KLEINIAN_DE";
				break;
			default: break;
		}
	}
	else if (deType == fractal::deltaDEType)
	{
		definesCollector += " -DDELTA_DE";
		switch (deFunctionType)
		{
			case fractal::linearDEFunction: definesCollector += " -DDELTA_LINEAR_DE"; break;
			case fractal::logarithmicDEFunction: definesCollector += " -DDELTA_LOG_DE"; break;
			case fractal::pseudoKleinianDEFunction:
				definesCollector += " -DDELTA_PSEUDO_KLEINIAN_DE";
				break;
			default: break;
		}
	}

	if (paramRender->limitsEnabled) definesCollector += " -DLIMITS_ENABLED";

	listOfUsedFormulas.clear();

	// creating list of used formuals
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractal::enumFractalFormula fractalFormula = fractals->GetFractal(i)->formula;
		int listIndex = cNineFractals::GetIndexOnFractalList(fractalFormula);
		QString formulaName = fractalList.at(listIndex).internalName;
		listOfUsedFormulas.append(formulaName);
	}

	// adding #defines to the list
	for (int i = 0; i < listOfUsedFormulas.size(); i++)
	{
		QString internalID = toCamelCase(listOfUsedFormulas.at(i));
		if (internalID != "")
		{
			QString functionName = internalID.left(1).toUpper() + internalID.mid(1) + "Iteration";
			definesCollector += " -DFORMULA_ITER_" + QString::number(i) + "=" + functionName;
		}
		else
		{
			QString functionName = "DummyIteration";
			definesCollector += " -DFORMULA_ITER_" + QString::number(i) + "=" + functionName;
		}
	}

	listOfUsedFormulas = listOfUsedFormulas.toSet().toList(); // eliminate duplicates

	qDebug() << "Constant buffer size" << sizeof(sClInConstants);

	delete paramRender;
	delete fractals;
}

bool cOpenClEngineRenderFractal::PreAllocateBuffers(const cParameterContainer *params)
{
	cl_int err;

	if (inCLConstBuffer) delete inCLConstBuffer;
	inCLConstBuffer = new cl::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(sClInConstants), constantInBuffer, &err);
	if (!checkErr(err,
				"cl::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, "
				"sizeof(sClInConstants), constantInBuffer, &err)"))
		return false;

	// this buffer will be used for color palettes, lights, etc...
	if (inBuffer) delete inBuffer;
	inBuffer = new sClInBuff;
	if (inCLBuffer) delete inCLBuffer;
	inCLBuffer = new cl::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(sClInBuff), inBuffer, &err);
	if (!checkErr(err,
				"Buffer::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, "
				"sizeof(sClInBuff), inBuffer, &err)"))
		return false;

	size_t buffSize = optimalJob.stepSize * sizeof(sClPixel);
	if (rgbbuff) delete rgbbuff;
	rgbbuff = new sClPixel[buffSize];

	if (outCL) delete outCL;
	outCL = new cl::Buffer(
		*hardware->getContext(), CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, rgbbuff, &err);
	if (!checkErr(err, "*context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, rgbbuff, &err"))
		return false;

	return true;
}

bool cOpenClEngineRenderFractal::ReAllocateImageBuffers()
{
	cl_int err;

	size_t buffSize = optimalJob.stepSize * sizeof(sClPixel);
	if (rgbbuff) delete rgbbuff;
	rgbbuff = new sClPixel[buffSize];

	if (outCL) delete outCL;
	outCL = new cl::Buffer(
		*hardware->getContext(), CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, rgbbuff, &err);
	if (!checkErr(err, "*context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, rgbbuff, &err"))
		return false;
}

bool cOpenClEngineRenderFractal::Render(cImage *image)
{
	if (programsLoaded)
	{
		int width = image->GetWidth();
		int height = image->GetHeight();

		cProgressText progressText;
		progressText.ResetTimer();

		emit updateProgressAndStatus(tr("OpenCl - rendering image"), progressText.getText(0.0), 0.0);

		QElapsedTimer timer;
		timer.start();

		QList<int> lastRenderedLines;

		for (int pixelIndex = 0; pixelIndex < width * height; pixelIndex += optimalJob.stepSize)
		{
			cl_int err;

			size_t pixelsLeft = width * height - pixelIndex;
			UpdateOptimalJobStart(pixelsLeft);

			size_t buffSize = optimalJob.stepSize * sizeof(sClPixel);

			ReAllocateImageBuffers();

			// assign parameters to kernel
			err = kernel->setArg(0, *outCL); // output image
			if (!checkErr(err, "kernel->setArg(0, *outCL)")) return false;
			err = kernel->setArg(1, *inCLBuffer); // input data in global memory
			if (!checkErr(err, "kernel->setArg(1, *inCLBuffer)")) return false;
			err =
				kernel->setArg(2, *inCLConstBuffer); // input data in constant memory (faster than global)
			if (!checkErr(err, "kernel->setArg(2, *inCLConstBuffer)")) return false;
			err = kernel->setArg(3, pixelIndex); // pixel offset
			if (!checkErr(err, "kernel->setArg(3, pixelIndex)")) return false;

			// writing data to queue
			err = queue->enqueueWriteBuffer(*inCLBuffer, CL_TRUE, 0, sizeof(sClInBuff), inBuffer);
			size_t usedGPUdMem = optimalJob.sizeOfPixel * optimalJob.stepSize;

			// qDebug() << "Used GPU mem (KB): " << usedGPUdMem / 1024;

			if (!checkErr(err, "ComamndQueue::enqueueWriteBuffer(inCLBuffer)")) return false;

			err = queue->finish();
			if (!checkErr(err, "ComamndQueue::finish() - inCLBuffer")) return false;

			err = queue->enqueueWriteBuffer(
				*inCLConstBuffer, CL_TRUE, 0, sizeof(sClInConstants), constantInBuffer);
			if (!checkErr(err, "ComamndQueue::enqueueWriteBuffer(inCLConstBuffer)")) return false;

			err = queue->finish();
			if (!checkErr(err, "ComamndQueue::finish() - inCLConstBuffer")) return false;

			// processing queue
			err = queue->enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(optimalJob.stepSize),
				cl::NDRange(optimalJob.workGroupSize));
			if (!checkErr(err, "ComamndQueue::enqueueNDRangeKernel()")) return false;

			// update image when OpenCl kernel is working
			if (lastRenderedLines.size() > 0)
			{
				QElapsedTimer timerImageRefresh;
				timerImageRefresh.start();
				image->NullPostEffect(&lastRenderedLines);
				image->CompileImage(&lastRenderedLines);
				image->ConvertTo8bit();
				image->UpdatePreview(&lastRenderedLines);
				image->GetImageWidget()->update();
				lastRenderedLines.clear();
				optimalJob.optimalProcessingCycle = 2.0 * timerImageRefresh.elapsed() / 1000.0;
				if (optimalJob.optimalProcessingCycle < 0.1) optimalJob.optimalProcessingCycle = 0.1;
			}

			err = queue->enqueueReadBuffer(*outCL, CL_TRUE, 0, buffSize, rgbbuff);
			if (!checkErr(err, "ComamndQueue::enqueueReadBuffer()")) return false;
			err = queue->finish();
			if (!checkErr(err, "ComamndQueue::finish() - ReadBuffer")) return false;

			UpdateOptimalJobEnd();

			for (unsigned int i = 0; i < optimalJob.stepSize; i++)
			{
				unsigned int a = pixelIndex + i;
				sClPixel pixelCl = rgbbuff[i];
				sRGBFloat pixel = {pixelCl.R, pixelCl.G, pixelCl.B};
				sRGB8 color = {pixelCl.colR, pixelCl.colG, pixelCl.colB};
				unsigned short opacity = pixelCl.opacity;
				unsigned short alpha = pixelCl.alpha;
				int x = a % width;
				int y = a / width;

				image->PutPixelImage(x, y, pixel);
				image->PutPixelZBuffer(x, y, rgbbuff[i].zBuffer);
				image->PutPixelColour(x, y, color);
				image->PutPixelOpacity(x, y, opacity);
				image->PutPixelAlpha(x, y, alpha);
			}

			for (unsigned int i = 0; i < optimalJob.stepSize; i += width)
			{
				unsigned int a = pixelIndex + i;
				int y = a / width;
				lastRenderedLines.append(y);
			}

			double percentDone = double(pixelIndex) / (width * height);
			emit updateProgressAndStatus(
				tr("OpenCl - rendering image"), progressText.getText(percentDone), percentDone);
			gApplication->processEvents();
		}

		qDebug() << "GPU jobs finished";
		qDebug() << "OpenCl Rendering time [s]" << timer.nsecsElapsed() / 1.0e9;

		// refresh image at end
		image->NullPostEffect();

		WriteLog("image->CompileImage()", 2);
		image->CompileImage();

		if (image->IsPreview())
		{
			WriteLog("image->ConvertTo8bit()", 2);
			image->ConvertTo8bit();
			WriteLog("image->UpdatePreview()", 2);
			image->UpdatePreview();
			WriteLog("image->GetImageWidget()->update()", 2);
			image->GetImageWidget()->update();
		}

		emit updateProgressAndStatus(tr("OpenCl - rendering finished"), progressText.getText(1.0), 1.0);

		return true;
	}
	else
	{
		return false;
	}
}

QString cOpenClEngineRenderFractal::toCamelCase(const QString &s)
{
	QStringList upperCaseLookup({"Vs", "Kifs", "De", "Xyz", "Cxyz", "Vcl", "Chs"});
	QStringList parts = s.split('_', QString::SkipEmptyParts);
	for (int i = 1; i < parts.size(); ++i)
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
