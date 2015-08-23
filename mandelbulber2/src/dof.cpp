/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * PostRendering_DOF - calculation of Depth of Field effect
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

#include "dof.hpp"
#include "common_math.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "progress_text.hpp"
#include "global_data.hpp"

cPostRenderingDOF::cPostRenderingDOF(cImage *_image) : QObject(), image(_image)
{
}

void cPostRenderingDOF::Render(double deep, double neutral, bool *stopRequest)
{
	int width = image->GetWidth();
	int height = image->GetHeight();

	sRGB16 *temp_image = new sRGB16[width * height];
	unsigned short *temp_alpha = new unsigned short[width * height];
	sSortZ<float> *temp_sort = new sSortZ<float>[width * height];
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int ptr = x + y * width;
			temp_image[ptr] = image->GetPixelImage16(x, y);
			temp_alpha[ptr] = image->GetPixelAlpha(x, y);
			temp_sort[ptr].z = image->GetPixelZBuffer(x, y);
			temp_sort[ptr].i = ptr;
		}
	}

	QString statusText = QObject::tr("Rendering Depth Of Field effect");
	QString progressTxt;

	emit updateProgressAndStatus(statusText, QObject::tr("Sorting zBuffer"), 0.0);
	gApplication->processEvents();

	QuickSortZBuffer(temp_sort, 1, height * width - 1);

	emit updateProgressAndStatus(statusText, QObject::tr("Randomizing zBuffer"), 0.0);
	gApplication->processEvents();


	//Randomize Z-buffer
	int imgSize = height*width;
	for (int i = imgSize - 1; i >= 0; i--)
	{
		if (*stopRequest)
		{
			// delete allocated memory
			delete [] temp_image;
			delete [] temp_alpha;
			delete [] temp_sort;
			return;
		}
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

				if (sizeCompare > 0.9) //originally was 0.7, but 0.9 will give better quality
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

	for (int i = 0; i < height * width; i++)
	{
		if (*stopRequest)
		{
			// delete allocated memory
			delete [] temp_image;
			delete [] temp_alpha;
			delete [] temp_sort;
			return;
		}

		int ii = temp_sort[height * width - i - 1].i;
		int x = ii % width;
		int y = ii / width;
		double z = image->GetPixelZBuffer(x, y);
		double blur = fabs((double) z - neutral) / z * deep;
		if (blur > 100) blur = 100.0;
		int size = blur;
		sRGB16 center = temp_image[x + y * width];
		unsigned short center_alpha = temp_alpha[x + y * width];
		double factor = blur * blur * sqrt(blur)* M_PI/3.0;

		#pragma omp parallel for
		for (int yy = y - size; yy <= y + size; yy++)
		{
			for (int xx = x - size; xx <= x + size; xx++)
			{
				if (xx >= 0 && xx < width && yy >= 0 && yy < height)
				{
					int dx = xx - x;
					int dy = yy - y;
					double r = sqrt((float)dx * dx + dy * dy);
					double op = (blur - r) / factor;
					if (op > 1.0) op = 1.0;
					if (op < 0.0) op = 0.0;
					if (op > 0.0)
					{
						double opN = 1.0 - op;
						sRGB16 old = image->GetPixelImage16(xx, yy);
						unsigned short old_alpha = image->GetPixelAlpha(xx, yy);
						sRGB16 pixel;
						pixel.R = old.R * opN + center.R * op;
						pixel.G = old.G * opN + center.G * op;
						pixel.B = old.B * opN + center.B * op;
						unsigned short alpha = old_alpha * opN + center_alpha * op;
						image->PutPixelImage16(xx, yy, pixel);
						image->PutPixelAlpha(xx, yy, alpha);
					}
				}
			}
		}

		if(timerRefreshProgressBar.elapsed() > 100)
		{
			timerRefreshProgressBar.restart();

			percentDone = (double)i / (height * width);
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
			timerRefresh.restart();
		}
	}

	emit updateProgressAndStatus(statusText, "Finished", 1.0);

	delete[] temp_image;
	delete[] temp_alpha;
	delete[] temp_sort;
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
        while(buffer[i].z < pivot.z)
        {
            i++;
        }
        while(pivot.z < buffer[j].z)
        {
            j--;
        }
        if(i <= j)
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
    }
    while(i <= j);

    // sort left side of pivot element
    if(l < j)
    {
        QuickSortZBuffer(buffer, l, j);
    }

    // sort right side of pivot element
    if(i < r)
    {
        QuickSortZBuffer(buffer, i, r);
    }
}
template void cPostRenderingDOF::QuickSortZBuffer<double>(sSortZ<double> *dane, int l, int p);
