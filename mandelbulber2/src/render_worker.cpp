/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <QtCore>

#include "ao_modes.h"
#include "calculate_distance.hpp"
#include "camera_target.hpp"
#include "cimage.hpp"
#include "common_math.h"
#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "hsv2rgb.h"
#include "material.h"
#include "projection_3d.hpp"
#include "region.hpp"
#include "render_data.hpp"
#include "scheduler.hpp"
#include "stereo.h"
#include "system.hpp"
#include "texture.hpp"

cRenderWorker::cRenderWorker(const sParamRender *_params, const cNineFractals *_fractal,
	sThreadData *_threadData, sRenderData *_data, cImage *_image)
{
	params = _params;
	fractal = _fractal;
	data = _data;
	image = _image;
	threadData = _threadData;
	cameraTarget = nullptr;
	rayBuffer = nullptr;
	rayStack = nullptr;
	AOVectorsAround = nullptr;
	AOVectorsCount = 0;
	baseX = CVector3(1.0, 0.0, 0.0);
	baseY = CVector3(0.0, 1.0, 0.0);
	baseZ = CVector3(0.0, 0.0, 1.0);
	maxRaymarchingSteps = 10000;
	reflectionsMax = 0;
	actualHue = 0.0;
	stopRequest = false;
}

cRenderWorker::~cRenderWorker()
{
	if (cameraTarget)
	{
		delete cameraTarget;
		cameraTarget = nullptr;
	}

	if (rayBuffer)
	{
		for (int i = 0; i < reflectionsMax + 3; i++)
		{
			delete[] rayBuffer[i].stepBuff;
		}
		delete[] rayBuffer;
	}

	if (AOVectorsAround)
	{
		delete[] AOVectorsAround;
		AOVectorsAround = nullptr;
	}

	if (rayStack) delete[] rayStack;
}

// main render engine function called as multiple threads
void cRenderWorker::doWork()
{
	// here will be rendering thread
	int width = image->GetWidth();
	int height = image->GetHeight();
	double aspectRatio = double(width) / height;

	if (params->perspectiveType == params::perspEquirectangular) aspectRatio = 2.0;

	bool monteCarlo = params->DOFMonteCarlo;
	bool antiAliasing = params->antialiasingEnabled;
	int antiAliasingSize = params->antialiasingSize;

	if (data->stereo.isEnabled() && (params->perspectiveType != params::perspEquirectangular))
		aspectRatio = data->stereo.ModifyAspectRatio(aspectRatio);

	PrepareMainVectors();
	PrepareReflectionBuffer();
	if (params->ambientOcclusionEnabled && params->ambientOcclusionMode == params::AOModeMultipleRays)
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

			// full dome hemisphere cut
			bool hemisphereCut = false;
			if (params->perspectiveType == params::perspFishEyeCut
					&& imagePoint.Length() > 0.5 / params->fov)
				hemisphereCut = true;

			// Ray marching
			int repeats = data->stereo.GetNumberOfRepeats();

			sRGBFloat finalPixel;
			sRGBFloat pixelLeftEye;
			sRGBFloat pixelRightEye;
			sRGB8 colour;
			unsigned short alpha = 65535;
			unsigned short opacity16 = 65535;
			sRGBFloat normalFloat;
			sRGBFloat specularFloat;
			double depth = 1e20;
			sRGBFloat worldPositionRGB;
			if (monteCarlo) repeats = params->DOFSamples;
			if (antiAliasing) repeats *= antiAliasingSize * antiAliasingSize;

			sRGBFloat finalPixelDOF;
			unsigned int finalAlphaDOF = 0;
			unsigned int finalOpacityDOF = 0;
			sRGB finalColourDOF;

			sRGBFloat monteCarloDOFStdDevSum;
			double monteCarloNoise = 0.0;

			CVector2<double> originalImagePoint = imagePoint;

			for (int repeat = 0; repeat < repeats; repeat++)
			{

				CVector3 viewVector;
				CVector3 startRay;

				if (antiAliasing)
				{
					int xStep = repeat / antiAliasingSize;
					int yStep = repeat % antiAliasingSize;
					double xOffset = double(xStep) / antiAliasingSize / image->GetWidth() * aspectRatio;
					double yOffset = double(yStep) / antiAliasingSize / image->GetHeight();
					imagePoint.x = originalImagePoint.x + xOffset;
					imagePoint.y = originalImagePoint.y + yOffset;
				}

				if (monteCarlo)
				{
					if (!antiAliasing)
					{
						// MC anti-aliasing
						imagePoint.x =
							originalImagePoint.x
							+ (double(Random(1000)) / 1000.0 - 0.5) / image->GetWidth() * aspectRatio;
						imagePoint.y =
							originalImagePoint.y + (double(Random(1000)) / 1000.0 - 0.5) / image->GetHeight();
					}

					viewVector = CalculateViewVector(imagePoint, params->fov, params->perspectiveType, mRot);
					startRay = start;

					if (params->DOFEnabled)
					{
						MonteCarloDOF(&startRay, &viewVector);
					}
				}
				else
				{
					// calculate direction of ray-marching
					viewVector = CalculateViewVector(imagePoint, params->fov, params->perspectiveType, mRot);
					startRay = start;
				}

				sRGBFloat rgbFromHsv;
				if (params->DOFMonteCarlo && params->DOFMonteCarloChromaticAberration)
				{
					actualHue = Random(3600) / 10.0;
					rgbFromHsv = Hsv2rgb(fmodf(360.0f + float(actualHue) - 60.0f, 360.0f), 1.0f, 2.0f);
					CVector3 randVector(
						0.0, actualHue / 20000.0f * params->DOFMonteCarloCACameraDispersion, 0.0);
					CVector3 randVectorRot = mRot.RotateVector(randVector);
					viewVector -= randVectorRot;
					viewVector.Normalize();
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
						// reduce of stereo effect on poles
						double stereoIntensity = (params->perspectiveType == params::perspEquirectangular)
																			 ? 1.0 - pow(imagePoint.y * 2.0, 10.0)
																			 : 1.0;

						startRay = data->stereo.CalcEyePosition(startRay, viewVector, params->topVector,
							params->stereoEyeDistance * stereoIntensity, stereoEye);
						data->stereo.ViewVectorCorrection(params->stereoInfiniteCorrection * stereoIntensity,
							mRot, mRotInv, stereoEye, params->perspectiveType, &viewVector);
					}
				}

				sRGBAfloat resultShader;
				sRGBAfloat objectColour;
				CVector3 normal;
				;

				double opacity = 1.0;
				depth = 1e20;

				// ray-marching loop (reflections)

				if (!hemisphereCut) // in fulldome mode, will not render pixels out of the fulldome
				{
					sRayRecursionIn recursionIn;

					sRayMarchingIn rayMarchingIn;
					CVector3 direction = viewVector;
					direction.Normalize();
					rayMarchingIn.binaryEnable = true;
					rayMarchingIn.direction = direction;
					rayMarchingIn.maxScan = params->viewDistanceMax;
					rayMarchingIn.minScan = 0; // params->viewDistanceMin;
					rayMarchingIn.start = startRay;
					rayMarchingIn.invertMode = false;
					recursionIn.rayMarchingIn = rayMarchingIn;
					recursionIn.calcInside = false;
					recursionIn.resultShader = resultShader;
					recursionIn.objectColour = objectColour;
					recursionIn.rayBranch = rayBranchReflection;

					sRayRecursionInOut recursionInOut;
					sRayMarchingInOut rayMarchingInOut;
					rayMarchingInOut.buffCount = &rayBuffer[0].buffCount;
					rayMarchingInOut.stepBuff = rayBuffer[0].stepBuff;
					recursionInOut.rayMarchingInOut = rayMarchingInOut;

					sRayRecursionOut recursionOut = RayRecursion(recursionIn, recursionInOut);

					resultShader = recursionOut.resultShader;
					objectColour = recursionOut.objectColour;
					depth = recursionOut.rayMarchingOut.depth;
					if (!recursionOut.found) depth = 1e20;
					opacity = recursionOut.fogOpacity;
					normal = recursionOut.normal;
					worldPositionRGB.R = recursionOut.rayMarchingOut.point.x;
					worldPositionRGB.G = recursionOut.rayMarchingOut.point.y;
					worldPositionRGB.B = recursionOut.rayMarchingOut.point.z;
					specularFloat.R = recursionOut.specular.R;
					specularFloat.G = recursionOut.specular.G;
					specularFloat.B = recursionOut.specular.B;
				}

				finalPixel.R = resultShader.R;
				finalPixel.G = resultShader.G;
				finalPixel.B = resultShader.B;

				if (params->DOFMonteCarlo && params->DOFMonteCarloChromaticAberration)
				{
					finalPixel.R *= rgbFromHsv.R;
					finalPixel.G *= rgbFromHsv.G;
					finalPixel.B *= rgbFromHsv.B;
				}

				if (data->stereo.isEnabled() && data->stereo.GetMode() == cStereo::stereoRedCyan)
				{
					if (stereoEye == cStereo::eyeLeft)
					{
						pixelLeftEye.R += finalPixel.R;
						pixelLeftEye.G += finalPixel.G;
						pixelLeftEye.B += finalPixel.B;
					}
					else if (stereoEye == cStereo::eyeRight)
					{
						pixelRightEye.R += finalPixel.R;
						pixelRightEye.G += finalPixel.G;
						pixelRightEye.B += finalPixel.B;
					}
				}

				alpha = ushort(resultShader.A * 65535);
				opacity16 = ushort(opacity * 65535);

				colour.R = uchar(objectColour.R * 255);
				colour.G = uchar(objectColour.G * 255);
				colour.B = uchar(objectColour.B * 255);

				if (image->GetImageOptional()->optionalNormal)
				{
					CVector3 normalRotated = mRotInv.RotateVector(normal);
					normalRotated.Normalize();
					normalFloat.R = (1.0 + normalRotated.x) / 2.0;
					normalFloat.G = (1.0 + normalRotated.z) / 2.0;
					normalFloat.B = 1.0 - normalRotated.y;
				}

				finalPixelDOF.R += finalPixel.R;
				finalPixelDOF.G += finalPixel.G;
				finalPixelDOF.B += finalPixel.B;
				finalAlphaDOF += alpha;
				finalOpacityDOF += opacity16;
				finalColourDOF.R += colour.R;
				finalColourDOF.G += colour.G;
				finalColourDOF.B += colour.B;

				// noise estimation
				if (monteCarlo)
				{
					monteCarloNoise =
						MonteCarloDOFNoiseEstimation(finalPixel, repeat, finalPixelDOF, monteCarloDOFStdDevSum);

					if (repeat > params->DOFMinSamples && monteCarloNoise < params->DOFMaxNoise * 0.01)
					{
						repeats = repeat + 1;
						break;
					}
				}

			} // next repeat

			if (monteCarlo || antiAliasing)
			{
				if (data->stereo.isEnabled() && data->stereo.GetMode() == cStereo::stereoRedCyan)
				{
					finalPixel = data->stereo.MixColorsRedCyan(pixelLeftEye, pixelRightEye);
					finalPixel.R = finalPixel.R / repeats * 2.0f;
					finalPixel.G = finalPixel.G / repeats * 2.0f;
					finalPixel.B = finalPixel.B / repeats * 2.0f;
				}
				else
				{
					finalPixel.R = finalPixelDOF.R / repeats;
					finalPixel.G = finalPixelDOF.G / repeats;
					finalPixel.B = finalPixelDOF.B / repeats;
					alpha = ushort(finalAlphaDOF / repeats);
					opacity16 = ushort(finalOpacityDOF / repeats);
					colour.R = uchar(finalColourDOF.R / repeats);
					colour.G = uchar(finalColourDOF.G / repeats);
					colour.B = uchar(finalColourDOF.B / repeats);
				}
				data->statistics.totalNumberOfDOFRepeats += repeats;
				data->statistics.totalNoise += monteCarloNoise;
			}
			else if (data->stereo.isEnabled() && data->stereo.GetMode() == cStereo::stereoRedCyan)
			{
				finalPixel = data->stereo.MixColorsRedCyan(pixelLeftEye, pixelRightEye);
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
							image->PutPixelImage(xxx, yyy, finalPixel);
							image->PutPixelColor(xxx, yyy, colour);
							image->PutPixelAlpha(xxx, yyy, alpha);
							image->PutPixelZBuffer(xxx, yyy, float(depth));
							image->PutPixelOpacity(xxx, yyy, opacity16);
							if (image->GetImageOptional()->optionalNormal)
								image->PutPixelNormal(xxx, yyy, normalFloat);
							if (image->GetImageOptional()->optionalSpecular)
								image->PutPixelSpecular(xxx, yyy, specularFloat);
							if (image->GetImageOptional()->optionalWorld)
								image->PutPixelWorld(xxx, yyy, worldPositionRGB);
							if (image->GetImageOptional()->optionalDiffuse)
								image->PutPixelDiffuse(
									xxx, yyy, sRGBFloat(colour.R / 255.0f, colour.G / 255.0f, colour.B / 255.0f));
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
void cRenderWorker::PrepareMainVectors()
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
void cRenderWorker::PrepareReflectionBuffer()
{

	reflectionsMax = params->reflectionsMax * 1;
	if (!params->raytracedReflections) reflectionsMax = 0;
	rayBuffer = new sRayBuffer[reflectionsMax + 4];

	for (int i = 0; i < reflectionsMax + 3; i++)
	{
		// rayMarching buffers
		rayBuffer[i].stepBuff = new sStep[maxRaymarchingSteps + 2];
		rayBuffer[i].buffCount = 0;
	}

	rayStack = new sRayStack[reflectionsMax + 1];
}

// calculating vectors for AmbientOcclusion
void cRenderWorker::PrepareAOVectors()
{
	AOVectorsAround = new sVectorsAround[10000];
	AOVectorsCount = 0;
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
			AOVectorsAround[counter].alpha = a;
			AOVectorsAround[counter].beta = b;
			AOVectorsAround[counter].v = d;
			int X = int((a + b) / (2.0 * M_PI) * lightMapWidth + lightMapWidth * 8.5) % lightMapWidth;
			int Y = int(b / (M_PI)*lightMapHeight + lightMapHeight * 8.5) % lightMapHeight;
			AOVectorsAround[counter].R = data->textures.lightmapTexture.FastPixel(X, Y).R;
			AOVectorsAround[counter].G = data->textures.lightmapTexture.FastPixel(X, Y).G;
			AOVectorsAround[counter].B = data->textures.lightmapTexture.FastPixel(X, Y).B;
			if (AOVectorsAround[counter].R > 10 || AOVectorsAround[counter].G > 10
					|| AOVectorsAround[counter].B > 10)
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
		AOVectorsAround[0].alpha = 0;
		AOVectorsAround[0].beta = 0;
		AOVectorsAround[0].v.x = 0;
		AOVectorsAround[0].v.y = 0;
		AOVectorsAround[0].v.z = 0;
		AOVectorsAround[0].R = 0;
		AOVectorsAround[0].G = 0;
		AOVectorsAround[0].B = 0;
	}
	AOVectorsCount = counter;
}

// calculation of distance where ray-marching stops
double cRenderWorker::CalcDistThresh(CVector3 point) const
{
	double distThresh;
	if (params->iterThreshMode)
	{
		distThresh = (params->camera - point).Length() * params->resolution * params->fov;
	}
	else
	{
		if (params->constantDEThreshold)
			distThresh = params->DEThresh;
		else
			distThresh =
				(params->camera - point).Length() * params->resolution * params->fov / params->detailLevel;
	}

	if (params->perspectiveType == params::perspEquirectangular
			|| params->perspectiveType == params::perspFishEye
			|| params->perspectiveType == params::perspFishEyeCut)
		distThresh *= M_PI;

	if (params->advancedQuality)
	{
		if (distThresh > params->detailSizeMax) distThresh = params->detailSizeMax;
		if (distThresh < params->detailSizeMin) distThresh = params->detailSizeMin;
	}

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
void cRenderWorker::RayMarching(
	sRayMarchingIn &in, sRayMarchingInOut *inOut, sRayMarchingOut *out) const
{
	CVector3 point;
	bool found = false;
	double scan = in.minScan;
	double dist = 0;
	double search_accuracy = 0.001 * params->detailLevel;
	double search_limit = 1.0 - search_accuracy;
	int counter = 0;
	double step = 0.0;
	(*inOut->buffCount) = 0;
	double distThresh = 0;
	out->objectId = 0;

	// qDebug() << "Start ************************";

	CVector3 lastPoint;
	bool deadComputationFound = false;

	for (int i = 0; i < 10000; i++)
	{
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
		}
		else
		{
			step = (dist - 0.5 * distThresh) * params->DEFactor * (1.0 - Random(1000) / 10000.0);
		}

		if (params->advancedQuality)
		{
			if (step > params->absMaxMarchingStep) step = params->absMaxMarchingStep;
			if (step < params->absMinMarchingStep) step = params->absMinMarchingStep;
			if (distThresh > params->absMinMarchingStep)
			{
				if (step > params->relMaxMarchingStep * distThresh)
					step = params->relMaxMarchingStep * distThresh;
			}
			if (step < params->relMinMarchingStep * distThresh)
				step = params->relMinMarchingStep * distThresh;
		}
		else
		{
			if (step > 3.0) step = 3.0;
		}

		inOut->stepBuff[i].point = point;

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
	out->point = point;
	data->statistics.numberOfRaymarchings++;
}

cRenderWorker::sRayRecursionOut cRenderWorker::RayRecursion(
	sRayRecursionIn in, sRayRecursionInOut &inOut)
{
	// qDebug() << "----------- new pixel ------------";
	int rayIndex = 0; // level of recursion

	rayStack[rayIndex].in = in;
	rayStack[rayIndex].rayBranch = rayBranchReflection;
	rayStack[rayIndex].goDeeper = true;

	for (int i = 0; i < reflectionsMax + 1; i++)
	{
		rayStack[i].rayBranch = rayBranchReflection;
		rayStack[i].reflectShader = sRGBAfloat();
		rayStack[i].transparentShader = sRGBAfloat();
	}

	do
	{
		if (rayStack[rayIndex].goDeeper)
		{
			*inOut.rayMarchingInOut.buffCount = 0;

			// trace the light in given direction
			sRayMarchingOut rayMarchingOut;

			RayMarching(rayStack[rayIndex].in.rayMarchingIn, &inOut.rayMarchingInOut, &rayMarchingOut);
			CVector3 point = rayMarchingOut.point;

			// prepare data for texture shaders
			CVector3 lightVector = shadowVector;
			sShaderInputData shaderInputData;
			shaderInputData.distThresh = rayMarchingOut.distThresh;
			shaderInputData.delta = CalcDelta(point);
			shaderInputData.lightVect = lightVector;
			shaderInputData.point = point;
			shaderInputData.viewVector = rayStack[rayIndex].in.rayMarchingIn.direction;
			shaderInputData.lastDist = rayMarchingOut.lastDist;
			shaderInputData.depth = rayMarchingOut.depth;
			shaderInputData.stepCount = *inOut.rayMarchingInOut.buffCount;
			shaderInputData.stepBuff = inOut.rayMarchingInOut.stepBuff;
			shaderInputData.invertMode = rayStack[rayIndex].in.calcInside;
			shaderInputData.objectId = rayMarchingOut.objectId;
			cObjectData objectData = data->objectData[shaderInputData.objectId];
			shaderInputData.material = &data->materials[objectData.materialId];

			float reflect = shaderInputData.material->reflectance;
			float transparent = shaderInputData.material->transparencyOfSurface;

			rayStack[rayIndex].out.rayMarchingOut = rayMarchingOut;

			CVector3 vn;

			// if found any object
			if (rayMarchingOut.found)
			{
				// calculate normal vector
				vn = CalculateNormals(shaderInputData);

				float roughnessGradient = 1.0;
				if (shaderInputData.material->useColorsFromPalette
						&& shaderInputData.material->roughnessGradientEnable)
				{
					sGradientsCollection gradients;
					SurfaceColour(shaderInputData, &gradients);
					roughnessGradient = gradients.roughness.R;
				}

				float roughnessTex = 1.0;
				if (shaderInputData.material->roughnessTexture.IsLoaded()
						&& shaderInputData.material->useRoughnessTexture)
				{
					float texRoughInt = shaderInputData.material->roughnessTextureIntensity;
					float texRoughIntN = 1.0f - shaderInputData.material->roughnessTextureIntensity;
					roughnessTex = RoughnessTexture(shaderInputData) * texRoughInt + texRoughIntN;
				}

				if (shaderInputData.material->roughSurface)
				{
					vn.x += roughnessTex * roughnessGradient * shaderInputData.material->surfaceRoughness
									* (Random(20000) / 10000.0f - 1.0f);
					vn.y += roughnessTex * roughnessGradient * shaderInputData.material->surfaceRoughness
									* (Random(20000) / 10000.0f - 1.0f);
					vn.z += roughnessTex * roughnessGradient * shaderInputData.material->surfaceRoughness
									* (Random(20000) / 10000.0f - 1.0f);
					vn.Normalize();
				}
				shaderInputData.normal = vn;

				if (shaderInputData.material->normalMapTexture.IsLoaded())
				{
					vn = NormalMapShader(shaderInputData);
				}

				float hueEffect = 1.0;
				if (params->DOFMonteCarlo && params->DOFMonteCarloChromaticAberration)
				{
					float aberrationStrength = params->DOFMonteCarloCADispersionGain * 0.01f;
					hueEffect = 1.0f - aberrationStrength + aberrationStrength * actualHue / 180.0f;
				}

				// prepare refraction values
				float n1, n2;
				if (rayStack[rayIndex].in.calcInside) // if trace is inside the object
				{
					n1 = shaderInputData.material->transparencyIndexOfRefraction
							 / hueEffect; // reverse refractive indices
					n2 = 1.0f;
				}
				else
				{
					n1 = 1.0f;
					n2 = shaderInputData.material->transparencyIndexOfRefraction / hueEffect;
				}

				rayStack[rayIndex].out.normal = vn;

				if (rayIndex < reflectionsMax)
				{
					if (rayStack[rayIndex].rayBranch == rayBranchReflection)
					{
						// qDebug() << "Reflection" << rayIndex;
						rayStack[rayIndex].rayBranch = rayBranchRefraction;

						// calculate reflection
						if (reflect > 0.0f)
						{
							rayIndex++; // increase recursion level

							sRayRecursionIn recursionIn;
							sRayMarchingIn rayMarchingIn;
							sRayMarchingInOut rayMarchingInOut;

							// calculate new direction of reflection
							CVector3 newDirection =
								ReflectionVector(vn, rayStack[rayIndex - 1].in.rayMarchingIn.direction);
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
							recursionIn.resultShader = rayStack[rayIndex - 1].in.resultShader;
							recursionIn.objectColour = rayStack[rayIndex - 1].in.objectColour;
							recursionIn.rayBranch = rayBranchReflection;

							// setup buffers for ray data

							rayMarchingInOut.buffCount = &rayBuffer[rayIndex].buffCount;
							rayMarchingInOut.stepBuff = rayBuffer[rayIndex].stepBuff;
							inOut.rayMarchingInOut = rayMarchingInOut;

							// recursion for reflection
							rayStack[rayIndex].in = recursionIn;
							rayStack[rayIndex].goDeeper = true;
							rayStack[rayIndex].rayBranch = rayBranchReflection;
							continue;
						}
					}

					if (rayStack[rayIndex].rayBranch == rayBranchRefraction)
					{
						rayStack[rayIndex].rayBranch = rayBranchDone;
						// qDebug() << "Transparency" << rayIndex;
						// calculate refraction (transparency)
						if (transparent > 0.0f)
						{

							rayIndex++; // increase recursion level

							sRayRecursionIn recursionIn;
							sRayMarchingIn rayMarchingIn;
							sRayMarchingInOut rayMarchingInOut;

							// calculate direction of refracted light
							CVector3 newDirection =
								RefractVector(vn, rayStack[rayIndex - 1].in.rayMarchingIn.direction, n1, n2);

							// move starting point a little
							CVector3 newPoint = point
																	+ rayStack[rayIndex - 1].in.rayMarchingIn.direction
																			* shaderInputData.distThresh * 1.0;

							// if is total internal reflection the use reflection instead of refraction
							bool internalReflection = false;
							if (newDirection.Length() == 0.0)
							{
								newDirection =
									ReflectionVector(vn, rayStack[rayIndex - 1].in.rayMarchingIn.direction);
								newPoint = point
													 + rayStack[rayIndex - 1].in.rayMarchingIn.direction
															 * shaderInputData.distThresh * 1.0;
								internalReflection = true;
							}

							// preparation for new recursion
							rayMarchingIn.binaryEnable = true;
							rayMarchingIn.direction = newDirection;
							rayMarchingIn.maxScan = params->viewDistanceMax;
							rayMarchingIn.minScan = 0.0;
							rayMarchingIn.start = newPoint;
							rayMarchingIn.invertMode =
								!rayStack[rayIndex - 1].in.calcInside || internalReflection;
							recursionIn.rayMarchingIn = rayMarchingIn;
							recursionIn.calcInside = !rayStack[rayIndex - 1].in.calcInside || internalReflection;
							recursionIn.resultShader = rayStack[rayIndex - 1].in.resultShader;
							recursionIn.objectColour = rayStack[rayIndex - 1].in.objectColour;
							recursionIn.rayBranch = rayBranchRefraction;

							// setup buffers for ray data
							rayMarchingInOut.buffCount = &rayBuffer[rayIndex].buffCount;
							rayMarchingInOut.stepBuff = rayBuffer[rayIndex].stepBuff;
							inOut.rayMarchingInOut = rayMarchingInOut;

							// recursion for refraction
							rayStack[rayIndex].in = recursionIn;
							rayStack[rayIndex].goDeeper = true;
							rayStack[rayIndex].rayBranch = rayBranchReflection;
							continue;
						}
					}

					if (rayStack[rayIndex].rayBranch == rayBranchDone)
					{
						// qDebug() << "Done" << rayIndex;
						rayStack[rayIndex].goDeeper = false;
					}
				} // reflectionsMax
				else
				{
					rayStack[rayIndex].goDeeper = false;
				}

			} // found
			else
			{
				rayStack[rayIndex].goDeeper = false;
			}
		} // goDeeper

		if (!rayStack[rayIndex].goDeeper)
		{
			// qDebug() << "Shaders" << rayIndex;
			sRayRecursionOut recursionOut;

			recursionOut = rayStack[rayIndex].out;

			sRayMarchingOut rayMarchingOut = recursionOut.rayMarchingOut;

			CVector3 point = rayMarchingOut.point;

			sRGBAfloat reflectShader = rayStack[rayIndex].reflectShader;
			sRGBAfloat transparentShader = rayStack[rayIndex].transparentShader;

			inOut.rayMarchingInOut.buffCount = &rayBuffer[rayIndex].buffCount;
			inOut.rayMarchingInOut.stepBuff = rayBuffer[rayIndex].stepBuff;

			// prepare data for shaders
			CVector3 lightVector = shadowVector;
			sShaderInputData shaderInputData;
			shaderInputData.distThresh = rayMarchingOut.distThresh;
			shaderInputData.delta = CalcDelta(point);
			shaderInputData.lightVect = lightVector;
			shaderInputData.point = point;
			shaderInputData.viewVector = rayStack[rayIndex].in.rayMarchingIn.direction;
			shaderInputData.lastDist = rayMarchingOut.lastDist;
			shaderInputData.depth = rayMarchingOut.depth;
			shaderInputData.stepCount = *inOut.rayMarchingInOut.buffCount;
			shaderInputData.stepBuff = inOut.rayMarchingInOut.stepBuff;
			shaderInputData.invertMode = rayStack[rayIndex].in.calcInside;
			shaderInputData.objectId = rayMarchingOut.objectId;
			cObjectData objectData = data->objectData[shaderInputData.objectId];
			shaderInputData.material = &data->materials[objectData.materialId];

			shaderInputData.normal = recursionOut.normal;

			// letting colors from textures (before normal map shader)
			if (shaderInputData.material->colorTexture.IsLoaded())
			{
				shaderInputData.texColor =
					TextureShader(shaderInputData, texture::texColor, shaderInputData.material);
			}
			else
				shaderInputData.texColor = sRGBFloat(1.0, 1.0, 1.0);

			if (shaderInputData.material->luminosityTexture.IsLoaded())
				shaderInputData.texLuminosity =
					TextureShader(shaderInputData, texture::texLuminosity, shaderInputData.material);
			else
				shaderInputData.texLuminosity = sRGBFloat(0.0, 0.0, 0.0);

			if (shaderInputData.material->diffusionTexture.IsLoaded())
				shaderInputData.texDiffuse =
					TextureShader(shaderInputData, texture::texDiffuse, shaderInputData.material);
			else
				shaderInputData.texDiffuse = sRGBFloat(1.0, 1.0, 1.0);

			if (shaderInputData.material->reflectanceTexture.IsLoaded())
				shaderInputData.texReflectance =
					TextureShader(shaderInputData, texture::texReflectance, shaderInputData.material);
			else
				shaderInputData.texReflectance = sRGBFloat(1.0, 1.0, 1.0);

			if (shaderInputData.material->transparencyTexture.IsLoaded())
				shaderInputData.texTransparency =
					TextureShader(shaderInputData, texture::texTransparency, shaderInputData.material);
			else
				shaderInputData.texTransparency = sRGBFloat(1.0, 1.0, 1.0);

			float reflect = shaderInputData.material->reflectance;
			float transparent = shaderInputData.material->transparencyOfSurface;

			sRGBAfloat resultShader = rayStack[rayIndex].in.resultShader;
			sRGBAfloat objectColour = rayStack[rayIndex].in.objectColour;
			sRGBFloat transparentColor =
				sRGBFloat(shaderInputData.material->transparencyInteriorColor.R / 65536.0f,
					shaderInputData.material->transparencyInteriorColor.G / 65536.0f,
					shaderInputData.material->transparencyInteriorColor.B / 65536.0f);
			resultShader.R = transparentColor.R;
			resultShader.G = transparentColor.G;
			resultShader.B = transparentColor.B;

			sRGBAfloat objectShader;
			sRGBAfloat backgroundShader;
			sRGBAfloat volumetricShader;
			sRGBFloat iridescence;

			if (rayMarchingOut.found)
			{
				// qDebug() << "Found" << rayIndex;
				// calculate effects for object surface
				sGradientsCollection gradients;

				objectShader = ObjectShader(
					shaderInputData, &objectColour, &recursionOut.specular, &iridescence, &gradients);

				if (params->DOFMonteCarlo && params->DOFMonteCarloGlobalIllumination)
				{
					// calculate global illumination
					sRGBFloat globalIlumination = GlobalIlumination(shaderInputData, objectColour);
					objectShader.R += globalIlumination.R;
					objectShader.G += globalIlumination.G;
					objectShader.B += globalIlumination.B;
				}

				// calculate reflectance according to Fresnel equations

				// prepare refraction values
				float n1, n2;
				if (rayStack[rayIndex].in.calcInside) // if trace is inside the object
				{
					n1 =
						shaderInputData.material->transparencyIndexOfRefraction; // reverse refractive indices
					n2 = 1.0;
				}
				else
				{
					n1 = 1.0;
					n2 = shaderInputData.material->transparencyIndexOfRefraction;
				}

				float reflectance = 1.0;
				float reflectanceN = 1.0;

				if (shaderInputData.material->fresnelReflectance)
				{
					reflectance = Reflectance(
						shaderInputData.normal, rayStack[rayIndex].in.rayMarchingIn.direction, n1, n2);
					if (reflectance < 0.0f) reflectance = 0.0f;
					if (reflectance > 1.0f) reflectance = 1.0f;
					reflectanceN = 1.0f - reflectance;
				}

				if (rayIndex == reflectionsMax)
				{
					reflectance = 0.0;
					reflectanceN = 1.0;
				}

				// combine all results
				resultShader.R = (objectShader.R + recursionOut.specular.R);
				resultShader.G = (objectShader.G + recursionOut.specular.G);
				resultShader.B = (objectShader.B + recursionOut.specular.B);

				if (shaderInputData.material->useColorsFromPalette
						&& shaderInputData.material->transparencyGradientEnable)
				{
					transparentShader.R *= gradients.trasparency.R;
					transparentShader.G *= gradients.trasparency.G;
					transparentShader.B *= gradients.trasparency.B;
				}
				else
				{
					transparentShader.R *= shaderInputData.material->transparencyColor.R / 65536.0f;
					transparentShader.G *= shaderInputData.material->transparencyColor.G / 65536.0f;
					transparentShader.B *= shaderInputData.material->transparencyColor.B / 65536.0f;
				}

				if (shaderInputData.material->useTransparencyTexture)
				{
					float texTransInt = shaderInputData.material->transparencyTextureIntensity;
					float texTransIntN = 1.0f - shaderInputData.material->transparencyTextureIntensity;
					transparentShader.R *= shaderInputData.texTransparency.R * texTransInt + texTransIntN;
					transparentShader.G *= shaderInputData.texTransparency.G * texTransInt + texTransIntN;
					transparentShader.B *= shaderInputData.texTransparency.B * texTransInt + texTransIntN;
				}

				if (reflectionsMax > 0)
				{
					sRGBFloat reflectDiffused;
					float diffusionIntensity = shaderInputData.material->diffusionTextureIntensity;
					float diffusionIntensityN = 1.0f - diffusionIntensity;
					reflectDiffused.R = reflect * shaderInputData.texDiffuse.R * diffusionIntensity
															+ reflect * diffusionIntensityN;
					reflectDiffused.G = reflect * shaderInputData.texDiffuse.G * diffusionIntensity
															+ reflect * diffusionIntensityN;
					reflectDiffused.B = reflect * shaderInputData.texDiffuse.B * diffusionIntensity
															+ reflect * diffusionIntensityN;

					if (shaderInputData.material->useColorsFromPalette
							&& shaderInputData.material->diffuseGradientEnable)
					{
						reflectDiffused.R *= gradients.diffuse.R;
						reflectDiffused.G *= gradients.diffuse.G;
						reflectDiffused.B *= gradients.diffuse.B;
					}

					if (shaderInputData.material->useReflectanceTexture)
					{
						float texRefInt = shaderInputData.material->reflectanceTextureIntensity;
						float texRefIntN = 1.0f - shaderInputData.material->reflectanceTextureIntensity;
						reflectDiffused.R *= shaderInputData.texReflectance.R * texRefInt + texRefIntN;
						reflectDiffused.G *= shaderInputData.texReflectance.G * texRefInt + texRefIntN;
						reflectDiffused.B *= shaderInputData.texReflectance.B * texRefInt + texRefIntN;
					}

					reflectDiffused.R *= iridescence.R;
					reflectDiffused.G *= iridescence.G;
					reflectDiffused.B *= iridescence.B;

					if (shaderInputData.material->useColorsFromPalette
							&& shaderInputData.material->reflectanceGradientEnable)
					{
						reflectDiffused.R *= gradients.reflectance.R;
						reflectDiffused.G *= gradients.reflectance.G;
						reflectDiffused.B *= gradients.reflectance.B;
					}
					else
					{
						reflectDiffused.R *= shaderInputData.material->reflectionsColor.R / 65536.0f;
						reflectDiffused.G *= shaderInputData.material->reflectionsColor.G / 65536.0f;
						reflectDiffused.B *= shaderInputData.material->reflectionsColor.B / 65536.0f;
					}

					resultShader.R = transparentShader.R * transparent * reflectanceN
													 + (1.0f - transparent * reflectanceN) * resultShader.R;
					resultShader.G = transparentShader.G * transparent * reflectanceN
													 + (1.0f - transparent * reflectanceN) * resultShader.G;
					resultShader.B = transparentShader.B * transparent * reflectanceN
													 + (1.0f - transparent * reflectanceN) * resultShader.B;

					float reflectDiffusedAvg =
						(reflectDiffused.R + reflectDiffused.G + reflectDiffused.B) / 3.0f;

					resultShader.R = reflectShader.R * reflectDiffused.R * reflectance
													 + (1.0f - reflectance * reflectDiffusedAvg) * resultShader.R;
					resultShader.G = reflectShader.G * reflectDiffused.G * reflectance
													 + (1.0f - reflectance * reflectDiffusedAvg) * resultShader.G;
					resultShader.B = reflectShader.B * reflectDiffused.B * reflectance
													 + (1.0f - reflectance * reflectDiffusedAvg) * resultShader.B;
				}
				if (resultShader.R < 0.0f) resultShader.R = 0.0f;
				if (resultShader.G < 0.0f) resultShader.G = 0.0f;
				if (resultShader.B < 0.0f) resultShader.B = 0.0f;
			}
			else // if object not found then calculate background
			{
				// qDebug() << "Background";
				backgroundShader = BackgroundShader(shaderInputData);
				resultShader = backgroundShader;
				rayMarchingOut.depth = 1e20;
				recursionOut.specular = sRGBAfloat();
				shaderInputData.normal = mRot.RotateVector(CVector3(0.0, -1.0, 0.0));
				// rayStack[rayIndex].goDeeper = false;
			}

			sRGBAfloat opacityOut;

			if (rayStack[rayIndex].in.calcInside) // if the object interior is traced, then the absorption
																						// of light has to be
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

					float opacity =
						(-1.0f + 1.0f / shaderInputData.material->transparencyOfInterior) * float(step);
					if (opacity > 1.0f) opacity = 1.0f;

					resultShader.R = opacity * transparentColor.R + (1.0f - opacity) * resultShader.R;
					resultShader.G = opacity * transparentColor.G + (1.0f - opacity) * resultShader.G;
					resultShader.B = opacity * transparentColor.B + (1.0f - opacity) * resultShader.B;
				}
			}
			else // if now is outside the object, then calculate all volumetric effects like fog, glow...
			{
				volumetricShader = VolumetricShader(shaderInputData, resultShader, &opacityOut);
				resultShader = volumetricShader;
			}

			recursionOut.point = point;
			recursionOut.rayMarchingOut = rayMarchingOut;
			recursionOut.objectColour = objectColour;
			recursionOut.resultShader = resultShader;
			recursionOut.found = rayMarchingOut.found;
			recursionOut.fogOpacity = opacityOut.R;
			recursionOut.normal = shaderInputData.normal;

			rayStack[rayIndex].out = recursionOut;
			if (rayIndex > 0)
			{
				if (rayStack[rayIndex].in.rayBranch == rayBranchReflection)
				{
					rayStack[rayIndex - 1].reflectShader = resultShader;
				}

				if (rayStack[rayIndex].in.rayBranch == rayBranchRefraction)
				{
					rayStack[rayIndex - 1].transparentShader = resultShader;
				}
				rayStack[rayIndex - 1].in.resultShader = resultShader;
				rayStack[rayIndex - 1].in.objectColour = objectColour;
			}

			rayIndex--;
		}
		// prepare final result

	} while (rayIndex >= 0);

	sRayRecursionOut out = rayStack[0].out;
	return out;
}

void cRenderWorker::MonteCarloDOF(CVector3 *startRay, CVector3 *viewVector) const
{
	if (params->perspectiveType == params::perspThreePoint)
	{
		double randR = 0.0015 * params->DOFRadius * params->DOFFocus * sqrt(Random(65536) / 65536.0);
		double randAngle = Random(65536);
		CVector3 randVector(randR * sin(randAngle), 0.0, randR * cos(randAngle));
		CVector3 randVectorRot = mRot.RotateVector(randVector);
		CVector3 viewVectorTemp = *viewVector;
		viewVectorTemp -= randVectorRot / params->DOFFocus;
		*viewVector = viewVectorTemp;
		*startRay = params->camera + randVectorRot;
	}
	else
	{
		CVector3 viewVectorTemp = *viewVector;
		double randR = 0.0015 * params->DOFRadius * params->DOFFocus * sqrt(Random(65536) / 65536.0);
		double randAngle = Random(65536);
		CVector3 randVector(randR * sin(randAngle), 0.0, randR * cos(randAngle));

		CVector3 side = viewVectorTemp.Cross(params->topVector);
		side.Normalize();
		CVector3 topTemp = side.Cross(viewVectorTemp);
		topTemp.Normalize();
		CVector3 randVectorRot = side * randVector.x + topTemp * randVector.z;

		viewVectorTemp -= randVectorRot / params->DOFFocus;
		*viewVector = viewVectorTemp;
		*startRay = params->camera + randVectorRot;
	}
}

double cRenderWorker::MonteCarloDOFNoiseEstimation(
	sRGBFloat pixel, int repeat, sRGBFloat pixelSum, sRGBFloat &StdDevSum)
{
	sRGBFloat monteCarloDOFAvg;
	monteCarloDOFAvg.R = pixelSum.R / (repeat + 1);
	monteCarloDOFAvg.G = pixelSum.G / (repeat + 1);
	monteCarloDOFAvg.B = pixelSum.B / (repeat + 1);

	sRGBFloat monteCarloDOFSquareDiff;
	monteCarloDOFSquareDiff.R = (pixel.R - monteCarloDOFAvg.R) * (pixel.R - monteCarloDOFAvg.R);
	monteCarloDOFSquareDiff.G = (pixel.G - monteCarloDOFAvg.G) * (pixel.G - monteCarloDOFAvg.G);
	monteCarloDOFSquareDiff.B = (pixel.B - monteCarloDOFAvg.B) * (pixel.B - monteCarloDOFAvg.B);

	StdDevSum.R += monteCarloDOFSquareDiff.R;
	StdDevSum.G += monteCarloDOFSquareDiff.G;
	StdDevSum.B += monteCarloDOFSquareDiff.B;

	sRGBFloat monteCarloDOFStdDev;
	double totalStdDev = sqrt((StdDevSum.R + StdDevSum.G + StdDevSum.B) / (repeat + 1.0));

	totalStdDev /= sqrt(repeat + 1);

	double sumBrightness = (pixelSum.R + pixelSum.G + pixelSum.B) / (repeat + 1.0);
	if (sumBrightness > 1.0) totalStdDev /= sumBrightness;

	// noise value
	return totalStdDev;
}
