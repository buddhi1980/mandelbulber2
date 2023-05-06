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

#include "ui_settings_browser.h"
#include "thumbnail_widget.h"
#include "src/system_data.hpp"

#include <QCloseEvent>
#include <QDebug>
#include <QString>
#include <QDir>

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

	actualDirectory = QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath());

	ui->lineEdit_folder->setText(actualDirectory);

	CreateListOfSettings();
	PrepareTable();
}

cSettingsBrowser::~cSettingsBrowser()
{
	delete ui;
	qDebug() << "delete cSettingsBrowser";
}

void cSettingsBrowser::closeEvent(QCloseEvent *event)
{
	event->accept();
}

void cSettingsBrowser::slotPressedLoad()
{
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
	ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Preview")));
	ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Filename")));
	ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Last modifued")));

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
	}
	ui->tableWidget->setColumnWidth(1, longestName);
}
