/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRenderWorker class - worker for rendering image on single CPU core
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

#include "render_worker.hpp"
#include <QtCore>
#include "calculate_distance.hpp"
#include "region.hpp"
#include "common_math.h"


cRenderWorker::cRenderWorker(const cParamRender *_params, const cFourFractals *_fractal, sThreadData *_threadData, const sRenderData *_data, cImage *_image)
{
	params = _params;
	fractal = _fractal;
	data = _data;
	image = _image;
	threadData = _threadData;
	cameraTarget = NULL;
	reflectBuff = NULL;
	AOvectorsAround = NULL;
	AOvectorsCount = 0;
	baseX = CVector3(1.0, 0.0, 0.0);
	baseY = CVector3(0.0, 1.0, 0.0);
	baseZ = CVector3(0.0, 0.0, 1.0);
	maxraymarchingSteps = 10000;
	missed_DE_counter = 0;
	pixelCounter = 0;
	DECounter = 0;
	for(int i=0; i<256; i++)
	{
		histogramDE[i] = 0;
		histogramIters[i] = 0;
	}
	reflectionsMax = 0;
	stopRequest = false;
}

cRenderWorker::~cRenderWorker()
{
	if (cameraTarget)
	{
		delete cameraTarget;
		cameraTarget = NULL;
	}

	if (reflectBuff)
	{
		for (int i = 0; i < reflectionsMax + 1; i++)
		{
			delete[] reflectBuff[i].stepBuff;
		}
		delete[] reflectBuff;
	}

	if(AOvectorsAround)
	{
		delete AOvectorsAround;
		AOvectorsAround = NULL;
	}
}

//main render engine function called as multiple threads
void cRenderWorker::doWork(void)
{
	// here will be rendering thread
	QTextStream out(stdout);

	int width = image->GetWidth();
	int height = image->GetHeight();
	double aspectRatio = (double) width / height;

	PrepareMainVectors();
	PrepareReflectionBuffer();
	if (params->ambientOcclusionEnabled && params->ambientOcclusionMode == params::AOmodeMultipeRays) PrepareAOVectors();

	//init of scheduler
	cScheduler *scheduler = threadData->scheduler;


	//start point for ray-marching
	CVector3 start = params->camera;


	scheduler->InitFirstLine(threadData->id, threadData->startLine);
	//main loop for y
	for (int ys = threadData->startLine; scheduler->ThereIsStillSomethingToDo(threadData->id); ys = scheduler->NextLine(threadData->id, ys))
	{
		//skip if line is out of region
		if (ys < data->screenRegion.y1 || ys > data->screenRegion.y2) continue;

		//main loop for x
		for (int xs = 0; xs < width; xs += scheduler->GetProgresiveStep())
		{

			//break if by coincidence this thread started rendering the same line as some other
			if (scheduler->ShouldIBreak(threadData->id, ys))
			{
				break;
			}

			if (scheduler->GetProgresivePass() > 1 && xs % (scheduler->GetProgresiveStep() * 2) == 0 && ys % (scheduler->GetProgresiveStep() * 2) == 0) continue;

			//skip if pixel is out of region;
			if (xs < data->screenRegion.x1 || xs > data->screenRegion.x2) continue;

			//calculate point in image coordinate system
			CVector2<int> screenPoint(xs, ys);
			CVector2<double> imagePoint = data->screenRegion.transpose(data->imageRegion, screenPoint);
			imagePoint.x *= aspectRatio;

			//full dome shemisphere cut
			bool hemisphereCut = false;
			if (params->perspectiveType == params::perspFishEyeCut && imagePoint.Length() > 0.5 / params->fov) hemisphereCut = true;

			//calculate direction of ray-marching
			CVector3 viewVector = CalculateViewVector(imagePoint, params->fov, params->perspectiveType, mRot);

			//---------------- 1us -------------

			//Ray marching
			CVector3 point;
			CVector3 startRay = start;
			sRGBAfloat resultShader;
			sRGBAfloat objectColour;
			int rayEnd = 0;

			//raymarching loop (reflections)

			if (!hemisphereCut) //in fulldome mode, will not render pixels out of the fuldome
			{
				sRayMarchingOut rayMarchingOut;
				for (int ray = 0; ray <= reflectionsMax; ray++)
				{
					reflectBuff[ray].start = startRay;
					reflectBuff[ray].viewVector = viewVector;
					reflectBuff[ray].found = false;
					reflectBuff[ray].buffCount = 0;

					double minScan = ray == 0 ? params->viewDistanceMin : 0.0;

					sRayMarchingIn rayMarchingIn;
					rayMarchingIn.binaryEnable = true;
					rayMarchingIn.direction = viewVector;
					rayMarchingIn.maxScan = params->viewDistanceMax;
					rayMarchingIn.minScan = minScan;
					rayMarchingIn.start = startRay;

					sRayMarchingInOut rayMarchingInOut;
					rayMarchingInOut.buffCount = &reflectBuff[ray].buffCount;
					rayMarchingInOut.stepBuff = reflectBuff[ray].stepBuff;

					point = RayMarching(rayMarchingIn, &rayMarchingInOut, &rayMarchingOut);

					reflectBuff[ray].point = point;
					reflectBuff[ray].distThresh = rayMarchingOut.distThresh;
					reflectBuff[ray].objectType = rayMarchingOut.object;
					reflectBuff[ray].lastDist = rayMarchingOut.lastDist;
					reflectBuff[ray].found = rayMarchingOut.found;
					reflectBuff[ray].depth = rayMarchingOut.depth;
					reflectBuff[ray].objectColor = rayMarchingOut.objectColor;
					reflectBuff[ray].reflect = rayMarchingOut.objectReflect;

					rayEnd = ray;
					if (!reflectBuff[ray].found) break;
					if (reflectBuff[ray].reflect == 0) break;

					//calculate new ray direction and start point
					startRay = point;
					sShaderInputData shaderInputData;
					shaderInputData.distThresh = reflectBuff[ray].distThresh;
					shaderInputData.lightVect = shadowVector;
					shaderInputData.point = point;
					shaderInputData.viewVector = viewVector;
					shaderInputData.delta = CalcDelta(point);
					CVector3 vn = CalculateNormals(shaderInputData);
					viewVector = viewVector - vn * viewVector.Dot(vn) * 2.0;
					startRay = startRay + viewVector * reflectBuff[ray].distThresh;
				}

				for (int ray = rayEnd; ray >= 0; ray--)
				{

					sRGBAfloat objectShader;
					objectShader.A = 0.0;
					sRGBAfloat backgroundShader;
					sRGBAfloat volumetricShader;
					sRGBAfloat specular;

					CVector3 lightVector = shadowVector;

					sShaderInputData shaderInputData;
					shaderInputData.distThresh = reflectBuff[ray].distThresh;
					shaderInputData.delta = CalcDelta(reflectBuff[ray].point);
					shaderInputData.lightVect = lightVector;
					shaderInputData.point = reflectBuff[ray].point;
					shaderInputData.viewVector = reflectBuff[ray].viewVector;
					shaderInputData.lastDist = reflectBuff[ray].lastDist;
					shaderInputData.depth = reflectBuff[ray].depth;
					shaderInputData.stepCount = reflectBuff[ray].buffCount;
					shaderInputData.stepBuff = reflectBuff[ray].stepBuff;
					shaderInputData.objectType = reflectBuff[ray].objectType;
					shaderInputData.objectColor = reflectBuff[ray].objectColor;

					//if fractal surface was found
					if (reflectBuff[ray].found)
					{
						objectShader = ObjectShader(shaderInputData, &objectColour, &specular);

					} //end if found
					else
					{
						backgroundShader = BackgroundShader(shaderInputData);
						reflectBuff[ray].depth = 1e20;
					}

					double reflect = reflectBuff[ray].reflect;

					sRGBAfloat pixel;

					if (reflectionsMax > 0 && rayEnd > 0 && ray != rayEnd)
					{

						pixel.R = resultShader.R * reflect + (1.0 - reflect) * (objectShader.R + backgroundShader.R) + specular.R;
						pixel.G = resultShader.G * reflect + (1.0 - reflect) * (objectShader.G + backgroundShader.G) + specular.G;
						pixel.B = resultShader.B * reflect + (1.0 - reflect) * (objectShader.B + backgroundShader.B) + specular.B;

					}
					else
					{
						pixel.R = objectShader.R + backgroundShader.R + specular.R;
						pixel.G = objectShader.G + backgroundShader.G + specular.G;
						pixel.B = objectShader.B + backgroundShader.B + specular.B;
					}

					pixel.A = objectShader.A + backgroundShader.A;

					sRGBAfloat opacityOut;

					volumetricShader = VolumetricShader(shaderInputData, pixel, &opacityOut);

					resultShader.R = volumetricShader.R;
					resultShader.G = volumetricShader.G;
					resultShader.B = volumetricShader.B;

					if (ray == 0)
					{
						for (int xx = 0; xx < scheduler->GetProgresiveStep(); ++xx)
						{
							for (int yy = 0; yy < scheduler->GetProgresiveStep(); ++yy)
							{
								image->PutPixelOpacity(screenPoint.x + xx, screenPoint.y + yy, (unsigned short) (opacityOut.R * 65535.0));
								image->PutPixelAlpha(screenPoint.x + xx, screenPoint.y + yy, (unsigned short) (volumetricShader.A * 65535.0));
							}
						}

					}
				}
			}

			sRGBfloat pixel2;
			pixel2.R = resultShader.R;
			pixel2.G = resultShader.G;
			pixel2.B = resultShader.B;

			sRGB8 colour;
			colour.R = objectColour.R * 255;
			colour.G = objectColour.G * 255;
			colour.B = objectColour.B * 255;

			for (int xx = 0; xx < scheduler->GetProgresiveStep(); ++xx)
			{
				for (int yy = 0; yy < scheduler->GetProgresiveStep(); ++yy)
				{
					image->PutPixelImage(screenPoint.x + xx, screenPoint.y + yy, pixel2);
					image->PutPixelColour(screenPoint.x + xx, screenPoint.y + yy, colour);
					image->PutPixelZBuffer(screenPoint.x + xx, screenPoint.y + yy, (float) reflectBuff[0].depth);
				}
			}

			pixelCounter++;
		}
	}

	//emit signal to main thread when finished
	emit finished();
	return;
}

//calculation of base vectors
void cRenderWorker::PrepareMainVectors(void)
{
	cameraTarget = new cCameraTarget(params->camera, params->target, params->topVector);
	//cameraTarget->SetCameraTargetRotation(params->camera, params->target, params->viewAngle);
	viewAngle = cameraTarget->GetRotation();

	//preparing rotation matrix
	mRot.RotateZ(viewAngle.x); //yaw
	mRot.RotateX(viewAngle.y); //pitch
	mRot.RotateY(viewAngle.z); //roll

	//preparing base vectors
	CVector3 vector;
	baseX = mRot.RotateVector(baseX);
	baseY = mRot.RotateVector(baseY);
	baseZ = mRot.RotateVector(baseZ);

	//main shadow direction vector
	double alpha = params->mainLightAlpha / 180.0 * M_PI;
	double beta = params->mainLightBeta / 180.0 * M_PI;
	vector.x = cos(alpha - 0.5 * M_PI) * cos(beta);
	vector.y = sin(alpha - 0.5 * M_PI) * cos(beta);
	vector.z = sin(beta);

	if (params->mainLightPositionAsRelative)
	{
		shadowVector = mRot.RotateVector(vector);
	}
	else
	{
		shadowVector = vector;
	}
}

//reflection data
void cRenderWorker::PrepareReflectionBuffer(void)
{

	reflectionsMax = params->reflectionsMax;
	if (!params->raytracedReflections) reflectionsMax = 0;
	reflectBuff = new sReflect[reflectionsMax + 2];

	for (int i = 0; i < reflectionsMax + 1; i++)
	{
		//rayMarching buffers
		reflectBuff[i].stepBuff = new sStep[maxraymarchingSteps + 2];
		reflectBuff[i].buffCount = 0;
	}
}

//calculating vectors for AmbientOcclusion
void cRenderWorker::PrepareAOVectors(void)
{
	AOvectorsAround = new sVectorsAround[10000];
	AOvectorsCount = 0;
	int counter = 0;
	int lightMapWidth = data->textures.lightmapTexture->Width();
	int lightMapHeight = data->textures.lightmapTexture->Height();
	for (double b = -0.49 * M_PI; b < 0.49 * M_PI; b += 1.0 / params->ambientOcclusionQuality)
	{
		for (double a = 0.0; a < 2.0 * M_PI; a += ((2.0 / params->ambientOcclusionQuality) / cos(b)))
		{
			CVector3 d;
			d.x = cos(a + b) * cos(b);
			d.y = sin(a + b) * cos(b);
			d.z = sin(b);
			AOvectorsAround[counter].alpha = a;
			AOvectorsAround[counter].beta = b;
			AOvectorsAround[counter].v = d;
			int X = (int) ((a + b) / (2.0 * M_PI) * lightMapWidth + lightMapWidth * 8.5) % lightMapWidth;
			int Y = (int) (b / (M_PI) * lightMapHeight + lightMapHeight * 8.5) % lightMapHeight;
			AOvectorsAround[counter].R = data->textures.lightmapTexture->FastPixel(X, Y).R;
			AOvectorsAround[counter].G = data->textures.lightmapTexture->FastPixel(X, Y).G;
			AOvectorsAround[counter].B = data->textures.lightmapTexture->FastPixel(X, Y).B;
			if (AOvectorsAround[counter].R > 10 || AOvectorsAround[counter].G > 10 || AOvectorsAround[counter].B > 10)
			{
				counter++;
			}
			if (counter >= 10000) break;
		}
		if (counter >= 10000) break;
	}
	if (counter == 0)
	{
		counter = 1;
		AOvectorsAround[0].alpha = 0;
		AOvectorsAround[0].beta = 0;
		AOvectorsAround[0].v.x = 0;
		AOvectorsAround[0].v.y = 0;
		AOvectorsAround[0].v.z = 0;
		AOvectorsAround[0].R = 0;
		AOvectorsAround[0].G = 0;
		AOvectorsAround[0].B = 0;
	}
	AOvectorsCount = counter;
}

//calculation of distance where ray-marching stops
double cRenderWorker::CalcDistThresh(CVector3 point)
{
	double distThresh;
	if (params->constantDEThreshold)
		distThresh = params->DEThresh;
	else
		distThresh = (params->camera - point).Length() * params->resolution * params->fov / params->detailLevel;
	if(params->perspectiveType == params::perspEquirectangular || params->perspectiveType == params::perspFishEye || params->perspectiveType == params::perspFishEyeCut) distThresh *= M_PI;
	distThresh /= data->reduceDetail;
	return distThresh;
}

//calculation of "voxel" size
double cRenderWorker::CalcDelta(CVector3 point)
{
	double delta;
	delta = (params->camera - point).Length() * params->resolution * params->fov;
	if(params->perspectiveType == params::perspEquirectangular || params->perspectiveType == params::perspFishEye || params->perspectiveType == params::perspFishEyeCut) delta *= M_PI;
	return delta;
}

//Ray-Marching
CVector3 cRenderWorker::RayMarching(sRayMarchingIn &in, sRayMarchingInOut *inOut, sRayMarchingOut *out)
{
	CVector3 point;
	bool found = false;
	double scan = in.minScan;
	double dist = 0;
	double search_accuracy = 0.01 * params->detailLevel;
	double search_limit = 1.0 - search_accuracy;
	int counter = 0;
	double step = 0.0;
	(*inOut->buffCount) = 0;
	double distThresh;

	//qDebug() << "Start ************************";
	for (int i = 0; i < 10000; i++)
	{
		counter++;

		point = in.start + in.direction * scan;

		distThresh = CalcDistThresh(point);


		sDistanceIn distanceIn(point, distThresh, false);
		sDistanceOut distanceOut;
		dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut);
		out->object = distanceOut.object;
		if(out->object == fractal::objFractal)
		{
			out->objectReflect = params->reflect;
			out->objectColor = sRGB(0,0,0);
		}
		else
		{
			out->objectReflect = distanceOut.objectReflect;
			out->objectColor = distanceOut.objectColor;
		}

		//-------------------- 4.18us for Calculate distance --------------

		//printf("Distance = %g\n", dist/distThresh);
		inOut->stepBuff[i].distance = dist;
		inOut->stepBuff[i].iters = distanceOut.iters;
		inOut->stepBuff[i].distThresh = distThresh;

		if(distanceOut.iters < 256)
			histogramIters[distanceOut.iters]++;
		else
			histogramIters[255]++;

		if (dist > 3.0) dist = 3.0;
		if (dist < distThresh)
		{
			if (dist < 0.1 * distThresh) missed_DE_counter++;
			found = true;
			break;
		}

		inOut->stepBuff[i].step = step;
		if(params->interiorMode)
		{
			step = (dist - 0.8 * distThresh) * params->DEFactor * (1.0 - Random(1000)/10000.0);;
		}
		else
		{
			step = (dist - 0.5 * distThresh) * params->DEFactor * (1.0 - Random(1000)/10000.0);;
		}
		inOut->stepBuff[i].point = point;
		//qDebug() << "i" << i << "dist" << inOut->stepBuff[i].distance << "iters" << inOut->stepBuff[i].iters << "distThresh" << inOut->stepBuff[i].distThresh << "step" << inOut->stepBuff[i].step << "point" << inOut->stepBuff[i].point.Debug();
		(*inOut->buffCount) = i + 1;
		scan += step / in.direction.Length(); //divided by length of view Vector to elimitate overstepping when fov is big
		if (scan > in.maxScan)
		{
			break;
		}
	}
	//------------- 83.2473 us for RayMarching loop -------------------------

	if (found && in.binaryEnable)
	{
		step *= 0.5;
		for (int i = 0; i < 20; i++)
		{
			counter++;
			if (dist < distThresh && dist > distThresh * search_limit)
			{
				break;
			}
			else
			{
				if (dist > distThresh)
				{
					scan += step;
					point = in.start + in.direction * scan;
				}
				else if (dist < distThresh * search_limit)
				{
					scan -= step;
					point = in.start + in.direction * scan;
				}
			}

			distThresh = CalcDistThresh(point);

			sDistanceIn distanceIn(point, distThresh, false);
			sDistanceOut distanceOut;
			dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut);
			out->object = distanceOut.object;

			if(out->object == fractal::objFractal)
			{
				out->objectReflect = params->reflect;
				out->objectColor = sRGB(0,0,0);
			}
			else
			{
				out->objectReflect = distanceOut.objectReflect;
				out->objectColor = distanceOut.objectColor;
			}

			if(distanceOut.iters < 256)
				histogramIters[distanceOut.iters]++;
			else
				histogramIters[255]++;

			step *= 0.5;
		}
	}
	//---------- 7.19605us for binary searching ---------------

	DECounter+= counter;
	//counters for drawing histogram
	int counter2 = counter / 4;
	if (counter2 < 256) histogramDE[counter2]++;
	else histogramDE[255]++;

	out->found = found;
	out->lastDist = dist;
	out->depth = scan;
	out->distThresh = distThresh;
	return point;
}


