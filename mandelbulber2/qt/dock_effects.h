/*
 * dock_effects.h
 *
 *  Created on: 8 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_DOCK_EFFECTS_H_
#define MANDELBULBER2_QT_DOCK_EFFECTS_H_

#include <QWidget>

#include "../src/automated_widgets.hpp"
#include "../src/parameters.hpp"

namespace Ui
{
class cDockEffects;
}

class cDockEffects : public QWidget
{
	Q_OBJECT
public:
	explicit cDockEffects(QWidget *parent = NULL);
	~cDockEffects();
	void ConnectSignals();

	void SynchronizeInterfaceBasicFogEnabled(cParameterContainer *par);
	void SynchronizeInterfaceDOFEnabled(cParameterContainer *par);
	void SynchronizeInterfaceLights(cParameterContainer *par);
	void SynchronizeInterfacerandomLights(cParameterContainer *par);

	double GetAuxLightManualPlacementDistance();
	void SetAuxLightManualPlacementDistance(double dist);

private slots:
	void slotPressedButtonAutoFog();
	void slotChangedCheckBoxDOFHDR(int state);
	void slotChangedComboAmbientOcclusionMode(int index);
	void slotEditedLineEditManualLightPlacementDistance(const QString &text);
	void slotSliderMovedEditManualLightPlacementDistance(int value);
	void slotPressedButtonSetDOFByMouse();
	void slotPressedButtonSetFogByMouse();
	void slotPressedButtonSetLight1ByMouse();
	void slotPressedButtonSetLight2ByMouse();
	void slotPressedButtonSetLight3ByMouse();
	void slotPressedButtonSetLight4ByMouse();
	void slotPressedButtonDOFUpdate();
	void slotPressedButtonPlaceRandomLightsByMouse();


private:
	Ui::cDockEffects *ui;

	cAutomatedWidgets *automatedWidgets;
};

#endif /* MANDELBULBER2_QT_DOCK_EFFECTS_H_ */
