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
 * compression / uncompression of a QByteArray with lzo
 * lzo compression gives a high throughput rate (~250MBps) with a moderate compression result of
 * ~30-35%
 * based on https://github.com/tex/fusecompress/blob/master/src/boost/iostreams/filter/lzo.hpp
 */

#ifndef MANDELBULBER2_SRC_LZO_COMPRESSION_H_
#define MANDELBULBER2_SRC_LZO_COMPRESSION_H_

#include <assert.h>
#include <lzo/lzo1x.h>
#include <lzo/lzoconf.h>

#include <QByteArray>
#include <QElapsedTimer>

#include "system.hpp"
#include "cast.hpp"

QByteArray lzoCompress(QByteArray data)
{
	QElapsedTimer time;
	time.start();
	void *wrkmem = malloc(LZO1X_1_MEM_COMPRESS);

	size_t len = data.size() + data.size() / 16 + 64 + 3;
	char *out = (char *)malloc(len);

	int ret = lzo1x_1_compress((lzo_bytep)data.data(), (lzo_uint)data.size(), (lzo_bytep)out,
		(lzo_uintp)&len, (lzo_voidp)wrkmem);

	assert(ret == LZO_E_OK);

	WriteLog(
		QString("lzo: %1 bytes compressed into %2 bytes, ratio: %3, in %4 micro seconds, %5 mBps")
			.arg(data.size())
			.arg((unsigned long)len)
			.arg(1.0 * len / data.size())
			.arg(time.nsecsElapsed() / 1000.0)
			.arg((data.size() / 1000000.0) / (time.nsecsElapsed() / 1e9)),
		3);

	QByteArray arr;
	arr.append((char *)out, CastSizeToInt(len));
	free(wrkmem);
	free(out);
	return arr;
}

QByteArray lzoUncompress(QByteArray data)
{
	QElapsedTimer time;
	time.start();
	lzo_uint len;
	void *tmp = nullptr;
	int decompressionFactor = 10;

	while (true)
	{
		len = data.size() * decompressionFactor;
		if (tmp) free(tmp);
		tmp = malloc(len);

		if (lzo1x_decompress_safe((lzo_bytep)data.data(), data.size(), (lzo_bytep)tmp, &len, NULL)
				== LZO_E_OUTPUT_OVERRUN)
		{
			decompressionFactor *= 2;
			continue;
		}
		break;
	}

	WriteLog(
		QString("lzo: %1 bytes uncompressed into %2 bytes, ratio: %3, in %4 micro seconds, %5 MBps")
			.arg(data.size())
			.arg((unsigned long)len)
			.arg(1.0 * len / data.size())
			.arg(time.nsecsElapsed() / 1000.0)
			.arg((data.size() / 1000000.0) / (time.nsecsElapsed() / 1e9)),
		3);

	QByteArray arr;
	arr.append((char *)tmp, len);
	free(tmp);
	return arr;
}

#endif /* MANDELBULBER2_SRC_LZO_COMPRESSION_H_ */
