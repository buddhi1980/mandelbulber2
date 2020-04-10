/*
 * custom_formula_editor.h
 *
 *  Created on: 19 mar 2020
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_CUSTOM_FORMULA_EDITOR_H_
#define MANDELBULBER2_QT_CUSTOM_FORMULA_EDITOR_H_

#include <QWidget>

namespace Ui
{
class cCustomFormulaEditor;
}

class cCustomFormulaEditor : public QWidget
{
	Q_OBJECT

public:
	explicit cCustomFormulaEditor(QWidget *parent = nullptr);
	~cCustomFormulaEditor();

private slots:
	void slotNewFormula();
	void slotTextChanged();
	void slotLoadBuiltIn();
	void slotCheckSyntax();

private:
	Ui::cCustomFormulaEditor *ui;
};

#endif /* MANDELBULBER2_QT_CUSTOM_FORMULA_EDITOR_H_ */
