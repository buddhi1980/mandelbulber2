#ifndef VOLUME_SLICE_DIALOG_H
#define VOLUME_SLICE_DIALOG_H

#include <QDialog>
#include "../src/global_data.hpp"
#include "../src/volume_slicer.hpp"
#include "../src/automated_widgets.hpp"

namespace Ui {
	class cVolumeSliceDialog;
}

class cVolumeSliceDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cVolumeSliceDialog(QWidget *parent = 0);
	~cVolumeSliceDialog();

public slots:
	void on_pushButton_start_render_slices_clicked();
	void on_pushButton_stop_render_slices_clicked();
	void on_pushButton_select_image_path_clicked();
	void on_pushButton_show_slices_clicked();
	void slotUpdateProgressAndStatus(const QString &text,
		const QString &progressText, double progress);
	void slotSlicerFinished();

protected:
	void closeEvent(QCloseEvent * event);
	void showEvent(QShowEvent * event);
private:
	Ui::cVolumeSliceDialog *ui;
	cAutomatedWidgets *automatedWidgets;
	bool initFinished;
	bool slicerBusy;
	cVolumeSlicer* volumeSlicer;
};

#endif // VOLUME_SLICE_DIALOG_H
