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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * RenderWindow class - main program window
 *
 * This file contains implementation of the miscellanious slots in RenderWindow class.
 * See also header render_window.hpp and whole implementation of class
 * spread over render_window_*.cpp
 */

#include "../qt/material_editor.h"
#include "render_window.hpp"
#include "../qt/preferences_dialog.h"
#include "../qt/thumbnail_widget.h"
#include "../qt/voxel_export_dialog.h"
#include "../qt/mesh_export_dialog.h"
#include "../src/material_item_model.h"
#include "animation_frames.hpp"
#include "cimage.hpp"
#include "image_scale.hpp"
#include "initparameters.hpp"
#include "interface.hpp"
#include "keyframes.hpp"
#include "settings.hpp"
#include "system.hpp"
#include "animation_flight.hpp"
#include "animation_keyframes.hpp"
#include "ui_render_window.h"

void RenderWindow::slotResizedScrolledAreaImage(int width, int height) const
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

void RenderWindow::slotChangedComboImageScale(int index) const
{
	if (gMainInterface->mainImage)
	{
		double scale = ImageScaleComboSelection2Double(index);
		int areaWidth = ui->scrollAreaForImage->VisibleAreaWidth();
		int areaHeight = ui->scrollAreaForImage->VisibleAreaHeight();
		scale = CalcMainImageScale(scale, areaWidth, areaHeight, gMainInterface->mainImage);

		gMainInterface->mainImage->CreatePreview(
			scale, areaWidth, areaHeight, gMainInterface->renderedImage);
		gMainInterface->mainImage->UpdatePreview();
		gMainInterface->renderedImage->setMinimumSize(
			gMainInterface->mainImage->GetPreviewWidth(), gMainInterface->mainImage->GetPreviewHeight());

		gPar->Set("image_preview_scale", index);
	}
}

void RenderWindow::slotMouseMovedOnImage(int x, int y)
{
	(void)x;
	(void)y;
	// qDebug() << "mouse move event";
	// CVector2<int> point(x, y);
}

void RenderWindow::slotMouseClickOnImage(int x, int y, Qt::MouseButton button) const
{
	int index = ui->comboBox_mouse_click_function->currentIndex();
	QList<QVariant> mode = ui->comboBox_mouse_click_function->itemData(index).toList();
	RenderedImage::enumClickMode clickMode = RenderedImage::enumClickMode(mode.at(0).toInt());

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
			// nothing
			break;
	}
}

void RenderWindow::slotChangedComboMouseClickFunction(int index) const
{
	if (index >= 0) // if list is empty, then index = -1
	{
		QComboBox *comboBox = static_cast<QComboBox *>(this->sender());
		QList<QVariant> item = comboBox->itemData(index).toList();
		gMainInterface->renderedImage->setClickMode(item);
	}
}

void RenderWindow::slotKeyPressOnImage(QKeyEvent *event)
{

	int key = event->key();
	Qt::KeyboardModifiers modifiers = event->modifiers();
	if (modifiers & Qt::ShiftModifier)
	{ // Shift pressed
		switch (key)
		{
			case Qt::Key_Up: gMainInterface->RotateCamera("bu_rotate_up"); break;
			case Qt::Key_Down: gMainInterface->RotateCamera("bu_rotate_down"); break;
			case Qt::Key_Left: gMainInterface->RotateCamera("bu_rotate_left"); break;
			case Qt::Key_Right: gMainInterface->RotateCamera("bu_rotate_right"); break;
			default: break;
		}
	}
	else if (modifiers & Qt::ControlModifier)
	{ // Ctrl pressed
		switch (key)
		{
			case Qt::Key_Up: gMainInterface->MoveCamera("bu_move_up"); break;
			case Qt::Key_Down: gMainInterface->MoveCamera("bu_move_down"); break;
			case Qt::Key_Left: gMainInterface->RotateCamera("bu_rotate_roll_left"); break;
			case Qt::Key_Right: gMainInterface->RotateCamera("bu_rotate_roll_right"); break;
			default: break;
		}
	}
	else
	{
		// No keyboard modifiers
		switch (key)
		{
			case Qt::Key_W: gMainInterface->MoveCamera("bu_move_up"); break;
			case Qt::Key_S: gMainInterface->MoveCamera("bu_move_down"); break;
			case Qt::Key_A: gMainInterface->MoveCamera("bu_move_left"); break;
			case Qt::Key_D: gMainInterface->MoveCamera("bu_move_right"); break;
			case Qt::Key_Up: gMainInterface->MoveCamera("bu_move_forward"); break;
			case Qt::Key_Down: gMainInterface->MoveCamera("bu_move_backward"); break;
			case Qt::Key_Left: gMainInterface->MoveCamera("bu_move_left"); break;
			case Qt::Key_Right: gMainInterface->MoveCamera("bu_move_right"); break;
			default: break;
		}
	}
}

void RenderWindow::slotKeyReleaseOnImage(QKeyEvent *event)
{
	(void)event;
}

void RenderWindow::slotMouseWheelRotatedOnImage(int delta) const
{
	int index = ui->comboBox_mouse_click_function->currentIndex();
	QList<QVariant> mode = ui->comboBox_mouse_click_function->itemData(index).toList();
	RenderedImage::enumClickMode clickMode = RenderedImage::enumClickMode(mode.at(0).toInt());
	switch (clickMode)
	{
		case RenderedImage::clickPlaceLight:
		{
			double deltaLog = exp(delta * 0.001);
			double dist = ui->widgetEffects->GetAuxLightManualPlacementDistance();
			dist *= deltaLog;
			ui->widgetEffects->SetAuxLightManualPlacementDistance(dist);
			break;
		}
		default: break;
	}
}

void RenderWindow::slotChangedCheckBoxCursorVisibility(int state)
{
	gMainInterface->renderedImage->SetCursorVisibility(state);
}

// adds dynamic actions to the toolbar (example settings)
void RenderWindow::slotPopulateToolbar(bool completeRefresh)
{
	WriteLog("cInterface::PopulateToolbar(QWidget *window, QToolBar *toolBar) started", 2);
	QDir toolbarDir = QDir(systemData.GetToolbarFolder());
	toolbarDir.setSorting(QDir::Time);
	QStringList toolbarFiles = toolbarDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
	QSignalMapper *mapPresetsFromExamplesLoad = new QSignalMapper(this);
	QSignalMapper *mapPresetsFromExamplesRemove = new QSignalMapper(this);
	ui->toolBar->setIconSize(
		QSize(gPar->Get<int>("toolbar_icon_size"), gPar->Get<int>("toolbar_icon_size")));

	QList<QAction *> actions = ui->toolBar->actions();
	QStringList toolbarInActions;
	for (int i = 0; i < actions.size(); i++)
	{
		QAction *action = actions.at(i);
		if (action->objectName() == "actionAdd_Settings_to_Toolbar") continue;
		if (!toolbarFiles.contains(action->objectName()) || completeRefresh)
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
		if (toolbarInActions.contains(toolbarFiles.at(i)))
		{
			// already present
			continue;
		}
		QString filename = systemData.GetToolbarFolder() + QDir::separator() + toolbarFiles.at(i);
		cThumbnailWidget *thumbWidget = nullptr;

		if (QFileInfo(filename).suffix() == QString("fract"))
		{
			WriteLogString("Generating thumbnail for preset", filename, 2);
			cSettings parSettings(cSettings::formatFullText);
			parSettings.BeQuiet(true);

			if (parSettings.LoadFromFile(filename))
			{
				cParameterContainer *par = new cParameterContainer;
				cFractalContainer *parFractal = new cFractalContainer;
				InitParams(par);
				for (int j = 0; j < NUMBER_OF_FRACTALS; j++)
					InitFractalParams(&parFractal->at(j));

				/****************** TEMPORARY CODE FOR MATERIALS *******************/

				InitMaterialParams(1, par);

				/*******************************************************************/

				if (parSettings.Decode(par, parFractal))
				{
					thumbWidget = new cThumbnailWidget(
						gPar->Get<int>("toolbar_icon_size"), gPar->Get<int>("toolbar_icon_size"), 2, this);
					thumbWidget->UseOneCPUCore(true);
					thumbWidget->AssignParameters(*par, *parFractal);
				}
				delete par;
				delete parFractal;
			}
		}

		if (thumbWidget)
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
			QApplication::connect(
				buttonRemove, SIGNAL(clicked()), mapPresetsFromExamplesRemove, SLOT(map()));
		}
		QApplication::processEvents();
	}
	QApplication::connect(
		mapPresetsFromExamplesLoad, SIGNAL(mapped(QString)), this, SLOT(slotMenuLoadPreset(QString)));

	QApplication::connect(mapPresetsFromExamplesRemove, SIGNAL(mapped(QString)), this,
		SLOT(slotMenuRemovePreset(QString)));

	WriteLog("cInterface::PopulateToolbar(QWidget *window, QToolBar *toolBar) finished", 2);
}

void RenderWindow::slotPresetAddToToolbar()
{
	cSettings parSettings(cSettings::formatCondensedText);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
	QString filename =
		systemData.GetToolbarFolder() + QDir::separator() + parSettings.GetHashCode() + ".fract";
	parSettings.SaveToFile(filename);
	slotPopulateToolbar();
}

void RenderWindow::slotMenuLoadPreset(QString filename)
{
	cSettings parSettings(cSettings::formatFullText);
	parSettings.LoadFromFile(filename);
	parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
	gMainInterface->RebuildPrimitives(gPar);
	gMainInterface->materialListModel->Regenerate();
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
	gMainInterface->ComboMouseClickUpdate();
	systemData.lastSettingsFile = gPar->Get<QString>("default_settings_path") + QDir::separator()
																+ QFileInfo(filename).fileName();

	gFlightAnimation->RefreshTable();
	gKeyframeAnimation->RefreshTable();
	showDescriptionPopup();
	this->setWindowTitle(QString("Mandelbulber (") + systemData.lastSettingsFile + ")");
}

void RenderWindow::slotMenuRemovePreset(QString filename)
{
	QFile::remove(filename);
	slotPopulateToolbar();
}

// adds dynamic actions to the view > window states
void RenderWindow::slotPopulateCustomWindowStates(bool completeRefresh)
{
	WriteLog("cInterface::slotPopulateCustomWindowStates() started", 2);
	QDir customWindowStateDir = QDir(systemData.GetCustomWindowStateFolder());
	customWindowStateDir.setSorting(QDir::Time);
	QStringList geometryFileExtension({"*.geometry"});
	customWindowStateDir.setNameFilters(geometryFileExtension);
	QStringList customWindowStateFiles =
		customWindowStateDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
	QSignalMapper *mapCustomWindowLoad = new QSignalMapper(this);
	QSignalMapper *mapCustomWindowRemove = new QSignalMapper(this);

	QList<QAction *> actions = ui->menuView->actions();
	QStringList customWindowActions;
	for (int i = 0; i < actions.size(); i++)
	{
		QAction *action = actions.at(i);
		if (!action->objectName().startsWith("window_")) continue;
		if (!customWindowStateFiles.contains(action->objectName()) || completeRefresh)
		{
			// preset has been removed
			ui->menuView->removeAction(action);
		}
		else
		{
			customWindowActions << action->objectName();
		}
	}

	for (int i = 0; i < customWindowStateFiles.size(); i++)
	{
		QString customWindowStateGeometryFile = customWindowStateFiles.at(i);
		QString customWindowStateFile = customWindowStateGeometryFile.replace(".geometry", "");
		if (customWindowActions.contains("window_" + customWindowStateFile))
		{
			// already present
			continue;
		}
		QString filename =
			systemData.GetCustomWindowStateFolder() + QDir::separator() + customWindowStateFile;

		QWidgetAction *action = new QWidgetAction(this);
		QPushButton *buttonLoad = new QPushButton;
		QHBoxLayout *tooltipLayout = new QHBoxLayout;
		QToolButton *buttonRemove = new QToolButton;
		QLabel *label = new QLabel;

		buttonLoad->setStyleSheet("QPushButton{ border: none; margin: 2px; padding: 1px; }");
		label->setText(QByteArray().fromBase64(QByteArray().append(customWindowStateFile)));
		tooltipLayout->setContentsMargins(5, 0, 0, 0);
		QIcon iconDelete = QIcon::fromTheme("list-remove", QIcon(":system/icons/list-remove.svg"));
		buttonRemove->setIcon(iconDelete);
		buttonRemove->setMaximumSize(QSize(15, 15));
		tooltipLayout->addWidget(buttonRemove);
		tooltipLayout->addWidget(label);
		buttonLoad->setLayout(tooltipLayout);
		action->setDefaultWidget(buttonLoad);
		action->setObjectName("window_" + customWindowStateFile);
		ui->menuView->addAction(action);

		mapCustomWindowLoad->setMapping(buttonLoad, filename);
		mapCustomWindowRemove->setMapping(buttonRemove, filename);
		QApplication::connect(buttonLoad, SIGNAL(clicked()), mapCustomWindowLoad, SLOT(map()));
		QApplication::connect(buttonRemove, SIGNAL(clicked()), mapCustomWindowRemove, SLOT(map()));
		QApplication::processEvents();
	}
	QApplication::connect(mapCustomWindowLoad, SIGNAL(mapped(QString)), this,
		SLOT(slotMenuLoadCustomWindowState(QString)));

	QApplication::connect(mapCustomWindowRemove, SIGNAL(mapped(QString)), this,
		SLOT(slotMenuRemoveCustomWindowState(QString)));

	WriteLog("cInterface::slotPopulateCustomWindowStates() finished", 2);
}

void RenderWindow::slotCustomWindowStateAddToMenu()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Add window settings"),
		tr("Enter a name for the new window settings"), QLineEdit::Normal, "", &ok);
	if (!ok || text.isEmpty())
	{
		qDebug() << "Cancelled window saving";
		return;
	}
	QString textEncoded = QByteArray().append(text).toBase64();
	QString basePath = systemData.GetCustomWindowStateFolder() + QDir::separator();
	QString filename = basePath + textEncoded;
	QString filenamGeometry = filename + ".geometry";
	QString filenameState = filename + ".state";
	QFile fileGeometry(filenamGeometry);
	QFile fileState(filenameState);
	if (!fileGeometry.open(QIODevice::WriteOnly) || !fileState.open(QIODevice::WriteOnly))
	{
		qWarning() << "Could not open output files: " << filename << ".[geometry,state]";
		return;
	}
	fileGeometry.write(saveGeometry());
	fileState.write(saveState());
	fileGeometry.close();
	fileState.close();

	slotPopulateCustomWindowStates();
}

void RenderWindow::slotMenuLoadCustomWindowState(QString filename)
{
	QFile fileGeometry(filename + ".geometry");
	QFile fileState(filename + ".state");
	if (!fileGeometry.open(QIODevice::ReadOnly) || !fileState.open(QIODevice::ReadOnly))
	{
		qWarning() << "Could not open input files: " << filename << ".[geometry,state]";
		return;
	}
	// this->restoreGeometry(fileGeometry.readAll());
	this->restoreState(fileState.readAll());
}

void RenderWindow::slotMenuRemoveCustomWindowState(QString filename)
{
	QFile::remove(filename + ".geometry");
	QFile::remove(filename + ".state");
	slotPopulateCustomWindowStates();
}

void RenderWindow::slotQuit()
{
	gMainInterface->QuitApplicationDialog();
}

void RenderWindow::closeEvent(QCloseEvent *event)
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

void RenderWindow::changeEvent(QEvent *event)
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
	double progress, cProgressText::enumProgressType progressType) const
{
	ui->statusbar->showMessage(text, 0);
	MyProgressBar *progressBar = nullptr;
	bool isQueue = this->sender() && this->sender()->objectName() == "Queue";
	switch (progressType)
	{
		case cProgressText::progress_IMAGE:
			if (isQueue)
				progressBar = gMainInterface->progressBarQueueImage;
			else
				progressBar = gMainInterface->progressBar;
			break;
		case cProgressText::progress_ANIMATION:
			if (isQueue)
				progressBar = gMainInterface->progressBarQueueAnimation;
			else
				progressBar = gMainInterface->progressBarAnimation;
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
	MyProgressBar *progressBar = nullptr;
	switch (progressType)
	{
		case cProgressText::progress_IMAGE: progressBar = gMainInterface->progressBar; break;
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
	if (!preferencesDialog)
	{
		preferencesDialog = new cPreferencesDialog;
	}

	preferencesDialog->show();
	preferencesDialog->raise();
	preferencesDialog->activateWindow();
}

void RenderWindow::slotExportVoxelLayers()
{
	if (!voxelExportDialog)
	{
		voxelExportDialog = new cVoxelExportDialog;
	}

	voxelExportDialog->show();
	voxelExportDialog->raise();
	voxelExportDialog->activateWindow();
}

void RenderWindow::slotExportMesh()
{
	if (!meshExportDialog)
	{
		meshExportDialog = new cMeshExportDialog;
	}

	meshExportDialog->show();
	meshExportDialog->raise();
	meshExportDialog->activateWindow();
}

void RenderWindow::slotQuestionMessage(const QString &questionTitle, const QString &questionText,
	QMessageBox::StandardButtons buttons, QMessageBox::StandardButton *reply) const
{
	*reply = QMessageBox::question(ui->centralwidget, questionTitle, questionText, buttons);
}

void RenderWindow::slotAutoRefresh()
{
	gMainInterface->PeriodicRefresh();
}

void RenderWindow::slotMaterialSelected(int matIndex) const
{
	gMainInterface->MaterialSelected(matIndex);
	ui->dockWidget_materialEditor->raise();
}

void RenderWindow::slotMaterialEdited()
{
	SynchronizeInterfaceWindow(gMainInterface->materialEditor, gPar, qInterface::write);
}
