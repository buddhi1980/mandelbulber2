/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Widget which contains UI for image adjustments
 */

#ifndef MANDELBULBER2_QT_DOCK_IMAGE_ADJUSTMENTS_H_
#define MANDELBULBER2_QT_DOCK_IMAGE_ADJUSTMENTS_H_

#include <QWidget>

// forward declarations
class cAutomatedWidgets;
class cParameterContainer;
class QToolButton;

namespace Ui
{
class cDockImageAdjustments;
}

class cDockImageAdjustments : public QWidget
{
	Q_OBJECT
public:
	explicit cDockImageAdjustments(QWidget *parent = nullptr);
	~cDockImageAdjustments() override;

	enum enumImageProportion
	{
		proportionFree = 0,
		proportion1_1 = 1,
		proportion4_3 = 2,
		proportion3_2 = 3,
		proportion16_9 = 4,
		proportion16_10 = 5,
		proportion2_1 = 6
	};

private:
	const int numberOfProportions = 7;

public:
	void ApplyImageChangesSetEnabled(bool enable) const;
	bool IsConnectDetailLevelEnabled() const;

public slots:
	void slotCheckedDetailLevelLock(int state) const;

private slots:
	void slotChangedComboPerspectiveType(int index) const;
	static void slotPressedButtonImageApply();
	// resolution
	void slotChangedComboImageProportion(int index) const;
	void slotPressedResolutionPreset() const;
	void slotPressedImageSizeIncrease() const;
	void slotPressedImageSizeDecrease() const;
	void slotImageHeightChanged(int value) const;
	void slotChangeResolutionPreset();
	void slotOptimalDistancesBetweenEyes() const;

	// Quality presets
	static void slotQualityPresetVeryLow();
	static void slotQualityPresetLow();
	static void slotQualityPresetNormal();
	static void slotQualityPresetHigh();
	static void slotDisableAutoRefresh();

private:
	void ConnectSignals() const;
	void InitResolutionPresets();
	double ProportionEnumToRatio(enumImageProportion proportion) const;
	int PresetButtonIndex(const QString &name) const;

	Ui::cDockImageAdjustments *ui;

	cAutomatedWidgets *automatedWidgets;

	cParameterContainer *resolutionPresets;
	QList<QToolButton *> listOfPresetPushButtons;
};

#endif /* MANDELBULBER2_QT_DOCK_IMAGE_ADJUSTMENTS_H_ */
