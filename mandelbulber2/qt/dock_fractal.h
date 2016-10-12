/*
 * dock_fractal.h
 *
 *  Created on: 12 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_DOCK_FRACTAL_H_
#define MANDELBULBER2_QT_DOCK_FRACTAL_H_

#include <QtWidgets/QtWidgets>
#include <QWidget>

#include "../src/automated_widgets.hpp"
#include "../src/fractal_container.hpp"
#include "../src/parameters.hpp"
#include "../src/synchronize_interface.hpp"

namespace Ui
{
class cDockFractal;
}

class cDockFractal : public QWidget
{
	Q_OBJECT
public:
	explicit cDockFractal(QWidget *parent = NULL);
	~cDockFractal();

	void InitializeFractalUi(QString &uiFileName);
	bool AreHybridFractalsEnabled();
	void EnableJuliaMode();
	void SynchronizeInterfaceFractals(
		cParameterContainer *par, cFractalContainer *parFractal, qInterface::enumReadWrite mode);
	void SynchronizeInterfaceJulia();
	void SynchronizeInterfacePrimitives();

	QWidget *GetContainerWithPrimitives();
	QVBoxLayout *GetLayoutWithPrimitives();

private slots:
	void slotChangedJuliaPoint();
	void slotPressedButtonGetJuliaConstant();
	void slotPressedButtonNewPrimitive();
	void slotGroupCheckJuliaModeToggled(bool state);
	void slotChangedCheckBoxHybridFractal(int state);
	void slotChangedCheckBoxBooleanOperators(bool state);
	void slotChangedCheckBoxJuliaMode(bool state);
	void slotToggledFractalEnable(int fractalIndex, bool enabled);
	void slotChangedComboFractal(int index);
	void slotFractalSwap(int swapA, int swapB);
	void slotPressedButtonResetFormula();

	// IFS
	void slotPressedButtonIFSDefaultsDodecahedron();
	void slotPressedButtonIFSDefaultsIcosahedron();
	void slotPressedButtonIFSDefaultsOctahedron();
	void slotPressedButtonIFSDefaultsMengerSponge();
	void slotPressedButtonIFSDefaultsReset();

private:
	void ConnectSignals();
	Ui::cDockFractal *ui;

	cAutomatedWidgets *automatedWidgets;

	QWidget **fractalWidgets;
};

#endif /* MANDELBULBER2_QT_DOCK_FRACTAL_H_ */
