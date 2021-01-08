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
#include "src/synchronize_interface.hpp"

cLightSourcesManager::cLightSourcesManager(QWidget *parent)
		: QWidget(parent), ui(new Ui::cLightSourcesManager)
{
	ui->setupUi(this);

	// AddLight(true, -1);

	connect(ui->pushButton_newLight, &QPushButton::clicked, this,
		&cLightSourcesManager::slotButtonAddLight);
	connect(ui->pushButton_deleteLight, &QPushButton::clicked, this,
		&cLightSourcesManager::slotButtonDeleteLight);
	connect(ui->pushButton_duplicateLight, &QPushButton::clicked, this,
		&cLightSourcesManager::slotButtonDuplicateLight);
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
		ui->tabWidget_lightSources->removeTab(i);
	}

	lightIndexOnTab.clear();

	QList<QString> list = gPar->GetListOfParameters();
	for (auto &parameterName : list)
	{
		if (parameterName.left(5) == "light")
		{
			int positionOfDash = parameterName.indexOf('_');
			int lightIndex = parameterName.midRef(5, positionOfDash - 5).toInt();
			if (lightIndexOnTab.indexOf(lightIndex) < 0)
			{
				AddLight(false, lightIndex);
			}
		}
	}
}

void cLightSourcesManager::slotButtonAddLight()
{
	AddLight(true, -1);
}

void cLightSourcesManager::slotButtonDuplicateLight() {}

void cLightSourcesManager::slotButtonDeleteLight() {}
