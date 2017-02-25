/*
 * detached_window.h
 *
 *  Created on: 25 lut 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_DETACHED_WINDOW_H_
#define MANDELBULBER2_QT_DETACHED_WINDOW_H_

#include <QMainWindow>

namespace Ui
{
class cDetachedWindow;
}

class cDetachedWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit cDetachedWindow(QWidget *parent = nullptr);
	~cDetachedWindow();

	void InstallImageWidget(QWidget *widget);
	void RemoveImageWidget(QWidget *widget);

private:
	Ui::cDetachedWindow *ui;
};

#endif /* MANDELBULBER2_QT_DETACHED_WINDOW_H_ */
