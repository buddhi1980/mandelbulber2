/*
 * light_sources_manader.cpp
 *
 *  Created on: 3 sty 2021
 *      Author: krzysztof
 */

#include "light_sources_manager.h"

#include "ui_light_sources_manager.h"

#include "light_editor.h"
#include "my_tab_bar_with_checkbox.h"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/light.h"
#include "src/lights.hpp"
#include "src/synchronize_interface.hpp"
#include "src/rendered_image_widget.hpp"
#include "src/render_window.hpp"
#include "src/settings.hpp"

cLightSourcesManager::cLightSourcesManager(QWidget *parent)
		: QWidget(parent), ui(new Ui::cLightSourcesManager)
{
	ui->setupUi(this);

	AddLight(true, -1);

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

	autoRefreshTimer = new QTimer(this);
	autoRefreshTimer->setSingleShot(true);
	connect(autoRefreshTimer, &QTimer::timeout, this, &cLightSourcesManager::slotPeriodicRefresh);
	autoRefreshTimer->start(int(gPar->Get<double>("auto_refresh_period") * 1000.0));
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
		InitLightParams(indexInParameters, gPar);
	}

	if (!init && indexInParameters < 0)
	{
		qCritical() << "cLightSourcesManager::AddLight(): Wrong light index!";
		return;
	}

	QString name = QString("# %1").arg(indexInParameters);
	ui->tabWidget_lightSources->addTab(newEditor, name);

	auto *tabBar = qobject_cast<MyTabBarWithCheckBox *>(ui->tabWidget_lightSources->tabBar());
	tabBar->AddCheckBox(newTabIndex - 1, QString("checkBox_light%1_enabled").arg(indexInParameters));

	if (gPar->Get<bool>("ui_colorize"))
		cInterface::ColorizeGroupBoxes(ui->tabWidget_lightSources->widget(newTabIndex - 1),
			gPar->Get<int>("ui_colorize_random_seed") + newTabIndex);

	if (init)
	{
		gPar->Set(cLight::Name("is_defined", indexInParameters), true);
		gPar->Set(cLight::Name("enabled", indexInParameters), true);
	}

	newEditor->AssignLight(gPar, indexInParameters);

	lightIndexOnTab.append(indexInParameters);

	SynchronizeInterfaceWindow(ui->tabWidget_lightSources, gPar, qInterface::write);
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
	QList<int> listOfFoundLights = cLights::GetListOfLights(gPar);

	// add lights in sorted order
	std::sort(listOfFoundLights.begin(), listOfFoundLights.end());
	for (int index : listOfFoundLights)
	{
		AddLight(false, index);
	}
	gMainInterface->ComboMouseClickUpdate();
}

void cLightSourcesManager::slotButtonAddLight()
{
	AddLight(true, -1);
	gMainInterface->ComboMouseClickUpdate();
	ui->tabWidget_lightSources->setCurrentIndex(ui->tabWidget_lightSources->count() - 1);
}

void cLightSourcesManager::slotButtonDuplicateLight()
{
	int currentTabIndex = ui->tabWidget_lightSources->currentIndex();
	int currentLightIndex = lightIndexOnTab.at(currentTabIndex);

	AddLight(true, -1);

	int newTabIndex = ui->tabWidget_lightSources->count() - 1;
	int newLightIndex = lightIndexOnTab.at(newTabIndex);

	for (QString parameterName : cLight::paramsList)
	{
		QString fullParameterNameSource =
			QString("light%1_%2").arg(currentLightIndex).arg(parameterName);
		QString fullParameterNameDest = QString("light%1_%2").arg(newLightIndex).arg(parameterName);

		cOneParameter sourcePar = gPar->GetAsOneParameter(fullParameterNameSource);
		cMultiVal sourceVar = sourcePar.GetMultiVal(valueActual);

		cOneParameter destPar = gPar->GetAsOneParameter(fullParameterNameDest);
		destPar.SetMultiVal(sourceVar, valueActual);
		gPar->SetFromOneParameter(fullParameterNameDest, destPar);
	}

	SynchronizeInterfaceWindow(ui->tabWidget_lightSources, gPar, qInterface::write);

	gMainInterface->ComboMouseClickUpdate();
	ui->tabWidget_lightSources->setCurrentIndex(ui->tabWidget_lightSources->count() - 1);
}

void cLightSourcesManager::slotButtonDeleteLight()
{
	int currentTabIndex = ui->tabWidget_lightSources->currentIndex();
	if (currentTabIndex >= 0)
	{
		int currentLightIndex = lightIndexOnTab.at(currentTabIndex);

		for (QString parameterName : cLight::paramsList)
		{
			QString fullParameterName = QString("light%1_%2").arg(currentLightIndex).arg(parameterName);

			gPar->DeleteParameter(fullParameterName);
		}

		Regenerate();
	}
}

void cLightSourcesManager::slotPeriodicRefresh()
{
	if (ui->checkBox_show_wireframe_lights->isChecked() && !visibleRegion().isEmpty())
	{
		SynchronizeInterfaceWindow(ui->tabWidget_lightSources, gPar, qInterface::read);
		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(gPar, nullptr);
		QString newHash = tempSettings.GetHashCode();

		if (newHash != autoRefreshLastHash)
		{
			autoRefreshLastHash = newHash;
			gMainInterface->UpdateMainImagePreview();
		}
	}

	autoRefreshTimer->start(int(gPar->Get<double>("auto_refresh_period") * 1000.0));
}

void cLightSourcesManager::slorChangedWireframeVisibikity(int enabled)
{
	gMainInterface->ChangeLightWireframeVisibility(static_cast<bool>(enabled));
}

void cLightSourcesManager::slotButtonPlaceLight()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlaceLight));

	int currentTabIndex = ui->tabWidget_lightSources->currentIndex();
	int currentLightIndex = lightIndexOnTab.at(currentTabIndex);
	item.append(currentLightIndex); // light number

	int index = gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
	gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
	const double distance = gMainInterface->GetDistanceForPoint(gPar->Get<CVector3>("camera"));
	double optimalDistance = distance * 0.1;
	gPar->Set("aux_light_manual_placement_dist", optimalDistance);
	emit signalChangeLightPlacementDistance(optimalDistance);
	// ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(distance * 0.1));
}

void cLightSourcesManager::slotChangedCurrentTab(int index)
{
	if (index >= 0)
	{
		gMainInterface->renderedImage->SetCurrentLightIndex(index + 1);
		gMainInterface->UpdateMainImagePreview();

		RenderedImage::enumClickMode actualClickMode =
			RenderedImage::enumClickMode(gMainInterface->mainWindow->GetComboBoxMouseClickFunction()
																		 ->currentData()
																		 .toList()
																		 .at(0)
																		 .toInt());

		if (actualClickMode == RenderedImage::clickPlaceLight)
		{
			QList<QVariant> item;
			item.append(int(RenderedImage::clickPlaceLight));
			if (index < lightIndexOnTab.size())
			{
				int currentLightIndex = lightIndexOnTab.at(index);
				item.append(currentLightIndex);
				int comboIndex =
					gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
				gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(comboIndex);
				gMainInterface->renderedImage->setClickMode(item);
			}
		}
	}
}
