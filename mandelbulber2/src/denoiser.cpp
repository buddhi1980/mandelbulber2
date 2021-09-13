/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2021 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: Denoising filters for Monte Carlo rendering
 */

#include "denoiser.h"

#include <algorithm>
#include <cmath>

#include "cimage.hpp"
#include "common_math.h"

using std::max;
using std::min;
using std::clamp;

cDenoiser::cDenoiser(int imageWidth, int imageHeight, enumStrength _strength)
{
	width = imageWidth;
	height = imageHeight;
	strength = _strength;

	switch (_strength)
	{
		case light:
			maxBlurRadius = 5.0;
			minBlurRadius = 0.4;
			maxMedianSize = 1.1;
			noiseMultiplier = 2500.0;
			zDepthFilterfactor = 100.0;
			normalFilterFactor = 10.0;
			break;
		case medium:
			maxBlurRadius = 10.0;
			minBlurRadius = 0.6;
			maxMedianSize = 2.0;
			noiseMultiplier = 5000.0;
			zDepthFilterfactor = 100.0;
			normalFilterFactor = 10.0;
			break;
		case strong:
			maxBlurRadius = 15.0;
			minBlurRadius = 0.7;
			maxMedianSize = 4.0;
			noiseMultiplier = 15000.0;
			zDepthFilterfactor = 10.0;
			normalFilterFactor = 3.0;
			break;
	}
}

cDenoiser::~cDenoiser()
{
	// TODO Auto-generated destructor stub
}

void cDenoiser::AllocMem()
{
	blurBuffer.resize(width * height);
	blurRadiusBuffer.resize(width * height);
}

void cDenoiser::UpdatePixel(int x, int y, const sRGBFloat color, float noise)
{
	float filterRadius = min(sqrt(noise * noiseMultiplier) + minBlurRadius, maxBlurRadius);

	blurRadiusBuffer[x + y * width] = filterRadius;
	blurBuffer[x + y * width] = color;
}

void cDenoiser::Denoise(int boxX, int boxY, int boxWidth, int boxHeight, bool preserveGeometry,
	std::shared_ptr<cImage> image, int loopCounter)
{
	// lens blur

	// Qt Concurrect is not used because this module is not available in ppa:beiner repository

#pragma omp parallel for schedule(dynamic, 1)
	for (int y = 0; y < int(boxHeight); y++)
	{
		for (int x = 0; x < int(boxWidth); x++)
		{
			size_t xx = x + boxX;
			size_t yy = y + boxY;

			float filterRadius = blurRadiusBuffer[xx + yy * width];

			float z = 0;
			CVector3 normal;
			if (preserveGeometry)
			{
				z = image->GetPixelZBuffer(xx, yy);

				sRGBFloat normalVectorRGB = image->GetPixelNormalWorld(xx, yy);
				normal = CVector3(normalVectorRGB.R, normalVectorRGB.G, normalVectorRGB.B);
			}

			// strong blur of low noise regions at first use of denoiser
			// it prevents from black spots
			if (loopCounter == 3)
			{
				sRGBFloat pixel = blurBuffer[xx + yy * width];
				float sum = pixel.R + pixel.G + pixel.B;
                filterRadius = filterRadius + clamp(0.1f / (sum * sum + 0.0000001f), 0.0f, 10.0f);
			}

			int delta = int(filterRadius + 1.0f);
			sRGBFloat averagePixel;
			float totalWeight = 0.0f;

			for (int dy = -delta; dy <= delta; dy++)
			{
				for (int dx = -delta; dx <= delta; dx++)
				{
					int fx = xx + dx;
					int fy = yy + dy;

					if (fx >= 0 && fx < width && fy >= 0 && fy < height)
					{
						float radius = sqrtf(float(dx * dx + dy * dy));

						// anti-aliased circle
						float fweight = clamp(filterRadius - radius, 0.0f, 1.0f);

						if (fweight < 0.001f) continue;

						// take samples mostly from places of high noise
						// it improves edges
						float filterRadiusForWeight = blurRadiusBuffer[fx + fy * width];
						float noiseWeight = clamp(filterRadiusForWeight / filterRadius, 0.0f, 1.0f);

						if (preserveGeometry)
						{
							// use surface normals to select samples from similar surface direction

							sRGBFloat filterNormalVectorRGB = image->GetPixelNormalWorld(fx, fy);
							CVector3 filterNormal(
								filterNormalVectorRGB.R, filterNormalVectorRGB.G, filterNormalVectorRGB.B);

							float normalDiff = (normal - filterNormal).Length();
							float normalWeight = clamp(1.0f - normalDiff * normalFilterFactor, 0.0f, 1.0f);

							noiseWeight *= normalWeight;

							// use samples form similar depth
							float z2 = image->GetPixelZBuffer(fx, fy);
							float deltaZ = fabsf((z - z2) / z);
							if (deltaZ > 0.0)
							{
								noiseWeight *= clamp(1.0f / (deltaZ * zDepthFilterfactor), 0.0f, 1.0f);
							}
						}

						// use less blur when median is used
						if (filterRadius <= maxMedianSize) noiseWeight *= 0.2f;

						if (dx == 0 && dy == 0) noiseWeight = 1.0f;

						if (loopCounter > 3)
						{
							fweight *= noiseWeight;
						}

						if (fweight > 0.0f)
						{
							sRGBFloat inputPixel = blurBuffer[fx + fy * width];

							// limit pixel brightness at first strong blur
							if (loopCounter == 3)
							{
								inputPixel.R = min(inputPixel.R, 1.5f);
								inputPixel.G = min(inputPixel.G, 1.5f);
								inputPixel.B = min(inputPixel.B, 1.5f);
							}

							averagePixel.R += inputPixel.R * fweight;
							averagePixel.G += inputPixel.G * fweight;
							averagePixel.B += inputPixel.B * fweight;
							totalWeight += fweight;
						}
					}
				}
			}

			if (totalWeight > 0.0f)
			{
				averagePixel.R /= totalWeight;
				averagePixel.G /= totalWeight;
				averagePixel.B /= totalWeight;

				image->PutPixelImage(xx, yy, averagePixel);
			}
		} // for x
	}		// for y

	// copy buffer
	for (int y = 0; y < int(boxHeight); y++)
	{
		for (int x = 0; x < int(boxWidth); x++)
		{
			size_t xx = x + boxX;
			size_t yy = y + boxY;

			blurBuffer[xx + yy * width] = image->GetPixelImage(xx, yy);
		}
	}

// median filter
#pragma omp parallel for schedule(dynamic, 1)
	for (int y = 0; y < int(boxHeight); y++)
	{
		for (int x = 0; x < int(boxWidth); x++)
		{
			size_t xx = x + boxX;
			size_t yy = y + boxY;

			float filterRadius = blurRadiusBuffer[xx + yy * width];

			float z = 0.0f;
			CVector3 normal;
			if (preserveGeometry)
			{
				z = image->GetPixelZBuffer(xx, yy);

				sRGBFloat normalVectorRGB = image->GetPixelNormalWorld(xx, yy);
				normal = CVector3(normalVectorRGB.R, normalVectorRGB.G, normalVectorRGB.B);
			}

			if (filterRadius <= maxMedianSize)
			{
				float weight = 1.0f;
				if (filterRadius < 1.0f + minBlurRadius)
				{
					weight = filterRadius - minBlurRadius;
					if (weight < 0.0f) weight = 0.0f;
					filterRadius = 1.0f;
				}

				int delta = int(filterRadius + 1.0f);

				int pixelCount = 0;

				std::vector<float> medianRInput;
				std::vector<float> medianGInput;
				std::vector<float> medianBInput;

				for (int dy = -delta; dy <= delta; dy++)
				{
					for (int dx = -delta; dx <= delta; dx++)
					{
						int fx = xx + dx;
						int fy = yy + dy;

						if (fx >= 0 && fx < width && fy >= 0 && fy < height)
						{
							float radius = sqrtf(float(dx * dx + dy * dy));

							if (radius > filterRadius) continue;

							float deltaZ = 0.0f;
							float normalWeight = 1.0f;

							if (preserveGeometry)
							{
								float z2 = image->GetPixelZBuffer(fx, fy);
								deltaZ = fabs((z - z2) / z);

								sRGBFloat filterNormalVectorRGB = image->GetPixelNormalWorld(fx, fy);
								CVector3 filterNormal(
									filterNormalVectorRGB.R, filterNormalVectorRGB.G, filterNormalVectorRGB.B);

								float normalDiff = (normal - filterNormal).Length();
								normalWeight = clamp(1.0f - normalDiff * normalFilterFactor, 0.0f, 1.0f);
							}

							if (radius <= filterRadius && normalWeight > 0.5f
									&& deltaZ < 1.0f / zDepthFilterfactor)
							{
								sRGBFloat inputPixel = blurBuffer[fx + fy * width];
								medianRInput.push_back(inputPixel.R);
								medianGInput.push_back(inputPixel.G);
								medianBInput.push_back(inputPixel.B);
								pixelCount++;
							}
						}
					}
				}

				if (pixelCount > 2)
				{
					sRGBFloat newPixel = blurBuffer[xx + yy * width];

					{
						std::vector<float> h(medianRInput.size() / 2 + 1);
						std::partial_sort_copy(medianRInput.begin(), medianRInput.end(), h.begin(), h.end());
						newPixel.R = h.back();
					}

					{
						std::vector<float> h(medianGInput.size() / 2 + 1);
						std::partial_sort_copy(medianGInput.begin(), medianGInput.end(), h.begin(), h.end());
						newPixel.G = h.back();
					}

					{
						std::vector<float> h(medianBInput.size() / 2 + 1);
						std::partial_sort_copy(medianBInput.begin(), medianBInput.end(), h.begin(), h.end());
						newPixel.B = h.back();
					}

					sRGBFloat oldPixel = blurBuffer[xx + yy * width];
					sRGBFloat newPixelMixed;

					float mixFactor = 1.0f / (loopCounter / 50.0f + 1.0f) * weight;

					newPixelMixed.R = oldPixel.R * (1.0f - mixFactor) + newPixel.R * mixFactor;
					newPixelMixed.G = oldPixel.G * (1.0f - mixFactor) + newPixel.G * mixFactor;
					newPixelMixed.B = oldPixel.B * (1.0f - mixFactor) + newPixel.B * mixFactor;

					image->PutPixelImage(xx, yy, newPixelMixed);
				}
			}
		} // for x
	}		// for y
}
