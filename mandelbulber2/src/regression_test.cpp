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

#include <memory>

#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QSysInfo>
#include <QTextStream>

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

sRegressionScene Scene(const QString &name, const QString &fileName, bool fillsFrame = false,
	const QMap<QString, double> &overrides = QMap<QString, double>())
{
	sRegressionScene scene;
	scene.name = name;
	scene.fileName = fileName;
	scene.fillsFrame = fillsFrame;
	scene.overrides = overrides;
	return scene;
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

QList<sRegressionScene> cRegressionTest::Scenes()
{
	// Scenes come from the bundled examples and cover the main rendering features. Monte Carlo
	// scenes render with fewer samples, like the example test does, to keep the set fast.
	const QMap<QString, double> fastMonteCarlo = {{"DOF_min_samples", 3}, {"DOF_max_noise", 10.0}};

	QList<sRegressionScene> scenes;
	scenes.append(Scene("mandelbulb001", "mandelbulb001.fract"));
	scenes.append(Scene("mandelbox001", "mandelbox001.fract"));
	scenes.append(Scene("mandelbox003", "mandelbox003.fract"));						// fake lights
	scenes.append(Scene("IFS_001", "IFS_001.fract"));											// auxiliary lights, AO
	scenes.append(Scene("IFS_002", "IFS_002.fract"));											// depth of field, basic fog
	scenes.append(Scene("Makin3D-Julia_001", "Makin3D-Julia_001.fract")); // AO, volumetric fog
	scenes.append(Scene("aboxmod1_001", "aboxmod1_001.fract"));						// DOF, AO, volumetric fog
	scenes.append(Scene("boolean001", "boolean001.fract"));								// boolean operators
	scenes.append(Scene("amazing_surf_mod1_001", "amazing surf mod1 001.fract")); // primitives
	scenes.append(Scene("ides2_001", "ides2_001.fract"));													// iteration fog
	scenes.append(Scene("hex_grid_001", "hex grid 001.fract", false, fastMonteCarlo)); // hybrid
	scenes.append(Scene("difs_tree", "difs_tree.fract", false, fastMonteCarlo)); // hybrid, MC DOF
	scenes.append(
		Scene("mandelnest_pow_5", "mandelnest pow 5.fract", false, fastMonteCarlo)); // MC GI
	return scenes;
}

bool cRegressionTest::RenderScene(const sRegressionScene &scene, QImage *result, QString *error)
{
	const QString fileName =
		QDir::toNativeSeparators(systemDirectories.sharedDir + QDir::separator() + "examples"
														 + QDir::separator() + scene.fileName);
	if (!QFileInfo::exists(fileName))
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

	cSettings parSettings(cSettings::formatFullText);
	parSettings.BeQuiet(true);
	if (!parSettings.LoadFromFile(fileName)
			|| !parSettings.Decode(par, parFractal, animFrames, keyframes))
	{
		*error = QString("scene file cannot be loaded: %1").arg(fileName);
		return false;
	}

	par->Set("image_width", scene.width);
	par->Set("image_height", scene.height);
	par->Set("opencl_enabled", false);
	par->Set("opencl_mode", 0);
	for (auto it = scene.overrides.constBegin(); it != scene.overrides.constEnd(); ++it)
		par->Set(it.key(), it.value());

	bool stopRequest = false;
	std::shared_ptr<cImage> image(new cImage(scene.width, scene.height));
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

	if (int(image->GetWidth()) != scene.width || int(image->GetHeight()) != scene.height)
	{
		*error = QString("rendered %1x%2 instead of %3x%4")
							 .arg(image->GetWidth())
							 .arg(image->GetHeight())
							 .arg(scene.width)
							 .arg(scene.height);
		return false;
	}

	image->ConvertTo8bitChar();
	QImage rendered(scene.width, scene.height, QImage::Format_RGBA8888);
	for (int y = 0; y < scene.height; y++)
	{
		uchar *line = rendered.scanLine(y);
		for (int x = 0; x < scene.width; x++)
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

	int failed = 0;
	const QList<sRegressionScene> scenes = Scenes();
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
								.arg(scene.width)
								.arg(scene.height)
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
	const QList<sRegressionScene> scenes = Scenes();
	for (const sRegressionScene &scene : scenes)
	{
		if (haveBeforeInfo && !beforeScenes.contains(scene.name))
		{
			added++;
			report += QString("\n[NEW ] %1 %2x%3\n").arg(scene.name).arg(scene.width).arg(scene.height);
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

		// a render that is broken in both builds must not pass just because the two agree
		if (haveBefore)
		{
			const sImageValidity validity = imageComparison::Validate(before, scene.fillsFrame);
			if (!validity.valid) failures.append("before image is not valid: " + validity.reason);
		}
		if (haveAfter)
		{
			const sImageValidity validity = imageComparison::Validate(after, scene.fillsFrame);
			if (!validity.valid) failures.append("after image is not valid: " + validity.reason);
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
				failures.append(imageComparison::Failures(measures, limits));
				measuresText = imageComparison::FormatMeasures(measures, limits) + "\n";
				imageComparison::DifferenceImage(before, after)
					.save(QDir(outputFolder).filePath(scene.name + "-diff.png"), "PNG");
			}
		}

		const bool passed = failures.isEmpty();
		if (!passed) failed++;
		report += QString("\n[%1] %2 %3x%4\n")
								.arg(passed ? "PASS" : "FAIL")
								.arg(scene.name)
								.arg(scene.width)
								.arg(scene.height);
		report += measuresText;
		if (!passed) report += "  failures: " + failures.join("; ") + "\n";
	}

	const int compared = scenes.size() - added;
	const QString addedText = added > 0 ? QString(", %1 new and not compared").arg(added) : QString();
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
