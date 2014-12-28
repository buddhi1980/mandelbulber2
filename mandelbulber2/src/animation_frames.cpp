/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Class to store settings for animation frames
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

#include "animation_frames.hpp"

cAnimationFrames::cAnimationFrames()
{
}

void cAnimationFrames::AddFrame(const cParameterContainer &params, const cFractalContainer &fractal)
{
	sAnimationFrame frame;

	for(int i=0; i < listOfParameters.size(); ++i)
	{
		const cParameterContainer *container = ContainerSelector(listOfParameters[i].containerName, &params, &fractal);
		QString parameterName = listOfParameters[i].parameterName;
		frame.par.AddParamFromOneParameter(container->GetContainerName() + "_" + parameterName, container->GetAsOneParameter(parameterName));
	}
	frames.append(frame);
}


void cAnimationFrames::AddAnimagedParameter(const QString &parameterName, const cOneParameter &defaultValue)
{
	if(IndexOnList(parameterName, defaultValue.GetOriginalContainerName()) == -1)
	{
		listOfParameters.append(sParameterDescription(parameterName, defaultValue.GetOriginalContainerName(), defaultValue.GetValueType()));
		for(int i = 0; i<frames.size(); ++i)
		{
			frames[i].par.AddParamFromOneParameter(defaultValue.GetOriginalContainerName() + "_" + parameterName, defaultValue);
		}
	}
	else
	{
		qWarning() << "cAnimationFrames::AddAnimagedParameter(const QString &parameterName, const cOneParameter &defaultValue): element '" << parameterName << "' already exists" << endl;
	}
}

int cAnimationFrames::GetNumberOfFrames()
{
	return frames.count();
}

void cAnimationFrames::Clear()
{
	frames.clear();
}

int cAnimationFrames::IndexOnList(QString parameterName, QString containerName)
{
	int index = -1;
	for(int i=0; i<listOfParameters.size(); ++i)
	{
		if(listOfParameters[i].parameterName == parameterName && listOfParameters[i].containerName == containerName)
		{
			index = i;
			break;
		}
	}
	return index;
}

const cParameterContainer* cAnimationFrames::ContainerSelector(QString containerName, const cParameterContainer *params, const cFractalContainer *fractal) const
{
	const cParameterContainer *container;
	if(containerName == "main")
	{
		container = params;
	}
	else if(containerName.indexOf("fractal") >=0 )
	{
		int index = containerName.right(1).toInt();
		if(index < 4)
		{
			container = &fractal->at(index);
		}
		else
		{
			qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, const cParameterContainer *params, const cFractalContainer *fractal): wrong fracta container index" << containerName << index;

		}
	}
	else
	{
		qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, const cParameterContainer *params, const cFractalContainer *fractal): wrong container name" << containerName;
	}

	return container;
}

cParameterContainer cAnimationFrames::GetFrame(int index)
{
	if(index >= 0 && index < frames.count())
	{
		return frames.at(index).par;
	}
	else
	{
		qWarning() << "cAnimationFrames::GetFrame(int index): wrong index" << index;
		return cParameterContainer();
	}
}
