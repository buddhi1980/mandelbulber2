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

#ifndef MANDELBULBER2_SRC_IMAGE_COMPARISON_HPP_
#define MANDELBULBER2_SRC_IMAGE_COMPARISON_HPP_

#include <QImage>
#include <QString>

// Measures of one pair of renders. Images are 8-bit RGBA: RGB is the final image, A is the alpha
// buffer, and alpha above 50% means the fractal covers the pixel. "4x" measures are taken on both
// images downscaled 4x with a box filter, which averages Monte Carlo sampling noise away.
struct sImageMeasures
{
	double pixels = 0.0;				// number of pixels compared
	double meanShift = 0.0;			// largest difference between the two images' mean R, G or B
	double meanAbsDiff = 0.0;		// mean absolute channel difference, 0..255
	double meanAbsDiff4x = 0.0; // the same, 4x downscaled
	double bigDiffShare = 0.0;	// share of pixels whose largest channel difference exceeds 32
	double psnr = 0.0;					// dB, infinity when the images are identical
	double psnr4x = 0.0;				// dB, 4x downscaled
	double ssim = 1.0;					// mean structural similarity of luminance
	double ssim4x = 1.0;				// the same, 4x downscaled
	double edgeF1 = 1.0;				// F1 score of Sobel edges, matching within one pixel
	double shapeIoU = 1.0;			// intersection over union of the coverage masks
};

// Checks of one image on its own, used when no other build's render exists
struct sImageValidity
{
	double lumaStdDev = 0.0;	// standard deviation of luminance, 0..255
	double coverage = 0.0;		// share of pixels the fractal covers
	double edgeDensity = 0.0; // share of pixels on a Sobel edge
	bool valid = false;
	QString reason;
};

// Limits. Renders are not reproducible: random sampling makes two renders of one scene by one
// build differ, by a little in most scenes and by a lot in Monte Carlo scenes. So every scene is
// rendered twice by the "before" build, and the measures of that pair are the scene's noise.
// The "after" render passes when no measure is worse than its limit:
//   limit = the noise, made worse by noiseMargin, but never stricter than the floor
// The floors keep a scene with almost no noise from failing on a difference nobody can see.
namespace regressionThresholds
{
// Measured over 30 ordered pairs of 6 renders of 13 scenes, with GCC on Linux and with MSVC on
// Windows, renders of unchanged code reached 1.56 times the noise of one pair. Over all ~1200
// scenes on four CI systems, a few of the roughly 10000 comparisons in a run went past twice the
// noise, so the margin is 3. A 5% change of light intensity still exceeds the limits.
constexpr double noiseMargin = 3.0;

// Floors: the least strict limit a measure can get. Each is about twice the largest noise
// measured in the scenes that have no Monte Carlo sampling.
constexpr double floorMeanShift = 0.5; // of 255; noise there is below 0.3

// The mean shift between two renders of unchanged code is the mean of their pixel noise, so it
// shrinks with the square root of the number of pixels. Its limit is never stricter than
// meanShiftNoiseFactor * meanAbsDiff / sqrt(pixels). The largest factor measured was 10.3,
// for a Monte Carlo scene at 160x90 pixels on a CI runner.
constexpr double meanShiftNoiseFactor = 20.0;
constexpr double floorMeanAbsDiff = 1.0;		// of 255
constexpr double floorMeanAbsDiff4x = 0.5;	// of 255
constexpr double floorBigDiffShare = 0.005; // half a percent of the pixels
constexpr double floorPsnr = 38.0;					// dB; a limit is never above this (noise: 40 dB)
constexpr double floorPsnr4x = 46.0;				// dB (noise: 51 dB)
constexpr double floorSsim = 0.99;					// a limit is never above this
constexpr double floorSsim4x = 0.995;
constexpr double floorEdgeF1 = 0.97;
constexpr double floorShapeIoU = 0.99;

// validity of a single image
constexpr double minLumaStdDev = 2.0;		 // of 255; a flat image is below this
constexpr double minEdgeDensity = 0.001; // a render without any structure is below this
} // namespace regressionThresholds

// the limits of one scene, derived from its noise
struct sImageLimits
{
	double maxMeanShift = 0.0;
	double maxMeanAbsDiff = 0.0;
	double maxMeanAbsDiff4x = 0.0;
	double maxBigDiffShare = 0.0;
	double minPsnr = 0.0;
	double minPsnr4x = 0.0;
	double minSsim = 0.0;
	double minSsim4x = 0.0;
	double minEdgeF1 = 0.0;
	double minShapeIoU = 0.0;
};

namespace imageComparison
{
// converts any image to the 8-bit RGBA format the measures use
QImage Normalized(const QImage &image);

// all measures of one pair; both images must have the same size
sImageMeasures Measure(const QImage &before, const QImage &after);

// the limits for a scene whose two "before" renders differ by the given measures
sImageLimits Limits(const sImageMeasures &noise);

// names every measure outside its limit, such as "ssim4x 0.912 < 0.950"; empty when all pass
QStringList Failures(const sImageMeasures &measures, const sImageLimits &limits);

// checks one image on its own; fillsFrame allows a render with no background
sImageValidity Validate(const QImage &image, bool fillsFrame);

// gray shows how much every pixel differs; red marks the 4x4 blocks whose mean difference is
// large, which is where the image really changed and not only its sampling noise
QImage DifferenceImage(const QImage &before, const QImage &after);

// the measures as report lines with their limits
QString FormatMeasures(const sImageMeasures &measures, const sImageLimits &limits);
} // namespace imageComparison

#endif /* MANDELBULBER2_SRC_IMAGE_COMPARISON_HPP_ */
