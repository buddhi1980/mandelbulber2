#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QtCore>
#include <QFileDialog>

cPreferencesDialog::cPreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::cPreferencesDialog)
{
  ui->setupUi(this);

  gMainInterface->SynchronizeInterfaceWindow(this, gPar, cInterface::write);
}

cPreferencesDialog::~cPreferencesDialog()
{
  delete ui;
}

void cPreferencesDialog::on_buttonBox_accepted()
{

}

void cPreferencesDialog::on_pushButton_select_image_path_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Select default directory for images"), gPar->Get<QString>("default_image_path"));
  if(dir.length() > 0)
  {
    gPar->Set("default_image_path", dir);
    gMainInterface->SynchronizeInterfaceWindow(this, gPar, cInterface::write);
  }
}

void cPreferencesDialog::on_pushButton_select_settings_path_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Select default directory for settings files"), gPar->Get<QString>("default_settings_path"));
  if(dir.length() > 0)
  {
    gPar->Set("default_settings_path", dir);
    gMainInterface->SynchronizeInterfaceWindow(this, gPar, cInterface::write);
  }
}

void cPreferencesDialog::on_pushButton_select_textures_path_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Select default directory for textures"), gPar->Get<QString>("default_textures_path"));
  if(dir.length() > 0)
  {
    gPar->Set("default_textures_path", dir);
    gMainInterface->SynchronizeInterfaceWindow(this, gPar, cInterface::write);
  }
}
