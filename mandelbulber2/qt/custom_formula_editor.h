/*
 * custom_formula_editor.h
 *
 *  Created on: 19 mar 2020
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_CUSTOM_FORMULA_EDITOR_H_
#define MANDELBULBER2_QT_CUSTOM_FORMULA_EDITOR_H_

#include <QWidget>
#include "src/one_parameter.hpp"

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
	void AssignSlot(int slot) { slotIndex = slot; }

private slots:
	void slotNewFormula();
	void slotTextChanged();
	void slotLoadBuiltIn();
	void slotAutoFormat();
	bool clangFormatPresent();
	void slotCheckSyntax();
	void slotInsertParameter();
	void slotSave();
	void slotLoad();
	void slotRebuildUI();
	void slotCursorMoved();

private:
	struct sParameterDesctiption
	{
		QString parameterName;
		cOneParameter parameter;
	};

private:
	QStringList CreateListOfParametersInCode();
	QList<sParameterDesctiption> ConvertListOfParameters(const QStringList &list);
	void BuildUI(const QList<sParameterDesctiption> &listOfParameters);
	void CreateConversionTable();
	void RemoveVectorComponent(QString &sourceName);

	Ui::cCustomFormulaEditor *ui;

	int slotIndex = 0;
	QMap<QString, QString> conversionTable;
	QMap<QString, QString> parameterTypes;
	QString actualFormulaFileName;
};

#endif /* MANDELBULBER2_QT_CUSTOM_FORMULA_EDITOR_H_ */
