/*
 * audio_fft_data.h
 *
 *  Created on: 4 gru 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_AUDIO_FFT_DATA_H_
#define MANDELBULBER2_SRC_AUDIO_FFT_DATA_H_

class cAudioFFTdata
{
public:
	static const int fftSize = 1024;

	cAudioFFTdata();
	~cAudioFFTdata();

	float data[fftSize];
};

#endif /* MANDELBULBER2_SRC_AUDIO_FFT_DATA_H_ */
