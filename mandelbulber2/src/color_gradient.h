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
		sRGB8 color;
		double position; // from 0 to 1.0

		static bool lessCompare(sColor a, sColor b) { return a.position < b.position; }
	};

	int AddColor(sRGB8 color, double position); // returns new color index
	void ModifyColor(int index, sRGB8 color);
	void ModifyPosition(int index, double position);
	void RemoveColor(int index);
	sRGB8 GetColor(double position, bool smooth);
	QList<sColor> GetListOfColors();
	QVector<sRGB8> GetGradient(int length, bool smooth);
	QString GetColorsAsString();
	void SetColorsFromString(const QString &string);

private:
	void SortGradient();
	int PaletteIterator(int paletteIndex, double position);
	sRGB8 Interpolate(int paletteIndex, double pos, bool smooth);
	double CorrectPosition(double position, int ignoreIndex);

	QList<sColor> colors;
	QList<sColor> sortedColors;
	bool sorted;
};

#endif /* MANDELBULBER2_SRC_COLOR_GRADIENT_H_ */
