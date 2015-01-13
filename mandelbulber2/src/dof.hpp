/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * PostRendering_DOF - calculation of Depth of Field effect
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

#ifndef DOF_HPP_
#define DOF_HPP_

#include "cimage.hpp"
#include <QProgressBar>
#include <QStatusBar>

class cPostRenderingDOF: public QObject
{
	Q_OBJECT

private:
	template <class TYPE>

	struct sSortZ
	{
		TYPE z;
		int i;
	};

public:
	cPostRenderingDOF(cImage *_image);

	void Render(double deep, double neutral, bool *stopRequest);
	template <class T> void QuickSortZBuffer(sSortZ<T> *dane, int l, int p);

	cImage *image;

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
};
#endif /* DOF_HPP_ */
