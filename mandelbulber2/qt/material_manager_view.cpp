/*
 * material_list_view.cpp
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#include "material_manager_view.h"
#include "ui_material_manager_view.h"
#include <QtWidgets/QtWidgets>

cMaterialManagerView::cMaterialManagerView(QWidget *parent) : QWidget(parent), ui(new Ui::cMaterialManagerView)
{
	ui->setupUi(this);

	// TODO Auto-generated constructor stub
	itemView = new cMaterialItemView(this);
	ui->verticalLayout_material_view->addWidget(itemView);
}

cMaterialManagerView::~cMaterialManagerView()
{
	// TODO Auto-generated destructor stub
}

void cMaterialManagerView::SetModel(cMaterialItemModel *model)
{
	itemView->setModel(model);
}
