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

	cInterface();
	void ShowUi(void);
	void ConnectSignals(void);
	void SynchronizeInterfaceWindow(QWidget *window, parameters::container *par, enumReadWrite mode);
	void ConnectSignalsForSlidersInWindow(QWidget *window);
	void MakeColorButtonsInWindow(QWidget *window);
	void GetNameAndType(QString name, QString *parameterName, QString *type);

	QApplication *application;
	RenderWindow *mainWindow;
	QImage *qimage;
	RenderedImage *renderedImage;

private:

};

extern cInterface *mainInterface;

void MakeIconForButton(QColor &color, QPushButton *pushbutton);

#endif /* INTERFACE_HPP_ */
