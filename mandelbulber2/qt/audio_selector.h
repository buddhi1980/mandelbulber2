/*
 * audio_selector.h
 *
 *  Created on: 12 lis 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_AUDIO_SELECTOR_H_
#define MANDELBULBER2_QT_AUDIO_SELECTOR_H_

#include <QWidget>

// forward declarations
class cAutomatedWidgets;

namespace Ui
{
class cAudioSelector;
}

class cAudioSelector : public QWidget
{
	Q_OBJECT
public:
	explicit cAudioSelector(QWidget *parent = NULL);
	~cAudioSelector();

private slots:
	void slotLoadAudioFile();

private:
	void ConnectSignals();

	Ui::cAudioSelector *ui;

	cAutomatedWidgets *automatedWidgets;
};

#endif /* MANDELBULBER2_QT_AUDIO_SELECTOR_H_ */
