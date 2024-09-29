/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Widget which contains UI for fractals
 */

#include "dock_fractal.h"

#include "ui_dock_fractal.h"

#include "dock_rendering_engine.h"
#include "my_tab_bar.h"

#include "src/ao_modes.h"
#include "src/automated_widgets.hpp"
#include "src/error_message.hpp"
#include "src/fractal_enums.h"
#include "src/global_data.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/render_window.hpp"
#include "src/rendered_image_widget.hpp"
#include "src/write_log.hpp"

#include "formula/definition/all_fractal_list.hpp"
#include "navigator_window.h"

cDockFractal::cDockFractal(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cDockFractal)
{
	ui->setupUi(this);

	fractalTabs.resize(NUMBER_OF_FRACTALS);
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractalTabs[i] =
			ui->tabWidget_fractals->findChild<cTabFractal *>(QString("widgetTabFractal_%1").arg(i + 1));
	}

	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);

	ui->label_repeat_from->setEnabled(false);
	ui->spinboxInt_repeat_from->setEnabled(false);
	ui->label_fractals_remark_julia->setVisible(false);

	ui->previewwidget_julia->SetSize(256, 256, 2);

	ui->tabWidget_fractals->setUsesScrollButtons(true);
	ui->tabWidget_fractals->setDocumentMode(true);
	ui->tabWidget_fractals->setElideMode(Qt::ElideNone);

	ConnectSignals();
}

cDockFractal::~cDockFractal()
{
	delete ui;
}

bool cDockFractal::AreHybridFractalsEnabled() const
{
	return ui->checkBox_hybrid_fractal_enable->isChecked();
}

void cDockFractal::SynchronizeInterfaceFractals(std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> parFractal, qInterface::enumReadWrite mode) const
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
		fractalTabs[i]->SynchronizeFractal(parFractal->at(i), mode);
		fractalTabs[i]->SynchronizeInterface(par, mode);
	}
}

void cDockFractal::slotSynchronizeInterfaceJulia(std::shared_ptr<cParameterContainer> par) const
{
	SynchronizeInterfaceWindow(ui->groupCheck_julia_mode, par, qInterface::write);
}

void cDockFractal::slotSynchronizeInterfacePrimitives(
	std::shared_ptr<cParameterContainer> par) const
{
	SynchronizeInterfaceWindow(ui->scrollArea_primitives, par, qInterface::write);
}

void cDockFractal::slotEnableJuliaMode() const
{
	ui->groupCheck_julia_mode->setChecked(true);
}

QWidget *cDockFractal::GetContainerWithPrimitives() const
{
	return ui->scrollAreaWidgetContents_primitives;
}

QVBoxLayout *cDockFractal::GetLayoutWithPrimitives() const
{
	return ui->verticalLayout_primitives;
}

bool cDockFractal::AreBooleanFractalsEnabled() const
{
	return ui->groupCheck_boolean_operators->isChecked();
}

void cDockFractal::SetTabText(int tabIndex, QString text) const
{
	ui->tabWidget_fractals->setTabText(tabIndex, text);
}

void cDockFractal::ConnectSignals() const
{
	connect(
		ui->vect3_julia_c_x, SIGNAL(textChanged(const QString &)), this, SLOT(slotChangedJuliaPoint()));
	connect(
		ui->vect3_julia_c_y, SIGNAL(textChanged(const QString &)), this, SLOT(slotChangedJuliaPoint()));
	connect(
		ui->vect3_julia_c_z, SIGNAL(textChanged(const QString &)), this, SLOT(slotChangedJuliaPoint()));
	connect(ui->logedit_julia_preview_distance, SIGNAL(textChanged(const QString &)), this,
		SLOT(slotChangedJuliaPoint()));

	connect(ui->pushButton_get_julia_constant, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonGetJuliaConstant()));

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
	connect(ui->tabWidget_fractals->tabBar(), SIGNAL(currentChanged(int)), this,
		SLOT(slotChangedFractalTab(int)));
	connect(
		ui->pushButton_local_navi, &QPushButton::clicked, this, &cDockFractal::slotPressedButtonNavi);

	connect(
		ui->tabWidget_fractals, SIGNAL(swapTabs(int, int)), this, SLOT(slotFractalSwap(int, int)));

	connect(ui->widgetPrimitivesManager, &cPrimitivesManager::signalUpdatePrimitivesCombos, this,
		&cDockFractal::signalUpdatePrimitivesCombos);
}

// initialize ui for hybrid fractal components
void cDockFractal::InitializeFractalUi() const
{
	WriteLog("cInterface::InitializeFractalUi(QString &uiFileName) started", 2);
	// MyUiLoader loader;

	// QFile uiFile(uiFileName);

	// if (uiFile.exists())
	//{
	//		uiFile.open(QFile::ReadOnly);
	//		fractalWidgets[0] = loader.load(&uiFile);
	//		ui->verticalLayout_fractal_1->addWidget(fractalWidgets[0]);
	//		fractalWidgets[0]->show();

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		if (i == 0)
			ui->tabWidget_fractals->setTabText(i, QString("#1: Mandelbulb"));
		else
		{
			ui->tabWidget_fractals->setTabText(i, QString("#%1: None").arg(i + 1));
			ui->tabWidget_fractals
				->findChild<QScrollArea *>("scrollArea_fractal_" + QString::number(i + 1))
				->setEnabled(false);
		}
		fractalTabs[i]->AssignParameterContainers(params, fractalParams);
		fractalTabs[i]->Init(i == 0, i);
	}

	static_cast<MyTabBar *>(ui->tabWidget_fractals->tabBar())->setupMoveButtons();
	//}
	WriteLog("cInterface::InitializeFractalUi(QString &uiFileName) finished", 2);
}

void cDockFractal::slotFractalSwap(int swapA, int swapB) const
{
	// qDebug() << "swapping " << swapA << " with " << swapB;

	// read all data from ui
	gMainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);

	// swap formula specific fields in gPar
	QStringList gParFormulaSpecificFields({"formula", "formula_iterations", "formula_weight",
		"formula_start_iteration", "formula_stop_iteration", "julia_mode", "julia_c",
		"fractal_constant_factor", "formula_position", "formula_rotation", "formula_repeat",
		"formula_scale", "dont_add_c_constant", "check_for_bailout", "formula_maxiter",
		"smooth_de_combine_enable", "smooth_de_combine_distance"});

	for (int i = 0; i < gParFormulaSpecificFields.size(); i++)
	{
		// to keep original properties of parameters
		cOneParameter formulaA =
			params->GetAsOneParameter(gParFormulaSpecificFields.at(i) + "_" + QString::number(swapA + 1));
		cOneParameter formulaB =
			params->GetAsOneParameter(gParFormulaSpecificFields.at(i) + "_" + QString::number(swapB + 1));

		// get only actual values and swap between slots
		cMultiVal multiA = formulaB.GetMultiVal(parameterContainer::valueActual);
		cMultiVal multiB = formulaA.GetMultiVal(parameterContainer::valueActual);

		// write values to OneParameters
		formulaA.SetMultiVal(multiA, parameterContainer::valueActual);
		formulaB.SetMultiVal(multiB, parameterContainer::valueActual);

		// writing parameters to original containers
		params->SetFromOneParameter(
			gParFormulaSpecificFields.at(i) + "_" + QString::number(swapA + 1), formulaA);
		params->SetFromOneParameter(
			gParFormulaSpecificFields.at(i) + "_" + QString::number(swapB + 1), formulaB);
	}

	// swap formula specific fields in fractalParams by swapping whole container
	fractalTabs[swapA]->SynchronizeFractal(fractalParams->at(swapB), qInterface::read);
	fractalTabs[swapB]->SynchronizeFractal(fractalParams->at(swapA), qInterface::read);

	// write swapped changes to ui
	gMainInterface->SynchronizeInterface(params, fractalParams, qInterface::write);
}

void cDockFractal::slotChangedCheckBoxBooleanOperators(bool state) const
{
	if (state) ui->checkBox_hybrid_fractal_enable->setChecked(false);
	gApplication->processEvents();

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		if (i > 0)
		{
			fractalTabs[i]->FrameIterationFormulaSetEnabled(state);
			ui->tabWidget_fractals
				->findChild<QScrollArea *>("scrollArea_fractal_" + QString::number(i + 1))
				->setEnabled(state);
		}

		fractalTabs[i]->FormulaTransformSetVisible(state);

		const fractal::enumCPixelAddition cPixelAddition =
			newFractalList[fractalTabs[i]->GetCurrentFractalIndexOnList()]->getCpixelAddition();

		if (cPixelAddition == fractal::cpixelAlreadyHas)
			fractalTabs[i]->CConstantAdditionSetVisible(false);
		else
			fractalTabs[i]->CConstantAdditionSetVisible(state);

		fractalTabs[i]->MaterialSetVisible(state);
		fractalTabs[i]->CalculationParametersSetVisible(state);
	}

	gMainInterface->mainWindow->GetWidgetDockRenderingEngine()->ComboDeltaDEFunctionSetEnabled(
		!state);
	ui->groupBox_material_fractal->setVisible(!state);
}

void cDockFractal::slotChangedCheckBoxHybridFractal(int state) const
{
	if (state) ui->groupCheck_boolean_operators->setChecked(false);
	gApplication->processEvents();

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractalTabs[i]->FrameIterationFormulaSetWidgetsVisibility(state);
		if (i > 0)
		{
			fractalTabs[i]->FrameIterationFormulaSetEnabled(state);
			ui->tabWidget_fractals
				->findChild<QScrollArea *>("scrollArea_fractal_" + QString::number(i + 1))
				->setEnabled(state);
		}
	}

	ui->label_fractals_remark_hybrid->setVisible(!state);
	ui->label_repeat_from->setEnabled(state);
	ui->spinboxInt_repeat_from->setEnabled(state);
}

void cDockFractal::slotChangedCheckBoxJuliaMode(bool state) const
{
	ui->label_fractals_remark_julia->setVisible(state);
}

void cDockFractal::slotToggledFractalEnable(int fractalIndex, bool enabled) const
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

void cDockFractal::slotChangedJuliaPoint() const
{
	if (ui->groupBox_julia_preview && ui->groupCheck_julia_mode->isChecked()
			&& params->Get<bool>("julia_preview") && gInterfaceReadyForSynchronization)
	{
		std::shared_ptr<cParameterContainer> params(new cParameterContainer());
		InitParams(params);
		InitMaterialParams(1, params);
		SynchronizeInterfaceWindow(ui->groupCheck_julia_mode, params, qInterface::read);
		params->SetContainerName("juliaPreview");

		const double cameraDistance = params->Get<double>("julia_preview_distance");
		CVector3 target(0.0, 0.0, 0.0);
		CVector3 direction = params->Get<CVector3>("camera") - params->Get<CVector3>("target");
		direction.Normalize();
		CVector3 camera = target + direction * cameraDistance;

		params->Set("camera", camera);
		params->Set("target", target);
		params->Set("julia_mode", true);
		params->Set("ambient_occlusion_enabled", true);
		params->Set("ambient_occlusion_mode", int(params::AOModeFast));
		params->Copy("camera_top", params);
		for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
		{
			params->Copy(QString("formula_%1").arg(i), params);
			params->Copy(QString("formula_iterations_%1").arg(i), params);
			params->Copy(QString("fractal_enable_%1").arg(i), params);
			params->Copy(QString("formula_weight_%1").arg(i), params);
			params->Copy(QString("formula_start_iteration_%1").arg(i), params);
			params->Copy(QString("formula_stop_iteration_%1").arg(i), params);
			params->Copy(QString("dont_add_c_constant_%1").arg(i), params);
			params->Copy(QString("check_for_bailout_%1").arg(i), params);
		}
		params->Copy("hybrid_fractal_enable", params);
		params->Copy("fractal_constant_factor", params);
		params->Copy("opencl_mode", params);
		params->Copy("opencl_enabled", params);

		ui->previewwidget_julia->AssignParameters(params, fractalParams);
		ui->previewwidget_julia->update();
	}
}

void cDockFractal::slotPressedButtonGetJuliaConstant()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickGetJuliaConstant));
	int index = gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
	gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void cDockFractal::slotChangedFractalTab(int index)
{
	if (index > 0)
	{
		if (!ui->checkBox_hybrid_fractal_enable->isChecked()
				&& !ui->groupCheck_boolean_operators->isChecked())
		{
			QMessageBox message;
			;

			QPushButton *buttonHybrid =
				message.addButton(tr("Enable hybrid fractals"), QMessageBox::AcceptRole);
			QPushButton *buttonBoolean =
				message.addButton(tr("Enable boolean mode"), QMessageBox::AcceptRole);
			const QPushButton *buttonCancel = message.addButton(QMessageBox::Cancel);

			message.setText(tr(
				"You have selected next fractal formula.\nDo you want to enable hybrid fractals or boolean "
				"mode?"));
			message.setWindowTitle(tr("More fractals..."));
			message.setIcon(QMessageBox::Question);
			const int result = message.exec();
			Q_UNUSED(result);

			if (message.clickedButton() != buttonCancel)
			{
				if (message.clickedButton() == buttonHybrid)
				{
					ui->checkBox_hybrid_fractal_enable->setChecked(true);
				}
				else if (message.clickedButton() == buttonBoolean)
				{
					ui->groupCheck_boolean_operators->setChecked(true);
				}
			}
		}
	}
}

void cDockFractal::AssignParameterContainers(
	std::shared_ptr<cParameterContainer> _params, std::shared_ptr<cFractalContainer> _fractalParams)
{
	cMyWidgetWithParams::AssignParameterContainers(_params, _fractalParams);
	ui->widgetPrimitivesManager->AssignParameterContainers(_params, _fractalParams);
	ui->widgetPrimitivesManager->Init();
}

void cDockFractal::AssignSpecialWidgets(
	RenderedImage *_renderedImage, QComboBox *_mouseFunctionCombo)
{
	cMyWidgetWithParams::AssignSpecialWidgets(_renderedImage, _mouseFunctionCombo);
	ui->widgetPrimitivesManager->AssignSpecialWidgets(_renderedImage, _mouseFunctionCombo);
}

void cDockFractal::RegeneratePrimitives()
{
	ui->widgetPrimitivesManager->Regenerate();
}

void cDockFractal::slotPressedButtonNavi()
{
	gMainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	cNavigatorWindow *navigator = new cNavigatorWindow();
	cDockFractal *leftWidget = new cDockFractal();
	navigator->AddLeftWidget(leftWidget);
	navigator->setAttribute(Qt::WA_DeleteOnClose);
	navigator->SetInitialParameters(params, fractalParams);
	navigator->SetMouseClickFunction(gMainInterface->GetMouseClickFunction());
	leftWidget->HideSomeWidgetsForNavi();
	leftWidget->InitializeFractalUi();
	navigator->SynchronizeInterface(qInterface::write);

	connect(navigator, &cNavigatorWindow::signalChangesAccepted, this,
		&cDockFractal::slotNewParametersFromNavi);

	navigator->show();
	navigator->AllPrepared();
}

void cDockFractal::slotNewParametersFromNavi()
{
	// TODO: implement this
}

void cDockFractal::HideSomeWidgetsForNavi()
{
	delete ui->pushButton_get_julia_constant;
	ui->pushButton_get_julia_constant = nullptr;
	delete ui->groupBox_julia_preview;
	ui->groupBox_julia_preview = nullptr;
}
