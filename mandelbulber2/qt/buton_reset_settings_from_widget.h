/*
 * buton_reset_settings_from_widget.h
 *
 *  Created on: 30 lis 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_BUTON_RESET_SETTINGS_FROM_WIDGET_H_
#define MANDELBULBER2_QT_BUTON_RESET_SETTINGS_FROM_WIDGET_H_

#include <QPushButton>

class cButtonResetSettingsFromWidget : public QPushButton
{
	Q_OBJECT

public:
	cButtonResetSettingsFromWidget(QWidget *parent = nullptr);
	~cButtonResetSettingsFromWidget() override;

private slots:
	void slotPressedButtonLocalReset();
};

#endif /* MANDELBULBER2_QT_BUTON_RESET_SETTINGS_FROM_WIDGET_H_ */
