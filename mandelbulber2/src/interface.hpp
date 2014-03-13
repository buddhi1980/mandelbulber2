/*
 * interface.hpp
 *
 *  Created on: Mar 11, 2014
 *      Author: krzysztof
 */

#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include "qt_interface.hpp"
#include "parameters.hpp"

using namespace std;

class cInterface
{
public:
	cInterface(int argc, char* argv[]);

	void ReadInterfaceWindow(QWidget *window, parameters::container *par);
	void SetSlotsForSlidersWindow(QWidget *window);
	void GetNameAndType(string name, string *parameterName, string *type);

	QApplication *application;
	QWidget *mainWindow;
	QWidget *shadersWindow;
	QImage *qimage;

private:
	InterfaceSlots *slot;

};

extern cInterface *interface;

#endif /* INTERFACE_HPP_ */
