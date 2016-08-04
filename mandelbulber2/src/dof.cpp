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
 * PostRendering_DOF - calculation of Depth of Field effect
 */

#include "dof.hpp"

#include "common_math.h"
#include "global_data.hpp"
#include "progress_text.hpp"
#include <algorithm>

using std::max;
using std::min;

#define MAX_DOF_BLUR_SIZE 500.0

cPostRenderingDOF::cPostRenderingDOF(cImage *_image) : QObject(), image(_image)
{
}

void cPostRenderingDOF::Render(cRegion<int> screenRegion, double deep, double neutral,
	bool floatVersion, int numberOfPasses, double blurOpacity, bool *stopRequest)
{
	int imageWidth = image->GetWidth();
	int imageHeight = image->GetHeight();

	if (floatVersion)
	{
		sRGBfloat *temp_image = new sRGBfloat[imageWidth * imageHeight];
		unsigned short *temp_alpha = new unsigned short[imageWidth * imageHeight];
		long int sortBufferSize = screenRegion.height * screenRegion.width;
		sSortZ<float> *temp_sort = new sSortZ<float>[sortBufferSize];
		long int index = 0;
		for (int y = screenRegion.y1; y < screenRegion.y2; y++)
		{
			for (int x = screenRegion.x1; x < screenRegion.x2; x++)
			{
				int ptr = x + y * imageWidth;
				temp_image[ptr] = image->GetPixelImage(x, y);
				temp_alpha[ptr] = image->GetPixelAlpha(x, y);
				temp_sort[index].z = image->GetPixelZBuffer(x, y);
				temp_sort[index].i = ptr;
				index++;
			}
		}

		QString statusText = QObject::tr("Rendering Depth Of Field effect - phase I");
		QString progressTxt;

		cProgressText progressText;
		progressText.ResetTimer();
		double percentDone = 0.0;

		emit updateProgressAndStatus(statusText, progressText.getText(0.0), 0.0);
		gApplication->processEvents();

		QElapsedTimer timerRefresh;
		timerRefresh.start();
		qint64 lastRefreshTime = 0;

		QElapsedTimer timerRefreshProgressBar;
		timerRefreshProgressBar.start();

		try
		{
			// preprocessing (1-st phase)
			for (int y = screenRegion.y1; y < screenRegion.y2; y++)
			{
				if (*stopRequest) throw tr("DOF terminated");

#pragma omp parallel for

				for (int x = screenRegion.x1; x < screenRegion.x2; x++)
				{
					double z = image->GetPixelZBuffer(x, y);
					double blur1 = (z - neutral) / z * deep;
					double blur = fabs(blur1);
					if (blur > MAX_DOF_BLUR_SIZE) blur = MAX_DOF_BLUR_SIZE;
					int size = blur;
					int xStart = max(x - size, 0);
					int xStop = min(x + size, screenRegion.x2 - 1);
					int yStart = max(y - size, 0);
					int yStop = min(y + size, screenRegion.y2 - 1);

					double totalWeight = 0;
					sRGBfloat tempPixel;
					for (int yy = yStart; yy <= yStop; yy++)
					{
						for (int xx = xStart; xx <= xStop; xx++)
						{
							double dx = x - xx;
							double dy = y - yy;
							double r = sqrt(dx * dx + dy * dy);
							double weight = blur - r;
							if (weight < 0.0) weight = 0.0;
							if (weight > 1.0) weight = 1.0;

							double z2 = image->GetPixelZBuffer(xx, yy);
							double blur2 = (z2 - neutral) / z2 * deep;
							if (blur1 > blur2)
							{
								if (blur1 * blur2 < 0)
								{
									weight = 0.0;
								}
								else
								{
									double weight2 = 0.0;
									if (blur1 > 0)
										weight2 = 1.1 - blur1 / blur2;
									else
										weight2 = 1.1 - blur2 / blur1;
									if (weight2 < 0.0) weight2 = 0.0;
									weight *= weight2 * 10.0;
								}
							}

							totalWeight += weight;
							if (weight > 0.0)
							{
								sRGBfloat pix = image->GetPixelImage(xx, yy);
								tempPixel.R += pix.R * weight;
								tempPixel.G += pix.G * weight;
								tempPixel.B += pix.B * weight;
							}
						}
					}

					sRGBfloat newPixel;
					if (totalWeight > 0.0)
					{
						newPixel = sRGBfloat(
							tempPixel.R / totalWeight, tempPixel.G / totalWeight, tempPixel.B / totalWeight);
					}
					else
					{
						newPixel = image->GetPixelImage(x, y);
					}
					int ptr = x + y * imageWidth;
					temp_image[ptr] = newPixel;
				}

				if (timerRefreshProgressBar.elapsed() > 100)
				{
					timerRefreshProgressBar.restart();

					percentDone = (double)(y - screenRegion.y1) / screenRegion.height;
					progressTxt = progressText.getText(percentDone / (numberOfPasses + 1));

					emit updateProgressAndStatus(statusText, progressTxt, percentDone / (numberOfPasses + 1));
					gApplication->processEvents();
				}
			}

			for (int y = screenRegion.y1; y < screenRegion.y2; y++)
			{
				for (int x = screenRegion.x1; x < screenRegion.x2; x++)
				{
					int ptr = x + y * imageWidth;
					image->PutPixelImage(x, y, temp_image[ptr]);
				}
			}

			image->CompileImage();
			if (image->IsPreview())
			{
				image->ConvertTo8bit();
				image->UpdatePreview();
				image->GetImageWidget()->update();
			}

			// 2-nd phase - reversed blur

			statusText = QObject::tr("Rendering Depth Of Field effect - phase II");

			emit updateProgressAndStatus(
				statusText, QObject::tr("Sorting zBuffer"), 1.0 / (numberOfPasses + 1.0));
			gApplication->processEvents();

			QuickSortZBuffer(temp_sort, 1, sortBufferSize - 1);

			for (int pass = 0; pass < numberOfPasses; pass++)
			{

				emit updateProgressAndStatus(
					statusText, QObject::tr("Randomizing zBuffer"), (pass + 1.0) / (numberOfPasses + 1.0));
				gApplication->processEvents();

				timerRefresh.restart();
				timerRefreshProgressBar.restart();
				lastRefreshTime = 0;

				// Randomize Z-buffer

				for (int i = sortBufferSize - 1; i >= 0; i--)
				{
					if (*stopRequest) throw tr("DOF terminated");
					sSortZ<float> temp;
					temp = temp_sort[i];
					double z1 = temp.z;
					double size1 = (z1 - neutral) / z1 * deep;

					int randomStep = i;

					bool done = false;
					int ii;
					do
					{
						ii = i - Random(randomStep);
						if (ii <= 0) ii = 0;
						sSortZ<float> temp2 = temp_sort[ii];
						double z2 = temp2.z;
						double size2 = (z2 - neutral) / z2 * deep;

						if (size1 == 0 && size2 == 0) done = true;

						if (size1 * size2 > 0)
						{
							double sizeCompare;
							if (size1 > 0)
							{
								sizeCompare = size2 / size1;
							}
							else
							{
								sizeCompare = size1 / size2;
							}

							int intDiff = (1.0 - sizeCompare) * 500;
							intDiff *= intDiff;
							if (intDiff < Random(10000))
							{
								done = true;
							}
							else
							{
								done = false;
							}
						}
						else
						{
							done = false;
						}
						randomStep = randomStep * 0.7 - 1.0;

						if (randomStep <= 0) done = true;
					} while (!done);
					temp_sort[i] = temp_sort[ii];
					temp_sort[ii] = temp;
				}

				for (int i = 0; i < screenRegion.width; i++)
				{
					if (*stopRequest) throw tr("DOF terminated");
#pragma omp parallel for
					for (int j = 0; j < screenRegion.height; j++)
					{
						int index = i * screenRegion.height + j;
						int ii = temp_sort[sortBufferSize - index - 1].i;
						int x = ii % imageWidth;
						int y = ii / imageWidth;
						double z = image->GetPixelZBuffer(x, y);
						double blur = fabs(z - neutral) / z * deep + 1.0;
						if (blur > MAX_DOF_BLUR_SIZE) blur = MAX_DOF_BLUR_SIZE;
						int size = blur;
						sRGBfloat center = temp_image[x + y * imageWidth];
						unsigned short center_alpha = temp_alpha[x + y * imageWidth];
						double blur_2 = blur * blur;
						double factor = (M_PI * (blur_2 - blur) + 1.0) / blurOpacity;

						for (int yy = y - size; yy <= y + size; yy++)
						{
							for (int xx = x - size; xx <= x + size; xx++)
							{
								if (xx >= screenRegion.x1 && xx < screenRegion.x2 && yy >= screenRegion.y1 && yy < screenRegion.y2)
								{
									int dx = xx - x;
									int dy = yy - y;
									double r_2 = dx * dx + dy * dy;
									if (blur_2 > r_2)
									{
										double r = sqrt(r_2);
										double op = (blur - r);
										if (op < 0.0) op = 0.0;
										if (op > 1.0) op = 1.0;
										op /= factor;
										if (op > 1.0) op = 1.0;
										image->BlendPixelImage(xx, yy, op, center);
										image->BlendPixelAlpha(xx, yy, op, center_alpha);
									}
								}
							}
						}
					}

					if (timerRefreshProgressBar.elapsed() > 100)
					{
						timerRefreshProgressBar.restart();

						percentDone = ((double)pass + 1.0 + (double)i / sortBufferSize) / (numberOfPasses + 1.0);
						progressTxt = progressText.getText(percentDone);

						emit updateProgressAndStatus(statusText, progressTxt, percentDone);
						gApplication->processEvents();
					}

					if (timerRefresh.elapsed() > lastRefreshTime && image->IsPreview())
					{
						timerRefresh.restart();

						image->CompileImage();
						image->ConvertTo8bit();
						image->UpdatePreview();
						image->GetImageWidget()->update();

						lastRefreshTime = timerRefresh.elapsed() * 20.0;
						if (lastRefreshTime < 100) lastRefreshTime = 100;
						timerRefresh.restart();
					}
				}
			} // next pass
			image->CompileImage();
			throw progressText.getText(1.0);
		}
		catch (QString &status)
		{
			emit updateProgressAndStatus(statusText, status, 1.0);
			delete[] temp_image;
			delete[] temp_alpha;
			delete[] temp_sort;
		}
	}
	else //**************** integer version compatible with SSAO *******************
	{
		sRGB16 *temp_image = new sRGB16[imageWidth * imageHeight];
		unsigned short *temp_alpha = new unsigned short[imageWidth * imageHeight];
		long int sortBufferSize = screenRegion.height * screenRegion.width;
		sSortZ<float> *temp_sort = new sSortZ<float>[sortBufferSize];
		long int index = 0;
		for (int y = screenRegion.y1; y < screenRegion.y2; y++)
		{
			for (int x = screenRegion.x1; x < screenRegion.x2; x++)
			{
				int ptr = x + y * imageWidth;
				temp_image[ptr] = image->GetPixelImage16(x, y);
				temp_alpha[ptr] = image->GetPixelAlpha(x, y);
				temp_sort[index].z = image->GetPixelZBuffer(x, y);
				temp_sort[index].i = ptr;
				index++;
			}
		}

		QString statusText = QObject::tr("Rendering Depth Of Field effect - phase I");
		QString progressTxt;

		cProgressText progressText;
		progressText.ResetTimer();
		double percentDone = 0.0;

		emit updateProgressAndStatus(statusText, progressText.getText(0.0), 0.0);
		gApplication->processEvents();

		QElapsedTimer timerRefresh;
		timerRefresh.start();
		qint64 lastRefreshTime = 0;

		QElapsedTimer timerRefreshProgressBar;
		timerRefreshProgressBar.start();

		try
		{
			// preprocessing (1-st phase)

			for (int y = screenRegion.y1; y < screenRegion.y2; y++)
			{
				if (*stopRequest) throw tr("DOF terminated");
#pragma omp parallel for
				for (int x = screenRegion.x1; x < screenRegion.x2; x++)
				{
					double z = image->GetPixelZBuffer(x, y);
					double blur1 = (z - neutral) / z * deep;
					double blur = fabs(blur1);
					if (blur > MAX_DOF_BLUR_SIZE) blur = MAX_DOF_BLUR_SIZE;
					int size = blur;
					int xStart = max(x - size, 0);
					int xStop = min(x + size, screenRegion.x2 - 1);
					int yStart = max(y - size, 0);
					int yStop = min(y + size, screenRegion.y2 - 1);

					double totalWeight = 0;
					sRGBfloat tempPixel;
					for (int yy = yStart; yy <= yStop; yy++)
					{
						for (int xx = xStart; xx <= xStop; xx++)
						{
							double dx = x - xx;
							double dy = y - yy;
							double r = sqrt(dx * dx + dy * dy);
							double weight = blur - r;
							if (weight < 0.0) weight = 0.0;
							if (weight > 1.0) weight = 1.0;

							double z2 = image->GetPixelZBuffer(xx, yy);
							double blur2 = (z2 - neutral) / z2 * deep;
							if (blur1 > blur2)
							{
								if (blur1 * blur2 < 0)
								{
									weight = 0.0;
								}
								else
								{
									double weight2 = 0.0;
									if (blur1 > 0)
										weight2 = 1.1 - blur1 / blur2;
									else
										weight2 = 1.1 - blur2 / blur1;
									if (weight2 < 0.0) weight2 = 0.0;
									weight *= weight2 * 10.0;
								}
							}

							totalWeight += weight;
							if (weight > 0.0)
							{
								sRGB16 pix = image->GetPixelImage16(xx, yy);
								tempPixel.R += pix.R * weight;
								tempPixel.G += pix.G * weight;
								tempPixel.B += pix.B * weight;
							}
						}
					}

					sRGB16 newPixel;
					if (totalWeight > 0.0)
					{
						newPixel = sRGB16(
							tempPixel.R / totalWeight, tempPixel.G / totalWeight, tempPixel.B / totalWeight);
					}
					else
					{
						newPixel = image->GetPixelImage16(x, y);
					}
					int ptr = x + y * imageWidth;
					temp_image[ptr] = newPixel;
				}

				if (timerRefreshProgressBar.elapsed() > 100)
				{
					timerRefreshProgressBar.restart();

					percentDone = (double)(y - screenRegion.y1) / screenRegion.height;
					progressTxt = progressText.getText(percentDone / (numberOfPasses + 1));

					emit updateProgressAndStatus(statusText, progressTxt, percentDone / (numberOfPasses + 1));
					gApplication->processEvents();
				}
			}

			for (int y = screenRegion.y1; y < screenRegion.y2; y++)
			{
				for (int x = screenRegion.x1; x < screenRegion.x2; x++)
				{
					int ptr = x + y * imageWidth;
					image->PutPixelImage16(x, y, temp_image[ptr]);
				}
			}

			if (image->IsPreview())
			{
				image->ConvertTo8bit();
				image->UpdatePreview();
				image->GetImageWidget()->update();
			}

			// 2-nd phase - reversed blur

			statusText = QObject::tr("Rendering Depth Of Field effect - phase II");

			emit updateProgressAndStatus(
				statusText, QObject::tr("Sorting zBuffer"), 1.0 / (numberOfPasses + 1.0));
			gApplication->processEvents();

			QuickSortZBuffer(temp_sort, 1, sortBufferSize - 1);

			for (int pass = 0; pass < numberOfPasses; pass++)
			{

				emit updateProgressAndStatus(
					statusText, QObject::tr("Randomizing zBuffer"), (pass + 1.0) / (numberOfPasses + 1.0));
				gApplication->processEvents();

				timerRefresh.restart();
				timerRefreshProgressBar.restart();
				lastRefreshTime = 0;

				// Randomize Z-buffer

				for (int i = sortBufferSize - 1; i >= 0; i--)
				{
					if (*stopRequest) throw tr("DOF terminated");
					sSortZ<float> temp;
					temp = temp_sort[i];
					double z1 = temp.z;
					double size1 = (z1 - neutral) / z1 * deep;

					int randomStep = i;

					bool done = false;
					int ii;
					do
					{
						ii = i - Random(randomStep);
						if (ii <= 0) ii = 0;
						sSortZ<float> temp2 = temp_sort[ii];
						double z2 = temp2.z;
						double size2 = (z2 - neutral) / z2 * deep;

						if (size1 == 0 && size2 == 0) done = true;

						if (size1 * size2 > 0)
						{
							double sizeCompare;
							if (size1 > 0)
							{
								sizeCompare = size2 / size1;
							}
							else
							{
								sizeCompare = size1 / size2;
							}

							int intDiff = (1.0 - sizeCompare) * 500;
							intDiff *= intDiff;
							if (intDiff < Random(10000))
							{
								done = true;
							}
							else
							{
								done = false;
							}
						}
						else
						{
							done = false;
						}
						randomStep = randomStep * 0.7 - 1.0;

						if (randomStep <= 0) done = true;
					} while (!done);
					temp_sort[i] = temp_sort[ii];
					temp_sort[ii] = temp;
				}

				for (int i = 0; i < screenRegion.width; i++)
				{
					if (*stopRequest) throw tr("DOF terminated");
#pragma omp parallel for
					for (int j = 0; j < screenRegion.height; j++)
					{
						int index = i * screenRegion.height + j;
						int ii = temp_sort[sortBufferSize - index - 1].i;
						int x = ii % imageWidth;
						int y = ii / imageWidth;
						double z = image->GetPixelZBuffer(x, y);
						double blur = fabs(z - neutral) / z * deep + 1.0;
						if (blur > MAX_DOF_BLUR_SIZE) blur = MAX_DOF_BLUR_SIZE;
						int size = blur;
						sRGB16 center = temp_image[x + y * imageWidth];
						unsigned short center_alpha = temp_alpha[x + y * imageWidth];
						double blur_2 = blur * blur;
						double factor = (M_PI * (blur_2 - blur) + 1.0) / blurOpacity;

						for (int yy = y - size; yy <= y + size; yy++)
						{
							for (int xx = x - size; xx <= x + size; xx++)
							{
								if (xx >= screenRegion.x1 && xx < screenRegion.x2 && yy >= screenRegion.y1 && yy < screenRegion.y2)
								{
									int dx = xx - x;
									int dy = yy - y;
									double r_2 = dx * dx + dy * dy;
									if (blur_2 > r_2)
									{
										double r = sqrt(r_2);
										double op = (blur - r);
										if (op < 0.0) op = 0.0;
										if (op > 1.0) op = 1.0;
										op /= factor;
										if (op > 1.0) op = 1.0;
										image->BlendPixelImage16(xx, yy, op, center);
										image->BlendPixelAlpha(xx, yy, op, center_alpha);
									}
								}
							}
						}
					} // next j

					if (timerRefreshProgressBar.elapsed() > 100)
					{
						timerRefreshProgressBar.restart();

						percentDone = ((double)pass + 1.0 + (double)i / sortBufferSize) / (numberOfPasses + 1.0);
						progressTxt = progressText.getText(percentDone);

						emit updateProgressAndStatus(statusText, progressTxt, percentDone);
						gApplication->processEvents();
					}

					if (timerRefresh.elapsed() > lastRefreshTime && image->IsPreview())
					{
						timerRefresh.restart();

						image->ConvertTo8bit();
						image->UpdatePreview();
						image->GetImageWidget()->update();

						lastRefreshTime = timerRefresh.elapsed() * 20.0;
						if (lastRefreshTime < 100) lastRefreshTime = 100;
						timerRefresh.restart();
					}
				} // next i
			}		// next pass
			throw progressText.getText(1.0);
		}
		catch (QString &status)
		{
			emit updateProgressAndStatus(statusText, status, 1.0);
			delete[] temp_image;
			delete[] temp_alpha;
			delete[] temp_sort;
		}
	}
}

template <class T>
void cPostRenderingDOF::QuickSortZBuffer(sSortZ<T> *buffer, int l, int r)
{
	// Sorts buffer by value of z asc
	int i, j;
	// Operating buffer typed variables
	sSortZ<T> pivot, swap;
	i = l;
	j = r;
	// set pivot to center of buffer
	pivot = buffer[(l + r) / 2];
	do
	{
		while (buffer[i].z < pivot.z)
		{
			i++;
		}
		while (pivot.z < buffer[j].z)
		{
			j--;
		}
		if (i <= j)
		{
			/* buffer[i] is to the left of pivot but greater
			 *  buffer[j] is to the right of pivot but smaller
			 * -> swap values of buffer[i] <> buffer[j] */
			swap = buffer[i];
			buffer[i] = buffer[j];
			buffer[j] = swap;
			i++;
			j--;
		}
	} while (i <= j);

	// sort left side of pivot element
	if (l < j)
	{
		QuickSortZBuffer(buffer, l, j);
	}

	// sort right side of pivot element
	if (i < r)
	{
		QuickSortZBuffer(buffer, i, r);
	}
}
template void cPostRenderingDOF::QuickSortZBuffer<double>(sSortZ<double> *buffer, int l, int p);
