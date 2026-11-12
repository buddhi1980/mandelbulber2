/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Robert Pancoast
 *
 * regression test: renders a fixed set of example scenes and compares the renders of two builds
 */

#ifndef MANDELBULBER2_SRC_REGRESSION_TEST_HPP_
#define MANDELBULBER2_SRC_REGRESSION_TEST_HPP_

#include <QImage>
#include <QList>
#include <QMap>
#include <QString>

// one scene of the regression set
struct sRegressionScene
{
	QString name;										 // short id used in file names
	QString fileName;								 // path relative to the examples folder
	int width = 160;								 // render width in pixels
	int height = 120;								 // render height in pixels
	bool fillsFrame = false;				 // the fractal covers the whole frame, so no background is valid
	QMap<QString, double> overrides; // parameter values set after the scene file is loaded
};

class cRegressionTest
{
public:
	// the fixed set of scenes, taken from the bundled examples
	static QList<sRegressionScene> Scenes();

	// renders one scene on the CPU into an 8-bit RGBA image (A is the alpha buffer);
	// returns false and sets error when the scene cannot be loaded or rendered
	static bool RenderScene(const sRegressionScene &scene, QImage *result, QString *error);

	// renders every scene twice into the folder (<name>.png and <name>.repeat.png);
	// returns 0 when all rendered, 1 when any failed, 2 when the folder cannot be created
	static int RenderAll(const QString &folder);

	// compares two folders written by RenderAll. The two "before" renders of a scene give its
	// noise, and the "after" render must stay within limits derived from that noise. Writes
	// before, after and difference images and report.txt into the output folder; returns 0 when every
	// scene passes, 1 when any fails, 2 when a folder is missing
	static int Compare(
		const QString &beforeFolder, const QString &afterFolder, const QString &outputFolder);

private:
	static QString ReadRenderInfoValue(const QString &folder, const QString &key);
};

#endif /* MANDELBULBER2_SRC_REGRESSION_TEST_HPP_ */
