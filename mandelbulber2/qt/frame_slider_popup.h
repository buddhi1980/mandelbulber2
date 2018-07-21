/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cFrameSliderPopup - popup slider for quick editing of numerical input fields
 * This widget popups when the associated input field is clicked and
 * contains a slider and different buttons to edit the field
 */

#ifndef MANDELBULBER2_QT_FRAME_SLIDER_POPUP_H_
#define MANDELBULBER2_QT_FRAME_SLIDER_POPUP_H_

#include <QFrame>

namespace Ui
{
class cFrameSliderPopup;
}

class cFrameSliderPopup : public QFrame
{
	Q_OBJECT
public:
	explicit cFrameSliderPopup(QWidget *parent = nullptr);
	~cFrameSliderPopup() override;

private:
	Ui::cFrameSliderPopup *ui;
	QTimer *sliderTimer;
	bool integerMode;
	bool dialMode;
	int integerMax;
	int integerMin;
	int dialScale;

public:
	int value() const;
	void SetIntegerMode(int min, int max, int val);
	void SetDialMode(int scale, double val);

private slots:
	void sliderReleased();
	void slotSliderTimerTrigger();
	void slotUpdateValue(int val);
	void slotUpdateValue(double val);
	void slotDialValueChanged(int val);

signals:
	void resetPressed();
	void zeroPressed();
	void halfPressed();
	void doublePressed();
	void intPressed();
	void upPressed();
	void downPressed();
	void leftPressed();
	void rightPressed();
	void timerTrigger();
	void valueChanged(int);
	void valueChanged(double);
};

#endif /* MANDELBULBER2_QT_FRAME_SLIDER_POPUP_H_ */
