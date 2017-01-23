/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * This class collects all audio tracks (cAudioTrack objects)
 */

#include "audio_track_collection.h"
#include "audio_track.h"
#include "one_parameter.hpp"
#include "parameters.hpp"

cAudioTrackCollection::cAudioTrackCollection()
{
}

cAudioTrackCollection::~cAudioTrackCollection()
{
	qDeleteAll(audioTracks);
}

/* Warning! this is fake constructor to avoid copying audio data to cUndo buffers */
cAudioTrackCollection::cAudioTrackCollection(const cAudioTrackCollection &collection)
{
	Q_UNUSED(collection);
	this->audioTracks.clear();
}

/* Warning! this is fake operator to avoid copying audio data to cUndo buffers */
cAudioTrackCollection &cAudioTrackCollection::operator=(const cAudioTrackCollection &collection)
{
	Q_UNUSED(collection);
	qDeleteAll(audioTracks);
	this->audioTracks.clear();
	return *this;
}

void cAudioTrackCollection::AddAudioTrack(
	const QString fullParameterName, cParameterContainer *params)
{
	if (audioTracks.contains(fullParameterName))
	{
		qCritical() << "cAudioTrackCollection::AddAudioTrack(): element '" << fullParameterName
								<< "' already existed";
	}
	else
	{
		audioTracks.insert(fullParameterName, new cAudioTrack());
		if (params) // params is nullptr when audio tracks are regenerated
		{
			AddParameters(params, fullParameterName);
		}
	}
}

void cAudioTrackCollection::DeleteAudioTrack(
	const QString fullParameterName, cParameterContainer *params)
{
	if (audioTracks.contains(fullParameterName))
	{
		delete audioTracks[fullParameterName];
		audioTracks.remove(fullParameterName);
		RemoveParameters(params, fullParameterName);
	}
	else
	{
		qCritical() << "cAudioTrackCollection::DeleteAudioTrack(): element '" << fullParameterName
								<< "' doesn't exist";
	}
}

void cAudioTrackCollection::DeleteAllAudioTracks(cParameterContainer *params)
{
	QStringList listOfAllParameters = audioTracks.keys();

	for (int i = 0; i < listOfAllParameters.length(); i++)
	{
		DeleteAudioTrack(listOfAllParameters.at(i), params);
	}
}

cAudioTrack *cAudioTrackCollection::GetAudioTrackPtr(const QString fullParameterName) const
{
	if (audioTracks.contains(fullParameterName))
	{
		return audioTracks[fullParameterName];
	}
	else
	{
		qCritical() << "cAudioTrackCollection::GetAudioTrackPtr(): element '" << fullParameterName
								<< "' doesn't exist";
		return nullptr;
	}
}

void cAudioTrackCollection::AddParameters(cParameterContainer *params, const QString parameterName)
{
	if (!params->IfExists(FullParameterName("enable", parameterName)))
	{
		using namespace parameterContainer;
		params->addParam(
			FullParameterName("midfreq", parameterName), 1000.0, 5.0, 20000.0, morphNone, paramStandard);
		params->addParam(
			FullParameterName("bandwidth", parameterName), 200.0, 5.0, 20000.0, morphNone, paramStandard);
		params->addParam(FullParameterName("additionfactor", parameterName), 1.0, 0.0, 20000.0,
			morphNone, paramStandard);
		params->addParam(
			FullParameterName("multfactor", parameterName), 1.0, 0.0, 20000.0, morphNone, paramStandard);

		params->addParam(FullParameterName("negative", parameterName), false, morphNone, paramStandard);
		params->addParam(
			FullParameterName("pitchmode", parameterName), false, morphNone, paramStandard);
		params->addParam(
			FullParameterName("decayfilter", parameterName), false, morphNone, paramStandard);
		params->addParam(
			FullParameterName("smoothfilter", parameterName), false, morphNone, paramStandard);
		params->addParam(
			FullParameterName("binaryfilter", parameterName), false, morphNone, paramStandard);
		params->addParam(FullParameterName("decaystrength", parameterName), 5.0, 0.01, 1000.0,
			morphNone, paramStandard);
		params->addParam(FullParameterName("smoothstrength", parameterName), 5.0, 0.01, 1000.0,
			morphNone, paramStandard);
		params->addParam(
			FullParameterName("binarythresh", parameterName), 0.5, 0.0, 1.0, morphNone, paramStandard);
		params->addParam(
			FullParameterName("binarylength", parameterName), 1, 1, 1000, morphNone, paramStandard);

		params->addParam(FullParameterName("enable", parameterName), false, morphNone, paramStandard);
		params->addParam(
			FullParameterName("soundfile", parameterName), QString(""), morphNone, paramStandard);
	}
}

void cAudioTrackCollection::RemoveParameters(
	cParameterContainer *params, const QString parameterName)
{
	if (params->IfExists(FullParameterName("enable", parameterName)))
	{
		params->DeleteParameter(FullParameterName("bandwidth", parameterName));
		params->DeleteParameter(FullParameterName("midfreq", parameterName));
		params->DeleteParameter(FullParameterName("additionfactor", parameterName));
		params->DeleteParameter(FullParameterName("multfactor", parameterName));
		params->DeleteParameter(FullParameterName("enable", parameterName));
		params->DeleteParameter(FullParameterName("soundfile", parameterName));
		params->DeleteParameter(FullParameterName("negative", parameterName));
		params->DeleteParameter(FullParameterName("pitchmode", parameterName));
		params->DeleteParameter(FullParameterName("decayfilter", parameterName));
		params->DeleteParameter(FullParameterName("smoothfilter", parameterName));
		params->DeleteParameter(FullParameterName("decaystrength", parameterName));
		params->DeleteParameter(FullParameterName("smoothstrength", parameterName));
		params->DeleteParameter(FullParameterName("binaryfilter", parameterName));
		params->DeleteParameter(FullParameterName("binarythresh", parameterName));
		params->DeleteParameter(FullParameterName("binarylength", parameterName));
	}
}

QString cAudioTrackCollection::FullParameterName(
	const QString &nameOfSoundParameter, const QString parameterName)
{
	return prefix + QString("_") + nameOfSoundParameter + "_" + parameterName;
}

void cAudioTrackCollection::LoadAllAudioFiles(cParameterContainer *params)
{
	QStringList listOfAllParameters = audioTracks.keys();

	for (int i = 0; i < listOfAllParameters.length(); i++)
	{
		QString filename =
			params->Get<QString>(FullParameterName("soundfile", listOfAllParameters.at(i)));
		if (!filename.isEmpty() && !audioTracks[listOfAllParameters[i]]->isLoaded())
		{
			audioTracks[listOfAllParameters[i]]->Clear();
			audioTracks[listOfAllParameters[i]]->LoadAudio(filename);
			audioTracks[listOfAllParameters[i]]->setFramesPerSecond(
				params->Get<double>("keyframe_frames_per_second"));
			audioTracks[listOfAllParameters[i]]->calculateFFT();
		}
	}
}

void cAudioTrackCollection::RefreshAllAudioTracks(cParameterContainer *params)
{
	QStringList listOfAllParameters = audioTracks.keys();

	for (int i = 0; i < listOfAllParameters.length(); i++)
	{
		QString parameterName = listOfAllParameters[i];
		if (audioTracks[parameterName]->isLoaded())
		{
			audioTracks[parameterName]->setFramesPerSecond(
				params->Get<double>("keyframe_frames_per_second"));
			audioTracks[parameterName]->calculateFFT();

			double midFreq = params->Get<double>(FullParameterName("midfreq", parameterName));
			double bandwidth = params->Get<double>(FullParameterName("bandwidth", parameterName));
			bool pitchmode = params->Get<bool>(FullParameterName("pitchmode", parameterName));
			audioTracks[parameterName]->calculateAnimation(midFreq, bandwidth, pitchmode);

			if (params->Get<bool>(FullParameterName("binaryfilter", parameterName)))
			{
				audioTracks[parameterName]->binaryFilter(
					params->Get<double>(FullParameterName("binarythresh", parameterName)),
					params->Get<int>(FullParameterName("binarylength", parameterName)));
			}
			if (params->Get<bool>(FullParameterName("decayfilter", parameterName)))
			{
				audioTracks[parameterName]->decayFilter(
					params->Get<double>(FullParameterName("decaystrength", parameterName)));
			}
			if (params->Get<bool>(FullParameterName("smoothfilter", parameterName)))
			{
				audioTracks[parameterName]->smoothFilter(
					params->Get<double>(FullParameterName("smoothstrength", parameterName)));
			}
		}
	}
}
