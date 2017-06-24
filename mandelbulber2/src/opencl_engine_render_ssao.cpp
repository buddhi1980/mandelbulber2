/*
 * opencl_engine_render_ssao.cpp
 *
 *  Created on: 21 cze 2017
 *      Author: krzysztof
 */

#include "opencl_engine_render_ssao.h"
#include "cimage.hpp"
#include "files.h"
#include "fractparams.hpp"
#include "opencl_hardware.h"
#include "progress_text.hpp"
#include "global_data.hpp"

cOpenClEngineRenderSSAO::cOpenClEngineRenderSSAO(cOpenClHardware *_hardware)
		: cOpenClEngine(_hardware)
{
#ifdef USE_OPENCL
	paramsSSAO.fov = 0.0f;
	paramsSSAO.height = 0;
	paramsSSAO.width = 0;
	paramsSSAO.height = 0;
	intensity = 0.0;
	numberOfPixels = 0;
	inCLZBuffer = nullptr;
	inZBuffer = nullptr;
	outBuffer = nullptr;
	outCl = nullptr;

	optimalJob.sizeOfPixel = sizeof(cl_float) + sizeof(cl_float);
	optimalJob.optimalProcessingCycle = 0.1;
#endif
}

cOpenClEngineRenderSSAO::~cOpenClEngineRenderSSAO()
{
#ifdef USE_OPENCL
	if (inCLZBuffer) delete inCLZBuffer;
	if (outCl) delete outCl;
	if (inZBuffer) delete[] inZBuffer;
	if (outBuffer) delete[] outBuffer;
#endif
}

#ifdef USE_OPENCL

QString cOpenClEngineRenderSSAO::GetKernelName()
{
	return QString("SSAO");
}

void cOpenClEngineRenderSSAO::SetParameters(const sParamRender *paramRender)
{
	paramsSSAO.width = paramRender->imageWidth;
	paramsSSAO.height = paramRender->imageHeight;
	paramsSSAO.fov = paramRender->fov;
	paramsSSAO.quality = paramRender->ambientOcclusionQuality * paramRender->ambientOcclusionQuality;
	if (paramsSSAO.quality < 3) paramsSSAO.quality = 3;
	numberOfPixels = paramsSSAO.width * paramsSSAO.height;
	intensity = paramRender->ambientOcclusion;
}

bool cOpenClEngineRenderSSAO::LoadSourcesAndCompile(const cParameterContainer *params)
{
	programsLoaded = false;
	readyForRendering = false;
	emit updateProgressAndStatus(
		tr("OpenCl SSAO - initializing"), tr("Compiling sources for SSAO"), 0.0);

	QString openclPath = systemData.sharedDir + "opencl" + QDir::separator();
	QString openclEnginePath = openclPath + "engines" + QDir::separator();

	QByteArray programEngine;
	// pass through define constants
	programEngine.append("#define USE_OPENCL 1\n");

	QStringList clHeaderFiles;
	clHeaderFiles.append("opencl_typedefs.h"); // definitions of common opencl types
	clHeaderFiles.append("ssao_cl.h");				 // main data structures

	for (int i = 0; i < clHeaderFiles.size(); i++)
	{
		programEngine.append("#include \"" + openclPath + clHeaderFiles.at(i) + "\"\n");
	}

	QString engineFileName = "ssao.cl";
	QString engineFullFileName = openclEnginePath + engineFileName;
	programEngine.append(LoadUtf8TextFromFile(engineFullFileName));

	// building OpenCl kernel
	QString errorString;

	QElapsedTimer timer;
	timer.start();
	if (Build(programEngine, &errorString))
	{
		programsLoaded = true;
	}
	else
	{
		programsLoaded = false;
		WriteLog(errorString, 0);
	}
	qDebug() << "Opencl SSAO build time [s]" << timer.nsecsElapsed() / 1.0e9;

	return programsLoaded;
}

bool cOpenClEngineRenderSSAO::PreAllocateBuffers(const cParameterContainer *params)
{
	Q_UNUSED(params);

	cl_int err;

	if (hardware->ContextCreated())
	{

		// output buffer
		size_t buffSize = numberOfPixels * sizeof(cl_float);
		if (outBuffer) delete[] outBuffer;
		outBuffer = new cl_float[numberOfPixels];

		if (outCl) delete outCl;
		outCl = new cl::Buffer(
			*hardware->getContext(), CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, outBuffer, &err);
		if (!checkErr(
					err, "*context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, outBuffer, &err"))
		{
			emit showErrorMessage(
				QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("output buffer")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}

		// input z-buffer
		if (inZBuffer) delete[] inZBuffer;
		inZBuffer = new cl_float[numberOfPixels];

		if (inCLZBuffer) delete inCLZBuffer;
		inCLZBuffer = new cl::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
			numberOfPixels * sizeof(cl_float), inZBuffer, &err);
		if (!checkErr(err,
					"Buffer::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, "
					"sizeof(sClInBuff), inZBuffer, &err)"))
		{
			emit showErrorMessage(
				QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for zBuffer")),
				cErrorMessage::errorMessage, nullptr);
			return false;
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

bool cOpenClEngineRenderSSAO::AssignParametersToKernel()
{
	int err = kernel->setArg(0, *inCLZBuffer); // input data in global memory
	if (!checkErr(err, "kernel->setArg(0, *inCLZBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("Z-Buffer data")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = kernel->setArg(1, *outCl); // output buffer
	if (!checkErr(err, "kernel->setArg(1, *outCl)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("output data")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = kernel->setArg(2, paramsSSAO); // pixel offset
	if (!checkErr(err, "kernel->setArg(2, pixelIndex)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("SSAO params")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderSSAO::WriteBuffersToQueue()
{
	cl_int err = queue->enqueueWriteBuffer(
		*inCLZBuffer, CL_TRUE, 0, numberOfPixels * sizeof(cl_float), inZBuffer);

	if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(inCLBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot enqueue writing OpenCL %1").arg(QObject::tr("input buffers")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = queue->finish();
	if (!checkErr(err, "CommandQueue::finish() - inCLBuffer"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot finish writing OpenCL %1").arg(QObject::tr("input buffers")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderSSAO::ProcessQueue(int pixelsLeft, int pixelIndex)
{
	size_t limitedWorkgroupSize = optimalJob.workGroupSize;
	int stepSize = optimalJob.stepSize;

	if (optimalJob.stepSize > pixelsLeft)
	{
		int mul = pixelsLeft / optimalJob.workGroupSize;
		if (mul > 0)
		{
			stepSize = mul * optimalJob.workGroupSize;
		}
		else
		{
			// in this case will be limited workGroupSize
			stepSize = pixelsLeft;
			limitedWorkgroupSize = pixelsLeft;
		}
	}

	cl_int err = queue->enqueueNDRangeKernel(
		*kernel, cl::NDRange(pixelIndex), cl::NDRange(stepSize), cl::NDRange(limitedWorkgroupSize));
	if (!checkErr(err, "CommandQueue::enqueueNDRangeKernel()"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot enqueue OpenCL rendering jobs"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderSSAO::ReadBuffersFromQueue()
{
	size_t buffSize = numberOfPixels * sizeof(cl_float);

	cl_int err = queue->enqueueReadBuffer(*outCl, CL_TRUE, 0, buffSize, outBuffer);
	if (!checkErr(err, "CommandQueue::enqueueReadBuffer()"))
	{
		emit showErrorMessage(QObject::tr("Cannot enqueue reading OpenCL output buffers"),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = queue->finish();
	if (!checkErr(err, "CommandQueue::finish() - ReadBuffer"))
	{
		emit showErrorMessage(QObject::tr("Cannot finish reading OpenCL output buffers"),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderSSAO::Render(cImage *image, bool *stopRequest)
{
	if (programsLoaded)
	{
		// The image resolution determines the total amount of work
		int width = image->GetWidth();
		int height = image->GetHeight();

		cProgressText progressText;
		progressText.ResetTimer();

		emit updateProgressAndStatus(tr("OpenCl - rendering SSAO"), progressText.getText(0.0), 0.0);

		QElapsedTimer timer;
		timer.start();

		QList<int> lastRenderedLines;

		// copy zBuffer to input buffer
		for (int i = 0; i < numberOfPixels; i++)
		{
			inZBuffer[i] = image->GetZBufferPtr()[i];
		}

		// writing data to queue
		if (!WriteBuffersToQueue()) return false;

		// TODO:
		// insert device for loop here
		// requires initialization for all opencl devices
		// requires optimalJob for all opencl devices
		for (int pixelIndex = 0; pixelIndex < width * height; pixelIndex += optimalJob.stepSize)
		{
			size_t pixelsLeft = width * height - pixelIndex;
			UpdateOptimalJobStart(pixelsLeft);

			// assign parameters to kernel
			if (!AssignParametersToKernel()) return false;

			// processing queue
			if (!ProcessQueue(pixelsLeft, pixelIndex)) return false;

			UpdateOptimalJobEnd();

			double percentDone = double(pixelIndex) / (width * height);
			emit updateProgressAndStatus(
				tr("OpenCl - rendering image"), progressText.getText(percentDone), percentDone);
			gApplication->processEvents();
			if (*stopRequest)
			{
				*stopRequest = false;
				break;
			}
		}

		if (!*stopRequest)
		{
			if (!ReadBuffersFromQueue()) return false;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					cl_float total_ambient = outBuffer[x + y * width];
					unsigned short opacity16 = image->GetPixelOpacity(x, y);
					float opacity = opacity16 / 65535.0f;
					sRGB8 colour = image->GetPixelColor(x, y);
					sRGBFloat pixel = image->GetPixelPostImage(x, y);
					float shadeFactor = 1.0f / 256.0f * total_ambient * intensity * (1.0f - opacity);
					// qDebug() << total_ambient << shadeFactor << opacity << colour.R;
					pixel.R = pixel.R + colour.R * shadeFactor;
					pixel.G = pixel.G + colour.G * shadeFactor;
					pixel.B = pixel.B + colour.B * shadeFactor;
					image->PutPixelPostImage(x, y, pixel);
				}
			}

			qDebug() << "GPU jobs finished";
			qDebug() << "OpenCl Rendering time [s]" << timer.nsecsElapsed() / 1.0e9;

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
		}

		emit updateProgressAndStatus(tr("OpenCl - rendering finished"), progressText.getText(1.0), 1.0);

		return true;
	}
	else
	{
		return false;
	}
}

#endif // USE_OPEMCL
