/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Functions for flight animation.
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

#ifndef SRC_ANIMATION_KEYFRAMES_HPP_
#define SRC_ANIMATION_KEYFRAMES_HPP_

#include "keyframes.hpp"
#include "interface.hpp"
#include "thumbnail_widget.h"
#include "files.h"

class cKeyframeAnimation : public QObject
{
 Q_OBJECT

public:
	cKeyframeAnimation(cInterface *_interface, cKeyframes *_frames, QObject *parent = 0);
	void RenderKeyframes();
	void RenderFrame(int index);
	void RefreshTable();
	QString GetParameterName(int rowNumber);
	void DeleteKeyframe(int index);
	void DeleteFramesFrom(int index);
	void DeleteFramesTo(int index);
	void InterpolateForward(int row, int column);
	parameterContainer::enumMorphType GetMorphType(int row);
	void ChangeMorphType(int row, parameterContainer::enumMorphType morphType);
	QList<int> CheckForCollisions(double minDist);

public slots:
	void UpdateLimitsForFrameRange();
	void slotRenderKeyframes();

private slots:
	void slotAddKeyframe();
	void slotInsertKeyframe();
	void slotDeleteKeyframe();
	void slotModifyKeyframe();
	void slotSelectKeyframeAnimImageDir();
	void slotTableCellChanged(int row, int column);
	void slotDeleteAllImages();
	void slotShowAnimation();
	void slotRefreshTable();
	void slotExportKeyframesToFlight();
	void slotMovedSliderFirstFrame(int value);
	void slotMovedSliderLastFrame(int value);
	void slotValidate();

private:
	void PrepareTable();
	void CreateRowsInTable();
	int AddVariableToTable(const cAnimationFrames::sParameterDescription &parameterDescription, int index);
	int AddColumn(const cAnimationFrames::sAnimationFrame &frame, int index = -1);
	void NewKeyframe(int index);
	QString GetKeyframeFilename(int index, int subindex);
	QColor MorphType2Color(parameterContainer::enumMorphType morphType);

	cInterface *mainInterface;
	Ui::RenderWindow *ui;
	cKeyframes *keyframes;
	QStringList tableRowNames;
	QVector<int> parameterRows; //position of parameter in table
	QVector<int> rowParameter; //index of parameter in row
	MyTableWidgetKeyframes *table;

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
};



#endif /* SRC_ANIMATION_KEYFRAMES_HPP_ */
