/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyFileSelectLabel class - promoted QLabel widget for selecting a file
 * showing the selected file in a MyLineEdit and opening the QFileDialog by pressing the QPushButton
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 */

#ifndef MANDELBULBER2_QT_FILE_SELECT_WIDGET_WIDGET_H_
#define MANDELBULBER2_QT_FILE_SELECT_WIDGET_WIDGET_H_

#include <QtGui>
#include <QtCore>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include "../src/parameters.hpp"

class FileSelectWidget : public QWidget
{
	Q_OBJECT

public:
	FileSelectWidget(QWidget *parent = 0);
	~FileSelectWidget();
	void AssignParameterContainer(cParameterContainer *container) {parameterContainer = container;}
	void AssingParameterName(QString name) {parameterName = name;}
	QString GetPath(){ return lineEdit->text(); }
	void SetPath(QString path);

private slots:
	void slotSelectFile();
	void slotChangedFile();
private:
	QLineEdit* lineEdit;
	QPushButton* button;
	QLabel* labelImage;

	QAction *actionResetToDefault;
	QAction *actionAddToFlightAnimation;
	QAction *actionAddToKeyframeAnimation;
	cParameterContainer *parameterContainer;
	QString parameterName;

	QString GetDefault();
	QString defaultValue;
	bool gotDefault;
	QString originalText;

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void paintEvent(QPaintEvent *event);
};

#endif /* MANDELBULBER2_QT_FILE_SELECT_WIDGET_WIDGET_H_ */
