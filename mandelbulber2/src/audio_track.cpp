/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
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
 * cAudioTrack - Audio decoder class for animation
 */

#include "audio_track.h"
#include <QAudioRecorder>
#include <QAudioFormat>
#include <QAudioDecoder>
#include <QFileInfo>

#ifdef USE_SNDFILE
#include <sndfile.h>
#endif

cAudioTrack::cAudioTrack(QObject *parent) : QObject(parent)
{
	decoder = NULL;
	memoryReserved = false;
	length = 0;
	sampleRate = 44100;
	loaded = false;
}

cAudioTrack::~cAudioTrack()
{
	// TODO Auto-generated destructor stub
}

void cAudioTrack::LoadAudio(const QString &filename)
{
	QString sufix = QFileInfo(filename).suffix();
	loaded = false;

#ifdef USE_SNDFILE
	if (sufix.toLower() == "wav")
	{
		SNDFILE *infile = NULL;
		SF_INFO sfinfo;
		memset(&sfinfo, 0, sizeof(sfinfo));

		if ((infile = sf_open(filename.toLocal8Bit().constData(), SFM_READ, &sfinfo)) == NULL)
		{
			qCritical() << "Not able to open input file:" << filename;
			qCritical() << sf_strerror(NULL);
			return;
		};

		qDebug() << "channels:" << sfinfo.channels << "rate:" << sfinfo.samplerate
						 << "samples:" << sfinfo.frames;

		sampleRate = sfinfo.samplerate;

		if (sfinfo.frames > 0)
		{
			rawAudio.reserve(sfinfo.frames);
			rawAudio.resize(sfinfo.frames);

			float *tempBuff = new float[sfinfo.frames * sfinfo.channels];
			sf_count_t readSamples = sf_readf_float(infile, tempBuff, sfinfo.frames);

			for (int64_t i = 0; i < readSamples; i++)
			{
				float sample = 0.0;
				for (int chan = 0; chan < sfinfo.channels; chan++)
				{
					sample += tempBuff[i * sfinfo.channels + chan];
				}
				sample /= sfinfo.channels;
				rawAudio[i] = sample;
			}

			length = readSamples;

			delete[] tempBuff;
		}

		sf_close(infile);
		loaded = true;
		emit loadingFinished();
	}
#endif

	if (!loaded)
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
		connect(
			decoder, SIGNAL(error(QAudioDecoder::Error)), this, SLOT(slotError(QAudioDecoder::Error)));

		decoder->start();
	}
}

void cAudioTrack::slotReadBuffer()
{
	QAudioBuffer audioBuffer = decoder->read();

	qint64 duration = decoder->duration();
	qint64 totalSamplesApprox = (duration + 1000) * sampleRate / 1000;

	// reservation of memory if length is already known
	if (duration > 0 && !memoryReserved)
	{
		rawAudio.reserve(totalSamplesApprox);
		memoryReserved = true;
	}

	int frameCount = audioBuffer.frameCount();
	if (frameCount > 0)
	{
		qint16 *frames = audioBuffer.data<qint16>();

		for (int i = 0; i < frameCount; i++)
		{
			rawAudio.append(frames[i] / 32768.0);
		}
	}
	length = rawAudio.size();
	double percent = (double)length / totalSamplesApprox * 100.0;
	emit loadingProgress(percent);
}

void cAudioTrack::slotFinished()
{
	qDebug() << "finished";
	qDebug() << length << (double)length / sampleRate;
	loaded = true;
	emit loadingFinished();
}

float cAudioTrack::getSample(int sampleIndex) const
{
	if (isLoaded() && sampleIndex < length)
	{
		return rawAudio[sampleIndex];
	}
	else
	{
		return 0.0;
	}
}

void cAudioTrack::slotError(QAudioDecoder::Error error)
{
	qDebug() << "error" << error;
}
