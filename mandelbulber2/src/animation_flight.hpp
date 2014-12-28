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
	cFlightAnimation(cInterface *_interface, QObject *parent = 0);
	void RecordFlight(cAnimationFrames *frames);

private slots:
void slotRecordFilght();

private:
	void PrepareTable();
	void CreateRowsInTable();
	int AddVariableToTable(const cAnimationFrames::sParameterDescription &parameterDescription);
	int AddColumn(const cParameterContainer &params);
	cInterface *interface;
	Ui::RenderWindow *ui;
	cAnimationFrames *frames;
	QStringList tableRowNames;
	QVector<int> parameterRows; //position of parameter in table
	QTableWidget *table;
};



#endif /* SRC_ANIMATION_FLIGHT_HPP_ */
