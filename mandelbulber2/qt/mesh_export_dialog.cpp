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
 * Authors: Stanislaw Adaszewski
 *
 * cMeshExportDialog -a dialog for exporting the fractal volume in ply format.
 */

#include "mesh_export_dialog.h"

#include "ui_mesh_export_dialog.h"

#include "player_widget.hpp"

#include "src/automated_widgets.hpp"
#include "src/error_message.hpp"
#include "src/file_mesh.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/mesh_export.hpp"
#include "src/system.hpp"

cMeshExportDialog::cMeshExportDialog(QWidget *parent)
		: QDialog(parent), ui(new Ui::cMeshExportDialog)
{
	initFinished = false;
	slicerBusy = false;
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	initFinished = true;
	ui->progressBar->hide();
	meshExport = nullptr;
}

cMeshExportDialog::~cMeshExportDialog()
{
	delete ui;
}

void cMeshExportDialog::on_pushButton_start_render_layers_clicked()
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
		QString outputFileName = gPar->Get<QString>("mesh_output_filename");
		int samplesX = gPar->Get<int>("voxel_samples_x");
		int samplesY = gPar->Get<int>("voxel_samples_y");
		int samplesZ = gPar->Get<int>("voxel_samples_z");

		QFileInfo fi(outputFileName);
		if (fi.exists())
		{
			if (QMessageBox::question(
						this, tr("Warning"), tr("Specified file already exists. Overwrite?"))
					!= QMessageBox::Yes)
			{
				/* cErrorMessage::showMessage(
								QObject::tr("Cannot start mesh export. Specified file (%1) already exists.")
												.arg(fi.path()),
								cErrorMessage::errorMessage); */
				return;
			}
		}
		QList<MeshFileSave::enumMeshContentType> meshContent({MeshFileSave::MESH_CONTENT_GEOMETRY});
		if (gPar->Get<bool>("mesh_color")) meshContent << MeshFileSave::MESH_CONTENT_COLOR;
		MeshFileSave::structSaveMeshConfig meshConfig(MeshFileSave::MESH_FILE_TYPE_PLY, meshContent,
			MeshFileSave::enumMeshFileModeType(gPar->Get<int>("mesh_file_mode")));

		slicerBusy = true;
		meshExport = new cMeshExport(
			samplesX, samplesY, samplesZ, limitMin, limitMax, fi.absoluteFilePath(), maxIter, meshConfig);
		QObject::connect(meshExport,
			SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
			SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));

		QThread *thread = new QThread; // deleted by deleteLater()
		meshExport->moveToThread(thread);
		QObject::connect(thread, SIGNAL(started()), meshExport, SLOT(ProcessVolume()));
		thread->setObjectName("MeshExport");
		thread->start();
		QObject::connect(meshExport, SIGNAL(finished()), this, SLOT(slotSlicerFinished()));
		QObject::connect(meshExport, SIGNAL(finished()), meshExport, SLOT(deleteLater()));
		QObject::connect(meshExport, SIGNAL(finished()), thread, SLOT(quit()));
		QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

		ui->pushButton_start_render_layers->setEnabled(false);
		ui->pushButton_stop_render_layers->setEnabled(true);
	}
	else
	{
		cErrorMessage::showMessage(
			QObject::tr(
				"Voxel Export is busy. Stop unfinished rendering before starting new voxel export job."),
			cErrorMessage::errorMessage);
	}
}

void cMeshExportDialog::on_pushButton_stop_render_layers_clicked() const
{
	if (slicerBusy)
	{
		meshExport->Stop();
	}
}

void cMeshExportDialog::on_pushButton_select_image_path_clicked()
{
	QString outputFileName = QFileDialog::getSaveFileName(
		this, tr("Select path for mesh output"), ui->text_mesh_output_filename->text());

	if (!outputFileName.isEmpty())
	{
		ui->text_mesh_output_filename->setText(outputFileName);
	}
}

void cMeshExportDialog::slotSlicerFinished()
{
	slicerBusy = false;
	ui->pushButton_start_render_layers->setEnabled(true);
	ui->pushButton_stop_render_layers->setEnabled(false);
}

void cMeshExportDialog::slotUpdateProgressAndStatus(
	const QString &text, const QString &progressText, double progress) const
{
	ui->label_info->setText(text);
	if (!ui->progressBar->isVisible()) ui->progressBar->setVisible(true);
	ui->progressBar->setValue(int(progress * 1000.0));
	ui->progressBar->setTextVisible(true);
	ui->progressBar->setFormat(progressText);
}

void cMeshExportDialog::closeEvent(QCloseEvent *event)
{
	if (!slicerBusy)
	{
		event->accept();
	}
	else
	{
		cErrorMessage::showMessage(
			QObject::tr(
				"Mesh Export is busy. Stop unfinished rendering before closing the mesh export dialog."),
			cErrorMessage::errorMessage);
		event->ignore();
	}
}

void cMeshExportDialog::showEvent(QShowEvent *event)
{
	(void)event;
	SynchronizeInterfaceWindow(this, gPar, qInterface::write);
}
