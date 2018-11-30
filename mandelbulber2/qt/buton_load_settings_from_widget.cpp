/*
 * buton_load_settings_from_widget.cpp
 *
 *  Created on: 30 lis 2018
 *      Author: krzysztof
 */

#include "buton_load_settings_from_widget.h"

#include "src/interface.hpp"

cButtonLoadSettingsFromWidget::cButtonLoadSettingsFromWidget(QWidget *parent) : QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(slotPressedButtonLocalLoad()));
}

cButtonLoadSettingsFromWidget::~cButtonLoadSettingsFromWidget()
{
	// TODO Auto-generated destructor stub
}

void cButtonLoadSettingsFromWidget::slotPressedButtonLocalLoad()
{
	QWidget *parentWidget = dynamic_cast<QWidget *>(parent());
	if (parentWidget)
	{
		gMainInterface->LoadLocalSettings(parentWidget);
	}
}
