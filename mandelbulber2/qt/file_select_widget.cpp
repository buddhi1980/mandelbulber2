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

#include "file_select_widget.h"
#include "../src/parameters.hpp"
#include <QLabel>
#include "../src/global_data.hpp"
#include "../src/preview_file_dialog.h"
#include <algorithm>

FileSelectWidget::FileSelectWidget(QWidget *parent) : QWidget(parent)
{
	QFrame *frame = new QFrame;
	QHBoxLayout *hLayout = new QHBoxLayout(this);
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->setSpacing(2);
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(2);
	lineEdit = new QLineEdit;
	button = new QPushButton;
	labelImage = new QLabel;
	labelImage->setFixedSize(80, 40);
	QIcon icon = QIcon::fromTheme("folder", QIcon(":/system/icons/folder.svg"));
	button->setIcon(icon);
	hLayout->addWidget(button);
	hLayout->addWidget(lineEdit);
	frame->setLayout(hLayout);

	vLayout->addWidget(labelImage);
	vLayout->addWidget(frame);
	setLayout(vLayout);

	actionResetToDefault = NULL;
	actionAddToFlightAnimation = NULL;
	actionAddToKeyframeAnimation = NULL;
	parameterContainer = NULL;
	gotDefault = false;
	defaultValue = "";

	connect(button, SIGNAL(clicked()), this, SLOT(slotSelectFile()));
}

FileSelectWidget::~FileSelectWidget(void)
{
}

void FileSelectWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;
	actionResetToDefault = menu->addAction(tr("Reset to default"));
	actionAddToFlightAnimation = menu->addAction(tr("Add to flight animation"));
	actionAddToKeyframeAnimation = menu->addAction(tr("Add to keyframe animation"));
	QAction *selectedItem = menu->exec(event->globalPos());
	if (selectedItem)
	{
		if (selectedItem == actionResetToDefault)
		{
			if (parameterContainer)
			{
				SetPath(defaultValue);
			}
			else
			{
				qCritical() << " FileSelectWidget::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
			}
		}
		else if (selectedItem == actionAddToFlightAnimation)
		{
			if (parameterContainer)
			{
				gAnimFrames->AddAnimatedParameter(parameterName, parameterContainer->GetAsOneParameter(parameterName));
				gFlightAnimation->RefreshTable();
			}
		}
		else if (selectedItem == actionAddToKeyframeAnimation)
		{
			if (parameterContainer)
			{
				gKeyframes->AddAnimatedParameter(parameterName, parameterContainer->GetAsOneParameter(parameterName));
				gKeyframeAnimation->RefreshTable();
			}
		}
	}
	delete menu;
}

QString FileSelectWidget::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
		gotDefault = true;

		QString toolTipText = toolTip();
		toolTipText += "\nParameter name: " + parameterName + "<br>";
		toolTipText += "Default: " + defaultValue;
		setToolTip(toolTipText);
	}
	return defaultValue;
}

void FileSelectWidget::slotSelectFile()
{
	PreviewFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp)"));
	dialog.setDirectory(QDir::toNativeSeparators(lineEdit->text()));
	dialog.selectFile(QDir::toNativeSeparators(lineEdit->text()));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Select file for %1").arg(objectName()));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		lineEdit->setText(filename);
		gMainInterface->ShowImageInLabel(labelImage, filename);
	}
}

void FileSelectWidget::SetPath(QString text)
{
	lineEdit->setText(text);
	gMainInterface->ShowImageInLabel(labelImage, text);
}
