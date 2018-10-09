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
 * PostRendering_DOF - calculation of Depth of Field effect
 */

#include "dof.hpp"

#include <algorithm>

#include "common_math.h"
#include "global_data.hpp"
#include "progress_text.hpp"

using std::max;
using std::min;

cPostRenderingDOF::cPostRenderingDOF(cImage *_image) : QObject(), image(_image)
{
}

void cPostRenderingDOF::Render(cRegion<int> screenRegion, float deep, float neutral,
	int numberOfPasses, float blurOpacity, float maxRadius, bool *stopRequest)
{
	int imageWidth = image->GetWidth();
	int imageHeight = image->GetHeight();

	sRGBFloat *temp_image = new sRGBFloat[quint64(imageWidth) * quint64(imageHeight)];
	unsigned short *temp_alpha = new unsigned short[quint64(imageWidth) * quint64(imageHeight)];
	quint64 sortBufferSize = quint64(screenRegion.height) * quint64(screenRegion.width);
	sSortZ<float> *temp_sort = new sSortZ<float>[sortBufferSize];

	{
		quint64 index = 0;
		for (int y = screenRegion.y1; y < screenRegion.y2; y++)
		{
			for (int x = screenRegion.x1; x < screenRegion.x2; x++)
			{
				quint64 ptr = quint64(x) + quint64(y) * quint64(imageWidth);
				temp_image[ptr] = image->GetPixelPostImage(x, y);
				temp_alpha[ptr] = image->GetPixelAlpha(x, y);
				temp_sort[index].z = image->GetPixelZBuffer(x, y);
				temp_sort[index].i = ptr;
				index++;
			}
		}
	}

	QString statusText = QObject::tr("Rendering Depth Of Field effect - phase I");
	QString progressTxt;

	cProgressText progressText;
	progressText.ResetTimer();
	float percentDone = 0.0;

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

#pragma omp parallel for schedule(dynamic, 1)

			for (int x = screenRegion.x1; x < screenRegion.x2; x++)
			{
				float z = image->GetPixelZBuffer(x, y);
				float blur1 = (z - neutral) / z * deep;
				float blur = fabs(blur1);
				if (blur > maxRadius) blur = maxRadius;
				int size = int(blur);
				int xStart = max(x - size, 0);
				int xStop = min(x + size, screenRegion.x2 - 1);
				int yStart = max(y - size, 0);
				int yStop = min(y + size, screenRegion.y2 - 1);

				float totalWeight = 0.0f;
				sRGBFloat tempPixel;
				for (int yy = yStart; yy <= yStop; yy++)
				{
					for (int xx = xStart; xx <= xStop; xx++)
					{
						float dx = x - xx;
						float dy = y - yy;
						float r = sqrtf(dx * dx + dy * dy);
						float weight = blur - r;
						if (weight < 0.0f) weight = 0.0f;
						if (weight > 1.0f) weight = 1.0f;

						float z2 = image->GetPixelZBuffer(xx, yy);
						float blur2 = (z2 - neutral) / z2 * deep;
						if (blur1 > blur2)
						{
							if (blur1 * blur2 < 0)
							{
								weight = 0.0;
							}
							else
							{
								float weight2 = 0.0f;
								if (blur1 > 0.0f)
									weight2 = 1.1f - blur1 / blur2;
								else
									weight2 = 1.1f - blur2 / blur1;
								if (weight2 < 0.0f) weight2 = 0.0f;
								weight *= weight2 * 10.0f;
							}
						}

						totalWeight += weight;
						if (weight > 0.0f)
						{
							sRGBFloat pix = image->GetPixelPostImage(xx, yy);
							tempPixel.R += pix.R * weight;
							tempPixel.G += pix.G * weight;
							tempPixel.B += pix.B * weight;
						}
					}
				}

				sRGBFloat newPixel;
				if (totalWeight > 0.0f)
				{
					newPixel = sRGBFloat(
						tempPixel.R / totalWeight, tempPixel.G / totalWeight, tempPixel.B / totalWeight);
				}
				else
				{
					newPixel = image->GetPixelPostImage(x, y);
				}
				quint64 ptr = quint64(x) + quint64(y) * quint64(imageWidth);
				temp_image[ptr] = newPixel;
			}

			if (timerRefreshProgressBar.elapsed() > 100)
			{
				timerRefreshProgressBar.restart();

				percentDone = float(y - screenRegion.y1) / screenRegion.height;
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
				image->PutPixelPostImage(x, y, temp_image[ptr]);
			}
		}

		image->CompileImage();
		if (image->IsPreview())
		{
			image->ConvertTo8bit();
			image->UpdatePreview();
			emit updateImage();
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

			for (qint64 i = sortBufferSize - 1; i >= 0; i--)
			{
				if (*stopRequest) throw tr("DOF terminated");
				sSortZ<float> temp;
				temp = temp_sort[i];
				float z1 = temp.z;
				float size1 = (z1 - neutral) / z1 * deep;

				qint64 randomStep = i;

				bool done = false;
				qint64 ii;
				do
				{
					ii = i - Random(randomStep);
					if (ii <= 0) ii = 0;
					sSortZ<float> temp2 = temp_sort[ii];
					float z2 = temp2.z;
					float size2 = (z2 - neutral) / z2 * deep;

					if (size1 * size2 > 0)
					{
						float sizeCompare;
						if (size1 > 0)
						{
							sizeCompare = size2 / size1;
						}
						else
						{
							sizeCompare = size1 / size2;
						}

						int intDiff = int((1.0f - sizeCompare) * 500);
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
					randomStep = int(randomStep * 0.7 - 1.0);

					if (randomStep <= 0) done = true;
				} while (!done);
				temp_sort[i] = temp_sort[ii];
				temp_sort[ii] = temp;
			}

			for (int i = 0; i < screenRegion.width; i++)
			{
				if (*stopRequest) throw tr("DOF terminated");
#pragma omp parallel for schedule(dynamic, 1)
				for (int j = 0; j < screenRegion.height; j++)
				{
					quint64 index = quint64(i) * quint64(screenRegion.height) + qint64(j);
					quint64 ii = temp_sort[sortBufferSize - index - 1].i;
					int x = int(ii % quint64(imageWidth));
					int y = int(ii / quint64(imageWidth));
					float z = image->GetPixelZBuffer(x, y);
					float blur = fabs(z - neutral) / z * deep + 1.0f;
					if (blur > maxRadius) blur = maxRadius;
					int size = int(blur);
					sRGBFloat center = temp_image[x + y * imageWidth];
					unsigned short center_alpha = temp_alpha[x + y * imageWidth];
					float blur_2 = blur * blur;
					float factor = (float(M_PI) * (blur_2 - blur) + 1.0f) / blurOpacity;

					for (int yy = y - size; yy <= y + size; yy++)
					{
						for (int xx = x - size; xx <= x + size; xx++)
						{
							if (xx >= screenRegion.x1 && xx < screenRegion.x2 && yy >= screenRegion.y1
									&& yy < screenRegion.y2)
							{
								int dx = xx - x;
								int dy = yy - y;
								float r_2 = dx * dx + dy * dy;
								if (blur_2 > r_2)
								{
									float r = sqrt(r_2);
									float op = (blur - r);
									if (op < 0.0f) op = 0.0f;
									if (op > 1.0f) op = 1.0f;
									op /= factor;
									if (op > 1.0f) op = 1.0f;
									image->BlendPixelPostImage(xx, yy, op, center);
									image->BlendPixelAlpha(xx, yy, op, center_alpha);
								}
							}
						}
					}
				}

				if (timerRefreshProgressBar.elapsed() > 100)
				{
					timerRefreshProgressBar.restart();

					percentDone =
						((float)pass + 1.0 + (float)i / screenRegion.width) / (numberOfPasses + 1.0);
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
					emit updateImage();

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

template <class T>
void cPostRenderingDOF::QuickSortZBuffer(sSortZ<T> *buffer, quint64 l, quint64 r)
{
	// Sorts buffer by value of z asc
	quint64 i, j;
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
template void cPostRenderingDOF::QuickSortZBuffer<float>(
	sSortZ<float> *buffer, quint64 l, quint64 p);
