/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * constant defines used in opencl code
 */

#ifndef MANDELBULBER2_OPENCL_OPENCL_DEFINES_CL_H_
#define MANDELBULBER2_OPENCL_OPENCL_DEFINES_CL_H_

// TODO: autogenerate from program constants

#define SQRT_1_3 0.57735026918962576450914878050196
#define SQRT_1_2 0.70710678118654752440084436210485
#define SQRT_2_3 0.81649658092772603273242802490196
#define SQRT_3_2 1.22474487139158904909864203735295
#define SQRT_3_4 0.86602540378443864676372317075294
#define SQRT_3_4d2 0.43301270189221932338186158537647
#define SQRT_3 1.73205080756887729352744634150587

#define FRAC_1_3 0.33333333333333333333333333333333

#ifndef M_PI_180
#define M_PI_180 0.01745329251994329576923690768489
#endif

#ifndef M_PI_8
#define M_PI_8 0.39269908169872415480783042290994
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830961566084581988
#endif

#ifndef M_PI_2x
#define M_PI_2x 6.283185307179586476925286766559
#endif

#define NUMBER_OF_FRACTALS 9
#define IFS_VECTOR_COUNT 9
#define HYBRID_COUNT 5
#define MANDELBOX_FOLDS 2
#define MAX_RAYMARCHING 10000
#define Q_UNUSED(x) (void)x

#endif /* MANDELBULBER2_OPENCL_OPENCL_DEFINES_CL_H_ */
