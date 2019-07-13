/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "error_message.hpp"
#include "opencl_device.h"
#include "system.hpp"

cOpenClHardware::cOpenClHardware(QObject *parent) : QObject(parent)
{
	openClAvailable = false;
	contextReady = false;
	// initialize multi-gpu devices' indices list with empty QList
	selectedDevicesIndices = QList<int>();
	missingOpenClDLL = false;
	selectedPlatformIndex = 0;

#ifdef USE_OPENCL
#ifdef _WIN32
#ifndef _MSC_VER
	const std::wstring openclDll(L"OpenCL.dll");
	int err = clewInit(openclDll.c_str());
	if (err)
	{
		qCritical() << clewErrorString(err);
		missingOpenClDLL = true;
	}
#endif //   _MSC_VER
#endif
	isNVidia = false;
	isAMD = false;

	// first context is created to query number of available devices
	contexts.resize(1);
	contexts[0] = nullptr;
#endif
}

cOpenClHardware::~cOpenClHardware()
{
#ifdef USE_OPENCL
	// deleting existing contexts
	for (cl::Context *context : contexts)
	{
		if (context) delete context;
	}
	contexts.clear();
#endif
}

#ifdef USE_OPENCL
void cOpenClHardware::ListOpenClPlatforms()
{
	clPlatforms.clear();
	platformsInformation.clear();

	if (!missingOpenClDLL)
	{
		// getting list of available platforms
		cl::Platform::get(&clPlatforms);
		if (checkErr(clPlatforms.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get"))
		{
			openClAvailable = true;

			std::string platformName;
			std::string platformVendor;
			std::string platformVersion;
			std::string platformProfile;

			// collecting information about platforms
			for (unsigned int i = 0; i < clPlatforms.size(); i++)
			{
				clPlatforms[i].getInfo(cl_platform_info(CL_PLATFORM_NAME), &platformName);
				clPlatforms[i].getInfo(cl_platform_info(CL_PLATFORM_VENDOR), &platformVendor);
				clPlatforms[i].getInfo(cl_platform_info(CL_PLATFORM_VERSION), &platformVersion);
				clPlatforms[i].getInfo(cl_platform_info(CL_PLATFORM_PROFILE), &platformProfile);

				sPlatformInformation platformInformation;
				platformInformation.name = platformName.c_str();
				platformInformation.vendor = platformVendor.c_str();
				platformInformation.version = platformVersion.c_str();
				platformInformation.profile = platformProfile.c_str();

				WriteLog(QString("OpenCL platform #") + QString::number(i) + ": " + platformInformation.name
									 + " " + platformInformation.vendor + " " + platformInformation.version + " "
									 + platformInformation.profile,
					2);

				platformsInformation.append(platformInformation);
			}
		}
		else
		{
			openClAvailable = false;
			cErrorMessage::showMessage(QObject::tr("There are no valid OpenCl platforms in the system"),
				cErrorMessage::errorMessage);
		}
	}
	else
	{
		openClAvailable = false;
		cErrorMessage::showMessage(
			QObject::tr("Missing OpenCL.dll in the system"), cErrorMessage::errorMessage);
	}
}

void cOpenClHardware::CreateContext(
	int platformIndex, cOpenClDevice::enumOpenClDeviceType deviceType)
{
	selectedPlatformIndex = platformIndex;
	if (platformIndex >= 0)
	{
		if (openClAvailable)
		{
			// platformIndex required
			// context operates exclusively with (1) platform
			cl_context_properties cProps[3] = {
				CL_CONTEXT_PLATFORM, cl_context_properties((clPlatforms[platformIndex])()), 0};

			// deleting existing contexts
			for (cl::Context *context : contexts)
			{
				if (context) delete context;
			}
			contexts.clear();
			contexts.resize(1);
			contextReady = false;

			cl_int err = 0;

			// Constructs a context including all or a subset of devices of a specified type.
			// supports multiple devices

			int numberOfContexts = 1;
			clDevices.clear();
			devicesInformation.clear();
			clDeviceWorkers.clear();

			for (int contextIndex = 0; contextIndex < numberOfContexts; contextIndex++)
			{
				switch (deviceType)
				{
					case cOpenClDevice::openClDeviceTypeACC:
						contexts[contextIndex] =
							new cl::Context(CL_DEVICE_TYPE_ACCELERATOR, cProps, nullptr, nullptr, &err);
						break;
					case cOpenClDevice::openClDeviceTypeALL:
						contexts[contextIndex] =
							new cl::Context(CL_DEVICE_TYPE_ALL, cProps, nullptr, nullptr, &err);
						break;
					case cOpenClDevice::openClDeviceTypeCPU:
						contexts[contextIndex] =
							new cl::Context(CL_DEVICE_TYPE_CPU, cProps, nullptr, nullptr, &err);
						break;
					case cOpenClDevice::openClDeviceTypeDEF:
						contexts[contextIndex] =
							new cl::Context(CL_DEVICE_TYPE_DEFAULT, cProps, nullptr, nullptr, &err);
						break;
					case cOpenClDevice::openClDeviceTypeGPU:
						contexts[contextIndex] =
							new cl::Context(CL_DEVICE_TYPE_GPU, cProps, nullptr, nullptr, &err);
						break;
				}

				if (checkErr(err, "Context::Context()"))
				{
					contextReady = true;

					ListOpenClDevices(contextIndex);
					isNVidia =
						platformsInformation[platformIndex].name.contains("nvidia", Qt::CaseInsensitive);
					isAMD = platformsInformation[platformIndex].name.contains("amd", Qt::CaseInsensitive);

					if (contextIndex == 0)
					{
						// get number of needed contexts (the same number as discovered devices)
						// number of devices is got from first context
						numberOfContexts = clDevices[0].size();
						contexts.resize(numberOfContexts);
					}
				}
				else
				{
					contextReady = false;
					cErrorMessage::showMessage(
						QObject::tr("OpenCL %1 cannot be created!").arg(QObject::tr("context")),
						cErrorMessage::errorMessage);
				}
			}
		}
		else
		{
			qCritical() << "OpenCl is not available";
			contextReady = false;
		}
	}
	else
	{
		qCritical() << "OpenCL platform index is negative number!";
		contextReady = false;
	}
}

void cOpenClHardware::ListOpenClDevices(int contextIndex)
{
	cl_int err = 0;

	if (contextReady)
	{
		clDevices.append(contexts[contextIndex]->getInfo<CL_CONTEXT_DEVICES>(&err));

		if (checkErr(err, "Context::getInfo()"))
		{
			if (clDevices.size() > 0)
			{
				for (unsigned int i = 0; i < clDevices[contextIndex].size(); i++)
				{
					cOpenClDevice::sDeviceInformation deviceInformation;
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_MAX_COMPUTE_UNITS, &deviceInformation.deviceAvailable);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_COMPILER_AVAILABLE, &deviceInformation.compilerAvailable);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_DOUBLE_FP_CONFIG, &deviceInformation.doubleFpConfig);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, &deviceInformation.globalMemCacheSize);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, &deviceInformation.globalMemCachelineSize);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_GLOBAL_MEM_SIZE, &deviceInformation.globalMemSize);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_LOCAL_MEM_SIZE, &deviceInformation.localMemSize);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_MAX_CLOCK_FREQUENCY, &deviceInformation.maxClockFrequency);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_MAX_COMPUTE_UNITS, &deviceInformation.maxComputeUnits);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &deviceInformation.maxConstantBufferSize);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_MAX_MEM_ALLOC_SIZE, &deviceInformation.maxMemAllocSize);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_MAX_PARAMETER_SIZE, &deviceInformation.maxParameterSize);
					clDevices[contextIndex][i].getInfo(
						CL_DEVICE_MAX_WORK_GROUP_SIZE, &deviceInformation.maxWorkGroupSize);

					std::string deviceName;
					clDevices[contextIndex][i].getInfo(CL_DEVICE_NAME, &deviceName);
					deviceInformation.deviceName = QString(deviceName.c_str());

					std::string deviceVersion;
					clDevices[contextIndex][i].getInfo(CL_DEVICE_VERSION, &deviceVersion);
					deviceInformation.deviceVersion = QString(deviceVersion.c_str());

					std::string driverVersion;
					clDevices[contextIndex][i].getInfo(CL_DRIVER_VERSION, &driverVersion);
					deviceInformation.driverVersion = QString(driverVersion.c_str());

					WriteLogInt("OpenCL Device # ", i, 2);
					WriteLogInt("CL_DEVICE_MAX_COMPUTE_UNITS", deviceInformation.deviceAvailable, 2);
					WriteLogInt("CL_DEVICE_COMPILER_AVAILABLE", deviceInformation.compilerAvailable, 2);
					WriteLogInt("CL_DEVICE_DOUBLE_FP_CONFIG", deviceInformation.doubleFpConfig, 2);
					WriteLogInt("CL_DEVICE_GLOBAL_MEM_CACHE_SIZE", deviceInformation.globalMemCacheSize, 2);
					WriteLogInt(
						"CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE", deviceInformation.globalMemCachelineSize, 2);
					WriteLogInt("CL_DEVICE_GLOBAL_MEM_SIZE", deviceInformation.globalMemSize, 2);
					WriteLogInt("CL_DEVICE_LOCAL_MEM_SIZE", deviceInformation.localMemSize, 2);
					WriteLogInt("CL_DEVICE_MAX_CLOCK_FREQUENCY", deviceInformation.maxClockFrequency, 2);
					WriteLogInt("CL_DEVICE_MAX_COMPUTE_UNITS", deviceInformation.maxComputeUnits, 2);
					WriteLogInt(
						"CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE", deviceInformation.maxConstantBufferSize, 2);
					WriteLogInt("CL_DEVICE_MAX_MEM_ALLOC_SIZE", deviceInformation.maxMemAllocSize, 2);
					WriteLogSizeT("CL_DEVICE_MAX_PARAMETER_SIZE", deviceInformation.maxParameterSize, 2);
					WriteLogSizeT("CL_DEVICE_MAX_WORK_GROUP_SIZE", deviceInformation.maxWorkGroupSize, 2);
					WriteLogString("CL_DEVICE_NAME", deviceInformation.deviceName, 2);
					WriteLogString("CL_DEVICE_VERSION", deviceInformation.deviceVersion, 2);
					WriteLogString("CL_DRIVER_VERSION", deviceInformation.driverVersion, 2);

					// calculate hash code
					QCryptographicHash hashCrypt(QCryptographicHash::Md4);
					hashCrypt.addData(deviceInformation.deviceName.toLocal8Bit());
					hashCrypt.addData(deviceInformation.deviceVersion.toLocal8Bit());
					QString indexString = QString::number(i);
					hashCrypt.addData(indexString.toLocal8Bit());
					deviceInformation.hash = hashCrypt.result().left(3);

					if (contextIndex == 0)
					{
						devicesInformation.append(deviceInformation);
					}

					if (int(i) == contextIndex)
					{
						// context n will use device n
						clDeviceWorkers.append(cOpenClDevice(&clDevices[i][i], deviceInformation));
					}
				}
			}
			else
			{
				cErrorMessage::showMessage(
					QObject::tr("There are no available devices for selected OpenCL platform"),
					cErrorMessage::errorMessage);
			}
		}
		else
		{
			cErrorMessage::showMessage(QObject::tr("Cannot list devices from selected OpenCL platform"),
				cErrorMessage::errorMessage);
		}
	}
}

bool cOpenClHardware::checkErr(cl_int err, QString functionName)
{
	if (err != CL_SUCCESS)
	{
		qCritical() << "OpenCl ERROR: " << functionName << " (" << err << ")";
		return false;
	}
	else
		return true;
}

void cOpenClHardware::EnableDevice(int index)
{
	if (index < devicesInformation.size())
	{
		if (devicesInformation[index].compilerAvailable)
		{
			selectedDevicesIndices.removeAll(index);
			selectedDevicesIndices.append(index);
			clDeviceWorkers[index].Enable();
		}
		else
		{
			qCritical() << "Selected device doesn't have compiler";
		}
	}
	else
	{
		qCritical() << "Too high device index";
	}
}

void cOpenClHardware::DisableDevice(int index)
{
	clDeviceWorkers[index].Disable();
}

void cOpenClHardware::EnableDevicesByHashList(const QString &list)
{
	QStringList stringList = list.split("|");

	// disable all devices
	selectedDevicesIndices.clear();
	for (int dev = 0; dev < clDeviceWorkers.size(); dev++)
	{
		DisableDevice(dev);
	}

	if (clDeviceWorkers.size() > 0)
	{
		// enable only devices from list
		for (int i = 0; i < stringList.size(); i++)
		{
			QByteArray hashFromList = QByteArray::fromHex(stringList.at(i).toLocal8Bit());

			for (int dev = 0; dev < clDeviceWorkers.size(); dev++)
			{
				QByteArray hashFromDevice = clDeviceWorkers[dev].getDeviceInformation().hash;
				if (hashFromDevice.left(3) == hashFromList.left(3))
				{
					EnableDevice(dev);
					WriteLog(QString("Device ") + clDeviceWorkers[dev].getDeviceInformation().deviceName
										 + " enabled",
						3);
				}
			}
		}

		if (selectedDevicesIndices.isEmpty())
		{
			QString errorMessage =
				"No OpenCL devices selected or selected unknown devices!\n"
				"Selected first available GPU device\n"
				"Check program preferences to select correct OpenCL device"
				"from list of available GPU devices";
			cErrorMessage::showMessage(errorMessage, cErrorMessage::errorMessage, nullptr);

			EnableDevice(0);
		}
	}
}

#endif
