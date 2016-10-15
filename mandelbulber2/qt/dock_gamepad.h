/*
 * dock_gamepad.h
 *
 *  Created on: 15 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_DOCK_GAMEPAD_H_
#define MANDELBULBER2_QT_DOCK_GAMEPAD_H_

#include <QWidget>

#include "../src/automated_widgets.hpp"

#ifdef USE_GAMEPAD
#include <QtGamepad/qgamepad.h>
#endif // USE_GAMEPAD

namespace Ui
{
class cDockGamepad;
}

class cDockGamepad : public QWidget
{
	Q_OBJECT
public:
	explicit cDockGamepad(QWidget *parent = NULL);
	~cDockGamepad();

private slots:
#ifdef USE_GAMEPAD
	void slotChangeGamepadIndex(int index);
	void slotGamePadDeviceConnected(int index);
	void slotGamePadDeviceDisconnected(int index);
	void slotShiftModeChange(bool isShifting);

	void slotGamepadPitch(double value);
	void slotGamepadYaw(double value);
	void slotGamepadRoll();

	void slotGamepadX(double value);
	void slotGamepadY(double value);
	void slotGamepadZ();
#endif // USE_GAMEPAD

private:
	void ConnectSignals();

	Ui::cDockGamepad *ui;

	cAutomatedWidgets *automatedWidgets;

#ifdef USE_GAMEPAD
	QGamepad gamepad;
#endif
};

#endif /* MANDELBULBER2_QT_DOCK_GAMEPAD_H_ */
