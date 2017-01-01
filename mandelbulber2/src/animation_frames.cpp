/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
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

#include "audio_track_collection.h"
#include "fractal_container.hpp"
#include "audio_track.h"
#include "initparameters.hpp"

cAnimationFrames *gAnimFrames = NULL;

cAnimationFrames::cAnimationFrames()
{
}

cAnimationFrames::~cAnimationFrames()
{
}

void cAnimationFrames::AddFrame(
	const cParameterContainer &params, const cFractalContainer &fractal, int index)
{
	sAnimationFrame frame;
	frame.alreadyRendered = false;
	for (int i = 0; i < listOfParameters.size(); ++i)
	{
		const cParameterContainer *container =
			ContainerSelector(listOfParameters[i].containerName, &params, &fractal);
		if (container)
		{
			QString parameterName = listOfParameters[i].parameterName;
			frame.parameters.AddParamFromOneParameter(container->GetContainerName() + "_" + parameterName,
				container->GetAsOneParameter(parameterName));
		}
		else
		{
			qCritical() << "cAnimationFrames::AddFrame(const cParameterContainer &params, const "
										 "cFractalContainer &fractal): Wrong container name: "
									<< listOfParameters[i].containerName;
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
		for (int i = 0; i < frames.size(); ++i)
		{
			frames[i].parameters.AddParamFromOneParameter(
				defaultValue.GetOriginalContainerName() + "_" + parameterName, defaultValue);
		}

		// if parameter container is NULL then will be used default global container for sound
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
	int firstUnderscore = fullParameterName.indexOf('_');
	QString containerName = fullParameterName.left(firstUnderscore);
	QString parameterName = fullParameterName.mid(firstUnderscore + 1);

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
	for (int i = 0; i < listOfParameters.length(); i++)
	{
		AddAudioParameter(listOfParameters[i].parameterName, listOfParameters[i].varType,
			listOfParameters[i].containerName);
	}

	audioTracks.LoadAllAudioFiles(param);
}

int cAnimationFrames::GetUnrenderedTotal()
{
	return GetUnrenderedTillIndex(frames.count() - 1);
}

int cAnimationFrames::GetUnrenderedTillIndex(int index)
{
	if (index >= 0 && index < frames.count())
	{
		int count = 0;
		for (int i = 0; i < index; i++)
		{
			if (!frames.at(i).alreadyRendered) count++;
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
	QString containerName, const cParameterContainer *params, const cFractalContainer *fractal) const
{
	const cParameterContainer *container = NULL;
	if (containerName == "main")
	{
		container = params;
	}
	else if (containerName.indexOf("fractal") >= 0)
	{
		int index = containerName.right(1).toInt();
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
	QString containerName, cParameterContainer *params, cFractalContainer *fractal) const
{
	cParameterContainer *container = NULL;
	if (containerName == "main" || containerName == "material")
	{
		container = params;
	}
	else if (containerName.indexOf("fractal") >= 0)
	{
		int index = containerName.right(1).toInt();
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

		for (int i = 0; i < listOfParameters.size(); ++i)
		{
			cParameterContainer *container =
				ContainerSelector(listOfParameters[i].containerName, params, fractal);
			QString parameterName = listOfParameters[i].parameterName;
			cOneParameter oneParameter =
				frame.GetAsOneParameter(listOfParameters[i].containerName + "_" + parameterName);
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

	for (int i = 0; i < frames.size(); ++i)
	{
		frames[i].parameters.DeleteParameter(fullParameterName);
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
	QString fullParameterName = originalContainerName + "_" + parameterName;

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
	QString fullParameterName = parameter.containerName + "_" + parameter.parameterName;
	enumVarType paramType = parameter.varType;

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

cAudioTrack *cAnimationFrames::GetAudioPtr(const QString fullParameterName)
{
	return audioTracks.GetAudioTrackPtr(fullParameterName);
}

cOneParameter cAnimationFrames::ApplyAudioAnimation(int frame, const cOneParameter &parameter,
	const QString &parameterName, const cParameterContainer *params) const
{
	cOneParameter newValue = parameter;
	QString fullParameterName = parameter.GetOriginalContainerName() + "_" + parameterName;
	enumVarType paramType = parameter.GetValueType();
	QString fullParameterNameWithSufix;

	switch (paramType)
	{
		case typeInt:
		{
			int value = parameter.Get<int>(valueActual);
			fullParameterNameWithSufix = fullParameterName;
			value = ApplyAudioAnimationOneComponent(frame, value, fullParameterNameWithSufix, params);
			newValue.Set(value, valueActual);
			break;
		}
		case typeDouble:
		{
			double value = parameter.Get<double>(valueActual);
			fullParameterNameWithSufix = fullParameterName;
			value = ApplyAudioAnimationOneComponent(frame, value, fullParameterNameWithSufix, params);
			newValue.Set(value, valueActual);
			break;
		}
		case typeVector3:
		{
			CVector3 value = parameter.Get<CVector3>(valueActual);
			fullParameterNameWithSufix = fullParameterName + "_x";
			value.x = ApplyAudioAnimationOneComponent(frame, value.x, fullParameterNameWithSufix, params);
			fullParameterNameWithSufix = fullParameterName + "_y";
			value.y = ApplyAudioAnimationOneComponent(frame, value.y, fullParameterNameWithSufix, params);
			fullParameterNameWithSufix = fullParameterName + "_z";
			value.z = ApplyAudioAnimationOneComponent(frame, value.z, fullParameterNameWithSufix, params);
			newValue.Set(value, valueActual);
			break;
		}
		case typeVector4:
		{
			CVector4 value = parameter.Get<CVector4>(valueActual);
			QString fullParameterNameWithSufix = fullParameterName + "_x";
			value.x = ApplyAudioAnimationOneComponent(frame, value.x, fullParameterNameWithSufix, params);
			fullParameterNameWithSufix = fullParameterName + "_y";
			value.y = ApplyAudioAnimationOneComponent(frame, value.y, fullParameterNameWithSufix, params);
			fullParameterNameWithSufix = fullParameterName + "_z";
			value.z = ApplyAudioAnimationOneComponent(frame, value.z, fullParameterNameWithSufix, params);
			fullParameterNameWithSufix = fullParameterName + "_w";
			value.w = ApplyAudioAnimationOneComponent(frame, value.w, fullParameterNameWithSufix, params);
			newValue.Set(value, valueActual);
			break;
		}
		case typeRgb:
		{
			sRGB value = parameter.Get<sRGB>(valueActual);
			fullParameterNameWithSufix = fullParameterName + "_R";
			value.R = ApplyAudioAnimationOneComponent(frame, value.R, fullParameterNameWithSufix, params);
			fullParameterNameWithSufix = fullParameterName + "_G";
			value.G = ApplyAudioAnimationOneComponent(frame, value.G, fullParameterNameWithSufix, params);
			fullParameterNameWithSufix = fullParameterName + "_B";
			value.B = ApplyAudioAnimationOneComponent(frame, value.B, fullParameterNameWithSufix, params);
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
	const QString &fullParameterNameWithSufix, const cParameterContainer *params) const
{
	T newVal = oldVal;
	bool isEnabled =
		params->Get<double>(QString("animsound_enable_%1").arg(fullParameterNameWithSufix));
	if (isEnabled)
	{
		double addiitionFactor =
			params->Get<double>(QString("animsound_additionfactor_%1").arg(fullParameterNameWithSufix));
		double multFactor =
			params->Get<double>(QString("animsound_multfactor_%1").arg(fullParameterNameWithSufix));
		float animSound = audioTracks.GetAudioTrackPtr(fullParameterNameWithSufix)->getAnimation(frame);
		newVal = oldVal * (1.0 + animSound * multFactor) + animSound * addiitionFactor;
	}
	return newVal;
}

template int cAnimationFrames::ApplyAudioAnimationOneComponent(int frame, int oldVal,
	const QString &fullParameterNameWithSufix, const cParameterContainer *params) const;
template double cAnimationFrames::ApplyAudioAnimationOneComponent(int frame, double oldVal,
	const QString &fullParameterNameWithSufix, const cParameterContainer *params) const;

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
