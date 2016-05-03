/*
 * synchronize_interface.hpp
 *
 *  Created on: 3 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_SYNCHRONIZE_INTERFACE_HPP_
#define MANDELBULBER2_SRC_SYNCHRONIZE_INTERFACE_HPP_

#include <QWidget>
#include "parameters.hpp"
#include "fractal_container.hpp"

namespace interface
{
enum enumReadWrite
{
	read, write
};
}

void SynchronizeInterfaceWindow(QWidget *window, cParameterContainer *par, interface::enumReadWrite mode);
void GetNameAndType(QString name, QString *parameterName, QString *type);

#endif /* MANDELBULBER2_SRC_SYNCHRONIZE_INTERFACE_HPP_ */
