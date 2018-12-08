/*
 * buton_reset_settings_from_widget.cpp
 *
 *  Created on: 30 lis 2018
 *      Author: krzysztof
 */

#include "buton_reset_settings_from_widget.h"

#include "src/interface.hpp"

cButtonResetSettingsFromWidget::cButtonResetSettingsFromWidget(QWidget *_parent)
		: QPushButton(_parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(slotPressedButtonLocalReset()));

	QWidget *parentWidget = dynamic_cast<QWidget *>(parent());
	if (parentWidget)
	{
		setToolTip(tr("Reset settings only in %1 widget").arg(parentWidget->objectName()));
	}
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
