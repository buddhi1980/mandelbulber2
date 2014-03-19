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

//class for main user interface

class cInterface
{
public:
	enum enumReadWrite {read, write};

	cInterface(int argc, char* argv[]);

	void SynchronizeInterfaceWindow(QWidget *window, parameters::container *par, enumReadWrite mode);

	void SetSlotsForSlidersWindow(QWidget *window);
	void GetNameAndType(QString name, QString *parameterName, QString *type);

	QApplication *application;
	QWidget *mainWindow;
	QImage *qimage;

private:
	InterfaceSlots *slot;

};

extern cInterface *mainInterface;

#endif /* INTERFACE_HPP_ */
