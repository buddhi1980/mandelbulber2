/*
 * opencl_engine.h
 *
 *  Created on: 3 maj 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_H_

#include <QtCore>

#ifdef USE_OPENCL
#include <CL/cl.hpp>
#endif

class cOpenClHardware;
class cParameterContainer;

class cOpenClEngine : public QObject
{
	Q_OBJECT

struct sOptimalJob
{
	size_t numberOfSteps;
	size_t workGroupSize;
	size_t pixelsPerJob;
	size_t stepSize;
};

public:
	cOpenClEngine(cOpenClHardware *hardware);
	~cOpenClEngine();

#ifdef USE_OPENCL

	virtual void LoadSourcesAndCompile(const cParameterContainer *params) = 0;
	bool CreateKernel4Program(const cParameterContainer *params);

protected:
	bool checkErr(cl_int err, QString fuctionName);
	bool Build(cl::Program *prog, QString *errorText);
	bool CreateKernel(cl::Program *prog);
	sOptimalJob CalculateOptimalJob(const cParameterContainer *params);

	virtual bool PreAllocateBuffers(const cParameterContainer *params) = 0;

	cl::Program *program;
	cl::Kernel *kernel;

	sOptimalJob optimalJob;
	bool programsLoaded;

#endif

	cOpenClHardware *hardware;
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_H_ */
