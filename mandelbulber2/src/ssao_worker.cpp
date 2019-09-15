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
 * cSSAOWorker class - worker for multi thread SSAO effect
 *
 * SSAO shades inner edges and corners. For each pixel the surrounding
 * z-buffer (depth from camera) is scanned and based on the angle of the surface
 * each pixel will be shaded. This class gets used in render_ssao.cpp as multiple threads.
 */

#include "ssao_worker.h"

#include "cimage.hpp"
#include "color_structures.hpp"
#include "common_math.h"
#include "fractparams.hpp"
#include "render_data.hpp"

cSSAOWorker::cSSAOWorker(
	const sParamRender *_params, sThreadData *_threadData, const sRenderData *_data, cImage *_image)
{
	params = _params;
	data = _data;
	image = _image;
	threadData = _threadData;
}

cSSAOWorker::~cSSAOWorker()
{
	// nothing to destroy
}

void cSSAOWorker::doWork()
{
	int quality = threadData->quality;
	int startLineInit = threadData->startLine;
	int startLine = threadData->region.y1;
	int endLine = threadData->region.y2;
	int width = threadData->region.width;
	int height = threadData->region.height;
	int startX = threadData->region.x1;
	int endX = threadData->region.x2;
	sRGBFloat aoColor =	threadData->color;

	double *cosine = new double[quality];
	double *sine = new double[quality];
	for (int i = 0; i < quality; i++)
	{
		sine[i] = sin(double(i) / quality * 2.0 * M_PI);
		cosine[i] = cos(double(i) / quality * 2.0 * M_PI);
	}

	params::enumPerspectiveType perspectiveType = params->perspectiveType;

	double scale_factor = double(width) / (quality * quality) / 2.0;
	double aspectRatio = double(width) / height;

	if (perspectiveType == params::perspEquirectangular) aspectRatio = 2.0;

	double fov = params->fov;

	float intensity = params->ambientOcclusion;

	int listIndex = 0;

	int step = threadData->progressive;
	if (step == 0) step = 1;

	for (int y = startLineInit; y < endLine; y += threadData->noOfThreads)
	{
		if (threadData->list)
		{
			if (listIndex >= threadData->list->size())
			{
				continue;
			}
			if (y < threadData->list->at(listIndex))
			{
				continue;
			}
			else
			{
				listIndex++;
			}
		}
		for (int x = startX; x < endX; x += step)
		{
			double z = double(image->GetPixelZBuffer(x, y));
			unsigned short opacity16 = image->GetPixelOpacity(x, y);
			float opacity = opacity16 / 65535.0f;
			float total_ambient = 0.0f;

			if (z < 1e19)
			{
				double x2, y2;
				if (perspectiveType == params::perspFishEye || perspectiveType == params::perspFishEyeCut)
				{
					x2 = M_PI * (double(x - startX) / width - 0.5) * aspectRatio;
					y2 = M_PI * (double(y - startLine) / height - 0.5);
					double r = sqrt(x2 * x2 + y2 * y2);
					if (r != 0.0)
					{
						x2 = x2 / r * sin(r * fov) * z;
						y2 = y2 / r * sin(r * fov) * z;
					}
				}
				else if (perspectiveType == params::perspEquirectangular)
				{
					x2 = M_PI * (double(x - startX) / width - 0.5) * aspectRatio;
					y2 = M_PI * (double(y - startLine) / height - 0.5);
					x2 = sin(fov * x2) * cos(fov * y2) * z;
					y2 = sin(fov * y2) * z;
				}
				else
				{
					x2 = (double(x - startX) / width - 0.5) * aspectRatio;
					y2 = double(y - startLine) / height - 0.5;
					x2 = x2 * z * fov;
					y2 = y2 * z * fov;
				}

				float ambient = 0.0f;
				double angleStep = M_PI * 2.0 / double(quality);
				int maxRandom = 62831 / quality;
				double rRandom = 1.0;

				if (params->SSAO_random_mode) rRandom = 0.5 + Random(65536) / 65536.0;

				for (int angleIndex = 0; angleIndex < quality; angleIndex++)
				{
					double ca, sa;
					double angle = angleIndex;
					if (params->SSAO_random_mode)
					{
						angle = angleStep * angleIndex + Random(maxRandom) / 10000.0;
						ca = cos(angle);
						sa = sin(angle);
					}
					else
					{
						ca = cosine[(int)angle];
						sa = sine[(int)angle];
					}

					double max_diff = -1e50;

					for (double r = 1.0; r < quality; r += rRandom)
					{
						double rr = r * r * scale_factor;
						double xx = x + rr * ca;
						double yy = y + rr * sa;

						if (int(xx) == x && int(yy) == y) continue;
						if (xx < startX || xx > endX - 1 || yy < startLine || yy > endLine - 1) continue;
						double z2 = double(image->GetPixelZBuffer(int(xx), int(yy)));

						double xx2, yy2;
						if (perspectiveType == params::perspFishEye
								|| perspectiveType == params::perspFishEyeCut)
						{
							xx2 = M_PI * ((xx - startX) / width - 0.5) * aspectRatio;
							yy2 = M_PI * ((yy - startLine) / height - 0.5);
							double r2 = sqrt(xx2 * xx2 + yy2 * yy2);
							if (r != 0.0)
							{
								xx2 = xx2 / r2 * sin(r2 * fov) * z2;
								yy2 = yy2 / r2 * sin(r2 * fov) * z2;
							}
						}
						else if (perspectiveType == params::perspEquirectangular)
						{
							xx2 = M_PI * ((xx - startX) / width - 0.5) * aspectRatio;
							yy2 = M_PI * ((yy - startLine) / height - 0.5);
							xx2 = sin(fov * xx2) * cos(fov * yy2) * z2;
							yy2 = sin(fov * yy2) * z2;
						}
						else
						{
							xx2 = ((xx - startX) / width - 0.5) * aspectRatio;
							yy2 = (yy - startLine) / height - 0.5;
							xx2 = xx2 * (z2 * fov);
							yy2 = yy2 * (z2 * fov);
						}

						double dx = xx2 - x2;
						double dy = yy2 - y2;
						double dz = z2 - z;
						double dr = sqrt(dx * dx + dy * dy);
						double diff = -dz / dr;

						if (diff > max_diff) max_diff = diff;
					}
					double max_angle = atan(max_diff);

					ambient += -max_angle / M_PI + 0.5;
				}

				total_ambient = ambient / quality;
				if (total_ambient < 0) total_ambient = 0;
			}

			for (int xx = 0; xx < step; xx++)
			{
				if (x + xx >= endX - 1) break;
				sRGB8 colour = image->GetPixelColor(x + xx, y);
				sRGBFloat pixel = image->GetPixelPostImage(x + xx, y);
				float shadeFactor = 1.0f / 256.0f * total_ambient * intensity * (1.0f - opacity);
				pixel.R = pixel.R + colour.R * shadeFactor * aoColor.R;
				pixel.G = pixel.G + colour.G * shadeFactor * aoColor.G;
				pixel.B = pixel.B + colour.B * shadeFactor * aoColor.B;
				image->PutPixelPostImage(x + xx, y, pixel);
			}
		}

		threadData->done++;

		if (threadData->stopRequest) break;
	}
	delete[] sine;
	delete[] cosine;

	// emit signal to main thread when finished
	emit finished();
	return;
}
