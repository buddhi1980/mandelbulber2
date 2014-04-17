/*
 * fractal_list.hpp
 *
 *  Created on: Apr 16, 2014
 *      Author: krzysztof
 */

#ifndef FRACTAL_LIST_HPP_
#define FRACTAL_LIST_HPP_

#include <QtCore>

struct sFractalDescription
{
	QString nameInComboBox;
	QString internalNane;
	int internalID;
	sFractalDescription(QString _nameInComboBox, QString _internalNane, int _internalID) : nameInComboBox(_nameInComboBox), internalNane(_internalNane), internalID(_internalID) {};
};

extern QList<sFractalDescription> fractalList;

void DefineFractalList(QList<sFractalDescription> *fractalList);

#endif /* FRACTAL_LIST_HPP_ */
