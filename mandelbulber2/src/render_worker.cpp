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
 * cRenderWorker class - worker for rendering image on single CPU core
 */

#include "render_worker.hpp"

#include "ao_modes.h"
#include "calculate_distance.hpp"
#include "camera_target.hpp"
#include "cimage.hpp"
#include "common_math.h"
#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "hsv2rgb.h"
#include "material.h"
#include "perlin_noise_octaves.h"
#include "projection_3d.hpp"
#include "region.hpp"
#include "render_data.hpp"
#include "scheduler.hpp"
#include "stereo.h"
#include "system_data.hpp"
#include "texture.hpp"

cRenderWorker::cRenderWorker(std::shared_ptr<const sParamRender> _params,
	std::shared_ptr<const cHybridFractalSequences> _fractal, std::shared_ptr<sThreadData> _threadData,
	std::shared_ptr<sRenderData> _data, std::shared_ptr<cImage> _image)
{
	params = _params.get();
	fractal = _fractal.get();
	data = _data.get();
	image = _image;
	threadData = _threadData;
	cameraTarget = nullptr;
	AOVectorsCount = 0;
	baseX = CVector3(1.0, 0.0, 0.0);
	baseY = CVector3(0.0, 1.0, 0.0);
	baseZ = CVector3(0.0, 0.0, 1.0);
	maxRaymarchingSteps = 10000;
	reflectionsMax = 0;
	actualHue = 0.0;
	stopRequest = false;
}

cRenderWorker::~cRenderWorker()
{
	// nothing to delete
}

// Main rendering entry point, executed as a thread. Processes a subset of image rows
// assigned by the scheduler. Each pixel goes through: view vector calculation → raymarching
// (with reflections/refractions) → shading → optional post-processing (AA, MC, stereo).
//
// The rendering pipeline:
// 1. For each pixel: calculate the camera ray direction
// 2. Raymarch along the ray using SDF distance estimation to find the surface
// 3. Recursively trace reflections and refractions (stack-based, not true recursion)
// 4. Shade the hit point (lights, AO, fog, etc.)
// 5. Accumulate samples for AA/MC/DOF and average
// 6. Write pixel and optional channels (normal, depth, shadow, GI) to the image buffer
void cRenderWorker::doWork()
{
	// here will be rendering thread
	int width = image->GetWidth();
	int height = image->GetHeight();
	// Aspect ratio for the image plane. For equirectangular (360°) projection,
	// the horizontal field of view is 2π, so the effective aspect ratio is 2.0.
	double aspectRatio = double(width) / height;

	if (params->perspectiveType == params::perspEquirectangular) aspectRatio = 2.0;

	bool monteCarlo = params->DOFMonteCarlo;
	bool antiAliasing = params->antialiasingEnabled;
	int antiAliasingSize = params->antialiasingSize;

	// Stereo rendering can modify the aspect ratio for anaglyph modes.
	if (data->stereo.isEnabled() && (params->perspectiveType != params::perspEquirectangular))
		aspectRatio = data->stereo.ModifyAspectRatio(aspectRatio);

	PrepareMainVectors();
	PrepareReflectionBuffer();
	if (params->ambientOcclusionEnabled && params->ambientOcclusionMode == params::AOModeMultipleRays)
		PrepareAOVectors();

	// init of scheduler
	cScheduler *scheduler = threadData->scheduler.get();

	// start point for ray-marching (camera position)
	CVector3 start = params->camera;

	scheduler->InitFirstLine(threadData->id, threadData->startLine);

	bool lastLineWasBroken = false;

	// main loop for y — rows are distributed across threads by the scheduler
	for (int ys = threadData->startLine; scheduler->ThereIsStillSomethingToDo(threadData->id);
		ys = scheduler->NextLine(threadData->id, ys, lastLineWasBroken))
	{
		// skip if line is out of region
		if (ys < 0) break;
		if (ys < data->screenRegion.y1 || ys > data->screenRegion.y2) continue;

		// main loop for x — stepped by scheduler.GetProgressiveStep() for progressive rendering
		for (int xs = 0; xs < width; xs += scheduler->GetProgressiveStep())
		{
			if (systemData.globalStopRequest) break;
			// break if by coincidence this thread started rendering the same line as some other
			lastLineWasBroken = false;
			if (scheduler->ShouldIBreak(threadData->id, ys))
			{
				lastLineWasBroken = true;
				break;
			}

			// Progressive rendering: skip pixels on even passes to reduce redundant work.
			// Each pass samples a staggered subset of pixels.
			if (scheduler->GetProgressivePass() > 1 && xs % (scheduler->GetProgressiveStep() * 2) == 0
					&& ys % (scheduler->GetProgressiveStep() * 2) == 0)
				continue;

			// skip if pixel is out of region;
			if (xs < data->screenRegion.x1 || xs > data->screenRegion.x2) continue;

			// calculate point in image coordinate system
			CVector2<int> screenPoint(xs, ys);
			// Map screen pixel coordinates to normalized image space.
			CVector2<double> imagePoint = data->screenRegion.transpose(data->imageRegion, screenPoint);
			cStereo::enumEye stereoEye = data->stereo.WhichEye(imagePoint);
			if (data->stereo.isEnabled())
			{
				imagePoint = data->stereo.ModifyImagePoint(imagePoint);
			}
			imagePoint.x *= aspectRatio;

			// full dome hemisphere cut: for fulldome (fisheye cut) projection,
			// pixels outside the hemisphere (angle > 90°) are skipped.
			bool hemisphereCut = false;
			if (params->perspectiveType == params::perspFishEyeCut
					&& imagePoint.Length() > M_PI * 0.5f / params->fov)
				hemisphereCut = true;

			// Determine the number of samples per pixel.
			// Stereo modes may require multiple repeats (left eye, right eye, or combined anaglyph).
			int repeats = data->stereo.GetNumberOfRepeats();

			sRGBFloat finalPixel;
			sRGBFloat pixelLeftEye;
			sRGBFloat pixelRightEye;
			sRGB8 colour;
			unsigned short alpha = 65535;
			unsigned short opacity16 = 65535;
			sRGBFloat normalFloat;
			sRGBFloat normalFloatWorld;
			sRGBFloat specularFloat;
			double depth = 1e20;
			sRGBFloat worldPositionRGB;
			sRGBFloat shadowsChannel;
			sRGBFloat giChannel;
			// Monte Carlo mode: each repeat is a DOF sample.
			if (monteCarlo) repeats = params->DOFSamples;
			// Anti-aliasing: each repeat is a sub-pixel sample position.
			if (antiAliasing) repeats *= antiAliasingSize * antiAliasingSize;

			sRGBFloat finalPixelDOF;
			unsigned int finalAlphaDOF = 0;
			unsigned int finalOpacityDOF = 0;
			sRGB finalColourDOF;

			sRGBFloat monteCarloDOFStdDevSum;
			double monteCarloNoise = 0.0;

			CVector2<double> originalImagePoint = imagePoint;

			// Sample accumulation loop: AA sub-pixel positions, MC DOF samples, or stereo eye passes.
			for (int repeat = 0; repeat < repeats; repeat++)
			{

				CVector3 viewVector;
				CVector3 startRay;

				if (antiAliasing)
				{
					// Anti-aliasing: offset the sample position within the pixel grid.
					// The 'repeat' index is decoded into a sub-pixel grid position.
					int xStep = repeat / antiAliasingSize;
					int yStep = repeat % antiAliasingSize;
					double xOffset = double(xStep) / antiAliasingSize / image->GetWidth() * aspectRatio;
					double yOffset = double(yStep) / antiAliasingSize / image->GetHeight();
					imagePoint.x = originalImagePoint.x + xOffset;
					imagePoint.y = originalImagePoint.y + yOffset;
				}

				if (monteCarlo)
				{
					if (!antiAliasing)
					{
						// MC anti-aliasing: random jitter within the pixel (same as AA but stochastic).
						imagePoint.x =
							originalImagePoint.x
							+ (double(Random(1000)) / 1000.0 - 0.5) / image->GetWidth() * aspectRatio;
						imagePoint.y =
							originalImagePoint.y + (double(Random(1000)) / 1000.0 - 0.5) / image->GetHeight();
					}

					viewVector = CalculateViewVector(imagePoint, params->fov, params->perspectiveType, mRot);
					startRay = start;

					// Depth of field: perturb the ray origin (lens position) and direction.
					if (params->DOFEnabled)
					{
						MonteCarloDOF(&startRay, &viewVector);
					}
				}
				else
				{
					// calculate direction of ray-marching
					viewVector = CalculateViewVector(imagePoint, params->fov, params->perspectiveType, mRot);
					startRay = start;
				}

				// Chromatic aberration: offset the view vector per sample based on a random hue.
				// The hue is converted to an RGB offset that shifts the ray direction,
				// simulating wavelength-dependent refraction through the lens.
				sRGBFloat rgbFromHsv;
				if (params->DOFMonteCarlo && params->DOFMonteCarloChromaticAberration)
				{
					actualHue = Random(3600) / 10.0;
					rgbFromHsv = Hsv2rgb(fmodf(360.0f + float(actualHue) - 60.0f, 360.0f), 1.0f, 2.0f);
					CVector3 randVector(
						0.0, actualHue / 20000.0f * params->DOFMonteCarloCACameraDispersion, 0.0);
					CVector3 randVectorRot = mRot.RotateVector(randVector);
					viewVector -= randVectorRot;
					viewVector.Normalize();
				}

				// Stereo rendering: adjust the ray origin and direction for each eye.
				// For anaglyph modes, WhichEyeForAnaglyph determines which eye this repeat belongs to.
				if (data->stereo.isEnabled())
				{
					data->stereo.WhichEyeForAnaglyph(&stereoEye, repeat);
					if (params->perspectiveType == params::perspFishEyeCut)
					{
						// For fisheye cut, compute left/right eye positions manually
						// by offsetting along the camera's right vector.
						CVector3 eyePosition;
						CVector3 sideVector = viewVector.Cross(params->topVector);
						sideVector.Normalize();
						double eyeDistance = params->stereoEyeDistance;
						if (data->stereo.AreSwapped()) eyeDistance *= -1.0;

						if (stereoEye == cStereo::eyeLeft)
						{
							eyePosition =
								startRay
								+ 0.5 * (cameraTarget->GetRightVector() * eyeDistance + sideVector * eyeDistance);
						}
						else
						{
							eyePosition =
								startRay
								- 0.5 * (cameraTarget->GetRightVector() * eyeDistance + sideVector * eyeDistance);
						}
						startRay = eyePosition;
					}
					else
					{
						// For standard stereo, use the stereo engine's eye position calculation.
						// Stereo intensity is reduced near the poles for equirectangular projection
						// to prevent excessive eye separation at the top/bottom of the 360° image.
						double stereoIntensity = (params->perspectiveType == params::perspEquirectangular)
																			 ? 1.0 - pow(imagePoint.y * 2.0, 10.0)
																			 : 1.0;

						startRay = data->stereo.CalcEyePosition(startRay, viewVector, params->topVector,
							params->stereoEyeDistance * stereoIntensity, stereoEye);
						data->stereo.ViewVectorCorrection(params->stereoInfiniteCorrection * stereoIntensity,
							mRot, mRotInv, stereoEye, params->perspectiveType, &viewVector);
					}
				}

				sRGBAFloat resultShader;
				sRGBAFloat objectColour;
				CVector3 normal;
				;

				double opacity = 1.0;
				depth = 1e20;

				// ray-marching loop (reflections)

				if (!hemisphereCut) // in fulldome mode, will not render pixels out of the fulldome
				{
					sRayRecursionIn recursionIn;

					sRayMarchingIn rayMarchingIn;
					CVector3 direction = viewVector;
					direction.Normalize();
					rayMarchingIn.binaryEnable = true;
					rayMarchingIn.direction = direction;
					rayMarchingIn.maxScan = params->viewDistanceMax;
					rayMarchingIn.minScan = 0; // params->viewDistanceMin;
					rayMarchingIn.start = startRay;
					rayMarchingIn.invertMode = false;
					recursionIn.rayMarchingIn = rayMarchingIn;
					recursionIn.calcInside = false;
					recursionIn.resultShader = resultShader;
					recursionIn.objectColour = objectColour;
					recursionIn.rayBranch = rayBranchReflection;

					sRayRecursionInOut recursionInOut;
					sRayMarchingInOut rayMarchingInOut;
					// Pass per-reflection-level buffers for step data.
					// Each reflection level gets its own rayBuffer slot.
					rayMarchingInOut.buffCount = &rayBuffer[0].buffCount;
					rayMarchingInOut.stepBuff = rayBuffer[0].stepBuff.data();
					recursionInOut.rayMarchingInOut = rayMarchingInOut;

					sRayRecursionOut recursionOut = RayRecursion(recursionIn, recursionInOut);

					resultShader = recursionOut.resultShader;
					objectColour = recursionOut.objectColour;
					depth = recursionOut.rayMarchingOut.depth;
					if (!recursionOut.found) depth = 1e20;
					opacity = recursionOut.fogOpacity;
					normal = recursionOut.normal;
					worldPositionRGB.R = recursionOut.rayMarchingOut.point.x;
					worldPositionRGB.G = recursionOut.rayMarchingOut.point.y;
					worldPositionRGB.B = recursionOut.rayMarchingOut.point.z;
					specularFloat.R = recursionOut.specular.R;
					specularFloat.G = recursionOut.specular.G;
					specularFloat.B = recursionOut.specular.B;
					shadowsChannel.R = recursionOut.outShadow.R;
					shadowsChannel.G = recursionOut.outShadow.G;
					shadowsChannel.B = recursionOut.outShadow.B;
					giChannel.R += recursionOut.outGlobalIllumination.R;
					giChannel.G += recursionOut.outGlobalIllumination.G;
					giChannel.B += recursionOut.outGlobalIllumination.B;
				}

				finalPixel.R = resultShader.R;
				finalPixel.G = resultShader.G;
				finalPixel.B = resultShader.B;

				if (params->DOFMonteCarlo && params->DOFMonteCarloChromaticAberration)
				{
					// Apply chromatic aberration: multiply each channel by its hue-based factor.
					finalPixel.R *= rgbFromHsv.R;
					finalPixel.G *= rgbFromHsv.G;
					finalPixel.B *= rgbFromHsv.B;
				}

				if (data->stereo.isEnabled() && data->stereo.GetMode() == cStereo::stereoRedCyan)
				{
					// Accumulate left/right eye colors separately for anaglyph mixing.
					if (stereoEye == cStereo::eyeLeft)
					{
						pixelLeftEye.R += finalPixel.R;
						pixelLeftEye.G += finalPixel.G;
						pixelLeftEye.B += finalPixel.B;
					}
					else if (stereoEye == cStereo::eyeRight)
					{
						pixelRightEye.R += finalPixel.R;
						pixelRightEye.G += finalPixel.G;
						pixelRightEye.B += finalPixel.B;
					}
				}

				alpha = ushort(resultShader.A * 65535);
				opacity16 = ushort(opacity * 65535);

				colour.R = uchar(objectColour.R * 255);
				colour.G = uchar(objectColour.G * 255);
				colour.B = uchar(objectColour.B * 255);

				// Write optional image channels (normals, depth, shadows, etc.).
				// These are stored as separate floating-point textures for post-processing or export.
				if (image->GetImageOptional()->optionalNormal)
				{
					CVector3 normalRotated = mRotInv.RotateVector(normal);
					normalRotated.Normalize();
					normalFloat.R = (1.0 + normalRotated.x) / 2.0;
					normalFloat.G = (1.0 + normalRotated.z) / 2.0;
					normalFloat.B = (1.0 - normalRotated.y) / 2.0; // <-- Also normalized B component.
					// normalFloat.B = 1.0 - normalRotated.y;  // <-- old
				}

				if (image->GetImageOptional()->optionalNormalWorld)
				{
					CVector3 normalNormalized = normal;
					normalNormalized.Normalize();
					normalFloatWorld.R = normalNormalized.x;
					normalFloatWorld.G = normalNormalized.y;
					normalFloatWorld.B = normalNormalized.z;
				}

				// Accumulate samples for AA/MC averaging.
				finalPixelDOF.R += finalPixel.R;
				finalPixelDOF.G += finalPixel.G;
				finalPixelDOF.B += finalPixel.B;
				finalAlphaDOF += alpha;
				finalOpacityDOF += opacity16;
				finalColourDOF.R += colour.R;
				finalColourDOF.G += colour.G;
				finalColourDOF.B += colour.B;

				// Adaptive Monte Carlo: estimate noise (standard deviation) per pixel.
				// If noise drops below the threshold after min samples, stop sampling early.
				if (monteCarlo)
				{
					monteCarloNoise =
						MonteCarloDOFNoiseEstimation(finalPixel, repeat, finalPixelDOF, monteCarloDOFStdDevSum);

					if (repeat > params->DOFMinSamples && monteCarloNoise < params->DOFMaxNoise * 0.01)
					{
						repeats = repeat + 1;
						break;
					}
				}

			} // next repeat

			// Average accumulated samples. For red-cyan anaglyph, mix left/right eye colors first.
			if (monteCarlo || antiAliasing)
			{
				if (data->stereo.isEnabled() && data->stereo.GetMode() == cStereo::stereoRedCyan)
				{
					finalPixel = data->stereo.MixColorsRedCyan(pixelLeftEye, pixelRightEye);
					finalPixel.R = finalPixel.R / repeats * 2.0f;
					finalPixel.G = finalPixel.G / repeats * 2.0f;
					finalPixel.B = finalPixel.B / repeats * 2.0f;
				}
				else
				{
					finalPixel.R = finalPixelDOF.R / repeats;
					finalPixel.G = finalPixelDOF.G / repeats;
					finalPixel.B = finalPixelDOF.B / repeats;
					alpha = ushort(finalAlphaDOF / repeats);
					opacity16 = ushort(finalOpacityDOF / repeats);
					colour.R = uchar(finalColourDOF.R / repeats);
					colour.G = uchar(finalColourDOF.G / repeats);
					colour.B = uchar(finalColourDOF.B / repeats);
					giChannel.R = giChannel.R / repeats;
					giChannel.G = giChannel.G / repeats;
					giChannel.B = giChannel.B / repeats;
				}
				data->statistics.totalNumberOfDOFRepeats += repeats;
				data->statistics.totalNoise += monteCarloNoise;
			}
			else if (data->stereo.isEnabled() && data->stereo.GetMode() == cStereo::stereoRedCyan)
			{
				finalPixel = data->stereo.MixColorsRedCyan(pixelLeftEye, pixelRightEye);
			}

			// Write the final pixel to the image buffer.
			// Progressive rendering: each pixel covers a block of scheduler.GetProgressiveStep()² pixels.
			for (int yy = 0; yy < scheduler->GetProgressiveStep(); ++yy)
			{
				int yyy = screenPoint.y + yy;
				if (yyy < data->screenRegion.y2)
				{
					for (int xx = 0; xx < scheduler->GetProgressiveStep(); ++xx)
					{
						int xxx = screenPoint.x + xx;
						if (xxx < data->screenRegion.x2)
						{
							image->PutPixelImage(xxx, yyy, finalPixel);
							image->PutPixelColor(xxx, yyy, colour);
							image->PutPixelAlpha(xxx, yyy, alpha);
							image->PutPixelZBuffer(xxx, yyy, float(depth));
							image->PutPixelOpacity(xxx, yyy, opacity16);
							if (image->GetImageOptional()->optionalNormal)
								image->PutPixelNormal(xxx, yyy, normalFloat);
							if (image->GetImageOptional()->optionalNormalWorld)
								image->PutPixelNormalWorld(xxx, yyy, normalFloatWorld);
							if (image->GetImageOptional()->optionalSpecular)
								image->PutPixelSpecular(xxx, yyy, specularFloat);
							if (image->GetImageOptional()->optionalWorld)
								image->PutPixelWorld(xxx, yyy, worldPositionRGB);
							if (image->GetImageOptional()->optionalDiffuse)
								image->PutPixelDiffuse(
									xxx, yyy, sRGBFloat(colour.R / 255.0f, colour.G / 255.0f, colour.B / 255.0f));
							if (image->GetImageOptional()->optionalShadows)
								image->PutPixelShadows(xxx, yyy, shadowsChannel);
							if (image->GetImageOptional()->optionalGlobalIlluination)
								image->PutPixelGlobalIllumination(xxx, yyy, giChannel);
						}
					}
				}
			}

			data->statistics.numberOfRenderedPixels++;

		} // next xs
	} // next ys

	// emit signal to main thread when finished
	emit finished();
	return;
}

// Set up the camera coordinate system: rotation matrix (mRot) transforms from
// camera-local space to world space. Also computes the inverse (mRotInv) for
// transforming world-space normals back to camera space for storage.
//
// The viewAngle contains yaw/pitch/roll derived from camera→target direction
// and the top vector. Sweet spot angles shift the center of the image plane
// (used for off-center perspectives).
void cRenderWorker::PrepareMainVectors()
{
	cameraTarget.reset(new cCameraTarget(params->camera, params->target, params->topVector));
	// cameraTarget->SetCameraTargetRotation(params->camera, params->target, params->viewAngle);
	viewAngle = cameraTarget->GetRotation();

	// preparing rotation matrix: build the camera-to-world rotation from Euler angles.
	// Order: Z (yaw) → X (pitch) → Y (roll), matching the camera target computation.
	mRot.RotateZ(viewAngle.x); // yaw
	mRot.RotateX(viewAngle.y); // pitch
	mRot.RotateY(viewAngle.z); // roll

	// preparing base vectors: transform the identity basis vectors by the camera rotation.
	// These are used for stereo eye offset calculations.
	CVector3 vector;
	baseX = mRot.RotateVector(baseX);
	baseY = mRot.RotateVector(baseY);
	baseZ = mRot.RotateVector(baseZ);

	// Apply sweet spot offset to the rotation matrix.
	mRot.RotateZ(-params->sweetSpotHAngle);
	mRot.RotateX(params->sweetSpotVAngle);

	// Inverse rotation: transpose for orthonormal matrices (faster than full inversion).
	mRotInv = mRot.Transpose();
}

// Allocate per-reflection-level buffers for ray marching step data.
// Each reflection level needs its own step buffer because reflections
// perform independent ray marches from the hit point of the previous level.
// The buffer size is maxRaymarchingSteps + 2 (safety margin).
void cRenderWorker::PrepareReflectionBuffer()
{

	reflectionsMax = params->reflectionsMax * 1;
	if (!params->raytracedReflections) reflectionsMax = 0;
	rayBuffer.resize(reflectionsMax + 4);

	for (int i = 0; i < reflectionsMax + 3; i++)
	{
		// rayMarching buffers
		rayBuffer[i].stepBuff.resize(maxRaymarchingSteps + 2);
		rayBuffer[i].buffCount = 0;
	}

	rayStack.resize(reflectionsMax + 1);
}

// Pre-compute ambient occlusion sample directions by sampling the lightmap texture.
// Directions are distributed on a hemisphere using a quasi-uniform angular sampling:
//   - Polar angle b: -49° to +49° (leaving a small gap at the horizon)
//   - Azimuthal angle a: 0 to 360°, with step size adjusted by cos(b) to maintain
//     approximately uniform density on the sphere surface.
//
// Each direction is rotated by mRotAmbientOcclusionLightMapRotation to align with
// the lightmap coordinate system. The lightmap color at the corresponding UV is stored
// as the ambient light color for that direction.
//
// Directions with negligible lightmap color are pruned to reduce the sample count.
// Maximum of 10000 AO directions are stored.
void cRenderWorker::PrepareAOVectors()
{
	AOVectorsAround.resize(10000);
	AOVectorsCount = 0;
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
			d = params->mRotAmbientOcclusionLightMapRotation.RotateVector(d);
			AOVectorsAround[counter].v = d;
			int X = int((a + b) / (2.0 * M_PI) * lightMapWidth + lightMapWidth * 8.5) % lightMapWidth;
			int Y = int(b / (M_PI)*lightMapHeight + lightMapHeight * 8.5) % lightMapHeight;
			sRGBFloat color(data->textures.lightmapTexture.FastPixel(X, Y));
			AOVectorsAround[counter].color = color;

			if (AOVectorsAround[counter].color.R > 0.001 || AOVectorsAround[counter].color.G > 0.001
					|| AOVectorsAround[counter].color.B > 0.001)
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
		AOVectorsAround[0].v.x = 0;
		AOVectorsAround[0].v.y = 0;
		AOVectorsAround[0].v.z = 0;
		AOVectorsAround[0].color = sRGBFloat();
	}
	AOVectorsCount = counter;
}

// Calculate the distance threshold that determines when the raymarcher stops.
// The threshold controls the balance between speed and accuracy:
// - Larger threshold = fewer SDF queries but lower precision
// - Smaller threshold = more SDF queries but higher precision
//
// Three modes:
// 1. iterThreshMode: threshold scales with camera distance (for "stop at maxIter" mode)
// 2. constantDEThreshold: fixed threshold (params->DEThresh)
// 3. Default: threshold scales with camera distance and detailLevel
//
// Advanced quality mode clamps the threshold to [detailSizeMin, detailSizeMax].
// The result is divided by reduceDetail for region-based detail adjustment.
double cRenderWorker::CalcDistThresh(CVector3 point) const
{
	double distThresh;
	if (params->iterThreshMode)
	{
		distThresh = (params->camera - point).Length() * params->resolution * params->fov;
	}
	else
	{
		if (params->constantDEThreshold)
			distThresh = params->DEThresh;
		else
			distThresh =
				(params->camera - point).Length() * params->resolution * params->fov / params->detailLevel;
	}

	if (params->perspectiveType == params::perspEquirectangular) distThresh *= 0.5;

	if (params->advancedQuality)
	{
		if (distThresh > params->detailSizeMax) distThresh = params->detailSizeMax;
		if (distThresh < params->detailSizeMin) distThresh = params->detailSizeMin;
	}

	distThresh /= data->reduceDetail;
	return distThresh;
}

// Calculate the "voxel" size at a given point: the approximate distance between
// adjacent pixels projected onto the scene at the point's distance from the camera.
// This is used by shaders to determine texture sampling step sizes.
double cRenderWorker::CalcDelta(CVector3 point) const
{
	double delta;
	delta = (params->camera - point).Length() * params->resolution * params->fov;
	if (params->perspectiveType == params::perspEquirectangular) delta *= 0.5f;
	return delta;
}

// Adaptive raymarching using SDF (Signed Distance Field) distance estimation.
// Steps along the ray, querying the fractal scene at each position to find
// the closest surface. Uses an adaptive step size based on the estimated
// distance to the nearest object (scaled by a safety factor).
//
// Algorithm:
// 1. Coarse march: step size = (dist - 0.5 * distThresh) * DEFactor
//    The 0.5 * distThresh safety margin prevents overshooting the surface.
//    Random jitter (1.0 - rand/10000) adds anti-aliasing to the step positions.
// 2. Binary search refinement: once the surface is approached (dist < distThresh),
//    halve the step size and search backward to find the precise surface intersection.
// 3. iterThreshMode correction: back off by distThresh to avoid noise from the
//    "stop at maxIter" bailout mode.
//
// The step data (point, distance, distThresh) is stored in the step buffer for
// use by shaders (AO, fog, subsurface scattering, etc.).
void cRenderWorker::RayMarching(
	sRayMarchingIn &in, sRayMarchingInOut *inOut, sRayMarchingOut *out) const
{
	// Initialize point to 1e30 so that the first iteration always advances.
	// 1e30 is specifically chosen because it won't equal any valid scene point,
	// and it enables dead computation detection (point == lastPoint).
	CVector3 point(1e30, 1e30, 1e30); // 1e30 is needed for detection of dead calculation
																		// and camera at (0,0,0)
	bool found = false;
	double scan = in.minScan;
	double dist = 0;
	// search_accuracy and search_limit define the convergence criterion for binary search.
	// The surface is considered found when dist is within search_limit of distThresh.
	double search_accuracy = 0.001 * params->detailLevel;
	double search_limit = 1.0 - search_accuracy;
	int counter = 0;
	double step = 0.0;
	(*inOut->buffCount) = 0;
	double distThresh = 0;
	out->objectId = 0;
	out->transformedPoint = in.start + in.direction * in.minScan;
	out->hasTransformedPoint = false;

	// qDebug() << "Start ************************";

	CVector3 lastPoint;
	bool deadComputationFound = false;

	for (int i = 0; i < params->maxRaymarchingSteps; i++)
	{
		lastPoint = point;

		counter++;

		// Advance along the ray by the current scan distance.
		point = in.start + in.direction * scan;

		// Dead computation detection: if the point hasn't moved (or became NaN),
		// the ray is stuck (e.g., at a fixed point of the fractal formula).
		if (point == lastPoint || point.IsNotANumber()) // detection of dead calculation
		{
			// qWarning() << "Dead computation\n"
			//		<< "Point:" << point.Debug()
			//		<< "\nPrevious point:" << lastPoint.Debug();
			point = lastPoint;
			found = true;
			deadComputationFound = true;
			break;
		}

		distThresh = CalcDistThresh(point);

		sDistanceIn distanceIn(point, distThresh, false);
		sDistanceOut distanceOut;
		dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);

		// Apply per-object detailLevelMultiplier to distThresh dynamically.
		// Objects with detailLevelMultiplier > 1.0 get a larger threshold (coarser),
		// while values < 1.0 get a smaller threshold (finer) for higher detail.
		if (distanceOut.detailLevelMultiplier > 0.0)
		{
			distThresh *= distanceOut.detailLevelMultiplier;
		}

		// qDebug() <<"thresh" <<  distThresh << "dist" << dist << "scan" << scan;
		if (in.invertMode)
		{
			// Invert mode: flip the distance sign for interior rendering.
			dist = distThresh * 1.99 - dist;
			if (dist < 0.0) dist = 0.0;
		}
		out->objectId = distanceOut.objectId;
		out->seqIndex = distanceOut.seqIndex;
		out->transformedPoint = distanceOut.transformedPoint;
		out->hasTransformedPoint = distanceOut.hasTransformedPoint;

		//-------------------- 4.18us for Calculate distance --------------

		// printf("Distance = %g\n", dist/distThresh);
		// Store step data for use by shaders (AO, fog, etc.).
		inOut->stepBuff[i].distance = dist;
		inOut->stepBuff[i].iters = distanceOut.iters;
		inOut->stepBuff[i].distThresh = distThresh;

		data->statistics.histogramIterations.Add(distanceOut.iters);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		// Surface hit: the SDF distance is less than the threshold.
		if (dist < distThresh)
		{
			// Track how often the DE underestimates significantly (potential artifacts).
			if (dist < 0.1 * distThresh) data->statistics.missedDE++;
			found = true;
			break;
		}

		inOut->stepBuff[i].step = step;
		// Compute the next step size. The base formula uses the SDF distance minus
		// a safety margin (0.5 * distThresh for exterior, 0.8 * distThresh for interior).
		// DEFactor scales the step (typically < 1.0 for safety).
		// Random jitter prevents regular patterns in the step sizes.
		if (params->interiorMode)
		{
			step = (dist - 0.8 * distThresh) * params->DEFactor * (1.0 - Random(1000) / 10000.0);
		}
		else
		{
			step = (dist - 0.5 * distThresh) * params->DEFactor * (1.0 - Random(1000) / 10000.0);
		}

		// Apply advanced quality clamps to the step size.
		if (params->advancedQuality)
		{
			if (step > params->absMaxMarchingStep) step = params->absMaxMarchingStep;
			if (step < params->absMinMarchingStep) step = params->absMinMarchingStep;
			if (distThresh > params->absMinMarchingStep)
			{
				if (step > params->relMaxMarchingStep * distThresh)
					step = params->relMaxMarchingStep * distThresh;
			}
			if (step < params->relMinMarchingStep * distThresh)
				step = params->relMinMarchingStep * distThresh;
		}
		else
		{
			if (step > 3.0) step = 3.0;
		}

		inOut->stepBuff[i].point = point;

		(*inOut->buffCount) = i + 1;
		// divided by length of view Vector to eliminate overstepping when fov is big
		scan += step / in.direction.Length();
		if (scan > in.maxScan)
		{
			break;
		}
	}
	//------------- 83.2473 us for RayMarching loop -------------------------

	point = in.start + in.direction * scan;

	// Binary search refinement: once the surface is approached, halve the step size
	// and search backward to find the precise intersection point. This reduces the
	// visual error from the coarse SDF step approximation.
	// qDebug() << "------------ binary search";
	if (found && in.binaryEnable && !deadComputationFound)
	{
		step *= 0.5;
		for (int i = 0; i < 30; i++)
		{
			counter++;
			// Convergence: if dist is within search_limit of distThresh, we're close enough.
			if (dist < distThresh && dist > distThresh * search_limit)
			{
				break;
			}
			else
			{
				if (dist > distThresh)
				{
					// Still outside: move forward by half step.
					scan += step;
					point = in.start + in.direction * scan;
				}
				else if (dist < distThresh * search_limit)
				{
					// Too far inside: move backward by half step.
					scan -= step;
					point = in.start + in.direction * scan;
				}
			}

			distThresh = CalcDistThresh(point);

			sDistanceIn distanceIn(point, distThresh, false);
			sDistanceOut distanceOut;
			dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);

			// Apply per-object detailLevelMultiplier to distThresh dynamically (binary search phase)
			if (distanceOut.detailLevelMultiplier > 0.0)
			{
				distThresh *= distanceOut.detailLevelMultiplier;
			}

			// qDebug() << "i" << i <<"thresh" <<  distThresh << "dist" << dist << "scan" << scan <<
			// "step" << step;

			if (in.invertMode)
			{
				dist = distThresh * 1.99 - dist;
				if (dist < 0.0) dist = 0.0;
			}

			out->objectId = distanceOut.objectId;
			out->seqIndex = distanceOut.seqIndex;
			out->transformedPoint = distanceOut.transformedPoint;
			out->hasTransformedPoint = distanceOut.hasTransformedPoint;

			data->statistics.histogramIterations.Add(distanceOut.iters);
			data->statistics.totalNumberOfIterations += distanceOut.totalIters;

			step *= 0.5;
		}
	}
	// iterThreshMode correction: back off by distThresh to avoid noise from the
	// "stop at maxIter" bailout mode. When maxIter is hit, the distance is set to
	// detailSize (a large value), which can cause the ray to stop too early.
	// Backing off ensures the ray continues past the maxIter point.
	if (params->common.iterThreshMode)
	{
		// this fixes problem with noise when there is used "stop at maxIter" mode
		scan -= distThresh;
		point = in.start + in.direction * scan;
		if (found)
		{
			sDistanceIn distanceIn(point, distThresh, false);
			sDistanceOut distanceOut;
			dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);

			// Apply per-object detailLevelMultiplier to distThresh dynamically (iterThreshMode)
			if (distanceOut.detailLevelMultiplier > 0.0)
			{
				distThresh *= distanceOut.detailLevelMultiplier;
			}

			out->objectId = distanceOut.objectId;
			out->seqIndex = distanceOut.seqIndex;
			out->transformedPoint = distanceOut.transformedPoint;
			out->hasTransformedPoint = distanceOut.hasTransformedPoint;
		}
	}

	//---------- 7.19605us for binary searching ---------------

	data->statistics.histogramStepCount.Add(counter);

	out->found = found;
	out->lastDist = dist;
	out->depth = scan;
	out->distThresh = distThresh;
	out->point = point;
	data->statistics.numberOfRaymarchings++;
}

// Trace a pixel's primary ray and all reflection/refraction bounces.
// Uses an explicit stack (rayStack) instead of function recursion to avoid
// stack overflow with high reflection counts. The stack depth is limited by
// reflectionsMax (from params).
//
// Rendering pipeline per ray level:
// 1. Raymarch to find surface intersection
// 2. Calculate normal (with roughness perturbation and normal map)
// 3. If reflection bounce limit not reached: push reflection ray onto stack
// 4. If refraction bounce limit not reached: push refraction ray onto stack
// 5. Shade the surface (lights, AO, fog, transparency, etc.)
// 6. Pop stack and blend results from child bounces
//
// The rayBranch enum tracks the current processing stage:
//   rayBranchReflection → rayBranchRefraction → rayBranchDone
// This ensures each bounce is processed for reflection first, then refraction.
cRenderWorker::sRayRecursionOut cRenderWorker::RayRecursion(
	sRayRecursionIn in, sRayRecursionInOut &inOut)
{
	// qDebug() << "----------- new pixel ------------";
	// Start at the primary ray level (index 0).
	int rayIndex = 0; // level of recursion

	rayStack[rayIndex].in = in;
	rayStack[rayIndex].rayBranch = rayBranchReflection;
	rayStack[rayIndex].goDeeper = true;

	// Initialize all stack frames.
	for (int i = 0; i < reflectionsMax + 1; i++)
	{
		rayStack[i].rayBranch = rayBranchReflection;
		rayStack[i].reflectShader = sRGBAFloat();
		rayStack[i].transparentShader = sRGBAFloat();
	}

	// Stack-based iteration loop (replaces function recursion).
	// The loop alternates between two phases based on goDeeper:
	//
	//   goDeeper == true  → "PUSH phase": raymarch, shade, and potentially
	//                        push reflection/refraction bounces onto the stack.
	//                        If a bounce is pushed, goDeeper stays true for the
	//                        new level and 'continue' jumps to process it immediately.
	//
	//   goDeeper == false → "POP phase": shade the surface, blend child bounce
	//                        results, and pop the stack (rayIndex--).
	//                        The parent level receives the child's result in
	//                        reflectShader or transparentShader, and in resultShader/objectColour.
	//
	// The rayBranch state machine (Reflection → Refraction → Done) at each level
	// ensures reflection is always processed before refraction, preventing incorrect
	// blending order and infinite loops.
	do
	{
		if (rayStack[rayIndex].goDeeper)
		{
			*inOut.rayMarchingInOut.buffCount = 0;

			// trace the light in given direction
			sRayMarchingOut rayMarchingOut;

			RayMarching(rayStack[rayIndex].in.rayMarchingIn, &inOut.rayMarchingInOut, &rayMarchingOut);
			CVector3 point = rayMarchingOut.point;

			// prepare data for texture shaders
			sShaderInputData shaderInputData;
			shaderInputData.distThresh = rayMarchingOut.distThresh;
			shaderInputData.delta = CalcDelta(point);
			shaderInputData.point = point;
			shaderInputData.transformedPoint = rayMarchingOut.transformedPoint;
			shaderInputData.viewVector = rayStack[rayIndex].in.rayMarchingIn.direction;
			shaderInputData.lastDist = rayMarchingOut.lastDist;
			shaderInputData.depth = rayMarchingOut.depth;
			shaderInputData.stepCount = *inOut.rayMarchingInOut.buffCount;
			shaderInputData.stepBuff = inOut.rayMarchingInOut.stepBuff;
			shaderInputData.invertMode = rayStack[rayIndex].in.calcInside;
			shaderInputData.objectId = rayMarchingOut.objectId;
			shaderInputData.seqIndex = rayMarchingOut.seqIndex;
			shaderInputData.hasTransformedPoint = rayMarchingOut.hasTransformedPoint;

			{
				const int objectId = shaderInputData.objectId;
				shaderInputData.material = nullptr;
				if (objectId >= 0 && objectId < static_cast<int>(data->objectData.size()))
				{
					const int matId = data->objectData[objectId].materialId;
					if (matId >= 0 && matId < static_cast<int>(data->materials.size()))
						shaderInputData.material = &data->materials[matId];
				}
			}

			// If material is null (materialId == -1 or not found), render object as black
			if (!shaderInputData.material)
			{
				rayStack[rayIndex].out.rayMarchingOut = rayMarchingOut;
				rayStack[rayIndex].out.resultShader = sRGBAFloat(0.0f, 0.0f, 0.0f, 1.0f);
				rayStack[rayIndex].out.objectColour = sRGBAFloat(0.0f, 0.0f, 0.0f, 1.0f);
				rayStack[rayIndex].rayBranch = rayBranchDone;
				rayIndex--;
				continue;
			}

			float reflect = shaderInputData.material->reflectance;
			float transparent = shaderInputData.material->transparencyOfSurface;

			rayStack[rayIndex].out.rayMarchingOut = rayMarchingOut;

			CVector3 vn;

			// if found any object
			if (rayMarchingOut.found)
			{
				// calculate normal vector
				vn = CalculateNormals(shaderInputData);

				float roughnessGradient = 1.0;
				if (shaderInputData.material->useColorsFromPalette
						&& shaderInputData.material->roughnessGradientEnable)
				{
					sGradientsCollection gradients;
					SurfaceColour(shaderInputData.point, shaderInputData, &gradients);
					roughnessGradient = gradients.roughness.R;
				}

				float roughnessTex = 1.0;
				if (shaderInputData.material->roughnessTexture.IsLoaded()
						&& shaderInputData.material->useRoughnessTexture)
				{
					float texRoughInt = shaderInputData.material->roughnessTextureIntensity;
					float texRoughIntN = 1.0f - shaderInputData.material->roughnessTextureIntensity;
					roughnessTex = RoughnessTexture(shaderInputData) * texRoughInt + texRoughIntN;
				}

				// Perturb the normal for rough surfaces: add random offsets proportional
				// to the surface roughness. This simulates microfacet scattering.
				if (shaderInputData.material->roughSurface)
				{
					vn.x += roughnessTex * roughnessGradient * shaderInputData.material->surfaceRoughness
									* (Random(20000) / 10000.0f - 1.0f);
					vn.y += roughnessTex * roughnessGradient * shaderInputData.material->surfaceRoughness
									* (Random(20000) / 10000.0f - 1.0f);
					vn.z += roughnessTex * roughnessGradient * shaderInputData.material->surfaceRoughness
									* (Random(20000) / 10000.0f - 1.0f);
					vn.Normalize();
				}
				shaderInputData.normal = vn;

				// Override the computed normal with a normal map if one is loaded.
				if (shaderInputData.material->normalMapTexture.IsLoaded())
				{
					vn = NormalMapShader(shaderInputData);
				}

				float hueEffect = 1.0;
				if (params->DOFMonteCarlo && params->DOFMonteCarloChromaticAberration)
				{
					float aberrationStrength = params->DOFMonteCarloCADispersionGain * 0.01f;
					hueEffect = 1.0f - aberrationStrength + aberrationStrength * actualHue / 180.0f;
				}

				// prepare refraction values for Fresnel and Snell's law.
				// When tracing inside an object, the refractive indices are reversed:
				// n1 = material IOR, n2 = air (1.0). Outside: n1 = air, n2 = material IOR.
				float n1, n2;
				if (rayStack[rayIndex].in.calcInside) // if trace is inside the object
				{
					n1 = shaderInputData.material->transparencyIndexOfRefraction
							 / hueEffect; // reverse refractive indices
					n2 = 1.0f;
				}
				else
				{
					n1 = 1.0f;
					n2 = shaderInputData.material->transparencyIndexOfRefraction / hueEffect;
				}

				rayStack[rayIndex].out.normal = vn;

				// Check if we can still trace more bounces (stack depth limit).
				if (rayIndex < reflectionsMax)
				{
					// === REFLECTION PUSH ===
					// The rayBranch state machine ensures reflection is processed before refraction
					// at each bounce level. This prevents processing refraction before reflection
					// which would cause incorrect blending order.
					if (rayStack[rayIndex].rayBranch == rayBranchReflection)
					{
						// qDebug() << "Reflection" << rayIndex;
						// Transition to refraction state — this bounce level will now process
						// refraction next time goDeeper is true.
						rayStack[rayIndex].rayBranch = rayBranchRefraction;

						// calculate reflection
						if (reflect > 0.0f)
						{
							rayIndex++; // increase recursion level (push onto stack)

							sRayRecursionIn recursionIn;
							sRayMarchingIn rayMarchingIn;
							sRayMarchingInOut rayMarchingInOut;

							// calculate new direction of reflection using the standard reflection formula:
							// R = D - 2(D·N)N where D is the incident direction and N is the normal.
							CVector3 newDirection =
								ReflectionVector(vn, rayStack[rayIndex - 1].in.rayMarchingIn.direction);
							// Offset the new ray origin by distThresh along the reflection direction
							// to prevent self-intersection (shadow acne).
							CVector3 newPoint = point + newDirection * shaderInputData.distThresh;

							// prepare for new recursion
							rayMarchingIn.binaryEnable = true;
							rayMarchingIn.direction = newDirection;
							rayMarchingIn.maxScan = params->viewDistanceMax;
							rayMarchingIn.minScan = 0.0;
							rayMarchingIn.start = newPoint;
							rayMarchingIn.invertMode = false;
							recursionIn.rayMarchingIn = rayMarchingIn;
							recursionIn.calcInside = false;
							// Pass the parent's result shader and object color to the child.
							// The child will blend its result with these values.
							recursionIn.resultShader = rayStack[rayIndex - 1].in.resultShader;
							recursionIn.objectColour = rayStack[rayIndex - 1].in.objectColour;
							recursionIn.rayBranch = rayBranchReflection;

							// setup buffers for ray data

							rayMarchingInOut.buffCount = &rayBuffer[rayIndex].buffCount;
							rayMarchingInOut.stepBuff = rayBuffer[rayIndex].stepBuff.data();
							inOut.rayMarchingInOut = rayMarchingInOut;

							// Push the reflection ray onto the stack and process it immediately (next iteration).
							rayStack[rayIndex].in = recursionIn;
							rayStack[rayIndex].goDeeper = true;
							rayStack[rayIndex].rayBranch = rayBranchReflection;
							continue;
						}
					}

					// === REFRACTION PUSH ===
					if (rayStack[rayIndex].rayBranch == rayBranchRefraction)
					{
						// Transition to done state — after this refraction is processed,
						// no more bounces will be spawned from this level.
						rayStack[rayIndex].rayBranch = rayBranchDone;
						// qDebug() << "Transparency" << rayIndex;
						// calculate refraction (transparency)
						if (transparent > 0.0f)
						{

							rayIndex++; // increase recursion level (push onto stack)

							sRayRecursionIn recursionIn;
							sRayMarchingIn rayMarchingIn;
							sRayMarchingInOut rayMarchingInOut;

							// calculate direction of refracted light using Snell's law.
							CVector3 newDirection =
								RefractVector(vn, rayStack[rayIndex - 1].in.rayMarchingIn.direction, n1, n2);

							// move starting point a little
							CVector3 newPoint = point
																	+ rayStack[rayIndex - 1].in.rayMarchingIn.direction
																			* shaderInputData.distThresh * 1.0;

							// if is total internal reflection the use reflection instead of refraction
							bool internalReflection = false;
							if (newDirection.Length() == 0.0)
							{
								newDirection =
									ReflectionVector(vn, rayStack[rayIndex - 1].in.rayMarchingIn.direction);
								newPoint = point
													 + rayStack[rayIndex - 1].in.rayMarchingIn.direction
															 * shaderInputData.distThresh * 1.0;
								internalReflection = true;
							}

							// preparation for new recursion
							rayMarchingIn.binaryEnable = true;
							rayMarchingIn.direction = newDirection;
							rayMarchingIn.maxScan = params->viewDistanceMax;
							rayMarchingIn.minScan = 0.0;
							rayMarchingIn.start = newPoint;
							// invertMode flips when crossing a boundary (inside→outside or outside→inside).
							// For total internal reflection, the ray stays on the same side, so invertMode
							// follows the parent's state.
							rayMarchingIn.invertMode =
								!rayStack[rayIndex - 1].in.calcInside || internalReflection;
							recursionIn.rayMarchingIn = rayMarchingIn;
							// calcInside tracks whether the ray is currently inside or outside the object.
							// Flipped when crossing a boundary; preserved for total internal reflection.
							recursionIn.calcInside = !rayStack[rayIndex - 1].in.calcInside || internalReflection;
							recursionIn.resultShader = rayStack[rayIndex - 1].in.resultShader;
							recursionIn.objectColour = rayStack[rayIndex - 1].in.objectColour;
							recursionIn.rayBranch = rayBranchRefraction;

							// setup buffers for ray data
							rayMarchingInOut.buffCount = &rayBuffer[rayIndex].buffCount;
							rayMarchingInOut.stepBuff = rayBuffer[rayIndex].stepBuff.data();
							inOut.rayMarchingInOut = rayMarchingInOut;

							// Push the refraction ray onto the stack and process it immediately.
							rayStack[rayIndex].in = recursionIn;
							rayStack[rayIndex].goDeeper = true;
							rayStack[rayIndex].rayBranch = rayBranchReflection;
							continue;
						}
					}

					if (rayStack[rayIndex].rayBranch == rayBranchDone)
					{
						// qDebug() << "Done" << rayIndex;
						rayStack[rayIndex].goDeeper = false;
					}
				} // reflectionsMax
				else
				{
					// At max reflection depth: no more bounces, mark as done.
					rayStack[rayIndex].goDeeper = false;
				}

			} // found
			else
			{
				rayStack[rayIndex].goDeeper = false;
			}
		} // goDeeper

		// === POP PHASE: shade the surface and blend child bounce results ===
		// This phase runs when goDeeper is false, meaning we've finished processing
		// all child bounces (reflection/refraction) for this ray level.
		// The rayBranch state machine ensures each level processes reflection first,
		// then refraction, then is done — preventing infinite loops.
		if (!rayStack[rayIndex].goDeeper)
		{
			// qDebug() << "Shaders" << rayIndex;
			sRayRecursionOut recursionOut;

			recursionOut = rayStack[rayIndex].out;

			sRayMarchingOut rayMarchingOut = recursionOut.rayMarchingOut;

			CVector3 point = rayMarchingOut.point;

			// Retrieve the reflection and refraction shader results from child bounces.
			// These were computed by deeper stack frames and stored before popping.
			sRGBAFloat reflectShader = rayStack[rayIndex].reflectShader;
			sRGBAFloat transparentShader = rayStack[rayIndex].transparentShader;

			inOut.rayMarchingInOut.buffCount = &rayBuffer[rayIndex].buffCount;
			inOut.rayMarchingInOut.stepBuff = rayBuffer[rayIndex].stepBuff.data();

			// prepare data for shaders
			sShaderInputData shaderInputData;
			shaderInputData.distThresh = rayMarchingOut.distThresh;
			shaderInputData.delta = CalcDelta(point);
			shaderInputData.point = point;
			shaderInputData.transformedPoint = rayMarchingOut.transformedPoint;
			shaderInputData.viewVector = rayStack[rayIndex].in.rayMarchingIn.direction;
			shaderInputData.lastDist = rayMarchingOut.lastDist;
			shaderInputData.depth = rayMarchingOut.depth;
			shaderInputData.stepCount = *inOut.rayMarchingInOut.buffCount;
			shaderInputData.stepBuff = inOut.rayMarchingInOut.stepBuff;
			shaderInputData.invertMode = rayStack[rayIndex].in.calcInside;
			shaderInputData.objectId = rayMarchingOut.objectId;
			shaderInputData.seqIndex = rayMarchingOut.seqIndex;
			shaderInputData.hasTransformedPoint = rayMarchingOut.hasTransformedPoint;

			{
				const int objectId = shaderInputData.objectId;
				shaderInputData.material = nullptr;
				if (objectId >= 0 && objectId < static_cast<int>(data->objectData.size()))
				{
					const int matId = data->objectData[objectId].materialId;
					if (matId >= 0 && matId < static_cast<int>(data->materials.size()))
						shaderInputData.material = &data->materials[matId];
				}
			}

			// If material is null (materialId == -1 or not found), render object as black and skip
			if (!shaderInputData.material)
			{
				rayStack[rayIndex].out.rayMarchingOut = rayMarchingOut;
				rayStack[rayIndex].out.resultShader = sRGBAFloat(0.0f, 0.0f, 0.0f, 1.0f);
				rayStack[rayIndex].out.objectColour = sRGBAFloat(0.0f, 0.0f, 0.0f, 1.0f);
				rayStack[rayIndex].rayBranch = rayBranchDone;
				rayIndex--;
				continue;
			}

			shaderInputData.normal = recursionOut.normal;

			// Sample all material texture maps (color, luminosity, diffuse, reflectance,
			// transparency, transparency alpha). Each texture is multiplied by its intensity
			// and blended with (1 - intensity) of the default value.
			if (shaderInputData.material->colorTexture.IsLoaded())
			{
				shaderInputData.texColor =
					TextureShader(shaderInputData, texture::texColor, shaderInputData.material);
			}
			else
				shaderInputData.texColor = sRGBFloat(1.0, 1.0, 1.0);

			if (shaderInputData.material->luminosityTexture.IsLoaded())
				shaderInputData.texLuminosity =
					TextureShader(shaderInputData, texture::texLuminosity, shaderInputData.material);
			else
				shaderInputData.texLuminosity = sRGBFloat(0.0, 0.0, 0.0);

			if (shaderInputData.material->diffusionTexture.IsLoaded())
				shaderInputData.texDiffuse =
					TextureShader(shaderInputData, texture::texDiffuse, shaderInputData.material);
			else
				shaderInputData.texDiffuse = sRGBFloat(1.0, 1.0, 1.0);

			if (shaderInputData.material->reflectanceTexture.IsLoaded())
				shaderInputData.texReflectance =
					TextureShader(shaderInputData, texture::texReflectance, shaderInputData.material);
			else
				shaderInputData.texReflectance = sRGBFloat(1.0, 1.0, 1.0);

			if (shaderInputData.material->transparencyTexture.IsLoaded())
				shaderInputData.texTransparency =
					TextureShader(shaderInputData, texture::texTransparency, shaderInputData.material);
			else
				shaderInputData.texTransparency = sRGBFloat(1.0, 1.0, 1.0);

			if (shaderInputData.material->transparencyAlphaTexture.IsLoaded())
				shaderInputData.texTransparencyAlpha =
					TextureShader(shaderInputData, texture::texTransparencyAlpha, shaderInputData.material);
			else
				shaderInputData.texTransparencyAlpha = sRGBFloat(1.0, 1.0, 1.0);

			float reflect = shaderInputData.material->reflectance;
			float transparent = shaderInputData.material->transparencyOfSurface;

			// Initialize the result shader with the material's interior transparency color.
			// This is the base color seen through the object (used for refraction).
			sRGBAFloat resultShader = rayStack[rayIndex].in.resultShader;
			sRGBAFloat objectColour = rayStack[rayIndex].in.objectColour;
			sRGBAFloat transparentColor = shaderInputData.material->transparencyInteriorColor;
			resultShader.R = transparentColor.R;
			resultShader.G = transparentColor.G;
			resultShader.B = transparentColor.B;

			sRGBAFloat objectShader;
			sRGBAFloat backgroundShader;
			sRGBAFloat volumetricShader;
			sRGBFloat iridescence;
			sRGBFloat luminosityEmissiveOut;

			if (rayMarchingOut.found)
			{
				PerlinNoiseForShaders(&shaderInputData, shaderInputData.point);

				// qDebug() << "Found" << rayIndex;
				// calculate effects for object surface: lighting, AO, shadows, emissive, iridescence.
				sGradientsCollection gradients;

				objectShader = ObjectShader(shaderInputData, &objectColour, &recursionOut.specular,
					&iridescence, &recursionOut.outShadow, &luminosityEmissiveOut, &gradients);

				if (params->DOFMonteCarlo && params->DOFMonteCarloGlobalIllumination)
				{
					// calculate global illumination
					sRGBFloat globalIlumination = GlobalIlumination(shaderInputData, objectColour, false);
					objectShader.R += globalIlumination.R;
					objectShader.G += globalIlumination.G;
					objectShader.B += globalIlumination.B;
					recursionOut.outGlobalIllumination = globalIlumination;
				}

				// calculate reflectance according to Fresnel equations

				// prepare refraction values
				float n1, n2;
				if (rayStack[rayIndex].in.calcInside) // if trace is inside the object
				{
					n1 =
						shaderInputData.material->transparencyIndexOfRefraction; // reverse refractive indices
					n2 = 1.0;
				}
				else
				{
					n1 = 1.0;
					n2 = shaderInputData.material->transparencyIndexOfRefraction;
				}

				float reflectance = 1.0;
				float reflectanceN = 1.0;

				if (shaderInputData.material->fresnelReflectance)
				{
					reflectance = Reflectance(
						shaderInputData.normal, rayStack[rayIndex].in.rayMarchingIn.direction, n1, n2);
					if (reflectance < 0.0f) reflectance = 0.0f;
					if (reflectance > 1.0f) reflectance = 1.0f;
					reflectanceN = 1.0f - reflectance;
				}

				// At the maximum reflection depth, force full transparency (no more bounces).
				if (rayIndex == reflectionsMax)
				{
					reflectance = 0.0;
					reflectanceN = 1.0;
				}

				// combine all results: object color + specular highlight
				resultShader.R = (objectShader.R + recursionOut.specular.R);
				resultShader.G = (objectShader.G + recursionOut.specular.G);
				resultShader.B = (objectShader.B + recursionOut.specular.B);
				resultShader.A = objectShader.A;

				// Apply transparency: start with the material's transparency color,
				// optionally modified by gradients or textures.
				if (shaderInputData.material->useColorsFromPalette
						&& shaderInputData.material->transparencyGradientEnable
						&& !shaderInputData.material->perlinNoiseTransparencyColorEnable)
				{
					transparentShader.R *= gradients.trasparency.R;
					transparentShader.G *= gradients.trasparency.G;
					transparentShader.B *= gradients.trasparency.B;
				}
				else
				{
					transparentShader.R *= shaderInputData.material->transparencyColor.R;
					transparentShader.G *= shaderInputData.material->transparencyColor.G;
					transparentShader.B *= shaderInputData.material->transparencyColor.B;
				}

				if (shaderInputData.material->useTransparencyTexture)
				{
					float texTransInt = shaderInputData.material->transparencyTextureIntensity;
					float texTransIntN = 1.0f - shaderInputData.material->transparencyTextureIntensity;
					transparentShader.R *= shaderInputData.texTransparency.R * texTransInt + texTransIntN;
					transparentShader.G *= shaderInputData.texTransparency.G * texTransInt + texTransIntN;
					transparentShader.B *= shaderInputData.texTransparency.B * texTransInt + texTransIntN;
				}

				// transparency perlin noise
				if (shaderInputData.material->perlinNoiseEnable
						&& shaderInputData.material->perlinNoiseTransparencyColorEnable)
				{
					PerlinNoiseForTransparency(shaderInputData, transparentShader, false);
				}

				// Blend reflection and refraction results using the Fresnel reflectance.
				// The Fresnel equation determines how much light is reflected vs. transmitted
				// at the surface based on the viewing angle and refractive indices.
				if (reflectionsMax > 0)
				{
					// Compute the diffused (scattered) reflection color.
					// This is the reflection that has been modified by the material's diffuse color,
					// reflectance texture, Perlin noise, iridescence, and reflectance gradients.
					sRGBFloat reflectDiffused;
					float diffusionIntensity = shaderInputData.material->diffusionTextureIntensity;
					float diffusionIntensityN = 1.0f - diffusionIntensity;
					reflectDiffused.R = reflect * shaderInputData.texDiffuse.R * diffusionIntensity
															+ reflect * diffusionIntensityN;
					reflectDiffused.G = reflect * shaderInputData.texDiffuse.G * diffusionIntensity
															+ reflect * diffusionIntensityN;
					reflectDiffused.B = reflect * shaderInputData.texDiffuse.B * diffusionIntensity
															+ reflect * diffusionIntensityN;

					if (shaderInputData.material->useColorsFromPalette
							&& shaderInputData.material->diffuseGradientEnable)
					{
						reflectDiffused.R *= gradients.diffuse.R;
						reflectDiffused.G *= gradients.diffuse.G;
						reflectDiffused.B *= gradients.diffuse.B;
					}

					// reflectance texture
					if (shaderInputData.material->useReflectanceTexture)
					{
						float texRefInt = shaderInputData.material->reflectanceTextureIntensity;
						float texRefIntN = 1.0f - shaderInputData.material->reflectanceTextureIntensity;
						reflectDiffused.R *= shaderInputData.texReflectance.R * texRefInt + texRefIntN;
						reflectDiffused.G *= shaderInputData.texReflectance.G * texRefInt + texRefIntN;
						reflectDiffused.B *= shaderInputData.texReflectance.B * texRefInt + texRefIntN;
					}

					// reflectance perlin noise
					if (shaderInputData.material->perlinNoiseEnable
							&& shaderInputData.material->perlinNoiseReflectanceEnable)
					{
						PerlinNoiseForReflectance(shaderInputData, reflectDiffused);
					}

					reflectDiffused.R *= iridescence.R;
					reflectDiffused.G *= iridescence.G;
					reflectDiffused.B *= iridescence.B;

					if (shaderInputData.material->useColorsFromPalette
							&& shaderInputData.material->reflectanceGradientEnable
							&& !shaderInputData.material->perlinNoiseReflectanceEnable)
					{
						reflectDiffused.R *= gradients.reflectance.R;
						reflectDiffused.G *= gradients.reflectance.G;
						reflectDiffused.B *= gradients.reflectance.B;
					}
					else
					{
						reflectDiffused.R *= shaderInputData.material->reflectionsColor.R;
						reflectDiffused.G *= shaderInputData.material->reflectionsColor.G;
						reflectDiffused.B *= shaderInputData.material->reflectionsColor.B;
					}

					if (shaderInputData.material->useTransparencyAlphaTexture)
					{
						transparent = transparent
													* (1.0
														 - shaderInputData.texTransparencyAlpha.R
																 * shaderInputData.material->transparencyAlphaTextureIntensity);
					}

					if (shaderInputData.material->perlinNoiseEnable
							&& shaderInputData.material->perlinNoiseTransparencyAlphaEnable)
					{
						float alpha = (shaderInputData.material->perlinNoiseTransparencyColorInvert)
														? 1.0f - shaderInputData.perlinNoise
														: shaderInputData.perlinNoise;
						alpha = clamp(
							alpha * shaderInputData.material->perlinNoiseTransparencyAlphaIntensity, 0.0f, 1.0f);
						transparent = transparent * (1.0f - alpha);
					}

					// Blend transparency: mix the transparent color with the background result
					// based on the material's transparency amount and Fresnel transmission.
					resultShader.R = transparentShader.R * transparent * reflectanceN
													 + (1.0f - transparent * reflectanceN) * resultShader.R;
					resultShader.G = transparentShader.G * transparent * reflectanceN
													 + (1.0f - transparent * reflectanceN) * resultShader.G;
					resultShader.B = transparentShader.B * transparent * reflectanceN
													 + (1.0f - transparent * reflectanceN) * resultShader.B;

					// Blend reflection: mix the diffused reflection color with the current result
					// based on the Fresnel reflectance. The avg term accounts for color channel
					// imbalance in the reflection.
					float reflectDiffusedAvg =
						(reflectDiffused.R + reflectDiffused.G + reflectDiffused.B) / 3.0f;

					resultShader.R = reflectShader.R * reflectDiffused.R * reflectance
													 + (1.0f - reflectance * reflectDiffusedAvg) * resultShader.R;
					resultShader.G = reflectShader.G * reflectDiffused.G * reflectance
													 + (1.0f - reflectance * reflectDiffusedAvg) * resultShader.G;
					resultShader.B = reflectShader.B * reflectDiffused.B * reflectance
													 + (1.0f - reflectance * reflectDiffusedAvg) * resultShader.B;
				}
				if (resultShader.R < 0.0f) resultShader.R = 0.0f;
				if (resultShader.G < 0.0f) resultShader.G = 0.0f;
				if (resultShader.B < 0.0f) resultShader.B = 0.0f;
			}
			else // if object not found then calculate background
			{
				// qDebug() << "Background";
				backgroundShader = BackgroundShader(shaderInputData);
				resultShader = backgroundShader;
				rayMarchingOut.depth = 1e20;
				recursionOut.specular = sRGBAFloat();
				recursionOut.outShadow = sRGBAFloat(1.0f, 1.0f, 1.0f, 1.0f);
				recursionOut.outGlobalIllumination = sRGBFloat();
				shaderInputData.normal = mRot.RotateVector(CVector3(0.0, -1.0, 0.0));
				// rayStack[rayIndex].goDeeper = false;
			}

			sRGBAFloat opacityOut;

			// Interior absorption: when tracing inside a transparent/translucent object,
			// compute light attenuation as the ray travels through the volume.
			// Uses a Beer-Lambert-like model where opacityCollected accumulates the
			// effective absorption coefficient along the path.
			if (rayStack[rayIndex].in.calcInside) // if the object interior is traced, then the absorption
																						// of light has to be
																						// calculated
			{
				sShaderInputData input2 = shaderInputData;

				double delta = CalcDistThresh(shaderInputData.point);
				double depth = std::min(rayMarchingOut.depth, params->viewDistanceMax);
				double startStep = std::min(std::max(depth / 10.0, delta), delta * 100);
				double endStep = shaderInputData.material->transparencyOfInterior * delta;
				double step = startStep;

				for (double scan = 0; scan < depth; scan += step)
				{

					transparentColor = shaderInputData.material->transparencyInteriorColor;

					CVector3 insidePoint = shaderInputData.point - shaderInputData.viewVector * scan;
					input2.point = insidePoint;

					double opacityGradient = 1.0;

					if (shaderInputData.material->insideColoringEnable)
					{
						sGradientsCollection gradients;
						sRGBAFloat color = SurfaceColour(insidePoint, shaderInputData, &gradients);
						transparentColor.R *= color.R;
						transparentColor.G *= color.G;
						transparentColor.B *= color.B;

						if (shaderInputData.material->diffuseGradientEnable)
							opacityGradient = gradients.diffuse.R;
					}

					if (shaderInputData.material->useTransparencyTexture)
					{
						if (shaderInputData.material->transparencyTexture.IsLoaded())
						{
							sRGBFloat tex =
								TextureShader(input2, texture::texTransparency, shaderInputData.material);
							float intensN = 1.0f - shaderInputData.material->transparencyTextureIntensityVol;
							transparentColor.R *=
								tex.R * shaderInputData.material->transparencyTextureIntensityVol + intensN;
							transparentColor.G *=
								tex.G * shaderInputData.material->transparencyTextureIntensityVol + intensN;
							transparentColor.B *=
								tex.B * shaderInputData.material->transparencyTextureIntensityVol + intensN;
						}
					}

					float texOpacity = 0.0;
					if (shaderInputData.material->useTransparencyAlphaTexture)
					{
						if (shaderInputData.material->transparencyAlphaTexture.IsLoaded())
						{
							sRGBFloat tex =
								TextureShader(input2, texture::texTransparencyAlpha, shaderInputData.material);
							texOpacity =
								(1.0f - tex.R) * shaderInputData.material->transparencyAlphaTextureIntensityVol
								+ 1e-6f;
						}
					}

					PerlinNoiseForShaders(&input2, insidePoint);
					// transparency perlin noise
					if (shaderInputData.material->perlinNoiseEnable
							&& shaderInputData.material->perlinNoiseTransparencyColorEnable)
					{
						PerlinNoiseForTransparency(input2, transparentColor, true);
					}

					float opacityCollected =
						shaderInputData.material->transparencyOfInterior * opacityGradient * (1.0 - texOpacity)
						+ texOpacity;

					if (shaderInputData.material->perlinNoiseEnable
							&& shaderInputData.material->perlinNoiseTransparencyAlphaEnable)
					{
						float alpha = (shaderInputData.material->perlinNoiseTransparencyColorInvert)
														? 1.0f - shaderInputData.perlinNoise
														: input2.perlinNoise;
						alpha =
							clamp(alpha * shaderInputData.material->perlinNoiseTransparencyAlphaIntensityVol,
								0.0f, 1.0f);
						alpha = 1.0f - alpha;
						opacityCollected = opacityCollected * (1.0f - alpha) + alpha + 1e-6f;
					}

					double opacity = (-1.0f + 1.0f / opacityCollected) * float(step);
					if (opacity > 1.0f) opacity = 1.0f;

					sRGBFloat lightColor;

					// Subsurface scattering: for each light, compute the light vector from the
					// inside point toward the light, apply cone/decay, and check shadows.
					// The accumulated lightColor is used to tint the transmitted light.
					if (shaderInputData.material->subsurfaceScattering)
					{
						input2.invertMode = false;

						for (int i = 0; i < data->lights.GetNumberOfLights(); i++)
						{
							const cLight *light = data->lights.GetLight(i);
							if (light->enabled)
							{
								double distanceLight = 0.0;
								CVector3 lightVectorTemp = light->CalculateLightVector(input2.point, input2.delta,
									params->resolution, params->viewDistanceMax, distanceLight);

								float intensity;
								if (light->type == cLight::lightDirectional)
									intensity = light->intensity;
								else if (light->type == cLight::lightConical)
									intensity = 10.0 * light->intensity;
								else
									intensity = 100 * light->intensity / light->Decay(distanceLight) / 6.0;

								sRGBFloat textureColor;
								intensity *= light->CalculateCone(input2.point, lightVectorTemp, textureColor);

								sRGBAFloat lightShadow(1.0, 1.0, 1.0, 1.0);
								if (intensity > 1e-3)
								{
									lightShadow = AuxShadow(input2, light, distanceLight, lightVectorTemp);
								}
								lightColor.R += lightShadow.R * light->color.R * intensity * textureColor.R;
								lightColor.G += lightShadow.G * light->color.G * intensity * textureColor.G;
								lightColor.B += lightShadow.B * light->color.B * intensity * textureColor.B;
							}
						}
					}
					else
					{
						lightColor = sRGBFloat(1.0, 1.0, 1.0);
					}

					// Apply volumetric absorption: mix the light-tinted transparent color with
					// the current result based on the computed opacity for this step.
					resultShader.R =
						opacity * transparentColor.R * lightColor.R + (1.0f - opacity) * resultShader.R;
					resultShader.G =
						opacity * transparentColor.G * lightColor.G + (1.0f - opacity) * resultShader.G;
					resultShader.B =
						opacity * transparentColor.B * lightColor.B + (1.0f - opacity) * resultShader.B;

					// Adaptive step sizing: increase the step as we progress through the volume
					// to speed up convergence, but never go below depth/1000.
					endStep = opacityCollected * CalcDistThresh(shaderInputData.point);
					step = std::max((endStep - startStep) * (scan / depth) + startStep, depth / 1000);
				}
			}
			// Volumetric effects (fog, glow): when the ray is outside any object,
			// compute volumetric scattering along the ray path.
			else // if now is outside the object, then calculate all volumetric effects like fog, glow...
			{
				volumetricShader = VolumetricShader(shaderInputData, resultShader, &opacityOut);
				resultShader = volumetricShader;
			}

			// Store the final result for this ray level.
			recursionOut.point = point;
			recursionOut.rayMarchingOut = rayMarchingOut;
			recursionOut.objectColour = objectColour;
			recursionOut.resultShader = resultShader;
			recursionOut.found = rayMarchingOut.found;
			recursionOut.fogOpacity = opacityOut.R;
			recursionOut.normal = shaderInputData.normal;

			rayStack[rayIndex].out = recursionOut;
			// Propagate results to the parent bounce: store reflection/refraction shader
			// results and pass the current result as the parent's input for the next bounce.
			if (rayIndex > 0)
			{
				if (rayStack[rayIndex].in.rayBranch == rayBranchReflection)
				{
					rayStack[rayIndex - 1].reflectShader = resultShader;
				}

				if (rayStack[rayIndex].in.rayBranch == rayBranchRefraction)
				{
					rayStack[rayIndex - 1].transparentShader = resultShader;
				}
				rayStack[rayIndex - 1].in.resultShader = resultShader;
				rayStack[rayIndex - 1].in.objectColour = objectColour;
			}

			// Pop the stack: move to the parent bounce level.
			rayIndex--;
		}
		// prepare final result

	} while (rayIndex >= 0);

	// Return the result from the primary ray (index 0).
	sRayRecursionOut out = rayStack[0].out;
	return out;
}

// Simulate depth of field by perturbing the ray origin and direction.
// The ray origin is offset on a circle perpendicular to the view direction,
// simulating a point on the camera lens. The view direction is then adjusted
// to aim through that lens point toward the focal plane.
//
// Two modes:
// - perspThreePoint: offset in the camera's local XZ plane (simpler, faster)
// - Other: offset in the plane spanned by (side = view × top, top = side × view)
//
// The radius is scaled by DOFRadius * DOFFocus and uses sqrt() on the random
// value to produce a uniform disk distribution (avoiding concentration at center).
void cRenderWorker::MonteCarloDOF(CVector3 *startRay, CVector3 *viewVector) const
{
	if (params->perspectiveType == params::perspThreePoint)
	{
		double randR = 0.0015 * params->DOFRadius * params->DOFFocus * sqrt(Random(65536) / 65536.0);
		double randAngle = Random(65536);
		CVector3 randVector(randR * sin(randAngle), 0.0, randR * cos(randAngle));
		CVector3 randVectorRot = mRot.RotateVector(randVector);
		CVector3 viewVectorTemp = *viewVector;
		viewVectorTemp -= randVectorRot / params->DOFFocus;
		*viewVector = viewVectorTemp;
		*startRay = params->camera + randVectorRot;
	}
	else
	{
		CVector3 viewVectorTemp = *viewVector;
		double randR = 0.0015 * params->DOFRadius * params->DOFFocus * sqrt(Random(65536) / 65536.0);
		double randAngle = Random(65536);
		CVector3 randVector(randR * sin(randAngle), 0.0, randR * cos(randAngle));

		CVector3 side = viewVectorTemp.Cross(params->topVector);
		side.Normalize();
		CVector3 topTemp = side.Cross(viewVectorTemp);
		topTemp.Normalize();
		CVector3 randVectorRot = side * randVector.x + topTemp * randVector.z;

		viewVectorTemp -= randVectorRot / params->DOFFocus;
		*viewVector = viewVectorTemp;
		*startRay = params->camera + randVectorRot;
	}
}

// Estimate noise in the accumulated pixel samples using the running standard deviation.
// Computes the per-sample standard deviation of RGB values, normalized by the mean
// brightness (coefficient of variation). This normalization makes the noise metric
// perceptually uniform across different brightness levels.
//
// Returns the normalized noise value. If this drops below DOFMaxNoise * 0.01 after
// DOFMinSamples, the Monte Carlo sampling for this pixel stops early (adaptive).
double cRenderWorker::MonteCarloDOFNoiseEstimation(
	sRGBFloat pixel, int repeat, sRGBFloat pixelSum, sRGBFloat &StdDevSum)
{
	sRGBFloat monteCarloDOFAvg;
	monteCarloDOFAvg.R = pixelSum.R / (repeat + 1);
	monteCarloDOFAvg.G = pixelSum.G / (repeat + 1);
	monteCarloDOFAvg.B = pixelSum.B / (repeat + 1);

	sRGBFloat monteCarloDOFSquareDiff;
	monteCarloDOFSquareDiff.R = (pixel.R - monteCarloDOFAvg.R) * (pixel.R - monteCarloDOFAvg.R);
	monteCarloDOFSquareDiff.G = (pixel.G - monteCarloDOFAvg.G) * (pixel.G - monteCarloDOFAvg.G);
	monteCarloDOFSquareDiff.B = (pixel.B - monteCarloDOFAvg.B) * (pixel.B - monteCarloDOFAvg.B);

	StdDevSum.R += monteCarloDOFSquareDiff.R;
	StdDevSum.G += monteCarloDOFSquareDiff.G;
	StdDevSum.B += monteCarloDOFSquareDiff.B;

	sRGBFloat monteCarloDOFStdDev;
	// Compute the standard deviation from the accumulated squared differences.
	double totalStdDev = sqrt((StdDevSum.R + StdDevSum.G + StdDevSum.B) / (repeat + 1.0));

	// Normalize by sqrt(n) to get the standard error of the mean.
	totalStdDev /= sqrt(repeat + 1);

	// Normalize by mean brightness for perceptual uniformity.
	// Brighter pixels can tolerate higher absolute noise.
	double sumBrightness = (pixelSum.R + pixelSum.G + pixelSum.B) / (repeat + 1.0);
	if (sumBrightness > 1.0) totalStdDev /= sumBrightness;

	// noise value
	return totalStdDev;
}
