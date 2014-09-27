/*
 * render_window.hpp
 *
 *  Created on: Mar 20, 2014
 *      Author: krzysztof
 */

#ifndef _RENDER_WINDOW_H
#define _RENDER_WINDOW_H

#include <QtGui>
#include <QtCore>
#include "../qt/ui_render_window.h"
#include <QSettings>
#include "cimage.hpp"

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

private slots:
	void slotStartRender();
	void slotStopRender();
	void load();

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
	void slotPresedColorButton();

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
	void slotPressedButtonDOFUpdate();
	void slotPressedButtonGetJuliaConstant();
	void slotPressedButtonGetPaletteFromImage();
	void slotPressedButtonImageApply();
	void slotPressedButtonNewRandomPalette();
	void slotPressedButtonRandomize();
	void slotPressedButtonSetDOFByMouse();
	void slotPressedButtonSetFogByMouse();
	void slotPressedButtonSetLight1ByMouse();
	void slotPressedButtonSetLight2ByMouse();
	void slotPressedButtonSetLight3ByMouse();
	void slotPressedButtonSetLight4ByMouse();
	void slotPressedButtonResetView();
	void slotResizedScrolledAreaImage(int width, int height);
	void slotSliderMovedEditManualLightPlacementDistance(int value);

	//camera
	void slotCameraMove();
	void slotCameraRotation();
	void slotCameraOrTargetEdited();
	void slotRotationEdited();
	void slotCameraDistanceEdited();
	void slotCameraDistanceSlider(int value);
	void slotMovementStepModeChanged(int index);

	void slotChangedCheckBoxHybridFractal(int state);

	//IFS
	void slotPressedButtonIFSDefaultsDodecahedron();
	void slotPressedButtonIFSDefaultsIcosahedron();
	void slotPressedButtonIFSDefaultsOctahedron();
	void slotPressedButtonIFSDefaultsMengerSponge();
	void slotPressedButtonIFSDefaultsReset();

	//pull down menu
	void slotMenuAboutMandelbulber();
	void slotMenuAboutQt();
	void slotMenuLoadSettings();
	void slotMenuSaveDocksPositions();
	void slotMenuSaveImageJPEG();
	void slotMenuSaveImagePNG16();
	void slotMenuSaveImagePNG16Alpha();
	void slotMenuSaveImagePNG8();
	void slotMenuSaveSettings();
	void slotMenuUndo();
	void slotMenuRedo();

	//textures
	void slotPressedButtonSelectBackgroundTexture();
	void slotPressedButtonSelectEnvMapTexture();
	void slotPressedButtonSelectLightMapTexture();
	void slotEditedLineEditBackgroundTexture(const QString &text);
	void slotEditedLineEditEnvMapTexture(const QString &text);
	void slotEditedLineEditLightMapTexture(const QString &text);
	void slotChangedComboAmbientOcclussionMode(int index);

	//resolution
	void slotChangedComboImageProportion(int index);
	void slotPressedResolutionPresset();
	void slotImageHeightChanged(int value);

	//rendered image widget
	void slotMouseMovedOnImage(int x, int y);
	void slotMouceClickOnImage(int x, int y, Qt::MouseButton button);
	void slotKeyPressOnImage(Qt::Key key);
	void slotKeyReleaseOnImage(Qt::Key key);
	void slotMouseWheelRotatedonImage(int delta);

private:
	Ui::RenderWindow *ui;
	QWidget **fractalWidgets;
	friend class cInterface;
	QSettings settings;

	enum enumImageProporton
	{
		proportionFree = 0,
		proportion1_1 = 1,
		proportion4_3 = 2,
		proportion3_2 = 3,
		proportion16_9 = 4,
		proportion16_10 = 5,
		proportion2_1 = 6
	};
};







#endif // _RENDER_WINDOW_H


