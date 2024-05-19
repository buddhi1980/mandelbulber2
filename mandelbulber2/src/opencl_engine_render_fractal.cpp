/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include <map>
#include <memory>

#include <QtAlgorithms>
#include <QtConcurrent>

#include "camera_target.hpp"
#include "cimage.hpp"
#include "common_math.h"
#include "denoiser.h"
#include "files.h"
#include "fractal.h"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "material.h"
#include "netrender.hpp"
#include "nine_fractals.hpp"
#include "opencl_dynamic_data.hpp"
#include "opencl_hardware.h"
#include "opencl_scheduler.h"
#include "opencl_textures_data.h"
#include "opencl_worker_output_queue.h"
#include "opencl_worker_thread.h"
#include "parameters.hpp"
#include "perlin_noise_octaves.h"
#include "progress_text.hpp"
#include "rectangle.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"
#include "system_data.hpp"
#include "system_directories.hpp"
#include "texture.hpp"
#include "wait.hpp"
#include "write_log.hpp"

#include "formula/definition/all_fractal_list.hpp"
#include "formula/definition/legacy_fractal_transforms.hpp"

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
	distanceMode = false;
	useOptionalImageChannels = false;
	reservedGpuTime = 0.0;

	// create empty list of custom formulas
	customFormulaCodes.reserve(NUMBER_OF_FRACTALS);
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		customFormulaCodes.append(QString());
	}
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
	backgroundImageBuffer.clear();
	dynamicData.reset();
	texturesData.reset();
	inBuffer.clear();
	inTextureBuffer.clear();
	perlinNoiseSeeds.clear();
	inCLPerlinNoiseSeedsBuffer.clear();

	cOpenClEngine::ReleaseMemory();
	distanceMode = false;
}

// name of main kernel function used by build function
QString cOpenClEngineRenderFractal::GetKernelName()
{
	return QString("fractal3D");
}

void cOpenClEngineRenderFractal::CreateListOfHeaderFiles(QStringList &clHeaderFiles)
{
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
	clHeaderFiles.append("light_cl.h");
	clHeaderFiles.append("render_data_cl.h");
}

void cOpenClEngineRenderFractal::CreateListOfIncludes(const QStringList &clHeaderFiles,
	const QString &openclPathSlash, std::shared_ptr<const cParameterContainer> params,
	const QString &openclEnginePath, QByteArray &programEngine)
{
	for (int i = 0; i < clHeaderFiles.size(); i++)
	{
		AddInclude(programEngine, openclPathSlash + clHeaderFiles.at(i));
	}
	if (params->Get<bool>("box_folding") || params->Get<bool>("spherical_folding"))
	{
		AddInclude(programEngine, openclEnginePath + "basic_foldings.cl");
	}
	// fractal formulas - only actually used
	for (int i = 0; i < listOfUsedFormulas.size(); i++)
	{
		QString formulaName = listOfUsedFormulas.at(i);
		if (formulaName != "" && formulaName != "none")
		{
			if (formulaName.startsWith("custom"))
			{
				AddInclude(programEngine,
					systemDirectories.GetOpenCLTempFolder() + QDir::separator() + formulaName + ".cl");
			}
			else
			{
				AddInclude(programEngine, systemDirectories.sharedDir + "formula" + QDir::separator()
																		+ "opencl" + QDir::separator() + formulaName + ".cl");
			}
		}
	}
	if (renderEngineMode != clRenderEngineTypeFast)
	{
		// fractal coloring
		AddInclude(programEngine, openclEnginePath + "fractal_coloring.cl");
	}
	if (params->Get<bool>("fake_lights_enabled"))
	{
		// fake lights based on orbit traps - shapes
		AddInclude(programEngine, openclEnginePath + "orbit_trap_shape.cl");
	}
	// compute fractal
	AddInclude(programEngine, openclEnginePath + "compute_fractal.cl");
	if (!distanceMode)
	{
		// texture mapping
		AddInclude(programEngine, openclEnginePath + "texture_mapping.cl");
		if (renderEngineMode != clRenderEngineTypeFast)
		{
			// calculation of texture pixel address
			AddInclude(programEngine, openclEnginePath + "shader_texture_pixel_address.cl");
			// calculation of bicubic interpolation for textures
			AddInclude(programEngine, openclEnginePath + "bicubic_interpolation.cl");
			// calculate displacement from textures
			AddInclude(programEngine, openclEnginePath + "displacement_map.cl");

			AddInclude(programEngine, openclEnginePath + "perlin_noise.cl");

			AddInclude(programEngine, openclEnginePath + "shader_perlin_noise_for_shaders.cl");
		}
	}
	// compute fractal
	AddInclude(programEngine, openclEnginePath + "primitives.cl");
	// calculate distance
	AddInclude(programEngine, openclEnginePath + "calculate_distance.cl");
	if (!distanceMode)
	{
		// normal vector calculation
		AddInclude(programEngine, openclEnginePath + "normal_vector.cl");
		// 3D projections (3point, equirectagular, fisheye)
		AddInclude(programEngine, openclEnginePath + "projection_3d.cl");
		// stereoscipic rendering
		if (params->Get<bool>("stereo_enabled"))
			AddInclude(programEngine, openclEnginePath + "stereo.cl");

		if (renderEngineMode != clRenderEngineTypeFast)
		{
			// shaders
			AddInclude(programEngine, openclEnginePath + "shader_iter_opacity.cl");
			AddInclude(programEngine, openclEnginePath + "shader_distance_fog_opacity.cl");
			AddInclude(programEngine, openclEnginePath + "shader_clouds_opacity.cl");
			AddInclude(programEngine, openclEnginePath + "shader_hsv2rgb.cl");
			AddInclude(programEngine, openclEnginePath + "shader_background.cl");
			AddInclude(programEngine, openclEnginePath + "shader_surface_color.cl");
			AddInclude(programEngine, openclEnginePath + "shader_specular_highlight.cl");
			AddInclude(programEngine, openclEnginePath + "shader_specular_highlight_combined.cl");
			AddInclude(programEngine, openclEnginePath + "shader_fast_ambient_occlusion.cl");
			AddInclude(programEngine, openclEnginePath + "shader_ambient_occlusion.cl");
			AddInclude(programEngine, openclEnginePath + "shader_texture.cl");
			AddInclude(programEngine, openclEnginePath + "shader_aux_shadow.cl");
			AddInclude(programEngine, openclEnginePath + "shader_light_shading.cl");
			AddInclude(programEngine, openclEnginePath + "shader_aux_lights_shader.cl");
			AddInclude(programEngine, openclEnginePath + "shader_fake_lights.cl");
			AddInclude(programEngine, openclEnginePath + "shader_iridescence.cl");
			AddInclude(programEngine, openclEnginePath + "shader_normal_map_texture.cl");
			AddInclude(programEngine, openclEnginePath + "shader_roughness_texture.cl");
			AddInclude(programEngine, openclEnginePath + "shader_fresnel.cl");
			AddInclude(programEngine, openclEnginePath + "shader_env_mapping.cl");
			AddInclude(programEngine, openclEnginePath + "shader_object.cl");
			if (params->Get<bool>("MC_fog_illumination"))
			{
				AddInclude(programEngine, openclEnginePath + "shader_global_illumination.cl");
				AddInclude(programEngine, openclEnginePath + "shader_volumetric.cl");
			}
			else
			{
				AddInclude(programEngine, openclEnginePath + "shader_volumetric.cl");
				AddInclude(programEngine, openclEnginePath + "shader_global_illumination.cl");
			}
		}
		if (renderEngineMode == clRenderEngineTypeFull)
		{
			// ray recursion
			AddInclude(programEngine, openclEnginePath + "ray_recursion.cl");
		}
		if (renderEngineMode != clRenderEngineTypeFast)
		{
			// Monte Carlo DOF
			AddInclude(programEngine, openclEnginePath + "monte_carlo_dof.cl");
		}
	}
}

void cOpenClEngineRenderFractal::LoadSourceWithMainEngine(
	const QString &openclEnginePath, QByteArray &programEngine)
{
	// main engine
	QString engineFileName;
	if (meshExportMode)
	{
		engineFileName = "slicer_engine.cl";
	}
	else if (distanceMode)
	{
		engineFileName = "distance_engine.cl";
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

	// adding hash code for custom formulas to the end of code to force recompile
	QCryptographicHash hashCryptProgram(QCryptographicHash::Md4);
	for (QString code : customFormulaCodes)
	{
		hashCryptProgram.addData(code.toUtf8());
	}
	QByteArray hashCustomPrograms = hashCryptProgram.result();
	QString hashText = QString("//%1").arg(QString(hashCustomPrograms.toHex()));
	programEngine.append(hashText.toUtf8());
}

bool cOpenClEngineRenderFractal::LoadSourcesAndCompile(
	std::shared_ptr<const cParameterContainer> params, QString *compilerErrorOutput)
{
	programsLoaded = false;
	readyForRendering = false;

	emit updateProgressAndStatus(tr("OpenCl - initializing"), tr("Compiling OpenCL programs"), 0.0);

	QByteArray programEngine;
	try
	{
		QString openclPath = systemDirectories.sharedDir + "opencl" + QDir::separator();
		QString openclEnginePath = openclPath + "engines" + QDir::separator();

		QStringList clHeaderFiles;
		CreateListOfHeaderFiles(clHeaderFiles);

		if (meshExportMode) clHeaderFiles.append("mesh_export_data_cl.h");

		// pass through define constants
		programEngine.append("#define USE_OPENCL 1\n");

		if (params->Get<bool>("opencl_precision"))
			programEngine.append(
				QString("#define DOUBLE_PRECISION " + QString::number(1) + "\n").toUtf8());

#ifdef _WIN32
		QString openclPathSlash = openclPath.replace("/", "\\"); // replace single slash with backslash
#else
		QString openclPathSlash = openclPath;
#endif

		CreateListOfIncludes(clHeaderFiles, openclPathSlash, params, openclEnginePath, programEngine);

		// main engine
		LoadSourceWithMainEngine(openclEnginePath, programEngine);

		// qDebug() << programEngine.toStdString().c_str();
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
	bool quiet = (compilerErrorOutput) ? true : false;

	QElapsedTimer timer;
	timer.start();
	if (Build(programEngine, &errorString, quiet))
	{
		programsLoaded = true;
	}
	else
	{
		programsLoaded = false;
		WriteLog(errorString, 0);
	}

	if (compilerErrorOutput) *compilerErrorOutput = errorString;

	WriteLogDouble(
		"cOpenClEngineRenderFractal: Opencl kernel build time [s]", timer.nsecsElapsed() / 1.0e9, 2);

	return programsLoaded;
}

void cOpenClEngineRenderFractal::SetParametersForDistanceEstimationMethod(
	cNineFractals *fractals, sParamRender *paramRender)
{
	// define distance estimation method
	fractal::enumDEType deType = fractals->GetDEType(0);
	fractal::enumDEFunctionType deFunctionType = fractals->GetDEFunctionType(0);
	if (fractals->IsHybrid()) definesCollector += " -DIS_HYBRID";

	bool useAnalyticDEType = false;
	bool useDeltaDEType = false;
	bool useLinearDEFunction = false;
	bool useLogarithmicDEFunction = false;
	bool usePseudoKleinianDEFunction = false;
	bool useJosKleinianDEFunction = false;
	bool useCustomDEFunction = false;
	bool useMaxAxisDEFunction = false;
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
				case fractal::customDEFunction: definesCollector += " -DANALYTIC_CUSTOM_DE"; break;
				case fractal::maxAxisDEFunction: definesCollector += " -DANALYTIC_MAXAXIS_DE"; break;
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
				case fractal::customDEFunction: useCustomDEFunction = true; break;
				case fractal::maxAxisDEFunction: useMaxAxisDEFunction = true; break;
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
					case fractal::customDEFunction: useCustomDEFunction = true; break;
					case fractal::maxAxisDEFunction: useMaxAxisDEFunction = true; break;
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

	if (useCustomDEFunction) definesCollector += " -DDELTA_DIFS_DE";

	if (useMaxAxisDEFunction) definesCollector += " -DDELTA_MAXAXIS_DE";
}

void cOpenClEngineRenderFractal::CreateListOfUsedFormulas(
	cNineFractals *fractals, std::shared_ptr<const cFractalContainer> fractalContainer)
{
	listOfUsedFormulas.clear();
	// creating list of used formulas
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractal::enumFractalFormula fractalFormula = fractals->GetFractal(i)->formula;
		int listIndex = cNineFractals::GetIndexOnFractalList(fractalFormula);
		QString formulaName = newFractalList.at(listIndex)->getInternalName();
		if (formulaName == "custom")
		{
			formulaName += QString::number(i);
			QString formulaCode = fractalContainer->at(i)->Get<QString>("formula_code");

			if (formulaCode.contains("CustomIteration("))
			{
				formulaCode = formulaCode.replace("CustomIteration", QString("Custom%1Iteration").arg(i));
				QFile qFile(
					systemDirectories.GetOpenCLTempFolder() + QDir::separator() + formulaName + ".cl");
				if (qFile.open(QIODevice::WriteOnly))
				{
					qFile.write(formulaCode.toUtf8());
					qFile.close();
				}
			}
			else
			{
				emit showErrorMessage(
					QObject::tr("Custom formula %1 has missing function name CustomIteration()!").arg(i),
					cErrorMessage::errorMessage, nullptr);
			}
			customFormulaCodes[i] = formulaCode;
		}
		else
		{
			customFormulaCodes[i] = QString();
		}
		listOfUsedFormulas.append(formulaName);
	}
	// adding #defines to the list
	for (int i = 0; i < listOfUsedFormulas.size(); i++)
	{
		QString internalID = toCamelCase(listOfUsedFormulas.at(i));
		if (internalID != "" && internalID != "None")
		{
			QString functionName = internalID.left(1).toUpper() + internalID.mid(1) + "Iteration";
			definesCollector += " -DFORMULA_ITER_" + QString::number(i) + "=" + functionName;
		}
	}

	listOfUsedFormulas.removeDuplicates(); // eliminate duplicates
}

void cOpenClEngineRenderFractal::SetParametersForPerspectiveProjection(sParamRender *paramRender)
{
	// ---- perspective projections ------
	switch (paramRender->perspectiveType)
	{
		case params::perspThreePoint: definesCollector += " -DPERSP_THREE_POINT"; break;
		case params::perspEquirectangular: definesCollector += " -DPERSP_EQUIRECTANGULAR"; break;
		case params::perspFishEye: definesCollector += " -DPERSP_FISH_EYE"; break;
		case params::perspFishEyeCut: definesCollector += " -DPERSP_FISH_EYE_CUT"; break;
	}
}

void cOpenClEngineRenderFractal::SetParametersForShaders(
	sParamRender *paramRender, sRenderData *renderData)
{
	// ------------ enabling shaders ----------
	bool anyVolumetricShaderUsed = false;

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

		bool anyLightVolumetric = false;
		bool anyLightVisible = false;
		bool anyLightCastShadows = false;
		bool anyLightConical = false;

		for (int l = 0; l < renderData->lights.GetNumberOfLights(); l++)
		{
			const cLight *light = renderData->lights.GetLight(l);

			if (light->enabled)
			{
				if (light->volumetric)
				{
					anyLightVolumetric = true;
					anyVolumetricShaderUsed = true;
				}

				if (light->visibility > 0.0 && light->type != cLight::lightDirectional)
				{
					anyLightVisible = true;
					anyVolumetricShaderUsed = true;
				}

				if (light->castShadows)
				{
					anyLightCastShadows = true;
				}

				if (light->type == cLight::lightConical)
				{
					anyLightConical = true;
				}
			}
		}

		if (anyLightVolumetric) definesCollector += " -DVOLUMETRIC_LIGHTS";
		if (anyLightVisible) definesCollector += " -DVISIBLE_AUX_LIGHTS";
		if (anyLightCastShadows) definesCollector += " -DSHADOWS";
		if (anyLightConical) definesCollector += " -DLIGHT_CONICAL";
	}

	if (paramRender->glowEnabled) definesCollector += " -DGLOW";

	if (paramRender->fogEnabled)
	{
		definesCollector += " -DBASIC_FOG";
		anyVolumetricShaderUsed = true;
		if (paramRender->fogCastShadows) definesCollector += " -DBASIC_FOG_CAST_SHADOWS";
	}
	if (paramRender->volFogEnabled && paramRender->volFogDensity > 0)
	{
		definesCollector += " -DVOLUMETRIC_FOG";
		anyVolumetricShaderUsed = true;
		if (paramRender->distanceFogShadows == true)
		{
			definesCollector += " -DDIST_FOG_SHADOWS";
		}
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

	if (paramRender->cloudsEnable)
	{
		definesCollector += " -DCLOUDS";
		anyVolumetricShaderUsed = true;

		if (paramRender->cloudsCastShadows)
		{
			definesCollector += " -DCLOUDSSHADOWS";
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
		if (paramRender->monteCarloGIVolumetric)
		{
			definesCollector += " -DMC_GI_VOLUMETRIC";
		}
		if (paramRender->monteCarloGIOfVolumetric)
		{
			definesCollector += " -DMC_GI_FOG_ILLUMINATION";
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

	if (paramRender->antialiasingEnabled && !paramRender->DOFMonteCarlo)
	{
		definesCollector += " -DANTIALIASING";
	}

	if (paramRender->envMappingEnable && renderEngineMode == clRenderEngineTypeFull)
	{
		definesCollector += " -DUSE_ENV_MAPPING";
	}

	if (paramRender->rayleighScatteringBlue > 0.0 || paramRender->rayleighScatteringRed > 0.0)
		definesCollector += " -DRAYLEIGH_SCATTERING";
}

void cOpenClEngineRenderFractal::SetParametersForStereoscopic(sRenderData *renderData)
{
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
}

QMap<QString, int> cOpenClEngineRenderFractal::SetParametersAndDataForTextures(
	sRenderData *renderData)
{
	QMap<QString, int> textureIndexes;
	// --------------- textures dynamic data -----------------
	if (renderEngineMode == clRenderEngineTypeFull)
	{
		WriteLog(QString("Creating dynamic data for textures"), 2);
		int numberOfTextures = cOpenClTexturesData::CheckNumberOfTextures(
			renderData->textures, renderData->materials, renderData->lights);
		texturesData.reset(new cOpenClTexturesData(numberOfTextures));
		texturesData->ReserveHeader();
		texturesData->BuildAllTexturesData(
			renderData->textures, renderData->materials, renderData->lights, &textureIndexes);
		texturesData->FillHeader();

		inTextureBuffer = texturesData->GetData();
		if (numberOfTextures > 0) definesCollector += " -DUSE_TEXTURES";

		definesCollector += " -DNUMBER_OF_TEXTURES=" + QString::number(numberOfTextures);
		definesCollector += texturesData->GetDefinesCollector();
		WriteLogInt(QString("Created dynamic data for textures"), inTextureBuffer.size(), 2);
	}
	return textureIndexes;
}

void cOpenClEngineRenderFractal::SetParametersAndDataForMaterials(
	const QMap<QString, int> &textureIndexes, sRenderData *renderData, sParamRender *paramRender)
{
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
	bool anyMaterialHasInnerColoring = false;
	bool anyMaterialHasSubsurfaceScattering = false;

	bool anyMaterialHasSurfaceGradient = false;
	bool anyMaterialHasSpecularGradient = false;
	bool anyMaterialHasDiffuseGradient = false;
	bool anyMaterialHasLuminosityGradient = false;
	bool anyMaterialHasRoughnessGradient = false;
	bool anyMaterialHasReflectanceGradient = false;
	bool anyMaterialHasTransparencyGradient = false;
	bool anyMaterialHasPerlin = false;

	for (auto const &materialPair : renderData->materials) // for each material from materials
	{
		cMaterial const &material = materialPair.second;

		if (material.reflectance > 0.0f) anyMaterialIsReflective = true;
		if (material.transparencyOfSurface > 0.0f) anyMaterialIsRefractive = true;
		if (material.fractalColoring.coloringAlgorithm != fractalColoring_Standard)
			anyMaterialHasSpecialColoring = true;
		if (material.iridescenceEnabled) anyMaterialHasIridescence = true;
		if (material.useColorsFromPalette) anyMaterialHasColoringEnabled = true;
		if (material.fractalColoring.extraColorEnabledFalse) anyMaterialHasExtraColoringEnabled = true;
		if (material.textureFractalize) anyMaterialHasTextureFractalize = true;
		if (material.roughSurface) anyMaterialHasRoughSurface = true;
		if (material.insideColoringEnable) anyMaterialHasInnerColoring = true;
		if (material.subsurfaceScattering) anyMaterialHasSubsurfaceScattering = true;

		if (material.surfaceGradientEnable) anyMaterialHasSurfaceGradient = true;
		if (material.specularGradientEnable) anyMaterialHasSpecularGradient = true;
		if (material.diffuseGradientEnable) anyMaterialHasDiffuseGradient = true;
		if (material.luminosityGradientEnable) anyMaterialHasLuminosityGradient = true;
		if (material.roughnessGradientEnable) anyMaterialHasRoughnessGradient = true;
		if (material.reflectanceGradientEnable) anyMaterialHasReflectanceGradient = true;
		if (material.transparencyGradientEnable) anyMaterialHasTransparencyGradient = true;
		if (material.perlinNoiseEnable) anyMaterialHasPerlin = true;
	}
	if (anyMaterialIsReflective) definesCollector += " -DUSE_REFLECTANCE";

	if (anyMaterialIsRefractive) definesCollector += " -DUSE_REFRACTION";

	if (anyMaterialHasSpecialColoring) definesCollector += " -DUSE_COLORING_MODES";

	if (anyMaterialHasIridescence) definesCollector += " -DUSE_IRIDESCENCE";

	if (renderEngineMode != clRenderEngineTypeFast && anyMaterialHasColoringEnabled)
		definesCollector += " -DUSE_FRACTAL_COLORING";

	if (anyMaterialHasExtraColoringEnabled) definesCollector += " -DUSE_EXTRA_COLORING";

	if (anyMaterialHasRoughSurface) definesCollector += " -DUSE_ROUGH_SURFACE";

	if (anyMaterialHasInnerColoring) definesCollector += " -DUSE_INNER_COLORING";

	if (anyMaterialHasSubsurfaceScattering) definesCollector += " -DUSE_SUBSURFACE_SCATTERING";

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

	if (anyMaterialHasSurfaceGradient) definesCollector += " -DUSE_SURFACE_GRADIENT";
	if (anyMaterialHasSpecularGradient) definesCollector += " -DUSE_SPECULAR_GRADIENT";
	if (anyMaterialHasDiffuseGradient) definesCollector += " -DUSE_DIFFUSE_GRADIENT";
	if (anyMaterialHasLuminosityGradient) definesCollector += " -DUSE_LUMINOSITY_GRADIENT";
	if (anyMaterialHasRoughnessGradient) definesCollector += " -DUSE_ROUGHNESS_GRADIENT";
	if (anyMaterialHasReflectanceGradient) definesCollector += " -DUSE_REFLECTANCE_GRADIENT";
	if (anyMaterialHasTransparencyGradient) definesCollector += " -DUSE_TRANSPARENCY_GRADIENT";

	if (renderEngineMode != clRenderEngineTypeFast && anyMaterialHasPerlin)
		definesCollector += " -DUSE_PERLIN_NOISE";
}

void cOpenClEngineRenderFractal::DynamicDataForAOVectors(
	std::shared_ptr<const sParamRender> paramRender, std::shared_ptr<const cNineFractals> fractals,
	std::shared_ptr<sRenderData> renderData)
{
	// AO colored vectors
	std::unique_ptr<cRenderWorker> tempRenderWorker(
		new cRenderWorker(paramRender, fractals, nullptr, renderData, nullptr));
	tempRenderWorker->PrepareAOVectors();
	const sVectorsAround *AOVectors = tempRenderWorker->getAOVectorsAround();
	int numberOfVectors = tempRenderWorker->getAoVectorsCount();
	dynamicData->BuildAOVectorsData(AOVectors, numberOfVectors);
}

void cOpenClEngineRenderFractal::SetParametersForIterationWeight(cNineFractals *fractals)
{
	bool weightUsed = false;
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		if (fractals->GetWeight(i) != 1.0)
		{
			weightUsed = true;
		}
	}
	if (weightUsed) definesCollector += " -DITERATION_WEIGHT";
}

void cOpenClEngineRenderFractal::SetParameters(
	std::shared_ptr<const cParameterContainer> paramContainer,
	std::shared_ptr<const cFractalContainer> fractalContainer,
	std::shared_ptr<sParamRender> paramRender, std::shared_ptr<cNineFractals> fractals,
	std::shared_ptr<sRenderData> renderData, bool meshExportModeEnable)
{
	Q_UNUSED(fractalContainer);

	WriteLog(QString("Setting parameters for OpenCL rendering"), 2);

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
	if (distanceMode) definesCollector += " -DDISTANCE_CALCULATION_MODE";
	if (paramRender->limitsEnabled) definesCollector += " -DLIMITS_ENABLED";
	if (paramRender->advancedQuality) definesCollector += " -DADVANCED_QUALITY";
	if (paramContainer->Get<bool>("optional_image_channels_enabled"))
		definesCollector += " -DOPTIONAL_IMAGE_CHANNELS";

	// define distance estimation method
	SetParametersForDistanceEstimationMethod(fractals.get(), paramRender.get());

	CreateListOfUsedFormulas(fractals.get(), fractalContainer);

	if (paramRender->common.foldings.boxEnable) definesCollector += " -DBOX_FOLDING";
	if (paramRender->common.foldings.sphericalEnable) definesCollector += " -DSPHERICAL_FOLDING";
	if (paramRender->interiorMode) definesCollector += " -DINTERIOR_MODE";
	if (paramRender->booleanOperatorsEnabled) definesCollector += " -DBOOLEAN_OPERATORS";

	SetParametersForIterationWeight(fractals.get());

	// ---- perspective projections ------
	SetParametersForPerspectiveProjection(paramRender.get());

	//----------- create dynamic data -----------
	WriteLog(QString("Creating dynamic data for OpenCL rendering"), 2);
	dynamicData.reset(new cOpenClDynamicData);
	dynamicData->ReserveHeader();

	// ------------ enabling shaders ----------
	if (renderData)
	{
		SetParametersForShaders(paramRender.get(), renderData.get());

		SetParametersForStereoscopic(renderData.get());

		WriteLogDouble("Constant buffer size [KB]", sizeof(sClInConstants) / 1024.0, 2);

		//---------- DYNAMIC DATA -------------

		renderData->ValidateObjects();

		QMap<QString, int> textureIndexes = SetParametersAndDataForTextures(renderData.get());

		// materials
		SetParametersAndDataForMaterials(textureIndexes, renderData.get(), paramRender.get());

		// AO colored vectors
		DynamicDataForAOVectors(paramRender, fractals, renderData);

		// random lights
		dynamicData->BuildLightsData(&renderData->lights, textureIndexes);
	}

	definesCollector += dynamicData->BuildPrimitivesData(&paramRender->primitives);
	if (paramRender->primitives.GetNumberOfPrimivives() > 0) definesCollector += " -DUSE_PRIMITIVES";

	if (renderData)
	{
		dynamicData->BuildObjectsData(&renderData->objectData);
		// definesCollector += " -DOBJ_ARRAY_SIZE=" + QString::number(renderData->objectData.size());
	}

	dynamicData->FillHeader();

	inBuffer = dynamicData->GetData();
	WriteLogInt(QString("Created dynamic data for OpenCL rendering"), inBuffer.size(), 2);

	//---------------- another parameters -------------
	autoRefreshMode = paramContainer->Get<bool>("auto_refresh");
	bool antiAliasing =
		paramRender->antialiasingEnabled && renderEngineMode == clRenderEngineTypeFull;
	monteCarlo =
		(paramRender->DOFMonteCarlo && renderEngineMode != clRenderEngineTypeFast) || antiAliasing;

	// copy all cl parameters to constant buffer
	constantInBuffer->params = clCopySParamRenderCl(*paramRender);

	constantInBuffer->params.viewAngle = toClFloat3(paramRender->viewAngle * M_PI / 180.0);

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		constantInBuffer->fractal[i] = clCopySFractalCl(*fractals->GetFractal(i));
	}

	// buffer for Perlin noise seeds
	perlinNoiseSeeds.resize(perlinNoiseArraySize);
	std::unique_ptr<cPerlinNoiseOctaves> perlinNoise(
		new cPerlinNoiseOctaves(paramRender->cloudsRandomSeed));
	std::uint8_t *seeds = perlinNoise->GetSeeds();
	for (int i = 0; i < perlinNoiseArraySize; i++)
	{
		perlinNoiseSeeds[i] = seeds[i];
	}

	useOptionalImageChannels = paramContainer->Get<bool>("optional_image_channels_enabled");

	fractals->CopyToOpenclData(&constantInBuffer->sequence);
}

void cOpenClEngineRenderFractal::RegisterInputOutputBuffers(
	std::shared_ptr<const cParameterContainer> params)
{
	Q_UNUSED(params);

	if (meshExportMode)
	{
		// buffer for distances
		outputBuffers[0] << sClInputOutputBuffer(sizeof(float),
			constantInMeshExportBuffer->sliceHeight * constantInMeshExportBuffer->sliceWidth,
			"mesh-distances-buffer");

		// buffer for colors
		outputBuffers[0] << sClInputOutputBuffer(sizeof(float),
			constantInMeshExportBuffer->sliceHeight * constantInMeshExportBuffer->sliceWidth,
			"mesh-colors-buffer");

		// buffer for colors
		outputBuffers[0] << sClInputOutputBuffer(sizeof(int),
			constantInMeshExportBuffer->sliceHeight * constantInMeshExportBuffer->sliceWidth,
			"mesh-iterations-buffer");
	}
	else if (distanceMode)
	{
		// buffer for distances
		outputBuffers[0] << sClInputOutputBuffer(sizeof(cl_float), 1, "distance-buffer");
	}
	else
	{
		outputBuffers.clear();
		for (int d = 0; d < hardware->getEnabledDevices().size(); d++)
		{
			outputBuffers.append(listOfBuffers());
			outputBuffers[d] << sClInputOutputBuffer(
				sizeof(sClPixel), optimalJob.stepSize, "output-buffer");
		}
	}
}

bool cOpenClEngineRenderFractal::PreAllocateBuffers(
	std::shared_ptr<const cParameterContainer> params)
{
	cOpenClEngine::PreAllocateBuffers(params);

	Q_UNUSED(params);

	cl_int err;

	for (int d = 0; d < hardware->getEnabledDevices().size(); d++)
	{
		if (hardware->ContextCreated())
		{
			WriteLog(QString("Allocating OpenCL buffer for constants"), 2);

			inCLConstBuffer.append(std::shared_ptr<cl::Buffer>(
				new cl::Buffer(*hardware->getContext(d), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
					sizeof(sClInConstants), constantInBuffer.get(), &err)));
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
				WriteLog(QString("Allocating OpenCL buffer for mesh constants"), 2);

				inCLConstMeshExportBuffer.append(std::shared_ptr<cl::Buffer>(
					new cl::Buffer(*hardware->getContext(d), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						sizeof(sClMeshExport), constantInMeshExportBuffer.get(), &err)));
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
			WriteLog(QString("Allocating OpenCL buffer for dynamic data"), 2);

			inCLBuffer.append(std::shared_ptr<cl::Buffer>(new cl::Buffer(*hardware->getContext(d),
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

			if (renderEngineMode == clRenderEngineTypeFull && !distanceMode)
			{
				WriteLog(QString("Allocating OpenCL buffer for texture data"), 2);

				// this buffer will be used for textures
				inCLTextureBuffer.append(std::shared_ptr<cl::Buffer>(
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

			if (renderEngineMode != clRenderEngineTypeFast && !meshExportMode && !distanceMode)
			{
				// this buffer will be used for Perlin noise seeds.
				WriteLog(QString("Allocating OpenCL buffer for perlin noise seeds"), 2);

				inCLPerlinNoiseSeedsBuffer.append(std::shared_ptr<cl::Buffer>(
					new cl::Buffer(*hardware->getContext(d), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						size_t(perlinNoiseSeeds.size() * sizeof(cl_uchar)), perlinNoiseSeeds.data(), &err)));
				if (!checkErr(err,
							"Buffer::Buffer(*hardware->getContext(), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, "
							"sizeof(perlinNoiseSeeds), perlinNoiseSeeds, &err)"))
				{
					emit showErrorMessage(QObject::tr("OpenCL %1 cannot be created!")
																	.arg(QObject::tr("buffer for perlin noise seeds")),
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

void cOpenClEngineRenderFractal::CreateThreadsForOpenCLWorkers(int numberOfOpenCLWorkers,
	const std::shared_ptr<cOpenClScheduler> &scheduler, quint64 width, quint64 height,
	const std::shared_ptr<cOpenCLWorkerOutputQueue> &outputQueue, int numberOfSamples,
	int antiAliasingDepth, QList<std::shared_ptr<QThread>> &threads,
	QList<std::shared_ptr<cOpenClWorkerThread>> &workers, bool *stopRequest)
{
	for (int d = 0; d < numberOfOpenCLWorkers; d++)
	{
		// allocating memory for threads and workers
		WriteLog(QString("Thread for OpenCL worker") + QString::number(d) + " create", 3);
		threads.append(std::shared_ptr<QThread>(new QThread));
		workers.append(
			std::shared_ptr<cOpenClWorkerThread>(new cOpenClWorkerThread(this, scheduler, d)));
		// pushing data to workers
		workers[d]->setImageWidth(width);
		workers[d]->setImageHeight(height);
		workers[d]->setOptimalStepX(optimalJob.stepSizeX);
		workers[d]->setOptimalStepY(optimalJob.stepSizeY);
		workers[d]->setHardware(hardware);
		workers[d]->setClKernel(clKernels[d]);
		workers[d]->setClQueue(clQueues[d]);
		workers[d]->setInputAndOutputBuffers(inputAndOutputBuffers[0]); // 0 because not used
		workers[d]->setOutputBuffers(outputBuffers[d]);
		workers[d]->setOutputQueue(outputQueue);
		workers[d]->setPixelMask(&pixelMask);
		workers[d]->setMaxMonteCarloSamples(numberOfSamples);
		workers[d]->setAntiAliasingDepth(antiAliasingDepth);
		workers[d]->setStopRequest(stopRequest);
		workers[d]->setReservedGpuTime(reservedGpuTime);
		workers[d]->setFullEngineFlag(renderEngineMode == clRenderEngineTypeFull);
		workers[d]->setMaxWorkgroupSize(
			hardware->getSelectedDevicesInformation().at(d).maxWorkGroupSize);
		// stating threads
		workers[d]->moveToThread(threads[d].get());
		QObject::connect(
			threads[d].get(), SIGNAL(started()), workers[d].get(), SLOT(ProcessRenderingLoop()));
		QObject::connect(workers[d].get(), SIGNAL(finished()), threads[d].get(), SLOT(quit()));
		connect(workers[d].get(),
			SIGNAL(showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *)), gErrorMessage,
			SLOT(slotShowMessage(QString, cErrorMessage::enumMessageType, QWidget *)));
		threads[d]->setObjectName("OpenCLWorker #" + QString::number(d));
		threads[d]->start();
		threads[d]->setPriority(systemData.GetQThreadPriority(systemData.threadsPriority));
		WriteLog(QString("Thread ") + QString::number(d) + " started", 3);
	}
}

sRGBFloat cOpenClEngineRenderFractal::MCMixColor(
	const cOpenCLWorkerOutputQueue::sClSingleOutput &output, const sRGBFloat &pixel,
	const sRGBFloat &oldPixel)
{
	sRGBFloat newPixel;
	if (output.monteCarloLoop == 1)
	{
		newPixel = pixel;
	}
	else
	{
		float mixFactor = 1.0f / output.monteCarloLoop;
		newPixel.R = oldPixel.R * (1.0f - mixFactor) + pixel.R * mixFactor;
		newPixel.G = oldPixel.G * (1.0f - mixFactor) + pixel.G * mixFactor;
		newPixel.B = oldPixel.B * (1.0f - mixFactor) + pixel.B * mixFactor;
	}
	return newPixel;
}

void cOpenClEngineRenderFractal::PutMultiPixel(quint64 xx, quint64 yy, const sRGBFloat &newPixel,
	unsigned short newAlpha, const sRGB8 &color, float zDepth, const sRGBFloat &normalWorld,
	unsigned short opacity, cImage *image)
{
	image->PutPixelImage(xx, yy, newPixel);
	image->PutPixelZBuffer(xx, yy, zDepth);
	image->PutPixelAlpha(xx, yy, newAlpha);
	image->PutPixelColor(xx, yy, color);
	image->PutPixelOpacity(xx, yy, opacity);
	if (image->GetImageOptional()->optionalNormalWorld)
		image->PutPixelNormalWorld(xx, yy, normalWorld);
}

void cOpenClEngineRenderFractal::PutMultiPixelOptional(quint64 xx, quint64 yy, sRGB8 color,
	const sRGBFloat &normal, const sRGBFloat &specular, const sRGBFloat &world,
	const sRGBFloat &shadows, const sRGBFloat &gi, const sRGBFloat &notDenoised,
	std::shared_ptr<cImage> &image)
{
	if (image->GetImageOptional()->optionalDiffuse)
		image->PutPixelDiffuse(xx, yy, sRGBFloat(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f));
	if (image->GetImageOptional()->optionalNormal)
		image->PutPixelNormal(xx, yy, sRGBFloat(normal.R, normal.G, normal.B));
	if (image->GetImageOptional()->optionalSpecular)
		image->PutPixelSpecular(xx, yy, sRGBFloat(specular.R, specular.G, specular.B));
	if (image->GetImageOptional()->optionalWorld)
		image->PutPixelWorld(xx, yy, sRGBFloat(world.R, world.G, world.B));
	if (image->GetImageOptional()->optionalShadows)
		image->PutPixelShadows(xx, yy, sRGBFloat(shadows.R, shadows.G, shadows.B));
	if (image->GetImageOptional()->optionalGlobalIlluination)
		image->PutPixelGlobalIllumination(xx, yy, sRGBFloat(gi.R, gi.G, gi.B));
	if (image->GetImageOptional()->optionalNotDenoised)
		image->PutPixelNotDenoised(xx, yy, sRGBFloat(notDenoised.R, notDenoised.G, notDenoised.B));
}

int cOpenClEngineRenderFractal::PeriodicRefreshOfTiles(int lastRefreshTime,
	QElapsedTimer &timerImageRefresh, std::shared_ptr<cImage> image, QList<QRect> &lastRenderedRects,
	QList<sRenderedTileData> &listOfRenderedTilesData)
{
	timerImageRefresh.restart();
	image->NullPostEffect(&lastRenderedRects);
	image->CompileImage(&lastRenderedRects);
	if (image->IsPreview())
	{
		image->ConvertTo8bitCharFromList(&lastRenderedRects);
		image->UpdatePreview(&lastRenderedRects);
		sendRenderedTilesList(listOfRenderedTilesData);
		updateImage();
	}
	lastRefreshTime = timerImageRefresh.nsecsElapsed() / lastRenderedRects.size();
	lastRenderedRects.clear();
	listOfRenderedTilesData.clear();
	timerImageRefresh.restart();
	return lastRefreshTime;
}

void cOpenClEngineRenderFractal::FinallRefreshOfImage(
	QList<QRect> lastRenderedRects, std::shared_ptr<cImage> image)
{
	QElapsedTimer timerImageRefresh;
	timerImageRefresh.start();
	image->NullPostEffect(&lastRenderedRects);
	image->CompileImage(&lastRenderedRects);
	if (image->IsPreview())
	{
		image->ConvertTo8bitCharFromList(&lastRenderedRects);
		image->UpdatePreview(&lastRenderedRects);
		updateImage();
	}
	lastRenderedRects.clear();
}

void cOpenClEngineRenderFractal::ConcurentProcessTile(sConcurentTileProcess &data)
{
	const cOpenCLWorkerOutputQueue::sClSingleOutput &output = data.tile;

	data.inOut->tilesRenderedCounter++;

	// information about tile coordinates
	quint64 jobWidth = output.jobWidth;
	quint64 jobHeight = output.jobHeight;
	quint64 jobX = output.jobX;
	quint64 jobY = output.jobY;

	data.inOut->pixelsRendered += jobWidth * jobHeight;

	float monteCarloNoiseSum = 0.0;
	float maxNoise = 0.0;
	float maxBrightness = 0.0;
	float minBrightness = 100.0;

	float maxEdge = 0.0;

	int pixelsToDoCounter = 0;

	bool anitiAliasingDepthFinished = true;
	if (data.in->useAntiAliasing)
	{
		if (output.monteCarloLoop == 1)
			anitiAliasingDepthFinished = true;
		else
		{
			if (output.monteCarloLoop + 1 != data.in->aaSampleNumberTable.at(output.aaDepth - 1))
				anitiAliasingDepthFinished = false;
		}
	}

	std::vector<bool> returnedMask;

	// update pixel mask based on output data
	if (data.in->monteCarlo && data.in->pixelLevelOptimization)
	{
		returnedMask.resize(data.in->optimalJob->stepSizeX * data.in->optimalJob->stepSizeY);
		std::fill(returnedMask.begin(), returnedMask.end(), 0);
		for (int i = 0; i < output.sequenceSize; i++)
		{
			int seq = output.pixelSequence[i];
			int x = seq % jobWidth;
			int y = seq / jobWidth;
			returnedMask[x + y * jobWidth] = true;
		}
	}

	// processing pixels of tile
	for (quint64 y = 0; y < jobHeight; y++)
	{
		for (quint64 x = 0; x < jobWidth; x++)
		{
			// getting pixel from output buffer
			sClPixel pixelCl = reinterpret_cast<const sClPixel *>(
				output.outputBuffers.at(data.in->outputIndex).data.data())[x + y * jobWidth];
			sRGBFloat pixel = {pixelCl.image.s0, pixelCl.image.s1, pixelCl.image.s2};
			sRGB8 color = {pixelCl.color.s0, pixelCl.color.s1, pixelCl.color.s2};
			unsigned short opacity = pixelCl.opacity;
			unsigned short alpha = pixelCl.alpha;

			size_t xx = x + jobX;
			size_t yy = y + jobY;

			// if MC then paint pixels and calculate noise statistics
			if (data.in->monteCarlo)
			{
				if (data.in->pixelLevelOptimization
						&& !returnedMask[x + y * jobWidth]) // skip masked pixels
				// painting pixels with reduced opacity (averaging of MC samples)
				{
					if (data.in->useDenoiser)
					{
						data.denoiser->UpdatePixel(xx, yy, data.image->GetPixelImage(xx, yy),
							data.image->GetPixelZBuffer(xx, yy), 0.0f); // use zero noise for masked pixels
					}
					if (!data.inOut->donePixelsMask[xx + yy * data.in->width])
						data.inOut->maskedPixelsCounter++;
					data.inOut->donePixelsMask[xx + yy * data.in->width] = true;
					continue;
				}
				sRGBFloat oldPixel = data.image->GetPixelImage(xx, yy);
				pixel.R = min(pixel.R, data.params->monteCarloGIRadianceLimit * 2.0f);
				pixel.G = min(pixel.G, data.params->monteCarloGIRadianceLimit * 2.0f);
				pixel.B = min(pixel.B, data.params->monteCarloGIRadianceLimit * 2.0f);

				sRGBFloat newPixel = MCMixColor(output, pixel, oldPixel);

				float zDepthOld = data.image->GetPixelZBuffer(xx, yy);
				float zDepth = zDepthOld;

				sRGBFloat nornalOld;
				sRGBFloat normalWorld;
				sRGBFloat globalIlluminationOut;
				sRGBFloat notDenoisedOut;

				if (output.monteCarloLoop == 1)
				{
					zDepth = pixelCl.zBuffer;
					normalWorld =
						sRGBFloat(pixelCl.normalWorld.s0, pixelCl.normalWorld.s1, pixelCl.normalWorld.s2);

					if (data.image->GetImageOptional()->optionalNormalWorld)
						normalWorld = clFloat3TosRGBFloat(pixelCl.normalWorld);

					if (data.image->GetImageOptional()->optionalGlobalIlluination)
						globalIlluminationOut = clFloat3TosRGBFloat(pixelCl.globalIllumination);

					if (data.image->GetImageOptional()->optionalNotDenoised) notDenoisedOut = pixel;
				}
				else
				{

					zDepth = 1.0f
									 / (1.0f / zDepthOld * (1.0f - 1.0f / output.monteCarloLoop)
											+ 1.0f / pixelCl.zBuffer * (1.0f / output.monteCarloLoop));

					if (data.image->GetImageOptional()->optionalNormalWorld)
					{
						sRGBFloat normalNew =
							sRGBFloat(pixelCl.normalWorld.s0, pixelCl.normalWorld.s1, pixelCl.normalWorld.s2);

						nornalOld = data.image->GetPixelNormalWorld(xx, yy);

						normalWorld.R = nornalOld.R * (1.0f - 1.0f / output.monteCarloLoop)
														+ normalNew.R * (1.0f / output.monteCarloLoop);
						normalWorld.G = nornalOld.G * (1.0f - 1.0f / output.monteCarloLoop)
														+ normalNew.G * (1.0f / output.monteCarloLoop);
						normalWorld.B = nornalOld.B * (1.0f - 1.0f / output.monteCarloLoop)
														+ normalNew.B * (1.0f / output.monteCarloLoop);
					}

					if (data.image->GetImageOptional()->optionalGlobalIlluination)
					{
						sRGBFloat globalIlluminationOutOld = data.image->GetPixelGlobalIllumination(xx, yy);

						globalIlluminationOut.R =
							globalIlluminationOutOld.R * (1.0f - 1.0f / output.monteCarloLoop)
							+ pixelCl.globalIllumination.s0 * (1.0f / output.monteCarloLoop);
						globalIlluminationOut.G =
							globalIlluminationOutOld.G * (1.0f - 1.0f / output.monteCarloLoop)
							+ pixelCl.globalIllumination.s1 * (1.0f / output.monteCarloLoop);
						globalIlluminationOut.B =
							globalIlluminationOutOld.B * (1.0f - 1.0f / output.monteCarloLoop)
							+ pixelCl.globalIllumination.s2 * (1.0f / output.monteCarloLoop);
					}

					if (data.image->GetImageOptional()->optionalNotDenoised)
					{
						sRGBFloat notDenoisedOutOld = data.image->GetPixelNotDenoised(xx, yy);

						notDenoisedOut.R = notDenoisedOutOld.R * (1.0f - 1.0f / output.monteCarloLoop)
															 + pixel.R * (1.0f / output.monteCarloLoop);
						notDenoisedOut.G = notDenoisedOutOld.G * (1.0f - 1.0f / output.monteCarloLoop)
															 + pixel.G * (1.0f / output.monteCarloLoop);
						notDenoisedOut.B = notDenoisedOutOld.B * (1.0f - 1.0f / output.monteCarloLoop)
															 + pixel.B * (1.0f / output.monteCarloLoop);
					}
				}

				unsigned short oldAlpha = data.image->GetPixelAlpha(xx, yy);
				unsigned short newAlpha = ushort(float(oldAlpha) * (1.0f - 1.0f / output.monteCarloLoop)
																				 + alpha * (1.0f / output.monteCarloLoop));

				PutMultiPixel(
					xx, yy, newPixel, newAlpha, color, zDepth, normalWorld, opacity, data.image.get());

				if (data.in->useOptionalImageChannels)
				{
					PutMultiPixelOptional(xx, yy, color, clFloat3TosRGBFloat(pixelCl.normal),
						clFloat3TosRGBFloat(pixelCl.specular), clFloat3TosRGBFloat(pixelCl.world),
						clFloat3TosRGBFloat(pixelCl.shadows), globalIlluminationOut, notDenoisedOut,
						data.image);
				}

				// noise estimation
				float noise = (newPixel.R - oldPixel.R) * (newPixel.R - oldPixel.R)
											+ (newPixel.G - oldPixel.G) * (newPixel.G - oldPixel.G)
											+ (newPixel.B - oldPixel.B) * (newPixel.B - oldPixel.B);
				noise *= 0.3333f;

				//								qDebug() << noise << 0.01f * sqrtf(noise);
				//								noise = 0.01f * sqrtf(noise);

				float sumBrightness = (newPixel.R + newPixel.G + newPixel.B) * 0.333f;
				maxBrightness = max(sumBrightness, maxBrightness);
				minBrightness = min(sumBrightness, minBrightness);

				if (qIsInf(sumBrightness))
				{
					sumBrightness = 0.0;
					noise = 0.0;
					data.image->PutPixelImage(xx, yy, sRGBFloat());
				}

				if (sumBrightness > 1.0f) noise /= (sumBrightness * sumBrightness * sumBrightness);

				if (data.in->useDenoiser)
				{
					data.denoiser->UpdatePixel(xx, yy, newPixel, zDepth, noise);
				}

				monteCarloNoiseSum += noise;
				if (noise > maxNoise) maxNoise = noise;

				float previousPixelNoise = data.inOut->pixelNoiseBuffer[xx + yy * data.in->width];
				float newPixelNoise;

				if (data.in->useAntiAliasing)
				{
					newPixelNoise =
						data.in->noiseFilterFactor * (noise - previousPixelNoise) + previousPixelNoise;
				}
				else
				{
					if (output.monteCarloLoop == 2)
					{
						newPixelNoise = noise;
					}
					else if (output.monteCarloLoop < data.in->minNumberOfSamples)
					{
						newPixelNoise = previousPixelNoise + noise;
					}
					else if (output.monteCarloLoop == data.in->minNumberOfSamples)
					{
						newPixelNoise = (previousPixelNoise + noise)
														/ data.in->minNumberOfSamples; // average noise from last n runs
					}
					else
					{
						newPixelNoise =
							data.in->noiseFilterFactor * (noise - previousPixelNoise) + previousPixelNoise;
					}
				}
				data.inOut->pixelNoiseBuffer[xx + yy * data.in->width] = newPixelNoise;

				if (data.in->pixelLevelOptimization
						&& ((data.in->useAntiAliasing
									&& (output.monteCarloLoop > 1 && anitiAliasingDepthFinished))
								|| (!data.in->useAntiAliasing
										&& output.monteCarloLoop > data.in->minNumberOfSamples)))
				{
					if (sqrtf(newPixelNoise) < data.in->noiseTarget / 100.0f)
					{
						data.pixelMask->at(xx + yy * data.in->width) = false;
					}
					else
					{
						pixelsToDoCounter++;
					}
				}
				else
				{
					pixelsToDoCounter++;
				}
			}
			// if not MC then just paint pixels
			else
			{
				PutMultiPixel(xx, yy, pixel, alpha, color, pixelCl.zBuffer,
					clFloat3TosRGBFloat(pixelCl.normalWorld), opacity, data.image.get());

				if (data.in->useOptionalImageChannels)
				{
					PutMultiPixelOptional(xx, yy, color, clFloat3TosRGBFloat(pixelCl.normal),
						clFloat3TosRGBFloat(pixelCl.specular), clFloat3TosRGBFloat(pixelCl.world),
						clFloat3TosRGBFloat(pixelCl.shadows), clFloat3TosRGBFloat(pixelCl.globalIllumination),
						sRGBFloat(), data.image);
				}
			}
		} // next y
	}		// next x

	if (data.in->useAntiAliasing && output.monteCarloLoop == 1 && data.in->pixelLevelOptimization)
	{
		pixelsToDoCounter = 0;
		for (int y = 0; y < int(jobHeight); y++)
		{
			for (int x = 0; x < int(jobWidth); x++)
			{
				float maxR = 0.0;
				float maxG = 0.0;
				float maxB = 0.0;
				float minR = 1e10;
				float minG = 1e10;
				float minB = 1e10;

				for (int yy = std::max(y - 1, 0); yy <= min(y + 1, int(jobHeight) - 1); yy++)
				{
					for (int xx = std::max(x - 1, 0); xx <= min(x + 1, int(jobWidth) - 1); xx++)
					{
						int xxx = xx + jobX;
						int yyy = yy + jobY;

						sRGBFloat pixel = data.image->GetPixelImage(xxx, yyy);
						maxR = max(pixel.R, maxR);
						maxG = max(pixel.G, maxG);
						maxB = max(pixel.B, maxB);

						minR = min(pixel.R, minR);
						minG = min(pixel.G, minG);
						minB = min(pixel.B, minB);
					}
				}

				float edge = max(max(maxR - minR, maxG - minG), maxB - minB);

				int xxx = x + jobX;
				int yyy = y + jobY;

				maxEdge = max(maxEdge, edge);

				data.inOut->pixelNoiseBuffer[xxx + yyy * data.in->width] = maxEdge;
				if (edge < data.in->noiseTarget / 100.0f)
				{
					data.pixelMask->at(xxx + yyy * data.in->width) = false;
					// maskedPixelsCounter++;
				}
				else
				{
					pixelsToDoCounter++;
				}
			}
		}
	}

	data.renderData->statistics.maskedPixels =
		double(data.inOut->maskedPixelsCounter) / (data.in->width * data.in->height);

	//					qDebug() << output.monteCarloLoop
	//									 << double(maskedPixelsCounter) / (width * height) * 100.0;

	// denoiser
	if (data.in->monteCarlo && data.in->useDenoiser)
	{
		if (output.monteCarloLoop > 2)
		{
			data.inOut->firstBlurcalculated = true;
			data.denoiser->Denoise(jobX, jobY, jobWidth, jobHeight,
				data.params->monteCarloDenoiserPreserveGeometry, data.image, output.monteCarloLoop);
		}
	}
	//-----------

	// total noise in last rectangle
	if (data.in->monteCarlo)
	{
		float weight = 0.3f;

		float totalNoiseRect;
		if (output.monteCarloLoop == 1)
		{
			if (data.in->useAntiAliasing)
				totalNoiseRect = weight * maxEdge;
			else
				totalNoiseRect = weight * (maxBrightness - minBrightness) / 3.0;
		}
		else
		{
			totalNoiseRect =
				sqrtf((1.0f - weight) * monteCarloNoiseSum / jobWidth / jobHeight + weight * maxNoise);
		}

		float previousNoiseLevel =
			data.inOut->noiseTable[output.gridX + output.gridY * (data.in->gridWidth + 1)];
		float smothedNoiseLevel =
			(data.in->useAntiAliasing || output.monteCarloLoop == 1)
				? totalNoiseRect
				: previousNoiseLevel + (totalNoiseRect - previousNoiseLevel) * data.in->noiseFilterFactor;
		// smothedNoiseLevel = totalNoiseRect;
		data.inOut->noiseTable[output.gridX + output.gridY * (data.in->gridWidth + 1)] =
			smothedNoiseLevel;

		if ((data.inOut->noiseTable[output.gridX + output.gridY * (data.in->gridWidth + 1)]
						< data.in->noiseTarget / 100.0f
					&& output.monteCarloLoop > data.in->minNumberOfSamples && anitiAliasingDepthFinished)
				|| pixelsToDoCounter == 0)
		{
			data.scheduler->DisableTile(output.tileIndex);
			data.inOut->doneMCpixels += jobWidth * jobHeight;
			data.inOut->doneMC = double(data.inOut->doneMCpixels) / double(data.in->numberOfPixels);
			data.renderData->statistics.tilesDone +=
				(double)jobWidth * jobHeight / (data.in->width * data.in->height);
		}
		data.inOut->lastMonteCarloLoop = output.monteCarloLoop; // needed for progress bar

		if (data.inOut->lastMonteCarloLoop == 1)
			data.renderData->statistics.numberOfRenderedPixels += jobHeight * jobWidth;
		data.renderData->statistics.totalNumberOfDOFRepeats += jobWidth * jobHeight;

		data.inOut->mutex.lock();
		data.inOut->listOfRenderedTilesData.append(
			sRenderedTileData(jobX, jobY, jobWidth, jobHeight, smothedNoiseLevel));
		data.inOut->mutex.unlock();

	} // endif montecarlo
	else
	{
		data.inOut->mutex.lock();
		data.inOut->listOfRenderedTilesData.append(
			sRenderedTileData(jobX, jobY, jobWidth, jobHeight, 0.0));
		data.inOut->mutex.unlock();
	}

	data.inOut->mutex.lock();
	data.inOut->lastRenderedRects.append(SizedRectangle(jobX, jobY, jobWidth, jobHeight));
	data.inOut->mutex.unlock();

	// FIXME
	//	if (!*data.stopRequest && data.inOut->tilesRenderedCounter > data.in->sequenceSize / 10)
	//		emit signalSmallPartRendered(data.pureRenderingTime->elapsed() / 1000.0);

	if (gNetRender->IsServer())
	{
		gApplication->processEvents();
	}
}

// Multi-threaded version of OpenCL render function
bool cOpenClEngineRenderFractal::RenderMulti(
	std::shared_ptr<cImage> image, bool *stopRequest, sRenderData *renderData)
{
	WriteLog(QString("Starting RenderMulti()"), 2);

	bool finallResult = true; // true if rendering was compiled successfully

	if (*stopRequest) return false;

	if (programsLoaded)
	{
		// The image resolution determines the total amount of work
		quint64 width = image->GetWidth();
		quint64 height = image->GetHeight();

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
		quint64 numberOfPixels = width * height;
		quint64 gridWidth = width / optimalJob.stepSizeX;
		quint64 gridHeight = height / optimalJob.stepSizeY;

		WriteLog(QString("Setting grid size to %1 x %2").arg(gridWidth).arg(gridHeight), 2);

		sConcurentTileProcessSharedData tileSharedData;

		// preparation of table for noise statistics used in MC method
		const quint64 noiseTableSize = (gridWidth + 1) * (gridHeight + 1);
		tileSharedData.noiseTable.resize(noiseTableSize);
		for (quint64 i = 0; i < noiseTableSize; i++)
		{
			tileSharedData.noiseTable[i] = 0.0;
		}

		// maximum count of rendering loop
		int numberOfSamples = 1;
		int minNumberOfSamples = constantInBuffer->params.DOFMinSamples;
		float noiseTarget = constantInBuffer->params.DOFMaxNoise;
		if (monteCarlo)
		{
			numberOfSamples = constantInBuffer->params.DOFSamples;
		}

		int antiAliasingDepth = 0;
		bool useAntiAliasing = constantInBuffer->params.antialiasingEnabled
													 && renderEngineMode == clRenderEngineTypeFull
													 && !constantInBuffer->params.DOFMonteCarlo;

		bool useDenoiser = monteCarlo && constantInBuffer->params.monteCarloDenoiserEnable
											 && !useAntiAliasing && renderEngineMode == clRenderEngineTypeFull;

		QVector<int> aaSampleNumberTable;
		if (useAntiAliasing)
		{
			antiAliasingDepth = constantInBuffer->params.antialiasingOclDepth + 1;
			aaSampleNumberTable.append(5);
			aaSampleNumberTable.append(9);
			aaSampleNumberTable.append(45);
			aaSampleNumberTable.append(81);
			aaSampleNumberTable.append(405);
			aaSampleNumberTable.append(729);
			numberOfSamples = aaSampleNumberTable.at(constantInBuffer->params.antialiasingOclDepth);
			if (constantInBuffer->params.antialiasingAdaptive)
			{
				minNumberOfSamples = 0;
				noiseTarget = 100.0 / numberOfSamples;
			}
			else
			{
				minNumberOfSamples = numberOfSamples;
			}
		}

		// counters for MC statistins
		tileSharedData.doneMC = 0.0;
		tileSharedData.doneMCpixels = 0;

		// calculation of tile sequence (growing ellipse)
		QList<QPoint> tileSequence = calculateOptimalTileSequence(gridWidth + 1, gridHeight + 1);

		// writing all inOut to OpenCL queue
		if (!WriteBuffersToQueue()) throw;

		// writing background texture data to OpenCL queue
		if (renderEngineMode != clRenderEngineTypeFast)
		{
			if (!PrepareBufferForBackground(renderData)) throw;
		}

		// number of openCL workers equals to number of selected OpenCL devices
		int numberOfOpenCLWorkers = hardware->getEnabledDevices().size();

		// prepare list of threads
		QList<std::shared_ptr<QThread>> threads;
		// prepare list of workers
		QList<std::shared_ptr<cOpenClWorkerThread>> workers;

		// create scheduler
		std::shared_ptr<cOpenClScheduler> scheduler(new cOpenClScheduler(&tileSequence));
		// create output FIFO buffer
		std::shared_ptr<cOpenCLWorkerOutputQueue> outputQueue(new cOpenCLWorkerOutputQueue);

		std::shared_ptr<cDenoiser> denoiser(new cDenoiser(
			width, height, cDenoiser::enumStrength(constantInBuffer->params.monteCarloDenoiserStrength)));

		if (monteCarlo)
		{
			tileSharedData.pixelNoiseBuffer.resize(width * height);
			pixelMask.resize(width * height);
			tileSharedData.donePixelsMask.resize(width * height);

			std::fill(pixelMask.begin(), pixelMask.end(), true);
			std::fill(
				tileSharedData.pixelNoiseBuffer.begin(), tileSharedData.pixelNoiseBuffer.end(), 0.0);
		}
		else
		{
			tileSharedData.pixelNoiseBuffer.clear();
			pixelMask.clear();
		}

		bool pixelLevelOptimization =
			constantInBuffer->params.monteCarloPixelLevelOptimization || useAntiAliasing;

		// chebykoh 1m:31s
		// josles 47s
		// grid 44s

		tileSharedData.firstBlurcalculated = false;
		bool autoRefreshBlurResetDone = false;

		if (useDenoiser)
		{
			denoiser->AllocMem();
		}

		// initializing and starting of all workers

		WriteLog(QString("Creating threads for OpenCL workers"), 2);
		CreateThreadsForOpenCLWorkers(numberOfOpenCLWorkers, scheduler, width, height, outputQueue,
			numberOfSamples, antiAliasingDepth, threads, workers, stopRequest);

		// while loop continue condition
		bool continueWhileLoop = false;

		// counters for MC statistics
		tileSharedData.pixelsRendered = 0; // number of
		tileSharedData.lastMonteCarloLoop = 1;

		tileSharedData.maskedPixelsCounter = 0;

		double noiseFilterFactor = min(1.0 / sqrt(numberOfSamples), 0.5);

		sConcurentTileProcessInputData tileInputData;
		tileInputData.useAntiAliasing = useAntiAliasing;
		tileInputData.pixelLevelOptimization = pixelLevelOptimization;
		tileInputData.useDenoiser = useDenoiser;
		tileInputData.monteCarlo = monteCarlo;
		tileInputData.useOptionalImageChannels = useOptionalImageChannels;
		tileInputData.minNumberOfSamples = minNumberOfSamples;
		tileInputData.outputIndex = outputIndex;
		tileInputData.noiseTarget = noiseTarget;
		tileInputData.noiseFilterFactor = noiseFilterFactor;
		tileInputData.width = width;
		tileInputData.height = height;
		tileInputData.gridWidth = gridWidth;
		tileInputData.gridHeight = gridHeight;
		tileInputData.numberOfPixels = numberOfPixels;
		tileInputData.aaSampleNumberTable = aaSampleNumberTable;
		tileInputData.optimalJob = &optimalJob;

		tileSharedData.tilesRenderedCounter = 0;
		QElapsedTimer pureRenderingTime;
		pureRenderingTime.start();

		// main image inOut collection loop
		WriteLog(QString("Starting main rendering loop"), 2);
		do
		{
			// repeat loop until there is something in output queue
			int queueCounter = 0;

			QList<sConcurentTileProcess> collectedOuputs;

			while (!outputQueue->isEmpty() && queueCounter < 100)
			{
				queueCounter++;

				sConcurentTileProcess itemToProcess;
				// getting image inOut from queue
				itemToProcess.tile = outputQueue->GetFromQueue();

				if (scheduler->IsTileEnabled(itemToProcess.tile.tileIndex))
				{
					itemToProcess.inOut = &tileSharedData;
					itemToProcess.in = &tileInputData;
					itemToProcess.image = image;
					itemToProcess.scheduler = scheduler;
					itemToProcess.denoiser = denoiser;
					itemToProcess.renderData = renderData;
					itemToProcess.params = &constantInBuffer->params;
					itemToProcess.pixelMask = &pixelMask;

					collectedOuputs.append(itemToProcess);
				}
			}

			if (collectedOuputs.size() > 0)
			{
				int numberOfTilesToProcess = collectedOuputs.size();

				QtConcurrent::blockingMap(collectedOuputs, ConcurentProcessTile);

				emit signalSmallPartRendered(pureRenderingTime.elapsed() / 1000.0);
			}

			// refreshing progress bar and statistics (not more than once per 100ms)
			if (tileSharedData.lastRenderedRects.size() > 0 && progressRefreshTimer.elapsed() > 100)
			{
				double percentDone;
				if (!monteCarlo)
				{
					percentDone = double(tileSharedData.pixelsRendered) / numberOfPixels;
				}
				else
				{
					percentDone = double(tileSharedData.pixelsRendered) / numberOfPixels / numberOfSamples;

					percentDone = percentDone * (1.0 - tileSharedData.doneMC) + tileSharedData.doneMC;

					double maskedPixelsPerc =
						min(1.0, double(tileSharedData.maskedPixelsCounter) / (width * height));
					percentDone = percentDone * (1.0 - maskedPixelsPerc) + maskedPixelsPerc;
				}
				emit updateProgressAndStatus(
					tr("OpenCl - rendering image"), progressText.getText(percentDone), percentDone);

				float totalNoise = 0.0;
				for (quint64 i = 0; i < noiseTableSize; i++)
				{
					totalNoise += tileSharedData.noiseTable[i];
				}
				totalNoise /= noiseTableSize;
				renderData->statistics.totalNoise = totalNoise * width * height;

				emit updateStatistics(renderData->statistics);

				progressRefreshTimer.restart();
			}

			// refreshing image (refreshing period depends on refreshing speed)
			if (tileSharedData.lastRenderedRects.size() > 0
					&& timerImageRefresh.nsecsElapsed() > lastRefreshTime * 1000)
			{
				lastRefreshTime = PeriodicRefreshOfTiles(lastRefreshTime, timerImageRefresh, image,
					tileSharedData.lastRenderedRects, tileSharedData.listOfRenderedTilesData);
				if (tileSharedData.firstBlurcalculated & !autoRefreshBlurResetDone)
				{
					lastRefreshTime = 0;
					autoRefreshBlurResetDone = true;
				}
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
		if (tileSharedData.lastRenderedRects.size() > 0)
		{
			FinallRefreshOfImage(tileSharedData.lastRenderedRects, image);
		}

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
	std::stable_sort(tiles.begin(), tiles.end(),
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
	QPoint cV1 = center - v1;
	cV1.setX(cV1.x() * gridHeight / gridWidth);
	QPoint cV2 = center - v2;
	cV2.setX(cV2.x() * gridHeight / gridWidth);
	float dist2V1 = cV1.x() * cV1.x() + cV1.y() * cV1.y();
	float dist2V2 = cV2.x() * cV2.x() + cV2.y() * cV2.y();
	if (!qFuzzyCompare(dist2V1, dist2V2)) return dist2V1 < dist2V2;

	// order tiles with same distsance clockwise
	int quartV1 = cV1.x() > 0.0f ? (cV1.y() > 0.0f ? 1 : 0) : (cV1.y() > 0.0f ? 2 : 3);
	int quartV2 = cV2.x() > 0.0f ? (cV2.y() > 0.0f ? 1 : 0) : (cV2.y() > 0.0f ? 2 : 3);
	if (quartV1 != quartV2) return quartV1 < quartV2;
	return quartV1 < 2 ? v1.y() > v2.y() : v1.y() < v2.y();
}

QString cOpenClEngineRenderFractal::toCamelCase(const QString &s)
{
	QStringList upperCaseLookup({"Vs", "Kifs", "De", "Xy", "Xyz", "Cxyz", "Vcl", "Chs", "Difs"});
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
	QStringList parts = s.split('_', QString::SkipEmptyParts);
#else
	QStringList parts = s.split('_', Qt::SkipEmptyParts);
#endif
	for (int i = 0; i < parts.size(); ++i)
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
	uint argIterator, int deviceIndex)
{
	WriteLog(QString("Assigning kernel parameters"), 3);

	int err = clKernels.at(deviceIndex)
							->setArg(argIterator++, *inCLBuffer[deviceIndex]); // input data in global memory
	if (!checkErr(err, "kernel->setArg(1, *inCLBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("input inOut")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	if (!meshExportMode && !distanceMode && renderEngineMode == clRenderEngineTypeFull)
	{
		err =
			clKernels.at(deviceIndex)
				->setArg(argIterator++, *inCLTextureBuffer[deviceIndex]); // input inOut in global memory
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
						*inCLConstBuffer[deviceIndex]); // input inOut in constant memory (faster than global)
	if (!checkErr(err, "kernel->setArg(2, *inCLConstBuffer)"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("constant inOut")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	if (meshExportMode)
	{
		err = clKernels.at(deviceIndex)
						->setArg(argIterator++,
							*inCLConstMeshExportBuffer[deviceIndex]); // input inOut in constant memory (faster
																												// than global)
		if (!checkErr(err, "kernel->setArg(3, *inCLConstMeshExportBuffer)"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("constant mesh inOut")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	if (renderEngineMode != clRenderEngineTypeFast && !meshExportMode && !distanceMode)
	{
		err =
			clKernels.at(deviceIndex)
				->setArg(argIterator++,
					*backgroundImage2D[deviceIndex]); // input data in constant memory (faster than global)
		if (!checkErr(err, "kernel->setArg(3, *backgroundImage2D)"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("background image")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	if (renderEngineMode != clRenderEngineTypeFast && !meshExportMode && !distanceMode)
	{
		err = clKernels.at(deviceIndex)
						->setArg(argIterator++,
							*inCLPerlinNoiseSeedsBuffer[deviceIndex]); // input inOut for perlin noise seeds
		if (!checkErr(err, "kernel->setArg(4, *inCLPerlinNoiseSeedsBuffer)"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("perlin noise seeds")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	if (!meshExportMode && !distanceMode)
	{
		err = clKernels.at(deviceIndex)->setArg(argIterator++, std::rand()); // random seed
		if (!checkErr(err, "kernel->setArg(4, initRandomSeed)"))
		{
			emit showErrorMessage(
				QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("random seed")),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}

	if (distanceMode)
	{
		err = clKernels.at(deviceIndex)->setArg(argIterator++, pointToCalculateDistance); // random seed
		if (!checkErr(err, "kernel->setArg(4, pointToCalculateDistance)"))
		{
			emit showErrorMessage(QObject::tr("Cannot set OpenCL argument for %1")
															.arg(QObject::tr("pointToCalculateDistance")),
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
		WriteLog(QString("Writing OpenCL input buffer"), 2);
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

		if (renderEngineMode == clRenderEngineTypeFull && !distanceMode)
		{
			WriteLog(QString("Writing OpenCL texture buffer"), 2);

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

			if (!meshExportMode)
			{
				WriteLog(QString("Writing OpenCL perlin noise buffer"), 2);
				err = clQueues.at(d)->enqueueWriteBuffer(*inCLPerlinNoiseSeedsBuffer[d], CL_TRUE, 0,
					perlinNoiseSeeds.size() * sizeof(cl_uchar), perlinNoiseSeeds.data());
				if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(inCLPerlinNoiseSeedsBuffer)"))
				{
					emit showErrorMessage(
						QObject::tr("Cannot enqueue writing OpenCL %1").arg(QObject::tr("perlin noise seeds")),
						cErrorMessage::errorMessage, nullptr);
					return false;
				}

				err = clQueues.at(d)->finish();
				if (!checkErr(err, "CommandQueue::finish() - inCLPerlinNoiseSeedsBuffer"))
				{
					emit showErrorMessage(
						QObject::tr("Cannot finish writing OpenCL %1").arg(QObject::tr("perlin noise seeds")),
						cErrorMessage::errorMessage, nullptr);
					return false;
				}
			}
		}

		WriteLog(QString("Writing OpenCL constant buffer"), 2);
		err = clQueues.at(d)->enqueueWriteBuffer(
			*inCLConstBuffer[d].get(), CL_TRUE, 0, sizeof(sClInConstants), constantInBuffer.get());
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
		WriteLog(QString("Writing OpenCL mesh constant buffer"), 2);
		err = clQueues.at(0)->enqueueWriteBuffer(*inCLConstMeshExportBuffer[0].get(), CL_TRUE, 0,
			sizeof(sClMeshExport), constantInMeshExportBuffer.get());
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
	quint64 jobX, quint64 jobY, quint64 pixelsLeftX, quint64 pixelsLeftY)
{
	WriteLog(QString("Processing OpenCL queue"), 3);
	quint64 stepSizeX = optimalJob.stepSizeX;
	if (pixelsLeftX < stepSizeX) stepSizeX = pixelsLeftX;
	quint64 stepSizeY = optimalJob.stepSizeY;
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
	WriteLog(QString("Reading OpenCL buffers"), 3);
	for (int d = 0; d < hardware->getEnabledDevices().size(); d++)
	{
		if (d > 0 && (meshExportMode || distanceMode)) break;
		if (!cOpenClEngine::ReadBuffersFromQueue(d)) return false;
	}
	return true;
}

size_t cOpenClEngineRenderFractal::CalcNeededMemory()
{
	if (!meshExportMode)
	{
		quint64 mem1 = optimalJob.sizeOfPixel * optimalJob.stepSize;
		quint64 mem2 = dynamicData->GetData().size();
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
	WriteLog(QString("Preparing buffer for background"), 3);

	quint64 texWidth = renderData->textures.backgroundTexture.Width();
	quint64 texHeight = renderData->textures.backgroundTexture.Height();

	backgroundImageBuffer.resize(texWidth * texHeight);
	quint64 backgroundImage2DWidth = texWidth;
	quint64 backgroundImage2DHeight = texHeight;

	for (quint64 y = 0; y < texHeight; y++)
	{
		for (quint64 x = 0; x < texWidth; x++)
		{
			sRGBFloat pixel = renderData->textures.backgroundTexture.FastPixel(x, y);
			backgroundImageBuffer[x + y * texWidth].s[0] = pixel.R;
			backgroundImageBuffer[x + y * texWidth].s[1] = pixel.G;
			backgroundImageBuffer[x + y * texWidth].s[2] = pixel.B;
			backgroundImageBuffer[x + y * texWidth].s[3] = 1.0f;
		}
	}

	for (int d = 0; d < hardware->getEnabledDevices().size(); d++)
	{
		cl_int err;
		backgroundImage2D.append(std::shared_ptr<cl::Image2D>(
			new cl::Image2D(*hardware->getContext(d), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
				cl::ImageFormat(CL_RGBA, CL_FLOAT), backgroundImage2DWidth, backgroundImage2DHeight,
				backgroundImage2DWidth * sizeof(cl_float4), backgroundImageBuffer.data(), &err)));
		if (!checkErr(err, "cl::Image2D(...backgroundImage...)")) return false;
	}
	return true;
}

// render function for mesh export
bool cOpenClEngineRenderFractal::Render(std::vector<double> *distances, std::vector<double> *colors,
	std::vector<int> *iterations, int sliceIndex, bool *stopRequest, sRenderData *renderData,
	size_t dataOffset)
{
	Q_UNUSED(renderData);
	Q_UNUSED(stopRequest);

	constantInMeshExportBuffer->sliceIndex = sliceIndex;

	// writing inOut to queue
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
				outputBuffers[0][outputMeshDistancesIndex].ptr.get()))[address];
			(*distances)[address + dataOffset] = distance;

			if (colors)
			{
				double color = (reinterpret_cast<cl_float *>(
					outputBuffers[0][outputMeshColorsIndex].ptr.get()))[address];
				(*colors)[address + dataOffset] = color;
			}

			if (iterations)
			{
				int iters = (reinterpret_cast<cl_int *>(
					outputBuffers[0][outputMeshIterationsIndex].ptr.get()))[address];
				(*iterations)[address + dataOffset] = iters;
			}
		}
	}

	return true;
}

float cOpenClEngineRenderFractal::CalculateDistance(CVector3 point)
{
	pointToCalculateDistance = {{cl_float(point.x), cl_float(point.y), cl_float(point.z)}};

	// writing inOut to queue
	if (!WriteBuffersToQueue()) return false;

	// assign parameters to kernel
	if (!AssignParametersToKernel(0)) return false;

	// processing queue
	if (!ProcessQueue(0, 0, 1, 1)) return false;

	if (!ReadBuffersFromQueue()) return false;

	float distance =
		(reinterpret_cast<cl_float *>(outputBuffers[0][outputMeshDistancesIndex].ptr.get()))[0];

	return distance;
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
