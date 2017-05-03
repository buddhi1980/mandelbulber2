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
	contextReady = false;

#ifdef USE_OPENCL
	context = nullptr;
#endif
}

cOpenClHardware::~cOpenClHardware()
{
	if(context) delete context;
}

#ifdef USE_OPENCL
void cOpenClHardware::ListOpenClPlatforms()
{
	clPlatforms.clear();
	platformsInformation.clear();

	cl::Platform::get(&clPlatforms);
	if (checkErr(clPlatforms.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get"))
	{
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
		qCritical() << "There are no valid OpenCl platforms in the system";
	}
}

void cOpenClHardware::CreateContext(int platformIndex, enumOpenClDeviceType deviceType)
{
	if (openClAvailable)
	{
		cl_context_properties cprops[3] = {
			CL_CONTEXT_PLATFORM, (cl_context_properties)(clPlatforms[platformIndex])(), 0};

		if (context) delete context;
		contextReady = false;

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
			contextReady = true;
			ListOpenClDevices();
		}
	}
	else
	{
		qCritical() << "OpenCl is not available";
	}
}

void cOpenClHardware::ListOpenClDevices()
{
	if (contextReady)
	{
		clDevices = context->getInfo<CL_CONTEXT_DEVICES>();
		devicesInformation.clear();

		if (clDevices.size() > 0)
		{
			for (unsigned int i = 0; i < clDevices.size(); i++)
			{
				sDeviceInformation deviceInformation;
				clDevices[i].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &deviceInformation.deviceAvailable);
				clDevices[i].getInfo(CL_DEVICE_COMPILER_AVAILABLE, &deviceInformation.compilerAvailable);
				clDevices[i].getInfo(CL_DEVICE_DOUBLE_FP_CONFIG, &deviceInformation.doubleFpConfig);
				clDevices[i].getInfo(
					CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, &deviceInformation.globalMemCacheSize);
				clDevices[i].getInfo(
					CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, &deviceInformation.globalMemCachelineSize);
				clDevices[i].getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &deviceInformation.globalMemSize);
				clDevices[i].getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &deviceInformation.localMemSize);
				clDevices[i].getInfo(CL_DEVICE_MAX_CLOCK_FREQUENCY, &deviceInformation.maxClockFrequency);
				clDevices[i].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &deviceInformation.maxComputeUnits);
				clDevices[i].getInfo(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &deviceInformation.maxConstantBufferSize);
				clDevices[i].getInfo(CL_DEVICE_MAX_MEM_ALLOC_SIZE, &deviceInformation.maxMemAllocSize);
				clDevices[i].getInfo(CL_DEVICE_MAX_PARAMETER_SIZE, &deviceInformation.maxParameterSize);
				clDevices[i].getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &deviceInformation.maxWorkGroupSize);

				std::string deviceName;
				clDevices[i].getInfo(CL_DEVICE_NAME, &deviceName);
				deviceInformation.deviceName = QString(deviceName.c_str());

				std::string deviceVersion;
				clDevices[i].getInfo(CL_DEVICE_VERSION, &deviceVersion);
				deviceInformation.deviceVersion = QString(deviceVersion.c_str());

				std::string driverVersion;
				clDevices[i].getInfo(CL_DRIVER_VERSION, &driverVersion);
				deviceInformation.driverVersion = QString(driverVersion.c_str());

				qDebug() << "Device # " << i;
				qDebug() << "CL_DEVICE_MAX_COMPUTE_UNITS" << deviceInformation.deviceAvailable;
				qDebug() << "CL_DEVICE_COMPILER_AVAILABLE" << deviceInformation.compilerAvailable;
				qDebug() << "CL_DEVICE_DOUBLE_FP_CONFIG" << deviceInformation.doubleFpConfig;
				qDebug() << "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE" << deviceInformation.globalMemCacheSize;
				qDebug() << "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE" << deviceInformation.globalMemCachelineSize;
				qDebug() << "CL_DEVICE_GLOBAL_MEM_SIZE" << deviceInformation.globalMemSize;
				qDebug() << "CL_DEVICE_LOCAL_MEM_SIZE" << deviceInformation.localMemSize;
				qDebug() << "CL_DEVICE_MAX_CLOCK_FREQUENCY" << deviceInformation.maxClockFrequency;
				qDebug() << "CL_DEVICE_MAX_COMPUTE_UNITS" << deviceInformation.maxComputeUnits;
				qDebug() << "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE" << deviceInformation.maxConstantBufferSize;
				qDebug() << "CL_DEVICE_MAX_MEM_ALLOC_SIZE" << deviceInformation.maxMemAllocSize;
				qDebug() << "CL_DEVICE_MAX_PARAMETER_SIZE" << deviceInformation.maxParameterSize;
				qDebug() << "CL_DEVICE_MAX_WORK_GROUP_SIZE" << deviceInformation.maxWorkGroupSize;
				qDebug() << "CL_DEVICE_NAME" << deviceInformation.deviceName;
				qDebug() << "CL_DEVICE_VERSION" << deviceInformation.deviceVersion;
				qDebug() << "CL_DRIVER_VERSION" << deviceInformation.driverVersion;

				devicesInformation.append(deviceInformation);
			}
		}
		else
		{
			qCritical() << "There are no available devices for selected platform";
		}
	}
}

bool cOpenClHardware::checkErr(cl_int err, QString fuctionName)
{
	if (err != CL_SUCCESS)
	{
		qCritical() << "OpenCl ERROR: " << fuctionName << " (" << err << ")";
		return false;
	}
	else
		return true;
}
#endif
