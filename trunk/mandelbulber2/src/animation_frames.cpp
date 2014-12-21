/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Class to store settings for animation frames
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "animation_frames.hpp"

cAnimationFrames::cAnimationFrames()
{
	list.clear();
}

void cAnimationFrames::AddFrame(const cParameterContainer &params, const cFractalContainer &fractal)
{
	sAnimationFrame frame;
	frame.fract = fractal;
	frame.par = params;
	list.append(frame);
}

bool cAnimationFrames::GetFrame(int index, cParameterContainer *params, cFractalContainer *fractal)
{
	int count = list.count();
	if(index < count)
	{
		*params = list.at(index).par;
		*fractal = list.at(index).fract;
		return true;
	}
	else
	{
		return false;
	}
}

int cAnimationFrames::GetNumberOfFrames()
{
	return list.count();
}

void cAnimationFrames::Clear()
{
	cAnimationFrames();
}


