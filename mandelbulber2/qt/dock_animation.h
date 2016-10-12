/*
 * dock_animation.h
 *
 *  Created on: 12 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_DOCK_ANIMATION_H_
#define MANDELBULBER2_QT_DOCK_ANIMATION_H_

#include <QWidget>

#include "../src/automated_widgets.hpp"

namespace Ui
{
class cDockAnimation;
}

class cDockAnimation : public QWidget
{
	Q_OBJECT
public:
	explicit cDockAnimation(QWidget *parent = NULL);
	~cDockAnimation();

private:
	Ui::cDockAnimation *ui;

	cAutomatedWidgets *automatedWidgets;

	friend class cFlightAnimation;
	friend class cKeyframeAnimation;
};

#endif /* MANDELBULBER2_QT_DOCK_ANIMATION_H_ */
