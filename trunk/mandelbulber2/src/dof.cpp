/*
 * dof.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: krzysztof
 */

#include "dof.hpp"
#include "common_math.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "progress_text.hpp"
#include "interface.hpp"

void PostRendering_DOF(cImage *image, double deep, double neutral)
{
	//TODO button for DOF refresh
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

	QString statusText = QString("Rendering Depth Of Field effect");
	QString progressTxt;

	mainInterface->StatusText(statusText, "Sorting zBuffer", 0.0);
	mainInterface->application->processEvents();

	QuickSortZBuffer(temp_sort, 1, height * width - 1);

	mainInterface->StatusText(statusText, "Randomizing zBuffer", 0.0);
	mainInterface->application->processEvents();

	//Randomize Z-buffer
	int imgSize = height*width;
	for(int i=imgSize-1; i>=0; i--)
	{
		if (mainInterface->stopRequest)
		{
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

			if(size1 == 0 && size2 ==0) done = true;

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

				if (sizeCompare > 0.7)
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
			randomStep = randomStep * 0.7 - 1;

			if(randomStep <= 0) done = true;
		}
		while(!done);
		temp_sort[i] = temp_sort[ii];
		temp_sort[ii] = temp;
	}

	cProgressText progressText;
	progressText.ResetTimer();
	double percentDone = 0.0;

	mainInterface->StatusText(statusText, progressText.getText(0.0), 0.0);
	mainInterface->application->processEvents();

	QElapsedTimer timerRefresh;
	timerRefresh.start();
	qint64 lastRefreshTime = 0;

	QElapsedTimer timerRefreshProgressBar;
	timerRefreshProgressBar.start();

	for (int i = 0; i < height * width; i++)
	{
		if (mainInterface->stopRequest)
		{
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
			mainInterface->StatusText(statusText, progressTxt, percentDone);
			mainInterface->application->processEvents();
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
	mainInterface->StatusText(statusText, "Finished", 1.0);

	delete[] temp_image;
	delete[] temp_alpha;
	delete[] temp_sort;

}

template <class T>
void QuickSortZBuffer(sSortZ<T> *dane, int l, int p)
{
  int i,j; //robocze indeksy
  sSortZ<T> x,y; //robocze zmienne typu sortowany obiekt

  i=l;           //{ i : = minimalny klucz }
  j=p;           //{ j : = maksymalny klucz }
  x=dane[(l+p)/2];  //{ x - wyraz srodkowy sortowanej tablicy }
  do
  {             //{ cala historie bedziemy powtarzac tak dlugo jak . . . }
	while(dane[i].z < x.z)              //{ dopoki jestesmy na lewo od srodkowego }
	{
	  i++;                       //{ ...powiekszamy indeks i }
	}
	while(x.z < dane[j].z)                  //{ dopoki na prawo od srodkowego }
	{
	  j--;                       //{ ...zmniejszamy indeks j }
	}
	if(i<=j)                        //{ jezeli i <= j wtedy : }
	{                               //{ zamieniamy miejscami wyrazy i, j }
	  y=dane[i];                      //{ podstawienie do zmiennej roboczej }
	  dane[i]=dane[j];                    //{ pierwszy etap zamiany }
	  dane[j]=y;                      //{ drugi etap zamiany }
	  i++;                       //{ zwiekszenie indeksu i }
	  j--;                       //{ zmniejszenie indeksu j }
	}
  }
  while(i<=j);      //{ nie zostanie spelniony warunek i>j }
  if(l<j) QuickSortZBuffer(dane, l, j);
  if(i<p) QuickSortZBuffer(dane, i, p);
}
template void QuickSortZBuffer<double>(sSortZ<double> *dane, int l, int p);
