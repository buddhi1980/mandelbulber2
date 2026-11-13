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
#include <QStringList>

// one scene of the regression set: a bundled example file, or the default scene with one
// fractal formula
struct sRegressionScene
{
	QString name;						 // id used in file names
	QString fileName;				 // example: path relative to the examples folder; empty for a formula
	int formulaId = 0;			 // formula scene: the fractal::enumFractalFormula to render
	bool fillsFrame = false; // the fractal covers the whole frame, so no background is valid
};

class cRegressionTest
{
public:
	// the examples of ExampleFiles(), followed by one scene for every fractal formula: the
	// default scene with that formula, so that a new formula is tested automatically
	static QList<sRegressionScene> Scenes();

	// the fixed list of examples the regression test renders, as paths relative to the examples
	// folder (regression_examples.cpp)
	static QStringList ExampleFiles();

	// the scenes of this run: all of Scenes(), or the named ones (see SetSceneNames), or one
	// shard of them (see SetShard)
	static QList<sRegressionScene> SelectedScenes();

	// selects only the scenes with these names (an empty list selects every scene), so that a
	// scheduler can give each machine a set of scenes that take about the same time to render
	static void SetSceneNames(const QStringList &names);

	// splits the scenes into count shards and selects shard index (1..count): scene i belongs
	// to shard (i mod count) + 1, so slow scenes spread over the shards; returns false when the
	// numbers are out of range
	static bool SetShard(int index, int count);

	// a few examples that cover the main rendering features, for the --test case
	static QList<sRegressionScene> CoreScenes();

	// the longer side of every render in pixels; the other side follows the scene's aspect ratio
	static constexpr int renderLongSide = 320;
	// the longer side for scenes with Monte Carlo sampling, which are much slower to render
	static constexpr int monteCarloLongSide = 160;
	// the longer side for the formula scenes: they are there to run every formula's code, which
	// a small image does as well, and a few formulas render very slowly in the default scene
	static constexpr int formulaLongSide = 160;

	// renders one scene on the CPU into an 8-bit RGBA image (A is the alpha buffer), with the
	// longer side renderLongSide pixels (monteCarloLongSide with Monte Carlo sampling,
	// formulaLongSide for a formula scene) and Monte Carlo sampling reduced; returns false and sets
	// error when the scene cannot be loaded or rendered
	static bool RenderScene(const sRegressionScene &scene, QImage *result, QString *error);

	// renders every scene twice into the folder (<name>.png and <name>.repeat.png), or once
	// (<name>.png) when repeat is false: the build under test needs only one render, because
	// the noise comes from the two renders of the build it is compared with; returns 0 when all
	// rendered, 1 when any failed, 2 when the folder cannot be created
	static int RenderAll(const QString &folder, bool repeat = true);

	// compares two folders written by RenderAll. The two "before" renders of a scene give its
	// noise, and the "after" render must stay within limits derived from that noise. Writes
	// before, after and difference images and report.txt into the output folder; returns 0 when every
	// scene passes, 1 when any fails, 2 when a folder is missing
	static int Compare(
		const QString &beforeFolder, const QString &afterFolder, const QString &outputFolder);

private:
	static QString ReadRenderInfoValue(const QString &folder, const QString &key);
	static int shardIndex;
	static int shardCount;
	static QStringList sceneNames;
};

#endif /* MANDELBULBER2_SRC_REGRESSION_TEST_HPP_ */
