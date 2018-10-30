/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <qpainter.h>

#include <QtCore>

#include "common_math.h"
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

	// optional image buffers

	AllocMem();
	progressiveFactor = 1;
	imageWidget = nullptr;
	isUsed = false;
	previewScale = 1.0;
	previewVisibleWidth = 1;
	previewVisibleHeight = 1;
	previewWidth = 0;
	previewHeight = 0;

	gammaTablePrepared = false;

	isStereoLeftRight = false;
	isMainImage = false;

	fastPreview = false;
}

cImage::~cImage()
{
	isAllocated = false;
	// qDebug() << "cImage::~cImage()";
	FreeImage();
}

bool cImage::AllocMem()
{
	isAllocated = false;
	// qDebug() << "bool cImage::AllocMem(void)";
	if (width > 0 && height > 0)
	{
		if (!allocLater)
		{
			try
			{
				imageFloat.reset(new sRGBFloat[quint64(width) * quint64(height)]);
				postImageFloat.reset(new sRGBFloat[quint64(width) * quint64(height)]);
				image16.reset(new sRGB16[quint64(width) * quint64(height)]);
				image8.reset(new sRGB8[quint64(width) * quint64(height)]);
				zBuffer.reset(new float[quint64(width) * quint64(height)]);
				alphaBuffer8.reset(new quint8[quint64(width) * quint64(height)]);
				alphaBuffer16.reset(new quint16[quint64(width) * quint64(height)]);
				opacityBuffer.reset(new quint16[quint64(width) * quint64(height)]);
				colourBuffer.reset(new sRGB8[quint64(width) * quint64(height)]);
				if (opt.optionalNormal) AllocRGB(normalFloat, normal16, normal8);
				ClearImage();
			}
			catch (std::bad_alloc &ba)
			{
				width = 0;
				height = 0;
				FreeImage();
				qCritical() << "bad_alloc caught in cImage: " << ba.what()
										<< ", maybe required image dimension to big?";
				return false;
			}
		}
	}
	else
	{
		fprintf(stderr, "Error! Cannot allocate memory for image (wrong image size)\n");
	}

	preview.reset();
	preview2.reset();
	previewAllocated = false;

	isAllocated = true;
	return true;
}

void cImage::AllocRGB(QScopedArrayPointer<sRGBFloat> &rgbFloat, QScopedArrayPointer<sRGB16> &rgb16,
	QScopedArrayPointer<sRGB8> &rgb8)
{
	rgbFloat.reset(new sRGBFloat[quint64(width) * quint64(height)]);
	rgb16.reset(new sRGB16[quint64(width) * quint64(height)]);
	rgb8.reset(new sRGB8[quint64(width) * quint64(height)]);
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

void cImage::ClearImage()
{
	memset(imageFloat.data(), 0, sizeof(sRGBFloat) * quint64(width) * quint64(height));
	memset(postImageFloat.data(), 0, sizeof(sRGBFloat) * quint64(width) * quint64(height));
	memset(image16.data(), 0, sizeof(sRGB16) * quint64(width) * quint64(height));
	memset(image8.data(), 0, sizeof(sRGB8) * quint64(width) * quint64(height));
	memset(alphaBuffer8.data(), 0, sizeof(quint8) * quint64(width) * quint64(height));
	memset(alphaBuffer16.data(), 0, sizeof(quint16) * quint64(width) * quint64(height));
	memset(opacityBuffer.data(), 0, sizeof(quint16) * quint64(width) * quint64(height));
	memset(colourBuffer.data(), 0, sizeof(sRGB8) * quint64(width) * quint64(height));

	if (opt.optionalNormal) ClearRGB(normalFloat, normal16, normal8);

	for (quint64 i = 0; i < quint64(width) * quint64(height); ++i)
		zBuffer[i] = float(1e20);
}

void cImage::ClearRGB(QScopedArrayPointer<sRGBFloat> &rgbFloat, QScopedArrayPointer<sRGB16> &rgb16,
	QScopedArrayPointer<sRGB8> &rgb8)
{
	if (rgbFloat) memset(rgbFloat.data(), 0, sizeof(sRGBFloat) * quint64(width) * quint64(height));
	if (rgb16) memset(rgb16.data(), 0, sizeof(sRGB16) * quint64(width) * quint64(height));
	if (rgb8) memset(rgb8.data(), 0, sizeof(sRGB8) * quint64(width) * quint64(height));
}

void cImage::FreeImage()
{
	isAllocated = false;
	// qDebug() << "void cImage::FreeImage(void)";
	imageFloat.reset();
	image16.reset();
	image8.reset();
	postImageFloat.reset();
	alphaBuffer8.reset();
	alphaBuffer16.reset();
	opacityBuffer.reset();
	colourBuffer.reset();
	zBuffer.reset();
	FreeRGB(normalFloat, normal16, normal8);

	gammaTable.reset();
	gammaTablePrepared = false;
}

void cImage::FreeRGB(QScopedArrayPointer<sRGBFloat> &rgbFloat, QScopedArrayPointer<sRGB16> &rgb16,
	QScopedArrayPointer<sRGB8> &rgb8)
{
	rgbFloat.reset();
	rgb16.reset();
	rgb8.reset();
}

sRGB16 cImage::CalculatePixel(sRGBFloat pixel)
{
	CalculateGammaTable();
	float R = pixel.R * adj.brightness;
	float G = pixel.G * adj.brightness;
	float B = pixel.B * adj.brightness;

	R = (R - 0.5f) * adj.contrast + 0.5f;
	G = (G - 0.5f) * adj.contrast + 0.5f;
	B = (B - 0.5f) * adj.contrast + 0.5f;

	if (R < 0.0f) R = 0.0f;
	if (G < 0.0f) G = 0.0f;
	if (B < 0.0f) B = 0.0f;

	if (adj.hdrEnabled)
	{
		R = tanhf(R);
		G = tanhf(G);
		B = tanhf(B);
	}

	// saturation
	float const rFactor = 0.299f;
	float const gFactor = .587f;
	float const bFactor = .114f;
	double V = sqrt(R * R * rFactor + G * G * gFactor + B * B * bFactor);
	R = V + (R - V) * adj.saturation;
	G = V + (G - V) * adj.saturation;
	B = V + (B - V) * adj.saturation;

	R = clamp(R, 0.0f, 1.0f);
	G = clamp(G, 0.0f, 1.0f);
	B = clamp(B, 0.0f, 1.0f);

	sRGB16 newPixel16;

	newPixel16.R = quint16(R * 65535.0f);
	newPixel16.G = quint16(G * 65535.0f);
	newPixel16.B = quint16(B * 65535.0f);
	newPixel16.R = quint16(gammaTable[newPixel16.R]);
	newPixel16.G = quint16(gammaTable[newPixel16.G]);
	newPixel16.B = quint16(gammaTable[newPixel16.B]);

	return newPixel16;
}

void cImage::CalculateGammaTable()
{
	if (!gammaTablePrepared)
	{
		gammaTable.reset(new int[65536]);

		for (int i = 0; i < 65536; i++)
		{
			gammaTable[i] = int(powf(i / 65536.0f, 1.0f / adj.imageGamma) * 65535.0f);
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
			qint64 address = qint64(x) + qint64(y) * width;
			sRGBFloat pixel = postImageFloat[address];
			sRGB16 newPixel16 = CalculatePixel(pixel);
			image16[address] = newPixel16;
		}
	}
}

void cImage::CompileImage(const QList<QRect> *list)
{
	if (imageFloat && postImageFloat)
	{
		for (auto rect : *list)
		{
			for (int y = rect.top(); y <= rect.bottom(); y++)
			{
				for (int x = rect.left(); x <= rect.right(); x++)
				{
					qint64 address = qint64(x) + qint64(y) * width;
					sRGBFloat pixel = postImageFloat[address];
					sRGB16 newPixel16 = CalculatePixel(pixel);
					image16[address] = newPixel16;
				}
			}
		}
	}
}

int cImage::GetUsedMB() const
{
	quint64 mb;

	quint64 zBufferSize = quint64(width) * quint64(height) * sizeof(float);
	quint64 alphaSize16 = quint64(width) * quint64(height) * sizeof(quint16);
	quint64 alphaSize8 = quint64(width) * quint64(height) * sizeof(quint8);
	quint64 imageFloatSize = quint64(width) * quint64(height) * sizeof(sRGBFloat);
	quint64 image16Size = quint64(width) * quint64(height) * sizeof(sRGB16);
	quint64 image8Size = quint64(width) * quint64(height) * sizeof(sRGB8);
	quint64 colorSize = quint64(width) * quint64(height) * sizeof(sRGB8);
	quint64 opacitySize = quint64(width) * quint64(height) * sizeof(quint16);
	quint64 optionalSize = 0;
	if (opt.optionalNormal)
		optionalSize +=
			quint64(width) * quint64(height) * (sizeof(sRGBFloat) + sizeof(sRGB16) + sizeof(sRGB8));

	mb = (zBufferSize + alphaSize16 + alphaSize8 + image16Size + image8Size + imageFloatSize * 2
				 + colorSize + opacitySize + optionalSize)
			 / 1024 / 1024;

	return int(mb);
}

void cImage::SetImageParameters(sImageAdjustments adjustments)
{
	adj = adjustments;
	gammaTablePrepared = false;
	CalculateGammaTable();
}

quint8 *cImage::ConvertTo8bit()
{
	for (quint64 i = 0; i < quint64(width) * quint64(height); i++)
	{
		image8[i].R = image16[i].R / 256;
		image8[i].G = image16[i].G / 256;
		image8[i].B = image16[i].B / 256;
	}
	return reinterpret_cast<quint8 *>(image8.data());
}

quint8 *cImage::ConvertTo8bit(const QList<QRect> *list)
{
	for (auto rect : *list)
	{
		{
			for (int y = rect.top(); y <= rect.bottom(); y++)
			{
				for (int x = rect.left(); x <= rect.right(); x++)
				{
					qint64 address = qint64(x) + qint64(y) * width;
					image8[address].R = image16[address].R / 256;
					image8[address].G = image16[address].G / 256;
					image8[address].B = image16[address].B / 256;
				}
			}
		}
	}
	return reinterpret_cast<quint8 *>(image8.data());
}

quint8 *cImage::ConvertAlphaTo8bit()
{
	for (quint64 i = 0; i < quint64(width) * quint64(height); i++)
	{
		alphaBuffer8[i] = alphaBuffer16[i] / 256;
	}
	return alphaBuffer8.data();
}

quint8 *cImage::ConvertGenericRGBTo8bit(
	QScopedArrayPointer<sRGBFloat> &from, QScopedArrayPointer<sRGB8> &to)
{
	for (quint64 i = 0; i < quint64(width) * quint64(height); i++)
	{
		to[i].R = quint8(from[i].R * 255.0f);
		to[i].G = quint8(from[i].G * 255.0f);
		to[i].B = quint8(from[i].B * 255.0f);
	}
	return reinterpret_cast<quint8 *>(to.data());
}

quint8 *cImage::ConvertGenericRGBTo16bit(
	QScopedArrayPointer<sRGBFloat> &from, QScopedArrayPointer<sRGB16> &to)
{
	for (quint64 i = 0; i < quint64(width) * quint64(height); i++)
	{
		to[i].R = quint16(from[i].R * 65535.0f);
		to[i].G = quint16(from[i].G * 65535.0f);
		to[i].B = quint16(from[i].B * 65535.0f);
	}
	return reinterpret_cast<quint8 *>(to.data());
}

quint8 *cImage::ConvertNormalTo16Bit()
{
	if (!opt.optionalNormal) return nullptr;
	return ConvertGenericRGBTo16bit(normalFloat, normal16);
}

quint8 *cImage::ConvertNormalTo8Bit()
{
	if (!opt.optionalNormal) return nullptr;
	return ConvertGenericRGBTo8bit(normalFloat, normal8);
}

sRGB8 cImage::Interpolation(float x, float y) const
{
	sRGB8 colour = sRGB8(0, 0, 0);
	if (x >= 0 && x < width - 1 && y >= 0 && y < height - 1)
	{
		int ix = int(x);
		int iy = int(y);
		int rx = int((x - ix) * 256);
		int ry = int((y - iy) * 256);
		int rxi = 255 - rx;
		int ryi = 255 - ry;
		int rxRy = rx * ry;
		int rxRyi = rx * ryi;
		int rxiRy = rxi * ry;
		int rxiRyi = rxi * ryi;

		sRGB8 k1 = image8[iy * width + ix];
		sRGB8 k2 = image8[iy * width + ix + 1];
		sRGB8 k3 = image8[(iy + 1) * width + ix];
		sRGB8 k4 = image8[(iy + 1) * width + ix + 1];
		colour.R = quint8((k1.R * rxiRyi + k2.R * rxRyi + k3.R * rxiRy + k4.R * rxRy) / 65536);
		colour.G = quint8((k1.G * rxiRyi + k2.G * rxRyi + k3.G * rxiRy + k4.G * rxRy) / 65536);
		colour.B = quint8((k1.B * rxiRyi + k2.B * rxRyi + k3.B * rxiRy + k4.B * rxRy) / 65536);
	}
	return colour;
}

quint8 *cImage::CreatePreview(
	double scale, int visibleWidth, int visibleHeight, QWidget *widget = nullptr)
{
	previewMutex.lock();
	int w = int(width * scale);
	int h = int(height * scale);

	if (w != previewWidth || h != previewHeight || !previewAllocated)
	{

		previewVisibleWidth = visibleWidth;
		previewVisibleHeight = visibleHeight;

		preview.reset(new sRGB8[w * h]);
		preview2.reset(new sRGB8[w * h]);

		memset(preview.data(), 0, sizeof(sRGB8) * quint64(w) * quint64(h));
		memset(preview2.data(), 0, sizeof(sRGB8) * quint64(w) * quint64(h));
		previewAllocated = true;
		previewWidth = w;
		previewHeight = h;
		previewScale = scale;
	}

	if (widget) imageWidget = widget;

	quint8 *ptr = reinterpret_cast<quint8 *>(preview.data());

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
			memcpy(preview.data(), image8.data(), quint64(width) * quint64(height) * sizeof(sRGB8));
		}
		else
		{
			float scaleX = float(width) / w;
			float scaleY = float(height) / h;

			// number of pixels to sum
			int countX = int(float(width) / w + 1);
			int countY = int(float(height) / h + 1);
			int factor = countX * countY;

			float deltaX = scaleX / countX;
			float deltaY = scaleY / countY;

			int listIndex = 0;

			for (int y = 0; y < int(h); y++)
			{

				if (list)
				{
					if (listIndex >= list->size()) break;

					if (scaleY * y < list->at(listIndex) - int(scaleY) - 1) continue;

					while (scaleY * y - int(scaleY) - 1 > list->at(listIndex))
					{
						listIndex++;
						if (listIndex >= list->size()) break;
					}
				}

				for (int x = 0; x < int(w); x++)
				{
					if (fastPreview)
					{
						qint64 xx = x * scaleX;
						qint64 yy = y * scaleY;
						preview[quint64(x) + quint64(y) * quint64(w)] = image8[yy * width + xx];
					}
					else
					{
						int R = 0;
						int G = 0;
						int B = 0;
						for (int j = 0; j < countY; j++)
						{
							float yy = y * scaleY + j * deltaY - 0.5;

							for (int i = 0; i < countX; i++)
							{
								float xx = x * scaleX + i * deltaX - 0.5;
								if (xx > 0 && xx < width - 1 && yy > 0 && yy < height - 1)
								{
									sRGB8 oldPixel = Interpolation(xx, yy);
									R += oldPixel.R;
									G += oldPixel.G;
									B += oldPixel.B;
								}
							} // next i
						}		// next j
						sRGB8 newPixel;
						newPixel.R = quint8(R / factor);
						newPixel.G = quint8(G / factor);
						newPixel.B = quint8(B / factor);
						preview[quint64(x) + quint64(y) * quint64(w)] = newPixel;
					}
				} // next x
			}		// next y
		}
		memcpy(preview2.data(), preview.data(), quint64(w) * quint64(h) * sizeof(sRGB8));
		previewMutex.unlock();
	}
	else
	{
		fprintf(stderr, "Error! Preview has not been prepared\n");
	}
}

void cImage::UpdatePreview(const QList<QRect> *list)
{
	if (previewAllocated && !allocLater)
	{
		previewMutex.lock();
		int w = previewWidth;
		int h = previewHeight;

		if (width == w && height == h)
		{
			memcpy(preview.data(), image8.data(), quint64(width) * quint64(height) * sizeof(sRGB8));
		}
		else
		{
			for (auto rect : *list)
			{
				float scaleX = float(width) / w;
				float scaleY = float(height) / h;

				// number of pixels to sum
				int countX = int(float(width) / w + 1);
				int countY = int(float(height) / h + 1);
				int factor = countX * countY;

				float deltaX = scaleX / countX;
				float deltaY = scaleY / countY;

				int xStart = rect.left() / scaleX;
				xStart = qMax(xStart, 0);
				int xEnd = (rect.right() + 1) / scaleX;
				xEnd = qMin(xEnd, w - 1);
				int yStart = rect.top() / scaleY;
				yStart = qMax(yStart, 0);
				int yEnd = (rect.bottom() + 1) / scaleY;
				yEnd = qMin(yEnd, h - 1);

				//#ifndef _WIN32
				//#pragma omp parallel for
				//#endif
				for (int y = yStart; y <= yEnd; y++)
				{
					for (int x = xStart; x <= xEnd; x++)
					{
						if (fastPreview)
						{
							qint64 xx = x * scaleX;
							qint64 yy = y * scaleY;
							preview[quint64(x) + quint64(y) * quint64(w)] = image8[yy * width + xx];
						}
						else
						{
							int R = 0;
							int G = 0;
							int B = 0;
							for (int j = 0; j < countY; j++)
							{
								float yy = y * scaleY + j * deltaY - 0.5;

								for (int i = 0; i < countX; i++)
								{
									float xx = x * scaleX + i * deltaX - 0.5;
									if (xx > 0 && xx < width - 1 && yy > 0 && yy < height - 1)
									{
										sRGB8 oldPixel = Interpolation(xx, yy);
										R += oldPixel.R;
										G += oldPixel.G;
										B += oldPixel.B;
									}
								} // next i
							}		// next j
							sRGB8 newPixel;
							newPixel.R = quint8(R / factor);
							newPixel.G = quint8(G / factor);
							newPixel.B = quint8(B / factor);
							preview[quint64(x) + quint64(y) * quint64(w)] = newPixel;
						}
					} // next x
				}		// next y
			}
		}

		memcpy(preview2.data(), preview.data(), quint64(w) * quint64(h) * sizeof(sRGB8));
		previewMutex.unlock();
	}
	else
	{
		fprintf(stderr, "Error! Preview has not been prepared\n");
	}
}

quint8 *cImage::GetPreviewPtr() const
{
	quint8 *ptr;
	if (previewAllocated)
	{
		ptr = reinterpret_cast<quint8 *>(preview2.data());
	}
	else
		abort();
	return ptr;
}

quint8 *cImage::GetPreviewPrimaryPtr() const
{
	quint8 *ptr;
	if (previewAllocated)
	{
		ptr = reinterpret_cast<quint8 *>(preview.data());
	}
	else
		abort();
	return ptr;
}

bool cImage::IsPreview() const
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

void cImage::RedrawInWidget(QWidget *qWidget)
{
	if (IsPreview())
	{
		// qDebug() << "Redraw" << endl;
		previewMutex.lock();

		QWidget *widget;
		if (qWidget)
			widget = qWidget;
		else
			widget = imageWidget;

		QPainter painter(widget);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

		QImage qImage(GetPreviewPtr(), int(previewWidth), int(previewHeight),
			int(previewWidth * sizeof(sRGB8)), QImage::Format_RGB888);
		painter.drawImage(QRect(0, 0, int(previewWidth), int(previewHeight)), qImage,
			QRect(0, 0, int(previewWidth), int(previewHeight)));
		memcpy(preview2.data(), preview.data(),
			quint64(previewWidth) * quint64(previewHeight) * sizeof(sRGB8));
		previewMutex.unlock();
	}
}

void cImage::Squares(int y, int pFactor)
{
	progressiveFactor = pFactor;
	for (int x = 0; x <= width - pFactor; x += pFactor)
	{
		quint64 ptr = quint64(x) + quint64(y) * quint64(width);
		sRGBFloat pixelTemp = imageFloat[ptr];
		sRGBFloat postPixelTemp = postImageFloat[ptr];
		float zBufferTemp = zBuffer[ptr];
		sRGB8 colourTemp = colourBuffer[ptr];
		quint16 alphaTemp = alphaBuffer16[ptr];
		quint16 opacityTemp = opacityBuffer[ptr];

		for (int yy = 0; yy < pFactor; yy++)
		{
			for (int xx = 0; xx < pFactor; xx++)
			{
				if (xx == 0 && yy == 0) continue;
				quint64 ptr2 = quint64(x + xx) + quint64(y + yy) * quint64(width);
				imageFloat[ptr2] = pixelTemp;
				postImageFloat[ptr2] = postPixelTemp;
				zBuffer[ptr2] = zBufferTemp;
				colourBuffer[ptr2] = colourTemp;
				alphaBuffer16[ptr2] = alphaTemp;
				opacityBuffer[ptr2] = opacityTemp;
			}
		}
	}
}

void cImage::PutPixelAlfa(qint64 x, qint64 y, float z, sRGB8 color, sRGBFloat opacity, int layer)
{
	if (x >= 0 && x < previewWidth && y >= 0 && y < previewHeight)
	{
		quint64 address = quint64(x) + quint64(y) * quint64(previewWidth);
		float zImage = GetPixelZBuffer(int(x / previewScale), int(y / previewScale));
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
		newPixel.R = quint8((oldPixel.R * (1.0f - opacity.R) + color.R * opacity.R));
		newPixel.G = quint8((oldPixel.G * (1.0f - opacity.G) + color.G * opacity.G));
		newPixel.B = quint8((oldPixel.B * (1.0f - opacity.B) + color.B * opacity.B));
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
	double x, double y, float z, sRGB8 color, sRGBFloat opacity, int layer)
{
	double deltaX = x - int(x);
	double deltaY = y - int(y);

	double intensity1 = (1.0 - deltaX) * (1.0 - deltaY);
	double intensity2 = (deltaX) * (1.0 - deltaY);
	double intensity3 = (1.0 - deltaX) * (deltaY);
	double intensity4 = (deltaX) * (deltaY);
	double sum = intensity1 + intensity2 + intensity3 + intensity4;

	sRGBFloat opacity2;

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
	sRGB8 color, sRGBFloat opacity, int layer)
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
						sRGBFloat opacity3;
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
						sRGBFloat opacity3;
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
	sRGBFloat opacity, int layer)
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
					sRGBFloat opacity3;
					opacity3.R = opacity2 * opacity.R;
					opacity3.G = opacity2 * opacity.G;
					opacity3.B = opacity2 * opacity.B;
					PutPixelAlfa(xx, yy, z, border, opacity3, layer);
				}
			}
		}
	}
}

void cImage::NullPostEffect(QList<int> *list)
{
	if (imageFloat && postImageFloat)
	{
		if (!list)
		{
			memcpy(postImageFloat.data(), imageFloat.data(),
				sizeof(sRGBFloat) * quint64(width) * quint64(height));
		}
		else
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

				memcpy(
					&postImageFloat[y * width], &imageFloat[y * width], sizeof(sRGBFloat) * quint64(width));
			}
		}
	}
}

void cImage::NullPostEffect(const QList<QRect> *list)
{
	if (imageFloat && postImageFloat)
	{
		for (auto rect : *list)
		{
			for (int y = rect.top(); y <= rect.bottom(); y++)
			{
				memcpy(&postImageFloat[y * width + rect.left()], &imageFloat[y * width + rect.left()],
					sizeof(sRGBFloat) * quint64(rect.width()));
			}
		}
	}
}

void cImage::GetStereoLeftRightImages(cImage *left, cImage *right)
{
	if (isStereoLeftRight && left && right)
	{
		int halfWidth = width / 2;
		left->ChangeSize(halfWidth, height, opt);
		right->ChangeSize(halfWidth, height, opt);

		for (qint64 y = 0; y < height; y++)
		{
			for (qint64 x = 0; x < halfWidth; x++)
			{
				qint64 ptrNew = x + y * halfWidth;
				qint64 ptrLeft = x + y * width;
				qint64 ptrRight = (x + halfWidth) + y * width;

				left->image8[ptrNew] = image8[ptrLeft];
				right->image8[ptrNew] = image8[ptrRight];

				left->image16[ptrNew] = image16[ptrLeft];
				right->image16[ptrNew] = image16[ptrRight];

				left->imageFloat[ptrNew] = imageFloat[ptrLeft];
				right->imageFloat[ptrNew] = imageFloat[ptrRight];

				left->postImageFloat[ptrNew] = postImageFloat[ptrLeft];
				right->postImageFloat[ptrNew] = postImageFloat[ptrRight];

				left->alphaBuffer8[ptrNew] = alphaBuffer8[ptrLeft];
				right->alphaBuffer8[ptrNew] = alphaBuffer8[ptrRight];

				left->alphaBuffer16[ptrNew] = alphaBuffer16[ptrLeft];
				right->alphaBuffer16[ptrNew] = alphaBuffer16[ptrRight];

				left->opacityBuffer[ptrNew] = opacityBuffer[ptrLeft];
				right->opacityBuffer[ptrNew] = opacityBuffer[ptrRight];

				left->colourBuffer[ptrNew] = colourBuffer[ptrLeft];
				right->colourBuffer[ptrNew] = colourBuffer[ptrRight];

				left->zBuffer[ptrNew] = zBuffer[ptrLeft];
				right->zBuffer[ptrNew] = zBuffer[ptrRight];

				if (opt.optionalNormal)
				{
					left->normalFloat[ptrNew] = normalFloat[ptrLeft];
					right->normalFloat[ptrNew] = normalFloat[ptrRight];

					left->normal8[ptrNew] = normal8[ptrLeft];
					right->normal8[ptrNew] = normal8[ptrRight];

					left->normal16[ptrNew] = normal16[ptrLeft];
					right->normal16[ptrNew] = normal16[ptrRight];
				}
			}
		}
	}
}
