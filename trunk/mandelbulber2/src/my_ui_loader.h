/*
 * MyUILoader.h
 *
 *  Created on: Jul 12, 2014
 *      Author: krzysztof
 */

#ifndef MYUILOADER_H_
#define MYUILOADER_H_

#include <QtUiTools/QtUiTools>

class MyUiLoader: public QUiLoader
{
	 Q_OBJECT
public:
	MyUiLoader(QObject *parent = 0) : QUiLoader(parent) {}
	~MyUiLoader();

	virtual QWidget* createWidget(const QString &className, QWidget *parent = 0, const QString &name = QString());
};

#endif /* MYUILOADER_H_ */
