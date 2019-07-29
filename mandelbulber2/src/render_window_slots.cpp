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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * RenderWindow class - main program window
 *
 * This file contains implementation of the miscellaneous slots in RenderWindow class.
 * See also header render_window.hpp and whole implementation of class
 * spread over render_window_*.cpp
 */

#include "ui_render_window.h"

#include "animation_flight.hpp"
#include "animation_frames.hpp"
#include "animation_keyframes.hpp"
#include "cimage.hpp"
#include "global_data.hpp"
#include "image_scale.hpp"
#include "initparameters.hpp"
#include "interface.hpp"
#include "keyframes.hpp"
#include "material_item_model.h"
#include "render_window.hpp"
#include "settings.hpp"
#include "system.hpp"

#include "qt/material_editor.h"
#include "qt/mesh_export_dialog.h"
#include "qt/preferences_dialog.h"
#include "qt/thumbnail_widget.h"
#include "qt/voxel_export_dialog.h"

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
		case RenderedImage::clickWrapLimitsAroundObject:
		{
			if (gMainInterface->renderedImage->GetEnableClickModes())
			{
				gMainInterface->SetByMouse(CVector2<double>(x, y), button, mode);
			}
			break;
		}
		case RenderedImage::clickDoNothing:
		case RenderedImage::clickFlightSpeedControl:
			// nothing
			break;
	}
}

void RenderWindow::slotMouseDragStart(int x, int y, Qt::MouseButtons buttons)
{
	int index = ui->comboBox_mouse_click_function->currentIndex();
	QList<QVariant> mode = ui->comboBox_mouse_click_function->itemData(index).toList();
	RenderedImage::enumClickMode clickMode = RenderedImage::enumClickMode(mode.at(0).toInt());

	switch (clickMode)
	{
		case RenderedImage::clickMoveCamera:
		{
			if (gMainInterface->renderedImage->GetEnableClickModes())
			{
				gMainInterface->MouseDragStart(CVector2<double>(x, y), buttons, mode);
			}
			break;
		}
		default:
			// nothing
			break;
	}
}

void RenderWindow::slotMouseDragFinish()
{
	gMainInterface->MouseDragFinish();
}

void RenderWindow::slotMouseDragDelta(int dx, int dy)
{
	gMainInterface->MouseDragDelta(dx, dy);
}

void RenderWindow::slotChangedComboMouseClickFunction(int index) const
{
	if (index >= 0) // if list is empty, then index = -1
	{
		QComboBox *comboBox = static_cast<QComboBox *>(sender());
		QList<QVariant> item = comboBox->itemData(index).toList();
		gMainInterface->renderedImage->setClickMode(item);
	}
}

void RenderWindow::slotKeyPressOnImage(QKeyEvent *event)
{
	currentKeyEvents.append(event->key());
	lastKeyEventModifiers = event->modifiers();
	slotKeyHandle();
	buttonPressTimer->start();
}

void RenderWindow::slotKeyReleaseOnImage(QKeyEvent *event)
{
	currentKeyEvents.removeOne(event->key());
	lastKeyEventModifiers = event->modifiers();
	slotKeyHandle();
	buttonPressTimer->stop();
}

void RenderWindow::slotButtonLongPress()
{
	slotKeyHandle();
}

void RenderWindow::slotKeyHandle()
{
	if (currentKeyEvents.size() == 0) return;

	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);

	bool render = false;

	for (int i = 0; i < currentKeyEvents.size(); i++)
	{
		int key = currentKeyEvents.at(i);
		Qt::KeyboardModifiers modifiers = lastKeyEventModifiers;
		if (modifiers & Qt::ShiftModifier)
		{ // Shift pressed
			switch (key)
			{
				case Qt::Key_Up:
					gMainInterface->MoveCamera("bu_move_forward");
					render = true;
					break;
				case Qt::Key_Down:
					gMainInterface->MoveCamera("bu_move_backward");
					render = true;
					break;
				case Qt::Key_Left:
					gMainInterface->MoveCamera("bu_move_left");
					render = true;
					break;
				case Qt::Key_Right:
					gMainInterface->MoveCamera("bu_move_right");
					render = true;
					break;
				default: break;
			}
		}
		else if (modifiers & Qt::ControlModifier)
		{ // Ctrl pressed
			switch (key)
			{
				case Qt::Key_Up:
					gMainInterface->MoveCamera("bu_move_forward");
					render = true;
					break;
				case Qt::Key_Down:
					gMainInterface->MoveCamera("bu_move_backward");
					render = true;
					break;
				case Qt::Key_Left:
					gMainInterface->RotateCamera("bu_rotate_roll_left");
					render = true;
					break;
				case Qt::Key_Right:
					gMainInterface->RotateCamera("bu_rotate_roll_right");
					render = true;
					break;
				default: break;
			}
		}
		else
		{
			// No keyboard modifiers
			switch (key)
			{
				case Qt::Key_W:
					gMainInterface->MoveCamera("bu_move_up");
					render = true;
					break;
				case Qt::Key_S:
					gMainInterface->MoveCamera("bu_move_down");
					render = true;
					break;
				case Qt::Key_A:
					gMainInterface->MoveCamera("bu_move_left");
					render = true;
					break;
				case Qt::Key_D:
					gMainInterface->MoveCamera("bu_move_right");
					render = true;
					break;
				case Qt::Key_Q:
					gMainInterface->MoveCamera("bu_move_forward");
					render = true;
					break;
				case Qt::Key_Z:
					gMainInterface->MoveCamera("bu_move_backward");
					render = true;
					break;
				case Qt::Key_Up:
					gMainInterface->RotateCamera("bu_rotate_up");
					render = true;
					break;
				case Qt::Key_Down:
					gMainInterface->RotateCamera("bu_rotate_down");
					render = true;
					break;
				case Qt::Key_Left:
					gMainInterface->RotateCamera("bu_rotate_left");
					render = true;
					break;
				case Qt::Key_Right:
					gMainInterface->RotateCamera("bu_rotate_right");
					render = true;
					break;

				case Qt::Key_I:
					currentKeyEvents.removeOne(key); // long press not allowed
					gKeyframeAnimation->slotAddKeyframe();
					break;
				case Qt::Key_M:
					currentKeyEvents.removeOne(key); // long press not allowed
					gKeyframeAnimation->slotModifyKeyframe();
					break;
				/*case Qt::Key_D:
				 *		currentKeyEvents.removeOne(key); // long press not allowed
				 *		gKeyframeAnimation->slotDeleteKeyframe();
				 * break;*/
				case Qt::Key_N:
					currentKeyEvents.removeOne(key); // long press not allowed
					gKeyframeAnimation->slotIncreaseCurrentTableIndex();
					break;
				case Qt::Key_P:
					currentKeyEvents.removeOne(key); // long press not allowed
					gKeyframeAnimation->slotDecreaseCurrentTableIndex();
					break;
				default: break;
			}
		}
	}
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);

	if (render)
	{
		gMainInterface->StartRender();
	}
}

// global shortcuts
void RenderWindow::keyPressEvent(QKeyEvent *event)
{
	int key = event->key();
	Qt::KeyboardModifiers modifiers = event->modifiers();

	if (modifiers & Qt::ShiftModifier)
	{
		switch (key)
		{
			case Qt::Key_Escape: gMainInterface->GlobalStopRequest(); break;
			default: break;
		}
	}
}

void RenderWindow::slotMouseWheelRotatedWithCtrlOnImage(int x, int y, int delta) const
{
	if (gMainInterface->renderedImage->GetEnableClickModes())
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
			case RenderedImage::clickMoveCamera:
			{
				Qt::MouseButton button = (delta > 0) ? Qt::LeftButton : Qt::RightButton;
				mode.append(QVariant(delta));
				gMainInterface->SetByMouse(CVector2<double>(x, y), button, mode);
				break;
			}
			default: break;
		}
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

	QApplication::connect(
		mapPresetsFromExamplesLoad, SIGNAL(mapped(QString)), this, SLOT(slotMenuLoadPreset(QString)));

	for (auto action : actions)
	{
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
		if (systemData.globalStopRequest) break;

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
	gMainInterface->DisablePeriodicRefresh();
	parSettings.LoadFromFile(filename);
	gInterfaceReadyForSynchronization = false;
	parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
	gMainInterface->RebuildPrimitives(gPar);
	gMainInterface->materialListModel->Regenerate();
	gInterfaceReadyForSynchronization = true;
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
	gMainInterface->ComboMouseClickUpdate();
	systemData.lastSettingsFile = gPar->Get<QString>("default_settings_path") + QDir::separator()
																+ QFileInfo(filename).fileName();

	gFlightAnimation->RefreshTable();
	gKeyframeAnimation->RefreshTable();
	showDescriptionPopup();
	setWindowTitle(QString("Mandelbulber (") + systemData.lastSettingsFile + ")");
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
	QStringList geometryFileExtension("*.geometry");
	customWindowStateDir.setNameFilters(geometryFileExtension);
	QStringList customWindowStateFiles =
		customWindowStateDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
	QSignalMapper *mapCustomWindowLoad = new QSignalMapper(this);
	// QSignalMapper *mapCustomWindowRemove = new QSignalMapper(this);

	QList<QAction *> actions = ui->menuSaved_window_layouts->actions();
	QStringList customWindowActions;
	for (auto action : actions)
	{
		if (!action->objectName().startsWith("window_")) continue;
		if (!customWindowStateFiles.contains(action->objectName()) || completeRefresh)
		{
			// preset has been removed
			ui->menuSaved_window_layouts->removeAction(action);
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

		/*
		QWidgetAction *action = new QWidgetAction(this);
		QLabel *buttonLoad = new QLabel;
		QHBoxLayout *tooltipLayout = new QHBoxLayout;
		QToolButton *buttonRemove = new QToolButton;
		QLabel *label = new QLabel;

		buttonLoad->setStyleSheet("QLabel{ border: none; margin: 2px; padding: 1px; }");
		label->setText(QByteArray().fromBase64(QByteArray().append(customWindowStateFile)));
		tooltipLayout->setContentsMargins(5, 0, 0, 0);
		QIcon iconDelete = QIcon::fromTheme("list-remove", QIcon(":system/icons/list-remove.svg"));
		buttonRemove->setIcon(iconDelete);
		buttonRemove->setMaximumSize(QSize(15, 15));
		tooltipLayout->addWidget(buttonRemove);
		tooltipLayout->addWidget(label);
		buttonLoad->setLayout(tooltipLayout);
		// FIXME - when minimum width is not specified the widget is very small
		buttonLoad->setMinimumWidth(300);
		action->setDefaultWidget(buttonLoad);*/

		QAction *action = new QAction(this);
		action->setText(QByteArray().fromBase64(QByteArray().append(customWindowStateFile)));
		action->setObjectName("window_" + customWindowStateFile);

		ui->menuSaved_window_layouts->addAction(action);

		mapCustomWindowLoad->setMapping(action, filename);
		QApplication::connect(action, SIGNAL(triggered()), mapCustomWindowLoad, SLOT(map()));
		QApplication::processEvents();
	}
	QApplication::connect(mapCustomWindowLoad, SIGNAL(mapped(QString)), this,
		SLOT(slotMenuLoadCustomWindowState(QString)));

	/*QApplication::connect(mapCustomWindowRemove, SIGNAL(mapped(QString)), this,
		SLOT(slotMenuRemoveCustomWindowState(QString)));*/

	WriteLog("cInterface::slotPopulateCustomWindowStates() finished", 2);
}

void RenderWindow::slotCustomWindowStateAddToMenu()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Add window settings"),
		tr("Enter a name for the new window settings"), QLineEdit::Normal, "", &ok);
	if (!ok || text.isEmpty())
	{
		WriteLogCout("Cancelled window saving", 2);
		return;
	}
	QString textEncoded = QByteArray().append(text).toBase64();
	QString basePath = systemData.GetCustomWindowStateFolder() + QDir::separator();
	QString filename = basePath + textEncoded;
	QString filenameGeometry = filename + ".geometry";
	QString filenameState = filename + ".state";
	QFile fileGeometry(filenameGeometry);
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
	// restoreGeometry(fileGeometry.readAll());
	restoreState(fileState.readAll());
}

void RenderWindow::slotCustomWindowRemovePopup()
{
	QDir customWindowStateDir = QDir(systemData.GetCustomWindowStateFolder());
	customWindowStateDir.setSorting(QDir::Time);
	QStringList geometryFileExtension("*.geometry");
	customWindowStateDir.setNameFilters(geometryFileExtension);
	QStringList customWindowStateFiles =
		customWindowStateDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
	QStringList itemsEscaped;
	if (customWindowStateFiles.size() == 0)
	{
		WriteLogCout("Nothing to remove", 2);
		return;
	}
	for (int i = 0; i < customWindowStateFiles.size(); i++)
	{
		QString customWindowStateFile = customWindowStateFiles[i].replace(".geometry", "");
		itemsEscaped.append(QByteArray().fromBase64(QByteArray().append(customWindowStateFile)));
	}
	bool ok;

	QString itemEscaped = QInputDialog::getItem(this, tr("Remove window settings"),
		tr("Select window setting to remove"), itemsEscaped, 0, false, &ok);
	if (!ok || itemEscaped.isEmpty())
	{
		WriteLogCout("Cancelled window removing", 2);
		return;
	}
	int index = itemsEscaped.indexOf(itemEscaped);
	QString filename =
		systemData.GetCustomWindowStateFolder() + QDir::separator() + customWindowStateFiles.at(index);
	slotMenuRemoveCustomWindowState(filename.replace(".geometry", ""));
}

void RenderWindow::slotMenuRemoveCustomWindowState(QString filename)
{
	QFile::remove(filename + ".geometry");
	QFile::remove(filename + ".state");
	slotPopulateCustomWindowStates();
}

void RenderWindow::slotPopulateRecentSettings(bool completeRefresh)
{
	WriteLog("cInterface::slotPopulateRecentSettings() started", 2);
	QFile recentFilesFile(systemData.GetRecentFilesListFile());
	if (!recentFilesFile.open(QFile::ReadOnly | QFile::Text))
	{
		// qDebug() << "cannot open recent file";
		return;
	}
	QTextStream in(&recentFilesFile);
	QString recentFilesFileContent = in.readAll();
	QStringList recentFiles =
		recentFilesFileContent.split(QRegExp("\n|\r\n|\r"), QString::KeepEmptyParts);

	QSignalMapper *mapRecentFileLoad = new QSignalMapper(this);
	QList<QAction *> actions = ui->menuRecent_Settings_list->actions();
	QStringList recentFileInActions;
	for (auto action : actions)
	{
		if (!action->objectName().startsWith("recent_")) continue;
		if (!recentFileInActions.contains(action->objectName()) || completeRefresh)
		{
			// preset has been removed
			ui->menuRecent_Settings_list->removeAction(action);
		}
		else
		{
			recentFileInActions << action->objectName();
		}
	}

	for (int i = 0; i < recentFiles.size(); i++)
	{
		QString settingsFile = recentFiles.at(i);
		if (recentFileInActions.contains("recent_" + settingsFile))
		{
			// already present
			continue;
		}

		QAction *action = new QAction(this);
		action->setText(QFileInfo(settingsFile).baseName());
		action->setObjectName("recent_" + settingsFile);

		ui->menuRecent_Settings_list->addAction(action);

		mapRecentFileLoad->setMapping(action, settingsFile);
		QApplication::connect(action, SIGNAL(triggered()), mapRecentFileLoad, SLOT(map()));
		QApplication::processEvents();
	}
	QApplication::connect(
		mapRecentFileLoad, SIGNAL(mapped(QString)), this, SLOT(slotMenuLoadSettingsFromFile(QString)));

	WriteLog("cInterface::slotPopulateRecentSettings() finished", 2);
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
	bool isQueue = sender() && sender()->objectName() == "Queue";
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

void RenderWindow::slotMenuProgramPreferences()
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
}

void RenderWindow::slotMaterialEdited()
{
	SynchronizeInterfaceWindow(gMainInterface->materialEditor, gPar, qInterface::write);
}

void RenderWindow::slotChangedComboGridType(int index)
{
	gMainInterface->renderedImage->SetGridType(RenderedImage::enumGridType(index));
}

void RenderWindow::ResetGlobalStopRequest()
{
	gMainInterface->ResetGlobalStopRequest();
}

bool RenderWindow::eventFilter(QObject *obj, QEvent *event)
{

	if (event->type() == QEvent::ToolTip && !gPar->Get<bool>("display_tooltips"))
	{
		{
			return true;
		}
	}
	else if (event->type() == QEvent::ShortcutOverride)
	{
		buttonPressTimer->stop();
		return QMainWindow::eventFilter(obj, event);
	}
	else
	{
		return QMainWindow::eventFilter(obj, event);
	}
}
