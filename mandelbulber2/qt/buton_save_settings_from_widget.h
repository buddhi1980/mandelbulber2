/*
 * buton_save_settings_from_widget.h
 *
 *  Created on: 30 lis 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_BUTON_SAVE_SETTINGS_FROM_WIDGET_H_
#define MANDELBULBER2_QT_BUTON_SAVE_SETTINGS_FROM_WIDGET_H_

#include <QPushButton>

class cButtonSaveSettingsFromWidget : public QPushButton
{
	Q_OBJECT

public:
	cButtonSaveSettingsFromWidget(QWidget *parent = nullptr);
	~cButtonSaveSettingsFromWidget() override;

private slots:
	void slotPressedButtonLocalSave();
};

#endif /* MANDELBULBER2_QT_BUTON_LOAD_SETTINGS_FROM_WIDGET_H_ */
