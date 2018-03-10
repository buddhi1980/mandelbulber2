/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * RenderWindow class - main program window
 *
 * This file contains implementation of the menu slots in RenderWindow class.
 * See also header render_window.hpp and whole implementation of class
 * spread over render_window_*.cpp
 */

#include "ui_render_window.h"

#include "animation_flight.hpp"
#include "animation_keyframes.hpp"
#include "file_image.hpp"
#include "files.h"
#include "global_data.hpp"
#include "initparameters.hpp"
#include "interface.hpp"
#include "material_item_model.h"
#include "old_settings.hpp"
#include "render_window.hpp"
#include "settings.hpp"

#include "qt/image_save_dialog.h"
#include "qt/preview_file_dialog.h"

void RenderWindow::slotImportOldSettings()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.lastSettingsFile));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Import settings from old Mandelbulber (v1.21)..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		oldSettings::cOldSettings oldSettings;
		oldSettings.LoadSettings(filename);
		oldSettings.ConvertToNewContainer(gPar, gParFractal);
		gMainInterface->RebuildPrimitives(gPar);
		gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
		gMainInterface->ComboMouseClickUpdate();
		systemData.lastSettingsFile = filename;
		this->setWindowTitle(QString("Mandelbulber (") + filename + ")");
	}
}

void RenderWindow::slotMenuAboutMandelbulber()
{
	QString text = "<h2>Mandelbulber</h2>";
	text += "version: <b>" + QString(MANDELBULBER_VERSION_STRING) + "</b>" + "<br>";
	text += "<br>";
	text += "Licence: GNU GPL version 3.0<br>";
	text += "Copyright Ⓒ 2018<br>";
	text += "project leader: Krzysztof Marczak<br>";
	text += "programmers:<br>";
	text += "Krzysztof Marczak<br>";
	text += "Sebastian Jennen<br>";
	text += "Graeme McLaren<br>";
	text += "Bernardo Martelli<br>";
	text += "Robert Pancoast<br>";
	text += "<br>";
	text +=
		"Thanks to many friends from <a "
		"href=\"http://www.fractalforums.com\">www.fractalforums.com</a> for help<br>";
	text += "<br>";
	text += "<a href=\"http://www.mandelbulber.com\">www.mandelbulber.com</a>";

	QMessageBox::about(this, "About Mandelbulber", text);
}

void RenderWindow::slotMenuAboutQt()
{
	QMessageBox::aboutQt(this);
}

void RenderWindow::slotMenuAboutManual()
{
	QString filename = systemData.docDir + "Mandelbulber_Manual.pdf";
	QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
}

void RenderWindow::slotMenuAboutNews()
{
	QString filename = systemData.docDir + "NEWS";

	QFile f(filename);
	QString text = "";
	if (f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		text = f.readAll();
	}

	QLabel *label = new QLabel;
	label->setText(text);
	label->setWordWrap(true);
	label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	QScrollArea *scroll = new QScrollArea();
	scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scroll->setWidget(label);
	scroll->setWidgetResizable(true);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(scroll);
	QDialog *dialog = new QDialog();
	dialog->setLayout(layout);
	dialog->setWindowTitle(QObject::tr("News"));
	dialog->show();
}

void RenderWindow::slotMenuAboutThirdParty()
{
	QString text = "<h2>Third Party</h2>";
	text += "<ul>";
	text += " <li><b>Dark Skin</b> based on <br>";
	text +=
		"		<a "
		"href=\"https://gist.github.com/QuantumCD/6245215\">gist.github.com/QuantumCD/6245215</"
		"a><br>thanks to QuantumCD</li>";
	text += " <li><b>Code highlighting in ui files</b><br>generated with<br>";
	text +=
		"		<a "
		"href=\"http://www.andre-simon.de/doku/highlight/highlight.php\">highlight</a><br>thanks to "
		"André Simon</li>";
	text += " <li><b>Export Mesh </b><br>generated with<br>";
	text +=
		"		<a "
		"href=\"https://github.com/pmneila/PyMCubes/\">PyMCubes</a><br>thanks to "
		"P. M. Neila</li>";
	text += " <li><b>Export Mesh </b><br>Designed with<br>";
	text +=
		"		<a "
		"href=\"http://algoholic.eu/export-meshes-from-mandelbulber/\">Stanford (PLY) mesh format "
		"Support</a><br>thanks to "
		"Stanislaw Adaszewski</li>";
	text += " <li><b>Fonts</b><br>";
	text +=
		"		<a href=\"http://typodermicfonts.com/\">Nasalization</a><br>"
		"thanks to Typodermic Fonts Inc.<br>";
	text += " For more information about font licenses, see README in fonts folder.</li>";
	text += "</ul>";
	QMessageBox::about(this, "About Third Party", text);
}

void RenderWindow::showDescriptionPopup()
{
	if (gPar->Get<bool>("description_popup_do_not_show_again")) return;
	if (gPar->Get<QString>("description") == "") return;

	QMessageBox *messageBox = new QMessageBox(this);
	messageBox->setText(gPar->Get<QString>("description"));
	messageBox->setWindowTitle(QObject::tr("Description"));
	messageBox->setIcon(QMessageBox::Information);
	messageBox->addButton(QMessageBox::Ok);
	QAbstractButton *btnOkDoNotShowAgain =
		messageBox->addButton(QObject::tr("Ok, don't show again"), QMessageBox::YesRole);
	messageBox->setDefaultButton(QMessageBox::Ok);
	int result = messageBox->exec();
	Q_UNUSED(result);
	if (messageBox->clickedButton() == btnOkDoNotShowAgain)
	{
		gPar->Set("description_popup_do_not_show_again", true);
	}
}

void RenderWindow::SaveSettingsToRecent(QString fileName)
{
	QFile recentFilesFile(systemData.GetRecentFilesListFile());
	QStringList recentFiles;
	if (recentFilesFile.open(QFile::ReadOnly | QFile::Text))
	{
		QTextStream in(&recentFilesFile);
		QString recentFilesFileContent = in.readAll();
		recentFilesFile.close();
		recentFiles = recentFilesFileContent.split(QRegExp("\n|\r\n|\r"), QString::KeepEmptyParts);
		recentFiles.removeOne(fileName);
	}
	recentFiles.prepend(fileName);
	if (recentFiles.size() > 15) recentFiles.removeLast();
	if (!recentFilesFile.open(QFile::WriteOnly | QFile::Text))
	{
		qCritical() << "Cannot open file to save as recent!";
		return;
	}
	QTextStream out(&recentFilesFile);
	out << recentFiles.join("\n");
	recentFilesFile.close();
	slotPopulateRecentSettings();
}

void RenderWindow::slotMenuLoadExample()
{
	PreviewFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(QDir::toNativeSeparators(
		systemData.sharedDir + QDir::separator() + "examples" + QDir::separator()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).fileName()));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load example settings..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		slotMenuLoadSettingsFromFile(filename);
	}
}

void RenderWindow::slotMenuLoadSettings()
{
	gMainInterface->SynchronizeInterface(
		gPar, gParFractal, qInterface::read); // update appParam before loading new settings

	PreviewFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.lastSettingsFile));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load settings..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		slotMenuLoadSettingsFromFile(filename);
	}
}

void RenderWindow::slotMenuLoadSettingsFromFile(QString fileName)
{
	cSettings parSettings(cSettings::formatFullText);
	gMainInterface->DisablePeriodicRefresh();
	gInterfaceReadyForSynchronization = false;
	parSettings.LoadFromFile(fileName);
	parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
	gMainInterface->RebuildPrimitives(gPar);
	gMainInterface->materialListModel->Regenerate();
	gInterfaceReadyForSynchronization = true;

	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
	gMainInterface->ComboMouseClickUpdate();
	systemData.lastSettingsFile = fileName;
	SaveSettingsToRecent(fileName);
	this->setWindowTitle(QString("Mandelbulber (") + fileName + ")");
	gFlightAnimation->RefreshTable();
	gKeyframeAnimation->RefreshTable();
	gMainInterface->ReEnablePeriodicRefresh();
	showDescriptionPopup();
}

void RenderWindow::slotMenuLoadSettingsFromClipboard()
{
	gMainInterface->SynchronizeInterface(
		gPar, gParFractal, qInterface::read); // update appParam before loading new settings

	cSettings parSettings(cSettings::formatFullText);

	if (parSettings.LoadFromClipboard())
	{
		gMainInterface->DisablePeriodicRefresh();
		gInterfaceReadyForSynchronization = false;
		parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
		gMainInterface->RebuildPrimitives(gPar);
		gMainInterface->materialListModel->Regenerate();
		gInterfaceReadyForSynchronization = true;
		gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
		gMainInterface->ComboMouseClickUpdate();
		systemData.lastSettingsFile = "from clipboard";
		this->setWindowTitle(QString("Mandelbulber (") + "from clipboard" + ")");
		gFlightAnimation->RefreshTable();
		gKeyframeAnimation->RefreshTable();
		gMainInterface->ReEnablePeriodicRefresh();
		showDescriptionPopup();
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Cannot load settings from clipboard!"),
			cErrorMessage::errorMessage, gMainInterface->mainWindow);
	}
}

void RenderWindow::slotMenuRedo()
{
	gMainInterface->Redo();
}

void RenderWindow::ResetDocksPositions()
{
	// restoreGeometry(defaultGeometry);
	restoreState(defaultState);
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

void RenderWindow::slotMenuResetDocksPositions()
{
	restoreState(gMainInterface->settings.value("mainWindowState").toByteArray());
}

void RenderWindow::slotMenuAnimationDocksPositions()
{
	ResetDocksPositions();
	ui->dockWidget_histogram->hide();
	ui->dockWidget_info->hide();
	ui->dockWidget_animation->show();
	ui->toolBar->hide();
#ifdef USE_GAMEPAD
	ui->dockWidget_gamepad_dock->hide();
#endif
	ui->dockWidget_queue_dock->hide();

	tabifyDockWidget(ui->dockWidget_materialEditor, ui->dockWidget_effects);
	tabifyDockWidget(ui->dockWidget_effects, ui->dockWidget_image_adjustments);
	tabifyDockWidget(ui->dockWidget_image_adjustments, ui->dockWidget_rendering_engine);
	tabifyDockWidget(ui->dockWidget_rendering_engine, ui->dockWidget_objects);
	tabifyDockWidget(ui->dockWidget_objects, ui->dockWidget_histogram);

	addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_Materials);
}

void RenderWindow::slotMenuSaveDocksPositions()
{
	gMainInterface->settings.setValue("mainWindowGeometry", saveGeometry());
	gMainInterface->settings.setValue("mainWindowState", saveState());
	// qDebug() << "settings saved";
}

void RenderWindow::slotMenuSaveImageJPEG()
{
	slotMenuSaveImage(
		ImageFileSave::IMAGE_FILE_TYPE_JPG, tr("JPEG images (*.jpg *.jpeg)"), "JPEG", "jpeg");
}

void RenderWindow::slotMenuSaveImagePNG()
{
	slotMenuSaveImage(ImageFileSave::IMAGE_FILE_TYPE_PNG, tr("PNG images (*.png)"), "PNG", "png");
}

#ifdef USE_EXR
void RenderWindow::slotMenuSaveImageEXR()
{
	slotMenuSaveImage(ImageFileSave::IMAGE_FILE_TYPE_EXR, tr("EXR images (*.exr)"), "EXR", "exr");
}
#endif // USE_EXR

#ifdef USE_TIFF
void RenderWindow::slotMenuSaveImageTIFF()
{
	slotMenuSaveImage(
		ImageFileSave::IMAGE_FILE_TYPE_TIFF, tr("TIFF images (*.tiff)"), "TIFF", "tiff");
}
#endif // USE_TIFF

void RenderWindow::slotMenuSaveImage(ImageFileSave::enumImageFileType imageFileType,
	QString nameFilter, QString titleType, QString defaultSuffix)
{
	cImageSaveDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(nameFilter);
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.GetImageFileNameSuggestion()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg(titleType));
	dialog.setDefaultSuffix(defaultSuffix);
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		gApplication->processEvents();
		SaveImage(filename, imageFileType, gMainInterface->mainImage, gMainInterface->mainWindow);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

void RenderWindow::slotMenuSaveImagePNG16()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("PNG images (*.png)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.GetImageFileNameSuggestion()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("16-bit PNG"));
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG"),
			tr("Saving image started"), 0.0, cProgressText::progress_IMAGE);
		gApplication->processEvents();
		ImageFileSave::structSaveImageChannel saveImageChannel(
			ImageFileSave::IMAGE_CONTENT_COLOR, ImageFileSave::IMAGE_CHANNEL_QUALITY_16, "");
		ImageFileSave::ImageConfig imageConfig;
		imageConfig.insert(ImageFileSave::IMAGE_CONTENT_COLOR, saveImageChannel);
		ImageFileSavePNG imageSaver(
			ImageFileSave::ImageNameWithoutExtension(filename), gMainInterface->mainImage, imageConfig);
		imageSaver.SetAppendAlphaCustom(false);
		imageSaver.SaveImage();
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG"),
			tr("Saving image finished"), 1.0, cProgressText::progress_IMAGE);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

void RenderWindow::slotMenuSaveImagePNG16Alpha()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("PNG images (*.png)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.GetImageFileNameSuggestion()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to %1 file...").arg("16-bit PNG + alpha channel"));
	dialog.setDefaultSuffix("png");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG + alpha channel"),
			tr("Saving image started"), 0.0, cProgressText::progress_IMAGE);
		gApplication->processEvents();
		ImageFileSave::structSaveImageChannel saveImageChannel(
			ImageFileSave::IMAGE_CONTENT_COLOR, ImageFileSave::IMAGE_CHANNEL_QUALITY_16, "");
		ImageFileSave::ImageConfig imageConfig;
		imageConfig.insert(ImageFileSave::IMAGE_CONTENT_COLOR, saveImageChannel);
		ImageFileSavePNG imageSaver(
			ImageFileSave::ImageNameWithoutExtension(filename), gMainInterface->mainImage, imageConfig);
		imageSaver.SetAppendAlphaCustom(true);
		imageSaver.SaveImage();
		cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG + alpha channel"),
			tr("Saving image finished"), 1.0, cProgressText::progress_IMAGE);
		gApplication->processEvents();
		systemData.lastImageFile = filename;
	}
}

void RenderWindow::slotMenuSaveSettings()
{
	cSettings parSettings(cSettings::formatCondensedText);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);

	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
	dialog.selectFile(
		QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save settings..."));
	dialog.setDefaultSuffix("fract");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		parSettings.SaveToFile(filename);
		systemData.lastSettingsFile = filename;
		SaveSettingsToRecent(filename);
		this->setWindowTitle(QString("Mandelbulber (") + filename + ")");

		// QString hash = parSettings.GetHashCode();
		// cThumbnail thumbnail(gPar, gParFractal, 200, 200);
		// thumbnail.Render();
		// thumbnail.Save(systemData.thumbnailDir + hash);
	}
}

void RenderWindow::slotMenuSaveSettingsToClipboard()
{
	cSettings parSettings(cSettings::formatCondensedText);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
	parSettings.SaveToClipboard();
	cErrorMessage::showMessage(
		QObject::tr("Settings saved to clipboard"), cErrorMessage::infoMessage);
}

void RenderWindow::slotMenuUndo()
{
	gMainInterface->Undo();
}

void RenderWindow::slotUpdateDocksAndToolbarByAction()
{
	// Animation dock
	if (ui->actionShow_animation_dock->isChecked() != ui->dockWidget_animation->isVisible())
	{
		if (ui->actionShow_animation_dock->isChecked())
		{
			addDockWidget(Qt::BottomDockWidgetArea, ui->dockWidget_animation);
		}
		else
		{
			removeDockWidget(ui->dockWidget_animation);
		}
		ui->dockWidget_animation->setVisible(ui->actionShow_animation_dock->isChecked());
	}

	// Information dock
	if (ui->actionShow_info_dock->isChecked() != ui->dockWidget_info->isVisible())
	{
		if (ui->actionShow_info_dock->isChecked())
		{
			addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_info);
		}
		else
		{
			removeDockWidget(ui->dockWidget_info);
		}
		ui->dockWidget_info->setVisible(ui->actionShow_info_dock->isChecked());
	}

	// Histogram dock
	if (ui->actionShow_statistics_dock->isChecked() != ui->dockWidget_histogram->isVisible())
	{
		if (ui->actionShow_statistics_dock->isChecked())
		{
			addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_histogram);
		}
		else
		{
			removeDockWidget(ui->dockWidget_histogram);
		}
		ui->dockWidget_histogram->setVisible(ui->actionShow_statistics_dock->isChecked());
	}

	// Toolbar
	if (ui->actionShow_toolbar->isChecked() != ui->toolBar->isVisible())
	{
		ui->toolBar->setVisible(ui->actionShow_toolbar->isChecked());
	}

// Gamepad dock
#ifdef USE_GAMEPAD
	if (ui->actionShow_gamepad_dock->isChecked() != ui->dockWidget_gamepad_dock->isVisible())
	{
		if (ui->actionShow_gamepad_dock->isChecked())
		{
			addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget_gamepad_dock);
		}
		else
		{
			removeDockWidget(ui->dockWidget_gamepad_dock);
		}
		ui->dockWidget_gamepad_dock->setVisible(ui->actionShow_gamepad_dock->isChecked());
	}
#endif

	// Queue dock
	if (ui->actionShow_queue_dock->isChecked() != ui->dockWidget_queue_dock->isVisible())
	{
		if (ui->actionShow_queue_dock->isChecked())
		{
			addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget_queue_dock);
		}
		else
		{
			removeDockWidget(ui->dockWidget_queue_dock);
		}
		ui->dockWidget_queue_dock->setVisible(ui->actionShow_queue_dock->isChecked());
	}

	// Queue dock
	if (ui->actionShow_measurement_dock->isChecked() != ui->dockWidget_measurement->isVisible())
	{
		if (ui->actionShow_measurement_dock->isChecked())
		{
			addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget_measurement);
		}
		else
		{
			removeDockWidget(ui->dockWidget_measurement);
		}
		ui->dockWidget_measurement->setVisible(ui->actionShow_measurement_dock->isChecked());
	}
}

void RenderWindow::slotUpdateDocksAndToolbarByView() const
{
	// Animation dock
	if (ui->actionShow_animation_dock->isChecked() != ui->dockWidget_animation->isVisible())
	{
		ui->actionShow_animation_dock->setChecked(ui->dockWidget_animation->isVisible());
	}

	// Log dock
	if (ui->actionShow_info_dock->isChecked() != ui->dockWidget_info->isVisible())
	{
		ui->actionShow_info_dock->setChecked(ui->dockWidget_info->isVisible());
	}

	// Histogram dock
	if (ui->actionShow_statistics_dock->isChecked() != ui->dockWidget_histogram->isVisible())
	{
		ui->actionShow_statistics_dock->setChecked(ui->dockWidget_histogram->isVisible());
	}

	// Toolbar
	if (ui->actionShow_toolbar->isChecked() != ui->toolBar->isVisible())
	{
		ui->actionShow_toolbar->setChecked(ui->toolBar->isVisible());
	}

#ifdef USE_GAMEPAD
	// Gamepad dock
	if (ui->actionShow_gamepad_dock->isChecked() != ui->dockWidget_gamepad_dock->isVisible())
	{
		ui->actionShow_gamepad_dock->setChecked(ui->dockWidget_gamepad_dock->isVisible());
	}
#endif

	// Queue dock
	if (ui->actionShow_queue_dock->isChecked() != ui->dockWidget_queue_dock->isVisible())
	{
		ui->actionShow_queue_dock->setChecked(ui->dockWidget_queue_dock->isVisible());
	}

	// Queue dock
	if (ui->actionShow_measurement_dock->isChecked() != ui->dockWidget_measurement->isVisible())
	{
		ui->actionShow_measurement_dock->setChecked(ui->dockWidget_measurement->isVisible());
	}
}

void RenderWindow::slotStackAllDocks()
{
	tabifyDockWidget(ui->dockWidget_materialEditor, ui->dockWidget_effects);
	tabifyDockWidget(ui->dockWidget_effects, ui->dockWidget_image_adjustments);
	tabifyDockWidget(ui->dockWidget_image_adjustments, ui->dockWidget_rendering_engine);
	tabifyDockWidget(ui->dockWidget_rendering_engine, ui->dockWidget_objects);
	tabifyDockWidget(ui->dockWidget_objects, ui->dockWidget_histogram);
	tabifyDockWidget(ui->dockWidget_histogram, ui->dockWidget_info);
}

void RenderWindow::slotDetachMainImage()
{
	if (ui->actionDetach_image_from_main_window->isChecked())
	{
		gMainInterface->DetachMainImageWidget();
	}
	else
	{
		gMainInterface->AttachMainImageWidget();
	}
}
