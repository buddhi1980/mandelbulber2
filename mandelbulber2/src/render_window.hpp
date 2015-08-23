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

#ifndef _RENDER_WINDOW_H
#define _RENDER_WINDOW_H

#include <QtGui>
#include <QtCore>
#include "../qt/ui_render_window.h"
#include <QSettings>
#include "cimage.hpp"
#include "statistics.h"
#ifdef USE_GAMEPAD
#include <QtGamepad/qgamepad.h>
#endif // USE_GAMEPAD

namespace Ui
{
class RenderWindow;
}

class RenderWindow: public QMainWindow
{
Q_OBJECT

public:
	explicit RenderWindow(QWidget *parent = 0);
	~RenderWindow();

private:
	void closeEvent(QCloseEvent * event);
	void changeEvent(QEvent* event);

private slots:
	void slotStartRender();
	void slotStopRender();
	void slotQuit();

	//automated sliders, dials, buttons
	void slotSliderMoved(int value);
	void slotDoubleSpinBoxChanged(double value);
	void slotIntSliderMoved(int value);
	void slotIntSpinBoxChanged(int value);
	void slotSlider3Moved(int value);
	void slotSpinBox3Changed(double value);
  void slotDial3Moved(int value);
	void slotSpinBoxD3Changed(double value);
  void slotDialMoved(int value);
	void slotSpinBoxDChanged(double value);
	void slotLogSliderMoved(int value);
	void slotLogLineEditChanged(const QString &text);

	//other
	void slotChangedCheckBoxCursorVisibility(int state);
	void slotChangedComboFractal(int index);
	void slotChangedComboImageScale(int index);
	void slotChangedComboMouseClickFunction(int index);
	void slotChangedComboPerspectiveType(int index);
	void slotChangedSpinBoxPaletteOffset(double value);
	void slotChangedSpinBoxPaletteSize(int value);
	void slotEditedLineEditManualLightPlacementDistance(const QString &text);
	void slotPressedButtonAutoFog();
	void slotPressedButtonDeletePrimitive();
	void slotPressedButtonDOFUpdate();
	void slotPressedButtonGetJuliaConstant();
	void slotPressedButtonGetPaletteFromImage();
	void slotPressedButtonImageApply();
	void slotPressedButtonNewPrimitive();
	void slotPressedButtonNewRandomPalette();
	void slotPressedButtonRandomize();
	void slotPressedButtonResetView();
	void slotPressedButtonSetDOFByMouse();
	void slotPressedButtonSetFogByMouse();
	void slotPressedButtonSetLight1ByMouse();
	void slotPressedButtonSetLight2ByMouse();
	void slotPressedButtonSetLight3ByMouse();
	void slotPressedButtonSetLight4ByMouse();
	void slotPressedButtonSetPositionPrimitive();
	void slotResizedScrolledAreaImage(int width, int height);
	void slotSliderMovedEditManualLightPlacementDistance(int value);
	void slotMenuLoadPreset(QString filename);
	void slotUpdateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void slotUpdateStatistics(cStatistics);
	void slotMenuProgramSettings();

	//camera
	void slotCameraMove();
	void slotCameraRotation();
	void slotCameraOrTargetEdited();
	void slotRotationEdited();
	void slotCameraDistanceEdited();
	void slotCameraDistanceSlider(int value);
	void slotMovementStepModeChanged(int index);

	void slotChangedCheckBoxHybridFractal(int state);
	void slotChangedCheckBoxBooleanOperators(bool state);

	//IFS
	void slotPressedButtonIFSDefaultsDodecahedron();
	void slotPressedButtonIFSDefaultsIcosahedron();
	void slotPressedButtonIFSDefaultsOctahedron();
	void slotPressedButtonIFSDefaultsMengerSponge();
	void slotPressedButtonIFSDefaultsReset();

	//pull down menu
	void slotImportOldSettings();
	void slotMenuAboutMandelbulber();
	void slotMenuAboutQt();
	void slotMenuAboutThirdParty();
	void slotMenuLoadExample();
	void slotMenuLoadSettings();
	void slotMenuLoadSettingsFromClipboard();
	void slotMenuRedo();
	void slotMenuResetDocksPositions();
	void slotMenuSaveDocksPositions();
	void slotMenuSaveImageJPEG();
	void slotMenuSaveImagePNG();
#ifdef USE_EXR
	void slotMenuSaveImageEXR();
#endif // USE_EXR

	void slotMenuSaveImagePNG16();
	void slotMenuSaveImagePNG16Alpha();

	void slotMenuSaveSettings();
	void slotMenuSaveSettingsToClipboard();
	void slotMenuUndo();
	void slotUpdateDocksandToolbarbyAction();
	void slotUpdateDocksandToolbarbyView();
	void slotStackAllDocks();

	//textures
	void slotPressedButtonSelectBackgroundTexture();
	void slotPressedButtonSelectEnvMapTexture();
	void slotPressedButtonSelectLightMapTexture();
	void slotEditedLineEditBackgroundTexture(const QString &text);
	void slotEditedLineEditEnvMapTexture(const QString &text);
	void slotEditedLineEditLightMapTexture(const QString &text);
	void slotChangedComboAmbientOcclusionMode(int index);

	//resolution
	void slotChangedComboImageProportion(int index);
	void slotPressedResolutionPreset();
	void slotPressedImagesizeIncrease();
	void slotPressedImagesizeDecrease();
	void slotImageHeightChanged(int value);

	//rendered image widget
	void slotMouseMovedOnImage(int x, int y);
	void slotMouseClickOnImage(int x, int y, Qt::MouseButton button);
	void slotKeyPressOnImage(Qt::Key key);
	void slotKeyReleaseOnImage(Qt::Key key);
	void slotMouseWheelRotatedOnImage(int delta);

	//NetRender
	void slotNetRenderServerStart();
	void slotNetRenderServerStop();
	void slotNetRenderClientConnect();
	void slotNetRenderClientDisconnect();
	void slotNetRenderClientServerChange(int index);
	void slotNetRenderClientListUpdate();
	void slotNetRenderClientListUpdate(int i);
	void slotNetRenderClientListUpdate(int i, int j);
	void slotNetRenderStatusServerUpdate();
	void slotNetRenderStatusClientUpdate();
	void slotCheckBoxDisableNetRender(bool on);

#ifdef USE_GAMEPAD
	// Gamepad
	void slotChangeGamepadIndex(int index);
	void slotGamePadDeviceConnected(int index);
	void slotGamePadDeviceDisconnected(int index);

	void slotGamepadPitch(double value);
	void slotGamepadYaw(double value);
	void slotGamepadRoll();

	void slotGamepadX(double value);
	void slotGamepadY(double value);
	void slotGamepadZ();
#endif // USE_GAMEPAD

private:
	Ui::RenderWindow *ui;
	QWidget **fractalWidgets;

	QSettings settings;
	QByteArray defaultGeometry;
	QByteArray defaultState;

#ifdef USE_GAMEPAD
	QGamepad gamepad;
#endif

	enum enumImageProportion
	{
		proportionFree = 0,
		proportion1_1 = 1,
		proportion4_3 = 2,
		proportion3_2 = 3,
		proportion16_9 = 4,
		proportion16_10 = 5,
		proportion2_1 = 6
	};

	signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void AppendToLog(const QString& text);

	friend class cInterface;
	friend class cFlightAnimation;
	friend class cKeyframeAnimation;
};







#endif // _RENDER_WINDOW_H


