/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * This file contains declaration of the RenderWindow class.
 * See also header render_window.hpp and whole implementation of class
 * spread over render_window_*.cpp
 */

#ifndef MANDELBULBER2_SRC_RENDER_WINDOW_HPP_
#define MANDELBULBER2_SRC_RENDER_WINDOW_HPP_

#include <QComboBox>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QtCore>
#include <QtGui>

#include "files.h"
#include "progress_text.hpp"

// forward declarations
class cAutomatedWidgets;
class cDockAnimation;
class cDockNavigation;
class cDockStatistics;
class cDockQueue;
class cDockImageAdjustments;
class cDockRenderingEngine;
class cDockFractal;
class cDockEffects;

namespace Ui
{
class RenderWindow;
}

class RenderWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit RenderWindow(QWidget *parent = nullptr);
	~RenderWindow();

	// Getters for UI elements
	cDockAnimation *GetWidgetDockAnimation() const;
	cDockNavigation *GetWidgetDockNavigation() const;
	cDockStatistics *GetWidgetDockStatistics() const;
	cDockQueue *GetWidgetDockQueue() const;
	cDockImageAdjustments *GetWidgetDockImageAdjustments() const;
	cDockRenderingEngine *GetWidgetDockRenderingEngine() const;
	cDockFractal *GetWidgetDockFractal() const;
	cDockEffects *GetWidgetDockEffects() const;

	QWidget *GetCentralWidget() const;
	QComboBox *GetComboBoxMouseClickFunction() const;

private:
	void closeEvent(QCloseEvent *event) override;
	void changeEvent(QEvent *event) override;

public slots:
	void slotUpdateProgressAndStatus(const QString &text, const QString &progressText,
		double progress,
		cProgressText::enumProgressType progressType = cProgressText::progress_IMAGE) const;
	void slotPopulateToolbar(bool completeRefresh = false);
	void slotPopulateCustomWindowStates(bool completeRefresh = false);
private slots:
	static void slotQuit();

	// other
	static void slotChangedCheckBoxCursorVisibility(int state);
	void slotChangedComboImageScale(int index) const;
	void slotChangedComboMouseClickFunction(int index) const;
	void slotChangedComboGridType(int index);

	void slotPressedButtonDeletePrimitive() const;
	void slotPressedButtonAllignPrimitiveAngle() const;

	void slotPressedButtonSetPositionPrimitive() const;
	void slotResizedScrolledAreaImage(int width, int height) const;
	void slotMenuLoadPreset(QString filename);
	void slotMenuRemovePreset(QString filename);
	void slotMenuLoadCustomWindowState(QString filename);
	void slotMenuRemoveCustomWindowState(QString filename);
	void slotPopulateRecentSettings(bool completeRefresh = false);
	void SaveSettingsToRecent(QString fileName);

	static void slotUpdateProgressHide(cProgressText::enumProgressType progressType);
	void slotMenuProgramPreferences();
	void slotExportVoxelLayers();
	void slotExportMesh();
	void slotQuestionMessage(const QString &questionTitle, const QString &questionText,
		QMessageBox::StandardButtons buttons, QMessageBox::StandardButton *reply) const;
	static void slotAutoRefresh();
	void slotMaterialSelected(int matIndex) const;
	static void slotMaterialEdited();
	void ResetDocksPositions();

	// pull down menu
	void slotImportOldSettings();
	void slotMenuAboutMandelbulber();
	void slotMenuAboutQt();
	void slotMenuAboutManual();
	void slotMenuAboutNews();
	void slotMenuAboutThirdParty();
	void showDescriptionPopup();
	void slotMenuLoadExample();
	void slotMenuLoadSettings();
	void slotMenuLoadSettingsFromFile(QString fileName);
	void slotMenuLoadSettingsFromClipboard();
	static void slotMenuRedo();
	void slotMenuResetDocksPositions();
	void slotMenuAnimationDocksPositions();
	void slotMenuSaveDocksPositions();

	void slotMenuSaveImageJPEG();
	void slotMenuSaveImagePNG();
#ifdef USE_EXR
	void slotMenuSaveImageEXR();
#endif // USE_EXR
#ifdef USE_TIFF
	void slotMenuSaveImageTIFF();
#endif // USE_TIFF
	void slotMenuSaveImage(ImageFileSave::enumImageFileType imageFileType, QString nameFilter,
		QString titleType, QString defaultSuffix);

	void slotMenuSaveImagePNG16();
	void slotMenuSaveImagePNG16Alpha();

	void slotMenuSaveSettings();
	static void slotMenuSaveSettingsToClipboard();
	static void slotMenuUndo();
	void slotUpdateDocksAndToolbarByAction();
	void slotUpdateDocksAndToolbarByView() const;
	void slotStackAllDocks();

	void slotDetachMainImage();

	// toolbar and custom window settings
	void slotPresetAddToToolbar();
	void slotCustomWindowStateAddToMenu();
	void slotCustomWindowRemovePopup();

	// rendered image widget
	static void slotMouseMovedOnImage(int x, int y);
	void slotMouseClickOnImage(int x, int y, Qt::MouseButton button) const;
	static void slotKeyPressOnImage(QKeyEvent *event);
	static void slotKeyReleaseOnImage(QKeyEvent *event);
	void slotMouseWheelRotatedOnImage(int delta) const;

private:
	Ui::RenderWindow *ui;
	QDialog *preferencesDialog;
	QDialog *voxelExportDialog;
	QDialog *meshExportDialog;
	cAutomatedWidgets *automatedWidgets;

	QByteArray defaultGeometry;
	QByteArray defaultState;

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void AppendToLog(const QString &text);

	friend class cInterface;
};

#endif /* MANDELBULBER2_SRC_RENDER_WINDOW_HPP_ */
