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

cLightSourcesManager::cLightSourcesManager(QWidget *parent)
		: QWidget(parent), ui(new Ui::cLightSourcesManager)
{
	ui->setupUi(this);

	AddLight();
}

cLightSourcesManager::~cLightSourcesManager()
{
	delete ui;
}

void cLightSourcesManager::AddLight()
{
	cLightEditor *newEditor = new cLightEditor;
	int newIndex = ui->tabWidget_lightSources->count() + 1;
	QString name = QString("# %1").arg(newIndex);
	ui->tabWidget_lightSources->addTab(newEditor, name);

	auto *tabBar = qobject_cast<MyTabBarWithCheckBox *>(ui->tabWidget_lightSources->tabBar());
	tabBar->AddCheckBox(newIndex - 1, QString("checkBox_light%1_enabled").arg(newIndex));

	newEditor->AssignLight(gPar, 1);
}
