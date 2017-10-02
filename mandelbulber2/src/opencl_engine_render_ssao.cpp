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
 * c++ - opencl connector for the SSAO OpenCL renderer
 */

#include "opencl_engine_render_ssao.h"

#include "cimage.hpp"
#include "files.h"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "opencl_hardware.h"
#include "progress_text.hpp"

cOpenClEngineRenderSSAO::cOpenClEngineRenderSSAO(cOpenClHardware *_hardware)
		: cOpenClEngine(_hardware)
{
#ifdef USE_OPENCL
	paramsSSAO.fov = 0.0f;
	paramsSSAO.height = 0;
	paramsSSAO.width = 0;
	paramsSSAO.height = 0;
	paramsSSAO.quality = 0;
	intensity = 0.0;
	numberOfPixels = 0;
	inCLZBuffer = nullptr;
	inZBuffer = nullptr;
	outBuffer = nullptr;
	outCl = nullptr;

	optimalJob.sizeOfPixel = 0; // memory usage doens't depend on job size
	optimalJob.optimalProcessingCycle = 0.5;
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

void cOpenClEngineRenderSSAO::ReleaseMemory()
{
	if (inCLZBuffer) delete inCLZBuffer;
	inCLZBuffer = nullptr;
	if (outCl) delete outCl;
	outCl = nullptr;
	if (inZBuffer) delete[] inZBuffer;
	inZBuffer = nullptr;
	if (outBuffer) delete[] outBuffer;
	outBuffer = nullptr;
}

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
	WriteLogDouble(
		"cOpenClEngineRenderSSAO: Opencl DOF build time [s]", timer.nsecsElapsed() / 1.0e9, 2);

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
	optimalJob.stepSize = stepSize;

	cl_int err = queue->enqueueNDRangeKernel(
		*kernel, cl::NDRange(pixelIndex), cl::NDRange(stepSize), cl::NDRange(limitedWorkgroupSize));
	if (!checkErr(err, "CommandQueue::enqueueNDRangeKernel()"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot enqueue OpenCL rendering jobs"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = queue->finish();
	if (!checkErr(err, "CommandQueue::finish() - enqueueNDRangeKernel"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot finish rendering SSAO"), cErrorMessage::errorMessage, nullptr);
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

			double percentDone = double(pixelIndex) / (width * height);
			emit updateProgressAndStatus(
				tr("OpenCl - rendering SSAO"), progressText.getText(percentDone), percentDone);
			gApplication->processEvents();

			UpdateOptimalJobEnd();

			if (*stopRequest)
			{
				return false;
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

			WriteLogDouble(
				"cOpenClEngineRenderSSAO: OpenCL Rendering time [s]", timer.nsecsElapsed() / 1.0e9, 2);

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

		emit updateProgressAndStatus(
			tr("OpenCl - rendering SSAO finished"), progressText.getText(1.0), 1.0);

		return true;
	}
	else
	{
		return false;
	}
}

size_t cOpenClEngineRenderSSAO::CalcNeededMemory()
{
	return numberOfPixels * sizeof(cl_float);
}

#endif // USE_OPEMCL
