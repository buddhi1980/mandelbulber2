/*
 * opencl_engine_render_nebula.h
 *
 *  Created on: 4 maj 2025
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_NEBULA_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_NEBULA_H_

#include "opencl_engine.h"

#ifdef USE_OPENCL
#include "opencl/input_data_structures.h"
#endif // USE_OPENCL

class cImage;
struct sParamRender;
class cFractalContainer;
class cNineFractals;

class cOpenClEngineRenderNebula : public cOpenClEngine
{
	Q_OBJECT

public:
	cOpenClEngineRenderNebula(cOpenClHardware *_hardware);
	~cOpenClEngineRenderNebula() override;

#ifdef USE_OPENCL
	void SetParameters(std::shared_ptr<const cParameterContainer> paramContainer,
		std::shared_ptr<const cFractalContainer> fractalContainer,
		std::shared_ptr<sParamRender> paramRender, std::shared_ptr<cNineFractals> fractals);
	bool LoadSourcesAndCompile(std::shared_ptr<const cParameterContainer> params,
		QString *compilerErrorOutput = nullptr) override;
	void RegisterInputOutputBuffers(std::shared_ptr<const cParameterContainer> params) override;
	bool PreAllocateBuffers(std::shared_ptr<const cParameterContainer> params) override;
	bool AssignParametersToKernelAdditional(uint argIterator, int deviceIndex) override;
	bool ProcessQueue();
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
	static QString toCamelCase(const QString &s);
#endif

private:
	QStringList customFormulaCodes;
	QStringList listOfUsedFormulas;
	quint64 numberOfPixels = 0;

	int jobSize = 1024;

	std::unique_ptr<sClInConstants> constantInBuffer;
	QList<std::shared_ptr<cl::Buffer>> inCLConstBuffer;

	const int inOutImageBufferIndex = 0;
	const int inRandomBufferIndex = 0;

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void updateImage();
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_NEBULA_H_ */
