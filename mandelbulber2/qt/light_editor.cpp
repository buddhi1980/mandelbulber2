/*
 * light_editor.cpp
 *
 *  Created on: 22 gru 2020
 *      Author: krzysztof
 */

#include "light_editor.h"
#include "ui_light_editor.h"

#include "my_combo_box.h"
#include "src/automated_widgets.hpp"
#include "src/light.h"

#include <QDebug>

cLightEditor::cLightEditor(QWidget *parent) : QWidget(parent), ui(new Ui::cLightEditor)
{
	ui->setupUi(this);

	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);

	connect(ui->comboBox_type, qOverload<int>(&MyComboBox::currentIndexChanged), this,
		&cLightEditor::slotChangedLightType);
	connect(ui->spinboxd3_rotation_x, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
		&cLightEditor::slotChangedLightAngleX);
	connect(ui->spinboxd3_rotation_y, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
		&cLightEditor::slotChangedLightAngleY);
	connect(ui->colorButton_color, &MyColorButton::valueChanged, this,
		&cLightEditor::slotChangedLightColor);
}

cLightEditor::~cLightEditor()
{
	delete ui;
}

void cLightEditor::AssignLight(std::shared_ptr<cParameterContainer> params, int index)
{
	if (isLightAssigned)
	{
		qCritical() << "Material is already assigned!";
	}
	else
	{
		lightIndex = index;
		parameterContainer = params;
		isLightAssigned = true;

		QList<QWidget *> lightWidgets = findChildren<QWidget *>();
		for (auto &lightWidget : lightWidgets)
		{
			if (!lightWidget->objectName().isEmpty())
			{
				QString objectName = lightWidget->objectName();
				int posOfDash = objectName.indexOf('_');
				if (posOfDash > 0)
				{
					QString newName = objectName.insert(posOfDash, QString("_light%1").arg(index));
					lightWidget->setObjectName(newName);
				}
			}
		}
	}
}

void cLightEditor::slotChangedLightType(int index)
{
	cLight::enumLightType lightType = static_cast<cLight::enumLightType>(index);

	ui->groupBox_cone_options->setEnabled(lightType == cLight::lightConical);
	ui->groupBox_projection_options->setEnabled(lightType == cLight::lightProjection);
}

void cLightEditor::slotChangedLightAngleX(double angle)
{
	double newLightAngle = angle / 180.0 * M_PI;
	if (newLightAngle != lightAngleAlpha)
	{
		lightAngleAlpha = newLightAngle;
		CVector3 lightAngle(-lightAngleAlpha, -lightAngleBeta, 0.0);
		ui->widget_angle_preview->SetLightAngle(lightAngle);
	}
}

void cLightEditor::slotChangedLightAngleY(double angle)
{
	double newLightAngle = angle / 180.0 * M_PI;
	if (newLightAngle != lightAngleBeta)
	{
		lightAngleBeta = newLightAngle;
		CVector3 lightAngle(-lightAngleAlpha, -lightAngleBeta, 0.0);
		ui->widget_angle_preview->SetLightAngle(lightAngle);
	}
}

void cLightEditor::slotChangedLightColor()
{
	sRGB color = ui->colorButton_color->GetColor();
	sRGBFloat fColor(color.R / 65535.0, color.G / 65535.0, color.B / 65535.0);
	ui->widget_angle_preview->SetLightColor(fColor);
}
