/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cPositionRandom - random values that depend only on a seed and a position
 *
 * Every value comes from the seed, the frame, the eye, and the value's position in the render,
 * using only 64-bit integer arithmetic. Nothing else feeds the state, so thread order can't change
 * a value, and one seed and position give the same value on every build.
 *
 * The algorithm and its known answers are in specs/002-deterministic-rendering/contracts/random-values.md
 */

#ifndef MANDELBULBER2_SRC_POSITION_RANDOM_HPP_
#define MANDELBULBER2_SRC_POSITION_RANDOM_HPP_

#include <cstdint>

class cPositionRandom
{
public:
	// which part of the program draws the values
	enum enumStage
	{
		stageRender = 1,
		stageSSAO = 2,
		stageDOF = 3,
		stageOutsideRender = 4
	};

	cPositionRandom() : key(0), state(0) {}

	cPositionRandom(int seed, int frame, int eye, int stage) : key(0), state(0)
	{
		key = Absorb(key, stage);
		key = Absorb(key, seed);
		key = Absorb(key, frame);
		key = Absorb(key, eye);
	}

	// restarts the sequence for one position
	void SetPosition(int a, int b, int c)
	{
		state = Absorb(Absorb(Absorb(key, a), b), c);
	}

	// returns the next value of the sequence, from 0 to max
	int Random(int max)
	{
		state += golden;
		return int((Mix(state) >> 32) % (std::uint64_t(max) + 1));
	}

private:
	static constexpr std::uint64_t golden = 0x9E3779B97F4A7C15ULL;

	static std::uint64_t Mix(std::uint64_t z)
	{
		z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
		z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
		return z ^ (z >> 31);
	}

	static std::uint64_t Absorb(std::uint64_t hash, int value)
	{
		return Mix(hash + std::uint64_t(std::uint32_t(value)) + golden);
	}

	std::uint64_t key;
	std::uint64_t state;
};

#endif /* MANDELBULBER2_SRC_POSITION_RANDOM_HPP_ */
