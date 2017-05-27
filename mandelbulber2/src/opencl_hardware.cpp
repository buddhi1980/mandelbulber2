/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Robert Pancoast (RobertPancoast77@gmail.com)
 *
 * These objects enable an OpenCL backend definition.
 */

#include "opencl_hardware.h"
#include "opencl_device.h"

cOpenClHardware::cOpenClHardware(QObject *parent) : QObject(parent)
{
	openClAvailable = false;
	contextReady = false;
	selectedDeviceIndex = 0;

#ifdef USE_OPENCL
	context = nullptr;
#endif
}

cOpenClHardware::~cOpenClHardware()
{
#ifdef USE_OPENCL
	if (context) delete context;
#endif
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

void cOpenClHardware::CreateContext(
	int platformIndex, cOpenClDevice::enumOpenClDeviceType deviceType)
{
	if (openClAvailable)
	{
		devicesInformation.clear();
		clDeviceWorkers.clear();
		clDevices.clear();

		cl_context_properties cprops[3] = {
			CL_CONTEXT_PLATFORM, (cl_context_properties)(clPlatforms[platformIndex])(), 0};

		if (context) delete context;
		contextReady = false;

		cl_int err;

		switch (deviceType)
		{
			case cOpenClDevice::openClDeviceTypeACC:
				context = new cl::Context(CL_DEVICE_TYPE_ACCELERATOR, cprops, NULL, NULL, &err);
				break;
			case cOpenClDevice::openClDeviceTypeALL:
				context = new cl::Context(CL_DEVICE_TYPE_ALL, cprops, NULL, NULL, &err);
				break;
			case cOpenClDevice::openClDeviceTypeCPU:
				context = new cl::Context(CL_DEVICE_TYPE_CPU, cprops, NULL, NULL, &err);
				break;
			case cOpenClDevice::openClDeviceTypeDEF:
				context = new cl::Context(CL_DEVICE_TYPE_DEFAULT, cprops, NULL, NULL, &err);
				break;
			case cOpenClDevice::openClDeviceTypeGPU:
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
		contextReady = false;
	}
}

void cOpenClHardware::ListOpenClDevices()
{
	devicesInformation.clear();
	clDeviceWorkers.clear();
	clDevices.clear();

	if (contextReady)
	{
		clDevices = context->getInfo<CL_CONTEXT_DEVICES>();

		if (clDevices.size() > 0)
		{
			for (unsigned int i = 0; i < clDevices.size(); i++)
			{
				cOpenClDevice::sDeviceInformation deviceInformation;
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
				clDevices[i].getInfo(
					CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &deviceInformation.maxConstantBufferSize);
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
				qDebug() << "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE"
								 << deviceInformation.globalMemCachelineSize;
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
				clDeviceWorkers.append(cOpenClDevice(clDevices[i], deviceInformation));
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

void cOpenClHardware::SelectDevice(int index)
{
	if (index < devicesInformation.size())
	{
		if (devicesInformation[index].compilerAvailable)
		{
			selectedDeviceIndex = index;
		}
		else
		{
			qCritical() << "Selected device doens't have compiler";
		}
	}
	else
	{
		qCritical() << "Too high device index";
	}
}

#endif
