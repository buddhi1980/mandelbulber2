/*
 * color_gradient.h
 *
 *  Created on: 1 cze 2019
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_COLOR_GRADIENT_H_
#define MANDELBULBER2_SRC_COLOR_GRADIENT_H_
#include "color_structures.hpp"

class cColorGradient
{
public:
	cColorGradient();
	~cColorGradient();

public:
	struct sColor
	{
		sRGB16 color;
		double position; // from 0 to 1.0
	};

	int AddColor(sRGB16 color, double position); // returns new color index
	void ModifyColor(int index, sRGB16 color);
	void ModifyPosition(int index, double position);
	void RemoveColor(int index);
	sRGB16 GetColor(double position);
	QList<sColor> GetListOColors();
	QVector<sRGB16> GetGradient(int length);

private:
	void SortGradient();

	QList<sColor> colors;
	QList<sColor> sortedColors;
};

#endif /* MANDELBULBER2_SRC_COLOR_GRADIENT_H_ */
