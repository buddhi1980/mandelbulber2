/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
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
 *                                          §= "=C=4 §"eM "=B:m\4"]#F,§~
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
 * Authors: Sebastian Jennen
 *
 * CommonMyWidgetWrapper - contains context menu and common widget functionality
 *
 * this class contains all common functionality needed for custom widgets, including:
 * contextmenu:   creation and result handling
 * assignment:    assignment of parameterContainer / parameterName
 * toolTipText:   puts additional info to tooltip
 * In order to use this class inherit it and override the pure virtual methods
 * See also example mycheckbox.*
 */

#include "common_my_widget_wrapper.h"
#include "../src/animation_flight.hpp"
#include "../src/animation_keyframes.hpp"

void CommonMyWidgetWrapper::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;
	actionResetToDefault = menu->addAction(QCoreApplication::translate("CommonMyWidgetWrapper","Reset to default"));
	actionAddToFlightAnimation = menu->addAction(QCoreApplication::translate("CommonMyWidgetWrapper", "Add to flight animation"));
	actionAddToKeyframeAnimation = menu->addAction(QCoreApplication::translate("CommonMyWidgetWrapper", "Add to keyframe animation"));
	QAction *selectedItem = menu->exec(event->globalPos());
	if (selectedItem)
	{
		if (selectedItem == actionResetToDefault)
		{
			if (parameterContainer)
			{
				resetToDefault();
			}
			else
			{
				qCritical() << " MyBasicContextMenuElement::contextMenuEvent(QContextMenuEvent *event): "
											 "parameter container not assigned. Object:"
										<< widget->objectName();
			}
		}
		else if (selectedItem == actionAddToFlightAnimation)
		{
			if (parameterContainer)
			{
				gAnimFrames->AddAnimatedParameter(
					getFullParameterName(), parameterContainer->GetAsOneParameter(getFullParameterName()));
				gFlightAnimation->RefreshTable();
			}
		}
		else if (selectedItem == actionAddToKeyframeAnimation)
		{
			if (parameterContainer)
			{
				gKeyframes->AddAnimatedParameter(
					getFullParameterName(), parameterContainer->GetAsOneParameter(getFullParameterName()));
				gKeyframeAnimation->RefreshTable();
			}
		}
	}
	delete menu;
}

void CommonMyWidgetWrapper::setToolTipText()
{
	QString toolTipText = widget->toolTip();
	toolTipText += "\n" + QCoreApplication::translate("CommonMyWidgetWrapper", "Parameter name: %1").arg(parameterName) + "<br>";
	toolTipText += QCoreApplication::translate("CommonMyWidgetWrapper", "Default value: %1").arg(getDefaultAsString());
	widget->setToolTip(toolTipText);
}
