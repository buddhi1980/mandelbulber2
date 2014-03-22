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

int main(int argc, char *argv[])
{
	//Initialization of system functions
	InitSystem();

	//class for interface windows
	mainInterface = new cInterface;

	WriteLog("Prepare QApplication");
	mainInterface->application = new QApplication(argc, argv);

	//Create default directiories and copy all needed files
	WriteLog("CreateDefaultFolders()");
	if(!CreateDefaultFolders())
	{
		qCritical() << "Files/directories initialization failed" << endl;
		return 73;
	}

	//create internal database with parameters
	InitParams(&gPar);

	mainInterface->ShowUi();

	//write parameters to ui
	mainInterface->SynchronizeInterfaceWindow(mainInterface->mainWindow, &gPar, cInterface::write);

	//start main Qt loop
	return mainInterface->application->exec();

}

