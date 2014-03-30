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

#include "fractal.h"
#include "texture.hpp"

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


class sParamRender
{
public:
	sFractal fractal;
	int image_width; //image width
	int image_height; //image height
	int globalIlumQuality; //ambient occlusion quality
	int reflectionsMax;
	int coloring_seed; //colouring random seed
	int auxLightRandomSeed;
	int auxLightNumber;
	int SSAOQuality;
	int startFrame;
	int endFrame;
	int framesPerKeyframe;
	int imageFormat;
	int noOfTiles;
	int tileCount;

	int OpenCLEngine;
	int OpenCLPixelsPerJob;
	int OpenCLDOFMethod;

	params::enumPerspectiveType perspectiveType;

	bool shadow; //enable shadows
	bool global_ilumination; //enable global ilumination
	bool fastGlobalIllumination; //enable fake global ilumination
	bool slowShading; //enable fake gradient calculation for shading
	bool texturedBackground; //enable testured background
	bool background_as_fulldome;
	bool recordMode; //path recording mode
	bool continueRecord; //continue recording mode
	bool playMode; //play mode
	bool animMode; //animation mode
	bool SSAOEnabled;
	bool DOFEnabled;
	bool auxLightPreEnabled[4];
	bool volumetricLightEnabled[5];
	bool penetratingLights;
	bool stereoEnabled;
	bool quiet;
	bool fishEyeCut;
	bool fakeLightsEnabled;
	sImageSwitches imageSwitches;

	sRGB background_color1; //background colour
	sRGB background_color2;
	sRGB background_color3;
	sRGB auxLightPreColour[4];
	sRGB fogColour1;
	sRGB fogColour2;
	sRGB fogColour3;
	sRGB primitivePlaneColour;
	sRGB primitiveBoxColour;
	sRGB primitiveInvertedBoxColour;
	sRGB primitiveSphereColour;
	sRGB primitiveInvertedSphereColour;
	sRGB primitiveWaterColour;
	sEffectColours effectColours;

	double zoom; //zoom
	double DE_factor; //factor for distance estimation steps
	double resolution; //resolution of image in fractal coordinates
	double persp; //perspective factor
	double quality; //DE threshold factor
	double smoothness;
	double alpha; //rotation of fractal
	double beta; //
	double gamma;
	double DOFFocus;
	double DOFRadius;
	double mainLightAlpha;
	double mainLightBeta;
	double auxLightIntensity;
	double auxLightMaxDist;
	double auxLightDistributionRadius;
	double auxLightVisibility;
	double auxLightPreIntensity[4];
	double stereoEyeDistance;
	double viewDistanceMin;
	double viewDistanceMax;
	double volumetricLightIntensity[5];
	double fogDensity;
	double fogColour1Distance;
	double fogColour2Distance;
	double fogDistanceFactor;
	double colourSaturation;
	double fastAoTune;
	double iterFogOpacity;
	double iterFogOpacityTrim;
	double fakeLightsIntensity;
	double fakeLightsVisibility;
	double fakeLightsVisibilitySize;
	double shadowConeAngle;
	double primitivePlaneReflect;
	double primitiveBoxReflect;
	double primitiveInvertedBoxReflect;
	double primitiveSphereReflect;
	double primitiveInvertedSphereReflect;
	double primitiveWaterReflect;

	sImageAdjustments imageAdjustments;

	CVector3 vp; //view point
	CVector3 auxLightPre[4];
	CVector3 auxLightRandomCenter;

	double settingsVersion;
};

struct sTextures
{
	cTexture *backgroundTexture;
	cTexture *envmapTexture;
	cTexture *lightmapTexture;
};

struct sRenderData
{
	sTextures textures;
	sRGB palette[256];
	std::vector<fractal::enumFractalFormula> formulaSequence;
	std::vector<double> hybridPowerSequence;
};
#endif /* FRACTPARAMS_HPP_ */
