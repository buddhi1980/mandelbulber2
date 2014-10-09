/*
 * render_window.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: krzysztof
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

#include <QtGui>
#include <QtUiTools/QtUiTools>
#include <QDial>
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>

#include "common_math.h"
#include "my_ui_loader.h"
#include "preview_file_dialog.h"

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
}

RenderWindow::~RenderWindow()
{
    delete ui;
    for(int i=0; i<4; i++)
    	delete fractalWidgets[i];
    delete[] fractalWidgets;
}


void RenderWindow::slotStartRender(void)
{
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	gUndo.Store(gPar, gParFractal);
	mainInterface->StartRender();
}

void RenderWindow::slotStopRender(void)
{
	mainInterface->stopRequest = true;
}

void RenderWindow::load(void)
{
	printf("load\n");
}

void RenderWindow::slotDoubleSpinBoxChanged(double value)
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	mainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
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
	mainInterface->GetNameAndType(lineEditName, &parameterName, &type);
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
	mainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
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
	mainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
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
	mainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
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
	mainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
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
	mainInterface->GetNameAndType(sliderName, &parameterName, &type);
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
	mainInterface->GetNameAndType(sliderName, &parameterName, &type);
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
	mainInterface->GetNameAndType(sliderName, &parameterName, &type);
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
	mainInterface->GetNameAndType(sliderName, &parameterName, &type);
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
	mainInterface->GetNameAndType(sliderName, &parameterName, &type);
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
	mainInterface->GetNameAndType(sliderName, &parameterName, &type);
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

void RenderWindow::slotPresedColorButton(void)
{
	QPushButton *pushButton = qobject_cast<QPushButton*>(this->sender());
	QString pushButtonName = pushButton->objectName();
	QColorDialog colorDialog(mainInterface->mainWindow);
	QColor color;
	color.setRed(pushButton->property("selectedColor_r").toInt() / 256);
	color.setGreen(pushButton->property("selectedColor_g").toInt() / 256);
	color.setBlue(pushButton->property("selectedColor_b").toInt() / 256);
	colorDialog.setCurrentColor(color);
	colorDialog.exec();
	color = colorDialog.currentColor();
	MakeIconForButton(color, pushButton);
	pushButton->setProperty("selectedColor_r", color.red() * 256);
	pushButton->setProperty("selectedColor_g", color.green() * 256);
	pushButton->setProperty("selectedColor_b", color.blue() * 256);
}

void RenderWindow::slotMenuSaveDocksPositions()
{
	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState", saveState());
	qDebug() << "settings saved";
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
		QString uiFilename = systemData.sharedDir + "qt" + QDir::separator() + "fractal_" + formulaName + ".ui";

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
			mainInterface->ConnectSignalsForSlidersInWindow(fractalWidgets[fractalNumber]);
			mainInterface->SynchronizeInterfaceWindow(fractalWidgets[fractalNumber], &gParFractal[fractalNumber], cInterface::write);

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
			cErrorMessage::showMessage(QString("Can't open file ") + uiFilename + QString("\nFractal ui file can't be loaded"), cErrorMessage::errorMessage, mainInterface->mainWindow);
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
	if (mainInterface->mainImage)
	{
		int selectedScale = ui->comboBox_image_preview_scale->currentIndex();

		if (selectedScale == 0)
		{
			double scale = mainInterface->CalcMainImageScale(0.0, width, height, mainInterface->mainImage);
			mainInterface->mainImage->CreatePreview(scale, width, height, mainInterface->renderedImage);
			mainInterface->mainImage->UpdatePreview();
			mainInterface->renderedImage->setMinimumSize(mainInterface->mainImage->GetPreviewWidth(), mainInterface->mainImage->GetPreviewHeight());
		}
	}
}

void RenderWindow::slotChangedComboImageScale(int index)
{
	if (mainInterface->mainImage)
	{
		double scale = mainInterface->ImageScaleComboSelection2Double(index);
		int areaWidth = ui->scrollAreaForImage->VisibleAreaWidth();
		int areaHeight = ui->scrollAreaForImage->VisibleAreaHeight();
		scale = mainInterface->CalcMainImageScale(scale, areaWidth, areaHeight, mainInterface->mainImage);

		mainInterface->mainImage->CreatePreview(scale, areaWidth, areaHeight, mainInterface->renderedImage);
		mainInterface->mainImage->UpdatePreview();
		mainInterface->renderedImage->setMinimumSize(mainInterface->mainImage->GetPreviewWidth(), mainInterface->mainImage->GetPreviewHeight());
	}
}

void RenderWindow::slotCameraMove()
{
	QString buttonName = this->sender()->objectName();
	mainInterface->MoveCamera(buttonName);
}

void RenderWindow::slotCameraRotation()
{
	QString buttonName = this->sender()->objectName();
	mainInterface->RotateCamera(buttonName);
}

void RenderWindow::slotCameraOrTargetEdited()
{
	mainInterface->CameraOrTargetEdited();
}

void RenderWindow::slotRotationEdited()
{
	mainInterface->RotationEdited();
}

void RenderWindow::slotCameraDistanceEdited()
{
	mainInterface->CameraDistanceEdited();
}

void RenderWindow::slotCameraDistanceSlider(int value)
{
	(void)value;
	mainInterface->CameraDistanceEdited();
}

void RenderWindow::slotChangedCheckBoxHybridFractal(int state)
{
	ui->label_formula_iterations_1->setEnabled(state);
	ui->spinboxInt_formula_iterations_1->setEnabled(state);
	ui->sliderInt_formula_iterations_1->setEnabled(state);
	ui->frame_iterations_formula_2->setEnabled(state);
	ui->frame_iterations_formula_3->setEnabled(state);
	ui->frame_iterations_formula_4->setEnabled(state);
	ui->scrollArea_fractal_2->setEnabled(state);
	ui->scrollArea_fractal_3->setEnabled(state);
	ui->scrollArea_fractal_4->setEnabled(state);
}

void RenderWindow::slotMenuSaveSettings()
{
	cSettings parSettings(cSettings::formatCondensedText);
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	parSettings.CreateText(gPar, gParFractal);

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(systemData.dataDirectory + QDir::separator() + "settings" + QDir::separator());
	dialog.selectFile(systemData.lastSettingsFile);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle("Save settings...");
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
	cSettings parSettings(cSettings::formatFullText);

	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(systemData.dataDirectory + QDir::separator() + "settings" + QDir::separator());
	dialog.selectFile(systemData.lastSettingsFile);
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle("Load settings...");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		parSettings.LoadFromFile(filename);
		parSettings.Decode(gPar, gParFractal);
		mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
		systemData.lastSettingsFile = filename;
		this->setWindowTitle(QString("Mandelbulber (") + filename + ")");
	}
}

void RenderWindow::slotPressedButtonIFSDefaultsDodecahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	mainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal[index], cInterface::read);
	mainInterface->IFSDefaultsDodecahedron(&gParFractal[index]);
	mainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal[index], cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsIcosahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	mainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal[index], cInterface::read);
	mainInterface->IFSDefaultsIcosahedron(&gParFractal[index]);
	mainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal[index], cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsOctahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	mainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal[index], cInterface::read);
	mainInterface->IFSDefaultsOctahedron(&gParFractal[index]);
	mainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal[index], cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsMengerSponge()
{
	int index = ui->tabWidget_fractals->currentIndex();
	mainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal[index], cInterface::read);
	mainInterface->IFSDefaultsMengerSponge(&gParFractal[index]);
	mainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal[index], cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsReset()
{
	int index = ui->tabWidget_fractals->currentIndex();
	mainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal[index], cInterface::read);
	mainInterface->IFSDefaultsReset(&gParFractal[index]);
	mainInterface->SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal[index], cInterface::write);
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
	text += "Copyright Ⓒ 2014 Krzysztof Marczak\n";
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
	dialog.setWindowTitle("Save image to JPEG file...");
	dialog.setDefaultSuffix("jpeg");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		mainInterface->StatusText(QString("Saving JPG image"), QString("Saving image started"), 0.0);
		mainInterface->application->processEvents();
		SaveJPEGQt(filename, mainInterface->mainImage->ConvertTo8bit(), mainInterface->mainImage->GetWidth(), mainInterface->mainImage->GetHeight(), 95);
		mainInterface->StatusText(QString("Saving JPG image"), QString("Saving image finished"), 1.0);
		mainInterface->application->processEvents();
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
	dialog.setWindowTitle("Save image to 8-bit PNG file...");
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		mainInterface->StatusText(QString("Saving PNG image"), QString("Saving PNG image started"), 0.0);
		mainInterface->application->processEvents();
		SavePNG(filename, mainInterface->mainImage->GetWidth(), mainInterface->mainImage->GetHeight(), mainInterface->mainImage->ConvertTo8bit());
		mainInterface->StatusText(QString("Saving PNG image"), QString("Saving PNG image finished"), 1.0);
		mainInterface->application->processEvents();
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
	dialog.setWindowTitle("Save image to 16-bit PNG file...");
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		mainInterface->StatusText(QString("Saving PNG 16-bit image"), QString("Saving PNG image started"), 0.0);
		mainInterface->application->processEvents();
		SavePNG16(filename, mainInterface->mainImage->GetWidth(), mainInterface->mainImage->GetHeight(), mainInterface->mainImage->GetImage16Ptr());
		mainInterface->StatusText(QString("Saving PNG 16-bit image"), QString("Saving PNG image finished"), 1.0);
		mainInterface->application->processEvents();
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
	dialog.setWindowTitle("Save image to 16-bit + alpha channel PNG file...");
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		mainInterface->StatusText(QString("Saving image to PNG 16-bit with alpha channel"), QString("Saving PNG image started"), 0.0);
		mainInterface->application->processEvents();
		SavePNG16Alpha(filename, mainInterface->mainImage->GetWidth(), mainInterface->mainImage->GetHeight(), mainInterface->mainImage);
		mainInterface->StatusText(QString("Saving image to PNG 16-bit with alpha channel"), QString("Saving PNG image finished"), 1.0);
		mainInterface->application->processEvents();
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
	dialog.setWindowTitle("Select background texture...");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		gPar->Set("file_background", filename);
		ui->text_file_background->setText(filename);
		mainInterface->ShowImageInLabel(ui->label_backgroundTextureView, filename);
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
	dialog.setWindowTitle("Select texture for environment mapping effect...");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		gPar->Set("file_envmap", filename);
		ui->text_file_envmap->setText(filename);
		mainInterface->ShowImageInLabel(ui->label_envmapTextureView, filename);
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
	dialog.setWindowTitle("Select texture for ambient occlussion light map...");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		gPar->Set("file_lightmap", filename);
		ui->text_file_lightmap->setText(filename);
		mainInterface->ShowImageInLabel(ui->label_lightmapTextureView, filename);
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
	dialog.setWindowTitle("Select image to grab colors...");
	QStringList filenames;
	if(dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		cColorPalette palette = mainInterface->GetPaletteFromImage(filename);
		ui->colorpalette_surface_color_palette->SetPalette(palette);
		systemData.lastImagePaletteFile = filename;
	}
}

void RenderWindow::slotEditedLineEditBackgroundTexture(const QString &text)
{
	mainInterface->ShowImageInLabel(ui->label_backgroundTextureView, text);
}

void RenderWindow::slotEditedLineEditEnvMapTexture(const QString &text)
{
	mainInterface->ShowImageInLabel(ui->label_envmapTextureView, text);
}

void RenderWindow::slotEditedLineEditLightMapTexture(const QString &text)
{
	mainInterface->ShowImageInLabel(ui->label_lightmapTextureView, text);
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
	enumImageProporton proportion;
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
	mainInterface->RefreshMainImage();
}

void RenderWindow::slotPressedButtonDOFUpdate()
{
	mainInterface->RefreshMainImage();
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
	mainInterface->SynchronizeInterfaceWindow(ui->groupCheck_fractal_color, gPar, cInterface::read);
	cColorPalette palette(gPar->Get<int>("coloring_palette_size"), gPar->Get<int>("coloring_random_seed"), gPar->Get<double>("coloring_saturation"));
	ui->colorpalette_surface_color_palette->SetPalette(palette);
}

void RenderWindow::slotChangedSpinBoxPaletteSize(int value)
{
	ui->slider_coloring_palette_offset->setMaximum(value * 100);
}

void RenderWindow::slotPressedButtonAutoFog()
{
	mainInterface->AutoFog();
}

void RenderWindow::slotMouseMovedOnImage(int x, int y)
{
	//qDebug() << "mouse move event";
	//CVector2<int> point(x, y);
}

void RenderWindow::slotMouceClickOnImage(int x, int y, Qt::MouseButton button)
{
	enum RenderedImage::enumClickMode clickMode = (RenderedImage::enumClickMode)ui->comboBox_mouse_click_function->currentIndex();
	switch(clickMode)
	{
		case RenderedImage::clickMoveCamera:
		case RenderedImage::clickFogVisibility:
		case RenderedImage::clickDOFFocus:
		case RenderedImage::clickPlaceLight1:
		case RenderedImage::clickPlaceLight2:
		case RenderedImage::clickPlaceLight3:
		case RenderedImage::clickPlaceLight4:
		case RenderedImage::clickGetJuliaConstant:
		{
			mainInterface->SetByMouse(CVector2<double>(x, y), button, clickMode);
			break;
		}
	}

}

void RenderWindow::slotMovementStepModeChanged(int index)
{
	mainInterface->MovementStepModeChanged(index);
}

void RenderWindow::slotPressedButtonSetDOFByMouse()
{
	ui->comboBox_mouse_click_function->setCurrentIndex(RenderedImage::clickDOFFocus);
	mainInterface->renderedImage->setClickMode(RenderedImage::clickDOFFocus);
}

void RenderWindow::slotPressedButtonSetFogByMouse()
{
	ui->comboBox_mouse_click_function->setCurrentIndex(RenderedImage::clickFogVisibility);
	mainInterface->renderedImage->setClickMode(RenderedImage::clickFogVisibility);
}

void RenderWindow::slotChangedComboMouseClickFunction(int index)
{
	mainInterface->renderedImage->setClickMode((RenderedImage::enumClickMode)index);
}

void RenderWindow::slotKeyPressOnImage(Qt::Key key)
{

	switch (key)
	{
		case Qt::Key_W:
			mainInterface->MoveCamera("bu_move_up");
			break;
		case Qt::Key_S:
			mainInterface->MoveCamera("bu_move_down");
			break;
		case Qt::Key_A:
			mainInterface->MoveCamera("bu_move_left");
			break;
		case Qt::Key_D:
			mainInterface->MoveCamera("bu_move_right");
			break;
		case Qt::Key_Up:
			mainInterface->MoveCamera("bu_move_forward");
			break;
		case Qt::Key_Down:
			mainInterface->MoveCamera("bu_move_backward");
			break;
		default:
			break;
	}
}

void RenderWindow::slotKeyReleaseOnImage(Qt::Key key)
{

}

void RenderWindow::slotMouseWheelRotatedonImage(int delta)
{
	enum RenderedImage::enumClickMode clickMode = (RenderedImage::enumClickMode)ui->comboBox_mouse_click_function->currentIndex();
	switch(clickMode)
	{
		case RenderedImage::clickPlaceLight1:
		case RenderedImage::clickPlaceLight2:
		case RenderedImage::clickPlaceLight3:
		case RenderedImage::clickPlaceLight4:
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
	mainInterface->renderedImage->SetFrontDist(text.toDouble());
}

void RenderWindow::slotSliderMovedEditManualLightPlacementDistance(int value)
{
	mainInterface->renderedImage->SetFrontDist(pow(10.0, value/100.0));
}

void RenderWindow::slotPressedButtonSetLight1ByMouse()
{
	ui->comboBox_mouse_click_function->setCurrentIndex(RenderedImage::clickPlaceLight1);
	mainInterface->renderedImage->setClickMode(RenderedImage::clickPlaceLight1);
}

void RenderWindow::slotPressedButtonSetLight2ByMouse()
{
	ui->comboBox_mouse_click_function->setCurrentIndex(RenderedImage::clickPlaceLight2);
	mainInterface->renderedImage->setClickMode(RenderedImage::clickPlaceLight2);
}

void RenderWindow::slotPressedButtonSetLight3ByMouse()
{
	ui->comboBox_mouse_click_function->setCurrentIndex(RenderedImage::clickPlaceLight3);
	mainInterface->renderedImage->setClickMode(RenderedImage::clickPlaceLight3);
}

void RenderWindow::slotPressedButtonSetLight4ByMouse()
{
	ui->comboBox_mouse_click_function->setCurrentIndex(RenderedImage::clickPlaceLight4);
	mainInterface->renderedImage->setClickMode(RenderedImage::clickPlaceLight4);
}

void RenderWindow::slotPressedButtonGetJuliaConstant()
{
	ui->comboBox_mouse_click_function->setCurrentIndex(RenderedImage::clickGetJuliaConstant);
	mainInterface->renderedImage->setClickMode(RenderedImage::clickGetJuliaConstant);
}

void RenderWindow::slotChangedCheckBoxCursorVisibility(int state)
{
	mainInterface->renderedImage->SetCursorVisibility(state);
}

void RenderWindow::slotMenuUndo()
{
	mainInterface->Undo();
}

void RenderWindow::slotMenuRedo()
{
	mainInterface->Redo();
}

void RenderWindow::slotPressedButtonResetView()
{
	mainInterface->ResetView();
}
