/*
 * denoiser.cpp
 *
 *  Created on: 31 maj 2021
 *      Author: krzysztof
 */

#include "denoiser.h"

#include <cmath>

#include "common_math.h"
#include "cimage.hpp"

using std::max;
using std::min;

#include <algorithm>
cDenoiser::cDenoiser(int imageWidth, int imageHeight, enumStrength _strength)
{
	width = imageWidth;
	height = imageHeight;
	strength = _strength;

	switch (_strength)
	{
		case light:
			maxBlurRadius = 5.0;
			minBlurRadius = 0.5;
			maxMedianSize = 1.5;
			noiseMultiplier = 2500.0;
			break;
		case medium:
			maxBlurRadius = 10.0;
			minBlurRadius = 0.6;
			maxMedianSize = 2;
			noiseMultiplier = 5000.0;
			break;
		case strong:
			maxBlurRadius = 15.0;
			minBlurRadius = 0.7;
			maxMedianSize = 4;
			noiseMultiplier = 15000.0;
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

void cDenoiser::Denoise(
	int boxX, int boxY, int boxWidth, int boxHeight, std::shared_ptr<cImage> image, int loopCounter)
{

#pragma omp parallel for schedule(dynamic, 1)

	for (int x = 0; x < int(boxWidth); x++)
	{
		for (int y = 0; y < int(boxHeight); y++)
		{
			size_t xx = x + boxX;
			size_t yy = y + boxY;

			float filterRadius = blurRadiusBuffer[xx + yy * width];
			int delta = int(filterRadius + 1.0);
			sRGBFloat averagePixel;
			double totalWeight = 0.0;

			for (int dy = -delta; dy <= delta; dy++)
			{
				for (int dx = -delta; dx <= delta; dx++)
				{
					int fx = xx + dx;
					int fy = yy + dy;

					if (fx >= 0 && fx < width && fy >= 0 && fy < height)
					{
						double radius = sqrt(double(dx * dx + dy * dy));
						double fweight = clamp(filterRadius - radius, 0.0, 1.0);

						float filterRadiusForWeight = blurRadiusBuffer[fx + fy * width];
						float noiseWeight = clamp(filterRadiusForWeight / filterRadius, 0.0f, 1.0f);
						fweight *= noiseWeight;

						if (fweight > 0.0)
						{
							sRGBFloat inputPixel = blurBuffer[fx + fy * width];
							averagePixel.R += inputPixel.R * fweight;
							averagePixel.G += inputPixel.G * fweight;
							averagePixel.B += inputPixel.B * fweight;
							totalWeight += fweight;
						}
					}
				}
			}

			if (totalWeight > 0.0)
			{
				averagePixel.R /= totalWeight;
				averagePixel.G /= totalWeight;
				averagePixel.B /= totalWeight;

				image->PutPixelImage(xx, yy, averagePixel);
			}
		}
	}

	// copy buffer
	for (int x = 0; x < int(boxWidth); x++)
	{
		for (int y = 0; y < int(boxHeight); y++)
		{
			size_t xx = x + boxX;
			size_t yy = y + boxY;

			blurBuffer[xx + yy * width] = image->GetPixelImage(xx, yy);
		}
	}

#pragma omp parallel for schedule(dynamic, 1)

	for (int x = 0; x < int(boxWidth); x++)
	{
		for (int y = 0; y < int(boxHeight); y++)
		{
			size_t xx = x + boxX;
			size_t yy = y + boxY;

			float filterRadius = blurRadiusBuffer[xx + yy * width];

			if (filterRadius <= maxMedianSize)
			{
				int delta = int(filterRadius + 1.0);

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
							double radius = sqrt(double(dx * dx + dy * dy));
							if (radius <= filterRadius)
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

					float mixFactor = 1.0f / (loopCounter / 50.0 + 1.0);

					newPixelMixed.R = oldPixel.R * (1.0f - mixFactor) + newPixel.R * mixFactor;
					newPixelMixed.G = oldPixel.G * (1.0f - mixFactor) + newPixel.G * mixFactor;
					newPixelMixed.B = oldPixel.B * (1.0f - mixFactor) + newPixel.B * mixFactor;

					image->PutPixelImage(xx, yy, newPixelMixed);
				}
			}

		} // for y
	}		// for x
}
