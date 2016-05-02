/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRenderingConfiguration - holds configuration for the render engine
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "rendering_configuration.hpp"
#include "system.hpp"
#include "netrender.hpp"

cRenderingConfiguration::cRenderingConfiguration()
{
	//default settings
	enableProgressiveRender = true;
	enableImageRefresh = true;
	enableNetRender = false;
	enableMultiThread = true;
	enableIgnoreErrors = false;
	refreshRate = 1000;
	maxRenderTime = 1e50;
}

bool cRenderingConfiguration::UseNetRender() const
{
	return (gNetRender->IsClient() || gNetRender->IsServer()) && enableNetRender;
}

bool cRenderingConfiguration::UseImageRefresh() const
{
	return enableImageRefresh;
}

bool cRenderingConfiguration::UseProgressive() const
{
	return enableProgressiveRender && !UseNetRender();
}

int cRenderingConfiguration::GetNumberOfThreads() const
{
	if (enableMultiThread) return systemData.numberOfThreads;
	else return 1;
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
	if (UseNetRender()) return refreshRate / 10;
	else return refreshRate;
}

bool cRenderingConfiguration::UseIgnoreErrors() const
{
	return enableIgnoreErrors;
}
