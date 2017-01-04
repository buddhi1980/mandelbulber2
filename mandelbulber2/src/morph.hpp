/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com), Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * Class to interpolate parameters for animation
 *
 * cMorph contains all methods to prepare parameters for interpolation and
 * interpolate them with different interpolation techniques.
 */

#ifndef MANDELBULBER2_SRC_MORPH_HPP_
#define MANDELBULBER2_SRC_MORPH_HPP_

#include "one_parameter.hpp"
#include <QList>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

class cMorph
{
public:
	struct sMorphParameter
	{
		sMorphParameter(int _keyframe, const cOneParameter &_parameter)
				: keyframe(_keyframe), parameter(_parameter)
		{
		}
		int keyframe;
		cOneParameter parameter;
	};

	cMorph();
	~cMorph();
	cMorph(const cMorph &source);
	cMorph &operator=(const cMorph &source);
	void AddData(const int keyframe, const cOneParameter &val);
	int findInMorph(const int keyframe);
	void Clear() { dataSets.clear(); }
	cOneParameter Interpolate(const int keyframe, double factor);
	cOneParameter None(const int key) const;
	cOneParameter Linear(const int key, const double factor, bool const angular);
	cOneParameter CatmullRom(const int key, const double factor, bool const angular);
	cOneParameter Akima(const int key, const double factor, bool const angular);

	double LinearInterpolate(const double factor, double v1, double v2, bool const angular) const;
	double CatmullRomInterpolate(
		const double factor, double v1, double v2, double v3, double v4, bool const angular) const;
	double AkimaInterpolate(const double factor, double v1, double v2, double v3, double v4,
		double v5, double v6, const bool angular) const;
	void NearestNeighbourAngle(QList<double *> vals) const;

private:
	int listSize;
	gsl_interp_accel *interpolationAccelerator;
	gsl_spline *splineAkimaPeriodic;
	QList<sMorphParameter> dataSets;
};

#endif /* MANDELBULBER2_SRC_MORPH_HPP_ */
