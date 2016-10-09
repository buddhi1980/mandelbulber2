/*
 * dock_rendering_engine.h
 *
 *  Created on: 9 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_DOCK_RENDERING_ENGINE_H_
#define MANDELBULBER2_QT_DOCK_RENDERING_ENGINE_H_

#include <QWidget>

#include "../src/automated_widgets.hpp"
#include "../src/parameters.hpp"

namespace Ui
{
class cDockRenderingEngine;
}

class cDockRenderingEngine : public QWidget
{
	Q_OBJECT
public:
	explicit cDockRenderingEngine(QWidget *parent = NULL);
	~cDockRenderingEngine();

	void SynchronizeInterfaceDistanceEstimation(cParameterContainer *par);
	void ComboDeltaDEFunctionSetEnabled(bool enabled);
	int ComboDeltaDEMethodCurrentIndex(void);
	void CheckboxConnectDetailLevelSetCheckState(Qt::CheckState state);
	void UpdateLabelWrongDEPercentage(const QString &text);
	void UpdateLabelUsedDistanceEstimation(const QString &text);

private slots:
	void slotChangedCheckBoxUseDefaultBailout(int state);
	void slotPressedButtonOptimizeForLQ();
	void slotPressedButtonOptimizeForMQ();
	void slotPressedButtonOptimizeForHQ();
	void slotDetailLevelChanged();
	void slotChangedComboDistanceEstimationMethod(int index);

	// NetRender
	void slotNetRenderClientConnect();
	void slotNetRenderClientDisconnect();
	void slotNetRenderServerStart();
	void slotNetRenderServerStop();
	void slotNetRenderClientServerChange(int index);
	void slotCheckBoxDisableNetRender(bool on);
	void slotNetRenderClientListUpdate();
	void slotNetRenderClientListUpdate(int i);
	void slotNetRenderClientListUpdate(int i, int j);
	void slotNetRenderStatusServerUpdate();
	void slotNetRenderStatusClientUpdate();

private:
	void ConnectSignals();

	Ui::cDockRenderingEngine *ui;

	cAutomatedWidgets *automatedWidgets;

signals:
	void stateChangedConnectDetailLevel(int);
};

#endif /* MANDELBULBER2_QT_DOCK_RENDERING_ENGINE_H_ */
