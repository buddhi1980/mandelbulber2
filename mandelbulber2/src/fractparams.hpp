/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * sParamRender struct - container for scene parameters
 */

#ifndef MANDELBULBER2_SRC_FRACTPARAMS_HPP_
#define MANDELBULBER2_SRC_FRACTPARAMS_HPP_

#include "ao_modes.h"
#include "color_gradient.h"
#include "common_params.hpp"
#include "fractal_enums.h"
#include "image_adjustments.h"
#include "primitives.h"
#include "projection_3d.hpp"

// forward declarations
class cObjectData;
class cParameterContainer;

namespace params
{
enum enumTextureMapType
{
	mapEquirectangular = 0,
	mapDoubleHemisphere = 1,
	mapFlat = 2
};

enum enumBooleanOperator
{
	booleanOperatorAND = 0,
	booleanOperatorOR = 1,
	booleanOperatorSUB = 2
};

} // namespace params

struct sParamRender
{
	// constructor with init
	sParamRender(
		const std::shared_ptr<cParameterContainer> par, QVector<cObjectData> *objectData = nullptr);

	int antialiasingSize;
	int antialiasingOclDepth;
	int ambientOcclusionQuality; // ambient occlusion quality
	int cloudsIterations;
	int cloudsRandomSeed;
	int frameNo;
	int imageHeight; // image height
	int imageWidth;	 // image width
	int formulaMaterialId[NUMBER_OF_FRACTALS];
	int minN; // minimum number of iterations
	int N;
	int nebulaNumberOfSamplesPerPixel;
	int nebulaMinIteration;
	int nebulaColorMixing; // 0 - lighten, 1 - darken, 2 - darken by brighness
	int reflectionsMax;
	int repeatFrom;
	int DOFNumberOfPasses;
	int DOFSamples;
	int DOFMinSamples;
	int monteCarloDenoiserStrength;
	int maxRaymarchingSteps;

	params::enumPerspectiveType perspectiveType;
	params::enumAOMode ambientOcclusionMode;
	params::enumTextureMapType texturedBackgroundMapType;
	params::enumBooleanOperator booleanOperator[NUMBER_OF_FRACTALS - 1];
	fractal::enumDEMethod delta_DE_method;
	fractal::enumDEFunctionType delta_DE_function;

	bool advancedQuality;
	bool allPrimitivesInvisibleAlpha;
	bool antialiasingEnabled;
	bool antialiasingAdaptive;
	bool ambientOcclusionEnabled; // enable global illumination
	bool background3ColorsEnable;
	bool booleanOperatorsEnabled;
	bool cloudsCastShadows;
	bool cloudsDistanceMode;
	bool cloudsEnable;
	bool cloudsPlaneShape;
	bool cloudsSharpEdges;
	bool constantDEThreshold;
	bool distanceFogShadows;
	bool DOFEnabled;
	bool DOFHDRMode;
	bool DOFMonteCarlo;
	bool DOFMonteCarloGlobalIllumination;
	bool DOFMonteCarloChromaticAberration;
	bool envMappingEnable;
	bool fakeLightsEnabled;
	bool fogEnabled;
	bool fogCastShadows;
	bool glowEnabled;
	bool hdrBlurEnabled;
	bool hybridFractalEnable;
	bool interiorMode;
	bool iterThreshMode;
	bool iterFogEnabled;
	bool iterFogShadows;
	bool legacyCoordinateSystem;
	bool limitsEnabled; // enable limits (intersections)
	bool monteCarloSoftShadows;
	bool monteCarloGIVolumetric;
	bool monteCarloGIOfVolumetric;
	bool monteCarloDenoiserEnable;
	bool monteCarloDenoiserPreserveGeometry;
	bool monteCarloPixelLevelOptimization;
	bool nebulaConstantBrighness;
	bool nebulaInnerEnabled;
	bool nebulaOuterEnabled;
	bool nebulaXAxisColorsEnabled;
	bool nebulaYAxisColorsEnabled;
	bool nebulaZAxisColorsEnabled;
	bool nebulaIterationsColorsEnabled;
	bool nebulaGridDomainEnabled;
	bool postChromaticAberrationEnabled;
	bool postChromaticAberrationReverse;
	bool raytracedReflections;
	bool slowShading; // enable fake gradient calculation for shading
	bool smoothDeCombineEnable[NUMBER_OF_FRACTALS];
	bool SSAO_random_mode;
	bool stereoSwapEyes;
	bool texturedBackground; // enable textured background
	bool useDefaultBailout;
	bool volFogEnabled;

	sRGBFloat ambientOcclusionColor;
	sRGBFloat background_color1; // background colour
	sRGBFloat background_color2;
	sRGBFloat background_color3;
	sRGBFloat cloudsColor;
	sRGBFloat fakeLightsColor;
	sRGBFloat fakeLightsColor2;
	sRGBFloat fakeLightsColor3;
	sRGBFloat fillLightColor;
	sRGBFloat fogColor;
	sRGBFloat glowColor1;
	sRGBFloat glowColor2;
	sRGBFloat iterFogColour1;
	sRGBFloat iterFogColour2;
	sRGBFloat iterFogColour3;
	sRGBFloat volFogColour1;
	sRGBFloat volFogColour2;
	sRGBFloat volFogColour3;

	double absMaxMarchingStep;
	double absMinMarchingStep;
	float ambientOcclusion;
	double ambientOcclusionFastTune;
	double background_brightness;
	double background_gamma;
	double backgroundHScale;
	double backgroundVScale;
	double backgroundTextureOffsetX;
	double backgroundTextureOffsetY;
	double cameraDistanceToTarget; // zoom
	double cloudsAmbientLight;
	double cloudsDEApproaching;
	double cloudsDEMultiplier;
	double cloudsDensity;
	double cloudsDetailAccuracy;
	double cloudsDistance;
	double cloudsDistanceLayer;
	double cloudsLightsBoost;
	double cloudsPeriod;
	double cloudsSharpness;
	double cloudsHeight;
	double cloudsOpacity;
	double constantFactor;
	double DEFactor; // factor for distance estimation steps
	double deltaDERelativeDelta;
	double detailLevel; // DE threshold factor
	double detailSizeMax;
	double detailSizeMin;
	double DEThresh;
	double DOFFocus;
	double DOFRadius;
	double DOFMaxRadius;
	double DOFBlurOpacity;
	double DOFMaxNoise;
	float DOFMonteCarloCADispersionGain;
	float DOFMonteCarloCACameraDispersion;
	double fakeLightsIntensity;
	float fakeLightsVisibility;
	float fakeLightsVisibilitySize;
	double fogVisibility;
	double formulaScale[NUMBER_OF_FRACTALS];
	double fov; // perspective factor
	float glowIntensity;
	double hdrBlurIntensity;
	double hdrBlurRadius;
	float iterFogColor1Maxiter;
	float iterFogColor2Maxiter;
	double iterFogOpacity;
	float iterFogOpacityTrim;
	float iterFogOpacityTrimHigh;
	float iterFogBrightnessBoost;
	float monteCarloGIRadianceLimit;
	float nebulaBrighness;
	float nebulaXGridSize;
	float nebulaYGridSize;
	float nebulaZGridSize;
	float postChromaticAberrationIntensity;
	float postChromaticAberrationRadius;
	float rayleighScatteringBlue;
	float rayleighScatteringRed;
	double relMaxMarchingStep;
	double relMinMarchingStep;
	double resolution; // resolution of image in fractal coordinates
	double smoothDeCombineDistance[NUMBER_OF_FRACTALS];
	double smoothness;
	double stereoEyeDistance;
	double stereoInfiniteCorrection;
	double sweetSpotHAngle;
	double sweetSpotVAngle;
	double viewDistanceMax;
	double viewDistanceMin;
	double volFogColour1Distance;
	double volFogColour2Distance;
	float volFogDensity;
	double volFogDistanceFactor;
	double volFogDistanceFromSurface;
	double volumetricLightDEFactor;

	sImageAdjustments imageAdjustments;

	CVector3 ambientOcclusionLightMapRotation;
	CVector3 backgroundRotation;
	CVector3 cloudsCenter;
	CVector3 cloudsRotation;
	CVector3 cloudsSpeed;
	CVector3 formulaPosition[NUMBER_OF_FRACTALS];
	CVector3 formulaRotation[NUMBER_OF_FRACTALS];
	CVector3 formulaRepeat[NUMBER_OF_FRACTALS];
	CVector3 limitMin;
	CVector3 limitMax;
	CVector3 repeat;
	CVector3 target;
	CVector3 camera; // view point
	CVector3 viewAngle;
	CVector3 topVector;

	CRotationMatrix mRotFormulaRotation[NUMBER_OF_FRACTALS];
	CRotationMatrix mRotBackgroundRotation;
	CRotationMatrix mRotCloudsRotation;
	CRotationMatrix mRotAmbientOcclusionLightMapRotation;

	cColorGradient nebulaXAxisColors;
	cColorGradient nebulaYAxisColors;
	cColorGradient nebulaZAxisColors;
	cColorGradient nebulaIterationsColors;

	cPrimitives primitives;

	sCommonParams common;
};

#endif /* MANDELBULBER2_SRC_FRACTPARAMS_HPP_ */
