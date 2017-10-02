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

#include "opencl_engine_render_fractal.h"

#include "camera_target.hpp"
#include "cimage.hpp"
#include "common_math.h"
#include "files.h"
#include "fractal.h"
#include "fractal_formulas.hpp"
#include "fractal_list.hpp"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "material.h"
#include "nine_fractals.hpp"
#include "opencl_dynamic_data.hpp"
#include "opencl_hardware.h"
#include "parameters.hpp"
#include "progress_text.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"

// custom includes
#ifdef USE_OPENCL
#include "opencl/fractal_cl.h"
#include "opencl/fractparams_cl.hpp"
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
	autoRefreshMode = false;
	monteCarlo = false;

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

void cOpenClEngineRenderFractal::ReleaseMemory()
{
	if (constantInBuffer) delete constantInBuffer;
	constantInBuffer = nullptr;
	if (inCLConstBuffer) delete inCLConstBuffer;
	inCLConstBuffer = nullptr;
	if (inCLBuffer) delete inCLBuffer;
	inCLBuffer = nullptr;
	if (rgbBuffer) delete[] rgbBuffer;
	rgbBuffer = nullptr;
	if (outCL) delete outCL;
	outCL = nullptr;
	dynamicData->Clear();
}

QString cOpenClEngineRenderFractal::GetKernelName()
{
	return QString("fractal3D");
}

bool cOpenClEngineRenderFractal::LoadSourcesAndCompile(const cParameterContainer *params)
{
	programsLoaded = false;
	readyForRendering = false;

	emit updateProgressAndStatus(tr("OpenCl - initializing"), tr("Compiling OpenCL programs"), 0.0);

	QByteArray programEngine;
	try
	{
		QString openclPath = systemData.sharedDir + "opencl" + QDir::separator();
		QString openclEnginePath = openclPath + "engines" + QDir::separator();

		QStringList clHeaderFiles;
		clHeaderFiles.append("defines_cl.h");
		clHeaderFiles.append("opencl_typedefs.h");
		clHeaderFiles.append("opencl_algebra.h");
		clHeaderFiles.append("common_params_cl.hpp");
		clHeaderFiles.append("image_adjustments_cl.h");
		clHeaderFiles.append("fractal_cl.h");
		clHeaderFiles.append("fractparams_cl.hpp");
		clHeaderFiles.append("fractal_sequence_cl.h");
		clHeaderFiles.append("material_cl.h");
		clHeaderFiles.append("input_data_structures.h");

		// pass through define constants
		programEngine.append("#define USE_OPENCL 1\n");

		if (params->Get<bool>("opencl_precision"))
			programEngine.append("#define DOUBLE_PRECISION " + QString::number(1) + "\n");

#ifdef _WIN32
		QString openclPathSlash = openclPath.replace("/", "\\"); // replace single slash with backslash
#else
		QString openclPathSlash = openclPath;
#endif

		for (int i = 0; i < clHeaderFiles.size(); i++)
		{
			programEngine.append("#include \"" + openclPathSlash + clHeaderFiles.at(i) + "\"\n");
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

		if (params->Get<int>("opencl_mode") != clRenderEngineTypeFast)
		{
			// shaders
			programEngine.append("#include \"" + openclEnginePath + "shaders.cl\"\n");
		}

		if (params->Get<int>("opencl_mode") == clRenderEngineTypeFull)
		{
			// ray recursion
			programEngine.append("#include \"" + openclEnginePath + "ray_recursion.cl\"\n");
		}

		if (params->Get<int>("opencl_mode") != clRenderEngineTypeFast)
		{
			// Monte Carlo DOF
			programEngine.append("#include \"" + openclEnginePath + "monte_carlo_dof.cl\"\n");
		}

		// main engine
		QString engineFileName;
		switch (enumClRenderEngineMode(params->Get<int>("opencl_mode")))
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
	WriteLogDouble(
		"cOpenClEngineRenderFractal: Opencl DOF build time [s]", timer.nsecsElapsed() / 1.0e9, 2);

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
	if (paramRender->iterFogEnabled == true)
	{
		definesCollector += " -DITER_FOG";
		anyVolumetricShaderUsed = true;
	}
	if (paramRender->fakeLightsEnabled == true)
	{
		definesCollector += " -DFAKE_LIGHTS";
		anyVolumetricShaderUsed = true;
	}
	if (!anyVolumetricShaderUsed) definesCollector += " -DSIMPLE_GLOW";

	if (paramRender->DOFMonteCarlo && paramRender->DOFEnabled)
		definesCollector += " -DMONTE_CARLO_DOF";

	listOfUsedFormulas = listOfUsedFormulas.toSet().toList(); // eliminate duplicates

	WriteLogDouble("Constant buffer size [KB]", sizeof(sClInConstants) / 1024.0, 3);

	//----------- create dynamic data -----------
	dynamicData->Clear();
	dynamicData->ReserveHeader();

	// materials
	QMap<int, cMaterial> materials;
	CreateMaterialsMap(paramContainer, &materials, true);
	dynamicData->BuildMaterialsData(materials);

	bool anyMaterialIsReflective = false;
	bool anyMaterialIsRefractive = false;
	foreach (cMaterial material, materials)
	{
		if (material.reflectance > 0.0) anyMaterialIsReflective = true;
		if (material.transparencyOfSurface > 0.0) anyMaterialIsRefractive = true;
	}
	if (anyMaterialIsReflective) definesCollector += " -DUSE_REFLECTANCE";
	if (anyMaterialIsRefractive) definesCollector += " -DUSE_REFRACTION";

	if ((anyMaterialIsReflective || anyMaterialIsRefractive) && paramRender->raytracedReflections)
	{
		definesCollector += " -DREFLECTIONS_MAX=" + QString::number(paramRender->reflectionsMax + 1);
	}
	else
	{
		paramRender->reflectionsMax = 0;
		definesCollector += " -DREFLECTIONS_MAX=1";
	}

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

	//---------------- another parameters -------------
	autoRefreshMode = paramContainer->Get<bool>("auto_refresh");
	monteCarlo = paramRender->DOFMonteCarlo && paramRender->DOFEnabled
							 && paramContainer->Get<int>("opencl_mode") != clRenderEngineTypeFast;

	// copy all cl parameters to constant buffer
	constantInBuffer->params = clCopySParamRenderCl(*paramRender);

	constantInBuffer->params.viewAngle = toClFloat3(paramRender->viewAngle * M_PI / 180.0);

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		constantInBuffer->fractal[i] = clCopySFractalCl(*fractals->GetFractal(i));
	}

	fractals->CopyToOpenclData(&constantInBuffer->sequence);

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
bool cOpenClEngineRenderFractal::Render(cImage *image, bool *stopRequest, sRenderData *renderData)
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

		int numberOfPixels = width * height;
		int gridWidth = width / optimalJob.stepSizeX;
		int gridHeight = height / optimalJob.stepSizeY;

		int noiseTableSize = (gridWidth + 1) * (gridHeight + 1);
		double *noiseTable = new double[noiseTableSize];
		for (int i = 0; i < noiseTableSize; i++)
		{
			noiseTable[i] = 1.0;
		}

		int numberOfSamples = 1;
		if (monteCarlo)
		{
			numberOfSamples = constantInBuffer->params.DOFSamples;
		}
		for (int monteCarloLoop = 1; monteCarloLoop <= numberOfSamples; monteCarloLoop++)
		{
			// TODO:
			// insert device for loop here
			// requires initialization for all opencl devices
			// requires optimalJob for all opencl devices

			QList<QRect> lastRenderedRects;

			int pixelsRendered = 0;
			int gridX = (gridWidth - 1) / 2;
			int gridY = gridHeight / 2;
			int dir = 0;
			int gridPass = 0;
			int gridStep = 1;

			while (pixelsRendered < numberOfPixels)
			{
				int jobX = gridX * optimalJob.stepSizeX;
				int jobY = gridY * optimalJob.stepSizeY;

				// check if noise is still too high
				bool bigNoise = true;
				if (monteCarlo)
				{
					if (noiseTable[gridX + gridY * (gridWidth + 1)]
								< constantInBuffer->params.DOFMaxNoise / 100.0
							&& monteCarloLoop > constantInBuffer->params.DOFMinSamples)
						bigNoise = false;
				}

				if (jobX >= 0 && jobX < width && jobY >= 0 && jobY < height)
				{
					size_t pixelsLeftX = width - jobX;
					size_t pixelsLeftY = height - jobY;
					int jobWidth2 = min(optimalJob.stepSizeX, pixelsLeftX);
					int jobHeight2 = min(optimalJob.stepSizeY, pixelsLeftY);

					if (monteCarloLoop == 1)
						renderData->statistics.numberOfRenderedPixels += jobHeight2 * jobWidth2;

					if (bigNoise)
					{
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
							if (image->IsPreview())
							{
								image->ConvertTo8bit(&lastRenderedRects);
								image->UpdatePreview(&lastRenderedRects);
								image->GetImageWidget()->update();
							}
							lastRenderedRects.clear();
							optimalJob.optimalProcessingCycle = 2.0 * timerImageRefresh.elapsed() / 1000.0;
							if (optimalJob.optimalProcessingCycle < 0.1) optimalJob.optimalProcessingCycle = 0.1;
						}

						if (!autoRefreshMode && !monteCarlo)
						{
							QRect currentCorners(jobX, jobY, jobWidth2, jobHeight2);
							MarkCurrentPendingTile(image, currentCorners);
						}

						if (!ReadBuffersFromQueue()) return false;

						// Collect Pixel information from the rgbBuffer
						// Populate the data into image->Put

						double monteCarloNoiseSum = 0.0;
						double maxNoise = 0.0;
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

								if (monteCarlo)
								{
									sRGBFloat oldPixel = image->GetPixelImage(xx, yy);
									sRGBFloat newPixel;
									newPixel.R =
										oldPixel.R * (1.0 - 1.0 / monteCarloLoop) + pixel.R * (1.0 / monteCarloLoop);
									newPixel.G =
										oldPixel.G * (1.0 - 1.0 / monteCarloLoop) + pixel.G * (1.0 / monteCarloLoop);
									newPixel.B =
										oldPixel.B * (1.0 - 1.0 / monteCarloLoop) + pixel.B * (1.0 / monteCarloLoop);
									image->PutPixelImage(xx, yy, newPixel);
									image->PutPixelZBuffer(xx, yy, rgbBuffer[x + y * jobWidth2].zBuffer);
									unsigned short oldAlpha = image->GetPixelAlpha(xx, yy);
									unsigned short newAlpha = (double)oldAlpha * (1.0 - 1.0 / monteCarloLoop)
																						+ alpha * (1.0 / monteCarloLoop);
									image->PutPixelAlpha(xx, yy, newAlpha);
									image->PutPixelColor(xx, yy, color);
									image->PutPixelOpacity(xx, yy, opacity);

									// noise estimation
									double noise = (newPixel.R - oldPixel.R) * (newPixel.R - oldPixel.R)
																 + (newPixel.G - oldPixel.G) * (newPixel.G - oldPixel.G)
																 + (newPixel.B - oldPixel.B) * (newPixel.B - oldPixel.B);
									noise *= 0.3333;
									monteCarloNoiseSum += noise;
									if (noise > maxNoise) maxNoise = noise;
								}
								else
								{
									image->PutPixelImage(xx, yy, pixel);
									image->PutPixelZBuffer(xx, yy, rgbBuffer[x + y * jobWidth2].zBuffer);
									image->PutPixelColor(xx, yy, color);
									image->PutPixelOpacity(xx, yy, opacity);
									image->PutPixelAlpha(xx, yy, alpha);
								}
							}
						}

						renderData->statistics.totalNumberOfDOFRepeats += jobWidth2 * jobHeight2;

						// total noise in last rectangle
						if (monteCarlo)
						{
							double weight = 0.2;
							double totalNoiseRect = sqrt(
								(1.0 - weight) * monteCarloNoiseSum / jobWidth2 / jobHeight2 + weight * maxNoise);
							noiseTable[gridX + gridY * (gridWidth + 1)] = totalNoiseRect;
						}

						lastRenderedRects.append(QRect(jobX, jobY, jobWidth2, jobHeight2));
					} // bigNoise

					pixelsRendered += jobWidth2 * jobHeight2;

					if (progressRefreshTimer.elapsed() > 100)
					{
						double percentDone;
						if (!monteCarlo)
						{
							percentDone = double(pixelsRendered) / numberOfPixels;
						}
						else
						{
							percentDone = double(monteCarloLoop - 1) / numberOfSamples
														+ double(pixelsRendered) / numberOfPixels / numberOfSamples;
						}
						emit updateProgressAndStatus(
							tr("OpenCl - rendering image"), progressText.getText(percentDone), percentDone);

						emit updateStatistics(renderData->statistics);

						gApplication->processEvents();
						progressRefreshTimer.restart();
					}

					if (*stopRequest)
					{
						delete[] noiseTable;
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

			// update last rectangle
			if (monteCarlo)
			{
				if (lastRenderedRects.size() > 0)
				{
					QElapsedTimer timerImageRefresh;
					timerImageRefresh.start();
					image->NullPostEffect(&lastRenderedRects);
					image->CompileImage(&lastRenderedRects);
					if (image->IsPreview())
					{
						image->ConvertTo8bit(&lastRenderedRects);
						image->UpdatePreview(&lastRenderedRects);
						image->GetImageWidget()->update();
					}
					lastRenderedRects.clear();
				}
				gApplication->processEvents();
			}

			double totalNoise = 0.0;
			for (int i = 0; i < noiseTableSize; i++)
			{
				totalNoise += noiseTable[i];
			}
			totalNoise /= noiseTableSize;
			renderData->statistics.totalNoise = totalNoise * width * height;

			emit updateStatistics(renderData->statistics);

		} // monte carlo loop

		delete[] noiseTable;

		WriteLogDouble(
			"cOpenClEngineRenderFractal: OpenCL Rendering time [s]", timer.nsecsElapsed() / 1.0e9, 2);

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

void cOpenClEngineRenderFractal::MarkCurrentPendingTile(cImage *image, QRect corners)
{
	int edgeWidth = max(1, min(corners.width(), corners.height()) / 10);
	int edgeLength = max(1, min(corners.width(), corners.height()) / 4);
	for (int _xx = 0; _xx < corners.width(); _xx++)
	{
		int xx = _xx + corners.x();
		for (int _yy = 0; _yy < corners.height(); _yy++)
		{
			int yy = _yy + corners.y();
			bool border = false;
			if (_xx < edgeWidth || _xx > corners.width() - edgeWidth)
			{
				border = _yy < edgeLength || _yy > corners.height() - edgeLength;
			}
			if (!border && (_yy < edgeWidth || _yy > corners.height() - edgeWidth))
			{
				border = _xx < edgeLength || _xx > corners.width() - edgeLength;
			}
			image->PutPixelImage(xx, yy, border ? sRGBFloat(1, 1, 1) : sRGBFloat(0, 0, 0));
			image->PutPixelColor(xx, yy, sRGB8(255, 255, 255));
			image->PutPixelOpacity(xx, yy, 65535);
			image->PutPixelAlpha(xx, yy, 1);
		}
	}
	QList<QRect> currentRenderededLines;
	currentRenderededLines << corners;
	image->NullPostEffect(&currentRenderededLines);
	image->CompileImage(&currentRenderededLines);
	if (image->IsPreview())
	{
		image->ConvertTo8bit(&currentRenderededLines);
		image->UpdatePreview(&currentRenderededLines);
		image->GetImageWidget()->update();
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
			QObject::tr("Cannot set OpenCL argument for %0").arg(QObject::tr("output data")),
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
			QObject::tr("Cannot set OpenCL argument for %2").arg(QObject::tr("constant data")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = kernel->setArg(3, Random(1000000)); // random seed
	if (!checkErr(err, "kernel->setArg(2, *inCLConstBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %3").arg(QObject::tr("random seed")),
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

size_t cOpenClEngineRenderFractal::CalcNeededMemory()
{
	size_t mem1 = optimalJob.sizeOfPixel * optimalJob.stepSize;
	size_t mem2 = dynamicData->GetData().size();
	return max(mem1, mem2);
}

#endif
