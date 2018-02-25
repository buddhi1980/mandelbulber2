/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cTabFractal - contains ui logic for the fractal and transform tabs
 * tab_fractal.ui is the layout ui for the outer ui.
 * The formula specific ui is loaded dynamically in slotChangedComboFractal()
 * and reads the corresponding ui from formula/ui/fractal_<FORMULA_NAME>.ui
 */

#include "tab_fractal.h"

#include "ui_tab_fractal.h"

#include "dock_fractal.h"

#include "src/automated_widgets.hpp"
#include "src/error_message.hpp"
#include "src/fractal_container.hpp"
#include "src/fractal_list.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/my_ui_loader.h"
#include "src/render_window.hpp"

cTabFractal::cTabFractal(QWidget *parent) : QWidget(parent), ui(new Ui::cTabFractal)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	// automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();

	fractalWidget = nullptr;
	tabIndex = 0;
}

cTabFractal::~cTabFractal()
{
	if (fractalWidget) delete fractalWidget;
	delete ui;
}

void cTabFractal::InitWidgetNames() const
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

	// set headings and separator of formulas and transforms
	QFont fontHeading;
	fontHeading.setBold(true);
	QList<QPair<int, QString> /* */> insertHeader;
	insertHeader << QPair<int, QString>(fractal::aboxMod1, QObject::tr("Formulas"));
	insertHeader << QPair<int, QString>(fractal::abox4d, QObject::tr("Formulas 4d"));
	insertHeader << QPair<int, QString>(fractal::transfAbsAddConstant, QObject::tr("Transforms"));
	insertHeader << QPair<int, QString>(fractal::transfAddConstant4d, QObject::tr("Transforms 4d"));
	insertHeader << QPair<int, QString>(fractal::transfHybridColor, QObject::tr("Hybrid coloring"));

	ui->comboBox_formula->populateItemsFromFractalList(fractalList, insertHeader);

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
		QString uiFilename = fractalList[index].getUiFilename();

		if (fractalWidget) delete fractalWidget;
		fractalWidget = nullptr;

		MyUiLoader loader;
		QFile uiFile(uiFilename);

		if (uiFile.exists())
		{
			uiFile.open(QFile::ReadOnly);
			fractalWidget = loader.load(&uiFile);
			QVBoxLayout *layout = ui->verticalLayout_fractal;
			layout->addWidget(fractalWidget);
			uiFile.close();

			if (gPar->Get<bool>("ui_colorize"))
				cInterface::ColorizeGroupBoxes(fractalWidget, gPar->Get<int>("ui_colorize_random_seed"));

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
				gMainInterface->mainWindow->GetWidgetDockFractal()->AreBooleanFractalsEnabled();

			if (cPixelAddition == fractal::cpixelAlreadyHas)
				CConstantAdditionSetVisible(false);
			else
				CConstantAdditionSetVisible(booleanState);

			if (fractalList[index].internalID == fractal::kaleidoscopicIfs)
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
		fractalWidget = nullptr;
	}

	gMainInterface->mainWindow->GetWidgetDockFractal()->SetTabText(
		tabIndex, QString("#%1: %2").arg(tabIndex + 1).arg(fullFormulaName));
}

void cTabFractal::FormulaTransformSetVisible(bool visible) const
{
	ui->groupBox_formula_transform->setVisible(visible);
}

int cTabFractal::GetCurrentFractalIndexOnList() const
{
	return ui->comboBox_formula->itemData(ui->comboBox_formula->currentIndex()).toInt();
}

void cTabFractal::CConstantAdditionSetVisible(bool visible) const
{
	ui->groupBox_c_constant_addition->setVisible(visible);
}

void cTabFractal::SynchronizeInterface(
	cParameterContainer *par, qInterface::enumReadWrite mode) const
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

void cTabFractal::FrameIterationFormulaSetWidgetsVisibility(bool visible) const
{
	ui->label_formula_iterations->setVisible(visible);
	ui->spinboxInt_formula_iterations->setVisible(visible);
	ui->label_formula_weight->setVisible(visible);
	ui->spinbox_formula_weight->setVisible(visible);
	ui->label_formula_start_iteration->setVisible(visible);
	ui->label_formula_stop_iteration->setVisible(visible);
	ui->spinboxInt_formula_start_iteration->setVisible(visible);
	ui->spinboxInt_formula_stop_iteration->setVisible(visible);
	ui->checkBox_check_for_bailout->setVisible(visible);
}

void cTabFractal::ConnectSignals()
{
}

void cTabFractal::MaterialSetVisible(bool visible) const
{
	ui->groupBox_material_fractal->setVisible(visible);
}

void cTabFractal::FrameIterationFormulaSetEnabled(bool enabled) const
{
	ui->frame_iterations_formula->setEnabled(enabled);
}

void cTabFractal::SynchronizeFractal(
	cParameterContainer *fractal, qInterface::enumReadWrite mode) const
{
	SynchronizeInterfaceWindow(fractalWidget, fractal, mode);
}

void cTabFractal::slotPressedButtonIFSDefaultsDodecahedron() const
{
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsDodecahedron(&gParFractal->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsIcosahedron() const
{
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsIcosahedron(&gParFractal->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsOctahedron() const
{
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsOctahedron(&gParFractal->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsMengerSponge() const
{
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsMengerSponge(&gParFractal->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsReset() const
{
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsReset(&gParFractal->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget, &gParFractal->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonResetFormula() const
{
	gMainInterface->ResetFormula(tabIndex);
}
