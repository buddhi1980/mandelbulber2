/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Mandelbulber Team
 *
 * WriteVerificationExport - writes the engine's buffers and exact statistics for regression tests
 */

#include "verification_export.hpp"

#include <vector>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QStringList>

#ifdef __GLIBC__
#include <gnu/libc-version.h>
#endif

#include "cimage.hpp"
#include "color_structures.hpp"
#include "statistics.h"

namespace
{
static_assert(sizeof(sRGBFloat) == 12, "float buffers need three packed floats per pixel");
static_assert(sizeof(sRGB16) == 6, "the final image needs three packed 16-bit samples per pixel");

QString CompilerIdentity()
{
#if defined(__clang__)
	return QString("clang ") + __clang_version__;
#elif defined(__GNUC__)
	return QString("gcc ") + __VERSION__;
#elif defined(_MSC_FULL_VER)
	return QString("msvc ") + QString::number(_MSC_FULL_VER);
#else
	return QString("unknown");
#endif
}

QString MathLibraryIdentity()
{
#ifdef __GLIBC__
	return QString("glibc ") + gnu_get_libc_version();
#else
	return QString("unknown");
#endif
}

bool WriteBytes(const QString &path, const char *data, qint64 size)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		qCritical() << "verification export: cannot open" << path;
		return false;
	}
	if (file.write(data, size) != size)
	{
		qCritical() << "verification export: cannot write" << path;
		return false;
	}
	return true;
}

template <typename T>
bool WriteSamples(const QString &path, const std::vector<T> &samples, quint64 expectedCount)
{
	if (samples.size() != expectedCount)
	{
		qCritical() << "verification export: wrong sample count for" << path;
		return false;
	}
	return WriteBytes(path, reinterpret_cast<const char *>(samples.data()),
		static_cast<qint64>(samples.size() * sizeof(T)));
}

template <typename GetPixel>
std::vector<sRGBFloat> CollectPixels(quint64 width, quint64 height, GetPixel getPixel)
{
	std::vector<sRGBFloat> pixels;
	pixels.reserve(width * height);
	for (quint64 y = 0; y < height; y++)
	{
		for (quint64 x = 0; x < width; x++)
		{
			pixels.push_back(getPixel(x, y));
		}
	}
	return pixels;
}

void AppendHistogram(QString &text, const QString &name, const cHistogram &histogram)
{
	const int size = histogram.GetSize();
	text += name + ".size " + QString::number(size) + "\n";
	// bucket "size" is the overflow bucket
	for (int i = 0; i <= size; i++)
	{
		text += name + "." + QString::number(i) + " " + QString::number(histogram.GetHist(i)) + "\n";
	}
}

bool WriteStatistics(const QString &path, const cStatistics &statistics)
{
	QString text;
	text += "total_iterations " + QString::number(statistics.totalNumberOfIterations) + "\n";
	text += "raymarch_count " + QString::number(statistics.numberOfRaymarchings) + "\n";
	text += "missed_de " + QString::number(statistics.missedDE) + "\n";
	AppendHistogram(text, "iteration_histogram", statistics.histogramIterations);
	AppendHistogram(text, "step_histogram", statistics.histogramStepCount);

	const QByteArray utf8 = text.toUtf8();
	return WriteBytes(path, utf8.constData(), utf8.size());
}

bool WriteManifest(const QString &path, quint64 width, quint64 height, const QStringList &buffers)
{
	QString text;
	text += "mandelbulber-verification-export 1\n";
	text += "width " + QString::number(width) + "\n";
	text += "height " + QString::number(height) + "\n";
	text += "compiler " + CompilerIdentity() + "\n";
	text += "math_library " + MathLibraryIdentity() + "\n";
	for (const QString &buffer : buffers)
	{
		text += "buffer " + buffer + "\n";
	}
	text += "statistics statistics.txt\n";
	text += "end\n";

	QSaveFile file(path);
	if (!file.open(QIODevice::WriteOnly))
	{
		qCritical() << "verification export: cannot open" << path;
		return false;
	}
	const QByteArray utf8 = text.toUtf8();
	if (file.write(utf8) != utf8.size() || !file.commit())
	{
		qCritical() << "verification export: cannot write" << path;
		return false;
	}
	return true;
}
} // namespace

bool WriteVerificationExport(
	const QString &directory, const std::shared_ptr<cImage> &image, const cStatistics &statistics)
{
	if (!QDir().mkpath(directory))
	{
		qCritical() << "verification export: cannot create" << directory;
		return false;
	}

	const QDir exportDir(directory);
	const QString manifestPath = exportDir.filePath("manifest.txt");
	if (QFile::exists(manifestPath) && !QFile::remove(manifestPath))
	{
		qCritical() << "verification export: cannot remove" << manifestPath;
		return false;
	}

	const quint64 width = image->GetWidth();
	const quint64 height = image->GetHeight();
	const quint64 pixels = width * height;
	QStringList buffers;

	if (!WriteSamples(exportDir.filePath("raw_render.bin"), image->GetImageFloat(), pixels))
		return false;
	buffers << "raw_render f32 3";

	if (!WriteSamples(exportDir.filePath("post_effect.bin"), image->GetPostImageFloat(), pixels))
		return false;
	buffers << "post_effect f32 3";

	if (!WriteSamples(exportDir.filePath("final_image.bin"), image->GetImage16(), pixels))
		return false;
	buffers << "final_image u16 3";

	if (!WriteSamples(exportDir.filePath("alpha.bin"), image->GetAlphaBuf(), pixels)) return false;
	buffers << "alpha u16 1";

	if (!WriteSamples(exportDir.filePath("depth.bin"), image->GetZBuffer(), pixels)) return false;
	buffers << "depth f32 1";

	if (image->GetImageOptional()->optionalNormal)
	{
		const std::vector<sRGBFloat> normal = CollectPixels(
			width, height, [&image](quint64 x, quint64 y) { return image->GetPixelNormal(x, y); });
		if (!WriteSamples(exportDir.filePath("normal.bin"), normal, pixels)) return false;
		buffers << "normal f32 3";
	}

	if (image->GetImageOptional()->optionalNormalWorld)
	{
		const std::vector<sRGBFloat> normalWorld = CollectPixels(
			width, height, [&image](quint64 x, quint64 y) { return image->GetPixelNormalWorld(x, y); });
		if (!WriteSamples(exportDir.filePath("normal_world.bin"), normalWorld, pixels)) return false;
		buffers << "normal_world f32 3";
	}

	if (!WriteStatistics(exportDir.filePath("statistics.txt"), statistics)) return false;

	return WriteManifest(manifestPath, width, height, buffers);
}
