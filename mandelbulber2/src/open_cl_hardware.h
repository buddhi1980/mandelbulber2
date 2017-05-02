/*
 * open_cl_engine.h
 *
 *  Created on: 2 maj 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPEN_CL_HARDWARE_H_
#define MANDELBULBER2_SRC_OPEN_CL_HARDWARE_H_

#include <QtCore>

#ifdef USE_OPENCL
#include <CL/cl.hpp>
#endif

class cOpenClHardware : public QObject
{
	Q_OBJECT

public:
	cOpenClHardware(QObject *parent);
	~cOpenClHardware();

#ifdef USE_OPENCL
	void ListOpenClPlatforms();
	QStringList GetPlatformNames();

	QList<cl::Platform> clPlatforms;
#endif

	bool openClAvailable;
	bool openClReady;
};

#endif /* MANDELBULBER2_SRC_OPEN_CL_HARDWARE_H_ */
