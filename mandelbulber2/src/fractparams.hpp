/*********************************************************
 /                   MANDELBULBER
 / definition of structures for image and effect
 / parameters
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 ********************************************************/

#ifndef FRACTPARAMS_HPP_
#define FRACTPARAMS_HPP_

#include "texture.hpp"
#include "cimage.hpp"
#include "algebra.hpp"
#include "primitives.h"
#include "parameters.hpp"

namespace params
{
enum enumPerspectiveType
{
	threePoint = 0, fishEye = 1, equirectangular = 2, fishEyeCut = 3
};

enum enumAOMode
{
	AOmodeFast = 0,
	AOmodeMultipeRays = 1,
	AOmodeScreenSpace = 2
};

enum enumTextureMapType
{
	mapEquirectangular = 0,
	mapDoubleHemisphere = 1,
};

}

class cParamRender
{
public:

	//constructor with init
	cParamRender(const parameters::container *par);

	int ambientOcclusionQuality; //ambient occlusion quality
	int auxLightNumber;
	int auxLightRandomSeed;
	int coloring_seed; //colouring random seed
	int endFrame;
	int fakeLightsMaxIter;
	int fakeLightsMinIter;
	int framesPerKeyframe;
	int frameNo;
	int imageFormat;
	int imageHeight; //image height
	int imageWidth; //image width
	int minN;	  // minimum number of iterations
	int N;
	int noOfTiles;
	int OpenCLDOFMethod;
	int OpenCLEngine;
	int OpenCLPixelsPerJob;
	int reflectionsMax;
	int startFrame;
	int tileNumber;

	params::enumPerspectiveType perspectiveType;
	params::enumAOMode ambientOcclusionMode;
	params::enumTextureMapType texturedBackgroundMapType;

	bool ambientOcclusionEnabled; //enable global ilumination
	bool analitycDEMode;	 //analytic DE mode
	bool animMode; //animation mode
	bool auxLightPreEnabled[4];
	bool background_as_fulldome;
	bool coloringEnabled;
	bool constantDEThreshold;
	bool continueRecord; //continue recording mode
	bool DOFEnabled;
	bool fakeLightsEnabled;
	bool fastGlobalIllumination; //enable fake global ilumination
	bool fishEyeCut;
	bool fogEnabled;
	bool glowEnabled;
	bool hybridCyclic;
	bool interiorMode;
	bool iterFogEnabled;
	bool iterThreshMode;	 //maxiter threshold mode
	bool juliaMode;
	bool limitsEnabled; // enable limits (intersections)
	bool linearDEmode;
	bool mainLightEnable;
	bool mainLightPositionAsRelative;
	bool normalCalculationMode;
	bool penetratingLights;
	bool playMode; //play mode
	bool quiet;
	bool raytracedReflections;
	bool recordMode; //path recording mode
	bool shadow; //enable shadows
	bool slowShading; //enable fake gradient calculation for shading
	bool sphericalFoldingMode;  // spherical folding mode
	bool SSAOEnabled;
	bool stereoEnabled;
	bool texturedBackground; //enable testured background
	bool tgladFoldingMode;		// Tglad's folding mode
	bool useCustomOCLFormula;
	bool volumetricLightEnabled[5];
	bool volFogEnabled;

	sRGB auxLightPreColour[4];
	sRGB background_color1; //background colour
	sRGB background_color2;
	sRGB background_color3;
	sRGB fogColor;
	sRGB volFogColour1;
	sRGB volFogColour2;
	sRGB volFogColour3;
	sRGB iterFogColour1;
	sRGB iterFogColour2;
	sRGB iterFogColour3;
	sRGB glowColor1;
	sRGB glowColor2;
	sRGB mainLightColour;

  double ambient;
  double ambientOcclusion;
	double ambientOcclusionFastTune;
	double auxLightDistributionRadius;
	double auxLightIntensity;
	double auxLightMaxDist;
	double auxLightPreIntensity[4];
	double auxLightVisibility;
  double coloring_speed;
	double colourSaturation;
	double constantFactor;
	double DEFactor; //factor for distance estimation steps
	double detailLevel; //DE threshold factor
	double DEThresh;
  double directLight;
	double DOFFocus;
	double DOFRadius;
	double fakeLightsIntensity;
	double fakeLightsVisibility;
	double fakeLightsVisibilitySize;
	double volFogColour1Distance;
	double volFogColour2Distance;
	double volFogDensity;
	double volFogDistanceFactor;
  double fogVisibility;
	double fov; //perspective factor
  double glowIntensity;
	double iterFogOpacity;
	double iterFogOpacityTrim;
	double iterFogColor1Maxiter;
	double iterFogColor2Maxiter;
	double mainLightAlpha;
	double mainLightBeta;
  double mainLightIntensity;
  double mainLightVisibility;
  double mainLightVisibilitySize;
  double paletteOffset;

  double reflect;
	double resolution; //resolution of image in fractal coordinates
	double shading;
	double shadowConeAngle;
	double smoothness;
  double specular;
	double stereoEyeDistance;
	double viewDistanceMax;
	double viewDistanceMin;
	double volumetricLightIntensity[5];
	double zoom; //zoom

	sImageAdjustments imageAdjustments;

	CVector3 auxLightPre[4];
	CVector3 auxLightRandomCenter;
	CVector3 fakeLightsOrbitTrap;
	CVector3 juliaC; // Julia constant
	CVector3 limitMin;
	CVector3 limitMax;
	CVector3 target;
	CVector3 viewAngle;
	CVector3 vp; //view point

	double settingsVersion;

	sPrimitives primitives;
};

struct sTextures
{
	cTexture *backgroundTexture;
	cTexture *envmapTexture;
	cTexture *lightmapTexture;
};


#endif /* FRACTPARAMS_HPP_ */
