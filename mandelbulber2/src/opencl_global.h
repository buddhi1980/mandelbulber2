/*
 * opencl_global.h
 *
 *  Created on: 7 maj 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_GLOBAL_H_
#define MANDELBULBER2_SRC_OPENCL_GLOBAL_H_

class cOpenClHardware;
class cOpenClEngineRenderFractal;

class cGlobalOpenCl
{
public:
	cGlobalOpenCl();
	~cGlobalOpenCl();

	cOpenClEngineRenderFractal *openClEngineRenderFractal;
	cOpenClHardware *openClHardware;
};

extern cGlobalOpenCl *gOpenCl;


#endif /* MANDELBULBER2_SRC_OPENCL_GLOBAL_H_ */
