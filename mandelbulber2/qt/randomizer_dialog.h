/*
 * randomizer_dialog.h
 *
 *  Created on: 29 gru 2019
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_RANDOMIZER_DIALOG_H_
#define MANDELBULBER2_QT_RANDOMIZER_DIALOG_H_

#include <QDialog>

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

private:
	Ui::cRandomizerDialog *ui;
};

#endif /* MANDELBULBER2_QT_RANDOMIZER_DIALOG_H_ */
