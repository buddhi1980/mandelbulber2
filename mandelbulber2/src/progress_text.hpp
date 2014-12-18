/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cProgressText class - takes care about progress information
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

#ifndef PROGRESS_TEXT_HPP_
#define PROGRESS_TEXT_HPP_

#include <QtCore>
#include <QProgressBar>
#include <QStatusBar>

class cProgressText
{
public:
	cProgressText();
	void ResetTimer();
	QString getText(double progress);
	double getTime() {return timer.elapsed()/1000.0;};

private:
	QElapsedTimer timer;
	QString TimeString(qint64 time);
	qint64 lastTimeForETA;
	double lastProgressForETA;
	double renderingSpeed;
};

void ProgressStatusText(const QString &text, const QString &progressText, double progress, QStatusBar *statusBar, QProgressBar *progressBar);




#endif /* PROGRESS_TEXT_HPP_ */
