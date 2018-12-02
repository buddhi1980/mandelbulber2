/*
 * my_combo_box.h
 *
 *  Created on: 2 gru 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MY_COMBO_BOX_H_
#define MANDELBULBER2_QT_MY_COMBO_BOX_H_

#include <QComboBox>

#include "common_my_widget_wrapper.h"

class MyComboBox : public QComboBox, public CommonMyWidgetWrapper
{
	Q_OBJECT

public:
	MyComboBox(QWidget *parent = nullptr);
	~MyComboBox();

protected:
	void contextMenuEvent(QContextMenuEvent *event) override;
	void paintEvent(QPaintEvent *event) override;

private:
	// methods to define from CommonMyWidgetWrapper
	void resetToDefault() override;
	QString getDefaultAsString() override;
	QString getFullParameterName() override;

	int GetDefault();
	int defaultValue;
};

#endif /* MANDELBULBER2_QT_MY_COMBO_BOX_H_ */
