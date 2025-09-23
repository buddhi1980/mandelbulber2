/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "common_math.h"

cImage::cImage(int w, int h, bool _allocLater)
{
	isAllocated = false;
	width = quint64(w);
	height = quint64(h);
	allocLater = _allocLater;
	useResizeOnChangeSize = false;
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

cImage::cImage(cImage &source)
{
	isAllocated = source.isAllocated;
	image8 = source.image8;
	image16 = source.image16;
	imageFloat = source.imageFloat;
	postImageFloat = source.postImageFloat;

	alphaBuffer8 = source.alphaBuffer8;
	alphaBuffer16 = source.alphaBuffer16;
	opacityBuffer = source.opacityBuffer;
	colourBuffer = source.colourBuffer;
	zBuffer = source.zBuffer;

	normalFloat = source.normalFloat;
	normalFloatWorld = source.normalFloatWorld;
	specularFloat = source.specularFloat;
	diffuseFloat = source.diffuseFloat;
	worldFloat = source.worldFloat;
	shadows = source.shadows;
	globalIllumination = source.globalIllumination;
	notDenoised = source.notDenoised;

	previewAllocated = false;
	imageWidget = nullptr;

	adj = source.adj;
	opt = source.opt;
	width = source.width;
	height = source.height;
	gammaTable = source.gammaTable;
	previewWidth = 0;
	previewHeight = 0;
	previewScale = 1.0;
	dpiScale = 1.0;
	previewVisibleWidth = 0;
	previewVisibleHeight = 0;
	isMainImage = source.isMainImage;
	gammaTablePrepared = source.gammaTablePrepared;
	allocLater = source.allocLater;
	isStereoLeftRight = source.isStereoLeftRight;
	fastPreview = false;
	useResizeOnChangeSize = false;
	meta = source.meta;

	progressiveFactor = source.progressiveFactor;

	isUsed = false;
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
				imageFloat.resize(width * height);
				postImageFloat.resize(width * height);
				image16.resize(width * height);
				image8.resize(width * height);
				zBuffer.resize(width * height);
				alphaBuffer8.resize(width * height);
				alphaBuffer16.resize(width * height);
				opacityBuffer.resize(width * height);
				colourBuffer.resize(width * height);
				if (opt.optionalNormal) normalFloat.resize(width * height);
				if (opt.optionalNormalWorld) normalFloatWorld.resize(width * height);
				if (opt.optionalSpecular) specularFloat.resize(width * height);
				if (opt.optionalDiffuse) diffuseFloat.resize(width * height);
				if (opt.optionalWorld) worldFloat.resize(width * height);
				if (opt.optionalShadows) shadows.resize(width * height);
				if (opt.optionalGlobalIlluination) globalIllumination.resize(width * height);
				if (opt.optionalNotDenoised) notDenoised.resize(width * height);

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

	preview.clear();
	preview2.clear();
	previewAllocated = false;

	isAllocated = true;
	return true;
}

void cImage::AllocRGB(
	std::vector<sRGBFloat> &rgbFloat, std::vector<sRGB16> &rgb16, std::vector<sRGB8> &rgb8)
{
	rgbFloat.resize(width * height);
	rgb16.resize(width * height);
	rgb8.resize(width * height);
}

bool cImage::ChangeSize(quint64 w, quint64 h, sImageOptional optional)
{
	if (w != width || h != height || !(optional == *GetImageOptional()) || allocLater)
	{
		previewMutex.lock();
		std::vector<sRGBFloat> imageCopy;
		std::vector<float> imageCopyZBuffer;

		if (useResizeOnChangeSize)
		{
			imageCopy = imageFloat;
			imageCopyZBuffer = zBuffer;
		}
		quint64 oldWidth = width;
		quint64 oldHeight = height;

		width = quint64(w);
		height = quint64(h);
		SetImageOptional(optional);
		FreeImage();
		allocLater = false;
		bool result = AllocMem();

		if (useResizeOnChangeSize)
		{
			// fast image scaling
			float scaleX = float(oldWidth) / w;
			float scaleY = float(oldHeight) / h;

			for (quint64 y = 0; y < h; y++)
			{
				quint64 yy = y * scaleY;

				for (quint64 x = 0; x < w; x++)
				{
					quint64 xx = x * scaleX;
					PutPixelImage(x, y, imageCopy[yy * oldWidth + xx]);
					PutPixelPostImage(x, y, imageCopy[yy * oldWidth + xx]);
					PutPixelZBuffer(x, y, imageCopyZBuffer[yy * oldWidth + xx]);
				}
			}
		}
		previewMutex.unlock();
		if (useResizeOnChangeSize)
		{
			CompileImage();
			ConvertTo8bitChar();
		}

		return result;
	}
	return true;
}

void cImage::ClearImage()
{
	std::fill(imageFloat.begin(), imageFloat.end(), sRGBFloat());
	std::fill(postImageFloat.begin(), postImageFloat.end(), sRGBFloat());
	std::fill(image16.begin(), image16.end(), sRGB16());
	std::fill(image8.begin(), image8.end(), sRGB8());
	std::fill(alphaBuffer8.begin(), alphaBuffer8.end(), 0);
	std::fill(alphaBuffer16.begin(), alphaBuffer16.end(), 0);
	std::fill(opacityBuffer.begin(), opacityBuffer.end(), 0);
	std::fill(colourBuffer.begin(), colourBuffer.end(), sRGB8());

	if (opt.optionalNormal) std::fill(normalFloat.begin(), normalFloat.end(), sRGBFloat());
	if (opt.optionalNormalWorld)
		std::fill(normalFloatWorld.begin(), normalFloatWorld.end(), sRGBFloat());
	if (opt.optionalSpecular) std::fill(specularFloat.begin(), specularFloat.end(), sRGBFloat());
	if (opt.optionalDiffuse) std::fill(diffuseFloat.begin(), diffuseFloat.end(), sRGBFloat());
	if (opt.optionalWorld) std::fill(worldFloat.begin(), worldFloat.end(), sRGBFloat());
	if (opt.optionalShadows) std::fill(shadows.begin(), shadows.end(), sRGBFloat());
	if (opt.optionalGlobalIlluination)
		std::fill(globalIllumination.begin(), globalIllumination.end(), sRGBFloat());
	if (opt.optionalNotDenoised) std::fill(notDenoised.begin(), notDenoised.end(), sRGBFloat());

	for (quint64 i = 0; i < quint64(width) * quint64(height); ++i)
		zBuffer[i] = float(1e20);
}

void cImage::ClearRGB(
	std::vector<sRGBFloat> &rgbFloat, std::vector<sRGB16> &rgb16, std::vector<sRGB8> &rgb8)
{
	if (!rgbFloat.empty()) std::fill(rgbFloat.begin(), rgbFloat.end(), sRGBFloat());
	if (!rgb16.empty()) std::fill(rgb16.begin(), rgb16.end(), sRGB16());
	if (!rgb8.empty()) std::fill(rgb8.begin(), rgb8.end(), sRGB8());
}

void cImage::FreeImage()
{
	isAllocated = false;
	// qDebug() << "void cImage::FreeImage(void)";
	imageFloat.clear();
	image16.clear();
	image8.clear();
	postImageFloat.clear();
	alphaBuffer8.clear();
	alphaBuffer16.clear();
	opacityBuffer.clear();
	colourBuffer.clear();
	zBuffer.clear();

	normalFloat.clear();
	normalFloatWorld.clear();
	specularFloat.clear();
	diffuseFloat.clear();
	worldFloat.clear();
	shadows.clear();
	globalIllumination.clear();
	notDenoised.clear();

	gammaTable.clear();
	gammaTablePrepared = false;
}

void cImage::FreeRGB(
	std::vector<sRGBFloat> &rgbFloat, std::vector<sRGB16> &rgb16, std::vector<sRGB8> &rgb8)
{
	rgbFloat.clear();
	rgb16.clear();
	rgb8.clear();
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
	float V = sqrtf(R * R * rFactor + G * G * gFactor + B * B * bFactor);
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
		gammaTable.resize(65536);

		for (quint64 i = 0; i < 65536; i++)
		{
			gammaTable[i] = int(powf(i / 65536.0f, 1.0f / adj.imageGamma) * 65535.0f);
		}
		gammaTablePrepared = true;
	}
}

void cImage::CompileImage(QList<int> *list)
{
	int listIndex = 0;
	for (quint64 y = 0; y < height; y++)
	{
		if (list)
		{
			if (listIndex >= list->size()) break;
			y = quint64(list->at(listIndex));
			listIndex++;
		}
		for (quint64 x = 0; x < width; x++)
		{
			quint64 address = x + y * width;
			sRGBFloat pixel = postImageFloat[address];
			sRGB16 newPixel16 = CalculatePixel(pixel);
			image16[address] = newPixel16;
		}
	}
}

void cImage::CompileImage(const QList<QRect> *list)
{
	if (!imageFloat.empty() && !postImageFloat.empty())
	{
		for (auto rect : *list)
		{
			for (quint64 y = quint64(rect.top()); y <= quint64(rect.bottom()); y++)
			{
				for (quint64 x = quint64(rect.left()); x <= quint64(rect.right()); x++)
				{
					quint64 address = x + y * width;
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

	quint64 zBufferSize = width * height * sizeof(float);
	quint64 alphaSize16 = width * height * sizeof(quint16);
	quint64 alphaSize8 = width * height * sizeof(quint8);
	quint64 imageFloatSize = width * height * sizeof(sRGBFloat);
	quint64 image16Size = width * height * sizeof(sRGB16);
	quint64 image8Size = width * height * sizeof(sRGB8);
	quint64 colorSize = width * height * sizeof(sRGB8);
	quint64 opacitySize = width * height * sizeof(quint16);
	quint64 optionalSize = 0;

	quint64 optionalChannels = 0;
	if (opt.optionalNormal) optionalChannels++;
	if (opt.optionalNormalWorld) optionalChannels++;
	if (opt.optionalSpecular) optionalChannels++;
	if (opt.optionalDiffuse) optionalChannels++;
	if (opt.optionalWorld) optionalChannels++;
	if (opt.optionalShadows) optionalChannels++;
	if (opt.optionalGlobalIlluination) optionalChannels++;
	if (opt.optionalNotDenoised) optionalChannels++;

	optionalSize +=
		optionalChannels * width * height * (sizeof(sRGBFloat) + sizeof(sRGB16) + sizeof(sRGB8));

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

quint8 *cImage::ConvertTo8bitChar()
{
	for (quint64 i = 0; i < quint64(width) * quint64(height); i++)
	{
		image8[i].R = image16[i].R / 256;
		image8[i].G = image16[i].G / 256;
		image8[i].B = image16[i].B / 256;
	}
	return reinterpret_cast<quint8 *>(image8.data());
}

quint8 *cImage::ConvertTo8bitCharFromList(const QList<QRect> *list)
{
	for (auto rect : *list)
	{
		{
			for (quint64 y = quint64(rect.top()); y <= quint64(rect.bottom()); y++)
			{
				for (quint64 x = quint64(rect.left()); x <= quint64(rect.right()); x++)
				{
					quint64 address = x + y * width;
					image8[address].R = image16[address].R / 256;
					image8[address].G = image16[address].G / 256;
					image8[address].B = image16[address].B / 256;
				}
			}
		}
	}
	return reinterpret_cast<quint8 *>(image8.data());
}

std::vector<quint8> &cImage::ConvertAlphaTo8bit()
{
	for (quint64 i = 0; i < quint64(width) * quint64(height); i++)
	{
		alphaBuffer8[i] = alphaBuffer16[i] / 256;
	}
	return alphaBuffer8;
}

quint8 *cImage::ConvertGenericRGBTo8bitChar(std::vector<sRGBFloat> &from, std::vector<sRGB8> &to)
{
	for (quint64 i = 0; i < quint64(width) * quint64(height); i++)
	{
		to[i].R = quint8(qBound(0.0f, from[i].R * 255.0f, 255.0f));
		to[i].G = quint8(qBound(0.0f, from[i].G * 255.0f, 255.0f));
		to[i].B = quint8(qBound(0.0f, from[i].B * 255.0f, 255.0f));
	}
	return reinterpret_cast<quint8 *>(to.data());
}

quint8 *cImage::ConvertGenericRGBTo16bitWord(std::vector<sRGBFloat> &from, std::vector<sRGB16> &to)
{
	for (quint64 i = 0; i < quint64(width) * quint64(height); i++)
	{
		to[i].R = quint16(qBound(0.0f, from[i].R * 65535.0f, 65535.0f));
		to[i].G = quint16(qBound(0.0f, from[i].G * 65535.0f, 65535.0f));
		to[i].B = quint16(qBound(0.0f, from[i].B * 65535.0f, 65535.0f));
	}
	return reinterpret_cast<quint8 *>(to.data());
}

sRGB8 cImage::Interpolation(float x, float y) const
{
	sRGB8 colour = sRGB8(0, 0, 0);
	if (x >= 0 && x < width - 1 && y >= 0 && y < height - 1)
	{
		quint64 ix = quint64(x);
		quint64 iy = quint64(y);
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

	dpiScale = widget->devicePixelRatioF();

	quint64 w = quint64(width * scale * dpiScale);
	quint64 h = quint64(height * scale * dpiScale);

	if (w != previewWidth || h != previewHeight || !previewAllocated)
	{

		previewVisibleWidth = visibleWidth;
		previewVisibleHeight = visibleHeight;

		preview.resize(w * h);
		preview2.resize(w * h);

		std::fill(preview.begin(), preview.end(), sRGB8());
		std::fill(preview2.begin(), preview2.end(), sRGB8());

		previewAllocated = true;
		previewWidth = w;
		previewHeight = h;
		previewScale = scale * dpiScale;
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
		quint64 w = previewWidth;
		quint64 h = previewHeight;

		if (width == w && height == h)
		{
			preview = image8;
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

			for (quint64 y = 0; y < h; y++)
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

				for (quint64 x = 0; x < w; x++)
				{
					if (fastPreview)
					{
						quint64 xx = quint64(x * scaleX);
						quint64 yy = quint64(y * scaleY);
						preview[x + y * w] = image8[yy * width + xx];
					}
					else
					{
						int R = 0;
						int G = 0;
						int B = 0;
						for (int j = 0; j < countY; j++)
						{
							float yy = y * scaleY + j * deltaY - 0.5f;

							for (int i = 0; i < countX; i++)
							{
								float xx = x * scaleX + i * deltaX - 0.5f;
								if (xx > 0 && xx < width - 1 && yy > 0 && yy < height - 1)
								{
									sRGB8 oldPixel = Interpolation(xx, yy);
									R += oldPixel.R;
									G += oldPixel.G;
									B += oldPixel.B;
								}
							} // next i
						} // next j
						sRGB8 newPixel;
						newPixel.R = quint8(R / factor);
						newPixel.G = quint8(G / factor);
						newPixel.B = quint8(B / factor);
						preview[x + y * w] = newPixel;
					}
				} // next x
			} // next y
		}
		preview2 = preview;
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
		quint64 w = previewWidth;
		quint64 h = previewHeight;

		if (width == w && height == h)
		{
			preview = image8;
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

				quint64 xStart = quint64(rect.left() / scaleX);
				xStart = qMax(xStart, 0ULL);
				quint64 xEnd = quint64((rect.right() + 1) / scaleX);
				xEnd = qMin(xEnd, w - 1);
				quint64 yStart = quint64(rect.top() / scaleY);
				yStart = qMax(yStart, 0ULL);
				quint64 yEnd = quint64((rect.bottom() + 1) / scaleY);
				yEnd = qMin(yEnd, h - 1);

				// #ifndef _WIN32
				// #pragma omp parallel for
				// #endif
				for (quint64 y = yStart; y <= yEnd; y++)
				{
					for (quint64 x = xStart; x <= xEnd; x++)
					{
						if (fastPreview)
						{
							quint64 xx = quint64(x * scaleX);
							quint64 yy = quint64(y * scaleY);
							preview[x + y * w] = image8[yy * width + xx];
						}
						else
						{
							int R = 0;
							int G = 0;
							int B = 0;
							for (int j = 0; j < countY; j++)
							{
								float yy = y * scaleY + j * deltaY - 0.5f;

								for (int i = 0; i < countX; i++)
								{
									float xx = x * scaleX + i * deltaX - 0.5f;
									if (xx > 0 && xx < width - 1 && yy > 0 && yy < height - 1)
									{
										sRGB8 oldPixel = Interpolation(xx, yy);
										R += oldPixel.R;
										G += oldPixel.G;
										B += oldPixel.B;
									}
								} // next i
							} // next j
							sRGB8 newPixel;
							newPixel.R = quint8(R / factor);
							newPixel.G = quint8(G / factor);
							newPixel.B = quint8(B / factor);
							preview[x + y * w] = newPixel;
						}
					} // next x
				} // next y
			}
		}

		preview2 = preview;
		previewMutex.unlock();
	}
	else
	{
		fprintf(stderr, "Error! Preview has not been prepared\n");
	}
}

const quint8 *cImage::GetPreviewConstPtr() const
{
	const quint8 *ptr;
	if (previewAllocated)
	{
		ptr = reinterpret_cast<const quint8 *>(preview2.data());
	}
	else
		abort();
	return ptr;
}

const quint8 *cImage::GetPreviewPrimaryConstPtr() const
{
	const quint8 *ptr;
	if (previewAllocated)
	{
		ptr = reinterpret_cast<const quint8 *>(preview.data());
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

		QImage qImage(GetPreviewConstPtr(), int(previewWidth), int(previewHeight),
			int(previewWidth * sizeof(sRGB8)), QImage::Format_RGB888);

		painter.drawImage(QRect(0, 0, int(previewWidth / dpiScale), int(previewHeight / dpiScale)),
			qImage, QRect(0, 0, int(previewWidth), int(previewHeight)));

		preview2 = preview;
		previewMutex.unlock();
	}
}

void cImage::Squares(quint64 y, int pFactor)
{
	progressiveFactor = pFactor;
	quint64 pf = quint64(pFactor);
	for (quint64 x = 0; x <= width - quint64(pf); x += pf)
	{
		quint64 ptr = x + y * width;
		sRGBFloat pixelTemp = imageFloat[ptr];
		sRGBFloat postPixelTemp = postImageFloat[ptr];
		float zBufferTemp = zBuffer[ptr];
		sRGB8 colourTemp = colourBuffer[ptr];
		// quint16 alphaTemp = alphaBuffer16[ptr];
		quint16 opacityTemp = opacityBuffer[ptr];

		for (quint64 yy = 0; yy < pf; yy++)
		{
			for (quint64 xx = 0; xx < pf; xx++)
			{
				if (xx == 0 && yy == 0) continue;
				quint64 ptr2 = (x + xx) + (y + yy) * (width);
				imageFloat[ptr2] = pixelTemp;
				postImageFloat[ptr2] = postPixelTemp;
				zBuffer[ptr2] = zBufferTemp;
				colourBuffer[ptr2] = colourTemp;
				// alphaBuffer16[ptr2] = alphaTemp;
				opacityBuffer[ptr2] = opacityTemp;
			}
		}
	}
}

void cImage::PutPixelAlfa(quint64 x, quint64 y, float z, sRGB8 color, sRGBFloat opacity, int layer)
{
	if (x < previewWidth && y < previewHeight)
	{
		quint64 address = quint64(x) + quint64(y) * quint64(previewWidth);
		float zImage = GetPixelZBuffer(int(x / previewScale), int(y / previewScale));
		if (z > zImage)
		{
			opacity.R *= 0.1f;
			opacity.G *= 0.1f;
			opacity.B *= 0.1f;
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

void cImage::AntiAliasedPoint(float x, float y, float z, sRGB8 color, sRGBFloat opacity, int layer)
{
	previewMutex.lock();
	float deltaX = x - int(x);
	float deltaY = y - int(y);

	float intensity1 = (1.0f - deltaX) * (1.0f - deltaY);
	float intensity2 = (deltaX) * (1.0f - deltaY);
	float intensity3 = (1.0f - deltaX) * (deltaY);
	float intensity4 = (deltaX) * (deltaY);
	float sum = intensity1 + intensity2 + intensity3 + intensity4;

	sRGBFloat opacity2;

	int ix = int(x);
	int iy = int(y);

	opacity2.R = opacity.R * intensity1 / sum;
	opacity2.G = opacity.G * intensity1 / sum;
	opacity2.B = opacity.B * intensity1 / sum;
	PutPixelAlfa(ix, iy, z, color, opacity2, layer);

	opacity2.R = opacity.R * intensity2 / sum;
	opacity2.G = opacity.G * intensity2 / sum;
	opacity2.B = opacity.B * intensity2 / sum;
	PutPixelAlfa(ix + 1, iy, z, color, opacity2, layer);

	opacity2.R = opacity.R * intensity3 / sum;
	opacity2.G = opacity.G * intensity3 / sum;
	opacity2.B = opacity.B * intensity3 / sum;
	PutPixelAlfa(ix, iy + 1, z, color, opacity2, layer);

	opacity2.R = opacity.R * intensity4 / sum;
	opacity2.G = opacity.G * intensity4 / sum;
	opacity2.B = opacity.B * intensity4 / sum;
	PutPixelAlfa(ix + 1, iy + 1, z, color, opacity2, layer);
	previewMutex.unlock();
}

void cImage::AntiAliasedLine(float x1, float y1, float x2, float y2, float z1, float z2,
	const sRGB8 &color, const sRGBFloat &opacity, float thickness, int layer)
{
	previewMutex.lock();
	if (z1 >= 0.0 || z2 >= 0.0)
	//			&& ((x1 >= 0.0 && x1 < previewWidth && y1 >= 0.0 && y1 < previewHeight)
	//					|| (x2 >= 0.0 && x2 < previewWidth && y2 >= 0.0 && y2 < previewHeight)))
	{
		float deltaX = x2 - x1;
		float deltaY = y2 - y1;
		float l2 = deltaX * deltaX + deltaY * deltaY;

		if (deltaX != 0.0f || deltaY != 0.0f)
		{
			float minX = std::min(x1, x2);
			float maxX = std::max(x1, x2);

			float halfThickness = thickness;

			qint64 xStart = qint64(minX - halfThickness);
			if (xStart < 0L) xStart = 0;
			qint64 xEnd = qint64(maxX + halfThickness);
			if (xEnd > qint64(previewWidth)) xEnd = qint64(previewWidth);

			bool vertical = false;
			float k = 0.0;
			float kz = 0.0;
			if (deltaX != 0.0)
			{
				k = deltaY / deltaX;
				kz = (z2 - z1) / (x2 - x1);
			}
			else
			{
				vertical = true;
			}

			//		  vec3 pa = p - a, ba = b - a;
			//		  float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
			//		  return length( pa - ba*h ) - r;

			for (qint64 ix = xStart; ix <= xEnd; ix++)
			{
				float yLow = k * (ix - x1 - halfThickness) + y1;
				float yHigh = k * (ix - x1 + halfThickness) + y1;
				qint64 yStart;
				qint64 yEnd;
				if (vertical)
				{
					yStart = std::min(y1, y2) - halfThickness;
					yEnd = std::max(y1, y2) + halfThickness;
				}
				else
				{
					yStart = std::min(yLow, yHigh) - halfThickness;
					yEnd = std::max(yLow, yHigh) + halfThickness;
				}

				if (yStart < 0L) yStart = 0;
				if (yEnd > qint64(previewHeight)) yEnd = qint64(previewHeight);
				for (qint64 iy = yStart; iy <= yEnd; iy++)
				{
					float pax = ix - x1;
					float pay = iy - y1;

					float dot = pax * deltaX + pay * deltaY;

					float h = clamp(dot / l2, 0.0f, 1.0f);

					float dx = pax - deltaX * h;
					float dy = pay - deltaY * h;

					float dist = sqrtf(dx * dx + dy * dy);

					float z;
					if (vertical)
					{
						z = (iy - y1) / deltaY * (z2 - z1) + z1;
					}
					else
					{
						z = kz * (ix - x1) + z1;
					}

					if (z >= 0.0f)
					{
						float opacity2 = clamp(halfThickness - dist, 0.0f, 1.0f);

						sRGBFloat opacity3;
						opacity3.R = opacity2 * opacity.R;
						opacity3.G = opacity2 * opacity.G;
						opacity3.B = opacity2 * opacity.B;
						PutPixelAlfa(ix, iy, z, color, opacity3, layer);
					}
				}
			}
		}
	}
	previewMutex.unlock();
}

void cImage::CircleBorder(
	float x, float y, float z, float r, sRGB8 border, float borderWidth, sRGBFloat opacity, int layer)
{
	previewMutex.lock();
	if (borderWidth > 0 && r > 0)
	{
		float r2 = r + borderWidth;
		float r1 = r - borderWidth;
		if (r1 < 0) r1 = 0;
		qint64 y1 = qint64(y - r2);
		qint64 y2 = qint64(y + r2);

		float wspJ = 1.0f / borderWidth;

		for (qint64 yy = y1; yy <= y2; yy++)
		{
			float dyy = yy - y;
			float dxx = r2 * r2 - dyy * dyy;
			if (dxx < 0) dxx = 0;
			float dx = sqrtf(dxx);
			qint64 x1 = qint64(x - dx);
			qint64 x2 = qint64(x + dx);

			float dxx2 = r1 * r1 - dyy * dyy;
			if (dxx2 < 0) dxx2 = 0;
			float dx2 = sqrtf(dxx2);
			qint64 x12 = qint64(x - dx2);
			qint64 x22 = qint64(x + dx2);

			for (qint64 xx = x1; xx <= x2; xx++)
			{
				if (xx <= x12 || xx >= x22)
				{
					float deltaX = xx - x;
					float deltaY = yy - y;
					float rr = sqrtf(deltaX * deltaX + deltaY * deltaY);
					float deltaR = fabsf(rr - r);
					if (deltaR > borderWidth) deltaR = borderWidth;
					float opacity2 = wspJ * (borderWidth - deltaR);
					sRGBFloat opacity3;
					opacity3.R = opacity2 * opacity.R;
					opacity3.G = opacity2 * opacity.G;
					opacity3.B = opacity2 * opacity.B;
					PutPixelAlfa(xx, yy, z, border, opacity3, layer);
				}
			}
		}
	}
	previewMutex.unlock();
}

void cImage::NullPostEffect(QList<int> *list)
{
	if (!imageFloat.empty() && !postImageFloat.empty())
	{
		if (!list)
		{
			postImageFloat = imageFloat;
		}
		else
		{
			int listIndex = 0;
			for (quint64 y = 0; y < height; y++)
			{
				if (list)
				{
					if (listIndex >= list->size()) break;
					y = quint64(list->at(listIndex));
					listIndex++;
				}

				qint64 offset = qint64(y * width);
				auto first = imageFloat.begin() + offset;
				auto last = imageFloat.begin() + offset + qint64(width);
				auto fisrDest = postImageFloat.begin() + offset;

				std::copy(first, last, fisrDest);
			}
		}
	}
}

void cImage::NullPostEffect(const QList<QRect> *list)
{
	if (!imageFloat.empty() && !postImageFloat.empty())
	{
		for (auto rect : *list)
		{
			for (quint64 y = quint64(rect.top()); y <= quint64(rect.bottom()); y++)
			{
				qint64 offset = qint64(y * width) + rect.left();
				auto first = imageFloat.begin() + offset;
				auto last = imageFloat.begin() + offset + rect.width();
				auto fisrDest = postImageFloat.begin() + offset;

				std::copy(first, last, fisrDest);
			}
		}
	}
}

void cImage::GetStereoLeftRightImages(std::shared_ptr<cImage> left, std::shared_ptr<cImage> right)
{
	if (isStereoLeftRight && left && right)
	{
		quint64 halfWidth = width / 2;
		left->ChangeSize(halfWidth, height, opt);
		right->ChangeSize(halfWidth, height, opt);

		for (quint64 y = 0; y < height; y++)
		{
			for (quint64 x = 0; x < halfWidth; x++)
			{
				quint64 ptrNew = x + y * halfWidth;
				quint64 ptrLeft = x + y * width;
				quint64 ptrRight = (x + halfWidth) + y * width;

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
				}
				if (opt.optionalNormalWorld)
				{
					left->normalFloatWorld[ptrNew] = normalFloatWorld[ptrLeft];
					right->normalFloatWorld[ptrNew] = normalFloatWorld[ptrRight];
				}
				if (opt.optionalSpecular)
				{
					left->specularFloat[ptrNew] = specularFloat[ptrLeft];
					right->specularFloat[ptrNew] = specularFloat[ptrRight];
				}
				if (opt.optionalDiffuse)
				{
					left->diffuseFloat[ptrNew] = diffuseFloat[ptrLeft];
					right->diffuseFloat[ptrNew] = diffuseFloat[ptrRight];
				}
				if (opt.optionalWorld)
				{
					left->worldFloat[ptrNew] = worldFloat[ptrLeft];
					right->worldFloat[ptrNew] = worldFloat[ptrRight];
				}
				if (opt.optionalShadows)
				{
					left->shadows[ptrNew] = shadows[ptrLeft];
					right->shadows[ptrNew] = shadows[ptrRight];
				}
				if (opt.optionalGlobalIlluination)
				{
					left->globalIllumination[ptrNew] = globalIllumination[ptrLeft];
					right->globalIllumination[ptrNew] = globalIllumination[ptrRight];
				}
				if (opt.optionalNotDenoised)
				{
					left->notDenoised[ptrNew] = notDenoised[ptrLeft];
					right->notDenoised[ptrNew] = notDenoised[ptrRight];
				}
			}
		}
	}
}

double cImage::VisualCompare(std::shared_ptr<cImage> refImage, bool checkIfBlank)
{
	ConvertTo8bitChar();
	refImage->ConvertTo8bitChar();

	int min = 255 * 3;
	int max = 0;

	unsigned int w = GetWidth();
	unsigned int h = GetHeight();
	int numberOfPixels = w * h;

	if (w != refImage->GetWidth() || h != refImage->GetHeight())
	{
		qCritical() << "cImage::VisualCompare(): images have different rosolutions!";
		return 0.0;
	}

	double totalDiff = 0.0;

	for (unsigned int y = 2; y < h - 2; y++)
	{
		for (unsigned int x = 2; x < w - 2; x++)
		{
			sRGB8 pixel1 = GetPixelImage8(x, y);
			sRGB8 pixel2 = refImage->GetPixelImage8(x, y);
			double rDiffR = double(pixel1.R) - pixel2.R;
			double rDiffG = double(pixel1.G) - pixel2.G;
			double rDiffB = double(pixel1.B) - pixel2.B;
			double diff = rDiffR * rDiffR + rDiffG * rDiffG + rDiffB * rDiffB;
			totalDiff += diff;

			min = qMin(min, pixel1.R + pixel1.G + pixel1.B);
			max = qMax(max, pixel1.R + pixel1.G + pixel1.B);
		}
	}
	double diffPerPixel = totalDiff / numberOfPixels;

	if (checkIfBlank)
	{
		if (min > 245 * 3 || max < 5 * 10 || max - min < 5)
		{
			diffPerPixel = 0;
			// qDebug() << "blank image";
		}
	}

	return diffPerPixel;
}

void cImage::FastResize(quint64 w, quint64 h)
{
	previewMutex.lock();
	std::vector<sRGBFloat> imageCopy = imageFloat;
	std::vector<float> imageCopyZBuffer = zBuffer;
	quint64 oldWidth = width;
	quint64 oldHeight = height;
	previewMutex.unlock();

	ChangeSize(w, h, opt);

	previewMutex.lock();
	float scaleX = float(oldWidth) / w;
	float scaleY = float(oldHeight) / h;

	for (quint64 y = 0; y < h; y++)
	{
		quint64 yy = y * scaleY;

		for (quint64 x = 0; x < w; x++)
		{
			quint64 xx = x * scaleX;
			PutPixelImage(x, y, imageCopy[yy * oldWidth + xx]);
			PutPixelPostImage(x, y, imageCopy[yy * oldWidth + xx]);
			PutPixelZBuffer(x, y, imageCopyZBuffer[yy * oldWidth + xx]);
		}
	}
	previewMutex.unlock();
}

double cImage::GetAverageBrightness() const
{
	double totalBrightness = 0.0;
	quint64 pixelCount = width * height;

	for (quint64 i = 0; i < pixelCount; i++)
	{
		sRGBFloat pixel = postImageFloat[i];
		totalBrightness += (pixel.R + pixel.G + pixel.B);
	}
	return totalBrightness / 3.0 / pixelCount;
}
