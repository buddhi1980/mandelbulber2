/*
 * mylineedit.hpp
 *
 *  Created on: May 10, 2014
 *      Author: krzysztof
 */

#ifndef MYDOUBLESPINBOX_HPP_
#define MYDOUBLESPINBOX_HPP_

#include <QtGui>
#include <QtCore>
#include <QDoubleSpinBox>
#include <QMenu>
#include "../src/parameters.hpp"

class MyDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT

public:
	MyDoubleSpinBox(QWidget *parent = 0)  : QDoubleSpinBox(parent)
	{
		actionResetToDefault = NULL;
		parameterContainer = NULL;
		gotDefault = false;
		defaultValue = 0;
	};

	void AssignParameterContainer(cParameterContainer *container) {parameterContainer = container;}
	void AssingParameterName(QString name) {parameterName = name;}

private:
	QAction *actionResetToDefault;
	QString GetType(const QString &name);
	cParameterContainer *parameterContainer;
	QString parameterName;

	double GetDefault();
	double defaultValue;
	bool gotDefault;

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void paintEvent(QPaintEvent *event);
};



#endif /* MYDOUBLESPINBOX_HPP_ */
