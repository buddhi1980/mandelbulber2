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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * cVoxelExportDialog class - class for handling the voxelexport dialog
 */

#include "voxel_export_dialog.h"

#include "../qt/player_widget.hpp"
#include "../src/automated_widgets.hpp"
#include "../src/error_message.hpp"
#include "../src/initparameters.hpp"
#include "../src/interface.hpp"
#include "../src/system.hpp"
#include "../src/voxel_export.hpp"
#include "ui_voxel_export_dialog.h"

cVoxelExportDialog::cVoxelExportDialog(QWidget *parent)
		: QDialog(parent), ui(new Ui::cVoxelExportDialog)
{
	initFinished = false;
	slicerBusy = false;
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	initFinished = true;
	ui->progressBar->hide();
	voxelExport = nullptr;
}

cVoxelExportDialog::~cVoxelExportDialog()
{
	delete ui;
}

void cVoxelExportDialog::on_pushButton_start_render_layers_clicked()
{
	if (!slicerBusy)
	{
		SynchronizeInterfaceWindow(this, gPar, qInterface::read);
		CVector3 limitMin;
		CVector3 limitMax;
		if (gPar->Get<bool>("voxel_custom_limit_enabled"))
		{
			limitMin = gPar->Get<CVector3>("voxel_limit_min");
			limitMax = gPar->Get<CVector3>("voxel_limit_max");
		}
		else
		{
			limitMin = gPar->Get<CVector3>("limit_min");
			limitMax = gPar->Get<CVector3>("limit_max");
		}
		int maxIter = gPar->Get<int>("voxel_max_iter");
		QString folderString = gPar->Get<QString>("voxel_image_path");
		int samplesX = gPar->Get<int>("voxel_samples_x");
		int samplesY = gPar->Get<int>("voxel_samples_y");
		int samplesZ = gPar->Get<int>("voxel_samples_z");

		QDir folder(folderString);
		if (folder.exists())
		{
			slicerBusy = true;
			voxelExport =
				new cVoxelExport(samplesX, samplesY, samplesZ, limitMin, limitMax, folder, maxIter);
			QObject::connect(voxelExport,
				SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
				SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));

			QThread *thread = new QThread; // deleted by deleteLater()
			voxelExport->moveToThread(thread);
			QObject::connect(thread, SIGNAL(started()), voxelExport, SLOT(ProcessVolume()));
			thread->setObjectName("Slicer");
			thread->start();
			QObject::connect(voxelExport, SIGNAL(finished()), this, SLOT(slotSlicerFinished()));
			QObject::connect(voxelExport, SIGNAL(finished()), voxelExport, SLOT(deleteLater()));
			QObject::connect(voxelExport, SIGNAL(finished()), thread, SLOT(quit()));
			QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

			ui->pushButton_start_render_layers->setEnabled(false);
			ui->pushButton_stop_render_layers->setEnabled(true);
		}
		else
		{
			cErrorMessage::showMessage(
				QObject::tr("Cannot start voxel export. Specified folder (%1) does not exist.")
					.arg(folderString),
				cErrorMessage::errorMessage);
		}
	}
	else
	{
		cErrorMessage::showMessage(
			QObject::tr(
				"Voxel Export is busy. Stop unfinished rendering before starting new voxel export job."),
			cErrorMessage::errorMessage);
	}
}

void cVoxelExportDialog::on_pushButton_stop_render_layers_clicked() const
{
	if (slicerBusy)
	{
		voxelExport->Stop();
	}
}

void cVoxelExportDialog::on_pushButton_select_image_path_clicked()
{
	QString dir = QDir::toNativeSeparators(
		QFileDialog::getExistingDirectory(this, tr("Select path for slice images"),
			QDir::toNativeSeparators(ui->text_voxel_image_path->text())));

	if (dir.length() > 0)
	{
		ui->text_voxel_image_path->setText(dir);
	}
}

void cVoxelExportDialog::on_pushButton_show_layers_clicked()
{
	WriteLog("Prepare PlayerWidget class", 2);
	SynchronizeInterfaceWindow(this, gPar, qInterface::read);
	if (!gMainInterface->imageSequencePlayer)
	{
		gMainInterface->imageSequencePlayer = new PlayerWidget;
	}

	gMainInterface->imageSequencePlayer->SetFilePath(gPar->Get<QString>("voxel_image_path"));
	gMainInterface->imageSequencePlayer->show();
	gMainInterface->imageSequencePlayer->raise();
	gMainInterface->imageSequencePlayer->activateWindow();
}

void cVoxelExportDialog::slotSlicerFinished()
{
	slicerBusy = false;
	ui->pushButton_start_render_layers->setEnabled(true);
	ui->pushButton_stop_render_layers->setEnabled(false);
}

void cVoxelExportDialog::slotUpdateProgressAndStatus(
	const QString &text, const QString &progressText, double progress) const
{
	ui->label_info->setText(text);
	if (!ui->progressBar->isVisible()) ui->progressBar->setVisible(true);
	ui->progressBar->setValue(int(progress * 1000.0));
	ui->progressBar->setTextVisible(true);
	ui->progressBar->setFormat(progressText);
}

void cVoxelExportDialog::closeEvent(QCloseEvent *event)
{
	if (!slicerBusy)
	{
		event->accept();
	}
	else
	{
		cErrorMessage::showMessage(
			QObject::tr(
				"Voxel Export is busy. Stop unfinished rendering before closing the voxel export dialog."),
			cErrorMessage::errorMessage);
		event->ignore();
	}
}

void cVoxelExportDialog::showEvent(QShowEvent *event)
{
	(void)event;
	SynchronizeInterfaceWindow(this, gPar, qInterface::write);
}
