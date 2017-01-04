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
 * cAutomatedWidgets class provides connection between sliders
 * and edit fields. This class should be subclassed in every window or dialog
 * with edit fields and called there ConnectSignalsForSlidersInWindow();
 */

#ifndef MANDELBULBER2_SRC_AUTOMATED_WIDGETS_HPP_
#define MANDELBULBER2_SRC_AUTOMATED_WIDGETS_HPP_

#include <QObject>

class cAutomatedWidgets : public QObject
{
	Q_OBJECT
public:
	cAutomatedWidgets(QObject *parent);
	void ConnectSignalsForSlidersInWindow(QWidget *window) const;

private slots:
	void slotSliderMoved(int value) const;
	void slotDoubleSpinBoxChanged(double value) const;
	void slotIntSliderMoved(int value) const;
	void slotIntSpinBoxChanged(int value) const;
	void slotSlider3Moved(int value) const;
	void slotSlider4Moved(int value) const;
	void slotSpinBox3Changed(double value) const;
	void slotSpinBox4Changed(double value) const;
	void slotDial3Moved(int value) const;
	void slotDial4Moved(int value) const;
	void slotSpinBoxD3Changed(double value) const;
	void slotSpinBoxD4Changed(double value) const;
	void slotDialMoved(int value) const;
	void slotSpinBoxDChanged(double value) const;
	void slotLogSliderMoved(int value) const;
	void slotLogSliderVect3Moved(int value) const;
	void slotLogLineEditChanged(const QString &text) const;
	void slotLogVect3Changed(const QString &text) const;
};

#endif /* MANDELBULBER2_SRC_AUTOMATED_WIDGETS_HPP_ */
