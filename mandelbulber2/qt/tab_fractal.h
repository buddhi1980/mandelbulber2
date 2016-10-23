/*
 * tab_fractal.h
 *
 *  Created on: 23 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_TAB_FRACTAL_H_
#define MANDELBULBER2_QT_TAB_FRACTAL_H_

#include <QWidget>

#include "../src/automated_widgets.hpp"
#include "../src/parameters.hpp"
#include "../src/synchronize_interface.hpp"

namespace Ui
{
class cTabFractal;
}

class cTabFractal : public QWidget
{
	Q_OBJECT
public:
	explicit cTabFractal(QWidget *parent = NULL);
	~cTabFractal();

	void InitWidgetNames();
	void Init(bool firstTab, int _tabIndex);
	void FormulaTransformSetVisible(bool visible);
	void CConstantAdditionSetVisible(bool visible);
	void MaterialSetVisible(bool visible);
	void FrameIterationFormulaSetEnabled(bool enabled);
	void FrameIterationFormulaSetWidgetsVisibility(bool visible);
	int GetCurrentFractalIndexOnList();
	void SynchronizeInterface(cParameterContainer *par, qInterface::enumReadWrite mode);
	void SynchronizeFractal(cParameterContainer *fractal, qInterface::enumReadWrite mode);

private slots:
	void slotChangedComboFractal(int indexInComboBox);
	void slotPressedButtonResetFormula();

	// IFS
	void slotPressedButtonIFSDefaultsDodecahedron();
	void slotPressedButtonIFSDefaultsIcosahedron();
	void slotPressedButtonIFSDefaultsOctahedron();
	void slotPressedButtonIFSDefaultsMengerSponge();
	void slotPressedButtonIFSDefaultsReset();

private:
	void ConnectSignals();

	Ui::cTabFractal *ui;

	int tabIndex;
	QWidget *fractalWidget;

	cAutomatedWidgets *automatedWidgets;
};

#endif /* MANDELBULBER2_QT_TAB_FRACTAL_H_ */
