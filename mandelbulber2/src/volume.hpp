/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cVolume calculates voxel volume and stores data in image slices for 3d reconstruction
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

#ifndef VOLUME_H_
#define VOLUME_H_

#include "algebra.hpp"
#include "global_data.hpp"
#include "system.hpp"

class cVolume
{
public:
	cVolume(int w, int h, int l, CVector3 tlf, CVector3 brb)
	{
		this->w = w;
		this->h = h;
		this->l = l;
		this->tlf = tlf;
		this->brb = brb;
		voxel = new unsigned char[w * h * l];
	}

	void CalculateVolume();
	void Store(QString folder);

	unsigned char* voxel;
	int w, h, l;
	CVector3 tlf;
	CVector3 brb;
};


#endif /* VOLUME_H_ */
