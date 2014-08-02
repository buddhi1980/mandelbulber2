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

	//sliders
	void slotSliderMoved(int value);
	void slotDoubleSpinBoxChanged(double value);
	void slotIntSliderMoved(int value);
	void slotIntSpinBoxChanged(int value);
	void slotSlider3Moved(int value);
	void slotSpinBox3Changed(double value);
  void slotDial3Moved(int value);
	void slotSpinBoxD3Changed(double value);
	void slotLogSliderMoved(int value);
	void slotLogLineEditChanged(const QString &text);

	void slotPresedOnColorButton();
	void slotMenuSaveDocksPositions();
	void slotChangedFractalCombo(int index);
	void slotImageScrolledAreaResized(int width, int height);
	void slotChangedImageScale(int index);
	void slotPressedImageApplyButton();

	//camera
	void slotCameraMove();
	void slotCameraRotation();
	void slotCameraOrTargetEdited();
	void slotRotationEdited();
	void slotCameraDistanceEdited();
	void slotCameraDistanceSlider(int value);

	void slotCheckBoxHybridFractalChanged(int state);

	//IFS
	void slotIFSDefaultsDodecahedron();
	void slotIFSDefaultsIcosahedron();
	void slotIFSDefaultsOctahedron();
	void slotIFSDefaultsMengerSponge();
	void slotIFSDefaultsReset();

	//load save
	void slotSaveSettings();
	void slotLoadSettings();
	void slotSaveImageJPEG();
	void slotSaveImagePNG8();
	void slotSaveImagePNG16();
	void slotSaveImagePNG16Alpha();

	//textures
	void slotSelectBackgroundTexture();
	void slotSelectEnvMapTexture();
	void slotSelectLightMapTexture();
	void slotLineEditBackgroundTextureEdited(const QString &text);
	void slotLineEditEnvMapTextureEdited(const QString &text);
	void slotLineEditLightMapTextureEdited(const QString &text);
	void slotChangedComboAmbientOcclussionMode(int index);

	//about
	void slotAboutQt();
	void slotAboutMandelbulber();

	//resolution
	void slotChangedComboImageProportion(int index);
	void slotPressedResolutionPresset();
	void slotImageHeightChanged(int value);


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


class RenderedImage: public QWidget
{
Q_OBJECT

public:
	RenderedImage(QWidget *parent = 0);

protected:
	void paintEvent(QPaintEvent *event);
};





#endif // _RENDER_WINDOW_H


