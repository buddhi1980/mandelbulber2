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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Robert Pancoast (RobertPancoast77@gmail.com),
 *  Sebastian Jennen (jenzebas@gmail.com)
 *
 * cOpenClEngineRenderFractal - prepares and executes fractal rendering on opencl
 */

#include "cimage.hpp"
#include "files.h"
#include "fractal_formulas.hpp"
#include "fractal_list.hpp"
#include "fractal.h"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "material.h"
#include "nine_fractals.hpp"
#include "opencl_engine_render_fractal.h"
#include "opencl_dynamic_data.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"

#include "camera_target.hpp"
#include "common_math.h"
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

	inCLBuffer = nullptr;

	rgbBuffer = nullptr;
	outCL = nullptr;

	dynamicData = new cOpenClDynamicData;

	optimalJob.sizeOfPixel = sizeof(sClPixel);

#endif
}

cOpenClEngineRenderFractal::~cOpenClEngineRenderFractal()
{
#ifdef USE_OPENCL
	if (constantInBuffer) delete constantInBuffer;
	if (inCLConstBuffer) delete inCLConstBuffer;
	if (inCLBuffer) delete inCLBuffer;
	if (rgbBuffer) delete[] rgbBuffer;
	if (outCL) delete outCL;
	if (dynamicData) delete dynamicData;
#endif
}

#ifdef USE_OPENCL

QString cOpenClEngineRenderFractal::GetKernelName()
{
	return QString("fractal3D");
}

bool cOpenClEngineRenderFractal::LoadSourcesAndCompile(const cParameterContainer *params)
{
	programsLoaded = false;
	readyForRendering = false;

	emit updateProgressAndStatus(tr("OpenCl - initializing"), tr("Compiling sources"), 0.0);

	QByteArray programEngine;
	try
	{
		QString openclPath = systemData.sharedDir + "opencl" + QDir::separator();
		QString openclEnginePath = openclPath + "engines" + QDir::separator();

		// pass through define constants
		programEngine.append("#define USE_OPENCL 1\n");
		programEngine.append(
			"#define NUMBER_OF_FRACTALS " + QString::number(NUMBER_OF_FRACTALS) + "\n");

		programEngine.append("#define SQRT_1_3 " + QString::number(SQRT_1_3) + "\n");
		programEngine.append("#define SQRT_1_2 " + QString::number(SQRT_1_2) + "\n");
		programEngine.append("#define SQRT_2_3 " + QString::number(SQRT_2_3) + "\n");
		programEngine.append("#define SQRT_3_2 " + QString::number(SQRT_3_2) + "\n");
		programEngine.append("#define SQRT_3_4 " + QString::number(SQRT_3_4) + "\n");
		programEngine.append("#define SQRT_3_4d2 " + QString::number(SQRT_3_4d2) + "\n");
		programEngine.append("#define SQRT_3 " + QString::number(SQRT_3) + "\n");
		programEngine.append("#define FRAC_1_3 " + QString::number(FRAC_1_3) + "\n");
		programEngine.append("#define M_PI_180 " + QString::number(M_PI_180) + "\n");
		programEngine.append("#define M_PI_8 " + QString::number(M_PI_8) + "\n");

		programEngine.append("#define IFS_VECTOR_COUNT " + QString::number(IFS_VECTOR_COUNT) + "\n");
		programEngine.append("#define HYBRID_COUNT " + QString::number(HYBRID_COUNT) + "\n");
		programEngine.append("#define MANDELBOX_FOLDS " + QString::number(MANDELBOX_FOLDS) + "\n");
		programEngine.append("#define Q_UNUSED(x) (void)x;\n");
		programEngine.append("#define MAX_RAYMARCHING " + QString::number(10000) + "\n");

		if (params->Get<bool>("gpu_precision"))
			programEngine.append("#define DOUBLE_PRECISION " + QString::number(1) + "\n");

		QStringList clHeaderFiles;
		clHeaderFiles.append("opencl_typedefs.h");			 // definitions of common opencl types
		clHeaderFiles.append("opencl_algebra.h");				 // algebra for kernels
		clHeaderFiles.append("common_params_cl.hpp");		 // common parameters
		clHeaderFiles.append("image_adjustments_cl.h");	// image adjustments
		clHeaderFiles.append("fractal_cl.h");						 // fractal data structures
		clHeaderFiles.append("fractparams_cl.hpp");			 // rendering data structures
		clHeaderFiles.append("fractal_sequence_cl.h");	 // sequence of fractal formulas
		clHeaderFiles.append("material_cl.h");					 // materials
		clHeaderFiles.append("input_data_structures.h"); // main data structures

		for (int i = 0; i < clHeaderFiles.size(); i++)
		{
			programEngine.append("#include \"" + openclPath + clHeaderFiles.at(i) + "\"\n");
		}

		// fractal formulas - only actually used
		for (int i = 0; i < listOfUsedFormulas.size(); i++)
		{
			QString formulaName = listOfUsedFormulas.at(i);
			if (formulaName != "")
			{
				programEngine.append("#include \"" + systemData.sharedDir + "formula" + QDir::separator()
														 + "opencl" + QDir::separator() + formulaName + ".cl\"\n");
			}
		}

		// compute fractal
		programEngine.append("#include \"" + openclEnginePath + "compute_fractal.cl\"\n");

		// calculate distance
		programEngine.append("#include \"" + openclEnginePath + "calculate_distance.cl\"\n");

		// normal vector calculation
		programEngine.append("#include \"" + openclEnginePath + "normal_vector.cl\"\n");

		// 3D projections (3point, equirectagular, fisheye)
		programEngine.append("#include \"" + openclEnginePath + "projection_3d.cl\"\n");

		if (params->Get<int>("gpu_mode") != clRenderEngineTypeFast)
		{
			// shaders
			programEngine.append("#include \"" + openclEnginePath + "shaders.cl\"\n");
		}

		if (params->Get<int>("gpu_mode") == clRenderEngineTypeFull)
		{
			// ray recursion
			programEngine.append("#include \"" + openclEnginePath + "ray_recursion.cl\"\n");
		}

		// main engine
		QString engineFileName;
		switch (enumClRenderEngineMode(params->Get<int>("gpu_mode")))
		{
			case clRenderEngineTypeFast: engineFileName = "fast_engine.cl"; break;
			case clRenderEngineTypeLimited: engineFileName = "limited_engine.cl"; break;
			case clRenderEngineTypeFull: engineFileName = "full_engine.cl"; break;
		}
		QString engineFullFileName = openclEnginePath + engineFileName;
		programEngine.append(LoadUtf8TextFromFile(engineFullFileName));
	}
	catch (const QString &ex)
	{
		qCritical() << "OpenCl program error: " << ex;
		return false;
	}

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
	qDebug() << "Opencl build time [s]" << timer.nsecsElapsed() / 1.0e9;

	return programsLoaded;
}

void cOpenClEngineRenderFractal::SetParameters(const cParameterContainer *paramContainer,
	const cFractalContainer *fractalContainer, sParamRender *paramRender, cNineFractals *fractals,
	sRenderData *renderData)
{
	if (constantInBuffer) delete constantInBuffer;
	constantInBuffer = new sClInConstants;

	definesCollector.clear();

	// update camera rotation data (needed for simplified calculation in opencl kernel)
	cCameraTarget cameraTarget(paramRender->camera, paramRender->target, paramRender->topVector);
	paramRender->viewAngle = cameraTarget.GetRotation() * 180.0 / M_PI;
	paramRender->resolution = 1.0 / paramRender->imageHeight;

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
			case fractal::josKleinianDEFunction: definesCollector += " -DANALYTIC_JOS_KLEINIAN_DE"; break;
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

	// creating list of used formulas
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
	}

	// ---- perspective projections ------
	switch (paramRender->perspectiveType)
	{
		case params::perspThreePoint: definesCollector += " -DPERSP_THREE_POINT"; break;
		case params::perspEquirectangular: definesCollector += " -DPERSP_EQUIRECTANGULAR"; break;
		case params::perspFishEye: definesCollector += " -DPERSP_FISH_EYE"; break;
		case params::perspFishEyeCut: definesCollector += " -DPERSP_FISH_EYE_CUT"; break;
	}

	// ------------ enabling shaders ----------
	bool anyVolumetricShaderUsed = false;

	if (paramRender->shadow) definesCollector += " -DSHADOWS";
	if (paramRender->ambientOcclusionEnabled)
	{
		if (paramRender->ambientOcclusionMode == params::AOModeFast)
			definesCollector += " -DAO_MODE_FAST";
		else if (paramRender->ambientOcclusionMode == params::AOModeMultipleRays)
			definesCollector += " -DAO_MODE_MULTIPLE_RAYS";
	}
	if (paramRender->slowShading) definesCollector += " -DSLOW_SHADING";
	if (renderData->lights.IsAnyLightEnabled())
	{
		definesCollector += " -DAUX_LIGHTS";
		if (paramRender->auxLightVisibility > 0.0)
		{
			definesCollector += " -DVISIBLE_AUX_LIGHTS";
			anyVolumetricShaderUsed = true;
		}
	}

	if (paramRender->interiorMode) definesCollector += " -DINTERIOR_MODE";

	bool isVolumetricLight = false;
	for (int i = 0; i < 5; i++)
	{
		if (paramRender->volumetricLightEnabled[i]) isVolumetricLight = true;
	}
	if (isVolumetricLight)
	{
		definesCollector += " -DVOLUMETRIC_LIGHTS";
		anyVolumetricShaderUsed = true;
	}

	if (paramRender->glowEnabled) definesCollector += " -DGLOW";
	if (paramRender->fogEnabled)
	{
		definesCollector += " -DBASIC_FOG";
		anyVolumetricShaderUsed = true;
	}
	if (paramRender->volFogEnabled && paramRender->volFogDensity > 0)
	{
		definesCollector += " -DVOLUMETRIC_FOG";
		anyVolumetricShaderUsed = true;
	}
	if (paramRender->iterFogEnabled > 0)
	{
		definesCollector += " -DITER_FOG";
		anyVolumetricShaderUsed = true;
	}
	if (paramRender->fakeLightsEnabled > 0)
	{
		definesCollector += " -DFAKE_LIGHTS";
		anyVolumetricShaderUsed = true;
	}
	if (!anyVolumetricShaderUsed) definesCollector += " -DSIMPLE_GLOW";

	listOfUsedFormulas = listOfUsedFormulas.toSet().toList(); // eliminate duplicates

	qDebug() << "Constant buffer size" << sizeof(sClInConstants);

	//----------- create dynamic data -----------
	dynamicData->Clear();
	dynamicData->ReserveHeader();

	// materials
	QMap<int, cMaterial> materials;
	CreateMaterialsMap(paramContainer, &materials, true);
	dynamicData->BuildMaterialsData(materials);

	// AO colored vectors
	cRenderWorker *tempRenderWorker =
		new cRenderWorker(paramRender, fractals, nullptr, renderData, nullptr);
	tempRenderWorker->PrepareAOVectors();
	sVectorsAround *AOVectors = tempRenderWorker->getAOVectorsAround();
	int numberOfVectors = tempRenderWorker->getAoVectorsCount();
	dynamicData->BuildAOVectorsData(AOVectors, numberOfVectors);
	dynamicData->BuildLightsData(&renderData->lights);

	dynamicData->FillHeader();

	inBuffer = dynamicData->GetData();

	delete tempRenderWorker;
}

bool cOpenClEngineRenderFractal::PreAllocateBuffers(const cParameterContainer *params)
{
	Q_UNUSED(params);

	cl_int err;

	if (hardware->ContextCreated())
	{

		if (inCLConstBuffer) delete inCLConstBuffer;
		inCLConstBuffer = new cl::Buffer(*hardware->getContext(),
			CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(sClInConstants), constantInBuffer, &err);
		if (!checkErr(err,
					"cl::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, "
					"sizeof(sClInConstants), constantInBuffer, &err)"))
		{
			emit showErrorMessage(
				QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for constants")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}

		// this buffer will be used for color palettes, lights, etc...
		if (inCLBuffer) delete inCLBuffer;
		inCLBuffer = new cl::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			size_t(inBuffer.size()), inBuffer.data(), &err);
		if (!checkErr(err,
					"Buffer::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, "
					"sizeof(sClInBuff), inBuffer, &err)"))
		{
			emit showErrorMessage(
				QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for variable data")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}

		size_t buffSize = optimalJob.stepSize * sizeof(sClPixel);
		if (rgbBuffer) delete[] rgbBuffer;
		rgbBuffer = new sClPixel[optimalJob.stepSize];

		if (outCL) delete outCL;
		outCL = new cl::Buffer(
			*hardware->getContext(), CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, rgbBuffer, &err);
		if (!checkErr(
					err, "*context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, rgbBuffer, &err"))
		{
			emit showErrorMessage(
				QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("output buffer")),
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

// TODO:
// This is the hot spot for heterogeneous execution
// requires opencl for all compute resources
bool cOpenClEngineRenderFractal::Render(cImage *image, bool *stopRequest)
{
	if (programsLoaded)
	{
		// The image resolution determines the total amount of work
		int width = image->GetWidth();
		int height = image->GetHeight();

		cProgressText progressText;
		progressText.ResetTimer();

		emit updateProgressAndStatus(tr("OpenCl - rendering image"), progressText.getText(0.0), 0.0);

		QElapsedTimer timer;
		timer.start();

		QElapsedTimer progressRefreshTimer;
		progressRefreshTimer.start();

		QList<QRect> lastRenderedRects;

		// TODO:
		// insert device for loop here
		// requires initialization for all opencl devices
		// requires optimalJob for all opencl devices

		int pixelsRendered = 0;
		int numberOfPixels = width * height;
		int gridWidth = width / optimalJob.stepSizeX;
		int gridHeight = height / optimalJob.stepSizeY;
		int gridX = (gridWidth - 1) / 2;
		int gridY = gridHeight / 2;
		int dir = 0;
		int gridPass = 0;
		int gridStep = 1;

		while (pixelsRendered < numberOfPixels)
		{
			int jobX = gridX * optimalJob.stepSizeX;
			int jobY = gridY * optimalJob.stepSizeY;

			if (jobX >= 0 && jobX < width && jobY >= 0 && jobY < height)
			{
				size_t pixelsLeftX = width - jobX;
				size_t pixelsLeftY = height - jobY;

				// assign parameters to kernel
				if (!AssignParametersToKernel()) return false;

				// writing data to queue
				if (!WriteBuffersToQueue()) return false;

				// processing queue
				if (!ProcessQueue(jobX, jobY, pixelsLeftX, pixelsLeftY)) return false;

				// update image when OpenCl kernel is working
				if (lastRenderedRects.size() > 0)
				{
					QElapsedTimer timerImageRefresh;
					timerImageRefresh.start();
					image->NullPostEffect(&lastRenderedRects);
					image->CompileImage(&lastRenderedRects);
					image->ConvertTo8bit(&lastRenderedRects);
					image->UpdatePreview(&lastRenderedRects);
					image->GetImageWidget()->update();
					lastRenderedRects.clear();
					optimalJob.optimalProcessingCycle = 2.0 * timerImageRefresh.elapsed() / 1000.0;
					if (optimalJob.optimalProcessingCycle < 0.1) optimalJob.optimalProcessingCycle = 0.1;
				}

				if (!ReadBuffersFromQueue()) return false;

				// Collect Pixel information from the rgbBuffer
				// Populate the data into image->Put

				int jobWidth2 = min(optimalJob.stepSizeX, pixelsLeftX);
				int jobHeight2 = min(optimalJob.stepSizeY, pixelsLeftY);
				for (int x = 0; x < jobWidth2; x++)
				{
					for (int y = 0; y < jobHeight2; y++)
					{
						sClPixel pixelCl = rgbBuffer[x + y * jobWidth2];
						sRGBFloat pixel = {pixelCl.R, pixelCl.G, pixelCl.B};
						sRGB8 color = {pixelCl.colR, pixelCl.colG, pixelCl.colB};
						unsigned short opacity = pixelCl.opacity;
						unsigned short alpha = pixelCl.alpha;
						int xx = x + jobX;
						int yy = y + jobY;

						image->PutPixelImage(xx, yy, pixel);
						image->PutPixelZBuffer(xx, yy, rgbBuffer[x + y * jobWidth2].zBuffer);
						image->PutPixelColor(xx, yy, color);
						image->PutPixelOpacity(xx, yy, opacity);
						image->PutPixelAlpha(xx, yy, alpha);
					}
				}

				lastRenderedRects.append(QRect(jobX, jobY, jobWidth2, jobHeight2));

				pixelsRendered += jobWidth2 * jobHeight2;

				if (progressRefreshTimer.elapsed() > 100)
				{
					double percentDone = double(pixelsRendered) / numberOfPixels;
					emit updateProgressAndStatus(
						tr("OpenCl - rendering image"), progressText.getText(percentDone), percentDone);
					gApplication->processEvents();
					progressRefreshTimer.restart();
				}

				if (*stopRequest)
				{
					return false;
				}
			}
			// selection of next piece
			switch (dir)
			{
				case 0:
					gridX++;
					if (gridStep > gridPass)
					{
						gridStep = 0;
						dir = 1;
					}
					break;

				case 1:
					gridY++;
					if (gridStep > gridPass)
					{
						gridStep = 0;
						gridPass++;
						dir = 2;
					}
					break;

				case 2:
					gridX--;
					if (gridStep > gridPass)
					{
						gridStep = 0;
						dir = 3;
					}
					break;

				case 3:
					gridY--;
					if (gridStep > gridPass)
					{
						gridStep = 0;
						gridPass++;
						dir = 0;
					}
					break;
			}

			gridStep++;
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

bool cOpenClEngineRenderFractal::AssignParametersToKernel()
{
	cl_int err = kernel->setArg(0, *outCL); // output image

	if (!checkErr(err, "kernel->setArg(0, *outCL)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("output data")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = kernel->setArg(1, *inCLBuffer); // input data in global memory
	if (!checkErr(err, "kernel->setArg(1, *inCLBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("input data")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = kernel->setArg(2, *inCLConstBuffer); // input data in constant memory (faster than global)
	if (!checkErr(err, "kernel->setArg(2, *inCLConstBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("constant data")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderFractal::WriteBuffersToQueue()
{
	cl_int err = queue->enqueueWriteBuffer(*inCLBuffer, CL_TRUE, 0, inBuffer.size(), inBuffer.data());

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

	err = queue->enqueueWriteBuffer(
		*inCLConstBuffer, CL_TRUE, 0, sizeof(sClInConstants), constantInBuffer);
	if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(inCLConstBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot enqueue writing OpenCL %1").arg(QObject::tr("constant buffers")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = queue->finish();
	if (!checkErr(err, "CommandQueue::finish() - inCLConstBuffer"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot finish writing OpenCL %1").arg(QObject::tr("constant buffers")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderFractal::ProcessQueue(int jobX, int jobY, int pixelsLeftX, int pixelsLeftY)
{
	//	size_t limitedWorkgroupSize = optimalJob.workGroupSize;
	//	int stepSize = optimalJob.stepSize;
	//
	//	if (optimalJob.stepSize > pixelsLeft)
	//	{
	//		int mul = pixelsLeft / optimalJob.workGroupSize;
	//		if (mul > 0)
	//		{
	//			stepSize = mul * optimalJob.workGroupSize;
	//		}
	//		else
	//		{
	//			// in this case will be limited workGroupSize
	//			stepSize = pixelsLeft;
	//			limitedWorkgroupSize = pixelsLeft;
	//		}
	//	}

	int stepSizeX = optimalJob.stepSizeX;
	if (pixelsLeftX < stepSizeX) stepSizeX = pixelsLeftX;
	int stepSizeY = optimalJob.stepSizeY;
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

bool cOpenClEngineRenderFractal::ReadBuffersFromQueue()
{
	size_t buffSize = optimalJob.stepSize * sizeof(sClPixel);

	cl_int err = queue->enqueueReadBuffer(*outCL, CL_TRUE, 0, buffSize, rgbBuffer);
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

#endif
