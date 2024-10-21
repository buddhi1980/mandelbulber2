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

#include <QDebug>

std::shared_ptr<cKeyframes> gKeyframes;

cKeyframes::cKeyframes() : cAnimationFrames() {}

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
	audioTracks = source.audioTracks;
	keyframesIndexesTable = source.keyframesIndexesTable;
	framesIndexesTable = source.framesIndexesTable;

	return *this;
}

cOneParameter cKeyframes::InterpolateSingleParameter(int morphTableItemIndex, int keyframe,
	const QString &parameterName, const QString &fullParameterName, int subIndex, int frameIndex,
	const std::shared_ptr<cParameterContainer> &params)
{
	// Prepare the interpolator
	if (morph.size() <= morphTableItemIndex)
	{
		morph.append(
			new cMorph()); // Append a new morph object if the size of morph is less than or equal to i

		int numberOfSubFrames = 0;
		for (int k = 0; k < frames.size(); k++)
		{
			cOneParameter onePar = frames.at(k).parameters.GetAsOneParameter(fullParameterName);
			numberOfSubFrames += frames.at(k).numberOfSubFrames;
			if (!onePar.IsEmpty())
			{
				morph[morphTableItemIndex]->AddData(k, numberOfSubFrames, onePar);
				numberOfSubFrames = 0;
			}
		}
	}

	// find keyframe which is not empty

	// Loop over the frames for interpolation
	//	for (int k = keyframe - 2; k <= keyframe + 3; k++)
	//	{
	//		int kClamped;
	//		if (looped)
	//		{
	//			kClamped = ((k % frames.size()) + frames.size()) % frames.size();
	//		}
	//		else
	//		{
	//			kClamped = qBound(0, k, frames.size() - 1);
	//		}
	//		// If the keyframe is not found in the morph, add it
	//		if (morph[morphTableItemIndex]->findInMorph(k) == -1)
	//		{
	//			morph[morphTableItemIndex]->AddData(k, frames.at(kClamped).numberOfSubFrames,
	//				frames.at(kClamped).parameters.GetAsOneParameter(fullParameterName));
	//		}
	//	}
	// Interpolate each parameter
	cOneParameter oneParameter = morph[morphTableItemIndex]->Interpolate(frameIndex);
	// Apply audio animation to the parameter
	oneParameter = ApplyAudioAnimation(frameIndex, oneParameter, parameterName, params);
	return oneParameter;
}

cAnimationFrames::sAnimationFrame cKeyframes::GetInterpolatedFrame(int frameIndex,
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractal)
{
	Q_UNUSED(fractal);													 // Unused fractal parameter
	int keyframe = GetKeyframeIndex(frameIndex); // Get the keyframe index for the given frame index
	int subIndex = GetSubIndex(frameIndex);			 // Get the sub-index for the given frame index

	sAnimationFrame interpolated; // Create an object to store the interpolated frame

	// Loop over all parameters
	for (int i = 0; i < listOfParameters.size(); i++)
	{
		// Construct the full parameter name
		QString fullParameterName =
			listOfParameters[i].containerName + "_" + listOfParameters[i].parameterName;

		cOneParameter oneParameter = InterpolateSingleParameter(i, keyframe,
			listOfParameters[i].parameterName, fullParameterName, subIndex, frameIndex, params);
		// Add the interpolated parameter to the interpolated frame
		interpolated.parameters.AddParamFromOneParameter(fullParameterName, oneParameter);
	}
	return interpolated; // Return the interpolated frame
}

void cKeyframes::GetInterpolatedFrameAndConsolidate(int index,
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractal)
{
	if (index >= 0 && index < GetTotalNumberOfFrames())
	{
		sAnimationFrame frame = GetInterpolatedFrame(index, params, fractal);

		for (auto &listOfParameter : listOfParameters)
		{
			std::shared_ptr<cParameterContainer> container =
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
void cKeyframes::AddAnimatedParameter(const QString &parameterName,
	const cOneParameter &defaultValue, std::shared_ptr<cParameterContainer> params)
{
	morph.clear();
	cAnimationFrames::AddAnimatedParameter(parameterName, defaultValue, params);
}

bool cKeyframes::AddAnimatedParameter(const QString &fullParameterName,
	std::shared_ptr<cParameterContainer> param, std::shared_ptr<cFractalContainer> fractal)
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

void cKeyframes::UpdateFramesIndexesTable()
{
	int globalFrameCounter = 0;
	keyframesIndexesTable.clear();
	framesIndexesTable.clear();

	if (frames.count() > 0)
	{
		for (int k = 0; k < frames.count(); k++)
		{
			int numberOfSubframes = frames.at(k).numberOfSubFrames;

			framesIndexesTable.push_back(globalFrameCounter);

			if (!looped && k == frames.count() - 1) // last keyframe
			{
				numberOfSubframes = 1;
			}

			for (int f = 0; f < numberOfSubframes; f++)
			{
				keyframesIndexesTable.push_back(k);
				globalFrameCounter++;
			}
		}
	}
}

int cKeyframes::GetTotalNumberOfFrames() const
{
	return keyframesIndexesTable.size();
}

int cKeyframes::GetFramesPerKeyframe(int keyframeIndex) const
{
	return frames.at(keyframeIndex).numberOfSubFrames;
}

int cKeyframes::GetKeyframeIndex(int frameIndex) const
{
	if (frameIndex < keyframesIndexesTable.size())
	{
		return keyframesIndexesTable.at(frameIndex);
	}
	else
	{
		qCritical() << "GetKeyframeIndex(int frameIndex): wrong frame index: " << frameIndex;
		return 0;
	}
}

int cKeyframes::GetFrameIndexForKeyframe(int keyframeIndex) const
{
	if (keyframeIndex < keyframesIndexesTable.size())
	{
		return framesIndexesTable.at(keyframeIndex);
	}
	else
	{
		qCritical() << "GetFrameIndexForKeyframe(int keyframeIndex): wrong keyframe index: "
								<< keyframeIndex;
		return 0;
	}
}

int cKeyframes::GetSubIndex(int frameIndex) const
{
	int keyFrameIndex = GetKeyframeIndex(frameIndex);
	int frameIndexForKeyframe = framesIndexesTable.at(keyFrameIndex);
	return frameIndex - frameIndexForKeyframe;
}
