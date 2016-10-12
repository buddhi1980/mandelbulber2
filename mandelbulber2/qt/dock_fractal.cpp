/*
 * dock_fractal.cpp
 *
 *  Created on: 12 paź 2016
 *      Author: krzysztof
 */

#include "dock_fractal.h"

#include "../qt/my_tab_bar.h"
#include "../src/error_message.hpp"
#include "../src/initparameters.hpp"
#include "../src/interface.hpp"
#include "../src/my_ui_loader.h"
#include "../src/system.hpp"
#include "../src/global_data.hpp"
#include "ui_dock_fractal.h"

cDockFractal::cDockFractal(QWidget *parent) : QWidget(parent), ui(new Ui::cDockFractal)
{
	ui->setupUi(this);

	fractalWidgets = new QWidget *[NUMBER_OF_FRACTALS];

	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);

	ui->label_repeat_from->setEnabled(false);
	ui->sliderInt_repeat_from->setEnabled(false);
	ui->spinboxInt_repeat_from->setEnabled(false);
	ui->label_fractals_remark_julia->setVisible(false);

	ui->previewwidget_julia->SetSize(256, 256, 2);

	ConnectSignals();
}

cDockFractal::~cDockFractal()
{
	delete ui;

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		delete fractalWidgets[i];
	delete[] fractalWidgets;
}

bool cDockFractal::AreHybridFractalsEnabled()
{
	return ui->checkBox_hybrid_fractal_enable->isChecked();
}

void cDockFractal::SynchronizeInterfaceFractals(
	cParameterContainer *par, cFractalContainer *parFractal, qInterface::enumReadWrite mode)
{
	WriteLog("cInterface::SynchronizeInterface: tabWidget_fractal_common", 3);
	SynchronizeInterfaceWindow(ui->tabWidget_fractal_common, par, mode);
	WriteLog("cInterface::SynchronizeInterface: tabWidget_fractal_hybrid", 3);
	SynchronizeInterfaceWindow(ui->tabWidget_fractal_hybrid, par, mode);
	WriteLog("cInterface::SynchronizeInterface: tab_primitives", 3);
	SynchronizeInterfaceWindow(ui->tab_primitives, par, mode);
	WriteLog("cInterface::SynchronizeInterface: tab_description", 3);
	SynchronizeInterfaceWindow(ui->tab_description, par, mode);

	WriteLog("cInterface::SynchronizeInterface: tabWidget_fractals", 3);
	SynchronizeInterfaceWindow(ui->tabWidget_fractals->tabBar(), par, mode);

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		WriteLog("cInterface::SynchronizeInterface: fractalWidgets[i]", 3);
		SynchronizeInterfaceWindow(fractalWidgets[i], &parFractal->at(i), mode);
		WriteLog("cInterface::SynchronizeInterface: frame_iterations_formula", 3);
		SynchronizeInterfaceWindow(ui->tabWidget_fractals->findChild<QFrame *>(
																 QString("frame_iterations_formula_%1").arg(i + 1)),
			par, mode);
		WriteLog("cInterface::SynchronizeInterface: groupBox_formula_transform", 3);
		SynchronizeInterfaceWindow(ui->tabWidget_fractals->findChild<QGroupBox *>(
																 QString("groupBox_formula_transform_%1").arg(i + 1)),
			par, mode);
		WriteLog("cInterface::SynchronizeInterface: groupBox_c_constant_addition", 3);
		SynchronizeInterfaceWindow(ui->tabWidget_fractals->findChild<QGroupBox *>(
																 QString("groupBox_c_constant_addition_%1").arg(i + 1)),
			par, mode);
		WriteLog("cInterface::SynchronizeInterface: groupBox_material_fractal", 3);
		SynchronizeInterfaceWindow(ui->tabWidget_fractals->findChild<QGroupBox *>(
																 QString("groupBox_material_fractal_%1").arg(i + 1)),
			par, mode);
	}
}

void cDockFractal::SynchronizeInterfaceJulia()
{
	SynchronizeInterfaceWindow(ui->groupCheck_julia_mode, gPar, qInterface::write);
}

void cDockFractal::SynchronizeInterfacePrimitives()
{
	SynchronizeInterfaceWindow(ui->scrollArea_primitives, gPar, qInterface::write);
}

void cDockFractal::EnableJuliaMode()
{
	ui->groupCheck_julia_mode->setChecked(true);
}

QWidget *cDockFractal::GetContainerWithPrimitives()
{
	return ui->scrollAreaWidgetContents_primitives;
}

QVBoxLayout *cDockFractal::GetLayoutWithPrimitives()
{
	return ui->verticalLayout_primitives;
}

void cDockFractal::ConnectSignals()
{
	connect(
		ui->vect3_julia_c_x, SIGNAL(textChanged(const QString &)), this, SLOT(slotChangedJuliaPoint()));
	connect(
		ui->vect3_julia_c_y, SIGNAL(textChanged(const QString &)), this, SLOT(slotChangedJuliaPoint()));
	connect(
		ui->vect3_julia_c_z, SIGNAL(textChanged(const QString &)), this, SLOT(slotChangedJuliaPoint()));

	connect(ui->pushButton_get_julia_constant, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonGetJuliaConstant()));

	connect(ui->pushButton_add_primitive_box, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonNewPrimitive()));
	connect(ui->pushButton_add_primitive_circle, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonNewPrimitive()));
	connect(ui->pushButton_add_primitive_cone, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonNewPrimitive()));
	connect(ui->pushButton_add_primitive_cylinder, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonNewPrimitive()));
	connect(ui->pushButton_add_primitive_plane, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonNewPrimitive()));
	connect(ui->pushButton_add_primitive_rectangle, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonNewPrimitive()));
	connect(ui->pushButton_add_primitive_sphere, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonNewPrimitive()));
	connect(ui->pushButton_add_primitive_water, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonNewPrimitive()));
	connect(ui->pushButton_add_primitive_torus, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonNewPrimitive()));

	connect(ui->groupCheck_julia_mode, SIGNAL(toggled(bool)), this,
		SLOT(slotGroupCheckJuliaModeToggled(bool)));

	connect(ui->checkBox_hybrid_fractal_enable, SIGNAL(stateChanged(int)), this,
		SLOT(slotChangedCheckBoxHybridFractal(int)));
	connect(ui->groupCheck_boolean_operators, SIGNAL(toggled(bool)), this,
		SLOT(slotChangedCheckBoxBooleanOperators(bool)));
	connect(ui->groupCheck_julia_mode, SIGNAL(toggled(bool)), this,
		SLOT(slotChangedCheckBoxJuliaMode(bool)));
	connect(ui->tabWidget_fractals->tabBar(), SIGNAL(toggledEnable(int, bool)), this,
		SLOT(slotToggledFractalEnable(int, bool)));

	connect(
		ui->tabWidget_fractals, SIGNAL(swapTabs(int, int)), this, SLOT(slotFractalSwap(int, int)));
}

// initialize ui for hybrid fractal components
void cDockFractal::InitializeFractalUi(QString &uiFileName)
{
	WriteLog("cInterface::InitializeFractalUi(QString &uiFileName) started", 2);
	MyUiLoader loader;

	QFile uiFile(uiFileName);

	if (uiFile.exists())
	{
		uiFile.open(QFile::ReadOnly);
		fractalWidgets[0] = loader.load(&uiFile);
		ui->verticalLayout_fractal_1->addWidget(this->fractalWidgets[0]);
		fractalWidgets[0]->show();
		for (int i = 1; i < NUMBER_OF_FRACTALS; i++)
		{
			this->fractalWidgets[i] = NULL;
		}

		for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
		{
			if (i == 1)
				ui->tabWidget_fractals->setTabText(i - 1, QString("#1: Mandelbulb"));
			else
				ui->tabWidget_fractals->setTabText(i - 1, QString("#%1: None").arg(i));

			QFrame *frame = ui->tabWidget_fractals->findChild<QFrame *>(
				"frame_iterations_formula_" + QString::number(i));

			QComboBox *combo =
				frame->findChild<QComboBox *>(QString("comboBox_formula_") + QString::number(i));
			combo->clear();

			combo->setIconSize(QSize(32, 32));
			combo->setFixedHeight(32);
			for (int f = 0; f < fractalList.size(); f++)
			{
				combo->addItem(QIcon(fractalList[f].getIconName()), fractalList[f].nameInComboBox, f);
			}

			// set headings and separator of formulas and transforms
			QFont fontHeading;
			fontHeading.setBold(true);
			combo->insertItem(0, QObject::tr("Formulas"));
			combo->setItemData(0, fontHeading, Qt::FontRole);
			combo->setItemData(0, Qt::AlignCenter, Qt::TextAlignmentRole);
			qobject_cast<QStandardItemModel *>(combo->model())->item(0)->setEnabled(false);
			int indexBeforeTransforms = combo->findText("Transform - Addition Constant");
			combo->insertItem(indexBeforeTransforms, QObject::tr("Transforms"));
			combo->setItemData(indexBeforeTransforms, fontHeading, Qt::FontRole);
			combo->setItemData(indexBeforeTransforms, Qt::AlignCenter, Qt::TextAlignmentRole);
			qobject_cast<QStandardItemModel *>(combo->model())
				->item(indexBeforeTransforms)
				->setEnabled(false);
			combo->insertSeparator(indexBeforeTransforms);

			connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangedComboFractal(int)));

			QPushButton *resetButton =
				frame->findChild<QPushButton *>(QString("pushButton_reset_formula_") + QString::number(i));
			connect(resetButton, SIGNAL(clicked()), this, SLOT(slotPressedButtonResetFormula()));

			frame->findChild<QLabel *>(QString("label_formula_iterations_") + QString::number(i))
				->setVisible(false);
			frame->findChild<MySpinBox *>(QString("spinboxInt_formula_iterations_") + QString::number(i))
				->setVisible(false);
			frame->findChild<QSlider *>(QString("sliderInt_formula_iterations_") + QString::number(i))
				->setVisible(false);

			frame->findChild<QLabel *>(QString("label_formula_weight_") + QString::number(i))
				->setVisible(false);
			frame->findChild<MyDoubleSpinBox *>(QString("spinbox_formula_weight_") + QString::number(i))
				->setVisible(false);
			frame->findChild<QSlider *>(QString("slider_formula_weight_") + QString::number(i))
				->setVisible(false);

			frame->findChild<QLabel *>(QString("label_formula_start_iteration_") + QString::number(i))
				->setVisible(false);
			frame->findChild<QLabel *>(QString("label_formula_stop_iteration_") + QString::number(i))
				->setVisible(false);
			frame->findChild<MySpinBox *>(QString("spinboxInt_formula_start_iteration_")
																		+ QString::number(i))->setVisible(false);
			frame->findChild<MySpinBox *>(QString("spinboxInt_formula_stop_iteration_")
																		+ QString::number(i))->setVisible(false);

			frame->findChild<MyCheckBox *>(QString("checkBox_check_for_bailout_") + QString::number(i))
				->setVisible(false);
			frame->findChild<MyCheckBox *>(QString("checkBox_dont_add_c_constant_") + QString::number(i))
				->setText(QObject::tr("Don't add global C constant"));

			if (i > 1)
			{
				frame->setEnabled(false);
				ui->tabWidget_fractals->findChild<QScrollArea *>("scrollArea_fractal_" + QString::number(i))
					->setEnabled(false);
			}

			ui->tabWidget_fractals->findChild<QGroupBox *>("groupBox_formula_transform_"
																										 + QString::number(i))->setVisible(false);
			ui->tabWidget_fractals->findChild<QGroupBox *>("groupBox_c_constant_addition_"
																										 + QString::number(i))->setVisible(false);
			ui->tabWidget_fractals->findChild<QGroupBox *>("groupBox_material_fractal_"
																										 + QString::number(i))->setVisible(false);
		}
		static_cast<MyTabBar *>(ui->tabWidget_fractals->tabBar())->setupMoveButtons();
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Can't open file ") + uiFileName
																 + QObject::tr(" Fractal ui files can't be loaded"),
			cErrorMessage::errorMessage, this);
	}
	WriteLog("cInterface::InitializeFractalUi(QString &uiFileName) finished", 2);
}

void cDockFractal::slotChangedComboFractal(int indexInComboBox)
{
	QString comboName = this->sender()->objectName();
	int index = qobject_cast<QComboBox *>(this->sender())->itemData(indexInComboBox).toInt();
	int fractalNumber = comboName.right(1).toInt() - 1;

	QString fullFormulaName = fractalList[index].nameInComboBox;
	if (fractalList[index].internalID > 0)
	{
		QString formulaName = fractalList[index].internalName;
		QString uiFilename =
			systemData.sharedDir + "qt_data" + QDir::separator() + "fractal_" + formulaName + ".ui";

		if (fractalWidgets[fractalNumber]) delete fractalWidgets[fractalNumber];
		fractalWidgets[fractalNumber] = NULL;

		MyUiLoader loader;
		QFile uiFile(uiFilename);

		if (uiFile.exists())
		{
			uiFile.open(QFile::ReadOnly);
			fractalWidgets[fractalNumber] = loader.load(&uiFile);
			QVBoxLayout *layout = this->findChild<QVBoxLayout *>(
				"verticalLayout_fractal_" + QString::number(fractalNumber + 1));
			layout->addWidget(fractalWidgets[fractalNumber]);
			uiFile.close();
			fractalWidgets[fractalNumber]->show();
			automatedWidgets->ConnectSignalsForSlidersInWindow(fractalWidgets[fractalNumber]);
			SynchronizeInterfaceWindow(
				fractalWidgets[fractalNumber], &gParFractal->at(fractalNumber), qInterface::write);

			QFrame *frame = ui->tabWidget_fractals->findChild<QFrame *>(
				"frame_iterations_formula_" + QString::number(fractalNumber + 1));
			MyCheckBox *caddCheckBoxframe = frame->findChild<MyCheckBox *>(
				QString("checkBox_dont_add_c_constant_") + QString::number(fractalNumber + 1));

			switch (fractalList[index].cpixelAddition)
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
				ui->tabWidget_fractals->findChild<QGroupBox *>("groupBox_c_constant_addition_"
																											 + QString::number(fractalNumber + 1))
					->setVisible(false);
			else
				ui->tabWidget_fractals->findChild<QGroupBox *>("groupBox_c_constant_addition_"
																											 + QString::number(fractalNumber + 1))
					->setVisible(boleanState);

			if (fractalList[index].internalID == fractal::kaleidoscopicIFS)
			{
				QWidget *pushButton_preset_dodecahedron =
					fractalWidgets[fractalNumber]->findChild<QWidget *>("pushButton_preset_dodecahedron");
				QApplication::connect(pushButton_preset_dodecahedron, SIGNAL(clicked()), this,
					SLOT(slotPressedButtonIFSDefaultsDodecahedron()));
				QWidget *pushButton_preset_icosahedron =
					fractalWidgets[fractalNumber]->findChild<QWidget *>("pushButton_preset_icosahedron");
				QApplication::connect(pushButton_preset_icosahedron, SIGNAL(clicked()), this,
					SLOT(slotPressedButtonIFSDefaultsIcosahedron()));
				QWidget *pushButton_preset_octahedron =
					fractalWidgets[fractalNumber]->findChild<QWidget *>("pushButton_preset_octahedron");
				QApplication::connect(pushButton_preset_octahedron, SIGNAL(clicked()), this,
					SLOT(slotPressedButtonIFSDefaultsOctahedron()));
				QWidget *pushButton_preset_menger_sponge =
					fractalWidgets[fractalNumber]->findChild<QWidget *>("pushButton_preset_menger_sponge");
				QApplication::connect(pushButton_preset_menger_sponge, SIGNAL(clicked()), this,
					SLOT(slotPressedButtonIFSDefaultsMengerSponge()));
				QWidget *pushButton_preset_reset =
					fractalWidgets[fractalNumber]->findChild<QWidget *>("pushButton_preset_reset");
				QApplication::connect(pushButton_preset_reset, SIGNAL(clicked()), this,
					SLOT(slotPressedButtonIFSDefaultsReset()));
			}
		}
		else
		{
			cErrorMessage::showMessage(
				QString("Can't open file ") + uiFilename + QString("\nFractal ui file can't be loaded"),
				cErrorMessage::errorMessage, gMainInterface->mainWindow);
		}
	}
	else
	{
		if (fractalWidgets[fractalNumber]) delete fractalWidgets[fractalNumber];
		fractalWidgets[fractalNumber] = NULL;
	}
	ui->tabWidget_fractals->setTabText(
		fractalNumber, QString("#%1: %2").arg(fractalNumber + 1).arg(fullFormulaName));
	// ui->tabWidget_fractals->setTabIcon(fractalNumber, QIcon(fractalList[index].getIconName()));
}

void cDockFractal::slotPressedButtonIFSDefaultsDodecahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), qInterface::read);
	gMainInterface->IFSDefaultsDodecahedron(&gParFractal->at(index));
	SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), qInterface::write);
}

void cDockFractal::slotPressedButtonIFSDefaultsIcosahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), qInterface::read);
	gMainInterface->IFSDefaultsIcosahedron(&gParFractal->at(index));
	SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), qInterface::write);
}

void cDockFractal::slotPressedButtonIFSDefaultsOctahedron()
{
	int index = ui->tabWidget_fractals->currentIndex();
	SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), qInterface::read);
	gMainInterface->IFSDefaultsOctahedron(&gParFractal->at(index));
	SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), qInterface::write);
}

void cDockFractal::slotPressedButtonIFSDefaultsMengerSponge()
{
	int index = ui->tabWidget_fractals->currentIndex();
	SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), qInterface::read);
	gMainInterface->IFSDefaultsMengerSponge(&gParFractal->at(index));
	SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), qInterface::write);
}

void cDockFractal::slotPressedButtonIFSDefaultsReset()
{
	int index = ui->tabWidget_fractals->currentIndex();
	SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), qInterface::read);
	gMainInterface->IFSDefaultsReset(&gParFractal->at(index));
	SynchronizeInterfaceWindow(fractalWidgets[index], &gParFractal->at(index), qInterface::write);
}

void cDockFractal::slotFractalSwap(int swapA, int swapB)
{
	// qDebug() << "swapping " << swapA << " with " << swapB;

	// read all data from ui
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);

	// swap formula specific fields in gPar
	QStringList gParFormulaSpecificFields({"formula", "formula_iterations", "formula_weight",
		"formula_start_iteration", "formula_stop_iteration", "julia_mode", "julia_c",
		"fractal_constant_factor", "formula_position", "formula_rotation", "formula_repeat",
		"formula_scale", "dont_add_c_constant", "check_for_bailout"});

	for (int i = 0; i < gParFormulaSpecificFields.size(); i++)
	{
		cOneParameter formulaA =
			gPar->GetAsOneParameter(gParFormulaSpecificFields.at(i) + "_" + QString::number(swapA + 1));
		cOneParameter formulaB =
			gPar->GetAsOneParameter(gParFormulaSpecificFields.at(i) + "_" + QString::number(swapB + 1));
		gPar->SetFromOneParameter(
			gParFormulaSpecificFields.at(i) + "_" + QString::number(swapA + 1), formulaB);
		gPar->SetFromOneParameter(
			gParFormulaSpecificFields.at(i) + "_" + QString::number(swapB + 1), formulaA);
	}

	// swap formula specific fields in gParFractal by swapping whole container
	SynchronizeInterfaceWindow(fractalWidgets[swapA], &gParFractal->at(swapB), qInterface::read);
	SynchronizeInterfaceWindow(fractalWidgets[swapB], &gParFractal->at(swapA), qInterface::read);

	// write swapped changes to ui
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

void cDockFractal::slotChangedCheckBoxBooleanOperators(bool state)
{
	if (state) ui->checkBox_hybrid_fractal_enable->setChecked(false);
	gApplication->processEvents();

	for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
	{
		QFrame *frame =
			ui->tabWidget_fractals->findChild<QFrame *>("frame_iterations_formula_" + QString::number(i));
		if (i > 1)
		{
			frame->setEnabled(state);
			ui->tabWidget_fractals->findChild<QScrollArea *>("scrollArea_fractal_" + QString::number(i))
				->setEnabled(state);
		}
		ui->tabWidget_fractals->findChild<QGroupBox *>("groupBox_formula_transform_"
																									 + QString::number(i))->setVisible(state);

		QComboBox *comboBox =
			ui->tabWidget_fractals->findChild<QComboBox *>("comboBox_formula_" + QString::number(i));
		fractal::enumCPixelAddition cPixelAddition =
			fractalList[comboBox->itemData(comboBox->currentIndex()).toInt()].cpixelAddition;

		if (cPixelAddition == fractal::cpixelAlreadyHas)
			ui->tabWidget_fractals->findChild<QGroupBox *>("groupBox_c_constant_addition_"
																										 + QString::number(i))->setVisible(false);
		else
			ui->tabWidget_fractals->findChild<QGroupBox *>("groupBox_c_constant_addition_"
																										 + QString::number(i))->setVisible(state);

		ui->tabWidget_fractals->findChild<QGroupBox *>(
															"groupBox_material_fractal_" + QString::number(i))->setVisible(state);
	}

	gMainInterface->mainWindow->ui->widgetDockRenderingEngine->ComboDeltaDEFunctionSetEnabled(!state);
	ui->groupBox_material_fractal->setVisible(!state);
}

void cDockFractal::slotChangedCheckBoxHybridFractal(int state)
{
	if (state) ui->groupCheck_boolean_operators->setChecked(false);
	gApplication->processEvents();

	for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
	{
		QFrame *frame =
			ui->tabWidget_fractals->findChild<QFrame *>("frame_iterations_formula_" + QString::number(i));

		frame->findChild<QLabel *>(QString("label_formula_iterations_") + QString::number(i))
			->setVisible(state);
		frame->findChild<MySpinBox *>(QString("spinboxInt_formula_iterations_") + QString::number(i))
			->setVisible(state);
		frame->findChild<QSlider *>(QString("sliderInt_formula_iterations_") + QString::number(i))
			->setVisible(state);

		frame->findChild<QLabel *>(QString("label_formula_weight_") + QString::number(i))
			->setVisible(state);
		frame->findChild<MyDoubleSpinBox *>(QString("spinbox_formula_weight_") + QString::number(i))
			->setVisible(state);
		frame->findChild<QSlider *>(QString("slider_formula_weight_") + QString::number(i))
			->setVisible(state);

		frame->findChild<QLabel *>(QString("label_formula_start_iteration_") + QString::number(i))
			->setVisible(state);
		frame->findChild<QLabel *>(QString("label_formula_stop_iteration_") + QString::number(i))
			->setVisible(state);
		frame->findChild<MySpinBox *>(QString("spinboxInt_formula_start_iteration_")
																	+ QString::number(i))->setVisible(state);
		frame->findChild<MySpinBox *>(
						 QString("spinboxInt_formula_stop_iteration_") + QString::number(i))->setVisible(state);

		frame->findChild<MyCheckBox *>(QString("checkBox_check_for_bailout_") + QString::number(i))
			->setVisible(state);
		// frame->findChild<MyCheckBox*>(QString("checkBox_dont_add_c_constant_") +
		// QString::number(i))->setVisible(state);

		if (i > 1)
		{
			frame->setEnabled(state);
			ui->tabWidget_fractals->findChild<QScrollArea *>("scrollArea_fractal_" + QString::number(i))
				->setEnabled(state);
		}
	}

	gMainInterface->mainWindow->ui->widgetDockRenderingEngine->ComboDeltaDEFunctionSetEnabled(
		state
		|| gMainInterface->mainWindow->ui->widgetDockRenderingEngine->ComboDeltaDEMethodCurrentIndex()
				 == (int)fractal::forceDeltaDEMethod);

	ui->label_fractals_remark_hybrid->setVisible(!state);
	ui->label_repeat_from->setEnabled(state);
	ui->sliderInt_repeat_from->setEnabled(state);
	ui->spinboxInt_repeat_from->setEnabled(state);

	// ui->comboBox_delta_DE_method->setEnabled(!state);
}

void cDockFractal::slotChangedCheckBoxJuliaMode(bool state)
{
	ui->label_fractals_remark_julia->setVisible(state);
}

void cDockFractal::slotToggledFractalEnable(int fractalIndex, bool enabled)
{
	ui->tabWidget_fractals->widget(fractalIndex - 1)->setEnabled(enabled);
}

void cDockFractal::slotGroupCheckJuliaModeToggled(bool state)
{
	if (state)
	{
		gMainInterface->DisablePeriodicRefresh();
	}
}

void cDockFractal::slotChangedJuliaPoint()
{
	if (ui->groupCheck_julia_mode->isChecked() && gMainInterface->interfaceReady)
	{
		cParameterContainer params;
		InitParams(&params);
		InitMaterialParams(1, &params);
		SynchronizeInterfaceWindow(ui->groupCheck_julia_mode, &params, qInterface::read);
		params.SetContainerName("juliaPreview");

		double cameraDistance = params.Get<double>("julia_preview_distance");
		CVector3 target(0.0, 0.0, 0.0);
		CVector3 direction = gPar->Get<CVector3>("camera") - gPar->Get<CVector3>("target");
		direction.Normalize();
		CVector3 camera = target + direction * cameraDistance;

		params.Set("camera", camera);
		params.Set("target", target);
		params.Set("julia_mode", true);
		params.Set("ambient_occlusion_enabled", true);
		params.Copy("camera_top", gPar);
		params.Copy("formula_1", gPar);
		params.Copy("fractal_constant_factor", gPar);

		ui->previewwidget_julia->AssignParameters(params, *gParFractal);
	}
}

void cDockFractal::slotPressedButtonGetJuliaConstant()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickGetJuliaConstant);
	int index = gMainInterface->mainWindow->ui->comboBox_mouse_click_function->findData(item);
	gMainInterface->mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void cDockFractal::slotPressedButtonNewPrimitive()
{
	QString buttonName = this->sender()->objectName();
	QString primitiveName = buttonName.mid(buttonName.lastIndexOf('_') + 1);
	gMainInterface->NewPrimitive(primitiveName);
}

void cDockFractal::slotPressedButtonResetFormula()
{
	QString comboName = this->sender()->objectName();
	int fractalNumber = comboName.right(1).toInt() - 1;
	gMainInterface->ResetFormula(fractalNumber);
}
