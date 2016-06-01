/*
 * material_list_view.cpp
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#include "material_manager_view.h"
#include "ui_material_manager_view.h"
#include <QtWidgets/QtWidgets>

#include "../qt/material_editor.h"
#include "../src/fractal_container.hpp"
#include "../src/error_message.hpp"
#include "../src/initparameters.hpp"
#include "../src/settings.hpp"
#include "../src/synchronize_interface.hpp"


cMaterialManagerView::cMaterialManagerView(QWidget *parent) : QWidget(parent), ui(new Ui::cMaterialManagerView)
{
	ui->setupUi(this);

	// TODO Auto-generated constructor stub
	itemView = new cMaterialItemView(this);
	ui->verticalLayout_material_view->addWidget(itemView);
	model = NULL;

	connect(ui->pushButton_newMaterial, SIGNAL(clicked()), this, SLOT(slotAddMaterial()));
	connect(ui->pushButton_deleteMaterial, SIGNAL(clicked()), this, SLOT(slotDeleteMaterial()));
	connect(ui->pushButton_editMaterial, SIGNAL(clicked()), this, SLOT(slotEditMaterial()));
	connect(ui->pushButton_LoadMaterial, SIGNAL(clicked()), this, SLOT(slotLoadMaterial()));
	connect(ui->pushButton_SaveMaterial, SIGNAL(clicked()), this, SLOT(slotSaveMaterial()));
	connect(itemView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotItemSelected(const QModelIndex&)));
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
		reply = QMessageBox::question(this,
																	QObject::tr("Delete material?"),
																	QObject::tr("Are you sure to delete selected material?"),
																	QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			QModelIndex index = itemView->currentIndex();
			itemView->setCurrentIndex(model->index(index.row(), 0));
			model->removeRows(index.row(), 1);
		}
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("You cannot delete last material!"),
															 cErrorMessage::errorMessage);
	}
}

void cMaterialManagerView::slotLoadMaterial()
{
  cSettings parSettings(cSettings::formatFullText);

  QFileDialog dialog(this);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
  dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.dataDirectory + "materials/").absolutePath()));
  //dialog.selectFile(QDir::toNativeSeparators("");
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setWindowTitle(tr("Load material..."));
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    parSettings.LoadFromFile(filename);

  	cParameterContainer params1;
    parSettings.Decode(&params1, NULL);

    model->insertRowWithParameters(&params1);
		emit materialEdited();
  }
}

void cMaterialManagerView::slotSaveMaterial()
{
	//take settings from model
	QModelIndex index = itemView->currentIndex();
	QString settingsFromModel = model->data(index).toString();
	int matIndex = model->materialIndex(index);
	cParameterContainer params;
	params.SetContainerName("materialToSave");
	cSettings tempSettings(cSettings::formatCondensedText);
	tempSettings.LoadFromString(settingsFromModel);
	tempSettings.Decode(&params, NULL);

	//change material number to 1
	cParameterContainer params1;
	InitMaterialParams(1, &params1);
	for(int i=0; i < cMaterial::paramsList.size(); i++)
	{
		cOneParameter parameter = params.GetAsOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), matIndex));
		params1.SetFromOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), 1), parameter);
	}

	cSettings settingsToSave(cSettings::formatCondensedText);
	settingsToSave.CreateText(&params1, NULL);

	QString suggestedFilename = params1.Get<QString>("mat1_name");

  QFileDialog dialog(this);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
  dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.dataDirectory + "materials/").absolutePath()));
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

void cMaterialManagerView::slotItemSelected(const QModelIndex& index)
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

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
	dialog->layout()->addWidget(buttonBox);

	QScrollArea *scrollArea = new QScrollArea(dialog);
	scrollArea->setWidgetResizable( true );
	dialog->layout()->addWidget(scrollArea);

	cMaterialEditor *materialEditor = new cMaterialEditor(dialog);

	scrollArea->setWidget(materialEditor);

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

	if(result == QDialog::Accepted)
	{
		SynchronizeInterfaceWindow(dialog, &params, qInterface::read);
		cSettings tempSettings2(cSettings::formatCondensedText);
		tempSettings2.CreateText(&params, NULL);
		model->setData(index, tempSettings2.GetSettingsText());
		emit materialEdited();
	}

	delete dialog;
}
