#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

cPreferencesDialog::cPreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::cPreferencesDialog)
{
  ui->setupUi(this);
}

cPreferencesDialog::~cPreferencesDialog()
{
  delete ui;
}

void cPreferencesDialog::on_buttonBox_accepted()
{

}
