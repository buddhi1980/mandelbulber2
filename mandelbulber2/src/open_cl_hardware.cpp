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
	context = nullptr;
}

cOpenClHardware::~cOpenClHardware()
{
	// TODO Auto-generated destructor stub
}

#ifdef USE_OPENCL
void cOpenClHardware::ListOpenClPlatforms()
{
	clPlatforms.clear();
	platformsInformation.clear();

	cl::Platform::get(&clPlatforms);
	if (checkErr(clPlatforms.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get"))
	{
		qDebug() << "OpenCL Platform number is: " << clPlatforms.size();
		openClAvailable = true;

		std::string platformName;
		std::string platformVendor;
		std::string platformVersion;
		std::string platformProfile;

		for (unsigned int i = 0; i < clPlatforms.size(); i++)
		{
			clPlatforms[i].getInfo((cl_platform_info)CL_PLATFORM_NAME, &platformName);
			clPlatforms[i].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
			clPlatforms[i].getInfo((cl_platform_info)CL_PLATFORM_VERSION, &platformVersion);
			clPlatforms[i].getInfo((cl_platform_info)CL_PLATFORM_PROFILE, &platformProfile);

			sPlatformInformation platformInformation;
			platformInformation.name = platformName.c_str();
			platformInformation.vendor = platformVendor.c_str();
			platformInformation.version = platformVersion.c_str();
			platformInformation.profile = platformProfile.c_str();

			qDebug() << "platform #" << i << ": " << platformInformation.name
							 << platformInformation.vendor << platformInformation.version
							 << platformInformation.profile;

			platformsInformation.append(platformInformation);
		}
	}
	else
	{
		openClAvailable = false;
	}
}

void cOpenClHardware::CreateContext(int platformIndex, enumOpenClDeviceType deviceType)
{
	cl_context_properties cprops[3] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)(clPlatforms[platformIndex])(), 0};

	if (context) delete context;

	cl_int err;

	switch (deviceType)
	{
		case openClDeviceTypeCPU:
			context = new cl::Context(CL_DEVICE_TYPE_CPU, cprops, NULL, NULL, &err);
			break;
		case openClDeviceTypeGPU:
			context = new cl::Context(CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err);
			break;
	}

	if (checkErr(err, "Context::Context()"))
	{
	}
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
