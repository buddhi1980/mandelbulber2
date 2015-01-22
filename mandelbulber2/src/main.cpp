/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * main function - 'main' function for application
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "main.hpp"
#include "system.hpp"
#include "fractparams.hpp"
#include "interface.hpp"
#include "initparameters.hpp"
#include "fractal_list.hpp"
#include "undo.h"
#include "global_data.hpp"
#include <qapplication.h>

int main(int argc, char *argv[])
{
	//Initialization of system functions
	InitSystem();

	//configure debug output
	qInstallMessageHandler(myMessageOutput);

	//class for interface windows
	mainInterface = new cInterface;

	WriteLog("Prepare QApplication");
	application = new QApplication(argc, argv);
	application->setOrganizationName("Mandelbulber");
	application->setApplicationName("Mandelbulber");

	// Set language from locale
	QTranslator main_translator;
	QTranslator qt_data_translator;
	QString locale = QLocale::system().name();
	WriteLogString("locale", locale);
	main_translator.load(locale, systemData.sharedDir + QDir::separator() + "language");
	qt_data_translator.load("qt_data_" + locale, systemData.sharedDir + QDir::separator() + "language");

	application->installTranslator(&main_translator);
	application->installTranslator(&qt_data_translator);

	//Create default directiories and copy all needed files
	WriteLog("CreateDefaultFolders()");
	if(!CreateDefaultFolders())
	{
		qCritical() << "Files/directories initialization failed" << endl;
		return 73;
	}

	//create internal database with parameters
	gPar = new cParameterContainer;
	gParFractal = new cFractalContainer;
	gPar->SetContainerName("main");
	InitParams(gPar);
	for(int i=0; i<NUMBER_OF_FRACTALS; i++)
	{
		gParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&gParFractal->at(i));
	}
	//Define list of fractal formulas
	DefineFractalList(&fractalList);

	mainInterface->ShowUi();

	//Alocate container for animation frames
	gAnimFrames = new cAnimationFrames;

	gFlightAnimation = new cFlightAnimation(mainInterface, gAnimFrames, mainInterface->mainWindow);

	//write parameters to ui
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);

	//start main Qt loop
	WriteLog("application->exec()");
	int result = application->exec();

	//clean objects when exit
	delete gPar;
	delete gParFractal;
	delete gFlightAnimation;
	delete gAnimFrames;
	delete mainInterface;
	delete application;
	return result;
}

