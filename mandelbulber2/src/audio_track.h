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

#ifndef MANDELBULBER2_SRC_AUDIO_TRACK_H_
#define MANDELBULBER2_SRC_AUDIO_TRACK_H_

#include <QObject>
#include <QAudioDecoder>

#include "audio_fft_data.h"

class cAudioTrack : public QObject
{
	Q_OBJECT

public:
	cAudioTrack(QObject *parent = NULL);
	~cAudioTrack();
	void Clear();

	void LoadAudio(const QString &filename);

	int getLength() const { return length; }
	bool isLoaded() const { return loaded; }
	int getSampleRate() const { return sampleRate; }
	float getSample(int sampleIndex) const;
	void calculateFFT();
	cAudioFFTdata getFFTSample(int frame) const;
	float getBand(int frame, double midFreq, double bandwidth) const;
	int getNumberOfFrames() const { return numberOfFrames; };
	void setFramesPerSecond(double _framesPerSecond);
	double getFramesPerSecond() const { return framesPerSecond; }
	float getMaxFft() const { return maxFft; }
	float getMaxVolume() const { return maxVolume; }
	int freq2FftPos(double freq) const;
	void calculateAnimation(double midFreq, double bandwidth);
	float getAnimation(int frame) const;

private slots:
	void slotReadBuffer();
	void slotFinished();
	void slotError(QAudioDecoder::Error error);

private:
	QAudioDecoder *decoder;
	QVector<float> rawAudio;
	cAudioFFTdata *fftAudio;
	QVector<float> animation;
	bool memoryReserved;
	int length;
	int sampleRate;
	bool loaded;
	bool loadingInProgress;

	double framesPerSecond;
	int numberOfFrames;

	float maxVolume;
	float maxFft;
	cAudioFFTdata maxFftArray;

signals:
	void loadingFinished();
	void loadingProgress(double percentage);
};

#endif /* MANDELBULBER2_SRC_AUDIO_TRACK_H_ */
