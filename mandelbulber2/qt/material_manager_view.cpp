/*
 * material_list_view.cpp
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#include "material_manager_view.h"
#include "ui_material_manager_view.h"
#include <QtWidgets/QtWidgets>
#include "../src/error_message.hpp"

cMaterialManagerView::cMaterialManagerView(QWidget *parent) : QWidget(parent), ui(new Ui::cMaterialManagerView)
{
	ui->setupUi(this);

	// TODO Auto-generated constructor stub
	itemView = new cMaterialItemView(this);
	ui->verticalLayout_material_view->addWidget(itemView);
	model = NULL;
	lastMaterialSelected = 1;

	connect(ui->pushButton_newMaterial, SIGNAL(clicked()), this, SLOT(slotAddMaterial()));
	connect(ui->pushButton_deleteMaterial, SIGNAL(clicked()), this, SLOT(slotDeleteMaterial()));
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

void cMaterialManagerView::slotDeleteMaterial()
{
	if (model->rowCount() > 1)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this,
																	QObject::tr("Delete material?"),
																	QObject::tr("Are you sure to delete selected material?"),
																	QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			QModelIndex index = itemView->currentIndex();
			itemView->setCurrentIndex(model->index(index.row()-1, 0));
			model->removeRows(index.row(), 1);
		}
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("You cannot delete last material!"),
															 cErrorMessage::errorMessage);
	}
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
