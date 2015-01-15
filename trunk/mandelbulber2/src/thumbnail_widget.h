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
	cThumbnailWidget(int _width, int _height, QObject *_parentWithProgressBar, QWidget *parent);
	~cThumbnailWidget();
	void AssignParameters(const cParameterContainer &_params, const cFractalContainer &_fractal);
	void UseOneCPUCore(bool onlyOne) {useOneCPUCore = onlyOne;}

private:
	void paintEvent(QPaintEvent *event);

private slots:
	void slotRender();
	void slotFullyRendered();
	void slotRandomRender();

private:
	cImage *image;
	cParameterContainer *params;
	cFractalContainer *fractal;
	int tWidth;
	int tHeight;
	QString hash;
	QProgressBar *progressBar;
	bool stopRequest;
	bool isRendered;
	bool hasParameters;
	QObject *parentWithProgressBar;
	bool useOneCPUCore;
	QTimer *timer; //timer for random trigger for rendering (renders thumbnail even when is not visible)

signals:
	void renderRequest();
	void thumbnailRendered();
};

#endif /* QT_THUMBNAIL_WIDGET_H_ */
