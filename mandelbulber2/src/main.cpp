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

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	//Initialization of system functions
	InitSystem();

	//Create default directiories and copy all needed files
	if(!CreateDefaultFolders())
	{
		cerr << "Files/directories initialization failed" << endl;
		return 73;
	}

	//create internal database with parameters
	InitParams(&gPar);

	//create Qt interface windows
	mainInterface = new cInterface(argc, argv);

	//write parameters to ui
	mainInterface->SynchronizeInterfaceWindow(mainInterface->mainWindow, &gPar, cInterface::write);

	//start main Qt loop
	return mainInterface->application->exec();

}

