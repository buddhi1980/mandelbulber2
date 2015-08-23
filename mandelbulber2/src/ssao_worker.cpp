/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cSSAOWorker class - worker for multithread SSAO effect
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

#include "ssao_worker.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "fractparams.hpp"

cSSAOWorker::cSSAOWorker(const cParamRender *_params, sThreadData *_threadData, const sRenderData *_data, cImage *_image)
{
	params = _params;
	data = _data;
	image = _image;
	threadData = _threadData;
}

cSSAOWorker::~cSSAOWorker()
{
	//nothing to destroy
}

void cSSAOWorker::doWork()
{
	int quality = threadData->quality;
	int startLine = threadData->startLine;
	int width = image->GetWidth();
	int height = image->GetHeight();

	double *cosine = new double[quality];
	double *sine = new double[quality];
	for (int i = 0; i < quality; i++)
	{
		sine[i] = sin((double) i / quality * 2.0 * M_PI);
		cosine[i] = cos((double) i / quality * 2.0 * M_PI);
	}

	double scale_factor = (double) width / (quality * quality) / 2.0;
	double aspectRatio = (double) width / height;

	params::enumPerspectiveType perspectiveType = params->perspectiveType;
	double fov = params->fov;

	double intensity = params->ambientOcclusion;

	int listIndex = 0;

	int step = threadData->progressive;
	if(step == 0) step = 1;

	for (int y = startLine; y < height; y += threadData->noOfThreads)
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
		for (int x = 0; x < width; x+=step)
		{
			double z = image->GetPixelZBuffer(x, y);
			unsigned short opacity16 = image->GetPixelOpacity(x, y);
			double opacity = opacity16 / 65535.0;
			double total_ambient = 0;

			if (z < 1e19)
			{
				//printf("SSAO point on object\n");
				double x2, y2;
				if (perspectiveType == params::perspFishEye)
				{
					x2 = M_PI * ((double) x / width - 0.5) * aspectRatio;
					y2 = M_PI * ((double) y / height - 0.5);
					double r = sqrt(x2 * x2 + y2 * y2);
					if (r != 0.0)
					{
						x2 = x2 / r * sin(r * fov) * z;
						y2 = y2 / r * sin(r * fov) * z;
					}
				}
				else if (perspectiveType == params::perspEquirectangular)
				{
					x2 = M_PI * ((double) x / width - 0.5) * aspectRatio;
					y2 = M_PI * ((double) y / height - 0.5);
					x2 = sin(fov * x2) * cos(fov * y2) * z;
					y2 = sin(fov * y2) * z;
				}
				else
				{
					x2 = ((double) x / width - 0.5) * aspectRatio;
					y2 = ((double) y / height - 0.5);
					x2 = x2 * z * fov;
					y2 = y2 * z * fov;
				}

				double ambient = 0;

				for (int angle = 0; angle < quality; angle++)
				{
					double ca = cosine[angle];
					double sa = sine[angle];

					double max_diff = -1e50;

					for (double r = 1.0; r < quality; r += 1.0)
					{
						double rr = r * r * scale_factor;
						double xx = x + rr * ca;
						double yy = y + rr * sa;

						if ((int) xx == (int) x && (int) yy == (int) y) continue;
						if (xx < 0 || xx > width - 1 || yy < 0 || yy > height - 1) continue;
						double z2 = image->GetPixelZBuffer(xx, yy);

						double xx2, yy2;
						if (perspectiveType == params::perspFishEye)
						{
							xx2 = M_PI * ((double) xx / width - 0.5) * aspectRatio;
							yy2 = M_PI * ((double) yy / height - 0.5);
							double r2 = sqrt(xx2 * xx2 + yy2 * yy2);
							if (r != 0.0)
							{
								xx2 = xx2 / r2 * sin(r2 * fov) * z2;
								yy2 = yy2 / r2 * sin(r2 * fov) * z2;
							}
						}
						else if (perspectiveType == params::perspEquirectangular)
						{
							xx2 = M_PI * (xx / width - 0.5) * aspectRatio;
							yy2 = M_PI * (yy / height - 0.5);
							xx2 = sin(fov * xx2) * cos(fov * yy2) * z2;
							yy2 = sin(fov * yy2) * z2;
						}
						else
						{
							xx2 = (xx / width - 0.5) * aspectRatio;
							yy2 = (yy / height - 0.5);
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
				if (xx >= width - 1) break;
				sRGB8 colour = image->GetPixelColor(x + xx, y);
				sRGB16 pixel = image->GetPixelImage16(x + xx, y);
				double R = pixel.R + 65535.0 * colour.R / 256.0 * total_ambient * intensity * (1.0 - opacity);
				double G = pixel.G + 65535.0 * colour.G / 256.0 * total_ambient * intensity * (1.0 - opacity);
				double B = pixel.B + 65535.0 * colour.B / 256.0 * total_ambient * intensity * (1.0 - opacity);
				if (R > 65535) R = 65535;
				if (G > 65535) G = 65535;
				if (B > 65535) B = 65535;

				pixel.R = R;
				pixel.G = G;
				pixel.B = B;
				image->PutPixelImage16(x + xx, y, pixel);
			}

}

		threadData->done++;

		if(threadData->stopRequest)	break;
	}
	delete[] sine;
	delete[] cosine;

	//emit signal to main thread when finished
	emit finished();
	return;
}
