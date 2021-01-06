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

	AddLight();

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

void cLightSourcesManager::AddLight()
{
	cLightEditor *newEditor = new cLightEditor;
	int newIndex = ui->tabWidget_lightSources->count() + 1;

	InitLightParams(newIndex, gPar);

	QString name = QString("# %1").arg(newIndex);
	ui->tabWidget_lightSources->addTab(newEditor, name);

	auto *tabBar = qobject_cast<MyTabBarWithCheckBox *>(ui->tabWidget_lightSources->tabBar());
	tabBar->AddCheckBox(newIndex - 1, QString("checkBox_light%1_enabled").arg(newIndex));

	if (gPar->Get<bool>("ui_colorize"))
		cInterface::ColorizeGroupBoxes(ui->tabWidget_lightSources->widget(newIndex - 1),
			gPar->Get<int>("ui_colorize_random_seed") + newIndex);

	gPar->Set(cLight::Name("is_defined", newIndex), true);
	gPar->Set(cLight::Name("enabled", newIndex), true);

	newEditor->AssignLight(gPar, newIndex);

	SynchronizeInterfaceWindow(ui->tabWidget_lightSources, gPar, qInterface::write);
}

void cLightSourcesManager::slotButtonAddLight()
{
	AddLight();
}

void cLightSourcesManager::slotButtonDuplicateLight() {}

void cLightSourcesManager::slotButtonDeleteLight() {}
