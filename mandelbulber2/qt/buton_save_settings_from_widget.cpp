/*
 * buton_save_settings_from_widget.cpp
 *
 *  Created on: 30 lis 2018
 *      Author: krzysztof
 */

#include "buton_save_settings_from_widget.h"

#include "src/interface.hpp"

cButtonSaveSettingsFromWidget::cButtonSaveSettingsFromWidget(QWidget *parent) : QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(slotPressedButtonLocalSave()));
}

cButtonSaveSettingsFromWidget::~cButtonSaveSettingsFromWidget()
{
	// TODO Auto-generated destructor stub
}

void cButtonSaveSettingsFromWidget::slotPressedButtonLocalSave()
{
	QWidget *parentWidget = dynamic_cast<QWidget *>(parent());
	if (parentWidget)
	{
		gMainInterface->SaveLocalSettings(parentWidget);
	}
}
