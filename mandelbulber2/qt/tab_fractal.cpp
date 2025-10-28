/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-22 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "custom_formula_editor.h"
#include "dock_fractal.h"
#include "navigator_window.h"

#include "src/automated_widgets.hpp"
#include "src/error_message.hpp"
#include "src/fractal_container.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/my_ui_loader.h"
#include "src/render_window.hpp"
#include "src/write_log.hpp"

#include "formula/definition/all_fractal_list.hpp"

cTabFractal::cTabFractal(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cTabFractal)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	// automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();

	tabIndex = 0;
}

cTabFractal::~cTabFractal()
{
	delete ui;
}

void cTabFractal::InitWidgetNames() const
{
	QList<QWidget *> widgetList = findChildren<QWidget *>();

	for (auto widget : widgetList)
	{
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
	insertHeader << QPair<int, QString>(
		fractal::aexion, QObject::tr("*** Formulas with analytic DE ***"));
	insertHeader << QPair<int, QString>(fractal::aexion, QObject::tr("Logarithmic DE"));
	insertHeader << QPair<int, QString>(fractal::aboxMod1, QObject::tr("Linear DE"));
	insertHeader << QPair<int, QString>(fractal::josKleinian, QObject::tr("JosLeys-Kleinian DE"));
	insertHeader << QPair<int, QString>(fractal::pseudoKleinian, QObject::tr("Pseudo Kleinian DE"));
	insertHeader << QPair<int, QString>(
		fractal::dIFSAmazingIfs, QObject::tr("Custom DE - dIFS Formulas"));
	insertHeader << QPair<int, QString>(
		fractal::transfDIFSAmazingIfs, QObject::tr("Custom DE - dIFS Transforms"));
	insertHeader << QPair<int, QString>(
		fractal::foldCutCube, QObject::tr("Custom DE - non-dIFS formulas"));

	insertHeader << QPair<int, QString>(
		fractal::aexionOctopusMod, QObject::tr("*** Formulas with delta-DE ***"));
	insertHeader << QPair<int, QString>(
		fractal::aexionOctopusMod, QObject::tr("Logarithmic delta-DE"));
	insertHeader << QPair<int, QString>(fractal::boxFoldBulbPow2, QObject::tr("Linear delta-DE"));

	insertHeader << QPair<int, QString>(
		fractal::transfAbsAddConditional, QObject::tr("*** Transforms ***"));

	insertHeader << QPair<int, QString>(
		fractal::transfHybridColor, QObject::tr("*** Hybrid coloring ***"));
	insertHeader << QPair<int, QString>(
		fractal::testing, QObject::tr("*** Experimental Do Not Use***"));

	ui->comboBox_formula->populateItemsFromFractalList(
		newFractalList, insertHeader, params->Get<int>("ui_colorize_random_seed"));

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
	QString comboName = sender()->objectName();
	int index = qobject_cast<QComboBox *>(sender())->itemData(indexInComboBox).toInt();

	QString fullFormulaName = newFractalList[index]->getNameInComboBox();
	if (newFractalList[index]->getInternalId() > 0)
	{
		QString formulaName = newFractalList[index]->getInternalName();
		QString uiFilename = newFractalList[index]->getUiFilename();

		bool widgetLoaded = false;

		if (formulaName == "custom")
		{
			fractalWidget.reset(new cCustomFormulaEditor());
			dynamic_cast<cCustomFormulaEditor *>(fractalWidget.get())->AssignSlot(tabIndex);
			widgetLoaded = true;
		}
		else
		{
			MyUiLoader loader;
			QFile uiFile(uiFilename);

			if (uiFile.exists())
			{
				uiFile.open(QFile::ReadOnly);
				fractalWidget.reset(loader.load(&uiFile));

				uiFile.close();
				widgetLoaded = true;
			}
		}

		if (widgetLoaded)
		{
			QVBoxLayout *layout = ui->verticalLayout_fractal;
			layout->addWidget(fractalWidget.get());

			if (params->Get<bool>("ui_colorize"))
				cInterface::ColorizeGroupBoxes(
					fractalWidget.get(), params->Get<int>("ui_colorize_random_seed"));
			cInterface::AdjustLayoutSpacing(fractalWidget.get(), gPar->Get<int>("ui_layout_spacing"));

			fractalWidget->show();
			automatedWidgets->ConnectSignalsForSlidersInWindow(fractalWidget.get());
			SynchronizeInterfaceWindow(
				fractalWidget.get(), fractalParams->at(tabIndex), qInterface::write);

			switch (newFractalList[index]->getCpixelAddition())
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

				case fractal::cpixelUndefined:
				{
					ui->checkBox_dont_add_c_constant->setText(QObject::tr("Don't add global C constant"));
					ui->checkBox_dont_add_c_constant->setEnabled(false);
					break;
				}
			};

			fractal::enumCPixelAddition cPixelAddition = newFractalList[index]->getCpixelAddition();

			if (cPixelAddition == fractal::cpixelAlreadyHas)
				CConstantAdditionSetVisible(false);
			else
			{
				if (parentDockFractal)
				{
					bool booleanState = parentDockFractal->AreBooleanFractalsEnabled();
					CConstantAdditionSetVisible(booleanState);
				}
			}

			if (newFractalList[index]->getInternalId() == fractal::kaleidoscopicIfs)
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
		fractalWidget.reset();
	}

	if (parentDockFractal)
	{
		parentDockFractal->SetTabText(
			tabIndex, QString("#%1: %2").arg(tabIndex + 1).arg(fullFormulaName));
	}
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

void cTabFractal::CalculationParametersSetVisible(bool visible) const
{
	ui->groupBox_calculation_parameters->setVisible(visible);
}

void cTabFractal::SynchronizeInterface(
	std::shared_ptr<cParameterContainer> par, qInterface::enumReadWrite mode) const
{
	WriteLog("cTabFractal::SynchronizeInterface: frame_iterations_formula", 3);
	SynchronizeInterfaceWindow(ui->frame_iterations_formula, par, mode);

	WriteLog("cTabFractal::SynchronizeInterface: groupBox_formula_transform", 3);
	SynchronizeInterfaceWindow(ui->groupBox_formula_transform, par, mode);

	WriteLog("cTabFractal::SynchronizeInterface: groupBox_c_constant_addition", 3);
	SynchronizeInterfaceWindow(ui->groupBox_c_constant_addition, par, mode);

	WriteLog("cTabFractal::SynchronizeInterface: groupBox_material_fractal", 3);
	SynchronizeInterfaceWindow(ui->groupBox_material_fractal, par, mode);

	WriteLog("cTabFractal::SynchronizeInterface: groupBox_material_fractal", 3);
	SynchronizeInterfaceWindow(ui->groupBox_calculation_parameters, par, mode);
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
	connect(
		ui->pushButton_local_navi, &QPushButton::clicked, this, &cTabFractal::slotPressedButtonNavi);
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
	std::shared_ptr<cParameterContainer> fractal, qInterface::enumReadWrite mode) const
{
	if (fractalWidget)
	{
		SynchronizeInterfaceWindow(fractalWidget.get(), fractal, mode);
	}
}

void cTabFractal::slotPressedButtonIFSDefaultsDodecahedron() const
{
	SynchronizeInterfaceWindow(fractalWidget.get(), fractalParams->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsDodecahedron(fractalParams->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget.get(), fractalParams->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsIcosahedron() const
{
	SynchronizeInterfaceWindow(fractalWidget.get(), fractalParams->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsIcosahedron(fractalParams->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget.get(), fractalParams->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsOctahedron() const
{
	SynchronizeInterfaceWindow(fractalWidget.get(), fractalParams->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsOctahedron(fractalParams->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget.get(), fractalParams->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsMengerSponge() const
{
	SynchronizeInterfaceWindow(fractalWidget.get(), fractalParams->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsMengerSponge(fractalParams->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget.get(), fractalParams->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonIFSDefaultsReset() const
{
	SynchronizeInterfaceWindow(fractalWidget.get(), fractalParams->at(tabIndex), qInterface::read);
	gMainInterface->IFSDefaultsReset(fractalParams->at(tabIndex));
	SynchronizeInterfaceWindow(fractalWidget.get(), fractalParams->at(tabIndex), qInterface::write);
}

void cTabFractal::slotPressedButtonResetFormula() const
{
	gMainInterface->ResetFormula(tabIndex);
}

void cTabFractal::slotPressedButtonNavi()
{
	gMainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	cNavigatorWindow *navigator = new cNavigatorWindow();
	cTabFractal *leftWidget = new cTabFractal();
	navigator->AddLeftWidget(leftWidget);
	navigator->setAttribute(Qt::WA_DeleteOnClose);
	navigator->SetInitialParameters(params, fractalParams);
	leftWidget->Init(true, tabIndex);
	navigator->SynchronizeInterface(qInterface::write);
	navigator->SetMouseClickFunction(gMainInterface->GetMouseClickFunction());
	navigator->show();
	navigator->AllPrepared();
}
