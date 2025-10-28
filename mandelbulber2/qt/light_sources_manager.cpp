/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2021-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "light_sources_manager.h"

#include <QInputDialog>

#include "ui_light_sources_manager.h"

#include "light_editor.h"
#include "my_tab_bar_with_checkbox.h"

#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/light.h"
#include "src/lights.hpp"
#include "src/render_window.hpp"
#include "src/rendered_image_widget.hpp"
#include "src/settings.hpp"
#include "src/synchronize_interface.hpp"

cLightSourcesManager::cLightSourcesManager(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cLightSourcesManager)
{
	ui->setupUi(this);

	connect(ui->pushButton_newLight, &QPushButton::clicked, this,
		&cLightSourcesManager::slotButtonAddLight);
	connect(ui->pushButton_deleteLight, &QPushButton::clicked, this,
		&cLightSourcesManager::slotButtonDeleteLight);
	connect(ui->pushButton_duplicateLight, &QPushButton::clicked, this,
		&cLightSourcesManager::slotButtonDuplicateLight);
	connect(ui->pushButton_placeLight, &QPushButton::clicked, this,
		&cLightSourcesManager::slotButtonPlaceLight);

	connect(ui->checkBox_show_wireframe_lights, &MyCheckBox::stateChanged, this,
		&cLightSourcesManager::slorChangedWireframeVisibikity);

	connect(ui->tabWidget_lightSources, &MyTabWidgetWithCheckboxes::currentChanged, this,
		&cLightSourcesManager::slotChangedCurrentTab);

	connect(ui->tabWidget_lightSources, &MyTabWidgetWithCheckboxes::contextMenuRequested, this,
		&cLightSourcesManager::slotContextMenu);

	autoRefreshTimer = new QTimer(this);
	autoRefreshTimer->setSingleShot(true);
	connect(autoRefreshTimer, &QTimer::timeout, this, &cLightSourcesManager::slotPeriodicRefresh);
}

void cLightSourcesManager::Init()
{
	QList<int> listOfFoundLights = cLights::GetListOfLights(params);
	if (listOfFoundLights.isEmpty())
	{
		AddLight(true, -1);
	}
	else
	{
		Regenerate();
	}
	autoRefreshTimer->start(int(params->Get<double>("auto_refresh_period") * 1000.0));
}

cLightSourcesManager::~cLightSourcesManager()
{
	delete ui;
}

void cLightSourcesManager::AddLight(bool init, int indexInParameters)
{
	cLightEditor *newEditor = new cLightEditor;
	int newTabIndex = ui->tabWidget_lightSources->count() + 1;

	if (init)
	{
		if (lightIndexOnTab.isEmpty())
		{
			indexInParameters = 1;
		}
		else
		{
			indexInParameters = lightIndexOnTab.last() + 1;
		}
		InitLightParams(indexInParameters, params);
	}

	if (!init && indexInParameters < 0)
	{
		qCritical() << "cLightSourcesManager::AddLight(): Wrong light index!";
		return;
	}

	// QString name = QString("# %1").arg(indexInParameters);
	QString name = params->Get<QString>(cLight::Name("name", indexInParameters));
	ui->tabWidget_lightSources->addTab(newEditor, name);

	auto *tabBar = qobject_cast<MyTabBarWithCheckBox *>(ui->tabWidget_lightSources->tabBar());
	tabBar->AddCheckBox(newTabIndex - 1, QString("checkBox_light%1_enabled").arg(indexInParameters));

	if (params->Get<bool>("ui_colorize"))
		cInterface::ColorizeGroupBoxes(ui->tabWidget_lightSources->widget(newTabIndex - 1),
			params->Get<int>("ui_colorize_random_seed") + newTabIndex);

	cInterface::AdjustLayoutSpacing(
		ui->tabWidget_lightSources->widget(newTabIndex - 1), params->Get<int>("ui_layout_spacing"));

	if (init)
	{
		params->Set(cLight::Name("is_defined", indexInParameters), true);
		params->Set(cLight::Name("enabled", indexInParameters), true);
	}

	newEditor->AssignLight(params, indexInParameters);

	lightIndexOnTab.append(indexInParameters);

	SynchronizeInterfaceWindow(ui->tabWidget_lightSources, params, qInterface::write);
}

void cLightSourcesManager::Regenerate()
{
	int count = ui->tabWidget_lightSources->count();
	for (int i = count - 1; i >= 0; i--)
	{
		delete ui->tabWidget_lightSources->widget(i);

		// checkbox is not deleted with tab, so need to be deleted separately
		qobject_cast<MyTabBarWithCheckBox *>(ui->tabWidget_lightSources->tabBar())->RemoveCheckBox(i);

		ui->tabWidget_lightSources->removeTab(i);
	}

	lightIndexOnTab.clear();

	// finding lights in whole parameter set
	QList<int> listOfFoundLights = cLights::GetListOfLights(params);

	// add lights in sorted order
	std::sort(listOfFoundLights.begin(), listOfFoundLights.end());
	for (int index : listOfFoundLights)
	{
		AddLight(false, index);
	}

	if (mouseFunctionComboWidget) cInterface::ComboMouseClickUpdate(mouseFunctionComboWidget, params);
}

void cLightSourcesManager::slotButtonAddLight()
{
	AddLight(true, -1);
	cInterface::ComboMouseClickUpdate(mouseFunctionComboWidget, params);
	ui->tabWidget_lightSources->setCurrentIndex(ui->tabWidget_lightSources->count() - 1);
	renderedImageWidget->SetCurrentLightIndex(ui->tabWidget_lightSources->count());
}

void cLightSourcesManager::slotButtonDuplicateLight()
{
	SynchronizeInterfaceWindow(ui->tabWidget_lightSources, params, qInterface::read);

	int currentTabIndex = ui->tabWidget_lightSources->currentIndex();
	int currentLightIndex = lightIndexOnTab.at(currentTabIndex);

	AddLight(true, -1);

	int newTabIndex = ui->tabWidget_lightSources->count() - 1;
	int newLightIndex = lightIndexOnTab.at(newTabIndex);

	for (const QString &parameterName : cLight::paramsList)
	{
		if (parameterName != "name")
		{
			QString fullParameterNameSource =
				QString("light%1_%2").arg(currentLightIndex).arg(parameterName);
			QString fullParameterNameDest = QString("light%1_%2").arg(newLightIndex).arg(parameterName);

			cOneParameter sourcePar = params->GetAsOneParameter(fullParameterNameSource);
			cMultiVal sourceVar = sourcePar.GetMultiVal(valueActual);

			cOneParameter destPar = params->GetAsOneParameter(fullParameterNameDest);
			destPar.SetMultiVal(sourceVar, valueActual);
			params->SetFromOneParameter(fullParameterNameDest, destPar);
		}
	}

	SynchronizeInterfaceWindow(ui->tabWidget_lightSources, params, qInterface::write);

	cInterface::ComboMouseClickUpdate(mouseFunctionComboWidget, params);
	ui->tabWidget_lightSources->setCurrentIndex(ui->tabWidget_lightSources->count() - 1);
}

void cLightSourcesManager::slotButtonDeleteLight()
{
	SynchronizeInterfaceWindow(ui->tabWidget_lightSources, params, qInterface::read);

	int currentTabIndex = ui->tabWidget_lightSources->currentIndex();
	if (currentTabIndex >= 0)
	{
		int currentLightIndex = lightIndexOnTab.at(currentTabIndex);

		for (QString parameterName : cLight::paramsList)
		{
			QString fullParameterName = QString("light%1_%2").arg(currentLightIndex).arg(parameterName);

			params->DeleteParameter(fullParameterName);
		}

		Regenerate();
	}
}

void cLightSourcesManager::slotPeriodicRefresh()
{
	if (ui->checkBox_show_wireframe_lights->isChecked() && !visibleRegion().isEmpty())
	{
		SynchronizeInterfaceWindow(ui->tabWidget_lightSources, params, qInterface::read);
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

void cLightSourcesManager::slorChangedWireframeVisibikity(int enabled)
{
	renderedImageWidget->SetLightsVisibility(enabled);
	renderedImageWidget->update();
}

void cLightSourcesManager::slotButtonPlaceLight()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlaceLight));

	int currentTabIndex = ui->tabWidget_lightSources->currentIndex();
	int currentLightIndex = lightIndexOnTab.at(currentTabIndex);
	item.append(currentLightIndex); // light number

	int index = mouseFunctionComboWidget->findData(item);
	mouseFunctionComboWidget->setCurrentIndex(index);
	renderedImageWidget->setClickMode(item);

	const double distance =
		cInterface::GetDistanceForPoint(params->Get<CVector3>("camera"), params, fractalParams);

	double optimalDistance = distance * 0.1;
	params->Set("aux_light_manual_placement_dist", optimalDistance);
	emit signalChangeLightPlacementDistance(optimalDistance);
	// ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(distance * 0.1));
}

void cLightSourcesManager::slotChangedCurrentTab(int index)
{
	if (index >= 0 && lightIndexOnTab.size() > 0)
	{
		int currentLightIndex = lightIndexOnTab.at(index);
		renderedImageWidget->SetCurrentLightIndex(currentLightIndex);
		renderedImageWidget->update();

		RenderedImage::enumClickMode actualClickMode =
			RenderedImage::enumClickMode(mouseFunctionComboWidget->currentData().toList().at(0).toInt());

		if (actualClickMode == RenderedImage::clickPlaceLight)
		{
			QList<QVariant> item;
			item.append(int(RenderedImage::clickPlaceLight));
			if (index < lightIndexOnTab.size())
			{
				item.append(currentLightIndex);
				int comboIndex = mouseFunctionComboWidget->findData(item);
				mouseFunctionComboWidget->setCurrentIndex(comboIndex);
				renderedImageWidget->setClickMode(item);
			}
		}
	}
}

void cLightSourcesManager::slotContextMenu(const QPoint &screenPoint, int tabIndex)
{
	QMenu menu(this);
	int currentLightIndex = lightIndexOnTab.at(tabIndex);

	QAction *actionRename = menu.addAction(tr("Rename light #%1").arg(currentLightIndex));

	QAction *selectedAction = menu.exec(screenPoint);

	QString oldName = params->Get<QString>(cLight::Name("name", currentLightIndex));

	if (selectedAction)
	{
		if (selectedAction == actionRename)
		{
			bool ok = false;
			QString newName = QInputDialog::getText(this, tr("Renaming light #%1").arg(currentLightIndex),
				tr("Enter name of the light #%1").arg(currentLightIndex), QLineEdit::Normal, oldName, &ok);

			if (ok)
			{
				params->Set(cLight::Name("name", currentLightIndex), newName);
				ui->tabWidget_lightSources->setTabText(tabIndex, newName);
			}
		}
	}
}
