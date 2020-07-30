/*
 * perlin_noise_octaves.cpp
 *
 *  Created on: 30 lip 2020
 *      Author: krzysztof
 */

// Code based on https://github.com/Reputeless/PerlinNoise/blob/master/PerlinNoise.hpp
//----------------------------------------------------------------------------------------
//
//	siv::PerlinNoise
//	Perlin noise library for modern C++
//
//	Copyright (C) 2013-2020 Ryo Suzuki <reputeless@gmail.com>
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.
//
//----------------------------------------------------------------------------------------

#include "perlin_noise_octaves.h"

#include <cstdint>
#include <algorithm>
#include <array>
#include <iterator>
#include <numeric>
#include <random>
#include <type_traits>
#include <QtGlobal>

cPerlinNoiseOctaves::cPerlinNoiseOctaves(std::uint32_t seed)
{
	reseed(seed);
}

void cPerlinNoiseOctaves::reseed(std::uint32_t seed)
{
	for (size_t i = 0; i < 256; ++i)
	{
		p[i] = static_cast<std::uint8_t>(i);
	}

	std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

	for (size_t i = 0; i < 256; ++i)
	{
		p[256 + i] = p[i];
	}
}

///////////////////////////////////////
//
//	Noise [-1, 1]
//

double cPerlinNoiseOctaves::noise1D(double x) const
{
	return noise3D(x, 0, 0);
}

double cPerlinNoiseOctaves::noise2D(double x, double y) const
{
	return noise3D(x, y, 0);
}

double cPerlinNoiseOctaves::noise3D(double x, double y, double z) const
{
	const std::int32_t X = static_cast<std::int32_t>(std::floor(x)) & 255;
	const std::int32_t Y = static_cast<std::int32_t>(std::floor(y)) & 255;
	const std::int32_t Z = static_cast<std::int32_t>(std::floor(z)) & 255;

	x -= std::floor(x);
	y -= std::floor(y);
	z -= std::floor(z);

	const double u = Fade(x);
	const double v = Fade(y);
	const double w = Fade(z);

	const std::int32_t A = p[X] + Y;
	const std::int32_t AA = p[A] + Z;
	const std::int32_t AB = p[A + 1] + Z;
	const std::int32_t B = p[X + 1] + Y;
	const std::int32_t BA = p[B] + Z;
	const std::int32_t BB = p[B + 1] + Z;

	return Lerp(w,
		Lerp(v, Lerp(u, Grad(p[AA], x, y, z), Grad(p[BA], x - 1, y, z)),
			Lerp(u, Grad(p[AB], x, y - 1, z), Grad(p[BB], x - 1, y - 1, z))),
		Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1), Grad(p[BA + 1], x - 1, y, z - 1)),
			Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1), Grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

///////////////////////////////////////
//
//	Noise [0, 1]
//

double cPerlinNoiseOctaves::noise1D_0_1(double x) const
{
	return noise1D(x) * double(0.5) + double(0.5);
}

double cPerlinNoiseOctaves::noise2D_0_1(double x, double y) const
{
	return noise2D(x, y) * double(0.5) + double(0.5);
}

double cPerlinNoiseOctaves::noise3D_0_1(double x, double y, double z) const
{
	return noise3D(x, y, z) * double(0.5) + double(0.5);
}

///////////////////////////////////////
//
//	Accumulated octave noise
//	* Return value can be outside the range [-1, 1]
//

double cPerlinNoiseOctaves::accumulatedOctaveNoise1D(double x, std::int32_t octaves) const
{
	double result = 0;
	double amp = 1;

	for (std::int32_t i = 0; i < octaves; ++i)
	{
		result += noise1D(x) * amp;
		x *= 2;
		amp /= 2;
	}

	return result; // unnormalized
}

double cPerlinNoiseOctaves::accumulatedOctaveNoise2D(double x, double y, std::int32_t octaves) const
{
	double result = 0;
	double amp = 1;

	for (std::int32_t i = 0; i < octaves; ++i)
	{
		result += noise2D(x, y) * amp;
		x *= 2;
		y *= 2;
		amp /= 2;
	}

	return result; // unnormalized
}

double cPerlinNoiseOctaves::accumulatedOctaveNoise3D(
	double x, double y, double z, std::int32_t octaves) const
{
	double result = 0;
	double amp = 1;

	for (std::int32_t i = 0; i < octaves; ++i)
	{
		result += noise3D(x, y, z) * amp;
		x *= 2;
		y *= 2;
		z *= 2;
		amp /= 2;
	}

	return result; // unnormalized
}

///////////////////////////////////////
//
//	Normalized octave noise [-1, 1]
//

double cPerlinNoiseOctaves::normalizedOctaveNoise1D(double x, std::int32_t octaves) const
{
	return accumulatedOctaveNoise1D(x, octaves) / Weight(octaves);
}

double cPerlinNoiseOctaves::normalizedOctaveNoise2D(double x, double y, std::int32_t octaves) const
{
	return accumulatedOctaveNoise2D(x, y, octaves) / Weight(octaves);
}

double cPerlinNoiseOctaves::normalizedOctaveNoise3D(
	double x, double y, double z, std::int32_t octaves) const
{
	return accumulatedOctaveNoise3D(x, y, z, octaves) / Weight(octaves);
}

///////////////////////////////////////
//
//	Accumulated octave noise clamped within the range [0, 1]
//

double cPerlinNoiseOctaves::accumulatedOctaveNoise1D_0_1(double x, std::int32_t octaves) const
{
	return qBound(0.0, accumulatedOctaveNoise1D(x, octaves) * double(0.5) + double(0.5), 1.0);
}

double cPerlinNoiseOctaves::accumulatedOctaveNoise2D_0_1(
	double x, double y, std::int32_t octaves) const
{
	return qBound(0.0, accumulatedOctaveNoise2D(x, y, octaves) * double(0.5) + double(0.5), 1.0);
}

double cPerlinNoiseOctaves::accumulatedOctaveNoise3D_0_1(
	double x, double y, double z, std::int32_t octaves) const
{
	return qBound(0.0, accumulatedOctaveNoise3D(x, y, z, octaves) * double(0.5) + double(0.5), 1.0);
}

///////////////////////////////////////
//
//	Normalized octave noise [0, 1]
//
double cPerlinNoiseOctaves::normalizedOctaveNoise1D_0_1(double x, std::int32_t octaves) const

{
	return normalizedOctaveNoise1D(x, octaves) * double(0.5) + double(0.5);
}

double cPerlinNoiseOctaves::normalizedOctaveNoise2D_0_1(
	double x, double y, std::int32_t octaves) const
{
	return normalizedOctaveNoise2D(x, y, octaves) * double(0.5) + double(0.5);
}

double cPerlinNoiseOctaves::normalizedOctaveNoise3D_0_1(
	double x, double y, double z, std::int32_t octaves) const
{
	return normalizedOctaveNoise3D(x, y, z, octaves) * double(0.5) + double(0.5);
}

///////////////////////////////////////
//
//	Serialization
//
void cPerlinNoiseOctaves::serialize(std::array<std::uint8_t, 256> &s) const
{
	for (std::size_t i = 0; i < 256; ++i)
	{
		s[i] = p[i];
	}
}

void cPerlinNoiseOctaves::deserialize(const std::array<std::uint8_t, 256> &s)
{
	for (std::size_t i = 0; i < 256; ++i)
	{
		p[256 + i] = p[i] = s[i];
	}
}
