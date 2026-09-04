/*
 * opencl_engine_render_nebula.h
 *
 *  Created on: 4 maj 2025
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_NEBULA_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_NEBULA_H_

#include "opencl_dynamic_data.hpp"
#include "opencl_engine.h"

#ifdef USE_OPENCL
#include "opencl/input_data_structures.h"

// CPU-side render data for nebula shader (minimal version)
struct sNebulaRenderData
{
	int nebulaSequencesCount = 0;
	intptr_t nebulaSequencesDataOffset = 0; // offset into dynamic data buffer
	int hybridSequencesCount = 0;
	intptr_t hybridSequencesDataOffset = 0;
	int fractalsCount = 0;
	intptr_t fractalsDataOffset = 0;
};
#endif // USE_OPENCL

class cImage;
struct sParamRender;
class cFractalContainer;
class cHybridFractalSequences;
class cHybridFractalSequences;

class cOpenClEngineRenderNebula : public cOpenClEngine
{
	Q_OBJECT

public:
	cOpenClEngineRenderNebula(cOpenClHardware *_hardware);
	~cOpenClEngineRenderNebula() override;

#ifdef USE_OPENCL
	void SetParameters(std::shared_ptr<const cParameterContainer> paramContainer,
		std::shared_ptr<const cFractalContainer> fractalContainer,
		std::shared_ptr<sParamRender> paramRender, std::shared_ptr<cHybridFractalSequences> fractals,
		const cHybridFractalSequences &hybridSequences);
	bool LoadSourcesAndCompile(std::shared_ptr<const cParameterContainer> params,
		QString *compilerErrorOutput = nullptr) override;
	void RegisterInputOutputBuffers(std::shared_ptr<const cParameterContainer> params) override;
	bool PreAllocateBuffers(std::shared_ptr<const cParameterContainer> params) override;
	bool WriteBuffersToQueue();
	bool AssignParametersToKernelAdditional(uint argIterator, int deviceIndex) override;
	bool ProcessQueue(qint64 offset);
	bool Render(std::shared_ptr<cImage> image, bool *stopRequest);
	void ReleaseMemory();
	size_t CalcNeededMemory() override;
	QString GetKernelName() override;

private:
	void CreateListOfHeaderFiles(QStringList &clHeaderFiles);
	void CreateListOfIncludes(const QStringList &clHeaderFiles, const QString &openclPathSlash,
		std::shared_ptr<const cParameterContainer> params, const QString &openclEnginePath,
		QByteArray &programEngine, const QByteArray &formulaSwitchCode);
	void GenerateFormulaSwitchCode(
		const QStringList &formulas, int totalFormulaCount, bool needsSwitch, QByteArray &out);
	static QString toCamelCase(const QString &s);

private:
	QStringList customFormulaCodes;
	QStringList listOfUsedFormulas;
	QStringList allFormulasPreDedup;
	bool isHybrid = false;
	quint64 numberOfPixels = 0;

	qint64 jobSize = 1024;

	std::unique_ptr<sClInConstants> constantInBuffer;
	QList<std::shared_ptr<cl::Buffer>> inCLConstBuffer;

	QByteArray inBuffer;
	QList<std::shared_ptr<cl::Buffer>> inCLBuffer;

	std::unique_ptr<cOpenClDynamicData> dynamicData;

	sNebulaRenderData renderData;
	QList<std::shared_ptr<cl::Buffer>> inCLRenderDataBuffer;

	const int inOutImageBufferIndex = 0;
	const int inRandomBufferIndex = 0;

	int maxiter = 0;

	double zBufferDefault = 0.0;

#endif

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void updateImage();
	void signalSmallPartRendered(double time);
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_NEBULA_H_ */
