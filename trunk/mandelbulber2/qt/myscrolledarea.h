/*
 * myscrolledarea.hpp
 *
 *  Created on: May 10, 2014
 *      Author: krzysztof
 */

#ifndef MYSCROLLEDAREA_HPP_
#define MYSCROLLEDAREA_HPP_

#include <QtGui>

class MyScrolledArea : public QScrollArea
{
	Q_OBJECT

public:
	MyScrolledArea(QWidget *parent = 0)  : QScrollArea(parent) {};

protected:
	void resizeEvent(QResizeEvent *event)
	{
		qDebug() << "resize event";
		QScrollArea::resizeEvent(event);
		emit resized(width(), height());
	}

signals:
	void resized(const int &newWidth, const int &newHeight);
};



#endif /* MYSCROLLEDAREA_HPP_ */
