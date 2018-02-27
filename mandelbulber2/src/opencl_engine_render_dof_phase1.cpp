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
 * c++ - opencl connector for the DOF Phase1 OpenCL renderer
 */

#include "opencl_engine_render_dof_phase1.h"

#include "cimage.hpp"
#include "files.h"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "opencl_hardware.h"
#include "progress_text.hpp"

cOpenClEngineRenderDOFPhase1::cOpenClEngineRenderDOFPhase1(cOpenClHardware *_hardware)
		: cOpenClEngine(_hardware)
{
#ifdef USE_OPENCL
	paramsDOF.height = 0;
	paramsDOF.width = 0;
	paramsDOF.height = 0;
	paramsDOF.radius = 0.0;
	paramsDOF.maxRadius = 0.0;
	numberOfPixels = 0;
	optimalJob.sizeOfPixel = 0; // memory usage doens't depend on job size
	optimalJob.optimalProcessingCycle = 0.5;
#endif
}

cOpenClEngineRenderDOFPhase1::~cOpenClEngineRenderDOFPhase1()
{
#ifdef USE_OPENCL
	ReleaseMemory();
#endif
}

#ifdef USE_OPENCL

QString cOpenClEngineRenderDOFPhase1::GetKernelName()
{
	return QString("DOFPhase1");
}

void cOpenClEngineRenderDOFPhase1::SetParameters(const sParamRender *paramRender)
{
	paramsDOF.width = paramRender->imageWidth;
	paramsDOF.height = paramRender->imageHeight;
	paramsDOF.deep =
		paramRender->DOFRadius * (paramRender->imageWidth + paramRender->imageHeight) / 2000.0;
	paramsDOF.neutral = paramRender->DOFFocus;
	paramsDOF.maxRadius = paramRender->DOFMaxRadius;

	numberOfPixels = paramsDOF.width * paramsDOF.height;
}

bool cOpenClEngineRenderDOFPhase1::LoadSourcesAndCompile(const cParameterContainer *params)
{
	Q_UNUSED(params);

	programsLoaded = false;
	readyForRendering = false;
	emit updateProgressAndStatus(
		tr("OpenCL DOF - initializing"), tr("Compiling sources for DOF phase 1"), 0.0);

	QString openclPath = systemData.sharedDir + "opencl" + QDir::separator();
	QString openclEnginePath = openclPath + "engines" + QDir::separator();

	QByteArray programEngine;
	// pass through define constants
	programEngine.append("#define USE_OPENCL 1\n");

	QStringList clHeaderFiles;
	clHeaderFiles.append("opencl_typedefs.h"); // definitions of common opencl types
	clHeaderFiles.append("dof_cl.h");					 // main data structures

	for (int i = 0; i < clHeaderFiles.size(); i++)
	{
		programEngine.append("#include \"" + openclPath + clHeaderFiles.at(i) + "\"\n");
	}

	QString engineFileName = "dof_phase1.cl";
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
		"cOpenClEngineRenderDOFPhase1: Opencl DOF build time [s]", timer.nsecsElapsed() / 1.0e9, 2);

	return programsLoaded;
}

void cOpenClEngineRenderDOFPhase1::RegisterInputOutputBuffers(const cParameterContainer *params)
{
	Q_UNUSED(params);
	inputBuffers << sClInputOutputBuffer(sizeof(cl_float), numberOfPixels, "z-buffer");
	inputBuffers << sClInputOutputBuffer(sizeof(cl_float4), numberOfPixels, "image buffer");
	outputBuffers << sClInputOutputBuffer(sizeof(cl_float4), optimalJob.stepSize, "output buffer");
}

bool cOpenClEngineRenderDOFPhase1::AssignParametersToKernelAdditional(int argIterator)
{
	int err = kernel->setArg(argIterator++, paramsDOF); // pixel offset
	if (!checkErr(err, "kernel->setArg(2, pixelIndex)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("DOF params")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderDOFPhase1::ProcessQueue(
	size_t jobX, size_t jobY, size_t pixelsLeftX, size_t pixelsLeftY)
{
	size_t stepSizeX = optimalJob.stepSizeX;
	if (pixelsLeftX < stepSizeX) stepSizeX = pixelsLeftX;
	size_t stepSizeY = optimalJob.stepSizeY;
	if (pixelsLeftY < stepSizeY) stepSizeY = pixelsLeftY;

	// optimalJob.stepSize = stepSize;
	cl_int err = queue->enqueueNDRangeKernel(
		*kernel, cl::NDRange(jobX, jobY), cl::NDRange(stepSizeX, stepSizeY), cl::NullRange);
	if (!checkErr(err, "CommandQueue::enqueueNDRangeKernel()"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot enqueue OpenCL rendering jobs"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderDOFPhase1::Render(cImage *image, bool *stopRequest)
{
	if (programsLoaded)
	{
		// The image resolution determines the total amount of work
		int width = image->GetWidth();
		int height = image->GetHeight();

		cProgressText progressText;
		progressText.ResetTimer();

		emit updateProgressAndStatus(
			tr("OpenCL - rendering DOF - phase 1"), progressText.getText(0.0), 0.0);

		QElapsedTimer timer;
		timer.start();

		qint64 numberOfPixels = qint64(width) * qint64(height);
		qint64 gridWidth = width / optimalJob.stepSizeX;
		qint64 gridHeight = height / optimalJob.stepSizeY;
		QList<QRect> lastRenderedRects;
		qint64 pixelsRendered = 0;

		// copy zBuffer and image to input buffers
		for (qint64 i = 0; i < numberOfPixels; i++)
		{
			((cl_float *)inputBuffers[zBufferIndex].ptr)[i] = image->GetZBufferPtr()[i];
			sRGBFloat imagePixel = image->GetPostImageFloatPtr()[i];
			float alpha = image->GetAlphaBufPtr()[i] / 65535.0;
			((cl_float4 *)inputBuffers[imageIndex].ptr)[i] =
				cl_float4{imagePixel.R, imagePixel.G, imagePixel.B, alpha};
		}

		// writing data to queue
		if (!WriteBuffersToQueue()) return false;

		// TODO:
		// insert device for loop here
		// requires initialization for all opencl devices
		// requires optimalJob for all opencl devices

		for (int gridY = 0; gridY <= gridHeight; gridY++)
		{
			for (int gridX = 0; gridX <= gridWidth; gridX++)
			{
				qint64 jobX = gridX * optimalJob.stepSizeX;
				qint64 jobY = gridY * optimalJob.stepSizeY;
				qint64 pixelsLeftX = width - jobX;
				qint64 pixelsLeftY = height - jobY;
				qint64 jobWidth2 = min(optimalJob.stepSizeX, pixelsLeftX);
				qint64 jobHeight2 = min(optimalJob.stepSizeY, pixelsLeftY);
				if (jobHeight2 <= 0) continue;
				if (jobWidth2 <= 0) continue;

				// assign parameters to kernel
				if (!AssignParametersToKernel()) return false;

				// processing queue
				if (!ProcessQueue(jobX, jobY, pixelsLeftX, pixelsLeftY)) return false;

				double percentDone = double(pixelsRendered) / numberOfPixels;
				emit updateProgressAndStatus(
					tr("OpenCL - rendering DOF phase 1"), progressText.getText(percentDone), percentDone);
				gApplication->processEvents();

				pixelsRendered += jobWidth2 * jobHeight2;

				if (!ReadBuffersFromQueue()) return false;

				for (int y = 0; y < jobHeight2; y++)
				{
					for (int x = 0; x < jobWidth2; x++)
					{
						cl_float4 pixelCl = ((cl_float4 *)outputBuffers[outputIndex].ptr)[x + y * jobWidth2];
						sRGBFloat pixel = {pixelCl.s[0], pixelCl.s[1], pixelCl.s[2]};
						quint16 alpha = pixelCl.s[3] * 65535;

						image->PutPixelPostImage(x + jobX, y + jobY, pixel);
						image->PutPixelAlpha(x + jobX, y + jobY, alpha);
					}
				}

				if (*stopRequest)
				{
					return false;
				}
			}
		}

		if (!*stopRequest)
		{
			WriteLogDouble(
				"cOpenClEngineRenderDOFPhase1: OpenCL Rendering time [s]", timer.nsecsElapsed() / 1.0e9, 2);

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
			tr("OpenCL - rendering DOF phase 1 finished"), progressText.getText(1.0), 1.0);

		return true;
	}
	else
	{
		return false;
	}
}

size_t cOpenClEngineRenderDOFPhase1::CalcNeededMemory()
{
	return numberOfPixels * sizeof(cl_float4);
}

#endif // USE_OPENCL
