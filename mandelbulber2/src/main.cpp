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
	mainInterface->application->setOrganizationName("Buddhi Software");
	mainInterface->application->setApplicationName("Mandelbulber");

	//Create default directiories and copy all needed files
	WriteLog("CreateDefaultFolders()");
	if(!CreateDefaultFolders())
	{
		qCritical() << "Files/directories initialization failed" << endl;
		return 73;
	}

	//create internal database with parameters
	gPar = new parameters::container;
	gParFractal = new parameters::container[4];
	InitParams(gPar);
	for(int i=0; i<4; i++)
		InitFractalParams(&gParFractal[i]);

	//Define list of fractal formulas
	DefineFractalList(&fractalList);

	mainInterface->ShowUi();

	//write parameters to ui
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);

	//start main Qt loop
	WriteLog("application->exec()");
	return mainInterface->application->exec();
}

