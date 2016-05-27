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
	model = NULL;
	lastMaterialSelected = 1;

	connect(ui->pushButton_newMaterial, SIGNAL(clicked()), this, SLOT(slotAddMaterial()));
	connect(itemView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotItemSelected(const QModelIndex&)));
}

cMaterialManagerView::~cMaterialManagerView()
{
	// TODO Auto-generated destructor stub
}

void cMaterialManagerView::SetModel(cMaterialItemModel *_model)
{
	itemView->setModel(_model);
	model = _model;
}

void cMaterialManagerView::slotAddMaterial()
{
	int rows = model->rowCount();
	model->insertRows(rows, 1);
}

void cMaterialManagerView::slotItemSelected(const QModelIndex& index)
{
	qDebug() << "slotItemSelected" << index;
	int selection = model->materialIndex(index);
	if(selection != lastMaterialSelected)
	{
		emit materialSelected(selection);
	}
	lastMaterialSelected = selection;
}
