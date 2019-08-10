/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "audio_track.h"
#include "audio_track_collection.h"
#include "fractal_container.hpp"
#include "initparameters.hpp"
#include "system.hpp"

cAnimationFrames *gAnimFrames = nullptr;

cAnimationFrames::cAnimationFrames() = default;

cAnimationFrames::~cAnimationFrames() = default;

void cAnimationFrames::AddFrame(
	const cParameterContainer &params, const cFractalContainer &fractal, int index)
{
	sAnimationFrame frame;

	for (auto &parameterDescription : listOfParameters)
	{
		const cParameterContainer *container =
			ContainerSelector(parameterDescription.containerName, &params, &fractal);

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

void cAnimationFrames::AddAnimatedParameter(
	const QString &parameterName, const cOneParameter &defaultValue, cParameterContainer *params)
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
							 << parameterName << "' already exists" << endl;
	}
}

bool cAnimationFrames::AddAnimatedParameter(
	const QString &fullParameterName, cParameterContainer *param, const cFractalContainer *fractal)
{
	const int firstUnderscore = fullParameterName.indexOf('_');
	const QString containerName = fullParameterName.left(firstUnderscore);
	const QString parameterName = fullParameterName.mid(firstUnderscore + 1);

	const cParameterContainer *container = ContainerSelector(containerName, param, fractal);
	if (container)
	{
		cOneParameter parameter = container->GetAsOneParameter(parameterName);
		if (parameter.IsEmpty())
		{
			qWarning() << "cAnimationFrames::AddAnimatedParameter(const QString &fullParameterName, "
										"const cParameterContainer *param, const cFractalContainer *fractal): unknown "
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
									 "cParameterContainer *param, const cFractalContainer *fractal): Wrong container "
									 "name: "
								<< containerName;
		return false;
	}
}

void cAnimationFrames::RegenerateAudioTracks(cParameterContainer *param)
{
	for (auto &parameterDescription : listOfParameters)
	{
		AddAudioParameter(parameterDescription.parameterName, parameterDescription.varType,
			parameterDescription.containerName, param);
	}

	audioTracks.LoadAllAudioFiles(param);
}

void cAnimationFrames::RefreshAllAudioTracks(cParameterContainer *param)
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
	int index = -1;
	for (int i = 0; i < listOfParameters.size(); ++i)
	{
		if (listOfParameters[i].parameterName == parameterName
				&& listOfParameters[i].containerName == containerName)
		{
			index = i;
			break;
		}
	}
	return index;
}

const cParameterContainer *cAnimationFrames::ContainerSelector(
	QString containerName, const cParameterContainer *params, const cFractalContainer *fractal)
{
	const cParameterContainer *container = nullptr;
	if (containerName == "main")
	{
		container = params;
	}
	else if (containerName.indexOf("fractal") >= 0)
	{
		const int index = containerName.rightRef(1).toInt();
		if (index < 4)
		{
			container = &fractal->at(index);
		}
		else
		{
			qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, const "
										"cParameterContainer *params, const cFractalContainer *fractal): wrong fractal "
										"container index"
								 << containerName << index;
		}
	}
	else
	{
		qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, const "
									"cParameterContainer *params, const cFractalContainer *fractal): wrong container "
									"name"
							 << containerName;
	}

	return container;
}

cParameterContainer *cAnimationFrames::ContainerSelector(
	QString containerName, cParameterContainer *params, cFractalContainer *fractal)
{
	cParameterContainer *container = nullptr;
	if (containerName == "main" || containerName == "material")
	{
		container = params;
	}
	else if (containerName.indexOf("fractal") >= 0)
	{
		const int index = containerName.rightRef(1).toInt();
		if (index < 4)
		{
			container = &fractal->at(index);
		}
		else
		{
			qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, "
										"cParameterContainer *params, cFractalContainer *fractal): wrong fractal "
										"container index"
								 << containerName << index;
		}
	}
	else
	{
		qWarning() << "cAnimationFrames::ContainerSelector(QString containerName, cParameterContainer "
									"*params, cFractalContainer *fractal): wrong container name"
							 << containerName;
	}

	return container;
}

cAnimationFrames::sAnimationFrame cAnimationFrames::GetFrame(int index) const
{
	if (index >= 0 && index < frames.count())
	{
		return frames.at(index);
	}
	else
	{
		qWarning() << "cAnimationFrames::GetFrame(int index): wrong index" << index;
		return sAnimationFrame();
	}
}

void cAnimationFrames::GetFrameAndConsolidate(
	int index, cParameterContainer *params, cFractalContainer *fractal)
{
	if (index >= 0 && index < frames.count())
	{
		cParameterContainer frame = frames.at(index).parameters;

		for (auto &listOfParameter : listOfParameters)
		{
			cParameterContainer *container =
				ContainerSelector(listOfParameter.containerName, params, fractal);
			const QString parameterName = listOfParameter.parameterName;
			const cOneParameter oneParameter =
				frame.GetAsOneParameter(listOfParameter.containerName + "_" + parameterName);
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

	for (auto &frame : frames)
	{
		frame.parameters.DeleteParameter(fullParameterName);
	}

	for (int i = 0; i < listOfParameters.size(); ++i)
	{
		if (listOfParameters[i].containerName + "_" + listOfParameters[i].parameterName
				== fullParameterName)
		{
			RemoveAudioParameter(listOfParameters[i]);
			listOfParameters.removeAt(i);
			break;
		}
	}
}

void cAnimationFrames::DeleteFrames(int begin, int end)
{
	for (int i = end; i >= begin; i--)
	{
		frames.removeAt(i);
	}
}

void cAnimationFrames::ModifyFrame(int index, sAnimationFrame &frame)
{
	if (index >= 0 && index < frames.size())
	{
		frames[index] = frame;
	}
}

void cAnimationFrames::AddFrame(const sAnimationFrame &frame)
{
	frames.append(frame);
}

void cAnimationFrames::AddAudioParameter(const QString &parameterName, enumVarType paramType,
	const QString originalContainerName, cParameterContainer *params)
{

	setAudioParameterPrefix();
	const QString fullParameterName = originalContainerName + "_" + parameterName;

	switch (paramType)
	{
		case typeVector3:
			audioTracks.AddAudioTrack(fullParameterName + "_x", params);
			audioTracks.AddAudioTrack(fullParameterName + "_y", params);
			audioTracks.AddAudioTrack(fullParameterName + "_z", params);
			break;
		case typeVector4:
			audioTracks.AddAudioTrack(fullParameterName + "_x", params);
			audioTracks.AddAudioTrack(fullParameterName + "_y", params);
			audioTracks.AddAudioTrack(fullParameterName + "_z", params);
			audioTracks.AddAudioTrack(fullParameterName + "_w", params);
			break;
		case typeRgb:
			audioTracks.AddAudioTrack(fullParameterName + "_R", params);
			audioTracks.AddAudioTrack(fullParameterName + "_G", params);
			audioTracks.AddAudioTrack(fullParameterName + "_B", params);
			break;
		default: audioTracks.AddAudioTrack(fullParameterName, params); break;
	}
}

void cAnimationFrames::RemoveAudioParameter(
	const sParameterDescription &parameter, cParameterContainer *params)
{
	if (!params) params = gPar;
	const QString fullParameterName = parameter.containerName + "_" + parameter.parameterName;
	const enumVarType paramType = parameter.varType;

	switch (paramType)
	{
		case typeVector3:
			audioTracks.DeleteAudioTrack(fullParameterName + "_x", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_y", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_z", params);
			break;
		case typeVector4:
			audioTracks.DeleteAudioTrack(fullParameterName + "_x", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_y", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_z", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_w", params);
			break;
		case typeRgb:
			audioTracks.DeleteAudioTrack(fullParameterName + "_R", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_G", params);
			audioTracks.DeleteAudioTrack(fullParameterName + "_B", params);
			break;
		default: audioTracks.DeleteAudioTrack(fullParameterName, params); break;
	}
}

QSharedPointer<cAudioTrack> cAnimationFrames::GetAudioPtr(const QString fullParameterName) const
{
	return audioTracks.GetAudioTrackPtr(fullParameterName);
}

cOneParameter cAnimationFrames::ApplyAudioAnimation(int frame, const cOneParameter &parameter,
	const QString &parameterName, const cParameterContainer *params) const
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
	const QString &fullParameterNameWithSuffix, const cParameterContainer *params) const
{
	T newVal = oldVal;
	const bool isEnabled =
		params->Get<bool>(QString("animsound_enable_%1").arg(fullParameterNameWithSuffix));
	if (isEnabled)
	{
		const double additionFactor =
			params->Get<double>(QString("animsound_additionfactor_%1").arg(fullParameterNameWithSuffix));
		const double multFactor =
			params->Get<double>(QString("animsound_multfactor_%1").arg(fullParameterNameWithSuffix));
		const double animSound =
			double(audioTracks.GetAudioTrackPtr(fullParameterNameWithSuffix)->getAnimation(frame));

		if (params->Get<bool>(QString("animsound_negative_%1").arg(fullParameterNameWithSuffix)))
		{
			newVal = oldVal / (1.0 + multFactor * animSound) - additionFactor * animSound;
		}
		else
		{
			newVal = oldVal * (1.0 + multFactor * animSound) + additionFactor * animSound;
		}
	}
	return newVal;
}

template int cAnimationFrames::ApplyAudioAnimationOneComponent(int frame, int oldVal,
	const QString &fullParameterNameWithSuffix, const cParameterContainer *params) const;
template double cAnimationFrames::ApplyAudioAnimationOneComponent(int frame, double oldVal,
	const QString &fullParameterNameWithSuffix, const cParameterContainer *params) const;

void cAnimationFrames::RemoveAllAudioParameters(cParameterContainer *params)
{
	if (!params) params = gPar;
	audioTracks.DeleteAllAudioTracks(params);
}

void cAnimationFrames::LoadAllAudioFiles(cParameterContainer *params)
{
	if (!params) params = gPar;
	audioTracks.LoadAllAudioFiles(params);
}

void cAnimationFrames::setAudioParameterPrefix()
{
	audioTracks.SetPrefix("flightanimsound");
}

void cAnimationFrames::SetListOfParametersAndClear(
	QList<sParameterDescription> _listOfParameters, cParameterContainer *params)
{
	listOfParameters = _listOfParameters;
	frames.clear();
	audioTracks.DeleteAllAudioTracks(params);
	RegenerateAudioTracks(params);
}

void cAnimationFrames::WipeFramesFromFolder(QString folder)
{
	// frames start with the string "frame_" followed by a number sequence
	// then an optional suffix (e.g. "_alpha") followed by the image file extension
	QString regex = "^frame_[0-9]+(?:_[a-z]+)?\\..+$";
	DeleteAllFilesFromDirectory(folder, regex, QRegExp::RegExp);
	DeleteAllFilesFromDirectory(
		folder + QDir::separator() + gPar->Get<QString>("alpha_postfix"), regex, QRegExp::RegExp);
	DeleteAllFilesFromDirectory(
		folder + QDir::separator() + gPar->Get<QString>("zbuffer_postfix"), regex, QRegExp::RegExp);
	DeleteAllFilesFromDirectory(
		folder + QDir::separator() + gPar->Get<QString>("normal_postfix"), regex, QRegExp::RegExp);
	DeleteAllFilesFromDirectory(
		folder + QDir::separator() + gPar->Get<QString>("specular_postfix"), regex, QRegExp::RegExp);
}
