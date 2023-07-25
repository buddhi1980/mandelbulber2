/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "my_widget_with_params.h"
#include "src/parameters.hpp"

// forward declarations
class cAutomatedWidgets;

namespace Ui
{
class cDockEffects;
}

class cDockEffects : public QWidget, public cMyWidgetWithParams
{
	Q_OBJECT
public:
	explicit cDockEffects(QWidget *parent = nullptr);
	~cDockEffects() override;

	double GetAuxLightManualPlacementDistance() const;

	void UpdateLabelAverageDOFSamples(const QString &avg);
	void UpdateLabelAverageDOFNoise(const QString &avg);
	void UpdateLabelMaskedPixels(const QString &avg);
	void UpdateLabelTilesDone(const QString &avg);
	void RegenerateLights();
	virtual void AssignParameterContainers(std::shared_ptr<cParameterContainer> _params,
		std::shared_ptr<cFractalContainer> _fractalParams) override;
	virtual void AssignSpecialWidgets(
		RenderedImage *_renderedImage, QComboBox *_mouseFunctionCombo) override;

public slots:
	void slotSetAuxLightManualPlacementDistance(double dist) const;
	void slotSynchronizeInterfaceBasicFog(std::shared_ptr<cParameterContainer> par) const;
	void slotSynchronizeInterfaceDOF(std::shared_ptr<cParameterContainer> par) const;
	void slotSynchronizeInterfaceLights(std::shared_ptr<cParameterContainer> par) const;
	void slotSynchronizeInterfaceRandomLights(std::shared_ptr<cParameterContainer> par) const;
	void slotPressedButtonNavi();
	void slotPressedButtonFogDistDouble();
	void slotPressedButtonFogDistHalf();
	void slotUpdatePrimitivesCombos();

private slots:
	void slotPressedButtonAutoFog();
	void slotChangedComboAmbientOcclusionMode(int index) const;
	void slotEditedLineEditManualLightPlacementDistance(const QString &text);
	void slotPressedButtonSetDOFByMouse();
	void slotPressedButtonSetFogByMouse();
	void slotPressedButtonUpdatePostEffects();
	void slotPressedButtonPlaceRandomLightsByMouse();
	void slotChangedPlaceLightBehindObjects(int state);
	void slotChangedEnableMCDOF(bool state);
	void slotChangedEnableGI(int state);
	void slotPressedButtonCloudsRandomize();
	void slotNewParametersFromNavi();
	void slotRandomLightColoringTypeChanged(int index);

private:
	void ConnectSignals() const;

signals:
	void signalRefreshPostEffects(void);

private:
	Ui::cDockEffects *ui;

	cAutomatedWidgets *automatedWidgets;
};

#endif /* MANDELBULBER2_QT_DOCK_EFFECTS_H_ */
