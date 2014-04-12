/*
 * render_window.hpp
 *
 *  Created on: Mar 20, 2014
 *      Author: krzysztof
 */

#ifndef _RENDER_WINDOW_H
#define _RENDER_WINDOW_H

#include <QMainWindow>
#include "../qt/ui_render_window.h"

namespace Ui
{
class RenderWindow;
}

class RenderWindow: public QMainWindow
{
Q_OBJECT

public:
	explicit RenderWindow(QWidget *parent = 0);
	~RenderWindow();

private slots:
	void testSlot();
	void load();
	void slotSliderMoved(int value);
	void slotDoubleSpinBoxChanged(double value);
	void slotIntSpinBoxChanged(int value);
	void slotLogSliderMoved(int value);
	void slotIntSliderMoved(int value);
	void slotLogLineEditChanged(const QString &text);
	void slotPresedOnColorButton();
private:
	Ui::RenderWindow *ui;

	friend class cInterface;
};


class RenderedImage: public QWidget
{
Q_OBJECT

public:
	RenderedImage(QWidget *parent = 0);

protected:
	void paintEvent(QPaintEvent *event);
};


#endif // _RENDER_WINDOW_H


