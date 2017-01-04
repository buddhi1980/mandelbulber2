/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cMaterialSelector class - promoted QWidget for selection of a material
 */

#include "material_selector.h"

#include "../qt/material_widget.h"
#include "../src/material.h"
#include "material_manager_view.h"
#include "../src/interface.hpp"
#include "../src/material_item_model.h"

cMaterialSelector::cMaterialSelector(QWidget *parent) : QWidget(parent), CommonMyWidgetWrapper(this)
{
	layout = new QHBoxLayout(this);
	layout->setSpacing(2);
	layout->setMargin(2);

	materialWidget = new cMaterialWidget(this);
	label = new QLabel(parent);
	label->setText("test");
	layout->addWidget(materialWidget);
	layout->addWidget(label);

	defaultValue = 0;
	actualValue = 0;

	setMinimumHeight(cMaterialWidget::previewHeight);

	connect(
		materialWidget, SIGNAL(clicked(Qt::MouseButton)), this, SLOT(slotClicked(Qt::MouseButton)));
}

cMaterialSelector::~cMaterialSelector()
{
}

void cMaterialSelector::resetToDefault()
{
	SetMaterialIndex(defaultValue);
	// emit valueChanged(defaultValue);
}

QString cMaterialSelector::getDefaultAsString()
{
	return QString::number(defaultValue);
}

QString cMaterialSelector::getFullParameterName()
{
	return parameterName;
}

void cMaterialSelector::contextMenuEvent(QContextMenuEvent *event)
{
	CommonMyWidgetWrapper::contextMenuEvent(event);
}

void cMaterialSelector::paintEvent(QPaintEvent *event)
{
	QFont f = font();
	f.setBold(actualValue != GetDefault());
	setFont(f);
	QWidget::paintEvent(event);
}

int cMaterialSelector::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
		gotDefault = true;
		setToolTipText();
	}
	return defaultValue;
}

void cMaterialSelector::SetMaterialIndex(int materialIndex)
{
	actualValue = materialIndex;
	if (actualValue > 0)
	{
		materialWidget->AssignMaterial(
			parameterContainer, actualValue, gMainInterface->scrollAreaMaterialEditor);
	}
	label->setText(QString("%1 [mat%2]")
									 .arg(parameterContainer->Get<QString>(cMaterial::Name("name", materialIndex)))
									 .arg(materialIndex));
}

void cMaterialSelector::slotClicked(Qt::MouseButton button)
{
	if (button == Qt::LeftButton)
	{
		cMaterialItemModel *model = gMainInterface->materialListModel;
		cMaterialManagerView *view = new cMaterialManagerView(this);
		view->SetModel(model);
		view->setWindowFlags(Qt::Dialog);
		view->setWindowModality(Qt::ApplicationModal);
		view->window()->setWindowTitle(tr("Material manager"));
		view->setAttribute(Qt::WA_DeleteOnClose);
		view->show();
		view->SetSelection(model->getModelIndexByMaterialId(actualValue));

		connect(view, SIGNAL(materialSelected(int)), this, SLOT(slotMaterialSelected(int)));
		connect(view, SIGNAL(materialEdited()), this, SLOT(slotMaterialEdited()));
	}
}

void cMaterialSelector::slotMaterialSelected(int matIndex)
{
	actualValue = matIndex;
	SetMaterialIndex(actualValue);
}

void cMaterialSelector::slotMaterialEdited() const
{
	SynchronizeInterfaceWindow(
		gMainInterface->scrollAreaMaterialEditor, parameterContainer, qInterface::write);
}
