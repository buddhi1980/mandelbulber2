/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <memory>

#include <QObject>
#include <QThread>

#include "algebra.hpp"
#include "color_structures.hpp"
#include "texture_enums.hpp"

// forward declarations
class cMaterial;
class cLight;
class cCameraTarget;
class cImage;
struct sRenderData;
struct sParamRender;
class cNineFractals;
class cScheduler;

#define MAX_RAYMARCHING 10000

// ambient occlusion data
struct sVectorsAround
{
	double alpha;
	double beta;
	CVector3 v;
	sRGBFloat color;
};

class cRenderWorker : public QObject
{
	Q_OBJECT

public:
	struct sThreadData
	{
		int id;
		int startLine;
		std::shared_ptr<cScheduler> scheduler;
	};

	cRenderWorker(std::shared_ptr<const sParamRender> _params,
		std::shared_ptr<const cNineFractals> _fractal, std::shared_ptr<sThreadData> _threadData,
		std::shared_ptr<sRenderData> _data, std::shared_ptr<cImage> _image);
	~cRenderWorker() override;

	// PrepareAOVectors() is public because is needed also for OpenCL data
	void PrepareAOVectors();
	const sVectorsAround *getAOVectorsAround() const { return AOVectorsAround.data(); }
	int getAoVectorsCount() const { return AOVectorsCount; }

	QThread workerThread;

private:
	// ray-marching step data
	struct sStep
	{
		double distance = 0.0;
		double step = 0.0;
		CVector3 point;
		int iters = 0;
		double distThresh = 0.0;
	};

	struct sRayBuffer
	{
		std::vector<sStep> stepBuff;
		int buffCount = 0;
	};

	struct sRayMarchingIn
	{
		inline sRayMarchingIn &operator=(const sRayMarchingIn &assign) = default;
		CVector3 start;
		CVector3 direction;
		double minScan = 0.0;
		double maxScan = 0.0;
		bool binaryEnable = false;
		bool invertMode = false;
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
		double lastDist = 0.0;
		double depth = 0.0;
		double distThresh = 0.0;
		int objectId = 0;
		bool found = false;
		;
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
		sRGBAfloat outShadow;
		sRGBFloat outGlobalIllumination;
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
		sRGBFloat texTransparencyAlpha;
		float perlinNoise;
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
	double CloudOpacity(
		CVector3 point, double distance, double detailSize, double *distanceOut) const;
	double DistanceFogOpacity(const double step, double distance, double volFogDistanceFromSurface,
		double volFogDistanceFactor, double volFogDensity, double &distanceShifted) const;
	sRayRecursionOut RayRecursion(sRayRecursionIn in, sRayRecursionInOut &inOut);
	void MonteCarloDOF(CVector3 *startRay, CVector3 *viewVector) const;
	double MonteCarloDOFNoiseEstimation(
		sRGBFloat pixel, int repeat, sRGBFloat pixelSum, sRGBFloat &StdDevSum);

	// shaders
	sRGBAfloat ObjectShader(const sShaderInputData &input, sRGBAfloat *surfaceColour,
		sRGBAfloat *specularOut, sRGBFloat *iridescence, sRGBAfloat *outShadow,
		sRGBFloat *luminosityEmissiveOut, sGradientsCollection *gradients) const;
	CVector3 CalculateNormals(const sShaderInputData &input) const;
	sRGBAfloat SpecularHighlight(const sShaderInputData &input, CVector3 lightVector,
		float specularWidth, float roughness, sRGBFloat diffuseGradient) const;
	sRGBAfloat SpecularHighlightCombined(const sShaderInputData &input, CVector3 lightVector,
		sRGBAfloat surfaceColor, sRGBFloat diffuseGradient) const;
	sRGBAfloat SurfaceColour(
		CVector3 point, const sShaderInputData &input, sGradientsCollection *gradients) const;
	sRGBAfloat FastAmbientOcclusion(const sShaderInputData &input) const;
	sRGBAfloat AmbientOcclusion(const sShaderInputData &input) const;
	sRGBAfloat EnvMapping(const sShaderInputData &input) const;
	sRGBAfloat AuxLightsShader(const sShaderInputData &input, sRGBAfloat surfaceColor,
		sGradientsCollection *gradients, sRGBAfloat *specularOut, sRGBAfloat *outShadow) const;
	sRGBAfloat AuxShadow(const sShaderInputData &input, const cLight *light, double distance,
		CVector3 lightVector) const;
	sRGBAfloat LightShading(const sShaderInputData &input, sRGBAfloat surfaceColor,
		const cLight *light, sGradientsCollection *gradients, sRGBAfloat *outSpecular,
		sRGBAfloat *outShadow) const;
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
	sRGBFloat GlobalIlumination(
		const sShaderInputData &input, sRGBAfloat objectColor, bool volumetricMode) const;
	void RayleighScattering(const CVector3 &lightVectorTemp, const sShaderInputData &input,
		sRGBFloat &raleighScatteringRGB, sRGBFloat &mieScatteringRGB) const;
	void PerlinNoiseForShaders(sShaderInputData *shaderInputData) const;
	void PerlinNoiseForReflectance(
		const sShaderInputData &shaderInputData, sRGBFloat &reflectDiffused);
	void PerlinNoiseForTransparency(
		const sShaderInputData &shaderInputData, sRGBAfloat &transparency);

	// data got from main thread
	const sParamRender *params;
	const cNineFractals *fractal;
	sRenderData *data;
	std::shared_ptr<sThreadData> threadData;
	std::shared_ptr<cImage> image;

	// internal variables
	int maxRaymarchingSteps;
	CRotationMatrix mRot;
	CRotationMatrix mRotInv;
	CVector3 baseX;
	CVector3 baseY;
	CVector3 baseZ;
	CVector3 viewAngle;
	// CVector3 shadowVector;
	float actualHue;
	int AOVectorsCount;
	int reflectionsMax;
	bool stopRequest;

	// allocated objects
	std::unique_ptr<cCameraTarget> cameraTarget;
	std::vector<sRayBuffer> rayBuffer;
	std::vector<sRayStack> rayStack;
	std::vector<sVectorsAround> AOVectorsAround;

public slots:
	void doWork();

signals:
	void finished();
	void error(const QString &result);
};

#endif /* MANDELBULBER2_SRC_RENDER_WORKER_HPP_ */
