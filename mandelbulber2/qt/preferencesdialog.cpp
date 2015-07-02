#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QtCore>
#include <QFileDialog>

cPreferencesDialog::cPreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::cPreferencesDialog)
{
  ui->setupUi(this);

	gMainInterface->ConnectSignalsForSlidersInWindow(this);
  gMainInterface->SynchronizeInterfaceWindow(this, gPar, cInterface::write);
	ui->comboBox_ui_style_type->addItems(QStyleFactory::keys());
	ui->comboBox_ui_style_type->setCurrentIndex(gPar->Get<int>("ui_style_type"));
	ui->comboBox_ui_skin->setCurrentIndex(gPar->Get<int>("ui_skin"));
	ui->comboBox_language->addItems(systemData.supportedLanguages.values());

	if(systemData.supportedLanguages.contains(gPar->Get<QString>("language")))
	{
		QString value = systemData.supportedLanguages[gPar->Get<QString>("language")];
		int index = ui->comboBox_language->findText(value);
		ui->comboBox_language->setCurrentIndex(index);
	}
	else
	{
		int index = ui->comboBox_language->findText(systemData.locale.name());
		ui->comboBox_language->setCurrentIndex(index);
	}

	connect(ui->comboBox_ui_style_type, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_ui_style_type(int)));
	connect(ui->comboBox_ui_skin, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_ui_skin(int)));
	connect(ui->comboBox_language, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_language(int)));
}

cPreferencesDialog::~cPreferencesDialog()
{
  delete ui;
}

void cPreferencesDialog::on_buttonBox_accepted()
{
	gMainInterface->SynchronizeInterfaceWindow(this, gPar, cInterface::read);

	QFont font = gMainInterface->mainWindow->font();
	font.setPixelSize(gPar->Get<int>("ui_font_size"));
	gMainInterface->mainWindow->setFont(font);
}

void cPreferencesDialog::on_pushButton_select_image_path_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Select default directory for images"), ui->text_default_image_path->text());
  if(dir.length() > 0)
  {
  	ui->text_default_image_path->setText(dir);
  }
}

void cPreferencesDialog::on_pushButton_select_settings_path_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Select default directory for settings files"), ui->text_default_settings_path->text());
  if(dir.length() > 0)
  {
  	ui->text_default_settings_path->setText(dir);
  }
}

void cPreferencesDialog::on_pushButton_select_textures_path_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Select default directory for textures"), ui->text_default_textures_path->text());
  if(dir.length() > 0)
  {
  	ui->text_default_textures_path->setText(dir);
  }
}

void cPreferencesDialog::on_comboBox_ui_style_type(int index)
{
	gPar->Set<int>("ui_style_type", index);
	UpdateUIStyle();
}

void cPreferencesDialog::on_comboBox_ui_skin(int index)
{
	gPar->Set<int>("ui_skin", index);
	UpdateUISkin();
}


void cPreferencesDialog::on_comboBox_language(int index)
{
	QString value = ui->comboBox_language->currentText();
	QString key = systemData.supportedLanguages.key(value);
	gPar->Set<QString>("language", key);
	UpdateLanguage();
}
