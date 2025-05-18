/*
 * opencl_engine_render_nebula.h
 *
 *  Created on: 4 maj 2025
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_NEBULA_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_NEBULA_H_

#include "opencl_engine.h"

class cImage;
struct sParamRender;

class cOpenClEngineRenderNebula : public cOpenClEngine
{
	Q_OBJECT

public:
	cOpenClEngineRenderNebula(cOpenClHardware *_hardware);
	~cOpenClEngineRenderNebula() override;

#ifdef USE_OPENCL
	void SetParameters(const sParamRender *paramRender);
	bool LoadSourcesAndCompile(std::shared_ptr<const cParameterContainer> params,
		QString *compilerErrorOutput = nullptr) override;
	void RegisterInputOutputBuffers(std::shared_ptr<const cParameterContainer> params) override;
	bool AssignParametersToKernelAdditional(uint argIterator, int deviceIndex) override;
	bool ProcessQueue(quint64 pixelsLeft, quint64 pixelIndex);
	bool Render(std::shared_ptr<cImage> image, bool *stopRequest);
	void ReleaseMemory();
	size_t CalcNeededMemory() override;
	QString GetKernelName() override;
#endif

private:
#ifdef USE_OPENCL
	void CreateListOfHeaderFiles(QStringList &clHeaderFiles);
	void CreateListOfIncludes(const QStringList &clHeaderFiles, const QString &openclPathSlash,
		std::shared_ptr<const cParameterContainer> params, const QString &openclEnginePath,
		QByteArray &programEngine);
#endif

private:
	QStringList customFormulaCodes;
	QStringList listOfUsedFormulas;
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_NEBULA_H_ */
