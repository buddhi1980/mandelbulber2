/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * EXRFileDialog class - extension of QFileDialog class to store EXR Image files
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Sebastian Jennen (sebastian.jennen@gmx.de)
 */

#include "exr_file_dialog.h"
#include <QGridLayout>

EXRFileDialog::EXRFileDialog(QWidget *parent) : QFileDialog(parent)
{
	setOption(QFileDialog::DontUseNativeDialog);
	vboxlayout = new QVBoxLayout();

	checkboxRGB = new QCheckBox(tr("RGB"));
	checkboxRGB->setChecked(true);

	checkboxAlpha = new QCheckBox(tr("Alpha"));
	checkboxAlpha->setChecked(true);

	checkboxZBuffer = new QCheckBox(tr("Z Buffer"));
	checkboxZBuffer->setChecked(true);

	info = new QLabel("");
	vboxlayout->addWidget(checkboxRGB);
	vboxlayout->addWidget(checkboxAlpha);
	vboxlayout->addWidget(checkboxZBuffer);
	vboxlayout->addWidget(info);
	vboxlayout->addStretch();

	//add to existing layout
	QGridLayout *gridlayout = (QGridLayout*)this->layout();
	gridlayout->addLayout(vboxlayout, 1, 3, 3, 1);
}

EXRFileDialog::~EXRFileDialog()
{
	delete vboxlayout;
	delete info;
}

bool EXRFileDialog::rgbChannel()
{
	return checkboxRGB->isChecked();
}

bool EXRFileDialog::alphaChannel()
{
	return checkboxAlpha->isChecked();
}

bool EXRFileDialog::zBufferChannel()
{
	return checkboxZBuffer->isChecked();
}
