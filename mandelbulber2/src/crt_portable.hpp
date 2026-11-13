/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Portable replacements for C runtime functions that MSVC declares deprecated
 * (warning C4996). MSVC gets its own non-deprecated equivalents, which behave
 * like the standard functions; other compilers get the standard functions.
 */

#ifndef MANDELBULBER2_SRC_CRT_PORTABLE_HPP_
#define MANDELBULBER2_SRC_CRT_PORTABLE_HPP_

#include <cstdio>
#include <ctime>

#ifdef _MSC_VER
#include <share.h>
#endif

// opens a file like fopen()
// MSVC: _fsopen() with _SH_DENYNO, the sharing mode of fopen(); fopen_s() would open the
// file without sharing
inline FILE *OpenFile(const char *fileName, const char *mode)
{
#ifdef _MSC_VER
	return _fsopen(fileName, mode, _SH_DENYNO);
#else
	return fopen(fileName, mode);
#endif
}

// converts a time to local time like localtime(), into a buffer of the caller;
// returns nullptr when the time cannot be converted
inline struct tm *LocalTime(const time_t *time, struct tm *result)
{
#if defined(_MSC_VER)
	return localtime_s(result, time) == 0 ? result : nullptr;
#elif defined(_WIN32)
	// MinGW: localtime() keeps its result per thread there
	struct tm *converted = localtime(time);
	if (!converted) return nullptr;
	*result = *converted;
	return result;
#else
	return localtime_r(time, result);
#endif
}

#endif /* MANDELBULBER2_SRC_CRT_PORTABLE_HPP_ */
