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

#ifndef MANDELBULBER2_SRC_CIMAGE_HPP_
#define MANDELBULBER2_SRC_CIMAGE_HPP_

//#include <QtGui/QWidget>
#include <atomic>
#include <cassert>

#include <QMutex>
#include <QWidget>

#include "color_structures.hpp"
#include "image_adjustments.h"

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
	sRGBFloat imageFloat;
	quint16 alphaBuffer;
	quint16 opacityBuffer;
	sRGBFloat normalFloat;
	sRGB8 colourBuffer;
	float zBuffer;
};

class cImage
{
public:
	cImage(int w, int h, bool _allocLater = false);
	void construct();

	~cImage();
	bool IsAllocated() const { return isAllocated; }
	bool ChangeSize(int w, int h, sImageOptional optional);
	void ClearImage();
	void ClearRGB(QScopedArrayPointer<sRGBFloat> &rgbFloat, QScopedArrayPointer<sRGB16> &rgb16,
		QScopedArrayPointer<sRGB8> &rgb8);
	void AllocRGB(QScopedArrayPointer<sRGBFloat> &rgbFloat, QScopedArrayPointer<sRGB16> &rgb16,
		QScopedArrayPointer<sRGB8> &rgb8);
	void FreeRGB(QScopedArrayPointer<sRGBFloat> &rgbFloat, QScopedArrayPointer<sRGB16> &rgb16,
		QScopedArrayPointer<sRGB8> &rgb8);

	bool IsUsed() const { return isUsed; }
	void BlockImage() { isUsed = true; }
	void ReleaseImage() { isUsed = false; }

	void SetAsMainImage() { isMainImage = true; }
	bool IsMainImage() const { return isMainImage; }

	void SetFastPreview(bool enable) { fastPreview = enable; }

	inline qint64 getImageIndex(const qint64 x, const qint64 y) const
	{
		// assert(x >= 0 && x < width && y >= 0 && y < height);
		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			return qint64(x) + qint64(y) * qint64(width);
		}
		else
		{
			qCritical() << "getImageIndex out of range"
									<< "x" << x << "y" << y;
			return 0;
		}
	}

	inline void PutPixelImage(qint64 x, qint64 y, sRGBFloat pixel)
	{
		imageFloat[getImageIndex(x, y)] = pixel;
	}
	inline void PutPixelPostImage(qint64 x, qint64 y, sRGBFloat pixel)
	{
		postImageFloat[getImageIndex(x, y)] = pixel;
	}
	inline void PutPixelImage16(qint64 x, qint64 y, sRGB16 pixel)
	{
		image16[getImageIndex(x, y)] = pixel;
	}
	inline void PutPixelColor(qint64 x, qint64 y, sRGB8 pixel)
	{
		colourBuffer[getImageIndex(x, y)] = pixel;
	}
	inline void PutPixelZBuffer(qint64 x, qint64 y, float pixel)
	{
		zBuffer[getImageIndex(x, y)] = pixel;
	}
	inline void PutPixelAlpha(qint64 x, qint64 y, quint16 pixel)
	{
		alphaBuffer16[getImageIndex(x, y)] = pixel;
	}
	inline void PutPixelOpacity(qint64 x, qint64 y, quint16 pixel)
	{
		opacityBuffer[getImageIndex(x, y)] = pixel;
	}
	inline void PutPixelNormal(qint64 x, qint64 y, sRGBFloat normal)
	{
		normalFloat[getImageIndex(x, y)] = normal;
	}
	inline sRGBFloat GetPixelImage(qint64 x, qint64 y) const
	{
		return imageFloat[getImageIndex(x, y)];
	}
	inline sRGBFloat GetPixelPostImage(qint64 x, qint64 y) const
	{
		return postImageFloat[getImageIndex(x, y)];
	}
	inline sRGB16 GetPixelImage16(qint64 x, qint64 y) const { return image16[getImageIndex(x, y)]; }
	inline sRGB8 GetPixelImage8(qint64 x, qint64 y) const { return image8[getImageIndex(x, y)]; }
	inline quint16 GetPixelAlpha(qint64 x, qint64 y) const
	{
		return alphaBuffer16[getImageIndex(x, y)];
	}
	inline quint8 GetPixelAlpha8(qint64 x, qint64 y) const
	{
		return alphaBuffer8[getImageIndex(x, y)];
	}
	inline quint16 GetPixelOpacity(qint64 x, qint64 y) const
	{
		return opacityBuffer[getImageIndex(x, y)];
	}
	inline sRGB8 GetPixelColor(qint64 x, qint64 y) const { return colourBuffer[getImageIndex(x, y)]; }
	inline float GetPixelZBuffer(qint64 x, qint64 y) const { return zBuffer[getImageIndex(x, y)]; }
	inline sRGBFloat GetPixelNormal(qint64 x, qint64 y) const
	{
		if (!opt.optionalNormal) return BlackFloat();
		return normalFloat[getImageIndex(x, y)];
	}
	inline sRGB16 GetPixelNormal16(qint64 x, qint64 y) const
	{
		if (!opt.optionalNormal) return Black16();
		return normal16[getImageIndex(x, y)];
	}
	inline sRGB8 GetPixelNormal8(qint64 x, qint64 y) const
	{
		if (!opt.optionalNormal) return Black8();
		return normal8[getImageIndex(x, y)];
	}
	inline void BlendPixelImage16(qint64 x, qint64 y, float factor, sRGB16 other)
	{
		float factorN = 1.0f - factor;
		qint64 imgIndex = getImageIndex(x, y);
		image16[imgIndex].R = quint16(image16[imgIndex].R * factorN + other.R * factor);
		image16[imgIndex].G = quint16(image16[imgIndex].G * factorN + other.G * factor);
		image16[imgIndex].B = quint16(image16[imgIndex].B * factorN + other.B * factor);
	}

	inline void BlendPixelPostImage(qint64 x, qint64 y, float factor, sRGBFloat other)
	{
		float factorN = 1.0f - factor;
		qint64 imgIndex = getImageIndex(x, y);
		postImageFloat[imgIndex].R = postImageFloat[imgIndex].R * factorN + other.R * factor;
		postImageFloat[imgIndex].G = postImageFloat[imgIndex].G * factorN + other.G * factor;
		postImageFloat[imgIndex].B = postImageFloat[imgIndex].B * factorN + other.B * factor;
	}

	inline void BlendPixelAlpha(qint64 x, qint64 y, float factor, quint16 other)
	{
		float factorN = 1.0f - factor;
		qint64 imgIndex = getImageIndex(x, y);
		alphaBuffer16[imgIndex] = quint16(alphaBuffer16[imgIndex] * factorN + other * factor);
	}

	sRGBFloat *GetImageFloatPtr() { return imageFloat.data(); }
	sRGBFloat *GetPostImageFloatPtr() { return postImageFloat.data(); }
	sRGB16 *GetImage16Ptr() { return image16.data(); }
	sRGB8 *GetImage8Ptr() { return image8.data(); }
	quint16 *GetAlphaBufPtr() { return alphaBuffer16.data(); }
	quint8 *GetAlphaBufPtr8() { return alphaBuffer8.data(); }
	float *GetZBufferPtr() { return zBuffer.data(); }
	sRGB8 *GetColorPtr() { return colourBuffer.data(); }
	quint16 *GetOpacityPtr() { return opacityBuffer.data(); }
	size_t GetZBufferSize() const { return sizeof(float) * quint64(height) * quint64(width); }
	QWidget *GetImageWidget() { return imageWidget; }

	void CompileImage(QList<int> *list = nullptr);
	void CompileImage(const QList<QRect> *list);
	void NullPostEffect(QList<int> *list = nullptr);
	void NullPostEffect(const QList<QRect> *list);

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	int GetPreviewWidth() const { return previewWidth; }
	int GetPreviewHeight() const { return previewHeight; }
	int GetPreviewVisibleWidth() const { return previewVisibleWidth; }
	int GetPreviewVisibleHeight() const { return previewVisibleHeight; }
	int GetUsedMB() const;
	void SetImageParameters(sImageAdjustments adjustments);
	sImageAdjustments *GetImageAdjustments() { return &adj; }
	void SetImageOptional(sImageOptional optInput) { opt = optInput; }
	sImageOptional *GetImageOptional() { return &opt; }

	quint8 *ConvertGenericRGBTo8bit(
		QScopedArrayPointer<sRGBFloat> &from, QScopedArrayPointer<sRGB8> &to);
	quint8 *ConvertGenericRGBTo16bit(
		QScopedArrayPointer<sRGBFloat> &from, QScopedArrayPointer<sRGB16> &to);
	quint8 *ConvertTo8bit();
	quint8 *ConvertTo8bit(const QList<QRect> *list);
	quint8 *ConvertAlphaTo8bit();
	quint8 *ConvertNormalTo16Bit();
	quint8 *ConvertNormalTo8Bit();

	quint8 *CreatePreview(double scale, int visibleWidth, int visibleHeight, QWidget *widget);
	void UpdatePreview(QList<int> *list = nullptr);
	void UpdatePreview(const QList<QRect> *list);
	quint8 *GetPreviewPtr() const;
	quint8 *GetPreviewPrimaryPtr() const;
	bool IsPreview() const;
	void RedrawInWidget(QWidget *qWidget = nullptr);
	double GetPreviewScale() const { return previewScale; }
	void Squares(int y, int progressiveFactor);
	void CalculateGammaTable();
	sRGB16 CalculatePixel(sRGBFloat pixel);

	void PutPixelAlfa(qint64 x, qint64 y, float z, sRGB8 color, sRGBFloat opacity, int layer);
	void AntiAliasedPoint(double x, double y, float z, sRGB8 color, sRGBFloat opacity, int layer);
	void AntiAliasedLine(double x1, double y1, double x2, double y2, float z1, float z2, sRGB8 color,
		sRGBFloat opacity, int layer);
	void CircleBorder(double x, double y, float z, double r, sRGB8 border, double borderWidth,
		sRGBFloat opacity, int layer);

	bool IsStereoLeftRight() const { return isStereoLeftRight; }
	void SetStereoLeftRight(bool isStereoLeftRightInput)
	{
		isStereoLeftRight = isStereoLeftRightInput;
	}
	void GetStereoLeftRightImages(cImage *left, cImage *right);

	int progressiveFactor;

private:
	bool isAllocated;
	sRGB8 Interpolation(float x, float y) const;
	bool AllocMem();
	void FreeImage();
	static inline sRGB16 Black16() { return sRGB16(0, 0, 0); }
	static inline sRGB8 Black8() { return sRGB8(0, 0, 0); }
	static inline sRGBFloat BlackFloat() { return sRGBFloat(0, 0, 0); }

	QScopedArrayPointer<sRGB8> image8;
	QScopedArrayPointer<sRGB16> image16;
	QScopedArrayPointer<sRGBFloat> imageFloat;
	QScopedArrayPointer<sRGBFloat> postImageFloat;

	QScopedArrayPointer<quint8> alphaBuffer8;
	QScopedArrayPointer<quint16> alphaBuffer16;
	QScopedArrayPointer<quint16> opacityBuffer;
	QScopedArrayPointer<sRGB8> colourBuffer;
	QScopedArrayPointer<float> zBuffer;

	// optional image buffers
	QScopedArrayPointer<sRGBFloat> normalFloat;
	QScopedArrayPointer<sRGB8> normal8;
	QScopedArrayPointer<sRGB16> normal16;

	QScopedArrayPointer<sRGB8> preview;
	QScopedArrayPointer<sRGB8> preview2;
	QWidget *imageWidget;

	sImageAdjustments adj;
	sImageOptional opt;
	qint64 width;
	qint64 height;
	QScopedArrayPointer<int> gammaTable;
	bool previewAllocated;
	int previewWidth;
	int previewHeight;
	double previewScale;
	int previewVisibleWidth;
	int previewVisibleHeight;
	bool isMainImage;
	bool gammaTablePrepared;
	bool allocLater;
	bool isStereoLeftRight;
	bool fastPreview;

	QMutex previewMutex;

	std::atomic<bool> isUsed;
};

sRGB PostRendering_Fog(double z, double min, double max, sRGB fog_color, sRGB color_before);

#endif /* MANDELBULBER2_SRC_CIMAGE_HPP_ */
