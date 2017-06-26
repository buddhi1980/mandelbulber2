/*
 * opencl_engine_render_ssao.h
 *
 *  Created on: 21 cze 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_SSAO_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_SSAO_H_

#include "opencl_engine.h"

struct sParamRender;
class cImage;

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4005) // macro redefinition
#pragma warning(disable : 4996) // declared deprecated
#endif													// _MSC_VER

#ifdef USE_OPENCL
#ifdef _WIN32
#ifndef _MSC_VER
// clew for cross compile
#include "clew.h"
#include "clew-cl.hpp"
#endif // NOT _MSC_VER
#endif // _WIN32
// OpenCL SDK for all others
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif
#include "../opencl/ssao_cl.h"
#endif // USE_OPENCL

class cOpenClEngineRenderSSAO : public cOpenClEngine
{
	Q_OBJECT

public:
	cOpenClEngineRenderSSAO(cOpenClHardware *hardware);
	~cOpenClEngineRenderSSAO();

#ifdef USE_OPENCL
	void SetParameters(const sParamRender *paramRender);
	bool LoadSourcesAndCompile(const cParameterContainer *params) override;
	bool PreAllocateBuffers(const cParameterContainer *params) override;
	bool AssignParametersToKernel();
	bool WriteBuffersToQueue();
	bool ProcessQueue(int pixelsLeft, int pixelIndex);
	bool ReadBuffersFromQueue();
	bool Render(cImage *image, bool *stopRequest);

private:
	QString GetKernelName() override;

	sParamsSSAO paramsSSAO;
	float intensity;

	cl_float *inZBuffer;
	cl::Buffer *inCLZBuffer;
	int numberOfPixels;

	cl_float *outBuffer;
	cl::Buffer *outCl;

#endif

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_SSAO_H_ */
