/*
 * settings_cleaner.h
 *
 *  Created on: 1 lut 2020
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_SETTINGS_CLEANER_H_
#define MANDELBULBER2_QT_SETTINGS_CLEANER_H_

#include <QDialog>
#include <QListWidgetItem>

#include "src/fractal_container.hpp"
#include "src/parameters.hpp"

namespace Ui
{
class cSettingsCleaner;
}

class cSettingsCleaner : public QDialog
{
	Q_OBJECT

public:
	explicit cSettingsCleaner(QWidget *parent = nullptr);
	~cSettingsCleaner();
	void runCleaner();

private:
	void UpdateProgressBar(double progress);

	// events
	void closeEvent(QCloseEvent *event) override;

private slots:
	void slotPressedStop();
	void slotPressedOK();
	void slotPressedCancel();
	void slotRemoveItem(QListWidgetItem *item);

private:
	Ui::cSettingsCleaner *ui;

	struct sDefaultedParameter
	{
		QString parameterName;
		cParameterContainer *actualContainer;
		cParameterContainer *originalContainer;
	};

	bool stopRequest = false;
	cParameterContainer actualParams;
	cFractalContainer actualFractalParams;
	QList<sDefaultedParameter> listOfAllDefaultedParameters;
};

#endif /* MANDELBULBER2_QT_SETTINGS_CLEANER_H_ */
