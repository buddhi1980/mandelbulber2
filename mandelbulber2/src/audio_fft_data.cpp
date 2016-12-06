/*
 * audio_fft_data.cpp
 *
 *  Created on: 4 gru 2016
 *      Author: krzysztof
 */

#include "audio_fft_data.h"

cAudioFFTdata::cAudioFFTdata()
{
	for(int i = 0; i < fftSize; i++)
	{
		data[i] = 0.0;
	}
}

cAudioFFTdata::~cAudioFFTdata()
{
	// TODO Auto-generated destructor stub
}
