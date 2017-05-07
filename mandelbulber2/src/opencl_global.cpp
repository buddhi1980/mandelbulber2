/*
 * opencl_global.cpp
 *
 *  Created on: 7 maj 2017
 *      Author: krzysztof
 */

#include "opencl_global.h"
#include "opencl_hardware.h"
#include "opencl_engine_render_fractal.h"
#include "initparameters.hpp"

cGlobalOpenCl *gOpenCl = nullptr;

cGlobalOpenCl::cGlobalOpenCl()
{
#ifdef USE_OPENCL
	openClHardware = new cOpenClHardware();

	openClHardware->ListOpenClPlatforms();
	openClHardware->CreateContext(0, cOpenClHardware::openClDeviceTypeGPU);
	openClHardware->SelectDevice(0);

	openClEngineRenderFractal = new cOpenClEngineRenderFractal(openClHardware);
#endif

}

cGlobalOpenCl::~cGlobalOpenCl()
{
#ifdef USE_OPENCL
	delete openClHardware;
	delete openClEngineRenderFractal;
#endif
}
