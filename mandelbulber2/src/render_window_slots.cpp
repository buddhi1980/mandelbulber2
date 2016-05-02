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
#include "my_ui_loader.h"
#include "initparameters.hpp"
#include "settings.hpp"
#include "../qt/preferencesdialog.h"
#include "../qt/volume_slice_dialog.h"
#include "interface.hpp"
#include "error_message.hpp"
#include "image_scale.hpp"

void RenderWindow::slotStartRender(void)
{
	gMainInterface->StartRender();
}

void RenderWindow::slotStopRender(void)
{
  gMainInterface->stopRequest = true;
}

void RenderWindow::slotChangedComboFractal(int indexInComboBox)
{
  QString comboName = this->sender()->objectName();
  int index = qobject_cast<QComboBox *>(this->sender())->itemData(indexInComboBox).toInt();
  int fractalNumber = comboName.right(1).toInt() - 1;

  QString fullFormulaName = fractalList[index].nameInComboBox;
  if (fractalList[index].internalID > 0)
  {
    QString formulaName = fractalList[index].internalName;
    QString uiFilename = systemData.sharedDir + "qt_data" + QDir::separator() + "fractal_"
        + formulaName + ".ui";

    if (fractalWidgets[fractalNumber]) delete fractalWidgets[fractalNumber];
    fractalWidgets[fractalNumber] = NULL;

    MyUiLoader loader;
    QFile uiFile(uiFilename);

    if (uiFile.exists())
    {
      uiFile.open(QFile::ReadOnly);
      fractalWidgets[fractalNumber] = loader.load(&uiFile);
      QVBoxLayout *layout =
          ui->dockWidget_fractal->findChild<QVBoxLayout*>("verticalLayout_fractal_"
              + QString::number(fractalNumber + 1));
      layout->addWidget(fractalWidgets[fractalNumber]);
      uiFile.close();
      fractalWidgets[fractalNumber]->show();
      gMainInterface->ConnectSignalsForSlidersInWindow(fractalWidgets[fractalNumber]);
      gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[fractalNumber],
                                                 &gParFractal->at(fractalNumber),
                                                 cInterface::write);

      QFrame *frame = ui->tabWidget_fractals->findChild<QFrame*>("frame_iterations_formula_"
          + QString::number(fractalNumber + 1));
      MyCheckBox *caddCheckBoxframe =
          frame->findChild<MyCheckBox*>(QString("checkBox_dont_add_c_constant_")
              + QString::number(fractalNumber + 1));

      switch(fractalList[index].cpixelAddition)
      {
        case fractal::cpixelEnabledByDefault:
          caddCheckBoxframe->setText(QObject::tr("Don't add global C constant"));
          caddCheckBoxframe->setEnabled(true);
          break;

        case fractal::cpixelDisabledByDefault:
        {
          caddCheckBoxframe->setText(QObject::tr("Add global C constant"));
          caddCheckBoxframe->setEnabled(true);
          break;
        }

        case fractal::cpixelAlreadyHas:
        {
          caddCheckBoxframe->setText(QObject::tr("Don't add global C constant"));
          caddCheckBoxframe->setEnabled(false);
          break;
        }
      }

      fractal::enumCPixelAddition cPixelAddition = fractalList[index].cpixelAddition;
      bool boleanState = ui->groupCheck_boolean_operators->isChecked();
      if (cPixelAddition == fractal::cpixelAlreadyHas)
        ui->tabWidget_fractals->findChild<QGroupBox*>("groupBox_c_constant_addition_"
            + QString::number(fractalNumber + 1))->setVisible(false);
      else ui->tabWidget_fractals->findChild<QGroupBox*>("groupBox_c_constant_addition_"
          + QString::number(fractalNumber + 1))->setVisible(boleanState);

      if (fractalList[index].internalID == fractal::kaleidoscopicIFS)
      {
        QWidget *pushButton_preset_dodecahedron =
            fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_dodecahedron");
        QApplication::connect(pushButton_preset_dodecahedron,
                              SIGNAL(clicked()),
                              this,
                              SLOT(slotPressedButtonIFSDefaultsDodecahedron()));
        QWidget *pushButton_preset_icosahedron =
            fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_icosahedron");
        QApplication::connect(pushButton_preset_icosahedron,
                              SIGNAL(clicked()),
                              this,
                              SLOT(slotPressedButtonIFSDefaultsIcosahedron()));
        QWidget *pushButton_preset_octahedron =
            fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_octahedron");
        QApplication::connect(pushButton_preset_octahedron,
                              SIGNAL(clicked()),
                              this,
                              SLOT(slotPressedButtonIFSDefaultsOctahedron()));
        QWidget *pushButton_preset_menger_sponge =
            fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_menger_sponge");
        QApplication::connect(pushButton_preset_menger_sponge,
                              SIGNAL(clicked()),
                              this,
                              SLOT(slotPressedButtonIFSDefaultsMengerSponge()));
        QWidget *pushButton_preset_reset =
            fractalWidgets[fractalNumber]->findChild<QWidget*>("pushButton_preset_reset");
        QApplication::connect(pushButton_preset_reset,
                              SIGNAL(clicked()),
                              this,
                              SLOT(slotPressedButtonIFSDefaultsReset()));
      }
    }
    else
    {
      cErrorMessage::showMessage(QString("Can't open file ") + uiFilename
                                     + QString("\nFractal ui file can't be loaded"),
                                 cErrorMessage::errorMessage,
                                 gMainInterface->mainWindow);
    }
  }
  else
  {
    if (fractalWidgets[fractalNumber]) delete fractalWidgets[fractalNumber];
    fractalWidgets[fractalNumber] = NULL;
  }
  ui->tabWidget_fractals->setTabText(fractalNumber, QString("#%1: %2").arg(fractalNumber + 1).arg(fullFormulaName));
  // ui->tabWidget_fractals->setTabIcon(fractalNumber, QIcon(fractalList[index].getIconName()));
}

void RenderWindow::slotResizedScrolledAreaImage(int width, int height)
{
  if (gMainInterface->mainImage)
  {
    int selectedScale = ui->comboBox_image_preview_scale->currentIndex();

    if (selectedScale == 0)
    {
      double scale = CalcMainImageScale(0.0, width, height, gMainInterface->mainImage);
      gMainInterface->mainImage->CreatePreview(scale, width, height, gMainInterface->renderedImage);
      gMainInterface->mainImage->UpdatePreview();
      gMainInterface->renderedImage->setMinimumSize(gMainInterface->mainImage->GetPreviewWidth(),
                                                    gMainInterface->mainImage->GetPreviewHeight());
    }
  }
}

void RenderWindow::slotChangedComboImageScale(int index)
{
  if (gMainInterface->mainImage)
  {
    double scale = ImageScaleComboSelection2Double(index);
    int areaWidth = ui->scrollAreaForImage->VisibleAreaWidth();
    int areaHeight = ui->scrollAreaForImage->VisibleAreaHeight();
    scale = CalcMainImageScale(scale, areaWidth, areaHeight, gMainInterface->mainImage);

    gMainInterface->mainImage->CreatePreview(scale,
                                             areaWidth,
                                             areaHeight,
                                             gMainInterface->renderedImage);
    gMainInterface->mainImage->UpdatePreview();
    gMainInterface->renderedImage->setMinimumSize(gMainInterface->mainImage->GetPreviewWidth(),
                                                  gMainInterface->mainImage->GetPreviewHeight());

    gPar->Set("image_preview_scale", index);
  }
}

void RenderWindow::slotChangedCheckBoxHybridFractal(int state)
{
  if (state) ui->groupCheck_boolean_operators->setChecked(false);
  gApplication->processEvents();

  for(int i = 1; i <= NUMBER_OF_FRACTALS; i++)
  {
    QFrame *frame = ui->tabWidget_fractals->findChild<QFrame*>("frame_iterations_formula_" + QString::number(i));

    frame->findChild<QLabel*>(QString("label_formula_iterations_") + QString::number(i))->setVisible(state);
    frame->findChild<MySpinBox*>(QString("spinboxInt_formula_iterations_") + QString::number(i))->setVisible(state);
    frame->findChild<QSlider*>(QString("sliderInt_formula_iterations_") + QString::number(i))->setVisible(state);

    frame->findChild<QLabel*>(QString("label_formula_weight_") + QString::number(i))->setVisible(state);
    frame->findChild<MyDoubleSpinBox*>(QString("spinbox_formula_weight_") + QString::number(i))->setVisible(state);
    frame->findChild<QSlider*>(QString("slider_formula_weight_") + QString::number(i))->setVisible(state);

    frame->findChild<QLabel*>(QString("label_formula_start_iteration_") + QString::number(i))->setVisible(state);
    frame->findChild<QLabel*>(QString("label_formula_stop_iteration_") + QString::number(i))->setVisible(state);
    frame->findChild<MySpinBox*>(QString("spinboxInt_formula_start_iteration_") + QString::number(i))->setVisible(state);
    frame->findChild<MySpinBox*>(QString("spinboxInt_formula_stop_iteration_") + QString::number(i))->setVisible(state);

    frame->findChild<MyCheckBox*>(QString("checkBox_check_for_bailout_") + QString::number(i))->setVisible(state);
    //frame->findChild<MyCheckBox*>(QString("checkBox_dont_add_c_constant_") + QString::number(i))->setVisible(state);

    if(i > 1)
    {
      frame->setEnabled(state);
      ui->tabWidget_fractals->findChild<QScrollArea*>("scrollArea_fractal_" + QString::number(i))->setEnabled(state);
    }
  }

  ui->comboBox_delta_DE_function->setEnabled(state || ui->comboBox_delta_DE_method->currentIndex() == (int)fractal::forceDeltaDEMethod);

  ui->label_fractals_remark_hybrid->setVisible(!state);
  ui->label_repeat_from->setEnabled(state);
  ui->sliderInt_repeat_from->setEnabled(state);
  ui->spinboxInt_repeat_from->setEnabled(state);

  //ui->comboBox_delta_DE_method->setEnabled(!state);
}

void RenderWindow::slotChangedCheckBoxBooleanOperators(bool state)
{
  if (state) ui->checkBox_hybrid_fractal_enable->setChecked(false);
  gApplication->processEvents();

  for(int i = 1; i <= NUMBER_OF_FRACTALS; i++)
  {
    QFrame *frame = ui->tabWidget_fractals->findChild<QFrame*>("frame_iterations_formula_" + QString::number(i));
    if(i > 1)
    {
      frame->setEnabled(state);
      ui->tabWidget_fractals->findChild<QScrollArea*>("scrollArea_fractal_" + QString::number(i))->setEnabled(state);
    }
    ui->tabWidget_fractals->findChild<QGroupBox*>("groupBox_formula_transform_" + QString::number(i))->setVisible(state);

    QComboBox *comboBox = ui->tabWidget_fractals->findChild<QComboBox*>("comboBox_formula_"
        + QString::number(i));
    fractal::enumCPixelAddition cPixelAddition = fractalList[comboBox->itemData(comboBox
        ->currentIndex()).toInt()].cpixelAddition;

    if (cPixelAddition == fractal::cpixelAlreadyHas)
      ui->tabWidget_fractals->findChild<QGroupBox*>("groupBox_c_constant_addition_"
          + QString::number(i))->setVisible(false);
    else ui->tabWidget_fractals->findChild<QGroupBox*>("groupBox_c_constant_addition_"
        + QString::number(i))->setVisible(state);
  }

  ui->comboBox_delta_DE_function->setEnabled(!state);
}

void RenderWindow::slotChangedCheckBoxJuliaMode(bool state)
{
  ui->label_fractals_remark_julia->setVisible(state);
}

void RenderWindow::slotChangedComboDistanceEstimationMethod(int index)
{
  ui->comboBox_delta_DE_function->setEnabled(ui->checkBox_hybrid_fractal_enable->isChecked() || index == (int)fractal::forceDeltaDEMethod);
}

void RenderWindow::slotChangedComboAmbientOcclusionMode(int index)
{
  bool enabled = index == params::AOmodeMultipeRays ? true : false;
  ui->frame_lightmap_texture->setEnabled(enabled);
  enabled = index == params::AOmodeFast ? true : false;
  ui->slider_ambient_occlusion_fast_tune->setEnabled(enabled);
  ui->spinbox_ambient_occlusion_fast_tune->setEnabled(enabled);
}

void RenderWindow::slotChangedComboImageProportion(int index)
{
  bool enableSlider = false;
  double ratio = 1.0;
  enumImageProportion proportionSelection = (enumImageProportion) index;

  switch (proportionSelection)
  {
    case proportionFree:
      enableSlider = true;
      break;
    case proportion1_1:
      ratio = 1.0;
      break;
    case proportion4_3:
      ratio = 4.0 / 3.0;
      break;
    case proportion3_2:
      ratio = 3.0 / 2.0;
      break;
    case proportion16_9:
      ratio = 16.0 / 9.0;
      break;
    case proportion16_10:
      ratio = 16.0 / 10.0;
      break;
    case proportion2_1:
      ratio = 2.0 / 1.0;
      break;
    default:
      ratio = 1.0;
      break;
  }

  ui->sliderInt_image_width->setEnabled(enableSlider);
  ui->spinboxInt_image_width->setEnabled(enableSlider);

  int height = ui->spinboxInt_image_height->value();
  int width = height * ratio;

  if (!enableSlider)
  {
    ui->spinboxInt_image_width->setValue(width);
  }
}

void RenderWindow::slotPressedResolutionPreset()
{
  int width = 0, height = 0;
  enumImageProportion proportion = proportionFree;
  QString buttonName = this->sender()->objectName();
  if (buttonName == QString("pushButton_resolution_preset_480"))
  {
    width = 720;
    height = 480;
    proportion = proportion3_2;
  }
  else if (buttonName == QString("pushButton_resolution_preset_720"))
  {
    width = 1280;
    height = 720;
    proportion = proportion16_9;
  }
  else if (buttonName == QString("pushButton_resolution_preset_1080"))
  {
    width = 1920;
    height = 1080;
    proportion = proportion16_9;
  }
  else if (buttonName == QString("pushButton_resolution_preset_1440"))
  {
    width = 2560;
    height = 1440;
    proportion = proportion16_9;
  }
  else if (buttonName == QString("pushButton_resolution_preset_2160"))
  {
    width = 4096;
    height = 2160;
    proportion = proportionFree;
  }
  else if (buttonName == QString("pushButton_resolution_preset_4320"))
  {
    width = 7680;
    height = 4320;
    proportion = proportion16_9;
  }
  else if (buttonName == QString("pushButton_resolution_preset_240"))
  {
    width = 320;
    height = 240;
    proportion = proportion4_3;
  }
  else if (buttonName == QString("pushButton_resolution_preset_600"))
  {
    width = 800;
    height = 600;
    proportion = proportion4_3;
  }
  else if (buttonName == QString("pushButton_resolution_preset_1200"))
  {
    width = 1600;
    height = 1200;
    proportion = proportion4_3;
  }

  ui->spinboxInt_image_width->setValue(width);
  ui->spinboxInt_image_height->setValue(height);
  ui->comboBox_image_proportion->setCurrentIndex(proportion);
}



void RenderWindow::slotImageHeightChanged(int value)
{
  (void) value;
  int index = ui->comboBox_image_proportion->currentIndex();
  slotChangedComboImageProportion(index);
}

void RenderWindow::slotChangedComboPerspectiveType(int index)
{
  params::enumPerspectiveType perspType = (params::enumPerspectiveType) index;
  if (perspType == params::perspFishEyeCut)
  {
    ui->comboBox_image_proportion->setCurrentIndex(proportion1_1);
    ui->spinbox_fov->setValue(1.0);
  }
  else if (perspType == params::perspEquirectangular)
  {
    ui->comboBox_image_proportion->setCurrentIndex(proportion2_1);
    ui->spinbox_fov->setValue(1.0);
  }
}

void RenderWindow::slotChangedSpinBoxPaletteOffset(double value)
{
  ui->colorpalette_surface_color_palette->SetOffset(value);
}

void RenderWindow::slotChangedSpinBoxPaletteSize(int value)
{
  ui->slider_coloring_palette_offset->setMaximum(value * 100);
}

void RenderWindow::slotMouseMovedOnImage(int x, int y)
{
  (void) x;
  (void) y;
  //qDebug() << "mouse move event";
  //CVector2<int> point(x, y);
}

void RenderWindow::slotMouseClickOnImage(int x, int y, Qt::MouseButton button)
{
  int index = ui->comboBox_mouse_click_function->currentIndex();
  QList<QVariant> mode = ui->comboBox_mouse_click_function->itemData(index).toList();
  RenderedImage::enumClickMode clickMode = (RenderedImage::enumClickMode) mode.at(0).toInt();

  switch (clickMode)
  {
    case RenderedImage::clickMoveCamera:
    case RenderedImage::clickFogVisibility:
    case RenderedImage::clickDOFFocus:
    case RenderedImage::clickPlaceLight:
    case RenderedImage::clickGetJuliaConstant:
    case RenderedImage::clickPlacePrimitive:
    case RenderedImage::clickPlaceRandomLightCenter:
    case RenderedImage::clickGetPoint:
    {
      gMainInterface->SetByMouse(CVector2<double>(x, y), button, mode);
      break;
    }
    case RenderedImage::clickDoNothing:
    case RenderedImage::clickFlightSpeedControl:
      //nothing
      break;
  }

}

void RenderWindow::slotChangedComboMouseClickFunction(int index)
{
  if(index >= 0) //if list is empty, then index = -1
  {
    QComboBox *comboBox = static_cast<QComboBox*>(this->sender());
    QList<QVariant> item = comboBox->itemData(index).toList();
    gMainInterface->renderedImage->setClickMode(item);
  }
}

void RenderWindow::slotKeyPressOnImage(Qt::Key key)
{

  switch (key)
  {
    case Qt::Key_W:
      gMainInterface->MoveCamera("bu_move_up");
      break;
    case Qt::Key_S:
      gMainInterface->MoveCamera("bu_move_down");
      break;
    case Qt::Key_A:
      gMainInterface->MoveCamera("bu_move_left");
      break;
    case Qt::Key_D:
      gMainInterface->MoveCamera("bu_move_right");
      break;
    case Qt::Key_Up:
      gMainInterface->MoveCamera("bu_move_forward");
      break;
    case Qt::Key_Down:
      gMainInterface->MoveCamera("bu_move_backward");
      break;
    default:
      break;
  }
}

void RenderWindow::slotKeyReleaseOnImage(Qt::Key key)
{
  (void) key;
}

void RenderWindow::slotMouseWheelRotatedOnImage(int delta)
{
  int index = ui->comboBox_mouse_click_function->currentIndex();
  QList<QVariant> mode = ui->comboBox_mouse_click_function->itemData(index).toList();
  RenderedImage::enumClickMode clickMode = (RenderedImage::enumClickMode) mode.at(0).toInt();
  switch (clickMode)
  {
    case RenderedImage::clickPlaceLight:
    {
      double deltaLog = exp(delta * 0.001);
      double dist = systemData.locale.toDouble(ui->logedit_aux_light_manual_placement_dist->text());
      dist *= deltaLog;
      ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(dist));
      break;
    }
    default:
      break;
  }
}

void RenderWindow::slotEditedLineEditManualLightPlacementDistance(const QString &text)
{
  gMainInterface->renderedImage->SetFrontDist(systemData.locale.toDouble(text));
}

void RenderWindow::slotSliderMovedEditManualLightPlacementDistance(int value)
{
  gMainInterface->renderedImage->SetFrontDist(pow(10.0, value / 100.0));
}

void RenderWindow::slotChangedCheckBoxCursorVisibility(int state)
{
  gMainInterface->renderedImage->SetCursorVisibility(state);
}

//adds dynamic actions to the toolbar (example settings)
void RenderWindow::slotPopulateToolbar(bool completeRefresh)
{
  WriteLog("cInterface::PopulateToolbar(QWidget *window, QToolBar *toolBar) started");
  QDir toolbarDir = QDir(systemData.dataDirectory + "toolbar");
  toolbarDir.setSorting(QDir::Time);
  QStringList toolbarFiles = toolbarDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
  QSignalMapper *mapPresetsFromExamplesLoad = new QSignalMapper(this);
  QSignalMapper *mapPresetsFromExamplesRemove = new QSignalMapper(this);
  ui->toolBar->setIconSize(QSize(gPar->Get<int>("toolbar_icon_size"), gPar->Get<int>("toolbar_icon_size")));

  QList<QAction *> actions = ui->toolBar->actions();
  QStringList toolbarInActions;
  for (int i = 0; i < actions.size(); i++)
  {
    QAction * action = actions.at(i);
    if(action->objectName() == "actionAdd_Settings_to_Toolbar") continue;
    if(!toolbarFiles.contains(action->objectName()) || completeRefresh)
    {
      // preset has been removed
      ui->toolBar->removeAction(action);
    }
    else
    {
      toolbarInActions << action->objectName();
    }
  }

  for (int i = 0; i < toolbarFiles.size(); i++)
  {
    if(toolbarInActions.contains(toolbarFiles.at(i)))
    {
      // already present
      continue;
    }
    QString filename = systemData.dataDirectory + "toolbar/" + toolbarFiles.at(i);
    cThumbnailWidget *thumbWidget = NULL;

    if (QFileInfo(filename).suffix() == QString("fract"))
    {
      WriteLogString("Generating thumbnail for preset", filename);
      cSettings parSettings(cSettings::formatFullText);
      parSettings.BeQuiet(true);

      if (parSettings.LoadFromFile(filename))
      {
        cParameterContainer *par = new cParameterContainer;
        cFractalContainer *parFractal = new cFractalContainer;
        InitParams(par);
        for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
          InitFractalParams(&parFractal->at(i));

      	/****************** TEMPORARY CODE FOR MATERIALS *******************/

      	InitMaterialParams(1, par);

      	/*******************************************************************/

        if (parSettings.Decode(par, parFractal))
        {
          thumbWidget = new cThumbnailWidget(gPar->Get<int>("toolbar_icon_size"), gPar->Get<int>("toolbar_icon_size"), 2, this);
          thumbWidget->UseOneCPUCore(true);
          thumbWidget->AssignParameters(*par, *parFractal);
        }
        delete par;
        delete parFractal;
      }
    }

    if(thumbWidget)
    {
      QWidgetAction *action = new QWidgetAction(this);
      QToolButton *buttonLoad = new QToolButton;
      QVBoxLayout *tooltipLayout = new QVBoxLayout;
      QToolButton *buttonRemove = new QToolButton;

      tooltipLayout->setContentsMargins(3, 3, 3, 3);
      tooltipLayout->addWidget(thumbWidget);
      QIcon iconDelete = QIcon::fromTheme("list-remove", QIcon(":system/icons/list-remove.svg"));
      buttonRemove->setIcon(iconDelete);
      buttonRemove->setMaximumSize(QSize(15, 15));
      buttonRemove->setStyleSheet("margin-bottom: -2px; margin-left: -2px;");
      tooltipLayout->addWidget(buttonRemove);
      buttonLoad->setToolTip(QObject::tr("Toolbar settings: ") + filename);
      buttonLoad->setLayout(tooltipLayout);
      action->setDefaultWidget(buttonLoad);
      action->setObjectName(toolbarFiles.at(i));
      ui->toolBar->addAction(action);

      mapPresetsFromExamplesLoad->setMapping(buttonLoad, filename);
      mapPresetsFromExamplesRemove->setMapping(buttonRemove, filename);
      QApplication::connect(buttonLoad, SIGNAL(clicked()), mapPresetsFromExamplesLoad, SLOT(map()));
      QApplication::connect(buttonRemove, SIGNAL(clicked()), mapPresetsFromExamplesRemove, SLOT(map()));
    }
  }
  QApplication::connect(mapPresetsFromExamplesLoad,
                        SIGNAL(mapped(QString)),
                        this,
                        SLOT(slotMenuLoadPreset(QString)));

  QApplication::connect(mapPresetsFromExamplesRemove,
                        SIGNAL(mapped(QString)),
                        this,
                        SLOT(slotMenuRemovePreset(QString)));

  WriteLog("cInterface::PopulateToolbar(QWidget *window, QToolBar *toolBar) finished");
}

void RenderWindow::slotPresetAddToToolbar()
{
  cSettings parSettings(cSettings::formatCondensedText);
  gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
  parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
  QString filename = systemData.dataDirectory + "toolbar/" + parSettings.GetHashCode() + ".fract";
  parSettings.SaveToFile(filename);
  slotPopulateToolbar();
}

void RenderWindow::slotMenuLoadPreset(QString filename)
{
  cSettings parSettings(cSettings::formatFullText);
  parSettings.LoadFromFile(filename);
  parSettings.Decode(gPar, gParFractal);
  gMainInterface->RebuildPrimitives(gPar);
  gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
  gMainInterface->ComboMouseClickUpdate();
  systemData.lastSettingsFile = gPar->Get<QString>("default_settings_path") + QDir::separator()
      + QFileInfo(filename).fileName();
  this->setWindowTitle(QString("Mandelbulber (") + systemData.lastSettingsFile + ")");
}

void RenderWindow::slotMenuRemovePreset(QString filename)
{
  QFile::remove(filename);
  slotPopulateToolbar();
}

void RenderWindow::slotQuit()
{
  gMainInterface->QuitApplicationDialog();
}

void RenderWindow::closeEvent(QCloseEvent * event)
{
  if (gMainInterface->QuitApplicationDialog())
  {
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

void RenderWindow::changeEvent(QEvent* event)
{
  if (event->type() == QEvent::LanguageChange)
  {
    // retranslate designer form (single inheritance approach)
		// ui->retranslateUi(this);
  }

  // remember to call base class implementation
  QMainWindow::changeEvent(event);
}

void RenderWindow::slotUpdateProgressAndStatus(const QString &text, const QString &progressText,
    double progress, cProgressText::enumProgressType progressType)
{
  ui->statusbar->showMessage(text, 0);
  QProgressBar *progressBar = NULL;
  bool isQueue = this->sender() && this->sender()->objectName() == "Queue";
  switch (progressType)
  {
    case cProgressText::progress_IMAGE:
      if (isQueue) progressBar = gMainInterface->progressBarQueueImage;
      else progressBar = gMainInterface->progressBar;
      break;
    case cProgressText::progress_ANIMATION:
      if (isQueue) progressBar = gMainInterface->progressBarQueueAnimation;
      else progressBar = gMainInterface->progressBarAnimation;
      break;
    case cProgressText::progress_QUEUE:
      // nothing to be done, no progress bar for queue in GUI
      break;
  }

  if (progressBar)
  {
    if (!progressBar->isVisible())
    {
      progressBar->setVisible(true);
    }
    progressBar->setValue(progress * 1000.0);
    progressBar->setTextVisible(true);
    progressBar->setFormat(progressText);
  }
}

void RenderWindow::slotUpdateProgressHide(cProgressText::enumProgressType progressType)
{
  QProgressBar *progressBar = NULL;
  switch (progressType)
  {
    case cProgressText::progress_IMAGE:
      progressBar = gMainInterface->progressBar;
      break;
    case cProgressText::progress_ANIMATION:
      progressBar = gMainInterface->progressBarAnimation;
      break;
    case cProgressText::progress_QUEUE:
      // nothing to be done, no progress bar for queue in GUI
      break;
  }

  if (progressBar)
  {
    if (progressBar->isVisible()) progressBar->setVisible(false);
  }
}

void RenderWindow::slotMenuProgramSettings()
{
	if(!preferencesDialog)
	{
		preferencesDialog = new cPreferencesDialog;
	}

	preferencesDialog->show();
	preferencesDialog->raise();
	preferencesDialog->activateWindow();
}

void RenderWindow::slotExportVolumeSlices()
{
	if(!volumeSliceDialog)
	{
		volumeSliceDialog = new cVolumeSliceDialog;
	}

	volumeSliceDialog->show();
	volumeSliceDialog->raise();
	volumeSliceDialog->activateWindow();
}

void RenderWindow::slotUpdateStatistics(cStatistics stat)
{
  ui->label_histogram_de->SetBarcolor(QColor(0, 255, 0));
  ui->label_histogram_de->UpdateHistogram(stat.histogramStepCount);
  ui->label_histogram_iter->UpdateHistogram(stat.histogramIterations);

  ui->tableWidget_statistics->item(0, 0)->setText(QString::number(stat.GetTotalNumberOfIterations()));
  ui->tableWidget_statistics->item(1, 0)->setText(QString::number(stat.GetNumberOfIterationsPerPixel()));
  ui->tableWidget_statistics->item(2, 0)->setText(QString::number(stat.GetNumberOfIterationsPerSecond()));
  ui->tableWidget_statistics->item(3, 0)->setText(stat.GetDETypeString());
  ui->tableWidget_statistics->item(4, 0)->setText(QString::number(stat.GetMissedDEPercentage()));
  ui->label_wrong_DE_percentage->setText(tr("Percentage of wrong distance estimations: %1").arg(stat.GetMissedDEPercentage()));
  ui->label_used_distance_estimation->setText(tr("Used distance estimation algorithm: %1").arg(stat.GetDETypeString()));
}

void RenderWindow::slotQuestionMessage(const QString &questionTitle, const QString &questionText,
    QMessageBox::StandardButtons buttons, QMessageBox::StandardButton *reply)
{
  *reply = QMessageBox::question(ui->centralwidget, questionTitle, questionText, buttons);
}

void RenderWindow::slotFractalSwap(int swapA, int swapB)
{
  // qDebug() << "swapping " << swapA << " with " << swapB;

  // read all data from ui
  gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);

  // swap formula specific fields in gPar
  QStringList gParFormulaSpecificFields;
  gParFormulaSpecificFields << "formula" << "formula_iterations" << "formula_weight"
    << "formula_start_iteration" << "formula_stop_iteration" << "julia_mode" << "julia_c"
    << "fractal_constant_factor" << "formula_position" << "formula_rotation" << "formula_repeat"
    << "formula_scale" << "dont_add_c_constant" << "check_for_bailout";

  for(int i = 0; i < gParFormulaSpecificFields.size(); i++)
  {
    cOneParameter formulaA = gPar->GetAsOneParameter(gParFormulaSpecificFields.at(i) + "_" + QString::number(swapA + 1));
    cOneParameter formulaB = gPar->GetAsOneParameter(gParFormulaSpecificFields.at(i) + "_" + QString::number(swapB + 1));
    gPar->SetFromOneParameter(gParFormulaSpecificFields.at(i) + "_" + QString::number(swapA + 1), formulaB);
    gPar->SetFromOneParameter(gParFormulaSpecificFields.at(i) + "_" + QString::number(swapB + 1), formulaA);
  }

  // swap formula specific fields in gParFractal by swapping whole container
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[swapA],
                                             &gParFractal->at(swapB),
                                             cInterface::read);
  gMainInterface->SynchronizeInterfaceWindow(fractalWidgets[swapB],
                                             &gParFractal->at(swapA),
                                             cInterface::read);

  // write swapped changes to ui
  gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
}

void RenderWindow::slotChangedCheckBoxUseDefaultBailout(int state)
{
  ui->logslider_bailout->setEnabled(!state);
  ui->logedit_bailout->setEnabled(!state);
}

void RenderWindow::slotChangedComboFractalColoringAlgorithm(int index)
{
	sFractalColoring::enumFractalColoringAlgorithm selection = (sFractalColoring::enumFractalColoringAlgorithm)index;
  ui->slider_fractal_coloring_sphere_radius->setEnabled(selection == sFractalColoring::fractalColoringSphere);
  ui->spinbox_fractal_coloring_sphere_radius->setEnabled(selection == sFractalColoring::fractalColoringSphere);
  ui->vect3_fractal_coloring_line_direction_x->setEnabled(selection == sFractalColoring::fractalColoringLine);
  ui->vect3_fractal_coloring_line_direction_y->setEnabled(selection == sFractalColoring::fractalColoringLine);
  ui->vect3_fractal_coloring_line_direction_z->setEnabled(selection == sFractalColoring::fractalColoringLine);
}

void RenderWindow::slotChangedCheckBoxDOFHDR(int state)
{
  ui->pushButton_DOF_update->setEnabled(!state);
  ui->pushButton_apply_image_changes->setEnabled(!state);
  if(ui->comboBox_ambient_occlusion_mode->currentIndex() == 2 && !state)
  {
    ui->comboBox_ambient_occlusion_mode->setCurrentIndex(0);
  }
}
