/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * PreviewFileDialog class - extension of QFileDialog class. Added preview display
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


#ifndef PREVIEWFILEDIALOG_H_
#define PREVIEWFILEDIALOG_H_

#include <QFileDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QProgressBar>
#include "thumbnail_widget.h"

class PreviewFileDialog: public QFileDialog
{
	Q_OBJECT
public:
	PreviewFileDialog(QWidget *parent);
	~PreviewFileDialog();
protected slots:
	void OnCurrentChanged(const QString & filename);
	void slotUpdateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void slotHideProgressBar();

private:
	QVBoxLayout* vboxlayout;
	QCheckBox *checkbox;
	QProgressBar *progressBar;
protected:
	QLabel *preview;
	QLabel *info;
	cThumbnailWidget *thumbWidget;
};

#endif /* PREVIEWFILEDIALOG_H_ */
