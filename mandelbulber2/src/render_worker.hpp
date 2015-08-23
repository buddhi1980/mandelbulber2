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

#ifndef RENDER_WORKER_HPP_
#define RENDER_WORKER_HPP_

#include <QThread>

#include "cimage.hpp"
#include "fractparams.hpp"
#include "fractal.h"
#include "render_image.hpp"
#include "scheduler.hpp"
#include "camera_target.hpp"

class cRenderWorker: public QObject
{
Q_OBJECT

public:

	struct sThreadData
	{
		int id;
		int startLine;
		cScheduler *scheduler;
	};

	cRenderWorker(const cParamRender *_params, const cFourFractals *_fractal, sThreadData *_threadData, sRenderData *_data, cImage *_image);
	~cRenderWorker();

	QThread workerThread;

private:

	//raymarching step data
	struct sStep
	{
		double distance;
		double step;
		CVector3 point;
		int iters;
		double distThresh;
	};

	struct sRayBuffer
	{
		sStep *stepBuff;
		int buffCount;
	};


	//ambient occlusion data
	struct sVectorsAround
	{
		double alpha;
		double beta;
		CVector3 v;
		int R;
		int G;
		int B;
		bool notBlack;
	};

	struct sRayMarchingIn
	{
		CVector3 start;
		CVector3 direction;
		double minScan;
		double maxScan;
		bool binaryEnable;
		bool invertMode;
	};

	struct sRayMarchingInOut
	{
		sStep *stepBuff;
		int *buffCount;
	};

	struct sRayMarchingOut
	{
		double lastDist;
		double depth;
		double distThresh;
		fractal::enumObjectType object;
		int formulaIndex;
		sRGB objectColor;
		double objectReflect;
		bool found;
	};

	struct sRayRecursionIn
	{
		sRayMarchingIn rayMarchingIn;
		bool calcInside;
		sRGBAfloat resultShader;
		sRGBAfloat objectColour;
	};

	struct sRayRecursionOut
	{
		sRayMarchingOut rayMarchingOut;
		CVector3 point;
		sRGBAfloat resultShader;
		sRGBAfloat objectColour;
		double fogOpacity;
		bool found;
	};

	struct sRayRecursionInOut
	{
		int rayIndex;
		sRayMarchingInOut rayMarchingInOut;
	};

	struct sShaderInputData
	{
		CVector3 point;
		CVector3 viewVector;
		CVector3 normal;
		CVector3 lightVect;
		double distThresh; //distance threshold depend on 'detailLevel'
		double lastDist;
		double delta; //initial step distance for shaders based on distance form camera
		double depth;
		sStep *stepBuff;
		int stepCount;
		fractal::enumObjectType objectType;
		int formulaIndex;
		sRGB objectColor;
		double objectReflect;
		bool invertMode;
	};

	//functions
	void PrepareMainVectors(void);
	void PrepareReflectionBuffer(void);
	void PrepareAOVectors(void);
	CVector3 RayMarching(sRayMarchingIn &in, sRayMarchingInOut *inOut, sRayMarchingOut *out);
	double CalcDistThresh(CVector3 point);
	double CalcDelta(CVector3 point);
	double IterOpacity(double step, double iters, double maxN, double trim, double opacitySp);
	sRayRecursionOut RayRecursion(sRayRecursionIn in, sRayRecursionInOut &inOut);

	//shaders
	sRGBAfloat ObjectShader(const sShaderInputData &input, sRGBAfloat *surfaceColour, sRGBAfloat *specularOut);
	CVector3 CalculateNormals(const sShaderInputData &input);
	sRGBAfloat MainShading(const sShaderInputData &input);
	sRGBAfloat MainShadow(const sShaderInputData &input);
	sRGBAfloat MainSpecular(const sShaderInputData &input);
	sRGBAfloat SurfaceColour(const sShaderInputData &input);
	sRGBAfloat FastAmbientOcclusion(const sShaderInputData &input);
	sRGBAfloat AmbientOcclusion(const sShaderInputData &input);
	sRGBAfloat EnvMapping(const sShaderInputData &input);
	sRGBAfloat AuxLightsShader(const sShaderInputData &input, sRGBAfloat *specularOut);
	double AuxShadow(const sShaderInputData &input, double distance, CVector3 lightVector);
	sRGBAfloat LightShading(const sShaderInputData &input, cLights::sLight light, int number, sRGBAfloat *outSpecular);
	sRGBAfloat BackgroundShader(const sShaderInputData &input);
	sRGBAfloat FakeLights(const sShaderInputData &input, sRGBAfloat *fakeSpec);
	sRGBAfloat VolumetricShader(const sShaderInputData &input, sRGBAfloat oldPixel, sRGBAfloat *opacityOut);

	//data got from main thread
	const cParamRender *params;
	const cFourFractals *fractal;
	sRenderData *data;
	sThreadData *threadData;
	cImage *image;

	//internal variables
	int maxraymarchingSteps;
	CRotationMatrix mRot;
	CVector3 baseX;
	CVector3 baseY;
	CVector3 baseZ;
	CVector3 viewAngle;
	CVector3 shadowVector;
	int AOvectorsCount;
	int reflectionsMax;
	bool stopRequest;

	//allocated objects
	cCameraTarget *cameraTarget;
	sRayBuffer *rayBuffer;
	sVectorsAround *AOvectorsAround;

public slots:
	void doWork(void);

signals:
	void finished();
	void error(const QString &result);

};

#endif /* RENDER_WORKER_HPP_ */
