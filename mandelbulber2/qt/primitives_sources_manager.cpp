/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2021 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#include "primitives_sources_manager.h"

#include "ui_primitives_manager.h"

//#include "primitive_editor.h"
#include "my_tab_bar_with_checkbox.h"

#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/primitives.h"
#include "src/render_window.hpp"
#include "src/rendered_image_widget.hpp"
#include "src/settings.hpp"
#include "src/synchronize_interface.hpp"

cPrimitivesManager::cPrimitivesManager(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cPrimitivesManager)
{
	ui->setupUi(this);

	// FIXME:	connect(ui->pushButton_newPrimitive, &QPushButton::clicked, this,
	//		&cPrimitivesManager::slotButtonAddPrimitive);
	connect(ui->pushButton_deletePrimitive, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonDeletePrimitive);
	connect(ui->pushButton_duplicatePrimitive, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonDuplicatePrimitive);
	connect(ui->pushButton_placePrimitive, &QPushButton::clicked, this,
		&cPrimitivesManager::slotButtonPlacePrimitive);

	connect(ui->checkBox_show_wireframe_primitives, &MyCheckBox::stateChanged, this,
		&cPrimitivesManager::slorChangedWireframeVisibikity);

	connect(ui->tabWidget_primitives, &MyTabWidgetWithCheckboxes::currentChanged, this,
		&cPrimitivesManager::slotChangedCurrentTab);

	autoRefreshTimer = new QTimer(this);
	autoRefreshTimer->setSingleShot(true);
	connect(autoRefreshTimer, &QTimer::timeout, this, &cPrimitivesManager::slotPeriodicRefresh);
}

void cPrimitivesManager::Init()
{
	// FIXME: init
	/*
	QList<int> listOfFoundPrimitives = cPrimitives::GetListOfPrimitives(params);
	if (listOfFoundPrimitives.isEmpty())
	{
		AddPrimitive(true, -1);
	}
	else
	{
		Regenerate();
	}
	autoRefreshTimer->start(int(params->Get<double>("auto_refresh_period") * 1000.0));
	*/
}

cPrimitivesManager::~cPrimitivesManager()
{
	delete ui;
}

void cPrimitivesManager::AddPrimitive(bool init, int indexInParameters)
{
	// FIXME: new primitive
	/*
	cPrimitiveEditor *newEditor = new cPrimitiveEditor;
	int newTabIndex = ui->tabWidget_primitiveSources->count() + 1;

	if (init)
	{
		if (primitiveIndexOnTab.isEmpty())
		{
			indexInParameters = 1;
		}
		else
		{
			indexInParameters = primitiveIndexOnTab.last() + 1;
		}
		InitPrimitiveParams(indexInParameters, params);
	}

	if (!init && indexInParameters < 0)
	{
		qCritical() << "cPrimitivesManager::AddPrimitive(): Wrong primitive index!";
		return;
	}

	QString name = QString("# %1").arg(indexInParameters);
	ui->tabWidget_primitiveSources->addTab(newEditor, name);

	auto *tabBar = qobject_cast<MyTabBarWithCheckBox *>(ui->tabWidget_primitiveSources->tabBar());
	tabBar->AddCheckBox(
		newTabIndex - 1, QString("checkBox_primitive%1_enabled").arg(indexInParameters));

	if (params->Get<bool>("ui_colorize"))
		cInterface::ColorizeGroupBoxes(ui->tabWidget_primitiveSources->widget(newTabIndex - 1),
			params->Get<int>("ui_colorize_random_seed") + newTabIndex);

	if (init)
	{
		params->Set(cPrimitive::Name("is_defined", indexInParameters), true);
		params->Set(cPrimitive::Name("enabled", indexInParameters), true);
	}

	newEditor->AssignPrimitive(params, indexInParameters);

	primitiveIndexOnTab.append(indexInParameters);

	SynchronizeInterfaceWindow(ui->tabWidget_primitiveSources, params, qInterface::write);
	*/
}

void cPrimitivesManager::Regenerate()
{
	// FIXME: regenerate primitives
	/*
	int count = ui->tabWidget_primitiveSources->count();
	for (int i = count - 1; i >= 0; i--)
	{
		delete ui->tabWidget_primitiveSources->widget(i);

		// checkbox is not deleted with tab, so need to be deleted separately
		qobject_cast<MyTabBarWithCheckBox *>(ui->tabWidget_primitiveSources->tabBar())
			->RemoveCheckBox(i);

		ui->tabWidget_primitiveSources->removeTab(i);
	}

	primitiveIndexOnTab.clear();

	// finding primitives in whole parameter set
	QList<int> listOfFoundPrimitives = cPrimitives::GetListOfPrimitives(params);

	// add primitives in sorted order
	std::sort(listOfFoundPrimitives.begin(), listOfFoundPrimitives.end());
	for (int index : listOfFoundPrimitives)
	{
		AddPrimitive(false, index);
	}
	cInterface::ComboMouseClickUpdate(mouseFunctionComboWidget, params);
	*/
}

void cPrimitivesManager::slotButtonAddPrimitive()
{
	AddPrimitive(true, -1);
	cInterface::ComboMouseClickUpdate(mouseFunctionComboWidget, params);
	ui->tabWidget_primitives->setCurrentIndex(ui->tabWidget_primitives->count() - 1);
}

void cPrimitivesManager::slotButtonDuplicatePrimitive()
{
	SynchronizeInterfaceWindow(ui->tabWidget_primitives, params, qInterface::read);

	int currentTabIndex = ui->tabWidget_primitives->currentIndex();
	int currentPrimitiveIndex = primitiveIndexOnTab.at(currentTabIndex);

	AddPrimitive(true, -1);

	int newTabIndex = ui->tabWidget_primitives->count() - 1;
	int newPrimitiveIndex = primitiveIndexOnTab.at(newTabIndex);

	// FIXME: duplicate primitive
	/*
	for (QString parameterName : cPrimitives::paramsList)
	{
		QString fullParameterNameSource =
			QString("primitive%1_%2").arg(currentPrimitiveIndex).arg(parameterName);
		QString fullParameterNameDest =
			QString("primitive%1_%2").arg(newPrimitiveIndex).arg(parameterName);

		cOneParameter sourcePar = params->GetAsOneParameter(fullParameterNameSource);
		cMultiVal sourceVar = sourcePar.GetMultiVal(valueActual);

		cOneParameter destPar = params->GetAsOneParameter(fullParameterNameDest);
		destPar.SetMultiVal(sourceVar, valueActual);
		params->SetFromOneParameter(fullParameterNameDest, destPar);
	}
	*/

	SynchronizeInterfaceWindow(ui->tabWidget_primitives, params, qInterface::write);

	cInterface::ComboMouseClickUpdate(mouseFunctionComboWidget, params);
	ui->tabWidget_primitives->setCurrentIndex(ui->tabWidget_primitives->count() - 1);
}

void cPrimitivesManager::slotButtonDeletePrimitive()
{
	int currentTabIndex = ui->tabWidget_primitives->currentIndex();
	if (currentTabIndex >= 0)
	{
		int currentPrimitiveIndex = primitiveIndexOnTab.at(currentTabIndex);

		// FIXME: delete primitive
		/*
		for (QString parameterName : cPrimitive::paramsList)
		{
			QString fullParameterName =
				QString("primitive%1_%2").arg(currentPrimitiveIndex).arg(parameterName);

			params->DeleteParameter(fullParameterName);
		}
		*/

		Regenerate();
	}
}

void cPrimitivesManager::slotPeriodicRefresh()
{
	if (ui->checkBox_show_wireframe_primitives->isChecked() && !visibleRegion().isEmpty())
	{
		SynchronizeInterfaceWindow(ui->tabWidget_primitives, params, qInterface::read);
		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(params, nullptr);
		QString newHash = tempSettings.GetHashCode();

		if (newHash != autoRefreshLastHash)
		{
			autoRefreshLastHash = newHash;
			renderedImageWidget->update();
		}
	}

	autoRefreshTimer->start(int(params->Get<double>("auto_refresh_period") * 1000.0));
}

void cPrimitivesManager::slorChangedWireframeVisibikity(int enabled)
{
	// FIXME: renderedImageWidget->SetPrimitivesVisibility(enabled);
	renderedImageWidget->update();
}

void cPrimitivesManager::slotButtonPlacePrimitive()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlacePrimitive));

	int currentTabIndex = ui->tabWidget_primitives->currentIndex();
	int currentPrimitiveIndex = primitiveIndexOnTab.at(currentTabIndex);
	item.append(currentPrimitiveIndex); // primitive number

	int index = mouseFunctionComboWidget->findData(item);
	mouseFunctionComboWidget->setCurrentIndex(index);
	renderedImageWidget->setClickMode(item);

	const double distance =
		cInterface::GetDistanceForPoint(params->Get<CVector3>("camera"), params, fractalParams);

	double optimalDistance = distance * 0.1;
	params->Set("aux_primitive_manual_placement_dist", optimalDistance);
	emit signalChangePrimitivePlacementDistance(optimalDistance);
	// ui->logedit_aux_primitive_manual_placement_dist->setText(QString("%L1").arg(distance * 0.1));
}

void cPrimitivesManager::slotChangedCurrentTab(int index)
{
	if (index >= 0 && primitiveIndexOnTab.size() > 0)
	{
		int currentPrimitiveIndex = primitiveIndexOnTab.at(index);
		// FIXME: renderedImageWidget->SetCurrentPrimitiveIndex(currentPrimitiveIndex);
		renderedImageWidget->update();

		RenderedImage::enumClickMode actualClickMode =
			RenderedImage::enumClickMode(mouseFunctionComboWidget->currentData().toList().at(0).toInt());

		if (actualClickMode == RenderedImage::clickPlacePrimitive)
		{
			QList<QVariant> item;
			item.append(int(RenderedImage::clickPlacePrimitive));
			if (index < primitiveIndexOnTab.size())
			{
				item.append(currentPrimitiveIndex);
				int comboIndex = mouseFunctionComboWidget->findData(item);
				mouseFunctionComboWidget->setCurrentIndex(comboIndex);
				renderedImageWidget->setClickMode(item);
			}
		}
	}
}
