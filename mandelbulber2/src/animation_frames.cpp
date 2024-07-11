/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Class to store settings for animation frames
 *
 * Handles the 2D matrix of the list of parameters / list of frames
 * and exposes functions to modify this matrix.
 */

#include "animation_frames.hpp"

#include <memory>

#include <QDir>
#include <QDirIterator>

#include "audio_track.h"
#include "audio_track_collection.h"
#include "fractal_container.hpp"
#include "initparameters.hpp"
#include "system.hpp"

std::shared_ptr<cAnimationFrames> gAnimFrames;

cAnimationFrames::cAnimationFrames() = default;

cAnimationFrames::~cAnimationFrames() = default;

void cAnimationFrames::AddFrame(const std::shared_ptr<cParameterContainer> params,
	const std::shared_ptr<cFractalContainer> fractal, int numberOfSubFrames, int index)
{
	sAnimationFrame frame;

	for (auto &parameterDescription : listOfParameters)
	{
		std::shared_ptr<cParameterContainer> container =
			ContainerSelector(parameterDescription.containerName, params, fractal);

		if (container)
		{
			const QString parameterName = parameterDescription.parameterName;
			const QString fullParameterName = container->GetContainerName() + "_" + parameterName;
			cOneParameter oneParameter = container->GetAsOneParameter(parameterName);

			// getting morph type from existing frame
			parameterContainer::enumMorphType morphType;
			if (frames.size() > 0)
			{
				morphType = frames[0].parameters.GetAsOneParameter(fullParameterName).GetMorphType();
			}
			else // if no frames yet
			{
				morphType = oneParameter.GetMorphType();
			}

			oneParameter.SetMorphType(morphType);
			frame.parameters.AddParamFromOneParameter(fullParameterName, oneParameter);
			frame.numberOfSubFrames = numberOfSubFrames;
		}
		else
		{
			qCritical() << "cAnimationFrames::AddFrame(const cParameterContainer &params, const "
										 "cFractalContainer &fractal): Wrong container name: "
									<< parameterDescription.containerName;
		}
	}
	int indexTemp = index;
	if (index == -1) indexTemp = frames.size();
	frames.insert(indexTemp, frame);
}

void cAnimationFrames::AddAnimatedParameter(const QString &parameterName,
	const cOneParameter &defaultValue, std::shared_ptr<cParameterContainer> params)
{
	if (IndexOnList(parameterName, defaultValue.GetOriginalContainerName()) == -1)
	{
		listOfParameters.append(
			sParameterDescription(parameterName, defaultValue.GetOriginalContainerName(),
				defaultValue.GetValueType(), defaultValue.GetMorphType()));
		for (auto &frame : frames)
		{
			frame.parameters.AddParamFromOneParameter(
				defaultValue.GetOriginalContainerName() + "_" + parameterName, defaultValue);
		}

		// if parameter container is nullptr then will be used default global container for sound
		// parameters
		if (!params) params = gPar;

		AddAudioParameter(
			parameterName, defaultValue.GetValueType(), defaultValue.GetOriginalContainerName(), params);
	}
	else
	{
		qWarning() << "cAnimationFrames::AddAnimatedParameter(const QString &parameterName, const "
									"cOneParameter &defaultValue): element '"
							 << parameterName << "' already exists";
	}
}

bool cAnimationFrames::AddAnimatedParameter(const QString &fullParameterName,
	std::shared_ptr<cParameterContainer> param, std::shared_ptr<cFractalContainer> fractal)
{
	const int firstUnderscore = fullParameterName.indexOf('_');
	const QString containerName = fullParameterName.left(firstUnderscore);
	const QString parameterName = fullParameterName.mid(firstUnderscore + 1);

	const std::shared_ptr<cParameterContainer> container =
		ContainerSelector(containerName, param, fractal);
	if (container)
	{
		cOneParameter parameter = container->GetAsOneParameter(parameterName);
		if (parameter.IsEmpty())
		{
			qWarning() << "cAnimationFrames::AddAnimatedParameter(const QString &fullParameterName, "
										"const std::shared_ptr<cParameterContainer> param, const "
										"std::shared_ptr<cFractalContainer> fractal): unknown "
										"parameter"
								 << fullParameterName;
			return false;
		}
		else
		{
			AddAnimatedParameter(parameterName, container->GetAsOneParameter(parameterName), param);
			return true;
		}
	}
	else
	{
		qCritical() << "cAnimationFrames::AddAnimatedParameter(const QString &fullParameterName, const "
									 "std::shared_ptr<cParameterContainer> param, const "
									 "std::shared_ptr<cFractalContainer> fractal): Wrong container "
									 "name: "
								<< containerName;
		return false;
	}
}

void cAnimationFrames::RegenerateAudioTracks(std::shared_ptr<cParameterContainer> param)
{
	// Iterate through each parameter description specified in the list of parameters
	for (auto &parameterDescription : listOfParameters)
	{
		// Add an audio parameter based on the parameter description to the audio tracks.
		AddAudioParameter(parameterDescription.parameterName, parameterDescription.varType,
			parameterDescription.containerName, param);
	}

	// Load all audio files associated with the specified parameters into the audio tracks.
	audioTracks.LoadAllAudioFiles(param);
}

void cAnimationFrames::RefreshAllAudioTracks(std::shared_ptr<cParameterContainer> param)
{
	audioTracks.RefreshAllAudioTracks(param);
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
	// Initialize the index to -1, indicating the item hasn't been found yet
	int index = -1;

	// Iterate through the 'listOfParameters' (assuming this is a member variable)
	for (int i = 0; i < listOfParameters.size(); ++i)
	{
		// Check if both the parameterName and containerName match the current item
		if (listOfParameters[i].parameterName == parameterName
				&& listOfParameters[i].containerName == containerName)
		{
			// If a match is found, set the index to the current position
			index = i;

			// Exit the loop since we've found the item
			break;
		}
	}

	// Return the final index (either the position of the item or -1 if not found)
	return index;
}

std::shared_ptr<const cParameterContainer> cAnimationFrames::ContainerSelector(
	QString containerName, std::shared_ptr<const cParameterContainer> params,
	std::shared_ptr<const cFractalContainer> fractal)
{
	// Initialize a shared pointer to the selected container
	std::shared_ptr<const cParameterContainer> container;

	// Check if the container name is "main"
	if (containerName == "main")
	{
		// If so, set the container to the main parameters
		container = params;
	}
	// Check if the container name contains "fractal"
	else if (containerName.indexOf("fractal") >= 0)
	{
		// If so, extract the index from the end of the name
		const int index = containerName.right(1).toInt();

		// Check if the index is within the valid range
		if (index < NUMBER_OF_FRACTALS)
		{
			// If so, set the container to the fractal parameters at the specified index
			container = fractal->at(index);
		}
		else
		{
			// If not, output a warning about the wrong index
			qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, const "
										"std::shared_ptr<cParameterContainer> params, const "
										"std::shared_ptr<cFractalContainer> fractal): wrong fractal "
										"container index"
								 << containerName << index;
		}
	}
	else
	{
		// If the container name is neither "main" nor contains "fractal", output a warning
		qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, const "
									"std::shared_ptr<cParameterContainer> params, const "
									"std::shared_ptr<cFractalContainer> fractal): wrong container "
									"name"
							 << containerName;
	}

	// Return the shared pointer to the selected container (or nullptr if not found)
	return container;
}

// This function selects a parameter container based on the given container name.
std::shared_ptr<cParameterContainer> cAnimationFrames::ContainerSelector(QString containerName,
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractal)
{
	// Initialize a shared pointer to the selected container
	std::shared_ptr<cParameterContainer> container;

	// Check if the container name is "main" or "material"
	if (containerName == "main" || containerName == "material")
	{
		// If so, assign the main parameters to the container
		container = params;
	}
	// Check if the container name contains "fractal"
	else if (containerName.indexOf("fractal") >= 0)
	{
		// If so, extract the index (the last character of the name)
		const int index = containerName.right(1).toInt();

		// Ensure the index is valid (less than the total number of fractals)
		if (index < NUMBER_OF_FRACTALS)
		{
			// If valid, assign the corresponding fractal parameters to the container
			container = fractal->at(index);
		}
		else
		{
			// If not valid, issue a warning message with the invalid index
			qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, "
										"std::shared_ptr<cParameterContainer> params, "
										"std::shared_ptr<cFractalContainer> fractal): wrong fractal "
										"container index"
								 << containerName << index;
		}
	}
	else
	{
		// If the container name is not recognized, issue a warning message
		qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, cParameterContainer "
									"*params, std::shared_ptr<cFractalContainer> fractal): wrong container name"
							 << containerName;
	}

	// Return the selected container (which may be nullptr if no valid container was found)
	return container;
}

// Method within the cAnimationFrames class to retrieve a specific animation frame
cAnimationFrames::sAnimationFrame cAnimationFrames::GetFrame(int index) const
{
	// Check if the requested index is within valid bounds of the frames container
	if (index >= 0 && index < frames.count())
	{
		// If valid, return the animation frame at the specified index
		return frames.at(index);
	}
	else
	{
		// If invalid index, print a warning message indicating the issue
		qWarning() << "cAnimationFrames::GetFrame(int index): wrong index" << index;

		// Return a default-constructed sAnimationFrame object (likely an empty or invalid frame)
		return sAnimationFrame();
	}
}

void cAnimationFrames::GetFrameAndConsolidate(int index,
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractal)
{
	// Check if the requested frame index is within the valid range
	if (index >= 0 && index < frames.count())
	{
		// Retrieve the parameter container associated with the specified frame
		cParameterContainer frame = frames.at(index).parameters;

		// Iterate over a list of parameter specifications (listOfParameters)
		for (auto &listOfParameter : listOfParameters)
		{
			// Determine the appropriate container (params or fractal) based on the containerName
			std::shared_ptr<cParameterContainer> container =
				ContainerSelector(listOfParameter.containerName, params, fractal);

			// Construct the full parameter name
			const QString parameterName = listOfParameter.parameterName;

			// Get the parameter value from the frame's parameter container as cOneParameter
			const cOneParameter oneParameter =
				frame.GetAsOneParameter(listOfParameter.containerName + "_" + parameterName);

			// Set the parameter value in the selected container
			container->SetFromOneParameter(parameterName, oneParameter);
		}
	}
	else
	{
		// If the index is invalid, print a warning message
		qWarning() << "cAnimationFrames::GetFrame(int index): wrong index" << index;
	}
}

void cAnimationFrames::RemoveAnimatedParameter(const QString &fullParameterName)
{
	// Iterate over each frame in the animation
	for (auto &frame : frames)
	{
		// Delete the specified parameter from the frame's parameter container
		frame.parameters.DeleteParameter(fullParameterName);
	}

	// Iterate over the list of animated parameters
	for (int i = 0; i < listOfParameters.size(); ++i)
	{
		// Construct the full parameter name for comparison
		if (listOfParameters[i].containerName + "_" + listOfParameters[i].parameterName
				== fullParameterName)
		{
			// If found, remove any associated audio parameters
			RemoveAudioParameter(listOfParameters[i]);

			// Remove the parameter from the list
			listOfParameters.removeAt(i);

			// Exit the loop since the parameter has been found and removed
			break;
		}
	}
}

void cAnimationFrames::RemoveMissingParameters(
	std::shared_ptr<const cParameterContainer> params, std::shared_ptr<const cFractalContainer> fractal)
{
	// List to store parameters that are missing
	QList<sParameterDescription> listOfMissingParameters;

	// Iterate through each animated parameter
	for (const sParameterDescription &animatedParameter : listOfParameters)
	{
		// Get the container for this parameter (either from params or fractal)
		std::shared_ptr<const cParameterContainer> container =
			ContainerSelector(animatedParameter.containerName, params, fractal);

		// If the container exists...
		if (container)
		{
			const QString parameterName = animatedParameter.parameterName;

			// Check if the parameter itself exists within the container
			if (!container->IfExists(parameterName))
			{
				// If not, add it to the list of missing parameters
				listOfMissingParameters.append(animatedParameter);
			}
		}
	}

	// Now, remove all the parameters that were identified as missing
	for (const sParameterDescription &parameterToDelete : listOfMissingParameters)
	{
		// Create a full name for the parameter
		const QString fullParameterName =
			parameterToDelete.containerName + "_" + parameterToDelete.parameterName;
		RemoveAnimatedParameter(fullParameterName);
	}
}

void cAnimationFrames::DeleteFrames(int begin, int end)
{
	// Loop through the frames in reverse order, starting from the end index
	for (int i = end; i >= begin; i--)
	{
		// Remove the frame at the current index
		frames.removeAt(i);
	}
}

void cAnimationFrames::ModifyFrame(int index, sAnimationFrame &frame)
{
	// Check if the provided index is valid (within the range of existing frames)
	if (index >= 0 && index < frames.size())
	{
		// Replace the frame at the specified index with the new frame data
		frames[index] = frame;
	}
}

void cAnimationFrames::AddFrame(const sAnimationFrame &frame)
{
	frames.append(frame);
}

void cAnimationFrames::AddAudioParameter(const QString &parameterName, enumVarType paramType,
	const QString originalContainerName, std::shared_ptr<cParameterContainer> params)
{
	// Set the prefix for audio parameters to ensure unique names.
	setAudioParameterPrefix();

	// Construct the full parameter name by combining the original container name and parameter name.
	const QString fullParameterName = originalContainerName + "_" + parameterName;

	// Switch based on the parameter type to handle different types of vector and RGB parameters.
	switch (paramType)
	{
		// For vector3 parameters (e.g., 3D coordinates), add three audio tracks
		case typeVector3:
			audioTracks.AddAudioTrack(fullParameterName + "_x", params);
			audioTracks.AddAudioTrack(fullParameterName + "_y", params);
			audioTracks.AddAudioTrack(fullParameterName + "_z", params);
			break;

		// For vector4 parameters (e.g., homogeneous coordinates), add four audio tracks
		case typeVector4:
			audioTracks.AddAudioTrack(fullParameterName + "_x", params);
			audioTracks.AddAudioTrack(fullParameterName + "_y", params);
			audioTracks.AddAudioTrack(fullParameterName + "_z", params);
			audioTracks.AddAudioTrack(fullParameterName + "_w", params);
			break;

		// For RGB color parameters, add three audio tracks (one for each color channel).
		case typeRgb:
			audioTracks.AddAudioTrack(fullParameterName + "_R", params);
			audioTracks.AddAudioTrack(fullParameterName + "_G", params);
			audioTracks.AddAudioTrack(fullParameterName + "_B", params);
			break;

		// For all other parameter types, add a single audio track with the full parameter name.
		default: audioTracks.AddAudioTrack(fullParameterName, params); break;
	}
}

void cAnimationFrames::RemoveAudioParameter(
	const sParameterDescription &parameter, std::shared_ptr<cParameterContainer> params)
{
	// If no parameter container is provided, use the global parameter container (gPar).
	if (!params) params = gPar;

	// Construct the full parameter name by combining the container name and parameter name.
	const QString fullParameterName = parameter.containerName + "_" + parameter.parameterName;

	// Get the parameter type.
	const enumVarType paramType = parameter.varType;

	// Switch based on the parameter type to handle different types of vector and RGB parameters.
	switch (paramType)
	{
		// For vector3 parameters (e.g., 3D coordinates), delete three audio tracks
		case typeVector3:
			audioTracks.DeleteAudioTrack(fullParameterName + "_x", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_y", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_z", params);
			break;

		// For vector4 parameters (e.g., homogeneous coordinates), delete four audio tracks
		case typeVector4:
			audioTracks.DeleteAudioTrack(fullParameterName + "_x", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_y", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_z", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_w", params);
			break;

		// For RGB color parameters, delete three audio tracks (one for each color channel).
		case typeRgb:
			audioTracks.DeleteAudioTrack(fullParameterName + "_R", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_G", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_B", params);
			break;

		// For all other parameter types, delete a single audio track with the full parameter name.
		default: audioTracks.DeleteAudioTrack(fullParameterName, params); break;
	}
}

std::shared_ptr<cAudioTrack> cAnimationFrames::GetAudioPtr(const QString fullParameterName) const
{
	return audioTracks.GetAudioTrackPtr(fullParameterName);
}

cOneParameter cAnimationFrames::ApplyAudioAnimation(int frame, const cOneParameter &parameter,
	const QString &parameterName, const std::shared_ptr<cParameterContainer> params) const
{
	cOneParameter newValue = parameter;
	const QString fullParameterName = parameter.GetOriginalContainerName() + "_" + parameterName;
	const enumVarType paramType = parameter.GetValueType();
	QString fullParameterNameWithSuffix;

	switch (paramType)
	{
		case typeInt:
		{
			int value = parameter.Get<int>(valueActual);
			fullParameterNameWithSuffix = fullParameterName;
			value = ApplyAudioAnimationOneComponent(frame, value, fullParameterNameWithSuffix, params);
			newValue.Set(value, valueActual);
			break;
		}
		case typeDouble:
		{
			double value = parameter.Get<double>(valueActual);
			fullParameterNameWithSuffix = fullParameterName;
			value = ApplyAudioAnimationOneComponent(frame, value, fullParameterNameWithSuffix, params);
			newValue.Set(value, valueActual);
			break;
		}
		case typeVector3:
		{
			CVector3 value = parameter.Get<CVector3>(valueActual);
			fullParameterNameWithSuffix = fullParameterName + "_x";
			value.x =
				ApplyAudioAnimationOneComponent(frame, value.x, fullParameterNameWithSuffix, params);
			fullParameterNameWithSuffix = fullParameterName + "_y";
			value.y =
				ApplyAudioAnimationOneComponent(frame, value.y, fullParameterNameWithSuffix, params);
			fullParameterNameWithSuffix = fullParameterName + "_z";
			value.z =
				ApplyAudioAnimationOneComponent(frame, value.z, fullParameterNameWithSuffix, params);
			newValue.Set(value, valueActual);
			break;
		}
		case typeVector4:
		{
			CVector4 value = parameter.Get<CVector4>(valueActual);
			fullParameterNameWithSuffix = fullParameterName + "_x";
			value.x =
				ApplyAudioAnimationOneComponent(frame, value.x, fullParameterNameWithSuffix, params);
			fullParameterNameWithSuffix = fullParameterName + "_y";
			value.y =
				ApplyAudioAnimationOneComponent(frame, value.y, fullParameterNameWithSuffix, params);
			fullParameterNameWithSuffix = fullParameterName + "_z";
			value.z =
				ApplyAudioAnimationOneComponent(frame, value.z, fullParameterNameWithSuffix, params);
			fullParameterNameWithSuffix = fullParameterName + "_w";
			value.w =
				ApplyAudioAnimationOneComponent(frame, value.w, fullParameterNameWithSuffix, params);
			newValue.Set(value, valueActual);
			break;
		}
		case typeRgb:
		{
			sRGB value = parameter.Get<sRGB>(valueActual);
			fullParameterNameWithSuffix = fullParameterName + "_R";
			value.R =
				ApplyAudioAnimationOneComponent(frame, value.R, fullParameterNameWithSuffix, params);
			fullParameterNameWithSuffix = fullParameterName + "_G";
			value.G =
				ApplyAudioAnimationOneComponent(frame, value.G, fullParameterNameWithSuffix, params);
			fullParameterNameWithSuffix = fullParameterName + "_B";
			value.B =
				ApplyAudioAnimationOneComponent(frame, value.B, fullParameterNameWithSuffix, params);
			newValue.Set(value, valueActual);
			break;
		}
		default:
		{
			// not possible to use animation by audio
			break;
		}
	}

	return newValue;
}

template <typename T>
T cAnimationFrames::ApplyAudioAnimationOneComponent(int frame, T oldVal,
	const QString &fullParameterNameWithSuffix,
	const std::shared_ptr<cParameterContainer> params) const
{
	T newVal = oldVal;

	// Check if audio animation is enabled for this parameter
	const bool isEnabled =
		params->Get<bool>(QString("animsound_enable_%1").arg(fullParameterNameWithSuffix));
	if (isEnabled)
	{
		// Retrieve parameters for animation calculation
		const double additionFactor =
			params->Get<double>(QString("animsound_additionfactor_%1").arg(fullParameterNameWithSuffix));
		const double multFactor =
			params->Get<double>(QString("animsound_multfactor_%1").arg(fullParameterNameWithSuffix));

		// Get audio track and sound delay for the parameter
		int soundDelay = audioTracks.GetAudioTrackPtr(fullParameterNameWithSuffix)->getSoundDelay();

		int soundFrame = frame - soundDelay;

		// Check if sound frame is within valid range for the audio track
		if (soundFrame >= 0
				&& soundFrame
						 < audioTracks.GetAudioTrackPtr(fullParameterNameWithSuffix)->getNumberOfFrames())
		{
			// Get animation value from the audio track for the current sound frame
			const double animSound =
				double(audioTracks.GetAudioTrackPtr(fullParameterNameWithSuffix)->getAnimation(soundFrame));

			// Apply animation based on 'animsound_negative' parameter
			if (params->Get<bool>(QString("animsound_negative_%1").arg(fullParameterNameWithSuffix)))
			{
				// Negative animation formula
				newVal = oldVal / (1.0 + multFactor * animSound) - additionFactor * animSound;
			}
			else
			{
				// Positive animation formula
				newVal = oldVal * (1.0 + multFactor * animSound) + additionFactor * animSound;
			}
		}
	}

	// Return the new calculated value after applying (or not) the audio animation
	return newVal;
}

template int cAnimationFrames::ApplyAudioAnimationOneComponent(int frame, int oldVal,
	const QString &fullParameterNameWithSuffix,
	const std::shared_ptr<cParameterContainer> params) const;
template double cAnimationFrames::ApplyAudioAnimationOneComponent(int frame, double oldVal,
	const QString &fullParameterNameWithSuffix,
	const std::shared_ptr<cParameterContainer> params) const;

void cAnimationFrames::RemoveAllAudioParameters(std::shared_ptr<cParameterContainer> params)
{
	if (!params) params = gPar;
	audioTracks.DeleteAllAudioTracks(params);
}

void cAnimationFrames::LoadAllAudioFiles(std::shared_ptr<cParameterContainer> params)
{
	if (!params) params = gPar;
	audioTracks.LoadAllAudioFiles(params);
}

void cAnimationFrames::setAudioParameterPrefix()
{
	audioTracks.SetPrefix("flightanimsound");
}

void cAnimationFrames::SetListOfParametersAndClear(
	QList<sParameterDescription> _listOfParameters, std::shared_ptr<cParameterContainer> params)
{
	// Update the internal list of parameters with the new list provided.
	listOfParameters = _listOfParameters;

	// Clear existing animation frames data, as the parameters have changed.
	frames.clear();

	// Delete all existing audio tracks associated with the previous parameters.
	audioTracks.DeleteAllAudioTracks(params);

	// Regenerate audio tracks based on the new list of parameters.
	RegenerateAudioTracks(params);
}

void cAnimationFrames::WipeFramesFromFolder(QString folder)
{
	// Define a regular expression to identify frame files.
	QString regex = "^frame_[0-9]+(?:_[a-z]+)?\\..+$";

	// Delete all files matching the regex from the main folder.
	DeleteAllFilesFromDirectory(folder, regex, false);

	// Delete files from subfolders for additional image channels (alpha, z-buffer, normal, specular).
	DeleteAllFilesFromDirectory(
		folder + QDir::separator() + gPar->Get<QString>("alpha_postfix"), regex, false);
	DeleteAllFilesFromDirectory(
		folder + QDir::separator() + gPar->Get<QString>("zbuffer_postfix"), regex, false);
	DeleteAllFilesFromDirectory(
		folder + QDir::separator() + gPar->Get<QString>("normal_postfix"), regex, false);
	DeleteAllFilesFromDirectory(
		folder + QDir::separator() + gPar->Get<QString>("specular_postfix"), regex, false);
}

void cAnimationFrames::WipeFramesFromFolder(QString folder, int start, int end)
{
	// Iterate through frame numbers in the specified range
	for (int i = start; i <= end; i++)
	{
		// Construct a regular expression to match frame filenames
		QString regex = QString("^frame_%1+(?:_[a-z]+)?\\..+$").arg(i, 7, 10, QChar('0'));

		// Delete files matching the pattern in the main folder
		DeleteAllFilesFromDirectory(folder, regex, false);

		// Delete files in potential subfolders (for different image types)
		DeleteAllFilesFromDirectory(
			folder + QDir::separator() + gPar->Get<QString>("alpha_postfix"), regex, false);
		DeleteAllFilesFromDirectory(
			folder + QDir::separator() + gPar->Get<QString>("zbuffer_postfix"), regex, false);
		DeleteAllFilesFromDirectory(
			folder + QDir::separator() + gPar->Get<QString>("normal_postfix"), regex, false);
		DeleteAllFilesFromDirectory(
			folder + QDir::separator() + gPar->Get<QString>("specular_postfix"), regex, false);
	}
}
