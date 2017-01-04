/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_SRC_RENDER_WORKER_HPP_
#define MANDELBULBER2_SRC_RENDER_WORKER_HPP_

#include <QThread>
#include <QObject>

#include "color_structures.hpp"
#include "texture_enums.hpp"
#include "algebra.hpp"

// forward declarations
class cMaterial;
struct sLight;
class cCameraTarget;
class cImage;
struct sRenderData;
class cParamRender;
class cNineFractals;
class cScheduler;

class cRenderWorker : public QObject
{
	Q_OBJECT

public:
	struct sThreadData
	{
		int id;
		int startLine;
		cScheduler *scheduler;
	};

	cRenderWorker(const cParamRender *_params, const cNineFractals *_fractal,
		sThreadData *_threadData, sRenderData *_data, cImage *_image);
	~cRenderWorker();

	QThread workerThread;

private:
	// raymarching step data
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

	// ambient occlusion data
	struct sVectorsAround
	{
		double alpha;
		double beta;
		CVector3 v;
		int R;
		int G;
		int B;
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
		int objectId;
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
		CVector3 normal;
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
		double distThresh; // distance threshold depend on 'detailLevel'
		double lastDist;
		double delta; // initial step distance for shaders based on distance form camera
		double depth;
		sStep *stepBuff;
		int stepCount;
		int objectId;
		bool invertMode;
		cMaterial *material;
		sRGBfloat texDiffuse;
		sRGBfloat texColor;
		sRGBfloat texLuminosity;
	};

	// functions
	void PrepareMainVectors();
	void PrepareReflectionBuffer();
	void PrepareAOVectors();
	CVector3 RayMarching(sRayMarchingIn &in, sRayMarchingInOut *inOut, sRayMarchingOut *out) const;
	double CalcDistThresh(CVector3 point) const;
	double CalcDelta(CVector3 point) const;
	static double IterOpacity(double step, double iters, double maxN, double trim, double opacitySp);
	sRayRecursionOut RayRecursion(sRayRecursionIn in, sRayRecursionInOut &inOut);
	void MonteCarloDOF(CVector2<double> imagePoint, CVector3 *startRay, CVector3 *viewVector) const;

	// shaders
	sRGBAfloat ObjectShader(
		const sShaderInputData &input, sRGBAfloat *surfaceColour, sRGBAfloat *specularOut) const;
	CVector3 CalculateNormals(const sShaderInputData &input) const;
	static sRGBAfloat MainShading(const sShaderInputData &input);
	sRGBAfloat MainShadow(const sShaderInputData &input) const;
	sRGBAfloat MainSpecular(const sShaderInputData &input) const;
	sRGBAfloat SurfaceColour(const sShaderInputData &input) const;
	sRGBAfloat FastAmbientOcclusion(const sShaderInputData &input) const;
	sRGBAfloat AmbientOcclusion(const sShaderInputData &input) const;
	sRGBAfloat EnvMapping(const sShaderInputData &input) const;
	sRGBAfloat AuxLightsShader(const sShaderInputData &input, sRGBAfloat *specularOut) const;
	double AuxShadow(const sShaderInputData &input, double distance, CVector3 lightVector) const;
	sRGBAfloat LightShading(
		const sShaderInputData &input, const sLight *light, int number, sRGBAfloat *outSpecular) const;
	sRGBAfloat BackgroundShader(const sShaderInputData &input) const;
	sRGBAfloat FakeLights(const sShaderInputData &input, sRGBAfloat *fakeSpec) const;
	sRGBAfloat VolumetricShader(
		const sShaderInputData &input, sRGBAfloat oldPixel, sRGBAfloat *opacityOut) const;

	sRGBfloat TextureShader(
		const sShaderInputData &input, texture::enumTextureSelection texSelect, cMaterial *mat) const;
	CVector3 NormalMapShader(const sShaderInputData &input) const;

	// data got from main thread
	const cParamRender *params;
	const cNineFractals *fractal;
	sRenderData *data;
	sThreadData *threadData;
	cImage *image;

	// internal variables
	int maxraymarchingSteps;
	CRotationMatrix mRot;
	CRotationMatrix mRotInv;
	CVector3 baseX;
	CVector3 baseY;
	CVector3 baseZ;
	CVector3 viewAngle;
	CVector3 shadowVector;
	int AOvectorsCount;
	int reflectionsMax;
	bool stopRequest;

	// allocated objects
	cCameraTarget *cameraTarget;
	sRayBuffer *rayBuffer;
	sVectorsAround *AOvectorsAround;

public slots:
	void doWork();

signals:
	void finished();
	void error(const QString &result);
};

#endif /* MANDELBULBER2_SRC_RENDER_WORKER_HPP_ */
