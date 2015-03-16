/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * RenderWindow class - main program window
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */


#include "render_window.hpp"
#include "interface.hpp"
#include "fractal_list.hpp"
#include "system.hpp"
#include "settings.hpp"
#include "error_message.hpp"
#include "files.h"
#include "fractparams.hpp"
#include "thumbnail.hpp"
#include "undo.h"
#include "old_settings.hpp"

#include <QtGui>
#include <QtUiTools/QtUiTools>
#include <QDial>
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>

#include "common_math.h"
#include "my_ui_loader.h"
#include "preview_file_dialog.h"
#include "global_data.hpp"

#include "progress_text.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

RenderWindow::RenderWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RenderWindow)
{
    ui->setupUi(this);
  	fractalWidgets = new QWidget*[4];

  	//store defauly geometry and state
  	defaultGeometry = saveGeometry();
  	defaultState = saveState();
		for(int i = 0; i < 256; i++){
			histogramDE[i] = 0;
			histogramIters[i] = 0;
		}
		countHistogramDE = 0;
		countHistogramIters = 0;
}

RenderWindow::~RenderWindow()
{
    delete ui;
    for(int i=0; i<NUMBER_OF_FRACTALS; i++)
    	delete fractalWidgets[i];
    delete[] fractalWidgets;
}


void RenderWindow::slotStartRender(void)
{
	gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	gUndo.Store(gPar, gParFractal);
	gMainInterface->StartRender();
}

void RenderWindow::slotStopRender(void)
{
	gMainInterface->stopRequest = true;
}

void RenderWindow::slotDoubleSpinBoxChanged(double value)
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
	QString sliderName = QString("slider_") + parameterName;

	QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);
	if (slider)
	{
		slider->setValue(value * 100.0);
	}
	else
	{
		qWarning() << "slotDoubleSpinBoxChanged() error: slider " << sliderName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotLogLineEditChanged(const QString &text)
{
	using namespace std;
	QString lineEditName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(lineEditName, &parameterName, &type);
	QString sliderName = QString("logslider_") + parameterName;

	QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);
	if (slider)
	{
		double value = text.toDouble();
		if(value > 0.0)
		{
			int sliderPosition = log10(text.toDouble()) * 100.0;
			slider->setValue(sliderPosition);
		}
		else
		{
			qWarning() << "slotLogLineEditChanged() error: value from " << lineEditName << " is not greater zero" << endl;
		}
	}
	else
	{
		qWarning() << "slotLogLineEditChanged() error: slider " << sliderName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotIntSpinBoxChanged(int value)
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
	QString sliderName = QString("sliderInt_") + parameterName;

	QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);
	if (slider)
	{
		slider->setValue(value);
	}
	else
	{
		qWarning() << "slotIntSpinBoxChanged() error: slider " << sliderName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotSpinBox3Changed(double value)
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
	QString sliderName = QString("slider3_") + parameterName;

	QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);
	if (slider)
	{
		slider->setValue(value * 100.0);
	}
	else
	{
		qWarning() << "slotSpinBox3Changed() error: slider " << sliderName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotSpinBoxD3Changed(double value)
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
	QString dialName = QString("dial3_") + parameterName;

	QDial *dial = this->sender()->parent()->findChild<QDial*>(dialName);
	if (dial)
	{
		dial->setValue(value * 100.0);
	}
	else
	{
		qWarning() << "slotSpinBox3Changed() error: slider " << dialName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotSpinBoxDChanged(double value)
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
	QString dialName = QString("dial_") + parameterName;

	QDial *dial = this->sender()->parent()->findChild<QDial*>(dialName);
	if (dial)
	{
		dial->setValue(value * 100.0);
	}
	else
	{
		qWarning() << "slotSpinBoxChanged() error: slider " << dialName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotSliderMoved(int value)
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinbox_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);
	if(spinBox)
	{
		spinBox->setValue(value/100.0);
	}
	else
	{
		qWarning() << "slotSliderMoved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotLogSliderMoved(int value)
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(sliderName, &parameterName, &type);
	QString lineEditName = QString("logedit_") + parameterName;

	QLineEdit *lineEdit = this->sender()->parent()->findChild<QLineEdit*>(lineEditName);
	if(lineEdit)
	{
		double dValue = pow(10.0, value/100.0);
		QString text = QString::number(dValue);
		lineEdit->setText(text);
	}
	else
	{
		qWarning() << "slotLogSliderMoved() error: lineEdit " << lineEditName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotIntSliderMoved(int value)
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(sliderName, &parameterName, &type);
	QString spinboxName = QString("spinboxInt_") + parameterName;

	QSpinBox *spinbox = this->sender()->parent()->findChild<QSpinBox*>(spinboxName);
	if(spinbox)
	{
		spinbox->setValue(value);
	}
	else
	{
		qWarning() << "slotLogSliderMoved() error: lineEdit " << spinboxName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotSlider3Moved(int value)
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinbox3_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);
	if(spinBox)
	{
		spinBox->setValue(value/100.0);
	}
	else
	{
		qWarning() << "slotSlider3Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotDial3Moved(int value)
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinboxd3_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);
	if(spinBox)
	{
		spinBox->setValue(value/100.0);
	}
	else
	{
		qWarning() << "slotDial3Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotDialMoved(int value)
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinboxd_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);
	if(spinBox)
	{
		spinBox->setValue(value/100.0);
	}
	else
	{
		qWarning() << "slotDialMoved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotMenuSaveDocksPositions()
{
	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState", saveState());
	//qDebug() << "settings saved";
}

void RenderWindow::slotMenuResetDocksPositions()
{
	restoreGeometry(defaultGeometry);
	restoreState(defaultState);
}

void RenderWindow::slotChangedComboFractal(int index)
{
	QString comboName = this->sender()->objectName();
	int fractalNumber = comboName.right(1).toInt() - 1;

	if(fractalList[index].internalID > 0)
	{
		QString formulaName = fractalList[index].internalNane;
		QString uiFilename = systemData.sharedDir + "qt_data" + QDir::separator() + "fractal_" + formulaName + ".ui";

		if(fractalWidgets[fractalNumber]) delete fractalWidgets[fractalNumber];
		fractalWidgets[fractalNumber] = NULL;

		MyUiLoader loader;
		QFile uiFile(uiFilename);

		if(uiFile.exists())
		{
			uiFile.open(QFile::ReadOnly);
			fractalWidgets[fractalNumber] = loader.load(&uiFile);
			QVBoxLayout *layout = ui->dockWidget_fractal->findChild<QVBoxLayout*>("verticalLayout_fractal_" + QString::number(fractalNumber + 1));
			layout->addWidget(fractalWidgets[fractalNumber]);
			uiFile.close();
			fractalWidgets[fractalNumber]->show();
			gMainInterface->ConnectSignalsForSlidersInWindow(fractalWidgets[fractalNumber]);
			gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[fractalNumber], &gParFractal->at(fractalNumber), cInterface::write);

			if(fractalList[index].internalID == fractal::kaleidoscopicIFS)
			{
				QWidget *pushButton_preset_dodecahedron = fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_dodecahedron");
				QApplication::connect(pushButton_preset_dodecahedron, SIGNAL(clicked()), this, SLOT(slotPressedButtonIFSDefaultsDodecahedron()));
				QWidget *pushButton_preset_icosahedron = fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_icosahedron");
				QApplication::connect(pushButton_preset_icosahedron, SIGNAL(clicked()), this, SLOT(slotPressedButtonIFSDefaultsIcosahedron()));
				QWidget *pushButton_preset_octahedron = fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_octahedron");
				QApplication::connect(pushButton_preset_octahedron, SIGNAL(clicked()), this, SLOT(slotPressedButtonIFSDefaultsOctahedron()));
				QWidget *pushButton_preset_menger_sponge = fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_menger_sponge");
				QApplication::connect(pushButton_preset_menger_sponge, SIGNAL(clicked()), this, SLOT(slotPressedButtonIFSDefaultsMengerSponge()));
				QWidget *pushButton_preset_reset = fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_reset");
				QApplication::connect(pushButton_preset_reset, SIGNAL(clicked()), this, SLOT(slotPressedButtonIFSDefaultsReset()));
			}
		}
		else
		{
			cErrorMessage::showMessage(QString("Can't open file ") + uiFilename + QString("\nFractal ui file can't be loaded"), cErrorMessage::errorMessage, gMainInterface->mainWindow);
		}
	}
	else
	{
		if(fractalWidgets[fractalNumber]) delete fractalWidgets[fractalNumber];
		fractalWidgets[fractalNumber] = NULL;
	}

}

void RenderWindow::slotResizedScrolledAreaImage(int width, int height)
{
	if (gMainInterface->mainImage)
	{
		int selectedScale = ui->comboBox_image_preview_scale->currentIndex();

		if (selectedScale == 0)
		{
			double scale = CalcMainImageScale(0.0, width, height, gMainInterface->mainImage);
			gMainInterface->mainImage->CreatePreview(scale, width, height, gMainInterface->renderedImage);
			gMainInterface->mainImage->UpdatePreview();
			gMainInterface->renderedImage->setMinimumSize(gMainInterface->mainImage->GetPreviewWidth(), gMainInterface->mainImage->GetPreviewHeight());
		}
	}
}

void RenderWindow::slotChangedComboImageScale(int index)
{
	if (gMainInterface->mainImage)
	{
		double scale = ImageScaleComboSelection2Double(index);
		int areaWidth = ui->scrollAreaForImage->VisibleAreaWidth();
		int areaHeight = ui->scrollAreaForImage->VisibleAreaHeight();
		scale = CalcMainImageScale(scale, areaWidth, areaHeight, gMainInterface->mainImage);

		gMainInterface->mainImage->CreatePreview(scale, areaWidth, areaHeight, gMainInterface->renderedImage);
		gMainInterface->mainImage->UpdatePreview();
		gMainInterface->renderedImage->setMinimumSize(gMainInterface->mainImage->GetPreviewWidth(), gMainInterface->mainImage->GetPreviewHeight());
	}
}

void RenderWindow::slotCameraMove()
{
	QString buttonName = this->sender()->objectName();
	gMainInterface->MoveCamera(buttonName);
}

void RenderWindow::slotCameraRotation()
{
	QString buttonName = this->sender()->objectName();
	gMainInterface->RotateCamera(buttonName);
}

void RenderWindow::slotCameraOrTargetEdited()
{
	gMainInterface->CameraOrTargetEdited();
}

void RenderWindow::slotRotationEdited()
{
	gMainInterface->RotationEdited();
}

void RenderWindow::slotCameraDistanceEdited()
{
	gMainInterface->CameraDistanceEdited();
}

void RenderWindow::slotCameraDistanceSlider(int value)
{
	(void)value;
	gMainInterface->CameraDistanceEdited();
}

void RenderWindow::slotChangedCheckBoxHybridFractal(int state)
{
	if(state)	ui->groupCheck_boolean_operators->setChecked(false);
	gApplication->processEvents();

	ui->label_formula_iterations_1->setVisible(state);
	ui->spinboxInt_formula_iterations_1->setVisible(state);
	ui->sliderInt_formula_iterations_1->setVisible(state);
	ui->label_formula_iterations_2->setVisible(state);
	ui->spinboxInt_formula_iterations_2->setVisible(state);
	ui->sliderInt_formula_iterations_2->setVisible(state);
	ui->label_formula_iterations_3->setVisible(state);
	ui->spinboxInt_formula_iterations_3->setVisible(state);
	ui->sliderInt_formula_iterations_3->setVisible(state);
	ui->label_formula_iterations_4->setVisible(state);
	ui->spinboxInt_formula_iterations_4->setVisible(state);
	ui->sliderInt_formula_iterations_4->setVisible(state);

	ui->frame_iterations_formula_2->setEnabled(state);
	ui->frame_iterations_formula_3->setEnabled(state);
	ui->frame_iterations_formula_4->setEnabled(state);
	ui->scrollArea_fractal_2->setEnabled(state);
	ui->scrollArea_fractal_3->setEnabled(state);
	ui->scrollArea_fractal_4->setEnabled(state);
}

void RenderWindow::slotChangedCheckBoxBooleanOperators(bool state)
{
	if(state)	ui->checkBox_hybrid_fractal_enable->setChecked(false);
	gApplication->processEvents();

	ui->frame_iterations_formula_2->setEnabled(state);
	ui->frame_iterations_formula_3->setEnabled(state);
	ui->frame_iterations_formula_4->setEnabled(state);
	ui->scrollArea_fractal_2->setEnabled(state);
	ui->scrollArea_fractal_3->setEnabled(state);
	ui->scrollArea_fractal_4->setEnabled(state);
	ui->groupBox_formula_transform_1->setVisible(state);
	ui->groupBox_formula_transform_2->setVisible(state);
	ui->groupBox_formula_transform_3->setVisible(state);
	ui->groupBox_formula_transform_4->setVisible(state);
}

void RenderWindow::slotMenuSaveSettings()
{
	cSettings parSettings(cSettings::formatCondensedText);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames);

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(systemData.dataDirectory + QDir::separator() + "settings" + QDir::separator());
	dialog.selectFile(systemData.lastSettingsFile);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save settings..."));
	dialog.setDefaultSuffix("fract");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		parSettings.SaveToFile(filename);
		systemData.lastSettingsFile = filename;
		this->setWindowTitle(QString("Mandelbulber (") + filename + ")");

		//QString hash = parSettings.GetHashCode();
		//cThumbnail thumbnail(gPar, gParFractal, 200, 200);
		//thumbnail.Render();
		//thumbnail.Save(systemData.thumbnailDir + hash);
	}
}

void RenderWindow::slotMenuLoadSettings()
{
	gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read); //update appParam before loading new settings

	cSettings parSettings(cSettings::formatFullText);

	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(systemData.dataDirectory + QDir::separator() + "settings" + QDir::separator());
	dialog.selectFile(systemData.lastSettingsFile);
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load settings..."));
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		parSettings.LoadFromFile(filename);
		parSettings.Decode(gPar, gParFractal, gAnimFrames);
		gMainInterface->RebuildPrimitives(gPar);
		gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
		gMainInterface->ComboMouseClickUpdate();
		systemData.lastSettingsFile = filename;
		this->setWindowTitle(QString("Mandelbulber (") + filename + ")");
		gFlightAnimation->RefreshTable();
	}
}

void RenderWindow::slotMenuLoadExample()
{
	cSettings parSettings(cSettings::formatFullText);

	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(systemData.sharedDir + QDir::separator() + "examples" + QDir::separator());
	dialog.selectFile(QFileInfo(systemData.lastSettingsFile).fileName());
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load example settings..."));
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		parSettings.LoadFromFile(filename);
		parSettings.Decode(gPar, gParFractal, gAnimFrames);
		gMainInterface->RebuildPrimitives(gPar);
		gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
		gMainInterface->ComboMouseClickUpdate();
		systemData.lastSettingsFile = systemData.dataDirectory + "settings" + QDir::separator() + QFileInfo(filename).fileName();
		this->setWindowTitle(QString("Mandelbulber (") + systemData.lastSettingsFile + ")");
	}
}

void RenderWindow::slotImportOldSettings()
{
	cSettings parSettings(cSettings::formatFullText);

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(systemData.dataDirectory + QDir::separator() + "settings" + QDir::separator());
	dialog.selectFile(systemData.lastSettingsFile);
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Import settings from old Mandelbulber (v1.21)..."));
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		oldSettings::cOldSettings oldSettings;
		oldSettings.LoadSettings(filename);
		oldSettings.ConvertToNewContainer(gPar, gParFractal);
		gMainInterface->RebuildPrimitives(gPar);
		gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
		gMainInterface->ComboMouseClickUpdate();
		systemData.lastSettingsFile = filename;
		this->setWindowTitle(QString("Mandelbulber (") + filename + ")");
	}
}


void RenderWindow::slotPressedButtonIFSDefaultsDodecahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), cInterface::read);
	gMainInterface->IFSDefaultsDodecahedron(&gParFractal->at(index));
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsIcosahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), cInterface::read);
	gMainInterface->IFSDefaultsIcosahedron(&gParFractal->at(index));
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsOctahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), cInterface::read);
	gMainInterface->IFSDefaultsOctahedron(&gParFractal->at(index));
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsMengerSponge()
{
	int index = ui->tabWidget_fractals->currentIndex();
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), cInterface::read);
	gMainInterface->IFSDefaultsMengerSponge(&gParFractal->at(index));
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsReset()
{
	int index = ui->tabWidget_fractals->currentIndex();
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), cInterface::read);
	gMainInterface->IFSDefaultsReset(&gParFractal->at(index));
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), cInterface::write);
}

void RenderWindow::slotMenuAboutQt()
{
	QMessageBox::aboutQt(this);
}

void RenderWindow::slotMenuAboutMandelbulber()
{
	QString text = "Mandelbulber\n";
	text += "version: " + QString(MANDELBULBER_VERSION_STRING) + ", build date: " + QString(__DATE__) + QString("\n");
	text += "\n";
	text += "Licence: GNU GPL version 3.0\n";
	text += "Copyright Ⓒ 2014\n";
	text += "project leader: Krzysztof Marczak\n";
	text += "commiters:\n";
	text +=	"Sebastian Jennen\n";
	text += "\n";
	text += "Thanks to many friends from www.fractalforums.com for help\n";
	text += "\n";
	text += "www.mandelbulber.com";

	QMessageBox::about(this, "About Mandelbulber", text);
}

void RenderWindow::slotMenuSaveImageJPEG()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("JPEG images (*.jpg *.jpeg)"));
	dialog.setDirectory(QFileInfo(systemData.lastImageFile).absolutePath());
	dialog.selectFile(QFileInfo(systemData.lastImageFile).completeBaseName());
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("JPEG"));
	dialog.setDefaultSuffix("jpeg");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		ProgressStatusText(tr("Saving %1 image").arg("JPG"), tr("Saving image started"), 0.0, ui->statusbar, gMainInterface->progressBar);
		gApplication->processEvents();
		SaveJPEGQt(filename, gMainInterface->mainImage->ConvertTo8bit(), gMainInterface->mainImage->GetWidth(), gMainInterface->mainImage->GetHeight(), 95);
		ProgressStatusText(tr("Saving %1 image").arg("JPG"), tr("Saving image finished"), 1.0, ui->statusbar, gMainInterface->progressBar);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

void RenderWindow::slotMenuSaveImagePNG8()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("PNG images (*.png)"));
	dialog.setDirectory(QFileInfo(systemData.lastImageFile).absolutePath());
	dialog.selectFile(QFileInfo(systemData.lastImageFile).completeBaseName());
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("8-bit PNG"));
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		ProgressStatusText(tr("Saving %1 image").arg("PNG"), tr("Saving PNG image started"), 0.0, ui->statusbar, gMainInterface->progressBar);
		gApplication->processEvents();
		SavePNG(filename, gMainInterface->mainImage->GetWidth(), gMainInterface->mainImage->GetHeight(), gMainInterface->mainImage->ConvertTo8bit());
		ProgressStatusText(tr("Saving %1 image").arg("PNG"), tr("Saving PNG image finished"), 1.0, ui->statusbar, gMainInterface->progressBar);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

void RenderWindow::slotMenuSaveImagePNG16()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("PNG images (*.png)"));
	dialog.setDirectory(QFileInfo(systemData.lastImageFile).absolutePath());
	dialog.selectFile(QFileInfo(systemData.lastImageFile).completeBaseName());
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("16-bit PNG"));
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG"), tr("Saving PNG image started"), 0.0, ui->statusbar, gMainInterface->progressBar);
		gApplication->processEvents();
		SavePNG16(filename, gMainInterface->mainImage->GetWidth(), gMainInterface->mainImage->GetHeight(), gMainInterface->mainImage->GetImage16Ptr());
		ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG"), tr("Saving PNG image finished"), 1.0, ui->statusbar, gMainInterface->progressBar);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

void RenderWindow::slotMenuSaveImagePNG16Alpha()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("PNG images (*.png)"));
	dialog.setDirectory(QFileInfo(systemData.lastImageFile).absolutePath());
	dialog.selectFile(QFileInfo(systemData.lastImageFile).completeBaseName());
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("16-bit PNG + alpha channel"));
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		ProgressStatusText(tr("Saving image to %1 ...").arg("16-bit PNG + alpha channel"), tr("Saving PNG image started"), 0.0, ui->statusbar, gMainInterface->progressBar);
		gApplication->processEvents();
		SavePNG16Alpha(filename, gMainInterface->mainImage->GetWidth(), gMainInterface->mainImage->GetHeight(), gMainInterface->mainImage);
		ProgressStatusText(tr("Saving image to %1 ...").arg("16-bit PNG + alpha channel"), tr("Saving PNG image started"), 1.0, ui->statusbar, gMainInterface->progressBar);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

void RenderWindow::slotPressedButtonSelectBackgroundTexture()
{
	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
	dialog.setDirectory(systemData.dataDirectory + QDir::separator() + "textures" + QDir::separator());
	dialog.selectFile(gPar->Get<QString>("file_background"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select background texture..."));
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		gPar->Set("file_background", filename);
		ui->text_file_background->setText(filename);
		gMainInterface->ShowImageInLabel(ui->label_backgroundTextureView, filename);
	}
}

void RenderWindow::slotPressedButtonSelectEnvMapTexture()
{
	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
	dialog.setDirectory(systemData.dataDirectory + QDir::separator() + "textures" + QDir::separator());
	dialog.selectFile(gPar->Get<QString>("file_envmap"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select texture for environment mapping effect..."));
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		gPar->Set("file_envmap", filename);
		ui->text_file_envmap->setText(filename);
		gMainInterface->ShowImageInLabel(ui->label_envmapTextureView, filename);
	}
}

void RenderWindow::slotPressedButtonSelectLightMapTexture()
{
	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
	dialog.setDirectory(systemData.dataDirectory + QDir::separator() + "textures" + QDir::separator());
	dialog.selectFile(gPar->Get<QString>("file_lightmap"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select texture for ambient occlussion light map..."));
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		gPar->Set("file_lightmap", filename);
		ui->text_file_lightmap->setText(filename);
		gMainInterface->ShowImageInLabel(ui->label_lightmapTextureView, filename);
	}
}

void RenderWindow::slotPressedButtonGetPaletteFromImage()
{
	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
	dialog.setDirectory(systemData.dataDirectory + QDir::separator() + "textures" + QDir::separator());
	dialog.selectFile(systemData.lastImagePaletteFile);
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select image to grab colors..."));
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		cColorPalette palette = gMainInterface->GetPaletteFromImage(filename);
		ui->colorpalette_surface_color_palette->SetPalette(palette);
		systemData.lastImagePaletteFile = filename;
	}
}

void RenderWindow::slotEditedLineEditBackgroundTexture(const QString &text)
{
	gMainInterface->ShowImageInLabel(ui->label_backgroundTextureView, text);
}

void RenderWindow::slotEditedLineEditEnvMapTexture(const QString &text)
{
	gMainInterface->ShowImageInLabel(ui->label_envmapTextureView, text);
}

void RenderWindow::slotEditedLineEditLightMapTexture(const QString &text)
{
	gMainInterface->ShowImageInLabel(ui->label_lightmapTextureView, text);
}

void RenderWindow::slotChangedComboAmbientOcclussionMode(int index)
{
	bool enabled = index == params::AOmodeMultipeRays ? true : false;
	ui->frame_lightmap_texture->setEnabled(enabled);
	enabled = index == params::AOmodeFast ? true : false;
	ui->slider_ambient_occlusion_fast_tune->setEnabled(enabled);
	ui->spinbox_ambient_occlusion_fast_tune->setEnabled(enabled);
}

void RenderWindow::slotChangedComboImageProportion(int index)
{
	bool enableSlider = false;
	double ratio = 1.0;
	enumImageProporton proportionSelection = (enumImageProporton)index;

	switch(proportionSelection)
	{
		case proportionFree:
			enableSlider = true;
			break;
		case proportion1_1:
			ratio = 1.0;
			break;
		case proportion4_3:
			ratio = 4.0 / 3.0;
			break;
		case proportion3_2:
			ratio = 3.0 / 2.0;
			break;
		case proportion16_9:
			ratio = 16.0 / 9.0;
			break;
		case proportion16_10:
			ratio = 16.0 / 10.0;
			break;
		case proportion2_1:
			ratio = 2.0 / 1.0;
			break;
		default:
			ratio = 1.0;
			break;
	}

	ui->sliderInt_image_width->setEnabled(enableSlider);
	ui->spinboxInt_image_width->setEnabled(enableSlider);

	int height = ui->spinboxInt_image_height->value();
	int width = height * ratio;

	if(!enableSlider)
	{
		ui->spinboxInt_image_width->setValue(width);
	}
}

void RenderWindow::slotPressedResolutionPresset()
{
	int width = 0, height = 0;
	enumImageProporton proportion = proportionFree;
	QString buttonName = this->sender()->objectName();
	if(buttonName == QString("pushButton_resolution_preset_480"))
	{
		width = 720; 		height = 480;		proportion = proportion3_2;
	}
	else if(buttonName == QString("pushButton_resolution_preset_720"))
	{
		width = 1280;		height = 720;		proportion = proportion16_9;
	}
	else if(buttonName == QString("pushButton_resolution_preset_1080"))
	{
		width = 1920;		height = 1080;		proportion = proportion16_9;
	}
	else if(buttonName == QString("pushButton_resolution_preset_1440"))
	{
		width = 2560;		height = 1440;		proportion = proportion16_9;
	}
	else if(buttonName == QString("pushButton_resolution_preset_2160"))
	{
		width = 4096;		height = 2160;		proportion = proportionFree;
	}
	else if(buttonName == QString("pushButton_resolution_preset_4320"))
	{
		width = 7680;		height = 4320;		proportion = proportion16_9;
	}
	else if(buttonName == QString("pushButton_resolution_preset_240"))
	{
		width = 320;		height = 240;		proportion = proportion4_3;
	}
	else if(buttonName == QString("pushButton_resolution_preset_600"))
	{
		width = 800;		height = 600;		proportion = proportion4_3;
	}
	else if(buttonName == QString("pushButton_resolution_preset_1200"))
	{
		width = 1600;		height = 1200;		proportion = proportion4_3;
	}

	ui->spinboxInt_image_width->setValue(width);
	ui->spinboxInt_image_height->setValue(height);
	ui->comboBox_image_proportion->setCurrentIndex(proportion);
}

void RenderWindow::slotImageHeightChanged(int value)
{
	(void)value;
	int index = ui->comboBox_image_proportion->currentIndex();
	slotChangedComboImageProportion(index);
}

void RenderWindow::slotPressedButtonImageApply()
{
	gMainInterface->RefreshMainImage();
}

void RenderWindow::slotPressedButtonDOFUpdate()
{
	gMainInterface->RefreshMainImage();
}

void RenderWindow::slotChangedComboPerspectiveType(int index)
{
	params::enumPerspectiveType perspType = (params::enumPerspectiveType)index;
	if(perspType == params::perspFishEyeCut)
	{
		ui->comboBox_image_proportion->setCurrentIndex(proportion1_1);
		ui->spinbox_fov->setValue(1.0);
	}
	else if(perspType == params::perspEquirectangular)
	{
		ui->comboBox_image_proportion->setCurrentIndex(proportion2_1);
		ui->spinbox_fov->setValue(1.0);
	}
}

void RenderWindow::slotChangedSpinBoxPaletteOffset(double value)
{
	ui->colorpalette_surface_color_palette->SetOffset(value);
}

void RenderWindow::slotPressedButtonRandomize()
{
	srand((unsigned int)clock());
	int seed = Random(999999);
	ui->spinboxInt_coloring_random_seed->setValue(seed);
	slotPressedButtonNewRandomPalette();
}

void RenderWindow::slotPressedButtonNewRandomPalette()
{
	gMainInterface->SynchronizeInterfaceWindow(ui->groupCheck_fractal_color, gPar, cInterface::read);
	cColorPalette palette(gPar->Get<int>("coloring_palette_size"), gPar->Get<int>("coloring_random_seed"), gPar->Get<double>("coloring_saturation"));
	ui->colorpalette_surface_color_palette->SetPalette(palette);
}

void RenderWindow::slotChangedSpinBoxPaletteSize(int value)
{
	ui->slider_coloring_palette_offset->setMaximum(value * 100);
}

void RenderWindow::slotPressedButtonAutoFog()
{
	gMainInterface->AutoFog();
}

void RenderWindow::slotMouseMovedOnImage(int x, int y)
{
	(void)x;
	(void)y;
	//qDebug() << "mouse move event";
	//CVector2<int> point(x, y);
}

void RenderWindow::slotMouceClickOnImage(int x, int y, Qt::MouseButton button)
{
	int index = ui->comboBox_mouse_click_function->currentIndex();
	QList<QVariant> mode = ui->comboBox_mouse_click_function->itemData(index).toList();
	RenderedImage::enumClickMode clickMode = (RenderedImage::enumClickMode)mode.at(0).toInt();

	switch(clickMode)
	{
		case RenderedImage::clickMoveCamera:
		case RenderedImage::clickFogVisibility:
		case RenderedImage::clickDOFFocus:
		case RenderedImage::clickPlaceLight:
		case RenderedImage::clickGetJuliaConstant:
		case RenderedImage::clickPlacePrimitive:
		{
			gMainInterface->SetByMouse(CVector2<double>(x, y), button, mode);
			break;
		}
		case RenderedImage::clickDoNothing:
		case RenderedImage::clickFlightSpeedControl:
			//nothing
			break;
	}

}

void RenderWindow::slotMovementStepModeChanged(int index)
{
	gMainInterface->MovementStepModeChanged(index);
}

void RenderWindow::slotPressedButtonSetDOFByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickDOFFocus);
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetFogByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickFogVisibility);
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotChangedComboMouseClickFunction(int index)
{
	QComboBox *comboBox = static_cast<QComboBox*>(this->sender());
	QList<QVariant> item = comboBox->itemData(index).toList();
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotKeyPressOnImage(Qt::Key key)
{

	switch (key)
	{
		case Qt::Key_W:
			gMainInterface->MoveCamera("bu_move_up");
			break;
		case Qt::Key_S:
			gMainInterface->MoveCamera("bu_move_down");
			break;
		case Qt::Key_A:
			gMainInterface->MoveCamera("bu_move_left");
			break;
		case Qt::Key_D:
			gMainInterface->MoveCamera("bu_move_right");
			break;
		case Qt::Key_Up:
			gMainInterface->MoveCamera("bu_move_forward");
			break;
		case Qt::Key_Down:
			gMainInterface->MoveCamera("bu_move_backward");
			break;
		default:
			break;
	}
}

void RenderWindow::slotKeyReleaseOnImage(Qt::Key key)
{
	(void)key;
}

void RenderWindow::slotMouseWheelRotatedonImage(int delta)
{
	int index = ui->comboBox_mouse_click_function->currentIndex();
	QList<QVariant> mode = ui->comboBox_mouse_click_function->itemData(index).toList();
	RenderedImage::enumClickMode clickMode = (RenderedImage::enumClickMode)mode.at(0).toInt();
	switch(clickMode)
	{
		case RenderedImage::clickPlaceLight:
		{
			double deltaLog = exp(delta * 0.001);
			double dist = ui->logedit_aux_light_manual_placement_dist->text().toDouble();
			dist *= deltaLog;
			ui->logedit_aux_light_manual_placement_dist->setText(QString::number(dist));
			break;
		}
		default:
			break;
	}
}

void RenderWindow::slotEditedLineEditManualLightPlacementDistance(const QString &text)
{
	gMainInterface->renderedImage->SetFrontDist(text.toDouble());
}

void RenderWindow::slotSliderMovedEditManualLightPlacementDistance(int value)
{
	gMainInterface->renderedImage->SetFrontDist(pow(10.0, value/100.0));
}

void RenderWindow::slotPressedButtonSetLight1ByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(1); //light number
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetLight2ByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(2); //light number
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetLight3ByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(3); //light number
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetLight4ByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(4); //light number
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonGetJuliaConstant()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickGetJuliaConstant);
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotChangedCheckBoxCursorVisibility(int state)
{
	gMainInterface->renderedImage->SetCursorVisibility(state);
}

void RenderWindow::slotPressedButtonSetPositionPrimitive()
{
	QString buttonName = this->sender()->objectName();
	QString primitiveName = buttonName.mid(buttonName.indexOf('_') + 1);
	QStringList split = primitiveName.split('_');
	QList<QVariant> item;
	item.append((int)RenderedImage::clickPlacePrimitive);
	item.append((int)PrimitiveNameToEnum(split.at(1)));
	item.append(split.at(2).toInt());
	item.append(primitiveName); //light number
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotMenuUndo()
{
	gMainInterface->Undo();
}

void RenderWindow::slotMenuRedo()
{
	gMainInterface->Redo();
}

void RenderWindow::slotPressedButtonResetView()
{
	gMainInterface->ResetView();
}

void RenderWindow::slotPressedButtonNewPrimitive()
{
	QString buttonName = this->sender()->objectName();
	QString primitiveName = buttonName.mid(buttonName.lastIndexOf('_') + 1);
	gMainInterface->NewPrimitive(primitiveName);
}

void RenderWindow::slotPressedButtonDeletePrimitive()
{
	QString buttonName = this->sender()->objectName();
	QString primitiveName = buttonName.mid(buttonName.indexOf('_') + 1);
	gMainInterface->DeletePrimitive(primitiveName);
}

void RenderWindow::slotStackAllDocks()
{
	tabifyDockWidget(ui->dockWidget_effects, ui->dockWidget_image_adjustments);
	tabifyDockWidget(ui->dockWidget_image_adjustments, ui->dockWidget_rendering_engine);
	tabifyDockWidget(ui->dockWidget_rendering_engine, ui->dockWidget_fractal);
}

void RenderWindow::slotUpdateDocksandToolbarbyAction()
{
	// Animation dock
	if(ui->actionShow_animation_dock->isChecked() != ui->dockWidget_animation->isVisible())
	{
		if(ui->actionShow_animation_dock->isChecked())
		{
			addDockWidget(Qt::BottomDockWidgetArea, ui->dockWidget_animation);
		}
		else
		{
			removeDockWidget(ui->dockWidget_animation);
		}
		ui->dockWidget_animation->setVisible(ui->actionShow_animation_dock->isChecked());
	}

	// Info dock
	if(ui->actionShow_info_dock->isChecked() != ui->dockWidget_info->isVisible())
	{
		if(ui->actionShow_info_dock->isChecked())
		{
			addDockWidget(Qt::TopDockWidgetArea, ui->dockWidget_info);
		}
		else
		{
			removeDockWidget(ui->dockWidget_info);
		}
		ui->dockWidget_info->setVisible(ui->actionShow_info_dock->isChecked());
	}

	// Toolbar
	if(ui->actionShow_toolbar->isChecked() != ui->toolBar->isVisible())
	{
			ui->toolBar->setVisible(ui->actionShow_toolbar->isChecked());
	}
}

void RenderWindow::slotUpdateDocksandToolbarbyView()
{
	// Animation dock
	if(ui->actionShow_animation_dock->isChecked() != ui->dockWidget_animation->isVisible())
	{
		ui->actionShow_animation_dock->setChecked(ui->dockWidget_animation->isVisible());
	}

	// Log dock
	if(ui->actionShow_info_dock->isChecked() != ui->dockWidget_info->isVisible())
	{
		ui->actionShow_info_dock->setChecked(ui->dockWidget_info->isVisible());
	}

	// Toolbar
	if(ui->actionShow_toolbar->isChecked() != ui->toolBar->isVisible())
	{
		ui->actionShow_toolbar->setChecked(ui->toolBar->isVisible());
	}
}
void RenderWindow::slotMenuLoadPreset(QString filename)
{
	cSettings parSettings(cSettings::formatFullText);

	parSettings.LoadFromFile(filename);
	parSettings.Decode(gPar, gParFractal);
	gMainInterface->RebuildPrimitives(gPar);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
	gMainInterface->ComboMouseClickUpdate();
	systemData.lastSettingsFile = systemData.dataDirectory + "settings" + QDir::separator() + QFileInfo(filename).fileName();
	this->setWindowTitle(QString("Mandelbulber (") + systemData.lastSettingsFile + ")");
}

void RenderWindow::slotQuit()
{
	gMainInterface->QuitApplicationDialog();
}

void RenderWindow::closeEvent(QCloseEvent * event)
{
	if(gMainInterface->QuitApplicationDialog())
	{
		event->accept();
		gApplication->quit();
	}
	else
	{
		event->ignore();
	}
}

void RenderWindow::slotUpdateProgressAndStatus(const QString &text, const QString &progressText, double progress)
{
	ProgressStatusText(text, progressText, progress, ui->statusbar, gMainInterface->progressBar);
}

void RenderWindow::slotMenuProgramSettings()
{
	// TODO show ProgramSettings interface
}

void RenderWindow::slotUpdateHistogramDE()
{
	ui->label_histogram_de->UpdateHistogram(histogramDE, 256, countHistogramDE);
}

void RenderWindow::slotUpdateHistogramIters()
{
	ui->label_histogram_iter->SetBarcolor(QColor(0, 255, 0)); // TODO move to setup
	ui->label_histogram_iter->UpdateHistogram(histogramIters, 256, countHistogramIters);
}
