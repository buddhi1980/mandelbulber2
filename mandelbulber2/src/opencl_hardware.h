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

#include "include_header_wrapper.hpp"
#include "opencl_device.h"

class cOpenClHardware : public QObject
{
	Q_OBJECT

public:
	struct sPlatformInformation
	{
		QString name;
		QString vendor;
		QString version;
		QString profile;
	};

	cOpenClHardware(QObject *parent = nullptr);
	~cOpenClHardware();

#ifdef USE_OPENCL
public:
	void ListOpenClPlatforms();
	void CreateContext(int platformIndex, cOpenClDevice::enumOpenClDeviceType deviceType);
	const QList<sPlatformInformation> &getPlatformsInformation() const
	{
		return platformsInformation;
	}
	const QList<cOpenClDevice::sDeviceInformation> &getDevicesInformation() const
	{
		return devicesInformation;
	}

	void EnableDevice(int index);
	void DisableDevice(int index);
	void EnableDevicesByHashList(const QString &list);

	const std::vector<cl::Device> &getClDevices() const { return clDevices; }
	const QList<cOpenClDevice> &getClWorkers() const { return clDeviceWorkers; }
	const cl::Device &getEnabledDevices() const
	{
		return clDeviceWorkers[selectedDeviceIndex].getDevice();
	}
	cl::Context *getContext() const { return context; }
	const cOpenClDevice::sDeviceInformation &getSelectedDeviceInformation() const
	{
		return clDeviceWorkers[selectedDeviceIndex].getDeviceInformation();
	}
	int getSelectedPlatformIndex() { return selectedPlatformIndex; }
	int getSelectedDeviceIndex() { return selectedDeviceIndex; }
	bool ContextCreated() const { return contextReady; }

	bool IsNVidia() const { return isNVidia; }

protected:
	static bool checkErr(cl_int err, QString functionName);

private:
	void ListOpenClDevices();

protected:
	std::vector<cl::Device> clDevices;
	QList<cOpenClDevice> clDeviceWorkers;
	QList<sPlatformInformation> platformsInformation;
	QList<cOpenClDevice::sDeviceInformation> devicesInformation;

	// TODO: The System only supports (1) platform
	// 1 context == 1 platform
	cl::Context *context;
	bool isNVidia;

private:
	std::vector<cl::Platform> clPlatforms;

#endif

protected:
	bool openClAvailable;
	bool contextReady;
	int selectedPlatformIndex;
	int selectedDeviceIndex;
	bool missingOpenClDLL;
};

#endif /* MANDELBULBER2_SRC_OPENCL_HARDWARE_H_ */
