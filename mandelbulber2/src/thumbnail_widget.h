/*
 * thumbnail_widget.h
 *
 *  Created on: Jan 11, 2015
 *      Author: krzysztof
 */

#ifndef QT_THUMBNAIL_WIDGET_H_
#define QT_THUMBNAIL_WIDGET_H_

#include <QWidget>
#include "thumbnail.hpp"
#include <QThread>

class cThumbnailWidget: public QWidget
{
	Q_OBJECT
public:
	cThumbnailWidget(int _width, int _height, QWidget *parent);
	~cThumbnailWidget();
	void AssignParameters(const cParameterContainer &_params, const cFractalContainer &_fractal);
	void AssignProgressBar(QProgressBar *_progressBar);

private:
	void paintEvent(QPaintEvent *event);

private slots:
	void slotRender();
	void slotFullyRendered();

private:
	cImage *image;
	cParameterContainer params;
	cFractalContainer fractal;
	int tWidth;
	int tHeight;
	QString hash;
	QProgressBar *progressBar;
	bool stopRequest;
	bool isRendered;
	bool hasParameters;

signals:
	void renderRequest();
	void thumbnailRendered();
};

#endif /* QT_THUMBNAIL_WIDGET_H_ */
