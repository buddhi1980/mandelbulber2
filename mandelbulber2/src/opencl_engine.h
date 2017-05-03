/*
 * opencl_engine.h
 *
 *  Created on: 3 maj 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_H_

#include "opencl_hardware.h"

class cOpenClEngine : public cOpenClHardware
{
	Q_OBJECT

public:
	cOpenClEngine(QObject *parent = nullptr);
	~cOpenClEngine();
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_H_ */
