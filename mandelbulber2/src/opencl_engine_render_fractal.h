/*
 * opencl_engine_render_fractal.h
 *
 *  Created on: 6 maj 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_FRACTAL_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_FRACTAL_H_

#include "opencl_engine.h"

#ifdef USE_OPENCL
#include <CL/cl.hpp>
#include "../opencl/mandelbulber_cl_data.h"
#endif

class cImage;

class cOpenClEngineRenderFractal : public virtual cOpenClEngine
{
public:
	cOpenClEngineRenderFractal(cOpenClHardware *hardware);
	~cOpenClEngineRenderFractal();

#ifdef USE_OPENCL
	void LoadSourcesAndCompile(const cParameterContainer *params) override;
	void SetParameters(const cParameterContainer *params);
	bool PreAllocateBuffers(const cParameterContainer *params) override;
	bool Render(cImage *image);

private:
	sClInConstants *constantInBuffer;
	cl::Buffer *inCLConstBuffer;

	sClInBuff *inBuffer;
	cl::Buffer *inCLBuffer;

	sClPixel *rgbbuff;
	cl::Buffer *outCL;
#endif
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_FRACTAL_H_ */
