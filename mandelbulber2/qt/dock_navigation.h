/*
 * dock_navigation.h
 *
 *  Created on: 13 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_DOCK_NAVIGATION_H_
#define MANDELBULBER2_QT_DOCK_NAVIGATION_H_

#include <QWidget>

#include "../src/automated_widgets.hpp"

namespace Ui
{
class cDockNavigation;
}

class cDockNavigation : public QWidget
{
	Q_OBJECT
public:
	explicit cDockNavigation(QWidget *parent = NULL);
	~cDockNavigation();

	void RenderButtonSetEnabled(bool enabled);
	void AutoRefreshSetChecked(bool state);
	bool AutoRefreshIsChecked();

private slots:
	void slotStartRender();
	void slotStopRender();

	void slotCameraMove();
	void slotCameraRotation();
	void slotCameraOrTargetEdited();
	void slotRotationEdited();
	void slotCameraDistanceEdited();
	void slotCameraDistanceSlider(int value);
	void slotMovementStepModeChanged(int index);
	void slotUndo();
	void slotRedo();
	void slotPressedButtonResetView();

private:
	void ConnectSignals();

	Ui::cDockNavigation *ui;

	cAutomatedWidgets *automatedWidgets;
};

#endif /* MANDELBULBER2_QT_DOCK_NAVIGATION_H_ */
