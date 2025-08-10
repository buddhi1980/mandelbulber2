/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * MyLineEdit class - promoted MyLineEdit widget with context menu
 */

#ifndef MANDELBULBER2_QT_MY_LINE_EDIT_H_
#define MANDELBULBER2_QT_MY_LINE_EDIT_H_

#include <QLineEdit>

#include "common_my_widget_wrapper.h"

class cFrameSliderPopup;
enum class enumSliderPrecision;

class MyLineEdit : public QLineEdit, public CommonMyWidgetWrapper
{
	Q_OBJECT

public:
	MyLineEdit(QWidget *parent = nullptr);
	~MyLineEdit() override;
	void UpdateScriptAppearance(bool hasScript) override;

private:
	QAction *actionResetVectorToDefault;
	QAction *actionCopyVectorToClipboard;
	QAction *actionPasteVectorFromClipboard;

	QString GetDefault();
	QString defaultText;
	void CopyToClipboard() const;
	void PasteFromClipboard();
	void ResetVectorToDefault();

	void resetToDefault() override;
	QString getDefaultAsString() override;
	QString getFullParameterName() override;

	cFrameSliderPopup *slider;
	double valueBeforeSliderDrag;
	enumSliderPrecision precision;

protected:
	void contextMenuEvent(QContextMenuEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	void focusInEvent(QFocusEvent *event) override;
	void focusOutEvent(QFocusEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;

private slots:
	void slotCopyVector() const;
	void slotPasteVector();
	void slotResetVector();
	void slotSliderTimerUpdateValue();
	void slotResetToDefault();
	void slotZeroValue();
	void slotDoubleValue();
	void slotHalfValue();
	void slotRoundValue();
	void slotInvertSign();
	void slotSliderPressed();
	void slotSliderReleased();
	void slotSliderMoved(int sliderPosition);
	void slotChangedPrecision(enumSliderPrecision _precision);
};

#endif /* MANDELBULBER2_QT_MY_LINE_EDIT_H_ */
