/*
 * image_scale.cpp
 *
 *  Created on: 2 maj 2016
 *      Author: krzysztof
 */

#include <QtCore>
#include "image_scale.hpp"
#include <algorithm>
using std::min;

double ImageScaleComboSelection2Double(int index)
{
	double scales[] = { 0.0, 4.0, 2.0, 1.0, 0.5, 0.25, 0.1 };
	if (index < 7)
	{
		return scales[index];
	}
	else
	{
		qCritical() << "Wrong image scale";
		return -1.0;
	}
}

double CalcMainImageScale(double scale, int previewWidth, int previewHeight, cImage *image)
{
	double scaleOut;
	if (scale == 0.0)
	{
		double scale1 = (double) previewHeight / image->GetHeight();
		double scale2 = (double) previewWidth / image->GetWidth();
		scaleOut = min(scale1, scale2);
	}
	else
	{
		scaleOut = scale;
	}
	return scaleOut;
}


