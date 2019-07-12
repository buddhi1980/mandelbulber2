/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com), Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * class for testing mandelbulber functionality on the cli
 */

#include "test.hpp"

#include "animation_flight.hpp"
#include "animation_frames.hpp"
#include "animation_keyframes.hpp"
#include "cimage.hpp"
#include "files.h"
#include "headless.h"
#include "initparameters.hpp"
#include "interface.hpp"
#include "keyframes.hpp"
#include "netrender.hpp"
#include "opencl_global.h"
#include "opencl_hardware.h"
#include "render_job.hpp"
#include "rendering_configuration.hpp"
#include "settings.hpp"
#include "system.hpp"

QString Test::testFolder()
{
	return systemData.GetDataDirectoryHidden() + ".temporaryTestFolder";
}

void Test::init()
{
	if (QFileInfo::exists(testFolder())) cleanup();
	CreateFolder(testFolder());
}

void Test::cleanup()
{
	QDir(testFolder()).removeRecursively();
}

// start of test cases
void Test::renderExamplesWrapper() const
{
	if (IsBenchmarking())
	{
		QBENCHMARK_ONCE { renderExamples(); }
	}
	else
	{
		renderExamples();
	}
}

void Test::renderExamples() const
{
	// this renders all example files in a resolution of 5x5 px
	// and benchmarks the runtime
	const QString examplePath =
		QDir::toNativeSeparators(systemData.sharedDir + QDir::separator() + "examples");
	QDirIterator it(
		examplePath, QStringList() << "*.fract", QDir::Files, QDirIterator::Subdirectories);

	cParameterContainer *testPar = new cParameterContainer;
	cFractalContainer *testParFractal = new cFractalContainer;
	cAnimationFrames *testAnimFrames = new cAnimationFrames;
	cKeyframes *testKeyframes = new cKeyframes;

	testPar->SetContainerName("main");
	InitParams(testPar);
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, testPar);

	/*******************************************************************/
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		testParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&testParFractal->at(i));
	}
	bool stopRequest = false;
	cImage *image = new cImage(testPar->Get<int>("image_width"), testPar->Get<int>("image_height"));
	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();

#ifdef USE_OPENCL
	if (gPar->Get<bool>("opencl_enabled"))
	{
		// enable full engine if opencl_mode is set to none
		if (gPar->Get<int>("opencl_mode") == 0)
		{
			gPar->Set<int>("opencl_mode", 3);
		}

		// propagate openCL pars
		testPar->Set<bool>("opencl_enabled", gPar->Get<bool>("opencl_enabled"));
		testPar->Set<int>("opencl_mode", gPar->Get<int>("opencl_mode"));
		testPar->Set<int>("opencl_platform", gPar->Get<int>("opencl_platform"));
		testPar->Set<int>("opencl_device_type", gPar->Get<int>("opencl_device_type"));
		testPar->Set<QString>("opencl_device_list", gPar->Get<QString>("opencl_device_list"));

		// log the opencl params
		WriteLogCout(
			QString("opencl_platform  [%1] ").arg(testPar->Get<int>("opencl_platform")) + "\n", 1);

		WriteLogCout(
			QString("opencl_device_list  [%1] ").arg(testPar->Get<QString>("opencl_device_list")) + "\n",
			1);

		WriteLogCout(QString("opencl_mode  [%1] ").arg(testPar->Get<int>("opencl_mode")) + "\n", 1);

		// rev the engine
		qInstallMessageHandler(myMessageOutput);
		gOpenCl->Reset();
		gOpenCl->openClHardware->ListOpenClPlatforms();
		if (testPar->Get<int>("opencl_platform") >= 0)
		{
			gOpenCl->openClHardware->CreateContext(testPar->Get<int>("opencl_platform"),
				cOpenClDevice::enumOpenClDeviceType(testPar->Get<int>("opencl_device_type")));
			gOpenCl->openClHardware->EnableDevicesByHashList(testPar->Get<QString>("opencl_device_list"));
		}
	}
#endif

	int loopRepeats = 1;
#ifdef USE_OPENCL
	if (gPar->Get<bool>("opencl_enabled"))
	{
		loopRepeats = 2;
	}
#endif

	while (it.hasNext())
	{
		const QString filename = it.next();

		for (int repeat = 0; repeat < loopRepeats; repeat++)
		{
			QString cpuGpuString;

			switch (repeat)
			{
				case 0: // CPU mode
				{
#ifdef USE_OPENCL
					testPar->Set("opencl_mode", 0);
#endif
					cpuGpuString = "CPU";
					break;
				}
				case 1: // GPU mode
				{
#ifdef USE_OPENCL
					testPar->Set("opencl_mode", 3);
#endif
					cpuGpuString = "GPU";
					break;
				}
			}

			QElapsedTimer timer;
			timer.start();
			WriteLogCout(cpuGpuString + " trying file: " + filename + "\n", 1);
			cSettings parSettings(cSettings::formatFullText);
			parSettings.BeQuiet(true);
			parSettings.LoadFromFile(filename);
			parSettings.Decode(testPar, testParFractal, testAnimFrames, testKeyframes);

			testPar->Set("image_width", IsBenchmarking() ? difficulty * 2 : 5);
			testPar->Set("image_height", IsBenchmarking() ? difficulty * 2 : 5);
			testPar->Set("DOF_max_noise", testPar->Get<double>("DOF_max_noise") * 10.0);
			testPar->Set("DOF_min_samples", 5);

			double distance = gMainInterface->GetDistanceForPoint(
				testPar->Get<CVector3>("camera"), testPar, testParFractal);

			cRenderJob *renderJob = new cRenderJob(testPar, testParFractal, image, &stopRequest);
			renderJob->Init(cRenderJob::still, config);

			if (IsBenchmarking())
				renderJob->Execute();
			else
				QVERIFY2(renderJob->Execute(), "example render failed.");

			qint64 elapsedTime = timer.elapsed();
			if (exampleOutputPath != "")
			{
				QString imgFileName = QDir(exampleOutputPath).absolutePath() + QDir::separator()
															+ QFileInfo(filename).baseName()
															+ QString(" %1 %2ms").arg(cpuGpuString).arg(elapsedTime);
				if (distance < 1e-5) imgFileName += " LOWDIST";

				SaveImage(imgFileName, ImageFileSave::IMAGE_FILE_TYPE_PNG, image, nullptr);
			}
			delete renderJob;

			WriteLog(
				QString("example: %1 rendered in %2 Milliseconds").arg(filename).arg(elapsedTime), 1);
			testPar->PrintListOfParameters();
		}
	}

	delete image;
	delete testKeyframes;
	delete testAnimFrames;
	delete testParFractal;
	delete testPar;
}

void Test::netrender() const
{
	if (IsBenchmarking()) return; // no reasonable generic network benchmark
	// test connection of server / client over localhost
	CNetRender *netRenderServer = new CNetRender();
	CNetRender *netRenderClient = new CNetRender();
	netRenderServer->SetServer(5555);
	netRenderClient->SetClient("127.0.0.1", 5555);

	QTest::qWait(500);

	netRenderStatus clientStatus = netRenderClient->GetStatus();
	QVERIFY2(
		clientStatus == netRender_READY, QString("client status wrong: should be 'READY' but is '%1'.")
																			 .arg(CNetRender::GetStatusText(clientStatus))
																			 .toStdString()
																			 .c_str());

	QVERIFY2(netRenderServer->GetClientCount() == 1,
		QString("client not connected to server.").toStdString().c_str());

	delete netRenderClient;
	delete netRenderServer;
}

void Test::testFlightWrapper() const
{
	if (IsBenchmarking())
	{
		QBENCHMARK_ONCE { testFlight(); }
	}
	else
	{
		testFlight();
	}
}

void Test::testFlight() const
{
	const QString exampleFlightFile =
		QDir::toNativeSeparators(systemData.sharedDir + QDir::separator() + "examples"
														 + QDir::separator() + "flight_anim_menger sponge_3.fract");

	cParameterContainer *testPar = new cParameterContainer;
	cFractalContainer *testParFractal = new cFractalContainer;
	cAnimationFrames *testAnimFrames = new cAnimationFrames;
	cKeyframes *testKeyframes = new cKeyframes;

	testPar->SetContainerName("main");
	InitParams(testPar);
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, testPar);

	/*******************************************************************/
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		testParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&testParFractal->at(i));
	}
	cImage *image = new cImage(testPar->Get<int>("image_width"), testPar->Get<int>("image_height"));
	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();

	cSettings parSettings(cSettings::formatFullText);
	parSettings.BeQuiet(true);
	parSettings.LoadFromFile(exampleFlightFile);
	parSettings.Decode(testPar, testParFractal, testAnimFrames, testKeyframes);
	testPar->Set("image_width", IsBenchmarking() ? 4 * difficulty : 5);
	testPar->Set("image_height", IsBenchmarking() ? 4 * difficulty : 5);
	testPar->Set("flight_first_to_render", 50);
	testPar->Set("flight_last_to_render", IsBenchmarking() ? 100 : 55);
	testPar->Set("anim_flight_dir", testFolder() + QDir::separator());

	cFlightAnimation *flightAnimation = new cFlightAnimation(
		gMainInterface, testAnimFrames, image, nullptr, testPar, testParFractal, nullptr);
	if (IsBenchmarking())
		flightAnimation->slotRenderFlight();
	else
		QVERIFY2(flightAnimation->slotRenderFlight(), "flight render failed.");

	delete image;
	delete testKeyframes;
	delete testAnimFrames;
	delete testParFractal;
	delete testPar;
	delete flightAnimation;
}

void Test::testKeyframeWrapper() const
{
	if (IsBenchmarking())
	{
		QBENCHMARK_ONCE { testKeyframe(); }
	}
	else
	{
		testKeyframe();
	}
}

void Test::testKeyframe() const
{
	const QString exampleKeyframeFile =
		QDir::toNativeSeparators(systemData.sharedDir + QDir::separator() + "examples"
														 + QDir::separator() + "keyframe_anim_mandelbulb.fract");

	cParameterContainer *testPar = new cParameterContainer;
	cFractalContainer *testParFractal = new cFractalContainer;
	cAnimationFrames *testAnimFrames = new cAnimationFrames;
	cKeyframes *testKeyframes = new cKeyframes;

	testPar->SetContainerName("main");
	InitParams(testPar);
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, testPar);

	/*******************************************************************/
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		testParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&testParFractal->at(i));
	}
	cImage *image = new cImage(testPar->Get<int>("image_width"), testPar->Get<int>("image_height"));
	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();

	cSettings parSettings(cSettings::formatFullText);
	parSettings.BeQuiet(true);
	parSettings.LoadFromFile(exampleKeyframeFile);
	parSettings.Decode(testPar, testParFractal, testAnimFrames, testKeyframes);
	testPar->Set("image_width", IsBenchmarking() ? 4 * difficulty : 5);
	testPar->Set("image_height", IsBenchmarking() ? 4 * difficulty : 5);
	testPar->Set("keyframe_first_to_render", 50);
	testPar->Set("keyframe_last_to_render", IsBenchmarking() ? 100 : 55);
	testPar->Set("anim_keyframe_dir", testFolder() + QDir::separator());

	cKeyframeAnimation *testKeyframeAnimation = new cKeyframeAnimation(
		gMainInterface, testKeyframes, image, nullptr, testPar, testParFractal, nullptr);
	if (IsBenchmarking())
		testKeyframeAnimation->slotRenderKeyframes();
	else
		QVERIFY2(testKeyframeAnimation->slotRenderKeyframes(), "keyframe render failed.");

	delete image;
	delete testKeyframes;
	delete testAnimFrames;
	delete testParFractal;
	delete testPar;
	delete testKeyframeAnimation;
}

void Test::renderSimpleWrapper() const
{
	if (IsBenchmarking())
	{
		QBENCHMARK_ONCE { renderSimple(); }
	}
	else
	{
		renderSimple();
	}
}

void Test::renderSimple() const
{
	// this renders an example file in an "usual" resolution of 100x100 px
	// and benchmarks the runtime
	const QString simpleExampleFileName =
		QDir::toNativeSeparators(systemData.sharedDir + QDir::separator() + "examples"
														 + QDir::separator() + "mandelbox001.fract");

	cParameterContainer *testPar = new cParameterContainer;
	cFractalContainer *testParFractal = new cFractalContainer;
	cAnimationFrames *testAnimFrames = new cAnimationFrames;
	cKeyframes *testKeyframes = new cKeyframes;

	testPar->SetContainerName("main");
	InitParams(testPar);
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, testPar);

	/*******************************************************************/
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		testParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&testParFractal->at(i));
	}
	bool stopRequest = false;
	cImage *image = new cImage(testPar->Get<int>("image_width"), testPar->Get<int>("image_height"));
	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();

	cSettings parSettings(cSettings::formatFullText);
	parSettings.BeQuiet(true);
	parSettings.LoadFromFile(simpleExampleFileName);
	parSettings.Decode(testPar, testParFractal, testAnimFrames, testKeyframes);
	testPar->Set("image_width", IsBenchmarking() ? 20 * difficulty : 100);
	testPar->Set("image_height", IsBenchmarking() ? 20 * difficulty : 100);
	cRenderJob *renderJob = new cRenderJob(testPar, testParFractal, image, &stopRequest);
	renderJob->Init(cRenderJob::still, config);

	if (IsBenchmarking())
		renderJob->Execute();
	else
		QVERIFY2(renderJob->Execute(), "example render failed.");

	delete renderJob;
	delete image;
	delete testKeyframes;
	delete testAnimFrames;
	delete testParFractal;
	delete testPar;
}

void Test::testImageSaveWrapper() const
{
	if (IsBenchmarking())
	{
		QBENCHMARK_ONCE { renderImageSave(); }
	}
	else
	{
		renderImageSave();
	}
}

void Test::renderImageSave() const
{
	// this renders an example file in an "usual" resolution of 100x100 px
	// and benchmarks the runtime, then saves each image type
	const QString simpleExampleFileName =
		QDir::toNativeSeparators(systemData.sharedDir + QDir::separator() + "examples"
														 + QDir::separator() + "mandelbulb001.fract");

	cParameterContainer *testPar = new cParameterContainer;
	cFractalContainer *testParFractal = new cFractalContainer;
	cAnimationFrames *testAnimFrames = new cAnimationFrames;
	cKeyframes *testKeyframes = new cKeyframes;

	testPar->SetContainerName("main");
	InitParams(testPar);
	testPar->Set("normal_enabled", true);
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, testPar);

	/*******************************************************************/
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		testParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&testParFractal->at(i));
	}
	bool stopRequest = false;
	cImage *image = new cImage(testPar->Get<int>("image_width"), testPar->Get<int>("image_height"));
	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();

	cSettings parSettings(cSettings::formatFullText);
	parSettings.BeQuiet(true);
	parSettings.LoadFromFile(simpleExampleFileName);
	parSettings.Decode(testPar, testParFractal, testAnimFrames, testKeyframes);
	testPar->Set("image_width", IsBenchmarking() ? 20 * difficulty : 100);
	testPar->Set("image_height", IsBenchmarking() ? 20 * difficulty : 100);

	cRenderJob *renderJob = new cRenderJob(testPar, testParFractal, image, &stopRequest);
	renderJob->Init(cRenderJob::still, config);

	if (IsBenchmarking())
		renderJob->Execute();
	else
		QVERIFY2(renderJob->Execute(), "example render failed.");

	QList<ImageFileSave::enumImageFileType> fileTypes = {
		ImageFileSave::IMAGE_FILE_TYPE_PNG, ImageFileSave::IMAGE_FILE_TYPE_JPG};
#ifdef USE_TIFF
	fileTypes.append(ImageFileSave::IMAGE_FILE_TYPE_TIFF);
#endif /* USE_TIFF */
#ifdef USE_EXR
	fileTypes.append(ImageFileSave::IMAGE_FILE_TYPE_EXR);
#endif /* USE_EXR */

	QStringList imageChannelNames = ImageFileSave::ImageChannelNames();
	for (auto fileType : fileTypes)
	{
		for (int appendAlphaIndex = 0; appendAlphaIndex < 2; appendAlphaIndex++)
		{
			bool isPngOrTiff = fileType == ImageFileSave::IMAGE_FILE_TYPE_PNG;
#ifdef USE_EXR
			isPngOrTiff = isPngOrTiff || fileType == ImageFileSave::IMAGE_FILE_TYPE_TIFF;
#endif																										 /* USE_EXR */
			if (appendAlphaIndex == 1 && !isPngOrTiff) continue; // ignore this test
			gPar->Set("append_alpha_png", appendAlphaIndex == 1 ? true : false);

			int combinationContentTypes = 2 * 2 * 2 * 2; // foreach content type enabled or not flags
			int combinationQualityTypes =
				3 * 3 * 3 * 3; // foreach content type the possible quality types (8, 16, 32bit)

			for (int c = 0; c < combinationContentTypes * combinationQualityTypes; c++)
			{
				int contentTypeIndex = c / combinationQualityTypes;
				int qualityTypeIndex = c % combinationContentTypes;
				bool skipPermutation = false;
				bool anyEnabled = false;
				QString filenameChannels = "";
				for (int content = 0; content < imageChannelNames.size(); content++)
				{
					QString imageChannelName = imageChannelNames[content];
					bool enabled = (1 << content) & contentTypeIndex;
					int qualityType = (qualityTypeIndex / (int)pow(3, content)) % 3;
					ImageFileSave::enumImageChannelQualityType qualityTypeValue =
						qualityType == 0 ? ImageFileSave::IMAGE_CHANNEL_QUALITY_8
														 : (qualityType == 1 ? ImageFileSave::IMAGE_CHANNEL_QUALITY_16
																								 : ImageFileSave::IMAGE_CHANNEL_QUALITY_32);
					QString qualityString = qualityType == 0 ? "8" : (qualityType == 1 ? "16" : "32");

					if (!enabled && qualityType != 0)
					{
						skipPermutation = true;
						break;
					}
					if (enabled)
					{
						if ((imageChannelName == "zbuffer" && fileType == ImageFileSave::IMAGE_FILE_TYPE_JPG)
								|| (qualityTypeValue == ImageFileSave::IMAGE_CHANNEL_QUALITY_8
										 && fileType == ImageFileSave::IMAGE_FILE_TYPE_EXR)
								|| (qualityTypeValue == ImageFileSave::IMAGE_CHANNEL_QUALITY_32
										 && fileType == ImageFileSave::IMAGE_FILE_TYPE_PNG)
								|| (qualityTypeValue != ImageFileSave::IMAGE_CHANNEL_QUALITY_8
										 && fileType == ImageFileSave::IMAGE_FILE_TYPE_JPG))
						{
							// jpg cannot save zbuffer, jpg can only save 8bit
							// exr cannot save 8 bit
							// png cannot save 32 bit
							skipPermutation = true;
							break;
						}
					}

					if (enabled) filenameChannels += imageChannelName + "-" + qualityString + " ";
					anyEnabled = anyEnabled || enabled;
					gPar->Set(imageChannelName + "_enabled", enabled);
					gPar->Set(imageChannelName + "_quality", (int)qualityTypeValue);
				}
				if (skipPermutation) continue;
				if (!anyEnabled) continue; // no image channels selected

				QString folderName = testFolder() + QDir::separator();
				folderName += ImageFileSave::ImageFileExtension(fileType) + " ";
				folderName += (appendAlphaIndex ? QString("[+alpha] ") : QString(""));
				folderName += filenameChannels;
				CreateFolder(folderName);
				SaveImage(folderName + QDir::separator() + "out", fileType, image, nullptr);
			}
		}
	}
	delete renderJob;
	delete image;
	delete testKeyframes;
	delete testAnimFrames;
	delete testParFractal;
	delete testPar;
}
