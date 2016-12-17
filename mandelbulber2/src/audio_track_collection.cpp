/*
 * audio_track_collection.cpp
 *
 *  Created on: 16 gru 2016
 *      Author: krzysztof
 */

#include "audio_track_collection.h"
#include "audio_track.h"

cAudioTrackCollection::cAudioTrackCollection(QObject *parent) : QObject(parent)
{
}

cAudioTrackCollection::~cAudioTrackCollection()
{
	qDeleteAll(audioTracks);
}

void cAudioTrackCollection::AddAudioTrack(const QString fullParameterName)
{
	if (audioTracks.contains(fullParameterName))
	{
		qCritical() << "cAudioTrackCollection::AddAudioTrack(): element '" << fullParameterName
								<< "' already existed";
	}
	else
	{
		audioTracks.insert(fullParameterName, new cAudioTrack(this));
	}
}

void cAudioTrackCollection::DeleteAudioTrack(const QString fullParameterName)
{
	if (audioTracks.contains(fullParameterName))
	{
		audioTracks.remove(fullParameterName);
	}
	else
	{
		qCritical() << "cAudioTrackCollection::DeleteAudioTrack(): element '" << fullParameterName
								<< "' doesn't exist";
	}
}

cAudioTrack *cAudioTrackCollection::GetAudioTrackPtr(const QString fullParameterName)
{
	if (audioTracks.contains(fullParameterName))
	{
		return audioTracks[fullParameterName];
	}
	else
	{
		qCritical() << "cAudioTrackCollection::GetAudioTrackPtr(): element '" << fullParameterName
								<< "' doesn't exist";
		abort();
		return NULL;
	}
}
