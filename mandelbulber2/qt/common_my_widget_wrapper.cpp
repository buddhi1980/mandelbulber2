/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * CommonMyWidgetWrapper - contains context menu and common widget functionality
 *
 * this class contains all common functionality needed for custom widgets, including:
 * contextMenu:   creation and result handling
 * assignment:    assignment of parameterContainer / parameterName
 * toolTipText:   puts additional information to tooltip
 * In order to use this class inherit it and override the pure virtual methods
 * See also example my_check_box.*
 */

#include "common_my_widget_wrapper.h"

#include "src/animation_flight.hpp"
#include "src/animation_keyframes.hpp"

const QAction *CommonMyWidgetWrapper::contextMenuEvent(
	QContextMenuEvent *event, QMenu *existingMenu)
{
	QMenu *menu;
	if (existingMenu)
	{
		menu = existingMenu;
		menu->addSeparator();
	}
	else
	{
		menu = new QMenu;
	}

	QIcon iconAdd = QIcon::fromTheme("list-add", QIcon(":system/icons/list-add.svg"));
	QIcon iconDelete = QIcon::fromTheme("list-remove", QIcon(":system/icons/list-remove.svg"));
	QIcon iconReset = QIcon(":system/icons/edit-undo.png");

	actionResetToDefault =
		menu->addAction(QCoreApplication::translate("CommonMyWidgetWrapper", "Reset to default"));
	actionResetToDefault->setIcon(iconReset);

	const QAction *selectedItem = nullptr;

	if (parameterContainer)
	{
		enumParameterType parameterType = parameterContainer->GetParameterType(getFullParameterName());
		if (parameterType == parameterContainer::paramStandard)
		{
			if (gAnimFrames->IndexOnList(getFullParameterName(), parameterContainer->GetContainerName())
					== -1)
			{
				actionAddToFlightAnimation = menu->addAction(
					QCoreApplication::translate("CommonMyWidgetWrapper", "Add to flight animation"));
				actionAddToFlightAnimation->setIcon(iconAdd);
			}
			else
			{
				actionRemoveFromFlightAnimation = menu->addAction(
					QCoreApplication::translate("CommonMyWidgetWrapper", "Remove from flight animation"));
				actionRemoveFromFlightAnimation->setIcon(iconDelete);
			}

			if (gKeyframes->IndexOnList(getFullParameterName(), parameterContainer->GetContainerName())
					== -1)
			{
				actionAddToKeyframeAnimation = menu->addAction(
					QCoreApplication::translate("CommonMyWidgetWrapper", "Add to keyframe animation"));
				actionAddToKeyframeAnimation->setIcon(iconAdd);
			}
			else
			{
				actionRemoveFromKeyframeAnimation = menu->addAction(
					QCoreApplication::translate("CommonMyWidgetWrapper", "Remove from keyframe animation"));
				actionRemoveFromKeyframeAnimation->setIcon(iconDelete);
			}
		}

		selectedItem = menu->exec(event->globalPos());
		if (selectedItem)
		{
			if (selectedItem == actionResetToDefault)
			{
				resetToDefault();
			}
			else if (selectedItem == actionAddToFlightAnimation)
			{
				gAnimFrames->AddAnimatedParameter(
					getFullParameterName(), parameterContainer->GetAsOneParameter(getFullParameterName()));
				gFlightAnimation->RefreshTable();
			}
			else if (selectedItem == actionRemoveFromFlightAnimation)
			{
				gAnimFrames->RemoveAnimatedParameter(
					parameterContainer->GetContainerName() + "_" + getFullParameterName());
				gFlightAnimation->RefreshTable();
			}
			else if (selectedItem == actionAddToKeyframeAnimation)
			{
				gKeyframes->AddAnimatedParameter(
					getFullParameterName(), parameterContainer->GetAsOneParameter(getFullParameterName()));
				gKeyframeAnimation->RefreshTable();
			}
			else if (selectedItem == actionRemoveFromKeyframeAnimation)
			{
				gKeyframes->RemoveAnimatedParameter(
					parameterContainer->GetContainerName() + "_" + getFullParameterName());
				gKeyframeAnimation->RefreshTable();
			}
		}
	}
	else
	{
		qCritical() << " MyBasicContextMenuElement::contextMenuEvent(QContextMenuEvent *event): "
									 "parameter container not assigned. Object:"
								<< widget->objectName();
	}
	delete menu;
	return selectedItem;
}

void CommonMyWidgetWrapper::setToolTipText()
{
	QString toolTipText = widget->toolTip();
	toolTipText +=
		QString("<table style='margin-top: 5px;'>") + "<tr><td>"
		+ QCoreApplication::translate("CommonMyWidgetWrapper", "Parameter&nbsp;name:&nbsp;")
		+ "</td><td><b>" + getFullParameterName() + "</b></td></tr>" + "<tr><td>"
		+ QCoreApplication::translate("CommonMyWidgetWrapper", "Default&nbsp;value:&nbsp;")
		+ "</td><td><b>" + getDefaultAsString() + "</b></td></tr>" + "</table>";
	widget->setToolTip(toolTipText);
}

QString CommonMyWidgetWrapper::GetType(const QString &name)
{
	int firstDashPosition = name.indexOf("_");
	return name.left(firstDashPosition);
}

QString CommonMyWidgetWrapper::getParameterContainerName()
{
	return parameterContainer->GetContainerName();
}
