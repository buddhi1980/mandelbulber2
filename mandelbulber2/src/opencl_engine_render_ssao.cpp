/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

void cOpenClEngineRenderSSAO::SetParameters(
	const sParamRender *paramRender, const cRegion<int> &region)
{
	imageRegion = region;
	paramsSSAO.width = region.width;
	paramsSSAO.height = region.height;
	paramsSSAO.fov = paramRender->fov;
	paramsSSAO.quality = paramRender->ambientOcclusionQuality * paramRender->ambientOcclusionQuality;
	if (paramsSSAO.quality < 3) paramsSSAO.quality = 3;
	paramsSSAO.random_mode = paramRender->SSAO_random_mode;
	numberOfPixels = quint64(paramsSSAO.width) * quint64(paramsSSAO.height);
	intensity = paramRender->ambientOcclusion;
	aoColor = paramRender->ambientOcclusionColor;

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
	inputBuffers[0] << sClInputOutputBuffer(sizeof(cl_float), numberOfPixels, "z-buffer");
	inputBuffers[0] << sClInputOutputBuffer(
		sizeof(cl_float), 2 * paramsSSAO.quality, "sine-cosine buffer");
	outputBuffers[0] << sClInputOutputBuffer(sizeof(cl_float), numberOfPixels, "output buffer");
}

bool cOpenClEngineRenderSSAO::AssignParametersToKernelAdditional(uint argIterator, int deviceIndex)
{
	int err = clKernels.at(deviceIndex)->setArg(argIterator++, paramsSSAO); // pixel offset
	if (!checkErr(err, "kernel->setArg(" + QString::number(argIterator) + ", paramsSSAO)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("SSAO params")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}
	return true;
}

bool cOpenClEngineRenderSSAO::ProcessQueue(quint64 pixelsLeft, quint64 pixelIndex)
{
	quint64 limitedWorkgroupSize = optimalJob.workGroupSize;
	quint64 stepSize = optimalJob.stepSize;

	if (optimalJob.stepSize > pixelsLeft)
	{
		quint64 mul = pixelsLeft / optimalJob.workGroupSize;
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

	cl_int err = clQueues.at(0)->enqueueNDRangeKernel(*clKernels.at(0), cl::NDRange(pixelIndex),
		cl::NDRange(stepSize), cl::NDRange(limitedWorkgroupSize));
	if (!checkErr(err, "CommandQueue::enqueueNDRangeKernel()"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot enqueue OpenCL rendering jobs"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = clQueues.at(0)->finish();
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
		quint64 width = imageRegion.width;
		quint64 height = imageRegion.height;

		cProgressText progressText;
		progressText.ResetTimer();

		emit updateProgressAndStatus(tr("OpenCl - rendering SSAO"), progressText.getText(0.0), 0.0);

		QElapsedTimer timer;
		timer.start();

		// copy zBuffer to input buffer

		for (quint64 y = 0; y < height; y++)
		{
			for (quint64 x = 0; x < width; x++)
			{
				quint64 i = x + y * width;
				reinterpret_cast<cl_float *>(inputBuffers[0][zBufferIndex].ptr.data())[i] =
					image->GetPixelZBuffer(x + imageRegion.x1, y + imageRegion.y1);
			}
		}

		for (int i = 0; i < paramsSSAO.quality; i++)
		{
			reinterpret_cast<cl_float *>(inputBuffers[0][sineCosineIndex].ptr.data())[i] =
				sinf(float(i) / paramsSSAO.quality * 2.0f * float(M_PI));
			reinterpret_cast<cl_float *>(
				inputBuffers[0][sineCosineIndex].ptr.data())[i + paramsSSAO.quality] =
				cosf(float(i) / paramsSSAO.quality * 2.0f * float(M_PI));
		}

		// writing data to queue
		if (!WriteBuffersToQueue()) return false;

		for (quint64 pixelIndex = 0; pixelIndex < width * height; pixelIndex += optimalJob.stepSize)
		{
			size_t pixelsLeft = width * height - pixelIndex;
			UpdateOptimalJobStart(pixelsLeft);

			// assign parameters to kernel
			if (!AssignParametersToKernel(0)) return false;

			// processing queue
			if (!ProcessQueue(pixelsLeft, pixelIndex)) return false;

			double percentDone = double(pixelIndex) / (width * height);
			emit updateProgressAndStatus(
				tr("OpenCl - rendering SSAO"), progressText.getText(percentDone), percentDone);
			gApplication->processEvents();

			UpdateOptimalJobEnd();

			if (*stopRequest || systemData.globalStopRequest)
			{
				return false;
			}
		}

		if (!*stopRequest || systemData.globalStopRequest)
		{
			if (!ReadBuffersFromQueue(0)) return false;

			for (quint64 y = 0; y < height; y++)
			{
				for (quint64 x = 0; x < width; x++)
				{
					quint64 xx = x + imageRegion.x1;
					quint64 yy = y + imageRegion.y1;

					cl_float total_ambient =
						reinterpret_cast<cl_float *>(outputBuffers[0][outputIndex].ptr.data())[x + y * width];

					unsigned short opacity16 = image->GetPixelOpacity(xx, yy);
					float opacity = opacity16 / 65535.0f;
					sRGB8 colour = image->GetPixelColor(xx, yy);
					sRGBFloat pixel = image->GetPixelPostImage(xx, yy);
					float shadeFactor = 1.0f / 256.0f * total_ambient * intensity * (1.0f - opacity);
					// qDebug() << total_ambient << shadeFactor << opacity << colour.R;
					pixel.R = pixel.R + colour.R * shadeFactor * aoColor.R;
					pixel.G = pixel.G + colour.G * shadeFactor * aoColor.G;
					pixel.B = pixel.B + colour.B * shadeFactor * aoColor.B;
					image->PutPixelPostImage(xx, yy, pixel);
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
