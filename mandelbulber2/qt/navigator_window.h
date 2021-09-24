/*
 * navigator_window.h
 *
 *  Created on: 24 wrz 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_
#define MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_

#include <QDialog>

namespace Ui
{
class cNavigatorWindow;
}

class cNavigatorWindow : public QDialog
{
	Q_OBJECT

public:
	explicit cNavigatorWindow(QWidget *parent = nullptr);
	~cNavigatorWindow();

private:
	Ui::cNavigatorWindow *ui;
};

#endif /* MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_ */
