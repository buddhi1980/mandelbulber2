/*
 * open_cl_engine.cpp
 *
 *  Created on: 2 maj 2017
 *      Author: krzysztof
 */

#include "open_cl_hardware.h"

cOpenClHardware::cOpenClHardware(QObject *parent = nullptr) : QObject(parent)
{
	openClReady = false;
	openClAvailable = false;
}

cOpenClHardware::~cOpenClHardware()
{
	// TODO Auto-generated destructor stub
}

#ifdef USE_OPENCL
void cOpenClHardware::ListOpenClPlatforms()
{

}

QStringList cOpenClHardware::GetPlatformNames()
{

}
#endif
