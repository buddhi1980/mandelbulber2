/*
 * script_dialog.h
 *
 *  Created on: 25 lis 2023
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_SCRIPT_DIALOG_H_
#define MANDELBULBER2_QT_SCRIPT_DIALOG_H_

#include <QDialog>
#include <QStringList>

class cOneParameter;

namespace Ui
{
class cScriptDialog;
}

class cScriptDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cScriptDialog(QWidget *parent = nullptr);
	~cScriptDialog() override;

	void AssignParameterName(const QString &parameterName, const QString &containerName);

protected:
	void closeEvent(QCloseEvent *event) override;

private slots:
	void slotAccepted();
	void slotTest();
	void slotInsertParameter();
	void slotPopulateComboWithParameters();
	void slotCursorPositionChanged(int oldPos, int newPos);

private:
	Ui::cScriptDialog *ui;
	QString parameterName;
	QString containerName;
	int lastCursorPosition = 0;
};

#endif /* MANDELBULBER2_QT_SCRIPT_DIALOG_H_ */
