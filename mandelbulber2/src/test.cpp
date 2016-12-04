/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
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

void Test::initTestCase()
{
	if(QFileInfo(testFolder()).exists()) cleanupTestCase();
	CreateFolder(testFolder());
}

void Test::cleanupTestCase()
{
	DeleteAllFilesFromDirectory(testFolder(), "*");
	QDir().rmdir(testFolder());
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
		qDebug() << filename;
		cSettings parSettings(cSettings::formatFullText);
		parSettings.BeQuiet(true);
		parSettings.LoadFromFile(filename);
		parSettings.Decode(testPar, testParFractal, testAnimFrames, testKeyframes);
		testPar->Set("image_width", 5);
		testPar->Set("image_height", 5);
		cRenderJob *renderJob = new cRenderJob(testPar, testParFractal, image, &stopRequest);
		renderJob->Init(cRenderJob::still, config);
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
	testPar->Set("image_width", 5);
	testPar->Set("image_height", 5);
	testPar->Set("flight_first_to_render", 5);
	testPar->Set("flight_last_to_render", 8);
	testPar->Set("anim_flight_dir", testFolder() + QDir::separator());

	cFlightAnimation *flightAnimation = new cFlightAnimation(
		gMainInterface, testAnimFrames, image, NULL, testPar, testParFractal, NULL);

	QVERIFY2(flightAnimation->slotRenderFlight(), "flight render failed.");
	delete image;
	delete testKeyframes;
	delete testAnimFrames;
	delete testParFractal;
	delete testPar;
	delete flightAnimation;
	flightAnimation = NULL;
	return;
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
	testPar->Set("image_width", 5);
	testPar->Set("image_height", 5);
	testPar->Set("keyframe_first_to_render", 50);
	testPar->Set("keyframe_last_to_render", 55);
	testPar->Set("anim_keyframe_dir", testFolder() + QDir::separator());

	cKeyframeAnimation *testKeyframeAnimation = new cKeyframeAnimation(
		gMainInterface, testKeyframes, image, NULL, testPar, testParFractal, NULL);

	QVERIFY2(testKeyframeAnimation->slotRenderKeyframes(), "keyframe render failed.");
	delete image;
	delete testKeyframes;
	delete testAnimFrames;
	delete testParFractal;
	delete testPar;
	delete testKeyframeAnimation;
	testKeyframeAnimation = NULL;
	return;
}
