/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com), Robert Pancoast (RobertPancoast77@gmail.com)
 *
 * cPreferencesDialog - dialog to configure and maintain application Preferences
 */

#include "preferences_dialog.h"

#include <QCryptographicHash>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCore>

#include "ui_preferences_dialog.h"

#include "dock_navigation.h"
#include "thumbnail_widget.h"

#include "src/automated_widgets.hpp"
#include "src/file_downloader.hpp"
#include "src/fractal_container.hpp"
#include "src/fractal_enums.h"
#include "src/global_data.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/opencl_engine_render_fractal.h"
#include "src/opencl_engine_render_ssao.h"
#include "src/opencl_global.h"
#include "src/opencl_hardware.h"
#include "src/progress_text.hpp"
#include "src/render_window.hpp"
#include "src/settings.hpp"
#include "src/system.hpp"

cPreferencesDialog::cPreferencesDialog(QWidget *parent)
		: QDialog(parent), ui(new Ui::cPreferencesDialog)
{
	initFinished = false;
	ui->setupUi(this);

	int maximumCores = get_cpu_count();
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

	ui->label_path_to_logfile->setText(tr("Path to log file: %1").arg(systemData.logfileName));

#ifdef USE_OPENCL
	UpdateOpenCLListBoxes();
	UpdateOpenCLMemoryLimits();
#else	// USE_OPENCL
	ui->tabWidget->removeTab(2); // hide GPU tab for now
#endif // USE_OPENCL

	if (gPar->Get<bool>("ui_colorize"))
		cInterface::ColorizeGroupBoxes(this, gPar->Get<int>("ui_colorize_random_seed"));

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
	font.setPointSizeF(gPar->Get<int>("ui_font_size"));
	gMainInterface->mainWindow->setFont(font);

	QString value = ui->comboboxLanguage->currentText();
	QString key = systemData.supportedLanguages.key(value);
	gPar->Set<QString>("language", key);

	systemData.loggingVerbosity = gPar->Get<int>("logging_verbosity");

#ifdef USE_OPENCL
	// OpenCL preference dialogue supports (1) platform
	int selectedPlatform = ui->listWidget_opencl_platform_list->currentIndex().row();
	if (selectedPlatform < 0) selectedPlatform = 0;
	gPar->Set("opencl_platform", selectedPlatform);

	QList<QListWidgetItem *> selectedDevicesItems =
		ui->listWidget_opencl_device_list->selectedItems();
	QList<QPair<QString, QString>> devices = GetOpenCLDevices();
	QStringList activeDevices;
	for (auto selectedDevicesItem : selectedDevicesItems)
	{
		activeDevices.append(selectedDevicesItem->data(1).toString());
	}
	QString listString = activeDevices.join("|");

	// OpenCL preference dialogue supports multiple devices
	gPar->Set("opencl_device_list", listString);
	gOpenCl->openClHardware->EnableDevicesByHashList(listString);

	gPar->Set<int>("toolbar_icon_size", gPar->Get<int>("toolbar_icon_size"));
	gMainInterface->mainWindow->slotPopulateToolbar(true);
#endif
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
	uint thumbnailDirCount = thumbnailDir.count();

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
		fileDownloader.downloadFileList();
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
		"This will render all settings files which are yet missing in your thumbnail cache.\n";
	infoGenerateCacheFiles +=
		"This process will take a lot of time and cannot be cancelled.\nProceed?";

	reply = QMessageBox::question(nullptr,
		QObject::tr("Are you sure to generate all example thumbnail cache files?"),
		infoGenerateCacheFiles, QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		// this renders all example files and from settings folder as a thumbnail and saves them to the
		// thumbnail cache

		QStringList listOfFiles;

		{
			QString examplePath = QDir::toNativeSeparators(systemData.sharedDir + "examples");
			QDirIterator it(
				examplePath, QStringList() << "*.fract", QDir::Files, QDirIterator::Subdirectories);
			QStringList exampleFiles;
			while (it.hasNext())
				listOfFiles << it.next();
		}
		{
			QDirIterator it(systemData.GetSettingsFolder(), QStringList() << "*.fract", QDir::Files,
				QDirIterator::Subdirectories);
			QStringList settingsFiles;
			while (it.hasNext())
				listOfFiles << it.next();
		}

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
		for (int i = 0; i < listOfFiles.size(); i++)
		{
			if (systemData.globalStopRequest) break;

			QString filename = listOfFiles.at(i);
			gMainInterface->mainWindow->slotUpdateProgressAndStatus(QString("Rendering examples"),
				tr("rendering %1, %2 of %3")
					.arg(filename, QString::number(i + 1), QString::number(listOfFiles.size())),
				1.0 * (i + 1) / listOfFiles.size(), cProgressText::progress_ANIMATION);

			cSettings parSettings(cSettings::formatFullText);
			parSettings.BeQuiet(true);
			parSettings.LoadFromFile(filename);

			if (parSettings.Decode(examplePar, exampleParFractal))
			{
				examplePar->Set("opencl_mode", gPar->Get<int>("opencl_mode"));
				examplePar->Set("opencl_enabled", gPar->Get<bool>("opencl_enabled"));

				thumbWidget->DisableTimer();
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

QList<QPair<QString, QString>> cPreferencesDialog::GetOpenCLDevices()
{
	QList<QPair<QString, QString>> devices;
#ifdef USE_OPENCL
	QList<cOpenClDevice::sDeviceInformation> openclDevs =
		gOpenCl->openClHardware->getDevicesInformation();
	for (cOpenClDevice::sDeviceInformation openclDev : openclDevs)
	{
		QByteArray hash = openclDev.hash;
		devices << QPair<QString, QString>(hash.toHex(), openclDev.deviceName);
	}
#endif // USE_OPENCL
	return devices;
}

#ifdef USE_OPENCL
void cPreferencesDialog::on_listWidget_opencl_platform_list_currentRowChanged(int index)
{
	if (index >= 0)
	{
		gOpenCl->Reset();
		gOpenCl->openClHardware->CreateContext(
			index, cOpenClDevice::enumOpenClDeviceType(ui->comboBox_opencl_device_type->currentIndex()));

		ui->listWidget_opencl_device_list->clear();

		QList<QPair<QString, QString>> devices = GetOpenCLDevices();
		QStringList selectedDevices = gPar->Get<QString>("opencl_device_list").split("|");
		for (auto device : devices)
		{
			QListWidgetItem *item = new QListWidgetItem(device.second);
			item->setData(1, device.first);
			bool selected = selectedDevices.contains(device.first);
			ui->listWidget_opencl_device_list->addItem(item);
			item->setSelected(selected);
		}
	}
}

void cPreferencesDialog::on_groupCheck_opencl_enabled_toggled(bool state)
{
	if (state)
	{
		gOpenCl->Reset();
		gOpenCl->openClHardware->ListOpenClPlatforms();
		if (gPar->Get<int>("opencl_platform") >= 0)
		{
			gOpenCl->openClHardware->CreateContext(gPar->Get<int>("opencl_platform"),
				cOpenClDevice::enumOpenClDeviceType(ui->comboBox_opencl_device_type->currentIndex()));
			gOpenCl->openClHardware->EnableDevicesByHashList(gPar->Get<QString>("opencl_device_list"));
		}

		UpdateOpenCLListBoxes();
	}

	gMainInterface->mainWindow->GetWidgetDockNavigation()->EnableOpenCLModeComboBox(state);
}

void cPreferencesDialog::UpdateOpenCLListBoxes()
{
	// list of platforms
	QList<cOpenClHardware::sPlatformInformation> platformsInformation =
		gOpenCl->openClHardware->getPlatformsInformation();

	ui->listWidget_opencl_platform_list->clear();
	for (auto &platformInformation : platformsInformation)
	{
		QListWidgetItem *item =
			new QListWidgetItem(platformInformation.name + ", " + platformInformation.vendor + ", "
													+ platformInformation.version);
		ui->listWidget_opencl_platform_list->addItem(item);
	}

	int selectedPlatformIndex = gPar->Get<int>("opencl_platform");
	if (selectedPlatformIndex < platformsInformation.size() && selectedPlatformIndex >= 0)
	{
		ui->listWidget_opencl_platform_list->item(selectedPlatformIndex)->setSelected(true);
		ui->listWidget_opencl_platform_list->setCurrentRow(selectedPlatformIndex);
	}

	ui->listWidget_opencl_device_list->clear();
	QList<QPair<QString, QString>> devices = GetOpenCLDevices();
	QStringList selectedDevices = gPar->Get<QString>("opencl_device_list").split("|");
	bool noDeviceSelected = selectedDevices.first().isEmpty();

	for (QPair<QString, QString> device : devices)
	{
		QListWidgetItem *item = new QListWidgetItem(device.second);
		item->setData(1, device.first);
		bool selected = selectedDevices.contains(device.first);
		ui->listWidget_opencl_device_list->addItem(item);
		if (noDeviceSelected)
		{
			noDeviceSelected = false;
			selected = true;
		}
		item->setSelected(selected);
	}
}

void cPreferencesDialog::on_comboBox_opencl_device_type_currentIndexChanged(int index)
{
	Q_UNUSED(index);
	on_listWidget_opencl_platform_list_currentRowChanged(
		ui->listWidget_opencl_platform_list->currentRow());
}

void cPreferencesDialog::UpdateOpenCLMemoryLimits()
{
	// TODO: support multi-GPU
	if (gOpenCl->openClHardware->getDevicesInformation().size() > 0
			&& gOpenCl->openClHardware->getSelectedDevicesInformation().size() > 0)
	{
		cOpenClDevice::sDeviceInformation deviceInformation =
			gOpenCl->openClHardware->getSelectedDevicesInformation().at(0);

		// TODO: Review here:
		for (auto i : gOpenCl->openClHardware->getSelectedDevicesInformation())
		{
			// Select device with lowest memory for symmetrical allocation limit
			if (i.globalMemSize < deviceInformation.globalMemSize) deviceInformation = i;
		}

		int globalMemSize = int(deviceInformation.globalMemSize / 1024 / 1024);
		int maxMemAllocSize = int(deviceInformation.maxMemAllocSize / 1024 / 1024);

		ui->spinboxInt_opencl_memory_limit->setMaximum(globalMemSize);
		ui->sliderInt_opencl_memory_limit->setMaximum(globalMemSize);
		ui->label_opencl_suggested_memory_limit->setText(
			tr("Suggested memory limit (based on CL_DEVICE_MAX_MEM_ALLOC_SIZE): %1 MB")
				.arg(maxMemAllocSize - 1));
	}
}
#endif
