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

#ifndef MANDELBULBER2_SRC_CIMAGE_HPP_
#define MANDELBULBER2_SRC_CIMAGE_HPP_

//#include <QtGui/QWidget>
#include "color_structures.hpp"
#include "image_adjustments.h"
#include <QMutex>
#include <QWidget>

struct sImageOptional
{
	sImageOptional() : optionalNormal(false) {}
	inline bool operator==(sImageOptional other) const
	{
		return other.optionalNormal == optionalNormal;
	}

	bool optionalNormal;
};

struct sAllImageData
{
	sRGBfloat imageFloat;
	unsigned short alphaBuffer;
	unsigned short opacityBuffer;
	sRGBfloat normalFloat;
	sRGB8 colourBuffer;
	float zBuffer;
};

class cImage
{
public:
	cImage(int w, int h, bool _allocLater = false);
	void construct(void);

	~cImage();
	bool IsAllocated() const { return isAllocated; }
	bool ChangeSize(int w, int h, sImageOptional optional);
	void ClearImage(void);

	bool IsUsed() const { return isUsed; }
	void BlockImage() { isUsed = true; }
	void ReleaseImage() { isUsed = false; }

	void SetAsMainImage() { isMainImage = true; }
	bool IsMainImage() const { return isMainImage; }

	inline void PutPixelImage(int x, int y, sRGBfloat pixel)
	{
		if (x >= 0 && x < width && y >= 0 && y < height) imageFloat[x + y * width] = pixel;
	}
	inline void PutPixelImage16(int x, int y, sRGB16 pixel)
	{
		if (x >= 0 && x < width && y >= 0 && y < height) image16[x + y * width] = pixel;
	}
	inline void PutPixelColour(int x, int y, sRGB8 pixel)
	{
		if (x >= 0 && x < width && y >= 0 && y < height) colourBuffer[x + y * width] = pixel;
	}
	inline void PutPixelZBuffer(int x, int y, float pixel)
	{
		if (x >= 0 && x < width && y >= 0 && y < height) zBuffer[x + y * width] = pixel;
	}
	inline void PutPixelAlpha(int x, int y, unsigned short pixel)
	{
		if (x >= 0 && x < width && y >= 0 && y < height) alphaBuffer16[x + y * width] = pixel;
	}
	inline void PutPixelOpacity(int x, int y, unsigned short pixel)
	{
		if (x >= 0 && x < width && y >= 0 && y < height) opacityBuffer[x + y * width] = pixel;
	}
	inline void PutPixelNormal(int x, int y, sRGBfloat normal)
	{
		if (x >= 0 && x < width && y >= 0 && y < height) normalFloat[x + y * width] = normal;
	}
	inline sRGBfloat GetPixelImage(int x, int y) const
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			return imageFloat[x + y * width];
		else
			return BlackFloat();
	}
	inline sRGB16 GetPixelImage16(int x, int y) const
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			return image16[x + y * width];
		else
			return Black16();
	}
	inline sRGB8 GetPixelImage8(int x, int y) const
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			return image8[x + y * width];
		else
			return Black8();
	}
	inline unsigned short int GetPixelAlpha(int x, int y) const
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			return alphaBuffer16[x + y * width];
		else
			return 0;
	}
	inline unsigned char GetPixelAlpha8(int x, int y) const
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			return alphaBuffer8[x + y * width];
		else
			return 0;
	}
	inline unsigned short int GetPixelOpacity(int x, int y) const
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			return opacityBuffer[x + y * width];
		else
			return 0;
	}
	inline sRGB8 GetPixelColor(int x, int y) const
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			return colourBuffer[x + y * width];
		else
			return Black8();
	}
	inline float GetPixelZBuffer(int x, int y) const
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			return zBuffer[x + y * width];
		else
			return float(1e20);
	}
	inline sRGBfloat GetPixelNormal(int x, int y) const
	{
		if (!opt.optionalNormal) return BlackFloat();
		if (x >= 0 && x < width && y >= 0 && y < height) return normalFloat[x + y * width];
		return BlackFloat();
	}
	inline sRGB16 GetPixelNormal16(int x, int y) const
	{
		if (!opt.optionalNormal) return Black16();
		if (x >= 0 && x < width && y >= 0 && y < height) return normal16[x + y * width];
		return Black16();
	}
	inline sRGB8 GetPixelNormal8(int x, int y) const
	{
		if (!opt.optionalNormal) return Black8();
		if (x >= 0 && x < width && y >= 0 && y < height) return normal8[x + y * width];
		return Black8();
	}
	inline void BlendPixelImage16(int x, int y, double factor, sRGB16 other)
	{
		double factorN = 1.0 - factor;
		image16[x + y * width].R = image16[x + y * width].R * factorN + other.R * factor;
		image16[x + y * width].G = image16[x + y * width].G * factorN + other.G * factor;
		image16[x + y * width].B = image16[x + y * width].B * factorN + other.B * factor;
	}

	inline void BlendPixelImage(int x, int y, double factor, sRGBfloat other)
	{
		double factorN = 1.0 - factor;
		imageFloat[x + y * width].R = imageFloat[x + y * width].R * factorN + other.R * factor;
		imageFloat[x + y * width].G = imageFloat[x + y * width].G * factorN + other.G * factor;
		imageFloat[x + y * width].B = imageFloat[x + y * width].B * factorN + other.B * factor;
	}

	inline void BlendPixelAlpha(int x, int y, double factor, unsigned short int other)
	{
		double factorN = 1.0 - factor;
		alphaBuffer16[x + y * width] = alphaBuffer16[x + y * width] * factorN + other * factor;
	}

	sRGB16 *GetImage16Ptr(void) { return image16; }
	sRGB8 *GetImage8Ptr(void) { return image8; }
	unsigned short *GetAlphaBufPtr(void) { return alphaBuffer16; }
	unsigned char *GetAlphaBufPtr8(void) { return alphaBuffer8; }
	float *GetZBufferPtr(void) { return zBuffer; }
	sRGB8 *GetColorPtr(void) { return colourBuffer; }
	unsigned short *GetOpacityPtr(void) { return opacityBuffer; }
	size_t GetZBufferSize(void) const { return sizeof(float) * height * width; }
	QWidget *GetImageWidget(void) { return imageWidget; }

	void CompileImage(QList<int> *list = NULL);

	int GetWidth(void) const { return width; }
	int GetHeight(void) const { return height; }
	int GetPreviewWidth(void) const { return previewWidth; }
	int GetPreviewHeight(void) const { return previewHeight; }
	int GetPreviewVisibleWidth(void) const { return previewVisibleWidth; }
	int GetPreviewVisibleHeight(void) const { return previewVisibleHeight; }
	int GetUsedMB(void) const;
	void SetImageParameters(sImageAdjustments adjustments);
	sImageAdjustments *GetImageAdjustments(void) { return &adj; }
	void SetImageOptional(sImageOptional opt) { this->opt = opt; }
	sImageOptional *GetImageOptional(void) { return &opt; }

	unsigned char *ConvertTo8bit(void);
	unsigned char *ConvertAlphaTo8bit(void);
	unsigned char *ConvertNormalto16Bit(void);
	unsigned char *ConvertNormalto8Bit(void);
	unsigned char *CreatePreview(double scale, int visibleWidth, int visibleHeight, QWidget *widget);
	void UpdatePreview(QList<int> *list = NULL);
	unsigned char *GetPreviewPtr(void);
	unsigned char *GetPreviewPrimaryPtr(void);
	bool IsPreview(void) const;
	void RedrawInWidget(QWidget *qwidget = NULL);
	double GetPreviewScale() const { return previewScale; }
	void Squares(int y, int progressiveFactor);
	void CalculateGammaTable(void);
	sRGB16 CalculatePixel(sRGBfloat pixel);

	void PutPixelAlfa(int x, int y, float z, sRGB8 color, sRGBfloat opacity, int layer);
	void AntiAliasedPoint(double x, double y, float z, sRGB8 color, sRGBfloat opacity, int layer);
	void AntiAliasedLine(double x1, double y1, double x2, double y2, float z1, float z2, sRGB8 color,
		sRGBfloat opacity, int layer);
	void CircleBorder(double x, double y, float z, double r, sRGB8 border, double borderWidth,
		sRGBfloat opacity, int layer);

	int progressiveFactor;

private:
	bool isAllocated;
	sRGB8 Interpolation(float x, float y) const;
	bool AllocMem(void);
	void FreeImage(void);
	inline sRGB16 Black16(void) const { return sRGB16(0, 0, 0); }
	inline sRGB8 Black8(void) const { return sRGB8(0, 0, 0); }
	inline sRGBfloat BlackFloat(void) const { return sRGBfloat(0, 0, 0); }

	sRGB8 *image8;
	sRGB16 *image16;
	sRGBfloat *imageFloat;

	unsigned char *alphaBuffer8;
	unsigned short *alphaBuffer16;
	unsigned short *opacityBuffer;
	sRGB8 *colourBuffer;
	float *zBuffer;

	// optional image buffers
	sRGBfloat *normalFloat;
	sRGB8 *normal8;
	sRGB16 *normal16;

	sRGB8 *preview;
	sRGB8 *preview2;
	QWidget *imageWidget;

	sImageAdjustments adj;
	sImageOptional opt;
	int width;
	int height;
	int *gammaTable;
	bool previewAllocated;
	int previewWidth;
	int previewHeight;
	double previewScale;
	int previewVisibleWidth;
	int previewVisibleHeight;
	bool isMainImage;
	bool gammaTablePrepared;
	bool allocLater;

	QMutex previewMutex;

	volatile bool isUsed;
};

sRGB PostRendering_Fog(double z, double min, double max, sRGB fog_color, sRGB color_before);

#endif /* MANDELBULBER2_SRC_CIMAGE_HPP_ */
