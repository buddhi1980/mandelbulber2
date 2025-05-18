/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cGlobalOpenCl - OpenCL setup code
 */

#include "opencl_global.h"

#include "initparameters.hpp"
#include "interface.hpp"
#include "opencl_engine_render_dof.h"
#include "opencl_engine_render_fractal.h"
#include "opencl_engine_render_post_filter.h"
#include "opencl_engine_render_ssao.h"
#include "opencl_engine_render_nebula.h"
#include "opencl_hardware.h"

cGlobalOpenCl *gOpenCl = nullptr;

cGlobalOpenCl::cGlobalOpenCl(QObject *parent) : QObject(parent)
{
#ifdef USE_OPENCL
	openClHardware = new cOpenClHardware(this);

	openClEngineRenderFractal = new cOpenClEngineRenderFractal(openClHardware);
	openClEngineRenderSSAO = new cOpenClEngineRenderSSAO(openClHardware);
	openclEngineRenderDOF = new cOpenClEngineRenderDOF(openClHardware);
	openclEngineRenderPostFilter = new cOpenClEngineRenderPostFilter(openClHardware);
	openclEngineRenderNebula = new cOpenClEngineRenderNebula(openClHardware);
#endif
}

cGlobalOpenCl::~cGlobalOpenCl()
{
	// all deleted by parent objects
}

#ifdef USE_OPENCL
void cGlobalOpenCl::Reset()
{
	openClEngineRenderFractal->Reset();
	openClEngineRenderSSAO->Reset();
	openclEngineRenderDOF->Reset();
	openclEngineRenderPostFilter->Reset();
	openclEngineRenderNebula->Reset();
}

void cGlobalOpenCl::InitPlatfromAndDevices()
{
	if (gPar->Get<bool>("opencl_enabled"))
	{
		QTextStream out(stdout);

		openClHardware->ListOpenClPlatforms();

		int selectedPlatformIndex = gPar->Get<int>("opencl_platform");

		if (openClHardware->getNumberOfPlatforms() > 0)
		{
			if (selectedPlatformIndex < 0
					|| selectedPlatformIndex > openClHardware->getNumberOfPlatforms() - 1)
			{
				qCritical() << "Selected wrong OpenCL platform. Will be used first avaiable.";
				selectedPlatformIndex = 0;
				gPar->Set("opencl_platform", 0);
			}

			if (selectedPlatformIndex >= 0)
			{
				cOpenClDevice::enumOpenClDeviceType deviceType =
					cOpenClDevice::enumOpenClDeviceType(gPar->Get<int>("opencl_device_type"));

				openClHardware->ListOpenClDevices(selectedPlatformIndex, deviceType);
				openClHardware->EnableDevicesByHashList(gPar->Get<QString>("opencl_device_list"));

				openClHardware->CreateAllContexts(selectedPlatformIndex, deviceType);
			}
		}
		else
		{
			qCritical() << "No OpenCL platforms available!";
			gPar->Set("opencl_enabled", false);
		}

		QList<cOpenClDevice> clDeviceWorkers = openClHardware->getClWorkers();
		out << "Selected OpenCL devices:\n";
		for (int i = 0; i < clDeviceWorkers.size(); i++)
		{
			out << "Device " << i << " : " << clDeviceWorkers[i].getDeviceInformation().deviceName
					<< "\n";
		}
	}
}
#endif
