/*
 * material_selector.cpp
 *
 *  Created on: 28 maj 2016
 *      Author: krzysztof
 */

#include "material_selector.h"
#include "../src/animation_flight.hpp"
#include "../src/animation_keyframes.hpp"
#include "../src/interface.hpp"

cMaterialSelector::cMaterialSelector(QWidget *parent) : QWidget(parent)
{
	// TODO Auto-generated constructor stub
	layout = new QHBoxLayout(this);
	layout->setSpacing(2);
	layout->setMargin(2);

	materialWidget = new cMaterialWidget(this);
	label = new QLabel(parent);
	label->setText("test");
	layout->addWidget(materialWidget);
	layout->addWidget(label);

	actionResetToDefault = NULL;
	actionAddToFlightAnimation = NULL;
	actionAddToKeyframeAnimation = NULL;
	parameterContainer = NULL;
	gotDefault = false;
	defaultValue = 0;
	actualValue = 0;

	connect(materialWidget, SIGNAL(clicked(Qt::MouseButton)), this, SLOT(slotClicked(Qt::MouseButton)));
}

cMaterialSelector::~cMaterialSelector()
{
	// TODO Auto-generated destructor stub
}

void cMaterialSelector::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;
	menu->addSeparator();
	actionResetToDefault = menu->addAction(tr("Reset to default"));
	actionAddToFlightAnimation = menu->addAction(tr("Add to flight animation"));
	actionAddToKeyframeAnimation = menu->addAction(tr("Add to keyframe animation"));
	QAction *selectedItem = menu->exec(event->globalPos());
	if (selectedItem)
	{
		if (selectedItem == actionResetToDefault)
		{
			if (parameterContainer)
			{
				SetMaterialIndex(defaultValue);
				//emit valueChanged(defaultValue);
			}
			else
			{
				qCritical() << "cMaterialSelector::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
			}
		}
		else if (selectedItem == actionAddToFlightAnimation)
		{
			if (parameterContainer)
			{
				gAnimFrames->AddAnimatedParameter(parameterName, parameterContainer->GetAsOneParameter(parameterName));
				gFlightAnimation->RefreshTable();
			}
		}
		else if (selectedItem == actionAddToKeyframeAnimation)
		{
			if (parameterContainer)
			{
				gKeyframes->AddAnimatedParameter(parameterName, parameterContainer->GetAsOneParameter(parameterName));
				gKeyframeAnimation->RefreshTable();
			}
		}
	}
	delete menu;
}

void cMaterialSelector::paintEvent(QPaintEvent *event)
{
	if (actualValue != GetDefault())
	{
		QFont f = font();
		f.setBold(true);
		setFont(f);
	}
	else
	{
		QFont f = font();
		f.setBold(false);
		setFont(f);
	}
	QWidget::paintEvent(event);
}

int cMaterialSelector::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
		gotDefault = true;

		QString toolTipText = toolTip();
		toolTipText += "\nParameter name: " + parameterName + "<br>";
		toolTipText += "Default: " + QString::number(defaultValue);
		setToolTip(toolTipText);
	}
	return defaultValue;
}

void cMaterialSelector::SetMaterialIndex(int materialIndex)
{
	actualValue = materialIndex;
	if(actualValue > 0)
	{
		materialWidget->AssignMaterial(parameterContainer, actualValue);
	}
	label->setText(QString("%1 [mat%2]").arg(parameterContainer->Get<QString>(cMaterial::Name("name", materialIndex))).arg(materialIndex));
}

void cMaterialSelector::slotClicked(Qt::MouseButton button)
{
	if(button == Qt::LeftButton)
	{
		cMaterialItemModel *model = gMainInterface->materialListModel;
		cMaterialManagerView *view = new cMaterialManagerView(this);
		view->SetModel(model);
		view->window()->setWindowTitle(tr("Material manager"));
		view->setWindowFlags(Qt::Dialog);
		view->setWindowModality(Qt::ApplicationModal);
		view->show();

		connect(view, SIGNAL(materialSelected(int)), this, SLOT(slotMaterialSelected(int)));
	}
}

void cMaterialSelector::slotMaterialSelected(int matIndex)
{
	actualValue = matIndex;
	SetMaterialIndex(actualValue);
}
