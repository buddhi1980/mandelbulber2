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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cMaterialWidget - promoted cThumbnailWidget for displaying a material in a widget
 *
 * This class periodically checks for changes in assigned material and rerenders
 * the material on a "default" mandelbulb fractal.
 * See also other functionality inherited from cThumbnailWidget.
 */

#ifndef MANDELBULBER2_QT_MATERIAL_WIDGET_H_
#define MANDELBULBER2_QT_MATERIAL_WIDGET_H_

#include "thumbnail_widget.h"

#include "src/parameters.hpp"

class cMaterialWidget : public cThumbnailWidget
{
	Q_OBJECT
public:
	cMaterialWidget(QWidget *parent);
	cMaterialWidget(int _width, int _height, int _oversample, QWidget *parent);
	~cMaterialWidget() override;
	void AssignMaterial(
		cParameterContainer *_params, int materialIndex, QWidget *_materialEditorWidget = nullptr);
	void AssignMaterial(const QString &settings, int materialIndex);

private:
	cParameterContainer *paramsHandle;
	cParameterContainer paramsCopy;
	int actualMaterialIndex;
	QTimer *timerPeriodicRefresh;
	QTimer *timerPeriodicUpdateData;
	void Init();
	QWidget *materialEditorWidget;
	QSize sizeHint() const override;
	void InitializeData();

	bool initialized;
	bool dataAssigned;
	double timeUpdateData;
	double timeAssignData;
	bool materialNotDefined;

	void mousePressEvent(QMouseEvent *event) override;

private slots:

	void slotPeriodicRender();
	void slotPeriodicUpdateData();

public slots:
	void slotMaterialChanged();

public:
	int previewWidth;
	int previewHeight;

signals:
	void materialChanged(int matIndex);
	void clicked(Qt::MouseButton button);
};

#endif /* MANDELBULBER2_QT_MATERIAL_WIDGET_H_ */
