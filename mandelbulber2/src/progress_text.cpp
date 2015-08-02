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

#include "progress_text.hpp"

cProgressText::cProgressText()
{
	timer.start();
	lastTimeForETA = 0;
	lastProgressForETA = 0;
	renderingSpeed = 0.0;
}

void cProgressText::ResetTimer()
{
	timer.restart();
}

QString cProgressText::getText(double progress)
{
	QString text;

	double progressLim = progress;
	if(progress < 0.0) progressLim = 1.0;
	if(progress > 1.0) progressLim = 1.0;

	qint64 time = timer.elapsed();

	if ((time > lastTimeForETA * 1.1L || time - lastTimeForETA > 60000) && progress > lastProgressForETA)
	{
		bool filter = true;
		double renderingSpeedNew = (progress - lastProgressForETA) / (time - lastTimeForETA);
		if(renderingSpeed > 0 && filter)
			renderingSpeed = renderingSpeed + (renderingSpeedNew - renderingSpeed) * 0.1;
		else
			renderingSpeed = renderingSpeedNew;

		lastProgressForETA = progress;
		lastTimeForETA = time;
	}

	qint64 timeToEnd;
	if (progressLim > 0.0 && renderingSpeed > 0)
	{
		timeToEnd = (1.0 - progressLim) / renderingSpeed;
	}
	else timeToEnd = -1;

	if (progressLim < 1.0)
	{
		text = QObject::tr("Done %1%, elapsed: %2, estimated to end: %3")
				.arg(QString::number(progressLim * 100.0, 'f', 2))
				.arg(TimeString(time))
				.arg(TimeString(timeToEnd));
	}
	else
	{
		text = QObject::tr("100% Done, total time: %3").arg(TimeString(time));
	}

	return text;
}

QString cProgressText::TimeString(qint64 time)
{
	QString timeString;
	double time_s = time/1000.0;
	int time_min = (int)time_s / 60;
	int time_h = (int)time_s / (60 * 60);
	int time_d = (int)time_s / (60 * 60 * 24);

	QString time_s_str, time_min_str, time_h_str, time_d_str;

	time_s_str = QString::number(fmod(time_s, 60.0), 'f', 2) + "s";

	if(time_min > 0) time_min_str = QString::number(time_min % 60) + "m ";
	if(time_h > 0) time_h_str = QString::number(time_h % 24) + "h ";
	if(time_d > 0) time_d_str = QString::number(time_d) + "d ";

	timeString = time_d_str + time_h_str + time_min_str + time_s_str;

	if(time == -1)
	{
		timeString = "n/a";
	}

	return timeString;
}

void ProgressStatusText(const QString &text, const QString &progressText, double progress, QStatusBar *statusBar, QProgressBar *progressBar)
{
	if(statusBar)
	{
		statusBar->showMessage(text, 0);
	}

	if(progressBar)
	{
		progressBar->setValue(progress * 1000.0);
		progressBar->setTextVisible(true);
		progressBar->setFormat(progressText);
	}
}
