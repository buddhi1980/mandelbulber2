/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cImage class - manipulation of multi-layer images
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef CIMAGE_HPP_
#define CIMAGE_HPP_

//#include <QtGui/QWidget>
#include <QWidget>
#include "color_structures.hpp"

//global variables

struct sImageAdjustments
{
	sImageAdjustments() : brightness(1.0), contrast(1.0), imageGamma(1.0), hdrEnabled(false) {};
	double brightness;
  double contrast;
  double imageGamma;
  bool hdrEnabled;
};

struct sAllImageData
{
	sRGBfloat imageFloat;
	unsigned short alphaBuffer;
	unsigned short opacityBuffer;
	sRGB8 colourBuffer;
	float zBuffer;
};

class cImage
{
public:
	cImage(int w, int h, bool low_mem = false);
	~cImage();
	bool ChangeSize(int w, int h);
	void ClearImage(void);

	bool IsUsed() {return isUsed;}
	void BlockImage() {isUsed = true;}
	void ReleaseImage() {isUsed = false;}

	void SetAsMainImage() {isMainImage = true;}
	bool IsMainImage() {return isMainImage;}

	inline void PutPixelImage(int x, int y, sRGBfloat pixel)	{if (x >= 0 && x < width && y >= 0 && y < height) imageFloat[x + y * width] = pixel;}
	inline void PutPixelImage16(int x, int y, sRGB16 pixel)	{if (x >= 0 && x < width && y >= 0 && y < height) image16[x + y * width] = pixel;}
	inline void PutPixelColour(int x, int y, sRGB8 pixel)	{if (x >= 0 && x < width && y >= 0 && y < height) colourBuffer[x + y * width] = pixel;}
	inline void PutPixelZBuffer(int x, int y, float pixel) {if (x >= 0 && x < width && y >= 0 && y < height) zBuffer[x + y * width] = pixel;}
	inline void PutPixelAlpha(int x, int y, unsigned short pixel) {if (x >= 0 && x < width && y >= 0 && y < height) alphaBuffer16[x + y * width] = pixel;}
	inline void PutPixelOpacity(int x, int y, unsigned short pixel) {if (x >= 0 && x < width && y >= 0 && y < height) opacityBuffer[x + y * width] = pixel;}

  inline sRGBfloat GetPixelImage(int x, int y)  {if (x >= 0 && x < width && y >= 0 && y < height) return imageFloat[x + y * width]; else return BlackFloat();}
  inline sRGB16 GetPixelImage16(int x, int y)  {if (x >= 0 && x < width && y >= 0 && y < height) return image16[x + y * width]; else return Black16();}
	inline sRGB8 GetPixelImage8(int x, int y)  {if (x >= 0 && x < width && y >= 0 && y < height) return image8[x + y * width]; else return Black8();}
	inline unsigned short int GetPixelAlpha(int x, int y)  {if (x >= 0 && x < width && y >= 0 && y < height) return alphaBuffer16[x + y * width]; else return 0;}
	inline unsigned char GetPixelAlpha8(int x, int y)  {if (x >= 0 && x < width && y >= 0 && y < height) return alphaBuffer8[x + y * width]; else return 0;}
  inline short int GetPixelOpacity(int x, int y)  {if (x >= 0 && x < width && y >= 0 && y < height) return opacityBuffer[x + y * width]; else return 0;}
  inline sRGB8 GetPixelColor(int x, int y)  {if (x >= 0 && x < width && y >= 0 && y < height) return colourBuffer[x + y * width]; else return Black8();}
  inline float GetPixelZBuffer(int x, int y)  {if (x >= 0 && x < width && y >= 0 && y < height) return zBuffer[x + y * width]; else return 1e20;}

  sRGB16* GetImage16Ptr(void) {return image16;}
	sRGB8* GetImage8Ptr(void) {return image8;}
	unsigned short* GetAlphaBufPtr(void) {return alphaBuffer16;}
	unsigned char* GetAlphaBufPtr8(void) {return alphaBuffer8;}
	float* GetZBufferPtr(void) {return zBuffer;}
  sRGB8* GetColorPtr(void) {return colourBuffer;}
  unsigned short* GetOpacityPtr(void) {return opacityBuffer;}
  size_t GetZBufferSize(void) {return sizeof(float) * height * width;}
  QWidget* GetImageWidget(void) {return imageWidget;};

  void CompileImage(QList<int> *list = NULL);

  int GetWidth(void) {return width;}
  int GetHeight(void) {return height;}
  int GetPreviewWidth(void) {return previewWidth;}
  int GetPreviewHeight(void) {return previewHeight;}
  int GetPreviewVisibleWidth(void) {return previewVisibleWidth;}
  int GetPreviewVisibleHeight(void) {return previewVisibleHeight;}
  int GetUsedMB(void);
  void SetImageParameters(sImageAdjustments adjustments);
  sImageAdjustments* GetImageAdjustments(void) {return &adj;}

  unsigned char* ConvertTo8bit(void);
	unsigned char* ConvertAlphaTo8bit(void);
  unsigned char* CreatePreview(double scale, int visibleWidth, int visibleHeight, QWidget *widget);
  void UpdatePreview(QList<int> *list = NULL);
  unsigned char* GetPreviewPtr(void);
  unsigned char* GetPreviewPrimaryPtr(void);
  bool IsPreview(void);
  void RedrawInWidget(QWidget *qwidget = NULL);
  double GetPreviewScale() {return previewScale;}
  void Squares(int y, int progressiveFactor);
  void CalculateGammaTable(void);
  sRGB16 CalculatePixel(sRGBfloat pixel);

  void PutPixelAlfa(int x, int y, float z, sRGB8 color, double opacity, int layer);
  void AntiAliasedPoint(double x, double y, float z, sRGB8 color, double opacity, int layer);
  void AntiAliasedLine(double x1, double y1, double x2, double y2, float z1, float z2, sRGB8 color, double opacity, int layer);
  void CircleBorder(double x, double y, float z, double r, sRGB8 border, double borderWidth, double opacity, int layer);

	int progressiveFactor;

private:
  sRGB8 Interpolation(float x, float y);
	bool AllocMem(void);
	void FreeImage(void);
	inline sRGB16 Black16(void) {return sRGB16(0,0,0);}
	inline sRGB8 Black8(void) {return sRGB8(0,0,0);}
	inline sRGBfloat BlackFloat(void) {return sRGBfloat(0,0,0);}

	sRGB8 *image8;
	sRGB16 *image16;
	sRGBfloat *imageFloat;

	unsigned char *alphaBuffer8;
	unsigned short *alphaBuffer16;
	unsigned short *opacityBuffer;
	sRGB8 *colourBuffer;
	float *zBuffer;

	sRGB8 *preview;
	sRGB8 *preview2;
	QWidget *imageWidget;

	sImageAdjustments adj;
	int width;
	int height;
	int *gammaTable;
	bool previewAllocated;
	int previewWidth;
	int previewHeight;
	double previewScale;
	int previewVisibleWidth;
	int previewVisibleHeight;
	bool lowMem;
	bool isMainImage;
	volatile bool isUsed;
};

sRGB PostRendering_Fog(double z, double min, double max, sRGB fog_color, sRGB color_before);

#endif /* CIMAGE_HPP_ */
