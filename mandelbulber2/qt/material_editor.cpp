/*
 * material_editor.cpp
 *
 *  Created on: 3 maj 2016
 *      Author: krzysztof
 */

#include "material_editor.h"
#include "ui_material_editor.h"
#include <QtCore>

#include "../src/common_math.h"
#include "../src/synchronize_interface.hpp"
#include "../src/fractal_coloring.hpp"
#include "../src/preview_file_dialog.h"
#include "../src/system.hpp"
#include "../src/material.h"

cMaterialEditor::cMaterialEditor(QWidget *parent) :
		QWidget(parent), ui(new Ui::cMaterialEditor)
{
	ui->setupUi(this);

	materialIndex = 0;
	parameterContainer = NULL;
	isMaterialAssigned = false;

	QList<QWidget*> materalWidgets = this->findChildren<QWidget*>();
	for(int i=0; i < materalWidgets.size(); i++)
	{
		if (!materalWidgets[i]->objectName().isEmpty())
		{
			QString objectName = materalWidgets[i]->objectName();
			int posOfDash = objectName.indexOf('_');
			if(posOfDash > 0)
			{
				QString newName = objectName.insert(posOfDash, "_mat1");
				materalWidgets[i]->setObjectName(newName);
			}
		}
	}

	ConnectSignals();
}

cMaterialEditor::~cMaterialEditor()
{
	delete ui;
}

void cMaterialEditor::ConnectSignals(void)
{
	connect(ui->pushButton_randomize, SIGNAL(clicked()), this, SLOT(slotPressedButtonRandomize()));
	connect(ui->pushButton_randomPalette,
					SIGNAL(clicked()),
					this,
					SLOT(slotPressedButtonNewRandomPalette()));
	connect(ui->spinbox_coloring_palette_offset,
					SIGNAL(valueChanged(double)),
					this,
					SLOT(slotChangedSpinBoxPaletteOffset(double)));
	connect(ui->spinboxInt_coloring_palette_size,
					SIGNAL(valueChanged(int)),
					this,
					SLOT(slotChangedSpinBoxPaletteSize(int)));
	connect(ui->comboBox_fractal_coloring_algorithm,
					SIGNAL(currentIndexChanged(int)),
					this,
					SLOT(slotChangedComboFractalColoringAlgorithm(int)));
	connect(ui->pushButton_getPaletteFromImage,
					SIGNAL(clicked()),
					this,
					SLOT(slotPressedButtonGetPaletteFromImage()));
}

void cMaterialEditor::AssignMaterial(cParameterContainer *params, int index)
{
	if(isMaterialAssigned)
	{
		qCritical() << "Material is already assigned!";
	}
	else
	{
		materialIndex = index;
		parameterContainer = params;
		ui->widget_material_preview->AssignMaterial(parameterContainer, 1, this);
		isMaterialAssigned = true;
	}
}

cColorPalette cMaterialEditor::GetPaletteFromImage(const QString &filename)
{
	cColorPalette palette;
	QImage imagePalette(filename);

	SynchronizeInterfaceWindow(ui->groupCheck_use_color_texture, parameterContainer, interface::read); //TODO get palette from image will work for actual material
	int paletteSize = parameterContainer->Get<int>(cMaterial::Name("coloring_palette_size", materialIndex));

	if (!imagePalette.isNull())
	{
		int width = imagePalette.width();
		int height = imagePalette.height();

		for (int i = 0; i < paletteSize; i++)
		{
			double angle = (double) i / paletteSize * M_PI * 2.0;
			double x = width / 2 + cos(angle) * width * 0.4;
			double y = height / 2 + sin(angle) * height * 0.4;
			QRgb pixel = imagePalette.pixel(x, y);
			sRGB pixelRGB(qRed(pixel), qGreen(pixel), qBlue(pixel));
			palette.AppendColor(pixelRGB);
		}

	}
	return palette;
}

void cMaterialEditor::slotPressedButtonNewRandomPalette()
{
  SynchronizeInterfaceWindow(ui->groupCheck_use_colors_from_palette, parameterContainer, interface::read); //TODO new palette will work for actual material
  cColorPalette palette(parameterContainer->Get<int>(cMaterial::Name("coloring_palette_size", materialIndex)),
  		parameterContainer->Get<int>(cMaterial::Name("coloring_random_seed", materialIndex)),
			parameterContainer->Get<double>(cMaterial::Name("coloring_saturation", materialIndex)));
  ui->colorpalette_surface_color_palette->SetPalette(palette);
}

void cMaterialEditor::slotPressedButtonRandomize()
{
  srand((unsigned int) clock());
  int seed = Random(999999);
  ui->spinboxInt_coloring_random_seed->setValue(seed);
  slotPressedButtonNewRandomPalette();
}

void cMaterialEditor::slotChangedSpinBoxPaletteOffset(double value)
{
  ui->colorpalette_surface_color_palette->SetOffset(value);
}

void cMaterialEditor::slotChangedSpinBoxPaletteSize(int value)
{
  ui->slider_coloring_palette_offset->setMaximum(value * 100);
}

void cMaterialEditor::slotChangedComboFractalColoringAlgorithm(int index)
{
	sFractalColoring::enumFractalColoringAlgorithm selection = (sFractalColoring::enumFractalColoringAlgorithm)index;
  ui->slider_fractal_coloring_sphere_radius->setEnabled(selection == sFractalColoring::fractalColoringSphere);
  ui->spinbox_fractal_coloring_sphere_radius->setEnabled(selection == sFractalColoring::fractalColoringSphere);
  ui->vect3_fractal_coloring_line_direction_x->setEnabled(selection == sFractalColoring::fractalColoringLine);
  ui->vect3_fractal_coloring_line_direction_y->setEnabled(selection == sFractalColoring::fractalColoringLine);
  ui->vect3_fractal_coloring_line_direction_z->setEnabled(selection == sFractalColoring::fractalColoringLine);
}

void cMaterialEditor::slotPressedButtonGetPaletteFromImage()
{
  PreviewFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
  dialog.setDirectory(QDir::toNativeSeparators(systemData.dataDirectory + QDir::separator() + "textures"
      + QDir::separator()));
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
