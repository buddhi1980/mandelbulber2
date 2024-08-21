/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2024 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#ifndef MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_
#define MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_

#include <memory>

#include <QDialog>
#include <QList>
#include <QString>
#include <QVariant>

#include "src/synchronize_interface.hpp"

namespace Ui
{
class cNavigatorWindow;
}

class cParameterContainer;
class cFractalContainer;
class cImage;
class cManipulations;

class cNavigatorWindow : public QDialog
{
	Q_OBJECT

public:
	explicit cNavigatorWindow(QWidget *parent = nullptr);
	~cNavigatorWindow();
	void AddLeftWidget(QWidget *widget);

	void SetInitialParameters(std::shared_ptr<cParameterContainer> _params,
		std::shared_ptr<cFractalContainer> _fractalParams);
	void SetMouseClickFunction(QList<QVariant> _clickMode);
	void StartRender();
	void StopRender();
	void SynchronizeInterface(qInterface::enumReadWrite mode);
	void AllPrepared();

public slots:
	void slotCameraMovementModeChanged(int index);
	void slotMouseClickOnImage(int x, int y, Qt::MouseButton button) const;
	void slotMouseDragStart(int x, int y, Qt::MouseButtons buttons);
	void slotMouseDragFinish();
	void slotMouseDragDelta(int dx, int dy);
	void slotMouseWheelRotatedWithKeyOnImage(
		int x, int y, int delta, Qt::KeyboardModifiers keyModifiers);
	void slotButtonUseParameters();
	void slotButtonUseParametersWithoutCamera();
	void slotButtonCancel();
	void slotSmallPartRendered(double time);
	void slotFullImageRendered();
	void slotDisablePeriodicRefresh();
	void slotReEnablePeriodicRefresh();
	void slotRefreshMainImage();

private slots:
	void slotPeriodicRefresh();
	void slotChangedComboMouseClickFunction(int index);
	void slotChangedPreviewSize();
	void slotDarkGlowEnabled(int state);
	void slotKeyPressOnImage(QKeyEvent *event);
	void slotKeyReleaseOnImage(QKeyEvent *event);
	void slotButtonLongPress();
	void slotKeyHandle();

private:
	void InitPeriodicRefresh();
	void closeEvent(QCloseEvent *event) override;

signals:
	void signalChangesAccepted();

private:
	Ui::cNavigatorWindow *ui;

	cManipulations *manipulations;
	QWidget *leftWidget = nullptr;

	std::shared_ptr<cParameterContainer> params;
	std::shared_ptr<cFractalContainer> fractalParams;
	std::shared_ptr<cParameterContainer> sourceParams;
	std::shared_ptr<cFractalContainer> sourceFractalParams;
	std::shared_ptr<cImage> image;

	bool stopRequest = false;

	QString autoRefreshLastHash;
	QTimer *autoRefreshTimer = nullptr;
	bool autoRefreshEnabled = false;

	QList<QVariant> mouseClickFunction;

	double lastRenderedTimeOfSmallPart = 1.0f;
	double lastSizefactor = 8.0;
	int lastIntSizeFactor = 8;
	int forcedSizeFactor = 0;

	int initImageWidth = 800;
	int initImageHeight = 600;
	double imageProportion = 1.0;

	QTimer *buttonPressTimer;
	QList<int> currentKeyEvents;
	Qt::KeyboardModifiers lastKeyEventModifiers;
};

#endif /* MANDELBULBER2_QT_NAVIGATOR_WINDOW_H_ */
