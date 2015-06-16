/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Class to interpolate parameters for animation
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com), Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef MORPH_HPP_
#define MORPH_HPP_

#include <QList>
#include "one_parameter.hpp"
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

class cMorph
{
public:
	struct sMorphParameter
	{
		sMorphParameter(int _keyframe, const cOneParameter &_parameter) : keyframe(_keyframe), parameter(_parameter) {}
		int keyframe;
		cOneParameter parameter;
	};

	cMorph();
	~cMorph();
	cMorph(const cMorph &source);
	cMorph& operator=(const cMorph &source);
	void AddData(const int keyframe, const cOneParameter &val);
	int findInMorph(const int keyframe);
	void Clear(){ dataSets.clear(); }
	cOneParameter Interpolate(const int keyframe, float factor);
	cOneParameter None(const int key);
	cOneParameter Linear(const int key, const double factor, bool const angular);
	cOneParameter CatmullRom(const int key, const double factor, bool const angular);
	cOneParameter Akima(const int key, const double factor, bool const angular);

	double LinearInterpolate(const double factor, double v1, double v2, bool const angular);
	double CatmullRomInterpolate(const double factor, double v1, double v2, double v3, double v4, bool const angular);
	double AkimaInterpolate(const double factor, double v1, double v2, double v3, double v4, double v5, double v6, const bool angular);
	void NearestNeighbourAngle(QList<double*> vals);
private:
	int listSize;
	gsl_interp_accel *interpolationAccelerator;
	gsl_spline *splineAkimaPeriodic;
	QList<sMorphParameter> dataSets;
};

#endif /* MORPH_HPP_ */
