/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <memory>

#include "animation_flight.hpp"
#include "animation_frames.hpp"
#include "animation_keyframes.hpp"
#include "cimage.hpp"
#include "files.h"
#include "headless.h"
#include "initparameters.hpp"
#include "interface.hpp"
#include "keyframes.hpp"
#include "image_comparison.hpp"
#include "netrender.hpp"
#include "netrender_file_receiver.hpp"
#include "opencl_global.h"
#include "opencl_hardware.h"
#include "regression_test.hpp"
#include "render_job.hpp"
#include "rendering_configuration.hpp"
#include "settings.hpp"
#include "system.hpp"
#include "system_directories.hpp"
#include "write_log.hpp"

QString Test::testFolder()
{
	return systemDirectories.GetDataDirectoryHidden() + ".temporaryTestFolder";
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
		QBENCHMARK_ONCE
		{
			renderExamples();
		}
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
		QDir::toNativeSeparators(systemDirectories.sharedDir + QDir::separator() + "examples");
	QDirIterator it(
		examplePath, QStringList() << "*.fract", QDir::Files, QDirIterator::Subdirectories);

	std::shared_ptr<cParameterContainer> testPar(new cParameterContainer());
	std::shared_ptr<cFractalContainer> testParFractal(new cFractalContainer());
	std::shared_ptr<cAnimationFrames> testAnimFrames(new cAnimationFrames());
	std::shared_ptr<cKeyframes> testKeyframes(new cKeyframes());

	testPar->SetContainerName("main");
	InitParams(testPar);
	InitNodeParams(1, testPar);
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, testPar);

	/*******************************************************************/
	for (int i = 0; i < testParFractal->size(); i++)
	{
		testParFractal->at(i)->SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(testParFractal->at(i));
	}
	bool stopRequest = false;
	std::shared_ptr<cImage> image(
		new cImage(testPar->Get<int>("image_width"), testPar->Get<int>("image_height")));
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
			cOpenClDevice::enumOpenClDeviceType deviceType =
				cOpenClDevice::enumOpenClDeviceType(gPar->Get<int>("opencl_device_type"));

			gOpenCl->openClHardware->ListOpenClDevices(testPar->Get<int>("opencl_platform"), deviceType);
			gOpenCl->openClHardware->EnableDevicesByHashList(gPar->Get<QString>("opencl_device_list"));

			gOpenCl->openClHardware->CreateAllContexts(testPar->Get<int>("opencl_platform"), deviceType);
		}
	}
#endif

	int loopRepeats = 1;
#ifdef USE_OPENCL
	if (gPar->Get<bool>("opencl_enabled"))
	{
		loopRepeats = 3;
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
#ifdef USE_OPENCL
				case 0: // GPU mode
				{
					testPar->Set("opencl_mode", 3);
					cpuGpuString = "GPU1";
					break;
				}
				case 1: // GPU mode
				{
					testPar->Set("opencl_mode", 3);
					cpuGpuString = "GPU2";
					break;
				}
				case 2: // CPU mode
				{
					testPar->Set("opencl_mode", 0);
					cpuGpuString = "CPU";
					break;
				}
				default: break;
#else
				case 0: // CPU mode
				{
					cpuGpuString = "CPU";
					break;
				}
				default: break;
#endif
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

			std::unique_ptr<cRenderJob> renderJob(
				new cRenderJob(testPar, testParFractal, image, 1, &stopRequest));
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

			WriteLog(
				QString("example: %1 rendered in %2 Milliseconds").arg(filename).arg(elapsedTime), 1);
			testPar->PrintListOfParameters();
		}
	}
}

void Test::netrender()
{
	if (IsBenchmarking()) return; // no reasonable generic network benchmark
	// test connection of server / client over localhost
	std::unique_ptr<cNetRender> netRenderServer(new cNetRender(this));
	std::unique_ptr<cNetRender> netRenderClient(new cNetRender(this));
	netRenderServer->SetServer(5555);
	netRenderClient->SetClient("127.0.0.1", 5555);

	QTest::qWait(500);

	netRenderStatus clientStatus = netRenderClient->GetStatus();
	QVERIFY2(clientStatus == netRenderSts_READY,
		QString("client status wrong: should be 'READY' but is '%1'.")
			.arg(cNetRender::GetStatusText(clientStatus))
			.toStdString()
			.c_str());

	QVERIFY2(netRenderServer->GetClientCount() == 1,
		QString("client not connected to server.").toStdString().c_str());
}

void Test::netRenderFileDestination() const
{
	if (IsBenchmarking()) return;
	// the frames which NetRender clients send to the server are saved in the output folder of the
	// animation being rendered, also for a flight animation (issue #755)
	const QString keyframeDir =
		testFolder() + QDir::separator() + "netrenderKeyframes" + QDir::separator();
	const QString flightDir =
		testFolder() + QDir::separator() + "netrenderFlight" + QDir::separator();
	CreateFolder(keyframeDir);
	CreateFolder(flightDir);
	const QString oldKeyframeDir = gPar->Get<QString>("anim_keyframe_dir");
	const QString oldFlightDir = gPar->Get<QString>("anim_flight_dir");
	gPar->Set("anim_keyframe_dir", keyframeDir);
	gPar->Set("anim_flight_dir", flightDir);

	const QByteArray data("frame data");
	cNetRenderFileReceiver receiver;
	auto receive = [&receiver, &data](int clientIndex, const QString &name)
	{
		receiver.ReceiveHeader(clientIndex, data.size(), name);
		receiver.ReceiveChunk(clientIndex, 1, data);
	};

	receive(0, "1_keyframe00001.png"); // keyframe animation is the default
	receiver.SetDestinationDirParameter("anim_flight_dir");
	receive(0, "1_flight00001.png");
	receive(1, "DIR[alpha]2_flight00002.png"); // optional image channel in a subfolder

	gPar->Set("anim_keyframe_dir", oldKeyframeDir);
	gPar->Set("anim_flight_dir", oldFlightDir);

	QVERIFY(QFile::exists(keyframeDir + "keyframe00001.png"));
	QVERIFY(QFile::exists(flightDir + "flight00001.png"));
	QVERIFY(!QFile::exists(keyframeDir + "flight00001.png"));
	QVERIFY(QFile::exists(flightDir + "alpha" + QDir::separator() + "flight00002.png"));
	QVERIFY(!receiver.IsReceiving());
}

void Test::testFlightWrapper() const
{
	if (IsBenchmarking())
	{
		QBENCHMARK_ONCE
		{
			testFlight();
		}
	}
	else
	{
		testFlight();
	}
}

void Test::testFlight() const
{
	const QString exampleFlightFile =
		QDir::toNativeSeparators(systemDirectories.sharedDir + QDir::separator() + "examples"
														 + QDir::separator() + "flight_anim_menger sponge_3.fract");

	std::shared_ptr<cParameterContainer> testPar(new cParameterContainer());
	std::shared_ptr<cFractalContainer> testParFractal(new cFractalContainer());
	std::shared_ptr<cAnimationFrames> testAnimFrames(new cAnimationFrames());
	std::shared_ptr<cKeyframes> testKeyframes(new cKeyframes());

	testPar->SetContainerName("main");
	InitParams(testPar);
	InitNodeParams(1, testPar);
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, testPar);

	/*******************************************************************/
	for (int i = 0; i < testParFractal->size(); i++)
	{
		testParFractal->at(i)->SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(testParFractal->at(i));
	}
	std::shared_ptr<cImage> image(
		new cImage(testPar->Get<int>("image_width"), testPar->Get<int>("image_height")));
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

	std::unique_ptr<cFlightAnimation> flightAnimation(new cFlightAnimation(
		gMainInterface, testAnimFrames, image, nullptr, testPar, testParFractal, nullptr));
	if (IsBenchmarking())
		flightAnimation->slotRenderFlight();
	else
		QVERIFY2(flightAnimation->slotRenderFlight(), "flight render failed.");
}

void Test::testKeyframeWrapper() const
{
	if (IsBenchmarking())
	{
		QBENCHMARK_ONCE
		{
			testKeyframe();
		}
	}
	else
	{
		testKeyframe();
	}
}

void Test::testKeyframe() const
{
	const QString exampleKeyframeFile =
		QDir::toNativeSeparators(systemDirectories.sharedDir + QDir::separator() + "examples"
														 + QDir::separator() + "keyframe_anim_mandelbulb.fract");

	std::shared_ptr<cParameterContainer> testPar(new cParameterContainer);
	std::shared_ptr<cFractalContainer> testParFractal(new cFractalContainer);
	std::shared_ptr<cAnimationFrames> testAnimFrames(new cAnimationFrames());
	std::shared_ptr<cKeyframes> testKeyframes(new cKeyframes());

	testPar->SetContainerName("main");
	InitParams(testPar);
	InitNodeParams(1, testPar);
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, testPar);

	/*******************************************************************/
	for (int i = 0; i < testParFractal->size(); i++)
	{
		testParFractal->at(i)->SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(testParFractal->at(i));
	}
	std::shared_ptr<cImage> image(
		new cImage(testPar->Get<int>("image_width"), testPar->Get<int>("image_height")));
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

	std::unique_ptr<cKeyframeAnimation> testKeyframeAnimation(new cKeyframeAnimation(
		gMainInterface, testKeyframes, image, nullptr, testPar, testParFractal, nullptr));
	if (IsBenchmarking())
		testKeyframeAnimation->slotRenderKeyframes();
	else
		QVERIFY2(testKeyframeAnimation->slotRenderKeyframes(), "keyframe render failed.");
}

void Test::renderSimpleWrapper() const
{
	if (IsBenchmarking())
	{
		QBENCHMARK_ONCE
		{
			renderSimple();
		}
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
		QDir::toNativeSeparators(systemDirectories.sharedDir + QDir::separator() + "examples"
														 + QDir::separator() + "mandelbox001.fract");

	std::shared_ptr<cParameterContainer> testPar(new cParameterContainer());
	std::shared_ptr<cFractalContainer> testParFractal(new cFractalContainer());
	std::shared_ptr<cAnimationFrames> testAnimFrames(new cAnimationFrames());
	std::shared_ptr<cKeyframes> testKeyframes(new cKeyframes());

	testPar->SetContainerName("main");
	InitParams(testPar);
	InitNodeParams(1, testPar);
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, testPar);

	/*******************************************************************/
	for (int i = 0; i < testParFractal->size(); i++)
	{
		testParFractal->at(i)->SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(testParFractal->at(i));
	}
	bool stopRequest = false;
	std::shared_ptr<cImage> image(
		new cImage(testPar->Get<int>("image_width"), testPar->Get<int>("image_height")));
	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();

	cSettings parSettings(cSettings::formatFullText);
	parSettings.BeQuiet(true);
	parSettings.LoadFromFile(simpleExampleFileName);
	parSettings.Decode(testPar, testParFractal, testAnimFrames, testKeyframes);
	testPar->Set("image_width", IsBenchmarking() ? 20 * difficulty : 100);
	testPar->Set("image_height", IsBenchmarking() ? 20 * difficulty : 100);
	std::unique_ptr<cRenderJob> renderJob(
		new cRenderJob(testPar, testParFractal, image, 1, &stopRequest));
	renderJob->Init(cRenderJob::still, config);

	if (IsBenchmarking())
		renderJob->Execute();
	else
		QVERIFY2(renderJob->Execute(), "example render failed.");
}

void Test::testImageSaveWrapper() const
{
	if (IsBenchmarking())
	{
		QBENCHMARK_ONCE
		{
			renderImageSave();
		}
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
		QDir::toNativeSeparators(systemDirectories.sharedDir + QDir::separator() + "examples"
														 + QDir::separator() + "mandelbulb001.fract");

	std::shared_ptr<cParameterContainer> testPar(new cParameterContainer());
	std::shared_ptr<cFractalContainer> testParFractal(new cFractalContainer());
	std::shared_ptr<cAnimationFrames> testAnimFrames(new cAnimationFrames());
	std::shared_ptr<cKeyframes> testKeyframes(new cKeyframes());

	testPar->SetContainerName("main");
	InitParams(testPar);
	testPar->Set("normal_enabled", true);
	/****************** TEMPORARY CODE FOR MATERIALS *******************/

	InitMaterialParams(1, testPar);

	/*******************************************************************/
	for (int i = 0; i < testParFractal->size(); i++)
	{
		testParFractal->at(i)->SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(testParFractal->at(i));
	}
	bool stopRequest = false;
	std::shared_ptr<cImage> image(
		new cImage(testPar->Get<int>("image_width"), testPar->Get<int>("image_height")));
	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();

	cSettings parSettings(cSettings::formatFullText);
	parSettings.BeQuiet(true);
	parSettings.LoadFromFile(simpleExampleFileName);
	parSettings.Decode(testPar, testParFractal, testAnimFrames, testKeyframes);
	testPar->Set("image_width", IsBenchmarking() ? 20 * difficulty : 100);
	testPar->Set("image_height", IsBenchmarking() ? 20 * difficulty : 100);

	std::unique_ptr<cRenderJob> renderJob(
		new cRenderJob(testPar, testParFractal, image, 1, &stopRequest));
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
#ifdef USE_JXL
	fileTypes.append(ImageFileSave::IMAGE_FILE_TYPE_JXL);
#endif /* USE_JXL */
	fileTypes.append(ImageFileSave::IMAGE_FILE_TYPE_HDR);

	QStringList imageChannelNames = ImageFileSave::ImageChannelNames();
	for (auto fileType : fileTypes)
	{
		for (int appendAlphaIndex = 0; appendAlphaIndex < 2; appendAlphaIndex++)
		{
			bool isPngOrTiff = fileType == ImageFileSave::IMAGE_FILE_TYPE_PNG;
#ifdef USE_EXR
			isPngOrTiff = isPngOrTiff || fileType == ImageFileSave::IMAGE_FILE_TYPE_TIFF;
#endif /* USE_EXR */
#ifdef USE_JXL
			isPngOrTiff = isPngOrTiff || fileType == ImageFileSave::IMAGE_FILE_TYPE_JXL;
#endif																										 /* USE_JXL */
			if (appendAlphaIndex == 1 && !isPngOrTiff) continue; // ignore this test
			gPar->Set("append_alpha_png", appendAlphaIndex == 1 ? true : false);
			gPar->Set("append_alpha_jxl", appendAlphaIndex == 1 ? true : false);

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
}

void Test::regressionScenesWrapper() const
{
	if (IsBenchmarking()) return; // comparing renders is not a benchmark
	regressionScenes();
}

void Test::regressionScenes() const
{
	// this renders the regression scenes and checks that every image is a valid render.
	// To compare them with the renders of another build use --regression-render
	// and --regression-compare
	const QList<sRegressionScene> scenes = cRegressionTest::CoreScenes();
	for (const sRegressionScene &scene : scenes)
	{
		WriteLogCout("regression scene: " + scene.name + "\n", 1);
		QImage image;
		QString error;
		const bool rendered = cRegressionTest::RenderScene(scene, &image, &error);
		QVERIFY2(rendered, qPrintable(QString("regression scene %1: %2").arg(scene.name, error)));

		const sImageValidity validity = imageComparison::Validate(image, scene.fillsFrame);
		QVERIFY2(validity.valid,
			qPrintable(
				QString("regression scene %1 is not a valid render: %2").arg(scene.name, validity.reason)));

		image.save(testFolder() + QDir::separator() + scene.name + ".png", "PNG");
	}
}

void Test::netRenderCacheCleanup() const
{
	// NetRender cache files not used for more than one hour are deleted (issue #766)
	QString dir = testFolder() + QDir::separator() + "netrender_cache_cleanup";
	QDir(dir).removeRecursively();
	QVERIFY(QDir().mkpath(dir));
	QString oldFile = dir + QDir::separator() + "old.png";
	QString newFile = dir + QDir::separator() + "new.png";
	QString reusedFile = dir + QDir::separator() + "reused.png";
	for (const QString &name : {oldFile, newFile, reusedFile})
	{
		QFile file(name);
		QVERIFY(file.open(QIODevice::WriteOnly));
		file.write("data");
		file.close();
	}
	QDateTime twoHoursAgo = QDateTime::currentDateTime().addSecs(-7200);
	for (const QString &name : {oldFile, reusedFile})
	{
		QFile file(name);
		QVERIFY(file.open(QIODevice::ReadWrite));
		QVERIFY(file.setFileTime(twoHoursAgo, QFileDevice::FileModificationTime));
		file.close();
	}

	// reusing a cached file marks it as recently used
	MarkFileAsUsed(reusedFile);
	QVERIFY(QFileInfo(reusedFile).lastModified() > QDateTime::currentDateTime().addSecs(-60));

	QCOMPARE(DeleteFilesNotUsedFor(dir, 3600), 1);
	QVERIFY(!QFile::exists(oldFile));
	QVERIFY(QFile::exists(newFile));
	QVERIFY(QFile::exists(reusedFile));
	QCOMPARE(DeleteFilesNotUsedFor(dir, 3600), 0);
	QDir(dir).removeRecursively();
}

void Test::animatedFileName() const
{
	// an animated texture (file name with %) loops over the sequence of files which exists,
	// also when the numbers don't start at 0 (issue #830)
	const QString folder = testFolder() + QDir::separator() + "animatedTexture";
	CreateFolder(folder);
	for (int i = 1; i <= 3; i++)
	{
		QFile file(folder + QDir::separator() + QString("tex%1.png").arg(i, 4, 10, QChar('0')));
		QVERIFY(file.open(QIODevice::WriteOnly));
	}
	// a file which matches the pattern but has no number
	QFile otherFile(folder + QDir::separator() + "texabcd.png");
	QVERIFY(otherFile.open(QIODevice::WriteOnly));
	otherFile.close();

	const QString pattern = folder + QDir::separator() + "tex%%%%.png";
	const int expected[] = {3, 1, 2, 3, 1, 2, 3, 1};
	for (int frame = 0; frame < 8; frame++)
	{
		const QString expectedName =
			folder + QDir::separator() + QString("tex%1.png").arg(expected[frame], 4, 10, QChar('0'));
		QCOMPARE(AnimatedFileName(pattern, frame), expectedName);
	}

	// a sequence which starts at 0 keeps its numbering
	QFile zeroFile(folder + QDir::separator() + "tex0000.png");
	QVERIFY(zeroFile.open(QIODevice::WriteOnly));
	zeroFile.close();
	QCOMPARE(AnimatedFileName(pattern, 0), folder + QDir::separator() + "tex0000.png");
	QCOMPARE(AnimatedFileName(pattern, 5), folder + QDir::separator() + "tex0001.png");

	// a sequence which doesn't exist keeps the frame number
	QCOMPARE(AnimatedFileName(folder + QDir::separator() + "none%%.png", 7),
		folder + QDir::separator() + "none07.png");
}

void Test::ensureFileExtension() const
{
	// the mesh export adds .ply when the file name has no such extension (issue #906)
	QCOMPARE(EnsureFileExtension("mesh", "ply"), QString("mesh.ply"));
	QCOMPARE(EnsureFileExtension("mesh.ply", "ply"), QString("mesh.ply"));
	QCOMPARE(EnsureFileExtension("mesh.PLY", "ply"), QString("mesh.PLY"));
	QCOMPARE(EnsureFileExtension("mesh.", "ply"), QString("mesh.ply"));
	QCOMPARE(EnsureFileExtension("mesh.v2", "ply"), QString("mesh.v2.ply"));
	QCOMPARE(EnsureFileExtension("my.ply.d/mesh", "ply"), QString("my.ply.d/mesh.ply"));
	QCOMPARE(EnsureFileExtension("mesh_ply", "ply"), QString("mesh_ply.ply"));
	QCOMPARE(EnsureFileExtension("", "ply"), QString(""));
}

void Test::animatedTextureFrame() const
{
	// speed of animated textures (issue #704): 1.0 keeps the animation frame number
	for (int frame = 0; frame < 1000; frame++)
		QCOMPARE(AnimatedTextureFrame(frame, 1.0), frame);

	const int half[] = {0, 0, 1, 1, 2, 2, 3, 3};
	for (int frame = 0; frame < 8; frame++)
		QCOMPARE(AnimatedTextureFrame(frame, 0.5), half[frame]);

	QCOMPARE(AnimatedTextureFrame(3, 2.0), 6);
	QCOMPARE(AnimatedTextureFrame(5, 0.0), 0);
	QCOMPARE(AnimatedTextureFrame(100, 0.29), 29);
	QCOMPARE(AnimatedTextureFrame(10, 0.1), 1);
	QCOMPARE(AnimatedTextureFrame(9, 0.1), 0);
}
