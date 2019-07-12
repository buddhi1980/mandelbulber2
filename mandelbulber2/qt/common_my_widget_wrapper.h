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

#ifndef MANDELBULBER2_QT_COMMON_MY_WIDGET_WRAPPER_H_
#define MANDELBULBER2_QT_COMMON_MY_WIDGET_WRAPPER_H_

#include <QMenu>
#include <QWidget>
#include <QtCore>
#include <QtGui>

// forward declarations
class cParameterContainer;

class CommonMyWidgetWrapper
{
public:
	CommonMyWidgetWrapper(QWidget *referenceWidget)
	{
		actionResetToDefault = nullptr;
		actionAddToFlightAnimation = nullptr;
		actionRemoveFromFlightAnimation = nullptr;
		actionAddToKeyframeAnimation = nullptr;
		actionRemoveFromKeyframeAnimation = nullptr;
		parameterContainer = nullptr;
		gotDefault = false;
		widget = referenceWidget;
	};

	void AssignParameterContainer(cParameterContainer *container)
	{
		if (!parameterContainer)
		{
			parameterContainer = container;
		}
	}
	void AssignParameterName(QString name) { parameterName = name; }

private:
	QAction *actionResetToDefault;
	QAction *actionAddToFlightAnimation;
	QAction *actionRemoveFromFlightAnimation;
	QAction *actionAddToKeyframeAnimation;
	QAction *actionRemoveFromKeyframeAnimation;

public:
	// these methods have to be implemented by widgets inheriting this class
	virtual void resetToDefault() = 0;

	// these methods have to be implemented by widgets inheriting this class
	virtual QString getDefaultAsString() = 0;
	virtual QString getFullParameterName() = 0;
	QString getParameterContainerName();

protected:
	~CommonMyWidgetWrapper() = default;
	cParameterContainer *parameterContainer;
	QString parameterName;
	bool gotDefault;
	QWidget *widget;

	void setToolTipText();
	static QString GetType(const QString &name);

	// contextMenuEvent doesn't overwrite method from child class, but this function cal be called
	const QAction *contextMenuEvent(QContextMenuEvent *event, QMenu *existingMenu = nullptr);
};

#endif /* MANDELBULBER2_QT_COMMON_MY_WIDGET_WRAPPER_H_ */
