/*
 * audio_track_collection.h
 *
 *  Created on: 16 gru 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_AUDIO_TRACK_COLLECTION_H_
#define MANDELBULBER2_SRC_AUDIO_TRACK_COLLECTION_H_

#include <QtCore>

// forward declarations
class cAudioTrack;

class cAudioTrackCollection : public QObject
{
	Q_OBJECT
public:
	cAudioTrackCollection(QObject *parent = 0);
	~cAudioTrackCollection();
	void AddAudioTrack(const QString fullParameterName);
	void DeleteAudioTrack(const QString fullParameterName);
	cAudioTrack *GetAudioTrackPtr(const QString fullParameterName);

private:
	QMap<QString, cAudioTrack *> audioTracks;
};

#endif /* MANDELBULBER2_SRC_AUDIO_TRACK_COLLECTION_H_ */
