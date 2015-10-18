/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cHeadless - class to handle CLI instructions without GUI manipulation
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen
 */

#ifndef MANDELBULBER2_SRC_HEADLESS_H_
#define MANDELBULBER2_SRC_HEADLESS_H_

#include <QtCore>
#include "progress_text.hpp"
#include "statistics.h"

class cHeadless : public QObject
{
	Q_OBJECT
public:
	cHeadless();
	~cHeadless();

	enum ansiColor
	{
		noExplicitColor = -1,
		ansiBlack = 0, ansiRed = 1, ansiGreen = 2, ansiYellow = 3,
		ansiBlue = 4, ansiMagenta = 5, ansiCyan = 6, ansiWhite = 7
	};

	void RenderStillImage(QString filename, QString imageFileFormat);
	void RenderFlightAnimation();
	void RenderKeyframeAnimation();
	static void RenderingProgressOutput(const QString &header, const QString &progressTxt, double percentDone);
	static QString colorize(QString text, ansiColor foregroundcolor, ansiColor backgroundColor = noExplicitColor, bool bold = false);
	static QString formatLine(const QString& text);
	static bool ConfirmMessage(QString message);
	static void MoveCursor(int leftRight, int downUp);

	public slots:
	void slotNetRender();
	void slotUpdateProgressAndStatus(const QString &text, const QString &progressText, double progress, cProgressText::enumProgressType progressType = cProgressText::progress_IMAGE);
	void slotUpdateStatistics(cStatistics stat);

	signals:
	void finished();

};

#endif /* MANDELBULBER2_SRC_HEADLESS_H_ */
