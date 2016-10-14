/*
 * dock_queue.h
 *
 *  Created on: 14 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_DOCK_QUEUE_H_
#define MANDELBULBER2_QT_DOCK_QUEUE_H_

#include "dock_queue.h"
#include <QWidget>

#include "../qt/my_progress_bar.h"
#include "../src/automated_widgets.hpp"
#include "../src/rendered_image_widget.hpp"

namespace Ui
{
class cDockQueue;
}

class cDockQueue : public QWidget
{
	Q_OBJECT
public:
	explicit cDockQueue(QWidget *parent = NULL);
	~cDockQueue();
	MyProgressBar* GetProgressBarImage();
	MyProgressBar* GetProgressBarAnimation();

private:
	Ui::cDockQueue *ui;

	cAutomatedWidgets *automatedWidgets;

	RenderedImage *renderedImageWidget;

	friend class cQueue;
};

#endif /* MANDELBULBER2_QT_DOCK_QUEUE_H_ */
