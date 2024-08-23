/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2021-22 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#ifndef MANDELBULBER2_QT_LIGHT_SOURCES_MANAGER_H_
#define MANDELBULBER2_QT_LIGHT_SOURCES_MANAGER_H_

#include <memory>

#include <QWidget>

#include "my_widget_with_params.h"

class cParameterContainer;

namespace Ui
{
class cLightSourcesManager;
}

class cLightSourcesManager : public QWidget, public cMyWidgetWithParams
{
	Q_OBJECT

public:
	explicit cLightSourcesManager(QWidget *parent = nullptr);
	~cLightSourcesManager() override;
	void Init();
	void Regenerate();

private:
	void AddLight(bool init, int indexInParameters);

	QList<int> lightIndexOnTab;
	Ui::cLightSourcesManager *ui;

	QTimer *autoRefreshTimer;
	QString autoRefreshLastHash;

private slots:
	void slotButtonAddLight();
	void slotButtonDuplicateLight();
	void slotButtonDeleteLight();
	void slotButtonPlaceLight();
	void slotPeriodicRefresh();
	void slorChangedWireframeVisibikity(int);
	void slotChangedCurrentTab(int);
	void slotContextMenu(const QPoint &screenPoint, int tabIndex);

signals:
	void signalChangeLightPlacementDistance(double);
};

#endif /* MANDELBULBER2_QT_LIGHT_SOURCES_MANAGER_H_ */
