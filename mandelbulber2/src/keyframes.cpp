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
		// Append a new morph object if the size of morph is less than or equal to morphTableItemIndex
		morph.append(new cMorph());

		//it cannot be looped if number of frames is less than two.
		if(frames.size() < 3)
		{
			looped = false;
		}

		// Create morph cache (pre-indexed - counting of number of subframes)
		int numberOfSubFrames = 0;
		// Determine the first keyframe index (index for cAnimationFrames) based on whether the
		// animation is looped
		int firstFrameIndex = looped ? frames.size() - 2 : 0;
		// Initialize keyframe index for looped or non-looped animation
		int keyframeIndex = (looped) ? -2 : 0;
		// Store the previous keyframe index
		int previousKeyframeIndex = keyframeIndex;

		// Get the parameter of the first frame
		cOneParameter previousOnePar =
			frames.at(firstFrameIndex).parameters.GetAsOneParameter(fullParameterName);
		// Store the previous frame index
		int previousK = firstFrameIndex;
		// Add the number of subframes of the first frame
		numberOfSubFrames += frames.at(firstFrameIndex).numberOfSubFrames;

		// Determine the start and end frames for the loop (for cAnimationFrames)
		int startFrame = looped ? frames.size() - 1 : 1;
		int endFrame = looped ? frames.size() + frames.size() + 2 : frames.size();
		// Increment keyframe index (first keyframe is already stored a s a previous one)
		keyframeIndex++;

		// Loop through the frames to build the morph cache
		for (int k = startFrame; k < endFrame; k++)
		{
			// Calculate the modulo of the current frame index
			int kMod = k % frames.size();
			// Get the parameter of the current frame
			cOneParameter onePar = frames.at(kMod).parameters.GetAsOneParameter(fullParameterName);
			// If the parameter is not empty, add data to the morph object
			if (!onePar.IsEmpty())
			{
				// add parameter from previous keyframe. This trick is needed for correct calculation of
				// number of subframes
				morph[morphTableItemIndex]->AddData(
					previousKeyframeIndex, numberOfSubFrames, previousOnePar);
				// Update the previous parameter and frame index
				previousOnePar = onePar;
				previousK = kMod;
				previousKeyframeIndex = keyframeIndex;
				// Reset the number of subframes
				numberOfSubFrames = 0;
			}
			// Add the number of subframes of the current frame
			numberOfSubFrames += frames.at(kMod).numberOfSubFrames;
			// Increment keyframe index
			keyframeIndex++;
		}

		// Handle the looped case
		if (looped)
		{
			// Calculate the frame index offset for the loop
			frameIndexOffsetForLoop = frames.at(frames.size() - 2).numberOfSubFrames
																+ frames.at(frames.size() - 1).numberOfSubFrames;

			// Add data for the last frame in the loop from frame #1
			morph[morphTableItemIndex]->AddData(frames.size() + 1, frames.at(1).numberOfSubFrames,
				frames.at(1).parameters.GetAsOneParameter(fullParameterName));
		}
		else
		{
			// Set the frame index offset for non-looped animation to 0
			frameIndexOffsetForLoop = 0;
			// Add data for the last frame
			morph[morphTableItemIndex]->AddData(frames.size() - 1, 1,
				frames.at(frames.size() - 1).parameters.GetAsOneParameter(fullParameterName));
		}
	}

	// Interpolate each parameter
	cOneParameter oneParameter =
		morph[morphTableItemIndex]->Interpolate(frameIndex + frameIndexOffsetForLoop);
	// Apply audio animation to the parameter
	oneParameter = ApplyAudioAnimation(frameIndex, oneParameter, parameterName, params);
	// Return the interpolated parameter
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
