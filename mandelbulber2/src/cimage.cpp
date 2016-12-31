/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
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
 * cImage class - manipulation of multi-layer images
 */

#include "cimage.hpp"
#include <QtCore>
#include <qpainter.h>

cImage::cImage(int w, int h, bool _allocLater)
{
	isAllocated = false;
	width = w;
	height = h;
	allocLater = _allocLater;
	construct();
}

void cImage::construct()
{
	previewAllocated = false;
	image8 = NULL;
	image16 = NULL;
	imageFloat = NULL;
	alphaBuffer8 = NULL;
	alphaBuffer16 = NULL;
	opacityBuffer = NULL;
	colourBuffer = NULL;
	zBuffer = NULL;
	gammaTable = NULL;

	// optional image buffers
	normalFloat = NULL;
	normal8 = NULL;
	normal16 = NULL;

	AllocMem();
	progressiveFactor = 1;
	imageWidget = NULL;
	isUsed = false;
	previewScale = 1.0;
	previewVisibleWidth = 1;
	previewVisibleHeight = 1;
	previewWidth = 0;
	previewHeight = 0;

	gammaTablePrepared = false;

	isMainImage = false;
}

cImage::~cImage()
{
	isAllocated = false;
	// qDebug() << "cImage::~cImage()";
	FreeImage();

	if (previewAllocated)
	{
		if (preview) delete[] preview;
		preview = NULL;
		if (preview2) delete[] preview2;
		preview2 = NULL;
	}
}

bool cImage::AllocMem(void)
{
	isAllocated = false;
	// qDebug() << "bool cImage::AllocMem(void)";
	if (width > 0 && height > 0)
	{
		if (!allocLater)
		{
			try
			{
				imageFloat = new sRGBfloat[width * height];
				image16 = new sRGB16[width * height];
				image8 = new sRGB8[width * height];
				zBuffer = new float[width * height];
				alphaBuffer8 = new unsigned char[width * height];
				alphaBuffer16 = new unsigned short[width * height];
				opacityBuffer = new unsigned short[width * height];
				colourBuffer = new sRGB8[width * height];
				if (opt.optionalNormal)
				{
					normalFloat = new sRGBfloat[width * height];
					normal16 = new sRGB16[width * height];
					normal8 = new sRGB8[width * height];
				}
				ClearImage();
			}
			catch (std::bad_alloc &ba)
			{
				width = 0;
				height = 0;
				FreeImage();
				qCritical() << "bad_alloc caught in cimage: " << ba.what()
										<< ", maybe required image dimension to big?";
				return false;
			}
		}
	}
	else
	{
		fprintf(stderr, "Error! Cannot allocate memory for image (wrong image size)\n");
	}

	if (previewAllocated)
	{
		delete[] preview;
		delete[] preview2;
	}
	previewAllocated = false;

	preview = NULL;
	preview2 = NULL;
	isAllocated = true;
	return true;
}

bool cImage::ChangeSize(int w, int h, sImageOptional optional)
{
	if (w != width || h != height || !(optional == *GetImageOptional()) || allocLater)
	{
		previewMutex.lock();
		width = w;
		height = h;
		SetImageOptional(optional);
		FreeImage();
		allocLater = false;
		bool result = AllocMem();
		previewMutex.unlock();
		return result;
	}
	return true;
}

void cImage::ClearImage(void)
{
	memset(imageFloat, 0, (unsigned long int)sizeof(sRGBfloat) * width * height);
	memset(image16, 0, (unsigned long int)sizeof(sRGB16) * width * height);
	memset(image8, 0, (unsigned long int)sizeof(sRGB8) * width * height);
	memset(alphaBuffer8, 0, (unsigned long int)sizeof(unsigned char) * width * height);
	memset(alphaBuffer16, 0, (unsigned long int)sizeof(unsigned short) * width * height);
	memset(opacityBuffer, 0, (unsigned long int)sizeof(unsigned short) * width * height);
	memset(colourBuffer, 0, (unsigned long int)sizeof(sRGB8) * width * height);
	if (opt.optionalNormal)
	{
		if (normalFloat) memset(normalFloat, 0, (unsigned long int)sizeof(sRGBfloat) * width * height);
		if (normal16) memset(normal16, 0, (unsigned long int)sizeof(sRGB16) * width * height);
		if (normal8) memset(normal8, 0, (unsigned long int)sizeof(sRGB8) * width * height);
	}
	for (long int i = 0; i < width * height; ++i)
		zBuffer[i] = float(1e20);
}

void cImage::FreeImage(void)
{
	isAllocated = false;
	// qDebug() << "void cImage::FreeImage(void)";
	if (imageFloat) delete[] imageFloat;
	imageFloat = NULL;
	if (image16) delete[] image16;
	image16 = NULL;
	if (image8) delete[] image8;
	image8 = NULL;
	if (alphaBuffer8) delete[] alphaBuffer8;
	alphaBuffer8 = NULL;
	if (alphaBuffer16) delete[] alphaBuffer16;
	alphaBuffer16 = NULL;
	if (opacityBuffer) delete[] opacityBuffer;
	opacityBuffer = NULL;
	if (colourBuffer) delete[] colourBuffer;
	colourBuffer = NULL;
	if (zBuffer) delete[] zBuffer;
	zBuffer = NULL;
	if (normalFloat) delete[] normalFloat;
	normalFloat = NULL;
	if (normal16) delete[] normal16;
	normal16 = NULL;
	if (normal8) delete[] normal8;
	normal8 = NULL;
	if (gammaTable) delete[] gammaTable;
	gammaTable = NULL;
	gammaTablePrepared = false;
}

sRGB16 cImage::CalculatePixel(sRGBfloat pixel)
{
	CalculateGammaTable();
	float R = pixel.R * adj.brightness;
	float G = pixel.G * adj.brightness;
	float B = pixel.B * adj.brightness;

	R = (R - 0.5) * adj.contrast + 0.5;
	G = (G - 0.5) * adj.contrast + 0.5;
	B = (B - 0.5) * adj.contrast + 0.5;

	if (R < 0.0) R = 0.0;
	if (G < 0.0) G = 0.0;
	if (B < 0.0) B = 0.0;

	// R = sqrt(tanh(R*R));
	// G = sqrt(tanh(G*G));
	// B = sqrt(tanh(B*B));
	if (adj.hdrEnabled)
	{
		R = tanh(R);
		G = tanh(G);
		B = tanh(B);
	}

	if (R > 1.0) R = 1.0;
	if (G > 1.0) G = 1.0;
	if (B > 1.0) B = 1.0;

	sRGB16 newPixel16;

	newPixel16.R = R * 65535.0;
	newPixel16.G = G * 65535.0;
	newPixel16.B = B * 65535.0;
	newPixel16.R = gammaTable[newPixel16.R];
	newPixel16.G = gammaTable[newPixel16.G];
	newPixel16.B = gammaTable[newPixel16.B];

	return newPixel16;
}

void cImage::CalculateGammaTable(void)
{
	if (!gammaTablePrepared)
	{
		if (!gammaTable) gammaTable = new int[65536];

		for (int i = 0; i < 65536; i++)
		{
			gammaTable[i] = pow(i / 65536.0, 1.0 / adj.imageGamma) * 65535;
		}
		gammaTablePrepared = true;
	}
}

void cImage::CompileImage(QList<int> *list)
{
	int listIndex = 0;
	for (int y = 0; y < height; y++)
	{
		if (list)
		{
			if (listIndex >= list->size()) break;
			y = list->at(listIndex);
			listIndex++;
		}
		for (int x = 0; x < width; x++)
		{
			unsigned long int address = x + y * width;
			sRGBfloat pixel = imageFloat[address];
			sRGB16 newPixel16 = CalculatePixel(pixel);
			image16[address] = newPixel16;
		}
	}
}

int cImage::GetUsedMB(void) const
{
	long int mb = 0;

	long int zBufferSize = (long int)width * height * sizeof(float);
	long int alphaSize16 = (long int)width * height * sizeof(unsigned short);
	long int alphaSize8 = (long int)width * height * sizeof(unsigned char);
	long int imageFloatSize = (long int)width * height * sizeof(sRGBfloat);
	long int image16Size = (long int)width * height * sizeof(sRGB16);
	long int image8Size = (long int)width * height * sizeof(sRGB8);
	long int colorSize = (long int)width * height * sizeof(sRGB8);
	long int opacitySize = (long int)width * height * sizeof(unsigned short);
	long int optionalSize = 0;
	if (opt.optionalNormal)
	{
		optionalSize += (long int)width * height * sizeof(sRGBfloat);
		optionalSize += (long int)width * height * sizeof(sRGB16);
		optionalSize += (long int)width * height * sizeof(sRGB8);
	}
	mb = (zBufferSize + alphaSize16 + alphaSize8 + image16Size + image8Size + imageFloatSize
				 + colorSize + opacitySize + optionalSize)
			 / 1024 / 1024;

	return mb;
}

void cImage::SetImageParameters(sImageAdjustments adjustments)
{
	adj = adjustments;
	gammaTablePrepared = false;
	CalculateGammaTable();
}

unsigned char *cImage::ConvertTo8bit(void)
{
	for (long int i = 0; i < width * height; i++)
	{
		image8[i].R = image16[i].R / 256;
		image8[i].G = image16[i].G / 256;
		image8[i].B = image16[i].B / 256;
	}
	return (unsigned char *)image8;
}

unsigned char *cImage::ConvertAlphaTo8bit(void)
{
	for (long int i = 0; i < width * height; i++)
	{
		alphaBuffer8[i] = alphaBuffer16[i] / 256;
	}
	return alphaBuffer8;
}

unsigned char *cImage::ConvertNormalto16Bit(void)
{
	if (!opt.optionalNormal) return NULL;
	for (long int i = 0; i < width * height; i++)
	{
		normal16[i].R = normalFloat[i].R * 65535;
		normal16[i].G = normalFloat[i].G * 65535;
		normal16[i].B = normalFloat[i].B * 65535;
	}
	return (unsigned char *)normal16;
}

unsigned char *cImage::ConvertNormalto8Bit(void)
{
	if (!opt.optionalNormal) return NULL;
	for (long int i = 0; i < width * height; i++)
	{
		normal8[i].R = normalFloat[i].R * 255;
		normal8[i].G = normalFloat[i].G * 255;
		normal8[i].B = normalFloat[i].B * 255;
	}
	return (unsigned char *)normal8;
}

sRGB8 cImage::Interpolation(float x, float y) const
{
	sRGB8 colour = sRGB8(0, 0, 0);
	if (x >= 0 && x < width - 1 && y >= 0 && y < height - 1)
	{
		int ix = x;
		int iy = y;
		int rx = (x - ix) * 256;
		int ry = (y - iy) * 256;
		sRGB8 k1 = image8[iy * width + ix];
		sRGB8 k2 = image8[iy * width + ix + 1];
		sRGB8 k3 = image8[(iy + 1) * width + ix];
		sRGB8 k4 = image8[(iy + 1) * width + ix + 1];
		colour.R = (k1.R * (255 - rx) * (255 - ry) + k2.R * (rx) * (255 - ry) + k3.R * (255 - rx) * ry
								 + k4.R * (rx * ry))
							 / 65536;
		colour.G = (k1.G * (255 - rx) * (255 - ry) + k2.G * (rx) * (255 - ry) + k3.G * (255 - rx) * ry
								 + k4.G * (rx * ry))
							 / 65536;
		colour.B = (k1.B * (255 - rx) * (255 - ry) + k2.B * (rx) * (255 - ry) + k3.B * (255 - rx) * ry
								 + k4.B * (rx * ry))
							 / 65536;
	}
	return colour;
}

unsigned char *cImage::CreatePreview(
	double scale, int visibleWidth, int visibleHeight, QWidget *widget = NULL)
{
	previewMutex.lock();
	int w = width * scale;
	int h = height * scale;
	previewVisibleWidth = visibleWidth;
	previewVisibleHeight = visibleHeight;
	if (previewAllocated)
	{
		delete[] preview;
		delete[] preview2;
	}
	preview = new sRGB8[w * h];
	preview2 = new sRGB8[w * h];

	memset(preview, 0, (unsigned long int)sizeof(sRGB8) * (w * h));
	memset(preview2, 0, (unsigned long int)sizeof(sRGB8) * (w * h));
	previewAllocated = true;
	previewWidth = w;
	previewHeight = h;
	previewScale = scale;
	unsigned char *ptr = (unsigned char *)preview;

	if (widget) imageWidget = widget;

	previewMutex.unlock();

	return ptr;
}

void cImage::UpdatePreview(QList<int> *list)
{
	if (previewAllocated && !allocLater)
	{
		previewMutex.lock();
		int w = previewWidth;
		int h = previewHeight;

		if (width == w && height == h)
		{
			memcpy(preview, image8, width * height * sizeof(sRGB8));
		}
		else
		{
			float scaleX = (float)width / w;
			float scaleY = (float)height / h;

			// number of pixels to sum
			int countX = (float)width / w + 1;
			int countY = (float)height / h + 1;
			int factor = countX * countY;

			float deltaX = scaleX / countX;
			float deltaY = scaleY / countY;

			int listIndex = 0;

			for (int y = 0; y < h; y++)
			{

				if (list)
				{
					if (listIndex >= list->size()) break;

					if (scaleY * y < list->at(listIndex) - (int)scaleY - 1) continue;

					while (scaleY * y - (int)scaleY - 1 > list->at(listIndex))
					{
						listIndex++;
						if (listIndex >= list->size()) break;
					}
				}

				//#ifndef WIN32
				//#pragma omp parallel for
				//#endif
				for (int x = 0; x < w; x++)
				{
					int R = 0;
					int G = 0;
					int B = 0;
					for (int j = 0; j < countY; j++)
					{
						float yy = y * scaleY + j * deltaY;

						for (int i = 0; i < countX; i++)
						{
							float xx = x * scaleX + i * deltaX;
							if (xx > 0 && xx < width - 1 && yy > 0 && yy < height - 1)
							{
								sRGB8 oldPixel = Interpolation(xx, yy);
								R += oldPixel.R;
								G += oldPixel.G;
								B += oldPixel.B;
							}
						} // next i
					}		// next j
					sRGB8 newpixel;
					newpixel.R = R / factor;
					newpixel.G = G / factor;
					newpixel.B = B / factor;
					preview[x + y * w] = newpixel;
				} // next x
			}		// next y
		}
		memcpy(preview2, preview, w * h * sizeof(sRGB8));
		previewMutex.unlock();
	}
	else
	{
		fprintf(stderr, "Error! Preview has not been prepared\n");
	}
}

unsigned char *cImage::GetPreviewPtr(void)
{
	unsigned char *ptr = 0;
	if (previewAllocated)
	{
		ptr = (unsigned char *)preview2;
	}
	else
		abort();
	return ptr;
}

unsigned char *cImage::GetPreviewPrimaryPtr(void)
{
	unsigned char *ptr = 0;
	if (previewAllocated)
	{
		ptr = (unsigned char *)preview;
	}
	else
		abort();
	return ptr;
}

bool cImage::IsPreview(void) const
{
	if (previewAllocated)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void cImage::RedrawInWidget(QWidget *qwidget)
{
	if (IsPreview())
	{
		// qDebug() << "Redraw" << endl;
		previewMutex.lock();

		QWidget *widget;
		if (qwidget)
			widget = qwidget;
		else
			widget = imageWidget;

		QPainter painter(widget);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

		QImage qimage((const uchar *)GetPreviewPtr(), previewWidth, previewHeight,
			previewWidth * sizeof(sRGB8), QImage::Format_RGB888);
		painter.drawImage(
			QRect(0, 0, previewWidth, previewHeight), qimage, QRect(0, 0, previewWidth, previewHeight));
		memcpy(preview2, preview, previewWidth * previewHeight * sizeof(sRGB8));
		previewMutex.unlock();
	}
}

void cImage::Squares(int y, int pFactor)
{
	progressiveFactor = pFactor;
	for (int x = 0; x <= width - pFactor; x += pFactor)
	{
		sRGBfloat pixelTemp = imageFloat[x + y * width];
		float zBufferTemp = zBuffer[x + y * width];
		sRGB8 colourTemp = colourBuffer[x + y * width];
		unsigned short alphaTemp = alphaBuffer16[x + y * width];
		unsigned short opacityTemp = opacityBuffer[x + y * width];

		for (int yy = 0; yy < pFactor; yy++)
		{
			for (int xx = 0; xx < pFactor; xx++)
			{
				if (xx == 0 && yy == 0) continue;
				imageFloat[x + xx + (y + yy) * width] = pixelTemp;
				zBuffer[x + xx + (y + yy) * width] = zBufferTemp;
				colourBuffer[x + xx + (y + yy) * width] = colourTemp;
				alphaBuffer16[x + xx + (y + yy) * width] = alphaTemp;
				opacityBuffer[x + xx + (y + yy) * width] = opacityTemp;
			}
		}
	}
}

void cImage::PutPixelAlfa(int x, int y, float z, sRGB8 color, sRGBfloat opacity, int layer)
{
	if (x >= 0 && x < previewWidth && y >= 0 && y < previewHeight)
	{
		size_t address = x + y * previewWidth;
		float zImage = GetPixelZBuffer(x / previewScale, y / previewScale);
		if (z > zImage)
		{
			opacity.R *= float(0.03);
			opacity.G *= float(0.03);
			opacity.B *= float(0.03);
		}
		sRGB8 oldPixel;
		if (layer == 0)
			oldPixel = preview[address];
		else if (layer == 1)
			oldPixel = preview2[address];
		sRGB8 newPixel;
		newPixel.R = (oldPixel.R * (1.0 - opacity.R) + color.R * opacity.R);
		newPixel.G = (oldPixel.G * (1.0 - opacity.G) + color.G * opacity.G);
		newPixel.B = (oldPixel.B * (1.0 - opacity.B) + color.B * opacity.B);
		if (layer == 0)
		{
			preview[address] = newPixel;
			preview2[address] = newPixel;
		}
		else if (layer == 1)
			preview2[address] = newPixel;
	}
}

void cImage::AntiAliasedPoint(
	double x, double y, float z, sRGB8 color, sRGBfloat opacity, int layer)
{
	double deltaX = x - (int)x;
	double deltaY = y - (int)y;

	double intensity1 = (1.0 - deltaX) * (1.0 - deltaY);
	double intensity2 = (deltaX) * (1.0 - deltaY);
	double intensity3 = (1.0 - deltaX) * (deltaY);
	double intensity4 = (deltaX) * (deltaY);
	double sum = intensity1 + intensity2 + intensity3 + intensity4;

	sRGBfloat opacity2;

	opacity2.R = opacity.R * intensity1 / sum;
	opacity2.G = opacity.G * intensity1 / sum;
	opacity2.B = opacity.B * intensity1 / sum;
	PutPixelAlfa(x, y, z, color, opacity2, layer);

	opacity2.R = opacity.R * intensity2 / sum;
	opacity2.G = opacity.G * intensity2 / sum;
	opacity2.B = opacity.B * intensity2 / sum;
	PutPixelAlfa(x + 1, y, z, color, opacity2, layer);

	opacity2.R = opacity.R * intensity3 / sum;
	opacity2.G = opacity.G * intensity3 / sum;
	opacity2.B = opacity.B * intensity3 / sum;
	PutPixelAlfa(x, y + 1, z, color, opacity2, layer);

	opacity2.R = opacity.R * intensity4 / sum;
	opacity2.G = opacity.G * intensity4 / sum;
	opacity2.B = opacity.B * intensity4 / sum;
	PutPixelAlfa(x + 1, y + 1, z, color, opacity2, layer);
}

void cImage::AntiAliasedLine(double x1, double y1, double x2, double y2, float z1, float z2,
	sRGB8 color, sRGBfloat opacity, int layer)
{
	if ((x1 >= 0 && x1 < previewWidth && y1 >= 0 && y1 < previewHeight)
			|| (x2 >= 0 && x2 < previewWidth && y2 >= 0 && y2 < previewHeight))
	{
		double deltaX = x2 - x1;
		double deltaY = y2 - y1;
		if (deltaX != 0 || deltaY != 0)
		{
			double a_deltaX = fabs(deltaX);
			double a_deltaY = fabs(deltaY);

			double A = y1 - y2;
			double B = x2 - x1;
			double C = y2 * x1 - x2 * y1;
			double denominator = 1.0 / sqrt(A * A + B * B);

			if (a_deltaX > a_deltaY)
			{
				x1 += 0.5;
				x2 += 0.5;
			}
			else
			{
				y1 += 0.5;
				y2 += 0.5;
			}

			if (a_deltaX > a_deltaY)
			{
				double k = (y2 - y1) / (x2 - x1);
				float kz = (z2 - z1) / (x2 - x1);

				double xx1, xx2;
				if (x1 < x2)
				{
					xx1 = x1;
					xx2 = x2;
				}
				else
				{
					xx1 = x2;
					xx2 = x1;
				}

				int start = xx1;
				if (start < 0) start = 0;
				int end = xx2;
				if (end > previewWidth) end = previewWidth;

				for (int intX = start; intX <= end; intX++)
				{
					double x = intX;
					double y = k * (x - x1) + y1;
					float z = kz * (x - x1) + z1;
					int xx = 0.5 + x;
					for (double d = -1; d <= 1; d++)
					{
						int yy = 0.5 + y + d;
						double distance = 1.0 * fabs(A * x + B * yy + C) * denominator;
						if (distance >= 1.0) distance = 1.0;
						double opacity2;
						opacity2 = (1.0 - distance);
						if (intX == start)
						{
							opacity2 = (1.0 - (xx1 - x)) * (1.0 - distance);
						}
						if (intX == end)
						{
							opacity2 = ((xx2 - x)) * (1.0 - distance);
						}
						sRGBfloat opacity3;
						opacity3.R = opacity2 * opacity.R;
						opacity3.G = opacity2 * opacity.G;
						opacity3.B = opacity2 * opacity.B;
						PutPixelAlfa(xx, yy, z, color, opacity3, layer);
					}
				}
			}
			else
			{
				double k = (x2 - x1) / (y2 - y1);
				float kz = (z2 - z1) / (y2 - y1);
				double yy1, yy2;
				if (y1 < y2)
				{
					yy1 = y1;
					yy2 = y2;
				}
				else
				{
					yy1 = y2;
					yy2 = y1;
				}

				int start = yy1;
				if (start < 0) start = 0;
				int end = yy2;
				if (end > previewHeight) end = previewHeight;

				for (int intY = start; intY <= end; intY++)
				{
					double y = intY;
					double x = k * (y - y1) + x1;
					float z = kz * (y - y1) + z1;
					int yy = 0.5 + y;
					for (int d = -1; d <= 1; d++)
					{
						int xx = 0.5 + x + d;
						double distance = fabs(A * xx + B * y + C) * denominator;
						if (distance >= 1.0) distance = 1.0;
						double opacity2;
						opacity2 = (1.0 - distance);
						if (intY == start)
						{
							opacity2 = (1.0 - (yy1 - (y))) * (1.0 - distance);
						}
						if (intY == end)
						{
							opacity2 = ((yy2 - (y))) * (1.0 - distance);
						}
						sRGBfloat opacity3;
						opacity3.R = opacity2 * opacity.R;
						opacity3.G = opacity2 * opacity.G;
						opacity3.B = opacity2 * opacity.B;
						PutPixelAlfa(xx, yy, z, color, opacity3, layer);
					}
				}
			}
		}
	}
}

void cImage::CircleBorder(double x, double y, float z, double r, sRGB8 border, double borderWidth,
	sRGBfloat opacity, int layer)
{
	if (borderWidth > 0 && r > 0)
	{
		double r2 = r + borderWidth;
		double r1 = r - borderWidth;
		if (r1 < 0) r1 = 0;
		int y1 = y - r2;
		int y2 = y + r2;

		double wspJ = 1.0 / borderWidth;

		for (int yy = y1; yy <= y2; yy++)
		{
			double dyy = yy - y;
			double dxx = r2 * r2 - dyy * dyy;
			if (dxx < 0) dxx = 0;
			double dx = sqrt(dxx);
			int x1 = x - dx;
			int x2 = x + dx;

			double dxx2 = r1 * r1 - dyy * dyy;
			if (dxx2 < 0) dxx2 = 0;
			double dx2 = sqrt(dxx2);
			int x12 = x - dx2;
			int x22 = x + dx2;

			for (int xx = x1; xx <= x2; xx++)
			{
				if (xx <= x12 || xx >= x22)
				{
					double deltaX = xx - x;
					double deltaY = yy - y;
					double rr = sqrt(deltaX * deltaX + deltaY * deltaY);
					double deltaR = fabs(rr - r);
					if (deltaR > borderWidth) deltaR = borderWidth;
					double opacity2 = wspJ * (borderWidth - deltaR);
					sRGBfloat opacity3;
					opacity3.R = opacity2 * opacity.R;
					opacity3.G = opacity2 * opacity.G;
					opacity3.B = opacity2 * opacity.B;
					PutPixelAlfa(xx, yy, z, border, opacity3, layer);
				}
			}
		}
	}
}
