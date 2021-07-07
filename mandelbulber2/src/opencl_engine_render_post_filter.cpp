/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * c++ - render post filers with OpenCL
 */

#include "opencl_engine_render_post_filter.h"

#include "cimage.hpp"
#include "files.h"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "opencl_hardware.h"
#include "parameters.hpp"
#include "progress_text.hpp"
#include "system_data.hpp"
#include "system_directories.hpp"
#include "write_log.hpp"

#ifdef USE_OPENCL
#include "opencl/opencl_algebra.h"
#endif

cOpenClEngineRenderPostFilter::cOpenClEngineRenderPostFilter(cOpenClHardware *_hardware)
		: cOpenClEngine(_hardware)
{
#ifdef USE_OPENCL
	paramsHDRBlur.radius = 0.0f;
	paramsHDRBlur.intensity = 0.0f;
	paramsHDRBlur.width = 0;
	paramsHDRBlur.height = 0;

	paramsChromaticAberration.width = 0;
	paramsChromaticAberration.height = 0;
	paramsChromaticAberration.blurRadius = 0.0f;
	paramsChromaticAberration.aberrationIntensity = 0.0f;
	paramsChromaticAberration.reverse = false;

	numberOfPixels = 0;
	optimalJob.sizeOfPixel = 0; // memory usage doens't depend on job size
	optimalJob.optimalProcessingCycle = 0.5;
	effectType = hdrBlur;
#endif
}

cOpenClEngineRenderPostFilter::~cOpenClEngineRenderPostFilter()
{
#ifdef USE_OPENCL
	ReleaseMemory();
#endif
}

#ifdef USE_OPENCL

QString cOpenClEngineRenderPostFilter::GetKernelName()
{
	return QString("PostFilter");
}

void cOpenClEngineRenderPostFilter::SetParameters(
	const sParamRender *paramRender, const cRegion<int> &region, enumPostEffectType _effectType)
{
	imageRegion = region;
	paramsHDRBlur.width = region.width;
	paramsHDRBlur.height = region.height;
	paramsHDRBlur.radius = paramRender->hdrBlurRadius;
	paramsHDRBlur.intensity = paramRender->hdrBlurIntensity;

	paramsChromaticAberration.width = region.width;
	paramsChromaticAberration.height = region.height;
	paramsChromaticAberration.blurRadius = paramRender->postChromaticAberrationRadius;
	paramsChromaticAberration.aberrationIntensity = paramRender->postChromaticAberrationIntensity;
	paramsChromaticAberration.reverse = paramRender->postChromaticAberrationReverse;

	numberOfPixels = quint64(region.width) * quint64(region.height);
	definesCollector.clear();
	effectType = _effectType;

	switch (effectType)
	{
		case hdrBlur: effectName = "HDR Blur"; break;
		case chromaticAberration: effectName = "Chromatic Aberration"; break;
	}
}

bool cOpenClEngineRenderPostFilter::LoadSourcesAndCompile(
	std::shared_ptr<const cParameterContainer> params, QString *compilerErrorOutput)
{
	programsLoaded = false;
	readyForRendering = false;
	emit updateProgressAndStatus(tr("OpenCl %1 - initializing").arg(effectName),
		tr("Compiling sources for %1").arg(effectName), 0.0);

	QString openclPath = systemDirectories.sharedDir + "opencl" + QDir::separator();
	QString openclEnginePath = openclPath + "engines" + QDir::separator();

	QByteArray programEngine;
	// pass through define constants
	programEngine.append("#define USE_OPENCL 1\n");

	QStringList clHeaderFiles;
	clHeaderFiles.append("opencl_typedefs.h"); // definitions of common opencl types

	switch (effectType)
	{
		case hdrBlur: clHeaderFiles.append("hdr_blur_cl.h"); break; // main data structures
		case chromaticAberration: clHeaderFiles.append("chromatic_aberration_cl.h"); break;
	}

	clHeaderFiles.append("opencl_algebra.h"); // definitions of common math functions
	for (int i = 0; i < clHeaderFiles.size(); i++)
	{
		AddInclude(programEngine, openclPath + clHeaderFiles.at(i));
	}

	QString engineFileName;
	switch (effectType)
	{
		case hdrBlur: engineFileName = "hdr_blur.cl"; break;
		case chromaticAberration: engineFileName = "chromatic_aberration.cl"; break;
	}

	QString engineFullFileName = openclEnginePath + engineFileName;
	programEngine.append(LoadUtf8TextFromFile(engineFullFileName));

	SetUseFastRelaxedMath(params->Get<bool>("opencl_use_fast_relaxed_math"));

	// building OpenCl kernel
	QString errorString;

	QElapsedTimer timer;
	timer.start();
	if (Build(programEngine, &errorString, false))
	{
		programsLoaded = true;
	}
	else
	{
		programsLoaded = false;
		WriteLog(errorString, 0);
	}

	if (compilerErrorOutput) *compilerErrorOutput = errorString;

	WriteLogDouble("cOpenClEngineRenderPostFilter: Opencl post filter build time [s]",
		timer.nsecsElapsed() / 1.0e9, 2);

	return programsLoaded;
}

void cOpenClEngineRenderPostFilter::RegisterInputOutputBuffers(
	std::shared_ptr<const cParameterContainer> params)
{
	Q_UNUSED(params);
	inputBuffers[0] << sClInputOutputBuffer(sizeof(cl_float4), numberOfPixels, "input image buffer");
	outputBuffers[0] << sClInputOutputBuffer(
		sizeof(cl_float4), numberOfPixels, "output image buffer");
}

bool cOpenClEngineRenderPostFilter::AssignParametersToKernelAdditional(
	uint argIterator, int deviceIndex)
{
	int err;

	switch (effectType)
	{
		case hdrBlur: err = clKernels.at(deviceIndex)->setArg(argIterator++, paramsHDRBlur); break;
		case chromaticAberration:;
			err = clKernels.at(deviceIndex)->setArg(argIterator++, paramsChromaticAberration);
			break;
	}

	if (!checkErr(err, "kernel->setArg(" + QString::number(argIterator) + ", paramsPostEffect)"))
	{
		emit showErrorMessage(QObject::tr("Cannot set OpenCL argument for %1")
														.arg(QObject::tr("%1 params").arg(effectName)),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}
	return true;
}

bool cOpenClEngineRenderPostFilter::ProcessQueue(quint64 pixelsLeft, quint64 pixelIndex)
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
		emit showErrorMessage(QObject::tr("Cannot finish rendering %1").arg(effectName),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderPostFilter::Render(std::shared_ptr<cImage> image, bool *stopRequest)
{
	if (programsLoaded)
	{
		// The image resolution determines the total amount of work
		quint64 width = imageRegion.width;
		quint64 height = imageRegion.height;

		cProgressText progressText;
		progressText.ResetTimer();

		emit updateProgressAndStatus(
			tr("OpenCl - rendering %1").arg(effectName), progressText.getText(0.0), 0.0);

		QElapsedTimer timer;
		timer.start();

		// copy zBuffer to input buffer

		for (quint64 y = 0; y < height; y++)
		{
			for (quint64 x = 0; x < width; x++)
			{
				quint64 i = x + y * width;

				sRGBFloat color = image->GetPixelPostImage(x + imageRegion.x1, y + imageRegion.y1);
				float alpha = image->GetPixelAlpha(x + imageRegion.x1, y + imageRegion.y1) / 65536;
				cl_float4 colorCl = {
					{cl_float(color.R), cl_float(color.G), cl_float(color.B), cl_float(alpha)}};

				reinterpret_cast<cl_float4 *>(inputBuffers[0][inputImageIndex].ptr.get())[i] = colorCl;
			}
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
			emit updateProgressAndStatus(tr("OpenCl - rendering %1").arg(effectName),
				progressText.getText(percentDone), percentDone);
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

					cl_float4 colorCl =
						reinterpret_cast<cl_float4 *>(outputBuffers[0][outputIndex].ptr.get())[x + y * width];

					sRGBFloat color(colorCl.s0, colorCl.s1, colorCl.s2);
					int alpha = int(colorCl.s3 * 65535);

					image->PutPixelPostImage(xx, yy, color);
					image->PutPixelAlpha(xx, yy, alpha);
				}
			}

			WriteLogDouble(
				"cOpenClEngineRenderHDRBlur: OpenCL Rendering time [s]", timer.nsecsElapsed() / 1.0e9, 2);

			WriteLog("image->CompileImage()", 2);
			image->CompileImage();

			if (image->IsPreview())
			{
				WriteLog("image->ConvertTo8bit()", 2);
				image->ConvertTo8bitChar();
				WriteLog("image->UpdatePreview()", 2);
				image->UpdatePreview();
				WriteLog("image->GetImageWidget()->update()", 2);
				emit updateImage();
			}
		}

		emit updateProgressAndStatus(
			tr("OpenCl - rendering %1 finished").arg(effectName), progressText.getText(1.0), 1.0);

		return true;
	}
	else
	{
		return false;
	}
}

size_t cOpenClEngineRenderPostFilter::CalcNeededMemory()
{
	return numberOfPixels * 2 * sizeof(cl_float4);
}

#endif // USE_OPENCL
