/*
 * headless.h
 *
 *  Created on: 12-09-2015
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_HEADLESS_H_
#define MANDELBULBER2_SRC_HEADLESS_H_

#include <QtCore>

class cHeadless
{
public:
	cHeadless();
	~cHeadless();

	enum ansiColor
	{
		noExplicitColor = -1,
		ansiBlack = 0, ansiRed = 1, ansiGreen = 2, ansiYellow = 3,
		ansiBlue = 4, ansiMagenta = 5, ansiCyan = 6, ansiWhite = 7
	};

	void RenderStillImage(void);
	static void RenderingProgressOutput(const QString &progressTxt, double percentDone, bool newLine = false);
	static QString colorize(QString text, ansiColor foregroundcolor, ansiColor backgroundColor = noExplicitColor, bool bold = false);
	static QString formatLine(const QString& text);
};

#endif /* MANDELBULBER2_SRC_HEADLESS_H_ */
