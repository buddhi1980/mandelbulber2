/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include "headless.h"
#include "initparameters.hpp"
#include "keyframes.hpp"
#include "netrender.hpp"
#include "render_job.hpp"
#include "settings.hpp"
#include "interface.hpp"
#include "rendering_configuration.hpp"
#include "system.hpp"

QString Test::testFolder()
{
	return systemData.GetDataDirectoryHidden() + ".temporaryTestFolder";
}

void Test::init()
{
	if (QFileInfo(testFolder()).exists()) cleanup();
	CreateFolder(testFolder());
}

void Test::cleanup()
{
	DeleteAllFilesFromDirectory(testFolder(), "*");
	QDir().rmdir(testFolder());
}

// start of test cases
void Test::renderExamplesWrapper()
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

void Test::renderExamples()
{
	// this renders all example files in a resolution of 5x5 px
	// and benchmarks the runtime
	QString examplePath =
		QDir::toNativeSeparators(systemData.sharedDir + QDir::separator() + "examples");
	QDirIterator it(examplePath, QStringList() << "*.fract", QDir::Files);

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

	while (it.hasNext())
	{
		QString filename = it.next();
		if (!IsBenchmarking()) qDebug() << "trying file: " << filename;
		cSettings parSettings(cSettings::formatFullText);
		parSettings.BeQuiet(true);
		parSettings.LoadFromFile(filename);
		parSettings.Decode(testPar, testParFractal, testAnimFrames, testKeyframes);
		testPar->Set("image_width", IsBenchmarking() ? 10 : 5);
		testPar->Set("image_height", IsBenchmarking() ? 10 : 5);
		cRenderJob *renderJob = new cRenderJob(testPar, testParFractal, image, &stopRequest);
		renderJob->Init(cRenderJob::still, config);

		if (IsBenchmarking())
			renderJob->Execute();
		else
			QVERIFY2(renderJob->Execute(), "example render failed.");

		delete renderJob;
	}

	delete image;
	delete testKeyframes;
	delete testAnimFrames;
	delete testParFractal;
	delete testPar;
}

void Test::netrender()
{
	if (IsBenchmarking()) return; // no reasonable generic network benchmark
	// test connection of server / client over localhost
	CNetRender *netRenderServer = new CNetRender(1);
	CNetRender *netRenderClient = new CNetRender(1);
	netRenderServer->SetServer(5555);
	netRenderClient->SetClient("127.0.0.1", 5555);

	QTest::qWait(500);

	CNetRender::netRenderStatus clientStatus = netRenderClient->GetStatus();
	QVERIFY2(clientStatus == CNetRender::netRender_READY,
		QString("client status wrong: should be 'READY' but is '%1'.")
			.arg(CNetRender::GetStatusText(clientStatus))
			.toStdString()
			.c_str());

	QVERIFY2(netRenderServer->GetClientCount() == 1,
		QString("client not connected to server.").toStdString().c_str());

	delete netRenderClient;
	delete netRenderServer;
}

void Test::testFlightWrapper()
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

void Test::testFlight()
{
	QString exampleFlightFile =
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
	testPar->Set("image_width", IsBenchmarking() ? 20 : 5);
	testPar->Set("image_height", IsBenchmarking() ? 20 : 5);
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

void Test::testKeyframeWrapper()
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

void Test::testKeyframe()
{
	QString exampleKeyframeFile =
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
	testPar->Set("image_width", IsBenchmarking() ? 20 : 5);
	testPar->Set("image_height", IsBenchmarking() ? 20 : 5);
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

void Test::renderSimpleWrapper()
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

void Test::renderSimple()
{
	// this renders an example file in an "usual" resolution of 100x100 px
	// and benchmarks the runtime
	QString simpleExampleFileName =
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
	testPar->Set("image_width", IsBenchmarking() ? 250 : 100);
	testPar->Set("image_height", IsBenchmarking() ? 250 : 100);
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
