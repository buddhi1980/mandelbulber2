/*
 * statistics.h
 *
 *  Created on: 31-05-2015
 *      Author: krzysztof
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
