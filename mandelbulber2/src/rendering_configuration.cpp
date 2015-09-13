/*
 * rendering_configuration.cpp
 *
 *  Created on: 13 wrz 2015
 *      Author: krzysztof
 */

#include "rendering_configuration.hpp"
#include "system.hpp"
#include "global_data.hpp"

cRenderingConfiguration::cRenderingConfiguration()
{
	//default settings
	enableProgressiveRender = true;
	enableImageRefresh = true;
	enableConsoleOutput = false;
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
	return enableProgressiveRender && UseImageRefresh() && !UseNetRender();
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

bool cRenderingConfiguration::UseConsoleOutput() const
{
	return enableConsoleOutput;
}

bool cRenderingConfiguration::UseRenderTimeEffects() const
{
	return UseProgressive() && UseImageRefresh();
}

int cRenderingConfiguration::GetRefreshRate() const
{
	if(UseNetRender())
		return refreshRate / 10;
	else
		return refreshRate;
}

bool cRenderingConfiguration::UseIgnoreErrors() const
{
	return enableIgnoreErrors;
}
