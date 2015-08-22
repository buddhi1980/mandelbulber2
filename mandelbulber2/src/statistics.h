/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * statistics - class to hold statistics of rendering stats
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef MANDELBULBER2_SRC_STATISTICS_H_
#define MANDELBULBER2_SRC_STATISTICS_H_

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
	int numberOfRenderedPixels;
	double time;

	double GetTotalNumberOfIterations() {return totalNumberOfIterations;}
	double GetNumberOfIterationsPerPixel() {return (double)totalNumberOfIterations / numberOfRenderedPixels;}
	double GetNumberOfIterationsPerSecond() {return (double)totalNumberOfIterations / time;}
	double GetMissedDEPercentage() {return (double)missedDE / numberOfRaymarchings * 100.0;}
	void Reset();
};

#endif /* MANDELBULBER2_SRC_STATISTICS_H_ */
