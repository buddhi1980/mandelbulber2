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
#include "initparameters.hpp"
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

#include "preferencesdialog.h"

#define _USE_MATH_DEFINES
#include <math.h>

RenderWindow::RenderWindow(QWidget *parent) :
		QMainWindow(parent), ui(new Ui::RenderWindow)
{
	ui->setupUi(this);
	fractalWidgets = new QWidget*[NUMBER_OF_FRACTALS];

	//store default geometry and state
	defaultGeometry = saveGeometry();
	defaultState = saveState();

#ifdef USE_GAMEPAD
	QApplication::connect(&gamepad,
												SIGNAL(axisLeftXChanged(double)),
												this,
												SLOT(slotGamepadPitch(double)));
	QApplication::connect(&gamepad,
												SIGNAL(axisLeftYChanged(double)),
												this,
												SLOT(slotGamepadYaw(double)));
	QApplication::connect(&gamepad, SIGNAL(buttonL2Changed(double)), this, SLOT(slotGamepadRoll()));
	QApplication::connect(&gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(slotGamepadRoll()));
	QApplication::connect(&gamepad,
												SIGNAL(buttonL1Changed(bool)),
												this,
												SLOT(slotShiftModeChange(bool)));

	QApplication::connect(&gamepad,
												SIGNAL(axisRightXChanged(double)),
												this,
												SLOT(slotGamepadX(double)));
	QApplication::connect(&gamepad,
												SIGNAL(axisRightYChanged(double)),
												this,
												SLOT(slotGamepadY(double)));
	QApplication::connect(&gamepad, SIGNAL(buttonAChanged(bool)), this, SLOT(slotGamepadZ()));
	QApplication::connect(&gamepad, SIGNAL(buttonBChanged(bool)), this, SLOT(slotGamepadZ()));
	QApplication::connect(this->ui->groupCheck_gamepad_enabled,
												SIGNAL(toggled(bool)),
												&gamepad,
												SLOT(setConnected(bool)));
#else
	ui->menuView->removeAction(ui->actionShow_gamepad_dock);
	removeDockWidget(ui->dockWidget_gamepad_dock);
#endif
}

RenderWindow::~RenderWindow()
{
	delete ui;
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		delete fractalWidgets[i];
	delete[] fractalWidgets;
}

void RenderWindow::slotStartRender(void)
{
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
		QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(this->sender());
		double decimals = spinBox->decimals();
		double multiplier = pow(10.0, decimals);
		slider->setValue(value * multiplier);
	}
	else
	{
		qWarning() << "slotDoubleSpinBoxChanged() error: slider " << sliderName << " doesn't exists"
				<< endl;
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
		double value = systemData.locale.toDouble(text);
		if (value > 0.0)
		{
			int sliderPosition = log10(systemData.locale.toDouble(text)) * 100.0;
			slider->setValue(sliderPosition);
		}
		else
		{
			qWarning() << "slotLogLineEditChanged() error: value from " << lineEditName
					<< " is not greater zero" << endl;
		}
	}
	else
	{
		qWarning() << "slotLogLineEditChanged() error: slider " << sliderName << " doesn't exists"
				<< endl;
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
		qWarning() << "slotIntSpinBoxChanged() error: slider " << sliderName << " doesn't exists"
				<< endl;
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
		QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(this->sender());
		double decimals = spinBox->decimals();
		double multiplier = pow(10.0, decimals);
		slider->setValue(value * multiplier);
	}
	else
	{
		qWarning() << "slotSpinBox3Changed() error: slider " << sliderName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotSpinBox4Changed(double value)
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
	QString sliderName = QString("slider4_") + parameterName;

	QSlider *slider = this->sender()->parent()->findChild<QSlider*>(sliderName);

	if (slider)
	{
		QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(this->sender());
		double decimals = spinBox->decimals();
		double multiplier = pow(10.0, decimals);
		slider->setValue(value * multiplier);
	}
	else
	{
		qWarning() << "slotSpinBox4Changed() error: slider " << sliderName << " doesn't exists" << endl;
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

void RenderWindow::slotSpinBoxD4Changed(double value)
{
	using namespace std;
	QString spinBoxName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(spinBoxName, &parameterName, &type);
	QString dialName = QString("dial4_") + parameterName;

	QDial *dial = this->sender()->parent()->findChild<QDial*>(dialName);
	if (dial)
	{
		dial->setValue(value * 100.0);
	}
	else
	{
		qWarning() << "slotSpinBox4Changed() error: slider " << dialName << " doesn't exists" << endl;
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
	if (spinBox)
	{
		double decimals = spinBox->decimals();
		double divider = pow(10.0, decimals);
		spinBox->setValue(value / divider);
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
	if (lineEdit)
	{
		double dValue = pow(10.0, value / 100.0);
		QString text = QString("%L1").arg(dValue);
		lineEdit->setText(text);
	}
	else
	{
		qWarning() << "slotLogSliderMoved() error: lineEdit " << lineEditName << " doesn't exists"
				<< endl;
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
	if (spinbox)
	{
		spinbox->setValue(value);
	}
	else
	{
		qWarning() << "slotLogSliderMoved() error: lineEdit " << spinboxName << " doesn't exists"
				<< endl;
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

	if (spinBox)
	{
		double decimals = spinBox->decimals();
		double divider = pow(10.0, decimals);
		spinBox->setValue(value / divider);
	}
	else
	{
		qWarning() << "slotSlider3Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotSlider4Moved(int value)
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinbox4_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);

	if (spinBox)
	{
		double decimals = spinBox->decimals();
		double divider = pow(10.0, decimals);
		spinBox->setValue(value / divider);
	}
	else
	{
		qWarning() << "slotSlider4Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
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
	if (spinBox)
	{
		spinBox->setValue(value / 100.0);
	}
	else
	{
		qWarning() << "slotDial3Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
	}
}

void RenderWindow::slotDial4Moved(int value)
{
	using namespace std;
	QString sliderName = this->sender()->objectName();
	QString type, parameterName;
	gMainInterface->GetNameAndType(sliderName, &parameterName, &type);
	QString spinBoxName = QString("spinboxd4_") + parameterName;

	QDoubleSpinBox *spinBox = this->sender()->parent()->findChild<QDoubleSpinBox*>(spinBoxName);
	if (spinBox)
	{
		spinBox->setValue(value / 100.0);
	}
	else
	{
		qWarning() << "slotDial4Moved() error: spinbox " << spinBoxName << " doesn't exists" << endl;
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
	if (spinBox)
	{
		spinBox->setValue(value / 100.0);
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
	ui->dockWidget_histogram->hide();
	ui->dockWidget_info->hide();
	ui->dockWidget_animation->hide();
	ui->dockWidget_gamepad_dock->hide();
	ui->dockWidget_queue_dock->hide();
}

void RenderWindow::slotChangedComboFractal(int indexInComboBox)
{
	QString comboName = this->sender()->objectName();
	int index = qobject_cast<QComboBox *>(this->sender())->itemData(indexInComboBox).toInt();
	int fractalNumber = comboName.right(1).toInt() - 1;

	QString fullFormulaName = fractalList[index].nameInComboBox;
	if (fractalList[index].internalID > 0)
	{
		QString formulaName = fractalList[index].internalName;
		QString uiFilename = systemData.sharedDir + "qt_data" + QDir::separator() + "fractal_"
				+ formulaName + ".ui";

		if (fractalWidgets[fractalNumber]) delete fractalWidgets[fractalNumber];
		fractalWidgets[fractalNumber] = NULL;

		MyUiLoader loader;
		QFile uiFile(uiFilename);

		if (uiFile.exists())
		{
			uiFile.open(QFile::ReadOnly);
			fractalWidgets[fractalNumber] = loader.load(&uiFile);
			QVBoxLayout *layout =
					ui->dockWidget_fractal->findChild<QVBoxLayout*>("verticalLayout_fractal_"
							+ QString::number(fractalNumber + 1));
			layout->addWidget(fractalWidgets[fractalNumber]);
			uiFile.close();
			fractalWidgets[fractalNumber]->show();
			gMainInterface->ConnectSignalsForSlidersInWindow(fractalWidgets[fractalNumber]);
			gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[fractalNumber],
																								 &gParFractal->at(fractalNumber),
																								 cInterface::write);

			QFrame *frame = ui->tabWidget_fractals->findChild<QFrame*>("frame_iterations_formula_"
					+ QString::number(fractalNumber + 1));
			MyCheckBox *caddCheckBoxframe =
					frame->findChild<MyCheckBox*>(QString("checkBox_dont_add_c_constant_")
							+ QString::number(fractalNumber + 1));

			switch(fractalList[index].cpixelAddition)
			{
				case fractal::cpixelEnabledByDefault:
					caddCheckBoxframe->setText(QObject::tr("Don't add global C constant"));
					caddCheckBoxframe->setEnabled(true);
					break;

				case fractal::cpixelDisabledByDefault:
				{
					caddCheckBoxframe->setText(QObject::tr("Add global C constant"));
					caddCheckBoxframe->setEnabled(true);
					break;
				}

				case fractal::cpixelAlreadyHas:
				{
					caddCheckBoxframe->setText(QObject::tr("Don't add global C constant"));
					caddCheckBoxframe->setEnabled(false);
					break;
				}
			}

			fractal::enumCPixelAddition cPixelAddition = fractalList[index].cpixelAddition;
			bool boleanState = ui->groupCheck_boolean_operators->isChecked();
			if (cPixelAddition == fractal::cpixelAlreadyHas)
				ui->tabWidget_fractals->findChild<QGroupBox*>("groupBox_c_constant_addition_"
						+ QString::number(fractalNumber + 1))->setVisible(false);
			else ui->tabWidget_fractals->findChild<QGroupBox*>("groupBox_c_constant_addition_"
					+ QString::number(fractalNumber + 1))->setVisible(boleanState);

			if (fractalList[index].internalID == fractal::kaleidoscopicIFS)
			{
				QWidget *pushButton_preset_dodecahedron =
						fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_dodecahedron");
				QApplication::connect(pushButton_preset_dodecahedron,
															SIGNAL(clicked()),
															this,
															SLOT(slotPressedButtonIFSDefaultsDodecahedron()));
				QWidget *pushButton_preset_icosahedron =
						fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_icosahedron");
				QApplication::connect(pushButton_preset_icosahedron,
															SIGNAL(clicked()),
															this,
															SLOT(slotPressedButtonIFSDefaultsIcosahedron()));
				QWidget *pushButton_preset_octahedron =
						fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_octahedron");
				QApplication::connect(pushButton_preset_octahedron,
															SIGNAL(clicked()),
															this,
															SLOT(slotPressedButtonIFSDefaultsOctahedron()));
				QWidget *pushButton_preset_menger_sponge =
						fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_menger_sponge");
				QApplication::connect(pushButton_preset_menger_sponge,
															SIGNAL(clicked()),
															this,
															SLOT(slotPressedButtonIFSDefaultsMengerSponge()));
				QWidget *pushButton_preset_reset =
						fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_reset");
				QApplication::connect(pushButton_preset_reset,
															SIGNAL(clicked()),
															this,
															SLOT(slotPressedButtonIFSDefaultsReset()));
			}
		}
		else
		{
			cErrorMessage::showMessage(QString("Can't open file ") + uiFilename
																		 + QString("\nFractal ui file can't be loaded"),
																 cErrorMessage::errorMessage,
																 gMainInterface->mainWindow);
		}
	}
	else
	{
		if (fractalWidgets[fractalNumber]) delete fractalWidgets[fractalNumber];
		fractalWidgets[fractalNumber] = NULL;
	}
	ui->tabWidget_fractals->setTabText(fractalNumber, QString("#%1: %2").arg(fractalNumber + 1).arg(fullFormulaName));
	// ui->tabWidget_fractals->setTabIcon(fractalNumber, QIcon(fractalList[index].getIconName()));
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
			gMainInterface->renderedImage->setMinimumSize(gMainInterface->mainImage->GetPreviewWidth(),
																										gMainInterface->mainImage->GetPreviewHeight());
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

		gMainInterface->mainImage->CreatePreview(scale,
																						 areaWidth,
																						 areaHeight,
																						 gMainInterface->renderedImage);
		gMainInterface->mainImage->UpdatePreview();
		gMainInterface->renderedImage->setMinimumSize(gMainInterface->mainImage->GetPreviewWidth(),
																									gMainInterface->mainImage->GetPreviewHeight());

		gPar->Set("image_preview_scale", index);
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
	(void) value;
	gMainInterface->CameraDistanceEdited();
}

void RenderWindow::slotChangedCheckBoxHybridFractal(int state)
{
	if (state) ui->groupCheck_boolean_operators->setChecked(false);
	gApplication->processEvents();

	for(int i = 1; i <= NUMBER_OF_FRACTALS; i++)
	{
		QFrame *frame = ui->tabWidget_fractals->findChild<QFrame*>("frame_iterations_formula_" + QString::number(i));

		frame->findChild<QLabel*>(QString("label_formula_iterations_") + QString::number(i))->setVisible(state);
		frame->findChild<MySpinBox*>(QString("spinboxInt_formula_iterations_") + QString::number(i))->setVisible(state);
		frame->findChild<QSlider*>(QString("sliderInt_formula_iterations_") + QString::number(i))->setVisible(state);

		frame->findChild<QLabel*>(QString("label_formula_weight_") + QString::number(i))->setVisible(state);
		frame->findChild<MyDoubleSpinBox*>(QString("spinbox_formula_weight_") + QString::number(i))->setVisible(state);
		frame->findChild<QSlider*>(QString("slider_formula_weight_") + QString::number(i))->setVisible(state);

		frame->findChild<QLabel*>(QString("label_formula_start_iteration_") + QString::number(i))->setVisible(state);
		frame->findChild<QLabel*>(QString("label_formula_stop_iteration_") + QString::number(i))->setVisible(state);
		frame->findChild<MySpinBox*>(QString("spinboxInt_formula_start_iteration_") + QString::number(i))->setVisible(state);
		frame->findChild<MySpinBox*>(QString("spinboxInt_formula_stop_iteration_") + QString::number(i))->setVisible(state);

		frame->findChild<MyCheckBox*>(QString("checkBox_check_for_bailout_") + QString::number(i))->setVisible(state);
		//frame->findChild<MyCheckBox*>(QString("checkBox_dont_add_c_constant_") + QString::number(i))->setVisible(state);

		if(i > 1)
		{
			frame->setEnabled(state);
			ui->tabWidget_fractals->findChild<QScrollArea*>("scrollArea_fractal_" + QString::number(i))->setEnabled(state);
		}
	}

	ui->comboBox_delta_DE_function->setEnabled(state || ui->comboBox_delta_DE_method->currentIndex() == (int)fractal::forceDeltaDEMethod);
	//ui->comboBox_delta_DE_method->setEnabled(!state);
}

void RenderWindow::slotChangedCheckBoxBooleanOperators(bool state)
{
	//TODO change number of fractals to 9 for boolean operators
	if (state) ui->checkBox_hybrid_fractal_enable->setChecked(false);
	gApplication->processEvents();

	for(int i = 1; i <= NUMBER_OF_FRACTALS; i++)
	{
		QFrame *frame = ui->tabWidget_fractals->findChild<QFrame*>("frame_iterations_formula_" + QString::number(i));
		if(i > 1)
		{
			frame->setEnabled(state);
			ui->tabWidget_fractals->findChild<QScrollArea*>("scrollArea_fractal_" + QString::number(i))->setEnabled(state);
		}
		ui->tabWidget_fractals->findChild<QGroupBox*>("groupBox_formula_transform_" + QString::number(i))->setVisible(state);

		QComboBox *comboBox = ui->tabWidget_fractals->findChild<QComboBox*>("comboBox_formula_"
				+ QString::number(i));
		fractal::enumCPixelAddition cPixelAddition = fractalList[comboBox->itemData(comboBox
				->currentIndex()).toInt()].cpixelAddition;

		if (cPixelAddition == fractal::cpixelAlreadyHas)
			ui->tabWidget_fractals->findChild<QGroupBox*>("groupBox_c_constant_addition_"
					+ QString::number(i))->setVisible(false);
		else ui->tabWidget_fractals->findChild<QGroupBox*>("groupBox_c_constant_addition_"
				+ QString::number(i))->setVisible(state);
	}

	ui->comboBox_delta_DE_function->setEnabled(!state);
}

void RenderWindow::slotChangedComboDistanceEstimationMethod(int index)
{
	ui->comboBox_delta_DE_function->setEnabled(ui->checkBox_hybrid_fractal_enable->isChecked() || index == (int)fractal::forceDeltaDEMethod);
}

void RenderWindow::slotMenuSaveSettings()
{
	cSettings parSettings(cSettings::formatCondensedText);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);

	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save settings..."));
	dialog.setDefaultSuffix("fract");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		parSettings.SaveToFile(filename);
		systemData.lastSettingsFile = filename;
		this->setWindowTitle(QString("Mandelbulber (") + filename + ")");

		//QString hash = parSettings.GetHashCode();
		//cThumbnail thumbnail(gPar, gParFractal, 200, 200);
		//thumbnail.Render();
		//thumbnail.Save(systemData.thumbnailDir + hash);
	}
}

void RenderWindow::slotMenuSaveSettingsToClipboard()
{
	cSettings parSettings(cSettings::formatCondensedText);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
	parSettings.SaveToClipboard();
	cErrorMessage::showMessage(QObject::tr("Settings saved to clipboard"),
														 cErrorMessage::infoMessage);
}

void RenderWindow::slotMenuLoadSettings()
{
	gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read); //update appParam before loading new settings

	cSettings parSettings(cSettings::formatFullText);

	PreviewFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.lastSettingsFile));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load settings..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		parSettings.LoadFromFile(filename);
		parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
		gMainInterface->RebuildPrimitives(gPar);
		gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
		gMainInterface->ComboMouseClickUpdate();
		systemData.lastSettingsFile = filename;
		this->setWindowTitle(QString("Mandelbulber (") + filename + ")");
		gFlightAnimation->RefreshTable();
		gKeyframeAnimation->RefreshTable();
	}
}

void RenderWindow::slotMenuLoadSettingsFromClipboard()
{
	gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read); //update appParam before loading new settings

	cSettings parSettings(cSettings::formatFullText);

	if (parSettings.LoadFromClipboard())
	{
		parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
		gMainInterface->RebuildPrimitives(gPar);
		gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
		gMainInterface->ComboMouseClickUpdate();
		systemData.lastSettingsFile = "from clipboard";
		this->setWindowTitle(QString("Mandelbulber (") + "from clipboard" + ")");
		gFlightAnimation->RefreshTable();
		gKeyframeAnimation->RefreshTable();
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Cannot load settings from clipboard!"),
															 cErrorMessage::errorMessage,
															 gMainInterface->mainWindow);
	}
}

void RenderWindow::slotMenuLoadExample()
{
	cSettings parSettings(cSettings::formatFullText);

	PreviewFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(QDir::toNativeSeparators(systemData.sharedDir + QDir::separator() + "examples" + QDir::separator()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).fileName()));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load example settings..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		parSettings.LoadFromFile(filename);
		parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
		gMainInterface->RebuildPrimitives(gPar);
		gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
		gMainInterface->ComboMouseClickUpdate();
		systemData.lastSettingsFile = systemData.dataDirectory + "settings" + QDir::separator()
				+ QFileInfo(filename).fileName();
		this->setWindowTitle(QString("Mandelbulber (") + systemData.lastSettingsFile + ")");
		gFlightAnimation->RefreshTable();
		gKeyframeAnimation->RefreshTable();
	}
}

void RenderWindow::slotImportOldSettings()
{
	cSettings parSettings(cSettings::formatFullText);

	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.lastSettingsFile));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Import settings from old Mandelbulber (v1.21)..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
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
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
																						 &gParFractal->at(index),
																						 cInterface::read);
	gMainInterface->IFSDefaultsDodecahedron(&gParFractal->at(index));
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
																						 &gParFractal->at(index),
																						 cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsIcosahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
																						 &gParFractal->at(index),
																						 cInterface::read);
	gMainInterface->IFSDefaultsIcosahedron(&gParFractal->at(index));
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
																						 &gParFractal->at(index),
																						 cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsOctahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
																						 &gParFractal->at(index),
																						 cInterface::read);
	gMainInterface->IFSDefaultsOctahedron(&gParFractal->at(index));
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
																						 &gParFractal->at(index),
																						 cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsMengerSponge()
{
	int index = ui->tabWidget_fractals->currentIndex();
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
																						 &gParFractal->at(index),
																						 cInterface::read);
	gMainInterface->IFSDefaultsMengerSponge(&gParFractal->at(index));
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
																						 &gParFractal->at(index),
																						 cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsReset()
{
	int index = ui->tabWidget_fractals->currentIndex();
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
																						 &gParFractal->at(index),
																						 cInterface::read);
	gMainInterface->IFSDefaultsReset(&gParFractal->at(index));
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
																						 &gParFractal->at(index),
																						 cInterface::write);
}

void RenderWindow::slotMenuAboutQt()
{
	QMessageBox::aboutQt(this);
}

void RenderWindow::slotMenuAboutThirdParty()
{
	QString text = "<h2>Third Party</h2>";
	text += "<ul>";
	text +=
			"	<li><b>Dark Skin</b> based on <br><a href=\"https://gist.github.com/QuantumCD/6245215\">gist.github.com/QuantumCD/6245215</a><br>thanks to QuantumCD</li>";
	text += "</ul>";
	QMessageBox::about(this, "About Third Party", text);
}

void RenderWindow::slotMenuAboutMandelbulber()
{
	QString text = "<h2>Mandelbulber</h2>";
	text += "version: <b>" + QString(MANDELBULBER_VERSION_STRING) + "</b>, build date: <b>"
			+ QString(__DATE__) + "</b><br>";
	text += "<br>";
	text += "Licence: GNU GPL version 3.0<br>";
	text += "Copyright Ⓒ 2015<br>";
	text += "project leader: Krzysztof Marczak<br>";
	text += "programmers:<br>";
	text += "Krzysztof Marczak<br>";
	text += "Sebastian Jennen<br>";
	text += "Graeme McLaren<br>";
	text += "Bernardo Martelli<br>";
	text += "<br>";
	text +=
			"Thanks to many friends from <a href=\"http://www.fractalforums.com\">www.fractalforums.com</a> for help<br>";
	text += "<br>";
	text += "<a href=\"http://www.mandelbulber.com\">www.mandelbulber.com</a>";

	QMessageBox::about(this, "About Mandelbulber", text);
}

void RenderWindow::slotMenuSaveImageJPEG()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("JPEG images (*.jpg *.jpeg)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("JPEG"));
	dialog.setDefaultSuffix("jpeg");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("JPG"),
																			tr("Saving image started"),
																			0.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		SaveImage(filename, IMAGE_FILE_TYPE_JPG, gMainInterface->mainImage);
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("JPG"),
																			tr("Saving image finished"),
																			1.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

void RenderWindow::slotMenuSaveImagePNG()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("PNG images (*.png)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("8-bit PNG"));
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("PNG"),
																			tr("Saving image started"),
																			0.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		SaveImage(filename, IMAGE_FILE_TYPE_PNG, gMainInterface->mainImage);
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("PNG"),
																			tr("Saving image finished"),
																			1.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

#ifdef USE_EXR
void RenderWindow::slotMenuSaveImageEXR()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("EXR images (*.exr)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("EXR"));
	dialog.setDefaultSuffix("exr");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("EXR"),
																			tr("Saving EXR image started"),
																			0.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		SaveImage(filename, IMAGE_FILE_TYPE_EXR, gMainInterface->mainImage);
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("EXR"),
																			tr("Saving EXR image finished"),
																			1.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}
#endif // USE_EXR

#ifdef USE_TIFF
void RenderWindow::slotMenuSaveImageTIFF()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("TIFF images (*.tiff)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("TIFF"));
	dialog.setDefaultSuffix("tiff");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("TIFF"),
																			tr("Saving TIFF image started"),
																			0.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		SaveImage(filename, IMAGE_FILE_TYPE_TIFF, gMainInterface->mainImage);
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("TIFF"),
																			tr("Saving TIFF image finished"),
																			1.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}
#endif // USE_TIFF

void RenderWindow::slotMenuSaveImagePNG16()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("PNG images (*.png)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("16-bit PNG"));
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG"),
																			tr("Saving image started"),
																			0.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		structSaveImageChannel saveImageChannel(IMAGE_CONTENT_COLOR, IMAGE_CHANNEL_QUALITY_16, "");
		SavePNG(filename, gMainInterface->mainImage, saveImageChannel, false);
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG"),
																			tr("Saving image finished"),
																			1.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

void RenderWindow::slotMenuSaveImagePNG16Alpha()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("PNG images (*.png)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("16-bit PNG + alpha channel"));
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG + alpha channel"),
																			tr("Saving image started"),
																			0.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		structSaveImageChannel saveImageChannel(IMAGE_CONTENT_COLOR, IMAGE_CHANNEL_QUALITY_16, "");
		SavePNG(filename, gMainInterface->mainImage, saveImageChannel, true);
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG + alpha channel"),
																			tr("Saving image finished"),
																			1.0,
																			cProgressText::progress_IMAGE);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

void RenderWindow::slotPressedButtonSelectBackgroundTexture()
{
	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
	dialog.setDirectory(QDir::toNativeSeparators(systemData.dataDirectory + QDir::separator()
			+ "textures"
			+ QDir::separator()));
	dialog.selectFile(QDir::toNativeSeparators(gPar->Get<QString>("file_background")));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select background texture..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
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
	dialog.setDirectory(QDir::toNativeSeparators(systemData.dataDirectory + QDir::separator()
			+ "textures"
			+ QDir::separator()));
	dialog.selectFile(QDir::toNativeSeparators(gPar->Get<QString>("file_envmap")));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select texture for environment mapping effect..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
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
	dialog.setDirectory(QDir::toNativeSeparators(systemData.dataDirectory + QDir::separator() + "textures"
			+ QDir::separator()));
	dialog.selectFile(QDir::toNativeSeparators(gPar->Get<QString>("file_lightmap")));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select texture for ambient occlusion light map..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
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
	dialog.setDirectory(QDir::toNativeSeparators(systemData.dataDirectory + QDir::separator() + "textures"
			+ QDir::separator()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.lastImagePaletteFile));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select image to grab colors..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
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

void RenderWindow::slotChangedComboAmbientOcclusionMode(int index)
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
	enumImageProportion proportionSelection = (enumImageProportion) index;

	switch (proportionSelection)
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

	if (!enableSlider)
	{
		ui->spinboxInt_image_width->setValue(width);
	}
}

void RenderWindow::slotPressedResolutionPreset()
{
	int width = 0, height = 0;
	enumImageProportion proportion = proportionFree;
	QString buttonName = this->sender()->objectName();
	if (buttonName == QString("pushButton_resolution_preset_480"))
	{
		width = 720;
		height = 480;
		proportion = proportion3_2;
	}
	else if (buttonName == QString("pushButton_resolution_preset_720"))
	{
		width = 1280;
		height = 720;
		proportion = proportion16_9;
	}
	else if (buttonName == QString("pushButton_resolution_preset_1080"))
	{
		width = 1920;
		height = 1080;
		proportion = proportion16_9;
	}
	else if (buttonName == QString("pushButton_resolution_preset_1440"))
	{
		width = 2560;
		height = 1440;
		proportion = proportion16_9;
	}
	else if (buttonName == QString("pushButton_resolution_preset_2160"))
	{
		width = 4096;
		height = 2160;
		proportion = proportionFree;
	}
	else if (buttonName == QString("pushButton_resolution_preset_4320"))
	{
		width = 7680;
		height = 4320;
		proportion = proportion16_9;
	}
	else if (buttonName == QString("pushButton_resolution_preset_240"))
	{
		width = 320;
		height = 240;
		proportion = proportion4_3;
	}
	else if (buttonName == QString("pushButton_resolution_preset_600"))
	{
		width = 800;
		height = 600;
		proportion = proportion4_3;
	}
	else if (buttonName == QString("pushButton_resolution_preset_1200"))
	{
		width = 1600;
		height = 1200;
		proportion = proportion4_3;
	}

	ui->spinboxInt_image_width->setValue(width);
	ui->spinboxInt_image_height->setValue(height);
	ui->comboBox_image_proportion->setCurrentIndex(proportion);
}

void RenderWindow::slotPressedImagesizeIncrease()
{
	int width = ui->spinboxInt_image_width->value();
	int height = ui->spinboxInt_image_height->value();
	width *= 2;
	height *= 2;
	ui->spinboxInt_image_width->setValue(width);
	ui->spinboxInt_image_height->setValue(height);
}

void RenderWindow::slotPressedImagesizeDecrease()
{
	int width = ui->spinboxInt_image_width->value();
	int height = ui->spinboxInt_image_height->value();
	width /= 2;
	height /= 2;
	ui->spinboxInt_image_width->setValue(width);
	ui->spinboxInt_image_height->setValue(height);
}

void RenderWindow::slotImageHeightChanged(int value)
{
	(void) value;
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
	params::enumPerspectiveType perspType = (params::enumPerspectiveType) index;
	if (perspType == params::perspFishEyeCut)
	{
		ui->comboBox_image_proportion->setCurrentIndex(proportion1_1);
		ui->spinbox_fov->setValue(1.0);
	}
	else if (perspType == params::perspEquirectangular)
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
	srand((unsigned int) clock());
	int seed = Random(999999);
	ui->spinboxInt_coloring_random_seed->setValue(seed);
	slotPressedButtonNewRandomPalette();
}

void RenderWindow::slotPressedButtonNewRandomPalette()
{
	gMainInterface->SynchronizeInterfaceWindow(ui->groupCheck_fractal_color, gPar, cInterface::read);
	cColorPalette palette(gPar->Get<int>("coloring_palette_size"),
												gPar->Get<int>("coloring_random_seed"),
												gPar->Get<double>("coloring_saturation"));
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
	(void) x;
	(void) y;
	//qDebug() << "mouse move event";
	//CVector2<int> point(x, y);
}

void RenderWindow::slotMouseClickOnImage(int x, int y, Qt::MouseButton button)
{
	int index = ui->comboBox_mouse_click_function->currentIndex();
	QList<QVariant> mode = ui->comboBox_mouse_click_function->itemData(index).toList();
	RenderedImage::enumClickMode clickMode = (RenderedImage::enumClickMode) mode.at(0).toInt();

	switch (clickMode)
	{
		case RenderedImage::clickMoveCamera:
		case RenderedImage::clickFogVisibility:
		case RenderedImage::clickDOFFocus:
		case RenderedImage::clickPlaceLight:
		case RenderedImage::clickGetJuliaConstant:
		case RenderedImage::clickPlacePrimitive:
		case RenderedImage::clickPlaceRandomLightCenter:
		case RenderedImage::clickGetPoint:
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
	item.append((int) RenderedImage::clickDOFFocus);
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetFogByMouse()
{
	QList<QVariant> item;
	item.append((int) RenderedImage::clickFogVisibility);
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonPlaceRandomLightsByMouse()
{
	QList<QVariant> item;
	item.append((int) RenderedImage::clickPlaceRandomLightCenter);
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
	(void) key;
}

void RenderWindow::slotMouseWheelRotatedOnImage(int delta)
{
	int index = ui->comboBox_mouse_click_function->currentIndex();
	QList<QVariant> mode = ui->comboBox_mouse_click_function->itemData(index).toList();
	RenderedImage::enumClickMode clickMode = (RenderedImage::enumClickMode) mode.at(0).toInt();
	switch (clickMode)
	{
		case RenderedImage::clickPlaceLight:
		{
			double deltaLog = exp(delta * 0.001);
			double dist = systemData.locale.toDouble(ui->logedit_aux_light_manual_placement_dist->text());
			dist *= deltaLog;
			ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(dist));
			break;
		}
		default:
			break;
	}
}

void RenderWindow::slotEditedLineEditManualLightPlacementDistance(const QString &text)
{
	gMainInterface->renderedImage->SetFrontDist(systemData.locale.toDouble(text));
}

void RenderWindow::slotSliderMovedEditManualLightPlacementDistance(int value)
{
	gMainInterface->renderedImage->SetFrontDist(pow(10.0, value / 100.0));
}

void RenderWindow::slotPressedButtonSetLight1ByMouse()
{
	QList<QVariant> item;
	item.append((int) RenderedImage::clickPlaceLight);
	item.append(1); //light number
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
	double distance = gMainInterface->GetDistanceForPoint(gPar->Get<CVector3>("camera"));
	ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(distance * 0.1));
}

void RenderWindow::slotPressedButtonSetLight2ByMouse()
{
	QList<QVariant> item;
	item.append((int) RenderedImage::clickPlaceLight);
	item.append(2); //light number
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetLight3ByMouse()
{
	QList<QVariant> item;
	item.append((int) RenderedImage::clickPlaceLight);
	item.append(3); //light number
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetLight4ByMouse()
{
	QList<QVariant> item;
	item.append((int) RenderedImage::clickPlaceLight);
	item.append(4); //light number
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonGetJuliaConstant()
{
	QList<QVariant> item;
	item.append((int) RenderedImage::clickGetJuliaConstant);
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonGetPoint()
{
	QList<QVariant> item;
	item.append((int) RenderedImage::clickGetPoint);
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
	item.append((int) RenderedImage::clickPlacePrimitive);
	item.append((int) PrimitiveNameToEnum(split.at(1)));
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
	tabifyDockWidget(ui->dockWidget_fractal, ui->dockWidget_histogram);
	tabifyDockWidget(ui->dockWidget_histogram, ui->dockWidget_info);
}

void RenderWindow::slotUpdateDocksandToolbarbyAction()
{
	// Animation dock
	if (ui->actionShow_animation_dock->isChecked() != ui->dockWidget_animation->isVisible())
	{
		if (ui->actionShow_animation_dock->isChecked())
		{
			addDockWidget(Qt::BottomDockWidgetArea, ui->dockWidget_animation);
		}
		else
		{
			removeDockWidget(ui->dockWidget_animation);
		}
		ui->dockWidget_animation->setVisible(ui->actionShow_animation_dock->isChecked());
	}

	// Information dock
	if (ui->actionShow_info_dock->isChecked() != ui->dockWidget_info->isVisible())
	{
		if (ui->actionShow_info_dock->isChecked())
		{
			addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_info);
		}
		else
		{
			removeDockWidget(ui->dockWidget_info);
		}
		ui->dockWidget_info->setVisible(ui->actionShow_info_dock->isChecked());
	}

	// Histogram dock
	if (ui->actionShow_statistics_dock->isChecked() != ui->dockWidget_histogram->isVisible())
	{
		if (ui->actionShow_statistics_dock->isChecked())
		{
			addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_histogram);
		}
		else
		{
			removeDockWidget(ui->dockWidget_histogram);
		}
		ui->dockWidget_histogram->setVisible(ui->actionShow_statistics_dock->isChecked());
	}

	// Toolbar
	if (ui->actionShow_toolbar->isChecked() != ui->toolBar->isVisible())
	{
		ui->toolBar->setVisible(ui->actionShow_toolbar->isChecked());
	}

	// Gamepad dock
	if (ui->actionShow_gamepad_dock->isChecked() != ui->dockWidget_gamepad_dock->isVisible())
	{
		if (ui->actionShow_gamepad_dock->isChecked())
		{
			addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget_gamepad_dock);
		}
		else
		{
			removeDockWidget(ui->dockWidget_gamepad_dock);
		}
		ui->dockWidget_gamepad_dock->setVisible(ui->actionShow_gamepad_dock->isChecked());
	}

	// Queue dock
	if (ui->actionShow_queue_dock->isChecked() != ui->dockWidget_queue_dock->isVisible())
	{
		if (ui->actionShow_queue_dock->isChecked())
		{
			addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget_queue_dock);
		}
		else
		{
			removeDockWidget(ui->dockWidget_queue_dock);
		}
		ui->dockWidget_queue_dock->setVisible(ui->actionShow_queue_dock->isChecked());
	}

  // Queue dock
  if (ui->actionShow_measurement_dock->isChecked() != ui->dockWidget_measurement->isVisible())
  {
    if (ui->actionShow_measurement_dock->isChecked())
    {
      addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget_measurement);
    }
    else
    {
      removeDockWidget(ui->dockWidget_measurement);
    }
    ui->dockWidget_measurement->setVisible(ui->actionShow_measurement_dock->isChecked());
  }
}

void RenderWindow::slotUpdateDocksandToolbarbyView()
{
	// Animation dock
	if (ui->actionShow_animation_dock->isChecked() != ui->dockWidget_animation->isVisible())
	{
		ui->actionShow_animation_dock->setChecked(ui->dockWidget_animation->isVisible());
	}

	// Log dock
	if (ui->actionShow_info_dock->isChecked() != ui->dockWidget_info->isVisible())
	{
		ui->actionShow_info_dock->setChecked(ui->dockWidget_info->isVisible());
	}

	// Histogram dock
	if (ui->actionShow_statistics_dock->isChecked() != ui->dockWidget_histogram->isVisible())
	{
		ui->actionShow_statistics_dock->setChecked(ui->dockWidget_histogram->isVisible());
	}

	// Toolbar
	if (ui->actionShow_toolbar->isChecked() != ui->toolBar->isVisible())
	{
		ui->actionShow_toolbar->setChecked(ui->toolBar->isVisible());
	}

	// Gamepad dock
	if (ui->actionShow_gamepad_dock->isChecked() != ui->dockWidget_gamepad_dock->isVisible())
	{
		ui->actionShow_gamepad_dock->setChecked(ui->dockWidget_gamepad_dock->isVisible());
	}

	// Queue dock
	if (ui->actionShow_queue_dock->isChecked() != ui->dockWidget_queue_dock->isVisible())
	{
		ui->actionShow_queue_dock->setChecked(ui->dockWidget_queue_dock->isVisible());
	}

  // Queue dock
  if (ui->actionShow_measurement_dock->isChecked() != ui->dockWidget_measurement->isVisible())
  {
    ui->actionShow_measurement_dock->setChecked(ui->dockWidget_measurement->isVisible());
  }
}

//adds dynamic actions to the toolbar (example settings)
void RenderWindow::slotPopulateToolbar(bool completeRefresh)
{
	WriteLog("cInterface::PopulateToolbar(QWidget *window, QToolBar *toolBar) started");
	QDir toolbarDir = QDir(systemData.dataDirectory + "toolbar");
	toolbarDir.setSorting(QDir::Time);
	QStringList toolbarFiles = toolbarDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
	QSignalMapper *mapPresetsFromExamplesLoad = new QSignalMapper(this);
	QSignalMapper *mapPresetsFromExamplesRemove = new QSignalMapper(this);
	ui->toolBar->setIconSize(QSize(gPar->Get<int>("toolbar_icon_size"), gPar->Get<int>("toolbar_icon_size")));

	QList<QAction *> actions = ui->toolBar->actions();
	QStringList toolbarInActions;
	for (int i = 0; i < actions.size(); i++)
	{
		QAction * action = actions.at(i);
		if(action->objectName() == "actionAdd_Settings_to_Toolbar") continue;
		if(!toolbarFiles.contains(action->objectName()) || completeRefresh)
		{
			// preset has been removed
			ui->toolBar->removeAction(action);
		}
		else
		{
			toolbarInActions << action->objectName();
		}
	}

	for (int i = 0; i < toolbarFiles.size(); i++)
	{
		if(toolbarInActions.contains(toolbarFiles.at(i)))
		{
			// already present
			continue;
		}
		QString filename = systemData.dataDirectory + "toolbar/" + toolbarFiles.at(i);
		cThumbnailWidget *thumbWidget = NULL;

		if (QFileInfo(filename).suffix() == QString("fract"))
		{
			WriteLogString("Generating thumbnail for preset", filename);
			cSettings parSettings(cSettings::formatFullText);
			parSettings.BeQuiet(true);

			if (parSettings.LoadFromFile(filename))
			{
				cParameterContainer *par = new cParameterContainer;
				cFractalContainer *parFractal = new cFractalContainer;
				InitParams(par);
				for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
					InitFractalParams(&parFractal->at(i));
				if (parSettings.Decode(par, parFractal))
				{
					thumbWidget = new cThumbnailWidget(gPar->Get<int>("toolbar_icon_size"), gPar->Get<int>("toolbar_icon_size"), 2, this);
					thumbWidget->UseOneCPUCore(true);
					thumbWidget->AssignParameters(*par, *parFractal);
				}
				delete par;
				delete parFractal;
			}
		}

		if(thumbWidget)
		{
			QWidgetAction *action = new QWidgetAction(this);
			QToolButton *buttonLoad = new QToolButton;
			QVBoxLayout *tooltipLayout = new QVBoxLayout;
			QToolButton *buttonRemove = new QToolButton;

			tooltipLayout->setContentsMargins(3, 3, 3, 3);
			tooltipLayout->addWidget(thumbWidget);
			QIcon iconDelete = QIcon::fromTheme("list-remove", QIcon(":system/icons/list-remove.svg"));
			buttonRemove->setIcon(iconDelete);
			buttonRemove->setMaximumSize(QSize(15, 15));
			buttonRemove->setStyleSheet("margin-bottom: -2px; margin-left: -2px;");
			tooltipLayout->addWidget(buttonRemove);
			buttonLoad->setToolTip(QObject::tr("Toolbar settings: ") + filename);
			buttonLoad->setLayout(tooltipLayout);
			action->setDefaultWidget(buttonLoad);
			action->setObjectName(toolbarFiles.at(i));
			ui->toolBar->addAction(action);

			mapPresetsFromExamplesLoad->setMapping(buttonLoad, filename);
			mapPresetsFromExamplesRemove->setMapping(buttonRemove, filename);
			QApplication::connect(buttonLoad, SIGNAL(clicked()), mapPresetsFromExamplesLoad, SLOT(map()));
			QApplication::connect(buttonRemove, SIGNAL(clicked()), mapPresetsFromExamplesRemove, SLOT(map()));
		}
	}
	QApplication::connect(mapPresetsFromExamplesLoad,
												SIGNAL(mapped(QString)),
												this,
												SLOT(slotMenuLoadPreset(QString)));

	QApplication::connect(mapPresetsFromExamplesRemove,
												SIGNAL(mapped(QString)),
												this,
												SLOT(slotMenuRemovePreset(QString)));

	WriteLog("cInterface::PopulateToolbar(QWidget *window, QToolBar *toolBar) finished");
}

void RenderWindow::slotPresetAddToToolbar()
{
	cSettings parSettings(cSettings::formatCondensedText);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
	QString filename = systemData.dataDirectory + "toolbar/" + parSettings.GetHashCode() + ".fract";
	parSettings.SaveToFile(filename);
	slotPopulateToolbar();
}

void RenderWindow::slotMenuLoadPreset(QString filename)
{
	cSettings parSettings(cSettings::formatFullText);
	parSettings.LoadFromFile(filename);
	parSettings.Decode(gPar, gParFractal);
	gMainInterface->RebuildPrimitives(gPar);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
	gMainInterface->ComboMouseClickUpdate();
	systemData.lastSettingsFile = gPar->Get<QString>("default_settings_path") + QDir::separator()
			+ QFileInfo(filename).fileName();
	this->setWindowTitle(QString("Mandelbulber (") + systemData.lastSettingsFile + ")");
}

void RenderWindow::slotMenuRemovePreset(QString filename)
{
	QFile::remove(filename);
	slotPopulateToolbar();
}

void RenderWindow::slotQuit()
{
	gMainInterface->QuitApplicationDialog();
}

void RenderWindow::closeEvent(QCloseEvent * event)
{
	if (gMainInterface->QuitApplicationDialog())
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void RenderWindow::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		// retranslate designer form (single inheritance approach)
		ui->retranslateUi(this);
	}

	// remember to call base class implementation
	QMainWindow::changeEvent(event);
}

void RenderWindow::slotUpdateProgressAndStatus(const QString &text, const QString &progressText,
		double progress, cProgressText::enumProgressType progressType)
{
	ui->statusbar->showMessage(text, 0);
	QProgressBar *progressBar = NULL;
	bool isQueue = this->sender() && this->sender()->objectName() == "Queue";
	switch (progressType)
	{
		case cProgressText::progress_IMAGE:
			if (isQueue) progressBar = gMainInterface->progressBarQueueImage;
			else progressBar = gMainInterface->progressBar;
			break;
		case cProgressText::progress_ANIMATION:
			if (isQueue) progressBar = gMainInterface->progressBarQueueAnimation;
			else progressBar = gMainInterface->progressBarAnimation;
			break;
		case cProgressText::progress_QUEUE:
			// nothing to be done, no progress bar for queue in GUI
			break;
	}

	if (progressBar)
	{
		if (!progressBar->isVisible())
		{
			progressBar->setVisible(true);
		}
		progressBar->setValue(progress * 1000.0);
		progressBar->setTextVisible(true);
		progressBar->setFormat(progressText);
	}
}

void RenderWindow::slotUpdateProgressHide(cProgressText::enumProgressType progressType)
{
	QProgressBar *progressBar = NULL;
	switch (progressType)
	{
		case cProgressText::progress_IMAGE:
			progressBar = gMainInterface->progressBar;
			break;
		case cProgressText::progress_ANIMATION:
			progressBar = gMainInterface->progressBarAnimation;
			break;
		case cProgressText::progress_QUEUE:
			// nothing to be done, no progress bar for queue in GUI
			break;
	}

	if (progressBar)
	{
		if (progressBar->isVisible()) progressBar->setVisible(false);
	}
}

void RenderWindow::slotMenuProgramSettings()
{
	cPreferencesDialog *preferencesDialog = new cPreferencesDialog;
	preferencesDialog->show();
}

void RenderWindow::slotUpdateStatistics(cStatistics stat)
{
	ui->label_histogram_de->SetBarcolor(QColor(0, 255, 0));
	ui->label_histogram_de->UpdateHistogram(stat.histogramStepCount);
	ui->label_histogram_iter->UpdateHistogram(stat.histogramIterations);

	ui->tableWidget_statistics->item(0, 0)->setText(QString::number(stat.GetTotalNumberOfIterations()));
	ui->tableWidget_statistics->item(1, 0)->setText(QString::number(stat.GetNumberOfIterationsPerPixel()));
	ui->tableWidget_statistics->item(2, 0)->setText(QString::number(stat.GetNumberOfIterationsPerSecond()));
	ui->tableWidget_statistics->item(3, 0)->setText(stat.GetDETypeString());
	ui->tableWidget_statistics->item(4, 0)->setText(QString::number(stat.GetMissedDEPercentage()));
	ui->label_wrong_DE_percentage->setText(tr("Percentage of wrong distance estimations: %1").arg(stat.GetMissedDEPercentage()));
}

void RenderWindow::slotNetRenderServerStart()
{
	gMainInterface->SynchronizeInterfaceWindow(ui->group_netrender, gPar, cInterface::read);
	qint32 port = gPar->Get<int>("netrender_server_local_port");
	gNetRender->SetServer(port);
}

void RenderWindow::slotNetRenderServerStop()
{
	gNetRender->DeleteServer();
}

void RenderWindow::slotNetRenderClientConnect()
{
	gMainInterface->SynchronizeInterfaceWindow(ui->group_netrender, gPar, cInterface::read);
	QString address = gPar->Get<QString>("netrender_client_remote_address");
	qint32 port = gPar->Get<int>("netrender_client_remote_port");
	gNetRender->SetClient(address, port);
}

void RenderWindow::slotNetRenderClientDisconnect()
{
	gNetRender->DeleteClient();
}

void RenderWindow::slotNetRenderStatusServerUpdate()
{
	QString text = CNetRender::GetStatusText(gNetRender->GetStatus());
	QString color = CNetRender::GetStatusColor(gNetRender->GetStatus());
	ui->label_netrender_server_status->setText(text);
	ui->label_netrender_server_status->setStyleSheet("QLabel { color: " + color
			+ "; font-weight: bold; }");

	ui->bu_netrender_start_server->setEnabled(!gNetRender->IsServer());
	ui->bu_netrender_stop_server->setEnabled(gNetRender->IsServer());
}

void RenderWindow::slotNetRenderStatusClientUpdate()
{
	QString text = CNetRender::GetStatusText(gNetRender->GetStatus());
	QString color = CNetRender::GetStatusColor(gNetRender->GetStatus());
	ui->label_netrender_client_status->setText(text);
	ui->label_netrender_client_status->setStyleSheet("QLabel { color: " + color
			+ "; font-weight: bold; }");

	ui->bu_netrender_connect->setEnabled(!gNetRender->IsClient());
	ui->bu_netrender_disconnect->setEnabled(gNetRender->IsClient());
	ui->pushButton_render->setEnabled(!gNetRender->IsClient());
}

void RenderWindow::slotNetRenderClientServerChange(int index)
{
	ui->groupBox_netrender_client_config->setVisible(index == CNetRender::netRender_CLIENT);
	ui->groupBox_netrender_server_config->setVisible(index == CNetRender::netRender_SERVER);
}

void RenderWindow::slotNetRenderClientListUpdate()
{
	QTableWidget *table = ui->tableWidget_netrender_connected_clients;

	// reset table
	if (gNetRender->GetClientCount() == 0)
	{
		table->clear();
		return;
	}

	// init table
	if (table->columnCount() == 0)
	{
		QStringList header;
		header << tr("Name") << tr("Host") << tr("CPUs") << tr("Status") << tr("Lines done");
		table->setColumnCount(header.size());
		table->setHorizontalHeaderLabels(header);
	}

	// change table
	if (table->rowCount() != gNetRender->GetClientCount())
	{
		table->setRowCount(gNetRender->GetClientCount());
	}

	// update table
	for (int i = 0; i < table->rowCount(); i++)
	{
		slotNetRenderClientListUpdate(i);
	}
}

void RenderWindow::slotNetRenderClientListUpdate(int i)
{
	// update row i
	QTableWidget *table = ui->tableWidget_netrender_connected_clients;
	for (int j = 0; j < table->columnCount(); j++)
	{
		slotNetRenderClientListUpdate(i, j);
	}
}

void RenderWindow::slotNetRenderClientListUpdate(int i, int j)
{
	// update element in row i, column j
	QTableWidget *table = ui->tableWidget_netrender_connected_clients;

	QTableWidgetItem *cell = table->item(i, j);
	if (!cell)
	{
		cell = new QTableWidgetItem;
		table->setItem(i, j, cell);
	}

	switch (j)
	{
		case 0:
			cell->setText(gNetRender->GetClient(i).name);
			break;
		case 1:
			cell->setText(gNetRender->GetClient(i).socket->peerAddress().toString());
			break;
		case 2:
			cell->setText(QString::number(gNetRender->GetClient(i).clientWorkerCount));
			break;
		case 3:
		{
			QString text = CNetRender::GetStatusText(gNetRender->GetClient(i).status);
			QString color = CNetRender::GetStatusColor(gNetRender->GetClient(i).status);

			cell->setText(text);
			cell->setTextColor(color);
			// ui->label_netrender_client_status->setStyleSheet("QLabel { background-color: " + color + "; }");
			// cell->setBackgroundColor(QColor(255, 0, 0));
			break;
		}
		case 4:
			cell->setText(QString::number(gNetRender->GetClient(i).linesRendered));
			break;
	}
}

void RenderWindow::slotCheckBoxDisableNetRender(bool on)
{
	if (!on)
	{
		gNetRender->DeleteClient();
		gNetRender->DeleteServer();
	}
}

void RenderWindow::slotQuestionMessage(const QString &questionTitle, const QString &questionText,
		QMessageBox::StandardButtons buttons, QMessageBox::StandardButton *reply)
{
	*reply = QMessageBox::question(ui->centralwidget, questionTitle, questionText, buttons);
}

void RenderWindow::slotPressedButtonOptimizeForLQ()
{
	gMainInterface->OptimizeStepFactor(1.0);
}
void RenderWindow::slotPressedButtonOptimizeForMQ()
{
	gMainInterface->OptimizeStepFactor(0.1);
}
void RenderWindow::slotPressedButtonOptimizeForHQ()
{
	gMainInterface->OptimizeStepFactor(0.01);
}

void RenderWindow::slotPressedButtonResetFormula()
{
	QString comboName = this->sender()->objectName();
	int fractalNumber = comboName.right(1).toInt() - 1;
	gMainInterface->ResetFormula(fractalNumber);
}

void RenderWindow::slotFractalSwap(int swapA, int swapB)
{
	qDebug() << "swapping " << swapA << " with " << swapB;
	QWidget *fractalWidgetTemp;

	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[swapB],
																						 &gParFractal->at(swapA),
																						 cInterface::read);
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[swapA],
																						 &gParFractal->at(swapB),
																						 cInterface::read);
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[swapA],
																						 &gParFractal->at(swapA),
																						 cInterface::write);
	gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[swapB],
																						 &gParFractal->at(swapB),
																						 cInterface::write);

	QString nameA = ui->tabWidget_fractals->widget(swapA)->objectName();
	ui->tabWidget_fractals->widget(swapA)->setObjectName(
				ui->tabWidget_fractals->widget(swapB)->objectName());
	ui->tabWidget_fractals->widget(swapB)->setObjectName(nameA);
}

void RenderWindow::slotChangedCheckBoxUseDefaultBailout(int state)
{
	ui->logslider_bailout->setEnabled(!state);
	ui->logedit_bailout->setEnabled(!state);
}

void RenderWindow::slotChangedComboFractalColoringAlgorithm(int index)
{
	fractal::enumFractalColoringAlgorithm selection = (fractal::enumFractalColoringAlgorithm)index;
	ui->slider_fractal_coloring_sphere_radius->setEnabled(selection == fractal::fractalColoringSphere);
	ui->spinbox_fractal_coloring_sphere_radius->setEnabled(selection == fractal::fractalColoringSphere);
	ui->vect3_fractal_coloring_line_direction_x->setEnabled(selection == fractal::fractalColoringLine);
	ui->vect3_fractal_coloring_line_direction_y->setEnabled(selection == fractal::fractalColoringLine);
	ui->vect3_fractal_coloring_line_direction_z->setEnabled(selection == fractal::fractalColoringLine);
}

void RenderWindow::slotChangedCheckBoxDOFHDR(int state)
{
	ui->pushButton_DOF_update->setEnabled(!state);
	ui->pushButton_apply_image_changes->setEnabled(!state);
	if(ui->comboBox_ambient_occlusion_mode->currentIndex() == 2 && !state)
	{
		ui->comboBox_ambient_occlusion_mode->setCurrentIndex(0);
	}
}

#ifdef USE_GAMEPAD
void RenderWindow::slotChangeGamepadIndex(int index)
{
	gamepad.setDeviceId(index);
	WriteLog("Gamepad - slotChangeGamepadIndex: " + QString::number(index));
}

void RenderWindow::slotGamePadDeviceConnected(int index)
{
	QString deviceName = ""; // TODO gamepad devicename
	if (deviceName == "") deviceName = "Device #" + QString::number(index);
	WriteLog("Gamepad - device connected | index: " + QString::number(index) + ", name: "
			+ deviceName);

	if (ui->comboBox_gamepad_device->count() == 0)
	{
		ui->comboBox_gamepad_device->setEnabled(true);
		ui->label_gamepad_no_device->hide();
	}
	ui->comboBox_gamepad_device->addItem(deviceName, index);
}

void RenderWindow::slotGamePadDeviceDisconnected(int index)
{
	WriteLog("Gamepad - device disconnected | index: " + QString::number(index) + ", name: "
			+ ui->comboBox_gamepad_device->itemText(index));
	ui->comboBox_gamepad_device->removeItem(index);
	if (ui->comboBox_gamepad_device->count() == 0)
	{
		ui->comboBox_gamepad_device->setEnabled(false);
		ui->label_gamepad_no_device->show();
	}
}

void RenderWindow::slotGamepadPitch(double value)
{
	WriteLog("Gamepad - slotGamepadPitch | value: " + QString::number(value));
	ui->sl_gamepad_angle_pitch->setValue(-100 + 200 * value);
	CVector2<double> yawPitch(value, gamepad.axisLeftY());
	yawPitch = (yawPitch * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->YawAndPitchChanged(yawPitch);
}

void RenderWindow::slotGamepadYaw(double value)
{
	WriteLog("Gamepad - slotGamepadYaw | value: " + QString::number(value));
	ui->sl_gamepad_angle_yaw->setValue(-100 + 200 * value);
	CVector2<double> yawPitch(gamepad.axisLeftX(), value);
	yawPitch = (yawPitch * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->YawAndPitchChanged(yawPitch);
}

void RenderWindow::slotGamepadRoll()
{
	double value = 0.5 + (gamepad.buttonR2() - gamepad.buttonL2()) / 2.0;
	WriteLog("Gamepad - slotGamepadRoll | value: " + QString::number(value));
	ui->sl_gamepad_angle_roll->setValue(-100 + 200 * value);
	emit gMainInterface->renderedImage->RotationChanged(value * 2.0 - 1.0);
}

void RenderWindow::slotShiftModeChange(bool isShifting)
{
	WriteLog("Gamepad - slotShiftModeChange | value: " + QString::number(isShifting));
	emit gMainInterface->renderedImage->ShiftModeChanged(isShifting);
}

void RenderWindow::slotGamepadX(double value)
{
	WriteLog("Gamepad - slotGamepadX | value: " + QString::number(value));
	ui->sl_gamepad_movement_x->setValue(-100 + 200 * value);
	CVector2<double> strafe(value, gamepad.axisRightY());
	strafe = (strafe * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->StrafeChanged(strafe);
}

void RenderWindow::slotGamepadY(double value)
{
	WriteLog("Gamepad - slotGamepadY | value: " + QString::number(value));
	ui->sl_gamepad_movement_y->setValue(-100 + 200 * value);
	CVector2<double> strafe(gamepad.axisRightX(), value);
	strafe = (strafe * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->StrafeChanged(strafe);
}

void RenderWindow::slotGamepadZ()
{
	double value = 0.5 + ((gamepad.buttonA() ? 1 : 0) - (gamepad.buttonB() ? 1 : 0)) / 2.0;
	WriteLog("Gamepad - slotGamepadZ | value: " + QString::number(value));
	ui->sl_gamepad_movement_z->setValue(-100 + 200 * value);
	if (gamepad.buttonA() != gamepad.buttonB())
	{
		if (gamepad.buttonA())
		{
			emit gMainInterface->renderedImage->SpeedChanged(1.1);
		}
		else
		{
			emit gMainInterface->renderedImage->SpeedChanged(0.9);
		}
	}
}

#endif // USE_GAMEPAD
