/*
 * palette_edit_widget.h
 *
 *  Created on: 1 cze 2019
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_GRADIENT_EDIT_WIDGET_H_
#define MANDELBULBER2_QT_GRADIENT_EDIT_WIDGET_H_

#include <QWidget>
#include "src/color_gradient.h"

class cGradientEditWidget : public QWidget
{
	Q_OBJECT
public:
	cGradientEditWidget(QWidget *parent = nullptr);
	~cGradientEditWidget();
	void SetViewModeOnly();
	QString GetColors() { return gradient.GetColorsAsString(); }
	void SetColors(const QString &colorsString);

private:
	void paintEvent(QPaintEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void contextMenuEvent(QContextMenuEvent *event) override;

	void PaintButton(const cColorGradient::sColor &posColor, QPainter &painter);
	int CalcButtonPosition(double colorPosition);
	int FindButtonAtPosition(int x);
	void AddColor(QContextMenuEvent *event);
	void RemoveColor(QContextMenuEvent *event);
	void Clear();
	void ChangeNumberOfColors();
	void GrabColors();
	void LoadColors();
	void SaveColors();
	bool DecodeGradientFromFile(QString string);
	void LoadFromClipboard();
	void SaveToClipboard();

	cColorGradient gradient;
	int buttonWidth;
	int margins;
	bool mouseDragStarted;
	int pressedColorIndex;
	int dragStartX;
	bool viewMode;

signals:
	void openEditor();
};

#endif /* MANDELBULBER2_QT_GRADIENT_EDIT_WIDGET_H_ */
