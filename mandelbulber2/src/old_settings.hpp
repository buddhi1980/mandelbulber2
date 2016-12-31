/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Rayan Hitchman
 *
 * cOldSettings class - importing settings from old Mandelbulber (v1.21)
 */

#ifndef MANDELBULBER2_SRC_OLD_SETTINGS_HPP_
#define MANDELBULBER2_SRC_OLD_SETTINGS_HPP_

#include <vector>

#include "algebra.hpp"
#include "color_structures.hpp"

// forward declarations
class cFractalContainer;
class cParameterContainer;

namespace oldSettings
{

const int IFS_VECTOR_COUNT = 9;
const int HYBRID_COUNT = 5;
const int MANDELBOX_FOLDS = 2;

enum enumPerspectiveType
{
	threePoint = 0,
	fishEye = 1,
	equirectangular = 2,
	fishEyeCut = 3
};

enum enumFractalFormula
{
	none = 0,
	trig_DE = 1,
	trig_optim = 2,
	fast_trig = 3,
	hypercomplex = 4,
	quaternion = 5,
	minus_fast_trig = 6,
	menger_sponge = 7,
	tglad = 8,
	kaleidoscopic = 10,
	xenodreambuie = 11,
	hybrid = 12,
	mandelbulb2 = 13,
	mandelbulb3 = 14,
	mandelbulb4 = 15,
	foldingIntPow2 = 16,
	smoothMandelbox = 17,
	mandelboxVaryScale4D = 18,
	aexion = 19,
	benesi = 20,
	bristorbrot = 21,
	invertX = 22,
	invertY = 23,
	invertZ = 24,
	invertR = 25,
	sphericalFold = 26,
	powXYZ = 27,
	scaleX = 28,
	scaleY = 29,
	scaleZ = 30,
	offsetX = 31,
	offsetY = 32,
	offsetZ = 33,
	angleMultiplyX = 34,
	angleMultiplyY = 35,
	angleMultiplyZ = 36,
	generalizedFoldBox = 37,
	ocl_custom = 38
};

enum enumCalculationMode
{
	normal = 0,
	colouring = 1,
	fake_AO = 2,
	deltaDE1 = 3,
	deltaDE2 = 4,
	orbitTrap = 5
};

enum enumGeneralizedFoldBoxType
{
	foldTet = 0,
	foldCube = 1,
	foldOct = 2,
	foldDodeca = 3,
	foldOctCube = 4,
	foldIcosa = 5,
	foldBox6 = 6,
	foldBox5 = 7
};

enum enumObjectType
{
	objFractal = 0,
	objPlane = 1,
	objWater = 2,
	objSphere = 3,
	objSphereInv = 4,
	objBox = 5,
	objBoxInv = 6
};

enum enumOCLDEMode
{
	calculated = 0,
	deltaDE = 1,
	noDE = 2
};

struct sFractalIFSD
{
	double rotationGamma;
	double rotationAlfa;
	double rotationBeta;
	double scale;
	double distance[IFS_VECTOR_COUNT];
	double alfa[IFS_VECTOR_COUNT];
	double beta[IFS_VECTOR_COUNT];
	double gamma[IFS_VECTOR_COUNT];
	double intensity[IFS_VECTOR_COUNT];
	CVector3 offset;
	CVector3 direction[IFS_VECTOR_COUNT];
	CVector3 edge;
};

struct sFractalGeneralizedFoldBox
{
	enum enumGeneralizedFoldBoxType type;
	CVector3 Nv_tet[4];
	CVector3 Nv_cube[6];
	CVector3 Nv_oct[8];
	CVector3 Nv_oct_cube[14];
	CVector3 Nv_dodeca[12];
	CVector3 Nv_icosa[20];
	CVector3 Nv_box6[8];
	CVector3 Nv_box5[7];
	int sides_tet;
	int sides_cube;
	int sides_oct;
	int sides_oct_cube;
	int sides_dodeca;
	int sides_icosa;
	int sides_box6;
	int sides_box5;
};

struct sFractalIFS
{
	sFractalIFSD doubles;
	bool absX, absY, absZ;
	bool foldingMode; // Kaleidoscopic IFS folding mode
	bool enabled[IFS_VECTOR_COUNT];
	bool mengerSpongeMode;
	int foldingCount;
	CRotationMatrix mainRot;
	CRotationMatrix rot[IFS_VECTOR_COUNT];
};

struct sFractalMandelboxVary4D
{
	double fold;
	double minR;
	double scaleVary;
	double wadd;
	double rPower;
};

struct sFractalMandelboxD
{
	double rotationMain[3];
	double rotation[MANDELBOX_FOLDS][3][3];
	double colorFactorX;
	double colorFactorY;
	double colorFactorZ;
	double colorFactorR;
	double colorFactorSp1;
	double colorFactorSp2;
	double scale;
	double foldingLimit;
	double foldingValue;
	double foldingSphericalMin;
	double foldingSphericalFixed;
	double sharpness;
	double solid;
	double melt;
	CVector3 offset;
	sFractalMandelboxVary4D vary4D;
};

struct sFractalMandelbox
{
	sFractalMandelboxD doubles;
	bool rotationsEnabled;
	CRotationMatrix mainRot;
	CRotationMatrix rot[MANDELBOX_FOLDS][3];
	CRotationMatrix rotinv[MANDELBOX_FOLDS][3];
};

struct sFractalPrimitivesD
{
	CVector3 planeCentre;
	CVector3 planeNormal;
	CVector3 boxCentre;
	CVector3 boxSize;
	CVector3 invertedBoxCentre;
	CVector3 invertedBoxSize;
	CVector3 sphereCentre;
	double sphereRadius;
	CVector3 invertedSphereCentre;
	double invertedSphereRadius;
	double waterHeight;
	double waterAmplitude;
	double waterLength;
	double waterRotation;
	double waterAnimSpeed;
};

struct sFractalPrimitives
{
	bool planeEnable;
	bool boxEnable;
	bool invertedBoxEnable;
	bool sphereEnable;
	bool invertedSphereEnable;
	bool waterEnable;
	bool onlyPlane;
	int waterIterations;
};

struct sFractalD
{
	double N;
	double amin; // fractal limits
	double amax;
	double bmin;
	double bmax;
	double cmin;
	double cmax;
	double constantFactor;
	double FoldingIntPowZfactor;
	double FoldingIntPowFoldFactor;
	double foldingLimit; // parameters of TGlad's folding
	double foldingValue;
	double foldingSphericalMin;
	double foldingSphericalFixed;
	double detailSize;
	double power; // power of fractal formula
	double cadd;
	double hybridPower[HYBRID_COUNT];
	CVector3 julia; // Julia constant
	CVector3 fakeLightsOrbitTrap;
	sFractalPrimitivesD primitives;
};

struct sFractal
{
	sFractalD doubles;

	// maximum number of iterations
	int minN; // minimum number of iterations

	bool limits_enabled;			 // enable limits (intersections)
	bool iterThresh;					 // maxiter threshold mode
	bool analyticDE;					 // analytic DE mode
	bool juliaMode;						 // Julia mode
	bool tgladFoldingMode;		 // Tglad's folding mode
	bool sphericalFoldingMode; // spherical folding mode
	bool interiorMode;
	bool hybridCyclic;
	bool linearDEmode;
	bool constantDEThreshold;
	bool useCustomOCLFormula;
	bool normalCalculationMode;

	enumFractalFormula formula;

	int hybridIters[HYBRID_COUNT];
	enumFractalFormula hybridFormula[HYBRID_COUNT];

	std::vector<enumFractalFormula> formulaSequence;
	std::vector<double> hybridPowerSequence;
	char customOCLFormulaName[100];
	enumOCLDEMode customOCLFormulaDEMode;

	sFractalIFS IFS;
	sFractalMandelbox mandelbox;
	sFractalGeneralizedFoldBox genFoldBox;
	sFractalPrimitives primitives;

	int frameNo;

	int itersOut;
	enumObjectType objectOut;

	int fakeLightsMinIter;
	int fakeLightsMaxIter;
};

struct sImageAdjustments
{
	double shading;
	double directLight;
	double ambient;
	double specular;
	double reflect;
	double globalIlum;
	double brightness;
	double glow_intensity;
	double fogVisibility;
	double coloring_speed;
	double imageGamma;
	double paletteOffset;
	double mainLightIntensity;
	double contrast;
};

struct sParamRenderD
{
	double zoom;			 // zoom
	double DE_factor;	// factor for distance estimation steps
	double resolution; // resolution of image in fractal coordinates
	double persp;			 // perspective factor
	double quality;		 // DE threshold factor
	double smoothness;
	double alpha; // rotation of fractal
	double beta;	//
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

	oldSettings::sImageAdjustments imageAdjustments;

	CVector3 vp; // view point
	CVector3 auxLightPre[4];
	CVector3 auxLightRandomCenter;
};

struct sImageSwitches
{
	bool coloringEnabled;
	bool fogEnabled;
	bool raytracedReflections;
	bool volumetricLightEnabled;
	bool iterFogEnabled;
	bool hdrEnabled;
};

struct sEffectColours
{
	sRGB glow_color1;
	sRGB glow_color2;
	sRGB fogColor;
	sRGB mainLightColour;
};

struct sParamRender
{
	sParamRenderD doubles;

	sFractal fractal;
	int image_width;			 // image width
	int image_height;			 // image height
	int globalIlumQuality; // ambient occlusion quality
	int reflectionsMax;
	int coloring_seed; // colouring random seed
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

	enumPerspectiveType perspectiveType;

	bool shadow;								 // enable shadows
	bool global_ilumination;		 // enable global illumination
	bool fastGlobalIllumination; // enable fake global illumination
	bool slowShading;						 // enable fake gradient calculation for shading
	bool texturedBackground;		 // enable textured background
	bool background_as_fulldome;
	bool recordMode;		 // path recording mode
	bool continueRecord; // continue recording mode
	bool playMode;			 // play mode
	bool animMode;			 // animation mode
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

	sRGB background_color1; // background colour
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

	sRGB palette[256];

	char file_destination[1000];
	char file_envmap[1000];
	char file_background[1000];
	char file_lightmap[1000];
	char file_path[1000];
	char file_keyframes[1000];

	std::vector<enumFractalFormula> formulaSequence;
	std::vector<double> hybridPowerSequence;

	double settingsVersion;
};

class cOldSettings
{
public:
	cOldSettings();
	~cOldSettings();
	bool LoadSettings(const QString &filename);
	void ConvertToNewContainer(cParameterContainer *params, cFractalContainer *fractal);

private:
	bool LoadSettings2(const QString &filename);
	bool LoadOneSetting(const char *str1, const char *str2, sParamRender *params);
	double atof2(const char *str);
	void GetPaletteFromString(sRGB *palette, const char *paletteString);
	sParamRender *oldData;
	bool paletteLoadedFromSettingsFile;
};

} /* namespace oldSettings */

#endif /* MANDELBULBER2_SRC_OLD_SETTINGS_HPP_ */
