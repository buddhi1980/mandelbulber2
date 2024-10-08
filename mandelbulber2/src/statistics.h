/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * statistics - class to hold statistics of rendering stats
 */

#ifndef MANDELBULBER2_SRC_STATISTICS_H_
#define MANDELBULBER2_SRC_STATISTICS_H_

#include <QString>

#include "histogram.hpp"

class cStatistics
{
public:
	cStatistics();
	~cStatistics();
	cHistogram histogramIterations;
	cHistogram histogramStepCount;
	long long totalNumberOfIterations;
	int missedDE;
	int numberOfRaymarchings;
	size_t numberOfRenderedPixels;
	double maskedPixels;
	double tilesDone;
	long long totalNumberOfDOFRepeats;
	double totalNoise;
	double time;
	QString usedDEType;

	double GetTotalNumberOfIterations() const { return totalNumberOfIterations; }
	double GetNumberOfIterationsPerPixel() const
	{
		return double(totalNumberOfIterations) / numberOfRenderedPixels;
	}
	double GetNumberOfIterationsPerSecond() const { return double(totalNumberOfIterations) / time; }
	double GetMissedDEPercentage() const { return double(missedDE) / numberOfRaymarchings * 100.0; }
	QString GetDETypeString() const { return usedDEType; }
	double GetAverageDOFSamples() const
	{
		return double(totalNumberOfDOFRepeats) / numberOfRenderedPixels;
	}
	double GetAverageDOFNoise() const { return totalNoise / numberOfRenderedPixels; }
	void Reset();

	double getMaskedPixels() const { return maskedPixels; }
	double getTilesDone() const { return tilesDone; }
};

#endif /* MANDELBULBER2_SRC_STATISTICS_H_ */
