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

#include "regression_test.hpp"

#include <algorithm>
#include <memory>

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QSize>
#include <QSysInfo>
#include <QTextStream>

#include "formula/definition/all_fractal_list.hpp"
#include "animation_frames.hpp"
#include "cimage.hpp"
#include "fractal_container.hpp"
#include "image_comparison.hpp"
#include "initparameters.hpp"
#include "keyframes.hpp"
#include "parameters.hpp"
#include "render_job.hpp"
#include "rendering_configuration.hpp"
#include "settings.hpp"
#include "system.hpp"
#include "system_directories.hpp"

namespace
{
const char *renderInfoFileName = "render-info.txt";
const char *reportFileName = "report.txt";

QString ExamplesFolder()
{
	return QDir::toNativeSeparators(systemDirectories.sharedDir + QDir::separator() + "examples");
}

// a file name made from the example's path: "Some folder/my scene 1.fract" ->
// Some_folder__my_scene_1
QString SceneName(const QString &relativePath)
{
	QString path = relativePath;
	if (path.endsWith(".fract", Qt::CaseInsensitive)) path.chop(6);
	QString name;
	for (const QChar c : path)
	{
		if (c == '/')
			name += "__";
		else if (c.isLetterOrNumber() && c.unicode() < 128)
			name += c;
		else if (c == '-' || c == '.')
			name += c;
		else
			name += '_';
	}
	return name;
}

// the render size: the longer side is longSide, the other follows the scene's aspect ratio
QSize RenderSize(int sceneWidth, int sceneHeight, int longSide)
{
	if (sceneWidth <= 0 || sceneHeight <= 0) return QSize(longSide, longSide * 3 / 4);
	if (sceneWidth >= sceneHeight)
		return QSize(longSide, std::max(16, int(qRound(double(longSide) * sceneHeight / sceneWidth))));
	return QSize(std::max(16, int(qRound(double(longSide) * sceneWidth / sceneHeight))), longSide);
}

QString CompilerName()
{
#if defined(_MSC_VER)
	return QString("MSVC %1").arg(_MSC_VER);
#elif defined(__clang__)
	return QString("Clang %1").arg(__clang_version__);
#elif defined(__GNUC__)
	return QString("GCC %1").arg(__VERSION__);
#else
	return "unknown";
#endif
}

QString CpuName()
{
#ifdef _WIN32
	return QString::fromLocal8Bit(qgetenv("PROCESSOR_IDENTIFIER"));
#else
	QFile file("/proc/cpuinfo");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		while (true)
		{
			const QString line = QString::fromUtf8(file.readLine());
			if (line.isEmpty()) break;
			if (line.startsWith("model name")) return line.section(':', 1).trimmed();
		}
	}
	return QSysInfo::currentCpuArchitecture();
#endif
}

bool WriteTextFile(const QString &fileName, const QString &text)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
	file.write(text.toUtf8());
	return true;
}

QString ScenePng(const QString &folder, const QString &name)
{
	return QDir(folder).filePath(name + ".png");
}

// the second render of the same scene, which shows how much two renders differ by chance
QString SceneRepeatPng(const QString &folder, const QString &name)
{
	return QDir(folder).filePath(name + ".repeat.png");
}

QString SceneFailure(const QString &folder, const QString &name)
{
	return QDir(folder).filePath(name + ".failed.txt");
}

// loads one render of a scene; on failure the error says why
bool LoadRender(const QString &folder, const QString &name, const QString &role, bool repeat,
	QImage *image, QString *error)
{
	const QString failureFile = SceneFailure(folder, name);
	if (QFileInfo::exists(failureFile))
	{
		QFile file(failureFile);
		QString reason;
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
			reason = QString::fromUtf8(file.readAll()).trimmed();
		*error = QString("%1 render failed: %2").arg(role, reason);
		return false;
	}
	const QString pngFile = repeat ? SceneRepeatPng(folder, name) : ScenePng(folder, name);
	if (!QFileInfo::exists(pngFile))
	{
		*error = QString("%1 image is missing").arg(role);
		return false;
	}
	if (!image->load(pngFile))
	{
		*error = QString("%1 image cannot be read").arg(role);
		return false;
	}
	*image = imageComparison::Normalized(*image);
	return true;
}
} // namespace

int cRegressionTest::shardIndex = 1;
int cRegressionTest::shardCount = 1;

QList<sRegressionScene> cRegressionTest::Scenes()
{
	// Examples that take far longer than the rest even at the regression test's size: each needs
	// more than 20 seconds for its two renders on 32 threads, about 10 minutes on a 4-core CI
	// machine. Measured on 2026-11-13 with GCC 13.3 on an Intel Core i9-14900K; the times of
	// Monte Carlo scenes were measured at 320 pixels and divided by 4 for their 160 pixels.
	const QStringList slowExamples = {
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Jos Leys Kleinian v3 "
		"SphereGrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/SphereInvBoxMenger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Tri_Grid "
		"asurf.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_pupuku "
		"pow-4.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_pupuku "
		"pow-4a.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudo kleinian "
		"abox13.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/pseudoKleinianMod2_boxBulb2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudoKleinianMod5.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudo_kleinian_mod6.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/xenodreambuieV2  pow -7 "
		"julia.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 27.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 33.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/aexion04.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/aexion06.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/clouds 2_v2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/clouds 2_v3.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid21.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid26.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hypercomplex 01.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/iter fog 008.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/iter fog 4.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/light circle.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox36.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox46.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb powe 6 - "
		"circle.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 2 - iter "
		"fog 02.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 2 - iter "
		"fog 04.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 2 - "
		"monte carlo.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 2 - "
		"slice 6.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/modified mandelbulb "
		"001.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/planet.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/winter (sss).fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/box-deuce-deuce.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/icoastahedron.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/menger-fatbackstrokin.fract",
		"Sebastian Jennen collection - license Creative Commons  (CC-BY 4.0)/pseudo kleinian std de "
		"001 plus mandelbox.fract",
		"amazing_surf 001.fract",
		"benchmark-GPU 2.fract",
		"glow.fract",
		"hybrid006.fract",
		"iteration fog.fract",
		"mandelbox_menger_with_textures.fract",
		"mandelbulb_bermarte 001.fract",
		"mandelbulb_power_2_001.fract",
		"monte carlo global illumination - cube scene.fract",
		"msltoe toroidal multi 001.fract",
		"msltoe_julia_bulb_mod3_001.fract",
		"subsurface scattering 001.fract",
		"texture transparency alpha.fract",
		"transparent001.fract",
	};

	const QDir examples(ExamplesFolder());
	QStringList files;
	QDirIterator it(
		examples.absolutePath(), QStringList() << "*.fract", QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext())
		files.append(examples.relativeFilePath(it.next()));
	files.sort();

	QList<sRegressionScene> scenes;
	for (const QString &file : files)
	{
		if (slowExamples.contains(file)) continue;
		sRegressionScene scene;
		scene.name = SceneName(file);
		scene.fileName = file;
		scenes.append(scene);
	}

	// every fractal formula in the default scene, so that every formula's code runs; the custom
	// formula is written in OpenCL by the user and has no code on the CPU
	for (const cAbstractFractal *formula : newFractalList)
	{
		if (formula->getInternalId() == fractal::none || formula->getInternalId() == fractal::custom)
			continue;
		sRegressionScene scene;
		scene.name = "formula__" + SceneName(QString::fromStdString(formula->getInternalName()));
		scene.formulaId = int(formula->getInternalId());
		scenes.append(scene);
	}
	return scenes;
}

bool cRegressionTest::SetShard(int index, int count)
{
	if (count < 1 || index < 1 || index > count) return false;
	shardIndex = index;
	shardCount = count;
	return true;
}

QList<sRegressionScene> cRegressionTest::SelectedScenes()
{
	const QList<sRegressionScene> all = Scenes();
	if (shardCount == 1) return all;
	QList<sRegressionScene> selected;
	for (int i = shardIndex - 1; i < all.size(); i += shardCount)
		selected.append(all.at(i));
	return selected;
}

QList<sRegressionScene> cRegressionTest::CoreScenes()
{
	// examples that cover the main rendering features
	const QStringList core = {
		"mandelbulb001.fract",
		"mandelbox001.fract",
		"mandelbox003.fract",					 // fake lights
		"IFS_001.fract",							 // auxiliary lights, ambient occlusion
		"IFS_002.fract",							 // depth of field, basic fog
		"Makin3D-Julia_001.fract",		 // ambient occlusion, volumetric fog
		"aboxmod1_001.fract",					 // depth of field, ambient occlusion, volumetric fog
		"boolean001.fract",						 // boolean operators
		"amazing surf mod1 001.fract", // primitives
		"ides2_001.fract",						 // iteration fog
		"hex grid 001.fract",					 // hybrid, Monte Carlo global illumination
		"difs_tree.fract",						 // hybrid, Monte Carlo depth of field
		"mandelnest pow 5.fract",			 // Monte Carlo global illumination
	};
	QList<sRegressionScene> scenes;
	for (const QString &file : core)
	{
		sRegressionScene scene;
		scene.name = SceneName(file);
		scene.fileName = file;
		scenes.append(scene);
	}
	return scenes;
}

bool cRegressionTest::RenderScene(const sRegressionScene &scene, QImage *result, QString *error)
{
	const QString fileName =
		QDir::toNativeSeparators(ExamplesFolder() + QDir::separator() + scene.fileName);
	if (scene.formulaId == 0 && !QFileInfo::exists(fileName))
	{
		*error = QString("scene file not found: %1").arg(fileName);
		return false;
	}

	// the scene renders with its own parameters, so no global or user setting changes the image
	std::shared_ptr<cParameterContainer> par(new cParameterContainer());
	std::shared_ptr<cFractalContainer> parFractal(new cFractalContainer());
	std::shared_ptr<cAnimationFrames> animFrames(new cAnimationFrames());
	std::shared_ptr<cKeyframes> keyframes(new cKeyframes());

	par->SetContainerName("main");
	InitParams(par);
	InitNodeParams(1, par);
	InitMaterialParams(1, par);
	for (int i = 0; i < parFractal->size(); i++)
	{
		parFractal->at(i)->SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(parFractal->at(i));
	}

	if (scene.formulaId != 0)
	{
		// the default scene, with this formula instead of the default one
		parFractal->at(0)->Set("formula", scene.formulaId);
	}
	else
	{
		cSettings parSettings(cSettings::formatFullText);
		parSettings.BeQuiet(true);
		if (!parSettings.LoadFromFile(fileName)
				|| !parSettings.Decode(par, parFractal, animFrames, keyframes))
		{
			*error = QString("scene file cannot be loaded: %1").arg(fileName);
			return false;
		}
	}

	// Monte Carlo scenes render at a smaller size, because they are much slower
	const bool monteCarlo = par->Get<bool>("DOF_monte_carlo");
	const QSize size = RenderSize(par->Get<int>("image_width"), par->Get<int>("image_height"),
		monteCarlo ? monteCarloLongSide : renderLongSide);
	const int width = size.width();
	const int height = size.height();
	par->Set("image_width", width);
	par->Set("image_height", height);
	par->Set("opencl_enabled", false);
	par->Set("opencl_mode", 0);
	// Monte Carlo scenes stop sampling earlier, like the example test case does, to keep the
	// set fast; the comparison measures each scene's noise, so a noisier render is fine
	par->Set("DOF_max_noise", par->Get<double>("DOF_max_noise") * 10.0);
	par->Set("DOF_min_samples", std::min(par->Get<int>("DOF_min_samples"), 3));

	bool stopRequest = false;
	std::shared_ptr<cImage> image(new cImage(width, height));
	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();

	std::unique_ptr<cRenderJob> renderJob(new cRenderJob(par, parFractal, image, 1, &stopRequest));
	renderJob->Init(cRenderJob::still, config);
	if (!renderJob->Execute())
	{
		*error = "render failed";
		return false;
	}

	// a stereo scene renders both views into one image, which is wider or taller than asked for
	const int imageWidth = int(image->GetWidth());
	const int imageHeight = int(image->GetHeight());

	image->ConvertTo8bitChar();
	QImage rendered(imageWidth, imageHeight, QImage::Format_RGBA8888);
	for (int y = 0; y < imageHeight; y++)
	{
		uchar *line = rendered.scanLine(y);
		for (int x = 0; x < imageWidth; x++)
		{
			const sRGB8 pixel = image->GetPixelImage8(x, y);
			line[4 * x + 0] = pixel.R;
			line[4 * x + 1] = pixel.G;
			line[4 * x + 2] = pixel.B;
			line[4 * x + 3] = uchar(image->GetPixelAlpha(x, y) / 256);
		}
	}
	*result = rendered;
	return true;
}

int cRegressionTest::RenderAll(const QString &folder)
{
	QTextStream out(stdout);
	if (!QDir().mkpath(folder))
	{
		out << "Cannot create folder " << folder << Qt::endl;
		return 2;
	}

	QString info;
	info += QString("version: %1\n").arg(MANDELBULBER_VERSION_STRING);
	info += QString("compiler: %1\n").arg(CompilerName());
	info += QString("host: %1\n").arg(QSysInfo::machineHostName());
	info += QString("cpu: %1\n").arg(CpuName());
	info += QString("date: %1\n").arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
	info += QString("long side: %1, with Monte Carlo sampling %2\n")
						.arg(renderLongSide)
						.arg(monteCarloLongSide);

	info += QString("shard: %1/%2\n").arg(shardIndex).arg(shardCount);

	int failed = 0;
	const QList<sRegressionScene> scenes = SelectedScenes();
	for (const sRegressionScene &scene : scenes)
	{
		QFile::remove(ScenePng(folder, scene.name));
		QFile::remove(SceneRepeatPng(folder, scene.name));
		QFile::remove(SceneFailure(folder, scene.name));

		// every scene is rendered twice: the difference between the two renders is its noise
		QElapsedTimer timer;
		timer.start();
		QImage image, repeat;
		QString error;
		bool ok = RenderScene(scene, &image, &error) && RenderScene(scene, &repeat, &error);
		if (ok
				&& !(image.save(ScenePng(folder, scene.name), "PNG")
						 && repeat.save(SceneRepeatPng(folder, scene.name), "PNG")))
		{
			ok = false;
			error = "image cannot be saved";
		}
		const qint64 elapsed = timer.elapsed();

		if (ok)
		{
			out << "rendered " << scene.name << " in " << elapsed << " ms" << Qt::endl;
			info += QString("scene: %1 %2x%3 %4 ms\n")
								.arg(scene.name)
								.arg(image.width())
								.arg(image.height())
								.arg(elapsed);
		}
		else
		{
			failed++;
			WriteTextFile(SceneFailure(folder, scene.name), error + "\n");
			out << "FAILED " << scene.name << ": " << error << Qt::endl;
			info += QString("scene: %1 failed: %2\n").arg(scene.name, error);
		}
	}

	WriteTextFile(QDir(folder).filePath(renderInfoFileName), info);
	return failed > 0 ? 1 : 0;
}

QString cRegressionTest::ReadRenderInfoValue(const QString &folder, const QString &key)
{
	QFile file(QDir(folder).filePath(renderInfoFileName));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return QString();
	const QStringList lines = QString::fromUtf8(file.readAll()).split('\n');
	for (const QString &line : lines)
	{
		if (line.startsWith(key + ":")) return line.section(':', 1).trimmed();
	}
	return QString();
}

int cRegressionTest::Compare(
	const QString &beforeFolder, const QString &afterFolder, const QString &outputFolder)
{
	QTextStream out(stdout);
	if (!QDir(beforeFolder).exists() || !QDir(afterFolder).exists())
	{
		out << "Both folders must exist: " << beforeFolder << " and " << afterFolder << Qt::endl;
		return 2;
	}
	if (!QDir().mkpath(outputFolder))
	{
		out << "Cannot create folder " << outputFolder << Qt::endl;
		return 2;
	}

	QString report;
	report += "Mandelbulber regression comparison\n";
	report += QString("before: %1\n").arg(QDir(beforeFolder).absolutePath());
	report += QString("after:  %1\n").arg(QDir(afterFolder).absolutePath());

	// a comparison only means something when both builds rendered on the same machine
	const QString hostBefore = ReadRenderInfoValue(beforeFolder, "host");
	const QString hostAfter = ReadRenderInfoValue(afterFolder, "host");
	const QString cpuBefore = ReadRenderInfoValue(beforeFolder, "cpu");
	const QString cpuAfter = ReadRenderInfoValue(afterFolder, "cpu");
	if (hostBefore.isEmpty() || hostAfter.isEmpty())
		report += "WARNING: cannot tell whether before and after were rendered on the same machine\n";
	else if (hostBefore != hostAfter || cpuBefore != cpuAfter)
		report += "WARNING: before and after were rendered on different hosts or CPUs\n";

	// the scenes the "before" build rendered; a scene added since then has nothing to compare with
	QStringList beforeScenes;
	bool haveBeforeInfo = false;
	{
		QFile file(QDir(beforeFolder).filePath(renderInfoFileName));
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			haveBeforeInfo = true;
			const QStringList lines = QString::fromUtf8(file.readAll()).split('\n');
			for (const QString &line : lines)
			{
				if (line.startsWith("scene: ")) beforeScenes.append(line.section(' ', 1, 1));
			}
		}
	}

	int failed = 0;
	int added = 0;
	int brokenInBoth = 0;
	const QList<sRegressionScene> scenes = SelectedScenes();
	for (const sRegressionScene &scene : scenes)
	{
		if (haveBeforeInfo && !beforeScenes.contains(scene.name))
		{
			added++;
			report += QString("\n[NEW ] %1\n").arg(scene.name);
			report += "  not rendered by the before build, so there is nothing to compare it with\n";
			continue;
		}

		QStringList failures;
		QImage before, beforeRepeat, after;
		QString error;
		const bool haveBefore = LoadRender(beforeFolder, scene.name, "before", false, &before, &error);
		if (!haveBefore) failures.append(error);
		const bool haveRepeat =
			haveBefore
			&& LoadRender(beforeFolder, scene.name, "repeated before", true, &beforeRepeat, &error);
		if (haveBefore && !haveRepeat) failures.append(error);
		const bool haveAfter = LoadRender(afterFolder, scene.name, "after", false, &after, &error);
		if (!haveAfter) failures.append(error);

		if (haveBefore) before.save(QDir(outputFolder).filePath(scene.name + "-before.png"), "PNG");
		if (haveAfter) after.save(QDir(outputFolder).filePath(scene.name + "-after.png"), "PNG");

		// A render that the "after" build broke fails. Some examples give a flat or empty image
		// at this size with either build; that is noted, and the scene is still compared.
		QStringList notes;
		// A scene whose structure is near the validity thresholds flips between valid and not
		// valid with the sampling noise, so the after image fails only when it is clearly broken
		// (at half the thresholds) while the before image is valid. When any of the three renders
		// has too little structure, shapes, edges and structural similarity compare noise and
		// are not checked.
		bool lowStructure = false;
		if (haveBefore && haveAfter)
		{
			const sImageValidity beforeValidity = imageComparison::Validate(before, scene.fillsFrame);
			const sImageValidity afterValidity = imageComparison::Validate(after, scene.fillsFrame);
			const sImageValidity afterBroken = imageComparison::Validate(after, scene.fillsFrame, 0.5);
			lowStructure =
				!beforeValidity.valid || !afterValidity.valid
				|| (haveRepeat && !imageComparison::Validate(beforeRepeat, scene.fillsFrame).valid);
			if (beforeValidity.valid && !afterBroken.valid)
				failures.append("after image is not valid: " + afterBroken.reason);
			else if (!beforeValidity.valid && !afterValidity.valid)
			{
				brokenInBoth++;
				notes.append("both builds render an image that is not valid: " + afterValidity.reason);
			}
			else if (!beforeValidity.valid)
				notes.append("the before image was not valid: " + beforeValidity.reason);
			else if (!afterValidity.valid)
				notes.append("the after image is near the validity limits: " + afterValidity.reason);
		}

		QString measuresText;
		if (haveBefore && haveRepeat && haveAfter)
		{
			if (before.size() != after.size() || before.size() != beforeRepeat.size())
			{
				failures.append(QString("sizes differ: before %1x%2, after %3x%4")
						.arg(before.width())
						.arg(before.height())
						.arg(after.width())
						.arg(after.height()));
			}
			else
			{
				// the two "before" renders give this scene's noise, which sets its limits
				const sImageMeasures noise = imageComparison::Measure(before, beforeRepeat);
				const sImageLimits limits = imageComparison::Limits(noise);
				const sImageMeasures measures = imageComparison::Measure(before, after);
				QStringList measureFailures = imageComparison::Failures(measures, limits);
				// with too little structure, shapes, edges and structural similarity compare
				// noise; the color and difference measures still apply
				if (lowStructure)
				{
					measureFailures.erase(std::remove_if(measureFailures.begin(), measureFailures.end(),
																	[](const QString &failure)
																	{
																		return failure.startsWith("shapeIoU")
																					 || failure.startsWith("edgeF1")
																					 || failure.startsWith("ssim");
																	}),
						measureFailures.end());
				}
				failures.append(measureFailures);
				measuresText = imageComparison::FormatMeasures(measures, limits) + "\n";
				imageComparison::DifferenceImage(before, after)
					.save(QDir(outputFolder).filePath(scene.name + "-diff.png"), "PNG");
			}
		}

		const bool passed = failures.isEmpty();
		if (!passed) failed++;
		const QSize size = haveBefore ? before.size() : (haveAfter ? after.size() : QSize());
		report += QString("\n[%1] %2 %3x%4\n")
								.arg(passed ? "PASS" : "FAIL")
								.arg(scene.name)
								.arg(size.width())
								.arg(size.height());
		report += measuresText;
		if (!notes.isEmpty()) report += "  note: " + notes.join("; ") + "\n";
		if (!passed) report += "  failures: " + failures.join("; ") + "\n";
	}

	const int compared = scenes.size() - added;
	QString addedText = added > 0 ? QString(", %1 new and not compared").arg(added) : QString();
	if (brokenInBoth > 0)
		addedText += QString(", %1 not a valid render in either build").arg(brokenInBoth);
	if (failed == 0)
		report += QString("\nRESULT: PASS (%1 of %1 scenes%2)\n").arg(compared).arg(addedText);
	else
		report += QString("\nRESULT: FAIL (%1 of %2 scenes failed%3)\n")
								.arg(failed)
								.arg(compared)
								.arg(addedText);

	WriteTextFile(QDir(outputFolder).filePath(reportFileName), report);
	out << report;
	out.flush();
	return failed > 0 ? 1 : 0;
}
