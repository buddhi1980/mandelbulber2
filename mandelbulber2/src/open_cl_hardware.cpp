/*
 * open_cl_engine.cpp
 *
 *  Created on: 2 maj 2017
 *      Author: krzysztof
 */

#include "open_cl_hardware.h"

cOpenClHardware::cOpenClHardware(QObject *parent) : QObject(parent)
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
	cl::Platform::get(&clPlatforms);
	if(checkErr(clPlatforms.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get"))
	{
		qDebug() << "OpenCL Platform number is: " << clPlatforms.size();
		openClAvailable = true;

		std::string platformVendor;

		for(int i = 0; i < clPlatforms.size(); i++)
		{
			clPlatforms[i].getInfo((cl_platform_info) CL_PLATFORM_VENDOR, &platformVendor);
			qDebug() << "platform #" << i << ": " << platformVendor.c_str();
		}
	}
}

QStringList cOpenClHardware::GetPlatformNames()
{

}

bool cOpenClHardware::checkErr(cl_int err, QString fuctionName)
{
	if (err != CL_SUCCESS)
	{
		qCritical() << "ERROR: " << fuctionName << " (" << err << ")";
		return false;
	}
	else
		return true;
}
#endif
