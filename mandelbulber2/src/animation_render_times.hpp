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

#ifndef MANDELBULBER2_SRC_ANIMATION_RENDER_TIMES_HPP_
#define MANDELBULBER2_SRC_ANIMATION_RENDER_TIMES_HPP_

#include <QElapsedTimer>
#include <QList>
#include <QString>

class cAnimationRenderTimes
{
public:
	enum enumAnimationType
	{
		animKeyframe,
		animFlight
	};

	// statistics are collected only when the 'anim_save_render_times' preference is enabled
	cAnimationRenderTimes(enumAnimationType type, const QString &animationFolder);
	~cAnimationRenderTimes(); // writes the summary of the rendered frames to the log

	bool IsEnabled() const { return enabled; }
	void StartFrame();
	// keyframeIndex and subIndex are used only for keyframe animations
	void FinishFrame(int frameIndex, int keyframeIndex = 0, int subIndex = 0);

	static QString csvFileName() { return QStringLiteral("render_times.csv"); }

private:
	struct sFrameTime
	{
		int frameIndex;
		double seconds;
	};

	void WriteSummary() const;

	enumAnimationType type;
	bool enabled = false;
	double threshold = 0.0;
	QString csvFilePath;
	QElapsedTimer timer;
	QList<sFrameTime> frameTimes;
};

#endif /* MANDELBULBER2_SRC_ANIMATION_RENDER_TIMES_HPP_ */
