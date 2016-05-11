#ifndef VOXEL_EXPORT_DIALOG_H
#define VOXEL_EXPORT_DIALOG_H

#include <QDialog>
#include "../src/global_data.hpp"
#include "../src/voxel_export.hpp"
#include "../src/automated_widgets.hpp"

namespace Ui {
	class cVoxelExportDialog;
}

class cVoxelExportDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cVoxelExportDialog(QWidget *parent = 0);
	~cVoxelExportDialog();

public slots:
	void on_pushButton_start_render_layers_clicked();
	void on_pushButton_stop_render_layers_clicked();
	void on_pushButton_select_image_path_clicked();
	void on_pushButton_show_layers_clicked();
	void slotUpdateProgressAndStatus(const QString &text,
		const QString &progressText, double progress);
	void slotSlicerFinished();

protected:
	void closeEvent(QCloseEvent * event);
	void showEvent(QShowEvent * event);
private:
	Ui::cVoxelExportDialog *ui;
	cAutomatedWidgets *automatedWidgets;
	bool initFinished;
	bool slicerBusy;
	cVoxelExport* voxelExport;
};

#endif // VOXEL_EXPORT_DIALOG_H
