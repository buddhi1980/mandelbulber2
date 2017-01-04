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

#include "morph.hpp"
#include "common_math.h"

cMorph::cMorph()
{
	listSize = 6;
	interpolationAccelerator = gsl_interp_accel_alloc();
	splineAkimaPeriodic = gsl_spline_alloc(gsl_interp_akima_periodic, size_t(listSize));
}

cMorph::~cMorph()
{
	gsl_spline_free(splineAkimaPeriodic);
	gsl_interp_accel_free(interpolationAccelerator);
	splineAkimaPeriodic = nullptr;
	interpolationAccelerator = nullptr;
}

cMorph::cMorph(const cMorph &source)
{
	splineAkimaPeriodic = nullptr;
	interpolationAccelerator = nullptr;
	*this = source;
}

cMorph &cMorph::operator=(const cMorph &source)
{
	if (this != &source)
	{
		listSize = source.listSize;
		interpolationAccelerator = gsl_interp_accel_alloc();
		splineAkimaPeriodic = gsl_spline_alloc(gsl_interp_akima_periodic, size_t(listSize));
		dataSets = source.dataSets;
	}
	return *this;
}

void cMorph::AddData(const int keyFrame, const cOneParameter &val)
{
	int key = findInMorph(keyFrame);
	if (key != -1) return;
	sMorphParameter mVal(keyFrame, val);
	dataSets.append(mVal);

	if (dataSets.size() > listSize)
	{
		dataSets.removeFirst();
	}
}

int cMorph::findInMorph(const int keyframe)
{
	for (int i = 0; i < dataSets.size(); i++)
	{
		if (dataSets[i].keyframe == keyframe)
		{
			return i;
		}
	}
	return -1;
}

cOneParameter cMorph::Interpolate(const int keyframe, double factor)
{
	int key = findInMorph(keyframe);
	if (key == -1) return dataSets[0].parameter;
	if (dataSets[key].parameter.GetValueType() == typeString
			|| dataSets[key].parameter.GetValueType() == typeBool)
	{
		return None(key);
	}

	switch (dataSets[0].parameter.GetMorphType())
	{
		case morphNone: return None(key);
		case morphLinear: return Linear(key, factor, false);
		case morphLinearAngle: return Linear(key, factor, true);
		case morphCatMullRom: return CatmullRom(key, factor, false);
		case morphCatMullRomAngle: return CatmullRom(key, factor, true);
		case morphAkima: return Akima(key, factor, false);
		case morphAkimaAngle: return Akima(key, factor, true);
		default: return None(key);
	}
}

cOneParameter cMorph::None(const int key) const
{
	return dataSets[key].parameter;
}

cOneParameter cMorph::Linear(const int key, const double factor, const bool angular)
{
	int k1, k2;
	if (key == dataSets.size() - 1) return dataSets[key].parameter;

	cOneParameter interpolated = dataSets[key].parameter;
	cMultiVal val;

	k1 = key;
	k2 = key + 1;

	switch (dataSets[key].parameter.GetValueType())
	{
		case typeNull:
		case typeString:
		case typeBool: return None(key);

		case typeDouble:
		case typeInt:
		{
			double v1, v2;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			val.Store(LinearInterpolate(factor, v1, v2, angular));
			break;
		}
		case typeRgb:
		{
			sRGB v1, v2;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			val.Store(sRGB(int(LinearInterpolate(factor, v1.R, v2.R, angular)),
				int(LinearInterpolate(factor, v1.G, v2.G, angular)),
				int(LinearInterpolate(factor, v1.B, v2.B, angular))));
			break;
		}
		case typeVector3:
		{
			CVector3 v1, v2;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			val.Store(CVector3(LinearInterpolate(factor, v1.x, v2.x, angular),
				LinearInterpolate(factor, v1.y, v2.y, angular),
				LinearInterpolate(factor, v1.z, v2.z, angular)));
			break;
		}
		case typeVector4:
		{
			CVector4 v1, v2;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			val.Store(CVector4(LinearInterpolate(factor, v1.x, v2.x, angular),
				LinearInterpolate(factor, v1.y, v2.y, angular),
				LinearInterpolate(factor, v1.z, v2.z, angular),
				LinearInterpolate(factor, v1.w, v2.w, angular)));
			break;
		}
		case typeColorPalette:
		{
			cColorPalette v1, v2;
			cColorPalette out;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			for (int i = 0; i < v1.GetSize(); i++)
			{
				out.AppendColor(
					sRGB(int(LinearInterpolate(factor, v1.GetColor(i).R, v2.GetColor(i).R, angular)),
						int(LinearInterpolate(factor, v1.GetColor(i).G, v2.GetColor(i).G, angular)),
						int(LinearInterpolate(factor, v1.GetColor(i).B, v2.GetColor(i).B, angular))));
			}
			val.Store(out);
			break;
		}
	}

	interpolated.SetMultival(val, valueActual);
	return interpolated;
}

cOneParameter cMorph::CatmullRom(int const key, double const factor, bool const angular)
{
	int k1, k2, k3, k4;

	if (key >= 1)
		k1 = key - 1;
	else
		k1 = key;

	if (key < dataSets.size())
		k2 = key;
	else
		k2 = dataSets.size() - 1;

	if (key < dataSets.size() - 1)
		k3 = key + 1;
	else
		k3 = dataSets.size() - 1;

	if (key < dataSets.size() - 2)
		k4 = key + 2;
	else
		k4 = dataSets.size() - 1;

	cOneParameter interpolated = dataSets[key].parameter;
	cMultiVal val;

	switch (dataSets[key].parameter.GetValueType())
	{
		case typeNull:
		case typeString:
		case typeBool: return None(key);

		case typeDouble:
		case typeInt:
		{
			double v1, v2, v3, v4;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			dataSets[k3].parameter.GetMultival(valueActual).Get(v3);
			dataSets[k4].parameter.GetMultival(valueActual).Get(v4);
			val.Store(CatmullRomInterpolate(factor, v1, v2, v3, v4, angular));
			break;
		}
		case typeRgb:
		{
			sRGB v1, v2, v3, v4;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			dataSets[k3].parameter.GetMultival(valueActual).Get(v3);
			dataSets[k4].parameter.GetMultival(valueActual).Get(v4);
			val.Store(sRGB(int(CatmullRomInterpolate(factor, v1.R, v2.R, v3.R, v4.R, angular)),
				int(CatmullRomInterpolate(factor, v1.G, v2.G, v3.G, v4.G, angular)),
				int(CatmullRomInterpolate(factor, v1.B, v2.B, v3.B, v4.B, angular))));
			break;
		}
		case typeVector3:
		{
			CVector3 v1, v2, v3, v4;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			dataSets[k3].parameter.GetMultival(valueActual).Get(v3);
			dataSets[k4].parameter.GetMultival(valueActual).Get(v4);
			val.Store(CVector3(CatmullRomInterpolate(factor, v1.x, v2.x, v3.x, v4.x, angular),
				CatmullRomInterpolate(factor, v1.y, v2.y, v3.y, v4.y, angular),
				CatmullRomInterpolate(factor, v1.z, v2.z, v3.z, v4.z, angular)));
			break;
		}
		case typeVector4:
		{
			CVector4 v1, v2, v3, v4;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			dataSets[k3].parameter.GetMultival(valueActual).Get(v3);
			dataSets[k4].parameter.GetMultival(valueActual).Get(v4);
			val.Store(CVector4(CatmullRomInterpolate(factor, v1.x, v2.x, v3.x, v4.x, angular),
				CatmullRomInterpolate(factor, v1.y, v2.y, v3.y, v4.y, angular),
				CatmullRomInterpolate(factor, v1.z, v2.z, v3.z, v4.z, angular),
				CatmullRomInterpolate(factor, v1.w, v2.w, v3.w, v4.w, angular)));
			break;
		}
		case typeColorPalette:
		{
			cColorPalette v1, v2, v3, v4;
			cColorPalette out;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			dataSets[k3].parameter.GetMultival(valueActual).Get(v3);
			dataSets[k4].parameter.GetMultival(valueActual).Get(v4);
			for (int i = 0; i < v1.GetSize(); i++)
			{
				out.AppendColor(sRGB(int(CatmullRomInterpolate(factor, v1.GetColor(i).R, v2.GetColor(i).R,
															 v3.GetColor(i).R, v4.GetColor(i).R, angular)),
					int(CatmullRomInterpolate(factor, v1.GetColor(i).G, v2.GetColor(i).G, v3.GetColor(i).G,
						v4.GetColor(i).G, angular)),
					int(CatmullRomInterpolate(factor, v1.GetColor(i).B, v2.GetColor(i).B, v3.GetColor(i).B,
						v4.GetColor(i).B, angular))));
			}
			val.Store(out);
			break;
		}
	}

	interpolated.SetMultival(val, valueActual);
	return interpolated;
}

cOneParameter cMorph::Akima(int const key, double const factor, bool const angular)
{
	int k1, k2, k3, k4, k5, k6;

	if (key >= 2)
		k1 = key - 2;
	else
		k1 = 0;

	if (key >= 1)
		k2 = key - 1;
	else
		k2 = 0;

	k3 = key;

	if (key < dataSets.size() - 1)
		k4 = key + 1;
	else
		k4 = dataSets.size() - 1;

	if (key < dataSets.size() - 2)
		k5 = key + 2;
	else
		k5 = dataSets.size() - 1;

	if (key < dataSets.size() - 3)
		k6 = key + 3;
	else
		k6 = dataSets.size() - 1;

	cOneParameter interpolated = dataSets[key].parameter;
	cMultiVal val;

	switch (dataSets[key].parameter.GetValueType())
	{
		case typeNull:
		case typeString:
		case typeBool: return None(key);

		case typeDouble:
		case typeInt:
		{
			double v1, v2, v3, v4, v5, v6;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			dataSets[k3].parameter.GetMultival(valueActual).Get(v3);
			dataSets[k4].parameter.GetMultival(valueActual).Get(v4);
			dataSets[k5].parameter.GetMultival(valueActual).Get(v5);
			dataSets[k6].parameter.GetMultival(valueActual).Get(v6);
			val.Store(AkimaInterpolate(factor, v1, v2, v3, v4, v5, v6, angular));
			break;
		}
		case typeRgb:
		{
			sRGB v1, v2, v3, v4, v5, v6;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			dataSets[k3].parameter.GetMultival(valueActual).Get(v3);
			dataSets[k4].parameter.GetMultival(valueActual).Get(v4);
			dataSets[k5].parameter.GetMultival(valueActual).Get(v5);
			dataSets[k6].parameter.GetMultival(valueActual).Get(v6);
			val.Store(sRGB(int(AkimaInterpolate(factor, v1.R, v2.R, v3.R, v4.R, v5.R, v6.R, angular)),
				int(AkimaInterpolate(factor, v1.G, v2.G, v3.G, v4.G, v5.G, v6.G, angular)),
				int(AkimaInterpolate(factor, v1.B, v2.B, v3.B, v4.B, v5.B, v6.B, angular))));
			break;
		}
		case typeVector3:
		{
			CVector3 v1, v2, v3, v4, v5, v6;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			dataSets[k3].parameter.GetMultival(valueActual).Get(v3);
			dataSets[k4].parameter.GetMultival(valueActual).Get(v4);
			dataSets[k5].parameter.GetMultival(valueActual).Get(v5);
			dataSets[k6].parameter.GetMultival(valueActual).Get(v6);
			val.Store(CVector3(AkimaInterpolate(factor, v1.x, v2.x, v3.x, v4.x, v5.x, v6.x, angular),
				AkimaInterpolate(factor, v1.y, v2.y, v3.y, v4.y, v5.y, v6.y, angular),
				AkimaInterpolate(factor, v1.z, v2.z, v3.z, v4.z, v5.z, v6.z, angular)));
			break;
		}
		case typeVector4:
		{
			CVector4 v1, v2, v3, v4, v5, v6;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			dataSets[k3].parameter.GetMultival(valueActual).Get(v3);
			dataSets[k4].parameter.GetMultival(valueActual).Get(v4);
			dataSets[k5].parameter.GetMultival(valueActual).Get(v5);
			dataSets[k6].parameter.GetMultival(valueActual).Get(v6);
			val.Store(CVector4(AkimaInterpolate(factor, v1.x, v2.x, v3.x, v4.x, v5.x, v6.x, angular),
				AkimaInterpolate(factor, v1.y, v2.y, v3.y, v4.y, v5.y, v6.y, angular),
				AkimaInterpolate(factor, v1.z, v2.z, v3.z, v4.z, v5.z, v6.z, angular),
				AkimaInterpolate(factor, v1.w, v2.w, v3.w, v4.w, v5.w, v6.w, angular)));
			break;
		}
		case typeColorPalette:
		{
			cColorPalette v1, v2, v3, v4, v5, v6;
			cColorPalette out;
			dataSets[k1].parameter.GetMultival(valueActual).Get(v1);
			dataSets[k2].parameter.GetMultival(valueActual).Get(v2);
			dataSets[k3].parameter.GetMultival(valueActual).Get(v3);
			dataSets[k4].parameter.GetMultival(valueActual).Get(v4);
			dataSets[k5].parameter.GetMultival(valueActual).Get(v5);
			dataSets[k6].parameter.GetMultival(valueActual).Get(v6);
			for (int i = 0; i < v1.GetSize(); i++)
			{
				out.AppendColor(
					sRGB(int(AkimaInterpolate(factor, v1.GetColor(i).R, v2.GetColor(i).R, v3.GetColor(i).R,
								 v4.GetColor(i).R, v5.GetColor(i).R, v6.GetColor(i).R, angular)),
						int(AkimaInterpolate(factor, v1.GetColor(i).G, v2.GetColor(i).G, v3.GetColor(i).G,
							v4.GetColor(i).G, v5.GetColor(i).G, v6.GetColor(i).G, angular)),
						int(AkimaInterpolate(factor, v1.GetColor(i).B, v2.GetColor(i).B, v3.GetColor(i).B,
							v4.GetColor(i).B, v5.GetColor(i).B, v6.GetColor(i).B, angular))));
			}
			val.Store(out);
			break;
		}
	}

	interpolated.SetMultival(val, valueActual);
	return interpolated;
}

double cMorph::LinearInterpolate(
	const double factor, double v1, double v2, bool const angular) const
{
	if (angular)
	{
		QList<double *> vals;
		vals << &v1 << &v2;
		NearestNeighbourAngle(vals);
	}
	double value = v1 + ((v2 - v1) * factor);
	if (angular)
	{
		return LimitAngle(value);
	}
	else
	{
		return value;
	}
}

double cMorph::CatmullRomInterpolate(
	const double factor, double v1, double v2, double v3, double v4, const bool angular) const
{
	if (angular)
	{
		QList<double *> vals;
		vals << &v1 << &v2 << &v3 << &v4;
		NearestNeighbourAngle(vals);
	}

	double factor2 = factor * factor;
	double factor3 = factor2 * factor;

	bool logarithmic = false;
	bool negative = false;
	if ((v1 > 0 && v2 > 0 && v3 > 0 && v4 > 0) || (v1 < 0 && v2 < 0 && v3 < 0 && v4 < 0))
	{
		if (v1 < 0) negative = true;
		double average = (v1 + v2 + v3 + v4) / 4.0;
		if (average > 0)
		{
			double deviation = (fabs(v2 - v1) + fabs(v3 - v2) + fabs(v4 - v3)) / average;
			if (deviation > 0.1)
			{
				v1 = log(fabs(v1));
				v2 = log(fabs(v2));
				v3 = log(fabs(v3));
				v4 = log(fabs(v4));
				logarithmic = true;
			}
		}
	}
	double value = 0.5 * ((2 * v2) + (-v1 + v3) * factor + (2 * v1 - 5 * v2 + 4 * v3 - v4) * factor2
												 + (-v1 + 3 * v2 - 3 * v3 + v4) * factor3);
	if (logarithmic)
	{
		if (negative)
			value = -exp(value);
		else
			value = exp(value);
	}
	if (value > 1e20) value = 1e20;
	if (value < -1e20) value = 1e20;
	if (fabs(value) < 1e-20) value = 0.0;

	if (angular)
	{
		return LimitAngle(value);
	}
	else
	{
		return value;
	}
}

double cMorph::AkimaInterpolate(const double factor, double v1, double v2, double v3, double v4,
	double v5, double v6, const bool angular) const
{
	if (angular)
	{
		QList<double *> vals;
		vals << &v1 << &v2 << &v3 << &v4 << &v5 << &v6;
		NearestNeighbourAngle(vals);
	}

	double x[] = {-2, -1, 0, 1, 2, 3};
	double y[] = {v1, v2, v3, v4, v5, v6};
	// more information: http://www.alglib.net/interpolation/spline3.php
	gsl_spline_init(splineAkimaPeriodic, x, y, size_t(listSize));
	double value = gsl_spline_eval(splineAkimaPeriodic, factor, interpolationAccelerator);

	if (angular)
	{
		return LimitAngle(value);
	}
	else
	{
		return value;
	}
}

void cMorph::NearestNeighbourAngle(QList<double *> vals) const
{
	// modify angles, so that each successive value is not more than 180deg (absolute)
	// away from last value
	// before execution of NearestNeighbourAngle two consecutive angles in vals shall not be
	// more than 360 degrees apart

	int deltaCircle = 0;
	for (int i = 1; i < vals.size(); i++)
	{
		double newVal = *vals[i] + deltaCircle;
		if (fabs(newVal - *vals[i - 1]) > 180.0)
		{
			// need to adjust next angle
			deltaCircle += newVal < *vals[i - 1] ? 360 : -360;
		}
		*vals[i] += deltaCircle;
	}
}
