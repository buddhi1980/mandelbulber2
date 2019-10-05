/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * Functions for flight animation.
 *
 * cKeyframeAnimation contains all functionality for editing and rendering of
 * a keyframe animation. The class holds a cKeyframes to store
 * the parameters of the keyframes and a table to display the frames in a widget.
 * The class can render interpolated sub-frames and validate their path (check
 * for collision). It exposes slots to manipulate the keyframes.
 */

#ifndef MANDELBULBER2_SRC_ANIMATION_KEYFRAMES_HPP_
#define MANDELBULBER2_SRC_ANIMATION_KEYFRAMES_HPP_

#include <qmessagebox.h>

#include "error_message.hpp"
#include "keyframes.hpp"
#include "progress_text.hpp"
#include "statistics.h"

// forward declarations
class cImage;
class cInterface;
class cFractalContainer;
class cParameterContainer;
class MyTableWidgetKeyframes;
class RenderedImage;

namespace Ui
{
class cDockAnimation;
}

class cKeyframeAnimation : public QObject
{
	Q_OBJECT

public:
	static const int reservedColumns = 1;
	static const int animSoundColumn = 0;

	cKeyframeAnimation(cInterface *_interface, cKeyframes *_frames, cImage *_image,
		QWidget *_imageWidget, cParameterContainer *_params, cFractalContainer *_fractal,
		QObject *parent = nullptr);

	bool RenderKeyframes(bool *stopRequest);
	void RenderFrame(int index) const;
	void RefreshTable();
	QString GetParameterName(int rowNumber);
	void DeleteKeyframe(int index) const;
	void DeleteFramesFrom(int index) const;
	void DeleteFramesTo(int index) const;
	void InterpolateForward(int row, int column);
	parameterContainer::enumMorphType GetMorphType(int row) const;
	void ChangeMorphType(int row, parameterContainer::enumMorphType morphType);
	QList<int> CheckForCollisions(double minDist, bool *stopRequest);
	void UpdateActualCameraPosition(const CVector3 &cameraPosition);
	void SetNetRenderStartingFrames(const QVector<int> &startingFrames);

public slots:
	void UpdateLimitsForFrameRange() const;
	bool slotRenderKeyframes();
	void slotAddKeyframe();
	void slotInsertKeyframe();
	void slotDeleteKeyframe() const;
	void slotIncreaseCurrentTableIndex();
	void slotDecreaseCurrentTableIndex();
	void slotModifyKeyframe();
	void slotNetRenderFinishedFrame(int clientIndex, int frameIndex, int sizeOfToDoList);
	void slotNetRenderUpdateFramesToDo(QList<int> listOfFrames);

private slots:
	void slotSelectKeyframeAnimImageDir() const;
	void slotTableCellChanged(int row, int column);
	void slotDeleteAllImages() const;
	void slotShowAnimation() const;
	void slotRefreshTable();
	void slotExportKeyframesToFlight();
	void slotMovedSliderFirstFrame(int value) const;
	void slotMovedSliderLastFrame(int value);
	void slotValidate();
	void slotCellDoubleClicked(int row, int column) const;
	void slotCellClicked(int row, int column) const;
	void slotSetConstantTargetDistance();
	void slotUpdateAnimationPathSelection();
	void slotAnimationStopRequest();

private:
	void PrepareTable();
	void CreateRowsInTable();
	int AddVariableToTable(
		const cAnimationFrames::sParameterDescription &parameterDescription, int index);
	int AddColumn(const cAnimationFrames::sAnimationFrame &frame, int index = -1);
	void AddRow(int row, const QString &fullParameterName, int index);
	void NewKeyframe(int index);
	QString GetKeyframeFilename(int index, int subIndex, bool netRenderCache) const;
	static QColor MorphType2Color(parameterContainer::enumMorphType morphType);
	void AddAnimSoundColumn() const;
	void UpdateAnimationPath() const;
	void UpdateCameraDistanceInformation() const;

	cInterface *mainInterface;
	Ui::cDockAnimation *ui;
	cKeyframes *keyframes;
	cImage *image;
	RenderedImage *imageWidget;
	cParameterContainer *params;
	cFractalContainer *fractalParams;
	QStringList tableRowNames;
	QVector<int> parameterRows; // position of parameter in table
	QVector<int> rowParameter;	// index of parameter in row
	MyTableWidgetKeyframes *table;
	bool lastToRenderMax = false;
	QSize previewSize;
	CVector3 actualCameraPosition;
	QList<int> netRenderListOfFramesToRender;
	QVector<bool> alreadyRenderedFrames;
	QVector<bool> reservedFrames;
	int renderedFramesCount = 0; // used for countig frames rendered with NetRender
	const int maxFramesForNetRender = 50;
	const int minFramesForNetRender = 5;
	bool animationStopRequest = false;

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress,
		cProgressText::enumProgressType progressType = cProgressText::progress_IMAGE);
	void updateStatistics(cStatistics statistics);
	void updateProgressHide(
		cProgressText::enumProgressType progressType = cProgressText::progress_ANIMATION);
	void QuestionMessage(const QString &questionTitle, const QString &questionText,
		QMessageBox::StandardButtons buttons, QMessageBox::StandardButton *reply);
	void showErrorMessage(
		QString text, cErrorMessage::enumMessageType messageType, QWidget *parent = nullptr);
	void notifyRenderKeyframeRenderStatus(QString text, QString progressText);

	void SendNetRenderSetup(int clientIndex, QList<int> startingPositions);
	void NetRenderCurrentAnimation(
		const cParameterContainer &settings, const cFractalContainer &fractal, bool isFlight);
	void NetRenderConfirmRendered(int frameIndex, int toDoListLength);
	void NetRenderSendFramesToDoList(int clientIndex, QList<int> frameNumbers);
	void NetRenderAddFileToSender(QString);
	void NetRenderStopAllClients();
	void NetRenderNotifyClientStatus();
};

extern cKeyframeAnimation *gKeyframeAnimation;

class cKeyframeRenderThread : public QThread
{
	Q_OBJECT;

public:
	cKeyframeRenderThread(QString &settingsText);

public slots:
	void startAnimationRender();

private:
	QString settingsText;

signals:
	void renderingFinished();
};

#endif /* MANDELBULBER2_SRC_ANIMATION_KEYFRAMES_HPP_ */
