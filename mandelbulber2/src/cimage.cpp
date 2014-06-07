/*********************************************************
 /                   MANDELBULBER
 / cImage class for offline and multiple layer image
 / handling
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 ********************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "cimage.hpp"

#include <iostream>
#include <new>
#include <QtGui>

cImage::cImage(int w, int h, bool low_mem)
{
	width = w;
	height = h;
	previewAllocated = false;
	lowMem = low_mem;
	AllocMem();
	gammaTable = new int[65536];
	progressiveFactor = 1;
	imageWidget = NULL;
	isUsed = false;
	previewScale = 1.0;
	previewVisibleWidth = 1;
	previewVisibleHeight = 1;
}

cImage::~cImage()
{
	if(imageFloat) delete[] imageFloat;
	imageFloat = NULL;
	if(image16) delete[] image16;
	image16 = NULL;
	if(image8) delete[] image8;
	image8 = NULL;
	if(alphaBuffer) delete[] alphaBuffer;
	alphaBuffer = NULL;
	if(opacityBuffer) delete[] opacityBuffer;
	opacityBuffer = NULL;
	if(colourBuffer) delete[] colourBuffer;
	colourBuffer = NULL;
	if(zBuffer) delete[] zBuffer;
	zBuffer = NULL;
	if(gammaTable) delete[] gammaTable;
	gammaTable = NULL;
	if (previewAllocated)
	{
		if(preview) delete[] preview;
		preview = NULL;
		if(preview2) delete[] preview2;
		preview2 = NULL;
	}
}

bool cImage::AllocMem(void)
{
	if (width > 0 && height > 0)
	{
		try
		{
			imageFloat = new sRGBfloat[width * height];
			image16 = new sRGB16[width * height];
			image8 = new sRGBA8[width * height];
			zBuffer = new float[width * height];
			alphaBuffer = new unsigned short[width * height];
			opacityBuffer = new unsigned short[width * height];
			colourBuffer = new sRGB8[width * height];
			ClearImage();
		}
		catch (std::bad_alloc& ba)
		{
			std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
			return false;
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
	return true;
}

bool cImage::ChangeSize(int w, int h)
{
	if (imageFloat) delete[] imageFloat;
	imageFloat = NULL;
	if (image16) delete[] image16;
	image16 = NULL;
	if (image8) delete[] image8;
	image8 = NULL;
	if (alphaBuffer) delete[] alphaBuffer;
	alphaBuffer = NULL;
	if (opacityBuffer) delete[] opacityBuffer;
	opacityBuffer = NULL;
	if (colourBuffer) delete[] colourBuffer;
	colourBuffer = NULL;
	if (zBuffer) delete[] zBuffer;
	zBuffer = NULL;
	width = w;
	height = h;
	return AllocMem();
}

void cImage::ClearImage(void)
{
	memset(imageFloat, 0, (unsigned long int)sizeof(sRGBfloat) * width * height);
	memset(image16, 0, (unsigned long int)sizeof(sRGB16) * width * height);
	memset(image8, 0, (unsigned long int)sizeof(sRGB8) * width * height);
	memset(alphaBuffer, 0, (unsigned long int)sizeof(unsigned short) * width * height);
	memset(opacityBuffer, 0, (unsigned long int)sizeof(unsigned short) * width * height);
	memset(colourBuffer, 0, (unsigned long int)sizeof(sRGB8) * width * height);

	for (long int i = 0; i < width * height; ++i)
		zBuffer[i] = 1e20;
}

sRGB16 cImage::CalculatePixel(sRGBfloat pixel)
{

	float R = pixel.R * adj.brightness;
	float G = pixel.G * adj.brightness;
	float B = pixel.B * adj.brightness;

	R = (R - 0.5) * adj.contrast + 0.5;
	G = (G - 0.5) * adj.contrast + 0.5;
	B = (B - 0.5) * adj.contrast + 0.5;

	if (R < 0.0) R = 0.0;
	if (G < 0.0) G = 0.0;
	if (B < 0.0) B = 0.0;

	//R = sqrt(tanh(R*R));
	//G = sqrt(tanh(G*G));
	//B = sqrt(tanh(B*B));
	if(adj.hdrEnabled)
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
	for (int i = 0; i < 65536; i++)
	{
		gammaTable[i] = pow(i / 65536.0, 1.0 / adj.imageGamma) * 65535;
	}
}

void cImage::CompileImage(QVector<int> *list)
{
	int listIndex = 0;
	for (int y = 0; y < height; y++)
	{
		if(list)
		{
			if(listIndex >= list->size()) break;
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

int cImage::GetUsedMB(void)
{
	long int mb = 0;

	long int zBufferSize = (long int) width * height * sizeof(float);
	long int alphaSize = (long int) width * height * sizeof(unsigned short);
	long int imageFloatSize = (long int) width * height * sizeof(sRGBfloat);
	long int image16Size = (long int) width * height * sizeof(sRGB16);
	long int image8Size = (long int) width * height * sizeof(sRGB8);
	long int colorSize = (long int) width * height * sizeof(sRGB8);
	long int opacitySize = (long int) width * height * sizeof(unsigned short);
	mb = (long int) (zBufferSize + alphaSize + image16Size + image8Size + imageFloatSize + colorSize + opacitySize) / 1024 / 1024;

	return mb;
}

void cImage::SetImageParameters(sImageAdjustments adjustments)
{
	adj = adjustments;
	CalculateGammaTable();
}

unsigned char* cImage::ConvertTo8bit(void)
{
	for (long int i = 0; i < width * height; i++)
	{
		image8[i].R = image16[i].R / 256;
		image8[i].G = image16[i].G / 256;
		image8[i].B = image16[i].B / 256;
		image8[i].A = 255;
	}
	unsigned char* ptr = (unsigned char*) image8;
	return ptr;
}

sRGBA8 cImage::Interpolation(float x, float y)
{
	sRGBA8 colour = sRGBA8(0, 0, 0, 255);
	if (x >= 0 && x < width - 1 && y >= 0 && y < height - 1)
	{
		int ix = x;
		int iy = y;
		int rx = (x - ix) * 256;
		int ry = (y - iy) * 256;
		sRGBA8 k1 = image8[iy * width + ix];
		sRGBA8 k2 = image8[iy * width + ix + 1];
		sRGBA8 k3 = image8[(iy + 1) * width + ix];
		sRGBA8 k4 = image8[(iy + 1) * width + ix + 1];
		colour.R = (k1.R * (255 - rx) * (255 - ry) + k2.R * (rx) * (255 - ry) + k3.R * (255 - rx) * ry + k4.R * (rx * ry)) / 65536;
		colour.G = (k1.G * (255 - rx) * (255 - ry) + k2.G * (rx) * (255 - ry) + k3.G * (255 - rx) * ry + k4.G * (rx * ry)) / 65536;
		colour.B = (k1.B * (255 - rx) * (255 - ry) + k2.B * (rx) * (255 - ry) + k3.B * (255 - rx) * ry + k4.B * (rx * ry)) / 65536;
		colour.A = 255;
	}
	return colour;
}

unsigned char* cImage::CreatePreview(double scale, int visibleWidth, int visibleHeight, QWidget *widget = NULL)
{
	int w = width * scale;
	int h = height * scale;
	previewVisibleWidth = visibleWidth;
	previewVisibleHeight = visibleHeight;
	if (previewAllocated)
	{
		delete[] preview;
		delete[] preview2;
	}
	preview = new sRGBA8[w * h];
	preview2 = new sRGBA8[w * h];

	memset(preview, 0, (unsigned long int)sizeof(sRGBA8) * (w * h));
	memset(preview2, 0, (unsigned long int)sizeof(sRGBA8) * (w * h));
	previewAllocated = true;
	previewWidth = w;
	previewHeight = h;
	previewScale = scale;
	unsigned char* ptr = (unsigned char*) preview;

	if(widget)
		imageWidget = widget;

	return ptr;
}

void cImage::UpdatePreview(QVector<int> *list)
{
	if (previewAllocated)
	{
		int w = previewWidth;
		int h = previewHeight;

		if (width == w && height == h)
		{
			memcpy(preview, image8, width * height * sizeof(sRGBA8));
		}
		else
		{
			float scaleX = (float) width / w;
			float scaleY = (float) height / h;

			//number of pixels to sum
			int countX = (float) width / w + 1;
			int countY = (float) height / h + 1;
			int factor = countX * countY;

			float deltaX = scaleX / countX;
			float deltaY = scaleY / countY;

			int listIndex = 0;

			for (int y = 0; y < h; y++)
			{

				if(list)
				{
					if(listIndex >= list->size()) break;

					if(scaleY * y < list->at(listIndex) - (int)scaleY - 1) continue;

					while(scaleY * y - (int)scaleY - 1> list->at(listIndex))
					{
						listIndex++;
						if(listIndex >= list->size()) break;
					}
				}

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
								sRGBA8 oldPixel = Interpolation(xx, yy);
								R += oldPixel.R;
								G += oldPixel.G;
								B += oldPixel.B;
							}
						}//next i
					}//next j
					sRGBA8 newpixel;
					newpixel.R = R / factor;
					newpixel.G = G / factor;
					newpixel.B = B / factor;
					newpixel.A = 255;
					preview[x + y * w] = newpixel;
				}//next x
			}//next y
		}
		memcpy(preview2, preview, w * h * sizeof(sRGBA8));
	}
	else
	{
		fprintf(stderr, "Error! Preview has not been prepared\n");
	}
}

unsigned char* cImage::GetPreviewPtr(void)
{
	unsigned char* ptr = 0;
	if (previewAllocated)
	{
		ptr = (unsigned char*) preview2;
	} else abort();
	return ptr;
}

bool cImage::IsPreview(void)
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
		QTextStream out(stdout);
		//out << "Redraw" << endl;

		QWidget *widget;
		if (qwidget)
			widget = qwidget;
		else
			widget = imageWidget;

		QPainter painter(widget);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

		QPen pen(Qt::white, 2, Qt::SolidLine);
		painter.setPen(pen);
		painter.drawLine(0, 0, 400, 20);

		QImage qimage((const uchar*)GetPreviewPtr(), previewWidth, previewHeight, previewWidth*sizeof(sRGBA8), QImage::Format_ARGB32);
		painter.drawImage(QRect(0,0,previewWidth,previewHeight), qimage, QRect(0,0,previewWidth, previewHeight));
		memcpy(preview2, preview, previewWidth * previewHeight * sizeof(sRGBA8));
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
		unsigned short alphaTemp = alphaBuffer[x + y * width];
		unsigned short opacityTemp = opacityBuffer[x + y * width];

		for (int yy = 0; yy < pFactor; yy++)
		{
			for (int xx = 0; xx < pFactor; xx++)
			{
				if (xx == 0 && yy == 0) continue;
				imageFloat[x + xx + (y + yy) * width] = pixelTemp;
				zBuffer[x + xx + (y + yy) * width] = zBufferTemp;
				colourBuffer[x + xx + (y + yy) * width] = colourTemp;
				alphaBuffer[x + xx + (y + yy) * width] = alphaTemp;
				opacityBuffer[x + xx + (y + yy) * width] = opacityTemp;
			}
		}
	}
}

void cImage::PutPixelAlfa(int x, int y, float z, sRGB8 color, double opacity, int layer)
{
	if (x >= 0 && x < previewWidth && y >= 0 && y < previewHeight)
	{
		size_t address = x + y * previewWidth;
		float zImage = GetPixelZBuffer(x / previewScale, y / previewScale);
		if (z > zImage) opacity *= 0.03;
		sRGBA8 oldPixel;
		if(layer == 0)
			oldPixel = preview[address];
		else if (layer == 1)
			oldPixel = preview2[address];
		sRGBA8 newPixel;
		newPixel.R = (oldPixel.R * (1.0 - opacity) + color.R * opacity);
		newPixel.G = (oldPixel.G * (1.0 - opacity) + color.G * opacity);
		newPixel.B = (oldPixel.B * (1.0 - opacity) + color.B * opacity);
		if(layer == 0)
		{
			preview[address] = newPixel;
			preview2[address] = newPixel;
		}
		else if (layer == 1)
			preview2[address] = newPixel;
	}
}

void cImage::AntiAliasedPoint(double x, double y, float z, sRGB8 color, double opacity, int layer)
{
	double deltaX = x - (int) x;
	double deltaY = y - (int) y;

	double intensity1 = (1.0 - deltaX) * (1.0 - deltaY);
	double intensity2 = (deltaX) * (1.0 - deltaY);
	double intensity3 = (1.0 - deltaX) * (deltaY);
	double intensity4 = (deltaX) * (deltaY);
	double sum = intensity1 + intensity2 + intensity3 + intensity4;

	double opacity2;

	opacity2 = opacity * intensity1 / sum;
	PutPixelAlfa(x, y, z, color, opacity2, layer);

	opacity2 = opacity * intensity2 / sum;
	PutPixelAlfa(x + 1, y, z, color, opacity2, layer);

	opacity2 = opacity * intensity3 / sum;
	PutPixelAlfa(x, y + 1, z, color, opacity2, layer);

	opacity2 = opacity * intensity4 / sum;
	PutPixelAlfa(x + 1, y + 1, z, color, opacity2, layer);
}

void cImage::AntiAliasedLine(double x1, double y1, double x2, double y2, float z1, float z2, sRGB8 color, double opacity, int layer)
{
	if ((x1 >= 0 && x1 < previewWidth && y1 >= 0 && y1 < previewHeight) || (x2 >= 0 && x2 < previewWidth && y2 >= 0 && y2 < previewHeight))
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
				if(start < 0) start = 0;
				int end = xx2;
				if(end > previewWidth) end = previewWidth;

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
						opacity2 = opacity * (1.0 - distance);
						if (intX == start)
						{
							opacity2 = opacity * (1.0 - (xx1 - x)) * (1.0 - distance);
						}
						if (intX == end)
						{
							opacity2 = opacity * ((xx2 - x)) * (1.0 - distance);
						}
						PutPixelAlfa(xx, yy, z, color, opacity2, layer);
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
				if(start < 0) start = 0;
				int end = yy2;
				if(end > previewHeight) end = previewHeight;

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
						opacity2 = opacity * (1.0 - distance);
						if (intY == start)
						{
							opacity2 = opacity * (1.0 - (yy1 - (y))) * (1.0 - distance);
						}
						if (intY == end)
						{
							opacity2 = opacity * ((yy2 - (y))) * (1.0 - distance);
						}
						PutPixelAlfa(xx, yy, z, color, opacity2, layer);
					}
				}
			}
		}
	}
}

void cImage::CircleBorder(double x, double y, float z, double r, sRGB8 border, double borderWidth, double opacity, int layer)
{
  if(borderWidth>0 && r>0)
  {
    double r2 = r + borderWidth;
    double r1 = r - borderWidth;
    if(r1<0) r1 = 0;
    int y1 = y - r2;
    int y2 = y + r2;

    double wspJ = 1.0/borderWidth * opacity;

    for (int yy = y1; yy<=y2; yy++)
    {
      double dyy = yy - y;
      double dxx = r2*r2 - dyy*dyy;
      if(dxx < 0) dxx = 0;
      double dx = sqrt(dxx);
      int x1 = x - dx;
      int x2 = x + dx;

      double dxx2 = r1*r1 - dyy*dyy;
      if(dxx2 < 0) dxx2 = 0;
      double dx2 = sqrt(dxx2);
      int x12 = x - dx2;
      int x22 = x + dx2;

      for(int xx = x1; xx<=x2; xx++)
      {
        if(xx<=x12 || xx>=x22)
        {
          double deltaX = xx - x;
          double deltaY = yy - y;
          double rr = sqrt(deltaX*deltaX + deltaY*deltaY);
          double deltaR = fabs(rr - r);
          if(deltaR > borderWidth) deltaR = borderWidth;
          double opacity2 = wspJ * (borderWidth - deltaR);
          PutPixelAlfa(xx,yy, z, border, opacity2, layer);
        }
      }
    }
  }
}

