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

#ifndef MANDELBULBER2_SRC_OPENCL_HARDWARE_H_
#define MANDELBULBER2_SRC_OPENCL_HARDWARE_H_

#include <QtCore>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4005) // macro redefinition
#pragma warning(disable : 4996) // declared deprecated
#endif  // _MSC_VER

#ifdef USE_OPENCL
#include <CL/cl.hpp>
#endif // USE_OPENCL

#if defined(_MSC_VER)
#pragma warning(pop)
#endif // _MSC_VER

class cOpenClHardware : public QObject
{
	Q_OBJECT

public:
	enum enumOpenClDeviceType
	{
		openClDeviceTypeCPU,
		openClDeviceTypeGPU
	};

	struct sPlatformInformation
	{
		QString name;
		QString vendor;
		QString version;
		QString profile;
	};

#ifdef USE_OPENCL
	struct sDeviceInformation
	{
		cl_bool deviceAvailable;
		cl_bool compilerAvailable;
		cl_device_fp_config doubleFpConfig;
		cl_ulong globalMemCacheSize;
		cl_uint globalMemCachelineSize;
		cl_ulong globalMemSize;
		cl_ulong localMemSize;
		cl_uint maxClockFrequency;
		cl_uint maxComputeUnits;
		cl_ulong maxConstantBufferSize;
		cl_ulong maxMemAllocSize;
		size_t maxParameterSize;
		size_t maxWorkGroupSize;
		QString deviceName;
		QString deviceVersion;
		QString driverVersion;
	};
#endif

	cOpenClHardware(QObject *parent = nullptr);
	~cOpenClHardware();

#ifdef USE_OPENCL
public:
	void ListOpenClPlatforms();
	void CreateContext(int platformIndex, enumOpenClDeviceType deviceType);
	const QList<sPlatformInformation> &getPlatformsInformation() const
	{
		return platformsInformation;
	}
	const QList<sDeviceInformation> &getDevicesInformation() const { return devicesInformation; }

	void SelectDevice(int index);

	const std::vector<cl::Device> &getClDevices() const { return clDevices; }
	const cl::Device &getSelectedDevice() const { return clDevices[selectedDeviceIndex]; }
	cl::Context *getContext() { return context; }
	const sDeviceInformation &getSelectedDeviceInformation() const
	{
		return devicesInformation[selectedDeviceIndex];
	}

protected:
	bool checkErr(cl_int err, QString fuctionName);

private:
	void ListOpenClDevices();

protected:
	std::vector<cl::Device> clDevices;
	QList<sPlatformInformation> platformsInformation;
	QList<sDeviceInformation> devicesInformation;
	cl::Context *context;

private:
	std::vector<cl::Platform> clPlatforms;

#endif

protected:
	bool openClAvailable;
	bool contextReady;
	int selectedDeviceIndex;
};

#endif /* MANDELBULBER2_SRC_OPEN_CL_HARDWARE_H_ */
