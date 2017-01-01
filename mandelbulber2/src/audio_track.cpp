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

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>

#include <QAudioRecorder>
#include <QAudioFormat>
#include <QAudioDecoder>
#include <QFileInfo>
#include <QtCore/QtGlobal>

#include "audio_fft_data.h"
#include "system.hpp"
#ifdef USE_SNDFILE
#include <sndfile.h>
#endif

cAudioTrack::cAudioTrack(QObject *parent) : QObject(parent)
{
	fftAudio = NULL;
	decoder = NULL;
	Clear();
}

cAudioTrack::~cAudioTrack()
{
	if (fftAudio) delete[] fftAudio;
	if (decoder) delete decoder;
}

void cAudioTrack::Clear()
{
	if (decoder) delete decoder;
	decoder = NULL;

	memoryReserved = false;
	length = 0;
	sampleRate = 44100;
	loaded = false;
	loadingInProgress = false;
	framesPerSecond = 30.0;
	numberOfFrames = 0;
	maxVolume = 0.0;
	maxFft = 0.0;
	rawAudio.clear();

	if (fftAudio) delete[] fftAudio;
	fftAudio = NULL;

	animation.clear();
	maxFftArray = cAudioFFTdata();
}

void cAudioTrack::LoadAudio(const QString &filename)
{
	WriteLogString("Loading audio started", filename, 2);

	Clear();

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
				maxVolume = qMax(sample, maxVolume);
			}

			length = readSamples;

			delete[] tempBuff;
		}

		sf_close(infile);
		loaded = true;
		WriteLog("Loading wave file finished", 2);
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

		if (decoder) delete decoder;
		decoder = new QAudioDecoder(this);
		decoder->setAudioFormat(desiredFormat);
		decoder->setSourceFilename(filename);

		connect(decoder, SIGNAL(bufferReady()), this, SLOT(slotReadBuffer()));
		connect(decoder, SIGNAL(finished()), this, SLOT(slotFinished()));
		connect(
			decoder, SIGNAL(error(QAudioDecoder::Error)), this, SLOT(slotError(QAudioDecoder::Error)));

		loadingInProgress = true;
		decoder->start();

		while (loadingInProgress)
		{
			QApplication::processEvents();
		}
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
			float sample = frames[i] / 32768.0;
			rawAudio.append(sample);
			maxVolume = qMax(sample, maxVolume);
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
	loadingInProgress = false;
	WriteLog("Loading mp3 file finished", 2);
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

float cAudioTrack::getAnimation(int frame) const
{
	if (frame < numberOfFrames)
	{
		return animation[frame];
	}
	else
	{
		return animation.last();
	}
}

void cAudioTrack::slotError(QAudioDecoder::Error error)
{
	qCritical() << "cAudioTrack::error" << error;
	loadingInProgress = false;
}

void cAudioTrack::calculateFFT()
{
	if (loaded && length > cAudioFFTdata::fftSize)
	{
		WriteLog("FFT calculation started", 2);

		if (fftAudio) delete[] fftAudio;
		fftAudio = new cAudioFFTdata[numberOfFrames];

#pragma omp parallel for
		for (int frame = 0; frame < numberOfFrames; ++frame)
		{
			int sampleOffset = (qint64)frame * sampleRate / framesPerSecond;
			// prepare complex data for fft transform
			double fftData[cAudioFFTdata::fftSize * 2];
			for (int i = 0; i < cAudioFFTdata::fftSize; i++)
			{
				fftData[2 * i] =
					getSample(i + sampleOffset) * 0.5
					* (1.0 - cos((2 * M_PI * i) / (cAudioFFTdata::fftSize - 1))); // Hann window function
				fftData[2 * i + 1] = 0.0;
			}

			// do FFT
			gsl_complex_packed_array data = fftData;
			gsl_fft_complex_radix2_forward(data, 1, cAudioFFTdata::fftSize);

			// write ready FFT data to storage buffer
			cAudioFFTdata fftFrame;
			for (int i = 0; i < cAudioFFTdata::fftSize; i++)
			{
				float re = fftData[2 * i];
				float im = fftData[2 * i + 1];
				float absVal = sqrt(re * re + im * im);
				fftFrame.data[i] = absVal;
				maxFft = qMax(absVal, maxFft);
				maxFftArray.data[i] = qMax(maxFftArray.data[i], absVal);
			}
			fftAudio[frame] = fftFrame;
		}
		WriteLog("FFT calculation finished", 2);
	}
}

cAudioFFTdata cAudioTrack::getFFTSample(int frame) const
{
	if (isLoaded() && frame < numberOfFrames)
	{
		return fftAudio[frame];
	}
	else
	{
		return cAudioFFTdata();
	}
}

float cAudioTrack::getBand(int frame, double midFreq, double bandwidth) const
{
	if (isLoaded() && frame < numberOfFrames)
	{
		cAudioFFTdata fft = fftAudio[frame];

		int first = freq2FftPos(midFreq - 0.5 * bandwidth);
		if (first < 0) first = 0;
		int last = freq2FftPos(midFreq + 0.5 * bandwidth);
		if (last > cAudioFFTdata::fftSize / 2) last = cAudioFFTdata::fftSize / 2;

		double sum = 0.0;
		float maxVal = 0.0;
		for (int i = first; i <= last; i++)
		{
			sum += fft.data[i];
			maxVal += maxFftArray.data[i];
		}
		int count = last - first + 1;

		maxVal /= count;
		float value = sum / count / maxVal;
		return value;
	}
	else
	{
		return 0.0;
	}
}

int cAudioTrack::freq2FftPos(double freq) const
{
	return (double)cAudioFFTdata::fftSize / (double)sampleRate * freq;
}

void cAudioTrack::setFramesPerSecond(double _framesPerSecond)
{
	framesPerSecond = _framesPerSecond;
	numberOfFrames = length * framesPerSecond / sampleRate;
}

void cAudioTrack::calculateAnimation(double midFreq, double bandwidth)
{
	animation.clear();
	animation.reserve(numberOfFrames);
	for (int i = 0; i < numberOfFrames; i++)
	{
		animation.append(getBand(i, midFreq, bandwidth));
	}
}
