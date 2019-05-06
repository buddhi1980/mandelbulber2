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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Robert Pancoast (RobertPancoast77@gmail.com),
 *  Sebastian Jennen (jenzebas@gmail.com)
 *
 * cOpenClEngineRenderFractal - prepares and executes fractal rendering on opencl
 */

#include "opencl_engine_render_fractal.h"

#include <functional>

#include <QtAlgorithms>

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
#include "opencl_scheduler.h"
#include "opencl_textures_data.h"
#include "opencl_worker_output_queue.h"
#include "opencl_worker_thread.h"
#include "parameters.hpp"
#include "progress_text.hpp"
#include "rectangle.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"
#include "texture.hpp"

// custom includes
#ifdef USE_OPENCL
#include "opencl/fractal_cl.h"
#include "opencl/fractparams_cl.hpp"
#include "../opencl/mesh_export_data_cl.h"
#endif

cOpenClEngineRenderFractal::cOpenClEngineRenderFractal(cOpenClHardware *_hardware)
		: cOpenClEngine(_hardware)
{
#ifdef USE_OPENCL
	optimalJob.sizeOfPixel = sizeof(sClPixel);
	autoRefreshMode = false;
	monteCarlo = false;

	renderEngineMode = clRenderEngineTypeNone;
	meshExportMode = false;
	reservedGpuTime = 0.0;

#endif
}

cOpenClEngineRenderFractal::~cOpenClEngineRenderFractal()
{
#ifdef USE_OPENCL
	ReleaseMemory();
#endif
}

#ifdef USE_OPENCL

void cOpenClEngineRenderFractal::ReleaseMemory()
{
	constantInBuffer.reset();
	inCLConstBuffer.clear();
	constantInMeshExportBuffer.reset();
	inCLConstMeshExportBuffer.clear();
	inCLBuffer.clear();
	inCLTextureBuffer.clear();
	backgroundImage2D.clear();
	backgroungImageBuffer.reset();
	dynamicData.reset();
	texturesData.reset();
	inBuffer.clear();
	inTextureBuffer.clear();

	cOpenClEngine::ReleaseMemory();
}

// name of main kernel function used by build function
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
		clHeaderFiles.append("fractal_coloring_cl.hpp");
		clHeaderFiles.append("fractparams_cl.hpp");
		clHeaderFiles.append("fractal_sequence_cl.h");
		clHeaderFiles.append("texture_enums_cl.h");
		clHeaderFiles.append("material_cl.h");
		clHeaderFiles.append("shader_input_data_cl.h");
		clHeaderFiles.append("object_type_cl.h");
		clHeaderFiles.append("object_data_cl.h");
		clHeaderFiles.append("primitives_cl.h");
		clHeaderFiles.append("input_data_structures.h");
		clHeaderFiles.append("render_data_cl.h");

		if (meshExportMode) clHeaderFiles.append("mesh_export_data_cl.h");

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

		if (params->Get<bool>("box_folding") || params->Get<bool>("spherical_folding"))
		{
			programEngine.append("#include \"" + openclEnginePath + "basic_foldings.cl\"\n");
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

		if (renderEngineMode != clRenderEngineTypeFast)
		{
			// fractal coloring
			programEngine.append("#include \"" + openclEnginePath + "fractal_coloring.cl\"\n");
		}

		if (params->Get<bool>("fake_lights_enabled"))
		{
			// fake lights based on orbit traps - shapes
			programEngine.append("#include \"" + openclEnginePath + "orbit_trap_shape.cl\"\n");
		}

		// compute fractal
		programEngine.append("#include \"" + openclEnginePath + "compute_fractal.cl\"\n");

		if (!meshExportMode || true)
		{
			// texture mapping
			programEngine.append("#include \"" + openclEnginePath + "texture_mapping.cl\"\n");

			if (renderEngineMode != clRenderEngineTypeFast)
			{
				// calculation of texture pixel address
				programEngine.append(
					"#include \"" + openclEnginePath + "shader_texture_pixel_address.cl\"\n");

				// calculation of bicubic interpolation for textures
				programEngine.append("#include \"" + openclEnginePath + "bicubic_interpolation.cl\"\n");

				// calculate displacement from textures
				programEngine.append("#include \"" + openclEnginePath + "displacement_map.cl\"\n");
			}
		}

		// compute fractal
		programEngine.append("#include \"" + openclEnginePath + "primitives.cl\"\n");

		// calculate distance
		programEngine.append("#include \"" + openclEnginePath + "calculate_distance.cl\"\n");

		if (!meshExportMode || true)
		{
			// normal vector calculation
			programEngine.append("#include \"" + openclEnginePath + "normal_vector.cl\"\n");

			// 3D projections (3point, equirectagular, fisheye)
			programEngine.append("#include \"" + openclEnginePath + "projection_3d.cl\"\n");

			// stereoscipic rendering
			if (params->Get<bool>("stereo_enabled"))
				programEngine.append("#include \"" + openclEnginePath + "stereo.cl\"\n");

			if (renderEngineMode != clRenderEngineTypeFast)
			{
				// shaders
				programEngine.append("#include \"" + openclEnginePath + "shader_iter_opacity.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_hsv2rgb.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_background.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_surface_color.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_main_shading.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_specular_highlight.cl\"\n");
				programEngine.append(
					"#include \"" + openclEnginePath + "shader_specular_highlight_combined.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_main_shadow.cl\"\n");
				programEngine.append(
					"#include \"" + openclEnginePath + "shader_fast_ambient_occlusion.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_ambient_occlusion.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_aux_shadow.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_light_shading.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_aux_lights_shader.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_fake_lights.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_iridescence.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_texture.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_normal_map_texture.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_object.cl\"\n");
				programEngine.append(
					"#include \"" + openclEnginePath + "shader_global_illumination.cl\"\n");
				programEngine.append("#include \"" + openclEnginePath + "shader_volumetric.cl\"\n");
			}

			if (renderEngineMode == clRenderEngineTypeFull)
			{
				// ray recursion
				programEngine.append("#include \"" + openclEnginePath + "ray_recursion.cl\"\n");
			}

			if (renderEngineMode != clRenderEngineTypeFast)
			{
				// Monte Carlo DOF
				programEngine.append("#include \"" + openclEnginePath + "monte_carlo_dof.cl\"\n");
			}
		}

		// main engine
		QString engineFileName;
		if (meshExportMode)
		{
			engineFileName = "slicer_engine.cl";
		}
		else
		{
			switch (renderEngineMode)
			{
				case clRenderEngineTypeFast: engineFileName = "fast_engine.cl"; break;
				case clRenderEngineTypeLimited: engineFileName = "limited_engine.cl"; break;
				case clRenderEngineTypeFull: engineFileName = "full_engine.cl"; break;
				case clRenderEngineTypeNone: break;
			}
		}
		QString engineFullFileName = openclEnginePath + engineFileName;
		programEngine.append(LoadUtf8TextFromFile(engineFullFileName));

		qDebug() << programEngine.toStdString().c_str();
	}
	catch (const QString &ex)
	{
		qCritical() << "OpenCl program error: " << ex;
		return false;
	}

	SetUseBuildCache(!params->Get<bool>("opencl_disable_build_cache"));
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
		"cOpenClEngineRenderFractal: Opencl DOF build time [s]", timer.nsecsElapsed() / 1.0e9, 2);

	return programsLoaded;
}

void cOpenClEngineRenderFractal::SetParameters(const cParameterContainer *paramContainer,
	const cFractalContainer *fractalContainer, sParamRender *paramRender, cNineFractals *fractals,
	sRenderData *renderData, bool meshExportModeEnable)
{
	Q_UNUSED(fractalContainer);

	meshExportMode = meshExportModeEnable;
	reservedGpuTime = paramContainer->Get<double>("opencl_reserved_gpu_time");

	constantInBuffer.reset(new sClInConstants);

	if (meshExportMode)
	{
		constantInMeshExportBuffer.reset(new sClMeshExport);
	}

	definesCollector.clear();

	renderEngineMode = enumClRenderEngineMode(paramContainer->Get<int>("opencl_mode"));

	// update camera rotation data (needed for simplified calculation in opencl kernel)
	cCameraTarget cameraTarget(paramRender->camera, paramRender->target, paramRender->topVector);
	paramRender->viewAngle = cameraTarget.GetRotation() * 180.0 / M_PI;
	paramRender->resolution = 1.0 / paramRender->imageHeight;

	if (renderEngineMode == clRenderEngineTypeFull) definesCollector += " -DFULL_ENGINE";

	if (meshExportMode) definesCollector += " -DMESH_EXPORT";

	// define distance estimation method
	fractal::enumDEType deType = fractals->GetDEType(0);
	fractal::enumDEFunctionType deFunctionType = fractals->GetDEFunctionType(0);

	if (fractals->IsHybrid()) definesCollector += " -DIS_HYBRID";

	bool useAnalyticDEType = false;
	bool useDeltaDEType = false;
	bool useLinearDEFunction = false;
	bool useLogarithmicDEFunction = false;
	bool usePseudoKleinianDEFunction = false;
	bool useJosKleinianDEFunction = true;

	if (fractals->IsHybrid())
	{
		if (deType == fractal::analyticDEType)
		{
			useAnalyticDEType = true;
			switch (deFunctionType)
			{
				case fractal::linearDEFunction: definesCollector += " -DANALYTIC_LINEAR_DE"; break;
				case fractal::logarithmicDEFunction: definesCollector += " -DANALYTIC_LOG_DE"; break;
				case fractal::pseudoKleinianDEFunction:
					definesCollector += " -DANALYTIC_PSEUDO_KLEINIAN_DE";
					break;
				case fractal::josKleinianDEFunction:
					definesCollector += " -DANALYTIC_JOS_KLEINIAN_DE";
					break;
				default: break;
			}
		}
		if (deType == fractal::deltaDEType)
		{
			useDeltaDEType = true;
			switch (fractals->GetDEFunctionType(0))
			{
				case fractal::linearDEFunction: useLinearDEFunction = true; break;
				case fractal::logarithmicDEFunction: useLogarithmicDEFunction = true; break;
				case fractal::pseudoKleinianDEFunction: usePseudoKleinianDEFunction = true; break;
				case fractal::josKleinianDEFunction: useJosKleinianDEFunction = true; break;
				default: break;
			}
		}
	}
	else // is not Hybrid
	{
		for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		{
			if (!paramRender->booleanOperatorsEnabled && i == 1) break;

			if (fractals->GetDEType(i) == fractal::analyticDEType)
			{
				useAnalyticDEType = true;
			}
			else if (fractals->GetDEType(i) == fractal::deltaDEType)
			{
				useDeltaDEType = true;
				switch (fractals->GetDEFunctionType(i))
				{
					case fractal::linearDEFunction: useLinearDEFunction = true; break;
					case fractal::logarithmicDEFunction: useLogarithmicDEFunction = true; break;
					case fractal::pseudoKleinianDEFunction: usePseudoKleinianDEFunction = true; break;
					case fractal::josKleinianDEFunction: useJosKleinianDEFunction = true; break;
					default: break;
				}
			}
		}
	}

	if (useAnalyticDEType) definesCollector += " -DANALYTIC_DE";
	if (useDeltaDEType) definesCollector += " -DDELTA_DE";
	if (useLinearDEFunction) definesCollector += " -DDELTA_LINEAR_DE";
	if (useLogarithmicDEFunction) definesCollector += " -DDELTA_LOG_DE";
	if (usePseudoKleinianDEFunction) definesCollector += " -DDELTA_PSEUDO_KLEINIAN_DE";
	if (useJosKleinianDEFunction) definesCollector += " -DDELTA_JOS_KLEINIAN_DE";

	if (paramRender->limitsEnabled) definesCollector += " -DLIMITS_ENABLED";

	if (paramRender->advancedQuality) definesCollector += " -DADVANCED_QUALITY";

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

	if (paramRender->common.foldings.boxEnable) definesCollector += " -DBOX_FOLDING";
	if (paramRender->common.foldings.sphericalEnable) definesCollector += " -DSPHERICAL_FOLDING";

	bool weightUsed = false;
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		if (fractals->GetWeight(i) != 1.0)
		{
			weightUsed = true;
		}
	}
	if (weightUsed) definesCollector += " -DITERATION_WEIGHT";

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

	if (paramRender->booleanOperatorsEnabled) definesCollector += " -DBOOLEAN_OPERATORS";

	bool isVolumetricLight = false;
	for (bool i : paramRender->volumetricLightEnabled)
	{
		if (i) isVolumetricLight = true;
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

		switch (paramRender->common.fakeLightsOrbitTrapShape)
		{
			case params::fakeLightsShapePoint: definesCollector += " -DFAKE_LIGHTS_POINT"; break;
			case params::fakeLightsShapeLine: definesCollector += " -DFAKE_LIGHTS_LINE"; break;
			case params::fakeLightsShapeCircle: definesCollector += " -DFAKE_LIGHTS_CIRCLE"; break;
			case params::fakeLightsShapeSquare: definesCollector += " -DFAKE_LIGHTS_SQUARE"; break;
			case params::fakeLightsShapeSphere: definesCollector += " -DFAKE_LIGHTS_SPHERE"; break;
			case params::fakeLightsShapeCube: definesCollector += " -DFAKE_LIGHTS_CUBE"; break;
			default: definesCollector += " -DFAKE_LIGHTS_POINT"; break;
		}
	}
	if (!anyVolumetricShaderUsed) definesCollector += " -DSIMPLE_GLOW";

	if (paramRender->DOFMonteCarlo)
	{
		definesCollector += " -DMONTE_CARLO";

		if (paramRender->DOFEnabled)
		{
			definesCollector += " -DMONTE_CARLO_DOF";
		}

		definesCollector += " -DMONTE_CARLO_ANTI_ALIASING";

		if (paramRender->DOFMonteCarloGlobalIllumination)
		{
			definesCollector += " -DMONTE_CARLO_DOF_GLOBAL_ILLUMINATION";
		}
		if (paramRender->DOFMonteCarloChromaticAberration)
		{
			definesCollector += " -DCHROMATIC_ABERRATION";
		}
		if (paramRender->monteCarloSoftShadows)
		{
			definesCollector += " -DMC_SOFT_SHADOWS";
		}
	}

	if (paramRender->texturedBackground)
	{
		definesCollector += " -DTEXTURED_BACKGROUND";
		switch (paramRender->texturedBackgroundMapType)
		{
			case params::mapEquirectangular: definesCollector += " -DBACKGROUND_EQUIRECTANGULAR"; break;
			case params::mapDoubleHemisphere:
				definesCollector += " -DBACKGROUND_DOUBLE_HEMISPHERE";
				break;
			case params::mapFlat: definesCollector += " -DBACKGROUND_FLAT"; break;
		}
	}

	if (renderData->stereo.GetMode() != cStereo::stereoDisabled)
		definesCollector += " -DSTEREOSCOPIC";
	if (renderData->stereo.GetMode() == cStereo::stereoRedCyan)
		definesCollector += " -DSTEREO_REYCYAN";
	if (renderData->stereo.GetMode() == cStereo::stereoLeftRight)
		definesCollector += " -DSTEREO_LEFTRIGHT";
	if (renderData->stereo.GetMode() == cStereo::stereoTopBottom)
		definesCollector += " -DSTEREO_TOPBOTTOM";
	if (renderData->stereo.GetForcedEye() == cStereo::eyeLeft)
		definesCollector += " -DSTEREO_FORCEDEYELEFT";
	if (renderData->stereo.GetForcedEye() == cStereo::eyeRight)
		definesCollector += " -DSTEREO_FORCEDEYERIGHT";

	listOfUsedFormulas = listOfUsedFormulas.toSet().toList(); // eliminate duplicates

	WriteLogDouble("Constant buffer size [KB]", sizeof(sClInConstants) / 1024.0, 3);

	renderData->ValidateObjects();

	QMap<QString, int> textureIndexes;

	// --------------- textures dynamic data -----------------
	if (renderEngineMode == clRenderEngineTypeFull)
	{
		int numberOfTextures =
			cOpenClTexturesData::CheckNumberOfTextures(renderData->textures, renderData->materials);

		texturesData.reset(new cOpenClTexturesData(numberOfTextures));
		texturesData->ReserveHeader();
		texturesData->BuildAllTexturesData(
			renderData->textures, renderData->materials, &textureIndexes);
		texturesData->FillHeader();
		inTextureBuffer = texturesData->GetData();

		if (numberOfTextures > 0) definesCollector += " -DUSE_TEXTURES";
		definesCollector += " -DNUMBER_OF_TEXTURES=" + QString::number(numberOfTextures);

		definesCollector += texturesData->GetDefinesCollector();
	}

	//----------- create dynamic data -----------
	dynamicData.reset(new cOpenClDynamicData);
	dynamicData->ReserveHeader();

	// materials
	int materialsArraySize = dynamicData->BuildMaterialsData(renderData->materials, textureIndexes);
	definesCollector += " -DMAT_ARRAY_SIZE=" + QString::number(materialsArraySize);

	bool anyMaterialIsReflective = false;
	bool anyMaterialIsRefractive = false;
	bool anyMaterialHasSpecialColoring = false;
	bool anyMaterialHasIridescence = false;
	bool anyMaterialHasColoringEnabled = false;
	bool anyMaterialHasExtraColoringEnabled = false;
	bool anyMaterialHasTextureFractalize = false;
	bool anyMaterialHasRoughSurface = false;
	foreach (cMaterial material, renderData->materials)
	{
		if (material.reflectance > 0.0) anyMaterialIsReflective = true;
		if (material.transparencyOfSurface > 0.0) anyMaterialIsRefractive = true;
		if (material.fractalColoring.coloringAlgorithm != fractalColoring_Standard)
			anyMaterialHasSpecialColoring = true;
		if (material.iridescenceEnabled) anyMaterialHasIridescence = true;
		if (material.useColorsFromPalette) anyMaterialHasColoringEnabled = true;
		if (material.fractalColoring.extraColorEnabledFalse) anyMaterialHasExtraColoringEnabled = true;
		if (material.textureFractalize) anyMaterialHasTextureFractalize = true;
		if (material.roughSurface) anyMaterialHasRoughSurface = true;
	}
	if (anyMaterialIsReflective) definesCollector += " -DUSE_REFLECTANCE";
	if (anyMaterialIsRefractive) definesCollector += " -DUSE_REFRACTION";
	if (anyMaterialHasSpecialColoring) definesCollector += " -DUSE_COLORING_MODES";
	if (anyMaterialHasIridescence) definesCollector += " -DUSE_IRIDESCENCE";
	if (renderEngineMode != clRenderEngineTypeFast && anyMaterialHasColoringEnabled)
		definesCollector += " -DUSE_FRACTAL_COLORING";
	if (anyMaterialHasExtraColoringEnabled) definesCollector += " -DUSE_EXTRA_COLORING";
	if (anyMaterialHasRoughSurface) definesCollector += " -DUSE_ROUGH_SURFACE";

	if (((anyMaterialIsReflective || anyMaterialIsRefractive) && paramRender->raytracedReflections)
			|| paramRender->DOFMonteCarloGlobalIllumination)
	{
		definesCollector += " -DREFLECTIONS_MAX=" + QString::number(paramRender->reflectionsMax + 1);
	}
	else
	{
		paramRender->reflectionsMax = 0;
		definesCollector += " -DREFLECTIONS_MAX=1";
	}
	if (anyMaterialHasTextureFractalize) definesCollector += " -DFRACTALIZE_TEXTURE";

	// AO colored vectors
	cRenderWorker *tempRenderWorker =
		new cRenderWorker(paramRender, fractals, nullptr, renderData, nullptr);
	tempRenderWorker->PrepareAOVectors();
	sVectorsAround *AOVectors = tempRenderWorker->getAOVectorsAround();
	int numberOfVectors = tempRenderWorker->getAoVectorsCount();
	dynamicData->BuildAOVectorsData(AOVectors, numberOfVectors);
	dynamicData->BuildLightsData(&renderData->lights);

	definesCollector += dynamicData->BuildPrimitivesData(&paramRender->primitives);
	if (paramRender->primitives.GetListOfPrimitives()->size() > 0)
		definesCollector += " -DUSE_PRIMITIVES";

	dynamicData->BuildObjectsData(&renderData->objectData);
	// definesCollector += " -DOBJ_ARRAY_SIZE=" + QString::number(renderData->objectData.size());

	dynamicData->FillHeader();

	inBuffer = dynamicData->GetData();

	//---------------- another parameters -------------
	autoRefreshMode = paramContainer->Get<bool>("auto_refresh");
	monteCarlo = paramRender->DOFMonteCarlo && renderEngineMode != clRenderEngineTypeFast;

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

void cOpenClEngineRenderFractal::RegisterInputOutputBuffers(const cParameterContainer *params)
{
	Q_UNUSED(params);

	if (!meshExportMode)
	{
		outputBuffers.clear();
		for (int d = 0; d < hardware->getEnabledDevices().size(); d++)
		{
			outputBuffers.append(listOfBuffers());
			outputBuffers[d] << sClInputOutputBuffer(
				sizeof(sClPixel), optimalJob.stepSize, "output-buffer");
		}
	}
	else
	{
		// buffer for distances
		outputBuffers[0] << sClInputOutputBuffer(sizeof(float),
			constantInMeshExportBuffer->sliceHeight * constantInMeshExportBuffer->sliceWidth,
			"mesh-distances-buffer");

		// buffer for colors
		outputBuffers[0] << sClInputOutputBuffer(sizeof(float),
			constantInMeshExportBuffer->sliceHeight * constantInMeshExportBuffer->sliceWidth,
			"mesh-colors-buffer");
	}
}

bool cOpenClEngineRenderFractal::PreAllocateBuffers(const cParameterContainer *params)
{
	cOpenClEngine::PreAllocateBuffers(params);

	Q_UNUSED(params);

	cl_int err;

	for (int d = 0; d < hardware->getEnabledDevices().size(); d++)
	{
		if (hardware->ContextCreated())
		{

			inCLConstBuffer.append(QSharedPointer<cl::Buffer>(
				new cl::Buffer(*hardware->getContext(d), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
					sizeof(sClInConstants), constantInBuffer.data(), &err)));
			if (!checkErr(err,
						"cl::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, "
						"sizeof(sClInConstants), constantInBuffer, &err)"))
			{
				emit showErrorMessage(
					QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for constants")),
					cErrorMessage::errorMessage, nullptr);
				return false;
			}

			if (meshExportMode)
			{
				inCLConstMeshExportBuffer.append(QSharedPointer<cl::Buffer>(
					new cl::Buffer(*hardware->getContext(d), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						sizeof(sClMeshExport), constantInMeshExportBuffer.data(), &err)));
				if (!checkErr(err,
							"cl::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, "
							"sizeof(inCLConstMeshExportBuffer), constantInBuffer, &err)"))
				{
					emit showErrorMessage(
						QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for constants")),
						cErrorMessage::errorMessage, nullptr);
					return false;
				}
			}

			// this buffer will be used for color palettes, lights, etc...
			inCLBuffer.append(QSharedPointer<cl::Buffer>(new cl::Buffer(*hardware->getContext(d),
				CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size_t(inBuffer.size()), inBuffer.data(), &err)));
			if (!checkErr(err,
						"Buffer::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, "
						"sizeof(sClInBuff), inBuffer, &err)"))
			{
				emit showErrorMessage(
					QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for variable data")),
					cErrorMessage::errorMessage, nullptr);
				return false;
			}

			if (renderEngineMode == clRenderEngineTypeFull)
			{
				// this buffer will be used for textures
				inCLTextureBuffer.append(QSharedPointer<cl::Buffer>(
					new cl::Buffer(*hardware->getContext(d), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						size_t(inTextureBuffer.size()), inTextureBuffer.data(), &err)));
				if (!checkErr(err,
							"Buffer::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, "
							"sizeof(sClInBuff), inBuffer, &err)"))
				{
					emit showErrorMessage(
						QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("buffer for texture data")),
						cErrorMessage::errorMessage, nullptr);
					return false;
				}
			}
		}
		else
		{
			emit showErrorMessage(
				QObject::tr("OpenCL context is not ready"), cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}
	return true;
}

// Multi-threaded version of OpenCL render function
bool cOpenClEngineRenderFractal::RenderMulti(
	cImage *image, bool *stopRequest, sRenderData *renderData)
{
	bool finallResult = true; // true if rendering was compiled successfully

	if (programsLoaded)
	{
		// The image resolution determines the total amount of work
		int width = image->GetWidth();
		int height = image->GetHeight();

		// progress bat text
		cProgressText progressText;
		progressText.ResetTimer();

		// initialize status bar
		emit updateProgressAndStatus(
			tr("OpenCl - rendering image (workgroup %1 pixels)").arg(optimalJob.workGroupSize),
			progressText.getText(0.0), 0.0);
		gApplication->processEvents(); // needed to process events to quit threads

		// timer for rereshing progress bar
		QElapsedTimer progressRefreshTimer;
		progressRefreshTimer.start();

		// timer for refreshing image
		QElapsedTimer timerImageRefresh;
		timerImageRefresh.start();
		int lastRefreshTime = 1;

		// calculation of tile grid size
		qint64 numberOfPixels = qint64(width) * qint64(height);
		qint64 gridWidth = width / optimalJob.stepSizeX;
		qint64 gridHeight = height / optimalJob.stepSizeY;

		// preparation of table for noise statistics used in MC method
		const qint64 noiseTableSize = (gridWidth + 1) * (gridHeight + 1);
		double *noiseTable = new double[noiseTableSize];
		for (qint64 i = 0; i < noiseTableSize; i++)
		{
			noiseTable[i] = 0.0;
		}

		// maximum count of rendering loop
		int numberOfSamples = 1;
		if (monteCarlo)
		{
			numberOfSamples = constantInBuffer->params.DOFSamples;
		}

		// list of latest rendered tiles - needed for image refreshing
		QList<QRect> lastRenderedRects;
		QList<sRenderedTileData> listOfRenderedTilesData;

		// counters for MC statistins
		double doneMC = 0.0f;
		qint64 doneMCpixels = 0;

		// calculation of tile sequence (growing ellipse)
		QList<QPoint> tileSequence = calculateOptimalTileSequence(gridWidth + 1, gridHeight + 1);

		// writing all data to OpenCL queue
		if (!WriteBuffersToQueue()) throw;

		// writing background texture data to OpenCL queue
		if (renderEngineMode != clRenderEngineTypeFast)
		{
			if (!PrepareBufferForBackground(renderData)) throw;
		}

		// number of openCL workers equals to number of selected OpenCL devices
		int numberOfOpenCLWorkers = hardware->getEnabledDevices().size();

		// prepare list of threads
		QList<QSharedPointer<QThread>> threads;
		// prepare list of workers
		QList<QSharedPointer<cOpenClWorkerThread>> workers;

		// create scheduler
		QSharedPointer<cOpenClScheduler> scheduler(new cOpenClScheduler(&tileSequence));
		// create output FIFO buffer
		QSharedPointer<cOpenCLWorkerOutputQueue> outputQueue(new cOpenCLWorkerOutputQueue);

		// initializing and starting of all workers
		for (int d = 0; d < numberOfOpenCLWorkers; d++)
		{
			// allocating memory for threads and workers
			WriteLog(QString("Thread for OpenCL worker") + QString::number(d) + " create", 3);
			threads.append(QSharedPointer<QThread>(new QThread));
			workers.append(
				QSharedPointer<cOpenClWorkerThread>(new cOpenClWorkerThread(this, scheduler, d)));

			// pushing data to workers
			workers[d]->setImageWidth(width);
			workers[d]->setImageHeight(height);
			workers[d]->setOptimalStepX(optimalJob.stepSizeX);
			workers[d]->setOptimalStepY(optimalJob.stepSizeY);
			workers[d]->setClKernel(clKernels[d]);
			workers[d]->setClQueue(clQueues[d]);
			workers[d]->setInputAndOutputBuffers(inputAndOutputBuffers[0]); // 0 because not used
			workers[d]->setOutputBuffers(outputBuffers[d]);
			workers[d]->setOutputQueue(outputQueue);
			workers[d]->setMaxMonteCarloSamples(numberOfSamples);
			workers[d]->setStopRequest(stopRequest);
			workers[d]->setReservedGpuTime(reservedGpuTime);

			// stating threads
			workers[d]->moveToThread(threads[d].data());
			QObject::connect(
				threads[d].data(), SIGNAL(started()), workers[d].data(), SLOT(ProcessRenderingLoop()));
			QObject::connect(workers[d].data(), SIGNAL(finished()), threads[d].data(), SLOT(quit()));
			connect(workers[d].data(),
				SIGNAL(showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *)), gErrorMessage,
				SLOT(slotShowMessage(QString, cErrorMessage::enumMessageType, QWidget *)));
			threads[d]->setObjectName("OpenCLWorker #" + QString::number(d));
			threads[d]->start();
			threads[d]->setPriority(GetQThreadPriority(systemData.threadsPriority));
			WriteLog(QString("Thread ") + QString::number(d) + " started", 3);
		}

		// while loop continue condition
		bool continueWhileLoop = false;

		// counters for MC statistics
		qint64 pixelsRendered = 0; // number of
		int lastMonteCarloLoop = 1;

		// main image data collection loop
		do
		{
			// repeat loop until there is something in output queue
			while (!outputQueue->isEmpty())
			{
				// getting image data from queue
				cOpenCLWorkerOutputQueue::sClSingleOutput output = outputQueue->GetFromQueue();

				// information about tile coordinates
				qint64 jobWidth = output.jobWidth;
				qint64 jobHeight = output.jobHeight;
				qint64 jobX = output.jobX;
				qint64 jobY = output.jobY;

				pixelsRendered += jobWidth * jobHeight;

				double monteCarloNoiseSum = 0.0;
				double maxNoise = 0.0;

				// processing pixels of tile
				for (int x = 0; x < jobWidth; x++)
				{
					for (int y = 0; y < jobHeight; y++)
					{
						// getting pixel from output buffer
						sClPixel pixelCl =
							((sClPixel *)output.outputBuffers.at(outputIndex).data.data())[x + y * jobWidth];
						sRGBFloat pixel = {pixelCl.R, pixelCl.G, pixelCl.B};
						sRGB8 color = {pixelCl.colR, pixelCl.colG, pixelCl.colB};
						unsigned short opacity = pixelCl.opacity;
						unsigned short alpha = pixelCl.alpha;
						size_t xx = x + jobX;
						size_t yy = y + jobY;

						// if MC then paint pixels and calculate noise statistics
						if (monteCarlo)
						{
							// painting pixels with reduced opacity (averaging of MC samples)
							sRGBFloat oldPixel = image->GetPixelImage(xx, yy);
							sRGBFloat newPixel;
							if (output.monteCarloLoop == 1)
							{
								newPixel = pixel;
							}
							else
							{
								newPixel.R = oldPixel.R * (1.0 - 1.0 / output.monteCarloLoop)
														 + pixel.R * (1.0 / output.monteCarloLoop);
								newPixel.G = oldPixel.G * (1.0 - 1.0 / output.monteCarloLoop)
														 + pixel.G * (1.0 / output.monteCarloLoop);
								newPixel.B = oldPixel.B * (1.0 - 1.0 / output.monteCarloLoop)
														 + pixel.B * (1.0 / output.monteCarloLoop);
							}
							image->PutPixelImage(xx, yy, newPixel);
							image->PutPixelZBuffer(xx, yy, pixelCl.zBuffer);
							unsigned short oldAlpha = image->GetPixelAlpha(xx, yy);
							unsigned short newAlpha = (double)oldAlpha * (1.0 - 1.0 / output.monteCarloLoop)
																				+ alpha * (1.0 / output.monteCarloLoop);
							image->PutPixelAlpha(xx, yy, newAlpha);
							image->PutPixelColor(xx, yy, color);
							image->PutPixelOpacity(xx, yy, opacity);

							// noise estimation
							double noise = (newPixel.R - oldPixel.R) * (newPixel.R - oldPixel.R)
														 + (newPixel.G - oldPixel.G) * (newPixel.G - oldPixel.G)
														 + (newPixel.B - oldPixel.B) * (newPixel.B - oldPixel.B);
							noise *= 0.3333;

							double sumBrightness = newPixel.R + newPixel.G + newPixel.B;

							if (qIsInf(sumBrightness))
							{
								sumBrightness = 0.0;
								noise = 0.0;
								image->PutPixelImage(xx, yy, sRGBFloat());
							}

							if (sumBrightness > 1.0) noise /= (sumBrightness * sumBrightness);

							monteCarloNoiseSum += noise;
							if (noise > maxNoise) maxNoise = noise;
						}
						// if not MC then just paint pixels
						else
						{
							image->PutPixelImage(xx, yy, pixel);
							image->PutPixelZBuffer(xx, yy, pixelCl.zBuffer);
							image->PutPixelColor(xx, yy, color);
							image->PutPixelOpacity(xx, yy, opacity);
							image->PutPixelAlpha(xx, yy, alpha);
						}
					} // next y
				}		// next x

				// total noise in last rectangle
				if (monteCarlo)
				{
					double weight = 0.2;
					double totalNoiseRect =
						sqrt((1.0 - weight) * monteCarloNoiseSum / jobWidth / jobHeight + weight * maxNoise);
					noiseTable[output.gridX + output.gridY * (gridWidth + 1)] = totalNoiseRect;

					if (noiseTable[output.gridX + output.gridY * (gridWidth + 1)]
								< constantInBuffer->params.DOFMaxNoise / 100.0
							&& output.monteCarloLoop > constantInBuffer->params.DOFMinSamples)
					{
						scheduler->DisableTile(output.tileIndex);
						doneMCpixels += jobWidth * jobHeight;
						doneMC = double(doneMCpixels) / double(numberOfPixels);
					}
					lastMonteCarloLoop = output.monteCarloLoop; // needed for progress bar

					if (lastMonteCarloLoop == 1)
						renderData->statistics.numberOfRenderedPixels += jobHeight * jobWidth;
					renderData->statistics.totalNumberOfDOFRepeats += jobWidth * jobHeight;

					listOfRenderedTilesData.append(
						sRenderedTileData(jobX, jobY, jobWidth, jobHeight, totalNoiseRect));
				} // endif montecarlo
				else
				{
					listOfRenderedTilesData.append(sRenderedTileData(jobX, jobY, jobWidth, jobHeight, 0.0));
				}

				lastRenderedRects.append(SizedRectangle(jobX, jobY, jobWidth, jobHeight));

			} // while something in queue

			// refreshing progress bar and statistics (not more than once per 100ms)
			if (lastRenderedRects.size() > 0 && progressRefreshTimer.elapsed() > 100)
			{
				double percentDone;
				if (!monteCarlo)
				{
					percentDone = double(pixelsRendered) / numberOfPixels;
				}
				else
				{
					percentDone = double(pixelsRendered) / numberOfPixels / numberOfSamples;

					percentDone = percentDone * (1.0 - doneMC) + doneMC;
				}
				emit updateProgressAndStatus(
					tr("OpenCl - rendering image (workgroup %1 pixels)").arg(optimalJob.workGroupSize),
					progressText.getText(percentDone), percentDone);

				double totalNoise = 0.0;
				for (qint64 i = 0; i < noiseTableSize; i++)
				{
					totalNoise += noiseTable[i];
				}
				totalNoise /= noiseTableSize;
				renderData->statistics.totalNoise = totalNoise * width * height;

				emit updateStatistics(renderData->statistics);

				progressRefreshTimer.restart();
			}

			// refreshing image (refreshing period depends on refreshing speed)
			if (lastRenderedRects.size() > 0 && timerImageRefresh.nsecsElapsed() > lastRefreshTime * 1000)
			{
				timerImageRefresh.restart();

				image->NullPostEffect(&lastRenderedRects);
				image->CompileImage(&lastRenderedRects);
				if (image->IsPreview())
				{
					image->ConvertTo8bit(&lastRenderedRects);
					image->UpdatePreview(&lastRenderedRects);
					emit sendRenderedTilesList(listOfRenderedTilesData);
					emit updateImage();
				}
				lastRefreshTime = timerImageRefresh.nsecsElapsed() / lastRenderedRects.size();

				lastRenderedRects.clear();
				listOfRenderedTilesData.clear();
				timerImageRefresh.restart();
			}

			// processing application events
			gApplication->processEvents();

			// waiting 10ms to reduce CPU usage by main thread (no impact on rendering speed)
			Wait(10);

			// checking if continue do-while loop
			continueWhileLoop = false;
			for (int d = 0; d < numberOfOpenCLWorkers; d++)
			{
				if (threads[d]->isRunning())
				{
					continueWhileLoop = true;
				}
				else
				{
					WriteLog(QString("Thread ") + QString::number(d) + " finished", 2);
					if (!workers[d]->wasFishedWithSuccess()) finallResult = false;
				}
			}
			if (!outputQueue->isEmpty()) continueWhileLoop = true;

		} while (continueWhileLoop);

		// finall refresh of image
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
				emit updateImage();
			}
			lastRenderedRects.clear();
		}

		delete[] noiseTable;

		emit updateProgressAndStatus(
			tr("OpenCL - rendering image finished"), progressText.getText(1.0), 1.0);
	}
	else
	{
		finallResult = false;
	}

	gApplication->processEvents(); // needed to process events to quit threads

	WriteLog(QString("OpenCL rendering done"), 2);
	return finallResult;
}

QList<QPoint> cOpenClEngineRenderFractal::calculateOptimalTileSequence(
	int gridWidth, int gridHeight)
{
	QList<QPoint> tiles;
	for (int i = 0; i < gridWidth * gridHeight; i++)
	{
		tiles.append(QPoint(i % gridWidth, i / gridWidth));
	}
	qSort(tiles.begin(), tiles.end(),
		std::bind(cOpenClEngineRenderFractal::sortByCenterDistanceAsc, std::placeholders::_1,
			std::placeholders::_2, gridWidth, gridHeight));
	return tiles;
}

bool cOpenClEngineRenderFractal::sortByCenterDistanceAsc(
	const QPoint &v1, const QPoint &v2, int gridWidth, int gridHeight)
{
	// choose the tile with the lower distance to the center
	QPoint center;
	center.setX((gridWidth - 1) / 2);
	center.setY((gridHeight - 1) / 2);
	QPointF cV1 = center - v1;
	cV1.setX(cV1.x() * gridHeight / gridWidth);
	QPointF cV2 = center - v2;
	cV2.setX(cV2.x() * gridHeight / gridWidth);
	double dist2V1 = cV1.x() * cV1.x() + cV1.y() * cV1.y();
	double dist2V2 = cV2.x() * cV2.x() + cV2.y() * cV2.y();
	if (dist2V1 != dist2V2) return dist2V1 < dist2V2;

	// order tiles with same distsance clockwise
	int quartV1 = cV1.x() > 0 ? (cV1.y() > 0 ? 1 : 0) : (cV1.y() > 0 ? 2 : 3);
	int quartV2 = cV2.x() > 0 ? (cV2.y() > 0 ? 1 : 0) : (cV2.y() > 0 ? 2 : 3);
	if (quartV1 != quartV2) return quartV1 < quartV2;
	return quartV1 < 2 ? v1.y() >= v2.y() : v1.y() < v2.y();
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
		emit updateImage();
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

bool cOpenClEngineRenderFractal::AssignParametersToKernelAdditional(
	int argIterator, int deviceIndex)
{
	int err = clKernels.at(deviceIndex)
							->setArg(argIterator++, *inCLBuffer[deviceIndex]); // input data in global memory
	if (!checkErr(err, "kernel->setArg(1, *inCLBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("input data")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	if (!meshExportMode && renderEngineMode == clRenderEngineTypeFull)
	{
		int err =
			clKernels.at(deviceIndex)
				->setArg(argIterator++, *inCLTextureBuffer[deviceIndex]); // input data in global memory
		if (!checkErr(err, "kernel->setArg(1, *inCLTextureBuffer)"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("input texture data")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	err = clKernels.at(deviceIndex)
					->setArg(argIterator++,
						*inCLConstBuffer[deviceIndex]); // input data in constant memory (faster than global)
	if (!checkErr(err, "kernel->setArg(2, *inCLConstBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %2").arg(QObject::tr("constant data")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	if (meshExportMode)
	{
		err = clKernels.at(deviceIndex)
						->setArg(argIterator++,
							*inCLConstMeshExportBuffer[deviceIndex]); // input data in constant memory (faster
																												// than global)
		if (!checkErr(err, "kernel->setArg(3, *inCLConstMeshExportBuffer)"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot set OpenCL argument for %2").arg(QObject::tr("constant mesh data")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	if (renderEngineMode != clRenderEngineTypeFast && !meshExportMode)
	{
		err =
			clKernels.at(deviceIndex)
				->setArg(argIterator++,
					*backgroundImage2D[deviceIndex]); // input data in constant memory (faster than global)
		if (!checkErr(err, "kernel->setArg(3, *backgroundImage2D)"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot set OpenCL argument for %2").arg(QObject::tr("background image")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	if (!meshExportMode)
	{
		err = clKernels.at(deviceIndex)->setArg(argIterator++, Random(1000000)); // random seed
		if (!checkErr(err, "kernel->setArg(4, *inCLConstBuffer)"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot set OpenCL argument for %3").arg(QObject::tr("random seed")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	return true;
}

bool cOpenClEngineRenderFractal::WriteBuffersToQueue()
{
	cOpenClEngine::WriteBuffersToQueue();
	cl_int err = 0;

	for (int d = 0; d < hardware->getEnabledDevices().size(); d++)
	{
		err = clQueues.at(d)->enqueueWriteBuffer(
			*inCLBuffer[d], CL_TRUE, 0, inBuffer.size(), inBuffer.data());
		if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(inCLBuffer)"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot enqueue writing OpenCL %1").arg(QObject::tr("input buffers")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}

		err = clQueues.at(d)->finish();
		if (!checkErr(err, "CommandQueue::finish() - inCLBuffer"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot finish writing OpenCL %1").arg(QObject::tr("input buffers")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}

		if (renderEngineMode == clRenderEngineTypeFull)
		{
			err = clQueues.at(d)->enqueueWriteBuffer(
				*inCLTextureBuffer[d], CL_TRUE, 0, inTextureBuffer.size(), inTextureBuffer.data());
			if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(inCLTextureBuffer)"))
			{
				emit showErrorMessage(
					QObject::tr("Cannot enqueue writing OpenCL %1").arg(QObject::tr("input texture buffers")),
					cErrorMessage::errorMessage, nullptr);
				return false;
			}

			err = clQueues.at(d)->finish();
			if (!checkErr(err, "CommandQueue::finish() - inCLTextureBuffer"))
			{
				emit showErrorMessage(
					QObject::tr("Cannot finish writing OpenCL %1").arg(QObject::tr("input texture buffers")),
					cErrorMessage::errorMessage, nullptr);
				return false;
			}
		}

		err = clQueues.at(d)->enqueueWriteBuffer(
			*inCLConstBuffer[d].data(), CL_TRUE, 0, sizeof(sClInConstants), constantInBuffer.data());
		if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(inCLConstBuffer)"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot enqueue writing OpenCL %1").arg(QObject::tr("constant buffers")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}

		err = clQueues.at(d)->finish();
		if (!checkErr(err, "CommandQueue::finish() - inCLConstBuffer"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot finish writing OpenCL %1").arg(QObject::tr("constant buffers")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	if (meshExportMode)
	{
		err = clQueues.at(0)->enqueueWriteBuffer(*inCLConstMeshExportBuffer[0].data(), CL_TRUE, 0,
			sizeof(sClMeshExport), constantInMeshExportBuffer.data());
		if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(inCLConstMeshExportBuffer)"))
		{
			emit showErrorMessage(QObject::tr("Cannot enqueue writing OpenCL %1")
															.arg(QObject::tr("constant mesh export buffers")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}

		err = clQueues.at(0)->finish();
		if (!checkErr(err, "CommandQueue::finish() - inCLConstMeshExportBuffer"))
		{
			emit showErrorMessage(QObject::tr("Cannot finish writing OpenCL %1")
															.arg(QObject::tr("constant mesh export buffers")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	return true;
}

// process queue used by Mesh Export
bool cOpenClEngineRenderFractal::ProcessQueue(
	size_t jobX, size_t jobY, size_t pixelsLeftX, size_t pixelsLeftY)
{

	size_t stepSizeX = optimalJob.stepSizeX;
	if (pixelsLeftX < stepSizeX) stepSizeX = pixelsLeftX;
	size_t stepSizeY = optimalJob.stepSizeY;
	if (pixelsLeftY < stepSizeY) stepSizeY = pixelsLeftY;

	// optimalJob.stepSize = stepSize;
	cl_int err = clQueues.at(0)->enqueueNDRangeKernel(
		*clKernels.at(0), cl::NDRange(jobX, jobY), cl::NDRange(stepSizeX, stepSizeY), cl::NullRange);
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
	for (int d = 0; d < hardware->getEnabledDevices().size(); d++)
	{
		if (!cOpenClEngine::ReadBuffersFromQueue(d)) return false;
	}
	return true;
}

size_t cOpenClEngineRenderFractal::CalcNeededMemory()
{
	if (!meshExportMode)
	{
		size_t mem1 = optimalJob.sizeOfPixel * optimalJob.stepSize;
		size_t mem2 = dynamicData->GetData().size();
		return max(mem1, mem2);
	}
	else
	{
		size_t mem = 0; // TODO calculation of amout of needed memory
		return mem;
	}
}

bool cOpenClEngineRenderFractal::PrepareBufferForBackground(sRenderData *renderData)
{
	// buffer for background image

	int texWidth = renderData->textures.backgroundTexture.Width();
	int texHeight = renderData->textures.backgroundTexture.Height();

	backgroungImageBuffer.reset(new cl_uchar4[texWidth * texHeight]);
	size_t backgroundImage2DWidth = texWidth;
	size_t backgroundImage2DHeight = texHeight;

	for (int y = 0; y < texHeight; y++)
	{
		for (int x = 0; x < texWidth; x++)
		{
			sRGBA16 pixel = renderData->textures.backgroundTexture.FastPixel(x, y);
			backgroungImageBuffer[x + y * texWidth].s[0] = pixel.R / 256;
			backgroungImageBuffer[x + y * texWidth].s[1] = pixel.G / 256;
			backgroungImageBuffer[x + y * texWidth].s[2] = pixel.B / 256;
			backgroungImageBuffer[x + y * texWidth].s[3] = CL_UCHAR_MAX;
		}
	}

	for (int d = 0; d < hardware->getEnabledDevices().size(); d++)
	{
		cl_int err;
		backgroundImage2D.append(QSharedPointer<cl::Image2D>(
			new cl::Image2D(*hardware->getContext(d), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
				cl::ImageFormat(CL_RGBA, CL_UNORM_INT8), backgroundImage2DWidth, backgroundImage2DHeight,
				backgroundImage2DWidth * sizeof(cl_uchar4), backgroungImageBuffer.data(), &err)));
		if (!checkErr(err, "cl::Image2D(...backgroundImage...)")) return false;
	}
	return true;
}

// render function for mesh export
bool cOpenClEngineRenderFractal::Render(double *distances, double *colors, int sliceIndex,
	bool *stopRequest, sRenderData *renderData, size_t dataOffset)
{
	Q_UNUSED(renderData);
	Q_UNUSED(stopRequest);

	constantInMeshExportBuffer->sliceIndex = sliceIndex;

	// writing data to queue
	if (!WriteBuffersToQueue()) return false;

	// assign parameters to kernel
	if (!AssignParametersToKernel(0)) return false;

	optimalJob.stepSizeX = constantInMeshExportBuffer->sliceWidth;
	size_t pixelsLeftX = optimalJob.stepSizeX;

	optimalJob.stepSizeY = constantInMeshExportBuffer->sliceHeight;
	size_t pixelsLeftY = optimalJob.stepSizeY;

	// processing queue
	if (!ProcessQueue(0, 0, pixelsLeftX, pixelsLeftY)) return false;

	if (!ReadBuffersFromQueue()) return false;

	for (int y = 0; y < constantInMeshExportBuffer->sliceHeight; y++)
	{
		for (int x = 0; x < constantInMeshExportBuffer->sliceWidth; x++)
		{
			size_t address = x + y * constantInMeshExportBuffer->sliceWidth;
			double distance = (reinterpret_cast<cl_float *>(
				outputBuffers[0][outputMeshDistancesIndex].ptr.data()))[address];
			distances[address + dataOffset] = distance;

			if (colors)
			{
				double color = (reinterpret_cast<cl_float *>(
					outputBuffers[0][outputMeshColorsIndex].ptr.data()))[address];
				colors[address + dataOffset] = color;
			}
		}
	}

	return true;
}

void cOpenClEngineRenderFractal::SetMeshExportParameters(const sClMeshExport *meshParams)
{
	*constantInMeshExportBuffer = *meshParams;
	if (meshParams->coloredMesh)
	{
		definesCollector += " -DMESH_EXPORT_COLOR";
	}
}

#endif
