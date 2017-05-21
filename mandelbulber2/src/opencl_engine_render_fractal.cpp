/*
 * opencl_engine_render_fractal.cpp
 *
 *  Created on: 6 maj 2017
 *      Author: krzysztof
 */

#include "opencl_engine_render_fractal.h"
#include "files.h"
#include "fractparams.hpp"
#include "opencl_hardware.h"
#include "fractal_list.hpp"
#include "cimage.hpp"
#include "nine_fractals.hpp"
#include "fractal.h"
#include "fractal_formulas.hpp"

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

	QByteArray progEngine;
	try
	{
		// passthrough define constants
		progEngine.append("#define USE_OPENCL 1\n");
		progEngine.append("#define NUMBER_OF_FRACTALS " + QString::number(NUMBER_OF_FRACTALS) + "\n");
		progEngine.append("#define M_PI_180 " + QString::number(M_PI_180) + "\n");
		progEngine.append("#define IFS_VECTOR_COUNT " + QString::number(IFS_VECTOR_COUNT) + "\n");
		progEngine.append("#define HYBRID_COUNT " + QString::number(HYBRID_COUNT) + "\n");
		progEngine.append("#define MANDELBOX_FOLDS " + QString::number(MANDELBOX_FOLDS) + "\n");
		progEngine.append("#define Q_UNUSED(x) (void)x;" + QString::number(MANDELBOX_FOLDS) + "\n");

		// definitions of common opencl types
		progEngine.append("#include \"" + systemData.sharedDir + "opencl" + QDir::separator()
											+ "opencl_typedefs.h\"\n");

		// algebra for kernels
		progEngine.append(
			"#include \"" + systemData.sharedDir + "opencl" + QDir::separator() + "opencl_algebra.h\"\n");

		// common parameters
		progEngine.append("#include \"" + systemData.sharedDir + "opencl" + QDir::separator()
											+ "common_params_cl.hpp\"\n");

		// image adjustments
		progEngine.append("#include \"" + systemData.sharedDir + "opencl" + QDir::separator()
											+ "image_adjustments_cl.h\"\n");

		// fractal data structures
		progEngine.append(
			"#include \"" + systemData.sharedDir + "opencl" + QDir::separator() + "fractal_cl.h\"\n");

		// rendering data stractures
		progEngine.append("#include \"" + systemData.sharedDir + "opencl" + QDir::separator()
											+ "fractparams_cl.hpp\"\n");

		// sequence of fractal formulas
		progEngine.append("#include \"" + systemData.sharedDir + "opencl" + QDir::separator()
											+ "fractal_sequence_cl.h\"\n");

		// main data structures
		progEngine.append("#include \"" + systemData.sharedDir + "opencl" + QDir::separator()
											+ "mandelbulber_cl_data.h\"\n");

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
		progEngine.append("#include \"" + systemData.sharedDir + "opencl" + QDir::separator()
											+ "engines" + QDir::separator() + "compute_fractal.cl\"\n");

		// calculate dustance
		progEngine.append("#include \"" + systemData.sharedDir + "opencl" + QDir::separator()
											+ "engines" + QDir::separator() + "calculate_distance.cl\"\n");

		// main engine
		progEngine.append(LoadUtf8TextFromFile(systemData.sharedDir + "opencl" + QDir::separator()
																					 + "engines" + QDir::separator() + "test_engine.cl"));
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

bool cOpenClEngineRenderFractal::Render(cImage *image)
{
	int width = image->GetWidth();
	int height = image->GetHeight();

	size_t sizeOfPixel = sizeof(sClPixel);

	QElapsedTimer timer;
	timer.start();

	for (int pixelIndex = 0; pixelIndex < width * height; pixelIndex += optimalJob.stepSize)
	{
		cl_int err;

		size_t buffSize = optimalJob.stepSize * sizeof(sClPixel);

		// assign parameters to kernel
		err = kernel->setArg(0, *outCL); // output image
		if (!checkErr(err, "kernel->setArg(0, *outCL)")) return false;
		err = kernel->setArg(1, *inCLBuffer); // input data in global memory
		if (!checkErr(err, "kernel->setArg(1, *inCLBuffer)")) return false;
		err = kernel->setArg(2, *inCLConstBuffer); // input data in constant memory (faster than global)
		if (!checkErr(err, "kernel->setArg(2, *inCLConstBuffer)")) return false;
		err = kernel->setArg(3, pixelIndex); // pixel offset
		if (!checkErr(err, "kernel->setArg(3, pixelIndex)")) return false;

		// writing data to queue
		err = queue->enqueueWriteBuffer(*inCLBuffer, CL_TRUE, 0, sizeof(sClInBuff), inBuffer);
		size_t usedGPUdMem = sizeOfPixel * optimalJob.stepSize;
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

		err = queue->enqueueReadBuffer(*outCL, CL_TRUE, 0, buffSize, rgbbuff);
		if (!checkErr(err, "ComamndQueue::enqueueReadBuffer()")) return false;
		err = queue->finish();
		if (!checkErr(err, "ComamndQueue::finish() - ReadBuffer")) return false;

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

	return true;
}

QString cOpenClEngineRenderFractal::toCamelCase(const QString &s)
{
	QStringList parts = s.split('_', QString::SkipEmptyParts);
	for (int i = 1; i < parts.size(); ++i)
		parts[i].replace(0, 1, parts[i][0].toUpper());

	return parts.join("");
}
#endif
