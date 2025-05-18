/*
 * opencl_engine_render_nebula.cpp
 *
 *  Created on: 4 maj 2025
 *      Author: krzysztof
 */

#include "opencl_engine_render_nebula.h"

#include "parameters.hpp"
#include "fractal.h"
#include "fractparams.hpp"
#include "system_data.hpp"
#include "system_directories.hpp"

cOpenClEngineRenderNebula::cOpenClEngineRenderNebula(cOpenClHardware *_hardware)
		: cOpenClEngine(_hardware)
{
	customFormulaCodes.reserve(NUMBER_OF_FRACTALS);
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		customFormulaCodes.append(QString());
	}
}

cOpenClEngineRenderNebula::~cOpenClEngineRenderNebula()
{
#ifdef USE_OPENCL
	ReleaseMemory();
#endif
}

#ifdef USE_OPENCL

void cOpenClEngineRenderNebula::SetParameters(const sParamRender *paramRender) {}

bool cOpenClEngineRenderNebula::LoadSourcesAndCompile(
	std::shared_ptr<const cParameterContainer> params, QString *compilerErrorOutput)
{
	return true;
}

void cOpenClEngineRenderNebula::RegisterInputOutputBuffers(
	std::shared_ptr<const cParameterContainer> params)
{
}

bool cOpenClEngineRenderNebula::AssignParametersToKernelAdditional(
	uint argIterator, int deviceIndex)
{
	return true;
}

bool cOpenClEngineRenderNebula::ProcessQueue(quint64 pixelsLeft, quint64 pixelIndex)
{
	return true;
}

bool cOpenClEngineRenderNebula::Render(std::shared_ptr<cImage> image, bool *stopRequest)
{
	return true;
}

void cOpenClEngineRenderNebula::ReleaseMemory()
{
	cOpenClEngine::ReleaseMemory();
}

QString cOpenClEngineRenderNebula::GetKernelName()
{
	return QString("RenderNebula");
}

size_t cOpenClEngineRenderNebula::CalcNeededMemory()
{
	return 0;
}

void cOpenClEngineRenderNebula::CreateListOfHeaderFiles(QStringList &clHeaderFiles)
{
	clHeaderFiles.append("defines_cl.h");
	clHeaderFiles.append("opencl_typedefs.h");
	clHeaderFiles.append("opencl_algebra.h");
	clHeaderFiles.append("common_params_cl.hpp");
	clHeaderFiles.append("image_adjustments_cl.h");
	clHeaderFiles.append("fractal_cl.h");
	clHeaderFiles.append("fractparams_cl.hpp");
	clHeaderFiles.append("fractal_sequence_cl.h");
	clHeaderFiles.append("input_data_structures.h");
}

void cOpenClEngineRenderNebula::CreateListOfIncludes(const QStringList &clHeaderFiles,
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

	// compute fractal
	AddInclude(programEngine, openclEnginePath + "compute_fractal.cl");
}

#endif
