/*
 * myscrolledarea.hpp
 *
 *  Created on: May 10, 2014
 *      Author: krzysztof
 */

#ifndef MYLINEEDIT_HPP_
#define MYLINEEDIT_HPP_

#include <QtGui>

class MyLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	MyLineEdit(QWidget *parent = 0)  : QLineEdit(parent)
	{
		this->setFixedHeight(40);
	};

private:

protected:

};



#endif /* MYLINEEDIT_HPP_ */
