/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cProgressText class - takes care about progress information
 */

#include "progress_text.hpp"

#include "../src/render_window.hpp"
#include "headless.h"
#include "interface.hpp"
#include "system.hpp"

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
	if (progress < 0.0) progressLim = 1.0;
	if (progress > 1.0) progressLim = 1.0;

	qint64 time = timer.elapsed();

	if ((time > lastTimeForETA * 1.1L || time - lastTimeForETA > 60000)
			&& progress > lastProgressForETA)
	{
		bool filter = true;
		double renderingSpeedNew = (progress - lastProgressForETA) / (time - lastTimeForETA);
		if (renderingSpeed > 0 && filter)
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
	else
		timeToEnd = -1;

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

QString cProgressText::TimeString(qint64 time) const
{
	QString timeString;
	double time_s = time / 1000.0;
	int time_min = int(time_s) / 60;
	int time_h = int(time_s) / (60 * 60);
	int time_d = int(time_s) / (60 * 60 * 24);

	QString time_s_str, time_min_str, time_h_str, time_d_str;

	time_s_str = QString::number(fmod(time_s, 60.0), 'f', 1) + "s";

	if (time_min > 0) time_min_str = QString::number(time_min % 60) + "m ";
	if (time_h > 0) time_h_str = QString::number(time_h % 24) + "h ";
	if (time_d > 0) time_d_str = QString::number(time_d) + "d ";

	timeString = time_d_str + time_h_str + time_min_str + time_s_str;

	if (time == -1)
	{
		timeString = "n/a";
	}

	return timeString;
}

void cProgressText::ProgressStatusText(
	const QString &text, const QString &progressText, double progress, enumProgressType progressType)
{
	if (systemData.noGui)
	{
		if (gMainInterface->headless)
		{
			gMainInterface->headless->slotUpdateProgressAndStatus(
				text, progressText, progress, progressType);
		}
	}
	else
	{
		if (gMainInterface->mainWindow)
		{
			gMainInterface->mainWindow->slotUpdateProgressAndStatus(
				text, progressText, progress, progressType);
		}
	}
}
