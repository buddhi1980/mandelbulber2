/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * cPreferencesDialog - dialog to configure program wide settings and program maintaining
 */

#include "preferences_dialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QtCore>

#include "../qt/thumbnail_widget.h"
#include "../src/automated_widgets.hpp"
#include "../src/file_downloader.hpp"
#include "../src/fractal_container.hpp"
#include "../src/fractal_enums.h"
#include "../src/global_data.hpp"
#include "../src/initparameters.hpp"
#include "../src/interface.hpp"
#include "../src/progress_text.hpp"
#include "../src/render_window.hpp"
#include "../src/settings.hpp"
#include "../src/system.hpp"
#include "ui_preferences_dialog.h"

cPreferencesDialog::cPreferencesDialog(QWidget *parent)
		: QDialog(parent), ui(new Ui::cPreferencesDialog)
{
	initFinished = false;
	ui->setupUi(this);

	int maximumCores = get_cpu_count();
	ui->sliderInt_limit_CPU_cores->setMaximum(maximumCores);
	ui->spinboxInt_limit_CPU_cores->setMaximum(maximumCores);

#ifndef WIN32
	ui->label_threads_priority->hide();
	ui->comboBox_threads_priority->hide();
#endif

	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);

	SynchronizeInterfaceWindow(this, gPar, qInterface::write);
	ui->comboBox_ui_style_type->addItems(QStyleFactory::keys());
	ui->comboBox_ui_style_type->setCurrentIndex(gPar->Get<int>("ui_style_type"));
	ui->comboBox_ui_skin->setCurrentIndex(gPar->Get<int>("ui_skin"));
	ui->comboboxLanguage->addItems(systemData.supportedLanguages.values());

	if (systemData.supportedLanguages.contains(gPar->Get<QString>("language")))
	{
		QString value = systemData.supportedLanguages[gPar->Get<QString>("language")];
		int index = ui->comboboxLanguage->findText(value);
		ui->comboboxLanguage->setCurrentIndex(index);
	}
	else
	{
		int index = ui->comboboxLanguage->findText(systemData.locale.name());
		ui->comboboxLanguage->setCurrentIndex(index);
	}

#ifdef USE_OPENCL
// TODO
#else	// USE_OPENCL
	ui->tabWidget->removeTab(2); // hide GPU tab for now
#endif // USE_OPENCL

	initFinished = true;
}

cPreferencesDialog::~cPreferencesDialog()
{
	delete ui;
}

void cPreferencesDialog::on_buttonBox_accepted()
{
	SynchronizeInterfaceWindow(this, gPar, qInterface::read);

	QFont font = gMainInterface->mainWindow->font();
	font.setPixelSize(gPar->Get<int>("ui_font_size"));
	gMainInterface->mainWindow->setFont(font);

	QString value = ui->comboboxLanguage->currentText();
	QString key = systemData.supportedLanguages.key(value);
	gPar->Set<QString>("language", key);

	gPar->Set<int>("toolbar_icon_size", gPar->Get<int>("toolbar_icon_size"));
	gMainInterface->mainWindow->slotPopulateToolbar(true);

	systemData.loggingVerbosity = gPar->Get<int>("logging_verbosity");
}

void cPreferencesDialog::on_pushButton_select_image_path_clicked()
{
	QString dir = QDir::toNativeSeparators(
		QFileDialog::getExistingDirectory(this, tr("Select default directory for images"),
			QDir::toNativeSeparators(ui->text_default_image_path->text())));
	if (dir.length() > 0)
	{
		ui->text_default_image_path->setText(dir);
	}
}

void cPreferencesDialog::on_pushButton_select_settings_path_clicked()
{
	QString dir = QDir::toNativeSeparators(
		QFileDialog::getExistingDirectory(this, tr("Select default directory for settings files"),
			QDir::toNativeSeparators(ui->text_default_settings_path->text())));
	if (dir.length() > 0)
	{
		ui->text_default_settings_path->setText(dir);
	}
}

void cPreferencesDialog::on_pushButton_select_textures_path_clicked()
{
	QString dir = QDir::toNativeSeparators(
		QFileDialog::getExistingDirectory(this, tr("Select default directory for textures"),
			QDir::toNativeSeparators(ui->text_default_textures_path->text())));
	if (dir.length() > 0)
	{
		ui->text_default_textures_path->setText(dir);
	}
}

void cPreferencesDialog::on_pushButton_clear_thumbnail_cache_clicked() const
{
	QDir thumbnailDir(systemData.GetThumbnailsFolder());
	thumbnailDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
	int thumbnailDirCount = thumbnailDir.count();

	// confirmation dialog before clearing
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(nullptr, QObject::tr("Are you sure to clear the thumbnail cache?"),
		QObject::tr("Are you sure to clear the thumbnail cache?\nThere are currently %1 thumbnails "
								"cached. These will be deleted and rerendered when necessary.\n Clear now?")
			.arg(thumbnailDirCount),
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		// match exact 32 char hash images, example filename: c0ad626d8c25ab6a25c8d19a53960c8a.png
		DeleteAllFilesFromDirectory(
			systemData.GetThumbnailsFolder(), "????????????????????????????????.*");
	}
	else
	{
		return;
	}
}

void cPreferencesDialog::on_pushButton_load_thumbnail_cache_clicked() const
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(nullptr,
		QObject::tr("Are you sure to load the thumbnail cache from the server?"),
		QObject::tr("This will try to load missing common thumbnails from the server.\nProceed?"),
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		cFileDownloader fileDownloader(
			QString("http://cdn.mandelbulber.org/thumbnail/%1").arg(MANDELBULBER_VERSION_STRING),
			systemData.GetThumbnailsFolder());
		QObject::connect(&fileDownloader,
			SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
			gMainInterface->mainWindow,
			SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
		fileDownloader.downloadFilelist();
	}
	else
	{
		return;
	}
}

void cPreferencesDialog::on_pushButton_generate_thumbnail_cache_clicked()
{
	QMessageBox::StandardButton reply;
	QString infoGenerateCacheFiles =
		"This is a development feature. If you want to obtain all example cache files, click load!\n\n";
	infoGenerateCacheFiles +=
		"This will render all example files which are yet missing in your thumbnail cache.\n";
	infoGenerateCacheFiles +=
		"This process will take a lot of time and cannot be cancelled.\nProceed?";

	reply = QMessageBox::question(nullptr,
		QObject::tr("Are you sure to generate all example thumbnail cache files?"),
		infoGenerateCacheFiles, QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		// this renders all example files as a thumbnail and saves them to the thumbnail cache
		QString examplePath =
			QDir::toNativeSeparators(systemData.sharedDir + QDir::separator() + "examples");
		QDirIterator it(
			examplePath, QStringList() << "*.fract", QDir::Files, QDirIterator::Subdirectories);
		QStringList exampleFiles;
		while (it.hasNext())
			exampleFiles << it.next();

		cParameterContainer *examplePar = new cParameterContainer;
		cFractalContainer *exampleParFractal = new cFractalContainer;
		cThumbnailWidget *thumbWidget = new cThumbnailWidget(200, 200, 1, this);
		QObject::connect(thumbWidget,
			SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
			gMainInterface->mainWindow,
			SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));

		examplePar->SetContainerName("main");
		InitParams(examplePar);
		/****************** TEMPORARY CODE FOR MATERIALS *******************/

		InitMaterialParams(1, examplePar);

		/*******************************************************************/
		for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		{
			exampleParFractal->at(i).SetContainerName(QString("fractal") + QString::number(i));
			InitFractalParams(&exampleParFractal->at(i));
		}
		for (int i = 0; i < exampleFiles.size(); i++)
		{
			QString filename = exampleFiles.at(i);
			gMainInterface->mainWindow->slotUpdateProgressAndStatus(QString("Rendering examples"),
				tr("rendering %1, %2 of %3")
					.arg(filename, QString::number(i + 1), QString::number(exampleFiles.size())),
				1.0 * (i + 1) / exampleFiles.size(), cProgressText::progress_ANIMATION);

			cSettings parSettings(cSettings::formatFullText);
			parSettings.BeQuiet(true);
			parSettings.LoadFromFile(filename);

			if (parSettings.Decode(examplePar, exampleParFractal))
			{
				thumbWidget->AssignParameters(*examplePar, *exampleParFractal);
				if (!thumbWidget->IsRendered())
				{
					thumbWidget->slotRender();
					while (!thumbWidget->IsRendered())
					{
						gApplication->processEvents();
						Wait(100);
					}
				}
				else
				{
					// "touch" the file, to mark it with current time stamp
					QFile file(thumbWidget->GetThumbnailFileName());
					if (file.open(QIODevice::Append))
					{
						QDataStream out(&file);
						out << "";
						file.close();
					}
				}
			}
		}
		delete exampleParFractal;
		delete examplePar;
		delete thumbWidget;
	}
	else
	{
		return;
	}
}

void cPreferencesDialog::on_comboBox_ui_style_type_currentIndexChanged(int index) const
{
	if (!initFinished) return;
	gPar->Set<int>("ui_style_type", index);
	UpdateUIStyle();
}

void cPreferencesDialog::on_comboBox_ui_skin_currentIndexChanged(int index) const
{
	if (!initFinished) return;
	gPar->Set<int>("ui_skin", index);
	UpdateUISkin();
}

void cPreferencesDialog::on_pushButton_retrieve_toolbar_clicked() const
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(nullptr, QObject::tr("Retrieve toolbar"),
		QObject::tr("Are you sure to retrieve default presets into toolbar?\nIt could overwrite some "
								"of existing presets.\nProceed?"),
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		RetrieveToolbarPresets(true);
	}
}

void cPreferencesDialog::on_pushButton_retrieve_materials_clicked() const
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(nullptr, QObject::tr("Retrieve materials"),
		QObject::tr("Are you sure to retrieve default materials into materials folder?\nIt could "
								"overwrite some of existing materials.\nProceed?"),
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		RetrieveExampleMaterials(true);
	}
}
