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
#include "parameters.hpp"
#include "fractparams.hpp"
#include "interface.hpp"
#include "initparameters.hpp"
#include "fractal_list.hpp"
#include "undo.h"

int main(int argc, char *argv[])
{
	//Initialization of system functions
	InitSystem();

	//class for interface windows
	mainInterface = new cInterface;

	WriteLog("Prepare QApplication");
	mainInterface->application = new QApplication(argc, argv);
	mainInterface->application->setOrganizationName("Mandelbulber");
	mainInterface->application->setApplicationName("Mandelbulber");

	// Set language from locale
	QTranslator main_translator;
	QTranslator qt_data_translator;
	QString locale = QLocale::system().name();
	WriteLogString("locale", locale);
	main_translator.load(locale, systemData.sharedDir + QDir::separator() + "language");
	qt_data_translator.load("qt_data_" + locale, systemData.sharedDir + QDir::separator() + "language");

	mainInterface->application->installTranslator(&main_translator);
	mainInterface->application->installTranslator(&qt_data_translator);

	//Create default directiories and copy all needed files
	WriteLog("CreateDefaultFolders()");
	if(!CreateDefaultFolders())
	{
		qCritical() << "Files/directories initialization failed" << endl;
		return 73;
	}

	//create internal database with parameters
	gPar = new cParameterContainer;
	gParFractal = new cParameterContainer[4];
	InitParams(gPar);
	gPar->SetContainerName("main");
	for(int i=0; i<4; i++)
	{
		InitFractalParams(&gParFractal[i]);
		gParFractal[i].SetContainerName(QString("fractal") + QString::number(i));
	}
	//Define list of fractal formulas
	DefineFractalList(&fractalList);

	mainInterface->ShowUi();

	//write parameters to ui
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);

	//setup main image
	mainInterface->mainImage = new cImage(gPar->Get<int>("image_width"),gPar->Get<int>("image_height"));
	mainInterface->mainImage->CreatePreview(1.0, 800, 600, mainInterface->renderedImage);
	mainInterface->renderedImage->setMinimumSize(mainInterface->mainImage->GetPreviewWidth(),mainInterface->mainImage->GetPreviewHeight());
	mainInterface->renderedImage->AssignImage(mainInterface->mainImage);
	mainInterface->renderedImage->AssignParameters(gPar);

	//start main Qt loop
	WriteLog("application->exec()");
	int result = mainInterface->application->exec();

	//clean objects when exit
	delete mainInterface;
	delete gPar;
	delete[] gParFractal;
	return result;
}

