/*
 * audio_track.cpp
 *
 *  Created on: 1 lis 2016
 *      Author: krzysztof marczak
 */

#include "audio_track.h"
#include <QAudioRecorder>
#include <QAudioFormat>
#include <QAudioDecoder>

cAudioTrack::cAudioTrack(QObject *parent) : QObject(parent)
{
	decoder = NULL;
	memoryReserved = false;
	length = 0;
}

cAudioTrack::~cAudioTrack()
{
	// TODO Auto-generated destructor stub
}

void cAudioTrack::LoadAudio(const QString &filename)
{
	QAudioFormat desiredFormat;
	desiredFormat.setChannelCount(1);
	desiredFormat.setCodec("audio/x-raw");
	desiredFormat.setSampleType(QAudioFormat::SignedInt);
	desiredFormat.setSampleRate(sampleRate);
	desiredFormat.setSampleSize(16);

	decoder = new QAudioDecoder(this);
	decoder->setAudioFormat(desiredFormat);
	decoder->setSourceFilename(filename);

	connect(decoder, SIGNAL(bufferReady()), this, SLOT(slotReadBuffer()));
	connect(decoder, SIGNAL(finished()), this, SLOT(slotFinished()));
	connect(decoder, SIGNAL(error(QAudioDecoder::Error)), this, SLOT(slotError(QAudioDecoder::Error)));

	decoder->start();
}

void cAudioTrack::slotReadBuffer()
{
	QAudioBuffer audioBuffer = decoder->read();

	qint64 duration = decoder->duration();
	qint64 totalSamplesApprox = (duration + 1000) * sampleRate / 1000;

	//reservation of memory if length is already known
	if(duration > 0 && !memoryReserved)
	{
		rawAudio.reserve(totalSamplesApprox);
		memoryReserved = true;
	}

	int frameCount = audioBuffer.frameCount();
	if(frameCount > 0)
	{
		qint16 *frames = audioBuffer.data<qint16>();

		for(int i = 0; i < frameCount; i++)
		{
			rawAudio.append(frames[i] / 32768.0);
		}
	}
	length = rawAudio.size();
	double percent = (double) length / totalSamplesApprox * 100.0;
	qDebug() << percent << "%" << length << (double)length / sampleRate;
}

void cAudioTrack::slotFinished()
{
	qDebug() << "finished";
	qDebug() << length << (double)length / sampleRate;
}

void cAudioTrack::slotError(QAudioDecoder::Error error)
{
	qDebug() << "error" << error;
}
