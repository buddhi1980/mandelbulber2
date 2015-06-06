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

void cAnimationFrames::AddFrame(const cParameterContainer &params, const cFractalContainer &fractal, int index)
{
	sAnimationFrame frame;
	frame.alreadyRendered = false;
	for(int i=0; i < listOfParameters.size(); ++i)
	{
		const cParameterContainer *container = ContainerSelector(listOfParameters[i].containerName, &params, &fractal);
		if(container)
		{
			QString parameterName = listOfParameters[i].parameterName;
			frame.parameters.AddParamFromOneParameter(container->GetContainerName() + "_" + parameterName, container->GetAsOneParameter(parameterName));
		}
		else
		{
			qCritical() << "cAnimationFrames::AddFrame(const cParameterContainer &params, const cFractalContainer &fractal): Wrong container name: " << listOfParameters[i].containerName;
		}
	}
	int indexTemp = index;
	if(index == -1) indexTemp = frames.size();
	frames.insert(indexTemp, frame);
}


void cAnimationFrames::AddAnimatedParameter(const QString &parameterName, const cOneParameter &defaultValue)
{
	if(IndexOnList(parameterName, defaultValue.GetOriginalContainerName()) == -1)
	{
		listOfParameters.append(sParameterDescription(parameterName, defaultValue.GetOriginalContainerName(), defaultValue.GetValueType(), defaultValue.GetMorphType()));
		for(int i = 0; i<frames.size(); ++i)
		{
			frames[i].parameters.AddParamFromOneParameter(defaultValue.GetOriginalContainerName() + "_" + parameterName, defaultValue);
		}
	}
	else
	{
		qWarning() << "cAnimationFrames::AddAnimatedParameter(const QString &parameterName, const cOneParameter &defaultValue): element '" << parameterName << "' already exists" << endl;
	}
}

bool cAnimationFrames::AddAnimatedParameter(const QString &fullParameterName, const cParameterContainer *param, const cFractalContainer *fractal)
{
	int firstUnderscore = fullParameterName.indexOf('_');
	QString containerName = fullParameterName.left(firstUnderscore);
	QString parameterName = fullParameterName.mid(firstUnderscore + 1);

	const cParameterContainer *container = ContainerSelector(containerName, param, fractal);
	if(container)
	{
		cOneParameter parameter = container->GetAsOneParameter(parameterName);
		if(parameter.IsEmpty())
		{
			qWarning() << "cAnimationFrames::AddAnimatedParameter(const QString &fullParameterName, const cParameterContainer *param, const cFractalContainer *fractal): unknown parameter" << fullParameterName;
			return false;
		}
		else
		{
			AddAnimatedParameter(parameterName, container->GetAsOneParameter(parameterName));
			return true;
		}
	}
	else
	{
		qCritical() << "cAnimationFrames::AddAnimatedParameter(const QString &fullParameterName, const cParameterContainer *param, const cFractalContainer *fractal): Wrong container name: " << containerName;
		return false;
	}
}

int cAnimationFrames::GetUnrenderedTotal()
{
	return GetUnrenderedTillIndex(frames.count() - 1);
}

int cAnimationFrames::GetUnrenderedTillIndex(int index)
{
	if(index >= 0 && index < frames.count())
	{
		int count = 0;
		for(int i = 0; i < index; i++){
			if(!frames.at(i).alreadyRendered) count++;
		}
		return count;
	}
	else
	{
		qWarning() << "cAnimationFrames::GetUnrenderedTillIndex(int index): wrong index" << index;
		return 0;
	}
}

int cAnimationFrames::GetNumberOfFrames() const
{
	return frames.count();
}

void cAnimationFrames::Clear()
{
	frames.clear();
}

void cAnimationFrames::ClearAll()
{
	frames.clear();
	listOfParameters.clear();
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
	const cParameterContainer *container = NULL;
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

cParameterContainer* cAnimationFrames::ContainerSelector(QString containerName, cParameterContainer *params, cFractalContainer *fractal) const
{
	cParameterContainer *container = NULL;
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
			qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, cParameterContainer *params, cFractalContainer *fractal): wrong fracta container index" << containerName << index;
		}
	}
	else
	{
		qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, cParameterContainer *params, cFractalContainer *fractal): wrong container name" << containerName;
	}

	return container;
}

cAnimationFrames::sAnimationFrame cAnimationFrames::GetFrame(int index) const
{
	if(index >= 0 && index < frames.count())
	{
		return frames.at(index);
	}
	else
	{
		qWarning() << "cAnimationFrames::GetFrame(int index): wrong index" << index;
		return sAnimationFrame();
	}
}

void cAnimationFrames::GetFrameAndConsolidate(int index, cParameterContainer *params, cFractalContainer *fractal)
{
	if(index >= 0 && index < frames.count())
	{
		cParameterContainer frame = frames.at(index).parameters;

		for(int i=0; i < listOfParameters.size(); ++i)
		{
			cParameterContainer *container = ContainerSelector(listOfParameters[i].containerName, params, fractal);
			QString parameterName = listOfParameters[i].parameterName;
			cOneParameter oneParameter = frames[index].parameters.GetAsOneParameter(listOfParameters[i].containerName + "_" + parameterName);
			container->SetFromOneParameter(parameterName, oneParameter);
		}

	}
	else
	{
		qWarning() << "cAnimationFrames::GetFrame(int index): wrong index" << index;
	}
}

void cAnimationFrames::RemoveAnimatedParameter(const QString &fullParameterName)
{
	for(int i = 0; i<frames.size(); ++i)
	{
		frames[i].parameters.DeleteParameter(fullParameterName);
	}

	for(int i=0; i<listOfParameters.size(); ++i)
	{
		if(listOfParameters[i].containerName + "_" + listOfParameters[i].parameterName == fullParameterName)
		{
			listOfParameters.removeAt(i);
			break;
		}
	}
}

void cAnimationFrames::DeleteFrames(int begin, int end)
{
	for(int i = end; i >= begin; i--)
	{
		frames.removeAt(i);
	}
}

void cAnimationFrames::ModifyFrame(int index, sAnimationFrame &frame)
{
	if(index >= 0 && index < frames.size())
	{
		frames[index] = frame;
	}
}

void cAnimationFrames::AddFrame(const sAnimationFrame &frame)
{
	frames.append(frame);
}

