/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * main function - 'main' function for application
 */

#include "main.hpp"

#include <qapplication.h>

#include "animation_flight.hpp"
#include "animation_keyframes.hpp"
#include "cimage.hpp"
#include "command_line_interface.hpp"
#include "error_message.hpp"
#include "fractal_list.hpp"
#include "global_data.hpp"
#include "headless.h"
#include "initparameters.hpp"
#include "interface.hpp"
#include "netrender.hpp"
#include "opencl_engine_render_fractal.h"
#include "opencl_global.h"
#include "queue.hpp"
#include "render_data.hpp"
#include "render_window.hpp"
#include "rendered_image_widget.hpp"
#include "settings.hpp"
#include "system.hpp"

int main(int argc, char *argv[])
{
	// Initialization of system functions
	InitSystem();

	// configure debug output
	qInstallMessageHandler(myMessageOutput);

	WriteLog("Prepare QApplication", 2);
	QCoreApplication *gCoreApplication = new QCoreApplication(argc, argv);
	gCoreApplication->setOrganizationName("Mandelbulber");
	gCoreApplication->setApplicationName("Mandelbulber");
	gCoreApplication->setApplicationVersion(MANDELBULBER_VERSION_STRING);

	UpdateLanguage(gCoreApplication);

	cCommandLineInterface commandLineInterface(gApplication);

	if (commandLineInterface.isNoGUI())
	{
		gApplication = qobject_cast<QApplication *>(gCoreApplication);
	}
	else
	{
		delete gCoreApplication;
		gApplication = new QApplication(argc, argv);
		gApplication->setOrganizationName("Mandelbulber");
		gApplication->setApplicationName("Mandelbulber");
		gApplication->setApplicationVersion(MANDELBULBER_VERSION_STRING);
	}

	// registering types for queued connections
	// notice: /* */ prevent formatters from collapsing nested templates,
	// since this may cause compiler errors on falsely interpreted operator>>()
	qRegisterMetaType<cStatistics>("cStatistics");
	qRegisterMetaType<QList<QByteArray> /* */>("QList<QByteArray>");
	qRegisterMetaType<QList<int> /* */>("QList<int>");
	qRegisterMetaType<cParameterContainer>("cParameterContainer");
	qRegisterMetaType<cFractalContainer>("cFractalContainer");
	qRegisterMetaType<sTextures>("sTextures");
	qRegisterMetaType<cProgressText::enumProgressType>("cProgressText::enumProgressType");
	qRegisterMetaType<QVector<int> /* */>("QVector<int>");
	qRegisterMetaType<CVector2<double> /* */>("CVector2<double>");
	qRegisterMetaType<QMessageBox::StandardButtons>("QMessageBox::StandardButtons");
	qRegisterMetaType<QMessageBox::StandardButtons *>("QMessageBox::StandardButtons*");
	qRegisterMetaType<cErrorMessage::enumMessageType>("cErrorMessage::enumMessageType");

	CalcPreferredFontSize(commandLineInterface.isNoGUI());

	// class for interface windows
	gMainInterface = new cInterface;

	gErrorMessage = new cErrorMessage;

	// create default directories and copy all needed files
	WriteLog("CreateDefaultFolders()", 2);
	if (!CreateDefaultFolders())
	{
		qCritical() << "Files/directories initialization failed" << endl;
		return 73;
	}

	// create internal database with parameters
	gPar = new cParameterContainer;
	gParFractal = new cFractalContainer;

	// Allocate container for animation frames
	gAnimFrames = new cAnimationFrames;

	// Allocate container for key frames
	gKeyframes = new cKeyframes;

	gPar->SetContainerName("main");
	InitParams(gPar);
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		gParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&gParFractal->at(i));
	}

	// Define list of fractal formulas
	DefineFractalList(&fractalList);

	// Netrender
	gNetRender = new CNetRender(systemData.numberOfThreads);

	// loading AppSettings
	QString iniFileName = systemData.GetIniFile();
	if (QFile(iniFileName).exists())
	{
		QTextStream out(stdout);
		out << "Settings file: " << iniFileName << endl;
		cSettings parSettings(cSettings::formatAppSettings);
		parSettings.LoadFromFile(iniFileName);
		parSettings.Decode(gPar, gParFractal);
	}

	systemData.loggingVerbosity = gPar->Get<int>("logging_verbosity");

	UpdateDefaultPaths();
	if (!commandLineInterface.isNoGUI())
	{
		UpdateUIStyle();
		UpdateUISkin();
	}
	UpdateLanguage(gApplication);

#ifdef USE_OPENCL
	gOpenCl = new cGlobalOpenCl();
#endif

	commandLineInterface.ReadCLI();

	if (!commandLineInterface.isNoGUI())
	{
		gMainInterface->ShowUi();
		gFlightAnimation = new cFlightAnimation(gMainInterface, gAnimFrames, gMainInterface->mainImage,
			gMainInterface->renderedImage, gPar, gParFractal, gMainInterface->mainWindow);
		gKeyframeAnimation =
			new cKeyframeAnimation(gMainInterface, gKeyframes, gMainInterface->mainImage,
				gMainInterface->renderedImage, gPar, gParFractal, gMainInterface->mainWindow);

		gMainInterface->ConnectProgressAndStatisticsSignals();

		try
		{
			gQueue = new cQueue(gMainInterface, systemData.GetQueueFractlistFile(),
				systemData.GetQueueFolder(), gMainInterface->mainWindow);
		}
		catch (QString &ex)
		{
			cErrorMessage::showMessage(
				QObject::tr("Cannot init queue: ") + ex, cErrorMessage::errorMessage);
			return -1;
		}
	}

	// write parameters to ui
	bool dataFoldersUpdated = false;
	if (!commandLineInterface.isNoGUI())
	{
		gMainInterface->ComboMouseClickUpdate();
		gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
		gMainInterface->ComboMouseClickUpdate();
		dataFoldersUpdated = gMainInterface->DataFolderUpgrade();
		gMainInterface->AutoRecovery();
		gMainInterface->InitPeriodicRefresh();
		gMainInterface->mainWindow->slotPopulateToolbar();
	}

	gInterfaceReadyForSynchronization = true;

	commandLineInterface.ProcessCLI();

	// start main Qt loop
	WriteLog("application->exec()", 2);
	int result = 0;
	if (!commandLineInterface.isNoGUI() && !dataFoldersUpdated) result = gApplication->exec();

	// clean objects when exit
	delete gPar;
	gPar = nullptr;
	delete gParFractal;
	if (gFlightAnimation) delete gFlightAnimation;
	if (gKeyframeAnimation) delete gKeyframeAnimation;
	delete gAnimFrames;
	delete gKeyframes;
	delete gNetRender;
	delete gQueue;
#ifdef USE_OPENCL
	delete gOpenCl;
#endif
	delete gMainInterface;
	delete gErrorMessage;
	delete gApplication;
	return result;
}
