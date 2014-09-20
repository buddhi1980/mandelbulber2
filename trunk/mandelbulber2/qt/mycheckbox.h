/*
 * mylineedit.hpp
 *
 *  Created on: May 10, 2014
 *      Author: krzysztof
 */

#ifndef MYCHECKBOX_HPP_
#define MYCHECKBOX_HPP_

#include <QtGui>
#include <QtCore>
#include <QCheckBox>
#include <QMenu>
#include "../src/parameters.hpp"

class MyCheckBox : public QCheckBox
{
	Q_OBJECT

public:
	MyCheckBox(QWidget *parent = 0)  : QCheckBox(parent)
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

	bool GetDefault();
	bool defaultValue;
	bool gotDefault;

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void paintEvent(QPaintEvent *event);
};



#endif /* MYCHECKBOX_HPP_ */
