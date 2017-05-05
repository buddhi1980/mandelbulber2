/*
 * open_cl_engine.h
 *
 *  Created on: 2 maj 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_HARDWARE_H_
#define MANDELBULBER2_SRC_OPENCL_HARDWARE_H_

#include <QtCore>

#ifdef USE_OPENCL
#include <CL/cl.hpp>
#endif

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
};

#endif /* MANDELBULBER2_SRC_OPEN_CL_HARDWARE_H_ */
