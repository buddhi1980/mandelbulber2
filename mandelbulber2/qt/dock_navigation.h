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
 * Widget which contains UI for navigation
 */

#ifndef MANDELBULBER2_QT_DOCK_NAVIGATION_H_
#define MANDELBULBER2_QT_DOCK_NAVIGATION_H_

#include <QWidget>

// forward declarations
class cAutomatedWidgets;

namespace Ui
{
class cDockNavigation;
}

class cDockNavigation : public QWidget
{
	Q_OBJECT
public:
	explicit cDockNavigation(QWidget *parent = nullptr);
	~cDockNavigation();

	void RenderButtonSetEnabled(bool enabled) const;
	void AutoRefreshSetChecked(bool state) const;
	bool AutoRefreshIsChecked() const;

private slots:
	static void slotStartRender();
	static void slotStopRender();

	void slotCameraMove() const;
	void slotCameraRotation() const;
	static void slotCameraOrTargetEdited();
	static void slotRotationEdited();
	static void slotCameraDistanceEdited();
	static void slotCameraDistanceSlider(int value);
	static void slotMovementStepModeChanged(int index);
	static void slotUndo();
	static void slotRedo();
	static void slotPressedButtonResetView();

private:
	void ConnectSignals() const;

	Ui::cDockNavigation *ui;

	cAutomatedWidgets *automatedWidgets;
};

#endif /* MANDELBULBER2_QT_DOCK_NAVIGATION_H_ */
