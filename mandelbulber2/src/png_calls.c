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
 * libpng calls that need setjmp() for error handling; see png_calls.h
 */

#include "png_calls.h"

int PngInitIo(png_structp png_ptr, FILE *fp)
{
	if (setjmp(png_jmpbuf(png_ptr))) return 0;
	png_init_io(png_ptr, fp);
	return 1;
}

int PngWriteHeader(png_structp png_ptr, png_infop info_ptr, png_uint_32 width, png_uint_32 height,
	int bitDepth, int colorType)
{
	if (setjmp(png_jmpbuf(png_ptr))) return 0;
	png_set_IHDR(png_ptr, info_ptr, width, height, bitDepth, colorType, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	png_set_swap(png_ptr);
	return 1;
}

int PngWriteRows(png_structp png_ptr, png_bytepp rows, png_uint_32 numberOfRows)
{
	if (setjmp(png_jmpbuf(png_ptr))) return 0;
	png_write_rows(png_ptr, rows, numberOfRows);
	return 1;
}

int PngWriteImage(png_structp png_ptr, png_bytepp rows)
{
	if (setjmp(png_jmpbuf(png_ptr))) return 0;
	png_write_image(png_ptr, rows);
	return 1;
}

int PngWriteEnd(png_structp png_ptr, png_infop info_ptr)
{
	if (setjmp(png_jmpbuf(png_ptr))) return 0;
	png_write_end(png_ptr, info_ptr);
	return 1;
}

int PngReadImage(png_structp png_ptr, png_infop info_ptr, FILE *fp, int signatureBytes)
{
	if (setjmp(png_jmpbuf(png_ptr))) return 0;
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, signatureBytes);
	png_read_png(png_ptr, info_ptr,
		PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_SWAP_ENDIAN, NULL);
	return 1;
}
