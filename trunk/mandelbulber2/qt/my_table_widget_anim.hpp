/*
 * my_table_widget_anim.hpp
 *
 *  Created on: Jan 1, 2015
 *      Author: krzysztof
 */

#ifndef QT_MY_TABLE_WIDGET_ANIM_HPP_
#define QT_MY_TABLE_WIDGET_ANIM_HPP_

#include <QTableWidget>

class MyTableWidgetAnim : public QTableWidget
{
	Q_OBJECT

public:
	MyTableWidgetAnim(QWidget *parent = 0);
	~MyTableWidgetAnim();

private:


public slots:
	void tableContextMenuRequest(QPoint point);
	void columnContextMenuRequest(QPoint point);
	void rowContextMenuRequest(QPoint point);
};

#endif /* QT_MY_TABLE_WIDGET_ANIM_HPP_ */
