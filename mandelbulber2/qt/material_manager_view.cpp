/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
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
 *                                          §= "=C=4 §"eM "=B:m\4"]#F,§~
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
 * cMaterialManagerView - promoted QWidget for managing a list of materials
 *
 * This class is a UI wrapper which exposes slots and signals to work on the underlying
 * cMaterialItemModel.
 * See also more information on QT's model-view scheme here:
 * http://doc.qt.io/qt-5/model-view-programming.html
 */

#include "material_manager_view.h"
#include "ui_material_manager_view.h"
#include <QtWidgets/QtWidgets>

#include "../qt/material_editor.h"
#include "../src/error_message.hpp"
#include "../src/fractal_container.hpp"
#include "../src/initparameters.hpp"
#include "../src/settings.hpp"
#include "../src/synchronize_interface.hpp"

cMaterialManagerView::cMaterialManagerView(QWidget *parent)
		: QWidget(parent), ui(new Ui::cMaterialManagerView)
{
	ui->setupUi(this);

	// TODO Auto-generated constructor stub
	itemView = new cMaterialItemView(this);
	ui->verticalLayout->addWidget(itemView);
	model = NULL;

	connect(ui->pushButton_newMaterial, SIGNAL(clicked()), this, SLOT(slotAddMaterial()));
	connect(ui->pushButton_deleteMaterial, SIGNAL(clicked()), this, SLOT(slotDeleteMaterial()));
	connect(ui->pushButton_editMaterial, SIGNAL(clicked()), this, SLOT(slotEditMaterial()));
	connect(ui->pushButton_LoadMaterial, SIGNAL(clicked()), this, SLOT(slotLoadMaterial()));
	connect(ui->pushButton_SaveMaterial, SIGNAL(clicked()), this, SLOT(slotSaveMaterial()));
	connect(itemView, SIGNAL(activated(const QModelIndex &)), this,
		SLOT(slotItemSelected(const QModelIndex &)));
}

cMaterialManagerView::~cMaterialManagerView()
{
	// TODO Auto-generated destructor stub
}

void cMaterialManagerView::SetModel(cMaterialItemModel *_model)
{
	model = _model;
	itemView->setModel(_model);
}

void cMaterialManagerView::slotAddMaterial()
{
	int rows = model->rowCount();
	model->insertRows(rows, 1);
}

void cMaterialManagerView::slotDeleteMaterial()
{
	if (model->rowCount() > 1)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, QObject::tr("Delete material?"),
			QObject::tr("Are you sure to delete selected material?"), QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			QModelIndex index = itemView->currentIndex();
			itemView->setCurrentIndex(model->index(index.row(), 0));
			model->removeRows(index.row(), 1);
		}
	}
	else
	{
		cErrorMessage::showMessage(
			QObject::tr("You cannot delete last material!"), cErrorMessage::errorMessage);
	}
}

void cMaterialManagerView::slotLoadMaterial()
{
	cSettings parSettings(cSettings::formatFullText);

	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(QFileInfo(systemData.dataDirectory + "materials/").absolutePath()));
	// dialog.selectFile(QDir::toNativeSeparators("");
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load material..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		for (int i = 0; i < filenames.size(); i++)
		{
			QString filename = QDir::toNativeSeparators(filenames[i]);
			parSettings.LoadFromFile(filename);

			cParameterContainer params1;
			parSettings.Decode(&params1, NULL);

			model->insertRowWithParameters(&params1);
			emit materialEdited();
		}
	}
}

void cMaterialManagerView::slotSaveMaterial()
{
	// take settings from model
	QModelIndex index = itemView->currentIndex();
	QString settingsFromModel = model->data(index).toString();
	int matIndex = model->materialIndex(index);
	cParameterContainer params;
	params.SetContainerName("materialToSave");
	cSettings tempSettings(cSettings::formatCondensedText);
	tempSettings.LoadFromString(settingsFromModel);
	tempSettings.Decode(&params, NULL);

	// change material number to 1
	cParameterContainer params1;
	InitMaterialParams(1, &params1);
	for (int i = 0; i < cMaterial::paramsList.size(); i++)
	{
		cOneParameter parameter =
			params.GetAsOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), matIndex));
		params1.SetFromOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), 1), parameter);
	}

	cSettings settingsToSave(cSettings::formatCondensedText);
	settingsToSave.CreateText(&params1, NULL);

	QString suggestedFilename = params1.Get<QString>("mat1_name");

	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(QFileInfo(systemData.dataDirectory + "materials/").absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(suggestedFilename));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save material..."));
	dialog.setDefaultSuffix("fract");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		settingsToSave.SaveToFile(filename);
	}
}

void cMaterialManagerView::slotItemSelected(const QModelIndex &index)
{
	int selection = model->materialIndex(index);
	emit materialSelected(selection);
}

void cMaterialManagerView::SetSelection(QModelIndex index)
{
	itemView->setCurrentIndex(index);
}

void cMaterialManagerView::slotEditMaterial()
{
	QDialog *dialog = new QDialog(this);
	QVBoxLayout *layout = new QVBoxLayout(dialog);
	dialog->setLayout(layout);

	QDialogButtonBox *buttonBox =
		new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
	dialog->layout()->addWidget(buttonBox);

	cMaterialEditor *materialEditor = new cMaterialEditor(dialog);
	materialEditor->setMinimumWidth(500);

	dialog->layout()->addWidget(materialEditor);

	QModelIndex index = itemView->currentIndex();
	QString settingsFromModel = model->data(index).toString();
	int matIndex = model->materialIndex(index);

	cParameterContainer params;
	params.SetContainerName("materialEdited");
	InitMaterialParams(matIndex, &params);

	cSettings tempSettings(cSettings::formatCondensedText);
	tempSettings.LoadFromString(settingsFromModel);
	tempSettings.Decode(&params, NULL);

	materialEditor->AssignMaterial(&params, matIndex);

	connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

	int result = dialog->exec();

	if (result == QDialog::Accepted)
	{
		SynchronizeInterfaceWindow(dialog, &params, qInterface::read);
		cSettings tempSettings2(cSettings::formatCondensedText);
		tempSettings2.CreateText(&params, NULL);
		model->setData(index, tempSettings2.GetSettingsText());
		emit materialEdited();
	}

	delete dialog;
}
