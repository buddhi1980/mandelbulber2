/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include "mandelbulb3d_settings.hpp"
#include "material_item_model.h"
#include "old_settings.hpp"
#include "render_window.hpp"
#include "settings.hpp"
#include "system.hpp"
#include "system_data.hpp"
#include "system_directories.hpp"

#include "qt/image_save_dialog.h"
#include "qt/preview_file_dialog.h"
#include "qt/settings_browser.h"

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
		gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
		gMainInterface->ComboMouseClickUpdate();
		systemData.lastSettingsFile = filename;
		setWindowTitle(QString("Mandelbulber (") + filename + ")");
	}
}

void RenderWindow::slotImportMandelbulb3dSettings()
{
	//	QFileDialog dialog(this);
	//	dialog.setOption(QFileDialog::DontUseNativeDialog);
	//	dialog.setFileMode(QFileDialog::ExistingFile);
	//	dialog.setNameFilter(tr("Fractals (*.m3d *.fract)"));
	//	dialog.setDirectory(
	//		QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
	//	dialog.selectFile(QDir::toNativeSeparators(systemData.lastSettingsFile));
	//	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	//	dialog.setWindowTitle(tr("Import settings from Mandelbulb3d settings file ..."));
	//	QStringList filenames;
	//	if (dialog.exec())
	//	{
	//		filenames = dialog.selectedFiles();
	//		QString filename = QDir::toNativeSeparators(filenames.first());
	//		cMandelbulb3dSettings m3dSettings;
	//		m3dSettings.LoadSettings(filename);
	//		m3dSettings.ConvertToNewContainer(gPar, gParFractal);
	//		gMainInterface->RebuildPrimitives(gPar);
	//		gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
	//		gMainInterface->ComboMouseClickUpdate();
	//		systemData.lastSettingsFile = filename;
	//		setWindowTitle(QString("Mandelbulber (") + filename + ")");
	//	}
}

void RenderWindow::slotMenuAboutMandelbulber()
{
	QString text = "<h2>Mandelbulber</h2>";
	text += "version: <b>" + QString(MANDELBULBER_VERSION_STRING) + "</b>" + "<br>";
	text += "<br>";
	text += "Licence: GNU GPL version 3.0<br>";
	text += "Copyright Ⓒ 2025<br>";
	text += "project leader: Krzysztof Marczak<br>";
	text += "Project contributors:<br>";
	text += "Sebastian Jennen, Graeme McLaren, Bernardo Martelli,<br>";
	text += "Robert Pancoast, knighty, makemeunsee, Marius Schilder,<br>";
	text += "Ryan Hitchman, Jeff Epler, Martin Reinecke, Quazgaa, Adrian Meyer<br>";
	text += "github:rikardfalkeborn, github:orbitcowboy, github:brunetton,<br>";
	text += "github:biberino, github:luchansky, github:jeroenrijckaert,<br>";
	text += "github:KoviRobi, github:psyriccio, github:valera-rozuvan,<br>";
	text += "github:probonopd, github:mia-0, github:gitter-badger, <br>";
	text += "github:danuni, github:Starmute<br>";
	text += "<br>";
	text += "Thanks to many friends from Mandelbulber community<br>";
	text +=
		"<a "
		"href=\"https://www.facebook.com/groups/mandelbulber/\">https://www.facebook.com/groups/"
		"mandelbulber/</a>";
	text += "<br>for help<br>";
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
	// qDebug() << systemDirectories.docDir;
	QString filename = systemDirectories.docDir + "Mandelbulber_Manual.pdf";
	QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
}

void RenderWindow::slotMenuAboutNews()
{
	QString filename = systemDirectories.docDir + "NEWS";

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

void RenderWindow::slotMenuAboutHotKeys()
{
	QString ctrlBadge =
		"<span style='background-color: #222; color: #FFF; padding: 3px;'>" + tr("Ctrl") + "</span>";
	QString altBadge =
		"<span style='background-color: #222; color: #FFF; padding: 3px;'>" + tr("Alt") + "</span>";
	QString shiftBadge =
		"<span style='background-color: #222; color: #FFF; padding: 3px;'>" + tr("Shift") + "</span>";

	// Main program hotkeys
	QString style = "<style>td { padding: 2px; color: black;}</style>";
	QString text = "<table><tr>";
	text += "<td style='padding: 5px; background-color: #DDF'>";
	text += "<h3>" + tr("Main Program Hotkeys") + "</h3>";
	text += "<table border='1'>";

	// File
	text += " <tr><th colspan='2'>" + tr("File") + "</th></tr>";
	text += " <tr><th>" + ctrlBadge + " + O</th><td>" + tr("Load settings...") + "</td></tr>";
	text +=
		" <tr><th>" + altBadge + " + O</th><td>" + tr("Load settings from clipboard...") + "</td></tr>";
	text += " <tr><th>" + ctrlBadge + " + " + altBadge + " + O</th><td>" + tr("Load example...")
					+ "</td></tr>";
	text += " <tr><th>" + ctrlBadge + " + S</th><td>" + tr("Save settings...") + "</td></tr>";
	text +=
		" <tr><th>" + altBadge + " + S</th><td>" + tr("Save settings to clipboard...") + "</td></tr>";
	text +=
		" <tr><th>" + ctrlBadge + " + I</th><td>" + tr("Import legacy settings...") + "</td></tr>";

	text += " <tr><th>" + ctrlBadge + " + M</th><td>" + tr("Export Mesh") + "</td></tr>";
	text += " <tr><th>" + ctrlBadge + " + " + altBadge + " + S</th><td>" + tr("Save as Image...")
					+ "</td></tr>";
	text += " <tr><th>" + ctrlBadge + " + P</th><td>" + tr("Program Preferences") + "</td></tr>";
	text += " <tr><th>" + ctrlBadge + " + Q</th><td>" + tr("Quit") + "</td></tr>";
	text += " <tr><th>F11</th><td>" + tr("Full screen") + "</td></tr>";
	text += "</table>";

	// Other
	text += "<table style='margin-top: 10px;' border='1'>";
	text += " <tr><th colspan='2'>" + tr("Other") + "</th></tr>";
	text += " <tr><th>" + ctrlBadge + " + Z</th><td>" + tr("Undo") + "</td></tr>";
	text += " <tr><th>" + ctrlBadge + " + Y</th><td>" + tr("Redo") + "</td></tr>";
	text += " <tr><th>" + ctrlBadge + " + H</th><td>" + tr("Show User Manual") + "</td></tr>";
	text += " <tr><th>" + altBadge + " + H</th><td>" + tr("Show (these) Hotkeys") + "</td></tr>";
	text +=
		" <tr><th>" + shiftBadge + " + ESC</th><td>" + tr("Terminate all calculations") + "</td></tr>";
	text += "</table>";
	text += "</td>";

	// Render window hotkeys
	text += "<td style='padding: 5px; background-color: #DFD'>";
	text += "<h3>" + tr("Render Window Hotkeys") + "</h3>";

	// Movement
	text += "<table border='1'>";
	text += " <tr><th colspan='2'>" + tr("Movement") + "</th></tr>";
	text +=
		" <tr><th>" + tr("Mouse left button click") + "</th><td>" + tr("jump forward") + "</td></tr>";
	text +=
		" <tr><th>" + tr("Mouse right button click") + "</th><td>" + tr("jump backward") + "</td></tr>";
	text += " <tr><th>" + tr("Mouse Wheel") + "</th><td>" + tr("forward / backward") + "</td></tr>";
	text += " <tr><th>" + tr("Mouse left+right buttons drag") + "</th><td>" + tr("Move camera")
					+ "</td></tr>";

	text += " <tr><th>W</th><td>" + tr("up") + "</td></tr>";
	text += " <tr><th>S</th><td>" + tr("down") + "</td></tr>";
	text += " <tr><th>A</th><td>" + tr("left") + "</td></tr>";
	text +=
		" <tr><th>" + shiftBadge + " + " + tr("Arrow left") + "</th><td>" + tr("left") + "</td></tr>";
	text += " <tr><th>D</th><td>" + tr("right") + "</td></tr>";
	text +=
		" <tr><th>" + shiftBadge + " + " + tr("Arrow right") + "</th><td>" + tr("right") + "</td></tr>";
	text += " <tr><th>Q</th><td>" + tr("forward") + "</td></tr>";
	text +=
		" <tr><th>" + ctrlBadge + " + " + tr("Arrow up") + "</th><td>" + tr("forward") + "</td></tr>";
	text +=
		" <tr><th>" + shiftBadge + " + " + tr("Arrow up") + "</th><td>" + tr("forward") + "</td></tr>";
	text += " <tr><th>Z</th><td>" + tr("backward") + "</td></tr>";
	text += " <tr><th>" + ctrlBadge + " + " + tr("Arrow down") + "</th><td>" + tr("backward")
					+ "</td></tr>";
	text += " <tr><th>" + shiftBadge + " + " + tr("Arrow down") + "</th><td>" + tr("backward")
					+ "</td></tr>";
	text += "</table>";

	// Rotation
	text += "<table style='margin-top: 10px;' border='1'>";
	text += " <tr><th colspan='2'>" + tr("Rotation") + "</th></tr>";
	text +=
		" <tr><th>" + tr("Mouse left button drag") + "</th><td>" + tr("Rotate camera") + "</td></tr>";
	text += " <tr><th>" + tr("Mouse right button drag") + "</th><td>"
					+ tr("Rotate camera around point") + "</td></tr>";
	text +=
		" <tr><th>" + tr("Mouse middle button drag") + "</th><td>" + tr("Roll camera") + "</td></tr>";
	text += " <tr><th>" + tr("Arrow up") + "</th><td>" + tr("up") + "</td></tr>";
	text += " <tr><th>" + tr("Arrow down") + "</th><td>" + tr("down") + "</td></tr>";
	text += " <tr><th>" + tr("Arrow left") + "</th><td>" + tr("left") + "</td></tr>";
	text += " <tr><th>" + tr("Arrow right") + "</th><td>" + tr("right") + "</td></tr>";
	text += " <tr><th>" + ctrlBadge + " + " + tr("Arrow left") + "</th><td>" + tr("Roll left")
					+ "</td></tr>";
	text += " <tr><th>" + ctrlBadge + " + " + tr("Arrow right") + "</th><td>" + tr("Roll right")
					+ "</td></tr>";
	text += "</table>";

	// Render Window, Keyframe, Flight
	text += "<td style='padding: 5px; background-color: #FDD'>";
	text += "<h3>" + tr("Render Window Keyframe / Flight") + "</h3>";

	// Flight
	text += "<table border='1'>";
	text += " <tr><th colspan='2'>" + tr("Flight") + "</th></tr>";
	text += " <tr><th>" + tr("Left mouse click") + "</th><td>" + tr("Increase speed") + "</td></tr>";
	text += " <tr><th>" + tr("Right mouse click") + "</th><td>" + tr("Decrease speed") + "</td></tr>";
	text +=
		" <tr><th>" + tr("Arrow keys") + "</th><td>" + tr("Move forward and sideward") + "</td></tr>";
	text += " <tr><th>" + shiftBadge + "+" + tr("Arrow keys") + "</th><td>" + tr("Move sideward only")
					+ "</td></tr>";
	text += " <tr><th>" + tr("Spacebar") + "</th><td>" + tr("Pause / Unpause") + "</td></tr>";
	text += " <tr><th>Z, X</th><td>" + tr("Roll rotation") + "</td></tr>";
	text += "</table>";

	// Keyframe
	text += "<table style='margin-top: 10px;' border='1'>";
	text += " <tr><th colspan='2'>" + tr("Keyframe") + "</th></tr>";
	text += " <tr><th>I</th><td>" + tr("Add Keyframe") + "</td></tr>";
	text += " <tr><th>M</th><td>" + tr("Modify current Keyframe") + "</td></tr>";
	text += " <tr><th>N</th><td>" + tr("Move to next Keyframe") + "</td></tr>";
	text += " <tr><th>P</th><td>" + tr("Move to previous Keyframe") + "</td></tr>";
	text += "</table>";

	text += "</td>";
	text += "</tr></table>";

	QLabel *label = new QLabel;
	label->setText(style + text);
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(label);
	QDialog *dialog = new QDialog(this);
	dialog->setLayout(layout);
	dialog->setWindowTitle(QObject::tr("HotKeys Cheatsheet"));
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
	QFile recentFilesFile(systemDirectories.GetRecentFilesListFile());
	QStringList recentFiles;
	if (recentFilesFile.open(QFile::ReadOnly | QFile::Text))
	{
		QTextStream in(&recentFilesFile);
		QString recentFilesFileContent = in.readAll();
		recentFilesFile.close();
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
		recentFiles = recentFilesFileContent.split(QRegExp("\n|\r\n|\r"), QString::KeepEmptyParts);
#else
		recentFiles =
			recentFilesFileContent.split(QRegularExpression("\n|\r\n|\r"), Qt::KeepEmptyParts);
#endif
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
	//	PreviewFileDialog dialog(this);
	//	dialog.setOption(QFileDialog::DontUseNativeDialog);
	//	dialog.setFileMode(QFileDialog::ExistingFile);
	//	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	//	dialog.setDirectory(QDir::toNativeSeparators(
	//		systemDirectories.sharedDir + QDir::separator() + "examples" + QDir::separator()));
	//	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).fileName()));
	//	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	//	dialog.setWindowTitle(tr("Load example settings..."));
	//	QStringList filenames;
	//	if (dialog.exec())
	//	{
	//		filenames = dialog.selectedFiles();
	//		QString filename = QDir::toNativeSeparators(filenames.first());
	//		slotMenuLoadSettingsFromFile(filename);
	//	}

	gMainInterface->SynchronizeInterface(
		gPar, gParFractal, qInterface::read); // update appParam before loading new settings

	cSettingsBrowser browser;
	browser.SetInitialFileName(QDir::toNativeSeparators(
		systemDirectories.sharedDir + QDir::separator() + "examples" + QDir::separator()));
	browser.exec();
	QString fileName = browser.getSelectedFileName();
	if (!fileName.isEmpty())
	{
		slotMenuLoadSettingsFromFile(fileName);
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
	gMainInterface->materialListModel->Regenerate();
	ui->widgetEffects->RegenerateLights();
	ui->widgetDockFractal->RegeneratePrimitives();

	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
	gInterfaceReadyForSynchronization = true;

	gMainInterface->ComboMouseClickUpdate();
	systemData.lastSettingsFile = fileName;
	SaveSettingsToRecent(fileName);
	setWindowTitle(QString("Mandelbulber (") + fileName + ")");
	gFlightAnimation->RefreshTable();
	gKeyframeAnimation->RefreshTable();
	showDescriptionPopup();
}

void RenderWindow::slotMenuLoadSettingsFromClipboard()
{
	gMainInterface->DisablePeriodicRefresh();

	gMainInterface->SynchronizeInterface(
		gPar, gParFractal, qInterface::read); // update appParam before loading new settings

	cSettings parSettings(cSettings::formatFullText);

	if (parSettings.LoadFromClipboard())
	{
		gMainInterface->DisablePeriodicRefresh();
		gInterfaceReadyForSynchronization = false;
		parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
		gMainInterface->materialListModel->Regenerate();
		ui->widgetEffects->RegenerateLights();
		ui->widgetDockFractal->RegeneratePrimitives();

		gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
		gInterfaceReadyForSynchronization = true;

		gMainInterface->ComboMouseClickUpdate();
		systemData.lastSettingsFile = "from clipboard";
		setWindowTitle(QString("Mandelbulber (") + "from clipboard" + ")");
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

void RenderWindow::slotMenuSettingsBrowser()
{
	gMainInterface->SynchronizeInterface(
		gPar, gParFractal, qInterface::read); // update appParam before loading new settings

	cSettingsBrowser browser;
	browser.SetInitialFileName(systemData.lastSettingsFile);
	browser.exec();
	QString fileName = browser.getSelectedFileName();
	if (!fileName.isEmpty())
	{
		slotMenuLoadSettingsFromFile(fileName);
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
	// restoreState(gMainInterface->settings.value("mainWindowState").toByteArray());
	ResetDocksPositions();
	ui->dockWidget_histogram->hide();
	ui->dockWidget_info->hide();
	ui->dockWidget_queue_dock->hide();
	ui->dockWidget_animation->hide();
	ui->dockWidget_measurement->hide();
	ui->dockWidget_gamepad_dock->hide();

	tabifyDockWidget(ui->dockWidget_materialEditor, ui->dockWidget_effects);
	tabifyDockWidget(ui->dockWidget_effects, ui->dockWidget_image_adjustments);
	tabifyDockWidget(ui->dockWidget_image_adjustments, ui->dockWidget_rendering_engine);
	tabifyDockWidget(ui->dockWidget_rendering_engine, ui->dockWidget_objects);
	tabifyDockWidget(ui->dockWidget_objects, ui->dockWidget_histogram);

	addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_Materials);
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

void RenderWindow::slotMenuSaveImageAll()
{
	cImageSaveDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	QStringList filters;
	filters << tr("images (*.jpg *.jpeg *.png *.exr *.tiff)");
	dialog.setNameFilters(filters);
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(systemData.GetImageFileNameSuggestion()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save image to file..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		ImageFileSave::enumImageFileType imageFileType =
			ImageFileSave::ImageFileType(QFileInfo(filename).suffix());
		gApplication->processEvents();
		SaveImage(filename, imageFileType, gMainInterface->mainImage, gMainInterface->mainWindow);
		gApplication->processEvents();
		systemData.lastImageFile = filename;

		if (gPar->Get<bool>("save_settings_with_image"))
		{
			QString fileWithoutExtension = ImageFileSave::ImageNameWithoutExtension(filename);
			cSettings parSettings(cSettings::formatCondensedText);
			gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);
			parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
			parSettings.SaveToFile(fileWithoutExtension + ".fract");
		}
	}
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

		if (gPar->Get<bool>("save_settings_with_image"))
		{
			QString fileWithoutExtension = ImageFileSave::ImageNameWithoutExtension(filename);
			cSettings parSettings(cSettings::formatCondensedText);
			gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);
			parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
			parSettings.SaveToFile(fileWithoutExtension + ".fract");
		}
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
		setWindowTitle(QString("Mandelbulber (") + filename + ")");

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

void RenderWindow::slotMenuRenderImage()
{
	gMainInterface->StartRender();
}

void RenderWindow::slotMenuStopRendering()
{
	gMainInterface->StopRender();
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

void RenderWindow::slotMenuRandomizeAll()
{
	gMainInterface->RandomizeLocalSettings(this);
}

void RenderWindow::slotCleanSettings()
{
	gMainInterface->CleanSettings();
}

void RenderWindow::slotSaveSettingsAsDefaut()
{
	cSettings parSettings(cSettings::formatCondensedText);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
	QString filename = QDir::toNativeSeparators(systemDirectories.GetDefaultSettingsFile());
	parSettings.SaveToFile(filename);
}

void RenderWindow::slotResetToDefault()
{
	QString filename = QDir::toNativeSeparators(systemDirectories.GetDefaultSettingsFile());
	if (QFile::exists(filename))
	{
		gMainInterface->SynchronizeInterface(
			gPar, gParFractal, qInterface::read); // update appParam before loading new settings
		slotMenuLoadSettingsFromFile(filename);
	}
	else
	{
		gPar->ResetAllToDefault();
		for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		{
			gParFractal->at(i)->ResetAllToDefault();
			gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
			gMainInterface->StartupDefaultSettings();
		}
	}
}
void RenderWindow::slotDeleteDefaultSettings()
{
	QString filename = QDir::toNativeSeparators(systemDirectories.GetDefaultSettingsFile());
	if (QFile::exists(filename))
	{
		QFile::remove(filename);
	}
}
