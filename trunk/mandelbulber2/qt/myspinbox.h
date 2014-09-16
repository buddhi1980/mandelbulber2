/*
 * mylineedit.hpp
 *
 *  Created on: May 10, 2014
 *      Author: krzysztof
 */

#ifndef MYSPINBOX_HPP_
#define MYSPINBOX_HPP_

#include <QtGui>
#include <QtCore>
#include <QSpinBox>
#include <QMenu>
#include "../src/parameters.hpp"

class MySpinBox : public QSpinBox
{
	Q_OBJECT

public:
	MySpinBox(QWidget *parent = 0)  : QSpinBox(parent)
	{
		actionResetToDefault = NULL;
		parameterContainer = NULL;

	};

	void AssignParameterContainer(cParameterContainer *container) {parameterContainer = container;}
	void AssingParameterName(QString name) {parameterName = name;}

private:
	QAction *actionResetToDefault;
	QString GetType(const QString &name);
	cParameterContainer *parameterContainer;
	QString parameterName;

protected:
	void contextMenuEvent(QContextMenuEvent *event);
};



#endif /* MYSPINBOX_HPP_ */
