/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * RenderWindow class - main program window
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
*/

#include "render_window.hpp"
#include "global_data.hpp"
#include "settings.hpp"
#include "old_settings.hpp"
#include "preview_file_dialog.h"
#include "file_image.hpp"
#include "interface.hpp"
#include "animation_flight.hpp"
#include "animation_keyframes.hpp"
#include "initparameters.hpp"
#include "files.h"

void RenderWindow::slotImportOldSettings()
{
  QFileDialog dialog(this);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
  dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
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
    gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
    gMainInterface->ComboMouseClickUpdate();
    systemData.lastSettingsFile = filename;
    this->setWindowTitle(QString("Mandelbulber (") + filename + ")");
  }
}

void RenderWindow::slotMenuAboutMandelbulber()
{
  QString text = "<h2>Mandelbulber</h2>";
  text += "version: <b>" + QString(MANDELBULBER_VERSION_STRING) + "</b>"
      + "<br>";
  text += "<br>";
  text += "Licence: GNU GPL version 3.0<br>";
  text += "Copyright Ⓒ 2015<br>";
  text += "project leader: Krzysztof Marczak<br>";
  text += "programmers:<br>";
  text += "Krzysztof Marczak<br>";
  text += "Sebastian Jennen<br>";
  text += "Graeme McLaren<br>";
  text += "Bernardo Martelli<br>";
  text += "<br>";
  text +=
      "Thanks to many friends from <a href=\"http://www.fractalforums.com\">www.fractalforums.com</a> for help<br>";
  text += "<br>";
  text += "<a href=\"http://www.mandelbulber.com\">www.mandelbulber.com</a>";

  QMessageBox::about(this, "About Mandelbulber", text);
}

void RenderWindow::slotMenuAboutQt()
{
  QMessageBox::aboutQt(this);
}

void RenderWindow::slotMenuAboutThirdParty()
{
  QString text = "<h2>Third Party</h2>";
  text += "<ul>";
	text += " <li><b>Dark Skin</b> based on <br>";
	text += "		<a href=\"https://gist.github.com/QuantumCD/6245215\">gist.github.com/QuantumCD/6245215</a><br>thanks to QuantumCD</li><br>";
	text += " <li><b>Code highlighting in ui files</b><br>generated with<br>";
	text += "		<a href=\"http://www.andre-simon.de/doku/highlight/highlight.php\">highlight</a><br>thanks to André Simon</li>";
	text += "</ul>";
  QMessageBox::about(this, "About Third Party", text);
}

void RenderWindow::slotMenuLoadExample()
{
  cSettings parSettings(cSettings::formatFullText);

  PreviewFileDialog dialog(this);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
  dialog.setDirectory(QDir::toNativeSeparators(systemData.sharedDir + QDir::separator() + "examples" + QDir::separator()));
  dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).fileName()));
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setWindowTitle(tr("Load example settings..."));
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    parSettings.LoadFromFile(filename);
    parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
    gMainInterface->RebuildPrimitives(gPar);
    gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
    gMainInterface->ComboMouseClickUpdate();
    systemData.lastSettingsFile = systemData.dataDirectory + "settings" + QDir::separator()
        + QFileInfo(filename).fileName();
    this->setWindowTitle(QString("Mandelbulber (") + systemData.lastSettingsFile + ")");
    gFlightAnimation->RefreshTable();
    gKeyframeAnimation->RefreshTable();
  }
}

void RenderWindow::slotMenuLoadSettings()
{
  gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read); //update appParam before loading new settings

  cSettings parSettings(cSettings::formatFullText);

  PreviewFileDialog dialog(this);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
  dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
  dialog.selectFile(QDir::toNativeSeparators(systemData.lastSettingsFile));
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setWindowTitle(tr("Load settings..."));
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    parSettings.LoadFromFile(filename);
    parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
    gMainInterface->RebuildPrimitives(gPar);
    gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
    gMainInterface->ComboMouseClickUpdate();
    systemData.lastSettingsFile = filename;
    this->setWindowTitle(QString("Mandelbulber (") + filename + ")");
    gFlightAnimation->RefreshTable();
    gKeyframeAnimation->RefreshTable();
  }
}

void RenderWindow::slotMenuLoadSettingsFromClipboard()
{
  gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read); //update appParam before loading new settings

  cSettings parSettings(cSettings::formatFullText);

  if (parSettings.LoadFromClipboard())
  {
    parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
    gMainInterface->RebuildPrimitives(gPar);
    gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
    gMainInterface->ComboMouseClickUpdate();
    systemData.lastSettingsFile = "from clipboard";
    this->setWindowTitle(QString("Mandelbulber (") + "from clipboard" + ")");
    gFlightAnimation->RefreshTable();
    gKeyframeAnimation->RefreshTable();
  }
  else
  {
    cErrorMessage::showMessage(QObject::tr("Cannot load settings from clipboard!"),
                               cErrorMessage::errorMessage,
                               gMainInterface->mainWindow);
  }
}

void RenderWindow::slotMenuRedo()
{
  gMainInterface->Redo();
}

void RenderWindow::slotMenuResetDocksPositions()
{
  restoreGeometry(defaultGeometry);
  restoreState(defaultState);
  ui->dockWidget_histogram->hide();
  ui->dockWidget_info->hide();
  ui->dockWidget_animation->hide();
  ui->dockWidget_gamepad_dock->hide();
  ui->dockWidget_queue_dock->hide();
}

void RenderWindow::slotMenuSaveDocksPositions()
{
  settings.setValue("mainWindowGeometry", saveGeometry());
  settings.setValue("mainWindowState", saveState());
  //qDebug() << "settings saved";
}

void RenderWindow::slotMenuSaveImageJPEG()
{
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("JPEG images (*.jpg *.jpeg)"));
  dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
  dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle(tr("Save image to %1 file...").arg("JPEG"));
  dialog.setDefaultSuffix("jpeg");
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("JPG"),
                                      tr("Saving image started"),
                                      0.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
		SaveImage(filename, ImageFileSave::IMAGE_FILE_TYPE_JPG, gMainInterface->mainImage);
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("JPG"),
                                      tr("Saving image finished"),
                                      1.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
    systemData.lastImageFile = filename;
  }
}

void RenderWindow::slotMenuSaveImagePNG()
{
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("PNG images (*.png)"));
  dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
  dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle(tr("Save image to %1 file...").arg("8-bit PNG"));
  dialog.setDefaultSuffix("png");
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("PNG"),
                                      tr("Saving image started"),
                                      0.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
		SaveImage(filename, ImageFileSave::IMAGE_FILE_TYPE_PNG, gMainInterface->mainImage);
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("PNG"),
                                      tr("Saving image finished"),
                                      1.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
    systemData.lastImageFile = filename;
  }
}

#ifdef USE_EXR
void RenderWindow::slotMenuSaveImageEXR()
{
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("EXR images (*.exr)"));
  dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
  dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle(tr("Save image to %1 file...").arg("EXR"));
  dialog.setDefaultSuffix("exr");
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("EXR"),
                                      tr("Saving EXR image started"),
                                      0.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
		SaveImage(filename, ImageFileSave::IMAGE_FILE_TYPE_EXR, gMainInterface->mainImage);
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("EXR"),
                                      tr("Saving EXR image finished"),
                                      1.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
    systemData.lastImageFile = filename;
  }
}
#endif // USE_EXR

#ifdef USE_TIFF
void RenderWindow::slotMenuSaveImageTIFF()
{
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("TIFF images (*.tiff)"));
  dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
  dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle(tr("Save image to %1 file...").arg("TIFF"));
  dialog.setDefaultSuffix("tiff");
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("TIFF"),
                                      tr("Saving TIFF image started"),
                                      0.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
		SaveImage(filename, ImageFileSave::IMAGE_FILE_TYPE_TIFF, gMainInterface->mainImage);
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("TIFF"),
                                      tr("Saving TIFF image finished"),
                                      1.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
    systemData.lastImageFile = filename;
  }
}
#endif // USE_TIFF

void RenderWindow::slotMenuSaveImagePNG16()
{
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("PNG images (*.png)"));
  dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).absolutePath()));
  dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle(tr("Save image to %1 file...").arg("16-bit PNG"));
  dialog.setDefaultSuffix("png");
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG"),
                                      tr("Saving image started"),
                                      0.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
		ImageFileSave::structSaveImageChannel saveImageChannel(
			ImageFileSave::IMAGE_CONTENT_COLOR, ImageFileSave::IMAGE_CHANNEL_QUALITY_16, "");
		ImageFileSavePNG::SavePNG(filename, gMainInterface->mainImage, saveImageChannel, false);
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG"),
                                      tr("Saving image finished"),
                                      1.0,
                                      cProgressText::progress_IMAGE);
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
  dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastImageFile).completeBaseName()));
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle(tr("Save image to %1 file...").arg("16-bit PNG + alpha channel"));
  dialog.setDefaultSuffix("png");
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG + alpha channel"),
                                      tr("Saving image started"),
                                      0.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
		ImageFileSave::structSaveImageChannel saveImageChannel(
			ImageFileSave::IMAGE_CONTENT_COLOR, ImageFileSave::IMAGE_CHANNEL_QUALITY_16, "");
		ImageFileSavePNG::SavePNG(filename, gMainInterface->mainImage, saveImageChannel, true);
    cProgressText::ProgressStatusText(tr("Saving %1 image").arg("16-bit PNG + alpha channel"),
                                      tr("Saving image finished"),
                                      1.0,
                                      cProgressText::progress_IMAGE);
    gApplication->processEvents();
    systemData.lastImageFile = filename;
  }
}

void RenderWindow::slotMenuSaveSettings()
{
  cSettings parSettings(cSettings::formatCondensedText);
  gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
  parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);

  QFileDialog dialog(this);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
  dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
  dialog.selectFile(QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).completeBaseName()));
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
    this->setWindowTitle(QString("Mandelbulber (") + filename + ")");

    //QString hash = parSettings.GetHashCode();
    //cThumbnail thumbnail(gPar, gParFractal, 200, 200);
    //thumbnail.Render();
    //thumbnail.Save(systemData.thumbnailDir + hash);
  }
}

void RenderWindow::slotMenuSaveSettingsToClipboard()
{
  cSettings parSettings(cSettings::formatCondensedText);
  gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
  parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
  parSettings.SaveToClipboard();
  cErrorMessage::showMessage(QObject::tr("Settings saved to clipboard"),
                             cErrorMessage::infoMessage);
}

void RenderWindow::slotMenuUndo()
{
  gMainInterface->Undo();
}

void RenderWindow::slotUpdateDocksandToolbarbyAction()
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

void RenderWindow::slotUpdateDocksandToolbarbyView()
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
  tabifyDockWidget(ui->dockWidget_effects, ui->dockWidget_image_adjustments);
  tabifyDockWidget(ui->dockWidget_image_adjustments, ui->dockWidget_rendering_engine);
  tabifyDockWidget(ui->dockWidget_rendering_engine, ui->dockWidget_fractal);
  tabifyDockWidget(ui->dockWidget_fractal, ui->dockWidget_histogram);
  tabifyDockWidget(ui->dockWidget_histogram, ui->dockWidget_info);
}

