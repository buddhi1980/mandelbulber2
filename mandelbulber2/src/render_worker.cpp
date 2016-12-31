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
 * cRenderWorker class - worker for rendering image on single CPU core
 */

#include "render_worker.hpp"
#include "calculate_distance.hpp"
#include "common_math.h"
#include "region.hpp"
#include <QtCore>

#include "ao_modes.h"
#include "camera_target.hpp"
#include "cimage.hpp"
#include "material.h"
#include "projection_3d.hpp"
#include "render_data.hpp"
#include "stereo.h"
#include "fractparams.hpp"
#include "scheduler.hpp"
#include "system.hpp"

cRenderWorker::cRenderWorker(const cParamRender *_params, const cNineFractals *_fractal,
	sThreadData *_threadData, sRenderData *_data, cImage *_image)
{
	params = _params;
	fractal = _fractal;
	data = _data;
	image = _image;
	threadData = _threadData;
	cameraTarget = NULL;
	rayBuffer = NULL;
	AOvectorsAround = NULL;
	AOvectorsCount = 0;
	baseX = CVector3(1.0, 0.0, 0.0);
	baseY = CVector3(0.0, 1.0, 0.0);
	baseZ = CVector3(0.0, 0.0, 1.0);
	maxraymarchingSteps = 10000;
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

	if (rayBuffer)
	{
		for (int i = 0; i < reflectionsMax + 3; i++)
		{
			delete[] rayBuffer[i].stepBuff;
		}
		delete[] rayBuffer;
	}

	if (AOvectorsAround)
	{
		delete[] AOvectorsAround;
		AOvectorsAround = NULL;
	}
}

// main render engine function called as multiple threads
void cRenderWorker::doWork(void)
{
	// here will be rendering thread
	int width = image->GetWidth();
	int height = image->GetHeight();
	double aspectRatio = (double)width / height;

	if (params->perspectiveType == params::perspEquirectangular) aspectRatio = 2.0;

	bool monteCarloDOF = params->DOFMonteCarlo && params->DOFEnabled;

	if (data->stereo.isEnabled() && (params->perspectiveType != params::perspEquirectangular))
		aspectRatio = data->stereo.ModifyAspectRatio(aspectRatio);

	PrepareMainVectors();
	PrepareReflectionBuffer();
	if (params->ambientOcclusionEnabled && params->ambientOcclusionMode == params::AOmodeMultipeRays)
		PrepareAOVectors();

	// init of scheduler
	cScheduler *scheduler = threadData->scheduler;

	// start point for ray-marching
	CVector3 start = params->camera;

	scheduler->InitFirstLine(threadData->id, threadData->startLine);

	bool lastLineWasBroken = false;

	// main loop for y
	for (int ys = threadData->startLine; scheduler->ThereIsStillSomethingToDo(threadData->id);
			 ys = scheduler->NextLine(threadData->id, ys, lastLineWasBroken))
	{
		// skip if line is out of region
		if (ys < 0) break;
		if (ys < data->screenRegion.y1 || ys > data->screenRegion.y2) continue;

		// main loop for x
		for (int xs = 0; xs < width; xs += scheduler->GetProgressiveStep())
		{
			if (systemData.globalStopRequest) break;
			// break if by coincidence this thread started rendering the same line as some other
			lastLineWasBroken = false;
			if (scheduler->ShouldIBreak(threadData->id, ys))
			{
				lastLineWasBroken = true;
				break;
			}

			if (scheduler->GetProgressivePass() > 1 && xs % (scheduler->GetProgressiveStep() * 2) == 0
					&& ys % (scheduler->GetProgressiveStep() * 2) == 0)
				continue;

			// skip if pixel is out of region;
			if (xs < data->screenRegion.x1 || xs > data->screenRegion.x2) continue;

			// calculate point in image coordinate system
			CVector2<int> screenPoint(xs, ys);
			CVector2<double> imagePoint = data->screenRegion.transpose(data->imageRegion, screenPoint);
			cStereo::enumEye stereoEye = data->stereo.WhichEye(imagePoint);
			if (data->stereo.isEnabled())
			{
				imagePoint = data->stereo.ModifyImagePoint(imagePoint);
			}
			imagePoint.x *= aspectRatio;

			// full dome shemisphere cut
			bool hemisphereCut = false;
			if (params->perspectiveType == params::perspFishEyeCut
					&& imagePoint.Length() > 0.5 / params->fov)
				hemisphereCut = true;

			//---------------- 1us -------------

			// Ray marching
			int repeats = data->stereo.GetNumberOfRepeats();

			sRGBfloat finallPixel;
			sRGBfloat pixelLeftEye;
			sRGBfloat pixelRightEye;
			sRGB8 colour;
			unsigned short alpha = 65535;
			unsigned short opacity16 = 65535;
			sRGBfloat normalFloat;
			double depth = 1e20;

			if (monteCarloDOF) repeats = params->DOFSamples;

			sRGBfloat finalPixelDOF;

			for (int repeat = 0; repeat < repeats; repeat++)
			{

				CVector3 viewVector;
				CVector3 startRay;

				if (monteCarloDOF)
				{
					MonteCarloDOF(imagePoint, &startRay, &viewVector);
				}
				else
				{
					// calculate direction of ray-marching
					viewVector = CalculateViewVector(imagePoint, params->fov, params->perspectiveType, mRot);
					startRay = start;
				}

				if (data->stereo.isEnabled())
				{
					data->stereo.WhichEyeForAnaglyph(&stereoEye, repeat);
					if (params->perspectiveType == params::perspFishEyeCut)
					{
						CVector3 eyePosition;
						CVector3 sideVector = viewVector.Cross(params->topVector);
						sideVector.Normalize();
						double eyeDistance = params->stereoEyeDistance;
						if (data->stereo.AreSwapped()) eyeDistance *= -1.0;

						if (stereoEye == cStereo::eyeLeft)
						{
							eyePosition =
								startRay
								+ 0.5 * (cameraTarget->GetRightVector() * eyeDistance + sideVector * eyeDistance);
						}
						else
						{
							eyePosition =
								startRay
								- 0.5 * (cameraTarget->GetRightVector() * eyeDistance + sideVector * eyeDistance);
						}
						startRay = eyePosition;
					}
					else
					{
						startRay = data->stereo.CalcEyePosition(
							startRay, viewVector, params->topVector, params->stereoEyeDistance, stereoEye);
						data->stereo.ViewVectorCorrection(params->stereoInfiniteCorrection, mRot, mRotInv,
							stereoEye, params->perspectiveType, &viewVector);
					}
				}

				sRGBAfloat resultShader;
				sRGBAfloat objectColour;
				CVector3 normal;

				double opacity = 1.0;
				depth = 1e20;

				// raymarching loop (reflections)

				if (!hemisphereCut) // in fulldome mode, will not render pixels out of the fulldome
				{
					sRayRecursionIn recursionIn;

					sRayMarchingIn rayMarchingIn;
					CVector3 direction = viewVector;
					direction.Normalize();
					rayMarchingIn.binaryEnable = true;
					rayMarchingIn.direction = direction;
					rayMarchingIn.maxScan = params->viewDistanceMax;
					rayMarchingIn.minScan = params->viewDistanceMin;
					rayMarchingIn.start = startRay;
					rayMarchingIn.invertMode = false;
					recursionIn.rayMarchingIn = rayMarchingIn;
					recursionIn.calcInside = false;
					recursionIn.resultShader = resultShader;
					recursionIn.objectColour = objectColour;

					sRayRecursionInOut recursionInOut;
					sRayMarchingInOut rayMarchingInOut;
					rayMarchingInOut.buffCount = &rayBuffer[0].buffCount;
					rayMarchingInOut.stepBuff = rayBuffer[0].stepBuff;
					recursionInOut.rayMarchingInOut = rayMarchingInOut;
					recursionInOut.rayIndex = 0;

					sRayRecursionOut recursionOut = RayRecursion(recursionIn, recursionInOut);

					resultShader = recursionOut.resultShader;
					objectColour = recursionOut.objectColour;
					depth = recursionOut.rayMarchingOut.depth;
					if (!recursionOut.found) depth = 1e20;
					opacity = recursionOut.fogOpacity;
					normal = recursionOut.normal;
				}

				finallPixel.R = resultShader.R;
				finallPixel.G = resultShader.G;
				finallPixel.B = resultShader.B;

				if (data->stereo.isEnabled() && data->stereo.GetMode() == cStereo::stereoRedCyan)
				{
					if (stereoEye == cStereo::eyeLeft)
					{
						pixelLeftEye.R += finallPixel.R;
						pixelLeftEye.G += finallPixel.G;
						pixelLeftEye.B += finallPixel.B;
					}
					else if (stereoEye == cStereo::eyeRight)
					{
						pixelRightEye.R += finallPixel.R;
						pixelRightEye.G += finallPixel.G;
						pixelRightEye.B += finallPixel.B;
					}
				}

				alpha = resultShader.A * 65535;
				opacity16 = opacity * 65535;

				colour.R = objectColour.R * 255;
				colour.G = objectColour.G * 255;
				colour.B = objectColour.B * 255;

				if (image->GetImageOptional()->optionalNormal)
				{
					CVector3 normalRotated = mRotInv.RotateVector(normal);
					normalFloat.R = (1.0 + normalRotated.x) / 2.0;
					normalFloat.G = (1.0 + normalRotated.z) / 2.0;
					normalFloat.B = 1.0 - normalRotated.y;
				}

				finalPixelDOF.R += finallPixel.R;
				finalPixelDOF.G += finallPixel.G;
				finalPixelDOF.B += finallPixel.B;

			} // next repeat

			if (monteCarloDOF)
			{
				if (data->stereo.isEnabled() && data->stereo.GetMode() == cStereo::stereoRedCyan)
				{
					finallPixel = data->stereo.MixColorsRedCyan(pixelLeftEye, pixelRightEye);
					finallPixel.R = finallPixel.R / repeats * 2.0;
					finallPixel.G = finallPixel.G / repeats * 2.0;
					finallPixel.B = finallPixel.B / repeats * 2.0;
				}
				else
				{
					finallPixel.R = finalPixelDOF.R / repeats;
					finallPixel.G = finalPixelDOF.G / repeats;
					finallPixel.B = finalPixelDOF.B / repeats;
				}
			}
			else if (data->stereo.isEnabled() && data->stereo.GetMode() == cStereo::stereoRedCyan)
			{
				finallPixel = data->stereo.MixColorsRedCyan(pixelLeftEye, pixelRightEye);
			}

			for (int yy = 0; yy < scheduler->GetProgressiveStep(); ++yy)
			{
				int yyy = screenPoint.y + yy;
				if (yyy < data->screenRegion.y2)
				{
					for (int xx = 0; xx < scheduler->GetProgressiveStep(); ++xx)
					{
						int xxx = screenPoint.x + xx;
						if (xxx < data->screenRegion.x2)
						{
							image->PutPixelImage(xxx, yyy, finallPixel);
							image->PutPixelColour(xxx, yyy, colour);
							image->PutPixelAlpha(xxx, yyy, alpha);
							image->PutPixelZBuffer(xxx, yyy, (float)depth);
							image->PutPixelOpacity(xxx, yyy, opacity16);
							if (image->GetImageOptional()->optionalNormal)
								image->PutPixelNormal(xxx, yyy, normalFloat);
						}
					}
				}
			}

			data->statistics.numberOfRenderedPixels++;

		} // next xs
	}		// next ys

	// emit signal to main thread when finished
	emit finished();
	return;
}

// calculation of base vectors
void cRenderWorker::PrepareMainVectors(void)
{
	cameraTarget = new cCameraTarget(params->camera, params->target, params->topVector);
	// cameraTarget->SetCameraTargetRotation(params->camera, params->target, params->viewAngle);
	viewAngle = cameraTarget->GetRotation();

	// preparing rotation matrix
	mRot.RotateZ(viewAngle.x); // yaw
	mRot.RotateX(viewAngle.y); // pitch
	mRot.RotateY(viewAngle.z); // roll

	// preparing base vectors
	CVector3 vector;
	baseX = mRot.RotateVector(baseX);
	baseY = mRot.RotateVector(baseY);
	baseZ = mRot.RotateVector(baseZ);

	// main shadow direction vector
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

	mRot.RotateZ(-params->sweetSpotHAngle);
	mRot.RotateX(params->sweetSpotVAngle);

	mRotInv = mRot.Transpose();
}

// reflection data
void cRenderWorker::PrepareReflectionBuffer(void)
{

	reflectionsMax = params->reflectionsMax * 2;
	if (!params->raytracedReflections) reflectionsMax = 0;
	rayBuffer = new sRayBuffer[reflectionsMax + 4];

	for (int i = 0; i < reflectionsMax + 3; i++)
	{
		// rayMarching buffers
		rayBuffer[i].stepBuff = new sStep[maxraymarchingSteps + 2];
		rayBuffer[i].buffCount = 0;
	}
}

// calculating vectors for AmbientOcclusion
void cRenderWorker::PrepareAOVectors(void)
{
	AOvectorsAround = new sVectorsAround[10000];
	AOvectorsCount = 0;
	int counter = 0;
	int lightMapWidth = data->textures.lightmapTexture.Width();
	int lightMapHeight = data->textures.lightmapTexture.Height();
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
			int X = (int)((a + b) / (2.0 * M_PI) * lightMapWidth + lightMapWidth * 8.5) % lightMapWidth;
			int Y = (int)(b / (M_PI)*lightMapHeight + lightMapHeight * 8.5) % lightMapHeight;
			AOvectorsAround[counter].R = data->textures.lightmapTexture.FastPixel(X, Y).R;
			AOvectorsAround[counter].G = data->textures.lightmapTexture.FastPixel(X, Y).G;
			AOvectorsAround[counter].B = data->textures.lightmapTexture.FastPixel(X, Y).B;
			if (AOvectorsAround[counter].R > 10 || AOvectorsAround[counter].G > 10
					|| AOvectorsAround[counter].B > 10)
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

// calculation of distance where ray-marching stops
double cRenderWorker::CalcDistThresh(CVector3 point) const
{
	double distThresh;
	if (params->constantDEThreshold)
		distThresh = params->DEThresh;
	else
		distThresh =
			(params->camera - point).Length() * params->resolution * params->fov / params->detailLevel;
	if (params->perspectiveType == params::perspEquirectangular
			|| params->perspectiveType == params::perspFishEye
			|| params->perspectiveType == params::perspFishEyeCut)
		distThresh *= M_PI;
	distThresh /= data->reduceDetail;
	return distThresh;
}

// calculation of "voxel" size
double cRenderWorker::CalcDelta(CVector3 point) const
{
	double delta;
	delta = (params->camera - point).Length() * params->resolution * params->fov;
	if (params->perspectiveType == params::perspEquirectangular
			|| params->perspectiveType == params::perspFishEye
			|| params->perspectiveType == params::perspFishEyeCut)
		delta *= M_PI;
	return delta;
}

// Ray-Marching
CVector3 cRenderWorker::RayMarching(
	sRayMarchingIn &in, sRayMarchingInOut *inOut, sRayMarchingOut *out)
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
	double distThresh = 0;
	out->objectId = 0;

	// qDebug() << "Start ************************";

	CVector3 lastPoint, lastGoodPoint;
	bool deadComputationFound = false;

	for (int i = 0; i < 10000; i++)
	{
		lastGoodPoint = lastPoint;
		lastPoint = point;

		counter++;

		point = in.start + in.direction * scan;

		if (point == lastPoint || point.IsNotANumber()) // detection of dead calculation
		{
			// qWarning() << "Dead computation\n"
			//		<< "Point:" << point.Debug()
			//		<< "\nPrevious point:" << lastPoint.Debug();
			point = lastPoint;
			found = true;
			deadComputationFound = true;
			break;
		}

		distThresh = CalcDistThresh(point);

		sDistanceIn distanceIn(point, distThresh, false);
		sDistanceOut distanceOut;
		dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);
		// qDebug() <<"thresh" <<  distThresh << "dist" << dist << "scan" << scan;
		if (in.invertMode)
		{
			dist = distThresh * 1.99 - dist;
			if (dist < 0.0) dist = 0.0;
		}
		out->objectId = distanceOut.objectId;

		//-------------------- 4.18us for Calculate distance --------------

		// printf("Distance = %g\n", dist/distThresh);
		inOut->stepBuff[i].distance = dist;
		inOut->stepBuff[i].iters = distanceOut.iters;
		inOut->stepBuff[i].distThresh = distThresh;

		data->statistics.histogramIterations.Add(distanceOut.iters);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		if (dist > 3.0) dist = 3.0;
		if (dist < distThresh)
		{
			if (dist < 0.1 * distThresh) data->statistics.missedDE++;
			found = true;
			break;
		}

		inOut->stepBuff[i].step = step;
		if (params->interiorMode)
		{
			step = (dist - 0.8 * distThresh) * params->DEFactor * (1.0 - Random(1000) / 10000.0);
			;
		}
		else
		{
			step = (dist - 0.5 * distThresh) * params->DEFactor * (1.0 - Random(1000) / 10000.0);
			;
		}
		inOut->stepBuff[i].point = point;
		// qDebug() << "i" << i << "dist" << inOut->stepBuff[i].distance << "iters" <<
		// inOut->stepBuff[i].iters << "distThresh" << inOut->stepBuff[i].distThresh << "step" <<
		// inOut->stepBuff[i].step << "point" << inOut->stepBuff[i].point.Debug();
		(*inOut->buffCount) = i + 1;
		// divided by length of view Vector to eliminate overstepping when fov is big
		scan += step / in.direction.Length();
		if (scan > in.maxScan)
		{
			break;
		}
	}
	//------------- 83.2473 us for RayMarching loop -------------------------

	// qDebug() << "------------ binary search";
	if (found && in.binaryEnable && !deadComputationFound)
	{
		step *= 0.5;
		for (int i = 0; i < 30; i++)
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
			dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);

			// qDebug() << "i" << i <<"thresh" <<  distThresh << "dist" << dist << "scan" << scan <<
			// "step" << step;

			if (in.invertMode)
			{
				dist = distThresh * 1.99 - dist;
				if (dist < 0.0) dist = 0.0;
			}

			out->objectId = distanceOut.objectId;

			data->statistics.histogramIterations.Add(distanceOut.iters);
			data->statistics.totalNumberOfIterations += distanceOut.totalIters;

			step *= 0.5;
		}
	}
	if (params->common.iterThreshMode)
	{
		// this fixes problem with noise when there is used "stop at maxIter" mode
		scan -= distThresh;
		point = in.start + in.direction * scan;
	}

	//---------- 7.19605us for binary searching ---------------

	data->statistics.histogramStepCount.Add(counter);

	out->found = found;
	out->lastDist = dist;
	out->depth = scan;
	out->distThresh = distThresh;
	data->statistics.numberOfRaymarchings++;
	return point;
}

cRenderWorker::sRayRecursionOut cRenderWorker::RayRecursion(
	sRayRecursionIn in, sRayRecursionInOut &inOut)
{
	sRayMarchingOut rayMarchingOut;

	*inOut.rayMarchingInOut.buffCount = 0;

	// trace the light in given direction
	CVector3 point = RayMarching(in.rayMarchingIn, &inOut.rayMarchingInOut, &rayMarchingOut);

	sRGBAfloat resultShader = in.resultShader;
	sRGBAfloat objectColour = in.objectColour;

	// here will be called branch for RayRecursion();

	sRGBAfloat objectShader;
	objectShader.A = 0.0;
	sRGBAfloat backgroundShader;
	sRGBAfloat volumetricShader;
	sRGBAfloat specular;

	// prepare data for shaders
	CVector3 lightVector = shadowVector;
	sShaderInputData shaderInputData;
	shaderInputData.distThresh = rayMarchingOut.distThresh;
	shaderInputData.delta = CalcDelta(point);
	shaderInputData.lightVect = lightVector;
	shaderInputData.point = point;
	shaderInputData.viewVector = in.rayMarchingIn.direction;
	shaderInputData.lastDist = rayMarchingOut.lastDist;
	shaderInputData.depth = rayMarchingOut.depth;
	shaderInputData.stepCount = *inOut.rayMarchingInOut.buffCount;
	shaderInputData.stepBuff = inOut.rayMarchingInOut.stepBuff;
	shaderInputData.invertMode = in.calcInside;
	shaderInputData.objectId = rayMarchingOut.objectId;

	cObjectData objectData = data->objectData[shaderInputData.objectId];
	shaderInputData.material = &data->materials[objectData.materialId];

	sRGBAfloat reflectShader = in.resultShader;
	double reflect = shaderInputData.material->reflectance;

	sRGBAfloat transparentShader = in.resultShader;
	double transparent = shaderInputData.material->transparencyOfSurface;
	sRGBfloat transparentColor =
		sRGBfloat(shaderInputData.material->transparencyInteriorColor.R / 65536.0,
			shaderInputData.material->transparencyInteriorColor.G / 65536.0,
			shaderInputData.material->transparencyInteriorColor.B / 65536.0);
	resultShader.R = transparentColor.R;
	resultShader.G = transparentColor.G;
	resultShader.B = transparentColor.B;

	CVector3 vn;

	// if found any object
	if (rayMarchingOut.found)
	{
		// calculate normal vector
		vn = CalculateNormals(shaderInputData);
		shaderInputData.normal = vn;

		// letting colors from textures (before normal map shader)
		if (shaderInputData.material->colorTexture.IsLoaded())
			shaderInputData.texColor =
				TextureShader(shaderInputData, texture::texColor, shaderInputData.material);
		else
			shaderInputData.texColor = sRGBfloat(1.0, 1.0, 1.0);

		if (shaderInputData.material->luminosityTexture.IsLoaded())
			shaderInputData.texLuminosity =
				TextureShader(shaderInputData, texture::texLuminosity, shaderInputData.material);
		else
			shaderInputData.texLuminosity = sRGBfloat(0.0, 0.0, 0.0);

		if (shaderInputData.material->diffusionTexture.IsLoaded())
			shaderInputData.texDiffuse =
				TextureShader(shaderInputData, texture::texDiffuse, shaderInputData.material);
		else
			shaderInputData.texDiffuse = sRGBfloat(1.0, 1.0, 1.0);

		if (shaderInputData.material->normalMapTexture.IsLoaded())
		{
			vn = NormalMapShader(shaderInputData);
		}

		// prepare refraction values
		double n1, n2;
		if (in.calcInside) // if trace is inside the object
		{
			n1 = shaderInputData.material->transparencyIndexOfRefraction; // reverse refractive indices
			n2 = 1.0;
		}
		else
		{
			n1 = 1.0;
			n2 = shaderInputData.material->transparencyIndexOfRefraction;
			;
		}

		if (inOut.rayIndex < reflectionsMax)
		{

			// calculate refraction (transparency)
			if (transparent > 0.0)
			{
				sRayRecursionIn recursionIn;
				sRayMarchingIn rayMarchingIn;
				sRayMarchingInOut rayMarchingInOut;

				// calculate direction of refracted light
				CVector3 newDirection = RefractVector(vn, in.rayMarchingIn.direction, n1, n2);

				// move starting point a little
				CVector3 newPoint = point + in.rayMarchingIn.direction * shaderInputData.distThresh * 1.0;

				// if is total internal reflection the use reflection instead of refraction
				bool internalReflection = false;
				if (newDirection.Length() == 0.0)
				{
					newDirection = ReflectionVector(vn, in.rayMarchingIn.direction);
					newPoint = point + in.rayMarchingIn.direction * shaderInputData.distThresh * 1.0;
					internalReflection = true;
				}

				// preparation for new recursion
				rayMarchingIn.binaryEnable = true;
				rayMarchingIn.direction = newDirection;
				rayMarchingIn.maxScan = params->viewDistanceMax;
				rayMarchingIn.minScan = 0.0;
				rayMarchingIn.start = newPoint;
				rayMarchingIn.invertMode = !in.calcInside || internalReflection;
				recursionIn.rayMarchingIn = rayMarchingIn;
				recursionIn.calcInside = !in.calcInside || internalReflection;
				recursionIn.resultShader = resultShader;
				recursionIn.objectColour = objectColour;

				// setup buffers for ray data
				inOut.rayIndex++; // increase recursion index
				rayMarchingInOut.buffCount = &rayBuffer[inOut.rayIndex].buffCount;
				rayMarchingInOut.stepBuff = rayBuffer[inOut.rayIndex].stepBuff;
				inOut.rayMarchingInOut = rayMarchingInOut;

				// recursion for refraction
				sRayRecursionOut recursionOutTransparent = RayRecursion(recursionIn, inOut);
				transparentShader = recursionOutTransparent.resultShader;
			}

			// calculate reflection
			if (reflect > 0.0)
			{
				sRayRecursionIn recursionIn;
				sRayMarchingIn rayMarchingIn;
				sRayMarchingInOut rayMarchingInOut;

				// calculate new direction of reflection
				CVector3 newDirection = ReflectionVector(vn, in.rayMarchingIn.direction);
				CVector3 newPoint = point + newDirection * shaderInputData.distThresh;

				// prepare for new recursion
				rayMarchingIn.binaryEnable = true;
				rayMarchingIn.direction = newDirection;
				rayMarchingIn.maxScan = params->viewDistanceMax;
				rayMarchingIn.minScan = 0.0;
				rayMarchingIn.start = newPoint;
				rayMarchingIn.invertMode = false;
				recursionIn.rayMarchingIn = rayMarchingIn;
				recursionIn.calcInside = false;
				recursionIn.resultShader = resultShader;
				recursionIn.objectColour = objectColour;

				// setup buffers for ray data
				inOut.rayIndex++; // increase recursion index
				rayMarchingInOut.buffCount = &rayBuffer[inOut.rayIndex].buffCount;
				rayMarchingInOut.stepBuff = rayBuffer[inOut.rayIndex].stepBuff;
				inOut.rayMarchingInOut = rayMarchingInOut;

				// recursion for reflection
				sRayRecursionOut recursionOutReflect = RayRecursion(recursionIn, inOut);
				reflectShader = recursionOutReflect.resultShader;
			}
			if (transparent > 0.0) inOut.rayIndex--; // decrease recursion index
			if (reflect > 0.0) inOut.rayIndex--;		 // decrease recursion index
		}

		shaderInputData.normal = vn;

		// calculate effects for object surface
		objectShader = ObjectShader(shaderInputData, &objectColour, &specular);

		// calculate reflectance according to Fresnel equations
		double reflectance = 1.0;
		double reflectanceN = 1.0;

		if (shaderInputData.material->fresnelReflectance)
		{
			reflectance = Reflectance(vn, in.rayMarchingIn.direction, n1, n2);
			if (reflectance < 0.0) reflectance = 0.0;
			if (reflectance > 1.0) reflectance = 1.0;
			reflectanceN = 1.0 - reflectance;
		}

		// combine all results
		resultShader.R = (objectShader.R + specular.R);
		resultShader.G = (objectShader.G + specular.G);
		resultShader.B = (objectShader.B + specular.B);

		if (reflectionsMax > 0)
		{
			sRGBfloat reflectDiffused;
			double diffIntes = shaderInputData.material->diffussionTextureIntensity;
			double diffIntesN = 1.0 - diffIntes;
			reflectDiffused.R = reflect * shaderInputData.texDiffuse.R * diffIntes + reflect * diffIntesN;
			reflectDiffused.G = reflect * shaderInputData.texDiffuse.G * diffIntes + reflect * diffIntesN;
			reflectDiffused.B = reflect * shaderInputData.texDiffuse.B * diffIntes + reflect * diffIntesN;

			resultShader.R = transparentShader.R * transparent * reflectanceN
											 + (1.0 - transparent * reflectanceN) * resultShader.R;
			resultShader.G = transparentShader.G * transparent * reflectanceN
											 + (1.0 - transparent * reflectanceN) * resultShader.G;
			resultShader.B = transparentShader.B * transparent * reflectanceN
											 + (1.0 - transparent * reflectanceN) * resultShader.B;

			resultShader.R = reflectShader.R * reflectDiffused.R * reflectance
											 + (1.0 - reflectDiffused.R * reflectance) * resultShader.R;
			resultShader.G = reflectShader.G * reflectDiffused.G * reflectance
											 + (1.0 - reflectDiffused.G * reflectance) * resultShader.G;
			resultShader.B = reflectShader.B * reflectDiffused.B * reflectance
											 + (1.0 - reflectDiffused.B * reflectance) * resultShader.B;
		}
		if (resultShader.R < 0.0) resultShader.R = 0.0;
		if (resultShader.G < 0.0) resultShader.G = 0.0;
		if (resultShader.B < 0.0) resultShader.B = 0.0;
	}
	else // if object not found then calculate background
	{
		backgroundShader = BackgroundShader(shaderInputData);
		resultShader = backgroundShader;
		shaderInputData.depth = 1e20;
		vn = mRot.RotateVector(CVector3(0.0, -1.0, 0.0));
	}

	sRGBAfloat opacityOut;

	if (in.calcInside) // if the object interior is traced, then the absorption of light has to be
										 // calculated
	{
		for (int index = shaderInputData.stepCount - 1; index > 0; index--)
		{
			double step = shaderInputData.stepBuff[index].step;

			// CVector3 point = shaderInputData.stepBuff[index].point;
			// shaderInputData.point = point;
			// sRGBAfloat color = SurfaceColour(shaderInputData);
			// transparentColor.R = color.R;
			// transparentColor.G = color.G;
			// transparentColor.B = color.B;

			double opacity = (-1.0 + 1.0 / shaderInputData.material->transparencyOfInterior) * step;
			if (opacity > 1.0) opacity = 1.0;

			resultShader.R = opacity * transparentColor.R + (1.0 - opacity) * resultShader.R;
			resultShader.G = opacity * transparentColor.G + (1.0 - opacity) * resultShader.G;
			resultShader.B = opacity * transparentColor.B + (1.0 - opacity) * resultShader.B;
		}
	}
	else // if now is outside the object, then calculate all volumetric effects like fog, glow...
	{
		volumetricShader = VolumetricShader(shaderInputData, resultShader, &opacityOut);
		resultShader = volumetricShader;
	}

	// prepare final result
	sRayRecursionOut out;
	out.point = point;
	out.rayMarchingOut = rayMarchingOut;
	out.objectColour = objectColour;
	out.resultShader = resultShader;
	out.found = (shaderInputData.depth == 1e20) ? false : true;
	out.fogOpacity = opacityOut.R;
	out.normal = vn;

	return out;
}

void cRenderWorker::MonteCarloDOF(
	CVector2<double> imagePoint, CVector3 *startRay, CVector3 *viewVector)
{
	if (params->perspectiveType == params::perspThreePoint)
	{
		double randR = 0.0015 * params->DOFRadius * params->DOFFocus * sqrt(Random(65536) / 65536.0);
		float randAngle = Random(65536);
		CVector3 randVector(randR * sin(randAngle), 0.0, randR * cos(randAngle));
		CVector3 randVectorRot = mRot.RotateVector(randVector);
		CVector2<double> imagePoint2;
		CVector3 viewVectorTemp =
			CalculateViewVector(imagePoint, params->fov, params->perspectiveType, mRot);
		viewVectorTemp -= randVectorRot / params->DOFFocus;
		*viewVector = viewVectorTemp;
		*startRay = params->camera + randVectorRot;
	}
	else
	{
		CVector3 viewVectorTemp =
			CalculateViewVector(imagePoint, params->fov, params->perspectiveType, mRot);
		double randR = 0.0015 * params->DOFRadius * params->DOFFocus * sqrt(Random(65536) / 65536.0);
		float randAngle = Random(65536);
		CVector3 randVector(randR * sin(randAngle), 0.0, randR * cos(randAngle));

		CVector3 side = viewVectorTemp.Cross(params->topVector);
		side.Normalize();
		CVector3 topTemp = side.Cross(viewVectorTemp);
		topTemp.Normalize();
		CVector3 randVectorRot = side * randVector.x + topTemp * randVector.z;

		CVector2<double> imagePoint2;
		viewVectorTemp -= randVectorRot / params->DOFFocus;
		*viewVector = viewVectorTemp;
		*startRay = params->camera + randVectorRot;
	}
}
