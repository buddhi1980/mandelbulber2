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

#ifndef SRC_ANIMATION_FLIGHT_HPP_
#define SRC_ANIMATION_FLIGHT_HPP_

#include "animation_frames.hpp"
#include "interface.hpp"

class cFlightAnimation : public QObject
{
 Q_OBJECT

public:
 enum enumSpeedMode
 {
	 speedRelative,
	 speedConstant
 };


	cFlightAnimation(cInterface *_interface, cAnimationFrames *_frames, QObject *parent = 0);
	void RecordFlight();
	void RenderFlight();
	void RenderFrame(int index);
	void RefreshTable();
	QString GetParameterName(int rowNumber);
	void DeleteFramesFrom(int index);
	void DeleteFramesTo(int index);
	void UpdateThumbnailFromImage(int index);

private slots:
void slotRecordFlight();
void slotRenderFlight();

private:
	void PrepareTable();
	void CreateRowsInTable();
	int AddVariableToTable(const cAnimationFrames::sParameterDescription &parameterDescription, int index);
	int AddColumn(const cParameterContainer &params);
	cInterface *interface;
	Ui::RenderWindow *ui;
	cAnimationFrames *frames;
	QStringList tableRowNames;
	QVector<int> parameterRows; //position of parameter in table
	QVector<int> rowParameter; //index of parameter in row
	MyTableWidgetAnim *table;
};



#endif /* SRC_ANIMATION_FLIGHT_HPP_ */
