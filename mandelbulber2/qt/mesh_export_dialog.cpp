/*
 * Copyright (C) Stanislaw Adaszewski, 2016
 * http://algoholic.eu
 */

#include "mesh_export_dialog.h"

#include "../qt/player_widget.hpp"
#include "../src/automated_widgets.hpp"
#include "../src/error_message.hpp"
#include "../src/initparameters.hpp"
#include "../src/interface.hpp"
#include "../src/system.hpp"
#include "../src/mesh_export.hpp"
#include "ui_mesh_export_dialog.h"

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
        meshExport = NULL;
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
                QString outFname = gPar->Get<QString>("mesh_output_filename");
		int samplesX = gPar->Get<int>("voxel_samples_x");
		int samplesY = gPar->Get<int>("voxel_samples_y");
		int samplesZ = gPar->Get<int>("voxel_samples_z");

                QFileInfo fi(outFname);
                if (fi.exists())
		{
                    if (QMessageBox::question(this, tr("Warning"), tr("Specified file already exists. Overwrite?")) != QMessageBox::Yes) {
                        /* cErrorMessage::showMessage(
                                QObject::tr("Cannot start mesh export. Specified file (%1) already exists.")
                                        .arg(fi.path()),
                                cErrorMessage::errorMessage); */
                        return;
                    }


                }

                slicerBusy = true;
                meshExport =
                        new cMeshExport(samplesX, samplesY, samplesZ, limitMin, limitMax, fi.absoluteFilePath(), maxIter);
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

void cMeshExportDialog::on_pushButton_stop_render_layers_clicked()
{
	if (slicerBusy)
	{
                meshExport->Stop();
	}
}

void cMeshExportDialog::on_pushButton_select_image_path_clicked()
{
    QString fname = QFileDialog::getSaveFileName(this, tr("Select path for mesh output"),
        ui->text_mesh_output_filename->text());

    if (!fname.isEmpty())
    {
        ui->text_mesh_output_filename->setText(fname);
    }
}


void cMeshExportDialog::slotSlicerFinished()
{
	slicerBusy = false;
	ui->pushButton_start_render_layers->setEnabled(true);
	ui->pushButton_stop_render_layers->setEnabled(false);
}

void cMeshExportDialog::slotUpdateProgressAndStatus(
	const QString &text, const QString &progressText, double progress)
{
	ui->label_info->setText(text);
	if (!ui->progressBar->isVisible()) ui->progressBar->setVisible(true);
	ui->progressBar->setValue((int)(progress * 1000.0));
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
