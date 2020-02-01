/*
 * settings_cleaner.h
 *
 *  Created on: 1 lut 2020
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_SETTINGS_CLEANER_H_
#define MANDELBULBER2_QT_SETTINGS_CLEANER_H_

#include <QDialog>

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

private:
	Ui::cSettingsCleaner *ui;

	bool stopRequest = false;
};

#endif /* MANDELBULBER2_QT_SETTINGS_CLEANER_H_ */
