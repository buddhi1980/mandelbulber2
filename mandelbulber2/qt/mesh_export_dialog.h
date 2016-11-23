/*
 * Copyright (C) Stanislaw Adaszewski, 2016
 * http://algoholic.eu
 */

#ifndef MANDELBULBER2_QT_MESH_EXPORT_DIALOG_H_
#define MANDELBULBER2_QT_MESH_EXPORT_DIALOG_H_

#include <QDialog>

// forward declarations
class cAutomatedWidgets;
class cMeshExport;

namespace Ui
{
class cMeshExportDialog;
}

class cMeshExportDialog : public QDialog
{
	Q_OBJECT

public:
        explicit cMeshExportDialog(QWidget *parent = 0);
        ~cMeshExportDialog();

public slots:
	void on_pushButton_start_render_layers_clicked();
	void on_pushButton_stop_render_layers_clicked();
	void on_pushButton_select_image_path_clicked();
	void slotUpdateProgressAndStatus(
		const QString &text, const QString &progressText, double progress);
	void slotSlicerFinished();

protected:
	void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent *event);

private:
        Ui::cMeshExportDialog *ui;
	cAutomatedWidgets *automatedWidgets;
	bool initFinished;
	bool slicerBusy;
        cMeshExport *meshExport;
};

#endif /* MANDELBULBER2_QT_MESH_EXPORT_DIALOG_H_ */
