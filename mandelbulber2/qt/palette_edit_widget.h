/*
 * palette_edit_widget.h
 *
 *  Created on: 1 cze 2019
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_PALETTE_EDIT_WIDGET_H_
#define MANDELBULBER2_QT_PALETTE_EDIT_WIDGET_H_

#include <QWidget>

class cPaletteEditWidget : public QWidget
{
	Q_OBJECT
public:
	cPaletteEditWidget(QWidget *parent = nullptr);
	~cPaletteEditWidget();

private:
	void paintEvent(QPaintEvent *event) override;
};

#endif /* MANDELBULBER2_QT_PALETTE_EDIT_WIDGET_H_ */
