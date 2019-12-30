/*
 * randomizer_dialog.h
 *
 *  Created on: 29 gru 2019
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_RANDOMIZER_DIALOG_H_
#define MANDELBULBER2_QT_RANDOMIZER_DIALOG_H_

#include <QDialog>

#include "src/fractal_container.hpp"
#include "src/parameters.hpp"
#include "src/tree_string_list.h"

namespace Ui
{
class cRandomizerDialog;
}

class cRandomizerDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cRandomizerDialog(QWidget *parent = nullptr);
	~cRandomizerDialog();

	void AssignSourceWidget(const QWidget *sourceWidget);

private:
	enum enimRandomizeStrength
	{
		randomizeSlight,
		randomizeMedium,
		randomizeHeavy
	};

	struct sParameterVersion
	{
		cParameterContainer params;
		cFractalContainer fractParams;
		QStringList modifiedParameters;
	};

	const int numberOfVersions = 9;

	void Randomize(enimRandomizeStrength sthength);
	void CreateParametersTreeInWidget(
		cTreeStringList *tree, const QWidget *widget, int &level, int parentId);

private slots:
	void slotClieckedSlightRandomize();
	void slotClieckedMediumRandomize();
	void slotClieckedHeavyRandomize();

private:
	Ui::cRandomizerDialog *ui;
	cTreeStringList parametersTree;
	QList<sParameterVersion> listOfVersions;
};

#endif /* MANDELBULBER2_QT_RANDOMIZER_DIALOG_H_ */
