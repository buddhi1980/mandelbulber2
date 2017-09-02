/*
 * opencl_engine_render_dof.cpp
 *
 *  Created on: 2 wrz 2017
 *      Author: krzysztof
 */

#include "opencl_engine_render_dof.h"

#include "opencl_engine_render_dof_phase1.h"
#include "opencl_global.h"
#include "opencl_hardware.h"
#include "parameters.hpp"

cOpenClEngineRenderDOF::cOpenClEngineRenderDOF(cOpenClHardware *hardware) : QObject(hardware)
{
#ifdef USE_OPENCL
	dofEnginePhase1 = new cOpenClEngineRenderDOFPhase1(hardware);
#endif
}

cOpenClEngineRenderDOF::~cOpenClEngineRenderDOF()
{
#ifdef USE_OPENCL
	delete dofEnginePhase1;
#endif
}

#ifdef USE_OPENCL
bool cOpenClEngineRenderDOF::RenderDOF(const sParamRender *paramRender,
	const cParameterContainer *params, cImage *image, bool *stopRequest)
{
	dofEnginePhase1->Lock();
	dofEnginePhase1->SetParameters(paramRender);
	bool result = false;
	if (dofEnginePhase1->LoadSourcesAndCompile(params))
	{
		dofEnginePhase1->CreateKernel4Program(params);
		dofEnginePhase1->PreAllocateBuffers(params);
		dofEnginePhase1->CreateCommandQueue();
		result = dofEnginePhase1->Render(image, stopRequest);
	}
	dofEnginePhase1->Unlock();

	return result;
}
#endif
