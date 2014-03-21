/*
 * interface.hpp
 *
 *  Created on: Mar 11, 2014
 *      Author: krzysztof
 */

#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include "parameters.hpp"
#include "render_window.hpp"

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
	RenderWindow *mainWindow;
	QImage *qimage;

private:

};

extern cInterface *mainInterface;

#endif /* INTERFACE_HPP_ */
