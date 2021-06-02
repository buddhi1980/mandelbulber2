/*
 * denoiser.h
 *
 *  Created on: 31 maj 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_DENOISER_H_
#define MANDELBULBER2_SRC_DENOISER_H_

#include <memory>
#include <vector>

#include "color_structures.hpp"

class cImage;

class cDenoiser
{
public:
	enum enumStrength
	{
		light = 0,
		medium = 1,
		strong = 2
	};

public:
	cDenoiser(int _imageWidth, int _imageHeight, enumStrength _strength);
	~cDenoiser();

public:
	void AllocMem();
	void UpdatePixel(int x, int y, const sRGBFloat color, float noise);
	void Denoise(int boxX, int boxY, int boxWidth, int boxHeight, bool preserveGeometry,
		std::shared_ptr<cImage> image, int loopCounter);

private:
	std::vector<sRGBFloat> blurBuffer;
	std::vector<float> blurRadiusBuffer;

	int width = 0;
	int height = 0;

	enumStrength strength;

	float maxBlurRadius;
	float minBlurRadius;
	int maxMedianSize;
	float noiseMultiplier;
};

#endif /* MANDELBULBER2_SRC_DENOISER_H_ */
