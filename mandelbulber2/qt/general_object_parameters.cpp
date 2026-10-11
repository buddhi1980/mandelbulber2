/*
 * general_object_parameters.cpp
 *
 *  Created on: 19 kwi 2026
 *      Author: krzysztof
 */

#include "general_object_parameters.h"
#include "ui_general_object_parameters.h"

#include <QComboBox>

#include "src/rendered_image_widget.hpp"

cGeneralObjectParameters::cGeneralObjectParameters(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cGeneralObjectParameters)
{
	ui->setupUi(this);
	connect(ui->pushButton_placeObject, &QPushButton::clicked, this,
		&cGeneralObjectParameters::onPushButtonPlaceObjectClicked);
}

void cGeneralObjectParameters::setPlaceObjectNodeId(int nodeId)
{
	placeObjectNodeId = nodeId;
}

void cGeneralObjectParameters::onPushButtonPlaceObjectClicked()
{
	if (mouseFunctionComboWidget == nullptr || placeObjectNodeId < 0) return;

	// Build the data key of the "Place <object>" combo entry for the current node:
	// [clickPlaceObject, nodeId] (same format as cInterface::ComboMouseClickUpdate)
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlaceObject));
	item.append(placeObjectNodeId);

	int index = mouseFunctionComboWidget->findData(item);
	if (index >= 0) mouseFunctionComboWidget->setCurrentIndex(index);
}

cGeneralObjectParameters::~cGeneralObjectParameters()
{
	delete ui;
}
