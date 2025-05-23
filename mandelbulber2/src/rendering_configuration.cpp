/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cRenderingConfiguration - holds configuration for the render engine
 */

#include "rendering_configuration.hpp"

#include "netrender.hpp"
#include "system_data.hpp"

cRenderingConfiguration::cRenderingConfiguration()
{
	// default settings
	enableProgressiveRender = true;
	enableImageRefresh = true;
	enableNetRender = false;
	enableMultiThread = true;
	enableIgnoreErrors = false;
	forceFastPreview = false;
	nebulaMode = false;
	refreshRate = 1000;
	maxRenderTime = 1e50;
}

bool cRenderingConfiguration::UseNetRender() const
{
	return (gNetRender->IsClient() || gNetRender->IsServer()) && enableNetRender && !nebulaMode;
}

bool cRenderingConfiguration::UseImageRefresh() const
{
	return enableImageRefresh;
}

bool cRenderingConfiguration::UseProgressive() const
{
	return enableProgressiveRender && !UseNetRender() && !nebulaMode;
}

int cRenderingConfiguration::GetNumberOfThreads() const
{
	if (enableMultiThread)
		return systemData.numberOfThreads;
	else
		return 1;
}

bool cRenderingConfiguration::UseRefreshRenderedList() const
{
	return enableImageRefresh || UseNetRender();
}

bool cRenderingConfiguration::UseRenderTimeEffects() const
{
	return UseProgressive() && UseImageRefresh();
}

int cRenderingConfiguration::GetRefreshRate() const
{
	if (UseNetRender())
		return refreshRate / 10;
	else
		return refreshRate;
}

bool cRenderingConfiguration::UseIgnoreErrors() const
{
	return enableIgnoreErrors;
}

bool cRenderingConfiguration::UseForcedFastPreview() const
{
	return forceFastPreview;
}

bool cRenderingConfiguration::IsNebulaMode() const
{
	return nebulaMode;
}
