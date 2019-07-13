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

#ifndef MANDELBULBER2_SRC_RENDER_WORKER_HPP_
#define MANDELBULBER2_SRC_RENDER_WORKER_HPP_

#include <QObject>
#include <QThread>

#include "algebra.hpp"
#include "color_structures.hpp"
#include "texture_enums.hpp"

// forward declarations
class cMaterial;
struct sLight;
class cCameraTarget;
class cImage;
struct sRenderData;
struct sParamRender;
class cNineFractals;
class cScheduler;

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

	cRenderWorker(const sParamRender *_params, const cNineFractals *_fractal,
		sThreadData *_threadData, sRenderData *_data, cImage *_image);
	~cRenderWorker() override;

	// PrepareAOVectors() is public because is needed also for OpenCL data
	void PrepareAOVectors();
	sVectorsAround *getAOVectorsAround() const { return AOVectorsAround; }
	int getAoVectorsCount() const { return AOVectorsCount; }

	QThread workerThread;

private:
	// ray-marching step data
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

	struct sRayMarchingIn
	{
		inline sRayMarchingIn &operator=(const sRayMarchingIn &assign) = default;
		CVector3 start;
		CVector3 direction;
		double minScan;
		double maxScan;
		bool binaryEnable;
		bool invertMode;
	};

	struct sRayMarchingInOut
	{
		inline sRayMarchingInOut &operator=(const sRayMarchingInOut &assign) = default;
		sStep *stepBuff;
		int *buffCount;
	};

	struct sRayMarchingOut
	{
		CVector3 point;
		double lastDist;
		double depth;
		double distThresh;
		int objectId;
		bool found;
	};

	enum enumRayBranch
	{
		rayBranchReflection,
		rayBranchRefraction,
		rayBranchDone,
	};

	struct sRayRecursionIn
	{
		sRayMarchingIn rayMarchingIn;
		bool calcInside;
		sRGBAfloat resultShader;
		sRGBAfloat objectColour;
		enumRayBranch rayBranch;
	};

	struct sRayRecursionOut
	{
		sRayMarchingOut rayMarchingOut;
		CVector3 point;
		sRGBAfloat resultShader;
		sRGBAfloat objectColour;
		sRGBAfloat specular;
		CVector3 normal;
		float fogOpacity;
		bool found;
	};

	struct sRayRecursionInOut
	{
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
		sRGBFloat texDiffuse;
		sRGBFloat texColor;
		sRGBFloat texLuminosity;
		sRGBFloat texReflectance;
		sRGBFloat texTransparency;
	};

	struct sRayStack
	{
		sRayRecursionIn in;
		sRayRecursionOut out;
		sRGBAfloat reflectShader;
		sRGBAfloat transparentShader;
		enumRayBranch rayBranch;
		bool goDeeper;
	};

	struct sGradientsCollection
	{
		sRGBFloat surface;
		sRGBFloat specular;
		sRGBFloat diffuse;
		sRGBFloat luminosity;
		sRGBFloat roughness;
		sRGBFloat reflectance;
		sRGBFloat trasparency;
	};

	// functions
	void PrepareMainVectors();
	void PrepareReflectionBuffer();
	void RayMarching(sRayMarchingIn &in, sRayMarchingInOut *inOut, sRayMarchingOut *out) const;
	double CalcDistThresh(CVector3 point) const;
	double CalcDelta(CVector3 point) const;
	static double IterOpacity(
		double step, double iters, double maxN, double trim, double trimHigh, double opacitySp);
	sRayRecursionOut RayRecursion(sRayRecursionIn in, sRayRecursionInOut &inOut);
	void MonteCarloDOF(CVector3 *startRay, CVector3 *viewVector) const;
	double MonteCarloDOFNoiseEstimation(
		sRGBFloat pixel, int repeat, sRGBFloat pixelSum, sRGBFloat &StdDevSum);

	// shaders
	sRGBAfloat ObjectShader(const sShaderInputData &input, sRGBAfloat *surfaceColour,
		sRGBAfloat *specularOut, sRGBFloat *iridescence, sGradientsCollection *gradients) const;
	CVector3 CalculateNormals(const sShaderInputData &input) const;
	static sRGBAfloat MainShading(const sShaderInputData &input);
	sRGBAfloat MainShadow(const sShaderInputData &input) const;
	sRGBAfloat SpecularHighlight(const sShaderInputData &input, CVector3 lightVector,
		float specularWidth, float roughness, sRGBFloat diffuseGradient) const;
	sRGBAfloat SpecularHighlightCombined(const sShaderInputData &input, CVector3 lightVector,
		sRGBAfloat surfaceColor, sRGBFloat diffuseGradient) const;
	sRGBAfloat SurfaceColour(const sShaderInputData &input, sGradientsCollection *gradients) const;
	sRGBAfloat FastAmbientOcclusion(const sShaderInputData &input) const;
	sRGBAfloat AmbientOcclusion(const sShaderInputData &input) const;
	sRGBAfloat EnvMapping(const sShaderInputData &input) const;
	sRGBAfloat AuxLightsShader(const sShaderInputData &input, sRGBAfloat surfaceColor,
		sGradientsCollection *gradients, sRGBAfloat *specularOut) const;
	double AuxShadow(
		const sShaderInputData &input, double distance, CVector3 lightVector, double intensity) const;
	sRGBAfloat LightShading(const sShaderInputData &input, sRGBAfloat surfaceColor,
		const sLight *light, int number, sGradientsCollection *gradients,
		sRGBAfloat *outSpecular) const;
	sRGBAfloat BackgroundShader(const sShaderInputData &input) const;
	sRGBAfloat FakeLights(
		const sShaderInputData &input, sRGBAfloat surfaceColor, sRGBAfloat *fakeSpec) const;
	sRGBAfloat VolumetricShader(
		const sShaderInputData &input, sRGBAfloat oldPixel, sRGBAfloat *opacityOut) const;

	sRGBFloat TextureShader(
		const sShaderInputData &input, texture::enumTextureSelection texSelect, cMaterial *mat) const;
	CVector3 NormalMapShader(const sShaderInputData &input) const;
	float RoughnessTexture(const sShaderInputData &input) const;
	sRGBFloat IridescenceShader(const sShaderInputData &input) const;
	sRGBFloat GlobalIlumination(const sShaderInputData &input, sRGBAfloat objectColor) const;

	// data got from main thread
	const sParamRender *params;
	const cNineFractals *fractal;
	sRenderData *data;
	sThreadData *threadData;
	cImage *image;

	// internal variables
	int maxRaymarchingSteps;
	CRotationMatrix mRot;
	CRotationMatrix mRotInv;
	CVector3 baseX;
	CVector3 baseY;
	CVector3 baseZ;
	CVector3 viewAngle;
	CVector3 shadowVector;
	float actualHue;
	int AOVectorsCount;
	int reflectionsMax;
	bool stopRequest;

	// allocated objects
	cCameraTarget *cameraTarget;
	sRayBuffer *rayBuffer;
	sRayStack *rayStack;
	sVectorsAround *AOVectorsAround;

public slots:
	void doWork();

signals:
	void finished();
	void error(const QString &result);
};

#endif /* MANDELBULBER2_SRC_RENDER_WORKER_HPP_ */
