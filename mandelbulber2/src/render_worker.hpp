/*
 * render_worker.hpp
 *
 *  Created on: May 4, 2014
 *      Author: krzysztof
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

	cRenderWorker(const cParamRender *_params, const cFourFractals *_fractal, sThreadData *_threadData, const sRenderData *_data, cImage *_image);
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

	//reflection data
	struct sReflect
	{
		CVector3 start;
		CVector3 point;
		CVector3 viewVector;
		CVector3 distance;
		sStep *stepBuff;
		int buffCount;
		double depth;
		double lastDist;
		bool found;
		double distThresh;
		fractal::enumObjectType objectType;
		double reflect;
	};

	//ambient occlussion data
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
		bool found;
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
	};

	//functions
	void PrepareMainVectors(void);
	void PrepareReflectionBuffer(void);
	void PrepareAOVectors(void);
	CVector3 RayMarching(sRayMarchingIn &in, sRayMarchingInOut *inOut, sRayMarchingOut *out);
	double ReflectValueForObject(fractal::enumObjectType object);
	double CalcDistThresh(CVector3 point);
	double CalcDelta(CVector3 point);
	double IterOpacity(double step, double iters, double maxN, double trim, double opacitySp);

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
	const sRenderData *data;
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
	long int missed_DE_counter;
	long int DECounter;
	long int pixelCounter;
	int histogramDE[256];
	int histogramIters[256];
	int reflectionsMax;
	bool stopRequest;

	//allocated objects
	cCameraTarget *cameraTarget;
	sReflect *reflectBuff;
	sVectorsAround *AOvectorsAround;

public slots:
	void doWork(void);

signals:
	void finished();
	void error(const QString &result);

};

#endif /* RENDER_WORKER_HPP_ */
