/*
 * buton_load_settings_from_widget.cpp
 *
 *  Created on: 30 lis 2018
 *      Author: krzysztof
 */

#include "buton_load_settings_from_widget.h"

#include "src/interface.hpp"

cButtonLoadSettingsFromWidget::cButtonLoadSettingsFromWidget(QWidget *_parent)
		: QPushButton(_parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(slotPressedButtonLocalLoad()));

	QWidget *parentWidget = dynamic_cast<QWidget *>(parent());
	if (parentWidget)
	{
		setToolTip(tr("Load settings only to %1 widget").arg(parentWidget->objectName()));
	}
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
