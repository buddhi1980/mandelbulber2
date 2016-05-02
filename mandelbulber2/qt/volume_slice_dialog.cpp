#include "volume_slice_dialog.h"
#include "ui_volume_slice_dialog.h"
#include <QtCore>
#include <QFileDialog>
#include <QMessageBox>

cVolumeSliceDialog::cVolumeSliceDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cVolumeSliceDialog)
{
	initFinished = false;
	slicerBusy = false;
	ui->setupUi(this);
	gMainInterface->ConnectSignalsForSlidersInWindow(this);
	initFinished = true;
	ui->progressBar->hide();
	volumeSlicer = NULL;
}

cVolumeSliceDialog::~cVolumeSliceDialog()
{
  delete ui;
}

void cVolumeSliceDialog::on_pushButton_start_render_slices_clicked()
{
	if(!slicerBusy)
	{
		gMainInterface->SynchronizeInterfaceWindow(this, gPar, cInterface::read);
		slicerBusy = true;
		CVector3 limitMin;
		CVector3 limitMax;
		if(gPar->Get<bool>("volume_slice_custom_limit_enabled"))
		{
			limitMin = gPar->Get<CVector3>("volume_slice_limit_min");
			limitMax = gPar->Get<CVector3>("volume_slice_limit_max");
		}
		else
		{
			limitMin = gPar->Get<CVector3>("limit_min");
			limitMax = gPar->Get<CVector3>("limit_max");
		}
		int maxIter = gPar->Get<int>("volume_slice_max_iter");
		QString folder = gPar->Get<QString>("volume_slice_image_path");
		int samplesX = gPar->Get<int>("volume_slice_samples_x");
		int samplesY = gPar->Get<int>("volume_slice_samples_y");
		int samplesZ = gPar->Get<int>("volume_slice_samples_z");
		volumeSlicer = new cVolumeSlicer(samplesX, samplesY, samplesZ, limitMin, limitMax, folder, maxIter);
		QObject::connect(volumeSlicer,
			SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)),
			this,
			SLOT(slotUpdateProgressAndStatus(const QString&, const QString&, double)));

		QThread *thread = new QThread; //deleted by deleteLater()
		volumeSlicer->moveToThread(thread);
		QObject::connect(thread, SIGNAL(started()), volumeSlicer, SLOT(ProcessVolume()));
		thread->setObjectName("Slicer");
		thread->start();
		QObject::connect(volumeSlicer, SIGNAL(finished()), thread, SLOT(deleteLater()));
		QObject::connect(volumeSlicer, SIGNAL(finished()), volumeSlicer, SLOT(deleteLater()));
		QObject::connect(volumeSlicer, SIGNAL(finished()), this, SLOT(slotSlicerFinished()));

		ui->pushButton_start_render_slices->setEnabled(false);
		ui->pushButton_stop_render_slices->setEnabled(true);
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Volume Slicer is busy. Stop unfinished rendering before starting new slice job."),
			cErrorMessage::errorMessage);
	}
}

void cVolumeSliceDialog::on_pushButton_stop_render_slices_clicked()
{
	if(slicerBusy)
	{
		volumeSlicer->Stop();
	}
}

void cVolumeSliceDialog::on_pushButton_select_image_path_clicked()
{
	QString dir = QDir::toNativeSeparators(
		QFileDialog::getExistingDirectory(
			this,
			tr("Select path for slice images"),
			QDir::toNativeSeparators(ui->text_volume_slice_image_path->text())
		)
	);

	if(dir.length() > 0)
	{
		ui->text_volume_slice_image_path->setText(dir);
	}
}

void cVolumeSliceDialog::on_pushButton_show_slices_clicked()
{
	WriteLog("Prepare PlayerWidget class");
	gMainInterface->SynchronizeInterfaceWindow(this, gPar, cInterface::read);
	if(!gMainInterface->imageSequencePlayer)
	{
		gMainInterface->imageSequencePlayer = new PlayerWidget;
	}

	gMainInterface->imageSequencePlayer->SetFilePath(gPar->Get<QString>("volume_slice_image_path"));
	gMainInterface->imageSequencePlayer->show();
	gMainInterface->imageSequencePlayer->raise();
	gMainInterface->imageSequencePlayer->activateWindow();
}

void cVolumeSliceDialog::slotSlicerFinished()
{
	slicerBusy = false;
	ui->pushButton_start_render_slices->setEnabled(true);
	ui->pushButton_stop_render_slices->setEnabled(false);
}

void cVolumeSliceDialog::slotUpdateProgressAndStatus(const QString &text,
		const QString &progressText, double progress)
{
	ui->label_info->setText(text);
	if (!ui->progressBar->isVisible()) ui->progressBar->setVisible(true);
	ui->progressBar->setValue(progress * 1000.0);
	ui->progressBar->setTextVisible(true);
	ui->progressBar->setFormat(progressText);
}

void cVolumeSliceDialog::closeEvent(QCloseEvent * event)
{
	if (!slicerBusy)
	{
		event->accept();
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Volume Slicer is busy. Stop unfinished rendering before closing the volume slice dialog."),
			cErrorMessage::errorMessage);
		event->ignore();
	}
}

void cVolumeSliceDialog::showEvent(QShowEvent * event)
{
	(void) event;
	gMainInterface->SynchronizeInterfaceWindow(this, gPar, cInterface::write);
}
