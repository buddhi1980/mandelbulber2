/*
 * buton_load_settings_from_widget.h
 *
 *  Created on: 30 lis 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_BUTON_LOAD_SETTINGS_FROM_WIDGET_H_
#define MANDELBULBER2_QT_BUTON_LOAD_SETTINGS_FROM_WIDGET_H_

#include <QPushButton>

class cButtonLoadSettingsFromWidget : public QPushButton
{
	Q_OBJECT

public:
	cButtonLoadSettingsFromWidget(QWidget *parent = nullptr);
	~cButtonLoadSettingsFromWidget() override;

private slots:
	void slotPressedButtonLocalLoad();
};

#endif /* MANDELBULBER2_QT_BUTON_LOAD_SETTINGS_FROM_WIDGET_H_ */
