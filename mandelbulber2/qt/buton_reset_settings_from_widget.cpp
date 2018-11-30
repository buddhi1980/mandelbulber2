/*
 * buton_reset_settings_from_widget.cpp
 *
 *  Created on: 30 lis 2018
 *      Author: krzysztof
 */

#include "buton_reset_settings_from_widget.h"

#include "src/interface.hpp"

cButtonResetSettingsFromWidget::cButtonResetSettingsFromWidget(QWidget *parent)
		: QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(slotPressedButtonLocalReset()));
}

cButtonResetSettingsFromWidget::~cButtonResetSettingsFromWidget()
{
	// TODO Auto-generated destructor stub
}

void cButtonResetSettingsFromWidget::slotPressedButtonLocalReset()
{
	QWidget *parentWidget = dynamic_cast<QWidget *>(parent());
	if (parentWidget)
	{
		gMainInterface->ResetLocalSettings(parentWidget);
	}
}
