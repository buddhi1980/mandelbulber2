/*
 * mylineedit.hpp
 *
 *  Created on: May 10, 2014
 *      Author: krzysztof
 */

#ifndef MYGROUPBOX_HPP_
#define MYGROUPBOX_HPP_

#include <QtGui>
#include <QtCore>
#include <QGroupBox>
#include <QMenu>
#include "../src/parameters.hpp"

class MyGroupBox : public QGroupBox
{
	Q_OBJECT

public:
	MyGroupBox(QWidget *parent = 0)  : QGroupBox(parent)
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
	QString originalText;

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void paintEvent(QPaintEvent *event);
};



#endif /* MYGROUPBOX_HPP_ */
