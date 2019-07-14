/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cPostEffectHdrBlur - Renders a weighted blur which works on HDR image data
 */

#include "post_effect_hdr_blur.h"

#include <QtCore>

#include "cimage.hpp"
#include "global_data.hpp"
#include "progress_text.hpp"
#include "system.hpp"

cPostEffectHdrBlur::cPostEffectHdrBlur(cImage *_image) : QObject(), image(_image)
{
	tempImage = new sRGBFloat[image->GetHeight() * image->GetWidth()];
	radius = 0;
	intensity = 0;
}

cPostEffectHdrBlur::~cPostEffectHdrBlur()
{
	delete[] tempImage;
}

void cPostEffectHdrBlur::Render(bool *stopRequest)
{

	memcpy(tempImage, image->GetPostImageFloatPtr(),
		image->GetHeight() * image->GetWidth() * sizeof(sRGBFloat));

	const double blurSize = radius * (image->GetWidth() + image->GetHeight()) * 0.001;
	const double blurSize2 = blurSize * blurSize;
	const int intBlurSize = int(blurSize + 1);
	const double limiter = intensity;

	QString statusText = QObject::tr("Rendering HDR Blur effect");
	QString progressTxt;

	cProgressText progressText;
	progressText.ResetTimer();
	double percentDone = 0.0;

	QElapsedTimer timerRefreshProgressBar;
	timerRefreshProgressBar.start();

	for (qint64 y = 0; y < qint64(image->GetHeight()); y++)
	{
		if (*stopRequest || systemData.globalStopRequest) break;

#pragma omp parallel for
		for (qint64 x = 0; x < qint64(image->GetWidth()); x++)
		{
			double weight = 0;
			int yStart = qMax(0LL, y - intBlurSize);
			int yEnd = qMin(qint64(image->GetHeight()), y + intBlurSize);

			sRGBFloat newPixel;

			for (int yy = yStart; yy < yEnd; yy++)
			{
				int xStart = qMax(0LL, x - intBlurSize);
				int xEnd = qMin(qint64(image->GetWidth()), x + intBlurSize);
				for (int xx = xStart; xx < xEnd; xx++)
				{
					double dx = x - xx;
					double dy = y - yy;
					double r2 = dx * dx + dy * dy;
					if (r2 < blurSize2)
					{
						double value = 1.0 / (r2 / (0.2 * blurSize) + limiter);
						// if(dx == 0 && dy == 0) value = 10.0;
						weight += value;
						sRGBFloat oldPixel = tempImage[xx + yy * image->GetWidth()];
						newPixel.R += oldPixel.R * value;
						newPixel.G += oldPixel.G * value;
						newPixel.B += oldPixel.B * value;
					}
				}
			}

			if (weight > 0)
			{
				newPixel.R /= weight;
				newPixel.G /= weight;
				newPixel.B /= weight;
			}
			image->PutPixelPostImage(x, y, newPixel);
		}

		if (timerRefreshProgressBar.elapsed() > 100)
		{
			timerRefreshProgressBar.restart();

			percentDone = float(y) / float(image->GetHeight());
			progressTxt = progressText.getText(percentDone);

			emit updateProgressAndStatus(statusText, progressTxt, percentDone);
			gApplication->processEvents();
		}
	}

	emit updateProgressAndStatus(statusText, progressText.getText(1.0), 1.0);
}

void cPostEffectHdrBlur::SetParameters(double _radius, double _intensity)
{
	radius = _radius;
	intensity = _intensity;
}
