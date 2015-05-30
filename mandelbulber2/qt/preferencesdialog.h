#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

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

private:
    Ui::cPreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
