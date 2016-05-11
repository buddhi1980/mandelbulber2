#include "voxel_export_dialog.h"
#include "ui_voxel_export_dialog.h"
#include <QtCore>
#include <QFileDialog>
#include <QMessageBox>
#include "../src/interface.hpp"
#include "../src/initparameters.hpp"
#include "../src/error_message.hpp"

cVoxelExportDialog::cVoxelExportDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cVoxelExportDialog)
{
	initFinished = false;
	slicerBusy = false;
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	initFinished = true;
	ui->progressBar->hide();
	voxelExport = NULL;
}

cVoxelExportDialog::~cVoxelExportDialog()
{
  delete ui;
}

void cVoxelExportDialog::on_pushButton_start_render_layers_clicked()
{
	if(!slicerBusy)
	{
		SynchronizeInterfaceWindow(this, gPar, qInterface::read);
		slicerBusy = true;
		CVector3 limitMin;
		CVector3 limitMax;
		if(gPar->Get<bool>("voxel_custom_limit_enabled"))
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
		QString folder = gPar->Get<QString>("voxel_image_path");
		int samplesX = gPar->Get<int>("voxel_samples_x");
		int samplesY = gPar->Get<int>("voxel_samples_y");
		int samplesZ = gPar->Get<int>("voxel_samples_z");
		voxelExport = new cVoxelExport(samplesX, samplesY, samplesZ, limitMin, limitMax, folder, maxIter);
		QObject::connect(voxelExport,
			SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)),
			this,
			SLOT(slotUpdateProgressAndStatus(const QString&, const QString&, double)));

		QThread *thread = new QThread; //deleted by deleteLater()
		voxelExport->moveToThread(thread);
		QObject::connect(thread, SIGNAL(started()), voxelExport, SLOT(ProcessVolume()));
		thread->setObjectName("Slicer");
		thread->start();
		QObject::connect(voxelExport, SIGNAL(finished()), thread, SLOT(deleteLater()));
		QObject::connect(voxelExport, SIGNAL(finished()), voxelExport, SLOT(deleteLater()));
		QObject::connect(voxelExport, SIGNAL(finished()), this, SLOT(slotSlicerFinished()));

		ui->pushButton_start_render_layers->setEnabled(false);
		ui->pushButton_stop_render_layers->setEnabled(true);
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Voxel Export is busy. Stop unfinished rendering before starting new voxel export job."),
			cErrorMessage::errorMessage);
	}
}

void cVoxelExportDialog::on_pushButton_stop_render_layers_clicked()
{
	if(slicerBusy)
	{
		voxelExport->Stop();
	}
}

void cVoxelExportDialog::on_pushButton_select_image_path_clicked()
{
	QString dir = QDir::toNativeSeparators(
		QFileDialog::getExistingDirectory(
			this,
			tr("Select path for slice images"),
			QDir::toNativeSeparators(ui->text_voxel_image_path->text())
		)
	);

	if(dir.length() > 0)
	{
		ui->text_voxel_image_path->setText(dir);
	}
}

void cVoxelExportDialog::on_pushButton_show_layers_clicked()
{
	WriteLog("Prepare PlayerWidget class", 2);
	SynchronizeInterfaceWindow(this, gPar, qInterface::read);
	if(!gMainInterface->imageSequencePlayer)
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

void cVoxelExportDialog::slotUpdateProgressAndStatus(const QString &text,
		const QString &progressText, double progress)
{
	ui->label_info->setText(text);
	if (!ui->progressBar->isVisible()) ui->progressBar->setVisible(true);
	ui->progressBar->setValue(progress * 1000.0);
	ui->progressBar->setTextVisible(true);
	ui->progressBar->setFormat(progressText);
}

void cVoxelExportDialog::closeEvent(QCloseEvent * event)
{
	if (!slicerBusy)
	{
		event->accept();
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Voxel Export is busy. Stop unfinished rendering before closing the voxel export dialog."),
			cErrorMessage::errorMessage);
		event->ignore();
	}
}

void cVoxelExportDialog::showEvent(QShowEvent * event)
{
	(void) event;
	SynchronizeInterfaceWindow(this, gPar, qInterface::write);
}
