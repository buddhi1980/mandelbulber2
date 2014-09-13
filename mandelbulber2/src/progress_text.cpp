/*
 * progress_text.cpp
 *
 *  Created on: Jun 2, 2014
 *      Author: krzysztof
 */

#include "progress_text.hpp"

cProgressText::cProgressText()
{
	timer.start();
	lastTimeForETA = 0;
	lastProgressForETA = 0;
	renderingSpeed = 0.0;
	lastSpeed = 0.0;
	renderingAcceleration = 0.0;
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

	if ((time > lastTimeForETA * 1.1L || time - lastTimeForETA > 600000) && progress > lastProgressForETA)
	{
		double renderingSpeedNew = (progress - lastProgressForETA) / (time - lastTimeForETA);
		if(renderingSpeed > 0)
		{
			renderingSpeed = renderingSpeed + (renderingSpeedNew - renderingSpeed) * 0.1;
		}
		else
		{
			renderingSpeed = renderingSpeedNew;
		}
		renderingAcceleration = (renderingSpeed - lastSpeed) / (time - lastTimeForETA);
		lastProgressForETA = progress;
		lastTimeForETA = time;
		lastSpeed = renderingSpeed;
	}

	qint64 timeToEnd;
	if (progressLim > 0.0 && renderingSpeed > 0)
	{
		timeToEnd = (1.0 - progressLim) / renderingSpeed;
	}
	else timeToEnd = -1;

	if (progressLim < 1.0)
	{
		text = "Done " + QString::number(progressLim * 100.0, 'f', 2) + " %, elapsed: " + TimeString(time) + ", estamined to end: " + TimeString(timeToEnd);
	}
	else
	{
		text = QString("100% Done, ") + QString("total time: ") + TimeString(time);
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
