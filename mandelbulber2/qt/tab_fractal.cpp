/*
 * tab_fractal.cpp
 *
 *  Created on: 23 paź 2016
 *      Author: krzysztof
 */

#include "tab_fractal.h"

#include "../src/error_message.hpp"
#include "../src/fractal_list.hpp"
#include "../src/interface.hpp"
#include "../src/my_ui_loader.h"
#include "ui_tab_fractal.h"

cTabFractal::cTabFractal(QWidget *parent) : QWidget(parent), ui(new Ui::cTabFractal)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	// automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();

	fractalWidget = NULL;
	tabIndex = 0;
}

cTabFractal::~cTabFractal()
{
	if (fractalWidget) delete fractalWidget;
	delete ui;
}

void cTabFractal::InitWidgetNames()
{
	QList<QWidget *> widgetList = findChildren<QWidget *>();

	for (int i = 0; i < widgetList.size(); i++)
	{
		QWidget *widget = widgetList[i];
		QString oldName = widget->objectName();
		if (oldName.size() > 0)
		{
			QString lastTwoLetters = oldName.right(2);
			QString newName;
			if (lastTwoLetters == "_x" || lastTwoLetters == "_y" || lastTwoLetters == "_z"
					|| lastTwoLetters == "_w")
			{
				newName =
					oldName.left(oldName.size() - 2) + "_" + QString::number(tabIndex + 1) + lastTwoLetters;
			}
			else
			{
				newName = oldName + "_" + QString::number(tabIndex + 1);
			}
			widget->setObjectName(newName);
		}
	}
}

void cTabFractal::Init(bool firstTab, int _tabIndex)
{
	tabIndex = _tabIndex;

	InitWidgetNames();

	ui->comboBox_formula->clear();
	ui->comboBox_formula->setIconSize(QSize(32, 32));
	ui->comboBox_formula->setFixedHeight(32);

	for (int f = 0; f < fractalList.size(); f++)
	{
		ui->comboBox_formula->addItem(
			QIcon(fractalList[f].getIconName()), fractalList[f].nameInComboBox, f);
	}

	// set headings and separator of formulas and transforms
	QFont fontHeading;
	fontHeading.setBold(true);
	ui->comboBox_formula->insertItem(0, QObject::tr("Formulas"));
	ui->comboBox_formula->setItemData(0, fontHeading, Qt::FontRole);
	ui->comboBox_formula->setItemData(0, Qt::AlignCenter, Qt::TextAlignmentRole);
	qobject_cast<QStandardItemModel *>(ui->comboBox_formula->model())->item(0)->setEnabled(false);
	int indexBeforeTransforms = ui->comboBox_formula->findText("Transform - Addition Constant");
	ui->comboBox_formula->insertItem(indexBeforeTransforms, QObject::tr("Transforms"));
	ui->comboBox_formula->setItemData(indexBeforeTransforms, fontHeading, Qt::FontRole);
	ui->comboBox_formula->setItemData(indexBeforeTransforms, Qt::AlignCenter, Qt::TextAlignmentRole);
	qobject_cast<QStandardItemModel *>(ui->comboBox_formula->model())
		->item(indexBeforeTransforms)
		->setEnabled(false);
	ui->comboBox_formula->insertSeparator(indexBeforeTransforms);

	connect(ui->comboBox_formula, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangedComboFractal(int)));
	connect(
		ui->pushButton_reset_formula, SIGNAL(clicked()), this, SLOT(slotPressedButtonResetFormula()));

	FrameIterationFormulaSetWidgetsVisibility(false);

	ui->checkBox_dont_add_c_constant->setText(QObject::tr("Don't add global C constant"));

	if (!firstTab)
	{
		ui->frame_iterations_formula->setEnabled(false);
	}

	ui->groupBox_formula_transform->setVisible(false);
	ui->groupBox_c_constant_addition->setVisible(false);
	ui->groupBox_material_fractal->setVisible(false);
}

void cTabFractal::slotChangedComboFractal(int indexInComboBox)
{
	QString comboName = this->sender()->objectName();
	int index = qobject_cast<QComboBox *>(this->sender())->itemData(indexInComboBox).toInt();

	QString fullFormulaName = fractalList[index].nameInComboBox;
	if (fractalList[index].internalID > 0)
	{
		QString formulaName = fractalList[index].internalName;
		QString uiFilename =
			systemData.sharedDir + "qt_data" + QDir::separator() + "fractal_" + formulaName + ".ui";

		if (fractalWidget) delete fractalWidget;
		fractalWidget = NULL;

		MyUiLoader loader;
		QFile uiFile(uiFilename);

		if (uiFile.exists())
		{
			uiFile.open(QFile::ReadOnly);
			fractalWidget = loader.load(&uiFile);
			QVBoxLayout *layout = ui->verticalLayout_fractal;
			layout->addWidget(fractalWidget);
			uiFile.close();
			fractalWidget->show();
			automatedWidgets->ConnectSignalsForSlidersInWindow(fractalWidget);
			SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);

			switch (fractalList[index].cpixelAddition)
			{
				case fractal::cpixelEnabledByDefault:
					ui->checkBox_dont_add_c_constant->setText(QObject::tr("Don't add global C constant"));
					ui->checkBox_dont_add_c_constant->setEnabled(true);
					break;

				case fractal::cpixelDisabledByDefault:
				{
					ui->checkBox_dont_add_c_constant->setText(QObject::tr("Add global C constant"));
					ui->checkBox_dont_add_c_constant->setEnabled(true);
					break;
				}

				case fractal::cpixelAlreadyHas:
				{
					ui->checkBox_dont_add_c_constant->setText(QObject::tr("Don't add global C constant"));
					ui->checkBox_dont_add_c_constant->setEnabled(false);
					break;
				}
			};

			fractal::enumCPixelAddition cPixelAddition = fractalList[index].cpixelAddition;
			bool booleanState =
				gMainInterface->mainWindow->ui->widgetDockFractal->AreBooleanFractalsEnabled();

			if (cPixelAddition == fractal::cpixelAlreadyHas)
				CConstantAdditionSetVisible(false);
			else
				CConstantAdditionSetVisible(booleanState);

			if (fractalList[index].internalID == fractal::kaleidoscopicIFS)
			{
				QWidget *pushButton_preset_dodecahedron =
					fractalWidget->findChild<QWidget *>("pushButton_preset_dodecahedron");
				QApplication::connect(pushButton_preset_dodecahedron, SIGNAL(clicked()), this,
					SLOT(slotPressedButtonIFSDefaultsDodecahedron()));
				QWidget *pushButton_preset_icosahedron =
					fractalWidget->findChild<QWidget *>("pushButton_preset_icosahedron");
				QApplication::connect(pushButton_preset_icosahedron, SIGNAL(clicked()), this,
					SLOT(slotPressedButtonIFSDefaultsIcosahedron()));
				QWidget *pushButton_preset_octahedron =
					fractalWidget->findChild<QWidget *>("pushButton_preset_octahedron");
				QApplication::connect(pushButton_preset_octahedron, SIGNAL(clicked()), this,
					SLOT(slotPressedButtonIFSDefaultsOctahedron()));
				QWidget *pushButton_preset_menger_sponge =
					fractalWidget->findChild<QWidget *>("pushButton_preset_menger_sponge");
				QApplication::connect(pushButton_preset_menger_sponge, SIGNAL(clicked()), this,
					SLOT(slotPressedButtonIFSDefaultsMengerSponge()));
				QWidget *pushButton_preset_reset =
					fractalWidget->findChild<QWidget *>("pushButton_preset_reset");
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
		if (fractalWidget) delete fractalWidget;
		fractalWidget = NULL;
	}

	gMainInterface->mainWindow->ui->widgetDockFractal->SetTabText(
		tabIndex, QString("#%1: %2").arg(tabIndex + 1).arg(fullFormulaName));
}

void cTabFractal::FormulaTransformSetVisible(bool visible)
{
	ui->groupBox_formula_transform->setVisible(visible);
}

int cTabFractal::GetCurrentFractalIndexOnList()
{
	return ui->comboBox_formula->itemData(ui->comboBox_formula->currentIndex()).toInt();
}

void cTabFractal::CConstantAdditionSetVisible(bool visible)
{
	ui->groupBox_c_constant_addition->setVisible(visible);
}

void cTabFractal::SynchronizeInterface(cParameterContainer *par, qInterface::enumReadWrite mode)
{
	WriteLog("cTabFractal::SynchronizeInterface: frame_iterations_formula", 3);
	SynchronizeInterfaceWindow(ui->frame_iterations_formula, par, mode);

	WriteLog("cTabFractal::SynchronizeInterface: groupBox_formula_transform", 3);
	SynchronizeInterfaceWindow(ui->groupBox_formula_transform, par, mode);

	WriteLog("cTabFractal::SynchronizeInterface: groupBox_c_constant_addition", 3);
	SynchronizeInterfaceWindow(ui->groupBox_c_constant_addition, par, mode);

	WriteLog("cTabFractal::SynchronizeInterface: groupBox_material_fractal", 3);
	SynchronizeInterfaceWindow(ui->groupBox_material_fractal, par, mode);
}

void cTabFractal::FrameIterationFormulaSetWidgetsVisibility(bool visible)
{
	ui->label_formula_iterations->setVisible(visible);
	ui->spinboxInt_formula_iterations->setVisible(visible);
	ui->sliderInt_formula_iterations->setVisible(visible);
	ui->label_formula_weight->setVisible(visible);
	ui->spinbox_formula_weight->setVisible(visible);
	ui->slider_formula_weight->setVisible(visible);
	ui->label_formula_start_iteration->setVisible(visible);
	ui->label_formula_stop_iteration->setVisible(visible);
	ui->spinboxInt_formula_start_iteration->setVisible(visible);
	ui->spinboxInt_formula_stop_iteration->setVisible(visible);
	ui->checkBox_check_for_bailout->setVisible(visible);
}

void cTabFractal::ConnectSignals()
{
}

void cTabFractal::MaterialSetVisible(bool visible)
{
	ui->groupBox_material_fractal->setVisible(visible);
}

void cTabFractal::FrameIterationFormulaSetEnabled(bool enabled)
{
	ui->frame_iterations_formula->setEnabled(enabled);
}

void cTabFractal::SynchronizeFractal(cParameterContainer *fractal, qInterface::enumReadWrite mode)
{
	SynchronizeInterfaceWindow(fractalWidget, fractal, mode);
}

void cTabFractal::slotPressedButtonIFSDefaultsDodecahedron()
{
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsDodecahedron(&gParFractal->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsIcosahedron()
{
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsIcosahedron(&gParFractal->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsOctahedron()
{
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsOctahedron(&gParFractal->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsMengerSponge()
{
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsMengerSponge(&gParFractal->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsReset()
{
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsReset(&gParFractal->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonResetFormula()
{
	gMainInterface->ResetFormula(tabIndex);
}
