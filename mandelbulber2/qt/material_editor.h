/*
 * material_editor.h
 *
 *  Created on: 3 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MATERIAL_EDITOR_H_
#define MANDELBULBER2_QT_MATERIAL_EDITOR_H_

#include <QWidget>

#include "../src/color_palette.hpp"
#include "../src/parameters.hpp"
#include "../src/automated_widgets.hpp"

namespace Ui {
	class cMaterialEditor;
}

class cMaterialEditor : public QWidget
{
	Q_OBJECT

public:
	explicit cMaterialEditor(QWidget *parent = NULL);
	~cMaterialEditor();
	void AssignMaterial(cParameterContainer *params, int index);

private slots:
void slotPressedButtonNewRandomPalette();
void slotPressedButtonGetPaletteFromImage();
void slotPressedButtonRandomize();
void slotChangedSpinBoxPaletteOffset(double value);
void slotChangedSpinBoxPaletteSize(int value);
void slotChangedComboFractalColoringAlgorithm(int index);

private:
	void ConnectSignals(void);
	cColorPalette GetPaletteFromImage(const QString &filename);

	Ui::cMaterialEditor *ui;
	int materialIndex;
	cParameterContainer *parameterContainer;
	bool isMaterialAssigned;

	cAutomatedWidgets *automatedWidgets;

signals:
	void materialChanged(int matIndex);
};

#endif /* MANDELBULBER2_QT_MATERIAL_EDITOR_H_ */



