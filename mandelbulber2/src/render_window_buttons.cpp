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
#include "common_math.h"

void RenderWindow::slotPressedButtonAutoFog()
{
  gMainInterface->AutoFog();
}

void RenderWindow::slotPressedButtonDeletePrimitive()
{
  QString buttonName = this->sender()->objectName();
  QString primitiveName = buttonName.mid(buttonName.indexOf('_') + 1);
  gMainInterface->DeletePrimitive(primitiveName);
}

void RenderWindow::slotPressedButtonDOFUpdate()
{
  gMainInterface->RefreshMainImage();
}

void RenderWindow::slotPressedButtonGetJuliaConstant()
{
  QList<QVariant> item;
  item.append((int) RenderedImage::clickGetJuliaConstant);
  int index = ui->comboBox_mouse_click_function->findData(item);
  ui->comboBox_mouse_click_function->setCurrentIndex(index);
  gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonGetPoint()
{
  QList<QVariant> item;
  item.append((int) RenderedImage::clickGetPoint);
  int index = ui->comboBox_mouse_click_function->findData(item);
  ui->comboBox_mouse_click_function->setCurrentIndex(index);
  gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonImageApply()
{
  gMainInterface->RefreshMainImage();
}

void RenderWindow::slotPressedButtonNewPrimitive()
{
  QString buttonName = this->sender()->objectName();
  QString primitiveName = buttonName.mid(buttonName.lastIndexOf('_') + 1);
  gMainInterface->NewPrimitive(primitiveName);
}

void RenderWindow::slotPressedButtonNewRandomPalette()
{
  gMainInterface->SynchronizeInterfaceWindow(ui->groupCheck_fractal_color, gPar, cInterface::read);
  cColorPalette palette(gPar->Get<int>("coloring_palette_size"),
                        gPar->Get<int>("coloring_random_seed"),
                        gPar->Get<double>("coloring_saturation"));
  ui->colorpalette_surface_color_palette->SetPalette(palette);
}

void RenderWindow::slotPressedButtonOptimizeForLQ()
{
  gMainInterface->OptimizeStepFactor(1.0);
}

void RenderWindow::slotPressedButtonOptimizeForMQ()
{
  gMainInterface->OptimizeStepFactor(0.1);
}
void RenderWindow::slotPressedButtonOptimizeForHQ()
{
  gMainInterface->OptimizeStepFactor(0.01);
}

void RenderWindow::slotPressedButtonRandomize()
{
  srand((unsigned int) clock());
  int seed = Random(999999);
  ui->spinboxInt_coloring_random_seed->setValue(seed);
  slotPressedButtonNewRandomPalette();
}

void RenderWindow::slotPressedButtonResetFormula()
{
  QString comboName = this->sender()->objectName();
  int fractalNumber = comboName.right(1).toInt() - 1;
  gMainInterface->ResetFormula(fractalNumber);
}

void RenderWindow::slotPressedButtonResetView()
{
  gMainInterface->ResetView();
}

void RenderWindow::slotPressedButtonSetDOFByMouse()
{
  QList<QVariant> item;
  item.append((int) RenderedImage::clickDOFFocus);
  int index = ui->comboBox_mouse_click_function->findData(item);
  ui->comboBox_mouse_click_function->setCurrentIndex(index);
  gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetFogByMouse()
{
  QList<QVariant> item;
  item.append((int) RenderedImage::clickFogVisibility);
  int index = ui->comboBox_mouse_click_function->findData(item);
  ui->comboBox_mouse_click_function->setCurrentIndex(index);
  gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetLight1ByMouse()
{
  QList<QVariant> item;
  item.append((int) RenderedImage::clickPlaceLight);
  item.append(1); //light number
  int index = ui->comboBox_mouse_click_function->findData(item);
  ui->comboBox_mouse_click_function->setCurrentIndex(index);
  gMainInterface->renderedImage->setClickMode(item);
  double distance = gMainInterface->GetDistanceForPoint(gPar->Get<CVector3>("camera"));
  ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(distance * 0.1));
}

void RenderWindow::slotPressedButtonSetLight2ByMouse()
{
  QList<QVariant> item;
  item.append((int) RenderedImage::clickPlaceLight);
  item.append(2); //light number
  int index = ui->comboBox_mouse_click_function->findData(item);
  ui->comboBox_mouse_click_function->setCurrentIndex(index);
  gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetLight3ByMouse()
{
  QList<QVariant> item;
  item.append((int) RenderedImage::clickPlaceLight);
  item.append(3); //light number
  int index = ui->comboBox_mouse_click_function->findData(item);
  ui->comboBox_mouse_click_function->setCurrentIndex(index);
  gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetLight4ByMouse()
{
  QList<QVariant> item;
  item.append((int) RenderedImage::clickPlaceLight);
  item.append(4); //light number
  int index = ui->comboBox_mouse_click_function->findData(item);
  ui->comboBox_mouse_click_function->setCurrentIndex(index);
  gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonPlaceRandomLightsByMouse()
{
  QList<QVariant> item;
  item.append((int) RenderedImage::clickPlaceRandomLightCenter);
  int index = ui->comboBox_mouse_click_function->findData(item);
  ui->comboBox_mouse_click_function->setCurrentIndex(index);
  gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonSetPositionPrimitive()
{
  QString buttonName = this->sender()->objectName();
  QString primitiveName = buttonName.mid(buttonName.indexOf('_') + 1);
  QStringList split = primitiveName.split('_');
  QList<QVariant> item;
  item.append((int) RenderedImage::clickPlacePrimitive);
  item.append((int) PrimitiveNameToEnum(split.at(1)));
  item.append(split.at(2).toInt());
  item.append(primitiveName); //light number
  int index = ui->comboBox_mouse_click_function->findData(item);
  ui->comboBox_mouse_click_function->setCurrentIndex(index);
  gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonIFSDefaultsDodecahedron()
{
  int index = ui->tabWidget_fractals->currentIndex();
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
                                             &gParFractal->at(index),
                                             cInterface::read);
  gMainInterface->IFSDefaultsDodecahedron(&gParFractal->at(index));
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
                                             &gParFractal->at(index),
                                             cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsIcosahedron()
{
  int index = ui->tabWidget_fractals->currentIndex();
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
                                             &gParFractal->at(index),
                                             cInterface::read);
  gMainInterface->IFSDefaultsIcosahedron(&gParFractal->at(index));
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
                                             &gParFractal->at(index),
                                             cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsOctahedron()
{
  int index = ui->tabWidget_fractals->currentIndex();
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
                                             &gParFractal->at(index),
                                             cInterface::read);
  gMainInterface->IFSDefaultsOctahedron(&gParFractal->at(index));
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
                                             &gParFractal->at(index),
                                             cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsMengerSponge()
{
  int index = ui->tabWidget_fractals->currentIndex();
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
                                             &gParFractal->at(index),
                                             cInterface::read);
  gMainInterface->IFSDefaultsMengerSponge(&gParFractal->at(index));
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
                                             &gParFractal->at(index),
                                             cInterface::write);
}

void RenderWindow::slotPressedButtonIFSDefaultsReset()
{
  int index = ui->tabWidget_fractals->currentIndex();
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
                                             &gParFractal->at(index),
                                             cInterface::read);
  gMainInterface->IFSDefaultsReset(&gParFractal->at(index));
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[index],
                                             &gParFractal->at(index),
                                             cInterface::write);
}

void RenderWindow::slotPressedImagesizeIncrease()
{
  int width = ui->spinboxInt_image_width->value();
  int height = ui->spinboxInt_image_height->value();
  width *= 2;
  height *= 2;
  ui->spinboxInt_image_width->setValue(width);
  ui->spinboxInt_image_height->setValue(height);
}

void RenderWindow::slotPressedImagesizeDecrease()
{
  int width = ui->spinboxInt_image_width->value();
  int height = ui->spinboxInt_image_height->value();
  width /= 2;
  height /= 2;
  ui->spinboxInt_image_width->setValue(width);
  ui->spinboxInt_image_height->setValue(height);
}

