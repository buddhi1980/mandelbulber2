/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2026 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * cAnimationRenderTimes - measures the render time of every frame of an
 * animation, saves them to a CSV file in the animation folder and writes a
 * summary (min / mean / max and frames slower than a threshold) to the log
 */

#include "animation_render_times.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QTextStream>

#include "initparameters.hpp"
#include "netrender.hpp"
#include "system_data.hpp"
#include "write_log.hpp"

cAnimationRenderTimes::cAnimationRenderTimes(
	enumAnimationType _type, const QString &animationFolder)
		: type(_type)
{
	// NetRender clients render only a part of the frames into a temporary folder, so the
	// statistics are collected by the computer which renders into the animation folder
	enabled = gPar->Get<bool>("anim_save_render_times") && !gNetRender->IsClient();
	if (!enabled) return;

	threshold = gPar->Get<double>("anim_render_time_threshold");
	csvFilePath = QDir(animationFolder).filePath(csvFileName());

	if (!QFileInfo::exists(csvFilePath))
	{
		QFile file(csvFilePath);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QTextStream out(&file);
			if (type == animKeyframe)
				out << "frame,keyframe,subframe,seconds\n";
			else
				out << "frame,seconds\n";
		}
		else
		{
			WriteLog("Cannot create file with frame render times: " + csvFilePath, 1);
		}
	}
}

cAnimationRenderTimes::~cAnimationRenderTimes()
{
	if (enabled && !frameTimes.isEmpty()) WriteSummary();
}

void cAnimationRenderTimes::StartFrame()
{
	if (enabled) timer.start();
}

void cAnimationRenderTimes::FinishFrame(int frameIndex, int keyframeIndex, int subIndex)
{
	if (!enabled) return;

	const double seconds = timer.nsecsElapsed() / 1.0e9;
	frameTimes.append({frameIndex, seconds});

	// the file is appended after every frame, so the times are kept when the rendering is
	// interrupted
	QFile file(csvFilePath);
	if (file.open(QIODevice::Append | QIODevice::Text))
	{
		QTextStream out(&file);
		if (type == animKeyframe)
			out << frameIndex << "," << keyframeIndex << "," << subIndex << ","
					<< QString::number(seconds, 'f', 3) << "\n";
		else
			out << frameIndex << "," << QString::number(seconds, 'f', 3) << "\n";
	}
}

void cAnimationRenderTimes::WriteSummary() const
{
	sFrameTime fastest = frameTimes.first();
	sFrameTime slowest = frameTimes.first();
	double total = 0.0;
	QStringList slowFrames;
	for (const sFrameTime &frameTime : frameTimes)
	{
		total += frameTime.seconds;
		if (frameTime.seconds < fastest.seconds) fastest = frameTime;
		if (frameTime.seconds > slowest.seconds) slowest = frameTime;
		if (threshold > 0.0 && frameTime.seconds > threshold)
			slowFrames.append(QString("%1 (%2 s)")
					.arg(frameTime.frameIndex)
					.arg(QString::number(frameTime.seconds, 'f', 3)));
	}

	QStringList summary;
	summary.append(
		QString("Frame render times: %1 frames, min %2 s (frame %3), mean %4 s, max %5 s (frame %6), "
						"total %7 s, saved to %8")
			.arg(frameTimes.size())
			.arg(QString::number(fastest.seconds, 'f', 3))
			.arg(fastest.frameIndex)
			.arg(QString::number(total / frameTimes.size(), 'f', 3))
			.arg(QString::number(slowest.seconds, 'f', 3))
			.arg(slowest.frameIndex)
			.arg(QString::number(total, 'f', 3))
			.arg(QDir::toNativeSeparators(csvFilePath)));

	if (threshold > 0.0)
	{
		summary.append(QString("Frames rendered longer than %1 s: %2")
				.arg(threshold)
				.arg(slowFrames.isEmpty() ? QString("none") : slowFrames.join(", ")));
	}

	for (const QString &line : summary)
	{
		if (systemData.noGui)
			WriteLogCout(line + "\n", 1);
		else
			WriteLog(line, 1);
	}
}
