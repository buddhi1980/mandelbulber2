/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2023 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "settings_browser.h"

#include <memory>
#include "ui_settings_browser.h"
#include "thumbnail_widget.h"
#include "src/settings.hpp"
#include "src/system_data.hpp"
#include "src/initparameters.hpp"
#include "src/fractal_enums.h"
#include "src/fractal_container.hpp"
#include "src/nine_fractals.hpp"
#include "formula/definition/abstract_fractal.h"
#include "formula/definition/all_fractal_list.hpp"

#include <QCloseEvent>
#include <QDebug>
#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QList>

cSettingsBrowser::cSettingsBrowser(QWidget *parent) : QDialog(parent), ui(new Ui::cSettingsBrowser)
{
	ui->setupUi(this);

	setModal(true);

	int baseSize = int(systemData.GetPreferredThumbnailSize());
	int sizeMultiply = 1.0;
	previewWidth = sizeMultiply * baseSize * 4 / 3;
	previewHeight = sizeMultiply * baseSize;

	connect(ui->pushButton_load, &QPushButton::clicked, this, &cSettingsBrowser::slotPressedLoad);
	connect(ui->pushButton_cancel, &QPushButton::clicked, this, &cSettingsBrowser::slotPressedCancel);
	connect(ui->pushButton_select_folder, &QPushButton::clicked, this,
		&cSettingsBrowser::slotPressedSelectDirectory);

	actualDirectory = QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath());

	ui->lineEdit_folder->setText(actualDirectory);

	if (!gPar->Get<bool>("opencl_enabled"))
	{
		ui->checkBox_useOpenCL->setChecked(false);
		ui->checkBox_useOpenCL->setDisabled(true);
	}

	ui->checkBox_useOpenCL->setChecked(gPar->Get<bool>("settings_browser_use_opencl"));

	CreateListOfSettings();
	PrepareTable();

	timer.setSingleShot(true);
	timer.start(refreshTimeMsec);
	connect(&timer, &QTimer::timeout, this, &cSettingsBrowser::slotTimer);
}

cSettingsBrowser::~cSettingsBrowser()
{
	DeleteAllThumbnails();
	delete ui;
	// qDebug() << "delete cSettingsBrowser";
}

void cSettingsBrowser::DeleteAllThumbnails()
{
	for (int row = 0; row < ui->tableWidget->rowCount(); row++)
	{
		if (ui->tableWidget->cellWidget(row, previewColumnIndex))
		{
			qobject_cast<cThumbnailWidget *>(ui->tableWidget->cellWidget(row, previewColumnIndex))
				->StopRequest();
			// qDebug() << "row" << row << "stopping";
		}
	}

	for (int row = 0; row < ui->tableWidget->rowCount(); row++)
	{
		if (QWidget *widget = ui->tableWidget->cellWidget(row, previewColumnIndex))
		{
			ui->tableWidget->removeCellWidget(row, previewColumnIndex);
			delete widget;
			// qDebug() << "row" << row << "deleted";
		}
	}
}

void cSettingsBrowser::closeEvent(QCloseEvent *event)
{
	DeleteAllThumbnails();
	event->accept();
}

void cSettingsBrowser::slotPressedLoad()
{
	int row = ui->tableWidget->currentRow();
	if (row >= 0)
	{
		selectedFileName =
			QDir::toNativeSeparators(actualDirectory + QDir::separator() + settingsList[row].filename);
	}

	gPar->Set("settings_browser_use_opencl", ui->checkBox_useOpenCL->isChecked());

	close();
}

void cSettingsBrowser::slotPressedCancel()
{
	close();
}

void cSettingsBrowser::CreateListOfSettings()
{
	settingsList.clear();
	QDir dir(actualDirectory);
	QFileInfoList fileList = dir.entryInfoList(QStringList({"*.fract"}), QDir::Files, QDir::Name);

	for (const QFileInfo &fileInfo : fileList)
	{
		sSettingsListItem newItem;
		newItem.filename = fileInfo.fileName();
		newItem.dateTime = fileInfo.lastModified();
		newItem.loaded = false;
		settingsList.append(newItem);
	}
}

void cSettingsBrowser::PrepareTable()
{
	QFontMetrics fm(font());

	ui->tableWidget->setRowCount(0);
	ui->tableWidget->setColumnCount(numberOfColumns);
	ui->tableWidget->clear();
	ui->tableWidget->setColumnWidth(0, previewWidth);
	ui->tableWidget->setHorizontalHeaderItem(previewColumnIndex, new QTableWidgetItem(tr("Preview")));
	ui->tableWidget->setHorizontalHeaderItem(
		fileNameColumnIndex, new QTableWidgetItem(tr("Filename")));
	ui->tableWidget->setHorizontalHeaderItem(
		dateColumnIndex, new QTableWidgetItem(tr("Last modifued")));
	ui->tableWidget->setHorizontalHeaderItem(
		fractalsColumnIndex, new QTableWidgetItem(tr("Formulas")));

	int longestName = 0;
	for (const sSettingsListItem &item : settingsList)
	{
		int newRowIndex = ui->tableWidget->rowCount();
		ui->tableWidget->insertRow(newRowIndex);
		ui->tableWidget->setItem(
			newRowIndex, fileNameColumnIndex, new QTableWidgetItem(QString(item.filename)));
		ui->tableWidget->setItem(
			newRowIndex, dateColumnIndex, new QTableWidgetItem(QString(item.dateTime.toString())));

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
		longestName = qMax(fm.horizontalAdvance(item.filename), longestName);
#else
		longestName = qMax(fm.width(item.filename), longestName);
#endif

		ui->tableWidget->setRowHeight(newRowIndex, previewHeight);
	}
	ui->tableWidget->setColumnWidth(fileNameColumnIndex, longestName);
	ui->tableWidget->setColumnWidth(fractalsColumnIndex, previewWidth);
}

void cSettingsBrowser::slotTimer()
{
	int firstRowVisible = ui->tableWidget->rowAt(0);
	int lastRowVisible = ui->tableWidget->rowAt(ui->tableWidget->height());
	if (lastRowVisible == -1) lastRowVisible = ui->tableWidget->rowCount() - 1;

	int rowToAdd = -1;

	for (int row = firstRowVisible; row <= lastRowVisible; row++)
	{
		if (!settingsList.at(row).loaded)
		{
			rowToAdd = row;
		}
	}

	if (rowToAdd == -1)
	{
		for (int row = 0; row < ui->tableWidget->rowCount(); row++)
		{
			if (!settingsList.at(row).loaded)
			{
				rowToAdd = row;
				break;
			}
		}
	}

	if (rowToAdd >= 0)
	{
		cSettings parSettings(cSettings::formatFullText);
		parSettings.BeQuiet(true);
		if (parSettings.LoadFromFile(
					actualDirectory + QDir::separator() + settingsList.at(rowToAdd).filename))
		{
			// qDebug() << rowToAdd << settingsList.at(rowToAdd).filename;

			//			progressBar->show();
			std::shared_ptr<cParameterContainer> par(new cParameterContainer);
			std::shared_ptr<cFractalContainer> parFractal(new cFractalContainer);
			InitParams(par);
			for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
				InitFractalParams(parFractal->at(i));

			InitMaterialParams(1, par);

			if (parSettings.Decode(par, parFractal))
			{
				QString listOfFormulas;
				QString prefix;
				if (par->Get<bool>("hybrid_fractal_enable")) prefix += "hybrid: ";
				if (par->Get<bool>("boolean_operators")) prefix += "boolean: ";
				for (int f = 1; f <= NUMBER_OF_FRACTALS; f++)
				{
					fractal::enumFractalFormula eFormula =
						fractal::enumFractalFormula(par->Get<int>("formula", f));
					if (eFormula != fractal::none && par->Get<bool>("fractal_enable", f + 1))
					{
						cAbstractFractal *fractalFormula =
							newFractalList[cNineFractals::GetIndexOnFractalList(eFormula)];
						if (listOfFormulas.length() > 0) listOfFormulas += "; ";
						listOfFormulas += fractalFormula->getNameInComboBox();
					}
				}
				ui->tableWidget->setItem(
					rowToAdd, fractalsColumnIndex, new QTableWidgetItem(prefix + listOfFormulas));

				if (ui->checkBox_useOpenCL->isChecked())
				{
					par->Set("opencl_enabled", true);
					par->Set("opencl_mode", 3);
				}
				else
				{
					par->Set("opencl_enabled", false);
					par->Set("opencl_mode", 0);
				}

				if (!gPar->Get<bool>("thumbnails_with_opencl")) par->Set("opencl_enabled", false);

				double dpiScale = ui->tableWidget->devicePixelRatioF();
				cThumbnailWidget *thumbWidget =
					new cThumbnailWidget(previewWidth, previewHeight, dpiScale * 2.0, nullptr);
				thumbWidget->UseOneCPUCore(true);
				thumbWidget->AssignParameters(par, parFractal);
				ui->tableWidget->setCellWidget(rowToAdd, previewColumnIndex, thumbWidget);
			}
		}
		settingsList[rowToAdd].loaded = true;
	}

	timer.start(refreshTimeMsec);
}

void cSettingsBrowser::slotPressedSelectDirectory()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select folder with fractal settings"),
		actualDirectory, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (!dir.isEmpty())
	{
		actualDirectory = dir;
		ui->lineEdit_folder->setText(actualDirectory);

		timer.stop();
		DeleteAllThumbnails();
		CreateListOfSettings();
		PrepareTable();
		timer.start(refreshTimeMsec);
	}
}
