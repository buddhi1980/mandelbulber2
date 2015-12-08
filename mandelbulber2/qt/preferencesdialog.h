#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include "../src/global_data.hpp"

namespace Ui {
	class cPreferencesDialog;
}

class cPreferencesDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cPreferencesDialog(QWidget *parent = 0);
	~cPreferencesDialog();

private slots:
	void on_buttonBox_accepted();
	void on_pushButton_select_image_path_clicked();
	void on_pushButton_select_settings_path_clicked();
	void on_pushButton_select_textures_path_clicked();
	void on_pushButton_clear_thumbnail_cache_clicked();
	void on_comboBox_ui_style_type_currentIndexChanged(int index);
	void on_comboBox_ui_skin_currentIndexChanged(int index);

private:
	Ui::cPreferencesDialog *ui;
	bool initFinished;
};

#endif // PREFERENCESDIALOG_H
