/*
 * audio.h
 *
 *  Created on: 1 lis 2016
 *      Author: krzysztof marczak
 */

#ifndef MANDELBULBER2_SRC_AUDIO_TRACK_H_
#define MANDELBULBER2_SRC_AUDIO_TRACK_H_

#include <QObject>
#include <QAudioDecoder>

class cAudioTrack : public QObject
{
	Q_OBJECT

public:
	cAudioTrack(QObject *parent);
	~cAudioTrack();

	void LoadAudio(const QString &filename);

	private slots:
	void slotReadBuffer();
	void slotFinished();
	void slotError(QAudioDecoder::Error error);

	private:
	QAudioDecoder *decoder;
	QVector<float> rawAudio;
	bool memoryReserved;
	int length;

	static const int sampleRate = 44100;
};


#endif /* MANDELBULBER2_SRC_AUDIO_TRACK_H_ */
