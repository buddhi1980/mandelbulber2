/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020-22 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * Wrapper for radiance HDR
 */

#include "radiance_hdr.h"

#include <QImage>
#include <QPixmap>

#include <cstdio>

// custom includes
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_HDR
#include "third-party/stb/stb_image.h"

cRadianceHDR::cRadianceHDR()
{
	// Just for testing
	int x, y, n;

	// check if it is radiance hdr image

	unsigned char *data = stbi_load("test.hdr", &x, &y, &n, 0);
	//    // ... process data if not NULL ...
	//    // ... x = width, y = height, n = # 8-bit components per pixel ...
	//    // ... replace '0' with '1'..'4' to force that many components per pixel
	//    // ... but 'n' will always be the number that it would have been if you said 0
	stbi_image_free(data);
}

cRadianceHDR::~cRadianceHDR()
{
	// TODO Auto-generated destructor stub
}

bool cRadianceHDR::Init(const QString filename, int *width, int *height)
{
	int w = 0;
	int h = 0;
	int bitsPerPixel = 0;

	if (stbi_is_hdr(filename.toLocal8Bit().constData()))
	{
		if (stbi_info(filename.toLocal8Bit().constData(), &w, &h, &bitsPerPixel))
		{
			*width = w;
			*height = h;
			imageWidth = w;
			imageHeight = h;
			actualFileName = filename;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void cRadianceHDR::Load(std::vector<sRGBFloat> *fBitmap)
{
	fBitmap->resize(imageWidth * imageHeight);
	int w = 0;
	int h = 0;
	int comp = 0;
	float *data = stbi_loadf(actualFileName.toLocal8Bit().constData(), &w, &h, &comp, 0);
	if (data)
	{
		quint64 numberOFPixels = w * h;
		for (quint64 i = 0; i < numberOFPixels; i++)
		{
			fBitmap->data()[i] = sRGBFloat(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
		}
		stbi_image_free(data);
	}
}

void cRadianceHDR::LoadToQPixmap(QPixmap *pixmap)
{
	std::vector<sRGBFloat> fBitmap(imageWidth * imageHeight);
	Load(&fBitmap);

	std::vector<sRGB8> image8(imageWidth * imageHeight);
	for (size_t i = 0; i < imageWidth * imageHeight; i++)
	{
		sRGBFloat fPixel = fBitmap[i];
		quint8 R = uchar(qBound(0.0f, powf(fPixel.R, 1.0f / 2.2f) * 255.0f, 255.0f));
		quint8 G = uchar(qBound(0.0f, powf(fPixel.G, 1.0f / 2.2f) * 255.0f, 255.0f));
		quint8 B = uchar(qBound(0.0f, powf(fPixel.B, 1.0f / 2.2f) * 255.0f, 255.0f));

		image8[i] = sRGB8(R, G, B);
	}

	const QImage qImage(reinterpret_cast<const uchar *>(image8.data()), imageWidth, imageHeight,
		imageWidth * sizeof(sRGB8), QImage::Format_RGB888);

	pixmap->convertFromImage(qImage);
}

// convert one scene linear pixel to a 4 byte RGBE sample (Radiance float2rgbe algorithm)
static void FloatToRgbe(const sRGBFloat &pixel, unsigned char *rgbe)
{
	float r = pixel.R > 0.0f ? pixel.R : 0.0f;
	float g = pixel.G > 0.0f ? pixel.G : 0.0f;
	float b = pixel.B > 0.0f ? pixel.B : 0.0f;
	float e = r > g ? (r > b ? r : b) : (g > b ? g : b);
	if (e < 1e-32f)
	{
		rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
		return;
	}
	int exponent = 0;
	float mantissa = frexpf(e, &exponent);
	float scale = mantissa * 256.0f / e;
	rgbe[0] = static_cast<unsigned char>(r * scale);
	rgbe[1] = static_cast<unsigned char>(g * scale);
	rgbe[2] = static_cast<unsigned char>(b * scale);
	rgbe[3] = static_cast<unsigned char>(exponent + 128);
}

// write one component plane of a scanline using the Radiance adaptive RLE scheme:
// runs of 4..127 equal bytes are stored as 0x80|count + value, others as literal blocks
// (a count of 128 would be ambiguous with the literal marker, so it must not be used)
static void WriteRleComponent(FILE *fp, const unsigned char *row, int width, int component)
{
	int x = 0;
	while (x < width)
	{
		int run = 1;
		while (run < 127 && x + run < width && row[(x + run) * 4 + component] == row[x * 4 + component])
		{
			run++;
		}
		if (run >= 4)
		{
			fputc(0x80 | run, fp);
			fputc(row[x * 4 + component], fp);
			x += run;
		}
		else
		{
			// collect a literal run until the next repeat candidate or the 128 byte limit
			long lit_start = ftell(fp);
			fputc(0, fp); // placeholder for the literal count
			int count = 0;
			while (x < width && count < 128)
			{
				int r = 1;
				while (r < 127 && x + r < width && row[(x + r) * 4 + component] == row[x * 4 + component])
				{
					r++;
				}
				if (r >= 4) break;
				fputc(row[x * 4 + component], fp);
				x++;
				count++;
			}
			long lit_end = ftell(fp);
			fseek(fp, lit_start, SEEK_SET);
			fputc(count, fp);
			fseek(fp, lit_end, SEEK_SET);
		}
	}
}

bool cRadianceHDR::SaveRGBE(const QString &filename, uint64_t width, uint64_t height,
	const std::vector<sRGBFloat> &rgb, const QMap<QString, QString> &meta,
	const std::function<void(double)> &progress)
{
	if (!width || !height || rgb.size() < width * height) return false;

	FILE *fp = fopen(filename.toLocal8Bit().constData(), "wb");
	if (!fp) return false;

	bool ok = true;

	// file header, meta data as comment lines (newlines are flattened)
	fprintf(fp, "#?RADIANCE\n");
	fprintf(fp, "# Software: Mandelbulber v2\n");
	for (auto it = meta.constBegin(); it != meta.constEnd(); ++it)
	{
		QString value = it.value();
		value.remove('\n');
		value.remove('\r');
		fprintf(fp, "# %s: %s\n", qPrintable(it.key()), qPrintable(value));
	}
	fprintf(fp, "FORMAT=32-bit_rle_rgbe\n\n");
	fprintf(fp, "-Y %u +X %u\n", static_cast<unsigned>(height), static_cast<unsigned>(width));

	bool useNewRle = (width >= 8 && width <= 0x7fff);
	std::vector<unsigned char> row(static_cast<size_t>(width) * 4);

	for (uint64_t y = 0; y < height && ok; y++)
	{
		const sRGBFloat *line = &rgb[y * width];
		unsigned char *rgbe = row.data();
		for (uint64_t x = 0; x < width; x++)
		{
			FloatToRgbe(line[x], rgbe + x * 4);
		}

		if (useNewRle)
		{
			fputc(2, fp);
			fputc(2, fp);
			fputc(static_cast<int>((width >> 8) & 0xff), fp);
			fputc(static_cast<int>(width & 0xff), fp);
			for (int component = 0; component < 4; component++)
			{
				WriteRleComponent(fp, row.data(), static_cast<int>(width), component);
			}
		}
		else
		{
			// old format: plain scanline with the 2-byte width prefix
			fputc(static_cast<int>((width >> 8) & 0x7f), fp);
			fputc(static_cast<int>(width & 0xff), fp);
			fwrite(row.data(), 1, row.size(), fp);
		}
		ok = ok && (ferror(fp) == 0);

		if (progress && (y % 64 == 0 || y == height - 1))
		{
			progress(1.0 * y / height);
		}
	}

	fclose(fp);
	if (!ok) remove(filename.toLocal8Bit().constData());
	return ok;
}
