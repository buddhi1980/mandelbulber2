/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * TODO: description
 */

#include "light_editor.h"

#include <QDebug>

#include "ui_light_editor.h"

#include "my_combo_box.h"

#include "src/automated_widgets.hpp"
#include "src/light.h"

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
	connect(ui->widget_angle_preview, &cLightWidget::angleChanged, this,
		&cLightEditor::slotChangedLightAngles);
	connect(ui->checkBox_relative_position, &MyCheckBox::stateChanged, this,
		&cLightEditor::slotChangedRelativeMode);
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
	ui->widget_angle_preview->setEnabled(lightType == cLight::lightDirectional);
}

void cLightEditor::slotChangedLightAngleX(double angle)
{
	double newLightAngle = angle / 180.0 * M_PI;
	if (newLightAngle != lightAngleAlpha)
	{
		lightAngleAlpha = newLightAngle;
		CVector3 lightAngle(-lightAngleAlpha, lightAngleBeta, 0.0);
		ui->widget_angle_preview->SetLightAngle(lightAngle);
	}
}

void cLightEditor::slotChangedLightAngleY(double angle)
{
	double newLightAngle = angle / 180.0 * M_PI;
	if (newLightAngle != lightAngleBeta)
	{
		lightAngleBeta = newLightAngle;
		CVector3 lightAngle(-lightAngleAlpha, lightAngleBeta, 0.0);
		ui->widget_angle_preview->SetLightAngle(lightAngle);
	}
}

void cLightEditor::slotChangedLightColor()
{
	sRGB color = ui->colorButton_color->GetColor();
	sRGBFloat fColor(color.R / 65535.0, color.G / 65535.0, color.B / 65535.0);
	ui->widget_angle_preview->SetLightColor(fColor);
}

void cLightEditor::slotChangedLightAngles(double alpha, double beta)
{
	ui->spinboxd3_rotation_x->setValue(-alpha * 180.0 / M_PI);
	ui->spinboxd3_rotation_y->setValue(-beta * 180.0 / M_PI);
}

void cLightEditor::slotChangedRelativeMode(int state)
{
	ui->widget_angle_preview->SetRelativeMode(bool(state));
}
