/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include "parameters.hpp"
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
	paramsSSAO.random_mode = false;
	intensity = 0.0;
	numberOfPixels = 0;
	optimalJob.sizeOfPixel = 0; // memory usage doens't depend on job size
	optimalJob.optimalProcessingCycle = 0.5;
#endif
}

cOpenClEngineRenderSSAO::~cOpenClEngineRenderSSAO()
{
#ifdef USE_OPENCL
	ReleaseMemory();
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
	paramsSSAO.random_mode = paramRender->SSAO_random_mode;
	numberOfPixels = paramsSSAO.width * paramsSSAO.height;
	intensity = paramRender->ambientOcclusion;

	definesCollector.clear();
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
	clHeaderFiles.append("opencl_algebra.h");	// definitions of common math functions
	for (int i = 0; i < clHeaderFiles.size(); i++)
	{
		programEngine.append("#include \"" + openclPath + clHeaderFiles.at(i) + "\"\n");
	}

	QString engineFileName = "ssao.cl";
	QString engineFullFileName = openclEnginePath + engineFileName;
	programEngine.append(LoadUtf8TextFromFile(engineFullFileName));

	SetUseFastRelaxedMath(params->Get<bool>("opencl_use_fast_relaxed_math"));

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

void cOpenClEngineRenderSSAO::RegisterInputOutputBuffers(const cParameterContainer *params)
{
	Q_UNUSED(params);
	inputBuffers << sClInputOutputBuffer(sizeof(cl_float), numberOfPixels, "z-buffer");
	inputBuffers << sClInputOutputBuffer(
		sizeof(cl_float), 2 * paramsSSAO.quality, "sine-cosine buffer");
	outputBuffers << sClInputOutputBuffer(sizeof(cl_float), numberOfPixels, "output buffer");
}

bool cOpenClEngineRenderSSAO::AssignParametersToKernelAdditional(int argIterator)
{
	int err = kernel->setArg(argIterator++, paramsSSAO); // pixel offset
	if (!checkErr(err, "kernel->setArg(" + QString::number(argIterator) + ", paramsSSAO)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("SSAO params")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}
	return true;
}

bool cOpenClEngineRenderSSAO::ProcessQueue(qint64 pixelsLeft, qint64 pixelIndex)
{
	qint64 limitedWorkgroupSize = optimalJob.workGroupSize;
	qint64 stepSize = optimalJob.stepSize;

	if (optimalJob.stepSize > pixelsLeft)
	{
		size_t mul = pixelsLeft / optimalJob.workGroupSize;
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

		// copy zBuffer to input buffer
		for (int i = 0; i < numberOfPixels; i++)
		{
			((cl_float *)inputBuffers[zBufferIndex].ptr.data())[i] = image->GetZBufferPtr()[i];
		}
		for (int i = 0; i < paramsSSAO.quality; i++)
		{
			((cl_float *)inputBuffers[sineCosineIndex].ptr.data())[i] =
				sin(float(i) / paramsSSAO.quality * 2.0 * M_PI);
			((cl_float *)inputBuffers[sineCosineIndex].ptr.data())[i + paramsSSAO.quality] =
				cos(float(i) / paramsSSAO.quality * 2.0 * M_PI);
		}

		// writing data to queue
		if (!WriteBuffersToQueue()) return false;

		// TODO:
		// insert device for loop here
		// requires initialization for all opencl devices
		// requires optimalJob for all opencl devices
		for (qint64 pixelIndex = 0; pixelIndex < qint64(width) * qint64(height);
				 pixelIndex += optimalJob.stepSize)
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
					cl_float total_ambient =
						((cl_float *)outputBuffers[outputIndex].ptr.data())[x + y * width];
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
				emit updateImage();
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
	return numberOfPixels * sizeof(cl_float) + paramsSSAO.quality * 2 * sizeof(cl_float);
}

#endif // USE_OPENCL
