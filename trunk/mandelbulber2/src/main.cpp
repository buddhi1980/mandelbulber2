/*
 * main.cpp
 *
 *  Created on: Mar 9, 2014
 *      Author: krzysztof
 */

#include "main.hpp"
#include "system.hpp"
#include "parameters.hpp"
#include "fractparams.hpp"
#include "interface.hpp"
#include "initparameters.hpp"
#include "fractal_list.hpp"

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

