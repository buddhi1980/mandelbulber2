/*
 * preview_file_dialog.h
 *
 *  Created on: Aug 2, 2014
 *      Author: krzysztof
 */

#ifndef PREVIEWFILEDIALOG_H_
#define PREVIEWFILEDIALOG_H_

#include <QFileDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QCheckBox>

class PreviewFileDialog: public QFileDialog
{
	Q_OBJECT
public:
	PreviewFileDialog(QWidget *parent);
	~PreviewFileDialog();
protected slots:
	void OnCurredbtChanged(const QString & filename);

private:
	QVBoxLayout* vboxlayout;
	QCheckBox *checkbox;
protected:
	QLabel *preview;
	QLabel *info;
};

#endif /* PREVIEWFILEDIALOG_H_ */
