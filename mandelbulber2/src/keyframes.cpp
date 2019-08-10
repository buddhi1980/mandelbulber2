/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * Class to store settings for keyframe animation
 *
 * Handles the 2D matrix of the list of parameters / list of frames
 * and exposes functions to modify this matrix. This functionality is
 * derived from the cAnimationFrames class. Additionally this class
 * interpolates sub-frames with the help of the cMorph class.
 */

#include "keyframes.hpp"

cKeyframes *gKeyframes = nullptr;

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

cKeyframes &cKeyframes::operator=(const cKeyframes &source)
{
	if (this != &source)
	{
		qDeleteAll(morph);
		morph.clear();
		for (auto i : source.morph)
		{
			morph.append(new cMorph(*i));
		}
	}
	frames = source.frames;
	listOfParameters = source.listOfParameters;
	framesPerKeyframe = source.framesPerKeyframe;
	audioTracks = source.audioTracks;
	return *this;
}

cAnimationFrames::sAnimationFrame cKeyframes::GetInterpolatedFrame(
	int index, cParameterContainer *params, cFractalContainer *fractal)
{
	Q_UNUSED(fractal);
	int keyframe = index / framesPerKeyframe;
	int subIndex = index % framesPerKeyframe;

	// if(subIndex == 0)
	//{
	// no need to interpolate
	//	return GetFrame(keyframe);
	//}

	sAnimationFrame interpolated;

	for (int i = 0; i < listOfParameters.size(); i++)
	{
		QString fullParameterName =
			listOfParameters[i].containerName + "_" + listOfParameters[i].parameterName;

		// prepare interpolator
		if (morph.size() <= i)
		{
			morph.append(new cMorph());
		}
		for (int k = qMax(0, keyframe - 2); k <= qMin(frames.size() - 1, keyframe + 3); k++)
		{
			if (morph[i]->findInMorph(k) == -1)
			{
				morph[i]->AddData(k, frames.at(k).parameters.GetAsOneParameter(fullParameterName));
			}
		}
		// interpolate each parameter
		cOneParameter oneParameter =
			morph[i]->Interpolate(keyframe, 1.0 * subIndex / framesPerKeyframe);

		// apply audio animation

		oneParameter =
			ApplyAudioAnimation(index, oneParameter, listOfParameters[i].parameterName, params);
		interpolated.parameters.AddParamFromOneParameter(fullParameterName, oneParameter);
	}
	return interpolated;
}

void cKeyframes::GetInterpolatedFrameAndConsolidate(
	int index, cParameterContainer *params, cFractalContainer *fractal)
{
	if (index >= 0 && index < frames.count() * framesPerKeyframe)
	{
		sAnimationFrame frame = GetInterpolatedFrame(index, params, fractal);

		for (auto &listOfParameter : listOfParameters)
		{
			cParameterContainer *container =
				ContainerSelector(listOfParameter.containerName, params, fractal);
			QString parameterName = listOfParameter.parameterName;
			cOneParameter oneParameter =
				frame.parameters.GetAsOneParameter(listOfParameter.containerName + "_" + parameterName);

			container->SetFromOneParameter(parameterName, oneParameter);
		}
	}
	else
	{
		qWarning() << "cAnimationFrames::GetInterpolatedFrameAndConsolidate(int index): wrong index"
							 << index;
	}
}

void cKeyframes::ChangeMorphType(int parameterIndex, parameterContainer::enumMorphType morphType)
{
	using namespace parameterContainer;
	enumMorphType oldMorphType = listOfParameters[parameterIndex].morphType;
	if (morphType != oldMorphType)
	{
		if (parameterIndex < morph.size()) morph[parameterIndex]->Clear();

		listOfParameters[parameterIndex].morphType = morphType;
		QString fullParameterName = listOfParameters[parameterIndex].containerName + "_"
																+ listOfParameters[parameterIndex].parameterName;

		for (auto &frame : frames)
		{
			cOneParameter parameter = frame.parameters.GetAsOneParameter(fullParameterName);
			parameter.SetMorphType(morphType);
			frame.parameters.SetFromOneParameter(fullParameterName, parameter);
		}
	}
}
void cKeyframes::AddAnimatedParameter(
	const QString &parameterName, const cOneParameter &defaultValue, cParameterContainer *params)
{
	morph.clear();
	cAnimationFrames::AddAnimatedParameter(parameterName, defaultValue, params);
}

bool cKeyframes::AddAnimatedParameter(
	const QString &fullParameterName, cParameterContainer *param, const cFractalContainer *fractal)
{
	morph.clear();
	return cAnimationFrames::AddAnimatedParameter(fullParameterName, param, fractal);
}

void cKeyframes::RemoveAnimatedParameter(const QString &fullParameterName)
{
	morph.clear();
	cAnimationFrames::RemoveAnimatedParameter(fullParameterName);
}

void cKeyframes::setAudioParameterPrefix()
{
	audioTracks.SetPrefix("animsound");
}
