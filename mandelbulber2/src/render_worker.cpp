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


cRenderWorker::cRenderWorker(const cParamRender *_params, const cFourFractals *_fractal, sThreadData *_threadData, sRenderData *_data, cImage *_image)
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

	bool lastLineWasBroken = false;

	//main loop for y
	for (int ys = threadData->startLine; scheduler->ThereIsStillSomethingToDo(threadData->id); ys = scheduler->NextLine(threadData->id, ys, lastLineWasBroken))
	{
		//skip if line is out of region
		if (ys < 0) break;
		if (ys < data->screenRegion.y1 || ys > data->screenRegion.y2) continue;

		//main loop for x
		for (int xs = 0; xs < width; xs += scheduler->GetProgressiveStep())
		{

			//break if by coincidence this thread started rendering the same line as some other
			lastLineWasBroken = false;
			if (scheduler->ShouldIBreak(threadData->id, ys))
			{
				lastLineWasBroken = true;
				break;
			}

			if (scheduler->GetProgressivePass() > 1 && xs % (scheduler->GetProgressiveStep() * 2) == 0 && ys % (scheduler->GetProgressiveStep() * 2) == 0) continue;

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
			double depth = 1e20;
			double opacity = 1.0;

			//raymarching loop (reflections)

			if (!hemisphereCut) //in fulldome mode, will not render pixels out of the fulldome
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
				if(!recursionOut.found) depth = 1e20;
				opacity = recursionOut.fogOpacity;
			}

			sRGBfloat pixel2;
			pixel2.R = resultShader.R;
			pixel2.G = resultShader.G;
			pixel2.B = resultShader.B;
			unsigned short alpha = resultShader.A * 65535;
			unsigned short opacity16 = opacity * 65535;

			sRGB8 colour;
			colour.R = objectColour.R * 255;
			colour.G = objectColour.G * 255;
			colour.B = objectColour.B * 255;

			for (int xx = 0; xx < scheduler->GetProgressiveStep(); ++xx)
			{
				for (int yy = 0; yy < scheduler->GetProgressiveStep(); ++yy)
				{
					image->PutPixelImage(screenPoint.x + xx, screenPoint.y + yy, pixel2);
					image->PutPixelColour(screenPoint.x + xx, screenPoint.y + yy, colour);
					image->PutPixelAlpha(screenPoint.x + xx, screenPoint.y + yy, alpha);
					image->PutPixelZBuffer(screenPoint.x + xx, screenPoint.y + yy, (float) depth);
					image->PutPixelOpacity(screenPoint.x + xx, screenPoint.y + yy, opacity16);
				}
			}

			data->statistics.numberOfRenderedPixels++;
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

	reflectionsMax = params->reflectionsMax * 2;
	if (!params->raytracedReflections) reflectionsMax = 0;
	rayBuffer = new sRayBuffer[reflectionsMax + 4];

	for (int i = 0; i < reflectionsMax + 3; i++)
	{
		//rayMarching buffers
		rayBuffer[i].stepBuff = new sStep[maxraymarchingSteps + 2];
		rayBuffer[i].buffCount = 0;
	}
}

//calculating vectors for AmbientOcclusion
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
			int X = (int) ((a + b) / (2.0 * M_PI) * lightMapWidth + lightMapWidth * 8.5) % lightMapWidth;
			int Y = (int) (b / (M_PI) * lightMapHeight + lightMapHeight * 8.5) % lightMapHeight;
			AOvectorsAround[counter].R = data->textures.lightmapTexture.FastPixel(X, Y).R;
			AOvectorsAround[counter].G = data->textures.lightmapTexture.FastPixel(X, Y).G;
			AOvectorsAround[counter].B = data->textures.lightmapTexture.FastPixel(X, Y).B;
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
		//qDebug() <<"thresh" <<  distThresh << "dist" << dist << "scan" << scan;
		if(in.invertMode)
		{
			dist = distThresh * 1.99 - dist;
			if(dist < 0.0) dist = 0.0;
		}
		out->object = distanceOut.object;
		out->formulaIndex = distanceOut.formulaIndex;
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

		data->statistics.histogramIterations.Add(distanceOut.iters);
		data->statistics.totalNumberOfIterations+=distanceOut.totalIters;

		if (dist > 3.0) dist = 3.0;
		if (dist < distThresh)
		{
			if (dist < 0.1 * distThresh) data->statistics.missedDE++;
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
		scan += step / in.direction.Length(); //divided by length of view Vector to eliminate overstepping when fov is big
		if (scan > in.maxScan)
		{
			break;
		}
	}
	//------------- 83.2473 us for RayMarching loop -------------------------

	//qDebug() << "------------ binary search";
	if (found && in.binaryEnable)
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
			dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut);

			//qDebug() << "i" << i <<"thresh" <<  distThresh << "dist" << dist << "scan" << scan << "step" << step;

			if(in.invertMode)
			{
				dist = distThresh * 1.99 - dist;
				if(dist < 0.0) dist = 0.0;
			}

			out->object = distanceOut.object;
			out->formulaIndex = distanceOut.formulaIndex;

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

			data->statistics.histogramIterations.Add(distanceOut.iters);
			data->statistics.totalNumberOfIterations+=distanceOut.totalIters;

			step *= 0.5;
		}
	}
	if(params->iterThreshMode)
	{
		//this fixes problem with noise when there is used "stop at maxIter" mode
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

cRenderWorker::sRayRecursionOut cRenderWorker::RayRecursion(sRayRecursionIn in, sRayRecursionInOut &inOut)
{
	sRayMarchingOut rayMarchingOut;

	*inOut.rayMarchingInOut.buffCount = 0;

	//trace the light in given direction
	CVector3 point = RayMarching(in.rayMarchingIn, &inOut.rayMarchingInOut, &rayMarchingOut);

	sRGBAfloat resultShader = in.resultShader;
	sRGBAfloat objectColour = in.objectColour;

	//here will be called branch for RayRecursion();

	sRGBAfloat objectShader;
	objectShader.A = 0.0;
	sRGBAfloat backgroundShader;
	sRGBAfloat volumetricShader;
	sRGBAfloat specular;

	//prepare data for shaders
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
	shaderInputData.objectType = rayMarchingOut.object;
	shaderInputData.objectColor = rayMarchingOut.objectColor;
	shaderInputData.formulaIndex = rayMarchingOut.formulaIndex;
	shaderInputData.invertMode = in.calcInside;


	sRGBAfloat reflectShader = in.resultShader;
	double reflect = rayMarchingOut.objectReflect;
	sRGBAfloat transparentShader = in.resultShader;
	double transparent = params->transparencyOfSurface;
	sRGBfloat transparentColor = sRGBfloat(	params->transparencyInteriorColor.R / 65536.0,
																					params->transparencyInteriorColor.G / 65536.0,
																					params->transparencyInteriorColor.B / 65536.0);
	resultShader.R = transparentColor.R;
	resultShader.G = transparentColor.G;
	resultShader.B = transparentColor.B;

	//if found any object
	if(rayMarchingOut.found)
	{
		//calculate normal vector
		CVector3 vn = CalculateNormals(shaderInputData);

		//prepare refraction values
		double n1, n2;
		if(in.calcInside) //if trance is inside the object
		{
			n1 = params->transparencyIndexOfRefraction; //reverse refractive indices
			n2 = 1.0;
		}
		else
		{
			n1 = 1.0;
			n2 = params->transparencyIndexOfRefraction;;
		}

		if(inOut.rayIndex < reflectionsMax)
		{

			//calculate refraction (transparency)
			if(transparent > 0.0)
			{
				sRayRecursionIn recursionIn;
				sRayMarchingIn rayMarchingIn;
				sRayMarchingInOut rayMarchingInOut;

				//calculate direction of refracted light
				CVector3 newDirection = RefractVector(vn, in.rayMarchingIn.direction, n1, n2);

				//move starting point a little
				CVector3 newPoint = point + in.rayMarchingIn.direction * shaderInputData.distThresh * 1.0;

				//if is total internal reflection the use reflection instead of refraction
				bool internalReflection = false;
				if(newDirection.Length() == 0.0)
				{
					newDirection = ReflectionVector(vn, in.rayMarchingIn.direction);
					newPoint = point + in.rayMarchingIn.direction * shaderInputData.distThresh * 1.0;
					internalReflection = true;
				}

				//preparation for new recursion
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

				//setup buffers for ray data
				inOut.rayIndex++; //increase recursion index
				rayMarchingInOut.buffCount = &rayBuffer[inOut.rayIndex].buffCount;
				rayMarchingInOut.stepBuff = rayBuffer[inOut.rayIndex].stepBuff;
				inOut.rayMarchingInOut = rayMarchingInOut;

				//recursion for refraction
				sRayRecursionOut recursionOutTransparent = RayRecursion(recursionIn, inOut);
				transparentShader = recursionOutTransparent.resultShader;
			}

			//calculate reflection
			if(reflect > 0.0)
			{
				sRayRecursionIn recursionIn;
				sRayMarchingIn rayMarchingIn;
				sRayMarchingInOut rayMarchingInOut;

				//calculate new direction of reflection
				CVector3 newDirection = ReflectionVector(vn, in.rayMarchingIn.direction);
				CVector3 newPoint = point + newDirection * shaderInputData.distThresh;

				//prepare for new recursion
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

				//setup buffers for ray data
				inOut.rayIndex++; //increase recursion index
				rayMarchingInOut.buffCount = &rayBuffer[inOut.rayIndex].buffCount;
				rayMarchingInOut.stepBuff = rayBuffer[inOut.rayIndex].stepBuff;
				inOut.rayMarchingInOut = rayMarchingInOut;

				//recursion for reflection
				sRayRecursionOut recursionOutReflect = RayRecursion(recursionIn, inOut);
				reflectShader = recursionOutReflect.resultShader;

			}
			if(transparent > 0.0) inOut.rayIndex--; //decrease recursion index
			if(reflect > 0.0) inOut.rayIndex--; //decrease recursion index
		}

		shaderInputData.normal = vn;

		//calculate effects for object surface
		objectShader = ObjectShader(shaderInputData, &objectColour, &specular);

		//calculate reflectance according to Fresnel equations
		double reflectance = 1.0;
		double reflectanceN = 1.0;

		if(params->fresnelReflectance)
		{
			reflectance = Reflectance(vn, in.rayMarchingIn.direction, n1, n2);
			if(reflectance < 0.0) reflectance = 0.0;
			if(reflectance > 1.0) reflectance = 1.0;
			reflectanceN = 1.0 - reflectance;
		}

		//combine all results
		resultShader.R = (objectShader.R + specular.R);
		resultShader.G = (objectShader.G + specular.G);
		resultShader.B = (objectShader.B + specular.B);

		if(reflectionsMax > 0)
		{
			resultShader.R = transparentShader.R * transparent * reflectanceN + (1.0 - transparent * reflectanceN) * resultShader.R;
			resultShader.G = transparentShader.G * transparent * reflectanceN + (1.0 - transparent * reflectanceN) * resultShader.G;
			resultShader.B = transparentShader.B * transparent * reflectanceN + (1.0 - transparent * reflectanceN) * resultShader.B;

			resultShader.R = reflectShader.R * reflect * reflectance + (1.0 - reflect * reflectance) * resultShader.R;
			resultShader.G = reflectShader.G * reflect * reflectance + (1.0 - reflect * reflectance) * resultShader.G;
			resultShader.B = reflectShader.B * reflect * reflectance + (1.0 - reflect * reflectance) * resultShader.B;
		}
	}
	else //if object not found then calculate background
	{
		backgroundShader = BackgroundShader(shaderInputData);
		resultShader = backgroundShader;
		shaderInputData.depth = 1e20;
	}

	sRGBAfloat opacityOut;

	if(in.calcInside) //if the object interior is traced, then the absorption of light has to be calculated
	{
		for(int index = shaderInputData.stepCount - 1; index > 0; index--)
		{
			double step = shaderInputData.stepBuff[index].step;

			//CVector3 point = shaderInputData.stepBuff[index].point;
			//shaderInputData.point = point;
			//sRGBAfloat color = SurfaceColour(shaderInputData);
			//transparentColor.R = color.R;
			//transparentColor.G = color.G;
			//transparentColor.B = color.B;

			double opacity = -log(params->transparencyOfInterior) * step;
			if(opacity > 1.0) opacity = 1.0;

			resultShader.R = opacity * transparentColor.R + (1.0 - opacity) * resultShader.R;
			resultShader.G = opacity * transparentColor.G + (1.0 - opacity) * resultShader.G;
			resultShader.B = opacity * transparentColor.B + (1.0 - opacity) * resultShader.B;
		}
	}
	else //if now is outside the object, then calculate all volumetric effects like fog, glow...
	{
		volumetricShader = VolumetricShader(shaderInputData, resultShader, &opacityOut);
		resultShader = volumetricShader;
	}

	//prepare final result
	sRayRecursionOut out;
	out.point = point;
	out.rayMarchingOut = rayMarchingOut;
	out.objectColour = objectColour;
	out.resultShader = resultShader;
	out.found = (shaderInputData.depth == 1e20) ? false : true;
	out.fogOpacity = opacityOut.R;

	return out;
}



