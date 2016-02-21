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
#include "preview_file_dialog.h"

void RenderWindow::slotPressedButtonSelectBackgroundTexture()
{
  PreviewFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
  dialog.setDirectory(QDir::toNativeSeparators(systemData.dataDirectory + QDir::separator()
      + "textures"
      + QDir::separator()));
  dialog.selectFile(QDir::toNativeSeparators(gPar->Get<QString>("file_background")));
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setWindowTitle(tr("Select background texture..."));
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    gPar->Set("file_background", filename);
    ui->text_file_background->setText(filename);
    gMainInterface->ShowImageInLabel(ui->label_backgroundTextureView, filename);
  }
}

void RenderWindow::slotPressedButtonSelectEnvMapTexture()
{
  PreviewFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
  dialog.setDirectory(QDir::toNativeSeparators(systemData.dataDirectory + QDir::separator()
      + "textures"
      + QDir::separator()));
  dialog.selectFile(QDir::toNativeSeparators(gPar->Get<QString>("file_envmap")));
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setWindowTitle(tr("Select texture for environment mapping effect..."));
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    gPar->Set("file_envmap", filename);
    ui->text_file_envmap->setText(filename);
    gMainInterface->ShowImageInLabel(ui->label_envmapTextureView, filename);
  }
}

void RenderWindow::slotPressedButtonSelectLightMapTexture()
{
  PreviewFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
  dialog.setDirectory(QDir::toNativeSeparators(systemData.dataDirectory + QDir::separator() + "textures"
      + QDir::separator()));
  dialog.selectFile(QDir::toNativeSeparators(gPar->Get<QString>("file_lightmap")));
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setWindowTitle(tr("Select texture for ambient occlusion light map..."));
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    gPar->Set("file_lightmap", filename);
    ui->text_file_lightmap->setText(filename);
    gMainInterface->ShowImageInLabel(ui->label_lightmapTextureView, filename);
  }
}

void RenderWindow::slotPressedButtonGetPaletteFromImage()
{
  PreviewFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
  dialog.setDirectory(QDir::toNativeSeparators(systemData.dataDirectory + QDir::separator() + "textures"
      + QDir::separator()));
  dialog.selectFile(QDir::toNativeSeparators(systemData.lastImagePaletteFile));
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setWindowTitle(tr("Select image to grab colors..."));
  QStringList filenames;
  if (dialog.exec())
  {
    filenames = dialog.selectedFiles();
    QString filename = QDir::toNativeSeparators(filenames.first());
    cColorPalette palette = gMainInterface->GetPaletteFromImage(filename);
    ui->colorpalette_surface_color_palette->SetPalette(palette);
    systemData.lastImagePaletteFile = filename;
  }
}

void RenderWindow::slotEditedLineEditBackgroundTexture(const QString &text)
{
  gMainInterface->ShowImageInLabel(ui->label_backgroundTextureView, text);
}

void RenderWindow::slotEditedLineEditEnvMapTexture(const QString &text)
{
  gMainInterface->ShowImageInLabel(ui->label_envmapTextureView, text);
}

void RenderWindow::slotEditedLineEditLightMapTexture(const QString &text)
{
  gMainInterface->ShowImageInLabel(ui->label_lightmapTextureView, text);
}




