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
	void slotCameraMove();
	void slotCameraRotation();
	void slotCameraOrTargetEdited();
	void slotRotationEdited();
	void slotCameraDistanceEdited();
	void slotCameraDistanceSlider(int value);
	void slotCheckBoxHybridFractalChanged(int state);
	void slotIFSDefaultsDodecahedron();
	void slotIFSDefaultsIcosahedron();
	void slotIFSDefaultsOctahedron();
	void slotIFSDefaultsMengerSponge();
	void slotIFSDefaultsReset();

	void slotSaveSettings();
	void slotLoadSettings();
	void slotSaveImageJPEG();
	void slotSaveImagePNG8();
	void slotSaveImagePNG16();
	void slotSaveImagePNG16Alpha();

	void slotAboutQt();
	void slotAboutMandelbulber();
private:


	Ui::RenderWindow *ui;

	QWidget **fractalWidgets;

	friend class cInterface;

	QSettings settings;
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


