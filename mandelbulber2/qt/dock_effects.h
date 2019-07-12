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
 * Widget which contains UI for effects
 */

#ifndef MANDELBULBER2_QT_DOCK_EFFECTS_H_
#define MANDELBULBER2_QT_DOCK_EFFECTS_H_

#include <QWidget>

#include "src/parameters.hpp"

// forward declarations
class cAutomatedWidgets;

namespace Ui
{
class cDockEffects;
}

class cDockEffects : public QWidget
{
	Q_OBJECT
public:
	explicit cDockEffects(QWidget *parent = nullptr);
	~cDockEffects() override;

	void SynchronizeInterfaceBasicFogEnabled(cParameterContainer *par) const;
	void SynchronizeInterfaceDOFEnabled(cParameterContainer *par) const;
	void SynchronizeInterfaceLights(cParameterContainer *par) const;
	void SynchronizeInterfaceRandomLights(cParameterContainer *par) const;

	double GetAuxLightManualPlacementDistance() const;
	void SetAuxLightManualPlacementDistance(double dist) const;
	void UpdateLabelAverageDOFSamples(const QString &avg);
	void UpdateLabelAverageDOFNoise(const QString &avg);

private slots:
	static void slotPressedButtonAutoFog();
	void slotChangedComboAmbientOcclusionMode(int index) const;
	static void slotEditedLineEditManualLightPlacementDistance(const QString &text);
	static void slotPressedButtonSetDOFByMouse();
	static void slotPressedButtonSetFogByMouse();
	void slotPressedButtonSetLight1ByMouse() const;
	static void slotPressedButtonSetLight2ByMouse();
	static void slotPressedButtonSetLight3ByMouse();
	static void slotPressedButtonSetLight4ByMouse();
	static void slotPressedButtonUpdatePostEffects();
	static void slotPressedButtonPlaceRandomLightsByMouse();
	void slotChangedPlaceLightBehindObjects(int state);
	void slotChangedEnableMCDOF(bool state);
	void slotChangedEnableGI(int state);

private:
	void ConnectSignals() const;

	Ui::cDockEffects *ui;

	cAutomatedWidgets *automatedWidgets;
};

#endif /* MANDELBULBER2_QT_DOCK_EFFECTS_H_ */
