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
 * statistical and computer vision measures for comparing two renders of one scene
 */

#include "image_comparison.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace
{
constexpr int bigDiffLevel = 32;	 // a channel difference above this counts as a big difference
constexpr int diffBlockSize = 4;	 // the difference image marks blocks of this size
constexpr int diffBlockLevel = 24; // a block is marked red when its mean difference exceeds this
constexpr int ssimWindow = 8;			 // SSIM window size in pixels
constexpr int ssimStride = 4;			 // SSIM window step in pixels
constexpr double edgeLevel = 0.25; // edge threshold, as a share of the 99th percentile gradient
constexpr double minEdgeGradient = 4.0; // gradients below this are never edges (flat images)

struct sPlane
{
	int width = 0;
	int height = 0;
	std::vector<double> values;
	double at(int x, int y) const { return values[size_t(y) * size_t(width) + size_t(x)]; }
};

sPlane Luminance(const QImage &image)
{
	sPlane plane;
	plane.width = image.width();
	plane.height = image.height();
	plane.values.resize(size_t(plane.width) * size_t(plane.height));
	for (int y = 0; y < plane.height; y++)
	{
		const uchar *line = image.constScanLine(y);
		for (int x = 0; x < plane.width; x++)
		{
			const uchar *p = line + 4 * x;
			plane.values[size_t(y) * size_t(plane.width) + size_t(x)] =
				0.2126 * p[0] + 0.7152 * p[1] + 0.0722 * p[2];
		}
	}
	return plane;
}

std::vector<bool> CoverageMask(const QImage &image)
{
	std::vector<bool> mask(size_t(image.width()) * size_t(image.height()));
	for (int y = 0; y < image.height(); y++)
	{
		const uchar *line = image.constScanLine(y);
		for (int x = 0; x < image.width(); x++)
			mask[size_t(y) * size_t(image.width()) + size_t(x)] = line[4 * x + 3] > 127;
	}
	return mask;
}

// box filter: each output pixel is the mean of a 4x4 block; a partial block at the edge is dropped
QImage Downscale4(const QImage &image)
{
	const int width = std::max(1, image.width() / 4);
	const int height = std::max(1, image.height() / 4);
	QImage result(width, height, QImage::Format_RGBA8888);
	for (int y = 0; y < height; y++)
	{
		uchar *out = result.scanLine(y);
		for (int x = 0; x < width; x++)
		{
			int sum[4] = {0, 0, 0, 0};
			int count = 0;
			for (int yy = 4 * y; yy < std::min(4 * y + 4, image.height()); yy++)
			{
				const uchar *line = image.constScanLine(yy);
				for (int xx = 4 * x; xx < std::min(4 * x + 4, image.width()); xx++)
				{
					for (int c = 0; c < 4; c++)
						sum[c] += line[4 * xx + c];
					count++;
				}
			}
			for (int c = 0; c < 4; c++)
				out[4 * x + c] = uchar((sum[c] + count / 2) / std::max(count, 1));
		}
	}
	return result;
}

struct sDiffStats
{
	double meanShift = 0.0;
	double meanAbsDiff = 0.0;
	double bigDiffShare = 0.0;
	double psnr = 0.0;
};

sDiffStats DiffStats(const QImage &before, const QImage &after)
{
	sDiffStats stats;
	double sumAbs = 0.0;
	double sumSquared = 0.0;
	double sumChannel[3] = {0.0, 0.0, 0.0}; // signed, so that noise cancels out
	qint64 bigCount = 0;
	const qint64 pixels = qint64(before.width()) * before.height();
	for (int y = 0; y < before.height(); y++)
	{
		const uchar *a = before.constScanLine(y);
		const uchar *b = after.constScanLine(y);
		for (int x = 0; x < before.width(); x++)
		{
			int largest = 0;
			for (int c = 0; c < 3; c++)
			{
				const int d = std::abs(int(a[4 * x + c]) - int(b[4 * x + c]));
				sumChannel[c] += int(a[4 * x + c]) - int(b[4 * x + c]);
				sumAbs += d;
				sumSquared += double(d) * d;
				largest = std::max(largest, d);
			}
			if (largest > bigDiffLevel) bigCount++;
		}
	}
	const double samples = 3.0 * double(std::max<qint64>(pixels, 1));
	for (int c = 0; c < 3; c++)
		stats.meanShift =
			std::max(stats.meanShift, std::abs(sumChannel[c]) / double(std::max<qint64>(pixels, 1)));
	stats.meanAbsDiff = sumAbs / samples;
	stats.bigDiffShare = double(bigCount) / double(std::max<qint64>(pixels, 1));
	const double mse = sumSquared / samples;
	stats.psnr =
		mse > 0.0 ? std::min(10.0 * std::log10(255.0 * 255.0 / mse), identicalPsnr) : identicalPsnr;
	return stats;
}

double WindowSsim(const sPlane &a, const sPlane &b, int x0, int y0, int w, int h)
{
	constexpr double c1 = (0.01 * 255.0) * (0.01 * 255.0);
	constexpr double c2 = (0.03 * 255.0) * (0.03 * 255.0);
	double sumA = 0.0, sumB = 0.0, sumAA = 0.0, sumBB = 0.0, sumAB = 0.0;
	for (int y = y0; y < y0 + h; y++)
	{
		for (int x = x0; x < x0 + w; x++)
		{
			const double va = a.at(x, y);
			const double vb = b.at(x, y);
			sumA += va;
			sumB += vb;
			sumAA += va * va;
			sumBB += vb * vb;
			sumAB += va * vb;
		}
	}
	const double n = double(w) * h;
	const double meanA = sumA / n;
	const double meanB = sumB / n;
	const double varA = std::max(0.0, sumAA / n - meanA * meanA);
	const double varB = std::max(0.0, sumBB / n - meanB * meanB);
	const double covariance = sumAB / n - meanA * meanB;
	return ((2.0 * meanA * meanB + c1) * (2.0 * covariance + c2))
				 / ((meanA * meanA + meanB * meanB + c1) * (varA + varB + c2));
}

double Ssim(const QImage &before, const QImage &after)
{
	const sPlane a = Luminance(before);
	const sPlane b = Luminance(after);
	// an image smaller than one window is compared as a single window
	if (a.width < ssimWindow || a.height < ssimWindow)
		return WindowSsim(a, b, 0, 0, a.width, a.height);

	double sum = 0.0;
	int count = 0;
	for (int y = 0; y + ssimWindow <= a.height; y += ssimStride)
	{
		for (int x = 0; x + ssimWindow <= a.width; x += ssimStride)
		{
			sum += WindowSsim(a, b, x, y, ssimWindow, ssimWindow);
			count++;
		}
	}
	return sum / count;
}

sPlane SobelMagnitude(const sPlane &luma)
{
	sPlane magnitude;
	magnitude.width = luma.width;
	magnitude.height = luma.height;
	magnitude.values.assign(luma.values.size(), 0.0);
	for (int y = 1; y < luma.height - 1; y++)
	{
		for (int x = 1; x < luma.width - 1; x++)
		{
			const double gx = luma.at(x + 1, y - 1) + 2.0 * luma.at(x + 1, y) + luma.at(x + 1, y + 1)
												- luma.at(x - 1, y - 1) - 2.0 * luma.at(x - 1, y) - luma.at(x - 1, y + 1);
			const double gy = luma.at(x - 1, y + 1) + 2.0 * luma.at(x, y + 1) + luma.at(x + 1, y + 1)
												- luma.at(x - 1, y - 1) - 2.0 * luma.at(x, y - 1) - luma.at(x + 1, y - 1);
			magnitude.values[size_t(y) * size_t(luma.width) + size_t(x)] = std::sqrt(gx * gx + gy * gy);
		}
	}
	return magnitude;
}

double Percentile99(std::vector<double> values)
{
	if (values.empty()) return 0.0;
	const size_t index = std::min(values.size() - 1, size_t(0.99 * double(values.size())));
	std::nth_element(values.begin(), values.begin() + qint64(index), values.end());
	return values[index];
}

std::vector<bool> EdgeMap(const sPlane &magnitude, double threshold)
{
	std::vector<bool> edges(magnitude.values.size());
	for (size_t i = 0; i < edges.size(); i++)
		edges[i] = magnitude.values[i] >= threshold;
	return edges;
}

// share of edge pixels in "from" that have an edge in "to" within one pixel
double MatchedShare(
	const std::vector<bool> &from, const std::vector<bool> &to, int width, int height)
{
	qint64 total = 0;
	qint64 matched = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (!from[size_t(y) * size_t(width) + size_t(x)]) continue;
			total++;
			bool found = false;
			for (int yy = std::max(0, y - 1); yy <= std::min(height - 1, y + 1) && !found; yy++)
				for (int xx = std::max(0, x - 1); xx <= std::min(width - 1, x + 1) && !found; xx++)
					found = to[size_t(yy) * size_t(width) + size_t(xx)];
			if (found) matched++;
		}
	}
	return total > 0 ? double(matched) / double(total) : -1.0;
}

double EdgeF1(const QImage &before, const QImage &after)
{
	const sPlane a = SobelMagnitude(Luminance(before));
	const sPlane b = SobelMagnitude(Luminance(after));
	const double threshold =
		std::max(minEdgeGradient, edgeLevel * std::max(Percentile99(a.values), Percentile99(b.values)));
	const std::vector<bool> edgesA = EdgeMap(a, threshold);
	const std::vector<bool> edgesB = EdgeMap(b, threshold);
	const double precision = MatchedShare(edgesB, edgesA, a.width, a.height);
	const double recall = MatchedShare(edgesA, edgesB, a.width, a.height);
	if (precision < 0.0 && recall < 0.0) return 1.0;	 // neither image has edges
	if (precision <= 0.0 || recall <= 0.0) return 0.0; // only one has edges, or none match
	return 2.0 * precision * recall / (precision + recall);
}

double ShapeIoU(const QImage &before, const QImage &after)
{
	const std::vector<bool> a = CoverageMask(before);
	const std::vector<bool> b = CoverageMask(after);
	qint64 intersection = 0;
	qint64 unionCount = 0;
	for (size_t i = 0; i < a.size(); i++)
	{
		if (a[i] && b[i]) intersection++;
		if (a[i] || b[i]) unionCount++;
	}
	// Both counts get a padding of half a percent of the image, so that a shape of a few pixels,
	// whose edge pixels flicker with the sampling noise, doesn't swing the measure; for a shape of
	// any size the padding hardly matters.
	const double padding = 0.005 * double(a.size());
	return (double(intersection) + padding) / (double(unionCount) + padding);
}

QString Number(double value, int decimals)
{
	if (value >= identicalPsnr) return QStringLiteral("inf");
	return QString::number(value, 'f', decimals);
}

QString Line(const QString &name, double value, const QString &op, double threshold, int decimals)
{
	return QString("%1 %2 %3 %4")
		.arg(name, -14)
		.arg(Number(value, decimals), 6)
		.arg(op)
		.arg(Number(threshold, decimals), -7);
}
} // namespace

namespace imageComparison
{
QImage Normalized(const QImage &image)
{
	return image.convertToFormat(QImage::Format_RGBA8888);
}

sImageMeasures Measure(const QImage &before, const QImage &after)
{
	const QImage a = Normalized(before);
	const QImage b = Normalized(after);
	const QImage a4 = Downscale4(a);
	const QImage b4 = Downscale4(b);

	sImageMeasures measures;
	const sDiffStats full = DiffStats(a, b);
	const sDiffStats reduced = DiffStats(a4, b4);
	measures.pixels = double(a.width()) * double(a.height());
	measures.meanShift = full.meanShift;
	measures.meanAbsDiff = full.meanAbsDiff;
	measures.bigDiffShare = full.bigDiffShare;
	measures.psnr = full.psnr;
	measures.meanAbsDiff4x = reduced.meanAbsDiff;
	measures.psnr4x = reduced.psnr;
	measures.ssim = Ssim(a, b);
	measures.ssim4x = Ssim(a4, b4);
	measures.edgeF1 = EdgeF1(a, b);
	measures.shapeIoU = ShapeIoU(a, b);
	return measures;
}

sImageLimits Limits(const sImageMeasures &noise)
{
	using namespace regressionThresholds;
	// a difference may grow by the margin; a similarity may lose the margin times its gap to 1;
	// PSNR is a logarithm of the squared error, so the margin becomes a fixed number of dB
	const double psnrMargin = 20.0 * std::log10(noiseMargin);
	sImageLimits limits;
	// the mean shift of one pair is a poor estimate of its own noise, because it is the small
	// remainder of differences that cancel out. So its limit also follows the pixel noise,
	// which averages out less in a small image.
	const double shiftFromPixelNoise =
		noise.pixels > 0.0 ? meanShiftNoiseFactor * noise.meanAbsDiff / std::sqrt(noise.pixels) : 0.0;
	limits.maxMeanShift =
		std::max({floorMeanShift, noiseMargin * noise.meanShift, shiftFromPixelNoise});
	limits.maxMeanAbsDiff = std::max(floorMeanAbsDiff, noiseMargin * noise.meanAbsDiff);
	limits.maxMeanAbsDiff4x = std::max(floorMeanAbsDiff4x, noiseMargin * noise.meanAbsDiff4x);
	limits.maxBigDiffShare = std::max(floorBigDiffShare, noiseMargin * noise.bigDiffShare);
	limits.minPsnr = std::min(floorPsnr, noise.psnr - psnrMargin);
	limits.minPsnr4x = std::min(floorPsnr4x, noise.psnr4x - psnrMargin);
	limits.minSsim = std::min(floorSsim, 1.0 - noiseMargin * (1.0 - noise.ssim));
	limits.minSsim4x = std::min(floorSsim4x, 1.0 - noiseMargin * (1.0 - noise.ssim4x));
	limits.minEdgeF1 = std::min(floorEdgeF1, 1.0 - noiseMargin * (1.0 - noise.edgeF1));
	limits.minShapeIoU = std::min(floorShapeIoU, 1.0 - noiseMargin * (1.0 - noise.shapeIoU));
	return limits;
}

QStringList Failures(const sImageMeasures &m, const sImageLimits &limits)
{
	QStringList failures;
	auto above = [&failures](const QString &name, double value, double limit)
	{
		if (value > limit)
			failures.append(QString("%1 %2 > %3").arg(name, Number(value, 3), Number(limit, 3)));
	};
	auto below = [&failures](const QString &name, double value, double limit)
	{
		if (value < limit)
			failures.append(QString("%1 %2 < %3").arg(name, Number(value, 3), Number(limit, 3)));
	};
	above("meanShift", m.meanShift, limits.maxMeanShift);
	above("meanAbsDiff", m.meanAbsDiff, limits.maxMeanAbsDiff);
	above("meanAbsDiff4x", m.meanAbsDiff4x, limits.maxMeanAbsDiff4x);
	above("bigDiffShare", m.bigDiffShare, limits.maxBigDiffShare);
	below("psnr", m.psnr, limits.minPsnr);
	below("psnr4x", m.psnr4x, limits.minPsnr4x);
	below("ssim", m.ssim, limits.minSsim);
	below("ssim4x", m.ssim4x, limits.minSsim4x);
	below("edgeF1", m.edgeF1, limits.minEdgeF1);
	below("shapeIoU", m.shapeIoU, limits.minShapeIoU);
	return failures;
}

sImageValidity Validate(const QImage &image, bool fillsFrame, double strictness)
{
	using namespace regressionThresholds;
	sImageValidity validity;
	const QImage normalized = Normalized(image);
	if (normalized.isNull() || normalized.width() < 3 || normalized.height() < 3)
	{
		validity.reason = "image is empty or smaller than 3x3 pixels";
		return validity;
	}

	const sPlane luma = Luminance(normalized);
	double sum = 0.0, sumSquared = 0.0;
	for (double v : luma.values)
	{
		sum += v;
		sumSquared += v * v;
	}
	const double n = double(luma.values.size());
	const double mean = sum / n;
	validity.lumaStdDev = std::sqrt(std::max(0.0, sumSquared / n - mean * mean));

	const std::vector<bool> mask = CoverageMask(normalized);
	validity.coverage = double(std::count(mask.begin(), mask.end(), true)) / n;

	const sPlane magnitude = SobelMagnitude(luma);
	const double threshold = std::max(minEdgeGradient, edgeLevel * Percentile99(magnitude.values));
	const std::vector<bool> edges = EdgeMap(magnitude, threshold);
	validity.edgeDensity = double(std::count(edges.begin(), edges.end(), true)) / n;

	QStringList reasons;
	if (validity.lumaStdDev < strictness * minLumaStdDev)
		reasons.append(
			QString("flat image, luminance deviation %1").arg(Number(validity.lumaStdDev, 2)));
	if (validity.coverage <= 0.0 && !fillsFrame) reasons.append("no fractal in the image");
	if (validity.edgeDensity < strictness * minEdgeDensity)
		reasons.append(QString("no structure, edge density %1").arg(Number(validity.edgeDensity, 4)));
	validity.reason = reasons.join("; ");
	validity.valid = reasons.isEmpty();
	return validity;
}

QImage DifferenceImage(const QImage &before, const QImage &after)
{
	const QImage a = Normalized(before);
	const QImage b = Normalized(after);
	const int width = a.width();
	const int height = a.height();
	QImage result(width, height, QImage::Format_RGB888);

	// gray: the difference of every pixel, doubled
	for (int y = 0; y < height; y++)
	{
		const uchar *pa = a.constScanLine(y);
		const uchar *pb = b.constScanLine(y);
		uchar *out = result.scanLine(y);
		for (int x = 0; x < width; x++)
		{
			int largest = 0;
			for (int c = 0; c < 3; c++)
				largest = std::max(largest, std::abs(int(pa[4 * x + c]) - int(pb[4 * x + c])));
			const uchar gray = uchar(std::min(255, largest * 2));
			out[3 * x + 0] = gray;
			out[3 * x + 1] = gray;
			out[3 * x + 2] = gray;
		}
	}

	// red: blocks whose mean difference is large. Sampling noise cancels out within a block,
	// so red marks where the image really changed, also in a noisy Monte Carlo render.
	for (int y0 = 0; y0 < height; y0 += diffBlockSize)
	{
		for (int x0 = 0; x0 < width; x0 += diffBlockSize)
		{
			int sum[3] = {0, 0, 0};
			int count = 0;
			for (int y = y0; y < std::min(y0 + diffBlockSize, height); y++)
			{
				const uchar *pa = a.constScanLine(y);
				const uchar *pb = b.constScanLine(y);
				for (int x = x0; x < std::min(x0 + diffBlockSize, width); x++)
				{
					for (int c = 0; c < 3; c++)
						sum[c] += int(pa[4 * x + c]) - int(pb[4 * x + c]);
					count++;
				}
			}
			int largest = 0;
			for (int c = 0; c < 3; c++)
				largest = std::max(largest, std::abs(sum[c]) / std::max(count, 1));
			if (largest <= diffBlockLevel) continue;

			for (int y = y0; y < std::min(y0 + diffBlockSize, height); y++)
			{
				uchar *out = result.scanLine(y);
				for (int x = x0; x < std::min(x0 + diffBlockSize, width); x++)
				{
					out[3 * x + 0] = 255;
					out[3 * x + 1] = 0;
					out[3 * x + 2] = 0;
				}
			}
		}
	}
	return result;
}

QString FormatMeasures(const sImageMeasures &m, const sImageLimits &limits)
{
	QStringList lines;
	lines.append("  " + Line("meanShift", m.meanShift, "<=", limits.maxMeanShift, 3) + "  "
							 + Line("bigDiffShare", m.bigDiffShare, "<=", limits.maxBigDiffShare, 4));
	lines.append("  " + Line("meanAbsDiff", m.meanAbsDiff, "<=", limits.maxMeanAbsDiff, 3) + "  "
							 + Line("meanAbsDiff4x", m.meanAbsDiff4x, "<=", limits.maxMeanAbsDiff4x, 3));
	lines.append("  " + Line("psnr", m.psnr, ">=", limits.minPsnr, 1) + "  "
							 + Line("psnr4x", m.psnr4x, ">=", limits.minPsnr4x, 1));
	lines.append("  " + Line("ssim", m.ssim, ">=", limits.minSsim, 3) + "  "
							 + Line("ssim4x", m.ssim4x, ">=", limits.minSsim4x, 3));
	lines.append("  " + Line("edgeF1", m.edgeF1, ">=", limits.minEdgeF1, 3) + "  "
							 + Line("shapeIoU", m.shapeIoU, ">=", limits.minShapeIoU, 3));
	return lines.join("\n");
}
} // namespace imageComparison
