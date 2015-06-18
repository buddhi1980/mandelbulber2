/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Class to store settings for animation keyframes
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 */

#include "keyframes.hpp"
#include "animation_frames.hpp"

cKeyframes::cKeyframes() : cAnimationFrames()
{
	framesPerKeyframe = 5;
}

cKeyframes::~cKeyframes()
{
	qDeleteAll(morph);
	morph.clear();
}

cKeyframes::cKeyframes(const cKeyframes &source)
{
	*this = source;
}

cKeyframes& cKeyframes::operator=(const cKeyframes &source)
{
	if(this != &source)
	{
		qDeleteAll(morph);
		morph.clear();
		for(int i = 0; i < source.morph.size(); i++){
			this->morph.append(new cMorph(*source.morph.at(i)));
		}
	}
	this->frames = source.frames;
	this->listOfParameters = source.listOfParameters;
	this->framesPerKeyframe = source.framesPerKeyframe;

	return *this;
}

cAnimationFrames::sAnimationFrame cKeyframes::GetInterpolatedFrame(int index)
{
	int keyframe = index / framesPerKeyframe;
	int subindex = index % framesPerKeyframe;

	//if(subindex == 0)
	//{
		// no need to interpolate
	//	return GetFrame(keyframe);
	//}

	sAnimationFrame interpolated;
	QList<QString> parameterList = frames.at(keyframe).parameters.GetListOfParameters();
	for (int i = 0; i < parameterList.size(); i++)
	{
		// prepare interpolator
		if(morph.size() <= i)
		{
			morph.append(new cMorph());
		}
		for(int k = fmax(0, keyframe - 2); k <= fmin(frames.size() - 1, keyframe + 3); k++)
		{
			if(morph[i]->findInMorph(k) == -1)
			{
				morph[i]->AddData(k, frames.at(k).parameters.GetAsOneParameter(parameterList.at(i)));
			}
		}
		// interpolate each parameter and write back
		interpolated.parameters.AddParamFromOneParameter(parameterList.at(i), morph[i]->Interpolate(keyframe, 1.0 * subindex / framesPerKeyframe));
	}
	return interpolated;
}

void cKeyframes::GetInterpolatedFrameAndConsolidate(int index, cParameterContainer *params, cFractalContainer *fractal)
{
	if(index >= 0 && index < frames.count() * framesPerKeyframe)
	{
		sAnimationFrame frame = GetInterpolatedFrame(index);

		for(int i=0; i < listOfParameters.size(); ++i)
		{
			cParameterContainer *container = ContainerSelector(listOfParameters[i].containerName, params, fractal);
			QString parameterName = listOfParameters[i].parameterName;
			cOneParameter oneParameter = frame.parameters.GetAsOneParameter(listOfParameters[i].containerName + "_" + parameterName);
			container->SetFromOneParameter(parameterName, oneParameter);
		}
	}
	else
	{
		qWarning() << "cAnimationFrames::GetInterpolatedFrameAndConsolidate(int index): wrong index" << index;
	}
}

int cKeyframes::GetUnrenderedTotal()
{
		return GetUnrenderedTillIndex(frames.count() * GetFramesPerKeyframe() - 1);
}

int cKeyframes::GetUnrenderedTillIndex(int frameIndex)
{
	if(frameIndex >= 0 && frameIndex < frames.count() * GetFramesPerKeyframe())
	{
		int count = 0;
		for(int index = 0; index < frameIndex; ++index)
		{
			int keyframe = index / GetFramesPerKeyframe();
			int subindex = index % GetFramesPerKeyframe();
			if(!frames.at(keyframe).alreadyRenderedSubFrames[subindex]) count++;
		}
		return count;
	}
	else
	{
		qWarning() << "cAnimationFrames::GetUnrenderedTillIndex(int index): wrong frameIndex: " << frameIndex;
		return 0;
	}
}

void cKeyframes::ChangeMorphType(int parameterIndex, parameterContainer::enumMorphType morphType)
{
	using namespace parameterContainer;
	enumMorphType oldMorphType = listOfParameters[parameterIndex].morphType;
	if(morphType != oldMorphType)
	{
		if(parameterIndex < morph.size())	morph[parameterIndex]->Clear();

		listOfParameters[parameterIndex].morphType = morphType;
		QString fullParameterName = listOfParameters[parameterIndex].containerName + "_" + listOfParameters[parameterIndex].parameterName;

		for(int i = 0; i < frames.size(); i++)
		{
			cOneParameter parameter = frames[i].parameters.GetAsOneParameter(fullParameterName);
			parameter.SetMorphType(morphType);
			frames[i].parameters.SetFromOneParameter(fullParameterName, parameter);
		}
	}
}
void cKeyframes::AddAnimatedParameter(const QString &parameterName, const cOneParameter &defaultValue)
{
	morph.clear();
	cAnimationFrames::AddAnimatedParameter(parameterName, defaultValue);
}

bool cKeyframes::AddAnimatedParameter(const QString &fullParameterName, const cParameterContainer *param, const cFractalContainer *fractal)
{
	morph.clear();
	return cAnimationFrames::AddAnimatedParameter(fullParameterName, param, fractal);
}

void cKeyframes::RemoveAnimatedParameter(const QString &fullParameterName)
{
	morph.clear();
	cAnimationFrames::RemoveAnimatedParameter(fullParameterName);
}
