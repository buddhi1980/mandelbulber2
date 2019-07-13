/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * cHeadless - class to handle CLI instructions without GUI manipulation
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
	~cHeadless() override;

	enum ansiColor
	{
		noExplicitColor = -1,
		ansiBlack = 0,
		ansiRed = 1,
		ansiGreen = 2,
		ansiYellow = 3,
		ansiBlue = 4,
		ansiMagenta = 5,
		ansiCyan = 6,
		ansiWhite = 7
	};

	void RenderStillImage(QString filename, QString imageFileFormat);
	[[noreturn]] static void RenderQueue();
	void RenderVoxel(QString voxelFormat);
	void RenderFlightAnimation() const;
	void RenderKeyframeAnimation() const;
	static void RenderingProgressOutput(
		const QString &header, const QString &progressTxt, double percentDone);
	static QString colorize(QString text, ansiColor foregroundColor,
		ansiColor backgroundColor = noExplicitColor, bool bold = false);
	static QString formatLine(const QString &text);
	static bool ConfirmMessage(QString message);
	static void MoveCursor(int leftRight, int downUp);
	static void EraseLine();

public slots:
	void slotNetRender();
	static void slotUpdateProgressAndStatus(const QString &text, const QString &progressText,
		double progress, cProgressText::enumProgressType progressType = cProgressText::progress_IMAGE);
	void slotUpdateStatistics(const cStatistics &stat) const;

signals:
	void finished();
};

#endif /* MANDELBULBER2_SRC_HEADLESS_H_ */
