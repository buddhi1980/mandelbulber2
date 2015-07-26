/*
 * statistics.cpp
 *
 *  Created on: 31-05-2015
 *      Author: krzysztof
 */

#include "statistics.h"

cStatistics::cStatistics()
{
  totalNumberOfIterations = 0;
  missedDE = 0;
  numberOfRaymarchings = 0;
  numberOfRenderedPixels = 0;
  time = 0.0;
}

cStatistics::~cStatistics()
{
}

void cStatistics::Reset()
{
  totalNumberOfIterations = 0;
  missedDE = 0;
  numberOfRaymarchings = 0;
  numberOfRenderedPixels = 0;
  time = 0.0;
  histogramIterations.Clear();
  histogramStepCount.Clear();
}
