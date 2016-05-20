/*
 * material_list_view.cpp
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#include "material_manager_view.h"
#include <QtWidgets/QtWidgets>

cMaterialManagerView::cMaterialManagerView(QWidget *parent) : QGroupBox(parent)
{
	// TODO Auto-generated constructor stub
	itemView = new cMaterialItemView(parent);
	QVBoxLayout *layout = new QVBoxLayout();
	this->setLayout(layout);
	layout->addWidget(itemView);
}

cMaterialManagerView::~cMaterialManagerView()
{
	// TODO Auto-generated destructor stub
}

void cMaterialManagerView::SetModel(cMaterialItemModel *model)
{
	itemView->setModel(model);
}
