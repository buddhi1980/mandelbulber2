/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cMaterialEditor - promoted QWidget for editing of a material
 */

#include "material_editor.h"
#include "ui_material_editor.h"
#include <QtCore>

#include "../src/automated_widgets.hpp"
#include "../src/common_math.h"
#include "../src/fractal_coloring.hpp"
#include "../src/material.h"
#include "../src/synchronize_interface.hpp"
#include "../src/system.hpp"
#include "preview_file_dialog.h"

cMaterialEditor::cMaterialEditor(QWidget *parent) : QWidget(parent), ui(new Ui::cMaterialEditor)
{
	ui->setupUi(this);

	materialIndex = 0;
	parameterContainer = nullptr;
	isMaterialAssigned = false;

	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);

	ConnectSignals();
}

cMaterialEditor::~cMaterialEditor()
{
	delete ui;
}

void cMaterialEditor::ConnectSignals()
{
	connect(ui->pushButton_randomize, SIGNAL(clicked()), this, SLOT(slotPressedButtonRandomize()));
	connect(ui->pushButton_randomPalette, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonNewRandomPalette()));
	connect(ui->spinbox_coloring_palette_offset, SIGNAL(valueChanged(double)), this,
		SLOT(slotChangedSpinBoxPaletteOffset(double)));
	connect(ui->spinboxInt_coloring_palette_size, SIGNAL(valueChanged(int)), this,
		SLOT(slotChangedSpinBoxPaletteSize(int)));
	connect(ui->comboBox_fractal_coloring_algorithm, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangedComboFractalColoringAlgorithm(int)));
	connect(ui->pushButton_getPaletteFromImage, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonGetPaletteFromImage()));
	connect(
		ui->widget_material_preview, SIGNAL(materialChanged(int)), this, SIGNAL(materialChanged(int)));
}

void cMaterialEditor::AssignMaterial(cParameterContainer *params, int index)
{
	if (isMaterialAssigned)
	{
		qCritical() << "Material is already assigned!";
	}
	else
	{
		materialIndex = index;
		parameterContainer = params;
		ui->widget_material_preview->AssignMaterial(parameterContainer, index, this);
		isMaterialAssigned = true;

		QList<QWidget *> materialWidgets = this->findChildren<QWidget *>();
		for (int i = 0; i < materialWidgets.size(); i++)
		{
			if (!materialWidgets[i]->objectName().isEmpty())
			{
				QString objectName = materialWidgets[i]->objectName();
				int posOfDash = objectName.indexOf('_');
				if (posOfDash > 0)
				{
					QString newName = objectName.insert(posOfDash, QString("_mat%1").arg(index));
					materialWidgets[i]->setObjectName(newName);
				}
			}
		}

		SynchronizeInterfaceWindow(this, parameterContainer, qInterface::write);
	}
}

cColorPalette cMaterialEditor::GetPaletteFromImage(const QString &filename) const
{
	cColorPalette palette;
	QImage imagePalette(filename);

	SynchronizeInterfaceWindow(
		ui->groupCheck_use_color_texture, parameterContainer, qInterface::read);
	int paletteSize =
		parameterContainer->Get<int>(cMaterial::Name("coloring_palette_size", materialIndex));

	if (!imagePalette.isNull())
	{
		int width = imagePalette.width();
		int height = imagePalette.height();

		for (int i = 0; i < paletteSize; i++)
		{
			double angle = double(i) / paletteSize * M_PI * 2.0;
			double x = width / 2 + cos(angle) * width * 0.4;
			double y = height / 2 + sin(angle) * height * 0.4;
			QRgb pixel = imagePalette.pixel(x, y);
			sRGB pixelRGB(qRed(pixel), qGreen(pixel), qBlue(pixel));
			palette.AppendColor(pixelRGB);
		}
	}
	return palette;
}

void cMaterialEditor::slotPressedButtonNewRandomPalette() const
{
	SynchronizeInterfaceWindow(
		ui->groupCheck_use_colors_from_palette, parameterContainer, qInterface::read);
	cColorPalette palette(
		parameterContainer->Get<int>(cMaterial::Name("coloring_palette_size", materialIndex)),
		parameterContainer->Get<int>(cMaterial::Name("coloring_random_seed", materialIndex)),
		parameterContainer->Get<double>(cMaterial::Name("coloring_saturation", materialIndex)));
	ui->colorpalette_surface_color_palette->SetPalette(palette);
}

void cMaterialEditor::slotPressedButtonRandomize() const
{
	srand(static_cast<unsigned int>(QTime::currentTime().msec()));
	int seed = Random(999999);
	ui->spinboxInt_coloring_random_seed->setValue(seed);
	slotPressedButtonNewRandomPalette();
}

void cMaterialEditor::slotChangedSpinBoxPaletteOffset(double value) const
{
	ui->colorpalette_surface_color_palette->SetOffset(value);
}

void cMaterialEditor::slotChangedSpinBoxPaletteSize(int value) const
{
	ui->slider_coloring_palette_offset->setMaximum(value * 100);
}

void cMaterialEditor::slotChangedComboFractalColoringAlgorithm(int index) const
{
	sFractalColoring::enumFractalColoringAlgorithm selection =
		sFractalColoring::enumFractalColoringAlgorithm(index);
	ui->slider_fractal_coloring_sphere_radius->setEnabled(
		selection == sFractalColoring::fractalColoringSphere);
	ui->spinbox_fractal_coloring_sphere_radius->setEnabled(
		selection == sFractalColoring::fractalColoringSphere);
	ui->vect3_fractal_coloring_line_direction_x->setEnabled(
		selection == sFractalColoring::fractalColoringLine);
	ui->vect3_fractal_coloring_line_direction_y->setEnabled(
		selection == sFractalColoring::fractalColoringLine);
	ui->vect3_fractal_coloring_line_direction_z->setEnabled(
		selection == sFractalColoring::fractalColoringLine);
}

void cMaterialEditor::slotPressedButtonGetPaletteFromImage()
{
	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
	// dialog.setDirectory(QDir::toNativeSeparators(
	// 	systemData.dataDirectory + QDir::separator() + "textures" + QDir::separator()));
	// there is no texture folder in data folder, is images folder OK?
	dialog.setDirectory(QDir::toNativeSeparators(systemData.GetImagesFolder() + QDir::separator()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.lastImagePaletteFile));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select image to grab colors..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		cColorPalette palette = GetPaletteFromImage(filename);
		ui->colorpalette_surface_color_palette->SetPalette(palette);
		systemData.lastImagePaletteFile = filename;
	}
}
