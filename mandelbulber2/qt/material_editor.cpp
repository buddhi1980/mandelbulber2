/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <QtCore>

#include "ui_material_editor.h"

#include "preview_file_dialog.h"

#include "src/automated_widgets.hpp"
#include "src/common_math.h"
#include "src/fractal_coloring.hpp"
#include "src/interface.hpp"
#include "src/material.h"
#include "src/synchronize_interface.hpp"
#include "src/system.hpp"

cMaterialEditor::cMaterialEditor(QWidget *parent) : QWidget(parent), ui(new Ui::cMaterialEditor)
{
	ui->setupUi(this);

	materialIndex = 0;
	parameterContainer = nullptr;
	isMaterialAssigned = false;

	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);

	ui->colorpalette_diffuse_gradient->SetGrayscale();
	ui->colorpalette_roughness_gradient->SetGrayscale();

	// ui->colorpalette_surface_color_gradient->SetViewModeOnly();

	ConnectSignals();
}

cMaterialEditor::~cMaterialEditor()
{
	delete ui;
}

void cMaterialEditor::ConnectSignals()
{
	connect(ui->comboBox_fractal_coloring_algorithm, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangedComboFractalColoringAlgorithm(int)));
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

		QList<QWidget *> materialWidgets = findChildren<QWidget *>();
		for (auto &materialWidget : materialWidgets)
		{
			if (!materialWidget->objectName().isEmpty())
			{
				QString objectName = materialWidget->objectName();
				int posOfDash = objectName.indexOf('_');
				if (posOfDash > 0)
				{
					QString newName = objectName.insert(posOfDash, QString("_mat%1").arg(index));
					materialWidget->setObjectName(newName);
				}
			}
		}

		SynchronizeInterfaceWindow(this, parameterContainer, qInterface::write);
	}
}

void cMaterialEditor::slotChangedComboFractalColoringAlgorithm(int index) const
{
	enumFractalColoring selection = enumFractalColoring(index);
	ui->spinbox_fractal_coloring_sphere_radius->setEnabled(selection == fractalColoring_Sphere);
	ui->vect4_fractal_coloring_line_direction_x->setEnabled(selection == fractalColoring_Line);
	ui->vect4_fractal_coloring_line_direction_y->setEnabled(selection == fractalColoring_Line);
	ui->vect4_fractal_coloring_line_direction_z->setEnabled(selection == fractalColoring_Line);
	ui->vect4_fractal_coloring_line_direction_w->setEnabled(selection == fractalColoring_Line);
}

void cMaterialEditor::Colorize(int randomSeed)
{
	cInterface::ColorizeGroupBoxes(this, randomSeed);
}
